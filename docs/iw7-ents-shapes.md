# `MapEnts::havokEntsShapeData` — the per-entity Havok shape list

Status: **decoded and generated.** Every layout below was read out of shipped IW7 blobs
and cross-checked against IW7's own reflection tables
([iw7-havok-reflection.txt](iw7-havok-reflection.txt)) and the ship binary. Convex
compounds are emitted for IW5 brush models by `havok::builder::build_ents_shape_list`
(§7), and the result validates against every stock-derived invariant (§8). The dummy
`PhysicsAsset` a brush model or trigger needs beside it is generated too (§7). Brush
models are wired by default; triggers too since 2026-09-17, because the MP touch test
runs against the trigger's body ([iw7-triggers.md](iw7-triggers.md) §3). Both confirmed
working in game on mp_test_h1 (2026-09-17) once the instance w lanes were fixed (§6).

The headline: **this blob does not need the compressed-mesh work.** Brush models and
triggers are convex polytopes in dynamic compounds, and every structure involved —
including the compound's AABB tree — is explicit, unquantised and fully decoded. The
unsolved top-level BVH codec from [iw7-havok-collision.md](iw7-havok-collision.md) §5a
does not appear anywhere in this blob.

---

## 1. The two load functions

`DB_CreateClipMapAsset` (`140B72F0`) is a one-line thunk to
`DB_CreateClipMapAsset_Internal` (`140571D50`), which does two things:

```c
if (clipMap->havokWorldShapeDataSize) {
    g_worldShapeList = Physics_AddShapeList(clipMap->havokWorldShapeData,
                                            clipMap->havokWorldShapeDataSize);
    for (name in g_worldShapeList->shapeNames)   // lowercased IN PLACE
        strlwr(name & ~1);                       // low bit is hkStringPtr's "owned" flag
    RegisterTagTable(&g_worldTagTable, g_worldShapeList + 104 /* shapeTagData */, ...);
}
if (clipMap->numStaticModels)
    StaticModels_CreateClipmapShapes(0, 0xFFFF, 255, 0);
```

`WorldCollision_AddMapEnts` (`140572320`) is the ents-side equivalent:

```c
void WorldCollision_AddMapEnts(MapEnts *ents)
{
    if (!ents->havokEntsShapeDataSize) return;          // <-- silently does nothing
    slot = (g_entsBlob[0] != 0);
    g_entsBlob[slot] = ents->havokEntsShapeData;
    g_entsList[slot] = Physics_AddShapeList(ents->havokEntsShapeData,
                                            ents->havokEntsShapeDataSize);
    if (!g_curEntsShapeList) WorldCollision_ActivateMapEnts(ents);
}
```

`WorldCollision_ActivateMapEnts` (`140571B00`) then publishes the list into
`g_curEntsShapeList` (`144BDF460`) and registers its `shapeTagData` as the active material
table. `Physics_AddShapeList` (`140572C30`) parses the packfile and, for every
`ShapeTagData` entry, looks `materialCRC` up in the game's surface-material CRC table and
writes the resolved index back into `materialId`.

Two slots exist so a transient zone's ents can be added alongside the map's;
`sub_140581AA0` is the matching teardown and sets `g_curEntsShapeList = 0`.

## 2. A null blob is not neutral

`g_curEntsShapeList` has **no static initialiser** — its only writers are
`WorldCollision_ActivateMapEnts` (set) and `sub_140581AA0` (clear to 0). And:

```c
// CM_ContentsOfBrushModel @ 140B7B220
idx = cm.mapEnts->cmodels[i].physicsShapeOverrideIdx;
if (idx >= g_curEntsShapeList->shapeContents.m_size)   // read at +128
    return 0xFDFFBFFF;                                 // permissive default
return g_curEntsShapeList->shapeContents.m_data[idx];  // read at +120
```

