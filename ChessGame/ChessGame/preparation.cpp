// ��Ϸǰ�ڹ���׼��

#include<stdio.h>
#include<graphics.h>
#include<string.h>
#include <iostream>
#include <conio.h>
#include "startgame.cpp"

using namespace std;

#define WINDOWS_X 1200
#define WINDOWS_Y 800

// ���������ӵ�׼��
#define PX 280				//����ƫ����X
#define PY 80				//����ƫ����Y
#define BBLACK 80			//�ո�Ĵ�С

#define CHESSSIZE 25		//���ӵĴ�С
#define SIZE 8				//���̵ĸ���

#define ESC 27				//ָ��ESC��ֵ
#define ESCEXIT (_kbhit()&&_getch()==ESC)   //_kbhit()�������Ƿ��а������з���-1��û�з���0
											//_getch()��ȡ��ֵ�Ѿ����뵽�����������߾���<conio.h>�ж���

const int black = 1;		//����
const int white = 0;		//����

int chessboard[SIZE][SIZE]; //��ά���飨���󣩴洢����
const int MOVE[8][2] = { { -1, 0 },{ 1, 0 },{ 0, -1 },{ 0, 1 },{ -1, -1 },{ 1, -1 },{ 1, 1 },{ -1, 1 } }; //�˸���λ
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
};										//���̸�λ��Ȩֵ���ƣ�Ϊ����AI׼��

IMAGE MAPIMAGE[SIZE][SIZE];             //���̻���׼��
IMAGE COUNT[2];

int expect[SIZE][SIZE];

int MYCOLOR;							//�������ӵ���ɫ ��ɫ1 ��ɫ0 ����-1
int NOWCOLOR;							//��ǰִ����ɫ
bool TIANYANS;							//�жϿ����� ����

