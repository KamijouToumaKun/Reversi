// Reversi.cpp : 定义应用程序的入口点。
//
#include "stdafx.h"
#include "fstream"
#include "Reversi.h"
#include "graphics.h"
#include "ctime"
using namespace std;
#pragma comment(lib, "material\\easyxw.lib")
/*************************常量声明*************************/
const int DEPTH = 7;								//搜索深度，估值太差深了也没有卵用TAT
const int BORDER = 2;								//棋子与边界在棋盘上的标记值 
const int BLACKONE = 1;
const int AVAIL = 0;
const int WHITEONE = -1;
const int value[8][8] = 
{ 90,-60,10,10,10,10,-60, 90,						//大致的棋子位置估价，没有动态考虑稳定子与模板等等 
-60,-80, 5, 5, 5, 5,-80,-60,
10,  5, 1, 1, 1, 1,  5, 10,
10,  5, 1, 1, 1, 1,  5, 10,
10,  5, 1, 1, 1, 1,  5, 10,
10,  5, 1, 1, 1, 1,  5, 10,
-60,-80, 5, 5, 5, 5,-80,-60,
90,-60,10,10,10,10,-60, 90 };
const int searchrank[64][2] =						//先搜角，再搜非与角相邻的边，再搜次边，再搜邻角，再搜中间
{
	{ 1,1 },{ 1,8 },{ 8,1 },{ 8,8 },
	{ 1,3 },{ 1,4 },{ 1,5 },{ 1,6 },{ 8,3 },{ 8,4 },{ 8,5 },{ 8,6 },{ 3,1 },{ 4,1 },{ 5,1 },{ 6,1 },{ 3,8 },{ 4,8 },{ 5,8 },{ 6,8 },
	{ 2,3 },{ 2,4 },{ 2,5 },{ 2,6 },{ 7,3 },{ 7,4 },{ 7,5 },{ 7,6 },{ 3,2 },{ 4,2 },{ 5,2 },{ 6,2 },{ 3,7 },{ 4,7 },{ 5,7 },{ 6,7 },
	{ 1,2 },{ 2,1 },{ 1,7 },{ 7,1 },{ 8,2 },{ 2,8 },{ 8,7 },{ 7,8 },{ 2,2 },{ 2,7 },{ 7,2 },{ 7,7 },
	{ 3,3 },{ 3,4 },{ 3,5 },{ 3,6 },{ 4,3 },{ 4,4 },{ 4,5 },{ 4,6 },{ 5,3 },{ 5,4 },{ 5,5 },{ 5,6 },{ 6,3 },{ 6,4 },{ 6,5 },{ 6,6 }
};
/*************************变量声明*************************/
int board[10][10] = { AVAIL };						//棋盘 
int lastBoard[10][10] = { AVAIL };					//备份棋盘，用于悔棋 
int availPos[20][64 * 8][4];						//每一方可下的位置 0为x 1为y 2为可以吃别人的方位 3为可吃的棋子数 
int top[20];										//栈顶：事先一口气算好是因为要对行动力估价 
int LastX, LastY;									//记录最后电脑给出的落子位置
int moves;											//记录当前双方一共走的步数，不同阶段下估值不同
													//也是复盘记录的长度，用于查开局库
int lastmoves;										//备份步数，用于悔棋
IMAGE img[10];										//保存图片
int comwhite;										//电脑是否后手行棋
int comside;										//电脑是哪一方
bool replay;										//是否再来一局
char gamerecord[140] = "";							//复盘的记录
char lastrecord[140] = "";							//备份记录，用于悔棋
int blacknum, whitenum, fairnum;					//从文件读入的黑优、白优、均势开局库与各自的大小
char blackLib[ 250 * 4 ][60], whiteLib[ 250 * 4 ][60], fairLib[ 250 * 4 ][60];
/*************************函数声明*************************/
bool libsearch(int index, char startlib[][60]);		//查开局库
void gameset();										//加载素材
void load();										//询问是否读档
void avail(int side, int step);						//先判断出所有可以落子的地方以及对应的可以吃别人的方位
void output();										//打印当前棋盘
void textclean();									//当成一个清屏来用
bool ply();											//玩家走棋
bool comFirst();									//开局查开局库
int com(int side, int step);						//电脑走棋 每一步都加上己方估值的变化 并弱剪枝
int comLast(int side, int step);					//残局完全搜索
void count(bool giveup);							//统计结局输出结果
void reversi();										//轮流走棋的过程 
/*************************函数内容*************************/


