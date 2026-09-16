#include "TSCProbe.h"
#include "IPlug_include_in_plug_src.h"

#include <algorithm>
#include <cmath>
#include <limits>

#if IPLUG_EDITOR
#include "IControls.h"
#endif

#if IPLUG_EDITOR
namespace
{
class TSC3DSurfaceControl final : public IControl
{
public:
  explicit TSC3DSurfaceControl(const IRECT& bounds)
  : IControl(bounds) {}

  void Draw(IGraphics& g) override
  {
    // Rounded outer enclosure.  Do not paint the entire editor as a square:
    // the transparent corners let the pedal silhouette stay genuinely rounded.
    constexpr float kShellRadius = 30.f;
    const IRECT shadow(mRECT.L + 6.f, mRECT.T + 8.f, mRECT.R - 2.f, mRECT.B - 2.f);
    const IRECT shell(mRECT.L + 3.f, mRECT.T + 3.f, mRECT.R - 4.f, mRECT.B - 5.f);
    g.FillRoundRect(IColor(185, 0, 0, 0), shadow, kShellRadius + 2.f);
    g.FillRoundRect(IColor(255, 25, 74, 39), shell, kShellRadius);

    // Smooth pseudo-PBR green paint.
    // IMPORTANT: do not build the enclosure with horizontal FillRect bands.
    // Even with a mathematically calculated inset those bands rasterize as a
    // staircase at the four corners, which is the square/notched artefact that
    // was still visible in the host.  Every layer that reaches the perimeter is
    // now a rounded rectangle, so the complete silhouette remains one smooth
    // quarter-circle at all four corners.
    const IRECT paintInner(shell.L + 2.f, shell.T + 2.f, shell.R - 2.f, shell.B - 2.f);
    g.FillRoundRect(IColor(255, 49, 145, 69), paintInner, kShellRadius - 2.f);

    // Broad soft lighting layers.  These are intentionally full rounded
    // surfaces rather than clipped rectangular gradients.  Their small alpha
    // values build depth without ever cutting a 90-degree shape into a corner.
    const IRECT upperGlow(shell.L + 5.f, shell.T + 5.f, shell.R - 5.f, shell.B - 16.f);
    g.FillRoundRect(IColor(34, 255, 255, 255), upperGlow, kShellRadius - 5.f);

    const IRECT bodyTone(shell.L + 7.f, shell.T + 11.f, shell.R - 7.f, shell.B - 8.f);
    g.FillRoundRect(IColor(92, 33, 132, 58), bodyTone, kShellRadius - 7.f);

    const IRECT centerPaint(shell.L + 8.f, shell.T + 13.f, shell.R - 8.f, shell.B - 11.f);
    g.FillRoundRect(IColor(255, 55, 158, 76), centerPaint, kShellRadius - 8.f);

    // Heavy beveled perimeter: bright top/left, dark bottom/right.
    // Keep every bevel segment away from the rounded corner arcs and give
    // each segment rounded caps.  The previous top + side rectangles met at
    // a hard 90-degree L shape, which visually squared off all four corners
    // even though the enclosure behind them was rounded.
    const float cornerClear = kShellRadius + 3.f;
    g.FillRoundRect(IColor(42, 255, 255, 255),
                    IRECT(shell.L + cornerClear, shell.T + 7.f,
                          shell.R - cornerClear, shell.T + 17.f), 5.f);
    g.FillRoundRect(IColor(35, 255, 255, 255),
                    IRECT(shell.L + 7.f, shell.T + cornerClear,
                          shell.L + 14.f, shell.B - cornerClear), 3.5f);
    g.FillRoundRect(IColor(65, 0, 0, 0),
                    IRECT(shell.L + cornerClear, shell.B - 20.f,
                          shell.R - cornerClear, shell.B - 9.f), 5.5f);
    g.FillRoundRect(IColor(55, 0, 0, 0),
                    IRECT(shell.R - 14.f, shell.T + cornerClear,
                          shell.R - 7.f, shell.B - cornerClear), 3.5f);

    // Raised upper control deck.
    const IRECT deck(shell.L + 13.f, shell.T + 20.f, shell.R - 13.f, 319.f);
    g.FillRoundRect(IColor(75, 0, 0, 0), IRECT(deck.L + 3.f, deck.T + 5.f, deck.R + 3.f, deck.B + 6.f), 15.f);
    g.FillRoundRect(IColor(35, 255, 255, 255), deck, 15.f);
    g.FillRoundRect(IColor(28, 0, 0, 0), IRECT(deck.L + 3.f, deck.B - 16.f, deck.R - 3.f, deck.B - 4.f), 8.f);

    // Recessed branding panel / seam, like a stamped metal enclosure.
    const IRECT brand(shell.L + 11.f, 322.f, shell.R - 11.f, 424.f);
    g.FillRoundRect(IColor(62, 0, 0, 0), brand, 10.f);
    g.FillRoundRect(IColor(20, 255, 255, 255), IRECT(brand.L + 3.f, brand.T + 3.f, brand.R - 3.f, brand.B - 6.f), 8.f);

    // Oversampling mini-panel: black anodized recess with bright bevel.
    const IRECT osPlate(17.f, 241.f, 131.f, 300.f);
    g.FillRoundRect(IColor(110, 0, 0, 0), IRECT(osPlate.L + 4.f, osPlate.T + 5.f, osPlate.R + 4.f, osPlate.B + 5.f), 9.f);
    g.FillRoundRect(IColor(255, 22, 28, 24), osPlate, 9.f);
    g.FillRoundRect(IColor(35, 255, 255, 255), IRECT(osPlate.L + 3.f, osPlate.T + 3.f, osPlate.R - 3.f, osPlate.T + 10.f), 5.f);

    // Footswitch well and metal mounting plate.
    const IRECT footPlate(106.f, 448.f, 284.f, 590.f);
    g.FillRoundRect(IColor(145, 0, 0, 0), IRECT(footPlate.L + 5.f, footPlate.T + 7.f, footPlate.R + 5.f, footPlate.B + 8.f), 18.f);
    g.FillRoundRect(IColor(255, 51, 58, 53), footPlate, 18.f);
    g.FillRoundRect(IColor(255, 115, 121, 115), IRECT(footPlate.L + 4.f, footPlate.T + 4.f, footPlate.R - 4.f, footPlate.B - 8.f), 14.f);
    g.FillRoundRect(IColor(48, 255, 255, 255), IRECT(footPlate.L + 9.f, footPlate.T + 8.f, footPlate.R - 9.f, footPlate.T + 18.f), 6.f);
    g.FillRoundRect(IColor(70, 0, 0, 0), IRECT(footPlate.L + 8.f, footPlate.B - 20.f, footPlate.R - 8.f, footPlate.B - 8.f), 7.f);

    // Four enclosure screws with individual shadow, metal ring and specular glint.
    const float sx[4] = {20.f, 370.f, 20.f, 370.f};
    const float sy[4] = {20.f, 20.f, 615.f, 615.f};
    for (int i = 0; i < 4; ++i)
    {
      g.FillCircle(IColor(150, 0, 0, 0), sx[i] + 2.f, sy[i] + 3.f, 6.5f);
      g.FillCircle(IColor(255, 72, 76, 72), sx[i], sy[i], 6.f);
      g.FillCircle(IColor(255, 145, 151, 145), sx[i] - 0.7f, sy[i] - 0.7f, 4.5f);
      g.FillCircle(IColor(135, 255, 255, 255), sx[i] - 2.1f, sy[i] - 2.1f, 1.3f);
      g.DrawLine(IColor(180, 42, 44, 42), sx[i] - 2.7f, sy[i], sx[i] + 2.7f, sy[i], nullptr, 1.1f);
    }
  }
};

class TSC3DKnobControl final : public IVKnobControl
{
public:
  TSC3DKnobControl(const IRECT& bounds, int paramIdx, const char* label, const IVStyle& style, bool valueIsEditable)
  : IVKnobControl(bounds, paramIdx, label, style, valueIsEditable) {}

