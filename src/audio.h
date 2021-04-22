#ifndef ROGOSYNTH_AUDIO_H
#define ROGOSYNTH_AUDIO_H
#ifdef __cplusplus
extern "C" {
#endif
void pan(float *samples, int num_samples, float position);
#ifdef __cplusplus
}
#endif
#endif