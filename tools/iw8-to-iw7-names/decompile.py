# Decompile a fixed list of addresses and write them to one file.
# Usage: idat.exe -A -Lxx.log -S"decompile.py <out.c> <ea>[,<ea>...]" <db.i64>
import idaapi, idc, ida_hexrays, ida_funcs, ida_name
import sys

OUT = idc.ARGV[1]
EAS = [int(x, 0) for x in idc.ARGV[2].split(",") if x.strip()]

if not ida_hexrays.init_hexrays_plugin():
    print("[dec] hex-rays not available")
    idc.qexit(1)

fh = open(OUT, "w", encoding="utf-8")
for ea in EAS:
    f = ida_funcs.get_func(ea)
    name = ida_funcs.get_func_name(ea) or ("sub_%X" % ea)
    fh.write("\n/* ===================== %s @ %X ===================== */\n" % (name, ea))
    if not f:
        fh.write("/* no function at this address */\n")
        continue
    try:
        cf = ida_hexrays.decompile(f.start_ea)
        if cf is None:
            fh.write("/* decompilation returned None */\n")
        else:
            fh.write(str(cf) + "\n")
    except Exception as e:
        fh.write("/* decompile failed: %r */\n" % (e,))
    print("[dec] %s" % name)
fh.close()
print("[dec] wrote %s" % OUT)
idc.qexit(0)