  void DrawHandle(IGraphics& g, const IRECT& bounds) override
  {
    const float cx = bounds.MW();
    const float cy = bounds.MH();
    const float r = std::min(bounds.W(), bounds.H()) * 0.47f;

    // Cast shadow and ambient occlusion.
    g.FillCircle(IColor(120, 0, 0, 0), cx + 4.5f, cy + 5.5f, r + 2.5f);
    g.FillCircle(IColor(80, 0, 0, 0), cx + 2.f, cy + 2.5f, r + 4.5f);

    // Machined outer ring / bevel.
    g.FillCircle(IColor(255, 12, 13, 13), cx, cy, r + 1.5f);
    g.FillCircle(IColor(255, 52, 54, 53), cx - 0.5f, cy - 0.5f, r - 1.f);
    g.FillCircle(IColor(255, 24, 25, 25), cx, cy + 1.f, r - 4.f);

    // Matte black knob face with a subtle domed light.
    g.FillCircle(IColor(255, 31, 32, 32), cx, cy, r - 7.f);
    g.FillCircle(IColor(70, 255, 255, 255), cx - r * 0.28f, cy - r * 0.34f, r * 0.23f);
    g.FillCircle(IColor(36, 255, 255, 255), cx - r * 0.10f, cy - r * 0.18f, r * 0.42f);
    g.FillCircle(IColor(45, 0, 0, 0), cx + r * 0.20f, cy + r * 0.28f, r * 0.50f);

    // Crisp edge gives the control the same 2.5D/PBR feel as the enclosure.
    g.DrawCircle(IColor(210, 5, 6, 6), cx, cy, r - 6.5f, nullptr, 1.5f);
    g.DrawCircle(IColor(80, 255, 255, 255), cx - 0.7f, cy - 0.8f, r - 2.f, nullptr, 1.f);
  }
};
}
#endif

