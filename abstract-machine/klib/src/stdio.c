#include <am.h>
#include <klib.h>
#include <klib-macros.h>
#include <stdarg.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

// //==============vsprintf==============
// #define is_digit(c) ((c)>='0'&&(c)<='9')

// #define ZEROPAD     1   //将输出的前面补上0
// #define SIGN        2   // unsigned/signed long
// #define PLUS        4   // 显示加号
// #define SPACE       8   // 正数前面输出空格，负数输出负号
// #define LEFT        16  // 左对齐
// #define SPECIAL     32  // '#'标志位
// #define SMALL       64  // 16进制使用'abcdef'而不是'ABCDEF'

// static int skip_aoti(const char** s)
// {
//     int i = 0;
//     while (is_digit(**s))
//         i = i * 10 + *((*s)++) - '0';
//     return i;
// }

// static char* number(char* str, int num, int base, int size, int precision, int type)
// {
//     char c, sign, tmp[36];
//     const char* digits = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";//0-36进制数的字符集
//     int i;
//     if (type & SMALL)//若规定输出小写字母字符集就是这个
//         digits = "0123456789abcdefghijklmnopqrstuvwxyz";
//     if (type & LEFT)
//         type &= ~ZEROPAD;//即‘0’和‘-’不能同时使用
//     if (base < 2 || base>36)
//         return 0;
//     c = (type & ZEROPAD) ? '0' : ' ';//判断填充满域宽的应该是什么字符
//     if ((type & SIGN) && num < 0)//若输出的是有符号数且为负数，则符号设为负号
//     {
//         sign = '-';
//         num = -num;
//     }
//     else
//         sign = (type & PLUS) ? '+' : (type & SPACE) ? ' ' : 0;//决定正数左端的符号
//     if (sign)
//         size--;//如果有符号
//     if (type & SPECIAL) {//type为井号
//         if (base == 16)//16进制前面输出0x或0X
//             size -= 2;
//         else if (base == 8)//8进制前面输出0
//             size--;
//     }
//     i = 0;
//     if (num == 0)
//         tmp[i++] = '0';
//     else
//     {
//         //进制转换
//         while (num != 0) {
//             tmp[i++] = digits[num % base];
//             num /= base;
//         }
//     }
//     if (i > precision)
//         precision = i;
//     size -= precision;
//     if (!(type & (ZEROPAD + LEFT)))//既不填0也不左对齐，那么填充的是空格
//     {
//         while (size-- > 0)
//             *str++ = ' ';
//     }
//     if (sign)
//         *str++ = sign;//加上符号
//     if (type & SPECIAL)
//     {
//         if (base == 8)
//             *str++ = '0';//八进制前面加0
//         else if (base == 16)
//         {
//             //16进制前加0x或0X
//             *str++ = '0';
//             *str++ = digits[33];
//         }
//     }
//     if (!(type & LEFT))//如果部署左对齐，左端要填上对应的填充符号
//     {
//         while (size-- > 0)
//             *str++ = c;
//     }
//     while (i < precision--)//精度不够的话，因为是整数所以要在左端加0
//         *str++ = '0';
//     while (i-- > 0)
//     {
//         *str++ = tmp[i];
//     }
//     while (size-- > 0)
//         *str++ = ' ';
//     return str;
// }

// int vsprintf(char* buff, const char* format, va_list args)
// {
//     int* ip;
//     char* str = buff;
//     int len;
//     int flags;//标志位
//     char* s;
//     int field_width;
//     int precision;
//     for (; *format; ++format)
//     {
//         if (*format != '%')
//         {
//             *str++ = *format;
//             continue;
//         }
//         flags = 0;
//     repate:
//         format++;
//         switch (*format)
//         {
//         case '-':
//             flags |= LEFT;
//             goto repate;
//         case '+':
//             flags |= PLUS;
//             goto repate;
//         case ' ':
//             flags |= SPACE;
//             goto repate;
//         case '#':
//             flags |= SPECIAL;
//             goto repate;
//         case '0':
//             flags |= ZEROPAD;
//             goto repate;
//         }
//         //获取域宽
//         field_width = -1;
//         if (is_digit(*format))
//             field_width = skip_aoti(&format);
//         else if (*format == '*')
//         {
//             field_width = va_arg(args, int);
//             if (field_width < 0)
//             {
//                 field_width = -field_width;
//                 flags |= LEFT;
//             }
//         }
//         //获取精度
//         precision = -1;
//         if (*format == '.')
//         {
//             ++format;
//             if (is_digit(*format))
//                 precision = skip_aoti(&format);
//             else if (*format == '*')
//             {
//                 precision = va_arg(args, int);
//             }
//             if (precision < 0)
//                 precision = 0;
//         }
//         if (*format == 'h' || *format == 'l' || *format == 'L')
//             ++format;

