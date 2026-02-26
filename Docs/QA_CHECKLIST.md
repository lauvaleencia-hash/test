# QA Checklist VST3/AU

## Sanidad general

- [ ] Carga/descarga plugin sin crash en Ableton Live (macOS y Windows).
- [ ] Cambio de sample rate en runtime (44.1/48/96/192 kHz) sin artefactos críticos.
- [ ] Cambio de buffer size sin clicks permanentes.
- [ ] Bypass DAW limpio (nivel consistente).

## Automatización y estado

- [ ] Todos los parámetros aparecen con nombre correcto en DAW.
- [ ] Escritura/lectura de automatización suave (sin zippering severo).
- [ ] Guardar/cerrar/reabrir proyecto conserva estado exacto.
- [ ] Instancias múltiples independientes (sin contaminación de estado).

## DSP

- [ ] Freeze mantiene textura estable.
- [ ] Scrub recorre memoria granular sin discontinuidades extremas.
- [ ] Spectral blur/stretch funcionan en material tonal y percusivo.
- [ ] Distorsión no produce NaN/Inf con niveles altos.
- [ ] Macro escala intensidad global musicalmente.
- [ ] Mix 0% = dry y Mix 100% = wet.

## Performance

- [ ] CPU < 5% por instancia en 48 kHz / 512 samples (objetivo base).
- [ ] Sin memory leaks al cerrar host (validar con Instruments/ASan/CRT).
- [ ] Sin asignaciones en audio thread (profiling).

## Formato / distribución

- [ ] VST3 valida con `pluginval`.
- [ ] AU valida con `auval` en macOS.
- [ ] Metadata correcta (fabricante, versión, categorías).