TSCProbe::TSCProbe(const InstanceInfo& info)
: Plugin(info, MakeConfig(kNumParams, kNumPresets))
{
  // New instances open at a practical reference setting: Drive 5, Tone 5,
  // and Level 10.
  GetParam(kParamPedalOn)->InitBool("Pedal On", true, "", 0, "", "Off", "On");
  GetParam(kParamDrive)->InitDouble("Drive", 5.0, 0.0, 10.0, 0.01, "");
  GetParam(kParamTone)->InitDouble("Tone", 5.0, 0.0, 10.0, 0.01, "");
  GetParam(kParamLevel)->InitDouble("Level", 10.0, 0.0, 10.0, 0.01, "");
  GetParam(kParamOversampling)->InitEnum("Oversampling", 3, 4, "", IParam::kFlagsNone, "", "1x", "2x", "4x", "8x");

#if IPLUG_EDITOR
  mMakeGraphicsFunc = [&]() { return MakeGraphics(*this, PLUG_WIDTH, PLUG_HEIGHT, PLUG_FPS); };
  mLayoutFunc = [&](IGraphics* pGraphics) {
    const IRECT bounds = pGraphics->GetBounds();
    pGraphics->LoadFont("Roboto-Regular", ROBOTO_FN);

    // Portrait layout inspired by a classic 808-style stompbox:
    // DRIVE / LEVEL across the top, TONE centred below them,
    // the extra project-only control (Oversampling) in the small-switch area,
    // and the pedal On/Off control at the bottom like a footswitch.
    const IColor darkControl(255, 29, 30, 30);
    pGraphics->AttachPanelBackground(IColor(0, 0, 0, 0));
    pGraphics->AttachControl(new TSC3DSurfaceControl(bounds));

    const IText knobLabelText(14.f);
    const IVStyle knobStyle = DEFAULT_STYLE
      .WithColor(kPR, darkControl)
      .WithColor(kFR, COLOR_WHITE)
      .WithColor(kSH, IColor(170, 0, 0, 0))
      .WithShadowOffset(3.f)
      .WithLabelText(knobLabelText);

    const IVStyle selectorStyle = DEFAULT_STYLE
      .WithColor(kPR, darkControl)
      .WithColor(kFR, COLOR_WHITE)
      // Selected oversampling mode: bright light-green indicator/text.
      // IVRadioButtonControl uses kON for the active radio button and label.
      .WithColor(kON, IColor(255, 130, 255, 155))
      .WithColor(kX1, IColor(255, 145, 150, 145))
      .WithColor(kSH, IColor(170, 0, 0, 0))
      .WithShadowOffset(2.f)
      .WithRoundness(0.75f)
      .WithLabelText(IText(10.f));

    const IVStyle footswitchStyle = DEFAULT_STYLE
      .WithColor(kPR, IColor(255, 120, 120, 120))
      .WithColor(kFR, darkControl)
      .WithColor(kSH, IColor(190, 0, 0, 0))
      .WithShadowOffset(5.f)
      .WithRoundness(0.18f)
      .WithShowLabel(false)
      .WithLabelText(DEFAULT_LABEL_TEXT.WithSize(1.f));

    // Main three knobs follow a familiar three-control stompbox hierarchy.
    const float knobSize = 108.f;
    const IRECT driveBounds(25.f, 45.f, 25.f + knobSize, 45.f + knobSize + 31.f);
    const IRECT levelBounds(bounds.R - 25.f - knobSize, 45.f, bounds.R - 25.f, 45.f + knobSize + 31.f);
    const IRECT toneBounds(bounds.MW() - knobSize * 0.5f, 161.f,
                           bounds.MW() + knobSize * 0.5f, 161.f + knobSize + 31.f);

    // The original 808/MOD mini-switch location is used for our project-only
    // Oversampling selector. No controls that do not exist in TSCProbe are added.
    const IRECT oversamplingBounds(23.f, 251.f, 125.f, 292.f);

    // Pedal branding area, deliberately kept clear of interactive controls.
    const IRECT titleBounds(0.f, 332.f, bounds.R, 388.f);
    const IRECT subtitleBounds(0.f, 381.f, bounds.R, 412.f);

    // Pedal On/Off occupies the footswitch position at the bottom.
    const IRECT switchBounds(bounds.MW() - 71.f, 464.f, bounds.MW() + 71.f, 573.f);
    const IRECT onLabelBounds(bounds.MW() - 109.f, 579.f, bounds.MW() + 109.f, 604.f);

    pGraphics->AttachControl(new TSC3DKnobControl(driveBounds, kParamDrive, "DRIVE", knobStyle, true), kCtrlDrive);
    pGraphics->AttachControl(new TSC3DKnobControl(levelBounds, kParamLevel, "LEVEL", knobStyle, true), kCtrlLevel);
    pGraphics->AttachControl(new TSC3DKnobControl(toneBounds, kParamTone, "TONE", knobStyle, true), kCtrlTone);

    pGraphics->AttachControl(new IVRadioButtonControl(
      oversamplingBounds, kParamOversampling, {"1x", "2x", "4x", "8x"},
      "OS", selectorStyle, EVShape::Ellipse, EDirection::Horizontal, 5.f));

    const IRECT titleShadow(titleBounds.L + 2.f, titleBounds.T + 3.f, titleBounds.R + 2.f, titleBounds.B + 3.f);
    const IRECT subtitleShadow(subtitleBounds.L + 1.5f, subtitleBounds.T + 2.f, subtitleBounds.R + 1.5f, subtitleBounds.B + 2.f);
    pGraphics->AttachControl(new ITextControl(titleShadow, "DER TONDEHR", IText(30.f, IColor(135, 0, 0, 0))));
    pGraphics->AttachControl(new ITextControl(
      titleBounds, "DER TONDEHR", IText(30.f, IColor(255, 245, 248, 244))),
      kCtrlTitle);
    pGraphics->AttachControl(new ITextControl(subtitleShadow, "GSD", IText(18.f, IColor(125, 0, 0, 0))));
    pGraphics->AttachControl(new ITextControl(
      subtitleBounds, "GSD", IText(18.f, IColor(255, 236, 241, 235))),
      kCtrlSubtitle);

    pGraphics->AttachControl(new IVToggleControl(
      switchBounds, kParamPedalOn, " ", footswitchStyle, "OFF", "ON"), kCtrlOn);
    pGraphics->AttachControl(new ITextControl(
      IRECT(onLabelBounds.L + 1.f, onLabelBounds.T + 2.f, onLabelBounds.R + 1.f, onLabelBounds.B + 2.f), "PEDAL ON / OFF",
      IText(12.f, IColor(120, 0, 0, 0))));
    pGraphics->AttachControl(new ITextControl(
      onLabelBounds, "PEDAL ON / OFF",
      IText(12.f, IColor(255, 239, 243, 238))));

  };
#endif
}

