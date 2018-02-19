// Reversi.cpp : ����Ӧ�ó������ڵ㡣
//
#include "stdafx.h"
#include "Reversi.h"
#include "graphics.h"
#include "board.h"
#include <time.h>
#include <stdio.h>
#pragma comment(lib, "material\\easyxw.lib")
/*************************��������*************************/
Board curr[100];									//����
int moves;											//��¼��ǰ˫��һ���ߵĲ�������ͬ�׶��¹�ֵ��ͬ
													//Ҳ�Ǹ��̼�¼�ĳ��ȣ����ڲ鿪�ֿ�
IMAGE img[10];										//����ͼƬ
int comside;										//��������һ��
int gamerecord[100][2];								//���̵ļ�¼
/*************************��������*************************/
void gameset();										//�����ز�
void load();										//ѯ���Ƿ����
void output();										//��ӡ��ǰ����
bool ply();											//�������
void count(bool giveup);							//ͳ�ƽ��������
void reversi();										//��������Ĺ��� 
/*************************��������*************************/

#define MAX_LOADSTRING 100

// ȫ�ֱ���: 
HINSTANCE hInst;                                // ��ǰʵ��
WCHAR szTitle[MAX_LOADSTRING];                  // �������ı�
WCHAR szWindowClass[MAX_LOADSTRING];            // ����������

// �˴���ģ���а����ĺ�����ǰ������: 
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

    // TODO: �ڴ˷��ô��롣
	gameset();											//�ҵĵ���
	while (true)
	{
		load();
		reversi();
	}
/*************************��������*************************/


    // ��ʼ��ȫ���ַ���
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_REVERSI, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // ִ��Ӧ�ó����ʼ��: 
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_REVERSI));

    MSG msg;

    // ����Ϣѭ��: 
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
//  ����: MyRegisterClass()
//
//  Ŀ��: ע�ᴰ���ࡣ
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
//   ����: InitInstance(HINSTANCE, int)
//
//   Ŀ��: ����ʵ�����������������
//
//   ע��: 
//
//        �ڴ˺����У�������ȫ�ֱ����б���ʵ�������
//        ��������ʾ�����򴰿ڡ�
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // ��ʵ������洢��ȫ�ֱ�����

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
//  ����: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  Ŀ��:    ���������ڵ���Ϣ��
//
//  WM_COMMAND  - ����Ӧ�ó���˵�
//  WM_PAINT    - ����������
//  WM_DESTROY  - �����˳���Ϣ������
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // �����˵�ѡ��: 
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
            // TODO: �ڴ˴����ʹ�� hdc ���κλ�ͼ����...
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

// �����ڡ������Ϣ�������
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

/*************************��������*************************/
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
	loadimage(&img[8], _T("material\\regret.jpg"));	//����ͼƬ�ز�
	loadimage(&img[9], _T("material\\star.bmp"));

	initgraph(340, 450);
	IMAGE img_board;
	loadimage(&img_board, _T("material\\board.bmp"));
	putimage(0, 0, &img_board);
	putimage(0, 340, &img[5]);
	setorigin(26, 26);
	SetWindowText(GetHWnd(), _T("Reversi"));		//���û����زģ���ʾ������
	for (int i = 0; i < 8; ++i)
		for (int j = 0; j < 8; ++j)
			putimage(37 * i, 37 * j, &img[0]);		//�ո���

	setbkmode(TRANSPARENT);
	setcolor(RED);
	settextstyle(20, 0, 0);							//����ı������ʽ

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
	Board temp(BLACKONE, POS(4, 3) | POS(3, 4), POS(3, 3) | POS(4, 4));//��ʼ����
	curr[0] = temp;
	moves = 0;
	if (fp)											//���ռ�¼���·�һ�����ָ�
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
	putimage(37 * 4.8, 37 * 8.25, &img[6]);			//�������Ҫ��ס����
	putimage(37 * 5.9, 37 * 8.25, &img[7]);
	putimage(37 * 7, 37 * 8.25, &img[8]);
	TCHAR num_b[MAX_LOADSTRING] = _T(""), num_w[MAX_LOADSTRING] = _T(""), s[MAX_LOADSTRING] = _T("Black: ");
	_itow_s(b, num_b, 10);
	_itow_s(w, num_w, 10);
	lstrcat(s, num_b);
	lstrcat(s, _T("     White: "));
	lstrcat(s, num_w);
	outtextxy(37 * 0.2, 37 * 8.5, s);				//���������
}

