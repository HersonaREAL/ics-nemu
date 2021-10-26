#include <common.h>

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

  *p = '\0';
  return p - s;
}

size_t dispinfo_read(void *buf, size_t offset, size_t len) {
  return 0;
}

size_t fb_write(const void *buf, size_t offset, size_t len) {
  return 0;
}

void init_device() {
  Log("Initializing devices...");
  ioe_init();
}