#if IPLUG_DSP
void TSCProbe::OnePoleLowPass::SetCutoff(double frequencyHz, double sampleRate)
{
  constexpr double kTwoPi = 6.28318530717958647692;
  const double safeFrequency = std::clamp(frequencyHz, 10.0, sampleRate * 0.45);
  mCoefficient = std::exp(-kTwoPi * safeFrequency / sampleRate);
}

double TSCProbe::OnePoleLowPass::Process(double input)
{
  mState = (1.0 - mCoefficient) * input + mCoefficient * mState;
  return mState;
}

void TSCProbe::ConfigureDSP()
{
  // Cache every sample-rate / oversampling dependent constant once. The old
  // implementation rebuilt several of these values for every audio sample.
  mHostRate = std::max(8000.0, GetSampleRate());
  mOversampledRate = mHostRate * mOversampleFactor;
  mSamplePeriod = 1.0 / mOversampledRate;

  constexpr double kR1 = 4700.0;
  constexpr double kC1 = 47.0e-9;
  constexpr double kC2 = 51.0e-12;
  mRho = mSamplePeriod / (2.0 * kR1 * kC1);
  mDelta = mSamplePeriod / (2.0 * kR1 * kC2);
  mDiodeStep = mSamplePeriod / (2.0 * kC2);
  mToneBilinear = 2.0 * mOversampledRate;
  mSmoothingCoefficient = std::exp(-1.0 / (0.030 * mHostRate));

  // Tone coefficients depend on the oversampled clock, so force one refresh
  // after every mode/sample-rate change. Drive/output caches remain valid.
  mCachedTonePosition = -1.0;

  // The two 1 uF / 10 k coupling stages of the TS808 input and op-amp input.
  // They remove DC and subsonics at 15.9 Hz and 15.6 Hz respectively.
  mDriveState.mInputHighPass[0].SetCutoff(15.9, mHostRate);
  mDriveState.mInputHighPass[1].SetCutoff(15.6, mHostRate);
  // Decimation must not double as a tone control. The previous eight
  // one-pole stages at 20 kHz were about -1.7 dB at 4.5 kHz and -7.7 dB at
  // 10 kHz at 48 kHz/8x. That is exactly the kind of premature upper-harmonic
  // collapse that should come from neither a TS808 nor an anti-alias stage.
  //
  // Use a 12th-order Butterworth low-pass instead. Its passband stays
  // essentially flat through 10–12 kHz, while the steep transition gives
  // useful rejection before the host-rate Nyquist limit. At 48 kHz the
  // cutoff is 16.32 kHz and the response is roughly -41 dB at 24 kHz.
  constexpr std::array<double, 6> kButterworthQ = {
    0.5043144803, 0.5411961001, 0.6302362070,
    0.8213398159, 1.3065629649, 3.8306487878
  };
  constexpr double kTwoPi = 6.28318530717958647692;
  const double antiAliasCutoff = std::min(20000.0, mHostRate * 0.34);
  const double omega = kTwoPi * antiAliasCutoff / mOversampledRate;
  const double cosine = std::cos(omega);
  const double sine = std::sin(omega);

  for (size_t stage = 0; stage < mDriveState.mAntiAlias.size(); ++stage)
  {
    const double alpha = sine / (2.0 * kButterworthQ[stage]);
    const double a0 = 1.0 + alpha;
    const double b0 = ((1.0 - cosine) * 0.5) / a0;
    const double b1 = (1.0 - cosine) / a0;
    const double b2 = b0;
    const double a1 = (-2.0 * cosine) / a0;
    const double a2 = (1.0 - alpha) / a0;
    mDriveState.mAntiAlias[stage].SetCoefficients(b0, b1, b2, a1, a2);
  }
}

void TSCProbe::OnReset()
{
  // This hook can arrive on a host-management thread. The actual reset is
  // performed at the next audio-block boundary, never concurrently with the
  // nonlinear solver.
  mDSPResetRequested.store(true, std::memory_order_release);
}

void TSCProbe::OnActivate(bool active)
{
  // REAPER's FX bypass uses the VST3 activate/deactivate lifecycle, not the
  // pedal's user parameter. Never retain capacitor, diode or filter memory
  // across that boundary: resuming an old feedback solution can otherwise
  // leave the processor silent or resonating until several toggles.
  // setActive is not the audio thread. Defer every mutable DSP operation to
  // ProcessBlock so REAPER cannot resume into a half-reset feedback network.
  (void) active;
  mDSPResetRequested.store(true, std::memory_order_release);
}

