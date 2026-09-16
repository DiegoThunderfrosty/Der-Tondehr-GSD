# Der Tondehr GSD

Der Tondehr GSD is an independent, open-source overdrive audio plugin built around a heavily modified, circuit-inspired interpretation of the classic soft-clipping overdrive topology commonly associated with the Tube Screamer family.

It is not a software clone of a commercial plugin and is not presented as an exact digital replica of a particular pedal revision or physical unit. The implementation uses its own numerical model, calibration, stability handling, oversampling system, control laws, user interface, and high-frequency behavior.

This is an unofficial technical and creative project. It is not affiliated with, sponsored by, endorsed by, or approved by Ibanez, Hoshino Gakki, Maxon, or any other pedal manufacturer. Third-party product and circuit-family names are used only where necessary to describe the historical circuit inspiration. They are not part of this plugin's name or branding, and all related marks belong to their respective owners.

## Features

- Mono VST3 overdrive for Windows x64.
- Drive, Tone, and Level controls.
- Deterministic pedal on/off switching with a true dry bypass path.
- Selectable 1x, 2x, 4x, and 8x nonlinear-stage oversampling.
- Frequency-dependent feedback network and nonlinear diode-feedback solver.
- Asymmetric clipping calibration.
- Active tone-stage model derived from the circuit topology rather than a generic post-distortion EQ.
- Input coupling and subsonic filtering.
- High-order anti-alias filtering designed to preserve the audible passband.
- Smoothed controls to reduce zipper noise during automation.
- Protection against non-finite samples and unstable state recovery.
- Custom pedal-style iPlug2 interface.

## What is modified

GSD retains the broad signal-flow ideas of a classic feedback-clipping overdrive, but it deliberately departs from a literal stock-pedal model. The current implementation includes independently calibrated diode equations, drive taper, internal level scaling, output compensation, numerical integration, solver safeguards, high-frequency extension, selectable oversampling, and a redesigned interface.

The result should therefore be described as **Tube Screamer-inspired** or **derived from a heavily modified soft-clipping overdrive topology**, not as an exact TS808, TS9, or commercial-plugin clone.

For a technical overview, see [ARCHITECTURE.md](ARCHITECTURE.md).

## Controls

| Control | Function |
| --- | --- |
| Pedal On / Off | Enables the modeled overdrive path or passes the dry input. |
| Drive | Changes the nonlinear feedback gain and saturation behavior. |
| Tone | Adjusts the active tone network. |
| Level | Controls the output level independently from the Drive control. |
| OS | Selects 1x, 2x, 4x, or 8x processing for the nonlinear stage. |

New instances open with Drive 5, Tone 5, Level 10, the pedal enabled, and 8x oversampling.

## Current status

This source package contains version `0.1.4`. The documented target is a Windows x64 VST3 effect with one input and one output channel. No standalone application, installer, factory preset library, audio examples, or precompiled binary is included in the Git repository.

## Repository contents

```text
Der-Tondehr-GSD/
|-- .github/                 Issue, pull-request, and build automation files
|-- docs/licenses/           Required third-party license copies
|-- resources/               Windows resources and the embedded UI font
|-- scripts/                 Dependency setup and Windows build scripts
|-- ARCHITECTURE.md          DSP design and project-scope overview
|-- BUILDING.md              Concise build instructions
|-- CHANGELOG.md             Public version history
|-- CMakeLists.txt           Main CMake configuration
|-- config.h                 Plugin identity and format configuration
|-- CONTRIBUTING.md          Contribution requirements
|-- LICENSE                  MIT License for original project code
|-- README.md                Project overview and full setup guide
|-- SECURITY.md              Vulnerability-reporting policy
|-- THIRD_PARTY.md           Dependency and trademark attribution
|-- TSCProbe.cpp             DSP implementation and editor construction
`-- TSCProbe.h               Processor declarations and circuit state
```

The internal C++ class and source filenames retain the historical name `TSCProbe`. The public product and repository name is Der Tondehr GSD.

Private research material, circuit drawings, comparison renders, DAW sessions, audio files, development notes, local builds, and downloaded dependencies are intentionally excluded. They are not required to compile the project and may carry separate rights or private paths.

## Requirements

1. Windows 10 or Windows 11, 64-bit.
2. Git for Windows.
3. Visual Studio with **Desktop development with C++**.
4. MSVC x64/x86 compiler tools.
5. A recent Windows SDK.
6. CMake tools for Windows.
7. PowerShell 5.1 or later.
8. An Internet connection for the first dependency setup and initial CMake configuration.

The known development environment uses Visual Studio 2026 and CMake 3.25 or later.

## Clean setup and build

Clone the repository:

```powershell
git clone https://github.com/DiegoThunderfrosty/Der-Tondehr-GSD.git
cd Der-Tondehr-GSD
```

Download the pinned framework and VST3 SDK revisions:

```powershell
powershell -NoProfile -ExecutionPolicy Bypass -File .\scripts\setup_dependencies.ps1
```

Build the Release VST3:

```powershell
powershell -NoProfile -ExecutionPolicy Bypass -File .\scripts\build.ps1
```

The expected bundle is:

```text
build\windows\out\DerTondehrGSD.vst3\
```

On Windows, a VST3 is a directory bundle even when File Explorer presents it as one item. Copy the complete directory, not only the module inside `Contents`.

## Installation

1. Close every audio host that may have loaded an earlier build.
2. Copy the complete `DerTondehrGSD.vst3` directory to:

```text
C:\Program Files\Common Files\VST3
```

3. Reopen the host and run a complete plugin rescan.
4. Search for `Der Tondehr GSD` under the manufacturer `Der Tondehr`.

Keep experimental or backup bundles outside all plugin-scan directories. Bundles sharing the same plugin identifier can otherwise be selected unpredictably by a host.

## Updating

```powershell
git pull
powershell -NoProfile -ExecutionPolicy Bypass -File .\scripts\setup_dependencies.ps1
powershell -NoProfile -ExecutionPolicy Bypass -File .\scripts\build.ps1
```

## Troubleshooting

### CMake or the compiler is not found

Open Visual Studio Installer and confirm that the desktop C++ workload, MSVC tools, a Windows SDK, and CMake tools are installed.

### iPlug2 or the VST3 SDK is missing

Run `scripts\setup_dependencies.ps1`. Do not manually create empty dependency directories; the script downloads and validates both repositories at their pinned revisions.

### The plugin is not listed

Confirm that the complete bundle is installed in a path scanned by the host. Remove duplicate development builds from scanned folders, clear any failed-plugin entry, and perform a full rescan.

### A build behaves differently after dependency updates

Return to the pinned dependency revision with `scripts\setup_dependencies.ps1`, use a fresh build directory, and rebuild. Framework changes can affect wrappers, graphics, and host lifecycle behavior.

## Contributions

Read [CONTRIBUTING.md](CONTRIBUTING.md) before submitting changes. Audio changes should include the sample rate, block size, oversampling mode, input level, control positions, and validation method.

## License

Original Der Tondehr GSD code is distributed under the [MIT License](LICENSE), Copyright (c) 2026 Diego Rodriguez.

Dependencies, the embedded font, and referenced trademarks retain their own licenses and ownership. See [THIRD_PARTY.md](THIRD_PARTY.md). The project license does not grant rights over third-party names, marks, documentation, circuit drawings, audio, or software.
