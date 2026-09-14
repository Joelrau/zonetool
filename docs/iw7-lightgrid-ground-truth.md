# IW7 light grid — ground truth from shipped data

Written 2026-09-03, after building a full sequential `.gfxmap` parser. Everything here is
measured off shipped assets, not inferred. Supersedes several claims in `iw7-lightgrid.md`
that were made before the data could be read directly.

## The parser

`tools/iw8-to-iw7-names/gfxmap.py` mirrors `ZoneTool::IW7::IGfxWorld::dump` exactly. The
stream is tagged, so every step asserts the next tag is one of {STRING, ASSET, ARRAY, OFFSET,
RAW} — a wrong element size derails into an invalid tag immediately rather than silently
returning garbage. Struct sizes and `GfxWorld` field offsets come from the IW7 database's own
type library (`dump_types.py`), which agrees with `assert_sizeof(GfxWorld, 0x11A8)`.

All eleven dumps parse: 100% of `mp_test_h1`, 85–99% of the stock maps (the remainder is the
tail past `smodelDrawInsts`, which the parser stops before).

Two element sizes are easy to get wrong and both desync the whole stream:
`GfxLightViewFrustum::planes` is `vec4_t` (16), not `cplane_s`; `dpvs.lodData` and
`dpvs.sortedSurfIndex` are `unsigned int*` (4), not `unsigned short*`. Sub-array counts must
come from the parent struct, never from the length of the dumped child — a `dump_array` whose
pointer was already emitted writes a back-reference carrying **no count**.

## 1. SOLVED: `indexFlags` bit 31 selects into `tetrahedronVisibility`

`tetrahedronCountVisible` is exactly the number of tetrahedra with **at least one** corner
carrying bit 31 in `indexFlags`. Not approximately — exactly, in every authentic map:

| map | tetrahedronCount | tetrahedronCountVisible | tets with any corner bit31 |
|---|---|---|---|
| mp_dome_dusk | 251,955 | 103,402 | **103,402** |
| mp_paris | 677,508 | 297,161 | **297,161** |
| mp_afghan | 695,332 | 319,972 | **319,972** |
| mp_breakneck | 466,466 | 213,259 | **213,259** |
| cp_zmb | 687,342 | 322,466 | **322,466** |
| mp_frontend | 88,808 | 13,786 | **13,786** |

(The "all four corners flagged" count does *not* match — 86,772 vs 103,402 on dome_dusk — so
the rule is **any**, not all.)

So bit 31 is **not** the cell-refinement marker `iw7-lightgrid.md` §1 assumed. It marks a
tetrahedron as having a visibility entry, and `tetrahedronVisibility` is indexed by the running
count of flagged tetrahedra — a compacted array, which is why its length is only 15–47% of the
tetrahedron count.

## 2. `tetrahedronVisibility` is 64 uint8 weights, not a 512-bit mask

Shipped entries look like this (mp_dome_dusk, entry 0):

```
FFFFFFFF FFFFFFFF FFFFFFFF FFFFDAFF   FFFFFFFF FFFFFFFF FFFFFFFF FF5D5656
FFFFFFFF FFFFFFFF FFFFFFFF FFFFFFDA   FFFFFFFF FFFFFFFF FFFFFFFF FFA0FFFF
```

Bytes take values like `0xDA`, `0x93`, `0x5D`, `0xA0`, saturating at `0xFF` — these are 8-bit
weights, not bits. The per-word statistics repeat with **period 4**, so the entry is four
blocks of 16 bytes; one block per tetrahedron vertex is the obvious reading, though what the 16
values within a block index is still unknown.

**No shipped entry is all-`0xFF`** (0.0% in every map), median popcount ≈ 370 of 512 (~72%
saturation), and 58–78% of entries are distinct. This is real baked occlusion data.

## 2a. `GfxSHProbeData::coeffs` — what the 32 slots hold

64 bytes = 32 float16 slots. Measured over every probe of all six authentic maps (486k probes)
and every zone fallback:

| slots | contents |
|---|---|
| 0..26 | 27 L2 SH coefficients, **three blocks of nine in channel order** (all R, then all G, then all B) |
| 27 | sky/sun visibility in [0, 1] — genuinely per-probe; **exactly 1.0** in every zone fallback |
| 28..31 | zero in every shipped probe and every shipped zone fallback, without exception |

**Channel-major, not coefficient-major.** Only slots 0, 9 and 18 are non-negative across the
whole population. Coefficient-major interleaving would make slots 0, 1, 2 the non-negative
ones.

**Ordering within a block is the standard real-SH one** —
`Y00, Y1-1(y), Y10(z), Y11(x), Y2-2(xy), Y2-1(yz), Y20, Y21(xz), Y22`. Slot 2 of each block is
the vertical term. Reconstructing radiance from the shipped coefficients and comparing straight
up against straight down, over probes above median energy:

| map | standard (y,z,x) | IW6/H1 (x,y,z) | dx-style (x,z,y) |
|---|---|---|---|
| mp_paris | **58.95** | 0.52 | 58.95 |
| mp_breakneck | **75.37** | 1.35 | 75.37 |
| mp_dome_dusk | **4.39** | 1.26 | 4.39 |
| mp_afghan | **1.10** | 1.02 | 1.10 |

The IW6/H1 ordering puts the *ground* brighter than the sky on mp_paris, which is impossible
for a daylit outdoor map. The dx-style variant is indistinguishable on this test (it also has z
at slot 2) but has consistently higher reconstruction negativity (13.2% vs 11.2% on paris,
3.2% vs 2.5% on dome_dusk), so the standard L2 assignment wins the tie-break. mp_afghan's low
ratio is a bright-sand map, not a counterexample — it still favours standard.

Per-map DC medians, useful as the calibration reference for `sh_ambient_scale`:

| map | DC median (R, G, B) | luminance p5 / p50 / p95 |
|---|---|---|
| mp_paris | 0.818, 0.620, 0.631 | 0.016 / 0.684 / 12.93 |
| mp_afghan | 2.443, 0.831, 0.454 | 0.040 / 1.156 / 6.17 |
| mp_breakneck | 0.899, 0.661, 0.710 | 0.001 / 0.737 / 2.79 |
| mp_dome_dusk | 0.890, 0.818, 0.861 | 0.002 / 0.836 / 3.65 |
| mp_frontend | 1.198, 1.488, 1.726 | 0.852 / 1.445 / 7.55 |
| cp_zmb | 0.022, 0.021, 0.036 | 0.000 / 0.024 / 0.16 |

### Derived without assumptions, and one correction to the ordering claim

Re-derived bottom-up, taking only what the loader proves (probeCount records of 64 bytes):

1. **Format.** The f32 reading gives magnitudes up to 1e16; f16 never exceeds 479. Decisive
   though is that the period-9 correlation structure below exists *only* under the f16 reading.
   64 bytes = 32 float16.
2. **Live slots.** Exactly slots 0..27 vary; 28..31 are constant 0.0 in all six maps.
3. **Grouping.** Mean |corr(slot i, slot i+L)| peaks hard at L = 9: 0.78-1.00, against 0.03-0.54
   at every other lag. Same-position triples score 0.78-0.95 at stride 9 versus 0.04-0.54 at
   stride 3. So slots 0..26 are **3 groups of 9**, not 9 groups of 3.
4. **Sign.** Only slots 0, 9, 18 are never negative - position 0 of each group is its mean term.
5. **Bands.** Per-position RMS within a group, averaged over groups, decays 1.000 | 0.19-0.32
   0.38-0.62 0.18-0.47 | 0.09-0.18 ... - clustering as **1 + 3 + 5**, the signature of a
   degree-2 expansion, with position 2 the largest of the middle band on every map.

**The directional meaning was then measured, not assumed.** A radiance distribution leans
toward brighter surroundings, so each coefficient must track the spatial gradient of the mean
term across neighbouring probes. On mp_dome_dusk (correlations against d/dx, d/dy, d/dz):