double TSCProbe::ProcessOverdrive(double input, double drive, double tone, double level, DriveState& state)
{
  const double safeDrive = std::clamp(drive, 0.0, 10.0) * 0.1;
  const double levelPosition = std::clamp(level, 0.0, 10.0) * 0.1;
  const double tonePosition = std::clamp(tone, 0.0, 10.0) * 0.1;

  // A reset happens while the incoming audio may already be at an arbitrary
  // non-zero point in its waveform. Treating mPreviousInput=0 as a real prior
  // sample creates an artificial full-scale edge on the first sample after
  // reactivation and can kick the nonlinear feedback solver into a bad state.
  if (state.mNeedsPriming)
  {
    state.mPreviousInput = input;
    state.mNeedsPriming = false;
  }

  // TS808 feedback stage, directly discretised from its R1/C1/R2/Drive/C2
  // network. The diodes are part of the op-amp feedback equation; this is
  // the broad, pushed saturation that a post-gain clipper cannot reproduce.
  constexpr double kR2 = 51000.0;
  constexpr double kDrivePot = 500000.0;
  constexpr double kDiodeSaturation1 = 2.10e-9;
  constexpr double kDiodeSaturation2 = 1.30e-9;
  constexpr double kDiodeBetaPositive = 20.6;
  constexpr double kDiodeBetaNegative = 20.2;
  constexpr double kInputVoltsAtFullScale = 8.00;
  constexpr double kDriveTaperExponent = 2.74;

  // pow() is one of the more expensive scalar operations in the hot path.
  // Recalculate the Drive law only while the smoothed control is actually
  // moving; stationary settings reuse the exact previously calculated value.
  if (safeDrive != mCachedDrivePosition)
  {
    mCachedDrivePosition = safeDrive;
    mCachedFeedbackResistance = kR2
      + kDrivePot * std::pow(safeDrive, kDriveTaperExponent);

    const double driveCompensationDb = 0.82 - 0.50 * safeDrive
      - 2.33 * safeDrive * safeDrive;
    mCachedDriveCompensation = std::pow(10.0, driveCompensationDb / 20.0);
  }

  // The Level control is linear. Cache the final broadband scale so it is not
  // rebuilt for every oversampled substep.
  mCachedOutputScale = 1.95 * levelPosition * mCachedDriveCompensation;

  // Exact second-order transfer function of the TS808 tone stage. Coefficients
  // are expensive to derive but only need to change when Tone moves (or when
  // ConfigureDSP invalidates the cache after changing oversampling/sample rate).
  if (tonePosition != mCachedTonePosition)
  {
    mCachedTonePosition = tonePosition;
    const double toneT = std::clamp(tonePosition, 1.0e-4, 1.0 - 1.0e-4);
    constexpr double kTonePot = 20000.0;
    constexpr double kToneSeriesResistance = 220.0;
    constexpr double kToneCapacitor = 0.22e-6;
    constexpr double kToneInputResistance = 10000.0;
    constexpr double kToneSourceResistance = 1000.0;
    constexpr double kToneFeedbackResistance = 1000.0;
    const double leftResistance = toneT * kTonePot;
    const double rightResistance = (1.0 - toneT) * kTonePot;
    const double parallelResistance = (leftResistance * rightResistance)
      / (leftResistance + rightResistance);
    const double y = (leftResistance + rightResistance)
      * (kToneSeriesResistance + parallelResistance);
    const double omegaZero = 1.0 / (kToneCapacitor * (kToneSeriesResistance + parallelResistance));
    const double omegaPole = 1.0 / (kToneCapacitor
      * ((kToneSourceResistance * kToneInputResistance)
        / (kToneSourceResistance + kToneInputResistance)));
    const double x = (rightResistance / (leftResistance + rightResistance)) * omegaZero;
    const double w = y / (leftResistance * kToneFeedbackResistance + y);
    const double gain = (leftResistance * kToneFeedbackResistance + y)
      / (y * kToneSourceResistance * kToneCapacitor);
    const double denominatorA = omegaPole + omegaZero + x;
    const double denominatorB = omegaPole * omegaZero;
    const double d0 = mToneBilinear * mToneBilinear
      + denominatorA * mToneBilinear + denominatorB;
    const double d1 = 2.0 * (denominatorB - mToneBilinear * mToneBilinear);
    const double d2 = mToneBilinear * mToneBilinear
      - denominatorA * mToneBilinear + denominatorB;
    const double numeratorCorner = w * omegaZero;
    state.mToneStack.SetCoefficients(
      gain * (mToneBilinear + numeratorCorner) / d0,
      gain * (2.0 * numeratorCorner) / d0,
      gain * (numeratorCorner - mToneBilinear) / d0,
      d1 / d0, d2 / d0);
  }

  double output = 0.0;
  const int oversampleFactor = mOversampleFactor;
  const double inputDelta = input - state.mPreviousInput;
  const double phi = mDiodeStep / mCachedFeedbackResistance;
  const double substepInputDelta = inputDelta / static_cast<double>(oversampleFactor);
  double previousInterpolatedInput = state.mPreviousInput;

  for (int step = 0; step < oversampleFactor; ++step)
  {
    const double interpolatedInput = previousInterpolatedInput + substepInputDelta;
    const double inputVoltage = interpolatedInput * kInputVoltsAtFullScale;
    // C1 is solved at the oversampled clock.  Using the host-rate previous
    // input here for every substep multiplied its derivative by eight and
    // latched the diode node at DC instead of letting it follow the signal.
    const double previousInputVoltage = previousInterpolatedInput * kInputVoltsAtFullScale;
    // Preserve the previous high-pass output before advancing C1. The diode
    // feedback ODE is integrated with the trapezoidal rule, so its forcing
    // current must be averaged over both ends of the sub-sample interval.
    // Using only the new current (as the previous revision did) is a
    // half-trapezoid: it changes the dynamic clipping law and can produce an
    // artificial bend/redistribution in the upper-harmonic envelope.
    const double previousFeedbackHighPass = std::isfinite(state.mFeedbackHighPassVoltage)
      ? state.mFeedbackHighPassVoltage : 0.0;
    const double feedbackHighPass = (inputVoltage - previousInputVoltage
      + (1.0 - mRho) * previousFeedbackHighPass) / (1.0 + mRho);
    state.mFeedbackHighPassVoltage = std::isfinite(feedbackHighPass) ? feedbackHighPass : 0.0;

    const double previousDiodeVoltage = std::isfinite(state.mDiodeFeedbackVoltage)
      ? state.mDiodeFeedbackVoltage : 0.0;
    // Shockley's law includes the -1 term.  It is essential with toleranced
    // diodes: without it, unequal saturation currents create a false DC
    // source at zero volts and bias the feedback loop instead of modelling
    // the intended positive/negative clipping asymmetry.
    const double previousPositiveArgument = kDiodeBetaPositive * previousDiodeVoltage;
    const double previousNegativeArgument = -kDiodeBetaNegative * previousDiodeVoltage;
    const double previousPositiveExp = std::exp(std::clamp(previousPositiveArgument, -40.0, 40.0));
    const double previousNegativeExp = std::exp(std::clamp(previousNegativeArgument, -40.0, 40.0));
    const double previousPositive = kDiodeSaturation1 * (previousPositiveExp - 1.0);
    const double previousNegative = kDiodeSaturation2 * (previousNegativeExp - 1.0);
    // Trapezoidal discretisation of
    //   dV/dt = In/C2 - V/(Rfb*C2) - Id(V)/C2
    // requires T/(2*C2) * (In[n] + In[n-1]). Since In = Vhp/R1,
    // delta already equals T/(2*R1*C2), hence both Vhp samples belong here.
    const double linearTerm = (1.0 - phi) * previousDiodeVoltage
      + mDelta * (feedbackHighPass + previousFeedbackHighPass);

    // The scalar diode equation is strictly monotonic for the real feedback
    // network, so there is one unique solution for every finite input sample.
    // Do not impose an arbitrary amplitude boundary on that solution: a host
    // sample at 0 dBFS (and even legal floating-point samples above 0 dBFS)
    // must remain an effected sample, never a reason to freeze/reset/bypass the
    // pedal. A warm-start Newton solve handles the normal continuous-audio
    // path; the original dynamic bracket + safeguarded Newton/bisection solver
    // remains below as a stability fallback.
    auto evaluateDiodeEquation = [&](double voltage, double* derivative)
    {
      const double positiveArgument = kDiodeBetaPositive * voltage;
      const double negativeArgument = -kDiodeBetaNegative * voltage;
      const double positiveExp = std::exp(std::clamp(positiveArgument, -40.0, 40.0));
      const double negativeExp = std::exp(std::clamp(negativeArgument, -40.0, 40.0));
      const double positive = kDiodeSaturation1 * (positiveExp - 1.0);
      const double negative = kDiodeSaturation2 * (negativeExp - 1.0);
      const double function = (1.0 + phi) * voltage - linearTerm
        + mDiodeStep * (positive + previousPositive - negative - previousNegative);

      if (derivative)
      {
        // Match the derivative to the clamped exponential used above.  Outside
        // the safe exponential interval the current term is constant, so its
        // derivative is zero rather than an enormous fictitious Newton slope.
        const double positiveSlope = (positiveArgument > -40.0 && positiveArgument < 40.0)
          ? kDiodeBetaPositive * kDiodeSaturation1 * positiveExp : 0.0;
        const double negativeSlope = (negativeArgument > -40.0 && negativeArgument < 40.0)
          ? kDiodeBetaNegative * kDiodeSaturation2 * negativeExp : 0.0;
        *derivative = (1.0 + phi) + mDiodeStep * (positiveSlope + negativeSlope);
      }
      return function;
    };

    double diodeVoltage = std::isfinite(previousDiodeVoltage) ? previousDiodeVoltage : 0.0;
    bool solved = false;

    // Fast path: the previous sub-sample voltage is an excellent initial guess
    // for a continuous guitar waveform, especially at 2x/4x/8x. Start Newton
    // there and avoid the old mandatory root-bracketing work. The first
    // function/slope evaluation reuses the diode exponentials already computed
    // above, saving another exp() pair per substep.
    if (std::isfinite(linearTerm))
    {
      const double previousPositiveSlope =
        (previousPositiveArgument > -40.0 && previousPositiveArgument < 40.0)
          ? kDiodeBetaPositive * kDiodeSaturation1 * previousPositiveExp : 0.0;
      const double previousNegativeSlope =
        (previousNegativeArgument > -40.0 && previousNegativeArgument < 40.0)
          ? kDiodeBetaNegative * kDiodeSaturation2 * previousNegativeExp : 0.0;

      double function = (1.0 + phi) * diodeVoltage - linearTerm
        + mDiodeStep * (2.0 * previousPositive - 2.0 * previousNegative);
      double slope = (1.0 + phi)
        + mDiodeStep * (previousPositiveSlope + previousNegativeSlope);

      if (std::isfinite(function) && std::abs(function) < 1.0e-12)
      {
        solved = true;
      }
      else
      {
        // Five warm-start Newton iterations are enough for the normal path.
        // If a pathological discontinuity does not converge, the original
        // safeguarded bracket+bisection solver below remains the exact fallback.
        for (int iteration = 0; iteration < 5 && !solved; ++iteration)
        {
          if (!std::isfinite(function) || !std::isfinite(slope) || slope <= 0.0)
            break;

          const double candidate = diodeVoltage - function / slope;
          if (!std::isfinite(candidate))
            break;

          diodeVoltage = candidate;
          function = evaluateDiodeEquation(diodeVoltage, &slope);
          solved = std::isfinite(function) && std::abs(function) < 1.0e-12;
        }
      }
    }

    if (!solved)
    {
      // Robust fallback for discontinuities, damaged states or extreme host
      // samples. This is the original monotonic root bracket, so optimisation
      // never trades away stability for CPU usage.
      double lowerVoltage = 0.0;
      double upperVoltage = 0.0;
      double lowerFunction = 0.0;
      double upperFunction = 0.0;
      const double zeroFunction = evaluateDiodeEquation(0.0, nullptr);
      bool bracketed = std::isfinite(zeroFunction);
      const bool solvedAtZero = bracketed && std::abs(zeroFunction) < 1.0e-14;

      if (bracketed && !solvedAtZero)
      {
        double span = 0.50;
        if (zeroFunction < 0.0)
        {
          lowerVoltage = 0.0;
          lowerFunction = zeroFunction;
          upperVoltage = span;
          upperFunction = evaluateDiodeEquation(upperVoltage, nullptr);
          for (int expansion = 0;
               expansion < 20 && (!std::isfinite(upperFunction) || upperFunction < 0.0);
               ++expansion)
          {
            span *= 2.0;
            upperVoltage = span;
            upperFunction = evaluateDiodeEquation(upperVoltage, nullptr);
          }
          bracketed = std::isfinite(upperFunction) && upperFunction >= 0.0;
        }
        else
        {
          upperVoltage = 0.0;
          upperFunction = zeroFunction;
          lowerVoltage = -span;
          lowerFunction = evaluateDiodeEquation(lowerVoltage, nullptr);
          for (int expansion = 0;
               expansion < 20 && (!std::isfinite(lowerFunction) || lowerFunction > 0.0);
               ++expansion)
          {
            span *= 2.0;
            lowerVoltage = -span;
            lowerFunction = evaluateDiodeEquation(lowerVoltage, nullptr);
          }
          bracketed = std::isfinite(lowerFunction) && lowerFunction <= 0.0;
        }
      }

      if (solvedAtZero)
      {
        diodeVoltage = 0.0;
      }
      else if (bracketed)
      {
        if (!std::isfinite(diodeVoltage)
            || diodeVoltage <= lowerVoltage || diodeVoltage >= upperVoltage)
          diodeVoltage = 0.5 * (lowerVoltage + upperVoltage);

        for (int iteration = 0; iteration < 14; ++iteration)
        {
          double slope = 0.0;
          const double function = evaluateDiodeEquation(diodeVoltage, &slope);
          if (!std::isfinite(function))
          {
            diodeVoltage = 0.5 * (lowerVoltage + upperVoltage);
            continue;
          }
          if (std::abs(function) < 1.0e-12)
            break;

          if (function > 0.0)
            upperVoltage = diodeVoltage;
          else
            lowerVoltage = diodeVoltage;

          double candidate = std::numeric_limits<double>::quiet_NaN();
          if (std::isfinite(slope) && slope > 0.0)
            candidate = diodeVoltage - function / slope;
          if (!std::isfinite(candidate) || candidate <= lowerVoltage || candidate >= upperVoltage)
            candidate = 0.5 * (lowerVoltage + upperVoltage);

          if (std::abs(candidate - diodeVoltage) < 1.0e-12)
          {
            diodeVoltage = candidate;
            break;
          }
          diodeVoltage = candidate;
        }
      }
      else
      {
        // Keep the wet circuit continuous even if corrupted non-finite state
        // ever prevents a bracket; never substitute the dry input.
        const double denominator = 1.0 + phi;
        diodeVoltage = (std::isfinite(linearTerm) && std::isfinite(denominator) && denominator > 0.0)
          ? linearTerm / denominator : 0.0;
        if (!std::isfinite(diodeVoltage))
          diodeVoltage = 0.0;
      }
    }

    state.mDiodeFeedbackVoltage = diodeVoltage;
    // The output is the physical sum Vout = Vdiode + Vin, not the diode
    // voltage by itself. That direct component is part of the TS808 feel.
    const double driven = (diodeVoltage + inputVoltage) / kInputVoltsAtFullScale;
    const double voiced = state.mToneStack.Process(driven);
    // Broadband Level + Drive compensation is cached outside the oversampled
    // loop. The gain law is unchanged; we simply avoid two pow()/multiply
    // calculations for every substep.
    output = mCachedOutputScale * voiced;
    // Filter only when an actual downsampling step follows. All six sections
    // are used together so the response stays Butterworth; changing the number
    // of sections with the oversampling factor would alter the audible tone.
    if (oversampleFactor > 1)
    {
      for (auto& filter : state.mAntiAlias)
        output = filter.Process(output);
    }

    previousInterpolatedInput = interpolatedInput;
  }

  state.mPreviousInput = input;
  return output;
}

