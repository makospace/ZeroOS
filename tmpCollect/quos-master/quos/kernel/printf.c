//文件名：printf.c
//功能：
//	格式化输出函数
// 编译方法：
//		cl /nologo /c /Gs /G3 /w /Gx- printf.obj printf.c
//  如斯
//2007-6-5
#include "include/stdc.h"

#define ZEROPAD	1		/* 是否补0 */
#define SIGN	2		/* unsigned/signed long */
#define PLUS	4		/* 是否显示正好 */
#define SPACE	8		/* 是否空出符号位 */
#define LEFT	16		/* left justified */
#define SPECIAL	32		/* 0x */
#define SMALL	64		/* use 'abcdef' instead of 'ABCDEF' */
#define is_digit(c)	((c) >= '0' && (c) <= '9')

static char far * number(char far * str, int num, int base, int size, int precision, int type)
{
	char c,sign,tmp[36];
	const char far *digits="0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
	int i;
//{
//char buf[20];
//ltoa(num, (char far *)buf);puts((char far *)buf);
////putchar(*fmt);
//puts((char far *)"\n");
//getch();
//}
	if (type&SMALL) digits="0123456789abcdefghijklmnopqrstuvwxyz";
	if (type&LEFT) type &= ~ZEROPAD;
	if (base<2 || base>36)
		return 0;

	c = (type & ZEROPAD) ? '0' : ' ' ;

	if (type&SIGN && num<0)
	{
		sign='-';
		num = -num;
	}
	else
	{
		sign=(type&PLUS) ? '+' : ((type&SPACE) ? ' ' : 0);
	}

	if (sign) size--;

	if (type&SPECIAL)
	{
		if (base==16) size -= 2;
		else if (base==8) size--;
	}
	i=0;
	if (num==0)
	{
		tmp[i++]='0';
	}
	else
	{
		int max = 0;
		while (num!=0)
		{
			// 这儿因为我们不能使用divl这个指令，所以在处理无符号数的时候会出错
			int __res;
			if(max++>=36)break;
			__res = ((unsigned long) num) % (unsigned) base;
			num = ((unsigned long) num) / (unsigned) base;

//{
//char buf[20];
//ltoa((unsigned long) num, (char far *)buf);
//puts((char far *)buf);
//getch();
//}
			tmp[i++]=digits[__res];
		}
	}
	if (i>precision) precision=i;
	size -= precision;
//{
//char buf[20];
//itoa(size, (char far *)buf);puts((char far *)buf);
////putchar(*fmt);
//puts((char far *)"\n");
//getch();
//}
	if (!(type&(ZEROPAD+LEFT)))
		while(size-->0)
			*str++ = ' ';
	if (sign)
		*str++ = sign;
	if (type&SPECIAL)
	{
		if (base==8)
		{
			*str++ = '0';
		}
		else if (base==16)
		{
			*str++ = '0';
			*str++ = digits[33];
		}
	}
	if (!(type&LEFT))
		while(size-->0)
			*str++ = c;
	while(i<precision--)
		*str++ = '0';
	while(i-->0)
		*str++ = tmp[i];
	while(size-->0)
		*str++ = ' ';
	return str;
}
static char far * numberl(char far * str, long num, int base, int size, int precision, int type)
{
	char c,sign,tmp[36];
	const char far *digits="0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
	int i;
//{
//char buf[20];
//ltoa(num, (char far *)buf);puts((char far *)buf);
////putchar(*fmt);
//puts((char far *)"\n");
//getch();
//}
	if (type&SMALL) digits="0123456789abcdefghijklmnopqrstuvwxyz";
	if (type&LEFT) type &= ~ZEROPAD;
	if (base<2 || base>36)
		return 0;

	c = (type & ZEROPAD) ? '0' : ' ' ;

	if (type&SIGN && num<0)
	{
		sign='-';
		num = -num;
	}
	else
	{
		sign=(type&PLUS) ? '+' : ((type&SPACE) ? ' ' : 0);
	}

	if (sign) size--;

	if (type&SPECIAL)
	{
		if (base==16) size -= 2;
		else if (base==8) size--;
	}
	i=0;
	if (num==0)
	{
		tmp[i++]='0';
	}
	else
	{
		int max = 0;
		while (num!=0)
		{
			// 这儿因为我们不能使用divl这个指令，所以在处理无符号数的时候会出错
			int __res;
			if(max++>=36)break;
			__res = ((unsigned long) num) % (unsigned) base;
			num = ((unsigned long) num) / (unsigned) base;

//{
//char buf[20];
//ltoa((unsigned long) num, (char far *)buf);
//puts((char far *)buf);
//getch();
//}
			tmp[i++]=digits[__res];
		}
	}
	if (i>precision) precision=i;
	size -= precision;
//{
//char buf[20];
//itoa(size, (char far *)buf);puts((char far *)buf);
////putchar(*fmt);
//puts((char far *)"\n");
//getch();
//}
	if (!(type&(ZEROPAD+LEFT)))
		while(size-->0)
			*str++ = ' ';
	if (sign)
		*str++ = sign;
	if (type&SPECIAL)
	{
		if (base==8)
		{
			*str++ = '0';
		}
		else if (base==16)
		{
			*str++ = '0';
			*str++ = digits[33];
		}
	}
	if (!(type&LEFT))
		while(size-->0)
			*str++ = c;
	while(i<precision--)
		*str++ = '0';
	while(i-->0)
		*str++ = tmp[i];
	while(size-->0)
		*str++ = ' ';
	return str;
}

