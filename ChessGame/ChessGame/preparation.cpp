// 游戏前期工作准备

#include<stdio.h>
#include<graphics.h>
#include<string.h>
#include <iostream>
#include <conio.h>
#include "startgame.cpp"

using namespace std;

#define WINDOWS_X 1200
#define WINDOWS_Y 800

// 棋盘与棋子的准备
#define PX 280				//棋盘偏移量X
#define PY 80				//棋盘偏移量Y
#define BBLACK 80			//空格的大小

#define CHESSSIZE 25		//棋子的大小
#define SIZE 8				//棋盘的格数

#define ESC 27				//指定ESC的值
#define ESCEXIT (_kbhit()&&_getch()==ESC)   //_kbhit()检测键盘是否有按键，有返回-1，没有返回0
											//_getch()获取键值已经输入到缓冲区，二者均在<conio.h>中定义

const int black = 1;		//黑棋
const int white = 0;		//白棋

int chessboard[SIZE][SIZE]; //二维数组（矩阵）存储棋盘
const int MOVE[8][2] = { { -1, 0 },{ 1, 0 },{ 0, -1 },{ 0, 1 },{ -1, -1 },{ 1, -1 },{ 1, 1 },{ -1, 1 } }; //八个方位
const int CHESSWEIGHT[8][8] =
{
	{90,-60,10,10,10,10,-60,90},
	{-60,-80,5,5,5,5,-80,-60},
	{10,5,1,1,1,1,5,10},
	{10,5,1,1,1,1,5,10},
	{10,5,1,1,1,1,5,10},
	{10,5,1,1,1,1,5,10},
	{-60,-80,5,5,5,5,-80,-60},
	{90,-60,10,10,10,10,-60,90}
};										//棋盘各位置权值估计，为后续AI准备

IMAGE MAPIMAGE[SIZE][SIZE];             //棋盘画面准备
IMAGE COUNT[2];

int expect[SIZE][SIZE];

int MYCOLOR;							//己方棋子的颜色 黑色1 白色0 其他-1
int NOWCOLOR;							//当前执子颜色
bool TIANYANS;							//判断可落子 天眼

class POINT2
{
public:
	int x, y;
	void INIT(int x, int y)				//初始化
	{
		this->x = x;
		this->y = y;
	}
	void ADD(int x, int y)
	{
		this->x += x;
		this->y += y;
	}
	void WIN2MAP(POINT2& MAP)			//棋盘到矩阵
	{
		MAP.x = (x - PX) / BBLACK;
		MAP.y = (y - PY) / BBLACK;
	}
	int WIN2MAPX()
	{
		return PX + x * BBLACK + 0.5 * BBLACK;
	}
	int WIN2MAPY()
	{
		return PY + y * BBLACK + 0.5 * BBLACK;
	}
	void MAP2WIN(POINT2 &WIN)			//矩阵到棋盘
	{
		WIN.x = PX + x * BBLACK + BBLACK / 2;
		WIN.y = PY + y * BBLACK + BBLACK / 2;
	}
	int MAP2WINX()
	{
		return (x - PX) / BBLACK;
	}
	int MAP2WINY()
	{
		return (y - PY) / BBLACK;
	}

};

POINT2 LATESTCHESS;						//记录最后一个棋子的位置

void chessadd(int x, int y, int color, int MAP[SIZE][SIZE])		//添加棋子到矩阵,其实就是在矩阵绘制特定颜色的点
{
	POINT2 WINDOWS2, MAP2;
	WINDOWS2.INIT(x, y);
	WINDOWS2.WIN2MAP(MAP2);
	MAP[MAP2.x][MAP2.y] = color ? 1 : -1;						//确定插入的颜色
}

void printcircle(int x, int y, int color, int MAP[SIZE][SIZE])	//绘制棋子的实现
{
	chessadd(x, y, color, MAP);

	setfillcolor(color ? BLACK : WHITE);
	for (int i = 0; i <= CHESSSIZE; ++i)
	{
		solidcircle(x, y, i);						//实现棋子绘制过程中从中心逐渐放大，最大半径为25个像素点
		Sleep(1);									//延时
	}
}

