#include <am.h>
#include <nemu.h>
#include <stdint.h>

#define SYNC_ADDR (VGACTL_ADDR + 4)

void __am_gpu_config(AM_GPU_CONFIG_T *cfg) {
  uint32_t vga_wh = inl(VGACTL_ADDR);
  *cfg = (AM_GPU_CONFIG_T) {
    .present = true, .has_accel = false,
    .width = vga_wh >> 16, .height = vga_wh & 0xFFFF,
    .vmemsz = 0
  };
}

void __am_gpu_init() {
  // AM_GPU_CONFIG_T cfg;
  // __am_gpu_config(&cfg);
  // int i;
  // int w = cfg.width;//TODO
  // int h = cfg.height;//TODO
  // uint32_t *fb = (uint32_t *)(uintptr_t)FB_ADDR;
  // for (i = 0; i < w * h ; i++)
  //   fb[i] = i;
  // outl(SYNC_ADDR,1);
}

void __am_gpu_fbdraw(AM_GPU_FBDRAW_T *ctl) {
  AM_GPU_CONFIG_T cfg;
  __am_gpu_config(&cfg);
  int sw = cfg.width;//, sh = cfg.height;
  int x = ctl->x, y = ctl->y, w = ctl->w, h = ctl->h;
  uint32_t *pixels = (uint32_t*)ctl->pixels, *fb = (uint32_t *)(uintptr_t)(FB_ADDR) + (y * sw) + x;
  int i,j;
  int k = 0;

  // sync ?
  if (w ==0 || h == 0) {
    return ctl->sync ? outl(SYNC_ADDR, 1) : NULL;
  }

  for (i = 0;i < h; ++i) {
    for (j = 0; j < w; ++j) {
      fb[i * sw + j] = pixels[k++];
    }
  }

  // sync ?
  if (ctl->sync) {
    outl(SYNC_ADDR, 1);
  }
}

void __am_gpu_status(AM_GPU_STATUS_T *status) {
  status->ready = true;
}
