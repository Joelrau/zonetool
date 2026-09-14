#!/usr/bin/env python3
"""Assumption-free re-derivation of the IW7 probe record.

The ONLY thing taken as given is what Load_GfxLightGridProbeData proves: the probe array is
probeCount records of 64 bytes (it streams `probeCount << 6`). Numeric format, field count,
grouping, band structure and directional meaning are all derived from the data.
"""
import sys, os
import numpy as np

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import gfxmap

MAPS = [
    ("mp_paris",     r"D:\Games\PC\IW7\dump\mp_paris\maps\mp\mp_paris.d3dbsp.gfxmap"),
    ("mp_afghan",    r"D:\Games\PC\IW7\dump\mp_afghan\maps\mp\mp_afghan.d3dbsp.gfxmap"),
    ("mp_breakneck", r"D:\Games\PC\IW7\dump\mp_breakneck\maps\mp\mp_breakneck.d3dbsp.gfxmap"),
    ("cp_zmb",       r"D:\Games\PC\IW7\dump\cp_zmb\maps\cp\cp_zmb.d3dbsp.gfxmap"),
    ("mp_dome_dusk", r"D:\Games\PC\IW7\zonetool\mp_dome_dusk\maps\mp\mp_dome_dusk.d3dbsp.gfxmap"),
    ("mp_frontend",  r"D:\Games\PC\IW7\zonetool\mp_frontend\maps\mp\mp_frontend.d3dbsp.gfxmap"),
]

RAW = {}
for tag, path in MAPS:
    d = gfxmap.parse(path, stop_after="probeData")
    if d["counts"]["probeCount"]:
        RAW[tag] = np.frombuffer(d["probes"], dtype=np.uint8).reshape(-1, 64)

print("=" * 78)
print("STEP 1  numeric format: which interpretation of 64 bytes is well-formed?")
print("=" * 78)
for tag, b in RAW.items():
    n = len(b)
    f16 = b.view("<f2").astype(np.float64)
    f32 = b.view("<f4").astype(np.float64)
    u16 = b.view("<u2")
    def bad(a):
        return 100.0 * (~np.isfinite(a)).mean()
    # a "sane" float population has no NaN/Inf and a moderate dynamic range
    print("  %-13s f16: %5.2f%% non-finite, |max| %10.4g | f32: %5.2f%% non-finite, |max| %10.4g"
          % (tag, bad(f16), np.abs(f16[np.isfinite(f16)]).max(),
             bad(f32), np.abs(f32[np.isfinite(f32)]).max() if np.isfinite(f32).any() else 0))
print("  -> f32 produces non-finite values and absurd magnitudes; f16 never does.")
print("  -> 64 bytes = 32 float16 slots.")

A = {t: b.view("<f2").astype(np.float64) for t, b in RAW.items()}

print()
print("=" * 78)
print("STEP 2  how many slots carry information at all?")
print("=" * 78)
for tag, a in A.items():
    used = [k for k in range(32) if len(np.unique(a[:, k])) > 1]
    const = {k: a[0, k] for k in range(32) if len(np.unique(a[:, k])) == 1}
    print("  %-13s varying slots: %s" % (tag, used))
    print("  %-13s constant slots: %s" % ("", const))

print()
print("=" * 78)
print("STEP 3  grouping: correlation structure across slots 0..26")
print("=" * 78)
print("  For each lag L, mean |corr(slot i, slot i+L)| over i. A repeating layout of period P")
print("  shows a peak at L = P.")
for tag, a in A.items():
    x = a[:, :27]
    x = x[np.abs(x).sum(axis=1) > 0]
    c = np.corrcoef(x.T)
    line = []
    for lag in range(1, 13):
        vals = [abs(c[i, i + lag]) for i in range(27 - lag)]
        line.append("%d:%.2f" % (lag, np.mean(vals)))
    print("  %-13s %s" % (tag, "  ".join(line)))

print()
print("  Same test restricted to same-position-within-group pairs, for the two rival readings:")
for tag, a in A.items():
    x = a[:, :27]
    x = x[np.abs(x).sum(axis=1) > 0]
    c = np.corrcoef(x.T)
    # reading A: 3 groups of 9  -> members of a triple are (i, i+9, i+18)
    triples_9 = [abs(c[i, i + 9]) for i in range(9)] + [abs(c[i + 9, i + 18]) for i in range(9)]
    # reading B: 9 groups of 3  -> members of a triple are (3i, 3i+1, 3i+2)
    triples_3 = [abs(c[3 * i, 3 * i + 1]) for i in range(9)] + [abs(c[3 * i + 1, 3 * i + 2]) for i in range(9)]
    print("  %-13s mean|corr| for stride-9 triples %.3f   for stride-3 triples %.3f"
          % (tag, np.mean(triples_9), np.mean(triples_3)))

print()
print("=" * 78)
print("STEP 4  sign structure: which slots are non-negative?")
print("=" * 78)
for tag, a in A.items():
    nonneg = [k for k in range(32) if (a[:, k] >= 0).all()]
    print("  %-13s slots that are never negative: %s" % (tag, nonneg))

print()
print("=" * 78)
print("STEP 5  band structure within a group (RMS by position)")
print("=" * 78)
print("  If a group is a degree-2 spherical harmonic expansion, magnitudes fall off by band")
print("  and should cluster 1 + 3 + 5.")
for tag, a in A.items():
    x = a[:, :27]
    x = x[np.abs(x).sum(axis=1) > 0]
    for g, name in ((0, "group0"), (9, "group1"), (18, "group2")):
        rms = [np.sqrt((x[:, g + k] ** 2).mean()) for k in range(9)]
        norm = [r / rms[0] for r in rms]
        print("  %-13s %s  RMS/RMS[0]: %s" % (tag, name,
              " ".join("%d:%.3f" % (k, v) for k, v in enumerate(norm))))
    break  # one map is enough to show the shape; the loop below covers all

print()
for tag, a in A.items():
    x = a[:, :27]
    x = x[np.abs(x).sum(axis=1) > 0]
    rms = np.array([[np.sqrt((x[:, g * 9 + k] ** 2).mean()) for k in range(9)] for g in range(3)])
    avg = rms.mean(axis=0) / rms.mean(axis=0)[0]
    band = [avg[0], avg[1:4].mean(), avg[4:9].mean()]
    print("  %-13s averaged over groups, per-position RMS: %s"
          % (tag, " ".join("%.3f" % v for v in avg)))
    print("  %-13s   -> band means  L0 %.3f  L1(pos1-3) %.3f  L2(pos4-8) %.3f"
          % ("", band[0], band[1], band[2]))
