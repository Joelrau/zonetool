# Headless IDA export: one JSONL record per function.
# Usage: idat.exe -A -Lxx.log -S"export_funcs.py <outpath>" <db.i64>
import idaapi, idautils, idc, ida_funcs, ida_bytes, ida_nalt, ida_name, ida_ua, ida_segment, ida_pro
import json, sys, os, hashlib, time

OUT = idc.ARGV[1] if len(idc.ARGV) > 1 else "funcs.jsonl"

# auto_wait skipped: the database is already fully analysed and the post-upgrade
# reanalysis queue costs an hour we do not need for a read-only export.

# ---- instruction class map for the control-flow "shape" signature -------------
CLS = {}
for m in ("call",):                                   CLS[m] = "C"
for m in ("jmp",):                                    CLS[m] = "J"
for m in ("ret", "retn", "leave"):                    CLS[m] = "R"
for m in ("mov","movzx","movsx","movsxd","lea","movaps","movups","movsd","movss","movdqa","movdqu","movq","movd"): CLS[m] = "M"
for m in ("add","sub","imul","mul","idiv","div","and","or","xor","not","neg","shl","shr","sar","inc","dec","adc","sbb"): CLS[m] = "A"
for m in ("cmp","test","ucomiss","ucomisd","comiss","comisd"): CLS[m] = "T"
for m in ("push","pop"):                              CLS[m] = "S"

def icls(m):
    c = CLS.get(m)
    if c: return c
    if m.startswith("j"):   return "B"   # conditional branch
    if m.startswith("set"): return "E"
    if m.startswith("cmov"):return "V"
    return "O"

strcache = {}
PRINTABLE = set(range(0x20, 0x7F)) | {0x09, 0x0A, 0x0D}


def get_str(ea):
    """Contents of the string literal at ea, or None.

    get_str_type takes an ADDRESS, not flags - passing flags silently yields nothing,
    which is how an earlier version of this export produced zero strings on both
    binaries. The unmarked-data fallback matters for the IW7 memory dump, where plenty
    of referenced literals were never turned into strlit items by auto-analysis.
    """
    if ea in strcache:
        return strcache[ea]
    s = None
    try:
        fl = ida_bytes.get_flags(ea)
        if ida_bytes.is_strlit(fl):
            t = ida_nalt.get_str_type(ea)
            if t is None or t == -1:
                t = ida_nalt.STRTYPE_C
            b = ida_bytes.get_strlit_contents(ea, -1, t)
            if b:
                s = b.decode("utf-8", "replace")
        elif not ida_bytes.is_code(fl):
            b = ida_bytes.get_strlit_contents(ea, -1, ida_nalt.STRTYPE_C)
            if b and 4 <= len(b) <= 400 and all(c in PRINTABLE for c in b):
                s = b.decode("ascii", "replace")
    except Exception:
        s = None
    if s is not None and (len(s) < 4 or len(s) > 400):
        s = None
    strcache[ea] = s
    return s


MININT = 0x100          # ignore tiny immediates (too common to discriminate)

def export():
    t0 = time.time()
    fh = open(OUT, "w", encoding="utf-8")
    n = 0
    insn = ida_ua.insn_t()
    for fea in idautils.Functions():
        f = ida_funcs.get_func(fea)
        if not f: continue
        name  = ida_funcs.get_func_name(fea) or ""
        flags = ida_bytes.get_flags(fea)
        user  = bool(ida_bytes.has_user_name(flags))
        mnems, shape = [], []
        hist = {}
        strs, consts, callees, datarefs = [], [], [], []
        ncall = nbr = ncbr = 0
        try:
            items = list(idautils.FuncItems(fea))
        except Exception:
            items = []
        for ea in items:
            if ida_ua.decode_insn(insn, ea) == 0: continue
            m = insn.get_canon_mnem() or ""
            mnems.append(m); shape.append(icls(m))
            hist[m] = hist.get(m, 0) + 1
            if idaapi.is_call_insn(insn):
                ncall += 1
                for xr in idautils.CodeRefsFrom(ea, 0):
                    callees.append(xr)
            elif m == "jmp": nbr += 1
            elif m.startswith("j"): ncbr += 1
            for op in insn.ops:
                if op.type == ida_ua.o_void: break
                if op.type == ida_ua.o_imm:
                    v = op.value & 0xFFFFFFFFFFFFFFFF
                    if v >= MININT: consts.append(v)
            for dr in idautils.DataRefsFrom(ea):
                s = get_str(dr)
                if s is not None: strs.append(s)
                else: datarefs.append(dr)
        rec = {
            "ea": fea,
            "name": name,
            "user": user,
            "size": f.end_ea - f.start_ea,
            "thunk": bool(f.flags & ida_funcs.FUNC_THUNK),
            "lib": bool(f.flags & ida_funcs.FUNC_LIB),
            "proto": idc.get_type(fea) or "",
            "ni": len(mnems),
            "ncall": ncall, "njmp": nbr, "ncjmp": ncbr,
            "hist": hist,
            "exact": hashlib.sha1(",".join(mnems).encode()).hexdigest()[:16],
            "shape": hashlib.sha1("".join(shape).encode()).hexdigest()[:16],
            "shapestr": "".join(shape)[:512],
            "strs": sorted(set(strs))[:64],
            "consts": sorted(set(consts))[:64],
            "callees": sorted(set(callees)),
            "drefs": sorted(set(datarefs))[:64],
        }
        fh.write(json.dumps(rec) + "\n")
        n += 1
        if n % 5000 == 0:
            print("[export] %d funcs  %.1fs" % (n, time.time() - t0))
            fh.flush()
    fh.close()
    print("[export] DONE %d funcs in %.1fs -> %s" % (n, time.time() - t0, OUT))

try:
    export()
except Exception as e:
    import traceback; traceback.print_exc()
idc.qexit(0)
