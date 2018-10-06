///////////////////////////////////////////////////////////////////////////////
// 程序名称：黑白棋
// 编译环境：Visual Studio 2017
// 程序编写：Abduwali
// 最后更新：2018-3-29
//
////////////////////////////////////////////////////////////////////////////////
//说明：根据“千千”的“墨攻棋阵”做了一些修改,强化了AI、加入了悔棋保存、背景音乐等功能，感谢超级可爱的千千的源代码
//超级可爱的千千的源代码网址：https://github.com/im0qianqian/
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

#define WINDOWS_X 1200	//窗口大小X
#define WINDOWS_Y 650	//窗口大小Y

#define PX 280			//棋盘偏移量X
#define PY 80			//棋盘偏移量Y
#define BBLACK 70		//空格大小

#define CHESSSIZE 20	//棋子大小
#define SIZE 8			//棋盘格数

#define ESC 27
#define ESCEXIT (_kbhit()&&_getch()==ESC)

#pragma comment (lib,"ws2_32.lib")						// 引用 Windows Multimedia API
#pragma comment(lib,"Winmm.lib")

const int black = 1;	//黑棋
const int white = 0;	//白棋
int step = 0;  //回合数
int diff = 1; //难度
int whitecount = 2;  //黑白棋得分
int blackcount = 2;
bool single = true; //单人模式
const int timeout_turn = 3000;		//每步棋的思考时间
clock_t start;					//AI开始思考的时间点
bool stopThink = false;			//AI是否停止思考
const int MAX_DEPTH = 20, MIN_DEPTH = 4;

int mapp[SIZE][SIZE] = {};		//棋盘矩阵存储
int historymap[100][SIZE][SIZE] = {};
const int MOVE[8][2] = { { -1, 0 },{ 1, 0 },{ 0, -1 },{ 0, 1 },{ -1, -1 },{ 1, -1 },{ 1, 1 },{ -1, 1 } };	//方位
const int MAPPOINTCOUNT[8][8] =												//棋盘各点权值估值
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

bool TOINTERNET = false;						//是否有网络数据
int MYCOLOR = 1;							//我的颜色 1黑色 0白色 -1其他
int NOWCOLOR = 1;							//当前执子颜色
bool TIANEYES = false;							//天眼模式

class POINT2
{
public:
	void WIN2MAP(POINT2 &MAP)			//建立棋盘与矩阵的映射关系
	{
		MAP.x = (x - PX) / BBLACK;
		MAP.y = (y - PY) / BBLACK;
	}
	void MAP2WIN(POINT2 &WIN)			//建立矩阵与棋盘的映射关系
	{
		WIN.x = PX + x*BBLACK + BBLACK / 2;
		WIN.y = PY + y*BBLACK + BBLACK / 2;
	}
	void INIT(int x, int y)				//插入元素
	{
		this->x = x;
		this->y = y;
	}
	void ADD(int x, int y)				//计算和
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

POINT2 LASTCH;								//上一步棋子方位

void gameStart();						//函数声明
void STARTVS(int YOURCOLOR, POINT2 P1(), POINT2 P2());

void mappadd(int x, int y, int color, int MAP[SIZE][SIZE])		//向地图中添加棋子
{
	POINT2 WINDOWS2, MAP2;
	WINDOWS2.INIT(x, y);
	WINDOWS2.WIN2MAP(MAP2);
	MAP[MAP2.x][MAP2.y] = color ? 1 : -1;
}

void printcircle(int x, int y, int color, int MAP[SIZE][SIZE])				//绘制棋子
{
	mappadd(x, y, color, MAP);
	//注释中为特效二
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
	POINT points1[4] = { { ZS.x,ZS.y },{ ZZS.x,ZZS.y },{ ZZX.x,ZZX.y },{ ZX.x,ZX.y }};			//定义点
	POINT points2[4] = { { YS.x,YS.y },{ ZYS.x,ZYS.y },{ ZYX.x,ZYX.y },{ YX.x,YX.y }};			//定一点
	setfillcolor(LIGHTRED);																		//翻转背景颜色
	putimage(ZS.x - 1, ZS.y - 1, &MAPIMAGE[X.x][X.y]);											//使用原图覆盖
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
	setfillcolor(color ? BLACK : WHITE);					//从中间放大变换
	for (int i = 0; i <= CHESSSIZE; ++i)
	{
		solidcircle(x, y, i);
		Sleep(1);
	}
}

void init()															//绘制棋盘
{
	memset(mapp, 0, sizeof(mapp));									//初始化
	memset(historymap, 0, sizeof(historymap));
	memset(expect, 0, sizeof(expect));
	TIANEYES = false;												//关闭天眼模式
	LASTCH.INIT(0, 0);
	step = 1;
	whitecount = 2;
	blackcount = 2;
	NOWCOLOR = 1;

	settextstyle(15, 0, "宋体");
	loadimage(NULL, "Picture/1.jpg");										//背景图片
	for (int x = PX; x < PX + BBLACK*(SIZE + 1); x += BBLACK)		//绘制棋盘 横线
	{
		if ((x / BBLACK % 2) == 0)setlinecolor(BLACK);
		else setlinecolor(RED);
		line(x, PY, x, PY + BBLACK*SIZE);
		line(x-1, PY, x-1, PY + BBLACK*SIZE);

	}
	for (int y = PY; y <PY + BBLACK*(SIZE + 1); y += BBLACK)		//绘制棋盘 竖线
	{
		if (y / BBLACK % 2 == 0)setlinecolor(BLACK);
		else setlinecolor(RED);
		line(PX, y, PX + BBLACK*SIZE, y);
		line(PX, y-1, PX + BBLACK*SIZE, y-1);
	}
	for (int i = PX; i <= PX + BBLACK*SIZE; i += BBLACK)			//获取每部分图片保存在图片指针中
	{
		for (int j = PY; j <= PY + BBLACK*SIZE; j += BBLACK)
		{
			POINT2 DATA;
			DATA.INIT(i, j);
			DATA.WIN2MAP(DATA);
			getimage(&MAPIMAGE[DATA.x][DATA.y], i, j, BBLACK, BBLACK);
		}
	}

	printcircle(PX + (SIZE / 2 - 1)* BBLACK + BBLACK / 2, PY + (SIZE / 2 - 1) * BBLACK + BBLACK / 2, white, mapp);		//初始的四枚棋子
	printcircle(PX + (SIZE / 2 - 1) * BBLACK + BBLACK / 2, PY + (SIZE / 2) * BBLACK + BBLACK / 2, black, mapp);
	printcircle(PX + (SIZE / 2) * BBLACK + BBLACK / 2, PY + (SIZE / 2) * BBLACK + BBLACK / 2, white, mapp);
	printcircle(PX + (SIZE / 2) * BBLACK + BBLACK / 2, PY + (SIZE / 2 - 1) * BBLACK + BBLACK / 2, black, mapp);


	getimage(COUNT, WINDOWS_X / 30, WINDOWS_Y - WINDOWS_Y / 6, 230, 40);											//成绩所在区域背景图片指针
	getimage(COUNT + 1, WINDOWS_X - WINDOWS_X / 5, WINDOWS_Y - WINDOWS_Y / 6, 230, 40);
	getimage(COUNT + 2, BBLACK / 4 + BBLACK * 2, BBLACK / 4 + 2 * BBLACK, 100, 40);   //回合数

	LOGFONT f;
	gettextstyle(&f);												// 获取字体样式
	f.lfHeight = 35;												// 设置字体高度
	strcpy_s(f.lfFaceName, _T("方正姚体"));							// 设置字体
	f.lfQuality = ANTIALIASED_QUALITY;								// 设置输出效果为抗锯齿
	settextstyle(&f);												// 设置字体样式

	settextcolor(BLACK);
	outtextxy(BBLACK / 4, BBLACK / 2, "你所执子");
	outtextxy(BBLACK / 4, BBLACK / 4 + BBLACK, "当前执子");
	outtextxy(BBLACK / 4, BBLACK / 4 + 2 * BBLACK, "当前回合");

	settextstyle(25, 0, "黑体");
	rectangle((WINDOWS_X - WINDOWS_X / 4 - 10), (0.5*BBLACK - 10), (WINDOWS_X - WINDOWS_X / 4 + 1.5*BBLACK), BBLACK);
	outtextxy(WINDOWS_X - WINDOWS_X / 4, 0.5*BBLACK, "保存游戏");          //保存，读取。悔棋
	rectangle((WINDOWS_X - WINDOWS_X / 4 + 2 * BBLACK), (0.5 * BBLACK - 10), (WINDOWS_X - WINDOWS_X / 4 + 3.5 * BBLACK + 10), BBLACK);
	outtextxy(WINDOWS_X - WINDOWS_X / 4 + 2 * BBLACK + 10, 0.5*BBLACK, "读取游戏");          //保存，读取。悔棋

	settextstyle(30, 0, "方正姚体");
	rectangle(WINDOWS_X - WINDOWS_X / 4 - 10,  1.5*BBLACK, WINDOWS_X - WINDOWS_X / 4 + BBLACK, 2 * BBLACK);
	outtextxy(WINDOWS_X - WINDOWS_X / 4,  1.5 * BBLACK, "悔棋");          //悔棋

}

void printmapp()         //画出棋盘上的棋子
{
	int i, j;
	POINT2 WINDOWS2, MAP2;
	for (i = 0; i < SIZE; ++i)                                     //画出棋盘上的棋子
		for (j = 0; j < SIZE; ++j) {
			if (mapp[i][j] == 1 || mapp[i][j] == -1) {
				MAP2.INIT(i, j);
				MAP2.MAP2WIN(WINDOWS2);
				setfillcolor(mapp[i][j] == 1 ? BLACK : WHITE);					//从中间放大变换
				for (int i = 0; i <= CHESSSIZE; ++i)
				{
					solidcircle(WINDOWS2.x, WINDOWS2.y, i);
				}
			}
		}
}


int Judge(int x, int y, int color, int MAP[SIZE][SIZE])									//预判当前位置能否下子
{
	if (MAP[x][y])return 0;																//如果当前位置已经有棋子
	int me = color ? 1 : -1;															//准备落棋棋子颜色
	POINT2 star;
	int count = 0, flag;																//count为该位置可以转换对手棋子个数
	for (int i = 0; i < SIZE; ++i)														//搜索
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
	return count;																		//返回该点转换对方棋子个数
}

void Change(POINT2 NOW, int MAP[SIZE][SIZE], bool F)												//落子后改变棋盘状态 F为是否输出到屏幕
{
	int me = MAP[NOW.x][NOW.y];																	//当前落子棋子颜色
	bool flag;
	POINT2 a, b;
	for (int i = 0; i<SIZE; ++i)																//搜索
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
						if (F)printcircle(b.MAP2WINX(), b.MAP2WINY(), (me == 1) ? black : white, MAP);	//改变棋子
						if (!F)mappadd(b.MAP2WINX(), b.MAP2WINY(), (me == 1) ? black : white, MAP);		//如果不输出到屏幕，改变地图数组
						b.ADD(MOVE[i][0], MOVE[i][1]);
					}
				}
				break;
			}
			a.ADD(MOVE[i][0], MOVE[i][1]);
		}
	}
}

int Statistics(int color)														//预判每个位置可以转化对手棋子个数
{
	int NOWEXPECT = 0;															//总的转化棋子个数
	for (int i = 0; i < SIZE; ++i)												//遍历
		for (int j = 0; j < SIZE; ++j)
		{
			expect[i][j] = Judge(i, j, color, mapp);									//存储该位置可以转化棋子个数
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

				if (TIANEYES)														//如果开启天眼模式
				{
					settextstyle(15, 0, "宋体");
					TCHAR s[20];													//天眼模式
					_stprintf_s(s, _T("%d"), expect[i][j]);
					outtextxy(a.MAP2WINX(), a.MAP2WINY() + 10, s);
				}
			}
		}
	return NOWEXPECT;
}

