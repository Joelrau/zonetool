# IW7 Havok collision — format notes and IW5 porting plan

Status: **reading is solved; writing is not.** The packfile writer round-trips every
`.hkx` on disk byte-exact, and the mesh decoder reads all three stock world blobs
(`mp_afghan` exactly; `mp_paris` +1 and `mp_breakneck` +10 triangles, both explained by
`0xDEADDEAD` padding) plus 46 of the 83 per-model mesh objects. What is still missing before anything can be *generated* is the BVH node
codec — see section 7. Everything in sections 1–5a is verified against shipped IW7 data or
against IW7's own reflection tables.

Goal: port IW5 (MW3) map collision to IW7. IW7 has no brush/BSP collision at all — the
whole collidable world is one Havok shape — so unlike every other asset in the IW5→IW7
path, this cannot be done as a struct copy. It requires *generating* Havok data.

Sources used:

- zonetool source (this repo)
- `D:\Files\IDB\iw7\iw7_ship_dump.exe.i64` (IDA 9.1). This is a **memory dump**, which
  matters: Havok's reflection tables are runtime-initialised and are therefore present
  and readable in the `.data` segment. See section 5.
- `D:\Games\PC\IW7\dump\**` — `.hkx` files dumped by this repo's IW7 dumper.
  **`mp_paris`, `mp_afghan` and `mp_breakneck` are stock** and are the ground truth for
  world collision. `mp_shipment` and `mp_test_h1` in the same tree are *custom ports*;
  `mp_shipment`'s Havok collision is known to be wrong and is used below only as a
  negative reference, never as validation. `D:\Games\PC\IW7\zone` has ~132 stock map
  zones if more samples are wanted.
- `D:\Files\LEAKS\havok2014\hk2014_1_0_r1` — a genuine Havok SDK, build #20140907.
  Used **only** for the packfile container structs (section 3). See the warning in
  section 2 about why it is useless for the shape itself.
- `D:\Files\IDB\iw8\1-game_test.exe.i64` / `.c` -- for the IW7-vs-IW8 comparison, written
  up separately in [iw7-vs-iw8-havok.md](iw7-vs-iw8-havok.md). **The `.c` is a 516 MB full
  Hex-Rays export**: grepping it is far cheaper than opening the database and does not
  disturb an IDA session already running. Note the two builds do *not* share a section
  layout -- IW8 reads `m_page` at section+91 and treats +76/+80/+84 as plain `uint32`,
  where IW7 packs those and puts `page` at +92 (verified: reading page from +92 puts 97.5%
  of vertices inside their section domain, from +91 only 12.9%). Take algorithms from IW8,
  never offsets.

---

## 1. Three corrections to the obvious assumptions

These are worth stating up front because they invert the natural plan.

**IW7 does not use `hkp*`.** It uses `hknp*` — the "Havok Physics next-gen" rewrite. The
give-away is in IW7's own assert strings, which carry the original source paths:

```
F:\trees\iw7\code_source\external\Havok\hk2014_2_5_r1\Source\Physics/Physics/Collide/Shape/hknpShapeUtil.inl
F:\trees\iw7\code_source\external\Havok\hk2014_2_5_r1\Source\Physics/Physics/Collide/Shape/Composite/hknpSparseCompactMap.inl
```

`Source/Physics/` is the hknp tree; `Source/Physics2012/` is the old `hkp` one. IW7's
binary does contain `hkp*` class names, but those come from the linked-in serialization
compat tables, not from anything the game constructs. So `hkpBvCompressedMeshShape`,
`hkpStaticCompoundShape` and `hkpMoppBvTreeShape` are all the wrong target.

**The root object is not a Havok class.** It is `HavokPhysicsShapeList`, an Infinity Ward
class, signature `0xC909A395`. Havok shapes hang off it. Per-model physics uses a second
IW class, `HavokPhysicsAsset` (`0x0DFB2195`).

**The local Havok SDK cannot build this.** `hk2014_1_0_r1` has no hknp collide/shape code
at all — `Source/Physics/` contains only `Constraint` and `ConstraintSolver`. The hknp
shape module first shipped in 2014.2. Its prebuilt libs are also `win32_vs2012_win7_noSimd`
only, i.e. 32-bit, while IW7 packfiles are `pointerSize == 8`. The SDK is good for the
container structs and nothing else.

---

## 2. What IW7's clipmap actually holds

`clipMap_t` in `src/IW7/Structs.hpp:3827` (0x180 bytes). Compared to IW5 the collision
content is essentially gone:

| IW5 `clipMap_t` | IW7 |
|---|---|
| `ClipInfo` with brushes, brushsides, brushEdges, leafbrushNodes, leafbrushes, materials | `ClipInfo` is **`{ int planeCount; cplane_s* planes; }`** — 16 bytes, planes only |
| `nodes`, `leafs` (BSP) | gone |
| `verts`, `triIndices`, `triEdgeIsWalkable`, `borders`, `partitions`, `aabbTrees` (trisoup) | gone |
| `cmodels` (brush submodels) | gone |
| — | `havokWorldShapeData` / `havokWorldShapeDataSize` |

So the collidable world is `havokWorldShapeData`, a raw Havok packfile blob. `MapEnts`
carries a second one, `havokEntsShapeData` (`src/IW7/Structs.hpp:4730`), and each
`PhysicsAsset` carries a third. The existing dumper already writes all three out as
`.hkx` — that is where the 516 sample files come from.

The planes array survives because IW7 still uses it for non-Havok queries; it is not
what the physics runtime collides against.

---

## 3. Packfile container (verified byte-exact)

The blob is a classic Havok **binary packfile**, not a tagfile. Layout is
`hkPackfileHeader` + N × `hkPackfileSectionHeader`, both taken verbatim from the SDK
headers and confirmed unchanged in 2014.2.5-r1.

`hkPackfileHeader`, 64 bytes:

