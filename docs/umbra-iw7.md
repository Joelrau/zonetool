# IW7 Umbra occlusion data — reverse engineering notes

Status: **consumption side fully mapped. The stopgap tome is implemented and confirmed
working in-game — converted IW5 maps render.** What it does not do is cull: static
visibility is forced fully on. Real occlusion generation is still open — see
[Open questions / blockers](#open-questions--blockers).

Goal: port IW5 (MW3) maps to IW7. IW7 ships Umbra 3 occlusion tomes in `GfxWorld`;
IW5 has none. These notes record what the IW7 tome format is, exactly how the engine
validates and consumes it, why a map with no tome renders nothing at all, and what stands
between us and generating a real one.

Sources used:

- zonetool source (this repo)
- `D:\Files\IDB\iw7\iw7_ship_dump.exe.i64` (IDA 9.1 + Hex-Rays; well symbolized,
  Arxan-obfuscated in places)
- `D:\Games\PC\IW7\dump\mp_paris\maps\mp\mp_paris.d3dbsp.gfxmap` — a **shipped IW7 map
  already dumped by this repo's IW7 dumper**. This is the ground truth for every layout
  claim below.
- `D:\SteamLibrary\...\Black Ops III\bin\umbraconvert.exe` (BO3 modtools)

All addresses are IDA VAs in `iw7_ship_dump.exe`, imagebase `0x140000000`.

---

## 1. Framing: Umbra is additive, but it is also load-bearing

IW7 did not replace AABB trees with Umbra. **IW7 still has AABB trees** — they moved out of
`GfxWorld` into the transient-zone asset:

```
src/IW7/Structs.hpp:5837   GfxAabbTree        (56 bytes)
src/IW7/Structs.hpp:5852   GfxWorldTransientZone { ... aabbTreeCounts; aabbTrees; }
```

IW7 also still has `GfxCell`, `GfxPortal`, `GfxWorldDpvsPlanes` — the whole classic
portal/cell DPVS system. So there is no "AABB tree to Umbra" transform to write; the AABB
tree is already handled by the existing IW5 to IW7 converter.

But the classic path is vestigial in IW7. In `mp_paris`, `dpvsPlanes.cellCount == 1` — one
cell for the entire map. Umbra does all the work, and, as section 4 shows, **the static
visibility buffers are only ever written from inside the Umbra code path**. A map with no
tome is not "unculled", it is invisible. This was confirmed in-game.

---

## 2. `GfxWorld` umbra fields (verified)

From `src/IW7/Structs.hpp`, offsets asserted in-tree and independently confirmed both by
parsing the raw `mp_paris` dump and by the field offsets used in `Load_GfxWorld`:

| Offset | Field | mp_paris value |
|---|---|---|
| 4424 | `numUmbraGates` | 0 |
| 4432 | `umbraGates` (`UmbraGate*`) | null |
| 4440 | `umbraTomeSize` | 9,720,688 |
| 4448 | `umbraTomeData` (`char*`) | — |
| 4456 | `umbraTomePtr` (`void*`) | == `umbraTomeData` |
| 4464 | `numUmbraGates2` | 10 |
| 4472 | `umbraGates2` (`UmbraGate2*`, 40 bytes each) | — |
| 4480 | `umbraTomeSize2` | 671,648 |
| 4488 | `umbraTomeData2` | — |
| 4496 | `umbraTomePtr2` | == `umbraTomeData2` |
| 4504 | `umbraUnkSize` | 4 |
| 4512 | `umbraUnkData` | 4 bytes = float `2400.0` |

`UmbraGate2`'s 40-byte size is confirmed twice over: the gap between the two tome records
in the dump is exactly 406 bytes (6-byte array header + 10 x 40), and `Load_GfxWorld`
computes its stream size as `r8 = (count + count*4) << 3` = `count * 40`.

There are **two tomes per map**:

- **Tome 1** — the primary occlusion tome, used by the static visibility path.
  `m_numGates == 0`.
- **Tome 2** — the gate tome. `m_numGates == 10`, matching `numUmbraGates2 == 10`.
  Smaller and coarser (`m_lodBaseDistance` 2048 vs 128).

### Stream alignment is handled by the loader

`Load_GfxWorld` calls `DB_PatchMem_FixStreamAlignment(0xF)` immediately before streaming
each tome — i.e. the game aligns the tome block to 16 bytes itself. This matters because
the validator rejects an unaligned tome (see section 4). `umbraTomePtr` is streamed
separately with `Load_Stream(0, &ptr, 8)` and then overwritten by `Load_UmbraTome`, so a
linker should leave it null.

---

## 3. Tome binary format (verified against real data)

`Umbra::ImpTome`, **368 bytes**, as already declared in `src/IW7/Structs.hpp:67`.
That declaration is **correct** — every offset was re-derived from `mp_paris` and every
internal `DataPtr` lands inside the blob.

Note the in-tree `src/X64/Utils/Umbra/umbra.hpp` copy is the **older 336-byte** variant
(it lacks the trailing `m_boundsMin` / `m_boundsMax` / `m_clusterCoordScale` / pad).
Use the IW7 one for IW7 work.

Header (all `DataPtr` values are byte offsets from the start of the tome):

```
  off  field                            mp_paris tome 1     tome 2
    0  m_versionMagic                   0xD6000014          0xD6000014
    4  m_crc32                          0xB9729593          0xC24D4414
    8  m_size                           9720688             671648
   12  m_lodBaseDistance                128.0               2048.0
   16  m_flags                          0                   0
   20  m_treeMin (Vector3)              -262304,-262144,-261248
   32  m_treeMax (Vector3)               261984, 262144, 263040
   44  m_tileTree (SerializedTreeData, 20 bytes)
   64  m_numObjects                     8864                4855
   68  m_objBounds        -> 0x00883230
   72  m_objDistances     -> 0x008B7130
   76  m_userIDStarts     -> 0x00911B40
   80  m_userIDs          -> 0x008FC530
   84  m_listWidths                     174                 173
   88  m_objectLists      -> 0x0091A5D0
   92  m_objectListSize                 73901               13565
   96  m_clusterLists                   0                   0
  100  m_clusterListSize                0                   0
  104  m_numGates                       0                   10
  108  m_gateIndexMap                   0                   0x000A2DE0
  112  m_gateVertices                   0                   0x000A2E40
  116  m_numGateVertices                0                   1112
  120  m_gateIndices                    0                   0x000A2E10
  124  m_numClusters                    8092                420
  128  m_clusters         -> 0x00000170  (first payload; header is 368 = 0x170)
  132  m_clusterPortals   -> 0x000279A0
  136  m_cellStarts       -> 0x0012BA90
  140  m_numLeafTiles                   3350                182
  144  m_numTiles                       6699                363
  148  m_bitsPerSlotPath                38                  25
  152  m_slotPaths        -> 0x00116CE0
  156  m_tileLodLevels    -> 0x0011E930
  160  m_tiles            -> 0x00132340
  164  m_tileMatchingData               0                   0
  168  m_matchingTrees                  0                   0
  172  m_numMatchingTrees               0                   0
  176  m_numTomes                       0                   0
  180  m_tomeClusterStarts              0                   0
  184  m_tomeClusterPortalStarts        0                   0
  188  m_computationString[128]         "" (all zero)       ""
  316  m_objectDepthmaps                0                   0
  320  m_depthmapFaces                  0                   0
  324  m_depthmapPalettes               0                   0
  328  m_numFaces                       0                   0
  332  m_tilePortalExpands -> 0x001251E0
  336  m_boundsMin (Vector3)
  348  m_boundsMax (Vector3)
  360  m_clusterCoordScale              8.0                 4.0
  364  m_pad[1]                         0
```

Useful observations:

- `m_clusters` == `0x170` == `sizeof(ImpTome)` in both tomes: the cluster array is the
  first payload immediately after the header.
- `m_computationString` is **zeroed** in shipped tomes — no build metadata to reproduce.
- `m_numTiles == m_tileTree.m_numSplitValues` in both tomes (6699, 363).
- `m_flags` is 0, so `TOMEFLAG_DEPTHMAPS` / `SHADOW_DEPTHMAPS` / `NO_OUTPUTBOUNDS` are all
  off, and the depthmap pointers are correspondingly null. Depthmaps are optional.

### CRC — algorithm confirmed

`m_crc32` is **CRC-32C (Castagnoli, poly 0x82F63B78, reflected)**, init `0xFFFFFFFF`,
final complement, over bytes `[8, m_size)` — from `m_size` onward, skipping
`m_versionMagic` and `m_crc32`.

Verified two ways (table-driven and bitwise): both reproduce `0xB9729593` and `0xC24D4414`
exactly on the shipped tomes.

---

## 4. How IW7 loads and consumes a tome

| Address | Symbol |
|---|---|
| `0x1409F3C50` | `Load_GfxWorld` |
| `0x1405F99A0` | `Load_UmbraTome` |
| `0x1405F9940` | `Load_UmbraTome2` |
| `0x140E92DB0` | `Umbra::Tome::init` (unnamed; the validator) |
| `0x1405FBA80` | `R_Umbra_Reset` |
| `0x1405FAFD0` | `R_Umbra_QueryStaticCamera` |
| `0x1405FB420` | `R_Umbra_QueryStaticVisibility` |
| `0x1405FB6D0` | static visibility worker — **the render gate** |
| `0x1405FAA30` | umbra object mask -> dpvs vis bits |
| `0x1405FA620` | `R_Umbra_CullSceneEnts` |
| `0x140DE04F0` | `R_AddWorldSurfacesDpvs` |
| `0x140DE3680` | `R_SetAllVisDataForScene` |
| `0x140E92C70` | `Umbra::Tome::getObjectUserIDs` |

`Load_GfxWorld` calls `Load_UmbraTome(&umbraTomePtr, umbraTomeData, umbraTomeSize)`.
Both loaders are:

```c
if (!tomeData) { *outPtr = 0; return; }     // null is tolerated at load
return Umbra_Tome_init(outPtr, tomeData, tomeSize);
```

### The validator (`0x140E92DB0`)

```c
status = 1 (UNINITIALIZED)          if tome == null
status = 2 (CORRUPT)                if (magic & 0xFFFF0000) != 0xD6000000
status = 5 (BAD_ENDIAN)             ... unless the low 16 bits byteswap to 0xD600
status = 4 (NEWER_VERSION)          if (magic & 0xFFFF) >  0x14
status = 3 (OLDER_VERSION)          if (magic & 0xFFFF) <  0x12
status = 6 (BAD_ALIGN)              if ((uintptr_t)tome & 0xF) != 0
status = 7 (OUT_OF_MEMORY)          if tomeSize < tome->m_size
status = 0 (OK)                     otherwise -> *outPtr = tome
```

So: **versions 0x12–0x14 are all accepted**, the **CRC is never checked at load time**, and
the tome must be **16-byte aligned** (which the loader arranges — section 2).

Note this means the abandoned `0xD6000012` experiment in the old converter did *not* fail
because of its magic. It failed further downstream, on a zero-filled 336-byte body.

### The render gate (`0x1405FB6D0`)

This is why a map with no tome draws nothing:

```c
umbraTomePtr = g_world->umbraTomePtr;
if (umbraTomePtr)
{
    ...build Umbra Query / Visibility / frustum...
    err = <umbra query>;
    switch (err) {
      case 0: goto done;                                    // use umbra's results
      case 2: R_WarnOncePerFrame(116); goto fallback;       // "Umbra query out of memory"
      case 5: R_WarnOncePerFrame(115); goto fallback;       // "Camera outside Umbra view volume"
    }
    if (err != 7) goto fallback;
    R_WarnOncePerFrame(117);                                // "Internal Umbra failure..."
fallback:
    R_SetAllVisDataForScene(sceneIndex);                    // <-- everything visible
done: ;
}
// <-- a null tome lands HERE, having written no visibility at all
```

Two things follow, and they are the whole story:

1. **Null tome means no visibility is ever written.** The vis-data buffers are only touched
   inside the `if (umbraTomePtr)` block. Skip it and nothing is marked visible, so nothing
   draws. This matches the observed behaviour exactly.

2. **Any non-zero Umbra error means everything is visible.** `R_SetAllVisDataForScene`
   `memset`s `surfaceVisData`, `smodelVisData`, `primaryLightVisData`,
   `reflectionProbeVisData`, `volumetricVisData` and `decalVisData` to `0xFF`, masking off
   only the unused tail bits of the final word. It is a deliberate, warned-about
   "draw everything" fallback.

That second point is the exploitable one: **a tome that loads but whose query always fails
yields a fully rendered, completely unculled map.**

The follow-up pass (`0x1405FAA30`) cannot undo it — it returns immediately when
`m_numObjects <= 0`, and every write it makes is an `|=`.

### Object user-ID contract

`0x1405FAA30` walks the umbra output object mask, calls
`Umbra::Tome::getObjectUserIDs(tome, objIndex, buf, 4096)`, and decodes each 32-bit user ID
as `type = id & 0x70000000`, `index = id & 0x00FFFFFF`:

| `type` | Target |
|---|---|
| `0x00000000` | `dpvs.surfaceVisData[scene]` at **`sortedSurfIndex[index]`** |
| `0x10000000` | `dpvs.smodelVisData[scene]` at **`lodData[index]`** |
| `0x30000000` | `dpvs.volumetricVisData[0]` at `index` (scene 0 only) |
| `0x40000000` | `dpvs.primaryLightVisData[0]` at `index` (scene 0 only) |
| `0x50000000` | `dpvs.reflectionProbeVisData[0]` at `index` (scene 0 only) |
| `0x60000000` | `dpvs.decalVisData[0]` at `index` (scene 0 only) |

Bit order within a word is **MSB-first**: `word[i >> 5] |= 0x80000000 >> (i & 0x1F)`.

Note that surfaces and static models are indirected through `sortedSurfIndex` and `lodData`
respectively — a real tome's object indices are indices into the *sorted* orders, not raw
surface/smodel indices.

### Dvars

| Dvar | Description string (verbatim from the binary) |
|---|---|
| `r_umbra` | "Umbra-based visibility culling mode." |
| `r_umbraExclusive` | "Toggle Umbra for exclusive static culling (disables static portal dpvs)" |
| `r_umbraShadowCasters` | "Enables Umbra-based shadow caster culling." |
| `r_umbraQueryParts` | "The number of parts the Umbra query frustum is broken into..." |
| `r_umbraDynamicDpvsSMP` | "Toggle SMP processing of dynamic object culling via Umbra." |
| `r_umbraSpotShadowSelection` | "Umbra culling for shadowed spot selection." |
| `r_umbraAccurateOcclusionThreshold` | — |
| `r_umbraMinObjectContribution` | — |

`r_umbraExclusive` suggests the static portal DPVS can still run, but that does not rescue a
null tome: the vis buffers are still only filled inside the umbra block in `0x1405FB6D0`.

---

## 5. `umbraconvert.exe` (BO3) — what it actually is

It is **not** a format converter. It is a full statically-linked **Umbra 3 Optimizer**,
built from Treyarch's tree:

```
Q:\t7\sdk\umbra3\source\optimizer/GraphicsContainer.hpp
Q:\t7\pc\tools\bin\UmbraConvert_modtools.pdb
```

So the real Umbra tome *builder* is sitting on disk. Confirmed by internal strings:
`"created tome with %d tiles, %d targets and %d gates"`, `"Tile grid %dx%dx%d (at %d,%d,%d)
created for scene: %d tiles"`, `"UMBRA_MAX_CELLS_PER_TILE exceeded by tile %d"`,
`"Grouped %d objects into %d groups"`, `"Connecting LOD tiles"`, `"Computed %d depthmaps"`.

### CLI

The printed usage is **stale and wrong** — passing `-convert_tome` yields
`ERROR: '-convert_tome' is not a recognized argument`. The real options parsed in `main`
(`0x140006280` in a fresh umbraconvert IDB) are:

```
-input_tiles <path>  -output_tiles <path>  [-compressed]          ... tile-compute worker mode
-input_scene <path>  -input_params <path>  -output_tome <path>    ... tome build mode
```

Values are space-separated (`argv[++i]`); options are prefix-matched via `memcmp`.

### Behavioural notes

- `main` reads **`TA_GAME_PATH`** and `SetCurrentDirectoryA`s to it before doing anything.
- It **runs without a license file** — `bin\umbra_license.txt` does not exist in the install
  and no dialog appeared (`UMBRA_SUPPRESS_LICENSE_DIALOG=1` was set as a precaution).
  Licence-failure strings do exist, so this may be build-dependent.
- Given a garbage `-input_scene` it exits silently with status 0 and produces nothing. No
  diagnostic, so black-box format discovery does not work.
- It shells out to itself for tile computation:
  `"%s -input_tiles %s -output_tiles %s%s"`, optionally distributed via SN-DBS.
- BO3 ships a real cached tile dataset at `share\assetconvert\umbra\cod2map\mp_test_t7\` —
  378 files, each a tile-input blob with a 12-byte header (magic `0x9510714F`, 4-byte hash,
  4-byte payload size). These are `-input_tiles` payloads, one level *below* the scene.
- `-input_params` carries at least `smallest occluder`, `smallest hole`,
  `vertexEqualityDistance`, `volume parameters`, `output flags`, `strict view volumes`.

---

## 6. What is implemented now: the "always fail" stopgap tome

Implemented in `src/IW5/Converter/IW7/Assets/GfxWorld.cpp` as `generate_umbra_tome`.

The idea follows directly from the render gate: emit a tome that **passes `Tome::init` but
makes every query fail**, so the engine takes its own `R_SetAllVisDataForScene` fallback and
draws the whole map unculled.

The tome is a bare 368-byte header, zero-filled apart from:

| Field | Value | Why |
|---|---|---|
| `m_versionMagic` | `0xD6000014` | inside the accepted `0x12`–`0x14` window |
| `m_size` | `368` | header only; `umbraTomeSize >= m_size` holds |
| `m_lodBaseDistance` | `128.0` | read by `R_Umbra_QueryStaticCamera`; must be positive |
| `m_flags` | `0` | no depthmaps, matching shipped tomes |
| `m_treeMin` / `m_treeMax` | 64-unit cube at `+200000` on every axis | see below |
| `m_boundsMin` / `m_boundsMax` | same cube | consistency |
| `m_clusterCoordScale` | `1.0` | non-zero |
| `m_crc32` | CRC-32C over `[8, 368)` | not checked at load, but correct anyway |

Everything else — `m_numObjects`, `m_numTiles`, `m_numClusters`, `m_numGates`, and every
`DataPtr` — stays zero. `m_numObjects == 0` in particular guarantees the object-expansion
pass at `0x1405FAA30` returns immediately.

**The dead view volume.** IW5/IW7 BSP coordinates are bounded to +/-131072, so a cube at
`+200000` on all three axes can never contain the camera, while still sitting inside the
+/-262144 range Umbra itself uses for tree bounds in shipped maps. Every query should
therefore report "Camera outside Umbra view volume" (error 5) and fall through to
`R_SetAllVisDataForScene`.

Alignment needs nothing from us: `Load_GfxWorld` calls
`DB_PatchMem_FixStreamAlignment(0xF)` before streaming the tome.

The gate tome (`umbraTomeData2`) is left null. Gates are a T7/IW7 authoring concept with no
IW5 equivalent, and the gate tome is only consulted by gate-state queries, not by the static
visibility path that decides what is drawn.

Verification done so far: the emitted bytes were reproduced independently and checked
against each of the validator's four conditions, and the CRC routine reproduces both
shipped `mp_paris` tome checksums exactly. `IW5.vcxproj` builds clean (Debug/Win32).

**Not yet verified in-game.** See risk 1 below.

---

## 7. Toward real occlusion (Route B)

Writing a genuine tome directly, with no Umbra SDK, is the realistic long-term path:

- the 368-byte header is fully decoded and validated against two real tomes;
- load-time validation is weak (no CRC, no structural check);
- `m_flags == 0` in shipped tomes, so depthmaps / matching trees / multi-tome are optional;
- `mp_paris` is a byte-exact reference to diff against;
- `Umbra::ImpTile` is already declared in `src/IW7/Structs.hpp:33`;
- the object user-ID contract is now known (section 4).

Remaining payload sub-structures to decode: the cluster array, cluster portals,
`m_cellStarts`, the tile KD-tree (`SerializedTreeData` + `m_splitValues`), `m_slotPaths`
(bit-packed at `m_bitsPerSlotPath` = 38 / 25 — *not* byte-aligned), object bounds /
distances / user-ID tables, and the object lists. Each can be decoded empirically from
`mp_paris` and cross-checked against the `Umbra::` accessors still symbolized in
`iw7_ship_dump.exe` (`ImpTome::getUserIDs` `0x140E92D80`,
`Tome::getObjectUserIDByIndex` `0x140E92C40`, `Tome::getObjectUserIDs` `0x140E92C70`,
`KDTree::getLUTSize` `0x140EA2FB0`).

The sensible increment is a single-tile, single-cluster, all-objects-visible tome that
returns success — proving the object-ID plumbing end to end — before attempting any real
occlusion computation.

---

## Open questions / blockers

1. **RESOLVED — the stopgap tome works in-game.** Converted IW5 maps render. The query does
   return an error cleanly on the degenerate tome rather than dereferencing its null payload
   pointers, and the engine takes the `R_SetAllVisDataForScene` fallback as predicted.

   ~~Dynamic visibility is unaffected: none of the six sub-culls dispatched by
   `R_Umbra_CullSceneEnts` dereference the tome.~~ **WRONG — corrected 2026-08-30.**

   The six sub-culls (`0x1405F99C0`, `0x1405F9CB0`, `0x1405F9E40`, `0x1405FA0C0`,
   `0x1405FA3B0`, `0x1405FA6C0`) do not touch the *tome*, but they do consult the *occlusion
   buffer* the umbra query writes:

   ```c
   v19 = sub_1405FAC80(bounds, off_144EC00E0[scene]);   // Umbra::OcclusionBuffer::isAABBVisible
   if (!v19) → entity marked culled (dynEntVisData[type][scene][ent] = 2)
   ```

   `sub_1405FAC80` returns *visible* when the buffer pointer is null, and `sub_140E92B60`
   early-outs *visible* when the buffer's byte at `+1432` is 0. Observed in game: with the
   stopgap tome, **players, grenades and dynamic brush models do not render**; with
   `r_umbra 0` they render but the static world does not (no vis bits are written at all).
   So with umbra on the buffer is non-null *with the flag set* — it holds real occlusion data,
   which means the "every query fails and nothing writes the buffer" premise below does not
   hold for the dynamic path.

   The render gate only clears that buffer inside the `if (!v6)` branch
   (`v6 = *(a1 + 128)`), and when `r_umbraQueryParts.x * .y > 1` the clear is redirected to a
   per-thread scratch buffer, so `off_144EC00E0[scene]` can retain data from the last query
   that succeeded — e.g. the menu map, which is authentic IW7 with a real tome. Neither
   `r_umbraQueryParts 1 1` nor `r_umbraExclusive` changed the symptom in game.

   **FIX, confirmed in game (2026-08-30):** patch
   `sub_140E92B60` (`0x140E92B60`, `Umbra::OcclusionBuffer::isAABBVisible`) to
   `mov eax, 1 / ret` — bytes `B8 01 00 00 00 C3`. Every occlusion test in the engine funnels
   through it, and its own first line already returns *visible* when the buffer holds no data
   (`if (!*(_BYTE *)(alignedBuffer + 1432)) return (2 * (flags & 1)) | 1;`), so forcing that
   result simply disables occlusion-buffer culling. It has exactly three callers, which are the
   three occlusion paths: `sub_1405FA0C0` and `sub_1405FA6C0` (two of the six scene ent cullers,
   direct) and `sub_1405FAC80` (the wrapper feeding the other four cullers, the frustum helper
   `sub_1405FAD40`, `sub_140DE0F20` and `sub_140DE4530`). One patch covers all of them.

   Note this is global: authentic IW7 maps also lose dynamic occlusion culling (more draw calls
   for dynamic objects, no visual change). Gate it on a dvar or on the world being a converted
   zone if that matters.

   *Rejected approach — do not retry:* hooking `R_Umbra_Reset` (`0x1405FBA80`) and nulling
   `off_144EC00E0[0]`. That table is not populated only there: `sub_1405FB190` assigns
   `off_144EC00E0[scene] = *(v7 + 41432)` and `R_Umbra_QueryStaticVisibility` zeroes then
   re-reads the entry, so the null gets overwritten during the query path.

   What remains is purely a cost question: **static geometry is never culled.** Every static
   surface, model, light, reflection probe, decal and volumetric is submitted every frame,
   in the shadow passes as well as the main view. Whether that matters should be *measured*
   on the largest converted map before any further work is committed — IW5 maps are small,
   and this may simply be fine.

2. **Known cosmetic side effect.** The failing query trips `R_WarnOncePerFrame` every frame
   (115 "Camera outside Umbra view volume", or 117 "Internal Umbra failure"), rate-limited
   by `r_warningRepeatDelay`. Harmless, but it will appear in the console.

3. **BLOCKER for driving umbraconvert — the `-input_scene` format is unknown.** It is
   consumed through `Umbra::InputStream` (RTTI `.?AVInputStream@Umbra@@`, wrapped by
   `T7UmbraInputFileStream`), i.e. Umbra 3's own `Scene` serialization. Bad input produces
   no diagnostic and exit code 0, so black-box discovery is dead. Determining it means
   reversing the deserializer inside `umbraconvert.exe` — feasible (3,494 functions, RTTI
   present, decompiles cleanly, not obfuscated) but a genuine separate effort. Also unknown:
   whether BO3's Umbra version falls inside IW7's accepted `0x12`–`0x14` window.

4. **IW8 not yet examined.** IW7 was unambiguous for everything covered here, so
   `1-game_test.exe.i64` was not needed. It remains the reference if the tome payload
   sub-structures (section 7) prove ambiguous.

5. **IW5 IDB not needed so far.** The IW5 structures were fully answered from
   `src/IW5/Structs.hpp`. Note the supplied IDB is a **Xenon (Xbox 360, PowerPC) debug
   build**, not the PC binary.

6. **`umbraUnkData` semantics** — 4 bytes, float `2400.0` in `mp_paris`. Almost certainly a
   distance. Not traced to a consumer; currently left null.

7. **`r_umbra` / `r_umbraExclusive` default values** could not be extracted — the dvar
   registration path is Arxan control-flow-flattened.

---

## Reference artifacts

Extracted from the `mp_paris` dump:

```
mp_paris_tome1.bin   9,720,688 bytes   primary tome
mp_paris_tome2.bin     671,648 bytes   gate tome
```

Locating a tome in any `.gfxmap` dump: the `GfxWorld` struct begins at file offset 6
(the `dump_array` header is `type=8, existing=1, uint32 count=1`), so `umbraTomeSize` is at
file offset `6 + 4440`. The tome payload is then the unique `08 01 <size:u32>` record for
that size.
