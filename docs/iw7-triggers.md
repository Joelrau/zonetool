# IW7 trigger volumes — how they are stored, and the IW5 → IW7 path

Status: **solved and implemented.** Everything below is checked against shipped data
(18 stock IW5 maps, 7 stock IW7 maps) or against IW7's own code in
`D:\Files\IDB\iw7\iw7_ship_dump.exe.i64`. Nothing here is inferred from struct shape
alone.

Short version: **IW7 stores trigger volumes exactly where IW5 does.** They are not Havok
geometry, they are not in the world collision blob, and they need no new format work —
`MapEnts::trigger` carries them in both games with a binary-compatible hull/slab layout.

---

## 1. The representation

```
MapEnts::trigger : MapTriggers
    TriggerModel[]  count           one per trigger entity
    TriggerHull[]   hullCount       a model owns hulls[firstHull .. +hullCount)
    TriggerSlab[]   slabCount       a hull owns slabs[firstSlab .. +slabCount)
```

A `TriggerHull` is an entity-local AABB (`Bounds`) plus a contents mask; its slabs are
extra half-spaces (`dir`, `midPoint`, `halfSize`) that cut the box down to a convex
volume, so a rotated or bevelled trigger is a box plus a handful of planes rather than a
mesh. `MapEnts::clientTrigger.trigger` is a second, identical array for client-side
volumes (vision sets, reverb zones).

Entities reach a model through the entity string:

| key | value | resolves to |
|---|---|---|
| `model` | `"*N"` | `MapEnts::cmodels[N]` — a solid brush model (door, mover, script_brushmodel) |
| `model` | `"?N"` | `MapEnts::trigger.models[N]` — a trigger volume |

`?N` is **zero-based in both games**. IW5: every one of 18 stock MW3 maps references
`?0 .. ?(trigger.count - 1)`, one entity per model, no gaps. IW7: `mp_afghan` does the
same (58 models, `?0..?57`); `mp_paris`, `mp_breakneck`, `mp_dome_dusk` and `cp_zmb`
leave `models[0]` unreferenced because it is a compiler-generated map-bounds volume
(the one in `mp_dome_dusk` is 55976 x 38560 x 4016 units, contents `0x28004000`). The
base does not change — those maps simply have one implicit model.

## 2. IW5 and IW7 agree bit for bit

| | IW5 | IW7 |
|---|---|---|
| `TriggerHull` | 32 bytes — `Bounds`, `int contents`, `u16 slabCount`, `u16 firstSlab` | identical |
| `TriggerSlab` | 20 bytes — `float dir[3]`, `float midPoint`, `float halfSize` | identical |
| `TriggerModel` | 8 bytes — `int contents`, `u16 hullCount`, `u16 firstHull` | 32 bytes — same three fields, then `windingCount`, `firstWinding`, `flags`, `PhysicsAsset* physicsAsset`, `u16 physicsShapeOverrideIdx` |
| hull bounds space | entity-local (midPoint ~0, entity `origin` applied by the caller) | same |
| contents values seen | `0x28000001`, `0x28000000`, `0x28004000` | the same three, nothing else |

So hulls and slabs pass through by pointer and only `TriggerModel` needs a real copy.
`REINTERPRET_CAST_SAFE` static_asserts the two size equalities at compile time.

Contents needing no remap is worth stating explicitly, because it is not obvious: the
value is not a "this is a trigger" flag, it is the source brush's contents mask, and it
ends up in `gentity_s::r.contents` for clip-mask tests. Measured distribution:

```
IW5  intro          483 x 0x28000001   87 x 0x28000000   50 x 0x28004000
IW5  mp_bootleg      33 x 0x28000001    8 x 0x28000000
IW7  mp_afghan       58 x 0x28000001
IW7  mp_dome_dusk    78 x 0x28000001    1 x 0x28004000
IW7  cp_zmb         142 x 0x28000001    4 x 0x28004000
```

`TriggerWinding` / `TriggerWindingPoint` are an IW7 addition that **no shipped IW7 map
uses** — `windingCount == 0` in all of afghan, paris, breakneck, dome_dusk and cp_zmb.
Leave them empty.