| off | size | field | IW7 value |
|---|---|---|---|
| 0 | 8 | `m_magic[2]` | `57E0E057 10C0C010` |
| 8 | 4 | `m_userTag` | 0 |
| 12 | 4 | `m_fileVersion` | 11 |
| 16 | 4 | `m_layoutRules[4]` | `08 01 00 01` |
| 20 | 4 | `m_numSections` | 3 |
| 24 | 4 | `m_contentsSectionIndex` | 2 |
| 28 | 4 | `m_contentsSectionOffset` | 0 |
| 32 | 4 | `m_contentsClassNameSectionIndex` | 0 |
| 36 | 4 | `m_contentsClassNameSectionOffset` | 75 |
| 40 | 16 | `m_contentsVersion` | `"hk_2014.2.5-r1"` |
| 56 | 4 | `m_flags` | 0 |
| 60 | 2 | `m_maxpredicate` | 21 |
| 62 | 2 | `m_predicateArraySizePlusPadding` | 0 |

`m_layoutRules` is `{ pointerSize=8, littleEndian=1, reuseBaseClassPadding=0,
emptyBaseClassOptimization=1 }`. This is what `havok.cpp:validate_header` already checks,
and it matches all 516 files.

**Padding is `0xFF`, not zero.** `hkPackfileHeader()` does
`hkString::memSet(this, -1, sizeof(*this))` before writing any field, so every byte not
explicitly assigned stays `0xFF`. In particular `m_contentsVersion` is
`"hk_2014.2.5-r1\0"` followed by one `0xFF` byte at offset 55. Getting this wrong is the
single most likely way to produce a file that looks right in a hex editor and still fails
to load — it was the only round-trip failure in the first version of the writer here.

`hkPackfileSectionHeader`, **64 bytes** (`char m_sectionTag[19]`, `char m_nullByte`,
seven `hkInt32`, then `hkInt32 m_pad[4]`):

| off | field |
|---|---|
| 0 | `m_sectionTag[19]` — `__classnames__`, `__types__`, `__data__` |
| 19 | `m_nullByte` — `0xFF` in shipped files |
| 20 | `m_absoluteDataStart` |
| 24 | `m_localFixupsOffset` |
| 28 | `m_globalFixupsOffset` |
| 32 | `m_virtualFixupsOffset` |
| 36 | `m_exportsOffset` |
| 40 | `m_importsOffset` |
| 44 | `m_endOffset` |
| 48 | `m_pad[4]` — `0xFF` filled |

All offsets after `m_absoluteDataStart` are relative to it, and each doubles as the end of
the previous region: data size is `m_localFixupsOffset`, local-fixup size is
`m_globalFixupsOffset - m_localFixupsOffset`, and so on.

Section layout is always the same three sections, in order. `__types__` is always empty
(zero length) — IW7 relies on the runtime class registry rather than embedding type info.

`__classnames__` is a packed table, `0xFF`-terminated, of records shaped

```
{ uint32 signature; uint8 0x09; char name[]; '\0' }
```

The `0x09` between the signature and the string is easy to miss and will silently prefix a
tab onto every name if you skip it. Offsets below are to the first character of the name,
which is what `m_contentsClassNameSectionOffset` stores. Every file opens with the same
four Havok bookkeeping entries and then lists only the classes it actually uses:

```
+5     33D42383  hkClass
+18    B0EFA719  hkClassMember
+37    8A3609CF  hkClassEnum
+54    CE6F8A6C  hkClassEnumItem
+75    C909A395  HavokPhysicsShapeList     <- contentsClassNameSectionOffset == 75
+102   1318CC9F  hknpCompressedMeshShape
+131   54FD8D57  hknpCompressedMeshShapeData
```

The root class lands at exactly +75 in every world and ents file because the four preamble
entries are fixed-length.

Three fixup tables live at the end of `__data__`:

- **local**, 8 bytes each `{ int32 srcOffset, int32 dstOffset }` — intra-section pointer
  patches. This is the bulk: `mp_paris` world collision has 1,908.
- **global**, 12 bytes each `{ int32 srcOffset, int32 dstSectionIndex, int32 dstOffset }`
  — cross-section pointers.
- **virtual**, 12 bytes each `{ int32 objectOffset, int32 classNameSectionIndex,
  int32 classNameOffset }` — this is the object table, and the only thing that says what
  class each object is. Entries are terminated by a `-1` row.

`tools/iw5cmconv/hkpackfile.py` implements all of the above. Correctness check:

```bash
python tools/iw5cmconv/hkxinfo.py --roundtrip "D:/Games/PC/IW7/dump/**/*.hkx"
```

```
byte-exact round-trip: 516 / 516
fileVersion      {11: 516}
contentsVersion  {'hk_2014.2.5-r1': 516}
layoutRules      {(8, 1, 0, 1): 516}
section layout   {'__classnames__/__types__/__data__': 516}
root classes:
     513 x HavokPhysicsAsset
       3 x HavokPhysicsShapeList
```

Every shipped file re-serializes to identical bytes from the parsed representation.

---

## 4. What is inside each blob

Three distinct shapes of content, all in the same container.

### World collision — `clipMap_t::havokWorldShapeData`

`mp_paris.d3dbsp.colmap.hkx`, 13,066,464 bytes. Object table:

```
sec2 +0        HavokPhysicsShapeList
sec2 +7904     hknpCompressedMeshShape
sec2 +98672    hknpCompressedMeshShapeData
```

