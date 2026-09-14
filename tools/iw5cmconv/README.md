# iw5cmconv — IW5 → IW7 collision tooling

Working area for porting IW5 (MW3) map collision into IW7's Havok format. Background and
format notes are in [docs/iw7-havok-collision.md](../../docs/iw7-havok-collision.md).

**Status: builders exist and validate.** The C++ writers live in
`src/IW7/Common/havok_builder.cpp` — a world `hknpCompressedMeshShape`, the per-entity
shape list of convex compounds, and the per-model `PhysicsAsset`. The Python here is the
instrumentation that validates them against shipped data; it is not the shipping path.

Current results, reproducible with the commands below:

| artifact | check |
|---|---|
| `PhysicsAsset` (all four shipped dummies) | **byte-identical** to stock |
| ents shape lists, generated | pass `entscheck.py` |
| ents shape lists, all four shipped maps | pass the same checker, no per-file exceptions |
| world blobs, 1 / 3 / 14 sections | pass `hkxtool check`; geometry, simdTree and fixups verified |
| every mesh object in every shipped `.hkx` | **389 / 389 decode**, zero failures |
| vertex resolution, all three stock world blobs | **100.00%** |
| per-section BVH leaf containment, all three | **100.00%** (354,425 leaves) |
| BVH leaf primitive indices | permutation of 0..pc-1 in all 5,302 stock sections; enforced |

Both of the long-standing open items are now closed. The ~7% vertex-resolution error (P2a)
was Havok *custom primitives* being decoded as triangles, and the per-section BVH (P2) is
`hkcdCompressedAabbCodecs::Aabb4BytesCodec` with a *quadratic* 4-bit inset,
`parentExtent * v^2 / 225`. Both are written up in
[docs/iw7-havok-backlog.md](../../docs/iw7-havok-backlog.md).

Three facts from the decoder that any converter needs:

- **Scale is consistent across blob types.** World, entity and per-model collision are all
  stored in CoD units **/32**. Applying a 1:1 world scale makes collision 32× too large.
- **`mp_paris`, `mp_afghan`, `mp_breakneck` are stock.** `mp_shipment` and `mp_test_h1` are
  custom ports; `mp_shipment`'s collision is known bad — useful as a negative reference,
  never as validation.
- **Havok pads unused primitive slots with `0xDEADDEAD`.** Never emit them; counting them
  as real quads over-counts triangles.
- **A primitive with `indices[1] == indices[2] == indices[3]` is a custom primitive, not a
  triangle.** Its `indices[0]` indexes `sharedVerticesIndex`, and the word there is a
  descriptor, not a vertex -- low nibble selects `s_customPrimitiveToShapeType =
  { GSK, SET_SHAPE_KEY_A, NOP }`. Every one in stock IW7 world collision is NOP. Decoding
  them as triangles is what produced the old 7% "bad vertex" rate.
- **Packed vertices are 11/11/10 bit-packed** in section space,
  `pos = codecParms[i] + raw * codecParms[3+i]`.
- **Shared vertices are 21/21/22** in *tree* space, reached via
  `sharedVertices[0x10000*section.page + sharedVerticesIndex[first + v - numPackedVertices]]`.
  The page term and the fact that `numSharedIndices` is *not* the pool size are both easy
  to get wrong.

## What works today

| file | what it does |
|---|---|
| `hkpackfile.py` | Havok binary packfile (`.hkx`) reader/writer for hk_2014.2.5-r1, x64 LE. Header, section table, class-name table, and all three fixup tables. Round-trips all 516 shipped files byte-exact. |
| `hkxinfo.py` | CLI to inspect, survey and round-trip-verify packfiles. |
| `hkcompressedmesh.py` | Decoder for `hknpCompressedMeshShape`. Reads **every** mesh object in every shipped `.hkx` -- 389 of 389, including the 38 single-section `numPackedVertices==0` variants that used to raise `NotImplementedError`. Skips `0xDEADDEAD` padding and custom primitives, reporting the latter on `DecodedMesh.custom_primitives`. |
| `hkxtool.py` | `info` / `check` / `diff` / `geom` / `aabb` / `survey` over any blob. `check` is the world-blob validator. |
| `entscheck.py` | Validator for `MapEnts::havokEntsShapeData`: object classes and sizes, `hknpShape` header fields, `hkRelArray` contiguity and alignment, face/plane pairing, CCW winding, convexity, vertex padding and `w`-lane indices, half-edge connectivity both ways, the dynamic AABB tree, the local-vs-global fixup split, and both array indexing schemes. Passes every shipped ents blob and every generated one. |

### Inspect one file

```bash
python hkxinfo.py "D:/Games/PC/IW7/dump/mp_paris/maps/mp/mp_paris.d3dbsp.colmap.hkx"
```

Prints the header, section table, class-name table, object table and fixup counts.

### Survey a tree

```bash
python hkxinfo.py --survey "D:/Games/PC/IW7/dump/**/*.hkx"
```

### Verify the writer

```bash
python hkxinfo.py --roundtrip "D:/Games/PC/IW7/dump/**/*.hkx"
```

Re-serializes every file from its parsed form and compares bytes. This is the correctness
proof for `hkpackfile.py` and should stay at 100%:

```
byte-exact round-trip: 516 / 516
```

## Two traps worth knowing about

Both were found the hard way here and both produce files that look fine in a hex editor.

1. **Packfile padding is `0xFF`, not `0x00`.** `hkPackfileHeader()` does
   `memSet(this, -1, sizeof(*this))` before assigning fields, so `m_contentsVersion` is
   `"hk_2014.2.5-r1\0"` plus a trailing `0xFF` at offset 55, and section-header
   `m_pad[4]` is `0xFF`-filled.
2. **Class-name records carry a `0x09` separator** between the 4-byte signature and the
   name string. Skip it or every class name gains a leading tab.

## What is missing

See section 7 of the format notes. The one blocking unknown is the `sharedVertices`
(`hkArray<uint64>`) encoding — 40 of the 83 per-model mesh objects and the world blob use
it. After
that: the builder, the `HavokPhysicsShapeList` wrapper, and the IW5 surface-flag map.
