#include "debug.h"
#include <SDL2/SDL_audio.h>
#include <common.h>
#include <device/map.h>
#include <SDL2/SDL.h>
#include <stdint.h>

enum {
  reg_freq,
  reg_channels,
  reg_samples,
  reg_sbuf_size,
  reg_init,
  reg_count,
  reg_qs,
  reg_qe,
  nr_reg
};

static uint8_t *sbuf = NULL;
static uint32_t *audio_base = NULL;

static void audio_play(void *userdata, uint8_t *stream, int len) {
  int nread = len;
  if (audio_base[reg_count] < len) nread = audio_base[reg_count];

  int i, start = audio_base[reg_qs];
  for (i = 0; i < nread; ++i) {
    stream[i] = sbuf[start];
    start = (start + 1) % CONFIG_SB_SIZE;
  }
  audio_base[reg_qs] = start;

  audio_base[reg_count] -= nread;
  if (len > nread) {
    memset(stream + nread, 0, len - nread);
  }
}

static void init_AU_SDL() {
  //init
  SDL_AudioSpec s = {};
  s.format = AUDIO_S16SYS;
  s.userdata = NULL;
  s.freq = audio_base[reg_freq];
  s.channels = audio_base[reg_channels];
  s.samples = audio_base[reg_samples];

  s.callback = audio_play;

  int ret = SDL_InitSubSystem(SDL_INIT_AUDIO);
  if (ret == 0) {
    SDL_OpenAudio(&s, NULL);
    SDL_PauseAudio(0);
  } else {
    panic("INIT SDL AUDIO FAIL!");
  }
}

static void audio_io_handler(uint32_t offset, int len, bool is_write) {
  switch (offset) {
    case reg_init << 2 : {
      return init_AU_SDL();
    }
    default:
      return;
  }
}

void init_audio() {
  uint32_t space_size = sizeof(uint32_t) * nr_reg;
  audio_base = (uint32_t *)new_space(space_size);
#ifdef CONFIG_HAS_PORT_IO
  add_pio_map ("audio", CONFIG_AUDIO_CTL_PORT, audio_base, space_size, audio_io_handler);
#else
  add_mmio_map("audio", CONFIG_AUDIO_CTL_MMIO, audio_base, space_size, audio_io_handler);
#endif

  sbuf = (uint8_t *)new_space(CONFIG_SB_SIZE);
  add_mmio_map("audio-sbuf", CONFIG_SB_ADDR, sbuf, CONFIG_SB_SIZE, NULL);
}
