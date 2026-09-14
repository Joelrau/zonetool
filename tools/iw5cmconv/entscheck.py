"""Validate a MapEnts::havokEntsShapeData blob against the stock-derived invariants.

    python entscheck.py <file.ents.data.hkx> [...]

Every rule here was read off the shipped ents blobs (mp_afghan, mp_breakneck, mp_paris,
cp_zmb, mp_dome_dusk) and is documented in docs/iw7-ents-shapes.md: object classes and
sizes, hknpShape header fields, hkRelArray payload contiguity and 16-byte alignment,
face/plane pairing, counter-clockwise winding about the outward normal, convexity, the
half-edge connectivity (vertexEdges starts at its vertex, faceLinks is the twin), and the
dynamic AABB tree (2L+1 nodes, zeroed sentinels, parent links, leaves a permutation of the
instances).

minHalfAngle is reported as a match rate rather than checked: the quantisation is
round(halfAngle / 45 * 127), which is exact for 83% of shipped faces, but the exact edge
set Havok minimises over is not pinned down. It is 127 for every right angle either way.

Two quirks of the shipped data are tolerated so stock passes too: cp_zmb ships some
per-shape arrays one entry short, and a handful of convexes have trailing vertexEdges
entries that do not start at their vertex.
"""

import math, os, sys, struct
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import hkxtool
from hkcompressedmesh import read_hkarray, u8, u16, u32, i32


def check_fixup_classes(pf):
    """Havok splits pointer patches by target: a pointer to a registered object is a
    GLOBAL fixup, a pointer to raw payload (array elements, string bytes) is a LOCAL one.
    Shipped blobs are completely consistent -- 532/532 globals land on an object and
    0/1420 locals do -- and both resolve to the same address inside one section, so
    getting it wrong produces a file that looks fine and is subtly not.
    """
    sec = pf.sections[2]
    objs = set(o for _s, o, _n in pf.objects())
    out = []
    strays = [s for s, dst in pf.local_fixups(sec) if dst in objs]
    if strays:
        out.append(f"{len(strays)} local fixup(s) point at a registered object "
                   f"(should be global), first at +{strays[0]}")
    misses = [s for s, _si, dst in pf.global_fixups(sec) if dst not in objs]
    if misses:
        out.append(f"{len(misses)} global fixup(s) do not point at an object, "
                   f"first at +{misses[0]}")
    # every object except the root should be reachable through exactly one global fixup
    pointed = set(dst for _s, _si, dst in pf.global_fixups(sec))
    orphans = objs - pointed - {0}
    if orphans:
        out.append(f"{len(orphans)} object(s) have no global fixup pointing at them: "
                   f"{sorted(orphans)[:4]}")
    return out