#define MAX_LOADSTRING 100

// 全局变量: 
HINSTANCE hInst;                                // 当前实例
WCHAR szTitle[MAX_LOADSTRING];                  // 标题栏文本
WCHAR szWindowClass[MAX_LOADSTRING];            // 主窗口类名

// 此代码模块中包含的函数的前向声明: 
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
/*
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: 在此放置代码。


    // 初始化全局字符串
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_REVERSI, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // 执行应用程序初始化: 
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_REVERSI));

    MSG msg;

    // 主消息循环: 
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int) msg.wParam;
*/
	gameset();											//我的调用
	do
	{
		load();
		reversi();
	} while (replay);
	closegraph();
	/*************************函数调用*************************/
	return 0;
}



//
//  函数: MyRegisterClass()
//
//  目的: 注册窗口类。
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_REVERSI));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_REVERSI);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   函数: InitInstance(HINSTANCE, int)
//
//   目的: 保存实例句柄并创建主窗口
//
//   注释: 
//
//        在此函数中，我们在全局变量中保存实例句柄并
//        创建和显示主程序窗口。
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // 将实例句柄存储在全局变量中

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  函数: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  目的:    处理主窗口的消息。
//
//  WM_COMMAND  - 处理应用程序菜单
//  WM_PAINT    - 绘制主窗口
//  WM_DESTROY  - 发送退出消息并返回
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // 分析菜单选择: 
            switch (wmId)
            {
            case IDM_ABOUT:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
            case IDM_EXIT:
                DestroyWindow(hWnd);
                break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            // TODO: 在此处添加使用 hdc 的任何绘图代码...
            EndPaint(hWnd, &ps);
        }
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// “关于”框的消息处理程序。
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}

/*************************函数内容*************************/
bool libsearch(int index, char startlib[][60])
{
	if (strlen(startlib[index]) <= 2 * moves) return false;
	int i;
	for (i = 0; i < 2 * moves; i++)					//从第一位开始检查是否相同
		if (startlib[index][i] != gamerecord[i]) return false;
	return true;
}

