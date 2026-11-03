# microui (vendored)

- Upstream: https://github.com/rxi/microui
- Version: 2.02, commit `0850aba860959c3e75fb3e97120ca92957f9d057` (2024-08-13)
- License: MIT (see `LICENSE`)

Third-party code: kept **unmodified** so it can be updated from upstream and so fixes can be
contributed back. TookOne's own controls (menu bar, toolbar, palette, film strip, canvas…) live
in `ui/`, not here. `docs/c-code-stlye.md` does not apply to this folder.

Known issue: `microui.c:873` casts to `long long`, which is not C89 (`gcc -std=c89 -pedantic`
warns; OpenWatcom accepts it). Candidate for a bug report upstream.
