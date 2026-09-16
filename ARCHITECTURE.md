# Architecture

## Project scope

Der Tondehr GSD is a circuit-inspired overdrive, not a netlist simulator and not an exact model of a single physical pedal. Its DSP follows the major functional blocks of a classic feedback-clipping overdrive while introducing extensive calibration and numerical changes.

```text
Mono input
  -> input coupling / subsonic filtering
  -> frequency-dependent feedback high-pass behavior
  -> nonlinear asymmetric diode-feedback solve
  -> active tone network
  -> Drive-dependent compensation and Level stage
  -> anti-alias filtering when oversampling is active
  -> mono output
```

## Circuit-inspired elements

The implementation represents these circuit concepts directly:

- AC-coupled input stages.
- A resistance/capacitance-dependent feedback path.
- Diodes inside the feedback equation rather than a generic waveshaper placed after a linear gain stage.
- A second-order active tone response calculated from the modeled tone-network values.
- Circuit memory stored as capacitor/filter and diode-node state.

## Project-specific modifications

The model intentionally includes substantial departures from a stock pedal:

- Independently calibrated positive and negative diode equations.
- A custom Drive taper and usable gain range.
- Internal full-scale voltage mapping and output compensation.
- Trapezoidal integration of the nonlinear feedback state.
- Warm-start Newton iterations with a safeguarded bracket/bisection fallback.
- State priming and deterministic reset behavior across host activation and bypass changes.
- Parameter smoothing for automation.
- Selectable 1x, 2x, 4x, and 8x nonlinear processing.
- A 12th-order Butterworth decimation response designed to keep the audible guitar/bass band substantially flatter than the project's earlier filter cascade.
- Explicit finite-value guards for hostile or damaged host input.

These choices are part of GSD's identity. A contribution should not replace them with an unrelated generic saturation algorithm while continuing to describe the result as the same circuit model.

## Oversampling

The Oversampling parameter selects a factor of 1, 2, 4, or 8. The nonlinear state is advanced at the selected internal rate. When downsampling is required, the anti-alias sections attenuate ultrasonic products before returning to the host rate.

Oversampling changes CPU cost and alias rejection. It should not be used as an undocumented tone control. Changes to interpolation, nonlinear stepping, or decimation must be compared at every mode and at multiple host sample rates.

## Control behavior

- **Drive** changes the nonlinear feedback resistance using a nonlinear taper. It is not intended to be the final output-volume control.
- **Tone** recalculates the active tone-network coefficients.
- **Level** applies output scaling after the modeled clipping/tone sections.
- **Pedal On / Off** skips the nonlinear path when off and resets circuit memory on transitions for deterministic reactivation.

## Real-time requirements

The audio callback must not allocate memory, lock, access files, perform network activity, or open UI dialogs. Expensive values should remain cached outside the inner sample loop when their controlling parameters have not changed.

All output must remain finite. Stability fallbacks must preserve an effected signal where possible and must never silently substitute the dry input merely because a legal floating-point sample exceeds 0 dBFS.

## Compatibility-sensitive data

The public product name is `Der Tondehr GSD`, while the historical processor class remains `TSCProbe`. Existing parameter order, parameter identifiers, plugin unique ID, and manufacturer ID are compatibility-sensitive. Changing them can break saved sessions or cause hosts to treat the build as a different plugin.
