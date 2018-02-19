// Reversi.cpp : 定义应用程序的入口点。
//
#include "stdafx.h"
#include "Reversi.h"
#include "graphics.h"
#include "board.h"
#include <time.h>
#include <stdio.h>
#pragma comment(lib, "material\\easyxw.lib")
/*************************变量声明*************************/
Board curr[100];									//棋盘
int moves;											//记录当前双方一共走的步数，不同阶段下估值不同
													//也是复盘记录的长度，用于查开局库
IMAGE img[10];										//保存图片
int comside;										//电脑是哪一方
int gamerecord[100][2];								//复盘的记录
/*************************函数声明*************************/
void gameset();										//加载素材
void load();										//询问是否读档
void output();										//打印当前棋盘
bool ply();											//玩家走棋
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
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: 在此放置代码。
	gameset();											//我的调用
	while (true)
	{
		load();
		reversi();
	}
/*************************函数调用*************************/


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
	loadimage(&img[9], _T("material\\star.bmp"));

	initgraph(340, 450);
	IMAGE img_board;
	loadimage(&img_board, _T("material\\board.bmp"));
	putimage(0, 0, &img_board);
	putimage(0, 340, &img[5]);
	setorigin(26, 26);
	SetWindowText(GetHWnd(), _T("Reversi"));		//放置基本素材，标示窗口名
	for (int i = 0; i < 8; ++i)
		for (int j = 0; j < 8; ++j)
			putimage(37 * i, 37 * j, &img[0]);		//空格子

	setbkmode(TRANSPARENT);
	setcolor(RED);
	settextstyle(20, 0, 0);							//标记文本输出格式

	if (!Board::loadBook())
		MessageBox(GetHWnd(), _T("Couldn't open the library file."), _T("Library Error"), MB_OK);
}

void load()
{
	HWND wnd = GetHWnd();
	int comwhite = MessageBox(wnd, _T("Do you want to go first?"), _T("Starting"), MB_YESNO | MB_ICONQUESTION);
	int key = MessageBox(wnd, _T("Do you want to load the saving file?"), _T("Load"), MB_YESNO | MB_ICONQUESTION);
	FILE *fp = NULL;
	if (key == IDYES)
	{
		if (comwhite == IDYES)
		{
			comside = WHITEONE;
			fopen_s(&fp, "material\\white_saving.txt", "r");
		}
		else
		{
			comside = BLACKONE;
			fopen_s(&fp, "material\\black_saving.txt", "r");
		}
		if (fp == NULL)
			MessageBox(wnd, _T("Couldn't open the saving file."), _T("Load Error"), MB_OK);
	}
	if (comwhite == IDYES) comside = WHITEONE;
	else comside = BLACKONE;
	Board temp(BLACKONE, POS(4, 3) | POS(3, 4), POS(3, 3) | POS(4, 4));//初始局面
	curr[0] = temp;
	moves = 0;
	if (fp)											//按照记录的下法一步步恢复
	{
		while (fscanf_s(fp, "%d,%d", &gamerecord[moves][0], &gamerecord[moves][1]) > 0)
		{
			if (curr[moves].makeMove(gamerecord[moves][0], gamerecord[moves][1], curr[moves + 1]))
				moves++;
			else
			{
				MessageBox(wnd, _T("There's an invalid move in the saving file."), _T("Load Error"), MB_OK);
				fclose(fp);
				fp = NULL;
				break;
			}
		}
	}
	if (fp)	
		fclose(fp);
	else
	{
		if (comwhite != IDYES)
		{
			curr[moves].makeMove(3, 2, curr[moves + 1]);
			gamerecord[moves][0] = 3;
			gamerecord[moves][1] = 2;
			moves++;
		}
	}
	output();
}