void boardinit()									//棋盘的绘制
{
	memset(chessboard, 0, sizeof(chessboard));		//初始化 memset是计算机中C/C++语言初始化函数。作用是将某一块内存中的内容全部设置为指定的值
	memset(expect, 0, sizeof(expect));

	TIANYANS = false;								//关闭天眼模式
	MYCOLOR = -1;									//设置己方棋子黑色
	LATESTCHESS.INIT(0, 0);							//初始化最开始的棋子

	settextstyle(15, 0, "楷体");						//高15，间距0
	setbkmode(TRANSPARENT);
	loadimage(NULL, "chessboard1.png");

	for (int x = PX; x < PX + BBLACK * (SIZE + 1); x += BBLACK) //绘制棋盘的横线
	{
		if ((x / BBLACK % 2) == 0)
			setlinecolor(BLACK);
		else setlinecolor(RED);
		line(x, PY, x, PY + BBLACK * SIZE);
	}
	for (int y = PY; y < PY + BBLACK * (SIZE + 1); y += BBLACK)		//绘制棋盘的竖线
	{
		if (y / BBLACK % 2 == 0)setlinecolor(BLACK);
		else setlinecolor(RED);
		line(PX, y, PX + BBLACK * SIZE, y);
	}
	for (int i = PX; i <= PX + BBLACK * SIZE; i += BBLACK)			//获取矩阵格子位置
	{
		for (int j = PY; j <= PY + BBLACK * SIZE; j += BBLACK)
		{
			POINT2 DATA;
			DATA.INIT(i, j);
			DATA.WIN2MAP(DATA);
			getimage(&MAPIMAGE[DATA.x][DATA.y], i, j, BBLACK, BBLACK); //getimage(保存图像的指针，左上角x坐标，y坐标，宽度，高度)，作用是从屏幕对象中获取图像

		}
	}
	printcircle(PX + (SIZE / 2 - 1) * BBLACK + BBLACK / 2, PY + (SIZE / 2 - 1)* BBLACK + BBLACK / 2, white, chessboard);
	printcircle(PX + (SIZE / 2 - 1) * BBLACK + BBLACK / 2, PY + (SIZE / 2) * BBLACK + BBLACK / 2, black, chessboard);
	printcircle(PX + (SIZE / 2) * BBLACK + BBLACK / 2, PY + (SIZE / 2) * BBLACK + BBLACK / 2, white, chessboard);
	printcircle(PX + (SIZE / 2) * BBLACK + BBLACK / 2, PY + (SIZE / 2 - 1) * BBLACK + BBLACK / 2, black, chessboard);

	getimage(COUNT, WINDOWS_X / 30, WINDOWS_Y - WINDOWS_Y / 6, 230, 40);
	getimage(COUNT + 1, WINDOWS_X - WINDOWS_X / 5, WINDOWS_Y - WINDOWS_Y / 6, 230, 40);

	LOGFONT f;							//设置字体
	gettextstyle(&f);
	f.lfHeight = 35;
	strcpy_s(f.lfFaceName, "楷体");
	f.lfQuality = ANTIALIASED_QUALITY;
	settextstyle(&f);

	settextcolor(RED);
	outtextxy(BBLACK / 4, BBLACK / 2, "你的执子");
	outtextxy(BBLACK / 4, BBLACK / 4 + BBLACK, "当前执子");

	getchar();

}


int Judge(int x, int y, int color, int MAP[SIZE][SIZE])			//判断当前位置能否下子，黑白棋的特性
{
	if (MAP[x][y])
		return 0;				//当前位置有子，返回 0
	int me = color ? 1 : -1;	//下棋的颜色
	POINT2 star;
	int count = 0, flag;		//count 为此位置可以转换对手的棋数
	for (int i = 0; i < SIZE; ++i)
	{
		flag = 0;
		star.INIT(x + MOVE[i][0], y + MOVE[i][1]);  //向八个方位移动，遍历整个棋盘
		while (star.x >= 0 && star.x < SIZE && star.y > 0 && star.y < SIZE && MAP[star.x][star.y])		//重点 理解循环
		{
			if (MAP[star.x][star.y] == -me)			
				flag++;
			else
			{
				count += flag;
				break;
			}
			star.ADD(MOVE[i][0], MOVE[i][1]);
		}
	}
	return count;
}

