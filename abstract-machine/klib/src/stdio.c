#include <am.h>
#include <klib.h>
#include <klib-macros.h>
#include <stdarg.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

//==============vsprintf==============
#define is_digit(c) ((c)>='0'&&(c)<='9')

#define ZEROPAD     1   //将输出的前面补上0
#define SIGN        2   // unsigned/signed long
#define PLUS        4   // 显示加号
#define SPACE       8   // 正数前面输出空格，负数输出负号
#define LEFT        16  // 左对齐
#define SPECIAL     32  // '#'标志位
#define SMALL       64  // 16进制使用'abcdef'而不是'ABCDEF'

static int skip_aoti(const char** s)
{
    int i = 0;
    while (is_digit(**s))
        i = i * 10 + *((*s)++) - '0';
    return i;
}

static char* number(char* str, int num, int base, int size, int precision, int type)
{
    char c, sign, tmp[36];
    const char* digits = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";//0-36进制数的字符集
    int i;
    if (type & SMALL)//若规定输出小写字母字符集就是这个
        digits = "0123456789abcdefghijklmnopqrstuvwxyz";
    if (type & LEFT)
        type &= ~ZEROPAD;//即‘0’和‘-’不能同时使用
    if (base < 2 || base>36)
        return 0;
    c = (type & ZEROPAD) ? '0' : ' ';//判断填充满域宽的应该是什么字符
    if ((type & SIGN) && num < 0)//若输出的是有符号数且为负数，则符号设为负号
    {
        sign = '-';
        num = -num;
    }
    else
        sign = (type & PLUS) ? '+' : (type & SPACE) ? ' ' : 0;//决定正数左端的符号
    if (sign)
        size--;//如果有符号
    if (type & SPECIAL) {//type为井号
        if (base == 16)//16进制前面输出0x或0X
            size -= 2;
        else if (base == 8)//8进制前面输出0
            size--;
    }
    i = 0;
    if (num == 0)
        tmp[i++] = '0';
    else
    {
        //进制转换
        while (num != 0) {
            tmp[i++] = digits[num % base];
            num /= base;
        }
    }
    if (i > precision)
        precision = i;
    size -= precision;
    if (!(type & (ZEROPAD + LEFT)))//既不填0也不左对齐，那么填充的是空格
    {
        while (size-- > 0)
            *str++ = ' ';
    }
    if (sign)
        *str++ = sign;//加上符号
    if (type & SPECIAL)
    {
        if (base == 8)
            *str++ = '0';//八进制前面加0
        else if (base == 16)
        {
            //16进制前加0x或0X
            *str++ = '0';
            *str++ = digits[33];
        }
    }
    if (!(type & LEFT))//如果部署左对齐，左端要填上对应的填充符号
    {
        while (size-- > 0)
            *str++ = c;
    }
    while (i < precision--)//精度不够的话，因为是整数所以要在左端加0
        *str++ = '0';
    while (i-- > 0)
    {
        *str++ = tmp[i];
    }
    while (size-- > 0)
        *str++ = ' ';
    return str;
}

int vsprintf(char* buff, const char* format, va_list args)
{
    int* ip;
    char* str = buff;
    int len;
    int flags;//标志位
    char* s;
    int field_width;
    int precision;
    for (; *format; ++format)
    {
        if (*format != '%')
        {
            *str++ = *format;
            continue;
        }
        flags = 0;
    repate:
        format++;
        switch (*format)
        {
        case '-':
            flags |= LEFT;
            goto repate;
        case '+':
            flags |= PLUS;
            goto repate;
        case ' ':
            flags |= SPACE;
            goto repate;
        case '#':
            flags |= SPECIAL;
            goto repate;
        case '0':
            flags |= ZEROPAD;
            goto repate;
        }
        //获取域宽
        field_width = -1;
        if (is_digit(*format))
            field_width = skip_aoti(&format);
        else if (*format == '*')
        {
            field_width = va_arg(args, int);
            if (field_width < 0)
            {
                field_width = -field_width;
                flags |= LEFT;
            }
        }
        //获取精度
        precision = -1;
        if (*format == '.')
        {
            ++format;
            if (is_digit(*format))
                precision = skip_aoti(&format);
            else if (*format == '*')
            {
                precision = va_arg(args, int);
            }
            if (precision < 0)
                precision = 0;
        }
        if (*format == 'h' || *format == 'l' || *format == 'L')
            ++format;

        switch (*format)
        {
            //输出字符
        case 'c':
            if (!(flags & LEFT))//右对齐
            {
                while (--field_width > 0)
                    *str++ = ' ';
            }
            *str++ = (unsigned char)va_arg(args, int);//获取要输出的字符
            while (--field_width > 0)
                *str++ = ' ';
            break;
            //输出字符串
        case 's':
            s = va_arg(args, char*);
            len = strlen(s);
            if (precision < 0)//代表未指定
                precision = len;
            else if (len > precision)
                len = precision;
            if (!(flags & LEFT))
            {
                while (--field_width)
                    *str++ = ' ';
            }
            for (int i = 0; i < len; ++i)
                *str++ = *s++;
            while (len < field_width--)
                *str++ = ' ';
            break;
        case 'o':
            str = number(str, va_arg(args, unsigned long), 8, field_width, precision, flags);
            break;
        case 'p':
            if (field_width == -1)
            {
                field_width = 8;//默认32位
                flags |= ZEROPAD;
            }
            str = number(str, (unsigned long)va_arg(args, void*), 16, field_width, precision, flags);
            break;
        case 'x':
            flags |= SMALL;
        case 'X':
            str = number(str, va_arg(args, unsigned long), 16, field_width, precision, flags);
            break;
        case 'i':
        case 'd':
            flags |= SIGN;
        case 'u':
            str = number(str, va_arg(args, unsigned long), 10, field_width, precision, flags);
            break;
        case 'b':
            str = number(str, va_arg(args, unsigned long), 2, field_width, precision, flags);
            break;
        case 'n':
            ip = va_arg(args, int*);
            *ip = (str - buff);
            break;
        default:
            if (*format != '%')
                *str++ = '%';
            if (*format) {
                *str++ = *format;
            }
            else {
                --format;
            }
            break;
        }
    }
    *str = '\0';
    return (str - buff);
}