| position | identified as | corr |
|---|---|---|
| 1 | **-y** | -0.720 |
| 2 | **+z** | +0.488 |
| 3 | **-x** | -0.580 |
| 4 | +d2/dxdy (xy) | +0.406 |
| 5 | -d2/dydz (yz) | -0.345 |
| 6 | +d2/dz2 (z^2) | +0.158 |
| 7 | -d2/dxdz (xz) | -0.278 |
| 8 | +d2/dx2 and -d2/dy2 (x^2-y^2) | +0.187 / -0.182 |

So the slot-to-basis-function mapping is the standard real-SH order
`Y00, Y1-1, Y10, Y11, Y2-2, Y2-1, Y20, Y21, Y22` - but **the frame is not world space.** The
signs flip for exactly y, x, yz and xz, and not for xy, z^2 or x^2-y^2: the set that is odd in
exactly one of x and y. A single substitution, **d -> (-x, -y, +z)** (a 180 degree yaw),
explains all eight independently measured signs.

**Our generator had this wrong.** The identical measurement on our own `mp_test_h1` output:
slot 1 -> +y (+0.697), slot 2 -> +z (+0.486), slot 3 -> +x (+0.687). Same magnitudes, matching
vertical term, mirrored horizontal terms. Every probe's horizontal lighting was rotated 180
degrees while the vertical stayed correct - which is precisely why an up/down sanity check
passes and models still look wrong. Fixed by negating x and y inside `sh_basis`.

### Probe positions are not on a uniform lattice

Shipped probePositions carry sub-unit offsets taking exactly two values, 1/32 and 1/16, and
the step histogram shows 32, 64, 128 and 256 spacings in the same map - adaptive cell sizes.
Our generator applies a single constant `probe_epsilon = 1/32` at every level. The exact rule
relating the offset to the local cell size is **not** resolved: on mp_paris the 1/32 group's
dominant spacing is 64 and the 1/16 group's is 32, but mp_dome_dusk's 1/16 group is dominated
by 64, which rules out the obvious proportional and inverse-proportional rules.

### The coeffs/pad split comes from reflection, not from the data

An earlier pass here concluded the struct should be `coeffs[28] + pad[4]`, reasoning that all
four trailing slots are equally always-zero so there was no basis for counting one as a
coefficient. **That was wrong.** IW8 registers the layout explicitly:

```
Load_RegisterStructSize("GfxSHProbeData", 0xC9BD7AA6, 0x40, 0x40)
  member "coeffs", ushort, offset 0,    size 0x3A (58 bytes), count 0x1D (29)
  member "pad",    ushort, offset 0x3A, size 6,               count 3
```

and IW8's own type export declares `coeffs[29]; pad[3];`. So slot 28 *is* a coefficient to the
engine, and `coeffs[29] + pad[3]` is correct. **"Always zero in shipped data" is a statement
about the compiler's output, not about what the consumer reads** — the two are different claims
and only the reflection table answers the second one. Where a member table exists, it outranks
any amount of statistics over the data.

Also worth knowing: IW8 has a *second*, compressed probe type —
`GfxSHCompressedProbeData` is `unsigned int data[8]`, 32 bytes, and IW8's
`GfxGpuLightGrid::probes` uses it rather than the 64-byte form. IW7 stores uncompressed
`GfxSHProbeData`, so this does not apply to us, but it explains why IW8's struct names appear
in two variants.

## 3. Stock probe sets are sparse; ours was dense

Every authentic map references **100.0%** of its probes from its tetrahedra, and its probe set
is a vanishing fraction of the corner lattice its voxel tree spans:

| map | probes | probes referenced by tets | probes as % of the tree's dense lattice |
|---|---|---|---|
| mp_dome_dusk | 44,092 | 100.0% | 0.1% |
| mp_paris | 114,336 | 100.0% | 0.1% |
| mp_afghan | 116,857 | 100.0% | 0.0% |
| mp_frontend | 15,987 | 100.0% | 0.2% |
| **mp_test_h1 (ours, before)** | **35,937** | **23.4%** | **100.0%** |

We emitted the whole `(n+1)³` lattice and tetrahedralised only the occupied fifth of it, so
27,527 of 35,937 probes were referenced by nothing — out of a budget hard-capped at
`max_probes`, which is exactly what forces the leaf size coarser.

