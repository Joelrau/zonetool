# Dump struct layouts from the IW7 database's type system as JSON.
# Usage: idat.exe -A -Lx.log -S"dump_types.py <out.json> <Type1,Type2,...>" <db.i64>
#
# The IW7 database carries hand-built, correct IW7 struct definitions, so its type library is
# the authority on offsets and sizes - better than re-deriving them from the C++ headers.
import idaapi, idc, ida_typeinf
import json

OUT = idc.ARGV[1]
ROOTS = [t.strip() for t in idc.ARGV[2].split(",") if t.strip()]

til = ida_typeinf.get_idati()
out = {"sizes": {}, "layout": {}}
seen = set()


def get_tinfo(name):
    ti = ida_typeinf.tinfo_t()
    if ti.get_named_type(til, name):
        return ti
    return None


def flatten(ti, prefix, base, rows, depth=0):
    """Flatten a struct into (path, offset, size, type) rows, expanding nested structs."""
    if depth > 6:
        return
    udt = ida_typeinf.udt_type_data_t()
    if not ti.get_udt_details(udt):
        return
    for m in udt:
        off = base + m.offset // 8
        mt = m.type
        nm = prefix + m.name
        sz = mt.get_size()
        rows.append({"name": nm, "offset": off, "size": sz, "type": str(mt)})
        # expand a nested struct that is not a pointer and not an array of structs
        if mt.is_struct() and not mt.is_ptr():
            flatten(mt, nm + ".", off, rows, depth + 1)


def record(name):
    if name in seen:
        return
    seen.add(name)
    ti = get_tinfo(name)
    if ti is None:
        out["sizes"][name] = None
        return
    out["sizes"][name] = ti.get_size()
    rows = []
    flatten(ti, "", 0, rows)
    if rows:
        out["layout"][name] = rows


for r in ROOTS:
    record(r)

# also record the size of every distinct struct type referenced by the roots, so the
# .gfxmap parser knows how many bytes each dumped array element takes
extra = set()
for name, rows in out["layout"].items():
    for row in rows:
        t = row["type"].replace("const ", "").strip()
        t = t.split("[")[0].strip()
        if t.endswith("*"):
            t = t[:-1].strip()
        if t and t[0].isalpha() and "::" not in t:
            extra.add(t)
for t in sorted(extra):
    if t in seen:
        continue
    ti = get_tinfo(t)
    if ti is not None:
        out["sizes"][t] = ti.get_size()

with open(OUT, "w", encoding="utf-8") as fh:
    json.dump(out, fh, indent=1)
print("[types] %d roots, %d sizes -> %s" % (len(ROOTS), len(out["sizes"]), OUT))
idc.qexit(0)
