#include <NDL.h>
#include <SDL.h>
#include <assert.h>
#include <stdio.h>

int SDL_OpenAudio(SDL_AudioSpec *desired, SDL_AudioSpec *obtained) {
  printf("SDL_OpenAudio not imp!\n");
  // assert(0);
  return 0;
}

void SDL_CloseAudio() {
  printf("SDL_CloseAudio not imp!\n");
  // assert(0);
}

void SDL_PauseAudio(int pause_on) {
  printf("SDL_PauseAudio not imp!\n");
  // assert(0);
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
