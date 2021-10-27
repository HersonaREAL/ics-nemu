#include <NDL.h>
#include <assert.h>
#include <stdio.h>
#include <sdl-timer.h>

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
  printf("SDL_GetTicks not imp!\n");
  assert(0);
  return 0;
}

void SDL_Delay(uint32_t ms) {
  printf("SDL_Delay not imp!\n");
  assert(0);
}
