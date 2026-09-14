# IW7 light grid — format, converter state, and open problems

Reference for the IW5 → IW7 `GfxWorld` light grid work. Written 2026-08-31.

Reference maps used throughout. **Only these are authentic IW7 content:** `mp_paris`,
`mp_afghan`, `mp_breakneck`, `cp_zmb` (from `D:\Games\PC\IW7\dump`), plus `mp_dome_dusk` and
`mp_frontend`. `mp_bog`, `mp_shipment` and `mp_credits_s1` are **H1 maps ported to IW7** — they carry
convincing-looking octree lightgrid, umbra and sun-shadow data purely because H1 has those fields and
the port copies them across. Never use them as a spec.

Two of them are traps on their own:

- **`mp_frontend`** has one static model and every probe byte-identical to its zone fallback. Fitting
  anything to it alone produced two separate wrong conclusions (see *Disproven*).
- **`mp_dome_dusk`** is a dusk map, so its sun and probe values are far dimmer than a daylit map's.

---

## 1. What the light grid actually is in IW7

`GfxLightGrid` contains two systems. The **octree + palette** (`tree`, `paletteBitstream`,
`paletteEntryAddress`) inherited from the IW6/H1 lineage is **dead** — every authentic map ships a
3-entry stub palette and a 2-node stub tree, and H1→IW7 ports that carry a full valid octree still
render every model black. Only `probeData` matters.

`probeData` is a **GPU tetrahedral SH probe volume**:

| field | meaning |
|---|---|
| `probes` | `GfxProbeData`, 64 B = 32 float16 per probe (see §2) |
| `probePositions` | world xyz per probe |
| `tetrahedrons` | `indexFlags[4]` — one probe index per vertex |
| `tetrahedronNeighbors` | `neighbors[k]` = tetrahedron opposite vertex `k`, `0xFFFFFFFF` = none |
| `voxelStartTetrahedron` | one entry per **voxel tree leaf**, `0xFFFFFFFF` = empty |
| `zones` | `GfxGpuLightGridZone`, 88 B, one per map |
| `gpuVisibleProbes*` | **not** a probe copy — see §4 |

The tetrahedralisation is a **regular cube grid**, not a Delaunay: every tetrahedron is positively
oriented and the dominant volume is exactly `64³/6`, i.e. axis-aligned cubes split into six
tetrahedra. Cell size is adaptive in shipped data (32/64/128/256); we emit a single uniform level.

`indexFlags` low bits are the probe index — **at least 17 bits** (mp_paris ships 114,336 probes with
indices reaching 114,335). Bits 17–30 are zero in every shipped map; **bit 31** is a per-corner
refinement marker, all-clear in 59% of dome_dusk's tetrahedra and all-set in 34%. A uniform grid
never straddles a level, so emitting a constant 0 is fine.

### Voxel tree traversal (world position → leaf index)

Validated on all five authentic maps — 100% of leaves consistent.

```
dx = floor(p.x) - boundMin.x                    // likewise dy, dz
cx = dx >> shift[0]  (etc.)
if (cx,cy,cz) outside rootNodeDimension     -> no leaf
col  = cy * rootNodeDimension[0] + cx           // row-major over XY, x fastest
(firstNodeIndex, zMin, zMax) = topDownViewNode[col]
if firstNodeIndex < 0 or cz not in [zMin,zMax] -> no leaf
node = firstNodeIndex + (cz - zMin)             // root cells stacked in z

for level in (1, 2):
    (fni, childNodeMask) = internalNode[node]
    childIndex = (((dz >> shift[level]) & 3) << 4)
               | (((dy >> shift[level]) & 3) << 2)
               |  ((dx >> shift[level]) & 3)     // z high, x low
    mask = (childNodeMask[1] << 32) | childNodeMask[0]
    if !((mask >> childIndex) & 1)           -> no leaf
    child = (fni & 0x7FFFFFFF) + popcount(mask & ((1 << childIndex) - 1))
    if fni < 0: return child                    // bit31 = children are leaves
    node = child
```

Shipped trees terminate at **mixed depth** — dome_dusk has ~1631 root cells of which only 61 descend
to level 2 — which is how adaptive cell sizing is expressed. `voxelLeafNodeCount ==
voxelStartTetrahedronCount` in every map. Wrong alternatives that were tested and rejected:
`col = cx * rootDim[1] + cy`, and all five other axis orderings for `childIndex`.

