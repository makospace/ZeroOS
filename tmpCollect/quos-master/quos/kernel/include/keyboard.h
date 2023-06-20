#ifndef KEYBOARD_H
#define KEYBOARD_H
	#include "defs.h"

	#define KEY_SCOLL_OPEN	0x10
	#define KEY_NUM_OPEN	0x20
	#define KEY_CAPS_OPEN	0x40
	#define KEY_INSERT_OPEN	0x80

	#define KEY_RSHIFT_OPEN	0x1
	#define KEY_LSHIFT_OPEN	0x2
	#define KEY_CTRL_OPEN	0x4
	#define KEY_ALT_OPEN	0x8

	#define KEY_ESC			01
	#define KEY_ENTER		28
	#define KEY_TAB			15
	#define KEY_HOME		71
	#define KEY_END			79

	BYTE keyBroadState(void);		//获取键盘状态
	BYTE getKeyCode(void);			//获取当前按键的扫描码(非阻塞)
	BYTE getKeyChar(void);			//获取当前按键的ASCII码(非阻塞)
	WORD getKeyCodeChar(void);		//获取当前按键的ASCII码和扫描码(非阻塞)
	BYTE getKeyCharWait(void);		//获取当前按键的扫描码(阻塞)
	BYTE getKeyCodeWait(void);		//获取当前按键的ASCII码(阻塞)
	WORD getKeyCodeCharWait(void);	//获取当前按键的ASCII码和扫描码(阻塞)


#endif //ifdef  KEYBOARD_H