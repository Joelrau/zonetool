"""
hkxtool -- inspect, validate, compare and export IW7 Havok collision blobs.

IW7 stores world collision as a Havok 2014.2.5-r1 binary packfile (.hkx) hanging off
clipMap_t::havokWorldShapeData, and the existing IW7 dumper writes it out beside the
.colmap. This reads those files -- shipped or generated -- so a generated blob can be
checked against the shipped ones.

    python hkxtool.py info     <file.hkx> [...]      header, sections, objects, mesh summary
    python hkxtool.py check    <file.hkx> [...]      validate every invariant taken from stock
    python hkxtool.py diff     <a.hkx> <b.hkx>       field-by-field comparison
    python hkxtool.py geom     <file.hkx> [-o o.obj] geometry stats, optional Wavefront OBJ
    python hkxtool.py survey   <glob>                one line per file

`check` is the important one. Every rule it applies was derived from and verified against
the six shipped stock maps on hand -- mp_frontend, mp_afghan, mp_paris, mp_breakneck,
cp_zmb and cp_rave -- and all twelve of their world and ents blobs pass.

It is a structural checker, not a proof of correctness. It validates the container, the
array shapes, both BVH levels, section/vertex/primitive consistency and the shared-vertex
paging, but it still reads almost nothing of the shapeTagData palette's CONTENT beyond
bounds-checking the runs that index it. Treat a clean run as "not obviously malformed",
not as "the collision is right".

Format notes live in docs/iw7-havok-collision.md.
"""

import argparse
import glob as globmod
import io
import os
import collections
import struct
import sys

from hkpackfile import Packfile
from hkcompressedmesh import (read_shape, read_mesh_tree, read_section, read_hkarray,
                              unpack_section_field, decode_mesh, decode_packed_vertex,
                              decode_shared_vertex, SHARED_VERTEX_PAGE_SIZE,
                              decode_section_tree,
                              u8, u16, u32, i32, u64)

sys.stdout = io.TextIOWrapper(sys.stdout.buffer, encoding="utf-8", errors="replace")

KEY_SECTION_SHIFT = 8
DEAD = bytes((0xDE, 0xAD, 0xDE, 0xAD))


# --------------------------------------------------------------------------- helpers

def load(path):
    """-> (packfile, data-section bytes, fixup map)"""
    pf = Packfile.load(path)
    sec = pf.sections[2]
    fix = dict(pf.local_fixups(sec))
    fix.update({s: dst for s, _si, dst in pf.global_fixups(sec)})
    return pf, sec.data, fix


def meshes(pf, d, fix):
    """Yield (shapeOffset, dataOffset, shape, meshTree) for each compressed mesh."""
    shapes = [o for _s, o, n in pf.objects() if n == "hknpCompressedMeshShape"]
    datas = [o for _s, o, n in pf.objects() if n == "hknpCompressedMeshShapeData"]
    for so, do in zip(shapes, datas):
        yield so, do, read_shape(d, so), read_mesh_tree(d, do + 16, fix)


def shape_list(pf, d, fix):
    off = [o for _s, o, n in pf.objects() if n == "HavokPhysicsShapeList"]
    if not off:
        return None
    o = off[0]
    names = ["shapes", "shapeIndices", "shapeNames", "vertCounts", "triCounts",
             "minMaxes", None, "shapeTagData", "shapeContents", "convexCounts"]
    offs = [0, 16, 32, 48, 64, 80, None, 104, 120, 136]
    out = {"numWorldGeoShapes": i32(d, o + 96), "_offset": o}
    for n, fo in zip(names, offs):
        if n:
            out[n] = read_hkarray(d, o + fo, fix)
    return out


def top_tree_leaves(d, mt):
    """-> {sectionIndex: nodeIndex}, visitedNodeCount, badRefs"""
    N = mt.nodes.size
    leaves, visited, bad, stack = {}, set(), 0, [0]
    while stack:
        n = stack.pop()
        if n < 0 or n >= N:
            bad += 1
            continue
        if n in visited:
            continue
        visited.add(n)
        b = d[mt.nodes.data + n * 5: mt.nodes.data + n * 5 + 5]
        hi, lo = b[3], b[4]
        if hi & 0x80:
            off = 2 * (((hi & 0x7F) << 8) | lo)
            stack.append(n + 1)
            stack.append(n + off)
        else:
            leaves[(hi << 8) | lo] = n
    return leaves, len(visited), bad


def simd_leaves(d, mt, simd):
    """-> (leafKeys, visitedNodes, badRefs)"""
    leaves, visited, bad, stack = [], set(), 0, [1]
    while stack:
        n = stack.pop()
        if n < 0 or n >= simd.size:
            bad += 1
            continue
        if n in visited:
            continue
        visited.add(n)
        base = simd.data + n * 112
        ab = [struct.unpack_from("<4f", d, base + i * 16) for i in range(6)]
        dat = struct.unpack_from("<4I", d, base + 96)
        for c in range(4):
            if ab[0][c] > ab[1][c]:      # inverted AABB = unused slot
                continue
            v = dat[c]
            if v & 1:
                leaves.append(v >> 1)
            else:
                stack.append(v >> 1)
    return leaves, len(visited), bad


