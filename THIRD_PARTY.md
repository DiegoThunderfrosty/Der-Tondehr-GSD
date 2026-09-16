# Third-party dependencies and attribution

Original Der Tondehr GSD code is distributed under the MIT License, Copyright (c) 2026 Diego Rodriguez. Dependencies and bundled assets retain their original licenses.

## Circuit-family and trademark notice

This project is an independent, heavily modified, circuit-inspired interpretation of a classic soft-clipping overdrive topology commonly associated with the Tube Screamer family. It is not affiliated with, sponsored by, endorsed by, or approved by Ibanez, Hoshino Gakki, Maxon, or any other pedal manufacturer.

Third-party product and circuit-family names are used only for limited historical and descriptive identification. They are not part of the plugin's name, repository name, binaries, or visual branding. All related trademarks belong to their respective owners.

The repository does not contain third-party pedal firmware, commercial-plugin code, decompiled software, manuals, circuit drawings, logos, recordings, or captured impulse responses.

## iPlug2

iPlug2 provides the plugin wrapper and graphics framework. The compatible revision is pinned in `scripts/setup_dependencies.ps1`:

```text
b64192fe18afd9bc9a1fe324db5aceb48f4a0eee
```

iPlug2 uses a permissive zlib-style license. A copy is preserved in `docs/licenses/iPlug2-LICENSE.txt`; the downloaded checkout retains notices for its transitive components.

## VST3 SDK

The pinned iPlug2 revision contains the VST3 SDK source required by this build. The included SDK license identifies it as MIT-licensed. A copy is preserved in `docs/licenses/VST3-LICENSE.txt`.

## Roboto

The user interface embeds `Roboto-Regular.ttf`, matching the copy distributed with the pinned iPlug2/NanoVG example resources. It is distributed under the SIL Open Font License 1.1. The license text is preserved in `docs/licenses/Roboto-OFL-1.1.txt`.

## Build-time dependencies

On Windows, the pinned iPlug2 CMake integration may download Microsoft WIL and the Microsoft WebView2 SDK while configuring framework targets. Their source packages and notices remain in the generated build/dependency directories and are not committed to this repository. Anyone redistributing binaries is responsible for retaining every notice required by the components actually linked into that distribution.

## Transitive components

iPlug2 includes or can use components such as Cockos WDL, NanoVG, NanoSVG, HIIR, RtAudio, and RtMidi. Their licenses remain in the downloaded iPlug2 checkout. The top-level MIT License covers only the original Der Tondehr GSD code; it does not replace dependency, font, or trademark terms.