class POINT2
{
public:
	int x, y;
	void INIT(int x, int y)				//��ʼ��
	{
		this->x = x;
		this->y = y;
	}
	void ADD(int x, int y)
	{
		this->x += x;
		this->y += y;
	}
	void WIN2MAP(POINT2& MAP)			//���̵�����
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
	void MAP2WIN(POINT2 &WIN)			//��������
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

POINT2 LATESTCHESS;						//��¼���һ�����ӵ�λ��

void chessadd(int x, int y, int color, int MAP[SIZE][SIZE])		//������ӵ�����,��ʵ�����ھ�������ض���ɫ�ĵ�
{
	POINT2 WINDOWS2, MAP2;
	WINDOWS2.INIT(x, y);
	WINDOWS2.WIN2MAP(MAP2);
	MAP[MAP2.x][MAP2.y] = color ? 1 : -1;						//ȷ���������ɫ
}

void printcircle(int x, int y, int color, int MAP[SIZE][SIZE])	//�������ӵ�ʵ��
{
	chessadd(x, y, color, MAP);

	setfillcolor(color ? BLACK : WHITE);
	for (int i = 0; i <= CHESSSIZE; ++i)
	{
		solidcircle(x, y, i);						//ʵ�����ӻ��ƹ����д������𽥷Ŵ����뾶Ϊ25�����ص�
		Sleep(1);									//��ʱ
	}
}

void boardinit()									//���̵Ļ���
{
	memset(chessboard, 0, sizeof(chessboard));		//��ʼ�� memset�Ǽ������C/C++���Գ�ʼ�������������ǽ�ĳһ���ڴ��е�����ȫ������Ϊָ����ֵ
	memset(expect, 0, sizeof(expect));

	TIANYANS = false;								//�ر�����ģʽ
	MYCOLOR = -1;									//���ü������Ӻ�ɫ
	LATESTCHESS.INIT(0, 0);							//��ʼ���ʼ������

	settextstyle(15, 0, "����");						//��15�����0
	setbkmode(TRANSPARENT);
	loadimage(NULL, "chessboard1.png");

	for (int x = PX; x < PX + BBLACK * (SIZE + 1); x += BBLACK) //�������̵ĺ���
	{
		if ((x / BBLACK % 2) == 0)
			setlinecolor(BLACK);
		else setlinecolor(RED);
		line(x, PY, x, PY + BBLACK * SIZE);
	}
	for (int y = PY; y < PY + BBLACK * (SIZE + 1); y += BBLACK)		//�������̵�����
	{
		if (y / BBLACK % 2 == 0)setlinecolor(BLACK);
		else setlinecolor(RED);
		line(PX, y, PX + BBLACK * SIZE, y);
	}
	for (int i = PX; i <= PX + BBLACK * SIZE; i += BBLACK)			//��ȡ�������λ��
	{
		for (int j = PY; j <= PY + BBLACK * SIZE; j += BBLACK)
		{
			POINT2 DATA;
			DATA.INIT(i, j);
			DATA.WIN2MAP(DATA);
			getimage(&MAPIMAGE[DATA.x][DATA.y], i, j, BBLACK, BBLACK); //getimage(����ͼ���ָ�룬���Ͻ�x���꣬y���꣬��ȣ��߶�)�������Ǵ���Ļ�����л�ȡͼ��

		}
	}
	printcircle(PX + (SIZE / 2 - 1) * BBLACK + BBLACK / 2, PY + (SIZE / 2 - 1)* BBLACK + BBLACK / 2, white, chessboard);
	printcircle(PX + (SIZE / 2 - 1) * BBLACK + BBLACK / 2, PY + (SIZE / 2) * BBLACK + BBLACK / 2, black, chessboard);
	printcircle(PX + (SIZE / 2) * BBLACK + BBLACK / 2, PY + (SIZE / 2) * BBLACK + BBLACK / 2, white, chessboard);
	printcircle(PX + (SIZE / 2) * BBLACK + BBLACK / 2, PY + (SIZE / 2 - 1) * BBLACK + BBLACK / 2, black, chessboard);

	getimage(COUNT, WINDOWS_X / 30, WINDOWS_Y - WINDOWS_Y / 6, 230, 40);
	getimage(COUNT + 1, WINDOWS_X - WINDOWS_X / 5, WINDOWS_Y - WINDOWS_Y / 6, 230, 40);

	LOGFONT f;							//��������
	gettextstyle(&f);
	f.lfHeight = 35;
	strcpy_s(f.lfFaceName, "����");
	f.lfQuality = ANTIALIASED_QUALITY;
	settextstyle(&f);

	settextcolor(RED);
	outtextxy(BBLACK / 4, BBLACK / 2, "���ִ��");
	outtextxy(BBLACK / 4, BBLACK / 4 + BBLACK, "��ǰִ��");

	getchar();

}


int Judge(int x, int y, int color, int MAP[SIZE][SIZE])			//�жϵ�ǰλ���ܷ����ӣ��ڰ��������
{
	if (MAP[x][y])
		return 0;				//��ǰλ�����ӣ����� 0
	int me = color ? 1 : -1;	//�������ɫ
	POINT2 star;
	int count = 0, flag;		//count Ϊ��λ�ÿ���ת�����ֵ�����
	for (int i = 0; i < SIZE; ++i)
	{
		flag = 0;
		star.INIT(x + MOVE[i][0], y + MOVE[i][1]);  //��˸���λ�ƶ���������������
		while (star.x >= 0 && star.x < SIZE && star.y > 0 && star.y < SIZE && MAP[star.x][star.y])		//�ص� ���ѭ��
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

void change(POINT2 NOW, int MAP[SIZE][SIZE], bool F)    //���Ӻ�ı����̵�״̬������ʵ�֣�bool F�����Ƿ��������Ļ
{
	int me = MAP[NOW.x][NOW.y];							//��ǰ�����ӵ���ɫ
	bool flag;											//flagԭ������Judge
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
						if (F)printcircle(b.MAP2WINX(), b.MAP2WINY(), (me == 1) ? black : white, MAP);		//�ı�����
						if (!F)chessadd(b.MAP2WINX(), b.MAP2WINY(), (me == 1) ? black : white, MAP);		//������������Ļ���ı��ͼ����
						b.ADD(MOVE[i][0], MOVE[i][1]);
					}
				}
				break;
			}
			a.ADD(MOVE[i][0], MOVE[i][1]);
		}
	}
}