//         switch (*format)
//         {
//             //输出字符
//         case 'c':
//             if (!(flags & LEFT))//右对齐
//             {
//                 while (--field_width > 0)
//                     *str++ = ' ';
//             }
//             *str++ = (unsigned char)va_arg(args, int);//获取要输出的字符
//             while (--field_width > 0)
//                 *str++ = ' ';
//             break;
//             //输出字符串
//         case 's':
//             s = va_arg(args, char*);
//             len = strlen(s);
//             if (precision < 0)//代表未指定
//                 precision = len;
//             else if (len > precision)
//                 len = precision;
//             if ((flags & LEFT))
//             {
//                 while (--field_width)
//                     *str++ = ' ';
//             }
//             for (int i = 0; i < len; ++i)
//                 *str++ = *s++;
//             while (len < field_width--)
//                 *str++ = ' ';
//             break;
//         case 'o':
//             str = number(str, va_arg(args, unsigned long), 8, field_width, precision, flags);
//             break;
//         case 'p':
//             if (field_width == -1)
//             {
//                 field_width = 8;//默认32位
//                 flags |= ZEROPAD;
//             }
//             str = number(str, (unsigned long)va_arg(args, void*), 16, field_width, precision, flags);
//             break;
//         case 'x':
//             flags |= SMALL;
//         case 'X':
//             str = number(str, va_arg(args, unsigned long), 16, field_width, precision, flags);
//             break;
//         case 'i':
//         case 'd':
//             flags |= SIGN;
//         case 'u':
//             str = number(str, va_arg(args, unsigned long), 10, field_width, precision, flags);
//             break;
//         case 'b':
//             str = number(str, va_arg(args, unsigned long), 2, field_width, precision, flags);
//             break;
//         case 'n':
//             ip = va_arg(args, int*);
//             *ip = (str - buff);
//             break;
//         default:
//             if (*format != '%')
//                 *str++ = '%';
//             if (*format) {
//                 *str++ = *format;
//             }
//             else {
//                 --format;
//             }
//             break;
//         }
//     }
//     *str = '\0';
//     return (str - buff);
// }

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

// int sprintf(char *out, const char *fmt, ...) {
//   int ret;
//   va_list vargs;
//   va_start(vargs, fmt);

//   ret = vsprintf(out, fmt, vargs);

//   va_end(vargs);
  
//   return ret;
// }

// int snprintf(char *out, size_t n, const char *fmt, ...) {
//   int ret;
//   va_list vargs;
//   va_start(vargs, fmt);

//   ret = vsnprintf(out, n,fmt, vargs);

//   va_end(vargs);
  
//   return ret;
// }

// int vsnprintf(char *buff, size_t n, const char *format, va_list args) {
//     int* ip;
//     char* str = buff;
//     int len;
//     int flags;//标志位
//     char* s;
//     int field_width;
//     int precision;
//     for (; *format && str - buff < n - 1; ++format)
//     {
//         if (*format != '%')
//         {
//             *str++ = *format;
//             continue;
//         }
//         flags = 0;
//     repate:
//         format++;
//         switch (*format)
//         {
//         case '-':
//             flags |= LEFT;
//             goto repate;
//         case '+':
//             flags |= PLUS;
//             goto repate;
//         case ' ':
//             flags |= SPACE;
//             goto repate;
//         case '#':
//             flags |= SPECIAL;
//             goto repate;
//         case '0':
//             flags |= ZEROPAD;
//             goto repate;
//         }
//         //获取域宽
//         field_width = -1;
//         if (is_digit(*format))
//             field_width = skip_aoti(&format);
//         else if (*format == '*')
//         {
//             field_width = va_arg(args, int);
//             if (field_width < 0)
//             {
//                 field_width = -field_width;
//                 flags |= LEFT;
//             }
//         }
//         //获取精度
//         precision = -1;
//         if (*format == '.')
//         {
//             ++format;
//             if (is_digit(*format))
//                 precision = skip_aoti(&format);
//             else if (*format == '*')
//             {
//                 precision = va_arg(args, int);
//             }
//             if (precision < 0)
//                 precision = 0;
//         }
//         if (*format == 'h' || *format == 'l' || *format == 'L')
//             ++format;