void CleanStatistics()										//清除期望点提示
{
	for (int i = 0; i < SIZE; ++i)
	{
		for (int j = 0; j < SIZE; ++j)
		{
			if (expect[i][j] && !mapp[i][j])				//如果当前点没有棋子或者有期望
			{
				POINT2 a;
				a.INIT(i, j);								//记录坐标
				putimage(a.MAP2WINX() - BBLACK / 2, a.MAP2WINY() - BBLACK / 2, &MAPIMAGE[i][j]);	//输出局部背景
			}
		}
	}
}

string INTTOCHI(int num, int color)											//当前局势成绩输出汉字
{
	string number[10] = { "","一","二","三","四","五","六","七","八","九" };
	string data = "";
	if (num >= 10)
	{
		data += number[num / 10 % 10];
		data += "十";
	}
	data += number[num % 10];
	//return (color ? "黑棋：" : "白棋：") + data;							//num>=0&&num<=64
	return (color ? "黑棋：" : "白棋：") + std::to_string(num);
}

void Printcount(int balckcount, int whitecount, int nowcolor)		//输出当前分数
{
	settextcolor(BLACK);											//更改字体颜色
	settextstyle(35, 0, "方正姚体");

	putimage(WINDOWS_X / 30, WINDOWS_Y - WINDOWS_Y / 6, COUNT);		//擦出原来痕迹
	putimage(WINDOWS_X - WINDOWS_X / 5, WINDOWS_Y - WINDOWS_Y / 6, COUNT + 1);
	putimage(BBLACK / 4 + BBLACK * 2, BBLACK / 4 + 2 * BBLACK, COUNT + 2);

	outtextxy(WINDOWS_X / 30, WINDOWS_Y - WINDOWS_Y / 6, INTTOCHI(whitecount, white).data());	//输出当前成绩
	outtextxy(WINDOWS_X - WINDOWS_X / 5, WINDOWS_Y - WINDOWS_Y / 6, INTTOCHI(balckcount, black).data());

	char s[5] = {};
	_itoa_s(step, s, 3, 10);
	settextstyle(55, 0, "Kristen ITC");
	outtextxy(BBLACK / 4 + BBLACK * 2, BBLACK / 4 + 2 * BBLACK - 10, s);

	setfillcolor(MYCOLOR == 1 ? BLACK : MYCOLOR == 0 ? WHITE : LIGHTCYAN);						//从中间放大变换
	solidcircle(BBLACK * 2 + 20, BBLACK * 3 / 4, CHESSSIZE * 3 / 4);
	setfillcolor((!nowcolor || balckcount + whitecount == 4) ? BLACK : WHITE);
	NOWCOLOR = (!nowcolor || balckcount + whitecount == 4) ? black : white;						//记录当前执子
	for (int i = 0; i <= CHESSSIZE * 3 / 4; ++i)
	{
		solidcircle(BBLACK * 2 + 20, BBLACK * 3 / 2, i);
		Sleep(1);
	}
}

