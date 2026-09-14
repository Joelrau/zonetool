"""
Mirror of src/IW7/Common/havok_builder.cpp, used only to validate the *design* of the
writer: emit a small mesh, then read it back with hkpackfile.py + hkcompressedmesh.py,
which are validated against the three shipped stock world blobs.

This is a test harness. The shipping writer is the C++ one.
"""

import struct
import sys
import io

sys.stdout = io.TextIOWrapper(sys.stdout.buffer, encoding="utf-8", errors="replace")

SIG = {
    "hkClass": 0x33D42383, "hkClassMember": 0xB0EFA719,
    "hkClassEnum": 0x8A3609CF, "hkClassEnumItem": 0xCE6F8A6C,
    "HavokPhysicsShapeList": 0xC909A395,
    "hknpCompressedMeshShape": 0x1318CC9F,
    "hknpCompressedMeshShapeData": 0x54FD8D57,
}
PACKED_BITS = (11, 11, 10)
SHARED_BITS = (21, 21, 22)
MAX_VERTS = 255
MAX_PRIMS = 127
KEY_SECTION_SHIFT = 8


class Buf:
    def __init__(self):
        self.b = bytearray()

    def __len__(self):
        return len(self.b)

    def w(self, data):
        self.b += data

    def u8(self, v):  self.w(struct.pack("<B", v))
    def u16(self, v): self.w(struct.pack("<H", v))
    def i32(self, v): self.w(struct.pack("<i", v))
    def u32(self, v): self.w(struct.pack("<I", v))
    def u64(self, v): self.w(struct.pack("<Q", v))
    def f32(self, v): self.w(struct.pack("<f", v))

    def fill(self, n, val=0):
        self.b += bytes([val]) * n

    def reserve(self, n):
        o = len(self.b)
        self.fill(n)
        return o

    def align(self, a, val=0):
        while len(self.b) % a:
            self.b.append(val)

    def hkarray(self, count):
        o = len(self.b)
        self.reserve(8)
        self.i32(count)
        self.u32((count | 0x80000000) & 0xFFFFFFFF)
        return o


def pack_shared(pos, mn, mx):
    packed, shift = 0, 0
    for i in range(3):
        step = (mx[i] - mn[i]) * (2.0 ** -SHARED_BITS[i])
        maxv = (1 << SHARED_BITS[i]) - 1
        raw = 0 if step <= 0 else max(0, min(maxv, int(round((pos[i] - mn[i]) / step))))
        packed |= raw << shift
        shift += SHARED_BITS[i]
    return packed


def pack_vertex(pos, cp):
    packed, shift = 0, 0
    for i in range(3):
        scale = cp[3 + i]
        maxv = (1 << PACKED_BITS[i]) - 1
        raw = 0 if scale <= 0 else int(round((pos[i] - cp[i]) / scale))
        raw = max(0, min(maxv, raw))
        packed |= raw << shift
        shift += PACKED_BITS[i]
    return packed


def split_sections(tris):
    out, cur = [], {"verts": [], "prims": [], "tags": []}

    def finish(s):
        mins = [min(v[i] for v in s["verts"]) for i in range(3)]
        maxs = [max(v[i] for v in s["verts"]) for i in range(3)]
        cp = list(mins) + [
            (maxs[i] - mins[i]) / ((1 << PACKED_BITS[i]) - 1) if maxs[i] > mins[i] else 0.0
            for i in range(3)]
        s["mins"], s["maxs"], s["cp"] = mins, maxs, cp
        return s

    for verts, tag in tris:
        if len(cur["verts"]) + 3 > MAX_VERTS or len(cur["prims"]) >= MAX_PRIMS:
            if cur["prims"]:
                out.append(finish(cur))
            cur = {"verts": [], "prims": [], "tags": []}
        idx = []
        for v in verts:
            if v in cur["verts"]:
                idx.append(cur["verts"].index(v))
            else:
                idx.append(len(cur["verts"]))
                cur["verts"].append(v)
        cur["prims"].append((idx[0], idx[1], idx[2], idx[2]))
        cur["tags"].append(tag)
    if cur["prims"]:
        out.append(finish(cur))
    return out


def tree_top(buf, first_leaf, count, leaf_nodes):
    self_i = len(buf) // 5
    if count <= 1:
        leaf_nodes[first_leaf] = self_i
        buf.fill(3)
        buf.u8((first_leaf >> 8) & 0x7F)
        buf.u8(first_leaf & 0xFF)
        return
    left = count // 2
    enc = (2 * left) // 2
    buf.fill(3)
    buf.u8(0x80 | ((enc >> 8) & 0x7F))
    buf.u8(enc & 0xFF)
    tree_top(buf, first_leaf, left, leaf_nodes)
    tree_top(buf, first_leaf + left, count - left, leaf_nodes)