int Statistics(int color)					// ������λ�õ�չʾ��������������λ�ã��ж��ܷ����ӣ��Լ����Ӻ��ת����������
{
	int NOWEXPECT = 0;						// ��¼���������Ͽ����ӵ�λ����
	for(int i = 0; i < SIZE; ++i)
		for (int j = 0; j < SIZE; ++j)
		{
			expect[i][j] = Judge(i, j, color, chessboard);  //�õ��ת������������û����Ϊ0
			if (expect[i][j])
			{
				++NOWEXPECT;

				POINT2 a;
				a.INIT(i, j);
				setfillcolor(RGB(96, 96, 96));				//ʵ�ָõ�Ķ�̬���ɣ���ʾ�õ��������
				circle(a.MAP2WINX(), a.MAP2WINY(), CHESSSIZE / 4);
				circle(a.MAP2WINX(), a.MAP2WINY(), CHESSSIZE / 4 - 1);

				if (TIANYANS)								//��������ģʽ����һ�����ظõ����Ӻ��ת���ӵĸ���
				{
					settextstyle(15, 0, "����");
					TCHAR s[20];
					_stprintf_s(s, _T("%d"), expect[i][j]);	//��ʾ��ת���ӵĸ���
					outtextxy(a.MAP2WINX(), a.MAP2WINY() + 10, s);
				}
			}
		}
	return NOWEXPECT;
}

void CleanStatistics()										//�����ʾ
{
	for (int i = 0; i < SIZE; ++i)
	{
		for (int j = 0; j < SIZE; ++j)
		{
			if (expect[i][j] && !chessboard[i][j]);			//�õ�û�����Ӳ�������ʾ�������
			{
				POINT2 a;
				a.INIT(i, j);
				putimage(a.MAP2WINX() - BBLACK / 2, a.MAP2WINY() - BBLACK / 2, &MAPIMAGE[i][j]); 
			}
		}
	}
}

string INTTOCHI(int num, int color)											//��ǰ���Ƴɼ��������, ��������ɫΪcolor����������
{
	string number[10] = { "","һ","��","��","��","��","��","��","��","��" };
	string data = "";
	if (num >= 10)
	{
		data += number[num / 10 % 10];
		data += "ʮ";
	}
	data += number[num % 10];
	return (color ? "���壺" : "���壺") + data;							//num>=0&&num<=64
}

void Printcount(int blackcount, int whitecount, int nowcolor)										//�Ե�ǰ���������
{
	settextcolor(DARKGRAY);			//��ɫ
	settextstyle(35, 0, "����");		//����

	putimage(WINDOWS_X / 30, WINDOWS_Y - WINDOWS_Y / 6, COUNT);		//����ԭ���ۼ�
	putimage(WINDOWS_X - WINDOWS_X / 5, WINDOWS_Y - WINDOWS_Y / 6, COUNT + 1);

	outtextxy(WINDOWS_X / 30, WINDOWS_Y - WINDOWS_Y / 6, INTTOCHI(whitecount, white).data());	//�����ǰ�ɼ�
	outtextxy(WINDOWS_X - WINDOWS_X / 5, WINDOWS_Y - WINDOWS_Y / 6, INTTOCHI(blackcount, black).data());

	setfillcolor(MYCOLOR == 1 ? BLACK : MYCOLOR == 0 ? WHITE : LIGHTCYAN);						//MYCOLOR = 1��ɫ��MYCOLOR = 0��ɫ��MYCOLOR������ɫ
	solidcircle(BBLACK * 2 + 10, BBLACK * 3 / 4, CHESSSIZE * 3 / 4);

	setfillcolor((!nowcolor || blackcount + whitecount == 4) ? BLACK : WHITE);					//��ʼʱ���뵱ǰ��ɫ��ͬΪ��ɫ
	NOWCOLOR = (!nowcolor || blackcount + whitecount == 4) ? black : white;

	for (int i = 0; i <= CHESSSIZE * 3 / 4; ++i)												//ʵ�����ӵ��𲽷Ŵ�
	{
		solidcircle(BBLACK * 2 + 10, BBLACK * 3 / 2, i);
		Sleep(1);
	}
}