int vsprintf(char far *buf, const char far *fmt, va_list args)
{
	int len;
	int i;
	char far * str;
	char far *s;
	int far *ip;

	int flags;		/* 用于记录一些标志，比如是否补0等等 */

	int field_width;	/* 输出域的宽度 */
	int precision;		/* 输出数据的精度 */
	int qualifier;		/* 'h', 'l', 或 'L'等扩展标志*/

	for (str=buf ; *fmt ; ++fmt)
	{
		if (*fmt != '%')
		{
			*str++ = *fmt;
			continue;
		}
//{
////char buf[20];
////itoa(size, (char far *)buf);puts((char far *)buf);
//putchar(*fmt);
//puts((char far *)"\n");
//getch();
//}
		/* process flags */
		flags = 0;
		repeat:
			++fmt;		/* this also skips first '%' */
			switch (*fmt)
			{
				case '-': flags |= LEFT; goto repeat;
				case '+': flags |= PLUS; goto repeat;
				case ' ': flags |= SPACE; goto repeat;
				case '#': flags |= SPECIAL; goto repeat;
				case '0': flags |= ZEROPAD; goto repeat;
			}

		/* 处理位宽 */
		field_width = -1;
		if (is_digit(*fmt))
		{
			field_width = atoi(fmt);
			// 我们我们采用的是笨法子。他原本的处理比较巧妙，可惜我们不能用
			// 它原来使用的是指向指针的指针，在我们这儿有问题，因为我们时刻需要长指针
			if(field_width<10)
				fmt++;
			else if(field_width<100)
				fmt+=2;
			else if(field_width<1000)
				fmt+=3;
			else if(field_width<10000)
				fmt+=4;
			else
				fmt+=5;
		}
		else if (*fmt == '*')
		{
			/* it's the next argument */
			field_width = va_arg(args, int);
			if (field_width < 0)
			{
				field_width = -field_width;
				flags |= LEFT;
			}
		}
		/* 处理精度 */
		precision = -1;
		if (*fmt == '.')
		{
			++fmt;	
			if (is_digit(*fmt))
				precision = atoi(fmt);
			else if (*fmt == '*')
			{
				/* it's the next argument */
				precision = va_arg(args, int);
			}
			if (precision < 0)
				precision = 0;
		}

		/* 处理转化限定词 */
		qualifier = -1;
		if (*fmt == 'h' || *fmt == 'l' || *fmt == 'L')
		{
			qualifier = *fmt;
			//field_width = 
			++fmt;
		}

		switch (*fmt) 
		{
		// 这是我们自己的扩展，用来输出二进制数
		case 'b':
			if(qualifier == 'h' || qualifier == 'l' || qualifier == 'L')
			{
				str = numberl(str, va_arg(args, DWORD), 2, field_width, precision, flags);
			}
			else
				str = number(str, va_arg(args, WORD), 2, field_width, precision, flags);
			break;

		case 'c':
			if (!(flags & LEFT))
				while (--field_width > 0)
					*str++ = ' ';
			*str++ = (unsigned char) va_arg(args, int);
			while (--field_width > 0)
				*str++ = ' ';
			break;

		case 's':
			s = va_arg(args, char far *);
 //{
////char buf[20];
////itoa(size, (char far *)buf);
//puts((char far *)s);
////putchar(*fmt);
//puts((char far *)"\n");
//getch(); 
//}
			len = strlen(s);
			if (precision < 0)
				precision = len;
			else if (len > precision)
				len = precision;

			if (!(flags & LEFT))
				while (len < field_width--)
					*str++ = ' ';
			for (i = 0; i < len; ++i)
				*str++ = *s++;
			while (len < field_width--)
				*str++ = ' ';
			break;

		case 'o':
			if(qualifier == 'h' || qualifier == 'l' || qualifier == 'L')
			{
				str = numberl(str, va_arg(args, DWORD), 8, field_width, precision, flags);
			}
			else
				str = number(str, va_arg(args,  WORD), 8, field_width, precision, flags);
			break;

		case 'p':
			if (field_width == -1)
			{
				field_width = 8;
				flags |= ZEROPAD;
			}
			str = numberl(str, va_arg(args, void far *), 16, field_width, precision, flags);
			break;

		case 'x':
			flags |= SMALL;
		case 'X':
			if(qualifier == 'h' || qualifier == 'l' || qualifier == 'L')
			{
				str = numberl(str, va_arg(args, DWORD), 16, field_width, precision, flags);
			}
			else
				str = number(str, va_arg(args,  WORD), 16, field_width, precision, flags);
			break;

		case 'd':
		case 'i':
			flags |= SIGN;
//		case 'u': // 因为我们无法使用divl指令，所以在将负数转化成无符号数的时候会出错，所以只能不提供这个功能了
			if(qualifier == 'h' || qualifier == 'l' || qualifier == 'L')
			{
				str = numberl(str, va_arg(args, DWORD), 10, field_width, precision, flags);
			}
			else
				str = number(str, va_arg(args, WORD), 10, field_width, precision, flags);
			break;

		case 'n':
			ip = va_arg(args, int far *);
			*ip = (str - buf);
			break;

		default:
			if (*fmt != '%')
				*str++ = '%';
			if (*fmt)
				*str++ = *fmt;
			else
				--fmt;
			break;
		}
	}
	*str = '\0';
	return str-buf;
}

int printf(const char far *pStrFormat, ...)
{
	int r = 0;
	static char buf[1024];
	va_list args;

	va_start(args, pStrFormat);
	r = vsprintf(buf, pStrFormat, args);
	va_end(args);
	puts(buf);
	return r;
}