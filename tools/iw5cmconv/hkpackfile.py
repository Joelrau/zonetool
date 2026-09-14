"""
Havok binary packfile (.hkx) reader/writer for hk_2014.2.5-r1, 64-bit little-endian.

Container layout is taken from the genuine Havok SDK headers

    Source/Common/Serialize/Packfile/Binary/hkPackfileHeader.h
    Source/Common/Serialize/Packfile/Binary/hkPackfileSectionHeader.h

(hk2014_1_0_r1, build #20140907). Those two structs are unchanged in 2014.2.5-r1:
verified by byte-exact round-trip against shipped IW7 files

    mp_paris.d3dbsp.colmap.hkx       world collision, 13,066,464 bytes
    mp_paris.d3dbsp.ents.data.hkx    map-ents collision
    physicsasset/*.hkx               per-model physics

This module handles the *container* only. The object payload inside __data__ is
opaque here -- see docs/iw7-havok-collision.md for what is and is not known
about hknpCompressedMeshShape.
"""

import struct
from dataclasses import dataclass, field
from typing import List, Tuple

MAGIC0 = 0x57E0E057
MAGIC1 = 0x10C0C010
HEADER_SIZE = 64
SECTION_HEADER_SIZE = 64          # char[19] tag + char nullByte + 7*int32 + int32 pad[4]
CONTENTS_VERSION_IW7 = b"hk_2014.2.5-r1"


@dataclass
class Layout:
    """hkPackfileHeader::m_layoutRules -- IW7 ships (8, 1, 0, 1)."""
    pointer_size: int = 8
    little_endian: int = 1
    reuse_base_class_padding: int = 0
    empty_base_class_optimization: int = 1

    def pack(self) -> bytes:
        return struct.pack("<4B", self.pointer_size, self.little_endian,
                           self.reuse_base_class_padding,
                           self.empty_base_class_optimization)


@dataclass
class Section:
    tag: str
    absolute_data_start: int
    local_fixups_offset: int
    global_fixups_offset: int
    virtual_fixups_offset: int
    exports_offset: int
    imports_offset: int
    end_offset: int
    data: bytes = b""

    @property
    def data_size(self) -> int:
        return self.local_fixups_offset


