#!/usr/bin/env python3
"""Sequential parser for IW7 .gfxmap dumps.

Mirrors ZoneTool::IW7::IGfxWorld::dump exactly. The stream is tagged, so every step asserts
that the next tag is one of {STRING, ASSET, ARRAY, OFFSET, RAW}; a wrong element size derails
into an invalid tag immediately instead of silently returning garbage.

Struct sizes and GfxWorld field offsets come from the IW7 IDA database's own type library
(scripts/dump_types.py), not from re-reading the C++ headers.
"""
import struct, sys, os, json

T_STRING, T_ASSET, T_ARRAY, T_OFFSET, T_RAW = 6, 7, 8, 9, 10
SZ_GFXWORLD = 4520

# GfxWorld field offsets (IDA type library)
O = {
    "planeCount": 20, "nodeCount": 24, "surfaceCount": 28, "skyCount": 32,
    "primaryLightCount": 52, "cellCount": 120,
    "reflectionProbeCount": 168, "probeRelightingCount": 192,
    "reflectionProbeGBufferImageCount": 208, "reflectionProbeInstanceCount": 232,
    "reindexCount": 284, "packedLightmapCount": 296,
    "decalVolumeCollectionCount": 320, "lightMapCount": 352,
    "indexCount": 664, "volumetricCount": 688,
    "stageCount": 751, "paletteEntryCount": 760, "paletteBitstreamSize": 776,
    "treeNodeCount": 2140, "treeLeafTableSize": 2200,
    "gpuVisibleProbesCount": 2216, "probeCount": 2264, "zoneCount": 2328,
    "tetrahedronCount": 2344, "tetrahedronCountVisible": 2348,
    "voxelStartTetrahedronCount": 2424,
    "voxelTreeCount": 2472, "heightfieldCount": 2488, "unk03Count": 2536,
    "modelCount": 2552, "materialMemoryCount": 2596,
    "lightAABBNodeCount": 2952, "lightAABBLightCount": 2954,
    "smodelCount": 2976, "staticSurfaceCount": 2980, "surfaceVisDataCount": 3020,
    "heroOnlyLightCount": 4412, "numUmbraGates": 4424, "umbraTomeSize": 4440,
}
U8 = ("stageCount",)
U16 = ("lightAABBNodeCount", "lightAABBLightCount")


class Reader:
    def __init__(self, path):
        self.b = open(path, "rb").read()
        self.o = 0
        self.trace = []

    def u8(self):
        v = self.b[self.o]; self.o += 1; return v

    def u32(self):
        v = struct.unpack_from("<I", self.b, self.o)[0]; self.o += 4; return v

    def _backref(self):
        self.u32(); self.u32()

    def array(self, elem_size, label=""):
        start = self.o
        t = self.u8()
        if t == T_OFFSET:
            self._backref(); self.trace.append((label, start, "backref")); return None
        if t != T_ARRAY:
            raise ValueError("%s: expected ARRAY at 0x%X, got tag %d" % (label, start, t))
        if not self.u8():
            self.trace.append((label, start, "absent")); return b""
        n = self.u32()
        need = n * elem_size
        if self.o + need > len(self.b):
            raise ValueError("%s: array of %d x %d overruns file at 0x%X" % (label, n, elem_size, start))
        d = self.b[self.o:self.o + need]; self.o += need
        self.trace.append((label, start, "%d x %d" % (n, elem_size)))
        return d

    def string(self, label=""):
        start = self.o
        t = self.u8()
        if t == T_OFFSET:
            self._backref(); return "<ref>"
        if t != T_STRING:
            raise ValueError("%s: expected STRING at 0x%X, got tag %d" % (label, start, t))
        if not self.u8():
            return None
        e = self.b.index(b"\x00", self.o)
        s = self.b[self.o:e].decode("utf-8", "replace"); self.o = e + 1
        return s

    def asset(self, label=""):
        start = self.o
        t = self.u8()
        if t == T_OFFSET:
            self._backref(); return "<ref>"
        if t != T_ASSET:
            raise ValueError("%s: expected ASSET at 0x%X, got tag %d" % (label, start, t))
        if not self.u8():
            return None
        e = self.b.index(b"\x00", self.o)
        s = self.b[self.o:e].decode("utf-8", "replace"); self.o = e + 1
        return s