`SV_LinkEntity` calls this for every entity whose model is `"*N"`. With
`havokEntsShapeDataSize == 0` the list pointer stays null and that first read is
`*(uint32*)128`. **So the blob has to exist even when it holds no shapes** — and an empty
list is genuinely safe, because every `physicsShapeOverrideIdx` (`0xFFFF` included) then
fails the bounds test and takes the `0xFDFFBFFF` branch. The shipped converted maps
`mp_bog` and `mp_frontend` carry exactly this: a 736-byte packfile whose only object is a
`HavokPhysicsShapeList` with all arrays empty except a 7-entry `shapeTagData`.

`CM_ContentsOfBrushModel` is also the answer to "where does a converted brush model get its
contents mask" — it is `shapeContents[idx]`, not anything in `cmodel_t`.

## 3. What a brush model needs before it collides

From the body-creation path (`sub_140146DA0`, and the gating check in `sub_14014C4B0`):

```c
cmodel = &cm.mapEnts->cmodels[ent->bmodelIndex];
if (cmodel->physicsShapeOverrideIdx != 0xFFFF
    && WorldCollision_GetMapEntsShape(cmodel->physicsShapeOverrideIdx)) {
    physicsAsset = cmodel->physicsAsset;
    contents     = CM_ContentsOfBrushModel(ent->bmodelIndex);
}
...
if (physicsAsset) { /* build the Havok body */ }
```

and in the DObj branch the dependency is spelled out even more plainly:

```c
if (cmodel->physicsAsset) shapeIdx = cmodel->physicsShapeOverrideIdx;
else                      shapeIdx = -1;
```

So **both** fields are required, and they are consistent in shipped data: in
`mp_dome_dusk`, exactly the 41 cmodels with a `physicsAsset` have a shape index, and the
17 with a null asset have `0xFFFF`. The runtime takes the asset from `cmodel_t` /
`TriggerModel` (above), not from the entity string. Stock entity strings do also carry
key `51961` (a static canonical string id, `0xCAF9`, with no text form in the ship exe)
on every `script_brushmodel`, holding `scriptbrushmodeldummydefault` /
`scriptbrushmodeldummyfixed` / `scriptbrushmodeldummy_mpairdropcrate`; the converter
emits that pair verbatim for parity, but nothing in the body-creation path reads it.
It must be emitted as the number: `G_ParseSpawnVars2` hashes a text key into the
dynamic canonical range, and iw7-mod's map_ents parser drops text keys its token table
does not know (0xCAF9 is one of them).

`WorldCollision_GetMapEntsShape` is a one-liner and settles the indexing:

```c
return ((hknpShape**)g_curEntsShapeList->shapes.m_data)[idx];   // shapes is at +0
```

## 4. How the list is partitioned

One flat array, indices `0 .. shapes.size-1`, shared by cmodels and trigger models with no
overlap. `mp_dome_dusk`, 118 shapes:

| owner | indices | count |
|---|---|---|
| `cmodels[i].physicsShapeOverrideIdx` | 8, 49–82, 89, 93, 96, 99, 102 | 40 |
| `trigger.models[i].physicsShapeOverrideIdx` | 1–7, 9–48, 83–88, 90–92, 94, 95, 97, 98, 100, 101, 103–117 | 78 |
| unclaimed / index 0 | 0 | 1 |

Index 0 is nominally referenced by `cmodels[0]` (the world submodel) and by
`trigger.models[0]`, but both have a **null `physicsAsset`**, so per §3 neither reference is
live. All 40 `clientTrigger` models likewise have a null asset and an override of 0.

Composition across the five stock ents blobs: 644 `hknpDynamicCompoundShape` and 32
`hknpCompressedMeshShape`. Every compound's children are `hknpConvexPolytopeShape`.

## 5. Coordinates are CoD units / 32

**This is the easiest thing to get wrong, and it is the opposite of the world blob.**
`havokWorldShapeData` is 1:1 with CoD units; `havokEntsShapeData` is 1/32, like per-model
`PhysicsAsset` blobs.