void WIN(int YOURCOLOR, int blackcount, int whitecount)			//�ж���Ӯ
{
	HWND wnd = GetHWnd();										//��ȡ���ھ��
									//HWND����Ϊ��ָ�򴰿ڵ�HANDLE����������¼����Ļ����ʾ�Ĳ�ͬ������������һ�����ڽ����������ӵ��һ���ô��ڶ����handle����HWND��
									/*��windows�ں��У�windows��һ�ż�¼���ں��������Ĳ�ͬ����ı����Щ�����細�ڣ���ť����ǩ���˵��ȡ�ÿһ����һ��Ķ�����һ��Ψһ�ı��ֵ������ID��������HANDLE��������������ű��м�¼�Ķ�����ͱ�������Ӧ��
									�Ǹ�HANDLEȥ��windows�ã�windows�����ض�Ӧ�Ķ���*/
	if (blackcount > whitecount)
	{
		MessageBox(wnd, YOURCOLOR == black ? "��ϲ�㣬ʤ����~" : YOURCOLOR == white ? "����Ŷ���ٽ������ɣ�����" : "�ڷ���ʤ~", "Result", MB_OK);
	}
	else if (blackcount < whitecount)
	{
		MessageBox(wnd, YOURCOLOR == white ? "��ϲ�㣬ʤ����~" : YOURCOLOR == black ? "����Ŷ���ٽ������ɣ�����" : "�׷���ʤ", "Result", MB_OK);
	}
	else
	{
		MessageBox(wnd, "��~ƽ�ְ���Ҫ��Ҫ����һ���أ�", "Result", MB_OK);
	}
}

void HL(int NOWWJ)
{
	if (NOWWJ != -1)
	{
		HWND wnd = GetHWnd();										//��ȡ���ھ��
		MessageBox(wnd, NOWWJ == MYCOLOR ? "��û�п����µ��ӣ�" : "�Է������ӿ��£�", "�غ�����", MB_OK);
	}
}

POINT2 MOUSE()
{
	MOUSEMSG m;
	while (true)
	{
		m = GetMouseMsg();						//��ȡ����״̬
		switch (m.uMsg)
		{
		case(WM_LBUTTONDOWN):					//������,��ȡ����λ��
		{
			POINT2 NOWMOUSE;
			NOWMOUSE.INIT(m.x, m.y);
			return NOWMOUSE;
			break;
		}
		case(WM_MOUSEMOVE):								//���� ������λ��
		{
			if (ESCEXIT)startgame();
			break;
		}
		case(WM_RBUTTONDOWN):								//�������Ҽ�����ʱ
		{
			TIANYANS = !TIANYANS;							//����OR�ر�����ģʽ
			break;
		}
		}
		
	}
}

