#include <am.h>
#include <klib.h>
#include <klib-macros.h>
#include <stdarg.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

static char* itoa(int value, char *str, int radix) {
    char reverse[36];   
    char *p = reverse;
    int sign = (value >= 0)?1:0;

    value = (value >= 0)?value:-value;
    *p++ = '\0';
    while (value >= 0){
        *p++ = "0123456789abcdef"[value%radix];
        value /= radix;
        if (value == 0) break;
    }

    if (!sign) {
        *p = '-';
    }
    else {
        p--;
    }

    while (p >= reverse){
        *str++ = *p--;
    }

    return str;
}

int printf(const char *fmt, ...) {
  char buf[4096];
  int ret,i;
  va_list vargs;
  va_start(vargs, fmt);

  ret = vsprintf(buf, fmt, vargs);
  //TODO PUT BUF TO IO
  for(i = 0; i < ret; ++i) {
    
  }

  va_end(vargs);
  
  return ret;
}

int vsprintf(char *out, const char *fmt, va_list ap) {
  char *p;

  for (p = out; *fmt != '\0'; ++fmt) {
    if (*fmt != '%') {
      *(p++) = *fmt;
      continue;
    }

    //skip %
    fmt++;
    switch (*fmt) {
      case 'd': 
        itoa(va_arg(ap,int), p, 10);
        p += strlen(p);
        break;
      case 's':
        strcat(p, va_arg(ap, char*));
        p += strlen(p);
        break;
      default:
        panic("Arg not implemented");
    }
  }
  *p = '\0';
  return p - out;
}

int sprintf(char *out, const char *fmt, ...) {
  int ret;
  va_list vargs;
  va_start(vargs, fmt);

  ret = vsprintf(out, fmt, vargs);

  va_end(vargs);
  
  return ret;
}

int snprintf(char *out, size_t n, const char *fmt, ...) {
  int ret;
  va_list vargs;
  va_start(vargs, fmt);

  ret = vsnprintf(out, n,fmt, vargs);

  va_end(vargs);
  
  return ret;
}

int vsnprintf(char *out, size_t n, const char *fmt, va_list ap) {
  char *p;
  size_t i;

  for (p = out,i = 0; *fmt != '\0' && i < n - 1; ++fmt) {
    if (*fmt != '%') {
      *(p++) = *fmt;
      continue;
    }

    //skip %
    fmt++;
    switch (*fmt) {
      case 'd': 
        itoa(va_arg(ap,int), p, 10);
        p += strlen(p);
        break;
      case 's':
        strcat(p, va_arg(ap, char*));
        p += strlen(p);
        break;
      default:
        panic("Arg not implemented");
    }

    ++i;
  }
  *p = '\0';
  return p - out;
}

#endif