Measured by comparing each trigger model's hull half-size against its shape's AABB
half-extent in `mp_dome_dusk`, over every axis of every trigger with exactly one hull:

```
samples = 204    min ratio = 32.0000    max = 32.0000    median = 32.0000
```

e.g. trigger 1 → shape 0: hull half `(348, 308, 116)` vs shape half `(10.875, 9.625, 3.625)`.
Shapes are also **entity-local** — the entity `origin` is applied by the runtime, exactly as
for trigger hulls.

## 6. Object layouts

All offsets from IW7's reflection tables; all values confirmed against shipped bytes.

### `HavokPhysicsShapeList` — 152 bytes, root object

| off | member | type | notes |
|---|---|---|---|
| 0 | `shapes` | `hkArray<hknpShape*>` | indexed by `physicsShapeOverrideIdx` |
| 16 | `shapeIndices` | `hkArray<int32>` | identity (`[i] == i`) in every shipped blob |
| 32 | `shapeNames` | `hkArray<hkStringPtr>` | `"<sourcemap>.map:entity N"`; lowercased in place for the world list only |
| 48 | `vertCounts` | `hkArray<int32>` | total vertices over all convex children |
| 64 | `triCounts` | `hkArray<int32>` | 0 for convex shapes |
| 80 | `minMaxes` | `hkArray<hkVector4f>` | 2 per shape — **all zero in every shipped ents blob** |
| 96 | `numWorldGeoShapes` | `int32` | 0 |
| 104 | `shapeTagData` | `hkArray<ShapeTagData>` | material palette; registered at load |
| 120 | `shapeContents` | `hkArray<int32>` | **the CoD contents mask** per shape, read by `CM_ContentsOfBrushModel` |
| 136 | `convexCounts` | `hkArray<int32>` | number of convex children |

`ShapeTagData` is 24 bytes: `uint32 collisionFilterInfo` @0, `int32 materialCRC` @4,
`uint16 materialId` @8, `uint64 userData` @16. `materialId` is `0xFFFF` in every shipped
entry — the runtime fills it in from `materialCRC`. `0x1AB7BC33` is the common default CRC.

**There are two indexing schemes, and they only coincide when the list has no holes.**
`shapes`, `shapeIndices`, `shapeNames` and `shapeContents` are indexed by **slot** and
sized `shapes.size`. `vertCounts`, `triCounts`, `convexCounts` and `minMaxes` are indexed
by the **compacted** index — the number of non-null shapes before this one, which is
exactly what `shapeIndices[i]` holds — and are sized by the count of non-null shapes.

Only `cp_zmb` ships a hole (slot 163 is a null shape), which is why every other map has
`shapeIndices` as the identity and all nine arrays the same length. cp_zmb gives it away:
`shapes`/`shapeIndices`/`shapeNames`/`shapeContents` are 216 while `vertCounts`/
`triCounts`/`convexCounts` are 215 and `minMaxes` is 430. Reading `convexCounts` by slot
matches 187/192 of its compounds; by compacted index, 192/192.

`shapeContents` *has* to be slot-indexed — `CM_ContentsOfBrushModel` looks it up directly
with `physicsShapeOverrideIdx`. A generator that emits no null shapes can use the identity
for both, which is what this one does.

`shapeContents` values seen in `mp_dome_dusk`: `0xC7FFBFFF` on all 78 trigger shapes, and
`0x200` / `0x1040` / `0x30000` / `0x30200` / `0x31640` on the brush-model shapes.

### `hknpShape` — 48 bytes, base of everything

`+16 flags:u16`, `+18 numShapeKeyBits:u8`, `+19 dispatchType:u8`, `+20 convexRadius:float`,
`+24 userData:u64`, `+32 properties:ptr`.

Shipped values: convex polytopes `flags=0x0143` (rarely `0x0103`), `numShapeKeyBits=0`,
`dispatchType=1`, `convexRadius=0`. Compounds `flags=0x0004`, `dispatchType=2`,
`numShapeKeyBits = instanceCount.bit_length()` — 1 for 1 instance, 2 for 2–3, 3 for 4–7,
4 for 8–15, 6 for 32. Verified on all 644 shipped compounds.

