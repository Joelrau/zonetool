# IW5 → IW7 light grid: load-path audit and defect list

Audit of the current converter against IW7's actual load path, 2026-09-03. Companion to
`iw7-lightgrid.md` (format) and `iw7-iw8-symbol-propagation.md` (how the symbols were
recovered). Everything below cites a function or a struct field.

The IW8 cross-binary pass contributed **nothing** here — IW8 2019 replaced this subsystem with
a light grid *volume* atlas (`GfxLightGridVolumeAtlas`, `R_LGV_*`) that IW7 has no counterpart
for. What follows came out of IW7's own symbols and strings.

---

## 1. The IW7 load path, as the binary actually has it

### `Load_GfxLightGridProbeData` @ `0x1409F0DC0`  (already symbolised in the IW7 database)

`Load_Stream(a1, varGfxLightGridProbeData, 240)` — the asset is **240 bytes**, matching
`IW7::GfxLightGridProbeData`. Every array's on-disk size, read straight off the loader:

| field | size expression | our emit |
|---|---|---|
| `gpuVisibleProbePositions` | `12 * gpuVisibleProbesCount` | correct |
| `gpuVisibleProbesData` | `(gpuVisibleProbesCount + 0x2000) << 6` | correct — **the `+0x2000` is confirmed, not a guess** |
| `probes` | `probeCount << 6` | correct |
| `probePositions` | `12 * probeCount` | correct |
| `zones` | `88 * zoneCount` | correct |
| `tetrahedrons` | `16 * tetrahedronCount` | correct |
| `tetrahedronNeighbors` | `16 * tetrahedronCount` | correct |
| `tetrahedronVisibility` | `tetrahedronCountVisible << 6` | correct (we set `tetrahedronCountVisible = tetrahedronCount` and allocate that many) |
| `voxelStartTetrahedron` | `4 * voxelStartTetrahedronCount` | correct |

Each array is followed by `R_CreateBufferInternal`-family calls that name the GPU resources:
`probesData` / `probesDataView` / `probesDataViewRW`, `probeTets`, `probeTetNeighbors`,
`probeTetVisibility`, `probeVoxelStartTet`, `gpuVisibleProbes` (+ `View` / `RWView`).

### `R_CreateSparseVoxelTree` @ `0x140DD0B90`  (already symbolised)

Uploads, in order: `voxelTreeHeader` as a **0x40-byte constant buffer** starting at
`rootNodeDimension`; `voxelTopDownViewNode` at **stride 12**; `voxelInternalNode` at **stride
16**; `voxelLeafNode` and `voxelLightList` as **2-byte** elements (format 57); then
`voxelInternalNodeDyamicLightList` — IW7's own typo — as **two** buffers of
`4 * voxelInternalNodeCount` each. Our `allocator.allocate<unsigned int>(2 * voxelInternalNodeCount)`
is exactly right.

**A tree with `voxelTopDownViewNodeCount == 0` is skipped entirely.**

### Voxel tree selection — `sub_140E3C1F0`

```c
for each voxelTree:
    if (!voxelTree->voxelTopDownViewNodeCount) continue;
    if (fabs(cam[i] - zoneBound.midPoint[i]) < zoneBound.halfSize[i] for all i) -> use it
if none matched:
    Sys_Error("No valid voxel trees.  Are there empty skyboxes in your map?");
```

Tree selection is a **strict containment test of the camera against `zoneBound`**, and failing
it is fatal, not a fallback. This is what defect #3 below is about.

### GPU sampling buffers — `sub_1404BE800` / `sub_1404BE870` / `sub_1404BE7B0` / `sub_1404BE920`

- `sub_1404BE800(n)` (device init): allocates `"light grid sampling history"` at **64 B x 0x2000
  fixed**, then calls `sub_1404BE870` twice — the request path is **double-buffered**.
- `sub_1404BE870(base, n)`: `"light grid sampling requests"` stride **16** x n;
  `"lightgrid sample constant buffer"` **112 B**; `"cached tet indices"` stride **4** x n.
- `sub_1404BE920(world)` (called from `R_LoadWorld`): sets the request count to
  `gpuVisibleProbesCount`, sets both dynamic cursors to **`gpuVisibleProbesCount + 65`**, then
  writes one 16-byte request per `gpuVisibleProbePositions` entry as
  **`{origin.x, origin.y, origin.z, 0x00FFFFFF}`** — the fourth dword is the initial *cached
  tetrahedron index*, `0xFFFFFF` meaning "no cached tet, walk from scratch".
