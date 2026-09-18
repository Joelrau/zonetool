# IW7 Umbra tome research log

Findings that drive `UmbraTome.cpp`, `src/UmbraTomeGen` and
`src/IW5/Converter/IW7/Assets/GfxWorldUmbra.cpp`. Addresses are
`iw7_ship_dump.exe` (`D:\Files\IDB\iw7`) unless marked IW8
(`D:\Files\IDB\iw8\1-game_test.exe.c`, which links Umbra with full symbols,
source paths and asserts - read it before reverse engineering IW7's copy).

## Verified

### Why a tome is mandatory

- The static visibility worker `0x1405FB6D0` (worker cmd 33, queued by
  `R_Umbra_QueryStaticVisibility 0x1405FB420`) fills the dpvs vis buffers only
  inside `if (g_world->umbraTomePtr)`. Null tome = nothing visible = empty world.
- Inside that block: `Query::init(tome)` (`0x140E904B0`), gate states
  (`0x140E92A30`), `Visibility::setOutputObjectMask` + optional occlusion
  buffer, `CameraTransform` (`0x140E928F0`), then `queryPortalVisibility`
  (`0x140E923E0`) or, under `r_umbra 0`, `queryFrustumVisibility`
  (`0x140E90860`). Any non-zero `Query::ErrorCode` (2 = OUT_OF_MEMORY, 5 =
  OUTSIDE_SCENE, 7 = UNSUPPORTED_OPERATION get `R_WarnOncePerFrame` 116/115/117)
  falls through to `R_SetAllVisDataForScene 0x140DE3680` = memset 0xFF = draw
  everything. That is the only fallback and it is safe.
- `0x1405FAA30` then expands the output object mask into vis bits through
  `Umbra::ImpTome::getObjectUserIDs` (`0x140E92C70`, 4096 IDs per object max):

      type = id & 0x70000000, index = id & 0xFFFFFF
      0x00000000  surfaceVisData[view][sortedSurfIndex[index]]
      0x10000000  smodelVisData[view][lodData[index]]      (converter: lodData[i+1] = i)
      0x30000000  volumetricVisData[0][index]              (view 0 only)
      0x40000000  primaryLightVisData[0][index]            (view 0 only)
      0x50000000  reflectionProbeVisData[0][index]         (view 0 only)
      0x60000000  decalVisData[0][index]                   (view 0 only)

  It returns immediately when `m_numObjects == 0`, so an empty tome can never
  restore visibility.

### Tome acceptance and versions

- `Load_UmbraTome -> Umbra::Tome::init 0x140E92DB0`: magic high word 0xD600,
  low word (version) in [0x12, 0x14], 16 byte alignment (the x64-zt linker
  does `buf->align(15)` before the blob, `iw7/assets/gfxworld.cpp:1574`),
  `umbraTomeSize >= m_size`. Nothing else, no CRC check.
- Shipped IW7 tomes are version 0x14. The Umbra 3.3.13 optimizer in
  `dep/umbra3` writes 0x12 (`TOME_VERSION_UMBRA_32_FINAL`,
  `umbraTomePrivate.hpp`).
- **IW7's runtime keeps complete 0x12 code paths**, so the optimizer's output
  is handed over unmodified. Evidence (offsets into the Umbra region
  `0x140E8F000..0x140EA6400`, line numbers refer to a `sed` extract of it):
  - query context init (`0x140E9A...`, extract line 11995): `flag = version >=
    0x14`; for `< 0x14` the cell/portal coordinate scale is
    `(tile.treeMax - tile.treeMin) / 65535` per axis and the expand is
    `tile.m_portalExpand` (tile+48); for `>= 0x14` it is the uniform
    `tile+68` scale, `tile+72` inverse and `tile+76` quantised tile bounds
    (`CoordinateSystem34`). The traversal receives that flag
    (`0x140E9AB00` argument) - both branches are live.
  - portal getters at extract lines 995/1051/9588/9651/9838/9942/10312/13666:
    `< 0x14` uses the per-axis 1/65535 scale (`xmmword_1414DD6A0`), `>= 0x14`
    `m_clusterCoordScale` (tome+0x168).
  - `< 0x13` ("33 tome", IW8's `is33Tome()`): gate vertex layout differs (12
    byte Vector3 vs 4 byte floats at +10, lines 9588/14859/15533) and
    `m_boundsMin/Max` are replaced by `m_treeMin/Max` (line 14130, also when
    `m_flags & 4` = TOMEFLAG_NO_OUTPUTBOUNDS).
  - cell lookup `0x140EA3480 -> 0x140EA3300`: tile flag bit 2 (value 4)
    selects the 0x14 `m_cellIndices` indirection; without it the 0x12 layout
    is walked (`m_bsp` at tile+64, 8 byte `TempBspNode`s). The optimizer never
    sets bit 2.
  - `m_userIDStarts == 0` (one ID per object) and `m_objDistances == 0` are
    null-checked everywhere they are read (`0x140E92C70`, extract lines
    1765/11484/7052).
  - IW8 additionally has `convertFrom34Beta` for version 0x13 at load; IW7
    does not need it for 0x12.
- The 0x12 header is 336 bytes, the 0x14 header 368: identical through
  `m_numFaces` (0x148); 0x14 appends `m_tilePortalExpands` (0x14C, unread by
  IW7 as far as grepping finds), `m_boundsMin/Max`, `m_clusterCoordScale`,
  pad. In a 0x12 tome 0x14C is the pad and is 0.
- `ImpTile`: 0x12 = 80 bytes `{treeMin, treeMax, viewTree(20), sizeAndFlags,
  portalExpand, numCellsAndClusters, cells, portals, bsp, numBspNodes, planes,
  numPlanes}`; 0x14 = 96 bytes with `m_cellIndices` after `m_portals` and the
  28 byte `CoordinateSystem34` in place of the BSP fields.
  `m_sizeAndFlags = (size << 8) | flags`, bit 0 leaf, bit 1 empty, bit 2
  cellIndices (0x14 only).
- `Portal` 16 bytes `{link, idx_z, xmn_xmx, ymn_ymx}` (`link >> 29` = face,
  bit 28 outside, bit 27 user/gate, bit 26 hierarchy, low 26 target tile;
  `idx_z >> 16` target cell, low 16 depth). `CellNode` 36 bytes, `ClusterNode`
  20 bytes, `PackedAABB` 12 bytes stored as `[mny|mnx, mnz|mxx, mxy|mxz]`
  (0.16 fixed over the reference box: tile AABB for cells/portals, tome tree
  for clusters). All identical between 0x12 and 0x14 in memory layout.
- `m_lodBaseDistance`: read by `R_Umbra_QueryStaticCamera 0x1405FAFD0` and
  passed as the accurate-occlusion distance. The optimizer writes
  `MINIMUM_ACCURATE_DISTANCE`, default `4 * SMALLEST_OCCLUDER`. Shipped: 128
  (mp_paris, mp_fallen, mp_breakneck), 512 (mp_afghan, mp_frontend); the
  second (gate) tome uses 2048.

### Shipped tome facts (`umbra-tomegen inspect <gfxmap>`)

    map           size      objects  userIDs  tiles(leaf)  cells   clusters  gates
    mp_frontend   42480     40       54       59 (30)      60      30        0
    mp_paris      9720688   8864     21891    6699 (3350)  14008   8092      0
    mp_paris #2   671648    4855     21708    363 (182)    565     420       10
    mp_afghan     7181856   6131     -        3501 (1751)  -       6230      0
    mp_breakneck  4961392   28830    -        1825 (913)   -       2604      0
    mp_fallen     3509968   8529     -        2603 (1302)  -       2363      1

- Object grouping was on (2.5 IDs per object in mp_paris). Lights and probes
  are objects too (mp_paris: 65 light IDs, 18 probe IDs).
- No shipped cell has zero portals; tree bounds are the full +/-262144 range
  with `m_clusterCoordScale` 8 (= 524288 / 65536).
- Every map carries a second tome (`umbraTomeData2`, gates, lodBase 2048) and
  a 4 byte `umbraUnkData` (float 2400.0). Neither is consulted by the static
  visibility path; converted maps ship neither.
- Computation strings are stripped from all shipped tomes.

### Generator

- `dep/umbra3` = Umbra 3.3.13 from `cohaereo/Deimos` (commit f3829ee,
  `crates/umbra3/umbra3-sys/umbra-source`), plus Eigen 3.4.0 headers that
  `source/standard` needs. Built x64, MBCS, C++14, optimized in every
  configuration; `UMBRA_UNLOCKED` disables the license check;
  `umbraMemory.cpp` is replaced by `umbra_stub_allocator.cpp`.
- Scene API: `Scene::insertModel(verts, indices)`, `insertObject(model,
  matrix, userId, OCCLUDER|TARGET|GATE|VOLUME)`, `insertViewVolume`,
  `insertSeedPoint`; `LocalComputation::create(params)->waitForResult()`
  returns the serialized tome. `ComputationParams`: SMALLEST_OCCLUDER,
  SMALLEST_HOLE (voxel size, holes smaller than this do not leak), BACKFACE_LIMIT
  (100 disables), CLUSTER_SIZE, OBJECT_GROUP_COST (0 = one ID per object),
  MINIMUM_ACCURATE_DISTANCE, OUTPUT_FLAGS.
- Tile grid = scene AABB (geometry + view volumes) / tile size, so a target
  box must never be larger than the view volume or the grid explodes.
- Without seed points Umbra drops cell clusters smaller than the largest
  connected one; a camera in a dropped region gets ERROR_OUTSIDE_SCENE, i.e.
  draw everything.

### IW7's occlusion buffer is built from objects, not cells (2026-09-19, confirmed in game)

- Symptom: brush model floor and the gun's reflection probe vanished direction-
  dependently; static surfaces in the same cell drew. A one-cell tome was fine.
  Brush models / scene entities are culled through `OcclusionBuffer::isAABBVisible`
  (`0x140E92B60`, from `R_Umbra_CullSceneEnts`), probes through the object mask.
- Cause: the camera's cell listed only 4 objects. The probe/sky/light proxy boxes
  are view-volume sized, so every *face* of them is behind the map's outer walls
  and Umbra (correctly) lists them in no interior cell unless the object is
  `SceneObject::VOLUME`, which makes its interior count. With them missing, the
  probe is invisible from those cells, and IW7's depth buffer - which evidently
  holds depth only where *visible objects* project, unlike the SDK's per-cell far
  Z - has nothing under the floor region, so the floor's box fails its test until
  some model happens to project there. Fix: `BOX_TARGET = TARGET | VOLUME` for
  every box-proxied target (GfxWorldUmbra.cpp). Verified: floor, reflections and
  popping all fixed.
- IW7 objects tests (`0x140EA4F20` coverage, `0x140EA5080` with depth) mirror
  the SDK's transformBox incl. near clipping; the AABB entity test returns
  visible for boxes crossing the near plane.
- Sealed rooms (the case boxes on mp_test_h1) lose their cells to the
  optimizer's reachability analysis (`g_reachabilityAnalysisThreshold` 0.1:
  clusters under 10% of the largest are dropped, "Filtered 2/3 inside clusters");
  a noclip camera inside then inherits a neighbour cell's visibility and the omni
  light in the room culled at most angles - reproduced in the SDK (49/60 views).
  umbra-tomegen sets the threshold to 0 so every cell survives.