### `hknpConvexPolytopeShape` — 96 bytes

```
: hknpConvexShape(64) : hknpShape(48)
  +48 vertices     hkRelArray<hkVector4f>
  +64 planes       hkRelArray<hkVector4f>
  +68 faces        hkRelArray<hknpConvexPolytopeShapeFace>
  +72 indices      hkRelArray<uint8>
  +80 connectivity hknpConvexPolytopeShapeConnectivity*
```

`hkRelArray<T>` is **`{ uint16 size; uint16 offset }`, with `offset` relative to the address
of the field itself**. All four payloads are packed contiguously right after the 96-byte
object, in declaration order, each 16-byte aligned. Decoded from a shipped box:

```
vertices  size=8  offset=48   -> cp+48+48  = cp+96    (8 x 16 bytes)
planes    size=6  offset=160  -> cp+64+160 = cp+224   (6 x 16)
faces     size=6  offset=252  -> cp+68+252 = cp+320   (6 x 4)
indices   size=24 offset=280  -> cp+72+280 = cp+352   (24 x 1)
```

- **vertices** — `hkVector4f`, position in CoD/32. The `w` lane is **not** padding: it
  carries the vertex's own index, as `0x3F000000 | i` — 0.5f with the index in the low
  mantissa bits. 16,257 of 16,660 shipped vertices match exactly; the 403 that do not are
  the padded copies below, which repeat the last real vertex's `w` along with its position.
  **The array is
  padded up to a multiple of four, repeating the last real vertex.** True for 1,848 of
  1,848 shipped convexes, with every pad entry a byte-copy of the last real one. Havok
  processes vertices four at a time, so an unpadded array is read past its end — a
  six-vertex wedge would pull two vertices out of the planes array that follows it.
  `hknpConvexPolytopeShapeConnectivity::vertexEdges` is padded the same way, repeating the
  last real half-edge.
- **planes** — `hkVector4f` `(nx, ny, nz, d)`, **outward** normal, plane is `dot(n,p) + d = 0`,
  so `d = -distance`. One per face, same order as `faces`.
- **faces** — `{ uint16 firstIndex; uint8 numIndices; uint8 minHalfAngle }`. `minHalfAngle`
  is `127` on every face of a box.
- **indices** — `uint8` vertex indices, **counter-clockwise about the outward normal**
  (verified by cross product on a decoded box face).
- **connectivity** (48 bytes) — `+16 vertexEdges hkArray<Edge>` (one per vertex),
  `+32 faceLinks hkArray<Edge>` (one per index entry), `Edge = { uint16 faceIndex;
  uint8 edgeIndex }` (4 bytes). Present on all 148/148 convexes in `mp_dome_dusk`.

### `hknpDynamicCompoundShape` — 208 bytes

```
: hknpCompoundShape(192) : hknpCompositeShape(96) : hknpShape(48)
  +48  edgeWeldingMap    hknpSparseCompactMap<uint16>  (40 B; zeroed, secondaryKeyMask = 0xFFFFFFFF)
  +88  shapeTagCodecInfo uint32                        (0xFFFFFFFF)
  +96  instances         hkFreeListArray<hknpShapeInstance>  { hkArray @0; int32 firstFree @16 = -1 }
  +128 aabb              hkAabb (32 B)   -- union of the children, in CoD/32
  +160 isMutable         bool = 1
  +168 mutationSignals   SERIALIZE_IGNORED
  +192 boundingVolumeData hknpDynamicCompoundShapeData*
```

`hknpShapeInstance` is 128 bytes: `+0 transform` (`hkTransform`: 3 rotation columns then
translation, all `hkVector4f`), `+64 scale` (`(1,1,1,1)`), `+80 shape*`, `+88 shapeTag:u16`
(indexes `shapeTagData`), `+90 destructionTag:u16` (`0xFFFF`), `+92 padding[30]`.