def parse(path, stop_after="smodelDrawInsts"):
    r = Reader(path)
    hdr = r.array(SZ_GFXWORLD, "GfxWorld")
    if hdr is None or len(hdr) < SZ_GFXWORLD:
        raise ValueError("bad header")

    def c(name):
        off = O[name]
        if name in U8:
            return hdr[off]
        if name in U16:
            return struct.unpack_from("<H", hdr, off)[0]
        return struct.unpack_from("<I", hdr, off)[0]

    out = {"counts": {k: c(k) for k in O}}
    r.string("name"); r.string("baseName")

    skies = r.array(32, "skies")
    for i in range(c("skyCount")):
        # GfxSky: skySurfCount@0 (int), skyStartSurfs@8, skyImage, ...
        n = struct.unpack_from("<i", skies, i * 32)[0] if skies else 0
        r.array(4, "skyStartSurfs"); r.asset("skyImage")

    r.array(20, "planes")
    r.array(2, "dpvsPlanes.nodes")
    r.array(4, "cellTransientInfos")
    cells = r.array(40, "cells")
    for i in range(c("cellCount")):
        portals = r.array(80, "cell%d.portals" % i)
        npor = struct.unpack_from("<I", cells, i * 40 + 24)[0] if cells else 0
        for j in range(npor):
            r.array(12, "portal.vertices")

    probes = r.array(48, "reflectionProbes")
    for i in range(c("reflectionProbeCount")):
        r.string("probe.livePath"); r.array(4, "probe.instances")
    r.asset("reflectionProbeArrayImage")
    r.array(32, "probeRelightingData")
    r.array(8, "reflectionProbeGBufferImages")
    for i in range(c("reflectionProbeGBufferImageCount")):
        r.asset("gbufferImage")
    r.array(152, "reflectionProbeInstances")
    for i in range(c("reflectionProbeInstanceCount")):
        r.string("inst.livePath"); r.string("inst.livePath2")
    r.array(32, "reflectionProbeLightgridSampleData")

    r.array(20, "reindexElement")
    r.array(8, "packedLightmap")
    r.asset("iesLookupTexture")
    r.array(388, "decalVolumeCollections")
    r.asset("lightmapOverridePrimary"); r.asset("lightmapOverrideSecondary")
    r.array(8, "lightMaps")
    for i in range(c("lightMapCount")):
        r.asset("lightMap")
    for i in range(32):
        r.asset("transientZone%d" % i)
    r.array(2, "indices")
    r.array(240, "volumetrics")
    for i in range(c("volumetricCount")):
        r.string("vol.livePath")
        for m in range(4):
            r.asset("vol.mask")

    r.array(4, "stageLightingContrastGain")
    r.array(4, "paletteEntryAddress")
    r.array(1, "paletteBitstream")
    r.array(4, "tree.nodeTable")
    r.array(1, "tree.leafTable")

    out["gpuVisibleProbePositions"] = r.array(12, "gpuVisibleProbePositions")
    out["gpuVisibleProbesData"] = r.array(64, "gpuVisibleProbesData")
    out["probes"] = r.array(64, "probes")
    out["probePositions"] = r.array(12, "probePositions")
    out["zones"] = r.array(88, "zones")
    out["tetrahedrons"] = r.array(16, "tetrahedrons")
    out["tetrahedronNeighbors"] = r.array(16, "tetrahedronNeighbors")
    out["tetrahedronVisibility"] = r.array(64, "tetrahedronVisibility")
    out["voxelStartTetrahedron"] = r.array(4, "voxelStartTetrahedron")
    if stop_after == "probeData":
        out["_reader"] = r
        return out

    fl = r.array(48, "frustumLights")
    if fl:
        for i in range(c("primaryLightCount")):
            r.array(2, "fl.indices"); r.array(1, "fl.vertices")
    lvf = r.array(48, "lightViewFrustums")
    if lvf:
        for i in range(c("primaryLightCount")):
            r.array(16, "lvf.planes")     # vec4_t, not cplane_s
            r.array(2, "lvf.indices")
            r.array(12, "lvf.vertices")

    out["voxelTree"] = r.array(112, "voxelTree")
    out["voxelTreeHeaders"] = []
    out["voxelTopDownViewNodes"] = []
    out["voxelInternalNodes"] = []
    out["voxelLeafNodes"] = []
    for i in range(c("voxelTreeCount")):
        out["voxelTreeHeaders"].append(r.array(64, "voxelTreeHeader"))
        out["voxelTopDownViewNodes"].append(r.array(12, "voxelTopDownViewNode"))
        out["voxelInternalNodes"].append(r.array(16, "voxelInternalNode"))
        out["voxelLeafNodes"].append(r.array(2, "voxelLeafNode"))
        r.array(2, "lightList")
    if stop_after == "voxelTree":
        out["_reader"] = r
        return out

    r.array(96, "heightfields")
    for i in range(c("heightfieldCount")):
        r.asset("heightfield.image")
    r.array(2, "unk01.unk03")
    r.array(60, "models")
    r.array(16, "materialMemory")
    for i in range(c("materialMemoryCount")):
        r.asset("materialMemory.material")
    r.asset("sun.spriteMaterial"); r.asset("sun.flareMaterial")
    r.asset("outdoorImage"); r.asset("dustMaterial")
    sg = r.array(24, "shadowGeomOptimized")
    if sg:
        for i in range(c("primaryLightCount")):
            r.array(4, "sg.sortedSurfIndex")   # unsigned int*, not u16
            r.array(2, "sg.smodelIndex")
    lr = r.array(16, "lightRegion")
    for i in range(c("primaryLightCount")):
        hulls = r.array(88, "lr.hulls")
        nh = struct.unpack_from("<I", lr, i * 16)[0] if lr else 0
        for j in range(nh):
            naxis = struct.unpack_from("<I", hulls, j * 88 + 72)[0] if hulls else 0
            r.array(20, "hull.axis")
    r.array(28, "lightAABB.nodeArray")
    r.array(2, "lightAABB.lightArray")
    r.array(4, "dpvs.lodData")
    r.array(4, "dpvs.sortedSurfIndex")
    r.array(36, "dpvs.smodelInsts")
    r.array(48, "dpvs.surfaces")
    for i in range(c("surfaceCount")):
        r.asset("surface.material")
    r.array(36, "dpvs.surfacesBounds")
    out["smodelDrawInsts"] = r.array(184, "dpvs.smodelDrawInsts")
    out["smodelModels"] = []
    for i in range(c("smodelCount")):
        out["smodelModels"].append(r.asset("smodel.model"))
        r.array(20, "smodel.lightingValues")
    out["_reader"] = r
    return out


if __name__ == "__main__":
    import glob
    paths = sys.argv[1:] or (
        sorted(glob.glob(r"D:\Games\PC\IW7\dump\*\maps\*\*.gfxmap")) +
        sorted(glob.glob(r"D:\Games\PC\IW7\zonetool\*\maps\*\*.gfxmap")))
    for p in paths:
        tag = os.path.basename(os.path.dirname(os.path.dirname(os.path.dirname(p))))
        try:
            d = parse(p)
            print("OK   %-16s smodels=%-6d gpuVis=%-7d probes=%-7d tets=%-7d consumed=%.1f%%"
                  % (tag, d["counts"]["smodelCount"], d["counts"]["gpuVisibleProbesCount"],
                     d["counts"]["probeCount"], d["counts"]["tetrahedronCount"],
                     100.0 * d["_reader"].o / len(d["_reader"].b)))
        except Exception as e:
            print("FAIL %-16s %s" % (tag, e))
