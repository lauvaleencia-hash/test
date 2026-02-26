# Arquitectura y mantenimiento

## Diseño escalable

1. **Capa host (`PluginProcessor`)**
   - Orquesta routing, parámetros, latencia, estados.
   - No implementa DSP complejo directo, sólo coordinación.

2. **Capa DSP (`Source/DSP`)**
   - Cada motor encapsulado y testeable de forma aislada.
   - Contrato homogéneo: `prepare`, `setParameters`, `process`/`processSample`.

3. **Capa UI (`PluginEditor`)**
   - Separada de DSP.
   - Attachments APVTS directos para automatización/recall robusto.

## Expansión futura recomendada

- Añadir `PresetManager` con `ValueTree` para browser con tags.
- Sustituir `SpectralEngine` por STFT overlap-add con ventanas Hann y hop variable.
- Añadir moduladores sincronizados a tempo host (`AudioPlayHead::CurrentPositionInfo`).
- Implementar matriz visual de modulación basada en grafo (source -> destination).
- Agregar pruebas unitarias de DSP (por bloque) con JUCE UnitTest o Catch2.

## Estrategia comercial

- Mantener ABI estable para sesiones guardadas (IDs de parámetros inmutables).
- Versionar presets por schema.
- Añadir telemetría opcional sólo si se requiere (off por defecto, sin red en audio thread).
