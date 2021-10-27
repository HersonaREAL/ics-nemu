#include <NDL.h>
#include <SDL.h>
#include <assert.h>
#include <stdio.h>

#define keyname(k) #k,

static const char *keyname[] = {
  "NONE",
  _KEYS(keyname)
};

int SDL_PushEvent(SDL_Event *ev) {
  printf("SDL_PushEvent not imp!\n");
  assert(0);
  return 0;
}

int SDL_PollEvent(SDL_Event *ev) {
  printf("SDL_PollEvent not imp!\n");
  assert(0);
  return 0;
}

int SDL_WaitEvent(SDL_Event *event) {
  printf("SDL_WaitEvent not imp!\n");
  assert(0);
  return 1;
}

int SDL_PeepEvents(SDL_Event *ev, int numevents, int action, uint32_t mask) {
  printf("SDL_PeepEvents not imp!\n");
  assert(0);
  return 0;
}

uint8_t* SDL_GetKeyState(int *numkeys) {
  printf("SDL_GetKeyState not imp!\n");
  assert(0);
  return NULL;
}
