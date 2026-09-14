#!/usr/bin/env python3
"""Parse an IW7 .commap dump and print its ComPrimaryLightEnv table.

Format is zonetool's assetmanager::dumper, which is a straight-line tagged stream:
  dump_array/dump_single -> [u8 8][u8 exists]([u32 count][count * sizeof(T) bytes])
  dump_string            -> [u8 6][u8 exists]([NUL-terminated bytes])
  back-reference         -> [u8 9][u32 index][u32 array_index]

IComWorld::dump writes, in order:
  single(ComWorld=0x68) | string(name) | array(ComPrimaryLight=144, primaryLightCount)
  | primaryLightCount * string(defName) | array(ComPrimaryLightEnv=10, primaryLightEnvCount)
  | string(changeListInfo.userName) | numUmbraGates * string(...)
"""
import struct, sys, glob, os

T_STRING, T_ASSET, T_ARRAY, T_OFFSET, T_RAW = 6, 7, 8, 9, 10
SZ_COMWORLD = 0x68
SZ_LIGHT = 144
SZ_ENV = 10


class R:
    def __init__(self, b):
        self.b, self.o = b, 0

    def u8(self):
        v = self.b[self.o]; self.o += 1; return v

    def u32(self):
        v = struct.unpack_from("<I", self.b, self.o)[0]; self.o += 4; return v

    def array(self, elem_size):
        t = self.u8()
        if t == T_OFFSET:
            self.u32(); self.u32(); return None
        assert t == T_ARRAY, "expected ARRAY at %d, got %d" % (self.o - 1, t)
        if not self.u8():
            return b""
        n = self.u32()
        d = self.b[self.o:self.o + n * elem_size]; self.o += n * elem_size
        return d

    def string(self):
        t = self.u8()
        if t == T_OFFSET:
            self.u32(); self.u32(); return "<backref>"
        assert t == T_STRING, "expected STRING at %d, got %d" % (self.o - 1, t)
        if not self.u8():
            return None
        e = self.b.index(b"\x00", self.o)
        s = self.b[self.o:e].decode("utf-8", "replace"); self.o = e + 1
        return s


def parse(path):
    r = R(open(path, "rb").read())
    cw = r.array(SZ_COMWORLD)
    name = r.string()
    # ComWorld: name@0 isInUse@8 useForwardPlus@12 bakeQuality@16 primaryLightCount@20
    #           primaryLights@24 scriptablePrimaryLightCount@32 firstScriptablePrimaryLight@36
    #           primaryLightEnvCount@40 primaryLightEnvs@48 ...
    light_count = struct.unpack_from("<I", cw, 20)[0]
    env_count = struct.unpack_from("<I", cw, 40)[0]
    first_scriptable = struct.unpack_from("<I", cw, 36)[0]
    lights = r.array(SZ_LIGHT)
    defnames = [r.string() for _ in range(light_count)]
    envs = r.array(SZ_ENV)
    return name, light_count, env_count, first_scriptable, lights, defnames, envs


for path in (sys.argv[1:] or
             sorted(glob.glob(r"D:\Games\PC\IW7\dump\*\maps\*\*.commap")) +
             sorted(glob.glob(r"D:\Games\PC\IW7\zonetool\*\maps\*\*.commap"))):
    tag = os.path.basename(os.path.dirname(os.path.dirname(os.path.dirname(path))))
    try:
        name, nl, ne, fs, lights, defnames, envs = parse(path)
    except Exception as e:
        print("%-16s PARSE FAILED: %s" % (tag, e))
        continue
    print("\n=== %s  (%s) ===" % (tag, name))
    print("primaryLightCount=%d  primaryLightEnvCount=%d  firstScriptablePrimaryLight=%d"
          % (nl, ne, fs))
    # ComPrimaryLight: type@0 canUseShadowMap@1 needsDynamicShadows@2 isVolumetric@3 ...
    types = [lights[i * SZ_LIGHT] for i in range(nl)] if lights else []
    TYPE = {0: "NONE", 1: "DIR(sun)", 2: "SPOT", 3: "OMNI"}
    print("light types: " + ", ".join("%d:%s" % (i, TYPE.get(t, str(t)))
                                      for i, t in enumerate(types[:12]))
          + (" ..." if nl > 12 else ""))
    if envs is None or not envs:
        print("primaryLightEnvs: <absent>")
        continue
    print("primaryLightEnvs (index: numIndices -> lights):")
    for i in range(min(ne, 16)):
        idx = struct.unpack_from("<4H", envs, i * SZ_ENV)
        n = envs[i * SZ_ENV + 8]
        print("   env %-3d numIndices=%d -> %s" % (i, n, list(idx[:n]) if n else "(none)"))
    if ne > 16:
        print("   ... %d more" % (ne - 16))
