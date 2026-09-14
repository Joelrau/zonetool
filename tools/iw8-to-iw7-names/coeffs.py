#!/usr/bin/env python3
"""Re-derive the GfxSHProbeData coefficient layout from shipped probe arrays.

64 bytes per probe = 32 float16 slots. This reports, per slot, what the shipped data
actually contains, so the boundary between "SH coefficient", "extra term" and "padding"
is measured rather than assumed.
"""
import sys, os, glob
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


def halfs(buf):
    return np.frombuffer(buf, dtype="<f2").reshape(-1, 32).astype(np.float64)


def slot_table(tag, a, label):
    print("\n--- %s : %s (%d entries) ---" % (tag, label, len(a)))
    print("slot  zero%   min        p50        max        distinct  note")
    for k in range(32):
        col = a[:, k]
        zero = 100.0 * (col == 0).mean()
        dist = len(np.unique(col))
        note = ""
        if zero == 100.0:
            note = "ALWAYS ZERO"
        elif dist == 1:
            note = "CONSTANT %.6g" % col[0]
        elif (col >= 0).all():
            note = "non-negative"
        print("%4d %6.1f  %10.4g %10.4g %10.4g %9d  %s"
              % (k, zero, col.min(), np.median(col), col.max(), dist, note))


for tag, path in MAPS:
    try:
        d = gfxmap.parse(path, stop_after="probeData")
    except Exception as e:
        print("%s: parse failed: %s" % (tag, e))
        continue
    c = d["counts"]
    if not c["probeCount"]:
        continue

    probes = halfs(d["probes"])
    slot_table(tag, probes, "probes")

    z = d["zones"]
    fb = np.frombuffer(z[24:24 + 64], dtype="<f2").astype(np.float64)
    print("  zone fallbackProbeData 32 slots:")
    print("   ", " ".join("%.4g" % v for v in fb[:16]))
    print("   ", " ".join("%.4g" % v for v in fb[16:]))

    gv = halfs(d["gpuVisibleProbesData"])[: c["gpuVisibleProbesCount"]]
    if len(gv):
        nz = [k for k in range(32) if (gv[:, k] != 0).any()]
        print("  gpuVisibleProbesData: %d authored entries, slots with any nonzero: %s"
              % (len(gv), nz))
        tail = halfs(d["gpuVisibleProbesData"])[c["gpuVisibleProbesCount"]:]
        print("  gpuVisibleProbesData 0x2000 tail all zero: %s" % bool((tail == 0).all()))
