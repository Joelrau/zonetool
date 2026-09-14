"""Minimal MSVC symbol reader.

Not a demangler - it only recovers the qualified *name* of a symbol, which is all the
matcher needs for comparing an IW7 symbol against an IW8 one and for printing a readable
report. IDA hands back mangled names on the IW8 side and mostly plain ones on the IW7
side, so comparing them raw reports false disagreements.
"""
import re

TEMPLATE_RE = re.compile(r"@\?\$")


def qualified_name(sym):
    """'?pump@bdLobbyConnection@@QEAA_NXZ' -> 'bdLobbyConnection::pump'."""
    if not sym:
        return ""
    s = sym
    # IDA thunk / import decorations
    while s.startswith(("j_", "__imp_")):
        s = s[2:] if s.startswith("j_") else s[6:]
    if not s.startswith("?"):
        return s
    body = s[1:]
    # the name part ends at the first '@@'
    cut = body.find("@@")
    if cut < 0:
        return s
    parts = [p for p in body[:cut].split("@") if p]
    if not parts:
        return s
    # template arguments show up as '?$Name' - keep just the template's own name
    parts = [p[2:] if p.startswith("?$") else p for p in parts]
    return "::".join(reversed(parts))


def norm(sym):
    """Comparison key: qualified name, case-folded, IDA uniquifier suffix removed."""
    n = qualified_name(sym)
    n = re.sub(r"_\d+$", "", n)
    return n.lower()