//         switch (*format)
//         {
//             //输出字符
//         case 'c':
//             if (!(flags & LEFT))//右对齐
//             {
//                 while (--field_width > 0)
//                     *str++ = ' ';
//             }
//             *str++ = (unsigned char)va_arg(args, int);//获取要输出的字符
//             while (--field_width > 0)
//                 *str++ = ' ';
//             break;
//             //输出字符串
//         case 's':
//             s = va_arg(args, char*);
//             len = strlen(s);
//             if (precision < 0)//代表未指定
//                 precision = len;
//             else if (len > precision)
//                 len = precision;
//             if ((flags & LEFT))
//             {
//                 while (--field_width)
//                     *str++ = ' ';
//             }
//             for (int i = 0; i < len; ++i)
//                 *str++ = *s++;
//             while (len < field_width--)
//                 *str++ = ' ';
//             break;
//         case 'o':
//             str = number(str, va_arg(args, unsigned long), 8, field_width, precision, flags);
//             break;
//         case 'p':
//             if (field_width == -1)
//             {
//                 field_width = 8;//默认32位
//                 flags |= ZEROPAD;
//             }
//             str = number(str, (unsigned long)va_arg(args, void*), 16, field_width, precision, flags);
//             break;
//         case 'x':
//             flags |= SMALL;
//         case 'X':
//             str = number(str, va_arg(args, unsigned long), 16, field_width, precision, flags);
//             break;
//         case 'i':
//         case 'd':
//             flags |= SIGN;
//         case 'u':
//             str = number(str, va_arg(args, unsigned long), 10, field_width, precision, flags);
//             break;
//         case 'b':
//             str = number(str, va_arg(args, unsigned long), 2, field_width, precision, flags);
//             break;
//         case 'n':
//             ip = va_arg(args, int*);
//             *ip = (str - buff);
//             break;
//         default:
//             if (*format != '%')
//                 *str++ = '%';
//             if (*format) {
//                 *str++ = *format;
//             }
//             else {
//                 --format;
//             }
//             break;
//         }
//     }
//     *str = '\0';
//     return (str - buff);
// }

static inline bool isdigit(char c) {
    return c >= '0' && c <= '9';
}

size_t strnlen(const char *s, size_t count)
{
    const char *sc;

    for (sc = s; count-- && *sc != '\0'; ++sc)
    ;
    return sc - s;
}

static int skip_atoi(const char **s)
{
	int i=0;

	while (isdigit(**s))
		i = i*10 + *((*s)++) - '0';
	return i;
}

