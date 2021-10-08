#include <klib.h>
#include <klib-macros.h>
#include <stdint.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

size_t strlen(const char *s) {
  size_t i = 0;
  while (s[i] != '\0')
    ++i;
  return i;
}

char *strcpy(char *dst, const char *src) {
  return memcpy (dst, src, strlen (src) + 1);
}

char *strncpy(char *dst, const char *src, size_t n) {
  return memcpy(dst, src, n);
}

char *strcat(char *dst, const char *src) {
  strcpy(dst + strlen(dst), src);
  return dst;
}

int strcmp(const char *s1, const char *s2) {
    for(;*s1==*s2;++s1,++s2)
        if(*s1=='\0')
            return(0);
    return((*(unsigned char*)s1 - *(unsigned char*)s2));
}

int strncmp(const char *s1, const char *s2, size_t n) {
  return memcmp(s1,s2,n);
}

void *memset(void *s, int c, size_t n) {
  const unsigned char ch = c;
  char *target = (char *)s;
  while (n--) {
    *(target++) = ch;
  }
  return s;
}

void *memmove(void *dst, const void *src, size_t n) {
    char *dst_tmp = (char *)dst;
    const char *src_tmp = (char *)src;

    if (src_tmp > dst_tmp || src_tmp + n <= dst_tmp) {
        while (n--) {
            *(dst_tmp++) = *(src_tmp++);
        }
    } else {
        dst_tmp += n - 1;
        src_tmp += n - 1;
        while (n--) {
            *(dst_tmp--) = *(src_tmp--);
        }
    }
    return dst;
}

void *memcpy(void *out, const void *in, size_t n) {
  return memmove(out,in,n);
}

int memcmp(const void *s1, const void *s2, size_t n) {
  const char *src1 = s1;
  const char *src2 = s2;
  if (!n)
    return(0);

  while ( --n && *src1 == *src2 ) {
    ++src1, ++src2;
  }
  
  return((*(unsigned char*)src1 - *(unsigned char*)src2));
}

#endif