def dead_primitives(d, mt):
    return sum(1 for i in range(mt.primitives.size)
               if d[mt.primitives.data + i * 4: mt.primitives.data + i * 4 + 4] == DEAD)


# ----------------------------------------------------------------------------- info

def cmd_info(args):
    for path in expand(args.paths):
        pf, d, fix = load(path)
        print("=" * 78)
        print(path)
        print("  fileVersion %d   contentsVersion %s   layout ptr=%d LE=%d rbcp=%d ebco=%d"
              % (pf.file_version, pf.contents_version.decode(), pf.layout.pointer_size,
                 pf.layout.little_endian, pf.layout.reuse_base_class_padding,
                 pf.layout.empty_base_class_optimization))
        print("  root class  %s" % pf.root_class_name())
        for i, s in enumerate(pf.sections):
            print("  section %d %-16s absStart=%-9d data=%-9d end=%d"
                  % (i, s.tag, s.absolute_data_start, s.data_size, s.end_offset))
        print("  classnames: %s" % ", ".join(n for _o, _s, n in pf.class_names()))
        counts = {}
        for _si, _o, n in pf.objects():
            counts[n] = counts.get(n, 0) + 1
        print("  objects: %s" % counts)

        sl = shape_list(pf, d, fix)
        if sl:
            print("  HavokPhysicsShapeList: numWorldGeoShapes=%d" % sl["numWorldGeoShapes"])
            for k in ("shapes", "shapeIndices", "shapeNames", "vertCounts", "triCounts",
                      "minMaxes", "shapeTagData", "shapeContents", "convexCounts"):
                print("      %-16s size=%d" % (k, sl[k].size))
            if sl["shapeContents"].size:
                print("      shapeContents[0] = 0x%08X" % u32(d, sl["shapeContents"].data))

        for so, do, shape, mt in meshes(pf, d, fix):
            simd = read_hkarray(d, do + 176 + 8, fix)
            print("  hknpCompressedMeshShape @%d" % so)
            print("      flags=0x%X numShapeKeyBits=%d dispatchType=%d convexRadius=%.4f"
                  % (shape.flags, shape.num_shape_key_bits, shape.dispatch_type,
                     shape.convex_radius))
            print("      shapeTagCodecInfo=0x%08X" % shape.shape_tag_codec_info)
            print("      sections=%d primitives=%d packedVerts=%d sharedVerts=%d dataRuns=%d"
                  % (mt.sections.size, mt.primitives.size, mt.packed_vertices.size,
                     mt.shared_vertices.size, mt.primitive_data_runs.size))
            print("      bitsPerKey=%d maxKeyValue=%d numPrimitiveKeys=%d simdNodes=%d"
                  % (mt.bits_per_key, mt.max_key_value, mt.num_primitive_keys, simd.size))
            print("      domain (Havok units; x32 for CoD units) (%.1f %.1f %.1f) .. (%.1f %.1f %.1f)"
                  % (mt.domain[0], mt.domain[1], mt.domain[2],
                     mt.domain[4], mt.domain[5], mt.domain[6]))


# ---------------------------------------------------------------------------- check

def section_primitive_aabbs(d, mt, s, fixups):
    """AABB of each primitive in a section, in section-local primitive order.
    None for 0xDEADDEAD padding and for custom primitives, which are not geometry."""
    from hkcompressedmesh import (decode_packed_vertex, decode_shared_vertex,
                                  SHARED_VERTEX_PAGE_SIZE, SIZEOF_PRIMITIVE, u16, u32, u64)
    prim_off, prim_count = unpack_section_field(s.primitives_raw)
    first_shared = s.shared_vertices_raw >> 8
    page_base = SHARED_VERTEX_PAGE_SIZE * s.page
    cache = {}

    def fetch(vi):
        if vi in cache:
            return cache[vi]
        if vi < s.num_packed_vertices:
            pos = decode_packed_vertex(
                u32(d, mt.packed_vertices.data + (s.first_packed_vertex + vi) * 4),
                s.codec_parms)
        else:
            j = first_shared + vi - s.num_packed_vertices
            if j >= mt.shared_vertices_index.size:
                raise NotImplementedError("shared index out of range")
            k = page_base + u16(d, mt.shared_vertices_index.data + j * 2)
            if k >= mt.shared_vertices.size:
                raise NotImplementedError("shared vertex out of range")
            pos = decode_shared_vertex(u64(d, mt.shared_vertices.data + k * 8), mt.domain)
        cache[vi] = pos
        return pos

    out = []
    for pi in range(prim_count):
        o = mt.primitives.data + (prim_off + pi) * SIZEOF_PRIMITIVE
        idx = [d[o], d[o + 1], d[o + 2], d[o + 3]]
        if idx == [0xDE, 0xAD, 0xDE, 0xAD] or idx[1] == idx[2] == idx[3]:
            out.append(None)
            continue
        pts = [fetch(i) for i in dict.fromkeys(idx)]
        out.append((tuple(min(p[k] for p in pts) for k in range(3)),
                    tuple(max(p[k] for p in pts) for k in range(3))))
    return out