void change(POINT2 NOW, int MAP[SIZE][SIZE], bool F)    //落子后改变棋盘的状态，动画实现，bool F决定是否输出到屏幕
{
	int me = MAP[NOW.x][NOW.y];							//当前所落子的颜色
	bool flag;											//flag原理类似Judge
	POINT2 a, b;
	for (int i = 0; i < SIZE; ++i)
	{
		flag = false;
		a.INIT(NOW.x + MOVE[i][0], NOW.y + MOVE[i][1]);
		while (a.x >= 0 && a.x < SIZE && a.y >= 0 && a.y < SIZE && MAP[a.x][a.y])
		{
			if (MAP[a.x][a.y] == -me)flag = true;
			else
			{
				if (flag)
				{
					a.ADD(-MOVE[i][0], -MOVE[i][1]);
					b.INIT(NOW.x + MOVE[i][0], NOW.y + MOVE[i][1]);
					while (((NOW.x <= b.x && b.x <= a.x) || (a.x <= b.x && b.x <= NOW.x)) && ((NOW.y <= b.y && b.y <= a.y) || (a.y <= b.y && b.y <= NOW.y)))
					{
						if (F)printcircle(b.MAP2WINX(), b.MAP2WINY(), (me == 1) ? black : white, MAP);		//改变棋子
						if (!F)chessadd(b.MAP2WINX(), b.MAP2WINY(), (me == 1) ? black : white, MAP);		//如果不输出到屏幕，改变地图数组
						b.ADD(MOVE[i][0], MOVE[i][1]);
					}
				}
				break;
			}
			a.ADD(MOVE[i][0], MOVE[i][1]);
		}
	}
}

int Statistics(int color)					// 可下子位置的展示，遍历棋盘所有位置，判断能否落子，以及落子后可转换的棋子数
{
	int NOWEXPECT = 0;						// 记录现在棋盘上可落子的位置数
	for(int i = 0; i < SIZE; ++i)
		for (int j = 0; j < SIZE; ++j)
		{
			expect[i][j] = Judge(i, j, color, chessboard);  //该点可转换的棋子数，没有则为0
			if (expect[i][j])
			{
				++NOWEXPECT;

				POINT2 a;
				a.INIT(i, j);
				setfillcolor(RGB(96, 96, 96));				//实现该点的动态生成，提示该点可以落子
				circle(a.MAP2WINX(), a.MAP2WINY(), CHESSSIZE / 4);
				circle(a.MAP2WINX(), a.MAP2WINY(), CHESSSIZE / 4 - 1);

				if (TIANYANS)								//开启天眼模式，进一步返回该点落子后可转换子的个数
				{
					settextstyle(15, 0, "楷体");
					TCHAR s[20];
					_stprintf_s(s, _T("%d"), expect[i][j]);	//显示可转换子的个数
					outtextxy(a.MAP2WINX(), a.MAP2WINY() + 10, s);
				}
			}
		}
	return NOWEXPECT;
}

void CleanStatistics()										//清除提示
{
	for (int i = 0; i < SIZE; ++i)
	{
		for (int j = 0; j < SIZE; ++j)
		{
			if (expect[i][j] && !chessboard[i][j]);			//该点没有棋子并且有提示点则清除
			{
				POINT2 a;
				a.INIT(i, j);
				putimage(a.MAP2WINX() - BBLACK / 2, a.MAP2WINY() - BBLACK / 2, &MAPIMAGE[i][j]); 
			}
		}
	}
}

string INTTOCHI(int num, int color)											//当前局势成绩输出汉字, 棋盘上颜色为color的棋子数量
{
	string number[10] = { "","一","二","三","四","五","六","七","八","九" };
	string data = "";
	if (num >= 10)
	{
		data += number[num / 10 % 10];
		data += "十";
	}
	data += number[num % 10];
	return (color ? "黑棋：" : "白棋：") + data;							//num>=0&&num<=64
}

void Printcount(int blackcount, int whitecount, int nowcolor)										//对当前分数的输出
{
	settextcolor(DARKGRAY);			//颜色
	settextstyle(35, 0, "楷体");		//字体

	putimage(WINDOWS_X / 30, WINDOWS_Y - WINDOWS_Y / 6, COUNT);		//擦出原来痕迹
	putimage(WINDOWS_X - WINDOWS_X / 5, WINDOWS_Y - WINDOWS_Y / 6, COUNT + 1);

	outtextxy(WINDOWS_X / 30, WINDOWS_Y - WINDOWS_Y / 6, INTTOCHI(whitecount, white).data());	//输出当前成绩
	outtextxy(WINDOWS_X - WINDOWS_X / 5, WINDOWS_Y - WINDOWS_Y / 6, INTTOCHI(blackcount, black).data());

	setfillcolor(MYCOLOR == 1 ? BLACK : MYCOLOR == 0 ? WHITE : LIGHTCYAN);						//MYCOLOR = 1黑色，MYCOLOR = 0白色，MYCOLOR其他淡色
	solidcircle(BBLACK * 2 + 10, BBLACK * 3 / 4, CHESSSIZE * 3 / 4);

	setfillcolor((!nowcolor || blackcount + whitecount == 4) ? BLACK : WHITE);					//初始时或与当前颜色不同为黑色
	NOWCOLOR = (!nowcolor || blackcount + whitecount == 4) ? black : white;

	for (int i = 0; i <= CHESSSIZE * 3 / 4; ++i)												//实现棋子的逐步放大
	{
		solidcircle(BBLACK * 2 + 10, BBLACK * 3 / 2, i);
		Sleep(1);
	}
}

