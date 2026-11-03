#!/usr/bin/env python3
"""
check_layers.py - verifies the module hierarchy of .agents/rules/layers.md.

Reads the ```layers block of layers.md, maps every C file to a module and checks
every #include "..." against the rules. Exit code 0 = clean, 1 = violations.
"""

import os
import re
import sys

RULES = os.path.join(".agents", "rules", "layers.md")
SKIP_DIRS = {".git", "build", "bin", "legacy", ".agents"}
INCLUDE = re.compile(r'^\s*#\s*include\s+"([^"]+)"')


def find_root():
    cur = os.path.abspath(os.getcwd())
    while cur != os.path.dirname(cur):
        if os.path.exists(os.path.join(cur, RULES)):
            return cur
        cur = os.path.dirname(cur)
    sys.exit("check_layers: %s not found (run from the project)" % RULES)


def read_table(root):
    """Returns [(level, pattern)] from the ```layers fenced block."""
    text = open(os.path.join(root, RULES), encoding="utf-8").read()
    match = re.search(r"```layers\n(.*?)```", text, re.DOTALL)
    if not match:
        sys.exit("check_layers: no ```layers block in %s" % RULES)
    table = []
    for line in match.group(1).splitlines():
        line = line.split("#", 1)[0].strip()
        if not line:
            continue
        level, pattern = line.split(None, 1)
        table.append((level, pattern.strip().rstrip("/")))
    return table


def expand_modules(root, table):
    """Returns {module_path: level}; 'deps/*' expands to each subfolder."""
    modules = {}
    for level, pattern in table:
        if pattern.endswith("/*"):
            base = pattern[:-2]
            full = os.path.join(root, base)
            if os.path.isdir(full):
                for name in sorted(os.listdir(full)):
                    if os.path.isdir(os.path.join(full, name)):
                        modules[base + "/" + name] = level
        else:
            modules[pattern] = level
    return modules


def module_of(path, modules):
    best = None
    for mod in modules:
        if path == mod or path.startswith(mod + "/"):
            if best is None or len(mod) > len(best):
                best = mod
    return best


def c_files(root):
    for dirpath, dirnames, filenames in os.walk(root):
        dirnames[:] = [d for d in dirnames if d not in SKIP_DIRS]
        for name in filenames:
            if name.endswith((".c", ".h")):
                yield os.path.relpath(os.path.join(dirpath, name), root)


def check(root):
    modules = expand_modules(root, read_table(root))
    problems = []

    for path in sorted(c_files(root)):
        src = module_of(path, modules)
        if src and src.startswith("external/"):
            continue                        # third party: not checked
        if src is None:
            problems.append(("unassigned", path, 0, "file belongs to no module"))
            continue
        src_level = modules[src]

        lines = open(os.path.join(root, path), encoding="latin-1").read().split("\n")
        for num, line in enumerate(lines, 1):
            m = INCLUDE.match(line)
            if not m:
                continue
            target = os.path.normpath(os.path.join(os.path.dirname(path), m.group(1)))
            dst = module_of(target, modules)
            if dst is None or dst == src:
                continue
            dst_level = modules[dst]
            what = "%s (%s, level %s) -> %s (%s, level %s)" % (
                path, src, src_level, m.group(1), dst, dst_level)

            if os.path.basename(target) == "private.h":
                problems.append(("private", path, num, what))
            elif dst_level == "P":
                problems.append(("platform", path, num, what))
            elif src_level == "P":
                if not (dst == "hal" or int(dst_level) <= 1):
                    problems.append(("upward", path, num, what))
            elif int(dst_level) == int(src_level):
                problems.append(("sibling", path, num, what))
            elif int(dst_level) > int(src_level):
                problems.append(("upward", path, num, what))
    return problems


def main():
    root = find_root()
    problems = check(root)
    print("Layer check: %s" % RULES)
    if not problems:
        print("[PASS] No hierarchy violations.")
        return 0
    for code, path, num, msg in problems:
        where = "%s:%d" % (path, num) if num else path
        print("  [%s] %s: %s" % (code, where, msg))
    print("[FAIL] %d violation(s)." % len(problems))
    return 1


if __name__ == "__main__":
    sys.exit(main())