def cmd_check(args):
    worst = 0
    for path in expand(args.paths):
        fails = []

        def want(cond, msg):
            if not cond:
                fails.append(msg)

        raw = open(path, "rb").read()
        pf = Packfile.parse(raw)
        d = pf.sections[2].data
        fix = dict(pf.local_fixups(pf.sections[2]))
        fix.update({s: dst for s, _si, dst in pf.global_fixups(pf.sections[2])})

        want(pf.build() == raw, "container does not round-trip byte-exact")
        want(pf.contents_version == b"hk_2014.2.5-r1",
             "contentsVersion is %r" % pf.contents_version)
        want((pf.layout.pointer_size, pf.layout.little_endian,
              pf.layout.reuse_base_class_padding,
              pf.layout.empty_base_class_optimization) == (8, 1, 0, 1),
             "layoutRules are not (8,1,0,1)")
        want([s.tag for s in pf.sections] == ["__classnames__", "__types__", "__data__"],
             "unexpected section layout")

        sl = shape_list(pf, d, fix)
        if sl is None:
            want(False, "no HavokPhysicsShapeList (not a world/ents blob?)")
        else:
            want(sl["numWorldGeoShapes"] == 0,
                 "numWorldGeoShapes is %d, every shipped blob uses 0" % sl["numWorldGeoShapes"])
            if sl["shapeContents"].size:
                c = u32(d, sl["shapeContents"].data)
                want(c != 0xFFFFFFFF, "shapeContents is 0xFFFFFFFF (not a real contents mask)")
            # A world shape carries a FIXED contents mask in every shipped blob:
            # 0x29033ED1 / 0x28033ED7 / 0x28033ED1. The top bits 0x28000000 are always
            # present. Deriving this mask from the source geometry instead produced
            # 0x08031E41, which loses 0x20000000 -- the world body then did not collide with
            # the player even though bullet traces still hit it.
            #
            # This used to be gated on "triCounts > 0 and convexCounts == 0", which was wrong
            # in both directions. Every shipped world blob has a non-zero convexCounts (it
            # counts the mesh's custom primitives, not separate convex shapes), so the rule
            # never ran on the data it was derived from; and in an ents blob the per-brush-
            # model mesh shapes DO match that gate, so it false-failed stock ents blobs --
            # 4 on mp_afghan, 24 on cp_zmb.
            #
            # A world blob is discriminated by its object table instead: exactly one
            # compressed mesh and no compound shapes. Ents blobs always carry compounds
            # (or, when empty, no shapes at all).
            classes = [n for _s, _o, n in pf.objects()]
            is_world_blob = (classes.count("hknpCompressedMeshShape") == 1
                             and "hknpDynamicCompoundShape" not in classes
                             and sl["shapes"].size == 1)
            if is_world_blob and sl["shapeContents"].size:
                WORLD_CONTENTS_REQUIRED = 0x28000000
                sc = u32(d, sl["shapeContents"].data)
                want((sc & WORLD_CONTENTS_REQUIRED) == WORLD_CONTENTS_REQUIRED,
                     "world shape has shapeContents 0x%08X, missing 0x%08X from the shipped "
                     "world mask -- the world body will not collide with the player"
                     % (sc, WORLD_CONTENTS_REQUIRED & ~sc))

            # Every non-empty root array must have a real data pointer. A writer that forgets
            # to record an array's offset leaves it pointing at 0 (or at another array's
            # data), and the runtime then reads zeros -- which is how shapeContents silently
            # became 0x00000000 and the world stopped colliding.
            for k in ("shapes", "shapeIndices", "shapeNames", "vertCounts", "triCounts",
                      "minMaxes", "shapeTagData", "shapeContents", "convexCounts"):
                a = sl[k]
                if a.size and not a.data:
                    want(False, "%s has %d entries but a null data pointer -- its offset was "
                                "never recorded" % (k, a.size))

            n = sl["shapes"].size
            for k in ("shapeIndices", "shapeNames", "shapeContents"):
                want(sl[k].size == n, "%s has %d entries, shapes has %d" % (k, sl[k].size, n))

            # The per-shape STATISTICS arrays are not always one-per-shape in stock: cp_zmb
            # ships 216 shapes with 215 vertCounts/triCounts/convexCounts and 430 minMaxes,
            # and cp_rave 132 with 131 and 262. The other four maps are exact. Why the last
            # shape is omitted is not understood, so accept n or n-1 rather than either
            # false-failing shipped data or dropping the check entirely.
            for k in ("vertCounts", "triCounts", "convexCounts"):
                want(sl[k].size in (n, max(0, n - 1)),
                     "%s has %d entries, shapes has %d (stock uses n or n-1)"
                     % (k, sl[k].size, n))
            want(sl["minMaxes"].size in (2 * n, max(0, 2 * (n - 1))),
                 "minMaxes has %d entries, expected 2 per shape (%d) or 2 per shape-1 (%d)"
                 % (sl["minMaxes"].size, 2 * n, max(0, 2 * (n - 1))))

        for so, do, shape, mt in meshes(pf, d, fix):
            S, N = mt.sections.size, mt.nodes.size
            want(N == 2 * S - 1, "top tree has %d nodes, expected 2*%d-1" % (N, S))

            leaves, visited, bad = top_tree_leaves(d, mt)
            want(bad == 0, "top tree has %d out-of-range child refs" % bad)
            want(visited == N, "top tree traversal reaches %d of %d nodes" % (visited, N))
            want(sorted(leaves) == list(range(S)),
                 "top tree leaves are not a permutation of the %d sections" % S)

            tot_p = 0
            for si in range(S):
                s = read_section(d, mt.sections.data, si, fix)
                _po, pc = unpack_section_field(s.primitives_raw)
                tot_p += pc
                want(pc <= 127,
                     "section %d has %d primitives; the 8-bit key packing allows 127" % (si, pc))
                want(s.num_packed_vertices <= 255,
                     "section %d has %d packed vertices (uint8 field)" % (si, s.num_packed_vertices))
                want(s.nodes.size == max(1, 2 * pc - 1),
                     "section %d has %d nodes, expected 2*%d-1" % (si, s.nodes.size, pc))
                if si in leaves:
                    want(s.leaf_index == leaves[si],
                         "section %d leafIndex=%d but its top-tree leaf is node %d"
                         % (si, s.leaf_index, leaves[si]))

                # Shared-vertex paging. The runtime resolves a shared vertex as
                #   sharedVertices[0x10000 * page + sharedVerticesIndex[first + v]]
                # so every index a section can produce has to land inside the pool. A
                # writer that leaves page at 0 while emitting more than 65,536 vertices
                # wraps its uint16 indices back to the start of the pool -- the geometry
                # is still "valid" and every other check here passes, so nothing caught
                # it. These three rules do.
                first = s.shared_vertices_raw >> 8
                n_idx = s.num_shared_indices
                npv = s.num_packed_vertices
                if n_idx and mt.shared_vertices_index.size:
                    want(first + n_idx <= mt.shared_vertices_index.size,
                         "section %d reads sharedVerticesIndex[%d..%d], array holds %d"
                         % (si, first, first + n_idx - 1, mt.shared_vertices_index.size))

                    # Only follow slots a real triangle actually uses as a vertex. A
                    # sharedVerticesIndex slot can also hold a custom-primitive shape-type
                    # descriptor, which is not an index into anything: mp_frontend's
                    # svi[0] is 2050, and reading it as a vertex index points far past a
                    # 307-entry pool.
                    po, pc = unpack_section_field(s.primitives_raw)
                    worst = None
                    for pi in range(pc):
                        off = mt.primitives.data + (po + pi) * 4
                        prim = d[off:off + 4]
                        if prim == DEAD or (prim[1] == prim[2] == prim[3]):
                            continue  # padding, or a custom primitive
                        for v in prim:
                            if v >= npv:
                                slot = first + v - npv
                                if slot >= mt.shared_vertices_index.size:
                                    continue
                                val = u16(d, mt.shared_vertices_index.data + 2 * slot)
                                if worst is None or val > worst:
                                    worst = val
                    if worst is not None:
                        want(s.page * 0x10000 + worst < mt.shared_vertices.size,
                             "section %d page %d index %d resolves to sharedVertices[%d], "
                             "pool holds %d -- vertex indices have wrapped"
                             % (si, s.page, worst, s.page * 0x10000 + worst,
                                mt.shared_vertices.size))
            want(tot_p == mt.primitives.size,
                 "section primitive counts sum to %d, array holds %d"
                 % (tot_p, mt.primitives.size))

            # primitiveDataRuns is {u16 value; u8 index; u8 count} and `value` indexes
            # shapeTagData -- the per-surface collisionFilterInfo / materialCRC / userData
            # palette. Nothing else here reads a byte of that palette, so an out-of-range
            # run (or a palette built with the wrong number of entries) used to sail
            # through. Physics_AddShapeList resolves the tag by index at load, so a run
            # pointing past the palette reads whatever follows it.
            if sl is not None and mt.primitive_data_runs.size:
                tag_n = sl["shapeTagData"].size
                bad_runs = 0
                for ri in range(mt.primitive_data_runs.size):
                    val = u16(d, mt.primitive_data_runs.data + 4 * ri)
                    if val >= tag_n:
                        bad_runs += 1
                want(bad_runs == 0,
                     "%d of %d primitiveDataRuns index past the %d-entry shapeTagData "
                     "palette" % (bad_runs, mt.primitive_data_runs.size, tag_n))

            # A pool past one page is unreachable unless some section says so. Stock pages
            # every world blob it ships: mp_afghan uses 3, mp_paris and cp_zmb 5.
            if mt.shared_vertices.size > 0x10000:
                pages = set()
                for si in range(S):
                    pages.add(read_section(d, mt.sections.data, si, fix).page)
                want(len(pages) > 1 or max(pages) > 0,
                     "sharedVertices holds %d entries but every section is on page 0, so "
                     "only the first 65536 are reachable -- indices past that have wrapped"
                     % mt.shared_vertices.size)

            # Every per-section BVH leaf box must contain its primitive. The tolerance is
            # 1% of the section extent with a 0.05-unit floor: the encoder built these
            # boxes from the original float geometry, while the vertices were separately
            # quantised, so a sub-percent disagreement is expected and is not an error.
            # The floor matters for very small sections, where 0.01 units is already
            # several percent. Across every leaf of every stock world blob the largest
            # absolute overshoot is 0.03 units. See the Aabb4BytesCodec note in
            # hkcompressedmesh.py.
            # A section leaf's data byte is (primitiveIndex << 1). Stock always uses each
            # primitive exactly once: in all 5,302 multi-primitive sections of the three
            # shipped world blobs the leaf indices are a permutation of 0..pc-1, never
            # duplicated. A writer that emits a bare 0 for every leaf produces a tree that
            # bounds the right boxes but names the wrong geometry, which this catches.
            badperm = 0
            for si in range(S):
                s = read_section(d, mt.sections.data, si, fix)
                _po, pc = unpack_section_field(s.primitives_raw)
                if pc < 2:
                    continue
                idx = [d[s.nodes.data + n * 4 + 3] >> 1 for n in range(s.nodes.size)
                       if not (d[s.nodes.data + n * 4 + 3] & 1)]
                if sorted(idx) != list(range(pc)):
                    badperm += 1
            want(badperm == 0,
                 "%d sections whose BVH leaf primitive indices are not a permutation of "
                 "0..primitiveCount-1" % badperm)

            # Structural invariants that hold exactly in stock and that a writer can
            # plausibly break. Section domains never escape the tree domain (0 violations
            # across all three shipped world blobs), and each section's domain bounds its
            # own vertices -- the latter only to within quantisation noise, since the
            # encoder computed it from float geometry before the vertices were quantised
            # (worst observed in stock: 0.0022 units).
            escaped = unbounded = 0
            tmn, tmx = mt.domain[0:3], mt.domain[4:7]
            for si in range(S):
                s = read_section(d, mt.sections.data, si, fix)
                if any(s.domain[k] < tmn[k] - 1e-2 or s.domain[4 + k] > tmx[k] + 1e-2
                       for k in range(3)):
                    escaped += 1
                try:
                    prims = [p for p in section_primitive_aabbs(d, mt, s, fix) if p]
                except NotImplementedError:
                    continue
                if not prims:
                    continue
                for k in range(3):
                    lo = min(p[0][k] for p in prims)
                    hi = max(p[1][k] for p in prims)
                    if lo < s.domain[k] - 0.05 or hi > s.domain[4 + k] + 0.05:
                        unbounded += 1
                        break
            want(escaped == 0,
                 "%d section domains fall outside the tree domain" % escaped)
            want(unbounded == 0,
                 "%d section domains do not bound their own vertices" % unbounded)

            # Data runs are RLE over the section's primitives: they start at index 0, tile
            # contiguously, and cover exactly primitiveCount. Exact in all stock sections.
            badruns = 0
            for si in range(S):
                s = read_section(d, mt.sections.data, si, fix)
                roff, rcnt = unpack_section_field(s.data_runs_raw)
                _po, pc = unpack_section_field(s.primitives_raw)
                expect = 0
                for r in range(rcnt):
                    o = mt.primitive_data_runs.data + (roff + r) * 4
                    if u8(d, o + 2) != expect:
                        break
                    expect = u8(d, o + 2) + u8(d, o + 3)
                else:
                    if expect == pc:
                        continue
                badruns += 1
            want(badruns == 0,
                 "%d sections whose data runs do not tile 0..primitiveCount" % badruns)

            # A primitive with indices[1] == indices[2] == indices[3] is a custom
            # primitive, and the runtime indexes a THREE-entry shape-type table with the
            # low nibble of sharedVerticesIndex[indices[0]]. Stock only ever uses type 2
            # (NOP). Anything else means a degenerate triangle was emitted as [a,b,c,c]
            # with b == c and is now being read as a custom primitive with an
            # out-of-bounds type.
            badcustom = collections.Counter()
            for si in range(S):
                s = read_section(d, mt.sections.data, si, fix)
                poff, pc = unpack_section_field(s.primitives_raw)
                first = s.shared_vertices_raw >> 8
                for pi in range(pc):
                    o = mt.primitives.data + (poff + pi) * 4
                    q = [d[o], d[o + 1], d[o + 2], d[o + 3]]
                    if q == [0xDE, 0xAD, 0xDE, 0xAD] or not (q[1] == q[2] == q[3]):
                        continue
                    j = first + q[0] - s.num_packed_vertices
                    if q[0] < s.num_packed_vertices or not (
                            0 <= j < mt.shared_vertices_index.size):
                        badcustom["unreadable"] += 1
                        continue
                    t = u16(d, mt.shared_vertices_index.data + j * 2) & 0xF
                    if t > 2:
                        badcustom[t] += 1
            want(not badcustom,
                 "%d custom primitives with a shape type outside the 3-entry table %s"
                 % (sum(badcustom.values()), dict(badcustom)))

            leaks = 0
            for si in range(S):
                s = read_section(d, mt.sections.data, si, fix)
                try:
                    prims = section_primitive_aabbs(d, mt, s, fix)
                except NotImplementedError:
                    continue
                ext = [s.domain[4 + k] - s.domain[k] for k in range(3)]
                eps = [max(1e-2 * e, 0.05) for e in ext]
                for pidx, lmn, lmx in decode_section_tree(d, s):
                    if pidx >= len(prims) or prims[pidx] is None:
                        continue
                    pmn, pmx = prims[pidx]
                    if not all(lmn[k] <= pmn[k] + eps[k] and lmx[k] >= pmx[k] - eps[k]
                               for k in range(3)):
                        leaks += 1
            want(leaks == 0,
                 "%d per-section BVH leaf boxes do not contain their primitive" % leaks)

            simd = read_hkarray(d, do + 176 + 8, fix)
            want(simd.size > 0,
                 "simdTree is EMPTY -- the shape will load and collide with nothing")
            if simd.size:
                keys, svisited, sbad = simd_leaves(d, mt, simd)
                real = mt.primitives.size - dead_primitives(d, mt)
                want(sbad == 0, "simdTree has %d out-of-range child refs" % sbad)
                want(len(keys) == real,
                     "simdTree indexes %d primitives, mesh has %d real ones" % (len(keys), real))
                want(len(set(keys)) == len(keys), "simdTree references a primitive twice")
                # A primitive key is (section << 8) | (primitiveIndex << 1). The low bit is
                # reserved, exactly as in the BVH leaf byte -- so the primitive index is
                # SHIFTED, and reading the low byte directly names the wrong primitive for
                # half of all keys. Verified on stock: under this decode 100% of keys name a
                # real (section, primitive) pair and the keys are a bijection onto the
                # primitive set (mp_afghan 105,752 keys / 105,752 primitives); every other
                # candidate decode fails.
                pcs = []
                for si in range(S):
                    sec = read_section(d, mt.sections.data, si, fix)
                    _po, pc = unpack_section_field(sec.primitives_raw)
                    pcs.append(pc)
                badkey = 0
                for k in keys:
                    si, pi = k >> KEY_SECTION_SHIFT, (k & 0xFF) >> 1
                    if si >= S or pi >= pcs[si]:
                        badkey += 1
                want(badkey == 0,
                     "%d simdTree keys do not name a real (section, primitive) pair under "
                     "(section << 8) | (primitive << 1)" % badkey)
                want(len({(k >> KEY_SECTION_SHIFT, (k & 0xFF) >> 1) for k in keys})
                     == len(keys),
                     "simdTree keys are not a bijection onto (section, primitive) pairs")

        name = os.path.basename(path)
        print("%-46s %s" % (name, "OK" if not fails else "FAIL (%d)" % len(fails)))
        for f in fails[:12]:
            print("      - " + f)
        if len(fails) > 12:
            print("      - ... and %d more" % (len(fails) - 12))
        worst = max(worst, 1 if fails else 0)
    return worst


