///////////////////////////////////////////////////////////////////////////////
// �������ƣ��ڰ���
// ���뻷����Visual Studio 2017
// �����д��Abduwali
// �����£�2018-3-29
//
////////////////////////////////////////////////////////////////////////////////
//˵�������ݡ�ǧǧ���ġ�ī����������һЩ�޸�,ǿ����AI�������˻��屣�桢�������ֵȹ��ܣ���л�����ɰ���ǧǧ��Դ����
//�����ɰ���ǧǧ��Դ������ַ��https://github.com/im0qianqian/
////////////////////////////////////////////////////////////////////////////////

#include<easyx.h>
#include<graphics.h>
#include<strstream>
#include<iostream>
#include<fstream>
#include<time.h>
#include<ctime>
#include<stdlib.h>
#include<stdio.h>
#include<conio.h>
#include<algorithm>
#include<math.h>
#include <map>
#include <string>

using namespace std;

#define WINDOWS_X 1200	//���ڴ�СX
#define WINDOWS_Y 650	//���ڴ�СY

#define PX 280			//����ƫ����X
#define PY 80			//����ƫ����Y
#define BBLACK 70		//�ո��С

#define CHESSSIZE 20	//���Ӵ�С
#define SIZE 8			//���̸���

#define ESC 27
#define ESCEXIT (_kbhit()&&_getch()==ESC)

#pragma comment (lib,"ws2_32.lib")						// ���� Windows Multimedia API
#pragma comment(lib,"Winmm.lib")

const int black = 1;	//����
const int white = 0;	//����
int step = 0;  //�غ���
int diff = 1; //�Ѷ�
int whitecount = 2;  //�ڰ���÷�
int blackcount = 2;
bool single = true; //����ģʽ
const int timeout_turn = 3000;		//ÿ�����˼��ʱ��
clock_t start;					//AI��ʼ˼����ʱ���
bool stopThink = false;			//AI�Ƿ�ֹͣ˼��
const int MAX_DEPTH = 20, MIN_DEPTH = 4;

int mapp[SIZE][SIZE] = {};		//���̾���洢
int historymap[100][SIZE][SIZE] = {};
const int MOVE[8][2] = { { -1, 0 },{ 1, 0 },{ 0, -1 },{ 0, 1 },{ -1, -1 },{ 1, -1 },{ 1, 1 },{ -1, 1 } };	//��λ
const int MAPPOINTCOUNT[8][8] =												//���̸���Ȩֵ��ֵ
{
	{ 90,-60,10,10,10,10,-60,90 },
	{ -60,-80,5,5,5,5,-80,-60 },
	{ 10,5,1,1,1,1,5,10 },
	{ 10,5,1,1,1,1,5,10 },
	{ 10,5,1,1,1,1,5,10 },
	{ 10,5,1,1,1,1,5,10 },
	{ -60,-80,5,5,5,5,-80,-60 },
	{ 90,-60,10,10,10,10,-60,90 }
};
IMAGE MAPIMAGE[SIZE][SIZE];
IMAGE COUNT[3];

int expect[SIZE][SIZE] = {};

SOCKET sockSer;
SOCKET sockConn;

bool TOINTERNET = false;						//�Ƿ�����������
int MYCOLOR = 1;							//�ҵ���ɫ 1��ɫ 0��ɫ -1����
int NOWCOLOR = 1;							//��ǰִ����ɫ
bool TIANEYES = false;							//����ģʽ

class POINT2
{
public:
	void WIN2MAP(POINT2 &MAP)			//��������������ӳ���ϵ
	{
		MAP.x = (x - PX) / BBLACK;
		MAP.y = (y - PY) / BBLACK;
	}
	void MAP2WIN(POINT2 &WIN)			//�������������̵�ӳ���ϵ
	{
		WIN.x = PX + x*BBLACK + BBLACK / 2;
		WIN.y = PY + y*BBLACK + BBLACK / 2;
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
		return PX + x*BBLACK + BBLACK / 2;
	}
	int MAP2WINY()
	{
		return PY + y*BBLACK + BBLACK / 2;
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

POINT2 LASTCH;								//��һ�����ӷ�λ

void gameStart();						//��������
void STARTVS(int YOURCOLOR, POINT2 P1(), POINT2 P2());

void mappadd(int x, int y, int color, int MAP[SIZE][SIZE])		//���ͼ���������
{
	POINT2 WINDOWS2, MAP2;
	WINDOWS2.INIT(x, y);
	WINDOWS2.WIN2MAP(MAP2);
	MAP[MAP2.x][MAP2.y] = color ? 1 : -1;
}

void printcircle(int x, int y, int color, int MAP[SIZE][SIZE])				//��������
{
	mappadd(x, y, color, MAP);
	//ע����Ϊ��Ч��
	/*POINT2 X;
	X.INIT(x, y);
	X.WIN2MAP(X);
	POINT2 ZS, YS, ZX, YX, ZZS, ZZX, ZYS, ZYX;
	ZS.INIT(x - BBLACK / 2+1, y - BBLACK / 2+1);
	ZX.INIT(x - BBLACK / 2+1, y + BBLACK / 2 - 1);
	YS.INIT(x + BBLACK / 2-1, y - BBLACK / 2+1);
	YX.INIT(x + BBLACK / 2 - 1, y + BBLACK / 2 - 1);
	ZZS.INIT(x, y - BBLACK / 2+1);
	ZZX.INIT(x, y + BBLACK / 2 - 1);
	ZYS = ZZS, ZYX = ZZX;

	for (int i = 0; i < BBLACK; i++)
	{
	POINT points1[4] = { { ZS.x,ZS.y },{ ZZS.x,ZZS.y },{ ZZX.x,ZZX.y },{ ZX.x,ZX.y }};			//�����
	POINT points2[4] = { { YS.x,YS.y },{ ZYS.x,ZYS.y },{ ZYX.x,ZYX.y },{ YX.x,YX.y }};			//��һ��
	setfillcolor(LIGHTRED);																		//��ת������ɫ
	putimage(ZS.x - 1, ZS.y - 1, &MAPIMAGE[X.x][X.y]);											//ʹ��ԭͼ����
	solidpolygon(points1, 4);
	solidpolygon(points2, 4);
	if (ZZS.x <= x - BBLACK / 2 + 1)break;
	ZZS.x-=3;
	++ZZS.y;
	++ZYS.y;
	ZYS.x += 3;
	--ZYX.y;
	ZYX.x += 3;
	ZZX.x-=3;
	--ZZX.y;

	Sleep(2);
	}*/
	setfillcolor(color ? BLACK : WHITE);					//���м�Ŵ�任
	for (int i = 0; i <= CHESSSIZE; ++i)
	{
		solidcircle(x, y, i);
		Sleep(1);
	}
}

void init()															//��������
{
	memset(mapp, 0, sizeof(mapp));									//��ʼ��
	memset(historymap, 0, sizeof(historymap));
	memset(expect, 0, sizeof(expect));
	TIANEYES = false;												//�ر�����ģʽ
	LASTCH.INIT(0, 0);
	step = 1;
	whitecount = 2;
	blackcount = 2;
	NOWCOLOR = 1;

	settextstyle(15, 0, "����");
	loadimage(NULL, "Picture/1.jpg");										//����ͼƬ
	for (int x = PX; x < PX + BBLACK*(SIZE + 1); x += BBLACK)		//�������� ����
	{
		if ((x / BBLACK % 2) == 0)setlinecolor(BLACK);
		else setlinecolor(RED);
		line(x, PY, x, PY + BBLACK*SIZE);
		line(x-1, PY, x-1, PY + BBLACK*SIZE);

	}
	for (int y = PY; y <PY + BBLACK*(SIZE + 1); y += BBLACK)		//�������� ����
	{
		if (y / BBLACK % 2 == 0)setlinecolor(BLACK);
		else setlinecolor(RED);
		line(PX, y, PX + BBLACK*SIZE, y);
		line(PX, y-1, PX + BBLACK*SIZE, y-1);
	}
	for (int i = PX; i <= PX + BBLACK*SIZE; i += BBLACK)			//��ȡÿ����ͼƬ������ͼƬָ����
	{
		for (int j = PY; j <= PY + BBLACK*SIZE; j += BBLACK)
		{
			POINT2 DATA;
			DATA.INIT(i, j);
			DATA.WIN2MAP(DATA);
			getimage(&MAPIMAGE[DATA.x][DATA.y], i, j, BBLACK, BBLACK);
		}
	}

	printcircle(PX + (SIZE / 2 - 1)* BBLACK + BBLACK / 2, PY + (SIZE / 2 - 1) * BBLACK + BBLACK / 2, white, mapp);		//��ʼ����ö����
	printcircle(PX + (SIZE / 2 - 1) * BBLACK + BBLACK / 2, PY + (SIZE / 2) * BBLACK + BBLACK / 2, black, mapp);
	printcircle(PX + (SIZE / 2) * BBLACK + BBLACK / 2, PY + (SIZE / 2) * BBLACK + BBLACK / 2, white, mapp);
	printcircle(PX + (SIZE / 2) * BBLACK + BBLACK / 2, PY + (SIZE / 2 - 1) * BBLACK + BBLACK / 2, black, mapp);


	getimage(COUNT, WINDOWS_X / 30, WINDOWS_Y - WINDOWS_Y / 6, 230, 40);											//�ɼ��������򱳾�ͼƬָ��
	getimage(COUNT + 1, WINDOWS_X - WINDOWS_X / 5, WINDOWS_Y - WINDOWS_Y / 6, 230, 40);
	getimage(COUNT + 2, BBLACK / 4 + BBLACK * 2, BBLACK / 4 + 2 * BBLACK, 100, 40);   //�غ���

	LOGFONT f;
	gettextstyle(&f);												// ��ȡ������ʽ
	f.lfHeight = 35;												// ��������߶�
	strcpy_s(f.lfFaceName, _T("����Ҧ��"));							// ��������
	f.lfQuality = ANTIALIASED_QUALITY;								// �������Ч��Ϊ�����
	settextstyle(&f);												// ����������ʽ

	settextcolor(BLACK);
	outtextxy(BBLACK / 4, BBLACK / 2, "����ִ��");
	outtextxy(BBLACK / 4, BBLACK / 4 + BBLACK, "��ǰִ��");
	outtextxy(BBLACK / 4, BBLACK / 4 + 2 * BBLACK, "��ǰ�غ�");

	settextstyle(25, 0, "����");
	rectangle((WINDOWS_X - WINDOWS_X / 4 - 10), (0.5*BBLACK - 10), (WINDOWS_X - WINDOWS_X / 4 + 1.5*BBLACK), BBLACK);
	outtextxy(WINDOWS_X - WINDOWS_X / 4, 0.5*BBLACK, "������Ϸ");          //���棬��ȡ������
	rectangle((WINDOWS_X - WINDOWS_X / 4 + 2 * BBLACK), (0.5 * BBLACK - 10), (WINDOWS_X - WINDOWS_X / 4 + 3.5 * BBLACK + 10), BBLACK);
	outtextxy(WINDOWS_X - WINDOWS_X / 4 + 2 * BBLACK + 10, 0.5*BBLACK, "��ȡ��Ϸ");          //���棬��ȡ������

	settextstyle(30, 0, "����Ҧ��");
	rectangle(WINDOWS_X - WINDOWS_X / 4 - 10,  1.5*BBLACK, WINDOWS_X - WINDOWS_X / 4 + BBLACK, 2 * BBLACK);
	outtextxy(WINDOWS_X - WINDOWS_X / 4,  1.5 * BBLACK, "����");          //����

}

void printmapp()         //���������ϵ�����
{
	int i, j;
	POINT2 WINDOWS2, MAP2;
	for (i = 0; i < SIZE; ++i)                                     //���������ϵ�����
		for (j = 0; j < SIZE; ++j) {
			if (mapp[i][j] == 1 || mapp[i][j] == -1) {
				MAP2.INIT(i, j);
				MAP2.MAP2WIN(WINDOWS2);
				setfillcolor(mapp[i][j] == 1 ? BLACK : WHITE);					//���м�Ŵ�任
				for (int i = 0; i <= CHESSSIZE; ++i)
				{
					solidcircle(WINDOWS2.x, WINDOWS2.y, i);
				}
			}
		}
}


int Judge(int x, int y, int color, int MAP[SIZE][SIZE])									//Ԥ�е�ǰλ���ܷ�����
{
	if (MAP[x][y])return 0;																//�����ǰλ���Ѿ�������
	int me = color ? 1 : -1;															//׼������������ɫ
	POINT2 star;
	int count = 0, flag;																//countΪ��λ�ÿ���ת���������Ӹ���
	for (int i = 0; i < SIZE; ++i)														//����
	{
		flag = 0;
		star.INIT(x + MOVE[i][0], y + MOVE[i][1]);
		while (star.x >= 0 && star.x < SIZE&&star.y >= 0 && star.y < SIZE&&MAP[star.x][star.y])
		{
			if (MAP[star.x][star.y] == -me)flag++;
			else
			{
				count += flag;
				break;
			}
			star.ADD(MOVE[i][0], MOVE[i][1]);
		}
	}
	return count;																		//���ظõ�ת���Է����Ӹ���
}

void Change(POINT2 NOW, int MAP[SIZE][SIZE], bool F)												//���Ӻ�ı�����״̬ FΪ�Ƿ��������Ļ
{
	int me = MAP[NOW.x][NOW.y];																	//��ǰ����������ɫ
	bool flag;
	POINT2 a, b;
	for (int i = 0; i<SIZE; ++i)																//����
	{
		flag = false;
		a.INIT(NOW.x + MOVE[i][0], NOW.y + MOVE[i][1]);
		while (a.x >= 0 && a.x<SIZE&&a.y >= 0 && a.y<SIZE&&MAP[a.x][a.y])
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
						if (F)printcircle(b.MAP2WINX(), b.MAP2WINY(), (me == 1) ? black : white, MAP);	//�ı�����
						if (!F)mappadd(b.MAP2WINX(), b.MAP2WINY(), (me == 1) ? black : white, MAP);		//������������Ļ���ı��ͼ����
						b.ADD(MOVE[i][0], MOVE[i][1]);
					}
				}
				break;
			}
			a.ADD(MOVE[i][0], MOVE[i][1]);
		}
	}
}

int Statistics(int color)														//Ԥ��ÿ��λ�ÿ���ת���������Ӹ���
{
	int NOWEXPECT = 0;															//�ܵ�ת�����Ӹ���
	for (int i = 0; i < SIZE; ++i)												//����
		for (int j = 0; j < SIZE; ++j)
		{
			expect[i][j] = Judge(i, j, color, mapp);									//�洢��λ�ÿ���ת�����Ӹ���
			if (expect[i][j])
			{
				++NOWEXPECT;

				POINT2 a;
				a.INIT(i, j);
				if (!single || color == MYCOLOR) {
					setfillcolor(RGB(96, 96, 96));
					circle(a.MAP2WINX(), a.MAP2WINY(), CHESSSIZE / 4);
					circle(a.MAP2WINX(), a.MAP2WINY(), CHESSSIZE / 4 - 1);
				}

				if (TIANEYES)														//�����������ģʽ
				{
					settextstyle(15, 0, "����");
					TCHAR s[20];													//����ģʽ
					_stprintf_s(s, _T("%d"), expect[i][j]);
					outtextxy(a.MAP2WINX(), a.MAP2WINY() + 10, s);
				}
			}
		}
	return NOWEXPECT;
}

void CleanStatistics()										//�����������ʾ
{
	for (int i = 0; i < SIZE; ++i)
	{
		for (int j = 0; j < SIZE; ++j)
		{
			if (expect[i][j] && !mapp[i][j])				//�����ǰ��û�����ӻ���������
			{
				POINT2 a;
				a.INIT(i, j);								//��¼����
				putimage(a.MAP2WINX() - BBLACK / 2, a.MAP2WINY() - BBLACK / 2, &MAPIMAGE[i][j]);	//����ֲ�����
			}
		}
	}
}