void output()
{
	int b = 0, w = 0;
	bitboard currBlack = curr[moves].getBoard(BLACKONE), currWhite = curr[moves].getBoard(WHITEONE);
	for (int i = 0; i < 8; ++i)
		for (int j = 0; j < 8; ++j)
		{
			if (currBlack & POS(i, j))
			{
				putimage(37 * i, 37 * j, &img[1]); 
				b++;
			}
			else if (currWhite & POS(i, j))
			{
				putimage(37 * i, 37 * j, &img[2]); 
				w++;
			}
			else
			{
				putimage(37 * i, 37 * j, &img[0]);
			}
		}
	int *validPos = curr[moves].getValidPos();
	int validTop = curr[moves].getValidTop();
	for (int n = 0; n < validTop; ++n)
		putimage(37 * validPos[2 * n], 37 * validPos[2 * n + 1], &img[9]);

	putimage(-26, 340-26, &img[5]);
	putimage(37 * 4.8, 37 * 8.25, &img[6]);			//这个数据要记住……
	putimage(37 * 5.9, 37 * 8.25, &img[7]);
	putimage(37 * 7, 37 * 8.25, &img[8]);
	TCHAR num_b[MAX_LOADSTRING] = _T(""), num_w[MAX_LOADSTRING] = _T(""), s[MAX_LOADSTRING] = _T("Black: ");
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
	double mx, my;
	FILE *fp = NULL;
	while (true)
	{
		m = GetMouseMsg();							//获取鼠标消息
		if (m.uMsg == WM_LBUTTONDOWN)				//如果左键点击
		{
			mx = (m.x - 26) / 37.0;
			my = (m.y - 26) / 37.0;
			if (!OUTOFBOARD(mx, my))				//点击棋盘内部
			{
				int a = (int)mx, b = (int)my;
				if (curr[moves].makeMove(a, b, curr[moves + 1]))//如果是可以落子的地方 
				{
					gamerecord[moves][0] = a;
					gamerecord[moves][1] = b;
					moves++;
					output();
					if (comside == WHITEONE) putimage(37 * a, 37 * b, &img[3]);//特殊地标识出当前下子点
					else putimage(37 * a, 37 * b, &img[4]);
					return false;
				}
			}
			else if (mx > 4.8 && mx < 5.8 && my > 8.25 && my < 9.25)
			{
				if (MessageBox(wnd, _T("Are you sure to replace the saving file before?"), _T("Save"), MB_YESNO | MB_ICONQUESTION) == IDYES)
				{
					if (comside == WHITEONE)
						fopen_s(&fp, "material\\white_saving.txt", "w");
					else
						fopen_s(&fp, "material\\black_saving.txt", "w");
					if (fp)
					{
						for (int i = 0; i < moves; ++i)	//存档
							fprintf_s(fp, "%d,%d ", gamerecord[i][0], gamerecord[i][1]);
						fclose(fp);
					}
					else
						MessageBox(wnd, _T("Couldn't open the saving file."), _T("Save Error"), MB_OK);
				}
			}
			else if (mx > 5.9 && mx < 6.9 && my > 8.25 && my < 9.25)
			{
				if (MessageBox(wnd, _T("Are you sure to surrender?"), _T("Surrender"), MB_YESNO | MB_ICONQUESTION) == IDYES) 
					return true;	//认输
			}
			else if (mx > 7 && mx < 8 && my > 8.25 && my < 9.25)
			{
				if (MessageBox(wnd, _T("Are you sure to regret?"), _T("Regret"), MB_YESNO | MB_ICONQUESTION) == IDYES)				//悔棋
				{
					if (moves >= 2)
					{
						moves -= 2;
						while (moves >= 2 && gamerecord[moves][0] == -1 && gamerecord[moves][1] == -1)
							moves -= 2;
						output();
					}
					else
						MessageBox(wnd, _T("It has been the beginning of the game."), _T("Regret Error"), MB_OK);
				}
			}
		}
	}
}

void count(bool giveup)
{
	int result;
	if (giveup == false)
	{
		result = curr[moves].countResult();
		if (curr[moves].getSide() == comside) result = -result;
	}
	else result = -1;
	HWND wnd = GetHWnd();
	int key;
	switch (result)
	{
		case 1: key = MessageBox(wnd, _T("Congratulations! Do you want to play again?"), _T("Victory"), MB_YESNO | MB_ICONQUESTION); break;
		case 0: key = MessageBox(wnd, _T("A draw. Do you want to play again?"), _T("Draw"), MB_YESNO | MB_ICONQUESTION); break;
		case -1: key = MessageBox(wnd, _T("Sorry, you lose. Do you want to play again?"), _T("Failure"), MB_YESNO | MB_ICONQUESTION); break;
	}
	if (key != IDYES)
	{
		closegraph();
		exit(0);
	}
}