def check(path, verbose=True):
    pf, d, fix = hkxtool.load(path)
    objs = {o: nm for _s, o, nm in pf.objects()}
    fixup_bad = check_fixup_classes(pf)
    sl = hkxtool.shape_list(pf, d, fix)
    SD = sl["shapes"].data
    compacted, _seen = [], 0
    for _i in range(sl["shapes"].size):
        compacted.append(_seen)
        if fix.get(SD + 8 * _i) is not None: _seen += 1
    v4 = lambda o: struct.unpack_from('<4f', d, o)
    rel = lambda o: (u16(d, o), u16(d, o + 2))
    bad = list(fixup_bad)

    # hkStringPtr stores its "I allocated this, free it" flag in bit 0 of the pointer
    # itself: hkStringPtr::operator= tests `ptr & 1` and calls easyFree(ptr - 1), and IW7
    # masks it off with `ptr & ~1` wherever it reads one. Section data starts 16-aligned,
    # so anything a local fixup points at must be at an even offset -- an odd one becomes a
    # pointer with bit 0 set, and unloading the zone hands that address, which is inside the
    # fastfile rather than the Havok heap, to hkLargeBlockAllocator::blockFree. It reads the
    # neighbouring bytes as a block header and dereferences a garbage free-tree node.
    #
    # This is silent until the map unloads, which is what made it expensive to find. No
    # stock blob has one: 992 of 992 local fixups in mp_paris land on even offsets.
    for _sec in pf.sections:
        for _src, _dst in pf.local_fixups(_sec):
            if _dst % 2:
                bad.append(f"local fixup {_src} -> odd offset {_dst} "
                           f"(hkStringPtr would read as owned and be freed on unload)")

    mha_stats = [0, 0]   # (faces checked, faces matching the fitted law)
    N = sl["shapes"].size
    # Two indexing schemes. shapes / shapeIndices / shapeNames / shapeContents are indexed
    # by SLOT (size == shapes.size) -- shapeContents has to be, since
    # CM_ContentsOfBrushModel looks it up with physicsShapeOverrideIdx. vertCounts /
    # triCounts / convexCounts / minMaxes are indexed by the COMPACTED index that
    # shapeIndices produces, so they are sized by the number of non-null shapes. The two
    # coincide unless the list has holes (only cp_zmb ships one).
    live = compacted[-1] + (1 if N and fix.get(SD + 8 * (N - 1)) is not None else 0) if N else 0
    for key in ("shapeIndices", "shapeNames", "shapeContents"):
        if sl[key].size != N: bad.append(f"{key} size {sl[key].size} != {N} (slot-indexed)")
    for key in ("vertCounts", "triCounts", "convexCounts"):
        if sl[key].size != live: bad.append(f"{key} size {sl[key].size} != {live} (compacted)")
    if sl["minMaxes"].size != 2 * live: bad.append(f"minMaxes size {sl['minMaxes'].size} != {2*live}")
    for i in range(N):
        # shapeIndices compacts over null shape slots: it counts non-null shapes seen so
        # far, so it is the identity only when the list has no holes (cp_zmb has one).
        if i < sl["shapeIndices"].size:
            got = i32(d, sl["shapeIndices"].data + 4 * i)
            if got != compacted[i]: bad.append(f"shapeIndices[{i}] {got}!={compacted[i]}")
        sp = fix.get(SD + 8 * i)
        if sp is None:
            continue  # cp_zmb ships null entries in the shape list
        if objs.get(sp) == "hknpCompressedMeshShape":
            continue  # legal entry, just not what this checker covers
        if objs.get(sp) != "hknpDynamicCompoundShape":
            bad.append(f"shape{i} class {objs.get(sp)}"); continue
        inst = read_hkarray(d, sp + 96, fix); L = inst.size
        bvd = fix[sp + 192]; nodes = read_hkarray(d, bvd + 16, fix)
        tail = struct.unpack_from('<6i', d, bvd + 32)
        if u8(d, sp + 18) != L.bit_length(): bad.append(f"shape{i} keyBits")
        if u16(d, sp + 16) != 4: bad.append(f"shape{i} flags")
        if u8(d, sp + 19) != 2: bad.append(f"shape{i} dispatchType")
        if u32(d, sp + 88) != 0xFFFFFFFF: bad.append(f"shape{i} shapeTagCodecInfo")
        if i32(d, sp + 96 + 16) != -1: bad.append(f"shape{i} firstFree")
        if u8(d, sp + 160) != 1: bad.append(f"shape{i} isMutable")
        if i32(d, sl["convexCounts"].data + 4 * compacted[i]) != L: bad.append(f"shape{i} convexCounts")
        if nodes.size != 2 * L + 1: bad.append(f"shape{i} nodeCount")
        if tail != (2 * L, 0, L, 0, 1, 0): bad.append(f"shape{i} tail {tail}")
        leaves, visited = [], set()
        stack = [(1, 0)]
        while stack:
            n, parent = stack.pop()
            if n in visited: bad.append(f"shape{i} cycle"); break
            visited.add(n)
            b = nodes.data + n * 32
            if u16(d, b + 12) != parent: bad.append(f"shape{i} node{n} parent")
            if u16(d, b + 14) != 0x3F00: bad.append(f"shape{i} node{n} filler")
            w = u32(d, b + 28)
            if (w & 0xFFFF) == 0: leaves.append(w >> 16)
            else: stack += [(w & 0xFFFF, n), (w >> 16, n)]
        if sorted(leaves) != list(range(L)): bad.append(f"shape{i} leaves {sorted(leaves)}")
        for z in (0, nodes.size - 1):
            if d[nodes.data + z * 32: nodes.data + z * 32 + 32] != b'\0' * 32: bad.append(f"shape{i} sentinel{z}")
        amn, amx = v4(sp + 128)[:3], v4(sp + 144)[:3]
        gmn, gmx = [1e30] * 3, [-1e30] * 3
        vtotal = 0
        for k in range(L):
            cp = fix[inst.data + k * 128 + 80]
            if objs.get(cp) != "hknpConvexPolytopeShape": bad.append(f"shape{i}.{k} class"); continue
            # 0x143 is the common value; 0x103 also ships (differs in one flag bit)
            if u16(d, cp + 16) not in (0x143, 0x103): bad.append(f"shape{i}.{k} flags")
            if u8(d, cp + 19) != 1: bad.append(f"shape{i}.{k} dispatchType")
            nv, ov = rel(cp + 48); npl, opl = rel(cp + 64); nf, of = rel(cp + 68); ni, oi = rel(cp + 72)
            vb, pb, fb, ib = cp + 48 + ov, cp + 64 + opl, cp + 68 + of, cp + 72 + oi
            for nm, a in (("verts", vb), ("planes", pb), ("faces", fb), ("indices", ib)):
                if a % 16: bad.append(f"shape{i}.{k} {nm} align")
            if npl != nf: bad.append(f"shape{i}.{k} planes!=faces")
            vtotal += nv
            verts = [v4(vb + 16 * j)[:3] for j in range(nv)]
            for v in verts:
                for c in range(3): gmn[c] = min(gmn[c], v[c]); gmx[c] = max(gmx[c], v[c])
            idx = list(d[ib:ib + ni]); pos = 0
            mha_expected = []
            faces = [struct.unpack_from('<HBB', d, fb + 4 * f) for f in range(nf)]
            for f in range(nf):
                fi, n_, mha = faces[f]
                if fi != pos: bad.append(f"shape{i}.{k} face{f} firstIndex")
                mha_expected.append((f, mha))
                run = idx[fi:fi + n_]; pos += n_
                # No face may name the same vertex twice. A repeated index is a zero-length
                # edge, which makes the half-edge ring inconsistent and the face degenerate.
                # It is exactly what vertex welding produces if the welded indices are not
                # de-duplicated, so this is a live risk for a generated hull, not a
                # theoretical one.
                if len(set(run)) != len(run):
                    bad.append(f"shape{i}.{k} face{f} repeats a vertex index")
                if len(run) < 3:
                    bad.append(f"shape{i}.{k} face{f} has {len(run)} indices")
                pl = v4(pb + 16 * f)
                for vi in run:
                    if abs(sum(pl[c] * verts[vi][c] for c in range(3)) + pl[3]) > 1e-3:
                        bad.append(f"shape{i}.{k} face{f} off-plane"); break
                area = [0.0, 0.0, 0.0]
                for e in range(len(run)):
                    a, b2 = verts[run[e]], verts[run[(e + 1) % len(run)]]
                    area[0] += a[1]*b2[2]-a[2]*b2[1]; area[1] += a[2]*b2[0]-a[0]*b2[2]; area[2] += a[0]*b2[1]-a[1]*b2[0]
                if sum(area[c] * pl[c] for c in range(3)) <= 0: bad.append(f"shape{i}.{k} face{f} winding")
                for v in verts:
                    if sum(pl[c] * v[c] for c in range(3)) + pl[3] > 1e-3:
                        bad.append(f"shape{i}.{k} face{f} not convex"); break
            if pos != ni: bad.append(f"shape{i}.{k} indexTotal")
            conn = fix.get(cp + 80)
            if conn is None:
                continue  # connectivity is optional -- some shipped convexes have none
            if objs.get(conn) != "hknpConvexPolytopeShapeConnectivity": bad.append(f"shape{i}.{k} connectivity"); continue
            ve = read_hkarray(d, conn + 16, fix); fl = read_hkarray(d, conn + 32, fix)
            if ve.size != nv: bad.append(f"shape{i}.{k} vertexEdges size")
            if fl.size != ni: bad.append(f"shape{i}.{k} faceLinks size")
            he = lambda f, e: (idx[faces[f][0] + e], idx[faces[f][0] + (e + 1) % faces[f][1]])
            # vertices are padded up to a multiple of 4 by repeating the last real one, so
            # only the used range has to satisfy "starts at its own vertex"
            used = len(set(idx))
            if nv != (used + 3) // 4 * 4:
                bad.append(f"shape{i}.{k} verts {nv} != roundup({used},4)")
            for j in range(used, nv):
                if verts[j] != verts[used - 1]: bad.append(f"shape{i}.{k} pad vert {j}")
            # the w lane carries the vertex index: 0x3F000000 | i, and a padded vertex
            # repeats the last real one's w along with its position
            for j in range(nv):
                w = u32(d, vb + 16 * j + 12)
                want = 0x3F000000 | min(j, used - 1)
                if w != want: bad.append(f"shape{i}.{k} vert{j} w=0x{w:08X} != 0x{want:08X}")
            for v in range(min(ve.size, used)):
                ef, ee = struct.unpack_from('<HB', d, ve.data + 4 * v)
                if he(ef, ee)[0] != v: bad.append(f"shape{i}.{k} vertexEdges[{v}]")
            g = 0
            for f in range(nf):
                for e in range(faces[f][1]):
                    tf, te = struct.unpack_from('<HB', d, fl.data + 4 * g)
                    if he(tf, te) != tuple(reversed(he(f, e))): bad.append(f"shape{i}.{k} faceLinks[{g}]")
                    g += 1
            # minHalfAngle: min dihedral half-angle over the face's edges, quantised
            # linearly over [0, 45] into [0, 127]
            planes = [v4(pb + 16 * f) for f in range(nf)]
            for f, mha in mha_expected:
                fi, n_, _ = faces[f]
                smallest = 45.0
                for e in range(n_):
                    tf, te = struct.unpack_from('<HB', d, fl.data + 4 * (fi + e))
                    if tf >= nf or tf == f: continue
                    c = max(-1.0, min(1.0, sum(planes[f][c2] * planes[tf][c2] for c2 in range(3))))
                    smallest = min(smallest, (180.0 - math.degrees(math.acos(c))) / 2.0)
                # IW8's encoder verbatim: v = (int)((angle*0.5 - 2^-23) * 41720.875 + 0.5),
                # minHalfAngle = clamp(v, 0, 65535) >> 8   (41720.875 ~= 131072/pi)
                h = math.radians(smallest)
                want = max(0, min(65535, int((h - 2.0 ** -23) * 41720.875 + 0.5))) >> 8
                mha_stats[0] += 1
                if abs(want - mha) <= 1: mha_stats[1] += 1
        if i32(d, sl["vertCounts"].data + 4 * compacted[i]) != vtotal: bad.append(f"shape{i} vertCounts")
        for c in range(3):
            if abs(amn[c] - gmn[c]) > 1e-4 or abs(amx[c] - gmx[c]) > 1e-4: bad.append(f"shape{i} aabb")
    return N, bad, mha_stats

def expand(args):
    """Accept globs so a whole dump tree can be checked in one command, matching
    hkxtool. Windows does not expand these for us."""
    import glob
    out = []
    for a in args:
        hits = sorted(glob.glob(a)) if any(c in a for c in "*?[") else [a]
        if not hits:
            print("no match: %s" % a)
        out.extend(hits)
    return out


if __name__ == "__main__":
    worst = 0
    for path in expand(sys.argv[1:]):
        n, bad, mha = check(path)
        worst = max(worst, 1 if bad else 0)
        name = os.path.basename(path)
        rate = f"minHalfAngle {100*mha[1]/mha[0]:.0f}% of {mha[0]}" if mha[0] else "minHalfAngle n/a"
        print(f"{name:44s} {n:4d} shapes  {'FAIL ' + str(bad[:6]) if bad else 'PASS'}   ({rate})")
    sys.exit(worst)
