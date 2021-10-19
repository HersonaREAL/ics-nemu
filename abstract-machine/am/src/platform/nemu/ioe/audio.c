#include <am.h>
#include <bits/stdint-uintn.h>
#include <nemu.h>

#define AUDIO_FREQ_ADDR      (AUDIO_ADDR + 0x00)
#define AUDIO_CHANNELS_ADDR  (AUDIO_ADDR + 0x04)
#define AUDIO_SAMPLES_ADDR   (AUDIO_ADDR + 0x08)
#define AUDIO_SBUF_SIZE_ADDR (AUDIO_ADDR + 0x0c)
#define AUDIO_INIT_ADDR      (AUDIO_ADDR + 0x10)
#define AUDIO_COUNT_ADDR     (AUDIO_ADDR + 0x14)
#define AUDIO_SBUF_START_ADDR (AUDIO_ADDR + 0x18)
#define AUDIO_SBUF_END_ADDR (AUDIO_ADDR + 0x1c)

static void audio_write(uint8_t *buf, int len) {
  uint32_t i, used = inl(AUDIO_COUNT_ADDR), bufsz = inl(AUDIO_SBUF_SIZE_ADDR);
  uint32_t end;
  while (used + len > bufsz) {
    used = inl(AUDIO_COUNT_ADDR);
  }

  end = inl(AUDIO_SBUF_END_ADDR);
  for (i = 0; i < len; ++i) {
    outb(AUDIO_SBUF_ADDR + end, buf[i]);
    end = (end + 1)%bufsz;
  }

  outl(AUDIO_COUNT_ADDR, used + len);
  outl(AUDIO_SBUF_END_ADDR,end);
}

void __am_audio_init() {
  outl(AUDIO_SBUF_START_ADDR,0);
  outl(AUDIO_SBUF_END_ADDR,0);
  outl(AUDIO_SBUF_SIZE_ADDR,0x10000);//64KB
  outl(AUDIO_COUNT_ADDR,0);
}

void __am_audio_config(AM_AUDIO_CONFIG_T *cfg) {
  cfg->present = true;
  cfg->bufsize = inl(AUDIO_SBUF_SIZE_ADDR);
}

void __am_audio_ctrl(AM_AUDIO_CTRL_T *ctrl) {
  outl(AUDIO_FREQ_ADDR, ctrl->freq);
  outl(AUDIO_CHANNELS_ADDR, ctrl->channels);
  outl(AUDIO_SAMPLES_ADDR, ctrl->samples);
  outl(AUDIO_INIT_ADDR,1);
}

void __am_audio_status(AM_AUDIO_STATUS_T *stat) {
  stat->count = inl(AUDIO_COUNT_ADDR);
}

void __am_audio_play(AM_AUDIO_PLAY_T *ctl) {
  int len = ctl->buf.end - ctl->buf.start;
  audio_write(ctl->buf.start, len);
}
