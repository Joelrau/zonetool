#!/usr/bin/env python3
"""Turn matches.json into the deliverable CSVs."""
import json, sys, os, csv, collections

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from msvcname import qualified_name, norm

OUTDIR = sys.argv[1]
IW7_JSONL = sys.argv[2]
m = json.load(open(os.path.join(OUTDIR, "matches.json"), encoding="utf-8"))

# every iw7 function, so the unmatched set can be reported honestly
all7 = {}
for line in open(IW7_JSONL, encoding="utf-8"):
    r = json.loads(line)
    all7[r["ea"]] = r

COLS = ["iw7_addr", "proposed_name", "iw7_existing_name", "iw8_addr", "confidence",
        "score", "margin", "evidence", "prototype"]


def row(e):
    return {
        "iw7_addr": "0x%X" % e["iw7_ea"],
        "proposed_name": qualified_name(e["name"]),
        "iw7_existing_name": e["iw7_old"] if e["iw7_named_already"] else "",
        "iw8_addr": "0x%X" % e["iw8_ea"],
        "confidence": e["conf"],
        "score": e["score"],
        "margin": e["margin"],
        "evidence": e["evidence"],
        "prototype": e["proto"],
    }


def dump(path, rows):
    with open(path, "w", newline="", encoding="utf-8") as fh:
        wtr = csv.DictWriter(fh, fieldnames=COLS)
        wtr.writeheader()
        for r in rows:
            wtr.writerow(r)
    return len(rows)


applied = [e for e in m if e["conf"] in ("high", "medium") and not e["iw7_named_already"]]
review = [e for e in m if e["conf"] == "low" and not e["iw7_named_already"]]
conflict = [e for e in m if e["conf"] in ("high", "medium") and e["iw7_named_already"]
            and norm(e["iw7_old"]) != norm(e["name"])]

n1 = dump(os.path.join(OUTDIR, "iw7_names_applied.csv"), [row(e) for e in applied])
n2 = dump(os.path.join(OUTDIR, "iw7_names_low_confidence.csv"), [row(e) for e in review])
n3 = dump(os.path.join(OUTDIR, "iw7_names_conflicting.csv"), [row(e) for e in conflict])

print("applied (high+medium, iw7 function was unnamed): %d" % n1)
print("low confidence, for review:                      %d" % n2)
print("disagrees with an existing iw7 symbol:           %d" % n3)
matched_eas = {e["iw7_ea"] for e in m}
named7 = {ea for ea, r in all7.items()
          if r["name"] and not r["name"].startswith(("sub_", "nullsub_", "j_sub_", "j_nullsub_"))}
print("\niw7 total functions:            %d" % len(all7))
print("iw7 already carrying a symbol:  %d" % len(named7))
print("iw7 unnamed and now proposed:   %d" % n1)
print("iw7 unnamed, no confident match:%d" % (len(all7) - len(named7) - n1))
by = collections.Counter(e["conf"] for e in applied)
print("\napplied breakdown: high=%d medium=%d" % (by["high"], by["medium"]))