#define ZEROPAD	1		/* pad with zero */
#define SIGN	2		/* unsigned/signed long */
#define PLUS	4		/* show plus */
#define SPACE	8		/* space if plus */
#define LEFT	16		/* left justified */
#define SPECIAL	32		/* 0x */
#define LARGE	64		/* use 'ABCDEF' instead of 'abcdef' */
#define INT_MAX 65000
static char * number(char * buf, char * end, long long num, int base, int size, int precision, int type)
{
	char c,sign,tmp[66];
	const char *digits;
	const char small_digits[] = "0123456789abcdefghijklmnopqrstuvwxyz";
	const char large_digits[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
	int i;

	digits = (type & LARGE) ? large_digits : small_digits;
	if (type & LEFT)
		type &= ~ZEROPAD;
	if (base < 2 || base > 36)
		return 0;
	c = (type & ZEROPAD) ? '0' : ' ';
	sign = 0;
	if (type & SIGN) {
		if (num < 0) {
			sign = '-';
			num = -num;
			size--;
		} else if (type & PLUS) {
			sign = '+';
			size--;
		} else if (type & SPACE) {
			sign = ' ';
			size--;
		}
	}
	if (type & SPECIAL) {
		if (base == 16)
			size -= 2;
		else if (base == 8)
			size--;
	}
	i = 0;
	if (num == 0)
		tmp[i++]='0';
	else while (num != 0) {
		tmp[i++] = digits[num%base];
        num /= base;
    }
	if (i > precision)
		precision = i;
	size -= precision;
	if (!(type&(ZEROPAD+LEFT))) {
		while(size-->0) {
			if (buf <= end)
				*buf = ' ';
			++buf;
		}
	}
	if (sign) {
		if (buf <= end)
			*buf = sign;
		++buf;
	}
	if (type & SPECIAL) {
		if (base==8) {
			if (buf <= end)
				*buf = '0';
			++buf;
		} else if (base==16) {
			if (buf <= end)
				*buf = '0';
			++buf;
			if (buf <= end)
				*buf = digits[33];
			++buf;
		}
	}
	if (!(type & LEFT)) {
		while (size-- > 0) {
			if (buf <= end)
				*buf = c;
			++buf;
		}
	}
	while (i < precision--) {
		if (buf <= end)
			*buf = '0';
		++buf;
	}
	while (i-- > 0) {
		if (buf <= end)
			*buf = tmp[i];
		++buf;
	}
	while (size-- > 0) {
		if (buf <= end)
			*buf = ' ';
		++buf;
	}
	return buf;
}

/**
* vsnprintf - Format a string and place it in a buffer
* @buf: The buffer to place the result into
* @size: The size of the buffer, including the trailing null space
* @fmt: The format string to use
* @args: Arguments for the format string
*
* Call this function if you are already dealing with a va_list.
* You probably want snprintf instead.
 */
int vsnprintf(char *buf, size_t size, const char *fmt, va_list args)
{
	int len;
	unsigned long long num;
	int i, base;
	char *str, *end, c;
	const char *s;

	int flags;		/* flags to number() */

	int field_width;	/* width of output field */
	int precision;		/* min. # of digits for integers; max
				   number of chars for from string */
	int qualifier;		/* 'h', 'l', or 'L' for integer fields */
				/* 'z' support added 23/7/1999 S.H.    */
				/* 'z' changed to 'Z' --davidm 1/25/99 */

	str = buf;
	end = buf + size - 1;

	if (end < buf - 1) {
		end = ((void *) -1);
		size = end - buf + 1;
	}

	for (; *fmt ; ++fmt) {
		if (*fmt != '%') {
			if (str <= end)
				*str = *fmt;
			++str;
			continue;
		}

		/* process flags */
		flags = 0;
		repeat:
			++fmt;		/* this also skips first '%' */
			switch (*fmt) {
				case '-': flags |= LEFT; goto repeat;
				case '+': flags |= PLUS; goto repeat;
				case ' ': flags |= SPACE; goto repeat;
				case '#': flags |= SPECIAL; goto repeat;
				case '0': flags |= ZEROPAD; goto repeat;
			}

		/* get field width */
		field_width = -1;
		if (isdigit(*fmt))
			field_width = skip_atoi(&fmt);
		else if (*fmt == '*') {
			++fmt;
			/* it's the next argument */
			field_width = va_arg(args, int);
			if (field_width < 0) {
				field_width = -field_width;
				flags |= LEFT;
			}
		}

		/* get the precision */
		precision = -1;
		if (*fmt == '.') {
			++fmt;	
			if (isdigit(*fmt))
				precision = skip_atoi(&fmt);
			else if (*fmt == '*') {
				++fmt;
				/* it's the next argument */
				precision = va_arg(args, int);
			}
			if (precision < 0)
				precision = 0;
		}

		/* get the conversion qualifier */
		qualifier = -1;
		if (*fmt == 'h' || *fmt == 'l' || *fmt == 'L' || *fmt =='Z') {
			qualifier = *fmt;
			++fmt;
			if (qualifier == 'l' && *fmt == 'l') {
				qualifier = 'L';
				++fmt;
			}
		}

		/* default base */
		base = 10;

		switch (*fmt) {
			case 'c':
				if (!(flags & LEFT)) {
					while (--field_width > 0) {
						if (str <= end)
							*str = ' ';
						++str;
					}
				}
				c = (unsigned char) va_arg(args, int);
				if (str <= end)
					*str = c;
				++str;
				while (--field_width > 0) {
					if (str <= end)
						*str = ' ';
					++str;
				}
				continue;

			case 's':
				s = va_arg(args, char *);
				if (!s)
					s = "<NULL>";

				len = strnlen(s, precision);

				if (!(flags & LEFT)) {
					while (len < field_width--) {
						if (str <= end)
							*str = ' ';
						++str;
					}
				}
				for (i = 0; i < len; ++i) {
					if (str <= end)
						*str = *s;
					++str; ++s;
				}
				while (len < field_width--) {
					if (str <= end)
						*str = ' ';
					++str;
				}
				continue;

			case 'p':
				if (field_width == -1) {
					field_width = 2*sizeof(void *);
					flags |= ZEROPAD;
				}
				str = number(str, end,
						(unsigned long) va_arg(args, void *),
						16, field_width, precision, flags);
				continue;


			case 'n':
				/* FIXME:
				* What does C99 say about the overflow case here? */
				if (qualifier == 'l') {
					long * ip = va_arg(args, long *);
					*ip = (str - buf);
				} else if (qualifier == 'Z') {
					size_t * ip = va_arg(args, size_t *);
					*ip = (str - buf);
				} else {
					int * ip = va_arg(args, int *);
					*ip = (str - buf);
				}
				continue;

			case '%':
				if (str <= end)
					*str = '%';
				++str;
				continue;

				/* integer number formats - set up the flags and "break" */
			case 'o':
				base = 8;
				break;

			case 'X':
				flags |= LARGE;
			case 'x':
				base = 16;
				break;

			case 'd':
			case 'i':
				flags |= SIGN;
			case 'u':
				break;

			default:
				if (str <= end)
					*str = '%';
				++str;
				if (*fmt) {
					if (str <= end)
						*str = *fmt;
					++str;
				} else {
					--fmt;
				}
				continue;
		}
		if (qualifier == 'L')
			num = va_arg(args, long long);
		else if (qualifier == 'l') {
			num = va_arg(args, unsigned long);
			if (flags & SIGN)
				num = (signed long) num;
		} else if (qualifier == 'Z') {
			num = va_arg(args, size_t);
		} else if (qualifier == 'h') {
			num = (unsigned short) va_arg(args, int);
			if (flags & SIGN)
				num = (signed short) num;
		} else {
			num = va_arg(args, unsigned int);
			if (flags & SIGN)
				num = (signed int) num;
		}
		str = number(str, end, num, base,
				field_width, precision, flags);
	}
	if (str <= end)
		*str = '\0';
	else if (size > 0)
		/* don't write out a null byte if the buf size is zero */
		*end = '\0';
	/* the trailing null byte doesn't count towards the total
	* ++str;
	*/
	return str-buf;
}

/**
 * snprintf - Format a string and place it in a buffer
 * @buf: The buffer to place the result into
 * @size: The size of the buffer, including the trailing null space
 * @fmt: The format string to use
 * @...: Arguments for the format string
 */
int snprintf(char * buf, size_t size, const char *fmt, ...)
{
	va_list args;
	int i;

	va_start(args, fmt);
	i=vsnprintf(buf,size,fmt,args);
	va_end(args);
	return i;
}

/**
 * vsprintf - Format a string and place it in a buffer
 * @buf: The buffer to place the result into
 * @fmt: The format string to use
 * @args: Arguments for the format string
 *
 * Call this function if you are already dealing with a va_list.
 * You probably want sprintf instead.
 */
int vsprintf(char *buf, const char *fmt, va_list args)
{
	return vsnprintf(buf, 0xFFFFFFFFUL, fmt, args);
}


/**
 * sprintf - Format a string and place it in a buffer
 * @buf: The buffer to place the result into
 * @fmt: The format string to use
 * @...: Arguments for the format string
 */
int sprintf(char * buf, const char *fmt, ...)
{
	va_list args;
	int i;

	va_start(args, fmt);
	i=vsprintf(buf,fmt,args);
	va_end(args);
	return i;
}
#endif
