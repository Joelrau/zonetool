#!/usr/bin/env python3
"""IW8 -> IW7 function matcher.

Differs from a vote-counting differ in two ways that the ground-truth calibration on this
pair of binaries forced:

  * Evidence is collected per candidate pair and scored ONCE, rather than each signal
    deciding on its own. A single uniquely-shared dvar description string is worth ~78%
    precision alone; the same string plus an agreeing callee set is worth far more.

  * Graph propagation compares *mapped neighbour sets* rather than counting shared
    neighbours. "We both call three functions that were matched to something" measured 25%
    precise; "the set of my matched callees, mapped through the matching, equals yours"
    is a different and much stronger claim.

Confidence thresholds are calibrated against the 8,386 IW7 functions that already carry a
symbol - every run reports its own measured precision per band on that held-out set.
"""
import json, sys, os, math, re
from collections import defaultdict, Counter

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from msvcname import norm as name_norm, qualified_name

IW8_PATH, IW7_PATH, OUTDIR = sys.argv[1], sys.argv[2], sys.argv[3]
os.makedirs(OUTDIR, exist_ok=True)

AUTO_RE = re.compile(r"^(sub_|nullsub_|j_sub_|unknown_libname_|loc_|SEH_|j_nullsub_)")
MAX_POSTING = 40      # a string/constant referenced by more functions than this is noise
MAX_CANDS = 32        # candidate iw8 functions kept per iw7 function
FANOUT_CAP = 60


def load(p, tag):
    out = {}
    with open(p, "r", encoding="utf-8") as fh:
        for line in fh:
            r = json.loads(line)
            r.pop("shapestr", None)
            r["strs"] = set(r["strs"])
            r["consts"] = set(r["consts"])
            r["callees"] = set(r["callees"])
            out[r["ea"]] = r
    print("  %s: %d functions" % (tag, len(out)))
    return out


def is_named(r):
    n = r.get("name") or ""
    return bool(n) and not AUTO_RE.match(n)


print("loading exports ...")
IW8 = load(IW8_PATH, "iw8")
IW7 = load(IW7_PATH, "iw7")

# ---------------------------------------------------------------- indexes
def posting(db, key):
    idx = defaultdict(list)
    for ea, r in db.items():
        for v in r[key]:
            idx[v].append(ea)
    return idx


s8, s7 = posting(IW8, "strs"), posting(IW7, "strs")
c8, c7 = posting(IW8, "consts"), posting(IW7, "consts")
d8, d7 = posting(IW8, "drefs"), posting(IW7, "drefs")

callers8, callers7 = defaultdict(set), defaultdict(set)
for ea, r in IW8.items():
    for c in r["callees"]:
        if c in IW8:
            callers8[c].add(ea)
for ea, r in IW7.items():
    for c in r["callees"]:
        if c in IW7:
            callers7[c].add(ea)

# strings/constants common enough to be shared by unrelated functions carry no signal
STOP_S = {s for s, e in s8.items() if len(e) > MAX_POSTING}
STOP_S |= {s for s, e in s7.items() if len(e) > MAX_POSTING}


def strset(r):
    return r["strs"] - STOP_S


# ---------------------------------------------------------------- similarity
def hist_cos(a, b):
    if not a or not b:
        return 0.0
    if len(a) > len(b):
        a, b = b, a
    dot = sum(v * b.get(k, 0) for k, v in a.items())
    na = math.sqrt(sum(v * v for v in a.values()))
    nb = math.sqrt(sum(v * v for v in b.values()))
    return dot / (na * nb) if na and nb else 0.0


_sim_cache = {}


def struct_sim(ea8, ea7):
    k = (ea8, ea7)
    v = _sim_cache.get(k)
    if v is not None:
        return v
    r8, r7 = IW8[ea8], IW7[ea7]
    if r8["exact"] == r7["exact"]:
        v = 1.0
    else:
        h = hist_cos(r8["hist"], r7["hist"])
        ni8, ni7 = max(r8["ni"], 1), max(r7["ni"], 1)
        size = min(ni8, ni7) / max(ni8, ni7)

        def cnt(a, b):
            return 1.0 if (a == 0 and b == 0) else min(a, b) / max(max(a, b), 1)

        v = (0.45 * h + 0.25 * size + 0.20 * cnt(r8["ncall"], r7["ncall"])
             + 0.10 * cnt(r8["ncjmp"], r7["ncjmp"]))
    _sim_cache[k] = v
    return v