void WIN(int YOURCOLOR, int blackcount, int whitecount)			//判断输赢
{
	HWND wnd = GetHWnd();										//获取窗口句柄
									//HWND被认为是指向窗口的HANDLE，被用来记录在屏幕上显示的不同对象。如果你想跟一个窗口交互，你必须拥有一个该窗口对象的handle，即HWND。
									/*在windows内核中，windows有一张记录了内核所需管理的不同对象的表格，这些对象如窗口，按钮，标签，菜单等。每一个这一类的对象都有一个唯一的标记值（类似ID），就是HANDLE。如果你想拿那张表中记录的对象，你就必须用相应的
									那个HANDLE去向windows拿，windows将返回对应的对象。*/
	if (blackcount > whitecount)
	{
		MessageBox(wnd, YOURCOLOR == black ? "恭喜你，胜利啦~" : YOURCOLOR == white ? "输了哦，再接再厉吧！！！" : "黑方得胜~", "Result", MB_OK);
	}
	else if (blackcount < whitecount)
	{
		MessageBox(wnd, YOURCOLOR == white ? "恭喜你，胜利啦~" : YOURCOLOR == black ? "输了哦，再接再厉吧！！！" : "白方得胜", "Result", MB_OK);
	}
	else
	{
		MessageBox(wnd, "噫~平局哎，要不要再来一次呢！", "Result", MB_OK);
	}
}

void HL(int NOWWJ)
{
	if (NOWWJ != -1)
	{
		HWND wnd = GetHWnd();										//获取窗口句柄
		MessageBox(wnd, NOWWJ == MYCOLOR ? "你没有可以下的子！" : "对方已无子可下！", "回合跳过", MB_OK);
	}
}

POINT2 MOUSE()
{
	MOUSEMSG m;
	while (true)
	{
		m = GetMouseMsg();						//获取鼠标的状态
		switch (m.uMsg)
		{
		case(WM_LBUTTONDOWN):					//左键点击,获取鼠标的位置
		{
			POINT2 NOWMOUSE;
			NOWMOUSE.INIT(m.x, m.y);
			return NOWMOUSE;
			break;
		}
		case(WM_MOUSEMOVE):								//调试 输出鼠标位置
		{
			if (ESCEXIT)startgame();
			break;
		}
		case(WM_RBUTTONDOWN):								//如果鼠标右键点下时
		{
			TIANYANS = !TIANYANS;							//开启OR关闭天眼模式
			break;
		}
		}
		
	}
}

bool putmouse(POINT2& m)
{
	bool flag = true;										//判断能否添加棋子到棋盘中
	int mouseinx[SIZE + 1], mouseiny[SIZE + 1];
	for (int i = 0; i < SIZE + 1; ++i)						//坐标映射到棋盘窗口像素点位置
	{
		mouseinx[i] = PX + i * BBLACK;
		mouseiny[i] = PY + i * BBLACK;
	}
	if (m.x < PX || m.x > PX + SIZE * BBLACK || m.y < PY || m.y > PY + SIZE * BBLACK)		//鼠标位于棋盘外
		flag = false;
	else													//鼠标位于某一正方形区域内时，定位到正中间，方便棋子的绘制
	{
		for (int i = 0; i < SIZE; ++i)
		{
			if (m.x >= mouseinx[i] && m.x <= mouseinx[i + 1])
			{
				if (m.x - mouseinx[i] > BBLACK / 8 && mouseinx[i + 1] - m.x > BBLACK / 8) //定位应更加准确，点到边角难以判断
				{
					m.x = (mouseinx[i] + mouseinx[i + 1]) / 2;
				}
				else flag = false;
			}
		}
		for (int i = 0; i < SIZE; ++i)
		{
			if (m.y >= mouseiny[i] && m.y <= mouseiny[i + 1])
			{
				if (m.y - mouseiny[i] > BBLACK / 8 && mouseiny[i + 1] - m.y > BBLACK / 8)		//重定向Y
				{
					m.y = (mouseiny[i] + mouseiny[i + 1]) / 2;
				}
				else flag = false;
			}
		}
	}
	return flag;
}


