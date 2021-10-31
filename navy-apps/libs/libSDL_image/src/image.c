#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#define SDL_malloc  malloc
#define SDL_free    free
#define SDL_realloc realloc

#define SDL_STBIMAGE_IMPLEMENTATION
#include "SDL_stbimage.h"

SDL_Surface* IMG_Load_RW(SDL_RWops *src, int freesrc) {
  assert(src->type == RW_TYPE_MEM);
  assert(freesrc == 0);
  assert(0);
  return NULL;
}

SDL_Surface* IMG_Load(const char *filename) {
  FILE *fp = fopen(filename, "r");
  printf("%s\n",filename);
  assert(fp);
  fseek(fp, 0, SEEK_END);
  size_t fsz = ftell(fp);
  printf("fsz: %lu\n",fsz);
  //malloc buf
  uint8_t *buf = (uint8_t *)malloc(sizeof(uint8_t) * fsz);

  //read file to buf
  rewind(fp);
  size_t n = fread(buf,fsz,1,fp);
  assert(n == 1);

  //gen surface
  SDL_Surface *s = STBIMG_LoadFromMemory(buf,fsz);

  free(buf);
  fclose(fp);
  return s;
}

int IMG_isPNG(SDL_RWops *src) {
  assert(0);
  return 0;
}

SDL_Surface* IMG_LoadJPG_RW(SDL_RWops *src) {
  return IMG_Load_RW(src, 0);
}

char *IMG_GetError() {
  return "Navy does not support IMG_GetError()";
}
