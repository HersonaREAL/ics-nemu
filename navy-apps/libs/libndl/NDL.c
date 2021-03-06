#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/time.h>

static int evtdev = -1;
static int fbdev = -1;
static int sbdev = -1;
static int sbctldev = -1;
static int screen_w = 0, screen_h = 0, max_w = 0, max_h = 0;
static int x_cor_val = 0, y_cor_val = 0;
static uint64_t tick_start;

static char *parseWH(char *buf, int *res) {
  *res = 0;
  while(*(buf++) != ':');
  while (*buf != '\n') {
    *res = *res * 10 + (*buf - '0');
    ++buf;
  }
  return buf;
}

uint32_t NDL_GetTicks() {
  struct timeval tv;
  gettimeofday(&tv, NULL);
  return ((uint64_t)tv.tv_sec * 1000 + (uint64_t)tv.tv_usec / 1000) - tick_start;
}

int NDL_PollEvent(char *buf, int len) {
  return read(evtdev,buf,len);
}

void NDL_OpenCanvas(int *w, int *h) {
  if (getenv("NWM_APP")) {
    int fbctl = 4;
    fbdev = 5;
    screen_w = *w; screen_h = *h;
    char buf[64];
    int len = sprintf(buf, "%d %d", screen_w, screen_h);
    // let NWM resize the window and create the frame buffer
    write(fbctl, buf, len);
    while (1) {
      // 3 = evtdev
      int nread = read(3, buf, sizeof(buf) - 1);
      if (nread <= 0) continue;
      buf[nread] = '\0';
      if (strcmp(buf, "mmap ok") == 0) break;
    }
    close(fbctl);
  } else {
    //get screen w h
    char disinfo[4096];
    int dpfd = open("/proc/dispinfo",0,0);
    read(dpfd, disinfo, sizeof(disinfo));
    char *p = disinfo;
    p = parseWH(p,&max_w);
    p = parseWH(p,&max_h);
    if (max_w <= 0) max_w = 400;
    if (max_h <= 0) max_h = 300;

    screen_w = *w > max_w || *w == 0 ? max_w : *w;
    screen_h = *h > max_h || *h == 0 ? max_h : *h;
    *w = screen_w;
    *h = screen_h;
    // printf("*w > max_w = %d\n",*w > max_w);
    // printf("w: %d,h: %d\n",*w,*h);
    // printf("max_w: %d,max_h: %d\n",max_w,max_h);
    // printf("screen_w: %d,screen_h: %d\n",screen_w,screen_h);
    //居中
    x_cor_val = (max_w - screen_w) / 2;
    y_cor_val = (max_h - screen_h) / 2;

    fbdev = open("/dev/fb", 0, 0);    
  }
}

void NDL_DrawRect(uint32_t *pixels, int x, int y, int w, int h) {
  if (x == 0 && y == 0 && w == 0 && h == 0) {
    w = screen_w;
    h = screen_h;
  }
  // printf("NDL_DrawRect x: %d, y: %d, w: %d, h: %d\n",x,y,w,h);
  size_t offset = 0;
  offset = ((y + y_cor_val) * max_w + x + x_cor_val) * 4;
  for (int i = 0; i < h; ++i) {
    lseek(fbdev, offset + i * max_w * 4, SEEK_SET);
    pixels += write(fbdev, pixels, w * 4) / 4;
  }
}

void NDL_OpenAudio(int freq, int channels, int samples) {
  sbdev =  open("/dev/sb", 0, 0);
  sbctldev = open("/dev/sbctl",0,0);
  assert(sbdev >= 0 && sbctldev >= 0);
  // printf("open audio %d, %d\n",sbdev,sbctldev);
  int data[3] = {freq,channels,samples};
  write(sbctldev, data, sizeof(data));
}

void NDL_CloseAudio() {
  close(sbdev);
  close(sbctldev);
  sbdev = -1;
  sbctldev = -1;
}

int NDL_PlayAudio(void *buf, int len) {
  if (sbdev < 0) return -1;
  // printf("play! len: %d\n",len);
  return write(sbdev, buf, len);
}

int NDL_QueryAudio() {
  static char buf[64];
  if (sbctldev < 0) return -1;
  read(sbctldev,buf,sizeof(buf));
  // printf("left: %s\n",buf);
  return atoi(buf);
}

int NDL_Init(uint32_t flags) {
  if (getenv("NWM_APP")) {
    evtdev = 3;
  } else {
    evtdev = open("/dev/events",0,0);
  }

  // get tick start
  struct timeval tv;
  gettimeofday(&tv, NULL);
  tick_start = ((uint64_t)tv.tv_sec * 1000 + (uint64_t)tv.tv_usec / 1000);
  return 0;
}

void NDL_Quit() {
}
