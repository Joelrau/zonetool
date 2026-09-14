#!/usr/bin/env python3
"""Identify each of the 9 positions in a probe group, from the data alone.

A directional radiance distribution leans toward brighter surroundings, so the linear
coefficient for axis A must track the SPATIAL gradient of the mean term along A across
neighbouring probes. Second differences separate the quadratic terms the same way.

Assumes only: probePositions are the probe locations, and brightness varies smoothly.
No SH basis, no ordering, no physical prior.
"""
import sys, os
import numpy as np
from collections import Counter

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import gfxmap

MAPS = [
    ("mp_dome_dusk", r"D:\Games\PC\IW7\zonetool\mp_dome_dusk\maps\mp\mp_dome_dusk.d3dbsp.gfxmap"),
    ("mp_paris",     r"D:\Games\PC\IW7\dump\mp_paris\maps\mp\mp_paris.d3dbsp.gfxmap"),
    ("mp_afghan",    r"D:\Games\PC\IW7\dump\mp_afghan\maps\mp\mp_afghan.d3dbsp.gfxmap"),
    ("mp_breakneck", r"D:\Games\PC\IW7\dump\mp_breakneck\maps\mp\mp_breakneck.d3dbsp.gfxmap"),
    # our own converter output, so the run is a stock-vs-ours comparison rather than
    # a re-measurement of the reference maps
    ("OURS",         r"D:\Games\PC\IW7\zonetool\mp_test_h1\maps\mp\mp_test_h1.d3dbsp.gfxmap"),
]
AX = "xyz"


def load(path):
    d = gfxmap.parse(path, stop_after="probeData")
    a = np.frombuffer(d["probes"], dtype="<f2").reshape(-1, 32).astype(np.float64)
    pos = np.frombuffer(d["probePositions"], dtype="<f4").reshape(-1, 3).astype(np.float64)
    return a, pos


def epsilon_report(tag, pos):
    frac = np.round(pos - np.floor(pos), 5)
    uniq = sorted(set(frac.ravel().tolist()))
    print("  epsilons present in probePositions: %s" % uniq)
    # for each epsilon group, what is the local spacing?
    for e in uniq:
        m = np.isclose(frac[:, 0], e)
        if m.sum() < 50:
            continue
        v = np.unique(np.round(pos[m, 0], 4))
        dif = np.diff(v)
        common = Counter(np.round(dif[dif > 1.0], 2)).most_common(3)
        print("    eps %.5f (= 1/%.0f): %6d probes, dominant x spacing %s   -> cell*%.0f?"
              % (e, 1.0 / e if e else 0, m.sum(), common,
                 (1.0 / e) * (common[0][0] if common else 0) / (1.0 / e) if common else 0))
        if common:
            cell = common[0][0]
            print("      cell %.0f / eps %.5f = %.0f" % (cell, e, cell / e))


def analyse(tag, path):
    a, pos = load(path)
    print("\n=== %s   %d probes" % (tag, len(a)))
    epsilon_report(tag, pos)

    # strip the epsilon, quantise to whole units, build a lookup
    q = np.rint(pos).astype(np.int64)
    base = q.min(axis=0)
    q = q - base
    key = (q[:, 0] << 42) | (q[:, 1] << 21) | q[:, 2]
    table = {int(k): i for i, k in enumerate(key)}

    grp = np.array([a[:, 0:9], a[:, 9:18], a[:, 18:27]]).sum(axis=0)
    mean_term = grp[:, 0]

    def neighbour(shift):
        qq = q + np.array(shift, dtype=np.int64)
        kk = (qq[:, 0] << 42) | (qq[:, 1] << 21) | qq[:, 2]
        idx = np.full(len(a), -1, dtype=np.int64)
        for i, k in enumerate(kk.tolist()):
            j = table.get(int(k), -1)
            idx[i] = j
        return idx

    # try both dominant spacings so adaptive levels are covered
    for spacing in (32, 64):
        gr, ok = [], []
        for ax in range(3):
            sp = [0, 0, 0]; sm = [0, 0, 0]
            sp[ax] = spacing; sm[ax] = -spacing
            ip = neighbour(sp); im = neighbour(sm)
            m = (ip >= 0) & (im >= 0)
            g = np.zeros(len(a))
            g[m] = mean_term[ip[m]] - mean_term[im[m]]
            gr.append(g); ok.append(m)
        n = min(int(o.sum()) for o in ok)
        if n < 200:
            continue
        print("  spacing %d: %d probes with neighbours on all axes" % (spacing, n))
        print("    L1  pos |    d/dx     d/dy     d/dz   -> axis")
        for k in range(1, 4):
            row = []
            for ax in range(3):
                m = ok[ax]
                row.append(np.corrcoef(grp[m, k], gr[ax][m])[0, 1])
            b = int(np.argmax(np.abs(row)))
            print("        %3d | %8.3f %8.3f %8.3f   -> %s%s"
                  % (k, row[0], row[1], row[2], ("-" if row[b] < 0 else "+"), AX[b]))

        # second differences
        def second(ax1, ax2):
            if ax1 == ax2:
                sp = [0, 0, 0]; sm = [0, 0, 0]
                sp[ax1] = spacing; sm[ax1] = -spacing
                ip = neighbour(sp); im = neighbour(sm)
                m = (ip >= 0) & (im >= 0)
                h = np.zeros(len(a))
                h[m] = mean_term[ip[m]] + mean_term[im[m]] - 2 * mean_term[m]
                return h, m
            acc = np.zeros(len(a)); mall = np.ones(len(a), bool)
            for s1 in (1, -1):
                for s2 in (1, -1):
                    s = [0, 0, 0]; s[ax1] = s1 * spacing; s[ax2] = s2 * spacing
                    i = neighbour(s)
                    mm = i >= 0
                    acc[mm] += s1 * s2 * mean_term[i[mm]]
                    mall &= mm
            return acc, mall

        pairs = [(2, 2), (0, 0), (1, 1), (0, 1), (1, 2), (0, 2)]
        names = ["d2/dz2", "d2/dx2", "d2/dy2", "d2/dxdy", "d2/dydz", "d2/dxdz"]
        sec = [second(p, r) for (p, r) in pairs]
        print("    L2  pos | " + " ".join("%9s" % s for s in names))
        for k in range(4, 9):
            row = []
            for (h, m) in sec:
                row.append(np.corrcoef(grp[m, k], h[m])[0, 1] if m.sum() > 200 else 0.0)
            b = int(np.argmax(np.abs(row)))
            print("        %3d | %s   -> %s%s"
                  % (k, " ".join("%9.3f" % v for v in row), ("-" if row[b] < 0 else "+"), names[b]))
        break


if len(sys.argv) > 1:
    MAPS = [(os.path.basename(a).split(".")[0], a) for a in sys.argv[1:]]

for tag, path in MAPS:
    try:
        analyse(tag, path)
    except Exception as e:
        print('=== %s FAILED: %s' % (tag, e))