void reversi()
{
	bool flag, searchflag = true;
	const int tipX = 37 * 0.2, timeY = 37 * 9.5, tipY = 37 * 10.5;
	TCHAR thoughtTime[MAX_LOADSTRING] = _T(""), s[MAX_LOADSTRING] = _T("");
	while (true)
	{
		flag = false;
		if (!curr[moves].stalemate())				//玩家有地方下
		{
			if (ply())								//认输则结束
			{
				count(true);
				return;
			}
		}
		else
		{
			outtextxy(tipX, tipY, _T("You cannot make a move."));
			Sleep(CLOCKS_PER_SEC);
			curr[moves].makeMove(-1, -1, curr[moves + 1]);
			gamerecord[moves][0] = -1;				//没地方下则记录-1,-1
			gamerecord[moves][1] = -1;
			moves++;
			output();
			flag = true;							//等会判断是不是电脑也没有地方下
		}

		if (!curr[moves].stalemate())				//电脑有地方下
		{
			outtextxy(tipX, timeY, _T("Computer is thinking..."));//输出正在思考
			clock_t start = clock();				//让计算机至少等一秒再落子，以便玩家看清楚

			int lastmove[2];
			if (searchflag)
				searchflag = curr[moves].searchBook(&gamerecord[0][0], moves, lastmove);
			if (searchflag)
				outtextxy(tipX, tipY, _T("This move is based on the book."));
			else
			{
				if (moves <= 34)
				{
					outtextxy(tipX, tipY, _T("This move is based on Alpha-Beta."));
					curr[moves].ABNegaMax(0, -1000000, 1000000, lastmove, 0);
				}
				else if (moves <= 44)
				{
					outtextxy(tipX, tipY, _T("This move is based on Alpha-Beta."));
					curr[moves].ABNegaMax(0, -1000000, 1000000, lastmove, 1);
				}
				/*else if (moves <= 44)
				{
					outtextxy(tipX, tipY, _T("This move is based on Monte-Carlo."));
					UCTNode::UCTsearch(curr[moves], LastX, LastY);
				}*/
				else
				{
					int result = curr[moves].ABNegaMax(0, -1000000, 1000000, lastmove, 2);
					if (result > 0)
						outtextxy(tipX, tipY, _T("Computer can find a way to win."));
					else if (result == 0) 
						outtextxy(tipX, tipY, _T("Computer can find a way to draw."));
					else
						outtextxy(tipX, tipY, _T("Computer will lose but still try."));
				}
			}
			
			_itow_s(clock() - start, thoughtTime, 10);
			while (clock() - start < CLOCKS_PER_SEC) ;//让计算机至少等一秒再落子，以便玩家看清楚
			lstrcpy(s, _T("Computer has thought for "));
			lstrcat(s, thoughtTime);
			lstrcat(s, _T(" ms."));
			curr[moves].makeMove(lastmove[0], lastmove[1], curr[moves + 1]);
			gamerecord[moves][0] = lastmove[0];
			gamerecord[moves][1] = lastmove[1];
			moves++;
			output();
			if (comside == WHITEONE) putimage(37 * lastmove[0], 37 * lastmove[1], &img[4]);//特殊地标识出当前下子点
			else putimage(37 * lastmove[0], 37 * lastmove[1], &img[3]);
			outtextxy(tipX, timeY, s);		//输出思考时间
		}
		else
		{
			Sleep(CLOCKS_PER_SEC);					//电脑等一秒再宣布无子可落
			curr[moves].makeMove(-1, -1, curr[moves + 1]);
			gamerecord[moves][0] = -1;
			gamerecord[moves][1] = -1;				//电脑无子可落
			moves++;
			output();
			if (flag)								//双方皆无子可落
			{
				outtextxy(tipX, timeY, _T("Game over."));
				count(false);
				return;
			}
			else
			{										//仅电脑无子可落
				outtextxy(tipX, timeY, _T("Computer cannot make a move."));
			}
		}
	}
}