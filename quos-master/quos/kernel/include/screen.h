#ifndef SCREEN_H
#define SCREEN_H
	#define MAX_COL 79
	#define MAX_ROW 24
	void screenInit(BYTE byModeNO);//文本模式初始化
	void screenShowChar(char ch);
	void update_cursor();
	void screenUp(BYTE byCount);//上卷屏
	void screenClear(void);//文本模式清屏
	void setScreenColor(BYTE byColor);
	BYTE getScreenColor();
	void getComebackScreenColor();
#endif //ifdef  SCREEN_H