double TSCProbe::RecoverOutputLevel(double signal, bool pedalEnabled, DriveState& state)
{
  if (!pedalEnabled)
  {
    state.mRecoveryEnvelope = 0.0;
    return signal;
  }
  // The earlier envelope recovery was effectively a fast compressor after the
  // pedal. It obscured the diode dynamics and produced artificial peak spikes
  // during development. Output level is now set by the analogue
  // stages above, so this is intentionally transparent.
  return signal;
}

void TSCProbe::ProcessBlock(sample** inputs, sample** outputs, int nFrames)
{
  if (NOutChansConnected() < 1)
    return;

  const int requestedOversampleFactor = 1 << std::clamp(GetParam(kParamOversampling)->Int(), 0, 3);
  // Rely on iPlug2's explicit reset/activate callbacks.  Project sample-position
  // discontinuities are a transport/scheduling detail in hosts such as REAPER,
  // not evidence that the analogue circuit has become invalid.  Resetting on
  // those discontinuities can create apparently random drop-outs at block or
  // loop boundaries.
  if (mDSPResetRequested.exchange(false, std::memory_order_acq_rel)
      || requestedOversampleFactor != mOversampleFactor)
  {
    mOversampleFactor = requestedOversampleFactor;
    ConfigureDSP();
    mDriveState.Reset();
    mSmoothedDrive = GetParam(kParamDrive)->Value();
    mSmoothedTone = GetParam(kParamTone)->Value();
    mSmoothedLevel = GetParam(kParamLevel)->Value();
    mWasPedalEnabled = GetParam(kParamPedalOn)->Bool();
  }

  const bool pedalOn = GetParam(kParamPedalOn)->Bool();
  if (pedalOn != mWasPedalEnabled)
  {
    // A bypass transition must not reintroduce a DC-latched feedback state.
    // Resetting the physical capacitors and diode node makes the next enable
    // deterministic on the first click instead of relying on repeated toggles.
    mDriveState.Reset();
    mSmoothedDrive = GetParam(kParamDrive)->Value();
    mSmoothedTone = GetParam(kParamTone)->Value();
    mSmoothedLevel = GetParam(kParamLevel)->Value();
    mWasPedalEnabled = pedalOn;
  }
  const bool hasInput = NInChansConnected() > 0;

  // True bypass is a genuinely cheap path now. While the pedal is off there is
  // no reason to run the input RC stages, parameter smoothing or any part of
  // the nonlinear oversampled circuit. The state is reset on each bypass
  // transition, so skipping it here cannot leave stale analogue memory behind.
  if (!pedalOn)
  {
    for (int frame = 0; frame < nFrames; ++frame)
    {
      double dryInput = hasInput ? inputs[0][frame] : 0.0;
      if (!std::isfinite(dryInput))
        dryInput = 0.0;
      outputs[0][frame] = dryInput;
    }
    return;
  }

  const double targetDrive = GetParam(kParamDrive)->Value();
  const double targetTone = GetParam(kParamTone)->Value();
  const double targetLevel = GetParam(kParamLevel)->Value();
  // Thirty milliseconds is quick enough to feel continuous while stopping
  // stepped host automation from modulating the non-linear feedback circuit.
  // The coefficient itself is cached in ConfigureDSP.
  constexpr double kControlSnapEpsilon = 1.0e-6;

  for (int frame = 0; frame < nFrames; ++frame)
  {
    mSmoothedDrive = targetDrive + mSmoothingCoefficient * (mSmoothedDrive - targetDrive);
    mSmoothedTone = targetTone + mSmoothingCoefficient * (mSmoothedTone - targetTone);
    mSmoothedLevel = targetLevel + mSmoothingCoefficient * (mSmoothedLevel - targetLevel);

    // Stop asymptotic denormal-like control movement once it is many orders of
    // magnitude below an audible/automatable step. This lets the Drive/Tone
    // coefficient caches become fully stationary after a knob move.
    if (std::abs(mSmoothedDrive - targetDrive) < kControlSnapEpsilon) mSmoothedDrive = targetDrive;
    if (std::abs(mSmoothedTone - targetTone) < kControlSnapEpsilon) mSmoothedTone = targetTone;
    if (std::abs(mSmoothedLevel - targetLevel) < kControlSnapEpsilon) mSmoothedLevel = targetLevel;

    double dryInput = hasInput ? inputs[0][frame] : 0.0;
    // 0 dBFS is a normal finite audio value, not an error condition. Floating-
    // point hosts can legally exceed 0 dBFS internally as well. Only sanitize
    // genuinely non-finite input data; never amplitude-clamp or bypass a peak.
    if (!std::isfinite(dryInput))
      dryInput = 0.0;
    double conditionedInput = dryInput;
    for (auto& filter : mDriveState.mInputHighPass)
      conditionedInput = filter.Process(conditionedInput);
    double output = ProcessOverdrive(conditionedInput, mSmoothedDrive, mSmoothedTone, mSmoothedLevel, mDriveState);
    // Never convert a loud sample into bypass.  With Pedal On, the dry signal
    // is not an emergency substitute.  The nonlinear solve above is designed to
    // remain finite for every finite host sample.  If an unrelated numerical
    // fault still reaches the output, hold the last valid WET sample for this
    // one frame; do not reset capacitor/diode memory and do not pass dry audio.
    if (!std::isfinite(output))
      output = mDriveState.mLastWetOutput;
    else
      mDriveState.mLastWetOutput = output;
    output = RecoverOutputLevel(output, true, mDriveState);

    // No post-pedal clipper: all compression and saturation must originate in
    // the feedback diode stage above, just as it does in an analogue pedal.
    outputs[0][frame] = output;
  }
}
#endif
