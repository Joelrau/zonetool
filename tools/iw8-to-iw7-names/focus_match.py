#!/usr/bin/env python3
"""Targeted cross-match for one subsystem.

The whole-binary pass is candidate-driven: an IW7 function with no shared string literal and
no matched neighbour never gets scored at all. IW7's GPU light-grid code is exactly that case,
so this scores an explicit cross product instead - every IW7 function in a given address range
against every IW8 function whose symbol matches a regex - and prints the ranked evidence for
manual reading rather than deciding anything.

Usage: focus_match.py <iw8.jsonl> <iw7.jsonl> <lo> <hi> <iw8-name-regex> [top]
"""
import json, sys, re, math
from collections import defaultdict

IW8_PATH, IW7_PATH = sys.argv[1], sys.argv[2]
LO, HI = int(sys.argv[3], 0), int(sys.argv[4], 0)
NAME_RE = re.compile(sys.argv[5], re.I)
TOP = int(sys.argv[6]) if len(sys.argv) > 6 else 3

sys.path.insert(0, __file__.rsplit("\\", 1)[0] if "\\" in __file__ else ".")
from msvcname import qualified_name

AUTO = re.compile(r"^(sub_|nullsub_|j_sub_|unknown_libname_|loc_|SEH_|j_nullsub_)")


def load(p, keep):
    out = {}
    for line in open(p, "r", encoding="utf-8"):
        r = json.loads(line)
        if not keep(r):
            continue
        r.pop("shapestr", None)
        r["strs"] = set(r["strs"])
        r["consts"] = set(r["consts"])
        out[r["ea"]] = r
    return out


IW8 = load(IW8_PATH, lambda r: bool(NAME_RE.search(r["name"])) and not AUTO.match(r["name"]))
IW7 = load(IW7_PATH, lambda r: LO <= r["ea"] < HI)
print("iw8 candidates matching /%s/: %d" % (sys.argv[5], len(IW8)))
print("iw7 functions in [%X, %X): %d" % (LO, HI, len(IW7)))


def hist_cos(a, b):
    if not a or not b:
        return 0.0
    dot = sum(v * b.get(k, 0) for k, v in a.items())
    na = math.sqrt(sum(v * v for v in a.values()))
    nb = math.sqrt(sum(v * v for v in b.values()))
    return dot / (na * nb) if na and nb else 0.0


def sim(r8, r7):
    if r8["exact"] == r7["exact"]:
        return 1.0
    h = hist_cos(r8["hist"], r7["hist"])
    ni8, ni7 = max(r8["ni"], 1), max(r7["ni"], 1)
    size = min(ni8, ni7) / max(ni8, ni7)

    def cnt(a, b):
        return 1.0 if (a == 0 and b == 0) else min(a, b) / max(max(a, b), 1)

    return (0.45 * h + 0.25 * size + 0.20 * cnt(r8["ncall"], r7["ncall"])
            + 0.10 * cnt(r8["ncjmp"], r7["ncjmp"]))


rows = []
for ea7, r7 in IW7.items():
    if r7["ni"] < 6:
        continue
    scored = []
    for ea8, r8 in IW8.items():
        s = sim(r8, r7)
        ss = len(r7["strs"] & r8["strs"])
        sc = len({v for v in (r7["consts"] & r8["consts"]) if v >= 0x1000})
        score = s + 0.5 * ss + 0.15 * sc
        scored.append((score, s, ss, sc, ea8))
    if not scored:
        continue
    scored.sort(reverse=True)
    rows.append((scored[0][0], ea7, r7, scored[:TOP]))

rows.sort(reverse=True)
for total, ea7, r7, top in rows:
    print("\niw7 %s @ %X  ni=%d calls=%d cjmp=%d  strs=%d"
          % (r7["name"], ea7, r7["ni"], r7["ncall"], r7["ncjmp"], len(r7["strs"])))
    if r7["strs"]:
        print("     strings: %s" % ", ".join(sorted(r7["strs"])[:4]))
    for score, s, ss, sc, ea8 in top:
        print("     %6.3f  sim=%.2f str=%d const=%d  %-52s @%X"
              % (score, s, ss, sc, qualified_name(IW8[ea8]["name"])[:52], ea8))
