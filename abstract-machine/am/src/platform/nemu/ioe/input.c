#include <am.h>
#include <bits/stdint-uintn.h>
#include <nemu.h>

#define KEYDOWN_MASK 0x8000

void __am_input_keybrd(AM_INPUT_KEYBRD_T *kbd) {
  uint32_t keycode = inl(KBD_ADDR);
  if (keycode & 0x8000) {
    kbd->keydown = 1;
    kbd->keycode = keycode & (~0x8000);
  } else {
    kbd->keydown = 0;
    kbd->keycode = keycode;//AM_KEY_NONE;
  }
}
