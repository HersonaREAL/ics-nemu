#include <NDL.h>
#include <SDL.h>
#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#define keyname(k) #k,

static const char *keyname[] = {
  "NONE",
  _KEYS(keyname)
};

static uint8_t keyState[sizeof(keyname)/sizeof(char *)] = {0};
void SDL_CallbackHelper();

int SDL_PushEvent(SDL_Event *ev) {
  printf("SDL_PushEvent not imp!\n");
  assert(0);
  return 0;
}

int SDL_PollEvent(SDL_Event *event) {
  char buf[64];
  SDL_CallbackHelper();
  if (!NDL_PollEvent(buf, sizeof(buf))) return 0;

  assert(buf[0] == 'k');
  if (buf[1] == 'u')  event->type = SDL_KEYUP;
  else if(buf[1] == 'd') event->type = SDL_KEYDOWN;
  else assert(0);
  
  char *p = buf + 3;
  char *s = p;
  while(*s != '\n') ++s;
  *s = '\0';
  for (uint8_t i = 0;i < sizeof(keyname)/sizeof(char *);++i) {
    if (strcmp(p, keyname[i]) == 0) {
      // printf("key actived! %s\n",keyname[i]);
      event->key.keysym.sym = i;
      keyState[i] = event->type == SDL_KEYUP ? 0 : 1;
      SDL_CallbackHelper();
      return 1;
    }
  }
  assert(0);
  return 1;
}

int SDL_WaitEvent(SDL_Event *event) {
  char buf[64];
  SDL_CallbackHelper();
  while (!NDL_PollEvent(buf, sizeof(buf)));
  assert(buf[0] == 'k');
  if (buf[1] == 'u')  event->type = SDL_KEYUP;
  else if(buf[1] == 'd') event->type = SDL_KEYDOWN;
  else assert(0);
  
  char *p = buf + 3;
  char *s = p;
  while(*s != '\n') ++s;
  *s = '\0';
  for (uint8_t i = 0;i < sizeof(keyname)/sizeof(char *);++i) {
    if (strcmp(p, keyname[i]) == 0) {
      event->key.keysym.sym = i;
      keyState[i] = event->type == SDL_KEYUP ? 0 : 1;
      SDL_CallbackHelper();
      return 1;
    }
  }
  assert(0);
  return 1;
}

int SDL_PeepEvents(SDL_Event *ev, int numevents, int action, uint32_t mask) {
  printf("SDL_PeepEvents not imp!\n");
  assert(0);
  return 0;
}

uint8_t* SDL_GetKeyState(int *numkeys) {
  SDL_CallbackHelper();
  return keyState;
}
