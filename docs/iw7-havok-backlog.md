# IW5 → IW7 Havok — working backlog

Live working list for the collision/physics port. Ordered by what unblocks the most.
Each item records what is known, what would settle it, and where the evidence lives.

Ground rules while working unattended: additive changes only, the solution must build
after every item, nothing gets committed, and anything unproven is labelled as such.

## Status

| item | state |
|---|---|
| P0 `PhysicsAsset` for brush models | ✅ generated, byte-identical to all four shipped dummies |
| P1 does the world blob validate | ✅ yes — structure, geometry, simdTree and fixups |
| P2 BVH AABB nibble codec | ✅ solved — full tree decodes, 100% leaf containment |
| P2a per-section vertex resolution | ✅ solved — they were custom primitives, not vertices |
| P3 Havok shapes for triggers | ✅ done, behind `ZT_HAVOK_TRIGGER_SHAPES=1` |
| P4 `minHalfAngle` | ✅ encoding exact from IW8; input angle approximate |
| P5 `shapeTagCodecInfo` | ✅ always `0xFFFFFFFF` — the old `0x02130004` note was wrong |
| P6 compressed mesh in the ents blob | ✅ a non-convex brush model; converter never needs one |
| P7 `mp_breakneck` X offset | ✅ it is the collision blob, not the entities |
| P8 surface materials | ✅ per-surface materials from an empirically derived CRC table |
| P9 clip volumes dropped from world collision | ✅ fixed — behaviour change, test it |
| P10 section BVH leaves all named primitive 0 | ✅ fixed, with a regression check |
| P11 writer audit against the solved decoders | ✅ complete — 2 bugs found (P10, P12), rest clean |
| P12 degenerate triangles emitted as custom primitives | ✅ fixed — confirmed in shipped output |
| P13 welded convex faces could repeat a vertex index | ✅ fixed, latent |
| P14 the six guessed material mappings | ◐ 1 confirmed, 3 unresolvable, 2 no signal |
| P4a exact `minHalfAngle` input angle | ⏸ deliberately deferred — low value, see P4 |

**Nothing here has been run in the game.** Everything is validated against shipped data,
byte-exact comparison, or IW8 ground truth.

## What to do next (needs the MW3 machine)

Three fixes this session change what collision a converted map actually gets, and none can
be confirmed from shipped data alone:

| fix | effect if right | risk if wrong |
|---|---|---|
| **P9** clip volumes kept | players stop walking through clip walls and off ledges | more collision than before, including invisible volumes |
| **P10** BVH leaf indices | collision queries resolve to the correct primitive | was resolving to the wrong one within a section |
| **P12** degenerate triangles dropped | no out-of-bounds custom-primitive shape types | — |

The check is mechanical:

1. Deploy the rebuilt `build\bin\Win32\Debug\ZoneTool.dll` to the MW3 directory.
2. **Re-dump** the map in MW3 — conversion happens at dump time, so relinking an existing
   zone will not pick these up.
3. Run the validator over the result:

```bash
python tools/iw5cmconv/hkxtool.py check "D:/Games/PC/IW7/dump/*/maps/mp/*.colmap.hkx"
```

and the entity shapes, which take globs too and exit non-zero on failure:

```bash
python tools/iw5cmconv/entscheck.py "D:/Games/PC/IW7/dump/*/maps/mp/*.ents.data.hkx"
```

The currently-shipped `mp_test_h1` output fails three of those checks, so a clean run is a
real signal rather than a vacuous one. For reference, before the fixes it reports:

```
- 7 sections whose BVH leaf primitive indices are not a permutation of 0..primitiveCount-1
- 4 custom primitives with a shape type outside the 3-entry table {4: 1, 10: 1, 14: 1, 12: 1}
- 640 per-section BVH leaf boxes do not contain their primitive
```

If the leaf-box failures persist after a re-dump they are a separate bug, not a P10
side-effect, and that distinction is worth knowing.

Three claims in the pre-existing notes turned out to be wrong and have been corrected:
`shapeTagCodecInfo == 0x02130004`, "leaf order equals primitive order" in the section BVH,
and the direction of the `mp_breakneck` offset.

---

## P0 — Generate a `PhysicsAsset` for brush models  ✅ **done**

`sub_140146DA0` only builds a Havok body when `cmodel_t::physicsAsset` is non-null -- the
shape override is read *only* when the asset is present -- so without this the convex
compounds produced no body at all.

`havok::builder::build_physics_asset()` emits the blob and reproduces all four shipped
dummies **byte for byte**, checked against copies from two different maps:

```
scriptbrushmodeldummydefault      BYTE-IDENTICAL
scriptbrushmodeldummyfixed        BYTE-IDENTICAL
triggermodeldummydefault          BYTE-IDENTICAL
triggermodelstaticdummydefault    BYTE-IDENTICAL
```

