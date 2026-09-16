# Contributing

Thank you for considering a contribution to Der Tondehr GSD.

## Before starting

1. Search for an existing issue covering the change.
2. Open a proposal first if the change affects sound, parameter behavior, saved state, identifiers, latency, or CPU usage.
3. Create a branch from the current main branch.
4. Keep each contribution focused on one concrete problem.

## Building and validation

Follow [BUILDING.md](BUILDING.md) and build the Release VST3.

For DSP changes, report:

- Windows and compiler versions.
- Host and plugin format.
- Sample rate and block size.
- Input level and test signal.
- Drive, Tone, Level, and bypass state.
- Oversampling mode.
- Previous behavior, new behavior, and the validation performed.

At minimum, check 1x, 2x, 4x, and 8x operation; pedal disable/enable; host disable/enable; silence; full-scale input; and ordinary instrument-level input.

## Code rules

- Keep the project compatible with C++17.
- Do not allocate memory, lock, access files, or open UI dialogs on the audio thread.
- Do not change existing parameter order, identifiers, plugin unique ID, or manufacturer ID without an explicit migration plan.
- Keep DSP state and output finite when input or host data is invalid.
- Document constants that are calibration choices rather than values directly derived from the modeled circuit.
- Keep the Drive control distinct from the final Level control.
- Preserve the circuit-inspired feedback topology unless the proposal explicitly introduces a new mode or product.
- Avoid adding a corrective per-frequency harmonic boost as a substitute for fixing the nonlinear algorithm.

## External material and trademarks

Do not add commercial plugin binaries, decompiled code, firmware, manuals, circuit drawings, logos, recordings, DAW sessions, impulse responses, or other files that you are not permitted to redistribute.

Do not put a third-party pedal or manufacturer name in the repository name, plugin name, binary name, visual branding, or topic list. Limited descriptive references to the historical circuit inspiration must remain accurate and include the project's non-affiliation statement.

## Pull requests

Include:

- The concrete problem being solved.
- Previous and new behavior.
- Main files changed.
- Validation performed.
- Known sound, compatibility, performance, or migration risks.

By contributing, you agree that your contribution may be distributed under the project's MIT License.