- Shadow views: with `r_umbraShadowCasters 1` (the in-game default; the dvar
  registers as 0 but the config sets 1, stock maps rely on it) IW7 runs Umbra
  queries from each shadow view (`0x1405FB190`, worker views 21-29,
  `QUERYFLAG_IGNORE_CAMERA_POSITION`, threshold FLT_MAX). Start cells are the
  cells the light near-plane quad intersects (SDK `startCellFilter.setQuad`);
  a quad outside the tome or in no cell is ERROR_OUTSIDE_SCENE (safe), a quad in
  cells traverses portals from there. With cells only inside the map, the back
  palm and dynent shadows went missing (back with the dvar at 0). Fix: the view
  volume is the full +/-262144 range, as in every shipped tome, so the empty
  space around the map has cells connected to it; the optimizer skips empty
  tiles, so mp_test_h1 only grows from 67 to 161 cells. Verified in game.
- Exit portals on every cell face (`UMBRA_IW7_CELL_EXIT_PORTALS` in the SDK
  writer) were added while chasing the above and are kept: they change nothing
  measurable in the SDK, make cells look like shipped ones (every shipped cell has
  a portal on all six faces) and only add "outside" max-depth coverage. Not
  proven necessary.
- `r_umbraMinObjectContribution` (20 px = 400 px² of the screen as a fraction),
  `r_umbraQueryParts` (4x1 jobs, atomic OR into one mask), `r_umbraAccurate
  OcclusionThreshold` (256), `r_umbraExclusive` had no effect on the symptoms.