---

## 2. `GfxProbeData` layout — 32 float16

| index | contents |
|---|---|
| 0–26 | L2 SH, three blocks of nine: all R, then all G, then all B |
| **27** | **sun/sky visibility in [0,1]** — per probe, *not* a constant |
| 28–31 | always zero |

**Index 27 is the single most misread field in this format.** It is real per-probe data:

| map | median | max | nonzero | distinct values |
|---|---|---|---|---|
| mp_dome_dusk (dusk) | 0.0002 | 1.0 | 62% | 10,611 |
| mp_paris | 0.0128 | 1.0 | 71% | 14,112 |
| mp_frontend (open lobby) | 0.99 | 1.0 | 94% | 2,815 |

Near zero on a dusk map, near one in a wide-open lobby — a baked occlusion term. Earlier notes here
called it "a constant 1.0"; that came from mp_frontend (where it genuinely is ~1 everywhere) and from
the zone *fallback*, which **is** 1.0 in every stock map. The per-probe field is not.

### SH basis

Standard orthonormal real SH in standard order:
`Y00, Y1-1(y), Y10(z), Y11(x), Y2-2(xy), Y2-1(yz), Y20(3z²-1), Y21(xz), Y22(x²-y²)`
with constants 0.282095 / 0.488603 / 1.092548 / 0.315392 / 0.546274.

Established by a physical up/down test: reconstructing radiance over mp_dome_dusk gives **up/down =
2.53** under this basis (sky brighter than ground) versus **1.04** under the IW6/H1 ordering. A
negativity test was tried first and was *not* decisive (0.7–3.4% for every candidate).

---

## 3. `GfxGpuLightGridZone` (88 B)

Six `unsigned int` then `fallbackProbeData` at offset 24. Confirmed by offset 20
(`numVoxelTetrahedronIndices`) equalling `voxelLeafNodeCount` in four maps. IW8's 92-byte zone has an
extra `voxelTetrahedronInternalNodeShift`; IW7's does not.

**`firstTetrahedron` must point at a valid tetrahedron.** Shipped maps disagree — paris/afghan/
breakneck/frontend store `count-1`, cp_zmb and dome_dusk store `0` — which made it look like a
harmless walk seed. It is not: emitting `count-1` left the barycentric march with no valid start and
every sample fell straight through to `fallbackProbeData`. **0 is the only value correct under both
readings.** This was the single change that made the volume resolve at all.

---

## 4. `gpuVisibleProbes` — a per-static-model sample list

Not a second probe set. `R_LoadWorld` → `sub_1404BE920` walks `gpuVisibleProbePositions`, writes each
origin plus `0xFFFFFF` as a 16-byte entry into the *"light grid sampling requests"* buffer, and
uploads `16 * gpuVisibleProbesCount` **once at load**. The GPU resolves the tetrahedral volume at
those positions and writes the SH into `gpuVisibleProbesData`, which is where static models read
their lighting.

The array is a concatenation of **per-model slices**. Model *i* owns `[unk0, unk0 + unk3)` of
`GfxStaticModelDrawInst`, where `unk0`/`unk1` form a 32-bit first index (the engine reads
`*(_DWORD *)&unk0`) and `unk3` is the count. Proven exactly on mp_dome_dusk: `sum(unk3) ==
gpuVisibleProbesCount == 21152`, and sorted `unk0` equals the running sum of `unk3` with **zero**
mismatches, no gaps, no overlaps.

`unk2` is a layout enum that pins the count:

| `unk2` | models | `unk3` |
|---|---|---|
| 0 | 2444 | always 2 |
| 1 | 166 | always 3 |
| 2 | 142 | 4–65 |
| 3 | 405 | 5–65 (mean 37) |

Leaving these zero points every model at slice 0. `lightingHandle`, `unk5` and `unk6[*]` are runtime
scratch — `R_ResetModelLighting` sets `lightingHandle = ((flags & 0x240) == 0)`.