string INTTOCHI(int num, int color)											//��ǰ���Ƴɼ��������
{
	string number[10] = { "","һ","��","��","��","��","��","��","��","��" };
	string data = "";
	if (num >= 10)
	{
		data += number[num / 10 % 10];
		data += "ʮ";
	}
	data += number[num % 10];
	//return (color ? "���壺" : "���壺") + data;							//num>=0&&num<=64
	return (color ? "���壺" : "���壺") + std::to_string(num);
}

void Printcount(int balckcount, int whitecount, int nowcolor)		//�����ǰ����
{
	settextcolor(BLACK);											//����������ɫ
	settextstyle(35, 0, "����Ҧ��");

	putimage(WINDOWS_X / 30, WINDOWS_Y - WINDOWS_Y / 6, COUNT);		//����ԭ���ۼ�
	putimage(WINDOWS_X - WINDOWS_X / 5, WINDOWS_Y - WINDOWS_Y / 6, COUNT + 1);
	putimage(BBLACK / 4 + BBLACK * 2, BBLACK / 4 + 2 * BBLACK, COUNT + 2);

	outtextxy(WINDOWS_X / 30, WINDOWS_Y - WINDOWS_Y / 6, INTTOCHI(whitecount, white).data());	//�����ǰ�ɼ�
	outtextxy(WINDOWS_X - WINDOWS_X / 5, WINDOWS_Y - WINDOWS_Y / 6, INTTOCHI(balckcount, black).data());

	char s[5] = {};
	_itoa_s(step, s, 3, 10);
	settextstyle(55, 0, "Kristen ITC");
	outtextxy(BBLACK / 4 + BBLACK * 2, BBLACK / 4 + 2 * BBLACK - 10, s);

	setfillcolor(MYCOLOR == 1 ? BLACK : MYCOLOR == 0 ? WHITE : LIGHTCYAN);						//���м�Ŵ�任
	solidcircle(BBLACK * 2 + 20, BBLACK * 3 / 4, CHESSSIZE * 3 / 4);
	setfillcolor((!nowcolor || balckcount + whitecount == 4) ? BLACK : WHITE);
	NOWCOLOR = (!nowcolor || balckcount + whitecount == 4) ? black : white;						//��¼��ǰִ��
	for (int i = 0; i <= CHESSSIZE * 3 / 4; ++i)
	{
		solidcircle(BBLACK * 2 + 20, BBLACK * 3 / 2, i);
		Sleep(1);
	}
}

void WIN(int YOURCOLOR, int balckcount, int whitecount)			//�ж���Ӯ
{
	HWND wnd = GetHWnd();										//��ȡ���ھ��
	if (balckcount>whitecount)
	{
		if (YOURCOLOR == black) mciSendString("play Music/ʤ��Ц��.wav", NULL, 0, NULL);
		else  mciSendString("play Music/ʧ��.wav", NULL, 0, NULL);
		MessageBox(wnd, YOURCOLOR == black ? "��ϲ�㣬ʤ����~" : YOURCOLOR == white ? "���˰�~����������ģ��´�һ������Ӯ�ģ�" : "�ڷ���ʤ~", "Result", MB_OK);
	}
	else if (balckcount<whitecount)
	{
		if (YOURCOLOR == white) mciSendString("play Music/ʤ��Ц��.wav", NULL, 0, NULL);
		else  mciSendString("play Music/ʧ��.wav", NULL, 0, NULL);
		MessageBox(wnd, YOURCOLOR == white ? "��ϲ�㣬ʤ����~" : YOURCOLOR == black ? "���˰�~����������ģ��´�һ������Ӯ�ģ�" : "�׷���ʤ", "Result", MB_OK);
	}
	else
	{
		mciSendString("play Music/ƽ��.wav", NULL, 0, NULL);
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

POINT2 Easy1()										//�˻���ս��AI
{
	POINT2 MAX;										//�����Լ���ʼ�����Ž�
	MAX.INIT(0, 0);
	int maxx = 0;
	for (int i = 0; i < SIZE; ++i)
		for (int j = 0; j < SIZE; ++j)
		{
			if (expect[i][j] >= maxx)				//Ѱ�ҿ���ת���������ĵ���Ϊ���Ž�
			{
				maxx = expect[i][j];
				MAX.INIT(i, j);
			}
		}
	if (ESCEXIT)gameStart();
	Sleep(200);										//��Ъ
	return MAX;										//�������Ž�
}

void copymap(int one[SIZE][SIZE], int last[SIZE][SIZE])						//������ͼ
{
	for (int i = 0; i < SIZE; i++)
		for (int j = 0; j < SIZE; j++)
			one[i][j] = last[i][j];
}

POINT2 Easy()										//�˻���ս�е�AI
{
	int ME = 0;										//AIȨֵ
	int maxx = 0;

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
			return w>a.w;
		}
		POINT2 INTOPOINT2()							//ת��ΪPOINT2����
		{
			POINT2 data;
			data.INIT(x, y);
			return data;
		}
	}WEA[SIZE*SIZE];

	int expectnow[SIZE][SIZE], mapnow[SIZE][SIZE];
	if (ESCEXIT)gameStart();												//��ESC�˳�
	Sleep(100);																//��Ъ0.1S

	for (int i = 0; i < SIZE; ++i)
		for (int j = 0; j < SIZE; ++j)
		{
			if (expect[i][j])												//�����ǰ���������
			{
				ME = MAPPOINTCOUNT[i][j] + expect[i][j];						//���㱾���ڸõ�Ȩֵ
				copymap(mapnow, mapp);
				mapnow[i][j] = NOWCOLOR ? 1 : -1;							//ģ������
				POINT2 NOWPOINT;
				NOWPOINT.INIT(i, j);
				if ((i == 0 && j == 0) || (i == 0 && j == SIZE - 1) || (i == SIZE - 1 && j == SIZE - 1) || (i == SIZE - 1 && j == 0))
				{
					return NOWPOINT;										//����ڽǣ����ؽ�����
				}

				Change(NOWPOINT, mapnow, false);								//ģ�����������ı��ͼ
				int YOU = -1050;											//֪̽�����ж��������
				for (int k = 0; k < SIZE; ++k)
					for (int l = 0; l < SIZE; ++l)
					{
						expectnow[k][l] = Judge(k, l, !NOWCOLOR, mapnow);	//�ж϶�������
						if (expectnow[k][l])
						{
							YOU = YOU < MAPPOINTCOUNT[k][l] + expectnow[k][l] ? MAPPOINTCOUNT[k][l] + expectnow[k][l] : YOU;
						}
					}
				WEA[maxx++].init(i, j, ME - YOU);							//��ṹ������
			}
		}
	sort(WEA, WEA + maxx);													//����Ȩֵ����
	for (int i = 0; i < maxx; ++i)
	{
		if ((WEA[i].x < 2 && WEA[i].y < 2) || (WEA[i].x < 2 && SIZE - WEA[i].y - 1 < 2) || (SIZE - 1 - WEA[i].x < 2 && WEA[i].y < 2) || (SIZE - 1 - WEA[i].x < 2 && SIZE - 1 - WEA[i].y < 2))continue;
		return WEA[i].INTOPOINT2();											//���طǽǱ����Ž�
	}
	return WEA[0].INTOPOINT2();												//���ؽǱ����Ž�
}


int difai(int x, int y, int mapnow[SIZE][SIZE], int expectnow[SIZE][SIZE], int depin, int depinmax)						//����С����
{
	if (depin >= depinmax)return 0;											//�ݹ����

	int maxx = -10005;														//���Ȩֵ
	POINT2 NOW;
	int expectnow2[SIZE][SIZE], mapnow2[SIZE][SIZE], mapnext[SIZE][SIZE], expectlast[SIZE][SIZE];					//������ʱ����

	copymap(mapnow2, mapnow);												//���Ƶ�ǰ����

	mapnow2[x][y] = NOWCOLOR ? 1 : -1;										//ģ���ڵ�ǰ����������
	int ME = MAPPOINTCOUNT[x][y] + expectnow[x][y];							//��ǰ����Ȩ
	NOW.INIT(x, y);

	Change(NOW, mapnow2, false);											//�ı�����AI����

	int MAXEXPECT = 0, LINEEXPECT = 0, COUNT = 0;
	for (int i = 0; i < SIZE; ++i)
		for (int j = 0; j < SIZE; ++j)
		{
			expectnow2[i][j] = Judge(i, j, !NOWCOLOR, mapnow2);				//Ԥ�жԷ��Ƿ��������
			if (expectnow2[i][j])
			{
				++MAXEXPECT;
				if ((i == 0 && j == 0) || (i == 0 && j == SIZE - 1) || (i == SIZE - 1 && j == SIZE - 1) || (i == SIZE - 1 && j == 0))return -1800;	//����Է���ռ�ǵĿ���
				if ((i < 2 && j < 2) || (i < 2 && SIZE - j - 1 < 2) || (SIZE - 1 - i < 2 && j < 2) || (SIZE - 1 - i < 2 && SIZE - 1 - j < 2))++LINEEXPECT;
			}
		}
	if (LINEEXPECT * 10 > MAXEXPECT * 7)return 1400;						//����Է��ߵ�����״̬�϶� ��֦

	for (int i = 0; i < SIZE; i++)
		for (int j = 0; j < SIZE; j++)
			if (expectnow2[i][j])											//����Է���������
			{
				int YOU = MAPPOINTCOUNT[i][j] + expectnow2[i][j];			//��ǰȨֵ
				copymap(mapnext, mapnow2);									//������ͼ
				mapnext[i][j] = (!NOWCOLOR) ? 1 : -1;						//ģ��Է�����
				NOW.INIT(i, j);
				Change(NOW, mapnext, false);								//�ı�����

				for (int k = 0; k < SIZE; k++)
					for (int l = 0; l < SIZE; l++)
						expectlast[k][l] = Judge(k, l, NOWCOLOR, mapnext);	//Ѱ��AI���е�

				for (int k = 0; k < SIZE; k++)
					for (int l = 0; l < SIZE; l++)
						if (expectlast[k][l])
						{
							int nowm = ME - YOU + difai(k, l, mapnext, expectlast, depin + 1, depinmax);
							maxx = maxx < nowm ? nowm : maxx;
						}
			}
	return maxx;
}

POINT2 Middle()									//�˻���ս����AI
{
	POINT2 MAX;
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
				int k = difai(i, j, mapp, expect, 0, 3);					//�ݹ����� ��������
				if (k >= maxx)
				{
					maxx = k;
					MAX.INIT(i, j);
				}
			}
		}
	return MAX;
}

////////////////////////////////////////////////////////////////////////////////////////////
// ���ص�ǰ���õ�����ʱ��
inline double GetTime()
{
	return (double)(clock() - start) / CLOCKS_PER_SEC * 1000;
}

struct SVpair {
	int move;
	double value;
	SVpair() { }
	SVpair(int x, double y) : move(x), value(y) { }
};
// ������ֹʱ��
static int start_clock;
const int BOARD_SIZE = 64;
const double  INF = 1e8;
const double BND = 1e8 + 100;
const int weight[SIZE][SIZE] = {
	20, -3, 11,  8,  8, 11, -3, 20,
	-3, -7, -4,  1,  1, -4, -7, -3,
	11, -4,  2,  2,  2,  2, -4, 11,
	8,  1,  2, -3, -3,  2,  1,  8,
	8,  1,  2, -3, -3,  2,  1,  8,
	11, -4,  2,  2,  2,  2, -4, 11,
	-3, -7, -4,  1,  1, -4, -7, -3,
	20, -3, 11,  8,  8, 11, -3, 20,
};
// ɢ�б�Ԫ��Ϊ64k
const int HASHTABLE_BIT_NUM = 16;
const int HASHTABLE_SIZE = 1u << HASHTABLE_BIT_NUM;
// ȡַ����
const int HASHTABLE_MASK = HASHTABLE_SIZE - 1;
// ɢ�б�
struct Hashtable_Node {
	// ��ֵ
	unsigned int lock;
	// ��ֵ�ϡ�����
	double lower, upper;
	// ����岽
	int best_move;
	// �������
	int depth;
};
struct Hashtable {
	// �������滻���͡�ʼ���滻���ڵ�
	struct Hashtable_Node deepest, newest;
};
Hashtable hashtable[HASHTABLE_SIZE];
// ��λ���ϳ��ֺ���ʱ��Ӧ��ɢ����
unsigned int zobrist_black[BOARD_SIZE][2] = {
	{ 1288827737,282448869 },{ 450174869,1246877580 },{ 2187254741,2285582077 },{ 3209756399,3373088701 },{ 4087561172,1416650225 },{ 63068892,2650566517 },{ 3446267773,2624715758 },{ 284220267,1298341087 },
	{ 3844988796,2642345446 },{ 231107512,1833054584 },{ 202260255,3928555381 },{ 1113080702,4100775676 },{ 30928383,3376494459 },{ 3672532980,2843567806 },{ 133562111,3034607591 },{ 1255276367,2982969333 },
	{ 344167933,4184993660 },{ 1336409332,1082687484 },{ 2772661342,1956239283 },{ 2484941485,1704900596 },{ 848977913,1951366502 },{ 4077908643,4240998118 },{ 1117582013,3050272501 },{ 1518567399,3162733525 },
	{ 469883390,2224652133 },{ 1319597891,1695839998 },{ 422254458,4185718199 },{ 214671292,261291881 },{ 2602547038,1502478333 },{ 1316000751,2630929002 },{ 2626813421,2502245053 },{ 2889396207,1148108789 },
	{ 3839385021,3874095087 },{ 2700238583,1138155004 },{ 3673571291,2462380970 },{ 747907070,2906124228 },{ 4170486510,3303690175 },{ 389504929,1451716780 },{ 3707960113,2430167988 },{ 3558993918,3901215743 },
};
// ��λ���ϳ��ְ���ʱ��Ӧ��ɢ����
unsigned int zobrist_white[BOARD_SIZE][2] = {
	{ 3051562733,64368366 },{ 38323581,2453241279 },{ 3364346607,1296948607 },{ 3703204827,3623485437 },{ 1614315502,241464730 },{ 2082766646,2012019581 },{ 2458434406,966852588 },{ 2112649625,2168093583 },
	{ 4151533533,3767791280 },{ 3143954409,2442755583 },{ 2220587006,449705967 },{ 1958936565,1279097596 },{ 118095230,216996063 },{ 472218747,4041784188 },{ 1085426173,262143466 },{ 3230752719,317382617 },
	{ 2700104327,4136203903 },{ 418860053,1880088319 },{ 2697543134,2003006270 },{ 791510990,3887234175 },{ 3181477756,2968361982 },{ 2868116474,2444664535 },{ 340817723,3521855348 },{ 3169642996,4119199725 },
	{ 2618419652,3760812031 },{ 2846637999,1628372956 },{ 25390829,2676080633 },{ 4104738558,1607360359 },{ 565444529,2563603838 },{ 725565357,1371864251 },{ 622031782,1089629694 },{ 1337229293,578614760 },
	{ 981450622,1071873982 },{ 3829037038,537386749 },{ 4063751393,3616764919 },{ 2514933677,4059889658 },{ 954909852,1794932348 },{ 2276941646,2961405868 },{ 4071112506,623640571 },{ 1033206781,457178798 },
	{ 904718843,3223862253 },{ 659389915,4033243119 },{ 647581166,1812719601 },{ 3415502079,1672556404 },{ 1895399153,3589357500 },{ 2467230300,3042983405 },{ 1690369017,1325453244 },{ 859758488,3913219670 },
	{ 2953326191,325463935 },{ 3113516541,2644463597 },{ 3126157279,2975514558 },{ 912572121,39379253 },{ 2677145304,2417474465 },{ 133361104,4023311831 },{ 3406622653,2531617660 },{ 3311468375,501969916 },
	{ 847769493,4094817270 },{ 3384827876,2065592252 },{ 824147932,66018199 },{ 1594466267,2741141180 },{ 39776890,1474229584 },{ 2299865081,2000467901 },{ 3711809529,3712442296 },{ 1890467676,3534938109 },
};
// ��λ���Ϸ���ʱ��Ӧ��ɢ����
unsigned int zobrist_flip[BOARD_SIZE][2];
// ��ӳ���巽�ֻ��ĸ���ɢ����
unsigned int zobrist_swap_player[2] = { 5454014,3473489900 };
int myplayer=1;

