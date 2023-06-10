#ifndef TIME_H
#define TIME_H

	void cmosTime(void);//显示从CMOS中获取的时间信息
	void cmosDate(void);//显示从CMOS中获取的日期信息
	BYTE getSecond(void);//从CMOS中获取当前秒
	BYTE getMinute(void);//从CMOS中获取当前分钟
	BYTE getHour(void);//从CMOS中获取当前小时
	BYTE getWeek(void);//从CMOS中获取当前星期值
	BYTE getDay(void);//从CMOS中获取当前天
	BYTE getMonth(void);//从CMOS中获取当前月份值
	WORD getYear(void);//从CMOS中获取当前年份值
	BYTE getCentury(void);//从CMOS中获取当前世纪值
#endif //ifdef  TIME_H