The family is two parameters: body name (`scriptbrushmodeldummy` / `triggermodeldummy`)
crossed with a body-quality CRC (`0x7923E35C` default / `0xD0452309` fixed). Details in
[iw7-ents-shapes.md](iw7-ents-shapes.md) section 7.

`IW7::IPhysicsAsset` writes the assetmanager stream beside it. The stream is
`dump_single(asset)` over the 0x50-byte struct, the name string, then one asset token per
SFX and VFX event slot (`numSFXEventAssets` and `numVFXEventAssets` are both 1 on every
shipped asset, and both point at nulls). The converter builds the asset, points every
shaped brush model at it, and the IW5->IW7 clipmap dumper emits each distinct one once.

Evidence the linker accepts the type: `mp_frontend`, itself a converted map, ships 32
physicsasset files and 16 `physicsasset,` CSV lines.

Not verified: that a converted map actually loads with it. That needs a re-dump and a run.

## P1 — Does the generated *world* blob validate?  ✅ **yes**

Answered. Three rooms were generated straight from `build_world_shape` (32, 320 and 1760
triangles, forcing 1, 3 and 14 sections so section splitting and the top-level BVH are
exercised) and put through the stock-derived checks:

| | `hkxtool check` | geometry round-trip | simdTree | fixups |
|---|---|---|---|---|
| 32 tris, 1 section | OK | 32/32, bounds exact | 22 nodes, 21 visited, 32/32 leaves | clean |
| 320 tris, 3 sections | OK | 320/320, bounds exact | 150 nodes, 149 visited | clean |
| 1760 tris, 14 sections | OK | 1760/1760, bounds exact | 1078 nodes, 1077 visited | clean |

`visited == nodes - 1` matches stock exactly (node 0 is the sentinel), `badRefs` is 0 in
all three, and the decoder reads back precisely the triangles and bounds that went in. The
fixup discipline that the ents emitter got wrong is already correct here: no local fixup
points at an object, no global fixup misses one, and every object is reachable.

Caveats: this validates against the decoder's understanding of the format, not against the
game. The conservative BVH boxes of P2 are still present (correct, just loose).

Status: **done.**

## P2 — Per-section BVH AABB codec  ✅ **solved**

Each section carries an `hkcdStaticTree::AabbTree<hkcdCompressedAabbCodecs::Aabb4BytesCodec>`
in `section.nodes`. A node is 4 bytes — `uint8 xyz[3]` then `uint8 data`:

```
isInternal = data & 1
left       = n + 1
right      = n + (data & 0xFE)
leaf primitive index = data >> 1

childMin[i] = parentMin[i] + (xyz[i] >> 4)^2   * parentExtent[i] / 225
childMax[i] = parentMax[i] - (xyz[i] & 0x0F)^2 * parentExtent[i] / 225
```

The root's parent box is the stored section domain. Each axis byte holds two 4-bit inset
codes, and **the inset is quadratic in the code, not linear**; `225 == 15^2`, so code 15
collapses the box onto the opposite face.

### Verification

Every leaf box must contain its primitive. Across all three stock world blobs, all
sections, that holds for **100.00%** of leaves — `mp_afghan` 97,962, `mp_paris` 121,843,
`mp_breakneck` 134,620. This is now enforced by `hkxtool check`, which passes on all three.

The containment test needs a tolerance of 1% of the section extent with a 0.05-unit floor,
and that deserves justification rather than being waved through. The encoder built these
boxes from the original float geometry while the vertices were separately quantised, so the
two disagree slightly by construction. The disagreement is small and bounded: median 0.13%
of the section extent, p99 0.3%, and **the largest absolute overshoot anywhere in a map is
0.03 units** — sub-millimetre at CoD scale, where a player is 72 units tall. The floor
exists because a handful of sections are only a fraction of a unit across, where 0.01 units
is already several percent.

Critically, that tolerance still discriminates. At the identical bound:

| codec | mp_afghan | mp_paris |
|---|---|---|
| **`v²/225`** | **100.00%** | **100.00%** |
| `v²/240` | 74.6% | 56.7% |
| `v²/256` | 54.6% | 34.6% |
| `v²/196` | 0.02% | 0.05% |
| linear `v/15` or `v/16` | 0.01% | 0.00% |

So the pass is not an artefact of a loose bound — the correct codec sits well inside it
(max overshoot 0.5–0.8%) while the nearest alternatives pile up against it.

### What had blocked it

Two things, both now fixed. P2a's custom primitives were corrupting the reference geometry.
And the containment check used an absolute `1e-3` epsilon on sections hundreds of units
across — far tighter than the encoder's own precision — which made the correct codec look
like a 7% match and sent the earlier investigation hunting for a different composition rule.
The tell was that parent-relative containment *decayed with depth* (83% → 47% → 33% → 14%),
the signature of a frame that is right but marginally too tight, compounding per level.

Implemented as `decode_section_tree` in `tools/iw5cmconv/hkcompressedmesh.py`.

## P2a — Per-section vertex resolution  ✅ **solved**

