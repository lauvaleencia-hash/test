# FRACTURE BLOOM

Plugin creativo JUCE (VST3/AU) orientado a mutación temporal-espectral musical.

## Estructura del proyecto

- `CMakeLists.txt`: configuración de build JUCE + targets AU/VST3.
- `Source/PluginProcessor.*`: host del plugin, parámetros APVTS, pipeline DSP.
- `Source/PluginEditor.*`: UI escalable oscura con macro principal y animación.
- `Source/DSP/GranularEngine.h`: motor granular con freeze/scrub/pitch/jitter.
- `Source/DSP/SpectralEngine.h`: resíntesis FFT con blur/stretch/bloom/shift.
- `Source/DSP/DistortionEngine.h`: distorsión híbrida (wavefold + saturación dependiente de envolvente).
- `Source/DSP/ModulationEngine.h`: 3 LFO + 2 followers + caos logístico.
- `Docs/QA_CHECKLIST.md`: checklist de validación VST3/AU.
- `Docs/ARCHITECTURE.md`: escalabilidad y mantenimiento.
- `Presets/FACTORY_PRESETS.md`: organización de presets de fábrica.

## Compatibilidad objetivo

- Formatos: VST3 y AU.
- Hosts: últimas versiones de Ableton Live.
- Plataformas: Windows 10/11 (x64), macOS Intel + Apple Silicon (universal).
- Sample rates: 44.1 kHz–192 kHz.
- 64-bit only.
- Automatización completa vía APVTS.

## Build rápido (CMake)

### Requisitos

1. CMake 3.22+
2. Compilador C++20
   - macOS: Xcode 15+
   - Windows: Visual Studio 2022 + Desktop C++
3. Git (para descargar JUCE vía FetchContent)

### macOS (Xcode)

```bash
cd /ruta/a/FRACTURE_BLOOM
cmake -S . -B build -G Xcode
cmake --build build --config Release
```

Salidas esperadas:

- `build/FRACTURE_BLOOM_artefacts/Release/AU/FRACTURE BLOOM.component`
- `build/FRACTURE_BLOOM_artefacts/Release/VST3/FRACTURE BLOOM.vst3`

### Windows (Visual Studio 2022)

```powershell
cd C:\ruta\FRACTURE_BLOOM
cmake -S . -B build -G "Visual Studio 17 2022" -A x64
cmake --build build --config Release
```

Salida esperada:

- `build/FRACTURE_BLOOM_artefacts/Release/VST3/FRACTURE BLOOM.vst3`

## Instalación del VST3/AU final

- macOS VST3: copiar a `~/Library/Audio/Plug-Ins/VST3/`
- macOS AU: copiar a `~/Library/Audio/Plug-Ins/Components/`
- Windows VST3: copiar a `C:\Program Files\Common Files\VST3\`

Luego reescanear plugins en Ableton Live.

## Notas técnicas

- Latencia reportada: `FFT_SIZE/2` para la sección espectral.
- Estado y recall DAW: serialización XML de `AudioProcessorValueTreeState`.
- Seguridad de hilos:
  - Parámetros en `std::atomic<float>` vía APVTS raw values.
  - Sin asignaciones dinámicas en `processBlock` (buffers preparados en `prepareToPlay`).
- CPU:
  - flujo modular para futura vectorización SIMD en bloques internos.