void WIN(int YOURCOLOR, int balckcount, int whitecount)			//判断输赢
{
	HWND wnd = GetHWnd();										//获取窗口句柄
	if (balckcount>whitecount)
	{
		if (YOURCOLOR == black) mciSendString("play Music/胜利笑声.wav", NULL, 0, NULL);
		else  mciSendString("play Music/失败.wav", NULL, 0, NULL);
		MessageBox(wnd, YOURCOLOR == black ? "恭喜你，胜利啦~" : YOURCOLOR == white ? "输了哎~，不过别灰心，下次一定可以赢的！" : "黑方得胜~", "Result", MB_OK);
	}
	else if (balckcount<whitecount)
	{
		if (YOURCOLOR == white) mciSendString("play Music/胜利笑声.wav", NULL, 0, NULL);
		else  mciSendString("play Music/失败.wav", NULL, 0, NULL);
		MessageBox(wnd, YOURCOLOR == white ? "恭喜你，胜利啦~" : YOURCOLOR == black ? "输了哎~，不过别灰心，下次一定可以赢的！" : "白方得胜", "Result", MB_OK);
	}
	else
	{
		mciSendString("play Music/平局.wav", NULL, 0, NULL);
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

POINT2 Easy1()										//人机对战简单AI
{
	POINT2 MAX;										//定义以及初始化最优解
	MAX.INIT(0, 0);
	int maxx = 0;
	for (int i = 0; i < SIZE; ++i)
		for (int j = 0; j < SIZE; ++j)
		{
			if (expect[i][j] >= maxx)				//寻找可以转化棋子最多的点作为最优解
			{
				maxx = expect[i][j];
				MAX.INIT(i, j);
			}
		}
	if (ESCEXIT)gameStart();
	Sleep(200);										//间歇
	return MAX;										//返回最优解
}

void copymap(int one[SIZE][SIZE], int last[SIZE][SIZE])						//拷贝地图
{
	for (int i = 0; i < SIZE; i++)
		for (int j = 0; j < SIZE; j++)
			one[i][j] = last[i][j];
}

POINT2 Easy()										//人机对战中等AI
{
	int ME = 0;										//AI权值
	int maxx = 0;

	struct _ADD
	{
		int x;										//X坐标
		int y;										//Y坐标
		int w;										//权值
		void init(int x, int y, int w)
		{
			this->x = x;
			this->y = y;
			this->w = w;
		}
		bool operator < (_ADD a)					//重载比较运算符
		{
			return w>a.w;
		}
		POINT2 INTOPOINT2()							//转换为POINT2类型
		{
			POINT2 data;
			data.INIT(x, y);
			return data;
		}
	}WEA[SIZE*SIZE];

	int expectnow[SIZE][SIZE], mapnow[SIZE][SIZE];
	if (ESCEXIT)gameStart();												//按ESC退出
	Sleep(100);																//间歇0.1S

	for (int i = 0; i < SIZE; ++i)
		for (int j = 0; j < SIZE; ++j)
		{
			if (expect[i][j])												//如果当前点可以走棋
			{
				ME = MAPPOINTCOUNT[i][j] + expect[i][j];						//计算本方在该点权值
				copymap(mapnow, mapp);
				mapnow[i][j] = NOWCOLOR ? 1 : -1;							//模拟走棋
				POINT2 NOWPOINT;
				NOWPOINT.INIT(i, j);
				if ((i == 0 && j == 0) || (i == 0 && j == SIZE - 1) || (i == SIZE - 1 && j == SIZE - 1) || (i == SIZE - 1 && j == 0))
				{
					return NOWPOINT;										//如果在角，返回角坐标
				}

				Change(NOWPOINT, mapnow, false);								//模拟走棋后虚拟改变地图
				int YOU = -1050;											//探知对手行动力与局势
				for (int k = 0; k < SIZE; ++k)
					for (int l = 0; l < SIZE; ++l)
					{
						expectnow[k][l] = Judge(k, l, !NOWCOLOR, mapnow);	//判断对手期望
						if (expectnow[k][l])
						{
							YOU = YOU < MAPPOINTCOUNT[k][l] + expectnow[k][l] ? MAPPOINTCOUNT[k][l] + expectnow[k][l] : YOU;
						}
					}
				WEA[maxx++].init(i, j, ME - YOU);							//入结构体数组
			}
		}
	sort(WEA, WEA + maxx);													//按照权值排序
	for (int i = 0; i < maxx; ++i)
	{
		if ((WEA[i].x < 2 && WEA[i].y < 2) || (WEA[i].x < 2 && SIZE - WEA[i].y - 1 < 2) || (SIZE - 1 - WEA[i].x < 2 && WEA[i].y < 2) || (SIZE - 1 - WEA[i].x < 2 && SIZE - 1 - WEA[i].y < 2))continue;
		return WEA[i].INTOPOINT2();											//返回非角边最优解
	}
	return WEA[0].INTOPOINT2();												//返回角边最优解
}


int difai(int x, int y, int mapnow[SIZE][SIZE], int expectnow[SIZE][SIZE], int depin, int depinmax)						//极大极小搜索
{
	if (depin >= depinmax)return 0;											//递归出口

	int maxx = -10005;														//最大权值
	POINT2 NOW;
	int expectnow2[SIZE][SIZE], mapnow2[SIZE][SIZE], mapnext[SIZE][SIZE], expectlast[SIZE][SIZE];					//定义临时数组

	copymap(mapnow2, mapnow);												//复制当前棋盘

	mapnow2[x][y] = NOWCOLOR ? 1 : -1;										//模拟在当前棋盘上下棋
	int ME = MAPPOINTCOUNT[x][y] + expectnow[x][y];							//当前棋子权
	NOW.INIT(x, y);

	Change(NOW, mapnow2, false);											//改变棋盘AI结束

	int MAXEXPECT = 0, LINEEXPECT = 0, COUNT = 0;
	for (int i = 0; i < SIZE; ++i)
		for (int j = 0; j < SIZE; ++j)
		{
			expectnow2[i][j] = Judge(i, j, !NOWCOLOR, mapnow2);				//预判对方是否可以走棋
			if (expectnow2[i][j])
			{
				++MAXEXPECT;
				if ((i == 0 && j == 0) || (i == 0 && j == SIZE - 1) || (i == SIZE - 1 && j == SIZE - 1) || (i == SIZE - 1 && j == 0))return -1800;	//如果对方有占角的可能
				if ((i < 2 && j < 2) || (i < 2 && SIZE - j - 1 < 2) || (SIZE - 1 - i < 2 && j < 2) || (SIZE - 1 - i < 2 && SIZE - 1 - j < 2))++LINEEXPECT;
			}
		}
	if (LINEEXPECT * 10 > MAXEXPECT * 7)return 1400;						//如果对方走到坏点状态较多 剪枝

	for (int i = 0; i < SIZE; i++)
		for (int j = 0; j < SIZE; j++)
			if (expectnow2[i][j])											//如果对方可以走棋
			{
				int YOU = MAPPOINTCOUNT[i][j] + expectnow2[i][j];			//当前权值
				copymap(mapnext, mapnow2);									//拷贝地图
				mapnext[i][j] = (!NOWCOLOR) ? 1 : -1;						//模拟对方走棋
				NOW.INIT(i, j);
				Change(NOW, mapnext, false);								//改变棋盘

				for (int k = 0; k < SIZE; k++)
					for (int l = 0; l < SIZE; l++)
						expectlast[k][l] = Judge(k, l, NOWCOLOR, mapnext);	//寻找AI可行点

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

POINT2 Middle()									//人机对战困难AI
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
					return MAX;										//如果在角，返回角坐标
				}
				int k = difai(i, j, mapp, expect, 0, 3);					//递归搜索 搜索三层
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
// 返回当前已用的搜索时间
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
// 搜索终止时刻
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
// 散列表单元数为64k
const int HASHTABLE_BIT_NUM = 16;
const int HASHTABLE_SIZE = 1u << HASHTABLE_BIT_NUM;
// 取址掩码
const int HASHTABLE_MASK = HASHTABLE_SIZE - 1;
// 散列表
struct Hashtable_Node {
	// 锁值
	unsigned int lock;
	// 估值上、下限
	double lower, upper;
	// 最佳棋步
	int best_move;
	// 搜索深度
	int depth;
};
struct Hashtable {
	// “更深替换”和“始终替换”节点
	struct Hashtable_Node deepest, newest;
};
Hashtable hashtable[HASHTABLE_SIZE];
// 各位置上出现黑棋时对应的散列码
unsigned int zobrist_black[BOARD_SIZE][2] = {
	{ 1288827737,282448869 },{ 450174869,1246877580 },{ 2187254741,2285582077 },{ 3209756399,3373088701 },{ 4087561172,1416650225 },{ 63068892,2650566517 },{ 3446267773,2624715758 },{ 284220267,1298341087 },
	{ 3844988796,2642345446 },{ 231107512,1833054584 },{ 202260255,3928555381 },{ 1113080702,4100775676 },{ 30928383,3376494459 },{ 3672532980,2843567806 },{ 133562111,3034607591 },{ 1255276367,2982969333 },
	{ 344167933,4184993660 },{ 1336409332,1082687484 },{ 2772661342,1956239283 },{ 2484941485,1704900596 },{ 848977913,1951366502 },{ 4077908643,4240998118 },{ 1117582013,3050272501 },{ 1518567399,3162733525 },
	{ 469883390,2224652133 },{ 1319597891,1695839998 },{ 422254458,4185718199 },{ 214671292,261291881 },{ 2602547038,1502478333 },{ 1316000751,2630929002 },{ 2626813421,2502245053 },{ 2889396207,1148108789 },
	{ 3839385021,3874095087 },{ 2700238583,1138155004 },{ 3673571291,2462380970 },{ 747907070,2906124228 },{ 4170486510,3303690175 },{ 389504929,1451716780 },{ 3707960113,2430167988 },{ 3558993918,3901215743 },
};
// 各位置上出现白棋时对应的散列码
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
// 各位置上翻棋时对应的散列码
unsigned int zobrist_flip[BOARD_SIZE][2];
// 反映下棋方轮换的附加散列码
unsigned int zobrist_swap_player[2] = { 5454014,3473489900 };
int myplayer=1;

bool isGameEnd(int board[8][8]) {
	bool flag = false;
	for (int i = 0; i < SIZE; ++i) {									//遍历
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


	// 棋子数量和边缘子数量
	if (my_tiles > opp_tiles)
		vtiles = (100.0 * my_tiles) / (my_tiles + opp_tiles);
	else if (my_tiles < opp_tiles)
		vtiles = -(100.0 * opp_tiles) / (my_tiles + opp_tiles);
	if (my_front_tiles > opp_front_tiles)
		vfront = -(100.0 * my_front_tiles) / (my_front_tiles + opp_front_tiles);
	else if (my_front_tiles < opp_front_tiles)
		vfront = (100.0 * opp_front_tiles) / (my_front_tiles + opp_front_tiles);

	//行动力
	if (my_mobil > opp_mobil)
		vmobil = (100.0 * my_mobil) / (my_mobil + opp_mobil);
	else if (my_mobil < opp_mobil)
		vmobil = -(100.0 * opp_mobil) / (my_mobil + opp_mobil);

	// 角落的权重
	vcorner = board[0][0] * myplayer + board[0][7] * myplayer + board[7][0] * myplayer + board[7][7] * myplayer;
	vcorner = 25 * vcorner;

	// 角落旁边位置的权重,即星位和c位
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


	// 加权得到总的得分  黑方
	double score = ((10 * vtiles) + (801.724 * vcorner) + (382.026 * vclose) + (78.922 * vmobil) + (74.396 * vfront) + (10 * map_weight)) * 100;

	return int(score);
}

unsigned int random() {
	return (rand() << 17) | (rand() << 2) | (rand());
}
// 对zobrist数组初始化
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

// 对zobrist数组初始化
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
// 计算散列码
void get_hashcode2(int pos, int flip[], int n_flip, unsigned int hashcode[], int color) {
	// 如果这一步棋跳步（Pass），需叠加下棋方轮换附加散列码
	if (pos == -1) {
		hashcode[0] = zobrist_swap_player[0];
		hashcode[1] = zobrist_swap_player[1];
		return;
	}
	// 如果是黑棋下，则叠加下棋位置上黑棋的散列码
	if (color == black) {
		hashcode[0] = zobrist_black[pos][0];
		hashcode[1] = zobrist_black[pos][1];
		// 否则是白棋下，则叠加下棋位置上白棋的散列码
	}
	else {
		hashcode[0] = zobrist_white[pos][0];
		hashcode[1] = zobrist_white[pos][1];
	}
	// 对于每个被翻转的棋子，叠加该位置上翻棋的散列码
	for (int i = 0; i<n_flip; ++i) {
		hashcode[0] ^= zobrist_flip[flip[i]][0];
		hashcode[1] ^= zobrist_flip[flip[i]][1];
	}
}

// 计算散列码
void get_hashcode(int board[][8], unsigned int hashcode[],int color) {
	hashcode[0] = hashcode[1] = 0;
	// 对于每个位置
	for (int i = 0; i < 8; ++i)
		for (int j = 0; j < 8; ++j) {          // pos = A1; pos <= H8
											   // 如果是黑棋，则叠加该位置上黑棋的散列码
			int pos = i * 8 + j;
			if (board[i][j] == 1) {    //black
				hashcode[0] ^= zobrist_black[pos][0];
				hashcode[1] ^= zobrist_black[pos][1];
				// 如果是白棋，则叠加该位置上白棋的散列码
			}
			else if (board[i][j] == -1) {   //white
				hashcode[0] ^= zobrist_white[pos][0];
				hashcode[1] ^= zobrist_white[pos][1];
			}
		}
	// 如果轮到白棋下，需叠加反映下棋方轮换的附加散列码
	if (color == white) {
		hashcode[0] ^= zobrist_swap_player[0];
		hashcode[1] ^= zobrist_swap_player[1];
	}
}

// 更新散列表，加入一个局面
void hash_update(unsigned int hashcode[], double lower, double upper, int best_move, int depth) {
	Hashtable *p = hashtable + (hashcode[0] & HASHTABLE_MASK);
	Hashtable_Node *deepest = &(p->deepest), *newest = &(p->newest);
	// 如果与“更深替换”的局面相同
	if ((hashcode[1] == deepest->lock) && (depth == deepest->depth)) {
		// 更新下限
		if (lower > deepest->lower) {
			deepest->lower = lower;
			// 保存最佳棋步
			deepest->best_move = best_move;
		}
		// 更新上限
		if (upper < deepest->upper) {
			deepest->upper = upper;
		}
		// 如果与“始终替换”的局面相同
	}
	else if ((hashcode[1] == newest->lock) && (depth == newest->depth)) {
		// 更新下限
		if (lower > newest->lower) {
			newest->lower = lower;
			// 保存最佳棋步
			newest->best_move = best_move;
		}
		// 更新上限
		if (upper < newest->upper) {
			newest->upper = upper;
		}
		// 否则表明发生冲突，进入冲突处理。如果搜索深度更大，进行“更深替换”
	}
	else if (depth > deepest->depth) {
		// 原先保存的局面仍有保留价值
		*newest = *deepest;
		// 保存当前搜索结果
		deepest->lock = hashcode[1];
		deepest->lower = lower;
		deepest->upper = upper;
		deepest->best_move = best_move;
		deepest->depth = depth;
		// 否则进行“始终替换”
	}
	else {
		// 保存当前搜索结果
		newest->lock = hashcode[1];
		newest->lower = lower;
		newest->upper = upper;
		newest->best_move = best_move;
		newest->depth = depth;
	}
}

// 查找散列表，取出一个局面
Hashtable_Node *hash_get(unsigned int hashcode[], int depth) {
	Hashtable *p = hashtable + (hashcode[0] & HASHTABLE_MASK);
	Hashtable_Node *deepest = &(p->deepest), *newest = &(p->newest);
	// 如果与“更深替换”的局面相同，则返回该局面节点
	if ((hashcode[1] == deepest->lock) && (depth == deepest->depth)) {
		return deepest;
		// 如果与“始终替换”的局面相同，则返回该局面节点
	}
	else if ((hashcode[1] == newest->lock) && (depth == newest->depth)) {
		return newest;
		// 否则表明无此局面，返回空指针
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
	// 计算当前局面的散列码
	get_hashcode(board, hashcode,color);
	// 查询散列表
	Hashtable_Node *node = hash_get(hashcode, depth);
	// 如果散列表中存在当前局面
	if (node) {
		numb++;
		// 如果保存的下限大于alpha值，则修正alpha值
		if (node->lower > alpha) {
			alpha = node->lower;
			// 如果下限大于或等于beta值，则发生剪枝
			if (alpha >= beta) {
				num1++;
				return SVpair(node->best_move,alpha);
			}
		}
		// 如果保存的上限小于beta值，则修正beta值
		if (node->upper < beta) {
			beta = node->upper;
			// 如果上限小于或等于alpha值，则向下剪枝
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
	// 进行常规的剪枝算法
	POINT2 NOW;
	double best_value = -BND;
	int bestmove = -1;
	int i, j;
	// 尝试每个下棋位置
	for (i = 0; i < 8; ++i)
		for (j = 0; j < 8; ++j) {
			// 试着下这步棋，如果棋步合法
			if (Judge(i, j, color, board)) {
				int boardnow[8][8];
				copymap(boardnow, board);									//拷贝地图
				boardnow[i][j] = color == 1 ? 1 : -1;						//模拟对方走棋
				NOW.INIT(i, j);
				Change(NOW, boardnow, false);								//改变棋盘
				double value;
				// 如果到达预定的搜索深度，直接给出局面估值
				if (depth <= 1) value = -evaluation(boardnow, 1 - color);
				// 否则，对所形成的局面进行递归搜索
				else value = -alpha_beta_with_hashtable(boardnow, -beta, -alpha, depth - 1, 1 - color, 0).value;
				copymap(boardnow, board);

				if (stopThink) return SVpair(bestmove, best_value);
				// 撤消棋步
				// 如果这步棋引发剪枝
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
	// 如果没有合法棋步
	if (best_value == -BND) {
		// 如果上一步棋也是跳步，表明对局结束
		if (pass) {
			// 直接给出精确比分
			best_value = getGameEndEval(board,color);
			// 否则这步棋跳步
		}
		else {
			//make_pass();
			// 递归搜索，并标明该跳步
			best_value = -alpha_beta_with_hashtable(board, -beta, -alpha, depth - 1, 1 - color, 1).value;
			// 撤消跳步
			//undo_pass();
		}
	}
	// 返回最佳结果
	// 在散列表中保存当前搜索结果
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

	// 棋子数量和边缘子数量
	if (my_tiles > opp_tiles)
		vtiles = (100.0 * my_tiles) / (my_tiles + opp_tiles);
	else if (my_tiles < opp_tiles)
		vtiles = -(100.0 * opp_tiles) / (my_tiles + opp_tiles);
	if (my_front_tiles > opp_front_tiles)
		vfront = -(100.0 * my_front_tiles) / (my_front_tiles + opp_front_tiles);
	else if (my_front_tiles < opp_front_tiles)
		vfront = (100.0 * opp_front_tiles) / (my_front_tiles + opp_front_tiles);

	//行动力
	if (my_mobil > opp_mobil)
		vmobil = (100.0 * my_mobil) / (my_mobil + opp_mobil);
	else if (my_mobil < opp_mobil)
		vmobil = -(100.0 * opp_mobil) / (my_mobil + opp_mobil);

	// 角落的权重
	vcorner = board[0][0] * myplayer + board[0][7] * myplayer + board[7][0] * myplayer + board[7][7] * myplayer;
	vcorner = 25 * vcorner;

	// 角落旁边位置的权重,即星位和c位
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


	// 加权得到总的得分  difficult2是白方，difficult是黑
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
	// 尝试每个下棋位置
	for (i = 0; i < 8; ++i)
		for (j = 0; j < 8; ++j) {
			// 试着下这步棋，如果棋步合法
			if (Judge(i, j, color, board)) {
				int boardnow[8][8];
				copymap(boardnow, board);									//拷贝地图
				boardnow[i][j] = color == 1 ? 1 : -1;						//模拟对方走棋
				NOW.INIT(i, j);
				Change(NOW, boardnow, false);								//改变棋盘
				double value;
				// 如果到达预定的搜索深度，直接给出局面估值
				if (depth <= 1) value = -evaluation2(boardnow, 1 - color);
				// 否则，对所形成的局面进行递归搜索
				else value = -alpha_beta_with_hashtable2(boardnow, -beta, -alpha, depth - 1, 1 - color, 0).value;
				copymap(boardnow, board);
				//if ((clock() - start_clock)>0.94 * CLOCKS_PER_SEC)
					//break;
				// 撤消棋步
				// 如果这步棋引发剪枝
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
	// 如果没有合法棋步
	if (best_value == -BND) {
		// 如果上一步棋也是跳步，表明对局结束
		if (pass) {
			// 直接给出精确比分
			best_value = getGameEndEval(board, color);
			// 否则这步棋跳步
		}
		else {
			//make_pass();
			// 递归搜索，并标明该跳步
			best_value = -alpha_beta_with_hashtable2(board, -beta, -alpha, depth - 1, 1 - color, 1).value;
			// 撤消跳步
			//undo_pass();
		}
	}
	// 返回最佳结果
	// 在散列表中保存当前搜索结果
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
	ofstream outfile("document.dat", ios::binary);//以二进制形式保存
	if (!outfile) {
		cerr << "保存失败！" << endl;
		HWND wnd = GetHWnd();										//获取窗口句柄
		MessageBox(wnd, "保存失败！", "保存游戏", MB_OK);
		abort();
	}
	outfile.write((char*)&mapp[0], sizeof(mapp));//保存棋盘
	outfile.write((char*)&step, sizeof(step));//步数
	outfile.write((char*)&MYCOLOR, sizeof(MYCOLOR));//玩家棋子颜色
	outfile.write((char*)&NOWCOLOR, sizeof(NOWCOLOR));//当前棋子颜色
	outfile.write((char*)&whitecount, sizeof(whitecount));//白棋得分
	outfile.write((char*)&blackcount, sizeof(blackcount));//黑棋得分
	outfile.write((char*)&diff, sizeof(diff));//难度
	outfile.write((char*)&single, sizeof(single));//单人模式
	outfile.write((char*)&historymap[0][0], sizeof(historymap));//历史棋盘
	outfile.close();

	HWND wnd = GetHWnd();										//获取窗口句柄
	MessageBox(wnd, "保存成功", "保存游戏", MB_OK);
}

void load_game() {
	ifstream infile("document.dat", ios::binary);//把文档以2进制形式保存
	if (!infile) {
		cerr << "读取失败！" << endl;
		HWND wnd = GetHWnd();										//获取窗口句柄
		MessageBox(wnd, "读取失败！", "读取游戏", MB_OK);
		abort();
	}
	init();
	infile.read((char*)&mapp[0], sizeof(mapp));//读取棋盘
	infile.read((char*)&step, sizeof(step));//步数
	infile.read((char*)&MYCOLOR, sizeof(MYCOLOR));//玩家棋子颜色
	infile.read((char*)&NOWCOLOR, sizeof(NOWCOLOR));//玩家棋子颜色
	infile.read((char*)&whitecount, sizeof(whitecount));//电脑棋子颜色
	infile.read((char*)&blackcount, sizeof(blackcount));//白棋得分
	infile.read((char*)&diff, sizeof(diff));//难度
	infile.read((char*)&single, sizeof(single));//单人模式
	infile.read((char*)&historymap[0][0], sizeof(historymap));//历史棋盘
	infile.close();

	printmapp();
	Printcount(blackcount, whitecount, MYCOLOR);					//当前形势
	Statistics(MYCOLOR);
	HWND wnd = GetHWnd();										//获取窗口句柄
	MessageBox(wnd, "载入游戏成功", "确定", MB_OK);
}

void hui_qi() {
	if (step < 2) {
		HWND wnd = GetHWnd();										//获取窗口句柄
		MessageBox(wnd, "不要太贪心呦，已经不能悔棋了", "悔棋", MB_OK);
		return;
	}
	bool flag = false;//用于判断这一把能否悔棋后下棋
	for (step -= 1; step > 0; step--) {//从上一回开始计算
		for (int i = 1; i <= 8; i++) {
			for (int j = 1; j <= 8; j++)
				if (historymap[step][i][j] != 0) {//若历史棋盘在某一位置不为0，则说明这个棋盘可用，因为保存棋盘时已经有能下棋的判断
					flag = true;//这个棋盘可用
					break;
				}
			if (flag)
				break;
		}
		if (flag) {//备份这个棋盘
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
	Printcount(blackcount, whitecount,NOWCOLOR);					//当前形势
	Statistics(MYCOLOR);

	HWND wnd = GetHWnd();										//获取窗口句柄
	MessageBox(wnd, "悔棋成功", "悔棋", MB_OK);
}

POINT2 MOUSE()										//鼠标事件
{
	MOUSEMSG m;
	while (true)
	{
		m = GetMouseMsg();							//获取鼠标信息
		switch (m.uMsg)
		{
		case(WM_LBUTTONDOWN):						//当鼠标左键按下时
		{
			POINT2 NOWMOUSE;
			NOWMOUSE.INIT(m.x, m.y);
			if (TOINTERNET)							//如果处于联网对战状态 发送当前数据
			{
				char Toyou[50] = { 0 };
				sprintf_s(Toyou, "%d,%d", m.x, m.y);
				send(sockConn, Toyou, sizeof(Toyou), 0);		//发送数据
			}
			return NOWMOUSE;						//返回鼠标坐标
			break;
		}
		case(WM_MOUSEMOVE):								//调试 输出鼠标位置
		{
			if (ESCEXIT)gameStart();
			break;
		}
		case(WM_RBUTTONDOWN):								//如果鼠标右键点下时
		{
			TIANEYES = !TIANEYES;							//开启OR关闭天眼模式
			break;
		}
		}
	}
}

bool putmouse(POINT2 &m)									//重定向鼠标坐标
{
	bool flag = true;
	int mouseinx[SIZE + 1], mouseiny[SIZE + 1];
	for (int i = 0; i < SIZE + 1; ++i)						//精确坐标打表
	{
		mouseinx[i] = PX + i*BBLACK;
		mouseiny[i] = PY + i*BBLACK;
	}
	if (m.x > WINDOWS_X - WINDOWS_X / 4 - 10 && m.x< WINDOWS_X - WINDOWS_X / 4 + 1.5*BBLACK && m.y > 0.5*BBLACK - 10 && m.y < BBLACK)  //保存游戏
	{
		mciSendString("play Music/点击.wav", NULL, 0, NULL);
		save_game();
	}
	else if (m.x > WINDOWS_X - WINDOWS_X / 4 + 2 * BBLACK && m.x< WINDOWS_X - WINDOWS_X / 4 + 3.5 * BBLACK + 10 && m.y > 0.5*BBLACK - 10 && m.y < BBLACK)  //读取存档
	{
		mciSendString("play Music/点击.wav", NULL, 0, NULL);
		load_game();
		if (!single) STARTVS(MYCOLOR, MOUSE, MOUSE);
		else {
			if (diff == 1) STARTVS(MYCOLOR, MOUSE, Easy);
			else if (diff == 2) STARTVS(MYCOLOR, MOUSE, Middle);
			else  STARTVS(MYCOLOR, MOUSE, Difficult);
		}
	}
	else if (m.x > WINDOWS_X - WINDOWS_X / 4 - 10 && m.x< WINDOWS_X - WINDOWS_X / 4 + BBLACK && m.y > 1.5*BBLACK && m.y < 2 * BBLACK)  //悔棋
	{
		mciSendString("play Music/悔棋.wav", NULL, 0, NULL);
		if (single) hui_qi();
		else {
			HWND wnd = GetHWnd();										//获取窗口句柄
			MessageBox(wnd, "双人游戏不允许悔棋", "悔棋", MB_OK);
		}
	}
	if (m.x < PX || m.x>PX + SIZE*BBLACK || m.y < PY || m.y>PY + SIZE*BBLACK)flag = false;	//如果点击在棋盘外
	else
	{
		mciSendString("play Music/下棋.wav", NULL, 0, NULL);
		for (int i = 0; i<SIZE; ++i)
		{
			if (m.x >= mouseinx[i] && m.x <= mouseinx[i + 1])
			{
				if (m.x - mouseinx[i]>BBLACK / 8 && mouseinx[i + 1] - m.x>BBLACK / 8)		//重定向X
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
				if (m.y - mouseiny[i]>BBLACK / 8 && mouseiny[i + 1] - m.y > BBLACK / 8)		//重定向Y
				{
					m.y = (mouseiny[i] + mouseiny[i + 1]) / 2;
				}
				else flag = false;
			}
		}
	}
	return flag;											//返回当前位置能否落子
}

void CleanLast(POINT2 WINDOWS2, int F)						//记录上一步走棋位置
{
	if (LASTCH.x > SIZE&&LASTCH.y > SIZE)					//以下取消上一步填充
	{
		setfillcolor(getpixel(LASTCH.x, LASTCH.y));			//获取原来棋子颜色
		putimage(LASTCH.x - BBLACK / 2, LASTCH.y - BBLACK / 2, &MAPIMAGE[LASTCH.WIN2MAPX()][LASTCH.WIN2MAPY()]);
		solidcircle(LASTCH.x, LASTCH.y, CHESSSIZE);
	}

	setfillcolor(RGB(49, 153, 182));						//以下为填充当前走棋
	LASTCH.INIT(WINDOWS2.x, WINDOWS2.y);

	solidrectangle(WINDOWS2.x - BBLACK / 2 + 2, WINDOWS2.y - BBLACK / 2 + 2, WINDOWS2.x + BBLACK / 2 - 2, WINDOWS2.y + BBLACK / 2 - 2);		//背景矩形
	setfillcolor(F ? BLACK : WHITE);
	solidcircle(WINDOWS2.x, WINDOWS2.y, CHESSSIZE);			//画棋子
}

int Playchess(int F, POINT2 WINDOWS2, int &balckcount, int &whitecount)	//开始
{
	//F 黑方为1  白方为0
	POINT2 MAP2;

	if (WINDOWS2.x < SIZE&&WINDOWS2.y < SIZE)					//如果传入的坐标为矩阵坐标
	{
		MAP2 = WINDOWS2;
		WINDOWS2.MAP2WIN(WINDOWS2);								//将其转换成实际展示坐标
	}
	else
	{
		if (!putmouse(WINDOWS2))return 0;						//鼠标输入坐标重定向
		WINDOWS2.WIN2MAP(MAP2);									//存储重定向之后的矩阵坐标
	}
	if (expect[MAP2.x][MAP2.y])									//有位置可行
	{
		CleanStatistics();										//清除屏幕提示
		if (F)													//判断如果为黑棋得分
		{
			balckcount += expect[MAP2.x][MAP2.y] + 1;
			whitecount -= expect[MAP2.x][MAP2.y];
		}
		else
		{
			whitecount += expect[MAP2.x][MAP2.y] + 1;
			balckcount -= expect[MAP2.x][MAP2.y];
		}

		printcircle(WINDOWS2.x, WINDOWS2.y, F, mapp);			//画棋子 mapp为输入数组
		CleanLast(WINDOWS2, F);									//当前走棋棋子提示
		Change(MAP2, mapp, true);									//翻转棋子 true为显示在屏幕
		Printcount(balckcount, whitecount, F);	//打印分数

		if (balckcount + whitecount >= SIZE*SIZE || !balckcount || !whitecount)return 3;	//如果胜负已分
		if (!Statistics(!F))									//如果对方无法走棋
		{
			if (Statistics(F))									//判断自己是否可以走棋
			{
				HL(!F);											//自己可走棋输出对方无法走棋信息
				Printcount(balckcount, whitecount, !F);			//因为前面已经改变了状态，这里用于还原
				return 2;
			}
			else return 3;										//双方都无法走棋
		}
		return 1;
	}
	return 0;
}

void STARTVS(int YOURCOLOR, POINT2 P1(), POINT2 P2())			//开始对战   (mycolor,mouse,easy)
{
	mciSendString("stop Music/背景音乐.mp3", NULL, 0, NULL);
	mciSendString("play Music/棋局.mp3", NULL, 0, NULL);
	time_t timestart , timeend ;
	int music_no = 1;
	timestart= time(NULL);
	MYCOLOR = YOURCOLOR;
	Printcount(blackcount, whitecount, NOWCOLOR);					//当前形势
	Statistics(NOWCOLOR);
	if (MYCOLOR == white && step == 1) goto CX2;
	if (NOWCOLOR == white && !single) goto CX2;
	while (true)
	{
		cout << "hello" << endl;
	CX1:														//本回合忽略
		timeend = time(NULL);
		if (music_no == 1 && difftime(timeend, timestart) > 170) {
			mciSendString("stop Music/背景音乐.mp3", NULL, 0, NULL);
			mciSendString("stop Music/棋局.mp3", NULL, 0, NULL);
			mciSendString("play Music/棋局2.mp3", NULL, 0, NULL);
			timestart = time(NULL);
			music_no = 2;
		}
		if (music_no == 2 && difftime(timeend, timestart) > 285) {
			mciSendString("stop Music/背景音乐.mp3", NULL, 0, NULL);
			mciSendString("stop Music/棋局2.mp3", NULL, 0, NULL);
			mciSendString("play Music/棋局.mp3", NULL, 0, NULL);
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
			goto CX1;											//输入失误或无输入
			break;
		case 1:
			break;												//正常结束
		case 2:
			if (MYCOLOR == white) step++;
			goto CX1;											//忽略对方
			break;
		case 3:
			goto ED;											//棋局结束
			break;
		}
	CX2:                                     //本回合忽略
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
ED:																	//结束
	WIN(YOURCOLOR, blackcount, whitecount);
	_getch();
	if (TOINTERNET)
	{
		closesocket(sockSer);
		closesocket(sockConn);
	}
	gameStart();
}

char *ip;												//定义IP地址变量
void Get_ip()											//获取本机IP地址
{
	WSADATA wsaData;
	char name[255];										//定义用于存放获得的主机名的变量
	PHOSTENT hostinfo;									//获得Winsock版本的正确值
	if (WSAStartup(MAKEWORD(2, 0), &wsaData) == 0)		//在是加载Winsock库，如果WSAStartup（）函数返回值为0，说明加载成功，程序可以继续
	{
		if (gethostname(name, sizeof(name)) == 0)		//成功地将本地主机名存放入由name参数指定的缓冲区中
		{
			if ((hostinfo = gethostbyname(name)) != NULL) //这是获取主机名
			{
				settextstyle(BBLACK / 2, 0, "方正姚体");
				outtextxy(WINDOWS_X / 2 - 5 * BBLACK / 2, BBLACK * 2, "翻转棋阵 服务器已创建");
				settextstyle(BBLACK / 4, 0, "楷体");
				ip = inet_ntoa(*(struct in_addr *)*hostinfo->h_addr_list);   //调用inet_ntoa（）函数，将hostinfo结构变量中的h_addr_list转化为标准的点分表示的IP
				char c[250] = { "1.告诉你的同学这个ip地址就可以联机啦~" };
				strcat_s(c, ip);
				outtextxy(WINDOWS_X / 2 - 3 * BBLACK, BBLACK * 7 / 2, c);
				outtextxy(WINDOWS_X / 2 - 3 * BBLACK, BBLACK * 4, "2.不过要先确保你们在同一个局域网下哦！");
			}
		}
		WSACleanup();										//卸载Winsock库，并释放所有资源
	}
}

POINT2 OURCLASS()									//解析对方发送的数据
{
	POINT2 YOU;
	char data[50] = { 0 };
	int x = 0, y = 0;
	int p = recv(sockConn, data, 50, 0);
	if (p == SOCKET_ERROR)
	{
		HWND wnd = GetHWnd();
		MessageBox(wnd, "对方已中断程序或已掉线,请重启程序", "连接中断", MB_OK | MB_ICONWARNING);
		exit(0);
	}
	sscanf_s(data, "%d,%d", &x, &y);				//保存在变量中
	YOU.INIT(x, y);
	return YOU;
}

void TOI(bool FUORKE)								//联机模式
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
		Get_ip();										//创建服务器
		sockSer = socket(AF_INET, SOCK_STREAM, 0);
	}
	else
	{
		settextstyle(BBLACK / 2, 0, "方正姚体");
		outtextxy(WINDOWS_X / 2 - 3 * BBLACK / 2, BBLACK * 3 / 2, "翻转棋阵的连接");
		settextstyle(BBLACK / 4, 0, "楷体");
		outtextxy(WINDOWS_X / 2 - 2 * BBLACK, BBLACK * 5 / 2, "1、请确保服务端已正常启动");
		outtextxy(WINDOWS_X / 2 - 2 * BBLACK, BBLACK * 3, "2、请确保你和朋友在同一局域网下");
		ip = (char*)malloc(sizeof(char) * 50);
		InputBox(ip, 50, "请输入服务端的IP地址");
		sockConn = socket(AF_INET, SOCK_STREAM, 0);
	}
	addrSer.sin_family = AF_INET;
	addrSer.sin_port = htons(5050);
	addrSer.sin_addr.S_un.S_addr = inet_addr(ip);
	if (FUORKE)											//如果为服务端
	{
		SOCKADDR_IN addrCli;
		bind(sockSer, (SOCKADDR*)&addrSer, sizeof(SOCKADDR));
		listen(sockSer, 5);
		int len = sizeof(SOCKADDR);
		settextstyle(BBLACK / 4, 0, "楷体");
		outtextxy(WINDOWS_X / 2 - 2 * BBLACK, BBLACK * 5, "服务器已创建，等待连接中...");
		sockConn = accept(sockSer, (SOCKADDR*)&addrCli, &len);
	}
	else res = connect(sockConn, (SOCKADDR*)&addrSer, sizeof(SOCKADDR));
	if (sockConn == INVALID_SOCKET || res)
	{
		outtextxy(2 * BBLACK, BBLACK * 6, "连接失败！");
		_getch();
		gameStart();									//返回主界面
	}
	else
	{
		outtextxy(2 * BBLACK, BBLACK * 6, "连接成功！点击任意键进入游戏~");
		_getch();
	}
}

void gameStart()
{
	mciSendString("stop Music/棋局.mp3", NULL, 0, NULL);
	mciSendString("play Music/背景音乐.mp3", NULL, 0, NULL);
	time_t timestart, timeend;
	timestart = time(NULL);

	IMAGE MM[11] = { 0 }, MB[3] = { 0 }, MC[3] = { 0 };
	initgraph(WINDOWS_X, WINDOWS_Y);
	setbkmode(TRANSPARENT);					//透明字体

	HWND hwnd = GetHWnd();					// 设置窗口标题文字
	SetWindowText(hwnd, "翻转棋阵");
	loadimage(NULL, "Picture/0.jpg", 1200, 800);

	const int bblack = 10;

	LOGFONT f;
	gettextstyle(&f);												// 获取字体样式
	f.lfHeight = BBLACK;												// 设置字体高度
	strcpy_s(f.lfFaceName, _T("方正姚体"));								// 设置字体
	f.lfQuality = ANTIALIASED_QUALITY;								// 设置输出效果为抗锯齿
	settextstyle(&f);												// 设置字体样式
	RECT r1 = { 0, 0, WINDOWS_X, WINDOWS_Y / 3 };
	drawtext("翻 转 棋 阵", &r1, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

	settextstyle(BBLACK / 3, 0, "方正姚体");
	RECT r2 = { WINDOWS_X / 2 - BBLACK,WINDOWS_Y / 3,WINDOWS_X / 2 + BBLACK ,WINDOWS_Y / 3 + BBLACK / 2 };
	rectangle(WINDOWS_X / 2 - BBLACK, WINDOWS_Y / 3, WINDOWS_X / 2 + BBLACK, WINDOWS_Y / 3 + BBLACK / 2);
	drawtext("单人模式", &r2, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

	RECT r3 = { WINDOWS_X / 2 - BBLACK,WINDOWS_Y / 3 + BBLACK / 2 + bblack,WINDOWS_X / 2 + BBLACK,WINDOWS_Y / 3 + BBLACK + bblack };
	rectangle(WINDOWS_X / 2 - BBLACK, WINDOWS_Y / 3 + BBLACK / 2 + bblack, WINDOWS_X / 2 + BBLACK, WINDOWS_Y / 3 + BBLACK + bblack);
	drawtext("双人模式", &r3, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

	RECT r4 = { WINDOWS_X / 2 - BBLACK,WINDOWS_Y / 3 + BBLACK + 2 * bblack,WINDOWS_X / 2 + BBLACK,(int)(WINDOWS_Y / 3 + BBLACK*1.5 + 2 * bblack) };
	rectangle(WINDOWS_X / 2 - BBLACK, WINDOWS_Y / 3 + BBLACK + 2 * bblack, WINDOWS_X / 2 + BBLACK, (int)(WINDOWS_Y / 3 + BBLACK*1.5 + 2 * bblack));
	drawtext("载入存档", &r4, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

	RECT r5 = { WINDOWS_X / 2 - BBLACK,(int)(WINDOWS_Y / 3 + BBLACK*1.5 + 3 * bblack),WINDOWS_X / 2 + BBLACK,WINDOWS_Y / 3 + BBLACK * 2 + 3 * bblack };
	rectangle(WINDOWS_X / 2 - BBLACK, (int)(WINDOWS_Y / 3 + BBLACK*1.5 + 3 * bblack), WINDOWS_X / 2 + BBLACK, WINDOWS_Y / 3 + BBLACK * 2 + 3 * bblack);
	drawtext("联机对战", &r5, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

	RECT r6 = { WINDOWS_X / 2 - BBLACK,WINDOWS_Y / 3 + BBLACK * 2 + 4 * bblack,WINDOWS_X / 2 + BBLACK,(int)(WINDOWS_Y / 3 + BBLACK * 2.5 + 4 * bblack) };
	rectangle(WINDOWS_X / 2 - BBLACK, WINDOWS_Y / 3 + BBLACK * 2 + 4 * bblack, WINDOWS_X / 2 + BBLACK, (int)(WINDOWS_Y / 3 + BBLACK * 2.5 + 4 * bblack));
	drawtext("观战模式", &r6, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

	RECT r7 = { WINDOWS_X / 2 - BBLACK,(int)(WINDOWS_Y / 3 + BBLACK * 2.5 + 5 * bblack),WINDOWS_X / 2 + BBLACK,WINDOWS_Y / 3 + BBLACK * 3 + 5 * bblack };
	rectangle(WINDOWS_X / 2 - BBLACK, (int)(WINDOWS_Y / 3 + BBLACK * 2.5 + 5 * bblack), WINDOWS_X / 2 + BBLACK, WINDOWS_Y / 3 + BBLACK * 3 + 5 * bblack);
	drawtext("游戏介绍", &r7, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

	RECT r8 = { WINDOWS_X / 2 - BBLACK,WINDOWS_Y / 3 + BBLACK * 3 + 6 * bblack,WINDOWS_X / 2 + BBLACK,(int)(WINDOWS_Y / 3 + BBLACK * 3.5 + 6 * bblack) };
	rectangle(WINDOWS_X / 2 - BBLACK, WINDOWS_Y / 3 + BBLACK * 3 + 6 * bblack, WINDOWS_X / 2 + BBLACK, (int)(WINDOWS_Y / 3 + BBLACK * 3.5 + 6 * bblack));
	drawtext("操作说明", &r8, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

	RECT r9 = { WINDOWS_X / 2 - BBLACK,(int)(WINDOWS_Y / 3 + BBLACK * 3.5 + 7 * bblack),WINDOWS_X / 2 + BBLACK,WINDOWS_Y / 3 + BBLACK * 4 + 7 * bblack };
	rectangle(WINDOWS_X / 2 - BBLACK, (int)(WINDOWS_Y / 3 + BBLACK * 3.5 + 7 * bblack), WINDOWS_X / 2 + BBLACK, WINDOWS_Y / 3 + BBLACK * 4 + 7 * bblack);
	drawtext("关    于", &r9, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

	RECT r10 = { WINDOWS_X / 2 - BBLACK,(int)(WINDOWS_Y / 3 + BBLACK * 4 + 8 * bblack),WINDOWS_X / 2 + BBLACK,(int)(WINDOWS_Y / 3 + BBLACK * 4.5 + 8 * bblack) };
	rectangle(WINDOWS_X / 2 - BBLACK, (int)(WINDOWS_Y / 3 + BBLACK * 4 + 8 * bblack), WINDOWS_X / 2 + BBLACK, (int)(WINDOWS_Y / 3 + BBLACK * 4.5 + 8 * bblack));
	drawtext("退出游戏", &r10, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

	for (int i = 0; i <= 8; i++)																						//保存按钮图片
		getimage(MM + i, WINDOWS_X / 2 - BBLACK, WINDOWS_Y / 3 + i*bblack + i*BBLACK / 2, 2 * BBLACK, BBLACK / 2);
	getimage(MM + 9, WINDOWS_X - BBLACK, WINDOWS_Y - BBLACK / 2, BBLACK - 11, BBLACK / 2 - 11);

	bool _HOME = true, _INTRODUCTION = false, _OPERATION = false, _ABOUT = false, _TOINTERNET = false, _DRMS = false, _SELECTDIFF = false;			//TRUE表示处于当前页面,selectdiff表示已选择难度，该选择棋子颜色
	MOUSEMSG m;
	while (_HOME)
	{
		timeend = time(NULL);
		if (difftime(timeend, timestart) > 257) {
			mciSendString("stop Music/背景音乐.mp3", NULL, 0, NULL);
			mciSendString("stop Music/棋局.mp3", NULL, 0, NULL);
			mciSendString("stop Music/棋局2.mp3", NULL, 0, NULL);
			mciSendString("play Music/背景音乐.mp3", NULL, 0, NULL);
			timestart = time(NULL);
		}
		BeginBatchDraw();
		m = GetMouseMsg();
		switch (m.uMsg)
		{
		case WM_LBUTTONDOWN:												//当鼠标左键击下时
			EndBatchDraw();
			mciSendString("stop Music/棋局.mp3", NULL, 0, NULL);
			//mciSendString("stop Music/关于.mp3", NULL, 0, NULL);
			//mciSendString("stop Music/游戏介绍.mp3", NULL, 0, NULL);
			mciSendString("play Music/背景音乐.mp3", NULL, 0, NULL);
			if (m.x>WINDOWS_X / 2 - BBLACK && m.x<WINDOWS_X / 2 + BBLACK && m.y>WINDOWS_Y / 3 && m.y<WINDOWS_Y / 3 + BBLACK / 2 && _HOME && !_INTRODUCTION && !_OPERATION && !_ABOUT && !_TOINTERNET && !_DRMS && !_SELECTDIFF)//单人模式
			{
				mciSendString("play Music/按键.wav", NULL, 0, NULL);
				single = true;
				_DRMS = true;									//离开HOME界面
				cleardevice();
				loadimage(NULL, "Picture/0.jpg");				//背景
				rectangle(BBLACK, BBLACK, WINDOWS_X - BBLACK, WINDOWS_Y - BBLACK);

				settextstyle(BBLACK / 2, 0, "方正姚体");
				RECT r0 = { 0, 0, WINDOWS_X, WINDOWS_Y / 3 };
				drawtext("选择游戏难度", &r0, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

				settextstyle(BBLACK / 3, 0, "方正姚体");
				RECT r1 = { WINDOWS_X / 2 - BBLACK,WINDOWS_Y / 3,WINDOWS_X / 2 + BBLACK ,WINDOWS_Y / 3 + BBLACK / 2 };
				rectangle(WINDOWS_X / 2 - BBLACK, WINDOWS_Y / 3, WINDOWS_X / 2 + BBLACK, WINDOWS_Y / 3 + BBLACK / 2);
				drawtext("简    单", &r1, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

				RECT r2 = { WINDOWS_X / 2 - BBLACK,WINDOWS_Y / 3 + BBLACK + 2 * bblack,WINDOWS_X / 2 + BBLACK,(int)(WINDOWS_Y / 3 + BBLACK*1.5 + 2 * bblack) };
				rectangle(WINDOWS_X / 2 - BBLACK, WINDOWS_Y / 3 + BBLACK + 2 * bblack, WINDOWS_X / 2 + BBLACK, (int)(WINDOWS_Y / 3 + BBLACK*1.5 + 2 * bblack));
				drawtext("中    等", &r2, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

				RECT r3 = { WINDOWS_X / 2 - BBLACK,WINDOWS_Y / 3 + BBLACK * 2 + 4 * bblack,WINDOWS_X / 2 + BBLACK,(int)(WINDOWS_Y / 3 + BBLACK * 2.5 + 4 * bblack) };
				rectangle(WINDOWS_X / 2 - BBLACK, WINDOWS_Y / 3 + BBLACK * 2 + 4 * bblack, WINDOWS_X / 2 + BBLACK, (int)(WINDOWS_Y / 3 + BBLACK * 2.5 + 4 * bblack));
				drawtext("困    难", &r3, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

				for (int i = 0; i < 3; i++)
				{
					getimage(MB + i, WINDOWS_X / 2 - BBLACK, WINDOWS_Y / 3 + BBLACK*i + 2 * i * bblack, 2 * BBLACK, BBLACK / 2);
				}

				RECT R = { WINDOWS_X - BBLACK,WINDOWS_Y - BBLACK / 2,WINDOWS_X - 10,WINDOWS_Y - 10 };
				rectangle(WINDOWS_X - BBLACK, WINDOWS_Y - BBLACK / 2, WINDOWS_X - 10, WINDOWS_Y - 10);
				drawtext("返回", &R, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
				break;
			}
			else if (m.x>WINDOWS_X / 2 - BBLACK && m.x<WINDOWS_X / 2 + BBLACK && m.y>WINDOWS_Y / 3 && m.y<WINDOWS_Y / 3 + BBLACK / 2 && _DRMS && !_SELECTDIFF)			//简单
			{
				mciSendString("play Music/按键.wav", NULL, 0, NULL);
				_SELECTDIFF = true;
				diff = 1;         //单人简单模式

				cleardevice();
				loadimage(NULL, "Picture/0.jpg");				//背景
				rectangle(BBLACK, BBLACK, WINDOWS_X - BBLACK, WINDOWS_Y - BBLACK);

				settextstyle(BBLACK / 2, 0, "方正姚体");
				r1 = { 0, 0, WINDOWS_X, WINDOWS_Y / 3 };
				drawtext("选择先手还是后手", &r1, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

				settextstyle(BBLACK / 3, 0, "方正姚体");
				r2 = { WINDOWS_X / 2 - 2 * BBLACK,WINDOWS_Y / 3,WINDOWS_X / 2   ,WINDOWS_Y / 3 + BBLACK / 2 };
				rectangle(WINDOWS_X / 2 - 2 * BBLACK, WINDOWS_Y / 3, WINDOWS_X / 2 + BBLACK, WINDOWS_Y / 3 + BBLACK / 2);
				drawtext("先   手", &r2, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
				setfillcolor(BLACK);
				solidcircle((int)(WINDOWS_X / 2 + BBLACK*0.5), (int)(WINDOWS_Y / 3 + BBLACK / 4), int(BBLACK / 6));

				r3 = { (int)WINDOWS_X / 2 - 2 * BBLACK,(int)WINDOWS_Y / 3 + BBLACK + 2 * bblack,(int)WINDOWS_X / 2  ,(int)(WINDOWS_Y / 3 + BBLACK*1.5 + 2 * bblack) };
				rectangle((int)WINDOWS_X / 2 - 2 * BBLACK, (int)WINDOWS_Y / 3 + BBLACK + 2 * bblack, (int)WINDOWS_X / 2 + BBLACK, (int)(WINDOWS_Y / 3 + BBLACK*1.5 + 2 * bblack));
				drawtext("后   手", &r3, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
				setfillcolor(WHITE);
				solidcircle((int)(WINDOWS_X / 2 + BBLACK *0.5), (int)(WINDOWS_Y / 3 + BBLACK*1.25 + 2 * bblack), int(BBLACK / 6));

				for (int i = 0; i < 2; i++)
				{
					getimage(MC + i, WINDOWS_X / 2 - 2 * BBLACK, WINDOWS_Y / 3 + BBLACK*i + 2 * i * bblack, 2 * BBLACK, BBLACK / 2);
				}

				RECT R = { WINDOWS_X - BBLACK,WINDOWS_Y - BBLACK / 2,WINDOWS_X - 10,WINDOWS_Y - 10 };
				rectangle(WINDOWS_X - BBLACK, WINDOWS_Y - BBLACK / 2, WINDOWS_X - 10, WINDOWS_Y - 10);
				drawtext("返回", &R, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

				break;
			}
			else if (m.x>WINDOWS_X / 2 - BBLACK && m.x<WINDOWS_X / 2 + BBLACK && m.y>WINDOWS_Y / 3 + BBLACK + 2 * bblack && m.y<(int)(WINDOWS_Y / 3 + BBLACK*1.5 + 2 * bblack) && _DRMS && !_SELECTDIFF)			//中等
			{
				mciSendString("play Music/按键.wav", NULL, 0, NULL);
				_SELECTDIFF = true;            //单人中等模式
				diff = 2;
				cleardevice();
				loadimage(NULL, "Picture/0.jpg");				//背景
				rectangle(BBLACK, BBLACK, WINDOWS_X - BBLACK, WINDOWS_Y - BBLACK);

				settextstyle(BBLACK / 2, 0, "方正姚体");
				r1 = { 0, 0, WINDOWS_X, WINDOWS_Y / 3 };
				drawtext("选择先手还是后手", &r1, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

				settextstyle(BBLACK / 3, 0, "方正姚体");
				r2 = { WINDOWS_X / 2 - 2 * BBLACK,WINDOWS_Y / 3,WINDOWS_X / 2  ,WINDOWS_Y / 3 + BBLACK / 2 };
				rectangle(WINDOWS_X / 2 - 2 * BBLACK, WINDOWS_Y / 3, WINDOWS_X / 2 + BBLACK, WINDOWS_Y / 3 + BBLACK / 2);
				drawtext("先   手", &r2, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
				setfillcolor(BLACK);
				solidcircle(WINDOWS_X / 2 + BBLACK / 2, WINDOWS_Y / 3 + BBLACK / 4, int(BBLACK / 6));

				r3 = { WINDOWS_X / 2 - 2 * BBLACK,WINDOWS_Y / 3 + BBLACK + 2 * bblack,WINDOWS_X / 2 ,(int)(WINDOWS_Y / 3 + BBLACK*1.5 + 2 * bblack) };
				rectangle(WINDOWS_X / 2 - 2 * BBLACK, WINDOWS_Y / 3 + BBLACK + 2 * bblack, WINDOWS_X / 2 + BBLACK, (int)(WINDOWS_Y / 3 + BBLACK*1.5 + 2 * bblack));
				drawtext("后   手", &r3, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
				setfillcolor(WHITE);
				solidcircle((int)(WINDOWS_X / 2 + BBLACK / 2), (int)(WINDOWS_Y / 3 + BBLACK*1.25 + 2 * bblack), int(BBLACK / 6));

				for (int i = 0; i < 2; i++)
				{
					getimage(MC + i, WINDOWS_X / 2 - 2 * BBLACK, WINDOWS_Y / 3 + BBLACK*i + 2 * i * bblack, 2 * BBLACK, BBLACK / 2);
				}

				RECT R = { WINDOWS_X - BBLACK,WINDOWS_Y - BBLACK / 2,WINDOWS_X - 10,WINDOWS_Y - 10 };
				rectangle(WINDOWS_X - BBLACK, WINDOWS_Y - BBLACK / 2, WINDOWS_X - 10, WINDOWS_Y - 10);
				drawtext("返回", &R, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

				break;
			}
			else if (m.x>WINDOWS_X / 2 - BBLACK && m.x<WINDOWS_X / 2 + BBLACK && m.y> WINDOWS_Y / 3 + BBLACK * 2 + 4 * bblack && m.y<(int)(WINDOWS_Y / 3 + BBLACK * 2.5 + 4 * bblack) && _DRMS && !_SELECTDIFF)			//困难
			{
				mciSendString("play Music/按键.wav", NULL, 0, NULL);
				_SELECTDIFF = true;
				diff = 3;            //单人困难模式
				cleardevice();
				zobrist_init();

				loadimage(NULL, "Picture/0.jpg");				//背景
				rectangle(BBLACK, BBLACK, WINDOWS_X - BBLACK, WINDOWS_Y - BBLACK);

				settextstyle(BBLACK / 2, 0, "方正姚体");
				r1 = { 0, 0, WINDOWS_X, WINDOWS_Y / 3 };
				drawtext("选择先手还是后手", &r1, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

				settextstyle(BBLACK / 3, 0, "方正姚体");
				r2 = { WINDOWS_X / 2 - 2 * BBLACK,WINDOWS_Y / 3,WINDOWS_X / 2  ,WINDOWS_Y / 3 + BBLACK / 2 };
				rectangle(WINDOWS_X / 2 - 2 * BBLACK, WINDOWS_Y / 3, WINDOWS_X / 2 + BBLACK, WINDOWS_Y / 3 + BBLACK / 2);
				drawtext("先   手", &r2, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
				setfillcolor(BLACK);
				solidcircle(WINDOWS_X / 2 + BBLACK / 2, WINDOWS_Y / 3 + BBLACK / 4, int(BBLACK / 6));

				r3 = { WINDOWS_X / 2 - 2 * BBLACK,WINDOWS_Y / 3 + BBLACK + 2 * bblack,WINDOWS_X / 2 ,(int)(WINDOWS_Y / 3 + BBLACK*1.5 + 2 * bblack) };
				rectangle(WINDOWS_X / 2 - 2 * BBLACK, WINDOWS_Y / 3 + BBLACK + 2 * bblack, WINDOWS_X / 2 + BBLACK, (int)(WINDOWS_Y / 3 + BBLACK*1.5 + 2 * bblack));
				drawtext("后   手", &r3, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
				setfillcolor(WHITE);
				solidcircle((int)(WINDOWS_X / 2 + BBLACK / 2), (int)(WINDOWS_Y / 3 + BBLACK*1.25 + 2 * bblack), int(BBLACK / 6));

				for (int i = 0; i < 2; i++)
				{
					getimage(MC + i, WINDOWS_X / 2 - 2 * BBLACK, WINDOWS_Y / 3 + BBLACK*i + 2 * i * bblack, 2 * BBLACK, BBLACK / 2);
				}

				RECT R = { WINDOWS_X - BBLACK,WINDOWS_Y - BBLACK / 2,WINDOWS_X - 10,WINDOWS_Y - 10 };
				rectangle(WINDOWS_X - BBLACK, WINDOWS_Y - BBLACK / 2, WINDOWS_X - 10, WINDOWS_Y - 10);
				drawtext("返回", &R, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

				break;
			}
			else if (m.x>WINDOWS_X / 2 - 2 * BBLACK && m.x<WINDOWS_X / 2 + BBLACK && m.y>WINDOWS_Y / 3 && m.y<(int)(WINDOWS_Y / 3 + BBLACK / 2) && _DRMS && _SELECTDIFF)			//选择先手
			{
				mciSendString("play Music/按键.wav", NULL, 0, NULL);
				MYCOLOR = black;
				_HOME = false;
				cleardevice();
				init();
				if (diff == 1) STARTVS(black, MOUSE, Easy);
				else if (diff == 2) STARTVS(black, MOUSE, Middle);
				else  STARTVS(black, MOUSE, Difficult);
				break;
			}
			else if (m.x>WINDOWS_X / 2 - 2 * BBLACK && m.x<WINDOWS_X / 2 + BBLACK && m.y>WINDOWS_Y / 3 + BBLACK + 2 * bblack && m.y<(int)(WINDOWS_Y / 3 + BBLACK*1.5 + 2 * bblack) && _DRMS && _SELECTDIFF)			//选择后手
			{
				mciSendString("play Music/按键.wav", NULL, 0, NULL);
				MYCOLOR = white;
				_HOME = false;
				cleardevice();
				init();
				if (diff == 1) STARTVS(white, MOUSE, Easy);
				else if (diff == 2) STARTVS(white, MOUSE, Middle);
				else  STARTVS(white, MOUSE, Difficult);
				break;
			}
			else if (m.x>WINDOWS_X / 2 - BBLACK && m.x<WINDOWS_X / 2 + BBLACK && m.y>WINDOWS_Y / 3 + BBLACK / 2 + bblack && m.y<WINDOWS_Y / 3 + BBLACK + bblack && _HOME && !_INTRODUCTION && !_OPERATION && !_ABOUT && !_TOINTERNET && !_DRMS && !_SELECTDIFF)//双人模式
			{
				mciSendString("play Music/按键.wav", NULL, 0, NULL);
				single = false;
				_HOME = false;									//离开HOME界面
				init();
				STARTVS(1, MOUSE, MOUSE);							//双人模式
				break;
			}
			else if (m.x>WINDOWS_X / 2 - BBLACK && m.x<WINDOWS_X / 2 + BBLACK && m.y>WINDOWS_Y / 3 + BBLACK + 2 * bblack && m.y<(int)(WINDOWS_Y / 3 + BBLACK*1.5 + 2 * bblack) && _HOME && !_OPERATION && !_ABOUT && !_INTRODUCTION && !_TOINTERNET && !_DRMS && !_SELECTDIFF)   //载入游戏
			{
				mciSendString("play Music/按键.wav", NULL, 0, NULL);
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
			else if (m.x>WINDOWS_X / 2 - BBLACK && m.x<WINDOWS_X / 2 + BBLACK && m.y>(int)(WINDOWS_Y / 3 + BBLACK*1.5 + 3 * bblack) && m.y<WINDOWS_Y / 3 + BBLACK * 2 + 3 * bblack && _HOME && !_OPERATION && !_ABOUT && !_INTRODUCTION && !_TOINTERNET && !_DRMS && !_SELECTDIFF)//联机对战
			{
				mciSendString("play Music/按键.wav", NULL, 0, NULL);
				single = false;
				_TOINTERNET = true;
				cleardevice();
				loadimage(NULL, "PICTURE/0.jpg");				//背景
				rectangle(BBLACK, BBLACK, WINDOWS_X - BBLACK, WINDOWS_Y - BBLACK);
				RECT R1 = { WINDOWS_X / 4, WINDOWS_Y / 2 + BBLACK, WINDOWS_X / 4 + 2 * BBLACK , WINDOWS_Y / 2 + 2 * BBLACK };
				RECT R3 = { WINDOWS_X - WINDOWS_X / 4 - 2 * BBLACK, WINDOWS_Y / 2 + BBLACK, WINDOWS_X - WINDOWS_X / 4, WINDOWS_Y / 2 + 2 * BBLACK };
				rectangle(WINDOWS_X / 4, WINDOWS_Y / 2 + BBLACK, WINDOWS_X / 4 + 2 * BBLACK, WINDOWS_Y / 2 + 2 * BBLACK);
				rectangle(WINDOWS_X - WINDOWS_X / 4 - 2 * BBLACK, WINDOWS_Y / 2 + BBLACK, WINDOWS_X - WINDOWS_X / 4, WINDOWS_Y / 2 + 2 * BBLACK);
				drawtext("我要创建", &R1, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
				drawtext("我要连接", &R3, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
				getimage(MM + 9, WINDOWS_X / 4, WINDOWS_Y / 2 + BBLACK, 2 * BBLACK, BBLACK);
				getimage(MM + 10, WINDOWS_X - WINDOWS_X / 4 - 2 * BBLACK, WINDOWS_Y / 2 + BBLACK, 2 * BBLACK, BBLACK);

				outtextxy(WINDOWS_X / 3 - 50, 150, "连接说明：");
				outtextxy(WINDOWS_X / 3, 185, "1、点击“我要创建”创建服务器");
				outtextxy(WINDOWS_X / 3, 220, "2、让你的朋友进入“我要连接”");
				outtextxy(WINDOWS_X / 3, 255, "3、输入服务器ip地址");
				outtextxy(WINDOWS_X / 3, 290, "4、进入游戏");
				outtextxy(WINDOWS_X / 3, 325, "*请确保你们在同一个局域网下哦~");
				RECT R2 = { WINDOWS_X - BBLACK,WINDOWS_Y - BBLACK / 2,WINDOWS_X - 10,WINDOWS_Y - 10 };
				rectangle(WINDOWS_X - BBLACK, WINDOWS_Y - BBLACK / 2, WINDOWS_X - 10, WINDOWS_Y - 10);
				drawtext("返回", &R2, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
				break;
			}
			else if (m.x>WINDOWS_X / 4 && m.x<WINDOWS_X / 4 + 2 * BBLACK  && m.y>WINDOWS_Y / 2 + BBLACK && m.y<WINDOWS_Y / 2 + 2 * BBLACK && _TOINTERNET)//我要创建
			{
				mciSendString("play Music/按键.wav", NULL, 0, NULL);
				TOINTERNET = true;
				cleardevice();
				loadimage(NULL, "Picture/0.jpg");				//背景
				rectangle(BBLACK, BBLACK, WINDOWS_X - BBLACK, WINDOWS_Y - BBLACK);
				TOI(true);								//创建服务器
				init();
				STARTVS(white, OURCLASS, MOUSE);
				break;
			}
			else if (m.x>WINDOWS_X - WINDOWS_X / 4 - 2 * BBLACK && m.x<WINDOWS_X - WINDOWS_X / 4 && m.y>WINDOWS_Y / 2 + BBLACK && m.y<WINDOWS_Y / 2 + 2 * BBLACK && _TOINTERNET)//我要连接
			{
				mciSendString("play Music/按键.wav", NULL, 0, NULL);
				TOINTERNET = true;
				cleardevice();
				loadimage(NULL, "Picture/0.jpg");				//背景
				rectangle(BBLACK, BBLACK, WINDOWS_X - BBLACK, WINDOWS_Y - BBLACK);
				TOI(false);								//连接服务器
				init();
				STARTVS(black, MOUSE, OURCLASS);
				break;
			}
			else if (m.x>WINDOWS_X / 2 - BBLACK && m.x<WINDOWS_X / 2 + BBLACK && m.y> WINDOWS_Y / 3 + BBLACK * 2 + 4 * bblack && m.y<(int)(WINDOWS_Y / 3 + BBLACK * 2.5 + 4 * bblack) && _HOME && !_OPERATION && !_ABOUT && !_INTRODUCTION && !_TOINTERNET && !_DRMS && !_SELECTDIFF)		//观战模式
			{
				mciSendString("play Music/按键.wav", NULL, 0, NULL);
				single = false;
				_HOME = false;										//离开HOME界面
				init();
				zobrist_init();
				STARTVS(-1, Difficult, Difficult2);								//观战模式
				break;
			}
			else if (m.x>WINDOWS_X / 2 - BBLACK && m.x<WINDOWS_X / 2 + BBLACK && m.y>(int)(WINDOWS_Y / 3 + BBLACK * 2.5 + 5 * bblack) && m.y<WINDOWS_Y / 3 + BBLACK * 3 + 5 * bblack && _HOME && !_INTRODUCTION && !_ABOUT && !_OPERATION && !_TOINTERNET && !_DRMS && !_SELECTDIFF)	//游戏介绍
			{
				mciSendString("play Music/按键.wav", NULL, 0, NULL);
				//mciSendString("stop Music/背景音乐.mp3", NULL, 0, NULL);
				//mciSendString("play Music/游戏介绍.mp3", NULL, 0, NULL);
				_INTRODUCTION = true;
				cleardevice();
				loadimage(NULL, "Picture/0.jpg");				//背景
				string data[16] = {
					"游戏介绍：" ,
					"人闲桂花落，夜静春山空",
					"黑云牛马形，白发今如此",
					"棋罢岳钟残,谱负平生志",
					"天下皆白 唯我独黑",
					"民生涂炭 奈之若何",
					"墨门绝术 克而不攻",
					"八横八纵 兼爱平生",
					"墨家主张非攻兼爱 要获得胜利",
					"并非一定要通过杀戮 攻城为下 攻心为上",
					"翻转棋局 棋子虽然不多",
					"但是敌我双方的转化 却是千变万化 步步惊心",
				};

				rectangle(BBLACK, BBLACK, WINDOWS_X - BBLACK, WINDOWS_Y - BBLACK);
				settextstyle(BBLACK / 2, 0, "方正姚体");
				settextcolor(RGB(0, 255, 255));
				outtextxy(WINDOWS_X / 3 - 100, 90, data[0].data());
				settextstyle(BBLACK / 3, 0, "隶书");
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
				drawtext("返回", &R1, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
				break;
			}
			else if (m.x>WINDOWS_X / 2 - BBLACK && m.x<WINDOWS_X / 2 + BBLACK && m.y>WINDOWS_Y / 3 + BBLACK * 3 + 6 * bblack && m.y < (int)(WINDOWS_Y / 3 + BBLACK * 3.5 + 6 * bblack) && _HOME && !_INTRODUCTION && !_OPERATION && !_ABOUT && !_TOINTERNET && !_DRMS && !_SELECTDIFF)	//操作说明
			{
				mciSendString("play Music/按键.wav", NULL, 0, NULL);
				//mciSendString("stop Music/背景音乐.mp3", NULL, 0, NULL);
				//mciSendString("play Music/游戏介绍.mp3", NULL, 0, NULL);
				_OPERATION = true;
				cleardevice();
				loadimage(NULL, "Picture/0.jpg");				//背景
				string data[16] = {
					"操作说明：" ,
					"翻转棋阵与围棋明显的不同就是",
					"翻转棋局中不会有任何棋子被杀死",
					"棋局开始时黑棋位于e4和d5，白棋位于d4和e5。",
					"黑方先行，双方交替下棋。",
					"一步合法的棋步包括：在一个空格新落下一个棋子，并且翻转对手一个或多个棋子。",
					"新落下的棋子与棋盘上已有的同色棋子间，对方被夹住的所有棋子都要翻转过来。",
					"可以是横着夹，竖着夹，或是斜着夹。夹住的位置上必须全部是对手的棋子，不能有空格。",
					"一步棋可以在数个方向上翻棋，任何被夹住的棋子都必须被翻转过来，棋手无权选择不去翻某个棋子。",
					"除非至少翻转了对手的一个棋子，否则就不能落子。",
					"如果一方没有合法棋步，也就是说不管他下到哪里，都不能至少翻转对手的一个棋子，那他这一轮只能弃权，",
					"如果一方至少有一步合法棋步可下，他就必须落子，不得弃权。",
					"棋局持续下去，直到棋盘填满或者双方都无合法棋步可下。",
				};

				rectangle(BBLACK, BBLACK, WINDOWS_X - BBLACK, WINDOWS_Y - BBLACK);
				settextstyle(BBLACK / 2, 0, "方正姚体");
				settextcolor(RGB(0, 255, 255));
				outtextxy(WINDOWS_X / 8 - 50, 90, data[0].data());
				settextstyle(BBLACK / 3, 0, "方正姚体");
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
				drawtext("返回", &R3, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
				break;
			}
			else if (m.x > WINDOWS_X / 2 - BBLACK && m.x<WINDOWS_X / 2 + BBLACK && m.y>(int)(WINDOWS_Y / 3 + BBLACK * 3.5 + 7 * bblack) && m.y < (int)(WINDOWS_Y / 3 + BBLACK * 4 + 7 * bblack) && _HOME && !_INTRODUCTION && !_OPERATION && !_ABOUT && !_TOINTERNET && !_DRMS && !_SELECTDIFF)//关于
			{
				mciSendString("play Music/按键.wav", NULL, 0, NULL);
				//mciSendString("stop Music/背景音乐.mp3", NULL, 0, NULL);
				//mciSendString("play Music/关于.mp3", NULL, 0, NULL);
				_ABOUT = true;
				cleardevice();
				loadimage(NULL, "Picture/0.jpg");				//背景
				string data[16] = {
					"关于:" ,
					"感觉主动进入这个页面的人都是想凑热闹的人哟！",
					"阿超级螺旋阿姆斯特朗无敌宇宙级惊呆银河系之生物诞生！",
					"其实我也不知道在这里写下什么o(╯□╰)o",
					"就随便写写我的喜欢的几个人的段子吧",
					"人闲桂花落，夜静春山空",
					"莱因克尔:“梅西让我意识到当初的我就是一坨屎”",
					"高斯不明白NP问题，因为他在第一次解决一个问题时用的就是最优解法",
					"有阳光的地方就会有阴影，所以有阴影的地方也一定会有阳光",
					"最后给你一个 么么哒(*≧▽≦*)",
				};

				rectangle(BBLACK, BBLACK, WINDOWS_X - BBLACK, WINDOWS_Y - BBLACK);
				settextstyle(BBLACK / 2, 0, "方正姚体");
				settextcolor(RGB(0, 255, 255));
				outtextxy(WINDOWS_X / 3 - 100, 90, data[0].data());
				settextstyle(BBLACK / 3, 0, "隶书");
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
				drawtext("返回", &R3, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
				settextstyle(BBLACK / 4, 0, "微软雅黑");
				string author[5] = {
					"Author: Walle",
					"Github: https://github.com/Walleclipse"
				};
				for (int i = 0; i < 5; i++)
				{
					outtextxy(5 * BBLACK / 4, WINDOWS_Y - (5 - i) * BBLACK / 2, author[i].data());
				}
				settextstyle(BBLACK / 4, 0, "方正姚体");
				break;
			}
			else if (m.x>WINDOWS_X - BBLACK && m.x<WINDOWS_X - 10 && m.y>WINDOWS_Y - BBLACK / 2 && m.y<WINDOWS_Y - 10 && (_INTRODUCTION || _OPERATION || _ABOUT || _TOINTERNET || _DRMS))					//返回
			{
				cleardevice();
				_HOME = false, _INTRODUCTION = false, _OPERATION = false, _ABOUT = false, _TOINTERNET = false, _DRMS = false;
				gameStart();
			}
			else if (m.x>WINDOWS_X / 2 - BBLACK && m.x<WINDOWS_X / 2 + BBLACK && m.y>(int)(WINDOWS_Y / 3 + BBLACK * 4 + 8 * bblack) && m.y<WINDOWS_Y / 3 + BBLACK * 4.5 + 8 * bblack && _HOME && !_INTRODUCTION && !_OPERATION && !_ABOUT && !_TOINTERNET && !_DRMS && !_SELECTDIFF)//退出游戏
			{
				exit(0);
			}
			else break;
		case WM_MOUSEMOVE:									//移动鼠标
			RECT r;
			if (_INTRODUCTION || _OPERATION || _ABOUT || _TOINTERNET || _DRMS)				//如果当前处于游戏介绍 操作说明 或者关于界面 或者联机对战界面 或者单人模式界面
			{
				if (ESCEXIT)gameStart();							//部分界面按ESC退出
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
					drawtext("返回", &r, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
				}
				else
				{
					if (getpixel(WINDOWS_X - BBLACK + 1, WINDOWS_Y - BBLACK / 2 + 1) == RED)
					{
						putimage(WINDOWS_X - BBLACK + 1, WINDOWS_Y - BBLACK / 2 + 1, MM + 8);
						setbkmode(TRANSPARENT);
						drawtext("返回", &r, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
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
								drawtext("简    单", &r, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
								break;
							case 1:
								drawtext("中    等", &r, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
								break;
							case 2:
								drawtext("困    难", &r, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
								break;
							}
						}
						else
						{
							if (getpixel(WINDOWS_X / 2 - BBLACK + 1, WINDOWS_Y / 3 + BBLACK*i + 2 * i * bblack + 1) == RED)
							{
								putimage(WINDOWS_X / 2 - BBLACK, WINDOWS_Y / 3 + BBLACK*i + 2 * i * bblack, MB + i);	//输出原来图片
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
								drawtext("先    手", &r, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
								break;
							case 1:
								drawtext("后    手", &r, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
								break;

							}
						}
						else
						{
							if (getpixel(WINDOWS_X / 2 - 2 * BBLACK + 1, WINDOWS_Y / 3 + BBLACK*i + 2 * i * bblack + 1) == RED)
							{
								putimage(WINDOWS_X / 2 - 2 * BBLACK, WINDOWS_Y / 3 + BBLACK*i + 2 * i * bblack, MC + i);	//输出原来图片

							}
						}
					}
				}
				if (_TOINTERNET)											//处于联机栏
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
						drawtext("我要创建", &r, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
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
						drawtext("我要连接", &r, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
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
							drawtext("单人模式", &r, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
							break;
						case 1:
							drawtext("双人模式", &r, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
							break;
						case 2:
							drawtext("载入存档", &r, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
							break;
						case 3:
							drawtext("联机对战", &r, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
							break;
						case 4:
							drawtext("观战模式", &r, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
							break;
						case 5:
							drawtext("游戏介绍", &r, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
							break;
						case 6:
							drawtext("操作说明", &r, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
							break;
						case 7:
							drawtext("关    于", &r, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
							break;
						case 8:
							drawtext("退出游戏", &r, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
							break;
						}
					}
					else
					{
						if (getpixel(WINDOWS_X / 2 - BBLACK + 1, WINDOWS_Y / 3 + i*bblack + i*BBLACK / 2 + 1) == RED)
						{
							putimage(WINDOWS_X / 2 - BBLACK, WINDOWS_Y / 3 + i*bblack + i*BBLACK / 2, MM + i);	//输出原来图片
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
	gameStart();					//主界面
	closegraph();					//关闭图形化界面
	return 0;
}