The long-standing "~7% of primitives decode to a vertex outside their own section domain"
error was not a vertex-resolution bug at all. **Those primitives are not triangles.**

A primitive whose `indices[1] == indices[2] == indices[3]` is a Havok **custom primitive**.
Its `indices[0]` does not name a vertex — it indexes `sharedVerticesIndex`, and the word
found there is a descriptor whose low nibble selects a shape type from

```c
// IW8 1-game_test.exe.c
const hknpShapeType::Enum
hknpCompressedMeshShapeInternals::s_customPrimitiveToShapeType[3] = { GSK, SET_SHAPE_KEY_A, NOP };
```

IW8 reads it that way in three places, e.g.

```c
v303 = s_customPrimitiveToShapeType[ decoder.m_sharedVerticesIndex[
           decoder.m_primitives[v343].m_indices[0] ] & 0xF ];
v168 = (decoder.m_sharedVerticesIndex[ ...m_indices[0] ] >> 4) & 3;
```

Feeding that descriptor word into the vertex path is what produced the phantom failures.

### The evidence

The `indices[1] == indices[2] == indices[3]` signature matches the failures **exactly** —
7,753 of 7,753 in `mp_afghan` and 18,250 of 18,250 in `mp_paris`, with no false positives
and no misses. The descriptor's low nibble is `2` (**NOP**) for every custom primitive in
all three stock world blobs, so they contribute no geometry and are correctly skipped.

Skipping them takes vertex resolution to **100.00%** on every stock world blob:

| map | before | after | custom primitives |
|---|---|---|---|
| `mp_afghan` | 97.5% | **100.00%** (384,810/384,810) | 7,790, all NOP |
| `mp_breakneck` | — | **100.00%** (532,739/532,739) | 13,633, all NOP |
| `mp_paris` | 95.4% | **100.00%** (478,080/478,080) | 18,404, all NOP |

It also fixed a second problem for free: the 38 per-model meshes that used to raise
`NotImplementedError` ("the single-section `numPackedVertices==0` variant") now decode.
**All 389 mesh objects across every shipped `.hkx` decode, zero failures.**

### Why the earlier investigation could not see it

Every structural hypothesis it tested was about *where the vertex lives* — page, field
packing, section attribution, the domain-as-bound assumption, the algorithm against IW8.
All of those were correctly eliminated, because all of them were the wrong question. The
tell was visible in the data the whole time: the failing entries are not distributed like
indices. They have low byte `0x02` in 7,751 of 7,753 cases and resolve to just 27 distinct
"vertices" — a descriptor word being read as an index, not an index gone astray.

One earlier note is now retracted: the claim that the 38 undecodable per-model meshes hold
"a stale constant pattern" in `sharedVerticesIndex` was wrong. That constant pattern is the
custom-primitive descriptor, and it is meaningful.

Status: **solved and implemented** in `tools/iw5cmconv/hkcompressedmesh.py`, which now skips
both `0xDEADDEAD` padding and custom primitives, and reports the latter on `DecodedMesh.
custom_primitives`. The converter never needs to *emit* a custom primitive; this was
blocking the decoder used to validate everything else.


## P3 — Havok shapes for triggers  ✅ **done (off by default)**

Stock gives every trigger a Havok compound alongside its slab hulls, for physics bodies
overlapping the volume. `collision::extract_trigger_hulls()` builds them: a `TriggerHull` is
an AABB intersected with its slabs, and a `TriggerSlab` is a *pair* of parallel planes at
`dot(p, dir) = midPoint ± halfSize`, so the volume is already convex and the same clipping
that turns a brush into a hull applies unchanged. Trigger hulls are entity-local in both
games, so unlike brush models these need no rebasing.

Validated against every trigger in all 18 stock MW3 maps:

```
1650 hulls -> 1650 closed convex polytopes    (>= 4 faces)
   0 degenerate, 0 failures
   0 exceeding their declared TriggerHull bounds
   hull extent / declared bounds: p10 1.000  p50 1.000  p90 1.000
```

and the slabs demonstrably cut rather than being silently dropped:

| slabs | resulting faces |
|---|---|
| 0 | 6 in all 996 hulls |
| 1 | mostly 7–8 |
| 5 | mostly 12 |
| 8 | 15 |

Hulls that keep 6 faces despite having slabs are ones whose slab planes fall outside the
box and cut nothing, which is legitimate.

**Off by default, behind `ZT_HAVOK_TRIGGER_SHAPES=1`.** Script triggers already work from
the slab hulls alone and read neither `physicsAsset` nor `physicsShapeOverrideIdx`
(docs/iw7-triggers.md section 3). A body attached with the wrong quality would make a
trigger solid, which is a worse regression than not having one — and that cannot be checked
without running the game. The geometry and the plumbing are done and testable; flipping the
flag is a one-line decision once someone can launch a converted map.

When enabled, triggers get `triggermodeldummydefault`, generated by the same byte-exact
`build_physics_asset()` as the brush-model dummy, and the dumper emits it.

