#ifndef ROGOSYNTH_ENVELOPE_H
#define ROGOSYNTH_ENVELOPE_H
#include <algorithm>

class Envelope {
    double mAttack, mDecay, mSustain, mRelease;
    double mStartTime;
    double mReleaseTime;

  public:
    Envelope() : Envelope(0.5,0.5,0.5,0.5) {}
    Envelope(double a, double d, double s, double r)
    {
        attack(a);
        decay(d);
        sustain(s);
        release(r);
        mStartTime = -1.0;
        mReleaseTime = -1.0;
    }
    void attack(double v) { mAttack = std::max(v, 0.0); }
    double attack() { return mAttack; }
    void decay(double v) { mDecay = std::max(v, 0.0); }
    double decay() { return mDecay; }
    void sustain(double v) { mSustain = std::clamp(v, 0.0, 1.0); }
    double sustain() { return mSustain; }
    void release(double v) { mRelease = std::max(v, 0.0); }
    double release() { return mRelease; }
    void noteOn(double time)
    {
        mStartTime = time;
        mReleaseTime = -1.0;
    }
    void noteOff(double time) { mReleaseTime = time; }
    bool active(double time)
    {
        if (mStartTime < 0.0) {
            return false;
        }
        else if (time >= mStartTime) {
            if (mReleaseTime < 0.0) {
                return true; // prior to noteOff
            }
            else if (time - mReleaseTime < mRelease) {
                return true; // during release
            }
            else {
                return false; // after release
            }
        }
        // time prior to startTime?
        return false;
    }
    double amplitude(double time)
    {
        if (mReleaseTime < mStartTime) {
            // Attack, Decay, Sustain
            double curTime = time - mStartTime;
            if (curTime <= mAttack) {
                // Attack
                return curTime / mAttack; // 0 .. 1.0
            }
            curTime -= mAttack;
            if (curTime <= mDecay) {
                // Decay
                return mSustain + (1.0 - mSustain) * (1.0 - curTime / mDecay);
            }
            // Sustain
            return mSustain;
        }
        else {
            double curTime = time - mReleaseTime;
            if (curTime <= mRelease) {
                // Release
                return mSustain * (1.0 - curTime / mDecay);
            }
            // done
            return 0.0;
        }
    }
};
#endif