bool isGameEnd(int board[8][8]) {
	bool flag = false;
	for (int i = 0; i < SIZE; ++i) {									//����
		for (int j = 0; j < SIZE; ++j) {
			if (Judge(i, j, black, board)) {
				flag = true; break;
			}
			if (Judge(i, j, white, board)) {
				flag = true; break;
			}
		}
		if(flag) break;
	}
	return !flag;
}

double getGameEndEval(int board[9][8],int color) {
	int myscore = 0, oppscore = 0;
	for (int i = 0; i<8; i++)
		for (int j = 0; j < 8; j++) {
			if (board[i][j] == myplayer) myscore++;
			else if (board[i][j] == myplayer) oppscore++;
		}
	double d = myscore - oppscore,result;
	if (d > 0) result = INF + d;
	else if (d < 0) result = -INF + d;
	else result = 0;
	return result;
}

int evaluation(int board[8][8], int color) {
	int my_tiles = 0, opp_tiles = 0, my_front_tiles = 0, opp_front_tiles = 0;
	int my_mobil = 0, opp_mobil = 0;
	int myplayer = color == 1 ? 1 : -1;
	double vtiles = 0, vfront = 0, vmobil = 0, vcorner = 0, vclose = 0;
	int map_weight = 0;

	int i, j;
	for (i = 0; i<8; i++)
		for (j = 0; j < 8; j++) {
			if (Judge(i, j, color, board)) { my_mobil++; continue; }
			else if (Judge(i, j, 1 - color, board)) { opp_mobil++; continue; }
			else if (board[i][j] == myplayer) {
				map_weight += weight[i][j]; my_tiles++;
				if (i > 0 && i < 7 && j>0 && j < 7) {
					if (board[i - 1][j] == 0) { my_front_tiles++; continue; }
					else if (board[i + 1][j] == 0) { my_front_tiles++; continue; }
					else if (board[i][j - 1] == 0) { my_front_tiles++; continue; }
					else if (board[i][j + 1] == 0) { my_front_tiles++; continue; }
					else if (board[i - 1][j - 1] == 0) { my_front_tiles++; continue; }
					else if (board[i - 1][j + 1] == 0) { my_front_tiles++; continue; }
					else if (board[i + 1][j - 1] == 0) { my_front_tiles++; continue; }
					else if (board[i + 1][j + 1] == 0) { my_front_tiles++; continue; }
				}
				else if (i == 0 || i == 7) {
					if (j > 0) {
						if (board[i][j - 1] == 0) { my_front_tiles++; continue; }
					}
					else if (j < 7) {
						if (board[i][j + 1] == 0) { my_front_tiles++; continue; }
					}
				}
				else if (j == 0 || j == 7) {
					if (i > 0) {
						if (board[i - 1][j] == 0) { my_front_tiles++; continue; }
					}
					else if (i < 7) {
						if (board[i + 1][j] == 0) { my_front_tiles++; continue; }
					}
				}

			}
			else if (board[i][j] == -myplayer) {
				map_weight -= weight[i][j]; opp_tiles++;
				if (i > 0 && i < 7 && j>0 && j < 7) {
					if (board[i - 1][j] == 0) { opp_front_tiles++; continue; }
					else if (board[i + 1][j] == 0) { opp_front_tiles++; continue; }
					else if (board[i][j - 1] == 0) { opp_front_tiles++; continue; }
					else if (board[i][j + 1] == 0) { opp_front_tiles++; continue; }
					else if (board[i - 1][j - 1] == 0) { opp_front_tiles++; continue; }
					else if (board[i - 1][j + 1] == 0) { opp_front_tiles++; continue; }
					else if (board[i + 1][j - 1] == 0) { opp_front_tiles++; continue; }
					else if (board[i + 1][j + 1] == 0) { opp_front_tiles++; continue; }
				}
				else if (i == 0 || i == 7) {
					if (j > 0) {
						if (board[i][j - 1] == 0) { opp_front_tiles++; continue; }
					}
					else if (j < 7) {
						if (board[i][j + 1] == 0) { opp_front_tiles++; continue; }
					}
				}
				else if (j == 0 || j == 7) {
					if (i > 0) {
						if (board[i - 1][j] == 0) { opp_front_tiles++; continue; }
					}
					else if (i < 7) {
						if (board[i + 1][j] == 0) { opp_front_tiles++; continue; }
					}
				}
			}
		}


	// ���������ͱ�Ե������
	if (my_tiles > opp_tiles)
		vtiles = (100.0 * my_tiles) / (my_tiles + opp_tiles);
	else if (my_tiles < opp_tiles)
		vtiles = -(100.0 * opp_tiles) / (my_tiles + opp_tiles);
	if (my_front_tiles > opp_front_tiles)
		vfront = -(100.0 * my_front_tiles) / (my_front_tiles + opp_front_tiles);
	else if (my_front_tiles < opp_front_tiles)
		vfront = (100.0 * opp_front_tiles) / (my_front_tiles + opp_front_tiles);

	//�ж���
	if (my_mobil > opp_mobil)
		vmobil = (100.0 * my_mobil) / (my_mobil + opp_mobil);
	else if (my_mobil < opp_mobil)
		vmobil = -(100.0 * opp_mobil) / (my_mobil + opp_mobil);

	// �����Ȩ��
	vcorner = board[0][0] * myplayer + board[0][7] * myplayer + board[7][0] * myplayer + board[7][7] * myplayer;
	vcorner = 25 * vcorner;

	// �����Ա�λ�õ�Ȩ��,����λ��cλ
	if (board[0][0] == 0) {
		vclose += board[0][1] * myplayer;
		vclose += board[1][0] * myplayer;
		vclose += board[1][1] * myplayer;
	}
	if (board[0][7] == 0) {
		vclose += board[0][6] * myplayer;
		vclose += board[1][7] * myplayer;
		vclose += board[1][6] * myplayer;
	}
	if (board[7][0] == 0) {
		vclose += board[6][0] * myplayer;
		vclose += board[7][1] * myplayer;
		vclose += board[6][1] * myplayer;
	}
	if (board[7][7] == 0) {
		vclose += board[7][6] * myplayer;
		vclose += board[6][7] * myplayer;
		vclose += board[6][6] * myplayer;
	}
	vclose = -12.5 * vclose;


	// ��Ȩ�õ��ܵĵ÷�  �ڷ�
	double score = ((10 * vtiles) + (801.724 * vcorner) + (382.026 * vclose) + (78.922 * vmobil) + (74.396 * vfront) + (10 * map_weight)) * 100;

	return int(score);
}

unsigned int random() {
	return (rand() << 17) | (rand() << 2) | (rand());
}
// ��zobrist�����ʼ��
void zobrist_init2() {
	zobrist_swap_player[0] = random();
	zobrist_swap_player[1] = random();
	for (int pos = 0; pos < 64; pos++) {
		zobrist_black[pos][0] = random() ^ zobrist_swap_player[0];
		zobrist_black[pos][1] = random() ^ zobrist_swap_player[1];

		zobrist_white[pos][0] = random() ^ zobrist_swap_player[0];
		zobrist_white[pos][1] = random() ^ zobrist_swap_player[1];

		zobrist_flip[pos][0] = zobrist_black[pos][0] ^ zobrist_white[pos][0];
		zobrist_flip[pos][1] = zobrist_black[pos][1] ^ zobrist_white[pos][1];
	}
}

// ��zobrist�����ʼ��
void zobrist_init() {
	/*zobrist_swap_player[0] = random();
	zobrist_swap_player[1] = random();
	for (int pos = 0; pos < 64; pos++) {
		zobrist_black[pos][0] = random();
		zobrist_black[pos][1] = random();

		zobrist_white[pos][0] = random();
		zobrist_white[pos][1] = random();
	}*/
}
// ����ɢ����
void get_hashcode2(int pos, int flip[], int n_flip, unsigned int hashcode[], int color) {
	// �����һ����������Pass������������巽�ֻ�����ɢ����
	if (pos == -1) {
		hashcode[0] = zobrist_swap_player[0];
		hashcode[1] = zobrist_swap_player[1];
		return;
	}
	// ����Ǻ����£����������λ���Ϻ����ɢ����
	if (color == black) {
		hashcode[0] = zobrist_black[pos][0];
		hashcode[1] = zobrist_black[pos][1];
		// �����ǰ����£����������λ���ϰ����ɢ����
	}
	else {
		hashcode[0] = zobrist_white[pos][0];
		hashcode[1] = zobrist_white[pos][1];
	}
	// ����ÿ������ת�����ӣ����Ӹ�λ���Ϸ����ɢ����
	for (int i = 0; i<n_flip; ++i) {
		hashcode[0] ^= zobrist_flip[flip[i]][0];
		hashcode[1] ^= zobrist_flip[flip[i]][1];
	}
}

// ����ɢ����
void get_hashcode(int board[][8], unsigned int hashcode[],int color) {
	hashcode[0] = hashcode[1] = 0;
	// ����ÿ��λ��
	for (int i = 0; i < 8; ++i)
		for (int j = 0; j < 8; ++j) {          // pos = A1; pos <= H8
											   // ����Ǻ��壬����Ӹ�λ���Ϻ����ɢ����
			int pos = i * 8 + j;
			if (board[i][j] == 1) {    //black
				hashcode[0] ^= zobrist_black[pos][0];
				hashcode[1] ^= zobrist_black[pos][1];
				// ����ǰ��壬����Ӹ�λ���ϰ����ɢ����
			}
			else if (board[i][j] == -1) {   //white
				hashcode[0] ^= zobrist_white[pos][0];
				hashcode[1] ^= zobrist_white[pos][1];
			}
		}
	// ����ֵ������£�����ӷ�ӳ���巽�ֻ��ĸ���ɢ����
	if (color == white) {
		hashcode[0] ^= zobrist_swap_player[0];
		hashcode[1] ^= zobrist_swap_player[1];
	}
}

// ����ɢ�б�����һ������
void hash_update(unsigned int hashcode[], double lower, double upper, int best_move, int depth) {
	Hashtable *p = hashtable + (hashcode[0] & HASHTABLE_MASK);
	Hashtable_Node *deepest = &(p->deepest), *newest = &(p->newest);
	// ����롰�����滻���ľ�����ͬ
	if ((hashcode[1] == deepest->lock) && (depth == deepest->depth)) {
		// ��������
		if (lower > deepest->lower) {
			deepest->lower = lower;
			// ��������岽
			deepest->best_move = best_move;
		}
		// ��������
		if (upper < deepest->upper) {
			deepest->upper = upper;
		}
		// ����롰ʼ���滻���ľ�����ͬ
	}
	else if ((hashcode[1] == newest->lock) && (depth == newest->depth)) {
		// ��������
		if (lower > newest->lower) {
			newest->lower = lower;
			// ��������岽
			newest->best_move = best_move;
		}
		// ��������
		if (upper < newest->upper) {
			newest->upper = upper;
		}
		// �������������ͻ�������ͻ�������������ȸ��󣬽��С������滻��
	}
	else if (depth > deepest->depth) {
		// ԭ�ȱ���ľ������б�����ֵ
		*newest = *deepest;
		// ���浱ǰ�������
		deepest->lock = hashcode[1];
		deepest->lower = lower;
		deepest->upper = upper;
		deepest->best_move = best_move;
		deepest->depth = depth;
		// ������С�ʼ���滻��
	}
	else {
		// ���浱ǰ�������
		newest->lock = hashcode[1];
		newest->lower = lower;
		newest->upper = upper;
		newest->best_move = best_move;
		newest->depth = depth;
	}
}

// ����ɢ�б�ȡ��һ������
Hashtable_Node *hash_get(unsigned int hashcode[], int depth) {
	Hashtable *p = hashtable + (hashcode[0] & HASHTABLE_MASK);
	Hashtable_Node *deepest = &(p->deepest), *newest = &(p->newest);
	// ����롰�����滻���ľ�����ͬ���򷵻ظþ���ڵ�
	if ((hashcode[1] == deepest->lock) && (depth == deepest->depth)) {
		return deepest;
		// ����롰ʼ���滻���ľ�����ͬ���򷵻ظþ���ڵ�
	}
	else if ((hashcode[1] == newest->lock) && (depth == newest->depth)) {
		return newest;
		// ��������޴˾��棬���ؿ�ָ��
	}
	else {
		return NULL;
	}
}

int numb = 0,num1=0,num2=0;

SVpair alpha_beta_with_hashtable(int board[SIZE][SIZE], double alpha, double beta, int depth, int color, int pass) {
	if (isGameEnd(board))
		return SVpair(-1,getGameEndEval(board,color));
	unsigned int hashcode[2];
	// ���㵱ǰ�����ɢ����
	get_hashcode(board, hashcode,color);
	// ��ѯɢ�б�
	Hashtable_Node *node = hash_get(hashcode, depth);
	// ���ɢ�б��д��ڵ�ǰ����
	if (node) {
		numb++;
		// �����������޴���alphaֵ��������alphaֵ
		if (node->lower > alpha) {
			alpha = node->lower;
			// ������޴��ڻ����betaֵ��������֦
			if (alpha >= beta) {
				num1++;
				return SVpair(node->best_move,alpha);
			}
		}
		// ������������С��betaֵ��������betaֵ
		if (node->upper < beta) {
			beta = node->upper;
			// �������С�ڻ����alphaֵ�������¼�֦
			if (beta <= alpha) {
				num2++;
				return SVpair(node->best_move,beta);
			}
		}
	}
	if (GetTime() + 30 >= timeout_turn)
	{
		stopThink = true;
		return SVpair(-1, alpha);
	}
	// ���г���ļ�֦�㷨
	POINT2 NOW;
	double best_value = -BND;
	int bestmove = -1;
	int i, j;
	// ����ÿ������λ��
	for (i = 0; i < 8; ++i)
		for (j = 0; j < 8; ++j) {
			// �������ⲽ�壬����岽�Ϸ�
			if (Judge(i, j, color, board)) {
				int boardnow[8][8];
				copymap(boardnow, board);									//������ͼ
				boardnow[i][j] = color == 1 ? 1 : -1;						//ģ��Է�����
				NOW.INIT(i, j);
				Change(NOW, boardnow, false);								//�ı�����
				double value;
				// �������Ԥ����������ȣ�ֱ�Ӹ��������ֵ
				if (depth <= 1) value = -evaluation(boardnow, 1 - color);
				// ���򣬶����γɵľ�����еݹ�����
				else value = -alpha_beta_with_hashtable(boardnow, -beta, -alpha, depth - 1, 1 - color, 0).value;
				copymap(boardnow, board);

				if (stopThink) return SVpair(bestmove, best_value);
				// �����岽
				// ����ⲽ��������֦
				if (value > best_value) {
					best_value = value;
					bestmove = i * 8 + j;
					if (value >= beta) {
						hash_update(hashcode, best_value, BND, bestmove, depth);
						return SVpair(bestmove,best_value);
					}
					if (value > alpha) {
						alpha = value;
					}
				}
			}
		}
	// ���û�кϷ��岽
	if (best_value == -BND) {
		// �����һ����Ҳ�������������Ծֽ���
		if (pass) {
			// ֱ�Ӹ�����ȷ�ȷ�
			best_value = getGameEndEval(board,color);
			// �����ⲽ������
		}
		else {
			//make_pass();
			// �ݹ�������������������
			best_value = -alpha_beta_with_hashtable(board, -beta, -alpha, depth - 1, 1 - color, 1).value;
			// ��������
			//undo_pass();
		}
	}
	// ������ѽ��
	// ��ɢ�б��б��浱ǰ�������
	if (best_value >= beta) {
		hash_update(hashcode, best_value, INF, bestmove, depth);
	}
	else if (best_value <= alpha) {
		hash_update(hashcode, -INF, best_value, bestmove, depth);
	}
	else {
		hash_update(hashcode, best_value, best_value, bestmove, depth);
	}
	return SVpair(bestmove,best_value);
}

