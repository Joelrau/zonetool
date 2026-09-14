# IW8 → IW7 symbol propagation

Whole-binary name/prototype transfer from the IW8 test build to the IW7 ship dump, run
2026-09-03. Method, measured accuracy, and what it did and did not recover.

## Inputs

| | |
|---|---|
| IW8 source | `D:\Files\IDB\iw8\1-game_test.exe.i64` — **335,605** functions, essentially all named |
| IW7 target | `D:\Files\IDB\iw7\iw7_ship_dump.exe.i64` — **93,358** functions, **8,393** already named |
| Tooling | IDA 9.1 headless (`idat.exe -A -S`), Hex-Rays x64. No BinDiff, no Diaphora |

Both databases were **copied to a scratchpad first**. IDA 9.1 silently upgrades format 700 →
910 under `-A`, which would have permanently rewritten the originals. The originals were
never opened.

`idaapi.auto_wait()` must **not** be called in these scripts. After a format upgrade IDA
queues a full reanalysis; waiting for it stalled the export indefinitely (>5 min with no
progress, one core pegged). Both databases are already fully analysed, so the export reads
stored state and completes in 34 s (IW7) / 88 s (IW8).

## Pipeline

`scripts/export_funcs.py` (headless, per binary) → JSONL, one record per function: address,
symbol, prototype, size, instruction count, call/branch counts, mnemonic histogram, exact and
control-flow-shape hashes, referenced string literals, referenced immediates ≥ 0x100, callees,
data references.

`scripts/match2.py` (offline) scores candidate pairs and assigns confidence.
`scripts/apply_names.py` (headless) writes the accepted names into a copy of the IW7 database.

### One export bug worth recording

The first run produced **zero** strings on both binaries. `ida_nalt.get_str_type()` takes an
**address**, not flags — passing `get_flags(ea)` fails silently and returns nothing. String
evidence is the single strongest signal in this pipeline, so the whole first pass was running
without it. The fix also added a fallback that reads unmarked data as a C string when it is
printable and 4–400 bytes, which matters for the IW7 memory dump where auto-analysis never
turned many referenced literals into strlit items.

Even fixed, the two binaries are asymmetric: **17.1%** of IW8 functions reference a string
against **8.0%** of IW7's, and only **5,275 string literals are common to both**. That
asymmetry, not the matcher, is what bounds coverage.

## Scoring

Evidence is collected per candidate pair and scored **once**, rather than each signal deciding
on its own. Confidence is not hand-assigned — it was calibrated against the IW7 functions that
already carry a symbol, and the earlier design was rejected on those numbers:

| signal, scored alone | measured precision |
|---|---|
| identical instruction sequence, unique both sides | **99.3%** (n=599) |
| unique shared rare constant | 94.1% (n=51) |
| one uniquely shared string literal | 78.1% (n=315) |
| string-set Jaccard | 63.5% (n=96) |
| shared matched neighbour *count* (vote-style graph propagation) | **13–33%** (n=459) |

Counting shared matched neighbours — what a naive differ does — is barely better than chance
here. It was replaced with **mapped-neighbour-set agreement**: take the IW7 function's callee
set, map it through the matching built so far, and require that mapped set to *agree* with the
candidate's callee set (Jaccard ≥ 0.75 over ≥ 3 mapped callees), not merely to intersect it.

Additional rules that the calibration forced:

- Structural similarity is a **gate, not evidence**. A pair that disagrees structurally cannot
  be high, however well its strings line up.
- A winner within 0.5 score of the runner-up is demoted from high to medium; within 0.25,
  medium is demoted to low. Ambiguity is never confidence.
- The 2,147 IW7 symbols that name exactly one IW8 function are seeded as **anchors** — ground
  truth, not predictions. They are excluded from the results and from the precision figures.

## Results

| | |
|---|---|
| IW7 functions | 93,358 |
| already carried a symbol | 8,393 |
| **new names proposed at high or medium** | **1,094** (330 high, 764 medium) |
| low confidence, listed for review only | 4,228 |
| proposals that disagree with an existing IW7 symbol | 59 (never applied) |
| no confident match | 83,871 |

Measured precision on held-out already-named IW7 functions: **high 92.6%**, medium 72.2%,
low 44.1%.

Those figures **understate** the real rate. Most of the "errors" are the same function under a
symbol IW8 renamed: `PM_WeaponProcessState` → `PM_Weapon_ProcessState`, `LUI_RestoreMenu` →
`LUI_RestoreMenu_Internal`, `ClientConnect` → `G_ClientMP_Connect`, `db_inflate` → `inflate`.
Reading the high-band errors individually, 2–3 of 14 are genuinely different functions, so the
true high-band precision is ~98%. Low confidence is reported and never applied, as asked.

## Deliverables

| file | contents |
|---|---|
| `iw7_named.i64` | IW7 database copy with the 1,093 accepted names applied and saved |
| `iw7_names_applied.csv` | IW7 addr → proposed name → IW8 addr → confidence → evidence → prototype |
| `iw7_names_low_confidence.csv` | 4,228 low-confidence proposals, for review |
| `iw7_names_conflicting.csv` | 59 proposals that contradict an existing IW7 symbol |

1,093 names and **94 prototypes** were applied. 999 prototypes were rejected because they
reference IW8-only types that do not exist in the IW7 database — expected, and harmless.

**IW8's 1,737 local types were deliberately not imported.** The IW7 database already carries
hand-built, correct IW7 definitions (`GfxLightGridProbeData`, `GfxVoxelTree`, and the rest),
and IW8 reuses many of those *names* for structs it re-laid-out in 2019 — its `GfxGpuLightGrid`
is 440 bytes where IW7's `GfxLightGridProbeData` is 240. A wholesale import would have silently
replaced correct definitions with wrong ones and corrupted the decompiler output that the light
grid work depends on. `apply_names.py` takes a types path but it is opt-in.

## What this did not recover

**None of IW7's GPU light-grid renderer functions were named by the cross-binary pass**, and no
amount of tuning would have changed that: IW8's light-grid subsystem was rewritten between the
two games. IW8 2019 has `GfxLightGridVolumeAtlas` / `R_LGV_*` — a light grid *volume* atlas that
IW7 has no equivalent of — so the whole family cross-matches to structural noise (`sim` ≈ 0.85
for everything, zero shared strings). A targeted cross-product over IW7's light-grid address
range confirmed this rather than producing matches.

The IW7 light-grid path was instead recovered directly, from IW7's own strings and symbols —
see `iw7-lightgrid-audit.md`.

**IW7's asset struct member tables are also a dead end.** IW8 registers layouts through
`Load_RegisterStructMemberSize(typeName, …, memberName, …, offset, size)`, which would give
authoritative field names. IW7's ship build has the same 40-byte record format — confirmed by
dumping the known table at `0x1414A2EF0`, whose records read `shapes`@0, `shapeIndices`@16,
`shapeNames`@32, `vertCounts`@48, `triCounts`@64, `minMaxes`@80, `numWorldGeoShapes`@96,
`shapeTagData`@104, `shapeContents`@120, `convexCounts`@136, with the packed word holding the
byte offset in its top 16 bits — but the *Gfx* name strings are stripped. A scan for
`GfxLightGrid`, `GfxWorld`, `probeData`, `tetrahedrons` and 20 other type/member names found
**none** of them anywhere in the binary. Only the Havok tables survive, because those come from
Havok's runtime reflection captured in the memory dump.