**The transform's w lanes are payload, not padding.** hknp packs an int24 into the low
mantissa bits of `0.5f` (`hkVector4::setInt24W`), the same trick as the polytope vertex
index. Column 0's w is the instance **flags**: `0x3F000040` on all 232 instances in
mp_fallen + mp_afghan. The translation's w is the index of the instance's **leaf node in
the compound's dynamic tree** (§ below): `0x3F000001` for a lone instance (the root is
the leaf), `2,3` for two instances, `2,4,5` for three, and so on. The compound's own
`aabb.min.w` is `0x3F000000` on 189 of 232 compounds (a small count on the rest) and
`aabb.max.w` is 0. Writing zeros here (as this generator did until 2026-09-17) produces
a file that passes every structural check and loads, but the runtime's shape queries skip
every instance: brush-model and trigger bodies were created and nothing ever overlapped
them -- players walked through `script_brushmodel`s and `trigger_hurt` never fired.
Confirmed fixed in game on mp_test_h1. `entscheck.py` now enforces all three lanes.

### `hknpDynamicCompoundShapeData` — 56 bytes, and its tree

`+16 aabbTree : hkcdDynamicTreeDefaultTree32Storage` (40 bytes):

```
+0  nodes     hkArray<Node>
+16 firstFree int32 = 2*L
+20 (0)
+24 numLeaves int32 = L
+28 (0)
+32 root      int32 = 1
+36 (0)
```

**Node is 32 bytes** and the links live in the `w` lanes of the AABB:

```
+0  float min[3]
+12 uint16 parent        (0 for the root)
+14 uint16 0x3F00        constant filler (top half of 0.5f)
+16 float max[3]
+28 uint32 data          leaf     -> (instanceIndex << 16), low half 0
                         internal -> leftChild | (rightChild << 16), low half non-zero
```

`nodes.size == 2*L + 1`. Node `0` and node `2L` are all-zero (sentinel and free slot); node
`1` is the root; the tree is a plain binary BVH over the instances. Leaf discriminator is
`(data & 0xFFFF) == 0`, which is unambiguous because a child index is always ≥ 2.

Verified on every compound in all five stock ents blobs: node count, the
`(2L, 0, L, 0, 1, 0)` tail, and that the decoded leaves are a permutation of `0..L-1`.

Emission order per shape in `__data__`: compound, then its convexes, then their
connectivity objects, then the compound's `hknpDynamicCompoundShapeData`.

## 7. The generator

`havok::builder::build_ents_shape_list(const ents_input&)` in
[havok_builder.cpp](../src/IW7/Common/havok_builder.cpp) writes the whole thing: the shape
list, one `hknpDynamicCompoundShape` per brush model, its `hknpConvexPolytopeShape`
children with their connectivity, and the dynamic AABB tree. Passing no shapes gives the
empty form from §2.

The geometry comes from `collision::extract_brush_models()` in
[ClipMapCollision.cpp](../src/IW5/Converter/IW7/Assets/ClipMapCollision.cpp), which reuses
the same half-space clipping that feeds the world mesh but keeps the polygons whole — a
polytope wants one face per plane, not triangles. Per brush model it walks the cmodel's
leafbrush tree, clips each brush to a hull, welds shared corners (0.05 unit tolerance,
since clipping produces corners that differ in the last bits), and orients each face
counter-clockwise about its outward normal by measuring the polygon normal rather than
assuming — `base_winding_for_plane` builds on a left-handed basis, so the raw windings come
out the wrong way round.

`extract_brush_models` also rebases each model so its hulls sit where `cmodel_t::bounds`
says they do. IW7 stores these shapes in entity space (every stock shape AABB matches its
cmodel bounds to within the one unit CoD pads them by), and rather than assume which space
IW5 keeps brush-model brushes in, the converter measures the offset between the hull union
and the bounds it is copying across, and cancels it. If the brushes are already model-local
that is a no-op.

