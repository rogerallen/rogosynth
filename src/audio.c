#include <math.h>

// From Supercollider doc: Two channel equal power panner. Pan2 takes the square
// root of the linear scaling factor going from 1 (left or right) to 0.5.sqrt
// (~=0.707) in the center, which is about 3dB reduction.  Avoids problem
// inherent to linear panning is that the perceived volume of the signal drops
// in the middle.
// position ranges from [-1,1]
void pan(float *samples, int num_samples, float position)
{
    for (int i = 0; i < num_samples; i += 2) {
        samples[i] *= sqrtf((1.0f - position) / 2.0f);
        samples[i + 1] *= sqrtf((1.0f + position) / 2.0f);
    }
}