## P4 — `minHalfAngle`  ✅ **quantisation solved exactly**

Read straight out of IW8's encoder rather than fitted. The caller of
`anonymous_namespace_::findFaceSupportAngle` does, with the support angle in radians:

```
v            = (int)((angle * 0.5 - 2^-23) * 41720.875 + 0.5)
minHalfAngle = clamp(v, 0, 65535) >> 8
```

`41720.875` is approximately `131072/pi`, so this is `floor(halfAngle / pi * 512)`. A right
angle gives `pi/4 * 41720.875 + 0.5 = 32766.4`, and `32766 >> 8 = 127` -- which is what
every axial brush produces, and what 6,441 of 7,794 shipped faces store. The emitter now
uses this verbatim; the earlier `round(half / 45 * 127)` was an approximation of it and was
off by one on cases like 40.14 degrees (113 vs the correct 114).

**What is still approximate is the input angle, not the encoding.** `findFaceSupportAngle`
scans *all* other faces and tests whether their first and last vertex indices appear in this
face, which is a different adjacency notion from the half-edge twin the tool uses. Measured
against stock the match rate is unchanged at 80-92% per map, so the residual lives entirely
in that definition. Since `minHalfAngle` is a query-widening hint rather than geometry, and
since brush-derived hulls are overwhelmingly right-angled where both definitions agree
exactly, this is left as a refinement.

Generated blobs are at 100% by construction (`gen_shapes.hkx`: minHalfAngle 100% of 64).

Status: **done for the encoding; input angle a known approximation, deliberately deferred.**