void gameset()
{
	loadimage(&img[0], _T("material\\avail.bmp"));
	loadimage(&img[1], _T("material\\black.bmp"));
	loadimage(&img[2], _T("material\\white.bmp"));
	loadimage(&img[3], _T("material\\black_now.bmp"));
	loadimage(&img[4], _T("material\\white_now.bmp"));
	loadimage(&img[5], _T("material\\background.jpg"));
	loadimage(&img[6], _T("material\\save.jpg"));
	loadimage(&img[7], _T("material\\giveup.jpg"));
	loadimage(&img[8], _T("material\\regret.jpg"));	//加载图片素材

	initgraph(340, 450);
	IMAGE img_board;
	loadimage(&img_board, _T("material\\board.bmp"));
	putimage(0, 0, &img_board);
	putimage(0, 340, &img[5]);
	setorigin(26, 26);
	SetWindowText(GetHWnd(), _T("黑白棋"));			//放置基本素材，标示窗口名
	setbkmode(TRANSPARENT);
	setcolor(RED);
	settextstyle(20, 0, 0);							//标记文本输出格式

	int len, i;
	blacknum = whitenum = fairnum = 0;
	ifstream blackin("material\\black_lib.txt");
	ifstream whitein("material\\white_lib.txt");
	ifstream fairin("material\\fair_lib.txt");
	while (blackin >> blackLib[blacknum])
	{
		len = strlen(blackLib[blacknum]);
		for (i=0;i<len;i+=2)
		{
			blackLib[blacknum+1][i] = blackLib[blacknum][i+1];
			blackLib[blacknum+1][i+1] = blackLib[blacknum][i];//交换
			blackLib[blacknum+2][i] = '9' - (blackLib[blacknum][i] - '0');
			blackLib[blacknum+2][i+1] = '9' - (blackLib[blacknum][i+1] - '0');//中心对称
			blackLib[blacknum+3][i] = blackLib[blacknum+2][i+1];
			blackLib[blacknum+3][i+1] = blackLib[blacknum+2][i];//中心对称再交换
		}
		blacknum += 4;
	}
	while (whitein >> whiteLib[whitenum])
	{
		len = strlen(whiteLib[whitenum]);
		for (i=0;i<len;i+=2)
		{
			whiteLib[whitenum+1][i] = whiteLib[whitenum][i+1];
			whiteLib[whitenum+1][i+1] = whiteLib[whitenum][i];//交换
			whiteLib[whitenum+2][i] = '9' - (whiteLib[whitenum][i] - '0');
			whiteLib[whitenum+2][i+1] = '9' - (whiteLib[whitenum][i+1] - '0');//中心对称
			whiteLib[whitenum+3][i] = whiteLib[whitenum+2][i+1];
			whiteLib[whitenum+3][i+1] = whiteLib[whitenum+2][i];//中心对称再交换
		}
		whitenum += 4;
	}
	while (fairin >> fairLib[fairnum])
	{
		len = strlen(fairLib[fairnum]);
		for (i=0;i<len;i+=2)
		{
			fairLib[fairnum+1][i] = fairLib[fairnum][i+1];
			fairLib[fairnum+1][i+1] = fairLib[fairnum][i];//交换
			fairLib[fairnum+2][i] = '9' - (fairLib[fairnum][i] - '0');
			fairLib[fairnum+2][i+1] = '9' - (fairLib[fairnum][i+1] - '0');//中心对称
			fairLib[fairnum+3][i] = fairLib[fairnum+2][i+1];
			fairLib[fairnum+3][i+1] = fairLib[fairnum+2][i];//中心对称再交换
		}
		fairnum += 4;
	}
}

void load()
{
	HWND wnd = GetHWnd();
	comwhite = MessageBox(wnd, _T("您要先手行棋吗？"), _T("欢迎"), MB_YESNO | MB_ICONQUESTION);
	int key = MessageBox(wnd, _T("您要加载上次的存档吗？"), _T("读档"), MB_YESNO | MB_ICONQUESTION);
	int i, j;
	for (i = 0; i<10; i++)
		board[0][i] = board[9][i] = board[i][0] = board[i][9] = BORDER;
	if (key == IDYES)
	{
		int i, j;
		if (comwhite == IDYES)
		{
			comside = WHITEONE;
			ifstream fin("material\\white_saving.txt");//存档文件
			for (i = 1; i <= 8; i++)
				for (j = 1; j <= 8; j++)
				{
					fin >> board[i][j];
					lastBoard[i][j] = board[i][j];
					if (board[i][j]) moves++;
				}
			fin >> gamerecord;
			moves -= 4;
		}
		else
		{
			comside = BLACKONE;
			ifstream fin("material\\black_saving.txt");
			for (i = 1; i <= 8; i++)
				for (j = 1; j <= 8; j++)
				{
					fin >> board[i][j];
					lastBoard[i][j] = board[i][j];
					if (board[i][j]) moves++;
				}
			fin >> gamerecord;
			moves -= 4;
		}
	}
	else
	{
		for (i = 1; i <= 8; i++)
			for (j = 1; j <= 8; j++)
				board[i][j] = AVAIL;
		if (comwhite == IDYES)
		{
			comside = WHITEONE;
			board[4][4] = board[5][5] = WHITEONE;
			board[4][5] = board[5][4] = BLACKONE;
			lastBoard[4][4] = lastBoard[5][5] = WHITEONE;
			lastBoard[4][5] = lastBoard[5][4] = BLACKONE;
			moves = 0;
		}
		else
		{
			comside = BLACKONE;
			board[5][5] = WHITEONE;
			board[4][3] = board[4][4] = board[4][5] = board[5][4] = BLACKONE;
			lastBoard[5][5] = WHITEONE;
			lastBoard[4][3] = lastBoard[4][4] = lastBoard[4][5] = lastBoard[5][4] = BLACKONE;
			gamerecord[0] = '4';
			gamerecord[1] = '3';
			moves = 1;
		}
	}
	strcpy_s(lastrecord, gamerecord);
	lastmoves = moves;
	output();
}

