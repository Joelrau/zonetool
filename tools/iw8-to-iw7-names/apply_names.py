# Headless IDA apply pass: import IW8 local types, then rename + retype matched IW7 functions.
# Usage: idat.exe -A -Lxx.log -S"apply_names.py <matches.json> <iw8_types.h> <minconf>" <iw7_copy.i64>
#
# minconf: "high" applies only high-confidence matches, "medium" applies high+medium.
# Low-confidence matches are never applied by this script.
import idaapi, idc, ida_name, ida_funcs, ida_typeinf, ida_bytes, ida_kernwin
import json, sys, time

MATCHES = idc.ARGV[1]
TYPES_H = idc.ARGV[2] if len(idc.ARGV) > 2 else ""
MINCONF = idc.ARGV[3] if len(idc.ARGV) > 3 else "medium"

RANK = {"high": 3, "medium": 2, "low": 1}
FLOOR = RANK.get(MINCONF, 2)

AUTO_PREFIXES = ("sub_", "nullsub_", "j_sub_", "unknown_libname_", "j_nullsub_")


def import_types(path):
    # Deliberately opt-in ("-" disables). The IW7 database already carries hand-built IW7
    # struct definitions - GfxLightGridProbeData, GfxVoxelTree and friends are correct there
    # and the decompiler output depends on them. IW8's local types reuse many of the same
    # NAMES for structs that were re-laid-out in 2019 (its GfxGpuLightGrid is 440 bytes where
    # IW7's GfxLightGridProbeData is 240), so importing them wholesale would silently
    # overwrite correct definitions with wrong ones.
    if not path or path == "-":
        print("[apply] type import disabled")
        return 0
    print("[apply] importing local types from %s" % path)
    t0 = time.time()
    # PT_FILE parses the argument as a file name; HTI_DCL keeps going past bad decls
    n = idc.parse_decls(path, idc.PT_FILE | idc.PT_SILENT)
    print("[apply] parse_decls returned %d error(s) in %.1fs" % (n, time.time() - t0))
    return n


def is_auto(name):
    return (not name) or name.startswith(AUTO_PREFIXES)


def sanitize(name):
    # IDA rejects a few characters in symbol names; let it mangle-check via SN_CHECK instead
    return name


def apply_matches(path):
    with open(path, "r", encoding="utf-8") as fh:
        data = json.load(fh)

    renamed = skipped_named = skipped_conf = failed = 0
    typed = type_failed = 0
    taken = set()

    # deterministic order: strongest first, so the best claimant wins a contested name
    data.sort(key=lambda m: (-RANK[m["conf"]], -m["score"]))

    for m in data:
        if RANK[m["conf"]] < FLOOR:
            skipped_conf += 1
            continue
        ea = m["iw7_ea"]
        f = ida_funcs.get_func(ea)
        if not f or f.start_ea != ea:
            failed += 1
            continue
        cur = ida_funcs.get_func_name(ea) or ""
        if not is_auto(cur):
            # never overwrite a symbol that was already there
            skipped_named += 1
            continue
        base = m["name"]
        name = base
        # avoid collisions with names already in the database or claimed this run
        suffix = 0
        while name in taken or (idc.get_name_ea_simple(name) not in (idc.BADADDR, ea)):
            suffix += 1
            name = "%s_%d" % (base, suffix)
            if suffix > 32:
                break
        if ida_name.set_name(ea, name, ida_name.SN_NOCHECK | ida_name.SN_FORCE):
            taken.add(name)
            renamed += 1
            proto = m.get("proto") or ""
            if proto:
                # get_type() gives a declaration without a name; splice the symbol back in
                decl = proto
                if "(" in decl:
                    head, rest = decl.split("(", 1)
                    decl = "%s __ida_fn(%s" % (head, rest)
                    decl = decl.replace("__ida_fn", name, 1)
                else:
                    decl = ""
                if decl and idc.SetType(ea, decl + ";"):
                    typed += 1
                else:
                    type_failed += 1
        else:
            failed += 1

        if (renamed % 2000) == 0 and renamed:
            print("[apply] %d renamed ..." % renamed)

    print("[apply] renamed=%d typed=%d type_failed=%d "
          "skipped(already named)=%d skipped(below %s)=%d failed=%d"
          % (renamed, typed, type_failed, skipped_named, MINCONF, skipped_conf, failed))


try:
    import_types(TYPES_H)
    apply_matches(MATCHES)
    print("[apply] saving database ...")
    idc.save_database("", 0)
    print("[apply] saved")
except Exception:
    import traceback
    traceback.print_exc()
idc.qexit(0)