# ----------------------------------------------------------------------------- diff

def summarise(path):
    pf, d, fix = load(path)
    r = {}
    sl = shape_list(pf, d, fix)
    if sl:
        r["numWorldGeoShapes"] = sl["numWorldGeoShapes"]
        for k in ("shapes", "shapeTagData", "minMaxes"):
            r["%s.size" % k] = sl[k].size
        if sl["shapeContents"].size:
            r["shapeContents[0]"] = "0x%08X" % u32(d, sl["shapeContents"].data)
        if sl["convexCounts"].size:
            r["convexCounts[0]"] = i32(d, sl["convexCounts"].data)
    for _so, do, shape, mt in meshes(pf, d, fix):
        r["shape.flags"] = "0x%X" % shape.flags
        r["shape.numShapeKeyBits"] = shape.num_shape_key_bits
        r["shape.dispatchType"] = shape.dispatch_type
        r["shape.convexRadius"] = "%.4f" % shape.convex_radius
        r["shape.shapeTagCodecInfo"] = "0x%08X" % shape.shape_tag_codec_info
        r["tree.sections"] = mt.sections.size
        r["tree.primitives"] = mt.primitives.size
        r["tree.packedVertices"] = mt.packed_vertices.size
        r["tree.sharedVertices"] = mt.shared_vertices.size
        r["tree.dataRuns"] = mt.primitive_data_runs.size
        r["tree.bitsPerKey"] = mt.bits_per_key
        r["tree.maxKeyValue"] = mt.max_key_value
        r["simd.nodes"] = read_hkarray(d, do + 176 + 8, fix).size
        s0 = read_section(d, mt.sections.data, 0, fix)
        r["sec0.numPackedVertices"] = s0.num_packed_vertices
        r["sec0.numSharedIndices"] = s0.num_shared_indices
        r["sec0.leafIndex"] = s0.leaf_index
        r["sec0.flags"] = s0.flags
        nz = sum(1 for i in range(mt.nodes.size)
                 if any(d[mt.nodes.data + i * 5 + c] for c in range(3)))
        r["topNodes.nonzeroAABB"] = "%d/%d" % (nz, mt.nodes.size)
        break
    return r