void avail(int side, int step)
{
	int i0, j0, i, j, x0, y0, n;
	top[step] = 0;
	for (n = 0; n < 64; n++)
	{
		i0 = searchrank[n][0];
		j0 = searchrank[n][1];						//按照给定顺序搜索可走的位置
		if (board[i0][j0] == AVAIL)					//这个位置可以走 
		{
			for (x0 = -1; x0 <= 1; x0++)
			{
				for (y0 = -1; y0 <= 1; y0++)		//八个方向
				{
					if (x0 == 0 && y0 == 0) continue;
					if (board[i0 + x0][j0 + y0] != -side) continue;//某个方位上第一颗得是别人的棋子 
					availPos[step][top[step]][3] = 0;
					for (i = i0 + x0, j = j0 + y0; board[i][j] == -side; i += x0, j += y0)
						availPos[step][top[step]][3] ++;//记录在这个方向上能吃的棋子数 
					if (board[i][j] == side)		//之后一直是别人的棋子 直到出现自己的棋子 
					{
						availPos[step][top[step]][0] = i0;
						availPos[step][top[step]][1] = j0;
						availPos[step][top[step]][2] = 3 * x0 + y0;
						top[step] ++;				//一个位置可以在n个方向上吃别人，则分别记录于相邻的n个位置 
					}
				}
			}
		}
	}
	availPos[step][top[step]][0] = availPos[step][top[step]][1] = -1;//防止越界
}

void output()
{
	int i, j, b = 0, w = 0;
	for (i = 1; i <= 8; i++)
	{
		for (j = 1; j <= 8; j++)
		{
			switch (board[i][j])					//打印棋盘
			{
				case AVAIL: 	putimage(37 * (i - 1), 37 * (j - 1), &img[0]); break;
				case BLACKONE: 	putimage(37 * (i - 1), 37 * (j - 1), &img[1]); b++; break;
				case WHITEONE: 	putimage(37 * (i - 1), 37 * (j - 1), &img[2]); w++; break;
			}
		}
	}
	putimage(-26, 340-26, &img[5]);
	putimage(37 * 4.8, 37 * 8.25, &img[6]);			//这个数据要记住……
	putimage(37 * 5.9, 37 * 8.25, &img[7]);
	putimage(37 * 7, 37 * 8.25, &img[8]);
	TCHAR num_b[3] = _T(""), num_w[3] = _T(""), s[100] = _T("Black: ");
	_itow_s(b, num_b, 10);
	_itow_s(w, num_w, 10);
	lstrcat(s, num_b);
	lstrcat(s, _T("     White: "));
	lstrcat(s, num_w);
	outtextxy(37 * 0.2, 37 * 8.5, s);				//输出棋子数
}

void textclean()
{
	int i, j, b = 0, w = 0;
	for (i = 1; i <= 8; i++)
	{
		for (j = 1; j <= 8; j++)
		{
			switch (board[i][j])					//打印棋盘
			{
				case BLACKONE: 	b++; break;
				case WHITEONE: 	w++; break;
			}
		}
	}
	putimage(-26, 340 - 26, &img[5]);
	putimage(37 * 4.8, 37 * 8.25, &img[6]);			//这个数据要记住……
	putimage(37 * 5.9, 37 * 8.25, &img[7]);
	putimage(37 * 7, 37 * 8.25, &img[8]);
	TCHAR num_b[3] = _T(""), num_w[3] = _T(""), s[100] = _T("Black: ");
	_itow_s(b, num_b, 10);
	_itow_s(w, num_w, 10);
	lstrcat(s, num_b);
	lstrcat(s, _T("     White: "));
	lstrcat(s, num_w);
	outtextxy(37 * 0.2, 37 * 8.5, s);				//输出棋子数
}

