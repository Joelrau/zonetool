# Exploratory: find how IW7 stores its asset struct-layout member tables.
#
# IW8 registers layouts through Load_RegisterStructMemberSize(typeName, typeHash, memberType,
# memberTypeHash, memberName, memberNameHash, offset, size, ...). IW7's ship build has the same
# mechanism but no symbols, and a known member table was previously located at 0x1414A2EF0
# (HavokPhysicsShapeList). This dumps that table's raw shape plus every data reference to a set
# of light-grid type-name strings, so the record format can be read off directly.
import idaapi, idc, ida_bytes, ida_segment, ida_name, idautils, ida_nalt
import sys

OUT = idc.ARGV[1] if len(idc.ARGV) > 1 else "structtables.txt"
fh = open(OUT, "w", encoding="utf-8")


def w(s):
    fh.write(s + "\n")


def qw(ea):
    return ida_bytes.get_qword(ea)


def cstr(ea, maxlen=96):
    if ea == 0:
        return None
    b = ida_bytes.get_bytes(ea, maxlen)
    if not b:
        return None
    z = b.find(b"\x00")
    if z <= 0:
        return None
    try:
        s = b[:z].decode("ascii")
    except Exception:
        return None
    if not all(0x20 <= c < 0x7F for c in b[:z]):
        return None
    return s


w("=== raw dump around the known member table at 0x1414A2EF0 ===")
base = 0x1414A2EF0
for i in range(-4, 60):
    ea = base + i * 8
    v = qw(ea)
    s = cstr(v)
    w("  %014X +%3d  %016X  %s" % (ea, i * 8, v, ("-> %r" % s) if s else ""))

# ---------------------------------------------------------------- string scan
TARGETS = [
    "GfxLightGrid", "GfxGpuLightGrid", "GfxGpuLightGridZone", "GfxSHProbeData",
    "GfxProbeData", "GfxVoxelTree", "GfxVoxelTreeHeader", "GfxVoxelInternalNode",
    "GfxVoxelTopDownViewNode", "GfxVoxelLeafNode", "GfxLightGridTree",
    "GfxLightGridColorsHDR", "GfxWorld", "GfxStaticModelDrawInst",
    "GfxGpuLightGridTetrahedron", "probeData", "probes", "probePositions",
    "tetrahedrons", "voxelStartTetrahedron", "gpuVisibleProbes", "fallbackProbeData",
    "numVoxelTetrahedronIndices", "firstTetrahedron",
]
TSET = set(TARGETS)

w("")
w("=== data addresses of light-grid type/member name strings ===")
found = {}
for seg_ea in idautils.Segments():
    seg = ida_segment.getseg(seg_ea)
    name = ida_segment.get_segm_name(seg)
    if name in (".text",):
        continue
    ea = seg.start_ea
    end = seg.end_ea
    # scan for NUL-terminated ascii runs matching a target exactly
    data = ida_bytes.get_bytes(ea, end - ea)
    if not data:
        continue
    for t in TARGETS:
        pat = t.encode() + b"\x00"
        off = 0
        while True:
            off = data.find(pat, off)
            if off < 0:
                break
            addr = ea + off
            # must start right after a NUL or at an aligned boundary, else it is a suffix
            if off == 0 or data[off - 1] == 0:
                found.setdefault(t, []).append(addr)
            off += 1
for t in TARGETS:
    for a in found.get(t, [])[:6]:
        w("  %-30s %014X" % (t, a))
    if not found.get(t):
        w("  %-30s (not found)" % t)

# ---------------------------------------------------------------- xref walk
w("")
w("=== 8-byte-aligned data words pointing at those strings, with neighbours ===")
wanted = set()
for t, addrs in found.items():
    for a in addrs:
        wanted.add(a)

hits = []
for seg_ea in idautils.Segments():
    seg = ida_segment.getseg(seg_ea)
    nm = ida_segment.get_segm_name(seg)
    if nm == ".text":
        continue
    ea, end = seg.start_ea, seg.end_ea
    data = ida_bytes.get_bytes(ea, end - ea)
    if not data:
        continue
    import struct
    n = (end - ea) // 8
    for i in range(n):
        v = struct.unpack_from("<Q", data, i * 8)[0]
        if v in wanted:
            hits.append((ea + i * 8, v))
w("  %d pointer slots reference a target string" % len(hits))
for at, v in hits[:400]:
    s = cstr(v)
    row = []
    for k in range(-2, 8):
        q = qw(at + k * 8)
        cs = cstr(q)
        row.append("%016X%s" % (q, ("(%s)" % cs) if cs else ""))
    w("  %014X -> %-28s | %s" % (at, s, " ".join(row)))

fh.close()
print("[probe] wrote %s" % OUT)
idc.qexit(0)
