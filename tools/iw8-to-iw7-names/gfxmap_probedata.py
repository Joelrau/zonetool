#!/usr/bin/env python3
"""Read GfxLightGridProbeData counts straight out of a .gfxmap dump.

The dump begins with a raw copy of the GfxWorld struct, so the light grid can be found by
signature instead of by parsing the whole asset: every authentic IW7 map ships
skyLightGridColors as 672 zero bytes immediately followed by defaultLightGridColors as
(0, 0, 0.21875) repeated 56 times. From the start of defaultLightGridColors:

    GfxLightGrid + 760   defaultLightGridColors   (672 B)
    GfxLightGrid + 1432  tree                     (80 B)
    GfxLightGrid + 1512  probeData                (240 B)

so probeData sits at defaultLightGridColors + 752.
"""
import struct, sys, os, glob

ROW = struct.pack("<fff", 0.0, 0.0, 0.21875)
DEFAULT_COLORS = ROW * 56          # 672 bytes
SKY_COLORS = b"\x00" * 672

FIELDS = [
    ("gpuVisibleProbesCount", 0, "I"),
    ("probeCount", 48, "I"),
    ("zoneCount", 112, "I"),
    ("tetrahedronCount", 128, "I"),
    ("tetrahedronCountVisible", 132, "I"),
    ("voxelStartTetrahedronCount", 208, "I"),
]
PTRS = [
    ("gpuVisibleProbePositions", 8), ("gpuVisibleProbesData", 16),
    ("probes", 56), ("probePositions", 88), ("zones", 120),
    ("tetrahedrons", 136), ("tetrahedronNeighbors", 160),
    ("tetrahedronVisibility", 184), ("voxelStartTetrahedron", 216),
]


def read_probedata(path):
    data = open(path, "rb").read()
    hits = []
    off = 0
    while True:
        off = data.find(DEFAULT_COLORS, off)
        if off < 0:
            break
        if off >= 672 and data[off - 672:off] == SKY_COLORS:
            hits.append(off)
        off += 1
    if not hits:
        return None, None
    pd = hits[0] + 752
    out = {}
    for name, o, fmt in FIELDS:
        out[name] = struct.unpack_from("<" + fmt, data, pd + o)[0]
    out["_ptrs"] = {n: struct.unpack_from("<Q", data, pd + o)[0] for n, o in PTRS}
    return out, len(hits)


paths = sys.argv[1:]
if not paths:
    paths = sorted(glob.glob(r"D:\Games\PC\IW7\dump\*\maps\*\*.gfxmap")) + \
            sorted(glob.glob(r"D:\Games\PC\IW7\zonetool\*\maps\*\*.gfxmap"))

hdr = ("map", "probes", "tets", "tetsVis", "voxStart", "gpuVis", "zones")
print("%-22s %10s %10s %10s %10s %10s %6s  %s" % (hdr + ("null pointers",)))
for p in paths:
    name = os.path.basename(p).replace(".d3dbsp.gfxmap", "")
    tag = os.path.basename(os.path.dirname(os.path.dirname(os.path.dirname(p))))
    pd, n = read_probedata(p)
    if pd is None:
        print("%-22s  (light grid signature not found - not a stock-shaped colour table)"
              % ("%s/%s" % (tag, name))[:22])
        continue
    nulls = [k for k, v in pd["_ptrs"].items() if v == 0]
    print("%-22s %10d %10d %10d %10d %10d %6d  %s"
          % (("%s/%s" % (tag, name))[:22], pd["probeCount"], pd["tetrahedronCount"],
             pd["tetrahedronCountVisible"], pd["voxelStartTetrahedronCount"],
             pd["gpuVisibleProbesCount"], pd["zoneCount"],
             ", ".join(nulls) if nulls else "-"))