bool ply()
{
	MOUSEMSG m;
	HWND wnd = GetHWnd();
	int key;
	int a, b, x0, y0, i, j, n;
	bool flag = false, regret = false;
	while (true)
	{
		while (true)
		{
			m = GetMouseMsg();						//获取鼠标消息
			if (m.uMsg == WM_LBUTTONDOWN)			//如果左键点击
			{
				if (m.x - 26 < 37 * 8 && m.y - 26 < 37 * 8)
					break;							//是落子 则跳出循环进行判断
				else if (m.x - 26 > 37 * 4.8 && m.x - 26 < 37 * 5.8 && m.y - 26 > 37 * 8.25 && m.y - 26 < 37 * 9.25)
				{
					key = MessageBox(wnd, _T("您确定要覆盖之前的存档吗？"), _T("存档"), MB_YESNO | MB_ICONQUESTION);
					if (key == IDYES)
					{
						if (comside == WHITEONE)
						{
							ofstream fout("material\\white_saving.txt");
							for (i = 1; i <= 8; i++)	//存档
							{
								for (j = 1; j <= 8; j++)
									fout << board[i][j] << " ";
								fout << endl;
							}
							gamerecord[2 * moves] = '\0';
							fout << gamerecord;
						}
						else
						{
							ofstream fout("material\\black_saving.txt");
							for (i = 1; i <= 8; i++)	//存档
							{
								for (j = 1; j <= 8; j++)
									fout << board[i][j] << " ";
								fout << endl;
							}
							gamerecord[2 * moves] = '\0';
							fout << gamerecord;
						}
					}
				}
				else if (m.x - 26 > 37 * 5.9 && m.x - 26 < 37 * 6.9 && m.y - 26 > 37 * 8.25 && m.y - 26 < 37 * 9.25)
				{
					key = MessageBox(wnd, _T("您确定要认输吗？"), _T("认输"), MB_YESNO | MB_ICONQUESTION);
					if (key == IDYES) return true;	//认输
				}
				else if (m.x - 26 > 37 * 7 && m.x - 26 < 37 * 8 && m.y - 26 > 37 * 8.25 && m.y - 26 < 37 * 9.25)
				{
					if (regret)
					{
						MessageBox(wnd, _T("您已经悔过棋了~"), _T("悔棋"), MB_OK );
						continue;
					}
					key = MessageBox(wnd, _T("您确定要悔棋吗？一次只能悔一步哦~"), _T("悔棋"), MB_YESNO | MB_ICONQUESTION);
					if (key == IDYES)				//悔棋
					{
						for (i = 1; i <= 8; i++)
							for (j = 1; j <= 8; j++)
								board[i][j] = lastBoard[i][j];
						moves = lastmoves;
						strcpy_s(gamerecord, lastrecord);
						output();
						avail(-comside, 0);			//可走的子与棋盘都回到上一步
						regret = true;
					}
				}
			}
		}
		a = (m.x - 26) / 37 + 1;
		b = (m.y - 26) / 37 + 1;
		for (n = 0; n<top[0]; n++)
			if (availPos[0][n][0] == a && availPos[0][n][1] == b)//如果是可以落子的地方 
			{
				for (i = 1; i <= 8; i++)
					for (j = 1; j <= 8; j++)
						lastBoard[i][j] = board[i][j];//覆盖上一步的棋盘
				lastmoves = moves;
				strcpy_s(lastrecord, gamerecord);
				while (availPos[0][n][0] == a && availPos[0][n][1] == b)
				{
					y0 = (availPos[0][n][2] + 6) % 3;//还原可以吃别人的方位  
					if (y0 == 2) y0 = -1;
					x0 = (availPos[0][n][2] - y0) / 3;
					board[a][b] = -comside;
					for (i = a + x0, j = b + y0; board[i][j] == comside; i += x0, j += y0)
						board[i][j] = -comside;		//在这个方位上吃别人 
					n++;
				}
				output();
				if (comside == WHITEONE) putimage(37 * (a - 1), 37 * (b - 1), &img[3]);//特殊地标识出当前下子点 3为黑 4为白
				else putimage(37 * (a - 1), 37 * (b - 1), &img[4]);
				flag = true;
				gamerecord[2 * moves] = a + '0';
				gamerecord[2 * moves + 1] = b + '0';
				break;
			}
		if (flag) break;
	}
	return false;									//未认输，继续行棋
}