Revisited: `findFaceSupportAngle`'s exact adjacency rule is recoverable from IW8 in
principle -- the outer structure is readable (for every other face, walk its indices and test
whether that index *and* that face's last index both occur in this face) -- but the body then
goes into dense inlined SIMD that would take substantial effort to transcribe faithfully.

Not worth it. `minHalfAngle` is a query-widening hint, not geometry: a wrong value costs
query breadth, never collision correctness. Brush-derived hulls are overwhelmingly
right-angled, where every candidate definition agrees exactly, and generated blobs are
already 100% self-consistent. The 8-20% residual against stock is the *checker's* fidelity to
Havok's tie-breaking, not a defect in what is emitted. Recorded as closed-by-choice rather
than left looking unfinished.

## P5 — `shapeTagCodecInfo`  ✅ **answered: it is always 0xFFFFFFFF**

The open question recorded it as `0x02130004`. That was a misread. Measured at
`hknpCompositeShape + 88` across every shipped composite shape:

```
hknpDynamicCompoundShape   0xFFFFFFFF  x651
hknpCompressedMeshShape    0xFFFFFFFF  x46
hknpStaticCompoundShape    0xFFFFFFFF  x2
```

699 of 699. Both emitters already write `0xFFFFFFFF`, so nothing to change.

The codec itself is another IW custom class, `HavokPhysicsShapeTagCodec`, and IW8 names
`HavokPhysicsShapeTagCodec::SetData(hkArray<HavokPhysicsShapeList::ShapeTagData>*)` plus
`findShapeTag(ShapeTagData*) -> uint16`. So the codec is handed the shape list's own
`shapeTagData` array and resolves tags out of it, which closes the material path end to end:
`primitiveDataRuns.value` -> `shapeTagData` index -> `materialCRC` -> runtime `materialId`.

Status: **done.**

## P6 — The compressed mesh inside the ents blob  ✅ **answered**

It is a brush model whose geometry is *not* a union of convex brushes, so the compiler
emitted a mesh instead of a convex compound.

Every MP map carries exactly one, and they are the same asset:

| map | name | tris | extent (CoD units) | contents |
|---|---|---|---|---|
| `mp_afghan` | `mp_afghan_paths.map:entity 31` | 192 | 5632 x 5632 x 3328 | `0x200` |
| `mp_paris` | `mp_paris_paths.map:entity 31` | 192 | 5632 x 5632 x 3328 | `0x200` |
| `mp_breakneck` | `..._spawns.map:entity 32` | 192 | 6432 x 5632 x 3328 | `0x200` |
| `mp_dome_dusk` | `mp_dome_dusk_paths.map:entity 31` | 192 | 5632 x 5632 x 3328 | `0x200` |

That is the air/helicopter boundary volume from the standard `_paths` prefab -- 113 verts,
192 triangles, identical everywhere, and `contents 0x200` marks it. In `mp_dome_dusk` it is
slot 8, referenced by `cmodels[1]`, whose bounds are +/-2817 x +/-1665 -- exactly 32x the
mesh's +/-88 x +/-52, confirming both the ownership and the 1/32 scale.

`cp_zmb` is the exception with ~20 of them: concave prefab props (`magic_wheel`,
`park_office_board_01`, blockout geometry) that genuinely need a mesh.

**Implication for the converter: it never needs to emit one.** IW5 brush models are unions
of `cbrush_t`, which are convex by construction, so convex compounds are always adequate on
this path.

Status: **done.**


## P7 — `mp_breakneck` entity-origin X offset  ✅ **answered (and the old note had it backwards)**

The note suspected the `.ents` key `543` reading. It is not that: **static models and entities
agree with each other, and the collision blob is the outlier.**

| mp_breakneck | p5 | p50 | p95 |
|---|---|---|---|
| entity origins, x | -42,576 | **-39,856** | -37,392 |
| static model origins, x | -45,731 | **-40,036** | -37,666 |
| collision domain, x | | **-2,506 .. 4,034** | |

Y and Z agree across all three. `mp_paris` and `mp_afghan` agree on all axes, so the parser
is fine -- it is breakneck specifically.

The collision blob really is authored ~40,000 units away in X: the shape list's `minMaxes`,
the mesh tree domain and all 2,029 section domains say the same thing, and the file contains
exactly one mesh, so this is not a misread of a multi-shape blob.

**It does not affect the converter.** What matters is that collision, static models and
entities stay in one consistent space *within* a map, and the IW5 -> IW7 path copies all
three across unchanged. It also does not disturb the 1:1 world-scale conclusion, which
rested on extents rather than positions -- and static model origins are a cleaner witness
for that than the entity cloud anyway.

Incidental, and consistent across both maps checked: `script_brushmodel` entities sit in
their own band exactly 10,000 units below the main entity cluster in X (paris: main 0,
brush models -10,000; breakneck: main -40,000, brush models -50,000).

Status: **done.**


## P8 — Surface materials  ✅ **implemented**

`ShapeTagData::materialCRC` drives footsteps, impact effects and penetration. The builder
used to write one constant for every tag, so a converted map had a single material
everywhere. It now carries the source surface's material through.

**The CRC names could not be recovered, and are not needed.** `HavokPhysicsMaterialList`
calls the field `m_materialsNameCRC32s`, so it is a CRC32 of a material name, but the values
match no CRC32 variant (poly/init/reflect/xorout swept, 32 combinations) of any plausible
surface-type name. Instead each CRC was identified by *what it is used on*, reading the
2,224 shipped `PhysicsAsset`s -- which are named after their models, so the material is
unambiguous:

| CRC | models | material |
|---|---|---|
| `0xCBF7A6C4` | 618 | ladders, handrails, racks, guardrails → **metal** |
| `0x4B02BC9D` | 531 | pipes, hoses, wires, thin panels → **thin metal** |
| `0x1AB7BC33` | 411 | concrete walls, barriers → **concrete** (also the dummies' default) |
| `0xE8F3FA9A` | 227 | AC units, lockers, gutters, vents → **painted metal** |
| `0x0AD71E4E` | 130 | crates, plywood, wooden chairs → **wood** |
| `0xA1F93A3B` | 54 | books, cardboard, magazines → **paper** |
| `0xCD123193` | 44 | caps, shirts, curtains, tarps → **cloth** |
| `0xF728E572` | 41 | bottles, broken glass, `glasschunkdummy` → **glass** |
| `0x0103BCE1` | 37 | rocks, cliffs, rubble → **rock** |
| `0x63A1FDAD` | 22 | bushes, trees, cactus, hedges → **foliage** |
| `0xF49C81BF` | 21 | mugs, plates, bowls, vases → **ceramic** |
| `0xFFD772CD` | 14 | balloons, tyres, yoga mat → **rubber** |
| `0x98C096F9` | 11 | sofas, pillows, insulation → **cushion** |
| `0x4724ADF2` | 10 | zombie limbs, ragdolls → **flesh** |
| `0x4FE888BA` | 6 | bananas, watermelon, lemons → **fruit** |
| `0x46FAE51C` | 6 | asphalt rubble → **asphalt** |
| `0x96309552` | 5 | sandbags → **sand** |
| `0xD8B39111` | 5 | cobblestone, broken brick → **brick** |
| `0xAE2DE6F5` | 1 | `destruction_mud_pile_01` → **mud** |
| `0x00C9A2F0` | 1 | `..._wishing_pool_01_water` → **water** |

`collision::iw7_material_crc()` maps IW5's `materialSurfType_t` (the top 12 bits of
`ClipMaterial::surfaceFlags`) onto these. Six of the 31 IW5 types have no shipped IW7
material to match against -- ice, plaster, snow, riot shield, slush, and plastic as distinct
from rubber -- and fall back to the nearest, marked in the table in the source. Those are
guesses at the closest material, not readings, and are the part most worth a second opinion.

The tag palette is keyed on `(contents, materialCRC)`, so surfaces differing in either get
their own `ShapeTagData` and identical ones collapse. Verified end to end: a test room with
wood, concrete and glass surfaces produces exactly three tags carrying `0x0AD71E4E`,
`0x1AB7BC33` and `0xF728E572`, and the blobs still pass `hkxtool check`.

Also corrected while here: a comment claiming `shapeTagData[0]` is reserved and that real
geometry landing there stops colliding. The code never implemented that reservation, and the
data contradicts the reasoning -- stock *does* reference tag 0, for exactly 6 primitives in
each of afghan, paris and breakneck. Six primitives with the sky contents bit (`0x800`) is
the sky box; it sits in slot 0 only because the compiler emits it first.

Status: **done, with six fallback mappings flagged for review.**


## P9 — World collision was dropping every clip volume  ✅ **fixed**

`ClipMapCollision::extract` skipped any brush without `CONTENTS_SOLID`, on the reasoning
that anything else is "a helper or a special-purpose volume, not a visible surface". That
conflates visible with collidable: clip volumes are invisible *and* collidable, and they are
exactly what stops a player walking through a gap or off a ledge.

Stock IW7 world collision is full of them:

| map | tags with PLAYERCLIP/MONSTERCLIP | solid-only tags | neither |
|---|---|---|---|
| `mp_afghan` | 31 | 58 | 43 |
| `mp_paris` | 28 | 84 | 204 |

In `mp_paris` only 84 of 316 surface tags are solid-only; the largest single class is
`0x00000010` with 142 tags. Requiring `CONTENTS_SOLID` would have discarded the majority of
what stock actually ships.

The filter now drops only what positively must not collide -- `CONTENTS_NONCOLLIDING`,
`CONTENTS_ORIGIN` (a brush model's pivot marker) and empty masks -- alongside the triggers
and brush models that leave through `MapEnts` instead. The contents mask rides into
`ShapeTagData::collisionFilterInfo`, so a playerclip surface still filters to players and
not to bullets.

**This is a behaviour change to world collision and is worth testing first.** Converted maps
will now have more collision than before, and legitimately more than their visible surfaces.
The extractor logs how many kept brushes are clip volumes -- that number used to be zero.

Also fixed alongside: trisoup triangles were all tagged `CONTENTS_SOLID` on the grounds that
"trisoup carries no contents mask of its own". Its `ClipMaterial` does, so sky, water and
clip trisoup were all colliding as world geometry. They now take the material's contents,
with solid as the floor if a material reports none.

Status: **done, needs an in-game check.**

## P10 — Every section BVH leaf named primitive 0  ✅ **fixed**

Found by turning the newly-solved P2 decode back on our own writer — the first time the
emitted BVH could be checked at all.

`emit_preorder_tree_section` wrote the leaf's data byte as a bare `0`:

```cpp
buf.write<std::uint8_t>(0); // leaf: bit 0 clear
```

Bit 0 clear does mark a leaf, but **the remaining 7 bits are the index of the primitive the
leaf bounds** — the byte is `(primitiveIndex << 1)`. Writing 0 pointed every leaf in every
section at primitive 0. The tree had the right shape and the right boxes, and named the
wrong geometry at every leaf.

Stock is unambiguous: in all **5,302** multi-primitive sections across the three shipped
world blobs, the leaf indices are exactly a permutation of `0..primitiveCount-1` — never
duplicated, never all-identical. The fix emits `(first_leaf << 1) & 0xFE`, and `first_leaf`
is already the primitive index at that call site.

This would not have shown up in any earlier check. The AABB containment test cannot catch it
either, because with the boxes correct a wrong index still lands inside a parent box. So the
invariant is now checked directly: `hkxtool check` asserts the leaf indices are a permutation
of `0..pc-1`. Verified both ways — all three stock blobs pass, and a writer deliberately
reverted to `u8(0)` fails with exactly that message.

The same bug was in the Python mirror `selftest_writer.py`; fixed there too.

Two stale comments were retired alongside it. The file header and the quantisation block both
still claimed the BVH nibbles were "emitted as zero" and that Havok's quantisation was "not
solved yet" — neither was true of the code beneath them.

**Not yet run in the game.** The practical effect of the old behaviour would have been
mis-resolved collision queries within a section.

## P11 — Auditing the writer against the solved decoders  ✅ **complete**

Solving P2 and P2a made it possible to check what the writer *emits*, not just what stock
contains. The sweep found two real bugs and cleared everything else.

| emitted structure | verdict |
|---|---|
| section BVH leaf primitive index | ✗ **broken** — P10, every leaf named primitive 0 |
| triangle indices | ✗ **broken** — P12, degenerate triangles read as custom primitives |
| BVH nibble quantisation | ✓ `quantise_inset` always rounds down, so the decoded box is guaranteed to contain the child, and it recurses on the *decoded* parent as the decoder does |
| data runs | ✓ RLE from index 0, tiling contiguously to `primitiveCount`, matching stock in all 5,317 sections |
| section domain | ✓ bounds its vertices and never escapes the tree domain |
| simdTree primitive keys | ✓ correct, including the easily-missed `<< 1` — see below |
| `max_key` / `bits_per_key` | ✓ the largest key is necessarily in the last section, so `sections.back()` is the right source |
| empty-input guards | ✓ both `input.triangles.empty()` and `sections.empty()` return early, so the new P12 filter cannot produce an empty-vector deref |

### The primitive key format

A key is **`(section << 8) | (primitiveIndex << 1)`**. The low bit is reserved, exactly as in
the BVH leaf byte — so the primitive index is *shifted*, and reading the low byte directly
names the wrong primitive for half of all keys.

Derived from stock rather than assumed. Under this decode 100% of keys name a real
`(section, primitive)` pair and the keys are a bijection onto the primitive set — `mp_afghan`
105,752 keys onto 105,752 primitives. Every other candidate fails badly:

| decode | keys naming a real primitive (mp_afghan) |
|---|---|
| **`sec = k>>8, prim = (k&0xFF)>>1`** | **105,752 / 105,752** |
| `sec = k>>8, prim = k&0xFF` | 53,225 |
| `sec = k>>8, prim = k&0x7F` | 80,507 (and not a bijection) |
| `sec = k>>9, prim = (k>>1)&0xFF` | 38,661 |
| `sec = k>>7, prim = k&0x7F` | 33,945 |

The writer already had this right. It is now asserted by `hkxtool check` so it stays right.

### Checks added this pass

`hkxtool check` now enforces, all passing on stock and on the mirror: BVH leaf indices are a
permutation of `0..pc-1`; BVH leaf boxes contain their primitives; custom-primitive shape
types are within the 3-entry table; section domains bound their vertices and stay inside the
tree domain; data runs tile `0..primitiveCount`; and primitive keys decode to a bijection.

### The gap that remains

`selftest_writer.py` emits **zero** quantisation nibbles, so the checker does not exercise
the C++ quantisation path. Only a real converted map does that — see P12, where checking one
found a bug.


## P12 — Degenerate triangles were emitted as custom primitives  ✅ **fixed**

A triangle is emitted as `[a, b, c, c]` — `indices[2] == indices[3]` is what marks it a
triangle rather than a quad. But Havok reads `indices[1] == indices[2] == indices[3]` as a
**custom primitive**, and then indexes

```c
hknpCompressedMeshShapeInternals::s_customPrimitiveToShapeType[3]  // { GSK, SET_SHAPE_KEY_A, NOP }
```

with the low nibble of `sharedVerticesIndex[indices[0]]`.

So any triangle whose second and third vertices weld to the same index collides with the
custom-primitive encoding *by accident*. Welding is by exact float equality and there was no
degeneracy filter, so a zero-area triangle in the source map produced one directly.

**This was not hypothetical.** The `mp_test_h1` conversion sitting on disk contains four of
them, and their descriptors decode to shape types **4, 10, 12 and 14** — every one out of
bounds on a three-entry table. What the runtime does with an out-of-bounds shape type is not
something worth finding out.

The fix drops degenerate triangles before welding — testing positions rather than welded
indices, so no orphan vertices are added for a triangle about to be discarded. Zero-area
triangles contribute no collision, so nothing is lost. The count is logged.

### Found by checking real converter output

This came out of running `hkxtool check` over `mp_test_h1.d3dbsp.colmap.hkx`, which is
genuine converter output rather than stock. It fails three ways:

```
- 7 sections whose BVH leaf primitive indices are not a permutation of 0..primitiveCount-1
- 4 custom primitives with a shape type outside the 3-entry table {4: 1, 10: 1, 14: 1, 12: 1}
- 640 per-section BVH leaf boxes do not contain their primitive
```

The first **independently confirms P10 was a real shipping bug**, not just a code-reading
error — that file predates the fix. The third is most likely a consequence of the same bug
(a wrong leaf index means containment is tested against the wrong primitive), but that
cannot be asserted until a map is re-dumped with the fixed writer; it is not being claimed
as fixed.

All three stock world blobs still pass every check, so the new assertions discriminate
rather than merely being strict.

Status: **fixed; needs a re-dump to confirm the output is clean.**

## P13 — A welded convex face could repeat a vertex index  ✅ **fixed (latent)**

Found by auditing `build_convex_hull` against the invariants `entscheck` already knows how
to verify.

Hull faces are built by clipping a winding against every other plane, then welding each
corner into a shared vertex list with a 0.05-unit tolerance. Welding can collapse two
adjacent corners of a face onto the same index. The code tested for that — but only to
*count* distinct corners:

```cpp
auto distinct = face.indices;
std::sort(...); distinct.erase(std::unique(...));
if (distinct.size() < 3 || ...) continue;
```

`face.indices` itself kept the duplicate. A repeated index is a zero-length edge, so the
half-edge connectivity Havok builds from the ring refers to an edge that does not exist.

The fix removes consecutive duplicates cyclically, so the ring stays a simple polygon, and
then rejects the face outright if any duplicate survives — a pinched polygon is not safe to
emit either. Dropping a face can leave the hull with fewer than four, in which case
`build_convex_hull` already returns false and the brush is skipped, which is the safe
outcome.

Winding is still measured from the original `w.points`, which is correct: removing duplicate
consecutive corners contributes zero area and cannot change the sense.

**This is latent, not observed.** No shipped conversion is known to contain it, and it needs
two corners of one face to land within 0.05 units of each other. It is the same shape of bug
as P12 — welding creating an encoding collision — which is why it was worth looking for.

The invariant is now asserted: `entscheck` rejects any face that repeats a vertex index or
carries fewer than three. Stock passes across every face of all 343 shipped convex shapes,
so the assertion discriminates rather than merely being strict.

## P14 — Revisiting the six guessed material mappings  ◐ **as far as the data goes**

Six IW5 surface types mapped to IW7 material CRCs on judgement rather than evidence. Two
approaches were tried.

### Cracking the hash — failed, and worth recording as closed

If the CRC were a hash of a material name, the whole table could be computed rather than
inferred. It was brute-forced against the 20 known CRCs: 36 base names x 14 prefixes x 11
suffixes x 3 cases x 13 hash functions (CRC32 IEEE/normal/inverted, CRC32C, FNV-1, FNV-1a,
djb2 both variants, sdbm, Jenkins, Adler-32, MurmurHash3) — **216,216 combinations, no hit**.

IW8 cannot help either: it sets `shapeTagData.m_materialCRC = 0` unconditionally at every
site. The field is IW7-specific.

So the name space is not a plain surface-type string, and this route is closed short of
finding IW7's own hashing code.

### Enrichment over the shipped corpus — settles one

For each candidate material, how much more often does a CRC appear on assets *named* for it
than across all 2,335 shipped `PhysicsAsset`s? The method validates on materials already
known: wood-named assets are **8.1x** enriched for the wood CRC, glass-named **10.7x** for
glass.

| type | assets | result |
|---|---|---|
| **PLASTIC** | 18 | **CONFIRMED → RUBBER**, 21.6x enrichment — the strongest measured anywhere, above both controls. IW7 has no separate plastic material. |
| ICE | 30 | no signal (best 3.2x on 2 assets), and the group is contaminated by substring matches like `ice_cream_cooler` |
| RIOT_SHIELD | 4 | discounted — the "shield" assets are zombie-mode crafting props (`p7_zm_ctl_shield_armory_*`), not riot shields, so they are not evidence about this surface type |
| SNOW | **0** | no evidence |
| PLASTER | **0** | no evidence |
| SLUSH | **0** | no evidence |

`PLASTIC -> RUBBER` is now a reading rather than a guess.

**SNOW, PLASTER and SLUSH cannot be settled from shipped IW7 data.** Not "weak evidence" —
the corpus contains no asset named for any of them. They stay guesses permanently unless
another source appears, and are now labelled that way rather than looking merely unverified.
Since all three fall back to the concrete/default CRC, the practical cost is footstep sounds
and impact effects on snow-type surfaces, not collision behaviour.

---

## Done

- **Fixup discipline** — object pointers must be *global* fixups, payload pointers *local*.
  The ents emitter had them all local. Verified 532/532 vs 0/1420 on stock; fixed and now
  enforced by `entscheck.py`.
- **Convex vertex padding** — `vertices` is padded to a multiple of four repeating the last
  vertex, and `vertexEdges` with it. 1848/1848 shipped convexes. The emitter did not pad,
  which would have made Havok read past the array for any hull whose vertex count is not a
  multiple of 4 (i.e. most non-box brushes).
- **The vertex `w` lane carries the vertex index** — `0x3F000000 | i`, i.e. 0.5f with the
  index in the low mantissa bits. 16,257 of 16,660 shipped vertices match exactly, and the
  403 that do not are the padded copies, which repeat the last real vertex's `w` with its
  position. The emitter was writing `0.0f`, so every vertex claimed index 0.
- **Fixup table alignment** — each table is padded to 16 with `0xFF`, so every section
  offset lands aligned. All nine shipped files do this; neither emitter did.
- **Local fixup ordering** — stock orders local fixups by *destination*, not source
  (virtual by source, global in emission order). Nine of nine shipped files.
- **Two indexing schemes in the shape list** — slot-indexed vs compacted-indexed arrays,
  which only differ when the list has a null shape (`cp_zmb` alone). Explains
  `shapeIndices`, which is a compaction map rather than the identity it looks like.
- `entscheck.py` now passes all four shipped ents blobs *and* the generated ones with no
  per-file exceptions.

- Trigger volumes: `MapEnts::trigger` slab hulls, `?N` zero-based, identical contents
  values. See [iw7-triggers.md](iw7-triggers.md).
- `MapEnts::havokEntsShapeData` decoded and generated (convex polytopes in dynamic
  compounds, CoD units / 32). See [iw7-ents-shapes.md](iw7-ents-shapes.md).
- `HavokPhysicsShapeList` member table, `CM_ContentsOfBrushModel` null-deref, the
  `physicsAsset` + `physicsShapeOverrideIdx` pairing.