def cmd_diff(args):
    a, b = summarise(args.a), summarise(args.b)
    print("%-30s %-24s %s" % ("field", os.path.basename(args.a), os.path.basename(args.b)))
    for k in sorted(set(a) | set(b)):
        va, vb = a.get(k, "-"), b.get(k, "-")
        mark = "" if str(va) == str(vb) else "   <<<"
        print("%-30s %-24s %-20s%s" % (k, va, vb, mark))


# ----------------------------------------------------------------------------- geom

def cmd_geom(args):
    pf, d, fix = load(args.path)
    total_v = total_t = 0
    obj = [] if args.obj else None
    base = 1
    for _so, do, shape, mt in meshes(pf, d, fix):
        try:
            dm = decode_mesh(d, mt, shape.convex_radius, fix)
        except NotImplementedError as exc:
            # This used to be swallowed as a "known decoder limitation" for sections with
            # numPackedVertices == 0. That gap is closed -- npv0 is in fact the ordinary
            # stock encoding (9,117 of the 9,708 sections in the shipped world blobs) and
            # all 601 mesh objects in the dump decode -- so reaching here now means a
            # genuinely malformed or unsupported blob, and it must not report success.
            print("  ERROR: geometry does not decode: %s" % exc)
            return 1
        bmin, bmax = dm.bounds()
        print("mesh: %d vertices, %d triangles (%d quads)"
              % (len(dm.vertices), len(dm.triangles), len(dm.quads)))
        print("  bounds (Havok units; x32 for CoD units) (%.1f %.1f %.1f) .. (%.1f %.1f %.1f)"
              % (*bmin, *bmax))
        up = down = 0
        for (ia, ib, ic) in dm.triangles:
            A, B, C = dm.vertices[ia], dm.vertices[ib], dm.vertices[ic]
            e1 = [B[i] - A[i] for i in range(3)]
            e2 = [C[i] - A[i] for i in range(3)]
            nz = e1[0] * e2[1] - e1[1] * e2[0]
            if nz > 0:
                up += 1
            elif nz < 0:
                down += 1
        print("  winding: %d CCW / %d CW about +Z" % (up, down))
        total_v += len(dm.vertices)
        total_t += len(dm.triangles)
        if obj is not None:
            for v in dm.vertices:
                obj.append("v %.4f %.4f %.4f" % v)
            for (ia, ib, ic) in dm.triangles:
                obj.append("f %d %d %d" % (base + ia, base + ib, base + ic))
            base += len(dm.vertices)
    if obj is not None:
        with open(args.obj, "w") as fh:
            fh.write("\n".join(obj) + "\n")
        print("wrote %s (%d verts, %d tris)" % (args.obj, total_v, total_t))


