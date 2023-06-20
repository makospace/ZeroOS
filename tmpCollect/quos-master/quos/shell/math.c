//文件名：math.c
//功能：
//	数学函数
// 编译方法：
//		cl /nologo /c /Gs /G3 /w /Gx- math.obj math.c
//  如斯
//2007-6-10
#include "include/stdc.h"
DWORD pow(WORD x, WORD y)
{
	DWORD t = 1;
	if(y<=0&x<=0)return 0;
	if(x==1)return x;
	while(y--)
	{
		t *= x;
	}
	return t;
}