## 3. The runtime path does not go through Havok

This was the open question: `TriggerModel::physicsAsset` and `physicsShapeOverrideIdx`
are set in every stock map (`triggermodeldummydefault` /
`triggermodelstaticdummydefault`, and an index into `MapEnts::havokEntsShapeData`), which
looks like the volume might really be an `hknp` shape with the hulls as vestigial data.
It is the other way round. From the ship binary:

```c
// sub_140C548F0 -- SV_SetTriggerModel, called for an entity whose model is "?N"
sub_140B7AF30(ent->bmodelIndex, &bounds);          // union of trigger.hulls[...] AABBs
ent->r.mins/maxs = bounds;
ent->r.contents  = CM_ContentsOfTriggerModel(ent->bmodelIndex);
SV_LinkEntity(ent);
```

```c
// CM_ContentsOfTriggerModel @ 140B7AD80
return cm.mapEnts->trigger.models[i].contents;     // for i < trigger.count
```

```c
// sub_140B7AF30 -- CM_TriggerModelBounds
hull = &trigger.hulls[model->firstHull];
for (n = 1; n < model->hullCount; n++) ...         // merge AABBs, nothing else read
```

and the point test itself (`sub_140B7B5A0`, shown here on the `stageTrigger` array — the
same code shape serves `mapEnts->trigger`):

```c
if (fabs(p[0] - hull->bounds.midPoint[0]) < hull->bounds.halfSize[0] && ...) {
    if (!hull->slabCount) return 1;
    for (each slab) if (fabs(dot(p, slab.dir) - slab.midPoint) >= slab.halfSize) break;
}
```

**Neither `physicsAsset` nor `physicsShapeOverrideIdx` is dereferenced anywhere on this
path.** They are the physics-body side of a trigger (a Havok body overlapping the volume),
not what makes a script trigger fire. `Load_TriggerModelArray` loads `physicsAsset`
through the normal nullable asset-pointer path, so null is a legal value in a zone.

Conclusion: a converted map gets working triggers from the struct copy alone, with
`physicsAsset = nullptr` and `physicsShapeOverrideIdx = 0xFFFF`.

## 4. Trigger brushes are correctly absent from the world collision blob

`ClipMapCollision::extract` drops IW5 brushes with `CONTENTS_TRIGGER` (`0x40000000`).
That is right, not a stopgap:

- All three stock IW7 world blobs (`havokWorldShapeData`) contain exactly one
  `hknpCompressedMeshShape` under one `HavokPhysicsShapeList` and nothing else — no
  per-volume shapes at all (see [iw7-havok-collision.md](iw7-havok-collision.md) §4).
- Turning a trigger into mesh geometry makes it solid, which is what the earlier
  experiment ran into (players stuck inside large invisible volumes).

The volumes leave through `MapEnts::trigger` instead, so the `skipped_trigger` count in
the converter log is not data loss.

Note that triggers are the *only* non-solid class the world mesh drops on contents grounds
(alongside `CONTENTS_NONCOLLIDING` and origin brushes). Clip volumes are kept — stock IW7
world collision is full of them, and dropping them was a real bug; see P9 in
[iw7-havok-backlog.md](iw7-havok-backlog.md).

## 5. What the converter does

`src/IW5/Converter/IW7/Assets/ClipMap.cpp`:

- `convert_map_triggers()` — one helper for both `MapTriggers` instances: per-element
  `TriggerModel` copy, hulls and slabs by pointer, windings zeroed.
- `audit_trigger_references()` — parses the converted entity string, counts `"?N"`
  references and warns if any index past `trigger.count`. An out-of-range `?N` is a silent
  OOB read at map load, so it is worth catching at dump time.
- `GenerateIW7ClipMap()` calls `generate_mapents()` and assigns the result to
  `clipMap_t::mapEnts`, instead of attaching a stub carrying only the name. The IW7 dumper
  dumps that same object rather than building a second one, so the clipmap and the MapEnts
  asset cannot disagree.
