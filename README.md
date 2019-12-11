# CS301 Clock

Crappy Clock.



## Design Doc

可用按键 3个
KEY0, KEY1, WK_UP

时钟/闹钟/倒计时设定走串口

按键操作
{主界面} [切换数字/模拟时钟] [设定时钟时间] [关闭闹钟/倒计时提醒]
{时间设定} (按照时\分\秒顺序) -> [当前+1] [当前-1] [下一项设定]

串口通信
set time YYYYMMDDhhmmss
set alarm hhmmss
set countdown hhmmss
list alarm
list countdown
cancel alarm [id]
cancel countdown [id]

功能分割
核心时钟：计时
显示：绘制时钟主界面、闹钟/倒计时 icon？
串口读写：设定时间、闹钟、倒计时
闹钟/倒计时：定时显示？(倒计时本质上来说就是个特殊闹钟)

设计问题
Q: 如何在到达特定时间后显示东西?
Q: 如何保证计时部分本身的独立性？
Q: 是否要用RTOS或者类似框架？还是直接STM裸写？

新建空初始化项目
问题：如何保证计时本身的精确性？