# ---------------------------------------------------------------- candidate generation
print("building candidates ...")
cand = defaultdict(Counter)      # ea7 -> Counter(ea8 -> string-share count)


def add_from_posting(p8, p7, weight):
    for key, eas8 in p8.items():
        eas7 = p7.get(key)
        if not eas7:
            continue
        if len(eas8) > MAX_POSTING or len(eas7) > MAX_POSTING:
            continue
        for b in eas7:
            ctr = cand[b]
            for a in eas8:
                ctr[a] += weight


add_from_posting({k: v for k, v in s8.items() if k not in STOP_S},
                 {k: v for k, v in s7.items() if k not in STOP_S}, 1)
add_from_posting({k: v for k, v in c8.items() if k >= 0x10000},
                 {k: v for k, v in c7.items() if k >= 0x10000}, 1)

# exact instruction-sequence hashes, unique on both sides
h8, h7 = defaultdict(list), defaultdict(list)
for ea, r in IW8.items():
    if r["ni"] >= 25:
        h8[r["exact"]].append(ea)
for ea, r in IW7.items():
    if r["ni"] >= 25:
        h7[r["exact"]].append(ea)
exact_pairs = {}
for h, eas8 in h8.items():
    eas7 = h7.get(h)
    if eas7 and len(eas8) == 1 and len(eas7) == 1:
        exact_pairs[eas7[0]] = eas8[0]
        cand[eas7[0]][eas8[0]] += 3
print("  %d candidate iw7 functions, %d unique exact-hash pairs" % (len(cand), len(exact_pairs)))

# ---------------------------------------------------------------- iterative scoring
MATCHED = {}        # ea7 -> ea8, accepted at high/medium
GLOBALS = {}        # iw7 global -> iw8 global
best = {}           # ea7 -> result dict

# an iw7 function whose symbol we already know: used only to measure, never to decide
GROUND = {ea: r["name"] for ea, r in IW7.items() if is_named(r)}


def mapped(seq, table):
    out = set()
    for x in seq:
        y = table.get(x)
        if y is not None:
            out.add(y)
    return out


def set_agreement(m7, c8set):
    """|m7 & c8| and the Jaccard of m7 against the part of c8 that is in the codomain."""
    if not m7:
        return 0, 0.0
    inter = len(m7 & c8set)
    union = len(m7 | (c8set & CODOMAIN))
    return inter, (inter / union if union else 0.0)


