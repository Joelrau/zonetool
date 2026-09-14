#!/usr/bin/env python3
"""Ground-truth analysis of IW7 light grid data, from parsed .gfxmap dumps."""
import struct, sys, os, glob
import numpy as np

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import gfxmap

SM = 184          # sizeof GfxStaticModelDrawInst
OFF_ORIGIN, OFF_UNK0, OFF_UNK2, OFF_UNK3 = 0, 140, 144, 146


def f32(b):
    return np.frombuffer(b, dtype="<f4")


def u32(b):
    return np.frombuffer(b, dtype="<u4")


def analyse(path, tag):
    d = gfxmap.parse(path)
    c = d["counts"]
    if not c["probeCount"]:
        print("\n### %-14s no probe volume (ported map) - skipped" % tag)
        return
    print("\n### %s" % tag)

    # ---------------- zones
    z = d["zones"]
    for i in range(c["zoneCount"]):
        nP, fP, nT, fT, fV, nV = struct.unpack_from("<6I", z, i * 88)
        coeffs = np.frombuffer(z[i * 88 + 24: i * 88 + 24 + 58], dtype="<u2")
        print("  zone %d: numProbes=%d firstProbe=%d numTets=%d firstTet=%d "
              "firstVoxTetIdx=%d numVoxTetIdx=%d" % (i, nP, fP, nT, fT, fV, nV))
        print("          zone matches counts? probes=%s tets=%s voxIdx-vs-voxStartCount=%s"
              % (nP == c["probeCount"], nT == c["tetrahedronCount"],
                 nV == c["voxelStartTetrahedronCount"]))

    # ---------------- voxel tree header: cell size and grid extent
    for i, h in enumerate(d["voxelTreeHeaders"]):
        if not h:
            continue
        rd = struct.unpack_from("<4i", h, 0)
        sh = struct.unpack_from("<4i", h, 16)
        bmin = struct.unpack_from("<4f", h, 32)
        bmax = struct.unpack_from("<4f", h, 48)
        leaf = 1 << sh[2] if sh[2] < 31 else -1
        cells = (rd[0] * 16, rd[1] * 16, rd[2] * 16)
        dense = (cells[0] + 1) * (cells[1] + 1) * (cells[2] + 1)
        print("  tree %d: rootNodeDimension=%s nodeCoordBitShift=%s -> leaf %d units"
              % (i, rd[:3], sh[:3], leaf))
        print("          bounds %s .. %s" % (tuple(round(v) for v in bmin[:3]),
                                             tuple(round(v) for v in bmax[:3])))
        print("          box would be %d x %d x %d cells; a DENSE corner grid = %d probes, "
              "actual probeCount = %d  (%.1f%%)"
              % (cells[0], cells[1], cells[2], dense, c["probeCount"],
                 100.0 * c["probeCount"] / dense if dense else 0))

    # ---------------- probe positions: spacing
    pp = f32(d["probePositions"]).reshape(-1, 3)
    for ax in range(3):
        v = np.unique(np.round(pp[:, ax], 4))
        if len(v) > 1:
            dif = np.diff(v)
            print("  probePositions axis %d: %d distinct, min step %.3f, median step %.3f, "
                  "frac part %.5f" % (ax, len(v), dif.min(), np.median(dif),
                                      v[0] - np.floor(v[0])))

    # ---------------- are probes referenced?
    tets = u32(d["tetrahedrons"]).reshape(-1, 4)
    idx = tets & 0x1FFFF
    used = np.unique(idx)
    print("  probes referenced by tetrahedra: %d of %d (%.1f%%)"
          % (len(used), c["probeCount"], 100.0 * len(used) / c["probeCount"]))
    hi = (tets >> 17) & 0x3FFF
    print("  indexFlags bits17-30 nonzero: %d   bit31 set: %.1f%% of corners"
          % (int((hi != 0).sum()), 100.0 * ((tets >> 31) & 1).mean()))

    # ---------------- tetrahedronVisibility
    tv = d["tetrahedronVisibility"]
    if tv:
        a = np.frombuffer(tv, dtype="<u4").reshape(-1, 16)
        allff = (a == 0xFFFFFFFF).all(axis=1)
        print("  tetrahedronVisibility: %d entries (%.1f%% of tets), all-0xFFFFFFFF: %.1f%%, "
              "all-zero: %.1f%%, mean popcount/entry %.1f of 512"
              % (len(a), 100.0 * len(a) / c["tetrahedronCount"], 100.0 * allff.mean(),
                 100.0 * (a == 0).all(axis=1).mean(),
                 float(np.unpackbits(np.frombuffer(tv, dtype=np.uint8)).mean() * 512)))

    # ---------------- voxelStartTetrahedron occupancy
    vs = u32(d["voxelStartTetrahedron"])
    empty = int((vs == 0xFFFFFFFF).sum())
    print("  voxelStartTetrahedron: %d leaves, %d empty (%.1f%%), %d distinct start tets"
          % (len(vs), empty, 100.0 * empty / len(vs), len(np.unique(vs[vs != 0xFFFFFFFF]))))

    # ---------------- per-model gpuVisibleProbes slices
    sd = d["smodelDrawInsts"]
    gp = f32(d["gpuVisibleProbePositions"]).reshape(-1, 3) if d["gpuVisibleProbePositions"] else None
    n = c["smodelCount"]
    if sd and gp is not None and n:
        firsts, counts, layouts, origins = [], [], [], []
        for i in range(n):
            b = i * SM
            u0, u1 = struct.unpack_from("<HH", sd, b + OFF_UNK0)
            counts.append(struct.unpack_from("<H", sd, b + OFF_UNK3)[0])
            layouts.append(struct.unpack_from("<H", sd, b + OFF_UNK2)[0])
            firsts.append(u0 | (u1 << 16))
            origins.append(struct.unpack_from("<3f", sd, b + OFF_ORIGIN))
        firsts = np.array(firsts); counts = np.array(counts)
        layouts = np.array(layouts); origins = np.array(origins)
        print("  smodel slices: sum(unk3)=%d vs gpuVisibleProbesCount=%d  contiguous=%s"
              % (counts.sum(), c["gpuVisibleProbesCount"],
                 bool((np.sort(firsts) == np.concatenate(([0], np.cumsum(counts[np.argsort(firsts)])[:-1]))).all())))
        for lay in sorted(set(layouts.tolist())):
            m = layouts == lay
            print("     unk2=%d : %5d models, unk3 in [%d..%d]" %
                  (lay, int(m.sum()), int(counts[m].min()), int(counts[m].max())))
        # THE question: are the sample points per model distinct, and where are they?
        dup_models = 0
        offs = []
        for i in range(n):
            f, k = firsts[i], counts[i]
            if k < 2 or f + k > len(gp):
                continue
            pts = gp[f:f + k]
            if np.all(pts == pts[0]):
                dup_models += 1
            offs.append(pts - origins[i])
        checked = len(offs)
        if checked:
            allo = np.concatenate(offs)
            print("     models whose sample points are ALL IDENTICAL: %d of %d (%.1f%%)"
                  % (dup_models, checked, 100.0 * dup_models / checked))
            print("     sample offset from model origin: |d| mean %.1f  median %.1f  max %.1f"
                  % (np.linalg.norm(allo, axis=1).mean(),
                     np.median(np.linalg.norm(allo, axis=1)),
                     np.linalg.norm(allo, axis=1).max()))
            print("     per-axis offset median (x,y,z) = (%.2f, %.2f, %.2f)"
                  % tuple(np.median(allo, axis=0)))
            # for the dominant 2-point layout, what separates the pair?
            two = [ (gp[firsts[i]:firsts[i]+2] - origins[i]) for i in range(n)
                    if counts[i] == 2 and firsts[i] + 2 <= len(gp) ]
            if two:
                t = np.array(two)
                delta = t[:, 1] - t[:, 0]
                print("     2-point layout: %d models, pair delta median (%.2f, %.2f, %.2f), "
                      "|delta| median %.2f, identical pairs %.1f%%"
                      % (len(t), *np.median(delta, axis=0),
                         np.median(np.linalg.norm(delta, axis=1)),
                         100.0 * np.all(delta == 0, axis=1).mean()))


paths = sys.argv[1:] or (sorted(glob.glob(r"D:\Games\PC\IW7\dump\*\maps\*\*.gfxmap")) +
                         sorted(glob.glob(r"D:\Games\PC\IW7\zonetool\*\maps\*\*.gfxmap")))
for p in paths:
    tag = os.path.basename(os.path.dirname(os.path.dirname(os.path.dirname(p))))
    try:
        analyse(p, tag)
    except Exception as e:
        import traceback
        print("\n### %s FAILED: %s" % (tag, e))
        traceback.print_exc()
