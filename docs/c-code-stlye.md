# Guía de estilo C

La guía oficial vive en las reglas de los agentes, para que haya una sola copia:

- [`.agents/rules/c-code-stlye.md`](../.agents/rules/c-code-stlye.md): estilo C89, nombres, errores y logging,
  módulos por responsabilidad, orden de los archivos (de afuera hacia adentro), tests `STANDALONE`, alcance.
- [`.agents/rules/layers.md`](../.agents/rules/layers.md): jerarquía de módulos (quién puede incluir a quién).

Verificación:

```bash
python3 .agents/skills/c89-conformance/scripts/check_c89.py --target all   # C89 en Linux y DOS
python3 .agents/skills/layer-check/scripts/check_layers.py                 # jerarquía de módulos
```