- `cmodel_t::physicsShapeOverrideIdx` is `0xFFFF` for every submodel. It used to be `0`
  for submodel 0, which indexes an empty shape list — `MapEnts::havokEntsShapeData` is
  null on this path.

## 6. Known gaps

- **`clipMap_t::stageTrigger` cannot survive the dump.** It is a third `MapTriggers`,
  indexed by `Stage::triggerIndex` rather than by an entity, and in every stock IW7 map it
  holds exactly the same counts as `MapEnts::trigger`:

  | map | `stageCount` | `stageTrigger` models/hulls/slabs | `MapEnts::trigger` |
  |---|---|---|---|
  | `mp_afghan` | 1 | 58 / 147 / 202 | 58 / 147 / 202 |
  | `mp_paris` | 2 | 68 / 203 / 99 | 68 / 203 / 99 |
  | `mp_breakneck` | 2 | 64 / 157 / 57 | 64 / 157 / 57 |
  | `mp_dome_dusk` | 2 | 79 / 100 / 71 | 79 / 100 / 71 |
  | `cp_zmb` | 5 | 146 / 411 / 324 | 146 / 411 / 324 |

  `IW7::IClipMap::dump` writes the `clipMap_t` struct but not these three arrays, so
  filling them in would put non-zero counts in the blob with no array data behind them and
  desync the linker's parse. They stay empty, and `stageCount` is clamped to 1 —
  `CM_GetStageFromPoint` (`sub_140E42170`) walks `stages[1..stageCount)` and feeds each
  `triggerIndex` into the hull test with no bounds check, but returns immediately at
  `stageCount <= 1`. Carrying stage triggers properly needs a matching change to
  `IClipMap::dump` and to the linker.

- **Trigger Havok shapes are generated but off by default.** Stock gives every trigger a
  compound alongside its slab hulls, for physics bodies overlapping the volume.
  `collision::extract_trigger_hulls()` builds them — a hull is its AABB intersected with its
  slabs, each slab being a pair of parallel planes, so the volume is already convex —
  and this reproduces a closed polytope for all 1,650 trigger hulls across the 18 stock MW3
  maps, none exceeding its declared bounds. It is gated behind
  `ZT_HAVOK_TRIGGER_SHAPES=1` because script triggers work without it and read neither
  field (§3), while a body with the wrong quality would make a trigger *solid* — a worse
  regression than having none, and not checkable without running the game.

- **`TriggerModel::physicsAsset` is null.** Stock points it at
  `triggermodeldummydefault`. Referencing that name would require the asset to exist in
  the converted zone, which it does not; and §3 shows the trigger path does not read it.

## 7. How to re-check any of this

The stock dumps parse straight out of the assetmanager stream
(`src/X64/Utils/IO/assetmanager.hpp`): `dump_array` is
`u8 type=8, u8 existing, u32 count, count * sizeof(T)`, and `IMapEnts::dump` opens with
`dump_single(asset)`, i.e. one `MapEnts` of 0x340 bytes, followed by the trigger arrays in
declaration order. Useful offsets inside `MapEnts`: `trigger` at 24, `clientTrigger` at
104, `havokEntsShapeDataSize` at 336, `numSubModels` at 352. Inside `MapTriggers`:
`count` 0, `models` 8, `hullCount` 16, `hulls` 24, `slabCount` 32, `slabs` 40,
`windingCount` 48, `windingPointCount` 64. Inside `clipMap_t`: `stageCount` at 104,
`stageTrigger` at 112. `sizeof(IW7::TriggerModel)` is 32, confirmed independently by
`Load_TriggerModelArray` (`Load_Stream(a1, varTriggerModel, 32 * count)`).

Sources: `D:\Games\PC\IW7\dump\**` and `D:\Games\PC\IW7\zonetool\**` for IW7,
`D:\SteamLibrary\steamapps\common\Call of Duty Modern Warfare 3\dump\**` for IW5
(`.ents` for the entity strings, `.ents.data` / `.ents.triggers` for the arrays).