bool comFirst()										//查开局库
{
	int i;
	if (comside == BLACKONE)
	{
		for (i = 0; i < blacknum; i++)
			if (libsearch(i, blackLib))
			{
				LastX = blackLib[i][2 * moves] - '0';
				LastY = blackLib[i][2 * moves + 1] - '0';
				return true;
			}
		if (i == blacknum)
		{
			for (i = 0; i < fairnum; i++)
				if (libsearch(i, fairLib))
				{
					LastX = fairLib[i][2 * moves] - '0';
					LastY = fairLib[i][2 * moves + 1] - '0';
					return true;
				}
			if (i == fairnum)
			{
				for (i = 0; i < whitenum; i++)
					if (libsearch(i, whiteLib))
					{
						LastX = whiteLib[i][2 * moves] - '0';
						LastY = whiteLib[i][2 * moves + 1] - '0';
						return true;
					}
				if (i == whitenum) return false;
			}
		}
	}
	else
	{
		for (i = 0; i < whitenum; i++)
			if (libsearch(i, whiteLib))
			{
				LastX = whiteLib[i][2 * moves] - '0';
				LastY = whiteLib[i][2 * moves + 1] - '0';
				return true;
			}
		if (i == whitenum)
		{
			for (i = 0; i < fairnum; i++)
				if (libsearch(i, fairLib))
				{
					LastX = fairLib[i][2 * moves] - '0';
					LastY = fairLib[i][2 * moves + 1] - '0';
					return true;
				}
			if (i == fairnum)
			{
				for (i = 0; i < blacknum; i++)
					if (libsearch(i, blackLib))
					{
						LastX = blackLib[i][2 * moves] - '0';
						LastY = blackLib[i][2 * moves + 1] - '0';
						return true;
					}
				if (i == blacknum) return false;
			}
		}
	}
}

int com(int side, int step)
{
	if (step >= DEPTH) return 0;
	if (step)
	{
		avail(side, step);
		if (top[step] == 0)
		{
			avail(-side, step);
			if (top[step]) return -com(-side, step);
			else return 0;							//下最后一步有利
		}
	}

	int i, j, maxn = 0, sum, n, x0, y0, a, b, indexX = availPos[step][0][0], indexY = availPos[step][0][1];//落子点有初值防止错误
	bool flag = false;
	int **t = new int *[8];
	for (i = 0; i<8; i++)
		t[i] = new int[8];
	for (i = 1; i <= 8; i++)
		for (j = 1; j <= 8; j++)
			t[i - 1][j - 1] = board[i][j];			//在动态数组中拷贝一份 

	for (n = 0; n<top[step]; )
	{
		a = availPos[step][n][0];
		b = availPos[step][n][1];
		sum = value[a - 1][b - 1];
		while (availPos[step][n][0] == a && availPos[step][n][1] == b)
		{
			y0 = (availPos[step][n][2] + 6) % 3;	//还原可以吃别人的方位  这一段是重复代码2 
			if (y0 == 2) y0 = -1;
			x0 = (availPos[step][n][2] - y0) / 3;
			board[a][b] = side;
			for (i = a + x0, j = b + y0; board[i][j] == -side; i += x0, j += y0)
			{
				board[i][j] = side;					//在这个方位上吃别人
				sum += 2 * value[i - 1][j - 1];		//此处sum=统计(a,b)下己方增加的总收益
			}
			n++;
		}
		if (moves <= 20) sum += 4 * top[step];
		else sum += 2 * top[step];					//一个尝试，将伪行动力加入估价
		if (flag == false || sum > maxn)			//此处算是剪枝吧
		{
			moves++;
			sum -= com(-side, step + 1);
			moves--;
			if (flag == false || sum > maxn)
			{
				maxn = sum;
				indexX = a;
				indexY = b;
				flag = true;						//maxn找到了初值 
			}
		}
		for (i = 1; i <= 8; i++)
			for (j = 1; j <= 8; j++)
				board[i][j] = t[i - 1][j - 1];		//拷贝回来，没有使用位棋盘
	}
	for (i = 0; i<8; i++)
		delete[] t[i];
	delete[] t;										//撤销空间

	if (step == 0)									//如果是第一步则标识白棋下子点
	{
		LastX = indexX;								//回头还要在LastX,LastY处落子 
		LastY = indexY;
	}

	return maxn;									//返回最优解
}