## 4. Probe spacing is decoupled from the voxel tree leaf size

This is the biggest structural difference and the generator still gets it wrong.

| map | nodeCoordBitShift | leaf size | probe spacing | rootNodeDimension |
|---|---|---|---|---|
| mp_dome_dusk | (12, 10, 8) | 256 | **32** | (41, 49, 5) |
| mp_paris | (12, 10, 8) | 256 | **32** | (56, 54, 8) |
| mp_afghan | (9, 7, 5) | 32 | **32** | (249, 282, 32) |
| mp_frontend | (9, 7, 5) | 32 | **64** | (13, 45, 3) |
| mp_test_h1 (ours) | (10, 8, 6) | 64 | 64 | (2, 2, 2) |

Stock uses only two shift triples — `(12,10,8)` and `(9,7,5)` — and probe spacing is
independently 32 or 64 units. Our generator derives both from a single `leaf_shift`, so the
voxel cell size and the probe spacing are always equal. Probe positions carry the `+1/32`
offset in every map, ours included.

Stock voxel trees are also far larger and emptier than ours: dome_dusk spans
-126976..40960 with **97.3%** of its 45,668 leaves empty (`0xFFFFFFFF`), afghan has 3,988,123
leaves of which 67.6% are empty. Ours has 7,056 leaves, 0% empty. The tree is sized for local
light binning (it also carries `lightListArray`), not for the probe volume.

## 5. Per-model sample points

The `unk2 == 0` two-sample layout writes the **same position twice** — 100% of pairs identical
in every authentic map. Emitting the position twice is correct, not a placeholder.

But not one of the 12,914 two-sample models across dome_dusk, paris and afghan samples exactly
at its placement origin. Offsets are small and centred (median 0.00 in x and y, slightly
positive in z) with a p5..p95 spread of a few units laterally and up to ±25 vertically — a
model-space **bounds centre** rotated into world space.

Layout distribution is stable across maps: `unk2=0` → 2 points (53–77% of models), `1` → 3,
`2` → 4–65, `3` → 5–65.

## 6. Zone fields

Single zone in every map, with `numProbes`, `numTetrahedrons` and `numVoxelTetrahedronIndices`
always equal to the corresponding global counts. `firstProbe` and `firstVoxelTetrahedronIndex`
are always 0. `firstTetrahedron` is `0` on dome_dusk and cp_zmb, `count - 1` on
paris/afghan/breakneck/frontend — confirming it is a walk seed rather than a range base, and
that `0` is safe.

## What changed in the converter

- **Probes are now emitted sparsely** — only corners of cells that actually get tetrahedra.
  The leaf-size search is sparsity-aware: when a candidate's dense corner count overflows
  `max_probes` it now measures the sparse count before coarsening, bounded by a cell-scan
  budget so a large box cannot make it crawl.
- **`tetrahedronCountVisible` is derived from the bit-31 rule** instead of being set to
  `tetrahedronCount`. We flag no corners, so the honest emission is a count of 0 and a null
  array — the old code declared an entry for every tetrahedron while flagging none, which was
  internally inconsistent and cost 64 bytes per tetrahedron that nothing could address.
- **Static-model samples moved to the model's bounds centre**, transformed by the placement
  axis and scale.

## Still open

- What the 16 values inside each `tetrahedronVisibility` vertex block index.
- Decoupling probe spacing from the voxel tree leaf size (§4). This is the remaining
  structural difference and the largest quality lever; it needs the tree build to take its own
  shift triple, with `voxelStartTetrahedron` pointing at any tetrahedron inside each leaf.
- Whether generating real bit-31 flags plus a baked occlusion table is worth it, given we have
  no ray casting for a per-probe occlusion bake.
- `primaryLightEnvIndex` is `unsigned __int8` at offset 176 in the IW7 database's type library,
  but `unsigned short` in `src/IW7/Structs.hpp`. Both put it at 176 so writes agree for small
  indices, but one of the two is wrong about byte 177.