POINT2 Difficult()
{
	start = clock();
	stopThink = false;
	int depth = 2, color = NOWCOLOR;
	myplayer = NOWCOLOR == 1 ? 1 : -1;
	POINT2 BEST;
	int tempboard[8][8];
	copymap(tempboard, mapp);
	SVpair sbest = alpha_beta_with_hashtable(tempboard, -BND, +BND, MIN_DEPTH, color, 0);
	for (depth = MIN_DEPTH+1; !stopThink && depth < MAX_DEPTH; depth++) {
		SVpair stemp = alpha_beta_with_hashtable(tempboard, -BND, +BND, depth, color, 0);
		if (stemp.value > INF) { sbest = stemp; stopThink = true; break;}
		if (!stopThink) sbest = stemp;
	}
	BEST.INIT(int(sbest.move/ 8), sbest.move % 8);
	return BEST;
}

////////////////////////////////////////////////////////////////////////////////////
int evaluation2(int board[8][8], int color) {
	int my_tiles = 0, opp_tiles = 0, my_front_tiles = 0, opp_front_tiles = 0;
	int my_mobil = 0, opp_mobil = 0;
	int myplayer = color == 1 ? 1 : -1;
	double vtiles = 0, vfront = 0, vmobil = 0, vcorner = 0, vclose = 0;
	int map_weight = 0;

	int i, j;
	for (i = 0; i<8; i++)
		for (j = 0; j < 8; j++) {
			if (Judge(i, j, color, board)) { my_mobil++; continue; }
			else if (Judge(i, j, 1 - color, board)) { opp_mobil++; continue; }
			else if (board[i][j] == myplayer) {
				map_weight += weight[i][j]; my_tiles++;
				if (i > 0 && i < 7 && j>0 && j < 7) {
					if (board[i - 1][j] == 0) { my_front_tiles++; continue; }
					else if (board[i + 1][j] == 0) { my_front_tiles++; continue; }
					else if (board[i][j - 1] == 0) { my_front_tiles++; continue; }
					else if (board[i][j + 1] == 0) { my_front_tiles++; continue; }
					else if (board[i - 1][j - 1] == 0) { my_front_tiles++; continue; }
					else if (board[i - 1][j + 1] == 0) { my_front_tiles++; continue; }
					else if (board[i + 1][j - 1] == 0) { my_front_tiles++; continue; }
					else if (board[i + 1][j + 1] == 0) { my_front_tiles++; continue; }
				}
				else if (i == 0 || i == 7) {
				if (j > 0) {
				if (board[i][j - 1] == 0) { my_front_tiles++; continue; }
				}
				else if (j < 7) {
				if (board[i][j + 1] == 0) { my_front_tiles++; continue; }
				}
				}
				else if (j == 0 || j == 7) {
				if (i > 0) {
				if (board[i - 1][j] == 0) { my_front_tiles++; continue; }
				}
				else if (i < 7) {
				if (board[i + 1][j] == 0) { my_front_tiles++; continue; }
				}
				}

			}
			else if (board[i][j] == -myplayer) {
				map_weight -= weight[i][j]; opp_tiles++;
				if (i > 0 && i < 7 && j>0 && j < 7) {
					if (board[i - 1][j] == 0) { opp_front_tiles++; continue; }
					else if (board[i + 1][j] == 0) { opp_front_tiles++; continue; }
					else if (board[i][j - 1] == 0) { opp_front_tiles++; continue; }
					else if (board[i][j + 1] == 0) { opp_front_tiles++; continue; }
					else if (board[i - 1][j - 1] == 0) { opp_front_tiles++; continue; }
					else if (board[i - 1][j + 1] == 0) { opp_front_tiles++; continue; }
					else if (board[i + 1][j - 1] == 0) { opp_front_tiles++; continue; }
					else if (board[i + 1][j + 1] == 0) { opp_front_tiles++; continue; }
				}
				else if (i == 0 || i == 7) {
				if (j > 0) {
				if (board[i][j - 1] == 0) { opp_front_tiles++; continue; }
				}
				else if (j < 7) {
				if (board[i][j + 1] == 0) { opp_front_tiles++; continue; }
				}
				}
				else if (j == 0 || j == 7) {
				if (i > 0) {
				if (board[i - 1][j] == 0) { opp_front_tiles++; continue; }
				}
				else if (i < 7) {
				if (board[i + 1][j] == 0) { opp_front_tiles++; continue; }
				}
				}
			}
		}

	// ���������ͱ�Ե������
	if (my_tiles > opp_tiles)
		vtiles = (100.0 * my_tiles) / (my_tiles + opp_tiles);
	else if (my_tiles < opp_tiles)
		vtiles = -(100.0 * opp_tiles) / (my_tiles + opp_tiles);
	if (my_front_tiles > opp_front_tiles)
		vfront = -(100.0 * my_front_tiles) / (my_front_tiles + opp_front_tiles);
	else if (my_front_tiles < opp_front_tiles)
		vfront = (100.0 * opp_front_tiles) / (my_front_tiles + opp_front_tiles);

	//�ж���
	if (my_mobil > opp_mobil)
		vmobil = (100.0 * my_mobil) / (my_mobil + opp_mobil);
	else if (my_mobil < opp_mobil)
		vmobil = -(100.0 * opp_mobil) / (my_mobil + opp_mobil);

	// �����Ȩ��
	vcorner = board[0][0] * myplayer + board[0][7] * myplayer + board[7][0] * myplayer + board[7][7] * myplayer;
	vcorner = 25 * vcorner;

	// �����Ա�λ�õ�Ȩ��,����λ��cλ
	if (board[0][0] == 0) {
		vclose += board[0][1] * myplayer;
		vclose += board[1][0] * myplayer;
		vclose += board[1][1] * myplayer;
	}
	if (board[0][7] == 0) {
		vclose += board[0][6] * myplayer;
		vclose += board[1][7] * myplayer;
		vclose += board[1][6] * myplayer;
	}
	if (board[7][0] == 0) {
		vclose += board[6][0] * myplayer;
		vclose += board[7][1] * myplayer;
		vclose += board[6][1] * myplayer;
	}
	if (board[7][7] == 0) {
		vclose += board[7][6] * myplayer;
		vclose += board[6][7] * myplayer;
		vclose += board[6][6] * myplayer;
	}
	vclose = -12.5 * vclose;


	// ��Ȩ�õ��ܵĵ÷�  difficult2�ǰ׷���difficult�Ǻ�
	double score = ((10 * vtiles) + (801.724 * vcorner) + (382.026 * vclose) + (78.922 * vmobil) + (74.396 * vfront) + (10 * map_weight)) * 100;

	return int(score);
}

SVpair alpha_beta_with_hashtable2(int board[SIZE][SIZE], double alpha, double beta, int depth, int color, int pass) {
	if (isGameEnd(board))
		return SVpair(-1, getGameEndEval(board, color));
	POINT2 NOW;
	double best_value = -BND;
	int bestmove = -1;
	int i, j;
	// ����ÿ������λ��
	for (i = 0; i < 8; ++i)
		for (j = 0; j < 8; ++j) {
			// �������ⲽ�壬����岽�Ϸ�
			if (Judge(i, j, color, board)) {
				int boardnow[8][8];
				copymap(boardnow, board);									//������ͼ
				boardnow[i][j] = color == 1 ? 1 : -1;						//ģ��Է�����
				NOW.INIT(i, j);
				Change(NOW, boardnow, false);								//�ı�����
				double value;
				// �������Ԥ����������ȣ�ֱ�Ӹ��������ֵ
				if (depth <= 1) value = -evaluation2(boardnow, 1 - color);
				// ���򣬶����γɵľ�����еݹ�����
				else value = -alpha_beta_with_hashtable2(boardnow, -beta, -alpha, depth - 1, 1 - color, 0).value;
				copymap(boardnow, board);
				//if ((clock() - start_clock)>0.94 * CLOCKS_PER_SEC)
					//break;
				// �����岽
				// ����ⲽ��������֦
				if (value > best_value) {
					best_value = value;
					bestmove = i * 8 + j;
					if (value >= beta) {
						return SVpair(bestmove, best_value);
					}
					if (value > alpha) {
						alpha = value;
					}
				}
			}
		}
	// ���û�кϷ��岽
	if (best_value == -BND) {
		// �����һ����Ҳ�������������Ծֽ���
		if (pass) {
			// ֱ�Ӹ�����ȷ�ȷ�
			best_value = getGameEndEval(board, color);
			// �����ⲽ������
		}
		else {
			//make_pass();
			// �ݹ�������������������
			best_value = -alpha_beta_with_hashtable2(board, -beta, -alpha, depth - 1, 1 - color, 1).value;
			// ��������
			//undo_pass();
		}
	}
	// ������ѽ��
	// ��ɢ�б��б��浱ǰ�������
	return SVpair(bestmove, best_value);
}

POINT2 Difficult2()
{
	int start1 = clock();
	myplayer = NOWCOLOR == 1 ? 1 : -1;
	start_clock = clock();
	int depth = 6, color = NOWCOLOR;
	if (step > 20) depth = 8;
	POINT2 BEST;
	int tempboard[8][8];
	copymap(tempboard, mapp);
	SVpair sbest = alpha_beta_with_hashtable2(tempboard, -BND, +BND, depth, color, 0);
	/*time61[step] = clock() - start1;
	sbest = alpha_beta_with_hashtable2(tempboard, -BND, +BND, depth+1, color, 0);
	time71[step] = clock() - start1- time61[step];
	if (step > 20) {
		sbest = alpha_beta_with_hashtable2(tempboard, -BND, +BND, depth+2, color, 0);
		time81[step] = clock() - start1 - time71[step]- time61[step];
		if(step>25) 		sbest = alpha_beta_with_hashtable2(tempboard, -BND, +BND, depth+4, color, 0);
		time101[step] = clock() - start1 - time81[step] - time71[step] - time61[step];
	}
	/*for ( depth = MIN_DEPTH + 1; depth <= MAX_DEPTH; ++depth) {
	SVpair stemp = alpha_beta_with_hashtable2(tempboard, -BND, +BND, depth,color, 0);
		if (clock() - start_clock> 0.94 * CLOCKS_PER_SEC)
			break;
		sbest = stemp;
	}*/
	BEST.INIT(int(sbest.move / 8), sbest.move % 8);
	return BEST;
}


////////////////////////////////////////////////////////////////////////////////
void save_game() {
	ofstream outfile("document.dat", ios::binary);//�Զ�������ʽ����
	if (!outfile) {
		cerr << "����ʧ�ܣ�" << endl;
		HWND wnd = GetHWnd();										//��ȡ���ھ��
		MessageBox(wnd, "����ʧ�ܣ�", "������Ϸ", MB_OK);
		abort();
	}
	outfile.write((char*)&mapp[0], sizeof(mapp));//��������
	outfile.write((char*)&step, sizeof(step));//����
	outfile.write((char*)&MYCOLOR, sizeof(MYCOLOR));//���������ɫ
	outfile.write((char*)&NOWCOLOR, sizeof(NOWCOLOR));//��ǰ������ɫ
	outfile.write((char*)&whitecount, sizeof(whitecount));//����÷�
	outfile.write((char*)&blackcount, sizeof(blackcount));//����÷�
	outfile.write((char*)&diff, sizeof(diff));//�Ѷ�
	outfile.write((char*)&single, sizeof(single));//����ģʽ
	outfile.write((char*)&historymap[0][0], sizeof(historymap));//��ʷ����
	outfile.close();

	HWND wnd = GetHWnd();										//��ȡ���ھ��
	MessageBox(wnd, "����ɹ�", "������Ϸ", MB_OK);
}

void load_game() {
	ifstream infile("document.dat", ios::binary);//���ĵ���2������ʽ����
	if (!infile) {
		cerr << "��ȡʧ�ܣ�" << endl;
		HWND wnd = GetHWnd();										//��ȡ���ھ��
		MessageBox(wnd, "��ȡʧ�ܣ�", "��ȡ��Ϸ", MB_OK);
		abort();
	}
	init();
	infile.read((char*)&mapp[0], sizeof(mapp));//��ȡ����
	infile.read((char*)&step, sizeof(step));//����
	infile.read((char*)&MYCOLOR, sizeof(MYCOLOR));//���������ɫ
	infile.read((char*)&NOWCOLOR, sizeof(NOWCOLOR));//���������ɫ
	infile.read((char*)&whitecount, sizeof(whitecount));//����������ɫ
	infile.read((char*)&blackcount, sizeof(blackcount));//����÷�
	infile.read((char*)&diff, sizeof(diff));//�Ѷ�
	infile.read((char*)&single, sizeof(single));//����ģʽ
	infile.read((char*)&historymap[0][0], sizeof(historymap));//��ʷ����
	infile.close();

	printmapp();
	Printcount(blackcount, whitecount, MYCOLOR);					//��ǰ����
	Statistics(MYCOLOR);
	HWND wnd = GetHWnd();										//��ȡ���ھ��
	MessageBox(wnd, "������Ϸ�ɹ�", "ȷ��", MB_OK);
}

void hui_qi() {
	if (step < 2) {
		HWND wnd = GetHWnd();										//��ȡ���ھ��
		MessageBox(wnd, "��Ҫ̫̰���ϣ��Ѿ����ܻ�����", "����", MB_OK);
		return;
	}
	bool flag = false;//�����ж���һ���ܷ���������
	for (step -= 1; step > 0; step--) {//����һ�ؿ�ʼ����
		for (int i = 1; i <= 8; i++) {
			for (int j = 1; j <= 8; j++)
				if (historymap[step][i][j] != 0) {//����ʷ������ĳһλ�ò�Ϊ0����˵��������̿��ã���Ϊ��������ʱ�Ѿ�����������ж�
					flag = true;//������̿���
					break;
				}
			if (flag)
				break;
		}
		if (flag) {//�����������
			copymap(mapp, historymap[step]);
			break;
		}
	}
	int i, j;
	int tstep = step, tMYCOLOR = MYCOLOR, tdiff = diff, tNOWCOLOR = NOWCOLOR;
	int tmapp[SIZE][SIZE], thistorymap[100][SIZE][SIZE];
	bool tsingle = single;
	copymap(tmapp, mapp);
	for (i = 0; i <= tstep; i++) copymap(thistorymap[i], historymap[i]);
	init();
	step = tstep, MYCOLOR = tMYCOLOR, diff = tdiff, single = tsingle, NOWCOLOR = 1-tNOWCOLOR;
	copymap(mapp, tmapp);
	for (i = 0; i <= tstep; i++) copymap(historymap[i], thistorymap[i]);
	whitecount = blackcount = 0;

	for (i = 0; i < SIZE; i++)
		for (j = 0; j < SIZE; j++) {
			if (mapp[i][j] == 1) blackcount++;
			else if (mapp[i][j] == -1) whitecount++;
		}

	printmapp();
	Printcount(blackcount, whitecount,NOWCOLOR);					//��ǰ����
	Statistics(MYCOLOR);

	HWND wnd = GetHWnd();										//��ȡ���ھ��
	MessageBox(wnd, "����ɹ�", "����", MB_OK);
}