bool ply()
{
	MOUSEMSG m;
	HWND wnd = GetHWnd();
	double mx, my;
	FILE *fp = NULL;
	while (true)
	{
		m = GetMouseMsg();							//��ȡ�����Ϣ
		if (m.uMsg == WM_LBUTTONDOWN)				//���������
		{
			mx = (m.x - 26) / 37.0;
			my = (m.y - 26) / 37.0;
			if (!OUTOFBOARD(mx, my))				//��������ڲ�
			{
				int a = (int)mx, b = (int)my;
				if (curr[moves].makeMove(a, b, curr[moves + 1]))//����ǿ������ӵĵط� 
				{
					gamerecord[moves][0] = a;
					gamerecord[moves][1] = b;
					moves++;
					output();
					if (comside == WHITEONE) putimage(37 * a, 37 * b, &img[3]);//����ر�ʶ����ǰ���ӵ�
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
						for (int i = 0; i < moves; ++i)	//�浵
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
					return true;	//����
			}
			else if (mx > 7 && mx < 8 && my > 8.25 && my < 9.25)
			{
				if (MessageBox(wnd, _T("Are you sure to regret?"), _T("Regret"), MB_YESNO | MB_ICONQUESTION) == IDYES)				//����
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
		if (!curr[moves].stalemate())				//����еط���
		{
			if (ply())								//���������
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
			gamerecord[moves][0] = -1;				//û�ط������¼-1,-1
			gamerecord[moves][1] = -1;
			moves++;
			output();
			flag = true;							//�Ȼ��ж��ǲ��ǵ���Ҳû�еط���
		}

		if (!curr[moves].stalemate())				//�����еط���
		{
			outtextxy(tipX, timeY, _T("Computer is thinking..."));//�������˼��
			clock_t start = clock();				//�ü�������ٵ�һ�������ӣ��Ա���ҿ����

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
			while (clock() - start < CLOCKS_PER_SEC) ;//�ü�������ٵ�һ�������ӣ��Ա���ҿ����
			lstrcpy(s, _T("Computer has thought for "));
			lstrcat(s, thoughtTime);
			lstrcat(s, _T(" ms."));
			curr[moves].makeMove(lastmove[0], lastmove[1], curr[moves + 1]);
			gamerecord[moves][0] = lastmove[0];
			gamerecord[moves][1] = lastmove[1];
			moves++;
			output();
			if (comside == WHITEONE) putimage(37 * lastmove[0], 37 * lastmove[1], &img[4]);//����ر�ʶ����ǰ���ӵ�
			else putimage(37 * lastmove[0], 37 * lastmove[1], &img[3]);
			outtextxy(tipX, timeY, s);		//���˼��ʱ��
		}
		else
		{
			Sleep(CLOCKS_PER_SEC);					//���Ե�һ�����������ӿ���
			curr[moves].makeMove(-1, -1, curr[moves + 1]);
			gamerecord[moves][0] = -1;
			gamerecord[moves][1] = -1;				//�������ӿ���
			moves++;
			output();
			if (flag)								//˫�������ӿ���
			{
				outtextxy(tipX, timeY, _T("Game over."));
				count(false);
				return;
			}
			else
			{										//���������ӿ���
				outtextxy(tipX, timeY, _T("Computer cannot make a move."));
			}
		}
	}
}