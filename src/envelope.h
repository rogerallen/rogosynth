#ifndef ROGOSYNTH_ENVELOPE_H
#define ROGOSYNTH_ENVELOPE_H
#include <algorithm>

class Envelope {
    float mAttack, mDecay, mSustain, mRelease;
    float mStartTime;
    float mReleaseTime;
    // need to track amplitude prior to release since release can come at any
    // time, not just when you get to sustain part.
    float mCurAmplitude;
    // snapshot cur amplitude at the start of release
    float mReleaseAmplitude;

  public:
    Envelope() : Envelope(0.5, 0.5, 0.5, 0.5) {}
    Envelope(float a, float d, float s, float r)
    {
        attack(a);
        decay(d);
        sustain(s);
        release(r);
        mCurAmplitude = 0.0f;
        mReleaseAmplitude = 0.0f;
        mStartTime = -1.0f;
        mReleaseTime = -1.0f;
    }
    void attack(float v) { mAttack = std::max(v, 0.0f); }
    float attack() { return mAttack; }
    void decay(float v) { mDecay = std::max(v, 0.0f); }
    float decay() { return mDecay; }
    void sustain(float v) { mSustain = std::clamp(v, 0.0f, 1.0f); }
    float sustain() { return mSustain; }
    void release(float v) { mRelease = std::max(v, 0.0f); }
    float release() { return mRelease; }
    void noteOn(float time)
    {
        mStartTime = time;
        mReleaseTime = -1.0;
        mCurAmplitude = 0.0;
        mReleaseAmplitude = 0.0;
    }
    void noteOff(float time)
    {
        mReleaseTime = time;
        // snapshot current amplitude.  No need to track it after this
        mReleaseAmplitude = mCurAmplitude;
    }
    bool active(float time)
    {
        if (mStartTime < 0.0) {
            return false;
        }
        else if (time >= mStartTime) {
            if (mReleaseTime < 0.0) {
                return true; // prior to noteOff
            }
            else if (time - mReleaseTime <= mRelease) {
                return true; // during release
            }
            else {
                return false; // after release
            }
        }
        // time prior to startTime?
        return false;
    }
    bool releasing(float time) { return mReleaseTime > 0.0; }
    float amplitude(float time)
    {
        if (mReleaseTime < mStartTime) {
            // Attack, Decay, Sustain
            float curTime = time - mStartTime;
            if (curTime <= mAttack) {
                // Attack
                mCurAmplitude = curTime / mAttack;
                return mCurAmplitude;
            }
            curTime -= mAttack;
            if (curTime <= mDecay) {
                // Decay
                mCurAmplitude =
                    mSustain + (1.0f - mSustain) * (1.0f - curTime / mDecay);
                return mCurAmplitude;
            }
            // Sustain
            mCurAmplitude = mSustain;
            return mCurAmplitude;
        }
        else {
            float curTime = time - mReleaseTime;
            if (curTime <= mRelease) {
                // Release
                return mReleaseAmplitude * (1.0f - curTime / mDecay);
            }
            // done
            return 0.0f;
        }
    }
};
#endif