POINT2 MOUSE()										//����¼�
{
	MOUSEMSG m;
	while (true)
	{
		m = GetMouseMsg();							//��ȡ�����Ϣ
		switch (m.uMsg)
		{
		case(WM_LBUTTONDOWN):						//������������ʱ
		{
			POINT2 NOWMOUSE;
			NOWMOUSE.INIT(m.x, m.y);
			if (TOINTERNET)							//�������������ս״̬ ���͵�ǰ����
			{
				char Toyou[50] = { 0 };
				sprintf_s(Toyou, "%d,%d", m.x, m.y);
				send(sockConn, Toyou, sizeof(Toyou), 0);		//��������
			}
			return NOWMOUSE;						//�����������
			break;
		}
		case(WM_MOUSEMOVE):								//���� ������λ��
		{
			if (ESCEXIT)gameStart();
			break;
		}
		case(WM_RBUTTONDOWN):								//�������Ҽ�����ʱ
		{
			TIANEYES = !TIANEYES;							//����OR�ر�����ģʽ
			break;
		}
		}
	}
}

bool putmouse(POINT2 &m)									//�ض����������
{
	bool flag = true;
	int mouseinx[SIZE + 1], mouseiny[SIZE + 1];
	for (int i = 0; i < SIZE + 1; ++i)						//��ȷ������
	{
		mouseinx[i] = PX + i*BBLACK;
		mouseiny[i] = PY + i*BBLACK;
	}
	if (m.x > WINDOWS_X - WINDOWS_X / 4 - 10 && m.x< WINDOWS_X - WINDOWS_X / 4 + 1.5*BBLACK && m.y > 0.5*BBLACK - 10 && m.y < BBLACK)  //������Ϸ
	{
		mciSendString("play Music/���.wav", NULL, 0, NULL);
		save_game();
	}
	else if (m.x > WINDOWS_X - WINDOWS_X / 4 + 2 * BBLACK && m.x< WINDOWS_X - WINDOWS_X / 4 + 3.5 * BBLACK + 10 && m.y > 0.5*BBLACK - 10 && m.y < BBLACK)  //��ȡ�浵
	{
		mciSendString("play Music/���.wav", NULL, 0, NULL);
		load_game();
		if (!single) STARTVS(MYCOLOR, MOUSE, MOUSE);
		else {
			if (diff == 1) STARTVS(MYCOLOR, MOUSE, Easy);
			else if (diff == 2) STARTVS(MYCOLOR, MOUSE, Middle);
			else  STARTVS(MYCOLOR, MOUSE, Difficult);
		}
	}
	else if (m.x > WINDOWS_X - WINDOWS_X / 4 - 10 && m.x< WINDOWS_X - WINDOWS_X / 4 + BBLACK && m.y > 1.5*BBLACK && m.y < 2 * BBLACK)  //����
	{
		mciSendString("play Music/����.wav", NULL, 0, NULL);
		if (single) hui_qi();
		else {
			HWND wnd = GetHWnd();										//��ȡ���ھ��
			MessageBox(wnd, "˫����Ϸ���������", "����", MB_OK);
		}
	}
	if (m.x < PX || m.x>PX + SIZE*BBLACK || m.y < PY || m.y>PY + SIZE*BBLACK)flag = false;	//��������������
	else
	{
		mciSendString("play Music/����.wav", NULL, 0, NULL);
		for (int i = 0; i<SIZE; ++i)
		{
			if (m.x >= mouseinx[i] && m.x <= mouseinx[i + 1])
			{
				if (m.x - mouseinx[i]>BBLACK / 8 && mouseinx[i + 1] - m.x>BBLACK / 8)		//�ض���X
				{
					m.x = (mouseinx[i] + mouseinx[i + 1]) / 2;
				}
				else flag = false;
			}
		}
		for (int i = 0; i<SIZE; ++i)
		{
			if (m.y >= mouseiny[i] && m.y <= mouseiny[i + 1])
			{
				if (m.y - mouseiny[i]>BBLACK / 8 && mouseiny[i + 1] - m.y > BBLACK / 8)		//�ض���Y
				{
					m.y = (mouseiny[i] + mouseiny[i + 1]) / 2;
				}
				else flag = false;
			}
		}
	}
	return flag;											//���ص�ǰλ���ܷ�����
}

void CleanLast(POINT2 WINDOWS2, int F)						//��¼��һ������λ��
{
	if (LASTCH.x > SIZE&&LASTCH.y > SIZE)					//����ȡ����һ�����
	{
		setfillcolor(getpixel(LASTCH.x, LASTCH.y));			//��ȡԭ��������ɫ
		putimage(LASTCH.x - BBLACK / 2, LASTCH.y - BBLACK / 2, &MAPIMAGE[LASTCH.WIN2MAPX()][LASTCH.WIN2MAPY()]);
		solidcircle(LASTCH.x, LASTCH.y, CHESSSIZE);
	}

	setfillcolor(RGB(49, 153, 182));						//����Ϊ��䵱ǰ����
	LASTCH.INIT(WINDOWS2.x, WINDOWS2.y);

	solidrectangle(WINDOWS2.x - BBLACK / 2 + 2, WINDOWS2.y - BBLACK / 2 + 2, WINDOWS2.x + BBLACK / 2 - 2, WINDOWS2.y + BBLACK / 2 - 2);		//��������
	setfillcolor(F ? BLACK : WHITE);
	solidcircle(WINDOWS2.x, WINDOWS2.y, CHESSSIZE);			//������
}

int Playchess(int F, POINT2 WINDOWS2, int &balckcount, int &whitecount)	//��ʼ
{
	//F �ڷ�Ϊ1  �׷�Ϊ0
	POINT2 MAP2;

	if (WINDOWS2.x < SIZE&&WINDOWS2.y < SIZE)					//������������Ϊ��������
	{
		MAP2 = WINDOWS2;
		WINDOWS2.MAP2WIN(WINDOWS2);								//����ת����ʵ��չʾ����
	}
	else
	{
		if (!putmouse(WINDOWS2))return 0;						//������������ض���
		WINDOWS2.WIN2MAP(MAP2);									//�洢�ض���֮��ľ�������
	}
	if (expect[MAP2.x][MAP2.y])									//��λ�ÿ���
	{
		CleanStatistics();										//�����Ļ��ʾ
		if (F)													//�ж����Ϊ����÷�
		{
			balckcount += expect[MAP2.x][MAP2.y] + 1;
			whitecount -= expect[MAP2.x][MAP2.y];
		}
		else
		{
			whitecount += expect[MAP2.x][MAP2.y] + 1;
			balckcount -= expect[MAP2.x][MAP2.y];
		}

		printcircle(WINDOWS2.x, WINDOWS2.y, F, mapp);			//������ mappΪ��������
		CleanLast(WINDOWS2, F);									//��ǰ����������ʾ
		Change(MAP2, mapp, true);									//��ת���� trueΪ��ʾ����Ļ
		Printcount(balckcount, whitecount, F);	//��ӡ����

		if (balckcount + whitecount >= SIZE*SIZE || !balckcount || !whitecount)return 3;	//���ʤ���ѷ�
		if (!Statistics(!F))									//����Է��޷�����
		{
			if (Statistics(F))									//�ж��Լ��Ƿ��������
			{
				HL(!F);											//�Լ�����������Է��޷�������Ϣ
				Printcount(balckcount, whitecount, !F);			//��Ϊǰ���Ѿ��ı���״̬���������ڻ�ԭ
				return 2;
			}
			else return 3;										//˫�����޷�����
		}
		return 1;
	}
	return 0;
}