bool putmouse(POINT2& m)
{
	bool flag = true;										//�ж��ܷ�������ӵ�������
	int mouseinx[SIZE + 1], mouseiny[SIZE + 1];
	for (int i = 0; i < SIZE + 1; ++i)						//����ӳ�䵽���̴������ص�λ��
	{
		mouseinx[i] = PX + i * BBLACK;
		mouseiny[i] = PY + i * BBLACK;
	}
	if (m.x < PX || m.x > PX + SIZE * BBLACK || m.y < PY || m.y > PY + SIZE * BBLACK)		//���λ��������
		flag = false;
	else													//���λ��ĳһ������������ʱ����λ�����м䣬�������ӵĻ���
	{
		for (int i = 0; i < SIZE; ++i)
		{
			if (m.x >= mouseinx[i] && m.x <= mouseinx[i + 1])
			{
				if (m.x - mouseinx[i] > BBLACK / 8 && mouseinx[i + 1] - m.x > BBLACK / 8) //��λӦ����׼ȷ���㵽�߽������ж�
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
				if (m.y - mouseiny[i] > BBLACK / 8 && mouseiny[i + 1] - m.y > BBLACK / 8)		//�ض���Y
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
	if (LATESTCHESS.x > SIZE && LATESTCHESS.y > SIZE)    //������һ�����ӵ�λ�ò�����Ҫ��ȡ��
	{
		setfillcolor(getpixel(LATESTCHESS.x, LATESTCHESS.y));  //getpixel��ȡ��ǰλ�õ�RGBֵ
		putimage(LATESTCHESS.x - BBLACK / 2, LATESTCHESS.y - BBLACK / 2, &MAPIMAGE[LATESTCHESS.WIN2MAPX()][LATESTCHESS.WIN2MAPY()]);
		solidcircle(LATESTCHESS.x, LATESTCHESS.y, CHESSSIZE);
	}

	setfillcolor(RGB(49, 153, 182));					//��ǰ��������
	LATESTCHESS.INIT(WINDOWS2.x, WINDOWS2.y);

	solidrectangle(WINDOWS2.x - BBLACK / 2 + 2, WINDOWS2.y - BBLACK / 2 + 2, WINDOWS2.x + BBLACK / 2 - 2, WINDOWS2.y + BBLACK / 2 - 2);		//��������
	setfillcolor(F ? BLACK : WHITE);
	solidcircle(WINDOWS2.x, WINDOWS2.y, CHESSSIZE);			//������

}

int Playchess(int F, POINT2 WINDOWS2, int& blackcount, int& whitecount)
{
	//F ��Ϊ1�� ��Ϊ0
	POINT2 MAP2;

	if (WINDOWS2.x < SIZE && WINDOWS2.y < SIZE)  //���������Ϊ�������꣬��ӳ�䵽���ص�����
	{
		MAP2 = WINDOWS2;
		WINDOWS2.MAP2WIN(WINDOWS2);
	}
	else
	{
		if (!putmouse(WINDOWS2))				//������������ض���
			return 0;
		WINDOWS2.WIN2MAP(MAP2);					//WINDOWS2�洢�ض��������꣬ MAP2�洢��������
	}
	if (expect[MAP2.x][MAP2.y])					//��λ�ÿ�������
	{
		CleanStatistics();						//�����ʾ
		if (F)
		{
			blackcount += expect[MAP2.x][MAP2.y] + 1; //��ת����������
			whitecount -= expect[MAP2.x][MAP2.y];
		}
		else
		{
			whitecount += expect[MAP2.x][MAP2.y] + 1;
			blackcount -= expect[MAP2.x][MAP2.y];
		}

		printcircle(WINDOWS2.x, WINDOWS2.y, F, chessboard);		//����ǰ����	
		Cleanlast(WINDOWS2, F);									//��ǰ������ʾ
		change(MAP2, chessboard, true);							//���Ӻ���ӵĶ�̬ʵ��
		Printcount(blackcount, whitecount, F);					//��ӡ����

		if (blackcount + whitecount >= SIZE * SIZE || !blackcount || !whitecount)  //�����������ߺ������屻������
			return 3;
		if (!Statistics(!F))						//��ǰ�Է�������λ����Ϊ0�����Է��޷�����
		{
			if (Statistics(F))						//������������
			{
				HL(!F);								//����Է��޷�������Ϣ
				Printcount(blackcount, whitecount, !F);		//�ɼ����
				return 2;
			}
			else
				return 3;							//˫�����޷�����
		}
		return 1;
	}
	return 0;
}				//��������������⣨�������ӣ����� 0 �� �����ӣ��������з��� 1�� �Է��޷����Ӽ����������ӣ�������Ӧ��Ϣ�� 2�� ��Ϸ�������� 3 

void STARTVS(int YOURCOLOR, POINT2 P1(), POINT2 P2())			//��ʼ��ս
{
	int blackcount = 2, whitecount = 2;					//��ʼ���ʼ����ö����

	MYCOLOR = YOURCOLOR;

	Printcount(blackcount, whitecount, black);			//��ǰ�ɼ�
	Statistics(black);
	while (true)
	{
	CX1:
		int PD = Playchess(black, (*P1)(), blackcount, whitecount);		//���ӵĻغ�
		switch (PD)
		{
		case 0:
			goto CX1;									//��������������⣨�������ӣ�,��������
			break;
		case 1:
			break;										//��������
		case 2:
			goto CX1;									//�Է��޷����ӣ����������
		case 3:
			goto ED;									//��Ϸ����
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
	_getch();											//_getch() ��������һ���ַ�ʱ ���� enter ����������
	startgame();										//���¿�ʼ��Ϸ
}

