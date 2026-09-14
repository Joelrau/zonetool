# IW8 -> IW7 symbol propagation

Cross-binary name/prototype transfer, plus the small IDA helpers written alongside it.
Written up in `docs/iw7-iw8-symbol-propagation.md`. RE instrumentation, not a shipping path.

Run order (always against a COPY of the .i64 - IDA 9.1 upgrades format 700 -> 910 in place):

    idat.exe -A -Lx.log -S"export_funcs.py out/iw8_funcs.jsonl" copy_of_iw8.i64
    idat.exe -A -Lx.log -S"export_funcs.py out/iw7_funcs.jsonl" copy_of_iw7.i64
    python match2.py out/iw8_funcs.jsonl out/iw7_funcs.jsonl out
    python report.py out out/iw7_funcs.jsonl
    idat.exe -A -Lx.log -S"apply_names.py out/matches.json - medium" copy_of_iw7.i64

`match2.py` prints its own measured precision per confidence band on every run, using the
IW7 functions that already carry a symbol as held-out ground truth. Retune against those
numbers, not against intuition.

Helpers:

- `focus_match.py` - cross-product one IW7 address range against one IW8 symbol regex, for
  a subsystem the candidate-driven pass never reaches. Prints evidence, decides nothing.
- `decompile.py` - dump Hex-Rays output for a list of addresses.
- `probe_structtables.py` - locate asset struct member-layout tables.
- `msvcname.py` - recover a qualified name from an MSVC mangled symbol (comparison only).

`out/` holds the results of the 2026-09-03 run.