@dataclass
class Packfile:
    user_tag: int = 0
    file_version: int = 11
    layout: Layout = field(default_factory=Layout)
    contents_section_index: int = 0
    contents_section_offset: int = 0
    contents_class_name_section_index: int = 0
    contents_class_name_section_offset: int = 0
    contents_version: bytes = CONTENTS_VERSION_IW7
    flags: int = 0
    max_predicate: int = 21
    predicate_array_size_plus_padding: int = 0
    sections: List[Section] = field(default_factory=list)

    # ------------------------------------------------------------------ read

    @classmethod
    def parse(cls, blob: bytes) -> "Packfile":
        m0, m1, user_tag, file_version = struct.unpack_from("<IIiI", blob, 0)
        if m0 != MAGIC0 or m1 != MAGIC1:
            raise ValueError("not a Havok binary packfile (magic %08X %08X)" % (m0, m1))
        ptr_size, little_endian, reuse_pad, ebco = struct.unpack_from("<4B", blob, 16)
        num_sections, csi, cso, ccnsi, ccnso = struct.unpack_from("<5i", blob, 20)
        contents_version = blob[40:56].split(b"\0")[0]
        flags, max_predicate, predpad = struct.unpack_from("<iHH", blob, 56)

        pf = cls(user_tag=user_tag,
                 file_version=file_version,
                 layout=Layout(ptr_size, little_endian, reuse_pad, ebco),
                 contents_section_index=csi,
                 contents_section_offset=cso,
                 contents_class_name_section_index=ccnsi,
                 contents_class_name_section_offset=ccnso,
                 contents_version=contents_version,
                 flags=flags,
                 max_predicate=max_predicate,
                 predicate_array_size_plus_padding=predpad)

        for i in range(num_sections):
            base = HEADER_SIZE + i * SECTION_HEADER_SIZE
            tag = blob[base:base + 19].split(b"\0")[0].decode("ascii")
            vals = struct.unpack_from("<7i", blob, base + 20)
            sec = Section(tag, *vals)
            start = sec.absolute_data_start
            sec.data = blob[start:start + sec.end_offset]
            pf.sections.append(sec)
        return pf

    @classmethod
    def load(cls, path: str) -> "Packfile":
        with open(path, "rb") as fh:
            return cls.parse(fh.read())

    # ------------------------------------------------------- class name table

    def class_names(self) -> List[Tuple[int, int, str]]:
        """[(name_offset, signature, name)] parsed out of __classnames__.

        Record format is { uint32 signature; uint8 0x09; char name[]; '\\0' } -- note the
        0x09 separator between the signature and the string. name_offset points at the
        first character of the name, i.e. past the separator, which is what
        m_contentsClassNameSectionOffset holds.
        """
        sec = self.sections[self.contents_class_name_section_index]
        blob = sec.data[:sec.data_size]
        out, p = [], 0
        while p + 6 <= len(blob):
            if blob[p:p + 1] == b"\xff":
                break
            sig = struct.unpack_from("<I", blob, p)[0]
            name_at = p + 5                       # skip the 0x09 separator
            end = blob.find(b"\0", name_at)
            if end < 0:
                break
            out.append((name_at, sig, blob[name_at:end].decode("ascii", "replace")))
            p = end + 1
        return out

    def _name_at(self, section_index: int, offset: int) -> str:
        sec = self.sections[section_index]
        return sec.data[offset:offset + 128].split(b"\0")[0].decode("ascii", "replace")

    def root_class_name(self) -> str:
        return self._name_at(self.contents_class_name_section_index,
                             self.contents_class_name_section_offset)

    # ----------------------------------------------------------- fixup tables

    def local_fixups(self, sec: Section) -> List[Tuple[int, int]]:
        """[(src_offset, dst_offset)] -- intra-section pointer patches."""
        out = []
        for p in range(sec.local_fixups_offset, sec.global_fixups_offset, 8):
            src, dst = struct.unpack_from("<2i", sec.data, p)
            if src != -1:
                out.append((src, dst))
        return out

    def _triples(self, sec: Section, start: int, stop: int):
        out = []
        for p in range(start, stop, 12):
            if p + 12 > len(sec.data):
                break
            vals = struct.unpack_from("<3i", sec.data, p)
            if vals[0] != -1:
                out.append(vals)
        return out

    def global_fixups(self, sec: Section):
        """[(src_offset, dst_section_index, dst_offset)]."""
        return self._triples(sec, sec.global_fixups_offset, sec.virtual_fixups_offset)

    def virtual_fixups(self, sec: Section):
        """[(object_offset, classname_section_index, classname_offset)] -- object table."""
        return self._triples(sec, sec.virtual_fixups_offset, sec.exports_offset)

    def objects(self) -> List[Tuple[int, int, str]]:
        """[(section_index, object_offset, class_name)] for every reflected object."""
        out = []
        for si, sec in enumerate(self.sections):
            for obj_off, cn_si, cn_off in self.virtual_fixups(sec):
                out.append((si, obj_off, self._name_at(cn_si, cn_off)))
        return out

    # ----------------------------------------------------------------- write

    def build(self) -> bytes:
        """Re-serialize. Section payloads are written verbatim at their recorded
        absolute offsets, so parse() -> build() is byte-exact."""
        out = bytearray()
        out += struct.pack("<IIiI", MAGIC0, MAGIC1, self.user_tag, self.file_version)
        out += self.layout.pack()
        out += struct.pack("<5i", len(self.sections),
                           self.contents_section_index,
                           self.contents_section_offset,
                           self.contents_class_name_section_index,
                           self.contents_class_name_section_offset)
        # hkPackfileHeader() does memSet(this, -1, sizeof(*this)) before writing
        # the version string, so the bytes after the NUL stay 0xFF -- not zero.
        cv = self.contents_version[:15]
        out += cv + b"\0" + b"\xff" * (16 - len(cv) - 1)
        out += struct.pack("<iHH", self.flags, self.max_predicate,
                           self.predicate_array_size_plus_padding)
        assert len(out) == HEADER_SIZE, len(out)

        for sec in self.sections:
            tag = sec.tag.encode("ascii")[:19]
            out += tag + b"\0" * (19 - len(tag))
            out += b"\xff"                                   # m_nullByte
            out += struct.pack("<7i", sec.absolute_data_start,
                               sec.local_fixups_offset, sec.global_fixups_offset,
                               sec.virtual_fixups_offset, sec.exports_offset,
                               sec.imports_offset, sec.end_offset)
            out += b"\xff" * 16                              # int32 m_pad[4]

        for sec in self.sections:
            if len(out) < sec.absolute_data_start:
                out += b"\xff" * (sec.absolute_data_start - len(out))
            end = sec.absolute_data_start + sec.end_offset
            out[sec.absolute_data_start:end] = sec.data[:sec.end_offset]
        return bytes(out)

    def save(self, path: str) -> None:
        with open(path, "wb") as fh:
            fh.write(self.build())