def tree_section(buf, count, first_leaf=0):
    if count <= 1:
        buf.fill(3)
        # A leaf's data byte is (primitiveIndex << 1); bit 0 clear marks the leaf and the
        # upper 7 bits are the primitive it bounds. Stock always uses a permutation of
        # 0..primitiveCount-1 here.
        buf.u8((first_leaf << 1) & 0xFE)
        return
    left = count // 2
    buf.fill(3)
    buf.u8(((2 * left) & 0xFE) | 1)
    tree_section(buf, left, first_leaf)
    tree_section(buf, count - left, first_leaf + left)


def data_runs(buf, tags):
    runs, i = 0, 0
    while i < len(tags):
        j = i
        while j < len(tags) and tags[j] == tags[i] and (j - i) < 0xFF:
            j += 1
        buf.u16(tags[i]); buf.u8(i); buf.u8(j - i)
        runs += 1
        i = j
    return runs


def build(tris, convex_radius=0.0):
    sections = split_sections(tris)

    palette = []
    for s in sections:
        for k, t in enumerate(s["tags"]):
            if t not in palette:
                palette.append(t)
            s["tags"][k] = palette.index(t)

    n_sec = len(sections)
    max_key = ((n_sec - 1) << KEY_SECTION_SHIFT) | ((len(sections[-1]["prims"]) - 1) << 1) | 1
    bits_per_key = 1
    while (1 << bits_per_key) <= max_key:
        bits_per_key += 1

    total_prims = sum(len(s["prims"]) for s in sections)
    total_verts = sum(len(s["verts"]) for s in sections)
    wmin = [min(s["mins"][i] for s in sections) for i in range(3)]
    wmax = [max(s["maxs"][i] for s in sections) for i in range(3)]

    top = Buf(); leaf_nodes = [0] * n_sec; tree_top(top, 0, n_sec, leaf_nodes)
    sec_nodes = []
    for s in sections:
        b = Buf(); tree_section(b, len(s["prims"]), 0); sec_nodes.append(b)

    prims, shared, svi_buf, runs = Buf(), Buf(), Buf(), Buf()
    first_prim, first_vert, first_run, run_count = [], [], [], []
    for s in sections:
        first_prim.append(len(prims) // 4)
        for p in s["prims"]:
            prims.w(bytes(p))
        first_vert.append(len(shared) // 8)
        for v in s["verts"]:
            shared.u64(pack_shared(v, wmin, wmax))
            svi_buf.u16(len(svi_buf) // 2)
        first_run.append(len(runs) // 4)
        run_count.append(data_runs(runs, s["tags"]))

    # ---- simd tree (4-wide BVH over primitives) ----
    items = []
    for si, sc in enumerate(sections):
        for pi, pr in enumerate(sc["prims"]):
            vs = [sc["verts"][pr[k]] for k in range(3)]
            lo = [min(v[c] for v in vs) for c in range(3)]
            hi = [max(v[c] for v in vs) for c in range(3)]
            items.append(((si << KEY_SECTION_SHIFT) | (pi << 1), lo, hi))

    FMAX = 3.4028234663852886e+38
    simd = [{"lo": [[FMAX]*4 for _ in range(3)], "hi": [[-FMAX]*4 for _ in range(3)],
             "data": [0, 0, 0, 0]}]

    def new_node():
        simd.append({"lo": [[FMAX]*4 for _ in range(3)], "hi": [[-FMAX]*4 for _ in range(3)],
                     "data": [0, 0, 0, 0]})
        return len(simd) - 1

    def build_simd(group):
        self_i = new_node()
        def set_slot(slot, lo, hi, data):
            for c in range(3):
                simd[self_i]["lo"][c][slot] = lo[c]
                simd[self_i]["hi"][c][slot] = hi[c]
            simd[self_i]["data"][slot] = data
        if len(group) <= 4:
            for i, (k, lo, hi) in enumerate(group):
                set_slot(i, lo, hi, (k << 1) | 1)
            return self_i
        lo = [min(g[1][c] for g in group) for c in range(3)]
        hi = [max(g[2][c] for g in group) for c in range(3)]
        axis = max(range(3), key=lambda c: hi[c] - lo[c])
        group.sort(key=lambda g: g[1][axis] + g[2][axis])
        n = len(group)
        for slot in range(4):
            b, e = n * slot // 4, n * (slot + 1) // 4
            if b >= e:
                continue
            sub = group[b:e]
            slo = [min(g[1][c] for g in sub) for c in range(3)]
            shi = [max(g[2][c] for g in sub) for c in range(3)]
            if len(sub) == 1:
                set_slot(slot, slo, shi, (sub[0][0] << 1) | 1)
            else:
                set_slot(slot, slo, shi, build_simd(sub) << 1)
        return self_i

    build_simd(items)

    buf = Buf()
    local, glob, virt = [], [], []
    A = lambda: buf.align(16)

    sl = len(buf)
    fields = []
    for c in (1, 1, 1, 1, 1, 2):
        fields.append(buf.hkarray(c))
    buf.i32(0); buf.reserve(4)            # numWorldGeoShapes
    for c in (len(palette), 1, 1):
        fields.append(buf.hkarray(c))
    assert len(buf) - sl == 152, len(buf) - sl
    A()

    local.append((fields[0], len(buf))); shape_slot = buf.reserve(8); A()
    local.append((fields[1], len(buf))); buf.i32(0); A()
    local.append((fields[2], len(buf))); name_slot = buf.reserve(8); A()
    name_at = len(buf); buf.w(b"World Entity 0\0"); A()
    local.append((name_slot, name_at))
    local.append((fields[3], len(buf))); buf.i32(total_verts); A()
    local.append((fields[4], len(buf))); buf.i32(total_prims); A()
    local.append((fields[5], len(buf)))
    for v in wmin: buf.f32(v)
    buf.f32(0.0)
    for v in wmax: buf.f32(v)
    buf.f32(0.0); A()
    local.append((fields[6], len(buf)))
    for t in palette:
        buf.u32(0x800); buf.i32(t); buf.u16(t); buf.reserve(6); buf.u64(0)
    A()
    local.append((fields[7], len(buf))); buf.u32(0x28033ED1); A()
    local.append((fields[8], len(buf))); buf.i32(0); A()

    shape_at = len(buf)
    virt.append((sl, "HavokPhysicsShapeList"))
    virt.append((shape_at, "hknpCompressedMeshShape"))
    glob.append((shape_slot, 2, shape_at))

    buf.reserve(16)
    buf.u16(4); buf.u8(bits_per_key); buf.u8(2); buf.f32(convex_radius)
    buf.u64(0); buf.reserve(8); buf.reserve(8)     # properties* + hknpShape tail padding
    buf.u32(0xFFFFFFFF); buf.u32(0)
    buf.hkarray(0); buf.hkarray(0)
    buf.u32(0xFFFFFFFF); buf.reserve(4)
    data_slot = buf.reserve(8)
    ibits = max_key + 1
    qbits = ibits // 2
    iwords = (ibits + 31) // 32
    qwords = (qbits + 31) // 32
    qf = buf.hkarray(qwords); buf.i32(qbits); buf.reserve(4)
    tf = buf.hkarray(iwords); buf.i32(ibits); buf.reserve(4)
    buf.i32(0); buf.i32(0)
    assert len(buf) - shape_at == 160, len(buf) - shape_at
    A()
    local.append((qf, len(buf))); buf.fill(qwords * 4); A()
    local.append((tf, len(buf))); buf.fill(iwords * 4); A()

    data_at = len(buf)
    virt.append((data_at, "hknpCompressedMeshShapeData"))
    glob.append((data_slot, 2, data_at))
    buf.reserve(16)
    tree_at = len(buf)
    f_nodes = buf.hkarray(len(top) // 5)
    for v in wmin: buf.f32(v)
    buf.f32(0.0)
    for v in wmax: buf.f32(v)
    buf.f32(0.0)
    buf.i32(total_prims); buf.i32(bits_per_key); buf.u32(max_key); buf.reserve(4)
    f_sec = buf.hkarray(n_sec)
    f_prim = buf.hkarray(total_prims)
    f_svi = buf.hkarray(len(svi_buf) // 2)
    buf.hkarray(0)
    f_shared = buf.hkarray(len(shared) // 8)
    f_run = buf.hkarray(len(runs) // 4)
    assert len(buf) - tree_at == 160, len(buf) - tree_at
    buf.reserve(8); f_simd = buf.hkarray(len(simd))
    buf.hkarray(0); buf.hkarray(0); buf.hkarray(0)
    buf.reserve(8)
    assert len(buf) - data_at == 256, len(buf) - data_at
    A()

    local.append((f_simd, len(buf)))
    for nd in simd:
        for c in range(3):
            for v in nd["lo"][c]: buf.f32(v)
            for v in nd["hi"][c]: buf.f32(v)
        for v in nd["data"]: buf.u32(v)
    A()
    local.append((f_nodes, len(buf))); buf.w(bytes(top.b)); A()
    local.append((f_sec, len(buf)))
    f_secnodes = []
    for i, s in enumerate(sections):
        base = len(buf)
        f_secnodes.append(buf.hkarray(len(sec_nodes[i]) // 4))
        for v in s["mins"]: buf.f32(v)
        buf.f32(0.0)
        for v in s["maxs"]: buf.f32(v)
        buf.f32(0.0)
        for _ in range(3): buf.f32(3.4028234663852886e+38)
        for _ in range(3): buf.f32(float("-inf"))
        buf.u32(0)
        buf.u32(first_vert[i] << 8)
        buf.u32((first_prim[i] << 8) | len(s["prims"]))
        buf.u32((first_run[i] << 8) | run_count[i])
        buf.u8(0); buf.u8(len(s["verts"])); buf.u16(leaf_nodes[i])
        buf.u8(0); buf.u8(0); buf.u8(0); buf.u8(0)
        assert len(buf) - base == 96, len(buf) - base
    A()
    for i in range(len(sections)):
        local.append((f_secnodes[i], len(buf))); buf.w(bytes(sec_nodes[i].b)); A()
    local.append((f_prim, len(buf))); buf.w(bytes(prims.b)); A()
    local.append((f_svi, len(buf))); buf.w(bytes(svi_buf.b)); A()
    local.append((f_shared, len(buf))); buf.w(bytes(shared.b)); A()
    local.append((f_run, len(buf))); buf.w(bytes(runs.b)); A()
    data_size = len(buf)

    names = Buf()
    name_off = {}
    for n in ("hkClass", "hkClassMember", "hkClassEnum", "hkClassEnumItem",
              "HavokPhysicsShapeList", "hknpCompressedMeshShape",
              "hknpCompressedMeshShapeData"):
        names.u32(SIG[n]); names.u8(0x09)
        name_off[n] = len(names)
        names.w(n.encode() + b"\0")
    names.align(16, 0xFF)

    fx = Buf()
    for s, d in sorted(local):
        fx.i32(s); fx.i32(d)
    lsz = len(fx)
    for s, si, d in sorted(glob):
        fx.i32(s); fx.i32(si); fx.i32(d)
    gsz = len(fx) - lsz
    for o, n in virt:
        fx.i32(o); fx.i32(0); fx.i32(name_off[n])
    vsz = len(fx) - lsz - gsz

    f = Buf()
    f.u32(0x57E0E057); f.u32(0x10C0C010); f.i32(0); f.i32(11)
    f.u8(8); f.u8(1); f.u8(0); f.u8(1)
    f.i32(3); f.i32(2); f.i32(0); f.i32(0); f.i32(name_off["HavokPhysicsShapeList"])
    ver = b"hk_2014.2.5-r1\0"
    f.w(ver); f.fill(16 - len(ver), 0xFF)
    f.i32(0); f.u16(21); f.u16(0)

    names_start = 64 + 3 * 64
    data_start = names_start + len(names)

    def sec_hdr(tag, abs_, payload, l, g, v):
        nm = tag.encode()[:19]
        f.w(nm + b"\0" * (19 - len(nm)))
        f.u8(0xFF)
        f.i32(abs_); f.i32(payload); f.i32(payload + l); f.i32(payload + l + g)
        end = payload + l + g + v
        f.i32(end); f.i32(end); f.i32(end)
        f.fill(16, 0xFF)

    sec_hdr("__classnames__", names_start, len(names), 0, 0, 0)
    sec_hdr("__types__", data_start, 0, 0, 0, 0)
    sec_hdr("__data__", data_start, data_size, lsz, gsz, vsz)
    f.w(bytes(names.b)); f.w(bytes(buf.b)); f.w(bytes(fx.b))
    return bytes(f.b)


def cube(scale=64.0, tag=7):
    c = [(x * scale, y * scale, z * scale)
         for x in (0, 1) for y in (0, 1) for z in (0, 1)]
    faces = [(0,1,3,2),(4,6,7,5),(0,4,5,1),(2,3,7,6),(0,2,6,4),(1,5,7,3)]
    out = []
    for a, b, d, e in faces:
        out.append(((c[a], c[b], c[d]), tag))
        out.append(((c[a], c[d], c[e]), tag))
    return out


if __name__ == "__main__":
    blob = build(cube())
    open("selftest.hkx", "wb").write(blob)
    print("wrote selftest.hkx, %d bytes" % len(blob))

    from hkpackfile import Packfile
    from hkcompressedmesh import load_compressed_meshes

    pf = Packfile.parse(blob)
    print("root:", pf.root_class_name())
    print("objects:", [(o, n) for _s, o, n in pf.objects()])
    print("classnames:", [n for _o, _s, n in pf.class_names()])
    assert pf.build() == blob, "container round-trip failed"
    print("container round-trip: OK")

    for shape, mt, dm in load_compressed_meshes("selftest.hkx"):
        print("sections=%d primitives=%d triangles=%d keys=%d verts=%d" % (
            mt.sections.size, mt.primitives.size, len(dm.triangles),
            mt.num_primitive_keys, len(dm.vertices)))
        bmin, bmax = dm.bounds()
        print("bounds min=(%.2f %.2f %.2f) max=(%.2f %.2f %.2f)" % (*bmin, *bmax))
