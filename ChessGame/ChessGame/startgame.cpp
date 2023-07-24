

#include <graphics.h>
#include <iostream>
#include <time.h>
#include<stdlib.h>
#include<stdio.h>
#include <conio.h>
#include<algorithm>
#include<math.h>
#include <map>



// ��Ϸǰ�ڹ���׼��




using namespace std;

#define WINDOWS_X 1200
#define WINDOWS_Y 800
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

void startgame();

class POINT2
{
public:
	void WIN2MAP(POINT2& MAP)			//��������������ӳ���ϵ
	{
		MAP.x = (x - PX) / BBLACK;
		MAP.y = (y - PY) / BBLACK;
	}
	void MAP2WIN(POINT2& WIN)			//�������������̵�ӳ���ϵ
	{
		WIN.x = PX + x * BBLACK + BBLACK / 2;
		WIN.y = PY + y * BBLACK + BBLACK / 2;
	}
	void INIT(int x, int y)				//����Ԫ��
	{
		this->x = x;
		this->y = y;
	}
	void ADD(int x, int y)				//�����
	{
		this->x += x;
		this->y += y;
	}
	int MAP2WINX()
	{
		return PX + x * BBLACK + BBLACK / 2;
	}
	int MAP2WINY()
	{
		return PY + y * BBLACK + BBLACK / 2;
	}
	int WIN2MAPX()
	{
		return (x - PX) / BBLACK;
	}
	int WIN2MAPY()
	{
		return (y - PY) / BBLACK;
	}
	int x, y;
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
	printcircle(PX + (SIZE / 2 - 1) * BBLACK + BBLACK / 2, PY + (SIZE / 2 - 1) * BBLACK + BBLACK / 2, white, chessboard);
	printcircle(PX + (SIZE / 2 - 1) * BBLACK + BBLACK / 2, PY + (SIZE / 2) * BBLACK + BBLACK / 2, black, chessboard);
	printcircle(PX + (SIZE / 2) * BBLACK + BBLACK / 2, PY + (SIZE / 2) * BBLACK + BBLACK / 2, white, chessboard);
	printcircle(PX + (SIZE / 2) * BBLACK + BBLACK / 2, PY + (SIZE / 2 - 1) * BBLACK + BBLACK / 2, black, chessboard);

	getimage(COUNT, WINDOWS_X / 30, WINDOWS_Y - WINDOWS_Y / 6, 230, 40);
	getimage(COUNT + 1, WINDOWS_X - WINDOWS_X / 5, WINDOWS_Y - WINDOWS_Y / 6, 230, 40);

	LOGFONT f;							//��������
	gettextstyle(&f);
	f.lfHeight = 35;
	strcpy_s(f.lfFaceName, _T("����"));
	f.lfQuality = ANTIALIASED_QUALITY;
	settextstyle(&f);