def score_pair(ea7, ea8):
    r7, r8 = IW7[ea7], IW8[ea8]
    S7, S8 = strset(r7), strset(r8)
    shared_s = len(S7 & S8)
    jac_s = shared_s / len(S7 | S8) if (S7 or S8) else 0.0
    shared_c = len({v for v in (r7["consts"] & r8["consts"]) if v >= 0x10000})
    exact = exact_pairs.get(ea7) == ea8
    sim = struct_sim(ea8, ea7)

    m7_callees = mapped(r7["callees"], MATCHED)
    ci, cj = set_agreement(m7_callees, r8["callees"])
    m7_callers = mapped(callers7.get(ea7, ()), MATCHED)
    ri, rj = set_agreement(m7_callers, callers8.get(ea8, set()))
    m7_globals = mapped(r7["drefs"], GLOBALS)
    gi, gj = set_agreement(m7_globals, set(r8["drefs"]))

    ev = []
    conf = None

    # --- tier 1: an identical instruction sequence, unique on both sides (99.3% measured)
    if exact:
        conf = "high"
        ev.append("identical %d-instruction sequence, unique in both binaries" % r7["ni"])

    # --- tier 2: several uniquely shared literals
    if shared_s >= 3 and jac_s >= 0.5:
        conf = conf or "high"
        ev.append("%d shared string literals (jaccard %.2f)" % (shared_s, jac_s))
    elif shared_s == 2 and jac_s >= 0.4:
        conf = conf or "medium"
        ev.append("2 shared string literals (jaccard %.2f)" % jac_s)
    elif shared_s == 1:
        ev.append("1 shared string literal")

    # --- tier 3: neighbourhood set agreement. Requires the *mapped* callee set to line up,
    # not merely to overlap - overlap alone measured 25% precise.
    if ci >= 3 and cj >= 0.75:
        conf = "high" if conf != "high" else conf
        ev.append("callee set agrees: %d/%d mapped callees (jaccard %.2f)" % (ci, len(m7_callees), cj))
    elif ci >= 2 and cj >= 0.5:
        conf = conf or "medium"
        ev.append("callee set overlaps: %d/%d mapped callees (jaccard %.2f)" % (ci, len(m7_callees), cj))
    elif ci >= 1:
        ev.append("%d mapped callee(s) in common" % ci)

    if ri >= 3 and rj >= 0.75:
        conf = conf or "high"
        ev.append("caller set agrees: %d/%d mapped callers (jaccard %.2f)" % (ri, len(m7_callers), rj))
    elif ri >= 2 and rj >= 0.5:
        conf = conf or "medium"
        ev.append("caller set overlaps: %d/%d mapped callers (jaccard %.2f)" % (ri, len(m7_callers), rj))

    if gi >= 2:
        conf = conf or "medium"
        ev.append("%d shared matched global(s)" % gi)
    elif gi == 1:
        ev.append("1 shared matched global")

    if shared_c >= 2:
        conf = conf or "medium"
        ev.append("%d shared rare constants" % shared_c)
    elif shared_c == 1:
        ev.append("1 shared rare constant")

    if not ev:
        return None
    ev.append("struct_sim=%.2f" % sim)

    # structural agreement is a gate, not evidence on its own: a pair that disagrees
    # structurally cannot be high no matter what else lines up
    if conf == "high" and sim < 0.55 and not exact:
        conf = "medium"
    if conf is None:
        conf = "low"
    if conf == "medium" and sim < 0.40:
        conf = "low"

    score = (3.0 * exact + 1.2 * shared_s + 1.0 * jac_s + 0.9 * ci + 0.8 * cj
             + 0.6 * ri + 0.5 * gi + 0.4 * shared_c + 1.5 * sim)
    return {"ea8": ea8, "conf": conf, "score": score, "ev": ev,
            "sim": sim, "shared_s": shared_s, "ci": ci, "ri": ri, "gi": gi}


CODOMAIN = set()

# ---------------------------------------------------------------- symbol anchors
# IW7 already carries 8,386 symbols. Where one of those names picks out exactly one IW8
# function, the pair is ground truth rather than a prediction: seed the matching with it so
# the mapped callee/caller sets have something to be measured against from round 0. These
# pairs are deliberately kept out of `best`, so they never appear as results and never
# inflate the precision figures printed at the end.
by_name8 = defaultdict(list)
for _ea, _r in IW8.items():
    if is_named(_r):
        by_name8[name_norm(_r["name"])].append(_ea)
ANCHORS = {}
for _ea7, _r7 in IW7.items():
    if not is_named(_r7):
        continue
    _c = by_name8.get(name_norm(_r7["name"]))
    if _c and len(_c) == 1:
        ANCHORS[_ea7] = _c[0]
MATCHED.update(ANCHORS)
print("  %d symbol anchors seeded from existing IW7 names" % len(ANCHORS))


def run_round(rnd):
    global CODOMAIN
    CODOMAIN = set(MATCHED.values())
    used8 = set(MATCHED.values())
    added = 0
    for ea7, ctr in cand.items():
        if ea7 in MATCHED:
            continue
        if ea7 in ANCHORS:
            continue
        r7 = IW7[ea7]
        if r7["ni"] < 4:
            continue
        results = []
        for ea8, _ in ctr.most_common(MAX_CANDS):
            if ea8 in used8 or not is_named(IW8[ea8]):
                continue
            res = score_pair(ea7, ea8)
            if res:
                results.append(res)
        if not results:
            continue
        results.sort(key=lambda d: -d["score"])
        top = results[0]
        runner = results[1]["score"] if len(results) > 1 else 0.0
        margin = top["score"] - runner
        # an ambiguous winner is never confident, however good its evidence looks
        if margin < 0.5 and top["conf"] == "high":
            top["conf"] = "medium"
            top["ev"].append("demoted: runner-up within %.2f" % margin)
        if margin < 0.25 and top["conf"] == "medium":
            top["conf"] = "low"
            top["ev"].append("demoted: runner-up within %.2f" % margin)
        top["margin"] = margin
        prev = best.get(ea7)
        if prev is None or top["score"] > prev["score"]:
            best[ea7] = top
        if top["conf"] in ("high", "medium") and top["ea8"] not in used8:
            MATCHED[ea7] = top["ea8"]
            used8.add(top["ea8"])
            added += 1
    return added


