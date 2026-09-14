# IW7 vs IW8 Havok — what transfers and what does not

Companion to [iw7-havok-collision.md](iw7-havok-collision.md). This is the structure diff
between the two engines' Havok integrations, written to answer one question: **when can
IW8 be used as a reference for IW7 work, and when will it actively mislead?**

Short answer: use IW8 for **algorithms**, never for **layout**.

Sources: `D:\Files\IDB\iw7\iw7_ship_dump.exe.i64`,
`D:\Files\IDB\iw8\1-game_test.exe.i64` and its 516 MB Hex-Rays dump
`1-game_test.exe.c`, plus 516 shipped IW7 `.hkx` files.

---

## 1. Versions

| | IW7 (Infinite Warfare) | IW8 (MW 2019) |
|---|---|---|
| Havok version | **hk_2014.2.5-r1** | **hk_2018.2.0-r1** |
| Source tree in build paths | `external\Havok\hk2014_2_5_r1` | `external\Havok\hk2018_2_0_r1` |
| Physics API | `hknp` | `hknp` |
| `hkp*` (Physics2012) shapes used | none | none |

Both strings are recoverable from assert paths compiled into each binary. The gap is four
years and several major releases.

The single most useful correction here: **both engines are hknp**. Neither uses the
classic `hkp` API. Grepping IW8 for `hkp*Shape` classes returns nothing — every shape
class in IW8 is `hknp*`. So any plan built around `hkpBvCompressedMeshShape`,
`hkpMoppBvTreeShape` or `hkpStaticCompoundShape` is targeting an API that neither game
instantiates.

---

## 2. Serialization: same idea, and both are packfiles

IW7 ships classic binary packfiles: `hkPackfileHeader` magic `57E0E057 10C0C010`,
`m_fileVersion == 11`, three sections (`__classnames__`, `__types__`, `__data__`),
`__types__` always empty. Verified byte-exact across all 516 files.

IW8 links a much larger serialization surface — `hkReflect` appears 57,627 times in the
IW8 dump versus a comparatively tiny reflection footprint in IW7 — because 2018.2 replaced
the old `hkClass` reflection with the newer `hkReflect` type system, and carries tagfile
support alongside packfiles.

**Practical consequence:** the packfile container structs are stable between the two, and
in fact stable all the way back to the 2014.1.0-r1 SDK headers on disk, which is why those
headers could be used directly. Nothing above the container is stable.

---

## 3. Class-by-class diff for the collision path

Every layout claim for IW7 below comes from IW7's own runtime `hkClass` tables
(see [iw7-havok-reflection.txt](iw7-havok-reflection.txt)); IW8 entries come from its
decompiled symbol set.

### Root objects — IW-custom, not Havok

| | IW7 |
|---|---|
| World collision root | `HavokPhysicsShapeList` sig `0xC909A395` |
| Map-ents root | `HavokPhysicsShapeList` |
| Per-model root | `HavokPhysicsAsset` sig `0x0DFB2195` |

These are Infinity Ward classes. They are not in any Havok SDK and not in IW8 under these
names. Anything about them has to come from IW7 itself.

### The world shape

| | IW7 | IW8 |
|---|---|---|
| Shape class | `hknpCompressedMeshShape` | `hknpCompressedMeshShape` |
| `objectSize` | **160** | differs (not verified) |
| reflection `version` | **5** | — |
| Data class | `hknpCompressedMeshShapeData`, size **256**, version 1 | same name |
| Tree | `hknpCompressedMeshShapeTree` : `hkcdStaticMeshTreeBase` | same name |

Same class *names*, four years apart. This is exactly the trap: the names match, so the
layouts look transferable, and they are not. IW7's `hknpCompressedMeshShape` puts `data`
at +96, `quadIsFlat` at +104, `triangleIsInterior` at +128, `numTriangles` at +152,
`numConvexShapes` at +156, on a 160-byte object. Do not assume any of those hold in 2018.2.

### Quantised AABB codecs — renamed between versions

This is the clearest single illustration of the version drift:

| IW7 (2014.2.5) | IW8 (2018.2) | size |
|---|---|---|
| `hkcdStaticTreeCodec3Axis` | — | 3 (`uint8 xyz[3]`) |
| `hkcdStaticTreeCodec3Axis4` | `hkcdCompressedAabbCodecs::Aabb4BytesCodec` | 4 |
| `hkcdStaticTreeCodec3Axis5` | `Aabb5BytesCodec` | 5 |
| `hkcdStaticTreeCodec3Axis6` | `Aabb6BytesCodec` | 6 |
| `hkcdStaticTreeDynamicStorage4/5/6` | `hkcdStaticTree::AabbTree<Codec>` | 16 |

