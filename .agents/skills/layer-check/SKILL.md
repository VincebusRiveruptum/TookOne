---
name: layer-check
description: >-
  Use this skill to verify that C modules respect the module hierarchy defined in
  .agents/rules/layers.md: a module only includes lower-level modules, siblings never
  include each other, private.h stays inside its module and platform backends are
  never included. Run it after adding modules, moving files or changing includes.
---

# Layer Hierarchy Check

The hierarchy is defined in [`.agents/rules/layers.md`](../../rules/layers.md), in the fenced block
marked `layers`. That block is the single source of truth: change levels there, not in the script.

## Run

From the project root:

```bash
python3 .agents/skills/layer-check/scripts/check_layers.py
```

Exit code: `0` when there are no violations, `1` otherwise.

## What it reports

| Code | Meaning | Typical fix |
|---|---|---|
| `upward` | a module includes one of a higher level | move the shared code down, or let the upper level coordinate |
| `sibling` | a module includes another of the same level | let the level above coordinate both |
| `private` | a file includes another module's `private.h` | expose what is needed in that module's public header |
| `platform` | a file includes a platform backend | include `hal/` instead |
| `unassigned` | a C file belongs to no module of the table | add the module to `layers.md`, or move the file |

`external/` code is not checked (third party), but other modules may include it at level 0.