- It also copies the baked slice into the runtime fields:
  `*(u32*)unk6 = *(u32*)&unk0; unk6[2] = unk2; unk6[3] = unk3;`
  So `unk0..unk3` are the authored fields and **`unk6` is runtime scratch that need not be
  baked** — which is what the converter does.
- `sub_1404BE7B0(count)`: `InterlockedExchangeAdd` on the cursor; if
  `result + count > base + 0x2000` it calls `R_WarnOncePerFrame(90)` and hands back the base.
  The dynamic window is exactly the `0x2000` tail that `gpuVisibleProbesData` is over-allocated
  by — the two are the same thing.

The `"cached tet indices"` buffer is new information: IW7 caches the resolved tetrahedron per
sample slot across frames, which is why `firstTetrahedron` behaves as a walk seed rather than a
range base.

### Other confirmations picked up in the same pass

- `Load_XSurface` allocates `"xsurface sh probe simplex vertices"` (+ view) — `XModel`'s SH
  probe simplex vertices are a real per-model GPU buffer, as `iw7-lightgrid.md` §4 assumed.
- `Load_GfxWorldReflectionProbeData` allocates `reflectionProbeLightgridSampleData`, and
  `sub_140E22180` allocates `"reflection probe light grid sampling requests"` — reflection
  probes resolve through the same tetrahedral volume.
- `sub_140E058B0` creates the `smodelShProbeParams` structured buffers (0x2000 / 0x800 / 0x800 /
  0x800 bytes). This is the closest thing found to a per-static-model SH parameter path and is
  the best remaining lead on the open problem in `iw7-lightgrid.md` §8. It is a lead, not an
  answer — the sample walk itself is still a compute shader and still not in the executable.
- `R_RegisterDvars` carries `"Adjust the contrast of light color from the light grid"`,
  `"Adjust the intensity of light color from the light grid"` and `"Default ambient light for
  failed lightgrid lookups."`

---

## 2. The IW5 side

`IW5::GfxLightGrid` is the legacy IW4-lineage grid: `mins[3]`/`maxs[3]` in grid space,
`rowAxis`/`colAxis`, a `rowDataStart[]` row index into `rawRowData[]`, `entries[]`
(`{u16 colorsIndex; u8 primaryLightIndex; u8 needsTrace;}`) and `colors[]`
(`u8 rgb[56][3]` — 56 directional bins, sqrt-encoded).

Grid space is 32 world units per cell in x/y and 64 in z, biased by 4096/4096/2048.
`lightgrid_tree::enumerate_row_data` walks `rowDataStart`/`rawRowData` and yields every
populated cell with its entry index; `ldr_colors_to_hdr` decodes a bin as `(b / 127.5)^2`,
giving a `[0, 4]` range.

The two formats share **nothing structural**. IW5 stores a per-cell index into a palette of 56
directional RGB bins; IW7 stores L2 spherical harmonics at the corners of a tetrahedral volume
addressed through a sparse voxel tree. The conversion is a resample-and-project, not a field
mapping, which is why the defects below are all in the generator rather than in field copies.

`project_sh` least-squares-fits the 56 decoded bins onto IW7's orthonormal basis; `constant_sh`
agrees with it (a constant radiance `L` projects to `c0 = L * 4pi * Y00 = L * 3.5449`).

---

## 3. Defects, ordered by likely visual impact

**1–3 are fixed** (2026-09-03, built clean, Debug|Win32). 4–6 are still open.

Note for anyone rebuilding: the checked-in `build/` was stale and did not list the IW7
converter sources at all, so an incremental build silently skipped them. Run
`tools\premake5.exe vs2022` (or `generate.bat`) before believing a build result.

### 1. A third of map bounds silently produce an invalid volume — and then an all-black fallback — FIXED

`src/X64/Utils/LightGrid/LightGridProbes.cpp:207-250`

The `leaf_shift` search estimates the probe count as `(ceil(size/root)*16 + 1)^3`, but `build()`
then recomputes `root_dim` **after snapping `origin` down to a root-cell boundary**, which can
add a whole root cell per axis. The two never agree, and when the recomputed
`probe_total > max_probes` the function returns at line 247 with `valid = false`.