# ----------------------------------------------------------------------------- aabb

def cmd_aabb(args):
    """Decode the top-level BVH with the quadratic nibble codec and report how tight the
    leaf boxes are against each section's own domain. All-zero nibbles mean every box is
    the whole world, which makes collision far larger than the visible surfaces."""
    K = 1.0 / 226.0
    for path in expand(args.paths):
        pf, d, fix = load(path)
        for _so, do, shape, mt in meshes(pf, d, fix):
            N, S = mt.nodes.size, mt.sections.size
            nonzero = sum(1 for i in range(N)
                          if any(d[mt.nodes.data + i * 5 + c] for c in range(3)))
            leaves, visited, bad = {}, set(), 0
            stack = [(0, list(mt.domain[0:3]), list(mt.domain[4:7]))]
            while stack:
                n, pmn, pmx = stack.pop()
                if n < 0 or n >= N or n in visited:
                    continue
                visited.add(n)
                b = d[mt.nodes.data + n * 5: mt.nodes.data + n * 5 + 5]
                ext = [pmx[i] - pmn[i] for i in range(3)]
                mn = [pmn[i] + ((b[i] >> 4) ** 2) * ext[i] * K for i in range(3)]
                mx = [pmx[i] - ((b[i] & 0xF) ** 2) * ext[i] * K for i in range(3)]
                hi, lo = b[3], b[4]
                if hi & 0x80:
                    off = 2 * (((hi & 0x7F) << 8) | lo)
                    stack.append((n + off, mn, mx))
                    stack.append((n + 1, mn, mx))
                else:
                    leaves[(hi << 8) | lo] = (mn, mx)
            contains = 0
            ratios = []
            for si, (mn, mx) in leaves.items():
                s_ = read_section(d, mt.sections.data, si, fix)
                dom = s_.domain
                if all(mn[i] <= dom[i] + 1e-3 and mx[i] >= dom[4 + i] - 1e-3 for i in range(3)):
                    contains += 1
                vb = 1.0
                vs = 1.0
                for i in range(3):
                    vb *= max(mx[i] - mn[i], 1e-6)
                    vs *= max(dom[4 + i] - dom[i], 1e-6)
                ratios.append(vb / vs)
            ratios.sort()
            med = ratios[len(ratios) // 2] if ratios else 0.0
            print("%-44s nodes with non-zero AABB: %d/%d" % (os.path.basename(path), nonzero, N))
            print("      leaf boxes containing their section domain: %d/%d"
                  % (contains, len(leaves)))
            print("      median (leaf box volume / section domain volume): %.1fx"
                  % med)
            if nonzero == 0:
                print("      -> every box is the whole world; collision will be far larger"
                      " than the surfaces")


# --------------------------------------------------------------------------- survey

def cmd_survey(args):
    print("%-46s %-10s %-9s %-9s %-8s %s"
          % ("file", "root", "sections", "prims", "simd", "domain"))
    for path in expand(args.paths):
        try:
            pf, d, fix = load(path)
        except Exception as exc:                      # noqa: BLE001
            print("%-46s <unreadable: %s>" % (os.path.basename(path), exc))
            continue
        row = None
        for _so, do, shape, mt in meshes(pf, d, fix):
            simd = read_hkarray(d, do + 176 + 8, fix)
            row = ("%-9d %-9d %-8d (%.1f %.1f %.1f)"
                   % (mt.sections.size, mt.primitives.size, simd.size,
                      mt.domain[4] - mt.domain[0], mt.domain[5] - mt.domain[1],
                      mt.domain[6] - mt.domain[2]))
            break
        print("%-46s %-10s %s" % (os.path.basename(path),
                                  pf.root_class_name()[:10], row or "(no mesh)"))


# ----------------------------------------------------------------------------- main

class NoFilesMatched(Exception):
    """Raised when a pattern matched nothing.

    This used to return an empty list, which made `check` fall straight through its
    loop and exit 0 without printing anything -- indistinguishable from a clean pass.
    The backlog's acceptance test for the P9/P10/P12 fixes is a glob, so a typo'd path,
    a map that was never re-dumped, or a cp_-prefixed map that the documented
    dump/*/maps/mp/* pattern does not reach all reported as success.
    """


def expand(patterns):
    out = {}
    for pat in patterns:
        hits = [pat] if os.path.isfile(pat) else globmod.glob(pat, recursive=True)
        for h in hits:
            out.setdefault(os.path.normcase(os.path.abspath(h)), h)
    if not out:
        raise NoFilesMatched("no files matched: %s" % " ".join(patterns))
    return [out[k] for k in sorted(out)]


def main():
    ap = argparse.ArgumentParser(description=__doc__,
                                 formatter_class=argparse.RawDescriptionHelpFormatter)
    sub = ap.add_subparsers(dest="cmd", required=True)

    p = sub.add_parser("info", help="header, sections, objects, mesh summary")
    p.add_argument("paths", nargs="+")
    p.set_defaults(fn=cmd_info)

    p = sub.add_parser("check", help="validate against the stock-derived invariants")
    p.add_argument("paths", nargs="+")
    p.set_defaults(fn=cmd_check)

    p = sub.add_parser("diff", help="compare two blobs field by field")
    p.add_argument("a")
    p.add_argument("b")
    p.set_defaults(fn=cmd_diff)

    p = sub.add_parser("geom", help="geometry stats, optional OBJ export")
    p.add_argument("path")
    p.add_argument("-o", "--obj", help="write a Wavefront OBJ of the collision mesh")
    p.set_defaults(fn=cmd_geom)

    p = sub.add_parser("aabb", help="report BVH box tightness")
    p.add_argument("paths", nargs="+")
    p.set_defaults(fn=cmd_aabb)

    p = sub.add_parser("survey", help="one line per file")
    p.add_argument("paths", nargs="+")
    p.set_defaults(fn=cmd_survey)

    args = ap.parse_args()
    try:
        return args.fn(args) or 0
    except NoFilesMatched as exc:
        print("ERROR: %s" % exc)
        return 2


if __name__ == "__main__":
    raise SystemExit(main())
