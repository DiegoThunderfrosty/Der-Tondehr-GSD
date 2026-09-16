#pragma once

#include "IPlug_include_in_plug_hdr.h"

#include <array>
#include <atomic>

const int kNumPresets = 1;

enum EParams
{
  kParamPedalOn = 0,
  kParamDrive,
  kParamTone,
  kParamLevel,
  kParamOversampling,
  kNumParams
};

enum ECtrlTags
{
  kCtrlTitle = 0,
  kCtrlSubtitle,
  kCtrlOn,
  kCtrlDrive,
  kCtrlTone,
  kCtrlLevel,
  kCtrlFooter,
  kCtrlVersion
};

using namespace iplug;
using namespace igraphics;

class TSCProbe final : public Plugin
{
public:
  TSCProbe(const InstanceInfo& info);

#if IPLUG_DSP
  void OnReset() override;
  void OnActivate(bool active) override;
  void ProcessBlock(sample** inputs, sample** outputs, int nFrames) override;
#endif

private:
  struct OnePoleLowPass
  {
    void SetCutoff(double frequencyHz, double sampleRate);
    void Reset() { mState = 0.0; }
    double Process(double input);

    double mCoefficient = 0.0;
    double mState = 0.0;
  };

  struct OnePoleHighPass
  {
    void SetCutoff(double frequencyHz, double sampleRate) { mLowPass.SetCutoff(frequencyHz, sampleRate); }
    void Reset() { mLowPass.Reset(); }
    double Process(double input) { return input - mLowPass.Process(input); }

    OnePoleLowPass mLowPass;
  };

  struct Biquad
  {
    void SetCoefficients(double b0, double b1, double b2, double a1, double a2)
    {
      mB0 = b0; mB1 = b1; mB2 = b2; mA1 = a1; mA2 = a2;
    }
    void Reset() { mX1 = mX2 = mY1 = mY2 = 0.0; }
    double Process(double input)
    {
      const double output = mB0 * input + mB1 * mX1 + mB2 * mX2
        - mA1 * mY1 - mA2 * mY2;
      mX2 = mX1; mX1 = input; mY2 = mY1; mY1 = output;
      return output;
    }

    double mB0 = 1.0, mB1 = 0.0, mB2 = 0.0, mA1 = 0.0, mA2 = 0.0;
    double mX1 = 0.0, mX2 = 0.0, mY1 = 0.0, mY2 = 0.0;
  };

  struct DriveState
  {
    void Reset()
    {
      for (auto& filter : mInputHighPass)
        filter.Reset();
      mToneStack.Reset();
      for (auto& filter : mAntiAlias)
        filter.Reset();
      mPreviousInput = 0.0;
      mFeedbackHighPassVoltage = 0.0;
      mDiodeFeedbackVoltage = 0.0;
      mRecoveryEnvelope = 0.0;
      mLastWetOutput = 0.0;
      mNeedsPriming = true;
    }

    // Input coupling capacitors and the complete active TS808 tone network.
    std::array<OnePoleHighPass, 2> mInputHighPass;
    Biquad mToneStack;
    // Proper high-order decimation filter for the oversampled nonlinear stage.
    // The former eight cascaded one-pole filters were already ~7.7 dB down at
    // 10 kHz (48 kHz host / 8x), so they were audibly shaping the pedal and
    // producing the premature 4–10 kHz collapse visible in spectrum tests.
    // Six biquads form a 12th-order Butterworth response: essentially flat
    // through the guitar band, then steep enough near host Nyquist to prevent
    // ultrasonic diode products from folding back during decimation.
    std::array<Biquad, 6> mAntiAlias;
    double mPreviousInput = 0.0;
    double mFeedbackHighPassVoltage = 0.0;
    double mDiodeFeedbackVoltage = 0.0;
    double mRecoveryEnvelope = 0.0;
    // Emergency continuity value only. Normal finite samples never use it.
    double mLastWetOutput = 0.0;
    // After a host/pedal reset, do not interpret the first non-zero sample as
    // a physical step from 0 V. Prime the oversampled differentiator first.
    bool mNeedsPriming = true;
  };

  void ConfigureDSP();
  double ProcessOverdrive(double input, double drive, double tone, double level, DriveState& state);
  double RecoverOutputLevel(double signal, bool pedalEnabled, DriveState& state);

  DriveState mDriveState;
  // Audio-rate slews prevent control zipper noise without changing the
  // circuit's steady-state response.
  double mSmoothedDrive = 5.0;
  double mSmoothedTone = 5.0;
  double mSmoothedLevel = 10.0;

  // Cached coefficients keep the expensive analogue-control maths out of the
  // hot sample loop whenever a knob is stationary. They are invalidated when
  // the sample rate / oversampling mode changes, so every mode keeps the same
  // circuit response while using much less CPU at steady settings.
  double mCachedDrivePosition = -1.0;
  double mCachedTonePosition = -1.0;
  double mCachedFeedbackResistance = 51000.0;
  double mCachedDriveCompensation = 1.0;
  double mCachedOutputScale = 1.0;

  double mHostRate = 48000.0;
  double mOversampledRate = 384000.0;
  double mSamplePeriod = 1.0 / 384000.0;
  double mRho = 0.0;
  double mDelta = 0.0;
  double mDiodeStep = 0.0;
  double mToneBilinear = 768000.0;
  double mSmoothingCoefficient = 0.0;

  int mOversampleFactor = 8;
  std::atomic<bool> mDSPResetRequested { true };
  bool mWasPedalEnabled = true;
};