	settextcolor(RED);
	outtextxy(BBLACK / 4 - 10, BBLACK / 2, "���ִ��");
	outtextxy(BBLACK / 4 - 10, BBLACK / 4 + BBLACK, "��ǰִ��");

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
			if (MAP[a.x][a.y] == -me)
				flag = true;
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
	for (int i = 0; i < SIZE; ++i)
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
			if (expect[i][j] && !chessboard[i][j])			//�õ�û�����Ӳ�������ʾ�������
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
	settextcolor(RED);			//��ɫ
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
			if (ESCEXIT)
				startgame();
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

POINT2 Easy()											//�������ӿɷ������ĶԷ����ӵ�λ��
{
	POINT2 MAX;											//��¼���Ž�
	MAX.INIT(0, 0);
	int maxcount = 0;
	for (int i = 0; i < SIZE; ++i)						//����Ѱ�Ҹ�λ�ã������Ϊһ������1���������������ģ������½�λ��
	{
		for (int j = 0; j < SIZE; ++j)
		{
			if (expect[i][j] >= maxcount)
			{
				maxcount = expect[i][j];
				MAX.INIT(i, j);
			}
		}
	}
	if (ESCEXIT)									//��ESC��ʱ���˻ص���ʼ����
		startgame();
	Sleep(1000);										//��Ъһ��ʱ��

	return MAX;				
}

void copymap(int one[SIZE][SIZE], int last[SIZE][SIZE])			//��last�����Ƶ�one����
{
	for (int i = 0; i < SIZE; i++)
		for (int j = 0; j < SIZE; j++)
			one[i][j] = last[i][j];
}

POINT2 Middle()						//�е��Ѷ�
{
	int ME = 0;							//ME��¼��λ�õ�Ȩֵ
	int maxx = 0;						//����maxx��λ�ã����ڼ�¼����

	struct _ADD
	{
		int x;										//X����
		int y;										//Y����
		int w;										//Ȩֵ
		void init(int x, int y, int w)
		{
			this->x = x;
			this->y = y;
			this->w = w;
		}
		bool operator < (_ADD a)					//���رȽ������
		{
			return w > a.w;
		}
		POINT2 INTOPOINT2()							//ת��ΪPOINT2����
		{
			POINT2 data;
			data.INIT(x, y);
			return data;
		}
	}WEA[SIZE * SIZE];					//WEA�����¼

	int expectnow[SIZE][SIZE], mapnow[SIZE][SIZE];
	if (ESCEXIT)
		startgame();					//��ESC���¿�ʼ
	Sleep(800);

	for (int i = 0; i < SIZE; ++i)
		for (int j = 0; j < SIZE; ++j)	//����
		{
			if (expect[i][j])			//�����ǰλ�ÿ�����
			{
				ME = CHESSWEIGHT[i][j] + expect[i][j];			//Ȩֵ=Ԥ����õ�Ȩֵ���ϵ�ǰλ�����ӿ�ת���Է�������
				copymap(mapnow, chessboard);
				mapnow[i][j] = NOWCOLOR ? 1 : -1;
				POINT2 NOWPOINT;
				NOWPOINT.INIT(i, j);
				if ((i == 0 && j == 0) || (i == 0 && j == SIZE) || (i == SIZE && j == 0) || (i == SIZE && j == SIZE))
					return NOWPOINT;							//���ȷ��ؽ�����

				change(NOWPOINT, mapnow, false);				//������Ļ��ʾ�ı�����
				int YOU = -1050;
				for (int k = 0; k < SIZE; ++k)
					for (int l = 0; l < SIZE; ++l)
					{
						expectnow[k][l] = Judge(k, l, !NOWCOLOR, mapnow);		//�ж϶����ж���
						if (expectnow[k][l])
						{
							YOU = YOU < CHESSWEIGHT[k][l] + expectnow[k][l] ? CHESSWEIGHT[k][l] + expectnow[k][l] : YOU;
						}
					}
				WEA[maxx++].init(i, j, ME - YOU);						//WEA��¼��λ�õ��ж�����ֵ�����鷽����������ҳ��������ֵ
			}
		}
		sort(WEA, WEA + maxx);													//����Ȩֵ����
	for (int i = 0; i < maxx; ++i)
	{
		if ((WEA[i].x < 2 && WEA[i].y < 2) || (WEA[i].x < 2 && SIZE - WEA[i].y - 1 < 2) || (SIZE - 1 - WEA[i].x < 2 && WEA[i].y < 2) || (SIZE - 1 - WEA[i].x < 2 && SIZE - 1 - WEA[i].y < 2))continue;
		return WEA[i].INTOPOINT2();											//���طǽǱ����Ž�
	}
	return WEA[0].INTOPOINT2();												//���ؽǱ����Ž�,sort���������ֵ����0λ
}

// ����Сֵ�����ҵ����ŵ�����
// x,yΪ��ǰ����λ�ã����ص�ǰ���������·��÷�
// mapnowΪ��ǰ����״̬��expectnowΪ��ǰ���̿�����״̬
// depin,depinmaxΪѭ��Ѱ�����
int findscoremax(int x, int y, int mapnow[SIZE][SIZE], int expectnow[SIZE][SIZE], int depin, int depinmax)
{
	if (depin >= depinmax)			//�ݹ����
		return 0;	

	int maxx = -10005;				//���Ȩֵ��Ҳ�����ķ���ֵ
	POINT2 NOW;

	int expectnow2[SIZE][SIZE], mapnow2[SIZE][SIZE], mapnext[SIZE][SIZE], expectlast[SIZE][SIZE];	//������ʱ����

	copymap(mapnow2, mapnow);			//���Ƶ�ǰ����

	mapnow2[x][y] = NOWCOLOR ? 1 : -1;	//ģ���ڵ�ǰ��������
	int ME = CHESSWEIGHT[x][y] + expectnow[x][y];	//��ǰ����ȨΪȨ�ؼӿ�ת��������
	NOW.INIT(x, y);

	change(NOW, mapnow2, false);		//ģ�����嵫���ı����̣�������ʾ����

	int MAXEXPECT = 0, LINEEXPECT = 0, COUNT = 0;
	for (int i = 0; i < SIZE; ++i)
		for (int j = 0; j < SIZE; ++j)
		{
			expectnow2[i][j] = Judge(i, j, !NOWCOLOR, mapnow2);		//���ֿ����ӵ�״̬��ע���ʱAI�Ѿ�ģ���¹���
			if (expectnow2[i][j])
			{
				++MAXEXPECT;		//������λ��������������������
				if ((i == 0 && j == 0) || (i == 0 && j == SIZE - 1) || (i == SIZE - 1 && j == SIZE - 1) || (i == SIZE - 1 && j == 0))
					return -1800;	//����Է���ռ�ǵĿ��ܣ������öԷ�������ռ��
				if ((i < 2 && j < 2) || (i < 2 && SIZE - j - 1 < 2) || (SIZE - 1 - i < 2 && j < 2) || (SIZE - 1 - i < 2 && SIZE - 1 - j < 2))
					++LINEEXPECT;	//�����ܽ��������뿿�ǵ������㣨���㣩
			}
		}

	if (LINEEXPECT * 10 > MAXEXPECT * 7)		//�Է��ߵ��Ļ���״̬�ܶ࣬���֦
		return 1400;

	for (int i = 0; i < SIZE; i++)
		for (int j = 0; j < SIZE; j++)
			if (expectnow2[i][j])				//����Է���������
			{
				int YOU = CHESSWEIGHT[i][j] + expectnow2[i][j];		//�Է����ӵ÷�ֵ
				copymap(mapnext, mapnow2);		//������ͼ��һ��һ��ʹ��
				mapnext[i][j] = (!NOWCOLOR) ? 1 : -1;	//ģ��Է�����
				NOW.INIT(i, j);
				change(NOW, mapnext, false);		//�ı����̵�����ʾ	

				for (int k = 0; k < SIZE; k++)
					for (int l = 0; l < SIZE; l++)
						expectlast[k][l] = Judge(k, l, NOWCOLOR, mapnext);	//Ѱ��AI���е�

				for (int k = 0; k < SIZE; k++)
					for (int l = 0; l < SIZE; l++)
						if (expectlast[k][l])
						{
							int nowm = ME - YOU + findscoremax(k, l, mapnext, expectlast, depin + 1, depinmax);
							maxx = maxx < nowm ? nowm : maxx;
						}
			}

	return maxx;
}

POINT2 Difficult()
{
	POINT2 MAX;			//�������ӵ�λ��
	int maxx = -10005;
	MAX.INIT(0, 0);

	for (int i = 0; i < SIZE; i++)
		for (int j = 0; j < SIZE; j++)
		{
			if (expect[i][j])
			{
				if ((i == 0 && j == 0) || (i == 0 && j == SIZE - 1) || (i == SIZE - 1 && j == SIZE - 1) || (i == SIZE - 1 && j == 0))
				{
					MAX.INIT(i, j);
					return MAX;										//����ڽǣ����ؽ�����
				}
				int k = findscoremax(i, j, chessboard, expect, 0, 3);					//�ݹ����� ��������
				if (k >= maxx)
				{
					maxx = k;
					MAX.INIT(i, j);
				}
			}
		}
	return MAX;

}

void startgame()
{
	IMAGE MM[6] = { 0 }, MB[3] = { 0 };
	initgraph(WINDOWS_X, WINDOWS_Y);
	setbkmode(TRANSPARENT);						//��������Ϊ͸��ɫ
	settextcolor(RED);
	setlinecolor(BLACK);
	setlinestyle(PS_SOLID, 2);
	

	HWND hwnd = GetHWnd();						//���ô��ڱ�������
	SetWindowText(hwnd, "�ڰ���");
	loadimage(NULL, "./startpicture2.png");		//���뿪ʼͼƬ
	
	const int bblack = 10;

	LOGFONT f;									//���ý������� ������ʽ
	gettextstyle(&f);
	
	f.lfHeight = BBLACK;						//���ָ߶�Ϊ80
	strcpy_s(f.lfFaceName, _T("����"));			//��������Ϊ����
	f.lfQuality = ANTIALIASED_QUALITY;			//��������Ϊ�����
	settextstyle(&f);							//Ӧ��������ʽ

	RECT r1 = {0, 0, WINDOWS_X, WINDOWS_Y / 3 };//��Ϸ��
	drawtext("�� �� ��", &r1, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
							//����        ��ֱ����      ������ʾ

	settextstyle(BBLACK / 2, 0, "����");
	RECT r2 = { WINDOWS_X / 2 - BBLACK,WINDOWS_Y / 3,WINDOWS_X / 2 + BBLACK ,WINDOWS_Y / 3 + BBLACK / 2 };
	rectangle(WINDOWS_X / 2 - BBLACK, WINDOWS_Y / 3, WINDOWS_X / 2 + BBLACK, WINDOWS_Y / 3 + BBLACK / 2);
	drawtext("����ģʽ", &r2, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

	RECT r3 = { WINDOWS_X / 2 - BBLACK,WINDOWS_Y / 3 + BBLACK / 2 + 3 * bblack,WINDOWS_X / 2 + BBLACK,WINDOWS_Y / 3 + BBLACK + 3 * bblack };
	rectangle(WINDOWS_X / 2 - BBLACK, WINDOWS_Y / 3 + BBLACK / 2 + 3 * bblack, WINDOWS_X / 2 + BBLACK, WINDOWS_Y / 3 + BBLACK + 3 * bblack);
	drawtext("˫��ģʽ", &r3, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

	RECT r4 = { WINDOWS_X / 2 - BBLACK,WINDOWS_Y / 3 + BBLACK + 6 * bblack,WINDOWS_X / 2 + BBLACK,(int)(WINDOWS_Y / 3 + BBLACK * 1.5 + 6 * bblack) };
	rectangle(WINDOWS_X / 2 - BBLACK, WINDOWS_Y / 3 + BBLACK + 6 * bblack, WINDOWS_X / 2 + BBLACK, (int)(WINDOWS_Y / 3 + BBLACK * 1.5 + 6 * bblack));
	drawtext("��Ϸ˵��", &r4, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

	RECT r5 = { WINDOWS_X / 2 - BBLACK,(int)(WINDOWS_Y / 3 + BBLACK * 1.5 + 9 * bblack),WINDOWS_X / 2 + BBLACK,WINDOWS_Y / 3 + BBLACK * 2 + 9 * bblack };
	rectangle(WINDOWS_X / 2 - BBLACK, (int)(WINDOWS_Y / 3 + BBLACK * 1.5 + 9 * bblack), WINDOWS_X / 2 + BBLACK, WINDOWS_Y / 3 + BBLACK * 2 + 9 * bblack);
	drawtext("��    ��", &r5, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

	RECT r6 = { WINDOWS_X / 2 - BBLACK,WINDOWS_Y / 3 + BBLACK * 2 + 12 * bblack,WINDOWS_X / 2 + BBLACK,(int)(WINDOWS_Y / 3 + BBLACK * 2.5 + 12 * bblack) };
	rectangle(WINDOWS_X / 2 - BBLACK, WINDOWS_Y / 3 + BBLACK * 2 + 12 * bblack, WINDOWS_X / 2 + BBLACK, (int)(WINDOWS_Y / 3 + BBLACK * 2.5 + 12 * bblack));
	drawtext("�˳���Ϸ", &r6, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

	for (int i = 0; i < 5; i++)																						//���水ťͼƬ
		getimage(MM + i, WINDOWS_X / 2 - BBLACK, WINDOWS_Y / 3 + i * bblack * 3 + i * BBLACK / 2, 2 * BBLACK, BBLACK / 2);
	getimage(MM + 5, WINDOWS_X - BBLACK, WINDOWS_Y - BBLACK / 2, BBLACK - 11, BBLACK / 2 - 11);

	bool _HOME = true, _INTRODUCTION = false, _ABOUT = false,  _DRMS = false;			//TRUE��ʾ���ڵ�ǰҳ��
	MOUSEMSG m;
	while (_HOME)
	{
		BeginBatchDraw();													//������ͼ��ʼ
		m = GetMouseMsg();
		switch (m.uMsg)
		{
		case WM_LBUTTONDOWN:												//������������ʱ
			EndBatchDraw();													//������ͼ����
			if (m.x > WINDOWS_X / 2 - BBLACK && m.x<WINDOWS_X / 2 + BBLACK && m.y>WINDOWS_Y / 3 && m.y < WINDOWS_Y / 3 + BBLACK / 2 && _HOME && !_INTRODUCTION  && !_ABOUT  && !_DRMS)//����ģʽ
			{
				_DRMS = true;									//�뿪HOME����
				cleardevice();
				loadimage(NULL, "startpicture2.png");				//����

				settextstyle(&f);
				RECT r0 = { 0, 0, WINDOWS_X, WINDOWS_Y / 3 };
				drawtext("ѡ���Ѷ�", &r1, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

				settextstyle(BBLACK / 2, 0, "����");
				RECT r1 = { WINDOWS_X / 2 - BBLACK,WINDOWS_Y / 3,WINDOWS_X / 2 + BBLACK ,WINDOWS_Y / 3 + BBLACK / 2 };
				rectangle(WINDOWS_X / 2 - BBLACK, WINDOWS_Y / 3, WINDOWS_X / 2 + BBLACK, WINDOWS_Y / 3 + BBLACK / 2);
				drawtext("��    ��", &r1, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

				RECT r2 = { WINDOWS_X / 2 - BBLACK,WINDOWS_Y / 3 + BBLACK + 2 * bblack,WINDOWS_X / 2 + BBLACK,(int)(WINDOWS_Y / 3 + BBLACK * 1.5 + 2 * bblack) };
				rectangle(WINDOWS_X / 2 - BBLACK, WINDOWS_Y / 3 + BBLACK + 2 * bblack, WINDOWS_X / 2 + BBLACK, (int)(WINDOWS_Y / 3 + BBLACK * 1.5 + 2 * bblack));
				drawtext("��    ��", &r2, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

				RECT r3 = { WINDOWS_X / 2 - BBLACK,WINDOWS_Y / 3 + BBLACK * 2 + 4 * bblack,WINDOWS_X / 2 + BBLACK,(int)(WINDOWS_Y / 3 + BBLACK * 2.5 + 4 * bblack) };
				rectangle(WINDOWS_X / 2 - BBLACK, WINDOWS_Y / 3 + BBLACK * 2 + 4 * bblack, WINDOWS_X / 2 + BBLACK, (int)(WINDOWS_Y / 3 + BBLACK * 2.5 + 4 * bblack));
				drawtext("��    ��", &r3, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

				for (int i = 0; i < 3; i++)
				{
					getimage(MB + i, WINDOWS_X / 2 - BBLACK, WINDOWS_Y / 3 + BBLACK * i + 2 * i * bblack, 2 * BBLACK, BBLACK / 2);
				}

				RECT R = { WINDOWS_X - BBLACK,WINDOWS_Y - BBLACK / 2,WINDOWS_X - 10,WINDOWS_Y - 10 };
				rectangle(WINDOWS_X - BBLACK, WINDOWS_Y - BBLACK / 2, WINDOWS_X - 10, WINDOWS_Y - 10);
				drawtext("����", &R, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
				break;
			}

			else if (m.x > WINDOWS_X / 2 - BBLACK && m.x<WINDOWS_X / 2 + BBLACK && m.y>WINDOWS_Y / 3 && m.y < WINDOWS_Y / 3 + BBLACK / 2 && _DRMS)			//��
			{
				_HOME = false;
				cleardevice();
				boardinit();
				STARTVS(black, MOUSE, Easy);							//���˼�ģʽ
				break;
			}

			else if (m.x > WINDOWS_X / 2 - BBLACK && m.x<WINDOWS_X / 2 + BBLACK && m.y>WINDOWS_Y / 3 + BBLACK + 2 * bblack && m.y < (int)(WINDOWS_Y / 3 + BBLACK * 1.5 + 2 * bblack) && _DRMS)			//�е�
			{
				_HOME = false;
				cleardevice();
				boardinit();
				STARTVS(black, MOUSE, Middle);							//�����е�ģʽ
				break;
			}

			else if (m.x > WINDOWS_X / 2 - BBLACK && m.x<WINDOWS_X / 2 + BBLACK && m.y> WINDOWS_Y / 3 + BBLACK * 2 + 4 * bblack && m.y < (int)(WINDOWS_Y / 3 + BBLACK * 2.5 + 4 * bblack) && _DRMS)			//����
			{
				_HOME = false;
				cleardevice();
				boardinit();
				STARTVS(black, MOUSE, Difficult);							//��������ģʽ
				break;
			}


			else if (m.x > WINDOWS_X / 2 - BBLACK && m.x < WINDOWS_X / 2 + BBLACK && m.y> WINDOWS_Y / 3 + 0.5 * BBLACK + 3 * bblack && m.y < (int)(WINDOWS_Y / 3 + BBLACK + 3 * bblack) && _HOME && !_INTRODUCTION && !_ABOUT  && !_DRMS) //˫�˶�ս
			{
				_HOME = false;
				boardinit();
				STARTVS(-1, MOUSE, MOUSE);
				break;
			}


			else if (m.x > WINDOWS_X / 2 - BBLACK && m.x<WINDOWS_X / 2 + BBLACK && m.y> WINDOWS_Y / 3 + BBLACK + 6 * bblack && m.y < (int)(WINDOWS_Y / 3 + BBLACK * 1.5 + 6 * bblack) && _HOME && !_INTRODUCTION && !_ABOUT   && !_DRMS)	//��Ϸ����
			{
				_INTRODUCTION = true;
				cleardevice();
				loadimage(NULL, "startpicture3.png");				//����
				using namespace std;
				string data[15] = {
					"��Ϸ���ܣ�" ,
					"�ڰ���ͨ���໥��ת�Է������ӣ������������˭�����Ӷ����ж�ʤ��",
					"�ڰ�������ÿ���ɺڰ���ɫ��ɣ�һ��ף�һ��ڣ���64�����������������4��)",

					"���̿ɷ�Ϊ���� �� �и������ֽ�����̶���8x8��Ϊ�ձ�"
					"�ڰ����������һ����8*8���������",
					"��ʼʱ���������������������ĸ����ӽ�����ã���������������",
					"���Լ����µ������ںᡢ����б�˸���������һ���Լ������ӣ��������м��ȫ����ת���Ϊ�Լ�������",

					"ֻ���ڿ��Է�ת���ӵĵط��ſ�������",
					"��������������û�еط��������ӣ������Ҷ��ֿ�������",
					"˫����û�����ӿ�����ʱ��ֽ�������������Ŀ������ʤ�������Ӷ��һ����ʤ",
					"�����̻�û������ʱ�����һ���������Ѿ����Է��Թ⣬�����Ҳ����",

					"����Ϸ˫���������ٰ���������ʱ����Ϸ�ͽ�����",
					"ͨ������Ϸ����ʱ�����ϻ���������ӡ�����ʱ˭���������˭����Ӯ��",
				};

				rectangle(BBLACK, BBLACK, WINDOWS_X - BBLACK, WINDOWS_Y - BBLACK);
				settextstyle(BBLACK / 2 - 5, 0, "����");
				settextcolor(RGB(100, 100, 255));
				outtextxy(100, 90, data[0].data());
				settextstyle(BBLACK / 3.5, 0, "����");
				settextcolor(RGB(0, 255, 255));

				int LEFT = 20;
				int TOP = 115;
				for (int i = 1; i < 15; i++)
				{
					LEFT = 100;

					if (i == 5 || i == 10 || i == 6)TOP += 30;
					outtextxy(LEFT, TOP + 40 * i, data[i].data());
				}
				RECT R1 = { WINDOWS_X - BBLACK,WINDOWS_Y - BBLACK / 2,WINDOWS_X - 10,WINDOWS_Y - 10 };
				rectangle(WINDOWS_X - BBLACK, WINDOWS_Y - BBLACK / 2, WINDOWS_X - 10, WINDOWS_Y - 10);
				drawtext("����", &R1, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
				break;
			}

			else if (m.x > WINDOWS_X / 2 - BBLACK && m.x<WINDOWS_X / 2 + BBLACK && m.y>WINDOWS_Y / 3 + BBLACK * 1.5 + 9 * bblack && m.y < (int)(WINDOWS_Y / 3 + BBLACK * 2 + 9 * bblack) && _HOME && !_INTRODUCTION  && !_ABOUT && !_DRMS)//����
			{
				_ABOUT = true;
				cleardevice();
				loadimage(NULL, "mikasa.png");		////����
				using namespace std;

				RECT R3 = { WINDOWS_X - BBLACK,WINDOWS_Y - BBLACK / 2,WINDOWS_X - 10,WINDOWS_Y - 10 };
				rectangle(WINDOWS_X - BBLACK, WINDOWS_Y - BBLACK / 2, WINDOWS_X - 10, WINDOWS_Y - 10);
				drawtext("����", &R3, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
				settextstyle(BBLACK / 3, 0, "΢���ź�");
				string author[5] = {
					"Author: Rorschach",
					"Course: ���ݽṹ",
					"Repositories��https://github.com/Rorschachqaq/ChessGame",
					"Email: 1013265317@qq.com",
					"Reference��https://www.dreamwings.cn/reversi/3013.html"
				};
				for (int i = 0; i < 5; i++)
				{
					outtextxy(5 * BBLACK / 4, WINDOWS_Y - (7 - i) * BBLACK / 2, author[i].data());
				}

				break;
			}

			else if (m.x > WINDOWS_X - BBLACK && m.x<WINDOWS_X - 10 && m.y>WINDOWS_Y - BBLACK / 2 && m.y < WINDOWS_Y - 10 && (_INTRODUCTION  || _ABOUT || _DRMS))					//����
			{
				cleardevice();
				_HOME = false, _INTRODUCTION = false,  _ABOUT = false, _DRMS = false;
				startgame();
			}

			else if (m.x > WINDOWS_X / 2 - BBLACK && m.x<WINDOWS_X / 2 + BBLACK && m.y>(int)(WINDOWS_Y / 3 + BBLACK * 2 + 12 * bblack) && m.y < WINDOWS_Y / 3 + BBLACK * 2.5 + 12 * bblack && _HOME && !_INTRODUCTION  && !_ABOUT && !_DRMS)//�˳���Ϸ
			{
				exit(0);
			}
			else break;

		case WM_MOUSEMOVE:					//����ƶ�����,��������ƶ�����λ��ʱ��Ч��ʾ
			RECT r;
			if (_INTRODUCTION  || _ABOUT || _DRMS)				//�����ǰ������Ϸ���� ���ڽ��� ���ߵ���ģʽ����
			{
				if (ESCEXIT)
					startgame();
				if (m.x > WINDOWS_X - BBLACK && m.x<WINDOWS_X - 10 && m.y>WINDOWS_Y - BBLACK / 2 && m.y < WINDOWS_Y - 10)
				{
					r.left = WINDOWS_X - BBLACK;
					r.top = WINDOWS_Y - BBLACK / 2;
					r.right = WINDOWS_X - 10;
					r.bottom = WINDOWS_Y - 10;
					POINT points[8] = { { r.left,r.top },{ r.right,r.top },{ r.right,r.bottom },{ r.left,r.bottom } };
					setfillcolor(LIGHTBLUE);
					fillpolygon(points, 4);
					setbkmode(TRANSPARENT);
					drawtext("����", &r, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
				}
				else
				{
					if (getpixel(WINDOWS_X - BBLACK + 1, WINDOWS_Y - BBLACK / 2 + 1) == LIGHTBLUE)
					{
						putimage(WINDOWS_X - BBLACK + 1, WINDOWS_Y - BBLACK / 2 + 1, MM + 5);
						setbkmode(TRANSPARENT);
						drawtext("����", &r, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
					}
				}
				if (_DRMS)
				{
					for (int i = 0; i < 3; i++)
					{
						if (m.x > WINDOWS_X / 2 - BBLACK && m.x<WINDOWS_X / 2 + BBLACK && m.y>WINDOWS_Y / 3 + BBLACK * i + 2 * i * bblack && m.y < WINDOWS_Y / 3 + BBLACK * i + 2 * i * bblack + BBLACK / 2)
						{
							r.left = WINDOWS_X / 2 - BBLACK;
							r.top = WINDOWS_Y / 3 + BBLACK * i + 2 * i * bblack;
							r.right = WINDOWS_X / 2 + BBLACK;
							r.bottom = WINDOWS_Y / 3 + BBLACK * i + 2 * i * bblack + BBLACK / 2;
							POINT points[8] = { { r.left,r.top },{ r.right,r.top },{ r.right,r.bottom },{ r.left,r.bottom } };
							setfillcolor(LIGHTBLUE);
							fillpolygon(points, 4);
							setbkmode(TRANSPARENT);
							switch (i)
							{
							case 0:
								drawtext("��    ��", &r, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
								break;
							case 1:
								drawtext("��    ��", &r, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
								break;
							case 2:
								drawtext("��    ��", &r, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
								break;
							}
						}
						else
						{
							if (getpixel(WINDOWS_X / 2 - BBLACK + 1, WINDOWS_Y / 3 + BBLACK * i + 2 * i * bblack + 1) == LIGHTBLUE)
							{
								putimage(WINDOWS_X / 2 - BBLACK, WINDOWS_Y / 3 + BBLACK * i + 2 * i * bblack, MB + i);	//���ԭ��ͼƬ
							}
						}
					}
				}
			}
			else
			{
				for (int i = 0; i < 5; i++)
				{
					if (m.x > WINDOWS_X / 2 - BBLACK && m.x<WINDOWS_X / 2 + BBLACK && m.y> WINDOWS_Y / 3 + i * bblack * 3 + i * BBLACK / 2 && m.y < WINDOWS_Y / 3 + i * bblack * 3 + i * BBLACK / 2 + BBLACK / 2)
					{
						r.left = WINDOWS_X / 2 - BBLACK;
						r.right = WINDOWS_X / 2 + BBLACK;
						r.top = m.y > WINDOWS_Y / 3 + i * bblack * 3 + i * BBLACK / 2;
						r.bottom = m.y < WINDOWS_Y / 3 + i * bblack * 3 + i * BBLACK / 2 + BBLACK / 2;
						POINT points[8] = { { r.left,r.top },{ r.right,r.top },{ r.right,r.bottom },{ r.left,r.bottom } };
						setfillcolor(LIGHTBLUE);
						fillpolygon(points, 4);						//�������
						setbkmode(TRANSPARENT);
						switch (i)
						{
						case 0:
							drawtext("����ģʽ", &r, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
							break;
						case 1:
							drawtext("˫��ģʽ", &r, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
							break;
						case 2:
							drawtext("��Ϸ˵��", &r, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
							break;
						case 3:
							drawtext("��    ��", &r, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
							break;
						case 4:
							drawtext("�˳���Ϸ", &r, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
							break;
						}
					}
					else
					{
						if (getpixel(WINDOWS_X / 2 - BBLACK + 1, WINDOWS_Y / 3 + i * bblack * 3 + i * BBLACK / 2 + 1) == LIGHTBLUE)
						{
							putimage(WINDOWS_X / 2 - BBLACK, WINDOWS_Y / 3 + i * bblack * 3 + i * BBLACK / 2, MM + i);	//���ԭ��ͼƬ
						}
					}
				}
			}

			FlushBatchDraw();
			break;
		default:break;
		}			
	}
	startgame();
}
	



int main()
{
	startgame();
	return 0;
}