int comLast(int side, int step)
{
	if (step)
	{
		avail(side, step);
		if (top[step] == 0)
		{
			avail(-side, step);
			if (top[step]) return -comLast(-side, step);
			else return comside == side ? -4 : 4;	//最后一手有利 
		}
	}

	int i, j, maxn = 0, sum, n, x0, y0, a, b, indexX = availPos[step][0][0], indexY = availPos[step][0][1];//落子点有初值防止错误
	bool flag = false;

	int **t = new int *[8];
	for (i = 0; i<8; i++)
		t[i] = new int[8];
	for (i = 1; i <= 8; i++)
		for (j = 1; j <= 8; j++)
			t[i - 1][j - 1] = board[i][j];			//在动态数组中拷贝一份 

	for (n = 0; n<top[step]; )
	{
		a = availPos[step][n][0];
		b = availPos[step][n][1];
		sum = 1;
		while (availPos[step][n][0] == a && availPos[step][n][1] == b)
		{
			y0 = (availPos[step][n][2] + 6) % 3;	//还原可以吃别人的方位  这一段是重复代码2 
			if (y0 == 2) y0 = -1;
			x0 = (availPos[step][n][2] - y0) / 3;
			board[a][b] = side;
			for (i = a + x0, j = b + y0; board[i][j] == -side; i += x0, j += y0)
			{
				board[i][j] = side;					//在这个方位上吃别人
				sum += 2;							//最后几步每个子的估价都一样 
			}
			n++;
		}
		if (flag == false || sum+4 > maxn)			//此处算是剪枝吧
		{
			moves++;
			sum -= comLast(-side, step + 1);
			moves--;
			if (flag == false || sum > maxn)
			{
				maxn = sum;
				indexX = a;
				indexY = b;
				flag = true;						//maxn找到了初值 
			}
		}
		for (i = 1; i <= 8; i++)
			for (j = 1; j <= 8; j++)
				board[i][j] = t[i - 1][j - 1];		//拷贝回来，没有使用位棋盘
	}

	for (i = 0; i<8; i++)
		delete[] t[i];
	delete[] t;										//撤销空间

	if (step == 0)									//如果是第一步则标识白棋下子点
	{
		LastX = indexX;								//回头还要在LastX,LastY处落子 
		LastY = indexY;
	}

	return maxn;									//返回最优解
}

void count(bool giveup)
{
	int result;
	if (giveup == false)
	{
		int b = 0, w = 0, i, j;
		for (i = 1; i <= 8; i++)
			for (j = 1; j <= 8; j++)
				if (board[i][j] == BLACKONE)
					b++;
				else if (board[i][j] == WHITEONE)
					w++;

		clock_t start = clock();					//让计算机等两秒再宣布结局，以便玩家看清楚
		while (clock() - start < 2 * CLOCKS_PER_SEC);
		if (b == w) result = 0;
		else if (b > w && comside == WHITEONE || b < w && comside == BLACKONE) result = 1;
		else result = -1;
	}
	else result = -1;
	HWND wnd = GetHWnd();
	int key;
	switch(result)
	{
		case 0: key = MessageBox(wnd, _T("平分秋色~~~再来一局吗？"), _T("和局"), MB_YESNO | MB_ICONQUESTION); break;
		case 1: key = MessageBox(wnd, _T("恭喜，您赢了！再来一局吗？"), _T("胜利"), MB_YESNO | MB_ICONQUESTION); break;
		case -1: key = MessageBox(wnd, _T("很遗憾，您输了。再来一局吗？"), _T("失败"), MB_YESNO | MB_ICONQUESTION); break;
	}
	if (key == IDYES) replay = true;
	else replay = false;
}