void Cleanlast(POINT2 WINDOWS2, int F)
{
	if (LATESTCHESS.x > SIZE && LATESTCHESS.y > SIZE)    //假如上一步棋子的位置不符合要求，取消
	{
		setfillcolor(getpixel(LATESTCHESS.x, LATESTCHESS.y));  //getpixel获取当前位置的RGB值
		putimage(LATESTCHESS.x - BBLACK / 2, LATESTCHESS.y - BBLACK / 2, &MAPIMAGE[LATESTCHESS.WIN2MAPX()][LATESTCHESS.WIN2MAPY()]);
		solidcircle(LATESTCHESS.x, LATESTCHESS.y, CHESSSIZE);
	}

	setfillcolor(RGB(49, 153, 182));					//当前走棋的填充
	LATESTCHESS.INIT(WINDOWS2.x, WINDOWS2.y);

	solidrectangle(WINDOWS2.x - BBLACK / 2 + 2, WINDOWS2.y - BBLACK / 2 + 2, WINDOWS2.x + BBLACK / 2 - 2, WINDOWS2.y + BBLACK / 2 - 2);		//背景矩形
	setfillcolor(F ? BLACK : WHITE);
	solidcircle(WINDOWS2.x, WINDOWS2.y, CHESSSIZE);			//画棋子

}

int Playchess(int F, POINT2 WINDOWS2, int& blackcount, int& whitecount)
{
	//F 黑为1， 白为0
	POINT2 MAP2;

	if (WINDOWS2.x < SIZE && WINDOWS2.y < SIZE)  //传入的坐标为矩阵坐标，则映射到像素点坐标
	{
		MAP2 = WINDOWS2;
		WINDOWS2.MAP2WIN(WINDOWS2);
	}
	else
	{
		if (!putmouse(WINDOWS2))				//鼠标输入坐标重定向
			return 0;
		WINDOWS2.WIN2MAP(MAP2);					//WINDOWS2存储重定向后的坐标， MAP2存储矩阵坐标
	}
	if (expect[MAP2.x][MAP2.y])					//该位置可以落子
	{
		CleanStatistics();						//清除提示
		if (F)
		{
			blackcount += expect[MAP2.x][MAP2.y] + 1; //可转换数加自身
			whitecount -= expect[MAP2.x][MAP2.y];
		}
		else
		{
			whitecount += expect[MAP2.x][MAP2.y] + 1;
			blackcount -= expect[MAP2.x][MAP2.y];
		}

		printcircle(WINDOWS2.x, WINDOWS2.y, F, chessboard);		//画当前棋子	
		Cleanlast(WINDOWS2, F);									//当前走棋提示
		change(MAP2, chessboard, true);							//落子后吃子的动态实现
		Printcount(blackcount, whitecount, F);					//打印分数

		if (blackcount + whitecount >= SIZE * SIZE || !blackcount || !whitecount)  //棋子落满或者黑棋或白棋被吃完了
			return 3;
		if (!Statistics(!F))						//当前对方可落子位置数为0，即对方无法落子
		{
			if (Statistics(F))						//己方可以落子
			{
				HL(!F);								//输出对方无法落子信息
				Printcount(blackcount, whitecount, !F);		//成绩输出
				return 2;
			}
			else
				return 3;							//双方都无法下棋
		}
		return 1;
	}
	return 0;
}				//鼠标错误或其他问题（不可落子）返回 0 ； 可落子，正常进行返回 1； 对方无法落子己方可以落子，返回相应信息和 2； 游戏结束返回 3 

void STARTVS(int YOURCOLOR, POINT2 P1(), POINT2 P2())			//开始对战
{
	int blackcount = 2, whitecount = 2;					//初始化最开始的四枚棋子

	MYCOLOR = YOURCOLOR;

	Printcount(blackcount, whitecount, black);			//当前成绩
	Statistics(black);
	while (true)
	{
	CX1:
		int PD = Playchess(black, (*P1)(), blackcount, whitecount);		//黑子的回合
		switch (PD)
		{
		case 0:
			goto CX1;									//鼠标错误或其他问题（不可落子）,重新落子
			break;
		case 1:
			break;										//正常进行
		case 2:
			goto CX1;									//对方无法落子，你继续落子
		case 3:
			goto ED;									//游戏结束
			break;
		}
	CX2:															
		PD = Playchess(!black, (*P2)(), blackcount, whitecount);
		switch (PD)
		{
		case 0:
			goto CX2;
			break;
		case 1:
			break;
		case 2:
			goto CX2;
			break;
		case 3:
			goto ED;
			break;
		}
	}
ED:
	WIN(YOURCOLOR, blackcount, whitecount);
	_getch();											//_getch() 函数读入一个字符时 不用 enter 代码会继续跑
	startgame();										//重新开始游戏
}