def propagate_globals():
    votes = defaultdict(Counter)
    for ea7, ea8 in MATCHED.items():
        g7, g8 = IW7[ea7]["drefs"], IW8[ea8]["drefs"]
        if not g7 or not g8 or len(g7) > 24 or len(g8) > 24:
            continue
        for a in g7:
            if a in GLOBALS:
                continue
            ctr = votes[a]
            for b in g8:
                ctr[b] += 1
    added = 0
    for a, ctr in votes.items():
        top = ctr.most_common(2)
        (b, v) = top[0]
        runner = top[1][1] if len(top) > 1 else 0
        if v >= 3 and v >= runner * 2:
            GLOBALS[a] = b
            added += 1
    return added


def expand_candidates():
    """Add neighbours of matched functions as candidates for the next round."""
    added = 0
    for ea7, ea8 in list(MATCHED.items()):
        n7 = (IW7[ea7]["callees"] | callers7.get(ea7, set()))
        n8 = (IW8[ea8]["callees"] | callers8.get(ea8, set()))
        if len(n7) > FANOUT_CAP or len(n8) > FANOUT_CAP:
            continue
        for t7 in n7:
            if t7 in MATCHED or t7 not in IW7:
                continue
            ctr = cand[t7]
            for t8 in n8:
                if t8 in IW8:
                    ctr[t8] += 0  # candidate only; the score decides
                    added += 1
    return added


expand_candidates()   # pull in the anchors' neighbourhoods before round 0

for rnd in range(8):
    g = propagate_globals()
    a = run_round(rnd)
    e = expand_candidates() if rnd < 7 else 0
    print("  round %d: +%d globals, +%d matched (%d total), %d candidate edges added"
          % (rnd, g, a, len(MATCHED), e))
    if a == 0 and g == 0:
        break

# ---------------------------------------------------------------- output + calibration
res = []
for ea7, b in best.items():
    r7 = IW7[ea7]
    r8 = IW8[b["ea8"]]
    res.append({
        "iw7_ea": ea7, "iw7_old": r7["name"], "iw7_named_already": is_named(r7),
        "iw8_ea": b["ea8"], "name": r8["name"], "display": qualified_name(r8["name"]),
        "proto": r8["proto"], "conf": b["conf"], "score": round(b["score"], 3),
        "margin": round(b.get("margin", 0.0), 3), "evidence": "; ".join(b["ev"]),
    })
res.sort(key=lambda m: (-{"high": 3, "medium": 2, "low": 1}[m["conf"]], -m["score"]))
with open(os.path.join(OUTDIR, "matches.json"), "w", encoding="utf-8") as fh:
    json.dump(res, fh, indent=1)

by_conf = Counter(m["conf"] for m in res)
print("\nTOTAL %d matches  high=%d medium=%d low=%d"
      % (len(res), by_conf["high"], by_conf["medium"], by_conf["low"]))
print("iw7 functions with no match at all: %d" % (len(IW7) - len(res)))

print("\nmeasured precision on iw7 functions that already carry a symbol:")
tally = defaultdict(lambda: [0, 0])
errs = defaultdict(list)
for m in res:
    if not m["iw7_named_already"]:
        continue
    ok = name_norm(m["iw7_old"]) == name_norm(m["name"])
    tally[m["conf"]][ok] += 1
    if not ok and len(errs[m["conf"]]) < 10:
        errs[m["conf"]].append((qualified_name(m["iw7_old"]), m["display"], m["evidence"][:100]))
for c in ("high", "medium", "low"):
    wrong, right = tally[c]
    n = wrong + right
    if n:
        print("  %-6s n=%-5d correct=%-5d wrong=%-5d precision=%.1f%%" % (c, n, right, wrong, 100.0 * right / n))
for c in ("high", "medium"):
    if errs[c]:
        print("  -- %s errors --" % c)
        for a, b, e in errs[c]:
            print("     %-40s -> %-40s | %s" % (a[:40], b[:40], e))