Two global fixups, and that is all: `+160 -> +7904` (the shape pointer) and
`+8000 -> +98672` (the shape's `data` pointer). **The entire collidable world is a single
`hknpCompressedMeshShape`.** There is no compound, no per-brush convex, no MOPP.

### Map-ents collision — `MapEnts::havokEntsShapeData`

`mp_paris.d3dbsp.ents.data.hkx`, 322,240 bytes:

```
    1 x HavokPhysicsShapeList
    1 x hknpCompressedMeshShape          + 1 x hknpCompressedMeshShapeData
  130 x hknpDynamicCompoundShape         + 130 x hknpDynamicCompoundShapeData
  291 x hknpConvexPolytopeShape          + 291 x hknpConvexPolytopeShapeConnectivity
```

So movers/brushmodels become convex polytopes grouped into dynamic compounds, and there is
*also* one compressed mesh here.

### Per-model physics — `PhysicsAsset`

513 files, typically ~2 KB:

```
HavokPhysicsAsset -> hknpPhysicsSystemData -> <shape>
                                            + hknpShapeMassProperties
                                            + hkRefCountedProperties
```

The shape is not always a convex hull. Across the 513 shipped assets:

| assets | shapes present |
|---|---|
| 276 | `hknpConvexPolytopeShape` alone |
| 158 | `hknpConvexPolytopeShape` + `hknpDynamicCompoundShape` |
| 53 | `hknpCompressedMeshShape` alone |
| 9 | `hknpConvexPolytopeShape` + `hknpStaticCompoundShape` |
| 7 | `hknpCompressedMeshShape` + `hknpDynamicCompoundShape` |
| 3 | `hknpSphereShape` |
| 1 | `hknpCapsuleShape` |
| 6 | other mixes |

63 of the 513 per-model assets contain a compressed mesh. That is useful: a small
per-model `.hkx` is a far easier first target for the mesh builder than a 13 MB world
blob, and there are 63 of them to diff against.

### `HavokPhysicsShapeList` (IW custom)

Not in any SDK. Its shape is legible from the shipped data: the object opens with nine
`hkArray` members at fixed offsets, all but one sized by entity count (131 in the ents
file, 1 in the world file):

| off | size in ents file | size in world file |
|---|---|---|
| 0 | 131 | 1 |
| 16 | 131 | 1 |
| 32 | 131 | 1 |
| 48 | 131 | 1 |
| 64 | 131 | 1 |
| 80 | 262 (= 2×N) | 2 |
| 104 | 319 | 316 |
| 120 | 131 | 1 |
| 136 | 131 | 1 |

`hkArray<T>` is `{ T* m_data; int m_size; int m_capacityAndFlags; }`, 16 bytes, with
`0x80000000` (don't-deallocate) always set in `m_capacityAndFlags` in serialized data.
The `+104` array is ~316 in both files and is a strong candidate for the material /
collision-filter table — that is where the IW5 surfaceflag mapping will have to land. The
world file also carries the string `"World Entity 0"` at `+200`.

The exact member names are not recoverable — `HavokPhysicsShapeList` is an IW class and
its reflection is registered by `sub_140005C30`, whose adjacent `.rdata` strings
(`convexCounts`, `bodyDrivers`, `constraintServerUsage`, `numConvexShapes`) name the
members but not their order. Reversing that function is the way to finish this table.

---

## 5. The shape format is fully specified

The important result: **IW7's binary carries complete Havok reflection**, so no layout has
to be guessed or borrowed from another SDK version.

Havok's `hkClass` / `hkClassMember` tables are runtime-initialised into `.data`. Because
`iw7_ship_dump.exe` is a memory dump, they are captured. They live around
`0x14B05_5000`–`0x14B06_7000`. `hkClass` is 80 bytes and `hkClassMember` is 40 bytes at
x64 (both confirmed against the SDK headers), and member `offset` is the full offset
including base-class subobjects.

187 collision-relevant classes are dumped to
[iw7-havok-reflection.txt](iw7-havok-reflection.txt). The chain that matters:

```
hknpCompressedMeshShape                                    objectSize=160  version=5
  : hknpCompositeShape                                     objectSize=96
      : hknpShape                                          objectSize=48   version=3
          : hkReferencedObject                             objectSize=16
              : hkBaseObject                               objectSize=8
    +16  flags                    hkFlags<..,uint16>
    +18  numShapeKeyBits          uint8
    +19  dispatchType             enum uint8
    +20  convexRadius             float
    +24  userData                 uint64
    +32  properties               hkRefCountedProperties*
    +48  edgeWeldingMap           hknpSparseCompactMap<uint16>
    +88  shapeTagCodecInfo        uint32
    +96  data                     hknpCompressedMeshShapeData*
    +104 quadIsFlat               hkBitField
    +128 triangleIsInterior       hkBitField
    +152 numTriangles             int32
    +156 numConvexShapes          int32

hknpCompressedMeshShapeData                                objectSize=256  version=1
  : hkReferencedObject
    +16  meshTree                 hknpCompressedMeshShapeTree   (160 bytes)
    +176 simdTree                 hkcdSimdTree                  (24 bytes)
    +200 connectivity             hkcdStaticMeshTreeBaseConnectivity (48 bytes)

hknpCompressedMeshShapeTree = hkcdStaticMeshTree<CommonConfig,uint32,uint64,1,1,2,1,DataRun>
  : hkcdStaticMeshTreeBase                                 objectSize=112
      : hkcdStaticTreeTree<hkcdStaticTreeDynamicStorage5>  objectSize=48
        +0   nodes                hkArray<hkcdStaticTreeCodec3Axis5>
        +16  domain               hkAabb
        +48  numPrimitiveKeys     int32
        +52  bitsPerKey           int32
        +56  maxKeyValue          uint32
        +64  sections             hkArray<hkcdStaticMeshTreeBaseSection>
        +80  primitives           hkArray<hkcdStaticMeshTreeBasePrimitive>
        +96  sharedVerticesIndex  hkArray<uint16>
    +112 packedVertices           hkArray<uint32>
    +128 sharedVertices           hkArray<uint64>
    +144 primitiveDataRuns        hkArray<hknpCompressedMeshShapeTreeDataRun>
```

The tree is two-level, with 8-bit quantised AABBs at both levels:

```
hkcdStaticTreeCodec3Axis     3 bytes   uint8 xyz[3]
hkcdStaticTreeCodec3Axis4    4 bytes   : Codec3Axis + uint8 data          (per-section nodes)
hkcdStaticTreeCodec3Axis5    5 bytes   : Codec3Axis + uint8 hiData, loData (top-level nodes)

hkcdStaticMeshTreeBaseSection                              objectSize=96  version=3
  : hkcdStaticTreeTree<hkcdStaticTreeDynamicStorage4>      objectSize=48
        +0   nodes                hkArray<hkcdStaticTreeCodec3Axis4>
        +16  domain               hkAabb
    +48  codecParms               float[6]
    +72  firstPackedVertex        uint32
    +76  sharedVertices           uint32   (packed offset/count)
    +80  primitives               uint32   (packed offset/count)
    +84  dataRuns                 uint32   (packed offset/count)
    +88  numPackedVertices        uint8
    +89  numSharedIndices         uint8
    +90  leafIndex                uint16
    +92  page                     uint8
    +93  flags                    uint8
    +94  layerData                uint8
    +95  unusedData               uint8

hkcdStaticMeshTreeBasePrimitive    4 bytes   uint8 indices[4]   (triangle or quad)
hknpCompressedMeshShapeTreeDataRun 4 bytes   : PrimitiveDataRunBase<uint16>
                                             { uint16 value; uint8 index; uint8 count; }
```

`codecParms[6]` is the per-section vertex quantisation (offset xyz + scale xyz);
`packedVertices` holds vertices bit-packed relative to it. `primitiveDataRuns` is the
run-length-encoded per-triangle payload — a `uint16 value` applied to `count` primitives
starting at `index` — which is where the **surface material / shape tag** lives, and
therefore where the IW5 surfaceflags have to be encoded.

Confirmed against the real file: `mp_paris` world shape at `+7904` has
`numTriangles`-adjacent `hkArray` sizes of 7,549 and 15,098 (exactly 2×), consistent with
a quad/triangle pairing, and `shapeTagCodecInfo` at `+88` reads `0xFFFFFFFF` -- as it does in all 699 composite
shapes across every shipped blob. (An earlier note here recorded `0x02130004`; that was a
misread.)

---

## 5a. The compressed mesh encoding (decoded and verified)

Layout alone is not enough to build one of these; the *encoding* had to be recovered from
shipped bytes. `tools/iw5cmconv/hkcompressedmesh.py` implements the decoder.
The 513 per-model assets hold 83 `hknpCompressedMeshShape` objects across 63 files.
**43 of those 83 decode and validate exactly**; the other 40 are blocked on one open item
(shared vertices, below).

### Unit scale: 1/32 everywhere

**These are different, and getting it backwards scales collision by 32x.**

**Per-model `PhysicsAsset` blobs are CoD units / 32.** Modular assets whose names encode
their length prove it, taking the longest axis of the section domain minus
`2 * convexRadius`:

| model | name says | extent x 32 |
|---|---|---|
| `industrial_conduit_metal_1_straight_8` | 8 | 8.000 |
| `industrial_conduit_metal_1_straight_16` | 16 | 16.000 |
| `industrial_conduit_metal_1_straight_64` | 64 | 63.999 |
| `industrial_conduit_metal_1_straight_128` | 128 | 127.998 |
| `industrial_conduit_metal_1_straight_256` | 256 | 255.997 |
| `sdf_interior_ladder_01_48` | 48 | 48.000 |
| `p7_rug_cloth_modern_128x256_03` | 128 x 256 | 129.9 x 253.5 |

Six models across five lengths land on 1/32 to within 0.003. The inch-to-metre factor
(0.0254) fits none of them. Model space is scaled up by the instance transform
(`cStaticModel_s::invScaledAxis`).

**CORRECTED 2026-09-09: the world blob is CoD units / 32, like everything else.**
Measured directly on `mp_frontend.d3dbsp.colmap.hkx`, whose mesh-tree domain is
`(-98, -226, -18)..(114, 546, 66)`, extent **212 x 772 x 84**. mp_frontend spans
6720 x 23040 CoD units, so /32 predicts 210 x 772 - x and y agree to within 1%. At 1:1
the domain would read in the thousands. quaK also confirmed in game that forcing 1:1
makes collision visibly worse, and that converted maps have always needed 1/32.

That also makes the three scales consistent, which is what one would expect of a single
Havok space: world mesh, ents shapes and per-model physics assets are all CoD units / 32.

Two traps this claim fell into, worth avoiding next time:

- The evidence below is an *order-of-magnitude* comparison, and its own ratio column
  ranges 0.9 to 4.1. A method with that much spread cannot separate 1 from 32; it was
  only ever evidence against some third, much larger factor.
- A ray test against the geometry *before* scaling lines up perfectly with CoD-unit
  world positions. That shows the source geometry is correct, not that the target space
  is 1:1 - the scale is applied after.

The superseded reasoning follows.

~~The world blob is CoD units, 1:1.~~ Checked against `cStaticModel_s::origin` values
parsed straight out of `mp_paris.d3dbsp.colmap`, which are unambiguously CoD units:

```
static-model origins (4,646 samples)   world-blob mesh-tree domain
  x  p1 -3064   p99  2556               x  -3987 .. 3903
  y  p1 -1718   p99  3378               y  -3304 .. 3936
  z  p1  -231   p99   797               z   -580 .. 1260
```

They coincide. Cross-checked on all three stock maps by comparing the extent of the
map-entity origin cloud (from the `.ents` dump, key hash `543`, definitively CoD units)
against the collision-domain extent:

| map | entity-cloud extent (p5..p95) | collision-domain extent | ratio |
|---|---|---|---|
| `mp_paris` | 3922 x 3191 x 1028 | 7890 x 7240 x 1840 | 2.0 / 2.3 / 1.8 |
| `mp_afghan` | 4992 x 4368 x 1024 | 4320 x 5130 x 2670 | 0.9 / 1.2 / 2.6 |
| `mp_breakneck` | 5184 x 2720 x 667 | 6540 x 4946 x 2750 | 1.3 / 1.8 / 4.1 |

Ratios of order 1 (the domain is legitimately larger, since it covers geometry beyond the
entity cloud). Under a 1/32 world scale these would all be ~0.03.

So an IW5 -> IW7 world collision converter emits vertices **unchanged**, in CoD units.

### Domain vs geometry

`domain` is the true geometry AABB expanded by `convexRadius` on every axis:

```
domain.min = geometryMin - convexRadius
domain.max = geometryMax + convexRadius
```

Shipped `convexRadius` is either `0.01` or `0.0`. `codecParms[0..2]` is exactly
`geometryMin`, which is how the relationship was spotted.

### Packed vertices — 11/11/10

`packedVertices` is `hkArray<uint32>`, one vertex per entry, in the owning section's
codec space:

```
rawX = packed        & 0x7FF     (11 bits)
rawY = (packed >> 11) & 0x7FF    (11 bits)
rawZ = (packed >> 22) & 0x3FF    (10 bits)

pos.x = codecParms[0] + rawX * codecParms[3]
pos.y = codecParms[1] + rawY * codecParms[4]
pos.z = codecParms[2] + rawZ * codecParms[5]
```

with `codecParms[3+i] = extent[i] / (2^bits[i] - 1)`, i.e. 2047, 2047, 1023. Verified by
decoding and checking every vertex lands inside `domain` and that the decoded AABB
reproduces `domain` shrunk by `convexRadius`, to well under one quantisation step.

A section with no packed vertices writes the sentinel `codecParms =
[FLT_MAX, FLT_MAX, FLT_MAX, -inf, -inf, -inf]` and sets `flags = 1`.

### Section table packing

The three `uint32` fields are `(offset << 8) | count`:

```
primitives_raw      = (firstPrimitive  << 8) | primitiveCount
data_runs_raw       = (firstDataRun    << 8) | dataRunCount
shared_vertices_raw = (firstSharedIdx  << 8) | numPackedVertices
```

Note the third is not symmetric: its low byte repeats `numPackedVertices`, and the actual
shared count is the separate `num_shared_indices` field. Confirmed on a 5-section mesh
where every cumulative sum closes exactly: primitives 423+61 = 484 = total,
sharedVerticesIndex 82+20 = 102 = total, and `first_packed_vertex` 383+46 = 429 = total.

### Primitives

`hkcdStaticMeshTreeBasePrimitive` is four `uint8` indices into the section's local vertex
pool (packed vertices first, then that section's shared vertices).
`indices[2] == indices[3]` marks a triangle; otherwise it is a quad, split
`(a,b,c)` + `(a,c,d)`. `numPrimitiveKeys` counts triangles, not primitives, and
`hknpCompressedMeshShape::quadIsFlat` / `triangleIsInterior` have exactly one bit per
primitive and per triangle respectively -- both are useful cross-checks.

`numTriangles` and `numConvexShapes` are **not** stored: reflection marks them
`0x400` (SERIALIZE_IGNORED) and they read 0 in every shipped file. They are recomputed at
load, so a builder must leave them zero.

### Shared vertices (solved)

`sharedVertices` is `hkArray<uint64>`, quantised over the **tree** domain (not the section
codec) at **21/21/22 bits**. The bit allocation comes from IW8, where
`hkcdStaticMeshTree::VertexCodecBase<hkUint64>::setupParameters` uses the constant
`__xmm@3f800000348000023500000435000004`, i.e. `invBitScales = { 2^-21, 2^-21, 2^-22, 1.0 }`:

```
rawX = packed        & 0x1FFFFF     (21 bits)
rawY = (packed >> 21) & 0x1FFFFF    (21 bits)
rawZ = (packed >> 42) & 0x3FFFFF    (22 bits)
pos[i] = treeDomain.min[i] + raw[i] * (treeDomain.extent[i] * invBitScales[i])
```

The lookup is stated directly by IW8:

```c
m_sharedVertices      = &mesh->m_sharedVertices.m_data[0x10000 * section->m_page];
m_sharedVerticesIndex = &mesh->m_sharedVerticesIndex.m_data[section->m_firstSharedVertexIndex];
m_sharedVerticesIndex -= section->m_numPackedVertices;
```

so for a primitive vertex index `v`:

```
v <  numPackedVertices -> packedVertices[firstPackedVertex + v], section codec, 11/11/10
v >= numPackedVertices -> sharedVertices[0x10000*page
                            + sharedVerticesIndex[firstSharedVertexIndex + v - numPackedVertices]]
                          tree codec, 21/21/22
```

Two traps here. **`page` matters** -- `mp_paris` has 316,789 shared vertices across 5 pages,
and ignoring `page` silently reads the wrong vertex. And **`numSharedIndices` is not the
section's vertex-pool size**: sections legitimately use indices past it (section 4 of
`mp_paris` has `numSharedIndices == 105` and references index 174). Bounding the pool by it
drops primitives.

**This rule is right; the ~7% of "vertices outside their section domain" were never
vertices.** A primitive with `indices[1] == indices[2] == indices[3]` is a Havok *custom
primitive*, and its `indices[0]` indexes `sharedVerticesIndex` to fetch a shape-type
descriptor rather than a vertex. Skipping those takes resolution to 100.00% on all three
stock world blobs. See P2a in [iw7-havok-backlog.md](iw7-havok-backlog.md).

Two claims that were made here while chasing that are now retracted. The section domain is
**not** a tight bound in general -- measured over all sections rather than the subset that
happened to decode cleanly, it equals the vertex bbox in only 263 of 1,388 sections in
`mp_afghan`, and is loose by more than a unit in 470. The "588 of 592 clean sections" figure
was circular: "clean" was defined by the very containment test it was being used to justify.
The domain is a *bound*, reliable as such (no section escapes the tree domain, and none
misses its own vertices by more than 0.0022 units), but it is not tight enough to identify a
vertex.

### Still open: the single-section `numPackedVertices == 0` variant

38 of the 83 per-model mesh objects are one section with `numPackedVertices == 0` whose
`sharedVerticesIndex` holds a constant stale-looking pattern -- the repeating `uint16`
triple `(0x0852, 4k, 0x3C24)`, identical across every affected file -- rather than usable
indices. Their primitives also read as degenerate under the normal rule (e.g. `[3,2,2,2]`),
and grouping shared vertices four-per-primitive does not produce planar quads either. The
decoder raises `NotImplementedError` for these rather than emit wrong geometry.

**World blobs are unaffected** -- `mp_paris` has 1,845 of 1,888 sections at
`numPackedVertices == 0` and they decode correctly, because their `sharedVerticesIndex`
holds real data. This variant only blocks some per-model physics assets, which are not on
the IW5 -> IW7 world-collision path.

### Decoder results

`tools/iw5cmconv/hkcompressedmesh.py`, over every shipped `.hkx`:

```
PASS 46   FAIL 2   unsupported-variant 38   (256,524 triangles decoded)
```

All three stock world blobs decode:

| blob | sections | primitives | quads | triangles | numPrimitiveKeys | verts outside domain |
|---|---|---|---|---|---|---|
| `mp_afghan` | 1,400 | 105,752 | 86.0% | 196,676 | 196,676 (**exact**) | 0 |
| `mp_paris` | 1,888 | 140,255 | 80.3% | 252,814 | 252,813 (+1) | 0 |
| `mp_breakneck` | 2,029 | 148,272 | 86.9% | 277,170 | 277,160 (+10) | 0 |

`mp_afghan` reproduces exactly. The residual +1 / +10 is explained below.

The three agree on every structural parameter — `bitsPerKey = 19`, `convexRadius = 0`,
`shapeTagCodecInfo = 0xFFFFFFFF`, `flags = 0x4`, one `hknpCompressedMeshShape` under one
`HavokPhysicsShapeList`, and the same nine-`hkArray` root layout — and all three are
overwhelmingly shared-vertex: 63 / 16 / 185 packed vertices against 316,789 / 176,653 /
306,394 shared, spread over 5 / 3 / 5 pages.

### `0xDEADDEAD` padding in the primitive array

Some primitive slots are filled with the four bytes `DE AD DE AD` (indices
`[222, 173, 222, 173]`). `mp_afghan` has none, `mp_paris` 8, `mp_breakneck` 19 — matching
the order of the residual triangle-count deltas (0, +1, +10). They fall inside sections'
primitive ranges rather than trailing the array. A builder should simply never emit them;
a decoder that counts them as real quads over-counts triangles slightly, which is the
entire source of the discrepancy above. The two "FAIL" rows are
quad-vs-triangle classification edge cases -- `mp_paris` +1 triangle out of 252,813 and
`dns_sandbag01_singlebent_iw6` -4 out of 750. The rule used is
`indices[2] == indices[3] -> triangle`; a handful of primitives evidently need a different
discriminator (`quadIsFlat` is the likely source).

### mp_shipment as a negative reference

`mp_shipment` in the dump tree is **not stock** -- it is a custom port whose Havok collision
is known bad. It is worth keeping precisely for that: it shows what a naive generator
produces, and it still *loads*, which bounds what the runtime will accept.

| | `mp_paris` (stock) | `mp_shipment` (custom, bad) |
|---|---|---|
| sections | 1,888 -- 1,845 with **no** packed vertices | 480 -- **all** packed |
| primitives | 80.3% quads | **0% quads**, all triangles |
| packed / shared vertices | 63 / 316,789 (5 pages) | 24,072 / **0** |
| `primitiveDataRuns` | 57,162 runs, **316 distinct values** | 480 runs, **1 distinct value (0)** |
| convexRadius | 0.0 | 0.25 |

Three things follow:

1. **`primitiveDataRuns.value` is the per-surface tag.** Stock has 316 distinct values
   across the map; the custom map has a single 0 for every triangle, i.e. no surface data
   at all. That alone would give wrong footstep/impact/penetration behaviour.
2. **A packed-only, triangle-only mesh is structurally accepted by IW7.** The custom map
   uses no shared vertices, no quads, and one section codec per section, and still loads.
   That is the simplest viable target for a first builder -- quad merging and shared-vertex
   promotion are optimisations, not requirements.
3. Stock leans the other way (63 packed vertices in the whole map vs 316,789 shared), so
   Havok's own builder promotes almost everything to shared vertices. Do not treat the
   stock ratio as a constraint.

Because this file is tool-generated, **none of its internals are evidence for the format**
-- including its BVH nodes, which may well be part of why it is wrong.

---

## 6. IW5 source data

`src/IW5/Structs.hpp:4660`. Two independent collision representations, both of which have
to end up in the one compressed mesh:

- **Brushes** — `ClipInfo.brushes` (`cbrush_t`), each with `numsides` non-axial
  `cbrushside_t` (plane + `materialNum`) plus six axial sides implied by `brushBounds`
  and `axialMaterialNum[2][3]`. These are half-space intersections and must be converted
  to explicit convex hulls, then triangulated.
- **Trisoup** — `verts` / `triIndices` / `partitions` / `borders` / `aabbTrees`, with
  `CollisionAabbTree.materialIndex` giving the per-leaf material. Already triangles.

Materials are `ClipMaterial { const char* name; int surfaceFlags; int contents; }`.
`src/IW5/Converter/H1/Assets/ClipMap.cpp:105` already has a `convert_surf_flags` for the
IW5→H1 direction, which is the right starting point for an IW5→IW7 table.

Axes agree — both are Z-up, right-handed — and world collision is stored at **1/32** CoD
units (section 5a). Winding does need care: CoD brush planes
point *outward*, and Havok triangle normals follow counter-clockwise winding, so hull
triangulation must emit CCW as seen from outside or every surface normal inverts.

---

## 7. What is left

Container done, layouts done, mesh *encoding* done except one variant. What remains is
building rather than reading:

1. **Finish the top-level BVH offsets** (section 5a). The AABB codec, both leaf rules and
   the whole per-section tree are solved and validated; only the 5-byte internal-node
   child offset is open. It is the last thing between here and being able to write a mesh.
2. **Write the builder** — the inverse of `hkcompressedmesh.py`: split triangles into
   sections (≤255 packed vertices each), quantise into `codecParms` + `packedVertices`,
   promote cross-section vertices into paged `sharedVertices`, build both BVH levels and
   the `simdTree`, and RLE per-triangle tags into `primitiveDataRuns`. Verify with
   `encode(decode(f)) == f` against the 46 meshes that already decode.
4. **Serialise.** Emit objects into `__data__` and generate the three fixup tables;
   `hkpackfile.py` already does the container half.
5. **IW5 extraction + material map.** Brushes → hulls → CCW triangles, trisoup passthrough,
   and an IW5 `surfaceFlags`/`contents` → IW7 shape-tag table.
6. **Integrate and test in-game** via the existing zonetool IW7 path.

Trigger volumes are *not* part of this: IW7 keeps them in `MapEnts::trigger` as slab
hulls, the same representation IW5 uses, and no stock world blob contains one. See
[iw7-triggers.md](iw7-triggers.md).

The *other* Havok blob, `MapEnts::havokEntsShapeData`, is a separate and much easier job --
convex polytopes in dynamic compounds, no quantisation and no static BVH, so none of the
open items above block it. It is decoded in [iw7-ents-shapes.md](iw7-ents-shapes.md).


### `HavokPhysicsShapeList` (solved)

`sub_140005C30` is the reflection registration call. Decompiled it is simply:

```c
sub_140F11690(&unk_144BFE8E0, "HavokPhysicsShapeList", 0, 152, 0,0,0,0,
              &unk_1414A2EF0, 10, 0,0,0, 1);
```

i.e. objectSize **152**, **10** members, member table at `0x1414A2EF0`. Parsing that table
the same way as any other `hkClassMember` array gives:

| off | member | type |
|---|---|---|
| 0 | `shapes` | `hkArray<hknpShape*>` |
| 16 | `shapeIndices` | `hkArray<int32>` |
| 32 | `shapeNames` | `hkArray<hkStringPtr>` |
| 48 | `vertCounts` | `hkArray<int32>` |
| 64 | `triCounts` | `hkArray<int32>` |
| 80 | `minMaxes` | `hkArray<hkVector4f>` — two per shape, min then max |
| 96 | `numWorldGeoShapes` | `int32` (scalar, not an array) |
| 104 | `shapeTagData` | `hkArray<ShapeTagData>` |
| 120 | `shapeContents` | `hkArray<int32>` |
| 136 | `convexCounts` | `hkArray<int32>` |

`HavokPhysicsShapeList::ShapeTagData`, 24 bytes:

| off | member | type |
|---|---|---|
| 0 | `collisionFilterInfo` | `uint32` |
| 4 | `materialCRC` | `int32` |
| 8 | `materialId` | `uint16` |
| 16 | `userData` | `uint64` |

This matches the shipped bytes exactly -- in `mp_paris` the arrays are sized
1,1,1,1,1,2,·,316,1,1 with `shapeNames[0]` pointing at the string `"World Entity 0"`,
`minMaxes` holding the world AABB, and `vertCounts`/`triCounts` holding 360,433 / 234,611.

**`shapeTagData` is the material table, and `primitiveDataRuns.value` indexes it.** Its
length is 316 / 132 / 155 in paris / afghan / breakneck -- exactly the distinct
`primitiveDataRuns` value counts measured independently. That closes the surface-material
path end to end: IW5 `surfaceFlags`/`contents` -> a `ShapeTagData` palette entry -> an RLE
run over primitives.

For reference, `HavokPhysicsAsset` (objectSize 144, 10 members) is the per-model analogue:
`isRagdoll`, `physicsSystemData`, `bodyQualityNameCRCLookup`, `materialNameCRCLookup`,
`motionPropertiesNameCRCLookup`, `bodySFXAssetNames`, `bodyVFXAssetNames`,
`bodyServerUsage`, `constraintServerUsage`, `bodyDrivers`.

### The simdTree (solved -- and mandatory)

`hknpCompressedMeshShapeData::simdTree` is a *second* acceleration structure, and it is not
optional: **0 of the 257 shipped compressed meshes have an empty one**, including the
custom `mp_shipment`. IW7's query path goes through it -- IW8 names the raycast
`hkcdSimdTreeUtils::ProcessSimdTreeRayCastLeaves<hknpCompressedMeshShapeInternals::RayCastQuery<1>,0>`
-- so a mesh with an empty simdTree loads cleanly and then collides with nothing. Symptom:
you fall through the map.

It is a 4-wide BVH over *primitives* (~0.5 nodes per primitive in stock maps).
`hkcdSimdTreeNode` is `hkcdFourAabb` followed by `uint32 data[4]`, 112 bytes:

```
hkcdFourAabb : six hkVector4f in SoA order -- lx, hx, ly, hy, lz, hz
               (component-major: four children per vector)
data[slot]   : (data & 1) -> leaf,     primitiveKey   = data >> 1
               else       -> internal, childNodeIndex = data >> 1
```

An unused slot has an inverted AABB (min `+FLT_MAX`, max `-FLT_MAX`) and `data == 0`.
**Node 0 is an all-inverted sentinel and node 1 is the real root**, which is why a stock
traversal reaches exactly `N - 1` nodes.

Verified by traversal on the stock maps: `mp_afghan` visits 52,780 of 52,781 nodes and
yields 105,752 leaves for 105,752 primitives, every key decoding to a valid
(section, primitive). `mp_paris` yields 140,247 for 140,255 -- short by exactly its 8
`0xDEADDEAD` padding primitives.

### Primitive keys

```
key = (sectionIndex << 8) | (localPrimitiveIndex << 1) | triangleInQuad
```

The shift is **8 in every shipped map**, which caps a section at 127 primitives. Do not
widen it even though `bitsPerKey` and `maxKeyValue` are stored: the decoder appears to
assume the 8-bit form, so the writer caps sections at 127 primitives to match.

`tools/iw5cmconv/check_simd.py` validates the simdTree of any blob, generated or shipped.

### The BVH node codecs (mostly solved)

Two levels of quantised AABB tree: a per-section tree of `hkcdStaticTreeCodec3Axis4`
(4 bytes) and a top-level tree over sections of `hkcdStaticTreeCodec3Axis5` (5 bytes).
Array strides confirmed empirically as 5 and 4 bytes.

**Both trees are plain binary BVHs with `numNodes == 2 * numLeaves - 1`.** Verified for
every section in all three stock maps (1400/1400, 1888/1888, 2029/2029) and for the
top-level tree (2799 = 2x1400-1, 3775 = 2x1888-1, 4057 = 2x2029-1).

**AABB encoding** -- taken from IW8's
`hkcdStaticTree::AabbTree<Aabb4BytesCodec>::getNodeAabb` and the matching code in
`convertDynamicTreeToStaticTree`, both of which spell it out in SIMD:

```
hi = xyz[i] >> 4          lo = xyz[i] & 0x0F        // two 4-bit fields per byte
scale[i] = (parentMax[i] - parentMin[i]) * K        // K = g_vectorfConstants[0x1E0]
childMin[i] = parentMin[i] + hi*hi * scale[i]
childMax[i] = parentMax[i] - lo*lo * scale[i]
```

Note the quantisation is **quadratic**, not linear -- `hi*hi` and `lo*lo`. `K` is
consistent with `1/225` (15^2), and an all-zero `xyz` means "child AABB == parent AABB",
which is what shipped single-node sections contain. The AABB is accumulated down the path
from the tree domain; there is no absolute AABB stored per node.

**Per-section tree (4 bytes) -- fully solved.** From IW8's `getNodeAabb`:

```
leftChild  = n + 1
rightChild = n + (data & 0xFE)          // bit 0 is not part of the offset
isInternal = (data & 1) != 0            // bit 0 set = internal, clear = leaf
primitive  = data >> 1                  // when it is a leaf
```

**A leaf's primitive index is `data >> 1`, not its position in the traversal.** Leaves come
out badly shuffled -- one 13-primitive section yields 10, 9, 8, 0, 11, 12, 7, 3, 2, 1, 6,
5, 4 -- and the index is a permutation of `0..primCount-1` in 5,988 of 5,988 sections
across `mp_afghan`, `mp_paris`, `mp_breakneck` and `mp_dome_dusk`. Pairing leaves with
primitives by position instead is why earlier attempts at the AABB codec found nothing.

Traversing with this rule reaches exactly `primitiveCount` leaves in **every section of
all three stock maps** (1400/1400, 1888/1888, 2029/2029).

**Top-level tree (5 bytes) -- leaf rule solved, offsets not.** Leaf identification is
certain:

```
isLeaf       = (hiData & 0x80) == 0
sectionIndex = (hiData << 8) | loData
```

The leaf count under this rule is exactly the section count in all three maps
(1400, 1888, 2029), and leaf `hiData` values run 0,1,2,... with 256 entries each, which is
just the section index paged into the high byte.

What does **not** yet work is the internal-node offset. `rightChild = n + (((hiData &
0x7F) << 8) | loData)` produces a structurally plausible spine -- node 0 offset 1391, node
1 offset 1369, node 2 offset 42, all in range and consistent with depth-first subtree sizes
-- but the traversal only reaches ~10 of 2799 nodes because the left spine terminates
early: node 3 encodes offset 1 (both children = n+1, a unary node), its left child node 4
is a leaf, and nothing then references nodes 5..43 even though those clearly form a real
subtree. So either the left child is not `n + 1` for this codec, or the offset is in
different units. This is the one remaining gap before a mesh can be written.

### The `hknpExternMeshShape` shortcut — checked, and weaker than it looks

IW7 registers `hknpExternMeshShape` (112 bytes, version 1), which looked like a way to
skip the compressor. Its reflection says otherwise:

```
hknpExternMeshShape       +96  geometry            hknpExternMeshShapeGeometry*   flags 0x100
                          +104 boundingVolumeData  hknpExternMeshShapeData*
hknpExternMeshShapeData   +16  aabbTree            hkcdStaticTreeDefaultTreeStorage6
                          +64  simdTree            hkcdSimdTree
                          +88  buildContext        void*                          flags 0x400
```

It does avoid vertex quantisation and section packing — `hknpDefaultExternMeshShapeGeometry`
just holds an `hkGeometry*`, a plain vertex/triangle array. But it still needs a built
`aabbTree` (the 6-byte quantised codec) **and** a `simdTree`, so the BVH construction work
does not go away; only the mesh compression does. Whether the clipmap load path accepts a
non-compressed shape at all is still unverified. Worth a try if the compressor stalls, not
a free win.

## Open questions

- **How is the top-level (5-byte) BVH internal-node child offset encoded?** Everything
  else about both trees is solved and validated. See section 5a.
- (answered) `mp_breakneck`'s ~-40,000 X offset is in the **collision blob**, not the entity
  reading. Its static model origins (p50 x = -40,036) agree with its entity origins
  (-39,856); only the collision domain (-2,506 .. 4,034) differs, consistently across the
  shape list's `minMaxes`, the tree domain and all 2,029 section domains. `mp_paris` and
  `mp_afghan` agree on all axes. Harmless for the converter, which keeps all three in one
  space. See [iw7-havok-backlog.md](iw7-havok-backlog.md) P7.
- What is the single-section `numPackedVertices == 0` variant doing?
- (answered) `HavokPhysicsShapeList` is fully reversed -- see section 5a.
- What are the section-splitting limits in the mesh tree (max primitives per section, max
  packed vertices)? `numPackedVertices` and `numSharedIndices` being `uint8` caps sections
  at 255 of each.
- (answered) `shapeTagCodecInfo` is `0xFFFFFFFF` in all 699 shipped composite shapes; the
  earlier `0x02130004` reading was wrong. The codec is `HavokPhysicsShapeTagCodec`, an IW
  class fed the shape list's own `shapeTagData` (IW8 names its `SetData` and `findShapeTag`).
- Is `hknpExternMeshShape` viable for the world blob?
- (answered) The compressed mesh in an ents blob is a brush model that is not a union of
  convex brushes. Every MP map has exactly one — the `_paths` prefab's air boundary volume,
  113 verts / 192 tris / contents `0x200` — while `cp_zmb` has ~20 concave prefab props.
  See [iw7-havok-backlog.md](iw7-havok-backlog.md) P6.