Same concept — 8-bit-per-axis quantised child AABBs — reorganised and renamed. IW7's
mesh tree uses the 5-byte codec at the top level and the 4-byte codec inside each section.

### Shapes present in each

IW7 registers (from the shipped `.hkx` class-name tables and its reflection):
`hknpCompressedMeshShape`, `hknpConvexPolytopeShape` (+`Connectivity`),
`hknpDynamicCompoundShape`, `hknpStaticCompoundShape`, `hknpExternMeshShape`,
`hknpScaledConvexShape`, `hknpDecoratorShape`, `hknpCompoundShape`, `hknpSphereShape`,
`hknpCapsuleShape`.

IW8 adds, among others: `hknpLodMeshShape`, `hknpLodShape`, `hknpMaskedShape`,
`hknpMaskedCompoundShape`, `hknpParticlesColliderShape`, `hknpScaledConvexVertexShape`,
`hknpConvexVertexShape`, `hknpInplaceTriangleShape`, `hknpBoxShape`, `hknpCylinderShape`,
`hknpDummyShape`, `hknpHeightFieldShape`.

So IW8's shape set is a superset. Nothing IW7 needs is missing from IW8 — the risk is the
opposite direction, reaching for an IW8 shape IW7 cannot deserialize.

---

## 4. Where IW8 is genuinely worth using

IW7's shipping build contains the *decoder* for compressed meshes and essentially none of
the *builder*. IW8's `1-game_test.exe` is a test build and contains both. Named
construction-side functions present in IW8 and absent or unnamed in IW7:

```
hknpCompressedMeshShape::hknpCompressedMeshShape(const hknpCompressedMeshShapeCinfo*)
hknpCompressedMeshShape::buildInternalData(const hknpCompressedMeshShapeCinfo*)
hknpCompressedMeshShape::buildConnectivity
hknpCompressedMeshShape::flagInteriorTriangles
hknpCompressedMeshShape::computeRuntimeInfo
hknpCompressedMeshShape::optimizeForSpeed
hkcdStaticMeshTree::Builder<hkcdDefaultStaticMeshTree>::buildSimdTree
hkcdStaticMeshTree::SectionDecoder<hkcdDefaultStaticMeshTree>::convertToGeometry
hkcdStaticMeshTree::SectionDecoder<...>::getTriangleVertices / getQuadVertices
hknpCompressedMeshShapeInternals::GeometryProvider::*
hknpCompressedMeshShapeCinfo, hknpDefaultCompressedMeshShapeCinfo
hkcdStaticMeshTreeBuilder
```

Plus the mirror-image decoders, which are the most direct statement of the encoding:
`SectionDecoder::getTriangleVertices` and `getQuadVertices` show exactly how
`codecParms[6]`, `packedVertices` and `hkcdStaticMeshTreeBasePrimitive::indices[4]`
combine to produce a vertex, and `GeometryProvider::getCustomPrimitiveInfos` /
`getTriangleLayerData` show how `primitiveDataRuns` is consumed.

**This is the correct use of IW8:** read those functions to recover the *packing
algorithm* — section splitting, vertex quantisation rounding, shared-vertex dedup, data-run
encoding — then implement it against **IW7's** field offsets, taken from IW7's reflection,
and validate against shipped IW7 bytes.

The algorithm is far more stable across versions than the struct layout is. Quantising a
vertex into 8-bit-per-axis coordinates relative to a per-section offset/scale is the same
operation in 2014.2 and 2018.2 even when the surrounding struct grew a field.

---

## 5. Rules of thumb

1. **Layout: IW7 only.** IW7's runtime `hkClass` tables are authoritative and complete for
   2014.2.5-r1. Never copy an offset or `objectSize` from IW8. Never copy one from the
   2014.1.0-r1 SDK either — it has no hknp shape module at all.
2. **Algorithms: IW8 first.** It has named builder and decoder functions; IW7 does not.
3. **Validation: shipped IW7 bytes, always.** For the mesh builder, start on the 43
   per-model mesh objects that already decode cleanly (see
   `tools/iw5cmconv/hkcompressedmesh.py`) -- the smallest file is 2,528 bytes with 6
   objects -- not on a world blob. Any claim that cannot be checked against shipped bytes
   is a guess.
4. **Class names are not a compatibility signal.** `hknpCompressedMeshShape` exists in
   both and means different bytes in each.

## Where this diff is incomplete

IW8's per-class field offsets were not extracted — doing so needs the same reflection
sweep run against the IW8 database, and 2018.2 uses `hkReflect` rather than `hkClass`, so
the extraction script would have to be rewritten. That work was skipped deliberately:
per rule 1 the IW8 offsets are not usable for IW7 anyway, so the only reason to gather
them would be to document the drift for its own sake.