void reversi()
{
	int i, j;
	bool flag, giveup, checklib = true;
	int n, x0, y0;
	TCHAR thoughtTime[10] = _T(""), s[100] = _T(""), thinking[] = _T("Computer is thinking...");
	while (true)
	{
		flag = false;
		avail(-comside, 0);
		if (top[0])									//玩家有地方下
		{
			giveup = ply();
			if (giveup)								//认输则结束
			{
				count(true);
				return;
			}
		}
		else
		{
			gamerecord[2 * moves] = '0';			//没地方下则记录0,0
			gamerecord[2 * moves + 1] = '0';
			flag = true;							//等会判断是不是电脑也没有地方下
		}
		moves++;
		
		avail(comside, 0);
		if (top[0])
		{
			textclean();
			outtextxy(37 * 0.2, 37 * 9.5, thinking);//输出正在思考
			clock_t start = clock();				//让计算机至少等一秒再落子，以便玩家看清楚
			if (!comFirst())
			{
				checklib = false;
				if (moves <= 46) com(comside, 0);
				else comLast(comside, 0);
			}
			gamerecord[2 * moves] = LastX + '0';	//记录下来
			gamerecord[2 * moves + 1] = LastY + '0';
			_itow_s(clock() - start, thoughtTime, 10);
			while (clock() - start < CLOCKS_PER_SEC) ;//让计算机至少等一秒再落子，以便玩家看清楚
			textclean();
			lstrcpy(s, _T("Computer has thought for "));
			lstrcat(s, thoughtTime);
			lstrcat(s, _T(" ms."));
			if (checklib) lstrcat(s, _T(" (BOOK)"));

			for (n = 0; n<top[0]; n++)				//这里是重复代码3 
				if (availPos[0][n][0] == LastX && availPos[0][n][1] == LastY)//如果是可以落子的地方 
				{
					while (availPos[0][n][0] == LastX && availPos[0][n][1] == LastY)
					{
						y0 = (availPos[0][n][2] + 6) % 3;//还原可以吃别人的方位  
						if (y0 == 2) y0 = -1;
						x0 = (availPos[0][n][2] - y0) / 3;
						board[LastX][LastY] = comside;
						for (i = LastX + x0, j = LastY + y0; board[i][j] == -comside; i += x0, j += y0)
							board[i][j] = comside;	//在这个方位上吃别人 
						n++;
					}
					output();
					if (comside == WHITEONE) putimage(37 * (LastX - 1), 37 * (LastY - 1), &img[4]);//特殊地标识出当前下子点
					else putimage(37 * (LastX - 1), 37 * (LastY - 1), &img[3]);
					if (flag)
					{
						textclean();
						outtextxy(37 * 0.2, 37 * 9, _T("No move available for you."));
					}
					break;
				}
			outtextxy(37 * 0.2, 37 * 9.5, s);		//输出思考时间
		}
		else
		{
			gamerecord[2 * moves] = '0';
			gamerecord[2 * moves + 1] = '0';		//电脑无子可落
			textclean();
			if (flag)								//双方皆无子可落
			{
				outtextxy(37 * 0.2, 37 * 9.5, _T("No move available for you."));
				outtextxy(37 * 0.2, 37 * 10.5, _T("No move available for computer."));
				count(false);
				moves++;
				return;
			}
			else
			{										//仅电脑无子可落
				outtextxy(37 * 0.2, 37 * 9.5, _T("No move available for computer."));
			}
		}
		moves++;
	}
}