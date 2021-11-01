#include <NDL.h>
#include <SDL.h>
#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

static uint8_t *stream = NULL;
static size_t stream_len = 0;
static void (*cb)(void *userdata, uint8_t *stream, int len) = NULL;
static uint64_t cb_interval = 0;
static int pause_play = 1;

void SDL_CallbackHelper() {
  static uint64_t last = 0, reen = 1;
  if (!reen) return;

  if (!stream || !cb) return;
  // return;
  if (pause_play == 1) {
    // printf("pause assert!\n");
    return;
  }
  if (NDL_QueryAudio() < stream_len) {
    // printf("no space to use! free space: %d, need: %lu\n",NDL_QueryAudio(),stream_len);
    return;
  }
  reen = 0;
  uint64_t now = SDL_GetTicks();
  // printf("call back! now: %lu, last: %lu, cb_interval: %lu\n",now,last,cb_interval);
  if (now - last >= cb_interval) {
    cb(NULL, stream, stream_len);
    NDL_PlayAudio(stream,stream_len);
    last = now;
  }
  reen = 1;
}

int SDL_OpenAudio(SDL_AudioSpec *desired, SDL_AudioSpec *obtained) {
  assert(desired);
  NDL_OpenAudio(desired->freq,desired->channels,desired->samples);
  pause_play = 1;

  stream = (uint8_t *)malloc(sizeof(uint8_t) * desired->samples * 2);
  stream_len = desired->samples * 2;

  cb = desired->callback;
  assert(cb);
  cb_interval = stream_len / ((desired->freq  / 1000) * desired->channels * 2);

  printf("freq: %d, channels: %d, samples: %d, cb_interval: %lu ms\n",desired->freq,desired->channels,desired->samples,cb_interval);
  return 0;
}

void SDL_CloseAudio() {
  NDL_CloseAudio();
  free(stream);
  stream = NULL;
  stream_len = 0;
  cb = NULL;
  cb_interval = 0;
  pause_play = 1;
}

void SDL_PauseAudio(int pause_on) {
  //non-zero to pause, 0 to unpause
  pause_play = pause_on;
}

void SDL_MixAudio(uint8_t *dst, uint8_t *src, uint32_t len, int volume) {
  printf("SDL_MixAudio not imp!\n");
  // assert(0);
}

SDL_AudioSpec *SDL_LoadWAV(const char *file, SDL_AudioSpec *spec, uint8_t **audio_buf, uint32_t *audio_len) {
  printf("SDL_LoadWAV not imp!\n");
  // assert(0);
  return NULL;
}

void SDL_FreeWAV(uint8_t *audio_buf) {
  printf("SDL_FreeWAV not imp!\n");
  // assert(0);
}

void SDL_LockAudio() {
  // printf("SDL_LockAudio not imp!\n");
  // assert(0);
}

void SDL_UnlockAudio() {
  // printf("SDL_UnlockAudio not imp!\n");
  // assert(0);
}