Simulated over 200,000 randomly generated CoD-scale map bounds, **35.4% select a `leaf_shift`
whose snapped grid overflows the cap**. Example: span (4186, 1094, 454) picks `leaf_shift = 5`,
snaps to `root_dim = (9, 3, 2)`, and needs 234,465 probes against a cap of 131,071.

It fails twice over. The early return happens **before** `zone_fallback_coeffs` is computed, so
`GfxWorld.cpp:1497` memcpy's 29 zeroed coefficients into `zone.fallbackProbeData`. A map that
trips this gets no probe volume *and* a black zone fallback — every XModel in the map renders
black, with no log line from `build()` to say why.

**Fixed.** The search now calls a `measure(candidate_shift)` lambda that does the full snapped
`origin`/`root_dim` computation and returns the real probe total, so the test measures what will
actually be built. An explicit `params.leaf_shift` became a starting point rather than a promise
— it coarsens from there instead of failing, because a coarse volume beats no volume. `measure`
also saturates its running total and clamps before narrowing to `int`, so a nonsense bounds box
cannot wrap past the cap or hit UB on the float→int conversion.

Re-running the same 200,000-case simulation against the patched selection: **0 overflows**. The
example above now picks `leaf_shift = 6`, `root_dim = (5, 2, 1)`, 45,441 probes. Chosen shifts
spread 5–9, so the search is not just bottoming out at a coarse level.

Separately, `GfxWorld.cpp` no longer copies a zeroed fallback through. When `!volume.valid` it
encodes the map-average `ambient` it already computed, logs a `ZONETOOL_WARNING`, and carries
`coeffs[27] = 1.0` like every shipped map's zone fallback. "Volume failed" now renders as flat
ambient with a log line, not as black with silence.

### 2. Tetrahedron neighbours alias above 65,536 probes — FIXED

`src/X64/Utils/LightGrid/LightGridProbes.cpp:478`

```cpp
if (j != k) v[n++] = out.tetrahedrons[t * 4 + j] & 0xFFFF;
```

The face key masks the probe index to **16 bits**, but `max_probes = 0x1FFFF` is **17** — the
header says so explicitly, and mp_paris ships 114,336 probes. Any volume above 65,536 probes
makes probes `n` and `n + 0x10000` collide in the face map, so faces get paired to the wrong
tetrahedron and `tetrahedron_neighbors` links across the map. The barycentric march then walks
into an unrelated tetrahedron or terminates, and those samples fall through to the zone
fallback.

**Fixed.** Masks with `0x7FFFFFFFu` now — strip the bit-31 corner flag and nothing else. Below
65,536 probes the two masks agree, so this changes no existing output; above it, faces stop
being paired across the map.

### 3. `zoneBound` is the IW5 world box, but tree selection is a strict containment test — FIXED

`src/IW5/Converter/IW7/Assets/GfxWorld.cpp:1466` —
`memcpy(&tree.zoneBound, &asset->bounds, sizeof(Bounds))`

`sub_140E3C1F0` picks the voxel tree by testing `fabs(cam - zoneBound.midPoint) < halfSize` on
all three axes and calls `Sys_Error("No valid voxel trees. ...")` when nothing contains the
camera. Meanwhile the volume the tree actually indexes spans `voxelTreeHeader->boundMin` ...
`boundMax`, which the generator derives by snapping the origin **down** to a root cell and
rounding the extent **up** — strictly larger than `asset->bounds`.

So the fatal test is run against the *smaller* of the two boxes, and a camera in the margin
between them hard-errors. There is no upside to the tighter box.

**Fixed.** `zoneBound` is now derived from `volume.bound_min`/`bound_max` — the same extent that
goes into `voxelTreeHeader->boundMin`/`boundMax` — as midpoint and half-size. Both `IW5::Bounds`
and `IW7::Bounds` are `{midPoint[3], halfSize[3]}`, so the old `memcpy` was layout-correct; it
was simply describing the wrong box.

### 4. `primaryLightEnvs[0]` is left empty — NOT A DEFECT. Two neighbouring ones were.

