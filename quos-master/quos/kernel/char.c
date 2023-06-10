//文件名：char.c
//功能：
//	字符输入、输出
// 编译方法：
//		cl /nologo /c /Gs /G3 /w /Gx- char.obj char.c
//  如斯
//2007-6-3 
#include "include/stdc.h"
#include "include/keyboard.h"
#include "include/screen.h"
//判断是否是可显示的ASCII码
BOOL isASCII(char inChar)
{
	if((inChar >= 32 && inChar <= 126) || inChar == 9)
		return 1;
	else
		return 0;
}
char getchar()
{
	char temp1 = getch();
	char temp2;
	putchar(temp1);
	temp2 = getch();
	while(temp2 != '\n' && temp2 != '\r')
	{
		temp1 = temp2;
		putchar(temp1);
		temp2 = getch();
	}
	putchar('\n');
	putchar('\r');
	return temp1;
}
char getch()
{
	return (char)getKeyCharWait();//调用键盘驱动
}
void putchar(char c)
{
	screenShowChar(c);// 调用屏幕驱动
}