- IW7's portal backfacing test is `d > 0` (SDK: `d > portalExpandLocal`); no
  measurable difference in the SDK check. Bit 25 of `link` is an IW7-only
  intra-tile flag with the target slot narrowed to 25 bits; equivalent for us.
- Shipped 0x14 tomes partition the whole +/-262144 space into cells (huge empty
  "outside" cells with exit portals, cells can overlap, every face has portals);
  the 3.3.13 optimizer's cells cover empty space only. IW7 copes with ours.

## Strong inference

- The per-portal `+4` uint16 in 0x14 tiles is a conservative expand in
  quantised units (`expand * tileScale` is a round world distance, 4.0 for
  most of mp_frontend). Irrelevant for 0x12 tomes, where the tile's float
  `m_portalExpand` is used.
- `TOMEFLAG_DEPTHMAPS 1`, `TOMEFLAG_SHADOW_DEPTHMAPS 2`,
  `TOMEFLAG_NO_OUTPUTBOUNDS 4` (from the source; shipped tomes use 0).

## Open

- Whether IW7 renders correctly from a 0x12 tome in practice (all evidence
  is static). First in-game test: mp_test_h1 with drawn-surface occluders.
- Clipmap brushes as occluders (caulk seals rooms; drawn surfaces do not).
- Object grouping (`OBJECT_GROUP_COST`) to match shipped ID density.
- The second tome and `umbraUnkData`.
- Static models as occluders (IW7 stock maps almost certainly do this).

## History

- Hand-written 0x14 writers (2026-09-18, not kept): a one-cell portal-less
  tome fixed `r_umbra 0` but culled nearly everything under `r_umbra 1`
  because a cell without portals emits nothing in the portal query; a 2-cell
  grid rendered a wedge (packing was 0.16 fixed over the reference box, not a
  unit scale); 4+ cells crashed at `0x140EA2507` inside the traversal
  `0x140EA16A0` with a corrupt queue slot (0x8006). Superseded by running the
  real optimizer.