**Dynamic models do not use this.** They allocate slots at runtime via `sub_1404BE7B0`, from a cursor
starting at `gpuVisibleProbesCount + 65`, bounded by `+ 0x2000`; the request buffer is allocated at
device init as `sub_1404BE800(0x40000)` = 262,144 entries. A model with SH probe simplex vertices
(`XModel.unknownVec3Count` @705, `unknownVec3` @712) submits one request per vertex; one without
still submits a single request at its own world origin.

---

## 5. Fields that are engine constants, not per-map data

`skyLightGridColors` and `defaultLightGridColors` are **byte-identical in all five authentic maps**:

- `skyLightGridColors` — all zeros
- `defaultLightGridColors` — `(0, 0, 0.21875)` repeated for all 56 bins

Converting them 1:1 from IW5's `colors[0]`/`colors[1]` (as the H1 converter does) writes a non-zero
sky table where every shipped map writes zeros, and a default table 3.5× too bright with no zeros in
it. That is a constant ambient floor across the whole map, independent of the probe volume.

Also constant across stock content: `unk[9] = {0,0,5,5,6,32,32,64,0}` in five of six maps (32/32/64
is the classic legacy grid cell size — an authoring setting, not geometry), `tableVersion` and
`paletteVersion` = 1, `rangeExponent8/12/16` = (0, 4, 23).

### The trap in "match stock"

`primaryLightEnvIndex` is 0 on **every** static model in both stock maps checked (3157/3157 and
13269/13269). Matching that **breaks converted maps**: `ComWorld.cpp` builds `primaryLightEnvs[i] →
light i` with a loop that **starts at `i = 1`**, so `primaryLightEnvs[0]` is left zeroed —
`numIndices = 0`, an env with no lights. Stock maps can use 0 because their ComWorld fills env 0.

> "Every stock map ships value X" justifies emitting X only when the *surrounding data* is also
> stock-shaped. It was right for the sky/default colour tables and wrong here, because the index
> resolves into a table our own converter builds differently.

`ComWorld.cpp`'s loop starting at 1 is a latent bug worth fixing on its own.

---

## 6. Converter state

### Committed — `b750347`, `73b7896` on `x64`

Confirmed working in game: the probe volume and voxel tree generator
(`src/X64/Utils/LightGrid/LightGridProbes.{hpp,cpp}`), `zone.firstTetrahedron = 0`, the level-1 node
content-scale fix, per-model `gpuVisibleProbes` slices, probe positions at `+1/32`, `max_probes`
raised to `0x1FFFF`, and `sh_ambient_scale` calibrated to 1.0.

Two generator details worth keeping in mind:

- The level-1 content test must use `l2_has_content` (4 leaf cells per axis), not `l1_has_content`
  (16). The wrong one emitted 192 of 392 nodes with an empty child mask, where dome_dusk has exactly
  one in its whole tree. It was self-consistent, so the descent still resolved — statistics didn't
  catch it, a structural diff against a stock map did.
- Probe positions carry `+1/32`, matching all six authentic maps, so the tetrahedral mesh doesn't
  share planes with voxel cell boundaries.

### Uncommitted, strongly evidenced

- sky/default colour tables → the engine constants above
- occupancy shaping (`build_params::cell_occupied`) so the volume follows the source grid's populated
  cells rather than filling the bounding box. Correct semantics; only culled 6.8% on mp_test, which
  is an open box — it will matter more on maps with interiors.
- `coeffs[27]` carried through the sampler as a real term instead of a hardcoded 1.0
- `reflectionProbeIndex = 0` (stock uses 0 universally; ours emitted 1 and 4, in range but unlikely
  to be intended)
- `primaryLightEnvIndex` back to IW5's value (see the trap above)

### Uncommitted, weak

`coeffs[27]`'s *value*. That 1.0 is wrong is solid. Our replacement is derived from
`is_sun_light(entries[i].primaryLightIndex, lastSunPrimaryLightIndex)` — the same classifier the H1
converter uses — which is **binary** where shipped maps carry a continuous term, then smoothed with
two 3×3×3 box passes. `primaryLightIndex` is not really an occlusion signal; a real fix needs an
occlusion bake (trace each probe at the sun), which the converter has no ray casting for.

---

## 7. Disproven — do not retry

