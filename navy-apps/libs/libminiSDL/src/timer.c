#include <NDL.h>
#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <sdl-timer.h>
#include <stdlib.h>
void SDL_CallbackHelper();
SDL_TimerID SDL_AddTimer(uint32_t interval, SDL_NewTimerCallback callback, void *param) {
  printf("SDL_AddTimer not imp!\n");
  assert(0);
  return NULL;
}

int SDL_RemoveTimer(SDL_TimerID id) {
  printf("SDL_RemoveTimer not imp!\n");
  assert(0);
  return 1;
}

uint32_t SDL_GetTicks() {
  SDL_CallbackHelper();
  return NDL_GetTicks();
}

void SDL_Delay(uint32_t ms) {
  // printf("will spend %u ms\n",ms);
  uint32_t now = SDL_GetTicks();
  while (SDL_GetTicks() - now < ms)
    SDL_CallbackHelper();
  // printf("ectually spend %u ms,%d\n",SDL_GetTicks() - now,rand());
}
