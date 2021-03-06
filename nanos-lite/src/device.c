#include "am.h"
#include <common.h>
#include <stdint.h>
#include <stdio.h>

#if defined(MULTIPROGRAM) && !defined(TIME_SHARING)
# define MULTIPROGRAM_YIELD() yield()
#else
# define MULTIPROGRAM_YIELD()
#endif

#define NAME(key) \
  [AM_KEY_##key] = #key,

static const char *keyname[256] __attribute__((used)) = {
  [AM_KEY_NONE] = "NONE",
  AM_KEYS(NAME)
};

size_t serial_write(const void *buf, size_t offset, size_t len) {
  int i;
  char *p = (char *)buf;
  for (i = 0; i < len; ++i) {
    putch(p[i]);
  }
  return i + 1;
}

size_t events_read(void *buf, size_t offset, size_t len) {
  if (len == 0) return 0;

  AM_INPUT_KEYBRD_T ev = io_read(AM_INPUT_KEYBRD);
  if (ev.keycode == AM_KEY_NONE) return 0;

  const char *status = ev.keydown ? "kd" : "ku", *kn = keyname[ev.keycode];
  char *p = (char *)buf, *s = (char *)buf;

  for (int i = 0; p - s < len - 1 && status[i]; ++i) {
    *(p++) = status[i];
  }

  if (p - s < len - 1) *(p++) = ' ';

  for (int i = 0; p - s < len - 1 && kn[i]; ++i) {
    *(p++) = kn[i];
  }

  if (p - s < len - 1) *(p++) = '\n';

  *p = '\0';
  return p - s;
}

size_t dispinfo_read(void *buf, size_t offset, size_t len) {
  int w = io_read(AM_GPU_CONFIG).width;
  int h = io_read(AM_GPU_CONFIG).height;
  return snprintf(buf, len, "WIDTH:%d\nHEIGHT:%d\n", w, h);
}

size_t fb_write(const void *buf, size_t offset, size_t len) {
  int w = io_read(AM_GPU_CONFIG).width;
  // int h = io_read(AM_GPU_CONFIG).height;
  int pos = offset - offset % 4;
  int x, y, nwrite = 0, need_w = 0;
  uint32_t *color_buf = (uint32_t *)buf;

  //εδΈε
  len = len - len % 4;

  while (nwrite < len) {
    y = (pos/4) / w ;
    x = (pos/4) % w ;
    need_w = (w - x) * 4 > len - nwrite ? len - nwrite : (w - x) * 4;
    io_write(AM_GPU_FBDRAW, x, y, color_buf, need_w / 4, 1, false);
    nwrite += need_w;
    pos += need_w;
  }
  
  io_write(AM_GPU_FBDRAW, 0, 0, NULL, 0, 0, true);
  return pos - offset;
}

size_t sb_write(const void *buf, size_t offset, size_t len) {
  Area sbuf;
  sbuf.start = (void *)buf;
  int nplay = 0;
  while (nplay < len) {
    int nwrite = (len - nplay > 4096 ? 4096 : len - nplay);
    sbuf.end = sbuf.start + nwrite;
    io_write(AM_AUDIO_PLAY, sbuf);
    sbuf.start += nwrite;
    nplay += nwrite;
  }
  
  return len;
}

size_t sbctl_write(const void *buf, size_t offset, size_t len) {
  assert(len == 12);
  int *p = (int *)buf;
  //int freq, int channels, int samples
  io_write(AM_AUDIO_CTRL, p[0], p[1], p[2]);
  return 1;
}

size_t sbctl_read(void *buf, size_t offset, size_t len) {
  size_t bufsz = io_read(AM_AUDIO_CONFIG).bufsize;
  size_t used = io_read(AM_AUDIO_STATUS).count;
  return snprintf((char *)buf, len, "%ld", bufsz - used);
}

void init_device() {
  Log("Initializing devices...");
  ioe_init();
}