void STARTVS(int YOURCOLOR, POINT2 P1(), POINT2 P2())			//��ʼ��ս   (mycolor,mouse,easy)
{
	mciSendString("stop Music/��������.mp3", NULL, 0, NULL);
	mciSendString("play Music/���.mp3", NULL, 0, NULL);
	time_t timestart , timeend ;
	int music_no = 1;
	timestart= time(NULL);
	MYCOLOR = YOURCOLOR;
	Printcount(blackcount, whitecount, NOWCOLOR);					//��ǰ����
	Statistics(NOWCOLOR);
	if (MYCOLOR == white && step == 1) goto CX2;
	if (NOWCOLOR == white && !single) goto CX2;
	while (true)
	{
		cout << "hello" << endl;
	CX1:														//���غϺ���
		timeend = time(NULL);
		if (music_no == 1 && difftime(timeend, timestart) > 170) {
			mciSendString("stop Music/��������.mp3", NULL, 0, NULL);
			mciSendString("stop Music/���.mp3", NULL, 0, NULL);
			mciSendString("play Music/���2.mp3", NULL, 0, NULL);
			timestart = time(NULL);
			music_no = 2;
		}
		if (music_no == 2 && difftime(timeend, timestart) > 285) {
			mciSendString("stop Music/��������.mp3", NULL, 0, NULL);
			mciSendString("stop Music/���2.mp3", NULL, 0, NULL);
			mciSendString("play Music/���.mp3", NULL, 0, NULL);
			timestart = time(NULL);
			music_no = 1;
		}
		copymap(historymap[step], mapp);
		int PD = Playchess(MYCOLOR, (*P1)(), blackcount, whitecount);
		if (single && MYCOLOR == white) step++;
		switch (PD)
		{
		case 0:
			if (MYCOLOR == white) step--;
			goto CX1;											//����ʧ���������
			break;
		case 1:
			break;												//��������
		case 2:
			if (MYCOLOR == white) step++;
			goto CX1;											//���ԶԷ�
			break;
		case 3:
			goto ED;											//��ֽ���
			break;
		}
	CX2:                                     //���غϺ���
		if (!single) step++;
		if (single && MYCOLOR == black) step++;
		PD = Playchess(!MYCOLOR, (*P2)(), blackcount, whitecount);
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
ED:																	//����
	WIN(YOURCOLOR, blackcount, whitecount);
	_getch();
	if (TOINTERNET)
	{
		closesocket(sockSer);
		closesocket(sockConn);
	}
	gameStart();
}

char *ip;												//����IP��ַ����
void Get_ip()											//��ȡ����IP��ַ
{
	WSADATA wsaData;
	char name[255];										//�������ڴ�Ż�õ��������ı���
	PHOSTENT hostinfo;									//���Winsock�汾����ȷֵ
	if (WSAStartup(MAKEWORD(2, 0), &wsaData) == 0)		//���Ǽ���Winsock�⣬���WSAStartup������������ֵΪ0��˵�����سɹ���������Լ���
	{
		if (gethostname(name, sizeof(name)) == 0)		//�ɹ��ؽ������������������name����ָ���Ļ�������
		{
			if ((hostinfo = gethostbyname(name)) != NULL) //���ǻ�ȡ������
			{
				settextstyle(BBLACK / 2, 0, "����Ҧ��");
				outtextxy(WINDOWS_X / 2 - 5 * BBLACK / 2, BBLACK * 2, "��ת���� �������Ѵ���");
				settextstyle(BBLACK / 4, 0, "����");
				ip = inet_ntoa(*(struct in_addr *)*hostinfo->h_addr_list);   //����inet_ntoa������������hostinfo�ṹ�����е�h_addr_listת��Ϊ��׼�ĵ�ֱ�ʾ��IP
				char c[250] = { "1.�������ͬѧ���ip��ַ�Ϳ���������~" };
				strcat_s(c, ip);
				outtextxy(WINDOWS_X / 2 - 3 * BBLACK, BBLACK * 7 / 2, c);
				outtextxy(WINDOWS_X / 2 - 3 * BBLACK, BBLACK * 4, "2.����Ҫ��ȷ��������ͬһ����������Ŷ��");
			}
		}
		WSACleanup();										//ж��Winsock�⣬���ͷ�������Դ
	}
}

POINT2 OURCLASS()									//�����Է����͵�����
{
	POINT2 YOU;
	char data[50] = { 0 };
	int x = 0, y = 0;
	int p = recv(sockConn, data, 50, 0);
	if (p == SOCKET_ERROR)
	{
		HWND wnd = GetHWnd();
		MessageBox(wnd, "�Է����жϳ�����ѵ���,����������", "�����ж�", MB_OK | MB_ICONWARNING);
		exit(0);
	}
	sscanf_s(data, "%d,%d", &x, &y);				//�����ڱ�����
	YOU.INIT(x, y);
	return YOU;
}

void TOI(bool FUORKE)								//����ģʽ
{
	WORD wVersionRequested;
	WSADATA wsaData;
	int err, res = 0;
	wVersionRequested = MAKEWORD(1, 1);
	err = WSAStartup(wVersionRequested, &wsaData);
	if (err != 0) return;
	if (LOBYTE(wsaData.wVersion) != 1 || HIBYTE(wsaData.wVersion) != 1)
	{
		WSACleanup();
		return;
	}
	SOCKADDR_IN addrSer;
	if (FUORKE)
	{
		Get_ip();										//����������
		sockSer = socket(AF_INET, SOCK_STREAM, 0);
	}
	else
	{
		settextstyle(BBLACK / 2, 0, "����Ҧ��");
		outtextxy(WINDOWS_X / 2 - 3 * BBLACK / 2, BBLACK * 3 / 2, "��ת���������");
		settextstyle(BBLACK / 4, 0, "����");
		outtextxy(WINDOWS_X / 2 - 2 * BBLACK, BBLACK * 5 / 2, "1����ȷ�����������������");
		outtextxy(WINDOWS_X / 2 - 2 * BBLACK, BBLACK * 3, "2����ȷ�����������ͬһ��������");
		ip = (char*)malloc(sizeof(char) * 50);
		InputBox(ip, 50, "���������˵�IP��ַ");
		sockConn = socket(AF_INET, SOCK_STREAM, 0);
	}
	addrSer.sin_family = AF_INET;
	addrSer.sin_port = htons(5050);
	addrSer.sin_addr.S_un.S_addr = inet_addr(ip);
	if (FUORKE)											//���Ϊ�����
	{
		SOCKADDR_IN addrCli;
		bind(sockSer, (SOCKADDR*)&addrSer, sizeof(SOCKADDR));
		listen(sockSer, 5);
		int len = sizeof(SOCKADDR);
		settextstyle(BBLACK / 4, 0, "����");
		outtextxy(WINDOWS_X / 2 - 2 * BBLACK, BBLACK * 5, "�������Ѵ������ȴ�������...");
		sockConn = accept(sockSer, (SOCKADDR*)&addrCli, &len);
	}
	else res = connect(sockConn, (SOCKADDR*)&addrSer, sizeof(SOCKADDR));
	if (sockConn == INVALID_SOCKET || res)
	{
		outtextxy(2 * BBLACK, BBLACK * 6, "����ʧ�ܣ�");
		_getch();
		gameStart();									//����������
	}
	else
	{
		outtextxy(2 * BBLACK, BBLACK * 6, "���ӳɹ�����������������Ϸ~");
		_getch();
	}
}

void gameStart()
{
	mciSendString("stop Music/���.mp3", NULL, 0, NULL);
	mciSendString("play Music/��������.mp3", NULL, 0, NULL);
	time_t timestart, timeend;
	timestart = time(NULL);

	IMAGE MM[11] = { 0 }, MB[3] = { 0 }, MC[3] = { 0 };
	initgraph(WINDOWS_X, WINDOWS_Y);
	setbkmode(TRANSPARENT);					//͸������

	HWND hwnd = GetHWnd();					// ���ô��ڱ�������
	SetWindowText(hwnd, "��ת����");
	loadimage(NULL, "Picture/0.jpg", 1200, 800);

	const int bblack = 10;

	LOGFONT f;
	gettextstyle(&f);												// ��ȡ������ʽ
	f.lfHeight = BBLACK;												// ��������߶�
	strcpy_s(f.lfFaceName, _T("����Ҧ��"));								// ��������
	f.lfQuality = ANTIALIASED_QUALITY;								// �������Ч��Ϊ�����
	settextstyle(&f);												// ����������ʽ
	RECT r1 = { 0, 0, WINDOWS_X, WINDOWS_Y / 3 };
	drawtext("�� ת �� ��", &r1, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

	settextstyle(BBLACK / 3, 0, "����Ҧ��");
	RECT r2 = { WINDOWS_X / 2 - BBLACK,WINDOWS_Y / 3,WINDOWS_X / 2 + BBLACK ,WINDOWS_Y / 3 + BBLACK / 2 };
	rectangle(WINDOWS_X / 2 - BBLACK, WINDOWS_Y / 3, WINDOWS_X / 2 + BBLACK, WINDOWS_Y / 3 + BBLACK / 2);
	drawtext("����ģʽ", &r2, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

	RECT r3 = { WINDOWS_X / 2 - BBLACK,WINDOWS_Y / 3 + BBLACK / 2 + bblack,WINDOWS_X / 2 + BBLACK,WINDOWS_Y / 3 + BBLACK + bblack };
	rectangle(WINDOWS_X / 2 - BBLACK, WINDOWS_Y / 3 + BBLACK / 2 + bblack, WINDOWS_X / 2 + BBLACK, WINDOWS_Y / 3 + BBLACK + bblack);
	drawtext("˫��ģʽ", &r3, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

	RECT r4 = { WINDOWS_X / 2 - BBLACK,WINDOWS_Y / 3 + BBLACK + 2 * bblack,WINDOWS_X / 2 + BBLACK,(int)(WINDOWS_Y / 3 + BBLACK*1.5 + 2 * bblack) };
	rectangle(WINDOWS_X / 2 - BBLACK, WINDOWS_Y / 3 + BBLACK + 2 * bblack, WINDOWS_X / 2 + BBLACK, (int)(WINDOWS_Y / 3 + BBLACK*1.5 + 2 * bblack));
	drawtext("����浵", &r4, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

	RECT r5 = { WINDOWS_X / 2 - BBLACK,(int)(WINDOWS_Y / 3 + BBLACK*1.5 + 3 * bblack),WINDOWS_X / 2 + BBLACK,WINDOWS_Y / 3 + BBLACK * 2 + 3 * bblack };
	rectangle(WINDOWS_X / 2 - BBLACK, (int)(WINDOWS_Y / 3 + BBLACK*1.5 + 3 * bblack), WINDOWS_X / 2 + BBLACK, WINDOWS_Y / 3 + BBLACK * 2 + 3 * bblack);
	drawtext("������ս", &r5, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

	RECT r6 = { WINDOWS_X / 2 - BBLACK,WINDOWS_Y / 3 + BBLACK * 2 + 4 * bblack,WINDOWS_X / 2 + BBLACK,(int)(WINDOWS_Y / 3 + BBLACK * 2.5 + 4 * bblack) };
	rectangle(WINDOWS_X / 2 - BBLACK, WINDOWS_Y / 3 + BBLACK * 2 + 4 * bblack, WINDOWS_X / 2 + BBLACK, (int)(WINDOWS_Y / 3 + BBLACK * 2.5 + 4 * bblack));
	drawtext("��սģʽ", &r6, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

	RECT r7 = { WINDOWS_X / 2 - BBLACK,(int)(WINDOWS_Y / 3 + BBLACK * 2.5 + 5 * bblack),WINDOWS_X / 2 + BBLACK,WINDOWS_Y / 3 + BBLACK * 3 + 5 * bblack };
	rectangle(WINDOWS_X / 2 - BBLACK, (int)(WINDOWS_Y / 3 + BBLACK * 2.5 + 5 * bblack), WINDOWS_X / 2 + BBLACK, WINDOWS_Y / 3 + BBLACK * 3 + 5 * bblack);
	drawtext("��Ϸ����", &r7, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

	RECT r8 = { WINDOWS_X / 2 - BBLACK,WINDOWS_Y / 3 + BBLACK * 3 + 6 * bblack,WINDOWS_X / 2 + BBLACK,(int)(WINDOWS_Y / 3 + BBLACK * 3.5 + 6 * bblack) };
	rectangle(WINDOWS_X / 2 - BBLACK, WINDOWS_Y / 3 + BBLACK * 3 + 6 * bblack, WINDOWS_X / 2 + BBLACK, (int)(WINDOWS_Y / 3 + BBLACK * 3.5 + 6 * bblack));
	drawtext("����˵��", &r8, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

	RECT r9 = { WINDOWS_X / 2 - BBLACK,(int)(WINDOWS_Y / 3 + BBLACK * 3.5 + 7 * bblack),WINDOWS_X / 2 + BBLACK,WINDOWS_Y / 3 + BBLACK * 4 + 7 * bblack };
	rectangle(WINDOWS_X / 2 - BBLACK, (int)(WINDOWS_Y / 3 + BBLACK * 3.5 + 7 * bblack), WINDOWS_X / 2 + BBLACK, WINDOWS_Y / 3 + BBLACK * 4 + 7 * bblack);
	drawtext("��    ��", &r9, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

	RECT r10 = { WINDOWS_X / 2 - BBLACK,(int)(WINDOWS_Y / 3 + BBLACK * 4 + 8 * bblack),WINDOWS_X / 2 + BBLACK,(int)(WINDOWS_Y / 3 + BBLACK * 4.5 + 8 * bblack) };
	rectangle(WINDOWS_X / 2 - BBLACK, (int)(WINDOWS_Y / 3 + BBLACK * 4 + 8 * bblack), WINDOWS_X / 2 + BBLACK, (int)(WINDOWS_Y / 3 + BBLACK * 4.5 + 8 * bblack));
	drawtext("�˳���Ϸ", &r10, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

	for (int i = 0; i <= 8; i++)																						//���水ťͼƬ
		getimage(MM + i, WINDOWS_X / 2 - BBLACK, WINDOWS_Y / 3 + i*bblack + i*BBLACK / 2, 2 * BBLACK, BBLACK / 2);
	getimage(MM + 9, WINDOWS_X - BBLACK, WINDOWS_Y - BBLACK / 2, BBLACK - 11, BBLACK / 2 - 11);

	bool _HOME = true, _INTRODUCTION = false, _OPERATION = false, _ABOUT = false, _TOINTERNET = false, _DRMS = false, _SELECTDIFF = false;			//TRUE��ʾ���ڵ�ǰҳ��,selectdiff��ʾ��ѡ���Ѷȣ���ѡ��������ɫ
	MOUSEMSG m;
	while (_HOME)
	{
		timeend = time(NULL);
		if (difftime(timeend, timestart) > 257) {
			mciSendString("stop Music/��������.mp3", NULL, 0, NULL);
			mciSendString("stop Music/���.mp3", NULL, 0, NULL);
			mciSendString("stop Music/���2.mp3", NULL, 0, NULL);
			mciSendString("play Music/��������.mp3", NULL, 0, NULL);
			timestart = time(NULL);
		}
		BeginBatchDraw();
		m = GetMouseMsg();
		switch (m.uMsg)
		{
		case WM_LBUTTONDOWN:												//������������ʱ
			EndBatchDraw();
			mciSendString("stop Music/���.mp3", NULL, 0, NULL);
			//mciSendString("stop Music/����.mp3", NULL, 0, NULL);
			//mciSendString("stop Music/��Ϸ����.mp3", NULL, 0, NULL);
			mciSendString("play Music/��������.mp3", NULL, 0, NULL);
			if (m.x>WINDOWS_X / 2 - BBLACK && m.x<WINDOWS_X / 2 + BBLACK && m.y>WINDOWS_Y / 3 && m.y<WINDOWS_Y / 3 + BBLACK / 2 && _HOME && !_INTRODUCTION && !_OPERATION && !_ABOUT && !_TOINTERNET && !_DRMS && !_SELECTDIFF)//����ģʽ
			{
				mciSendString("play Music/����.wav", NULL, 0, NULL);
				single = true;
				_DRMS = true;									//�뿪HOME����
				cleardevice();
				loadimage(NULL, "Picture/0.jpg");				//����
				rectangle(BBLACK, BBLACK, WINDOWS_X - BBLACK, WINDOWS_Y - BBLACK);

				settextstyle(BBLACK / 2, 0, "����Ҧ��");
				RECT r0 = { 0, 0, WINDOWS_X, WINDOWS_Y / 3 };
				drawtext("ѡ����Ϸ�Ѷ�", &r0, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

				settextstyle(BBLACK / 3, 0, "����Ҧ��");
				RECT r1 = { WINDOWS_X / 2 - BBLACK,WINDOWS_Y / 3,WINDOWS_X / 2 + BBLACK ,WINDOWS_Y / 3 + BBLACK / 2 };
				rectangle(WINDOWS_X / 2 - BBLACK, WINDOWS_Y / 3, WINDOWS_X / 2 + BBLACK, WINDOWS_Y / 3 + BBLACK / 2);
				drawtext("��    ��", &r1, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

				RECT r2 = { WINDOWS_X / 2 - BBLACK,WINDOWS_Y / 3 + BBLACK + 2 * bblack,WINDOWS_X / 2 + BBLACK,(int)(WINDOWS_Y / 3 + BBLACK*1.5 + 2 * bblack) };
				rectangle(WINDOWS_X / 2 - BBLACK, WINDOWS_Y / 3 + BBLACK + 2 * bblack, WINDOWS_X / 2 + BBLACK, (int)(WINDOWS_Y / 3 + BBLACK*1.5 + 2 * bblack));
				drawtext("��    ��", &r2, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

				RECT r3 = { WINDOWS_X / 2 - BBLACK,WINDOWS_Y / 3 + BBLACK * 2 + 4 * bblack,WINDOWS_X / 2 + BBLACK,(int)(WINDOWS_Y / 3 + BBLACK * 2.5 + 4 * bblack) };
				rectangle(WINDOWS_X / 2 - BBLACK, WINDOWS_Y / 3 + BBLACK * 2 + 4 * bblack, WINDOWS_X / 2 + BBLACK, (int)(WINDOWS_Y / 3 + BBLACK * 2.5 + 4 * bblack));
				drawtext("��    ��", &r3, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

				for (int i = 0; i < 3; i++)
				{
					getimage(MB + i, WINDOWS_X / 2 - BBLACK, WINDOWS_Y / 3 + BBLACK*i + 2 * i * bblack, 2 * BBLACK, BBLACK / 2);
				}

				RECT R = { WINDOWS_X - BBLACK,WINDOWS_Y - BBLACK / 2,WINDOWS_X - 10,WINDOWS_Y - 10 };
				rectangle(WINDOWS_X - BBLACK, WINDOWS_Y - BBLACK / 2, WINDOWS_X - 10, WINDOWS_Y - 10);
				drawtext("����", &R, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
				break;
			}
			else if (m.x>WINDOWS_X / 2 - BBLACK && m.x<WINDOWS_X / 2 + BBLACK && m.y>WINDOWS_Y / 3 && m.y<WINDOWS_Y / 3 + BBLACK / 2 && _DRMS && !_SELECTDIFF)			//��
			{
				mciSendString("play Music/����.wav", NULL, 0, NULL);
				_SELECTDIFF = true;
				diff = 1;         //���˼�ģʽ

				cleardevice();
				loadimage(NULL, "Picture/0.jpg");				//����
				rectangle(BBLACK, BBLACK, WINDOWS_X - BBLACK, WINDOWS_Y - BBLACK);

				settextstyle(BBLACK / 2, 0, "����Ҧ��");
				r1 = { 0, 0, WINDOWS_X, WINDOWS_Y / 3 };
				drawtext("ѡ�����ֻ��Ǻ���", &r1, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

				settextstyle(BBLACK / 3, 0, "����Ҧ��");
				r2 = { WINDOWS_X / 2 - 2 * BBLACK,WINDOWS_Y / 3,WINDOWS_X / 2   ,WINDOWS_Y / 3 + BBLACK / 2 };
				rectangle(WINDOWS_X / 2 - 2 * BBLACK, WINDOWS_Y / 3, WINDOWS_X / 2 + BBLACK, WINDOWS_Y / 3 + BBLACK / 2);
				drawtext("��   ��", &r2, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
				setfillcolor(BLACK);
				solidcircle((int)(WINDOWS_X / 2 + BBLACK*0.5), (int)(WINDOWS_Y / 3 + BBLACK / 4), int(BBLACK / 6));

				r3 = { (int)WINDOWS_X / 2 - 2 * BBLACK,(int)WINDOWS_Y / 3 + BBLACK + 2 * bblack,(int)WINDOWS_X / 2  ,(int)(WINDOWS_Y / 3 + BBLACK*1.5 + 2 * bblack) };
				rectangle((int)WINDOWS_X / 2 - 2 * BBLACK, (int)WINDOWS_Y / 3 + BBLACK + 2 * bblack, (int)WINDOWS_X / 2 + BBLACK, (int)(WINDOWS_Y / 3 + BBLACK*1.5 + 2 * bblack));
				drawtext("��   ��", &r3, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
				setfillcolor(WHITE);
				solidcircle((int)(WINDOWS_X / 2 + BBLACK *0.5), (int)(WINDOWS_Y / 3 + BBLACK*1.25 + 2 * bblack), int(BBLACK / 6));

				for (int i = 0; i < 2; i++)
				{
					getimage(MC + i, WINDOWS_X / 2 - 2 * BBLACK, WINDOWS_Y / 3 + BBLACK*i + 2 * i * bblack, 2 * BBLACK, BBLACK / 2);
				}

				RECT R = { WINDOWS_X - BBLACK,WINDOWS_Y - BBLACK / 2,WINDOWS_X - 10,WINDOWS_Y - 10 };
				rectangle(WINDOWS_X - BBLACK, WINDOWS_Y - BBLACK / 2, WINDOWS_X - 10, WINDOWS_Y - 10);
				drawtext("����", &R, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

				break;
			}
			else if (m.x>WINDOWS_X / 2 - BBLACK && m.x<WINDOWS_X / 2 + BBLACK && m.y>WINDOWS_Y / 3 + BBLACK + 2 * bblack && m.y<(int)(WINDOWS_Y / 3 + BBLACK*1.5 + 2 * bblack) && _DRMS && !_SELECTDIFF)			//�е�
			{
				mciSendString("play Music/����.wav", NULL, 0, NULL);
				_SELECTDIFF = true;            //�����е�ģʽ
				diff = 2;
				cleardevice();
				loadimage(NULL, "Picture/0.jpg");				//����
				rectangle(BBLACK, BBLACK, WINDOWS_X - BBLACK, WINDOWS_Y - BBLACK);

				settextstyle(BBLACK / 2, 0, "����Ҧ��");
				r1 = { 0, 0, WINDOWS_X, WINDOWS_Y / 3 };
				drawtext("ѡ�����ֻ��Ǻ���", &r1, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

				settextstyle(BBLACK / 3, 0, "����Ҧ��");
				r2 = { WINDOWS_X / 2 - 2 * BBLACK,WINDOWS_Y / 3,WINDOWS_X / 2  ,WINDOWS_Y / 3 + BBLACK / 2 };
				rectangle(WINDOWS_X / 2 - 2 * BBLACK, WINDOWS_Y / 3, WINDOWS_X / 2 + BBLACK, WINDOWS_Y / 3 + BBLACK / 2);
				drawtext("��   ��", &r2, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
				setfillcolor(BLACK);
				solidcircle(WINDOWS_X / 2 + BBLACK / 2, WINDOWS_Y / 3 + BBLACK / 4, int(BBLACK / 6));

				r3 = { WINDOWS_X / 2 - 2 * BBLACK,WINDOWS_Y / 3 + BBLACK + 2 * bblack,WINDOWS_X / 2 ,(int)(WINDOWS_Y / 3 + BBLACK*1.5 + 2 * bblack) };
				rectangle(WINDOWS_X / 2 - 2 * BBLACK, WINDOWS_Y / 3 + BBLACK + 2 * bblack, WINDOWS_X / 2 + BBLACK, (int)(WINDOWS_Y / 3 + BBLACK*1.5 + 2 * bblack));
				drawtext("��   ��", &r3, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
				setfillcolor(WHITE);
				solidcircle((int)(WINDOWS_X / 2 + BBLACK / 2), (int)(WINDOWS_Y / 3 + BBLACK*1.25 + 2 * bblack), int(BBLACK / 6));

				for (int i = 0; i < 2; i++)
				{
					getimage(MC + i, WINDOWS_X / 2 - 2 * BBLACK, WINDOWS_Y / 3 + BBLACK*i + 2 * i * bblack, 2 * BBLACK, BBLACK / 2);
				}

				RECT R = { WINDOWS_X - BBLACK,WINDOWS_Y - BBLACK / 2,WINDOWS_X - 10,WINDOWS_Y - 10 };
				rectangle(WINDOWS_X - BBLACK, WINDOWS_Y - BBLACK / 2, WINDOWS_X - 10, WINDOWS_Y - 10);
				drawtext("����", &R, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

				break;
			}
			else if (m.x>WINDOWS_X / 2 - BBLACK && m.x<WINDOWS_X / 2 + BBLACK && m.y> WINDOWS_Y / 3 + BBLACK * 2 + 4 * bblack && m.y<(int)(WINDOWS_Y / 3 + BBLACK * 2.5 + 4 * bblack) && _DRMS && !_SELECTDIFF)			//����
			{
				mciSendString("play Music/����.wav", NULL, 0, NULL);
				_SELECTDIFF = true;
				diff = 3;            //��������ģʽ
				cleardevice();
				zobrist_init();

				loadimage(NULL, "Picture/0.jpg");				//����
				rectangle(BBLACK, BBLACK, WINDOWS_X - BBLACK, WINDOWS_Y - BBLACK);

				settextstyle(BBLACK / 2, 0, "����Ҧ��");
				r1 = { 0, 0, WINDOWS_X, WINDOWS_Y / 3 };
				drawtext("ѡ�����ֻ��Ǻ���", &r1, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

				settextstyle(BBLACK / 3, 0, "����Ҧ��");
				r2 = { WINDOWS_X / 2 - 2 * BBLACK,WINDOWS_Y / 3,WINDOWS_X / 2  ,WINDOWS_Y / 3 + BBLACK / 2 };
				rectangle(WINDOWS_X / 2 - 2 * BBLACK, WINDOWS_Y / 3, WINDOWS_X / 2 + BBLACK, WINDOWS_Y / 3 + BBLACK / 2);
				drawtext("��   ��", &r2, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
				setfillcolor(BLACK);
				solidcircle(WINDOWS_X / 2 + BBLACK / 2, WINDOWS_Y / 3 + BBLACK / 4, int(BBLACK / 6));

				r3 = { WINDOWS_X / 2 - 2 * BBLACK,WINDOWS_Y / 3 + BBLACK + 2 * bblack,WINDOWS_X / 2 ,(int)(WINDOWS_Y / 3 + BBLACK*1.5 + 2 * bblack) };
				rectangle(WINDOWS_X / 2 - 2 * BBLACK, WINDOWS_Y / 3 + BBLACK + 2 * bblack, WINDOWS_X / 2 + BBLACK, (int)(WINDOWS_Y / 3 + BBLACK*1.5 + 2 * bblack));
				drawtext("��   ��", &r3, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
				setfillcolor(WHITE);
				solidcircle((int)(WINDOWS_X / 2 + BBLACK / 2), (int)(WINDOWS_Y / 3 + BBLACK*1.25 + 2 * bblack), int(BBLACK / 6));

				for (int i = 0; i < 2; i++)
				{
					getimage(MC + i, WINDOWS_X / 2 - 2 * BBLACK, WINDOWS_Y / 3 + BBLACK*i + 2 * i * bblack, 2 * BBLACK, BBLACK / 2);
				}

				RECT R = { WINDOWS_X - BBLACK,WINDOWS_Y - BBLACK / 2,WINDOWS_X - 10,WINDOWS_Y - 10 };
				rectangle(WINDOWS_X - BBLACK, WINDOWS_Y - BBLACK / 2, WINDOWS_X - 10, WINDOWS_Y - 10);
				drawtext("����", &R, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

				break;
			}
			else if (m.x>WINDOWS_X / 2 - 2 * BBLACK && m.x<WINDOWS_X / 2 + BBLACK && m.y>WINDOWS_Y / 3 && m.y<(int)(WINDOWS_Y / 3 + BBLACK / 2) && _DRMS && _SELECTDIFF)			//ѡ������
			{
				mciSendString("play Music/����.wav", NULL, 0, NULL);
				MYCOLOR = black;
				_HOME = false;
				cleardevice();
				init();
				if (diff == 1) STARTVS(black, MOUSE, Easy);
				else if (diff == 2) STARTVS(black, MOUSE, Middle);
				else  STARTVS(black, MOUSE, Difficult);
				break;
			}
			else if (m.x>WINDOWS_X / 2 - 2 * BBLACK && m.x<WINDOWS_X / 2 + BBLACK && m.y>WINDOWS_Y / 3 + BBLACK + 2 * bblack && m.y<(int)(WINDOWS_Y / 3 + BBLACK*1.5 + 2 * bblack) && _DRMS && _SELECTDIFF)			//ѡ�����
			{
				mciSendString("play Music/����.wav", NULL, 0, NULL);
				MYCOLOR = white;
				_HOME = false;
				cleardevice();
				init();
				if (diff == 1) STARTVS(white, MOUSE, Easy);
				else if (diff == 2) STARTVS(white, MOUSE, Middle);
				else  STARTVS(white, MOUSE, Difficult);
				break;
			}
			else if (m.x>WINDOWS_X / 2 - BBLACK && m.x<WINDOWS_X / 2 + BBLACK && m.y>WINDOWS_Y / 3 + BBLACK / 2 + bblack && m.y<WINDOWS_Y / 3 + BBLACK + bblack && _HOME && !_INTRODUCTION && !_OPERATION && !_ABOUT && !_TOINTERNET && !_DRMS && !_SELECTDIFF)//˫��ģʽ
			{
				mciSendString("play Music/����.wav", NULL, 0, NULL);
				single = false;
				_HOME = false;									//�뿪HOME����
				init();
				STARTVS(1, MOUSE, MOUSE);							//˫��ģʽ
				break;
			}
			else if (m.x>WINDOWS_X / 2 - BBLACK && m.x<WINDOWS_X / 2 + BBLACK && m.y>WINDOWS_Y / 3 + BBLACK + 2 * bblack && m.y<(int)(WINDOWS_Y / 3 + BBLACK*1.5 + 2 * bblack) && _HOME && !_OPERATION && !_ABOUT && !_INTRODUCTION && !_TOINTERNET && !_DRMS && !_SELECTDIFF)   //������Ϸ
			{
				mciSendString("play Music/����.wav", NULL, 0, NULL);
				load_game();
				_HOME = false;
				if (!single) STARTVS(MYCOLOR, MOUSE, MOUSE);
				else {
					if (diff == 1) STARTVS(MYCOLOR, MOUSE, Easy);
					else if (diff == 2) STARTVS(MYCOLOR, MOUSE, Middle);
					else  STARTVS(MYCOLOR, MOUSE, Difficult);
				}
				break;
			}
			else if (m.x>WINDOWS_X / 2 - BBLACK && m.x<WINDOWS_X / 2 + BBLACK && m.y>(int)(WINDOWS_Y / 3 + BBLACK*1.5 + 3 * bblack) && m.y<WINDOWS_Y / 3 + BBLACK * 2 + 3 * bblack && _HOME && !_OPERATION && !_ABOUT && !_INTRODUCTION && !_TOINTERNET && !_DRMS && !_SELECTDIFF)//������ս
			{
				mciSendString("play Music/����.wav", NULL, 0, NULL);
				single = false;
				_TOINTERNET = true;
				cleardevice();
				loadimage(NULL, "PICTURE/0.jpg");				//����
				rectangle(BBLACK, BBLACK, WINDOWS_X - BBLACK, WINDOWS_Y - BBLACK);
				RECT R1 = { WINDOWS_X / 4, WINDOWS_Y / 2 + BBLACK, WINDOWS_X / 4 + 2 * BBLACK , WINDOWS_Y / 2 + 2 * BBLACK };
				RECT R3 = { WINDOWS_X - WINDOWS_X / 4 - 2 * BBLACK, WINDOWS_Y / 2 + BBLACK, WINDOWS_X - WINDOWS_X / 4, WINDOWS_Y / 2 + 2 * BBLACK };
				rectangle(WINDOWS_X / 4, WINDOWS_Y / 2 + BBLACK, WINDOWS_X / 4 + 2 * BBLACK, WINDOWS_Y / 2 + 2 * BBLACK);
				rectangle(WINDOWS_X - WINDOWS_X / 4 - 2 * BBLACK, WINDOWS_Y / 2 + BBLACK, WINDOWS_X - WINDOWS_X / 4, WINDOWS_Y / 2 + 2 * BBLACK);
				drawtext("��Ҫ����", &R1, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
				drawtext("��Ҫ����", &R3, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
				getimage(MM + 9, WINDOWS_X / 4, WINDOWS_Y / 2 + BBLACK, 2 * BBLACK, BBLACK);
				getimage(MM + 10, WINDOWS_X - WINDOWS_X / 4 - 2 * BBLACK, WINDOWS_Y / 2 + BBLACK, 2 * BBLACK, BBLACK);

				outtextxy(WINDOWS_X / 3 - 50, 150, "����˵����");
				outtextxy(WINDOWS_X / 3, 185, "1���������Ҫ����������������");
				outtextxy(WINDOWS_X / 3, 220, "2����������ѽ��롰��Ҫ���ӡ�");
				outtextxy(WINDOWS_X / 3, 255, "3�����������ip��ַ");
				outtextxy(WINDOWS_X / 3, 290, "4��������Ϸ");
				outtextxy(WINDOWS_X / 3, 325, "*��ȷ��������ͬһ����������Ŷ~");
				RECT R2 = { WINDOWS_X - BBLACK,WINDOWS_Y - BBLACK / 2,WINDOWS_X - 10,WINDOWS_Y - 10 };
				rectangle(WINDOWS_X - BBLACK, WINDOWS_Y - BBLACK / 2, WINDOWS_X - 10, WINDOWS_Y - 10);
				drawtext("����", &R2, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
				break;
			}
			else if (m.x>WINDOWS_X / 4 && m.x<WINDOWS_X / 4 + 2 * BBLACK  && m.y>WINDOWS_Y / 2 + BBLACK && m.y<WINDOWS_Y / 2 + 2 * BBLACK && _TOINTERNET)//��Ҫ����
			{
				mciSendString("play Music/����.wav", NULL, 0, NULL);
				TOINTERNET = true;
				cleardevice();
				loadimage(NULL, "Picture/0.jpg");				//����
				rectangle(BBLACK, BBLACK, WINDOWS_X - BBLACK, WINDOWS_Y - BBLACK);
				TOI(true);								//����������
				init();
				STARTVS(white, OURCLASS, MOUSE);
				break;
			}
			else if (m.x>WINDOWS_X - WINDOWS_X / 4 - 2 * BBLACK && m.x<WINDOWS_X - WINDOWS_X / 4 && m.y>WINDOWS_Y / 2 + BBLACK && m.y<WINDOWS_Y / 2 + 2 * BBLACK && _TOINTERNET)//��Ҫ����
			{
				mciSendString("play Music/����.wav", NULL, 0, NULL);
				TOINTERNET = true;
				cleardevice();
				loadimage(NULL, "Picture/0.jpg");				//����
				rectangle(BBLACK, BBLACK, WINDOWS_X - BBLACK, WINDOWS_Y - BBLACK);
				TOI(false);								//���ӷ�����
				init();
				STARTVS(black, MOUSE, OURCLASS);
				break;
			}
			else if (m.x>WINDOWS_X / 2 - BBLACK && m.x<WINDOWS_X / 2 + BBLACK && m.y> WINDOWS_Y / 3 + BBLACK * 2 + 4 * bblack && m.y<(int)(WINDOWS_Y / 3 + BBLACK * 2.5 + 4 * bblack) && _HOME && !_OPERATION && !_ABOUT && !_INTRODUCTION && !_TOINTERNET && !_DRMS && !_SELECTDIFF)		//��սģʽ
			{
				mciSendString("play Music/����.wav", NULL, 0, NULL);
				single = false;
				_HOME = false;										//�뿪HOME����
				init();
				zobrist_init();
				STARTVS(-1, Difficult, Difficult2);								//��սģʽ
				break;
			}
			else if (m.x>WINDOWS_X / 2 - BBLACK && m.x<WINDOWS_X / 2 + BBLACK && m.y>(int)(WINDOWS_Y / 3 + BBLACK * 2.5 + 5 * bblack) && m.y<WINDOWS_Y / 3 + BBLACK * 3 + 5 * bblack && _HOME && !_INTRODUCTION && !_ABOUT && !_OPERATION && !_TOINTERNET && !_DRMS && !_SELECTDIFF)	//��Ϸ����
			{
				mciSendString("play Music/����.wav", NULL, 0, NULL);
				//mciSendString("stop Music/��������.mp3", NULL, 0, NULL);
				//mciSendString("play Music/��Ϸ����.mp3", NULL, 0, NULL);
				_INTRODUCTION = true;
				cleardevice();
				loadimage(NULL, "Picture/0.jpg");				//����
				string data[16] = {
					"��Ϸ���ܣ�" ,
					"���й��䣬ҹ����ɽ��",
					"����ţ���Σ��׷������",
					"������Ӳ�,�׸�ƽ��־",
					"���½԰� Ψ�Ҷ���",
					"����Ϳ̿ ��֮����",
					"ī�ž��� �˶�����",
					"�˺���� �氮ƽ��",
					"ī�����ŷǹ��氮 Ҫ���ʤ��",
					"����һ��Ҫͨ��ɱ¾ ����Ϊ�� ����Ϊ��",
					"��ת��� ������Ȼ����",
					"���ǵ���˫����ת�� ȴ��ǧ���� ��������",
				};

				rectangle(BBLACK, BBLACK, WINDOWS_X - BBLACK, WINDOWS_Y - BBLACK);
				settextstyle(BBLACK / 2, 0, "����Ҧ��");
				settextcolor(RGB(0, 255, 255));
				outtextxy(WINDOWS_X / 3 - 100, 90, data[0].data());
				settextstyle(BBLACK / 3, 0, "����");
				settextcolor(WHITE);
				int LEFT, TOP = 115;
				for (int i = 1; i < 16; i++)
				{
					LEFT = WINDOWS_X / 3;
					//if (i == 5 || i >= 10)LEFT -= 25;
					//if (i == 5 || i == 10 || i == 6)TOP += 30;
					outtextxy(LEFT, TOP + 35 * i, data[i].data());
				}
				RECT R1 = { WINDOWS_X - BBLACK,WINDOWS_Y - BBLACK / 2,WINDOWS_X - 10,WINDOWS_Y - 10 };
				rectangle(WINDOWS_X - BBLACK, WINDOWS_Y - BBLACK / 2, WINDOWS_X - 10, WINDOWS_Y - 10);
				drawtext("����", &R1, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
				break;
			}
			else if (m.x>WINDOWS_X / 2 - BBLACK && m.x<WINDOWS_X / 2 + BBLACK && m.y>WINDOWS_Y / 3 + BBLACK * 3 + 6 * bblack && m.y < (int)(WINDOWS_Y / 3 + BBLACK * 3.5 + 6 * bblack) && _HOME && !_INTRODUCTION && !_OPERATION && !_ABOUT && !_TOINTERNET && !_DRMS && !_SELECTDIFF)	//����˵��
			{
				mciSendString("play Music/����.wav", NULL, 0, NULL);
				//mciSendString("stop Music/��������.mp3", NULL, 0, NULL);
				//mciSendString("play Music/��Ϸ����.mp3", NULL, 0, NULL);
				_OPERATION = true;
				cleardevice();
				loadimage(NULL, "Picture/0.jpg");				//����
				string data[16] = {
					"����˵����" ,
					"��ת������Χ�����ԵĲ�ͬ����",
					"��ת����в������κ����ӱ�ɱ��",
					"��ֿ�ʼʱ����λ��e4��d5������λ��d4��e5��",
					"�ڷ����У�˫���������塣",
					"һ���Ϸ����岽��������һ���ո�������һ�����ӣ����ҷ�ת����һ���������ӡ�",
					"�����µ����������������е�ͬɫ���Ӽ䣬�Է�����ס���������Ӷ�Ҫ��ת������",
					"�����Ǻ��żУ����żУ�����б�żС���ס��λ���ϱ���ȫ���Ƕ��ֵ����ӣ������пո�",
					"һ������������������Ϸ��壬�κα���ס�����Ӷ����뱻��ת������������Ȩѡ��ȥ��ĳ�����ӡ�",
					"�������ٷ�ת�˶��ֵ�һ�����ӣ�����Ͳ������ӡ�",
					"���һ��û�кϷ��岽��Ҳ����˵�������µ�������������ٷ�ת���ֵ�һ�����ӣ�������һ��ֻ����Ȩ��",
					"���һ��������һ���Ϸ��岽���£����ͱ������ӣ�������Ȩ��",
					"��ֳ�����ȥ��ֱ��������������˫�����޺Ϸ��岽���¡�",
				};

				rectangle(BBLACK, BBLACK, WINDOWS_X - BBLACK, WINDOWS_Y - BBLACK);
				settextstyle(BBLACK / 2, 0, "����Ҧ��");
				settextcolor(RGB(0, 255, 255));
				outtextxy(WINDOWS_X / 8 - 50, 90, data[0].data());
				settextstyle(BBLACK / 3, 0, "����Ҧ��");
				settextcolor(WHITE);
				int LEFT, TOP = 115;
				for (int i = 1; i < 16; i++)
				{
					LEFT = WINDOWS_X / 8;
					//if (i == 1)LEFT -= 50;
					//if (i == 6 || i == 10)LEFT -= 25;
					//if (i == 12)TOP += 30;
					outtextxy(LEFT, TOP + 35 * i, data[i].data());
				}
				RECT R3 = { WINDOWS_X - BBLACK,WINDOWS_Y - BBLACK / 2,WINDOWS_X - 10,WINDOWS_Y - 10 };
				rectangle(WINDOWS_X - BBLACK, WINDOWS_Y - BBLACK / 2, WINDOWS_X - 10, WINDOWS_Y - 10);
				drawtext("����", &R3, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
				break;
			}
			else if (m.x > WINDOWS_X / 2 - BBLACK && m.x<WINDOWS_X / 2 + BBLACK && m.y>(int)(WINDOWS_Y / 3 + BBLACK * 3.5 + 7 * bblack) && m.y < (int)(WINDOWS_Y / 3 + BBLACK * 4 + 7 * bblack) && _HOME && !_INTRODUCTION && !_OPERATION && !_ABOUT && !_TOINTERNET && !_DRMS && !_SELECTDIFF)//����
			{
				mciSendString("play Music/����.wav", NULL, 0, NULL);
				//mciSendString("stop Music/��������.mp3", NULL, 0, NULL);
				//mciSendString("play Music/����.mp3", NULL, 0, NULL);
				_ABOUT = true;
				cleardevice();
				loadimage(NULL, "Picture/0.jpg");				//����
				string data[16] = {
					"����:" ,
					"�о������������ҳ����˶���������ֵ���Ӵ��",
					"������������ķ˹�����޵����漶��������ϵ֮���ﵮ����",
					"��ʵ��Ҳ��֪��������д��ʲôo(�s���t)o",
					"�����дд�ҵ�ϲ���ļ����˵Ķ��Ӱ�",
					"���й��䣬ҹ����ɽ��",
					"����˶�:��÷��������ʶ���������Ҿ���һ��ʺ��",
					"��˹������NP���⣬��Ϊ���ڵ�һ�ν��һ������ʱ�õľ������Žⷨ",
					"������ĵط��ͻ�����Ӱ����������Ӱ�ĵط�Ҳһ����������",
					"������һ�� ôô��(*�R���Q*)",
				};

				rectangle(BBLACK, BBLACK, WINDOWS_X - BBLACK, WINDOWS_Y - BBLACK);
				settextstyle(BBLACK / 2, 0, "����Ҧ��");
				settextcolor(RGB(0, 255, 255));
				outtextxy(WINDOWS_X / 3 - 100, 90, data[0].data());
				settextstyle(BBLACK / 3, 0, "����");
				settextcolor(WHITE);
				int LEFT, TOP = 115;
				for (int i = 1; i < 12; i++)
				{
					LEFT = WINDOWS_X / 4;
					//if (i == 1 || i == 9)LEFT -= 50;
					//else if (i == 2 || i == 8)LEFT -= 25;
					//if (i == 9)TOP += 30;
					outtextxy(LEFT, TOP + 35 * i, data[i].data());
				}
				RECT R3 = { WINDOWS_X - BBLACK,WINDOWS_Y - BBLACK / 2,WINDOWS_X - 10,WINDOWS_Y - 10 };
				rectangle(WINDOWS_X - BBLACK, WINDOWS_Y - BBLACK / 2, WINDOWS_X - 10, WINDOWS_Y - 10);
				drawtext("����", &R3, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
				settextstyle(BBLACK / 4, 0, "΢���ź�");
				string author[5] = {
					"Author: Walle",
					"Github: https://github.com/Walleclipse"
				};
				for (int i = 0; i < 5; i++)
				{
					outtextxy(5 * BBLACK / 4, WINDOWS_Y - (5 - i) * BBLACK / 2, author[i].data());
				}
				settextstyle(BBLACK / 4, 0, "����Ҧ��");
				break;
			}
			else if (m.x>WINDOWS_X - BBLACK && m.x<WINDOWS_X - 10 && m.y>WINDOWS_Y - BBLACK / 2 && m.y<WINDOWS_Y - 10 && (_INTRODUCTION || _OPERATION || _ABOUT || _TOINTERNET || _DRMS))					//����
			{
				cleardevice();
				_HOME = false, _INTRODUCTION = false, _OPERATION = false, _ABOUT = false, _TOINTERNET = false, _DRMS = false;
				gameStart();
			}
			else if (m.x>WINDOWS_X / 2 - BBLACK && m.x<WINDOWS_X / 2 + BBLACK && m.y>(int)(WINDOWS_Y / 3 + BBLACK * 4 + 8 * bblack) && m.y<WINDOWS_Y / 3 + BBLACK * 4.5 + 8 * bblack && _HOME && !_INTRODUCTION && !_OPERATION && !_ABOUT && !_TOINTERNET && !_DRMS && !_SELECTDIFF)//�˳���Ϸ
			{
				exit(0);
			}
			else break;
		case WM_MOUSEMOVE:									//�ƶ����
			RECT r;
			if (_INTRODUCTION || _OPERATION || _ABOUT || _TOINTERNET || _DRMS)				//�����ǰ������Ϸ���� ����˵�� ���߹��ڽ��� ����������ս���� ���ߵ���ģʽ����
			{
				if (ESCEXIT)gameStart();							//���ֽ��水ESC�˳�
				if (m.x>WINDOWS_X - BBLACK && m.x<WINDOWS_X - 10 && m.y>WINDOWS_Y - BBLACK / 2 && m.y<WINDOWS_Y - 10)
				{
					r.left = WINDOWS_X - BBLACK;
					r.top = WINDOWS_Y - BBLACK / 2;
					r.right = WINDOWS_X - 10;
					r.bottom = WINDOWS_Y - 10;
					POINT points[8] = { { r.left,r.top },{ r.right,r.top },{ r.right,r.bottom },{ r.left,r.bottom } };
					setfillcolor(RED);
					fillpolygon(points, 4);
					setbkmode(TRANSPARENT);
					drawtext("����", &r, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
				}
				else
				{
					if (getpixel(WINDOWS_X - BBLACK + 1, WINDOWS_Y - BBLACK / 2 + 1) == RED)
					{
						putimage(WINDOWS_X - BBLACK + 1, WINDOWS_Y - BBLACK / 2 + 1, MM + 8);
						setbkmode(TRANSPARENT);
						drawtext("����", &r, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
					}
				}
				if (_DRMS && !_SELECTDIFF)
				{
					for (int i = 0; i < 3; i++)
					{
						if (m.x > WINDOWS_X / 2 - BBLACK && m.x<WINDOWS_X / 2 + BBLACK && m.y>WINDOWS_Y / 3 + BBLACK*i + 2 * i * bblack && m.y < WINDOWS_Y / 3 + BBLACK*i + 2 * i * bblack + BBLACK / 2)
						{
							r.left = WINDOWS_X / 2 - BBLACK;
							r.top = WINDOWS_Y / 3 + BBLACK*i + 2 * i * bblack;
							r.right = WINDOWS_X / 2 + BBLACK;
							r.bottom = WINDOWS_Y / 3 + BBLACK*i + 2 * i * bblack + BBLACK / 2;
							POINT points[8] = { { r.left,r.top },{ r.right,r.top },{ r.right,r.bottom },{ r.left,r.bottom } };
							setfillcolor(RED);
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
							if (getpixel(WINDOWS_X / 2 - BBLACK + 1, WINDOWS_Y / 3 + BBLACK*i + 2 * i * bblack + 1) == RED)
							{
								putimage(WINDOWS_X / 2 - BBLACK, WINDOWS_Y / 3 + BBLACK*i + 2 * i * bblack, MB + i);	//���ԭ��ͼƬ
							}
						}
					}
				}
				if (_SELECTDIFF)
				{
					for (int i = 0; i < 2; i++)
					{
						if (m.x > WINDOWS_X / 2 - 2 * BBLACK && m.x<WINDOWS_X / 2 + BBLACK && m.y>WINDOWS_Y / 3 + BBLACK*i + 2 * i * bblack && m.y < WINDOWS_Y / 3 + BBLACK*i + 2 * i * bblack + BBLACK / 2)
						{
							r.left = WINDOWS_X / 2 - 2 * BBLACK;
							r.top = WINDOWS_Y / 3 + BBLACK*i + 2 * i * bblack;
							r.right = WINDOWS_X / 2;
							r.bottom = WINDOWS_Y / 3 + BBLACK*i + 2 * i * bblack + BBLACK / 2;
							POINT points[8] = { { r.left,r.top },{ r.right,r.top },{ r.right,r.bottom },{ r.left,r.bottom } };
							setfillcolor(RED);
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

							}
						}
						else
						{
							if (getpixel(WINDOWS_X / 2 - 2 * BBLACK + 1, WINDOWS_Y / 3 + BBLACK*i + 2 * i * bblack + 1) == RED)
							{
								putimage(WINDOWS_X / 2 - 2 * BBLACK, WINDOWS_Y / 3 + BBLACK*i + 2 * i * bblack, MC + i);	//���ԭ��ͼƬ

							}
						}
					}
				}
				if (_TOINTERNET)											//����������
				{
					if (m.x > WINDOWS_X / 4 && m.x<WINDOWS_X / 4 + 2 * BBLACK  && m.y>WINDOWS_Y / 2 + BBLACK && m.y < WINDOWS_Y / 2 + 2 * BBLACK)
					{
						r.left = WINDOWS_X / 4;
						r.top = WINDOWS_Y / 2 + BBLACK;
						r.right = WINDOWS_X / 4 + 2 * BBLACK;
						r.bottom = WINDOWS_Y / 2 + 2 * BBLACK;
						POINT points[8] = { { r.left,r.top },{ r.right,r.top },{ r.right,r.bottom },{ r.left,r.bottom } };
						setfillcolor(RED);
						fillpolygon(points, 4);
						setbkmode(TRANSPARENT);
						drawtext("��Ҫ����", &r, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
					}
					else
					{
						if (getpixel(WINDOWS_X / 4 + 1, WINDOWS_Y / 2 + BBLACK + 1) == RED)
						{
							putimage(WINDOWS_X / 4, WINDOWS_Y / 2 + BBLACK, MM + 9);
						}
					}
					if (m.x>WINDOWS_X - WINDOWS_X / 4 - 2 * BBLACK && m.x<WINDOWS_X - WINDOWS_X / 4 && m.y>WINDOWS_Y / 2 + BBLACK && m.y<WINDOWS_Y / 2 + 2 * BBLACK)
					{
						r.left = WINDOWS_X - WINDOWS_X / 4 - 2 * BBLACK;
						r.top = WINDOWS_Y / 2 + BBLACK;
						r.right = WINDOWS_X - WINDOWS_X / 4;
						r.bottom = WINDOWS_Y / 2 + 2 * BBLACK;
						POINT points[8] = { { r.left,r.top },{ r.right,r.top },{ r.right,r.bottom },{ r.left,r.bottom } };
						setfillcolor(RED);
						fillpolygon(points, 4);
						setbkmode(TRANSPARENT);
						drawtext("��Ҫ����", &r, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
					}
					else
					{
						if (getpixel(WINDOWS_X - WINDOWS_X / 4 - 2 * BBLACK + 1, WINDOWS_Y / 2 + BBLACK + 1) == RED)
						{
							putimage(WINDOWS_X - WINDOWS_X / 4 - 2 * BBLACK, WINDOWS_Y / 2 + BBLACK, MM + 10);
						}
					}
				}
			}
			else
			{
				for (int i = 0; i <= 8; i++)
				{
					if (m.x>WINDOWS_X / 2 - BBLACK && m.x<WINDOWS_X / 2 + BBLACK && m.y>WINDOWS_Y / 3 + i*bblack + i*BBLACK / 2 && m.y<WINDOWS_Y / 3 + i*bblack + i*BBLACK / 2 + BBLACK / 2)
					{
						r.left = WINDOWS_X / 2 - BBLACK;
						r.top = WINDOWS_Y / 3 + i*bblack + i*BBLACK / 2;
						r.right = WINDOWS_X / 2 + BBLACK;
						r.bottom = WINDOWS_Y / 3 + i*bblack + i*BBLACK / 2 + BBLACK / 2;
						POINT points[8] = { { r.left,r.top },{ r.right,r.top },{ r.right,r.bottom },{ r.left,r.bottom } };
						setfillcolor(RED);
						fillpolygon(points, 4);
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
							drawtext("����浵", &r, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
							break;
						case 3:
							drawtext("������ս", &r, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
							break;
						case 4:
							drawtext("��սģʽ", &r, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
							break;
						case 5:
							drawtext("��Ϸ����", &r, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
							break;
						case 6:
							drawtext("����˵��", &r, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
							break;
						case 7:
							drawtext("��    ��", &r, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
							break;
						case 8:
							drawtext("�˳���Ϸ", &r, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
							break;
						}
					}
					else
					{
						if (getpixel(WINDOWS_X / 2 - BBLACK + 1, WINDOWS_Y / 3 + i*bblack + i*BBLACK / 2 + 1) == RED)
						{
							putimage(WINDOWS_X / 2 - BBLACK, WINDOWS_Y / 3 + i*bblack + i*BBLACK / 2, MM + i);	//���ԭ��ͼƬ
						}
					}
				}
			}
			FlushBatchDraw();
			break;
		default: break;
		}
	}
	gameStart();
}

int main()
{
	gameStart();					//������
	closegraph();					//�ر�ͼ�λ�����
	return 0;
}