int printf(const char *fmt, ...) {
  char buf[4096];
  int ret,i;
  va_list vargs;
  va_start(vargs, fmt);

  ret = vsprintf(buf, fmt, vargs);
  //TODO PUT BUF TO IO
  for(i = 0; i < ret; ++i) {
    putch(buf[i]);
  }

  va_end(vargs);
  
  return ret;
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

int vsnprintf(char *buff, size_t n, const char *format, va_list args) {
    int* ip;
    char* str = buff;
    int len;
    int flags;//标志位
    char* s;
    int field_width;
    int precision;
    for (; *format && str - buff < n - 1; ++format)
    {
        if (*format != '%')
        {
            *str++ = *format;
            continue;
        }
        flags = 0;
    repate:
        format++;
        switch (*format)
        {
        case '-':
            flags |= LEFT;
            goto repate;
        case '+':
            flags |= PLUS;
            goto repate;
        case ' ':
            flags |= SPACE;
            goto repate;
        case '#':
            flags |= SPECIAL;
            goto repate;
        case '0':
            flags |= ZEROPAD;
            goto repate;
        }
        //获取域宽
        field_width = -1;
        if (is_digit(*format))
            field_width = skip_aoti(&format);
        else if (*format == '*')
        {
            field_width = va_arg(args, int);
            if (field_width < 0)
            {
                field_width = -field_width;
                flags |= LEFT;
            }
        }
        //获取精度
        precision = -1;
        if (*format == '.')
        {
            ++format;
            if (is_digit(*format))
                precision = skip_aoti(&format);
            else if (*format == '*')
            {
                precision = va_arg(args, int);
            }
            if (precision < 0)
                precision = 0;
        }
        if (*format == 'h' || *format == 'l' || *format == 'L')
            ++format;

        switch (*format)
        {
            //输出字符
        case 'c':
            if (!(flags & LEFT))//右对齐
            {
                while (--field_width > 0)
                    *str++ = ' ';
            }
            *str++ = (unsigned char)va_arg(args, int);//获取要输出的字符
            while (--field_width > 0)
                *str++ = ' ';
            break;
            //输出字符串
        case 's':
            s = va_arg(args, char*);
            len = strlen(s);
            if (precision < 0)//代表未指定
                precision = len;
            else if (len > precision)
                len = precision;
            if (!(flags & LEFT))
            {
                while (--field_width)
                    *str++ = ' ';
            }
            for (int i = 0; i < len; ++i)
                *str++ = *s++;
            while (len < field_width--)
                *str++ = ' ';
            break;
        case 'o':
            str = number(str, va_arg(args, unsigned long), 8, field_width, precision, flags);
            break;
        case 'p':
            if (field_width == -1)
            {
                field_width = 8;//默认32位
                flags |= ZEROPAD;
            }
            str = number(str, (unsigned long)va_arg(args, void*), 16, field_width, precision, flags);
            break;
        case 'x':
            flags |= SMALL;
        case 'X':
            str = number(str, va_arg(args, unsigned long), 16, field_width, precision, flags);
            break;
        case 'i':
        case 'd':
            flags |= SIGN;
        case 'u':
            str = number(str, va_arg(args, unsigned long), 10, field_width, precision, flags);
            break;
        case 'b':
            str = number(str, va_arg(args, unsigned long), 2, field_width, precision, flags);
            break;
        case 'n':
            ip = va_arg(args, int*);
            *ip = (str - buff);
            break;
        default:
            if (*format != '%')
                *str++ = '%';
            if (*format) {
                *str++ = *format;
            }
            else {
                --format;
            }
            break;
        }
    }
    *str = '\0';
    return (str - buff);
}

#endif