This was listed as a defect on the reasoning "stock maps use env 0 on every static model, which
means their env 0 is populated". **That reasoning was wrong**, and the stock data says so
flatly. Parsing the `primaryLightEnvs` table out of all nine dumped IW7 ComWorlds
(`tools/iw8-to-iw7-names/commap.py`):

| map | primaryLightCount | primaryLightEnvCount | env 0 |
|---|---|---|---|
| cp_zmb | 1039 | 1039 | `numIndices = 0` |
| mp_afghan | 11 | 11 | `numIndices = 0` |
| mp_breakneck | 53 | 53 | `numIndices = 0` |
| mp_paris | 68 | 68 | `numIndices = 0` |
| mp_dome_dusk | 34 | 34 | `numIndices = 0` |
| mp_frontend | 55 | 55 | `numIndices = 0` |
| mp_bog, mp_credits_s1, mp_shipment | = count | = count | `numIndices = 0` |

Every map: `primaryLightEnvCount == primaryLightCount`, light 0 is type `NONE`, `env[0]` is
empty, `env[i] = {i}` for `i >= 1`, and there is no trailing sentinel. **`env 0` is the
engine's "no primary light environment"**, and since every static model in every stock map
carries `primaryLightEnvIndex == 0`, IW7 static models are simply not lit through this path at
all. Filling env 0 would have made us the only map in existence that does.

What the comparison *did* find, by diffing our `mp_test_h1` output against that table
(ours: 4 lights, 5 envs):

- `primaryLightEnvCount` was `primaryLightCount + 1`. **Fixed** — it is now `== primaryLightCount`.
- A trailing env with `numIndices = 1, primaryLightIndices[0] = 2047` that no stock map has.
  Light 2047 does not exist in any map we emit, so anything that resolved that env would have
  read a `ComPrimaryLight` a long way off the end of the array. **Fixed** — removed.

Env 0 stays empty, now with a comment saying why so it does not get "fixed" again.

**Left open deliberately:** `GfxWorld.cpp:1947` writes `primaryLightEnvIndex = IW5's
primaryLightIndex`, where stock writes 0 on every static model. That change was made on the
same wrong premise, so it deserves re-testing — but the note in `iw7-lightgrid.md` §5 records a
real in-game observation (env 0 everywhere made models flat and needed `sh_ambient_scale`
~32x), and one wrong premise does not make that observation wrong. It is a one-line A/B, so
test it in game rather than reasoning about it.

### 5. Debug paint reads one float past the end of its array

`src/IW5/Converter/IW7/Assets/GfxWorld.cpp:1519` — `float sh[27];` is passed to
`encode_probe_sh`, whose signature became `const float sh[28]` when the visibility term was
added. `constant_sh` still writes only 27. The 28th read is uninitialised stack, and it becomes
the probe's baked sun-visibility. Off by default (`debug_probe_paint = false`), so it is a
latent trap for the next person who turns it on rather than a live bug.

**Fix:** `float sh[28] = {}; ... sh[27] = 1.0f;`

### 6. Dead half-float encoder

`LightGridProbes.cpp:26` — `to_half` is now unreferenced (line 188 switched to `float_to_half`,
leaving `// to_half(sh[k])` commented in place). The switch is an improvement: `to_half`
truncates the mantissa and flushes denormals to zero, which matters for the small `coeffs[27]`
visibility values, while `float_to_half` rounds to nearest-even and encodes denormals. Worth
knowing that `float_to_half` returns `0x7FFF` — a half NaN — above 65504 rather than saturating;
SH magnitudes never get there, but it is not a saturating encoder.

---

## 4. Things that are correct and should not be "fixed"

Verified against the loader this pass, not inferred:

- `gpuVisibleProbesData` over-allocated by `+0x2000` — matches `Load_GfxLightGridProbeData`
  exactly, and `sub_1404BE7B0` shows that tail *is* the dynamic-model allocation window.
- `tetrahedronCountVisible = tetrahedronCount` with a full permissive visibility table — the
  loader sizes that array by `tetrahedronCountVisible`, so the two must agree, and they do.
- `voxelInternalNodeDynamicLightList` at `2 * voxelInternalNodeCount` uints.
- `unk0` / `unk1` / `unk2` / `unk3` written per static model, `unk6` left alone.
- The 88-byte zone, 64-byte probe, 16-byte tetrahedron and 12-byte probe position strides.