`generate_mapents` then assigns `cmodels[N].physicsShapeOverrideIdx` and, per §3, a
`physicsAsset` — without which no body is ever built. It names
`scriptbrushmodeldummydefault`, the same dummy stock uses, and logs that the zone has to
contain it:

```
mapents: N brush models given havok shapes; the zone must contain
         "physicsasset,scriptbrushmodeldummydefault"
```

**That asset is now generated too** — `havok::builder::build_physics_asset()` emits the
`.hkx`, and reproduces all four shipped dummies **byte for byte**:

```
scriptbrushmodeldummydefault      BYTE-IDENTICAL
scriptbrushmodeldummyfixed        BYTE-IDENTICAL
triggermodeldummydefault          BYTE-IDENTICAL
triggermodelstaticdummydefault    BYTE-IDENTICAL
```

They are 1760 bytes each, byte-identical across every shipped map, and the whole family is
two parameters: the body name (`scriptbrushmodeldummy` or `triggermodeldummy`) crossed with
a body-quality CRC (`0x7923E35C` "default" or `0xD0452309` "fixed"/static). The chain is
`HavokPhysicsAsset` → `hknpPhysicsSystemData` → one static `hknpBodyCinfo` → a placeholder
`hknpConvexPolytopeShape` with `hkRefCountedProperties` → `hknpShapeMassProperties`.

The placeholder box decodes to half-extents 19.68 × 24.68 × 14.68 CoD units — that is
20/25/15 shrunk by the 0.01 (= 0.32 CoD) convex radius, which confirms **Havok stores a
convex polytope's vertices shrunk inward by `convexRadius` and adds it back when
colliding**. The shipped ents convexes all use `convexRadius = 0`, so the ents emitter is
right not to shrink. The box itself is never collided against, since
`physicsShapeOverrideIdx` replaces the shape; its vertices, planes and compressed mass
properties are emitted as verbatim bit patterns because only an exact copy reproduces
stock's `-0.0f` lanes and opaque quantised inertia tensor.

`IW7::IPhysicsAsset` writes the assetmanager stream that sits beside the `.hkx`, at
`physicsasset/<name>`. Its order, taken from the shipped dumps: `dump_single(asset)` over
the 0x50-byte `PhysicsAsset` struct, then the name string, then one asset token per SFX and
VFX event slot — `numSFXEventAssets` and `numVFXEventAssets` are both 1 on every shipped
asset and both point at nulls. The converter builds the asset, points every shaped brush
model at it, and the IW5→IW7 clipmap dumper emits each distinct one once.

`mp_frontend` — itself a converted map — ships 32 physicsasset files and 16 CSV lines, so
the linker does handle the type. What is *not* verified is that a converted map loads with
one attached; that needs a re-dump and a run.

### minHalfAngle

Read out of IW8's encoder, not fitted. With the support angle in radians:

```
v            = (int)((angle * 0.5 - 2^-23) * 41720.875 + 0.5)
minHalfAngle = clamp(v, 0, 65535) >> 8
```

`41720.875` is about `131072/pi`, so the field is `floor(halfAngle / pi * 512)`. A right
angle gives `pi/4 * 41720.875 + 0.5 = 32766.4` and `32766 >> 8 = 127` — which is what every
axial brush produces, and what 6,441 of 7,794 shipped faces store.

The angle itself is the smallest support angle to an adjacent face. IW8's
`findFaceSupportAngle` identifies adjacency by scanning all other faces for shared first and
last vertex indices, where this emitter uses the half-edge twin; the two agree on right
angles and diverge on some oblique ones, which is why the match rate against stock sits at
80–92% per map. As a query-widening hint rather than geometry, that is left as a refinement.

## 8. Validation