**Scaling the sun colour.** Stock sun (`GFX_LIGHT_TYPE_DIR`) colour maxima are 7.41 (paris), 4.58
(breakneck), 0.32 (dome_dusk), 0.14 (cp_zmb); ours is 0.811. Multiplying by 6 to close that gap
**overexposed the entire map**. The premise — that the world's sunlight is baked into lightmaps and
wouldn't respond — is false: the world responds immediately. The sun is not missing and that colour
gap is not a defect to correct.

Note the sun and spot/omni are on completely different scales: `convertColorToPhysicallyBased` is
×1550 (`candelas * 1000 * 1550 * 0.001`) and stock spots run 2k–160k. **Not** applying it to the sun
is correct — a ×1550 sun would be ~1256.

**IRLS reweighting in `project_sh`.** `lightgrid_sh::hdr_colors_to_sh` (used by the working H1 path)
follows its least-squares fit with two Gauss-Newton passes weighted by `1/eval`. Adding that to the
IW7 path changes nothing useful — measured over 300 shipped probes pushed through IW5's LDR storage
and back, plain least squares retains **97.0%** of the L2 band and the reweighted fit **96.1%**.

**The sampler compressing dynamic range.** Converter logs for mp_test: 20,646 populated cells; source
cell luminance min 0.0328 / p5 0.3645 / p50 0.7979 / p95 0.9365 / max 1.1157. Source p5/p95 ratios
are 0.46× / 1.17× of median and our probes' are 0.46× / 1.26× — a near-exact match. The narrow range
is the IW5 data's own (it is ambient-only; bytes ~12–72 of 255), not compression in conversion.

**`lightGrid.unk[]`, `tetrahedronVisibility`, `XModel.unknownIndex2`.** `unknownIndex2 = 0xFF` is the
*most common* stock value (405 of 715 dumped models) and `unknownVec3Count = 0` matches 413 of them,
so the converter is already correct there.

---

## 8. The open problem

**Models receive far less sun than world surfaces do.**

The evidence, in the order it accumulated:

1. Models are correctly exposed on average — our probe median matches stock — but flat, with no
   directional form.
2. `sh_ambient_scale = 1` "looks off", `32` "looks kind of ok just too bright". Needing 32× of a
   *non-directional* term means it is standing in for a missing *directional* one.
3. It affects **dynamic models too**, so it is nothing in `smodelDrawInsts`.
4. At 6× sun the **world blew out while the cars barely changed** — the sun reaches world surfaces
   far more than it reaches models.

So the remaining defect is a **per-model gate on the sun**: not the light grid (measured faithful),
not the sun's colour (disproven), not `smodelDrawInsts` (dynamic models show it). Candidates not yet
eliminated include `sunShadowFlags` (we set 1 on all 40 models; stock sets 0 on the majority —
2546/3157 in dome_dusk), and whatever binds the sun in the model shading path.

The lightgrid sample walk itself is a **compute shader**, not in the executable — the probe buffers
appear only in asset-load code, bound as SRVs (`"probeVoxelStartTet"`). Reversing
`iw7_ship_dump.exe.i64` will not recover it, and zonetool has no IW7 techset/shader dumper.

---

## 9. Methods that actually worked

- **Render the data and compare against known-good data rendered identically.** Distributions,
  percentiles and round-trip tests all looked fine while hiding structural differences that were
  obvious in one image. Top-down colour slices of the probe volume (PIL) found the empty-node defect
  and the flat-slab shape; a per-coefficient zero count found `coeffs[27]`.
- **Diff structural statistics against a stock map**, not against expectations — empty-node counts,
  per-field histograms, distinct-value counts.
- **Simulate the whole pipeline, not one stage.** Validating only the voxel descent passed and hid
  the problem; simulating descent → `voxelStartTetrahedron` → barycentric march (2000/2000 resolved)
  is what cleared the data and redirected the search.
- **Parse the dump before blaming the format.** This redirected the diagnosis at least three times —
  once when a stub voxel tree was silently overwriting the generated one.
- **Add a converter log line rather than guess.** The resolve-radius and source-luminance logs
  settled the "is the sampler compressing range" question in one round trip.
- **Paint diagnostics into the data.** `debug_probe_paint` (zone fallback magenta,
  `gpuVisibleProbesData[0]` green) distinguishes "walk failed", "reading slot 0", "working" and "lit
  from elsewhere" in a single run. Both it and `debug_position_ramp` are still in the converter,
  default off. Keep them.