`tools/iw5cmconv/entscheck.py` checks a blob against every invariant in this document:
object classes and sizes, the `hknpShape` header fields, `hkRelArray` payload contiguity
and 16-byte alignment, face/plane pairing, counter-clockwise winding, convexity (no vertex
in front of any face plane), the half-edge connectivity in both directions, and the whole
dynamic tree.

```
gen_shapes.hkx      5 shapes  PASS   (minHalfAngle 100% of 64)   boxes + wedges
gen_empty.hkx       0 shapes  PASS
cp_zmb            216 shapes  PASS   (minHalfAngle  80% of 3406)
mp_afghan         106 shapes  PASS   (minHalfAngle  84% of 1332)
mp_breakneck      106 shapes  PASS   (minHalfAngle  92% of 1269)
mp_paris          131 shapes  PASS   (minHalfAngle  91% of 1787)
```

The generated blobs also round-trip byte-exact through `hkpackfile.py`'s writer. The test
geometry deliberately includes triangular prisms so the 6→8 vertex padding and a non-right
dihedral are exercised, not just boxes.

**Pointer patches are split by what they point at.** A pointer to a registered object —
anything with a virtual fixup — is a **global** fixup with `dstSectionIndex = 2`; a pointer
to raw payload (array elements, string bytes) is a **local** one. Shipped files are
completely consistent: 532/532 global fixups land on an object and 0/1420 local ones do,
and every object except the root has exactly one global fixup pointing at it. Both resolve
to the same address inside a single section, so getting this wrong yields a file that
parses and loads; the loader uses the distinction to track objects.

Layout bugs caught this way, all of which produce a file that looks plausible and is
silently wrong:

- `hknpShape`'s fields end at +40 but the object is **48** bytes, so both the compound and
  the convex need 8 bytes of padding before their derived members.
- `hknpShapeInstance`'s `padding[30]` ends at +122 and the object is **128**. Getting this
  wrong shifts every instance after the first, and only shows up on a multi-brush model.
- `hknpCompoundShape` needs padding after `instances` (+96, 24 bytes) to put `aabb` at its
  16-aligned +128, and again after `mutationSignals` to put `boundingVolumeData` at +192.
- Object pointers were emitted as local fixups instead of global ones.
- Convex vertex arrays were not padded to a multiple of four.
- The vertex `w` lane was written as `0.0f` instead of `0x3F000000 | index`.
- The instance transform's w lanes (flags, leaf node index) were written as `0.0f`. This
  one survived every offline check and only showed up as "body exists, overlap returns 0"
  at runtime.
- Fixup tables were not padded to a 16-byte boundary with `0xFF`. Every section offset in
  every shipped file is 16-aligned, with 0–12 bytes of slack between tables; this affected
  the world emitter as well.
- Local fixups were sorted by source. Stock orders them by **destination** — true in all
  nine shipped files, while virtual fixups are ordered by source and global ones are in
  emission order. This is the last thing that stood between the generated PhysicsAsset and
  a byte-exact match.

The emitter asserts each object's size as it writes it, and `entscheck.py` now enforces the
fixup discipline, the padding rule and both indexing schemes. All four shipped ents blobs
and both generated ones pass it with no per-file exceptions.

## 9. Re-checking this


`tools/iw5cmconv/hkxtool.py info <ents.data.hkx>` prints the shape list summary. The
decoding in this document was done with `hkxtool.load()` plus the offsets above; the
per-shape correlation with `cmodels` / `trigger.models` comes from parsing the sibling
`.ents.data` assetmanager stream (`MapEnts` is 0x340 bytes; `trigger` at +24,
`clientTrigger` at +104, `havokEntsShapeDataSize` at +336, `numSubModels` at +352;
`cmodel_t` is 56 bytes with `physicsShapeOverrideIdx` at +48).

Stock ents blobs: `mp_afghan` 106 shapes, `mp_breakneck` 106, `mp_paris` 131,
`cp_zmb` 216, `mp_dome_dusk` 118. Converted/stub: `mp_bog` and `mp_frontend`, 0 shapes.
