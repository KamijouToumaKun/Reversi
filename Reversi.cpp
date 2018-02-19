// Reversi.cpp : ����Ӧ�ó������ڵ㡣
//
#include "stdafx.h"
#include "fstream"
#include "Reversi.h"
#include "graphics.h"
#include "ctime"
using namespace std;
#pragma comment(lib, "material\\easyxw.lib")
/*************************��������*************************/
const int DEPTH = 7;								//������ȣ���ֵ̫������Ҳû������TAT
const int BORDER = 2;								//������߽��������ϵı��ֵ 
const int BLACKONE = 1;
const int AVAIL = 0;
const int WHITEONE = -1;
const int value[8][8] = 
{ 90,-60,10,10,10,10,-60, 90,						//���µ�����λ�ù��ۣ�û�ж�̬�����ȶ�����ģ��ȵ� 
-60,-80, 5, 5, 5, 5,-80,-60,
10,  5, 1, 1, 1, 1,  5, 10,
10,  5, 1, 1, 1, 1,  5, 10,
10,  5, 1, 1, 1, 1,  5, 10,
10,  5, 1, 1, 1, 1,  5, 10,
-60,-80, 5, 5, 5, 5,-80,-60,
90,-60,10,10,10,10,-60, 90 };
const int searchrank[64][2] =						//���ѽǣ����ѷ�������ڵıߣ����Ѵαߣ������ڽǣ������м�
{
	{ 1,1 },{ 1,8 },{ 8,1 },{ 8,8 },
	{ 1,3 },{ 1,4 },{ 1,5 },{ 1,6 },{ 8,3 },{ 8,4 },{ 8,5 },{ 8,6 },{ 3,1 },{ 4,1 },{ 5,1 },{ 6,1 },{ 3,8 },{ 4,8 },{ 5,8 },{ 6,8 },
	{ 2,3 },{ 2,4 },{ 2,5 },{ 2,6 },{ 7,3 },{ 7,4 },{ 7,5 },{ 7,6 },{ 3,2 },{ 4,2 },{ 5,2 },{ 6,2 },{ 3,7 },{ 4,7 },{ 5,7 },{ 6,7 },
	{ 1,2 },{ 2,1 },{ 1,7 },{ 7,1 },{ 8,2 },{ 2,8 },{ 8,7 },{ 7,8 },{ 2,2 },{ 2,7 },{ 7,2 },{ 7,7 },
	{ 3,3 },{ 3,4 },{ 3,5 },{ 3,6 },{ 4,3 },{ 4,4 },{ 4,5 },{ 4,6 },{ 5,3 },{ 5,4 },{ 5,5 },{ 5,6 },{ 6,3 },{ 6,4 },{ 6,5 },{ 6,6 }
};
/*************************��������*************************/
int board[10][10] = { AVAIL };						//���� 
int lastBoard[10][10] = { AVAIL };					//�������̣����ڻ��� 
int availPos[20][64 * 8][4];						//ÿһ�����µ�λ�� 0Ϊx 1Ϊy 2Ϊ���ԳԱ��˵ķ�λ 3Ϊ�ɳԵ������� 
int top[20];										//ջ��������һ�����������ΪҪ���ж������� 
int LastX, LastY;									//��¼�����Ը���������λ��
int moves;											//��¼��ǰ˫��һ���ߵĲ�������ͬ�׶��¹�ֵ��ͬ
													//Ҳ�Ǹ��̼�¼�ĳ��ȣ����ڲ鿪�ֿ�
int lastmoves;										//���ݲ��������ڻ���
IMAGE img[10];										//����ͼƬ
int comwhite;										//�����Ƿ��������
int comside;										//��������һ��
bool replay;										//�Ƿ�����һ��
char gamerecord[140] = "";							//���̵ļ�¼
char lastrecord[140] = "";							//���ݼ�¼�����ڻ���
int blacknum, whitenum, fairnum;					//���ļ�����ĺ��š����š����ƿ��ֿ�����ԵĴ�С
char blackLib[ 250 * 4 ][60], whiteLib[ 250 * 4 ][60], fairLib[ 250 * 4 ][60];
/*************************��������*************************/
bool libsearch(int index, char startlib[][60]);		//�鿪�ֿ�
void gameset();										//�����ز�
void load();										//ѯ���Ƿ����
void avail(int side, int step);						//���жϳ����п������ӵĵط��Լ���Ӧ�Ŀ��ԳԱ��˵ķ�λ
void output();										//��ӡ��ǰ����
void textclean();									//����һ����������
bool ply();											//�������
bool comFirst();									//���ֲ鿪�ֿ�
int com(int side, int step);						//�������� ÿһ�������ϼ�����ֵ�ı仯 ������֦
int comLast(int side, int step);					//�о���ȫ����
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
/*
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: �ڴ˷��ô��롣


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
*/
	gameset();											//�ҵĵ���
	do
	{
		load();
		reversi();
	} while (replay);
	closegraph();
	/*************************��������*************************/
	return 0;
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
bool libsearch(int index, char startlib[][60])
{
	if (strlen(startlib[index]) <= 2 * moves) return false;
	int i;
	for (i = 0; i < 2 * moves; i++)					//�ӵ�һλ��ʼ����Ƿ���ͬ
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
	loadimage(&img[8], _T("material\\regret.jpg"));	//����ͼƬ�ز�

	initgraph(340, 450);
	IMAGE img_board;
	loadimage(&img_board, _T("material\\board.bmp"));
	putimage(0, 0, &img_board);
	putimage(0, 340, &img[5]);
	setorigin(26, 26);
	SetWindowText(GetHWnd(), _T("�ڰ���"));			//���û����زģ���ʾ������
	setbkmode(TRANSPARENT);
	setcolor(RED);
	settextstyle(20, 0, 0);							//����ı������ʽ

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
			blackLib[blacknum+1][i+1] = blackLib[blacknum][i];//����
			blackLib[blacknum+2][i] = '9' - (blackLib[blacknum][i] - '0');
			blackLib[blacknum+2][i+1] = '9' - (blackLib[blacknum][i+1] - '0');//���ĶԳ�
			blackLib[blacknum+3][i] = blackLib[blacknum+2][i+1];
			blackLib[blacknum+3][i+1] = blackLib[blacknum+2][i];//���ĶԳ��ٽ���
		}
		blacknum += 4;
	}
	while (whitein >> whiteLib[whitenum])
	{
		len = strlen(whiteLib[whitenum]);
		for (i=0;i<len;i+=2)
		{
			whiteLib[whitenum+1][i] = whiteLib[whitenum][i+1];
			whiteLib[whitenum+1][i+1] = whiteLib[whitenum][i];//����
			whiteLib[whitenum+2][i] = '9' - (whiteLib[whitenum][i] - '0');
			whiteLib[whitenum+2][i+1] = '9' - (whiteLib[whitenum][i+1] - '0');//���ĶԳ�
			whiteLib[whitenum+3][i] = whiteLib[whitenum+2][i+1];
			whiteLib[whitenum+3][i+1] = whiteLib[whitenum+2][i];//���ĶԳ��ٽ���
		}
		whitenum += 4;
	}
	while (fairin >> fairLib[fairnum])
	{
		len = strlen(fairLib[fairnum]);
		for (i=0;i<len;i+=2)
		{
			fairLib[fairnum+1][i] = fairLib[fairnum][i+1];
			fairLib[fairnum+1][i+1] = fairLib[fairnum][i];//����
			fairLib[fairnum+2][i] = '9' - (fairLib[fairnum][i] - '0');
			fairLib[fairnum+2][i+1] = '9' - (fairLib[fairnum][i+1] - '0');//���ĶԳ�
			fairLib[fairnum+3][i] = fairLib[fairnum+2][i+1];
			fairLib[fairnum+3][i+1] = fairLib[fairnum+2][i];//���ĶԳ��ٽ���
		}
		fairnum += 4;
	}
}

void load()
{
	HWND wnd = GetHWnd();
	comwhite = MessageBox(wnd, _T("��Ҫ����������"), _T("��ӭ"), MB_YESNO | MB_ICONQUESTION);
	int key = MessageBox(wnd, _T("��Ҫ�����ϴεĴ浵��"), _T("����"), MB_YESNO | MB_ICONQUESTION);
	int i, j;
	for (i = 0; i<10; i++)
		board[0][i] = board[9][i] = board[i][0] = board[i][9] = BORDER;
	if (key == IDYES)
	{
		int i, j;
		if (comwhite == IDYES)
		{
			comside = WHITEONE;
			ifstream fin("material\\white_saving.txt");//�浵�ļ�
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
		j0 = searchrank[n][1];						//���ո���˳���������ߵ�λ��
		if (board[i0][j0] == AVAIL)					//���λ�ÿ����� 
		{
			for (x0 = -1; x0 <= 1; x0++)
			{
				for (y0 = -1; y0 <= 1; y0++)		//�˸�����
				{
					if (x0 == 0 && y0 == 0) continue;
					if (board[i0 + x0][j0 + y0] != -side) continue;//ĳ����λ�ϵ�һ�ŵ��Ǳ��˵����� 
					availPos[step][top[step]][3] = 0;
					for (i = i0 + x0, j = j0 + y0; board[i][j] == -side; i += x0, j += y0)
						availPos[step][top[step]][3] ++;//��¼������������ܳԵ������� 
					if (board[i][j] == side)		//֮��һֱ�Ǳ��˵����� ֱ�������Լ������� 
					{
						availPos[step][top[step]][0] = i0;
						availPos[step][top[step]][1] = j0;
						availPos[step][top[step]][2] = 3 * x0 + y0;
						top[step] ++;				//һ��λ�ÿ�����n�������ϳԱ��ˣ���ֱ��¼�����ڵ�n��λ�� 
					}
				}
			}
		}
	}
	availPos[step][top[step]][0] = availPos[step][top[step]][1] = -1;//��ֹԽ��
}

void output()
{
	int i, j, b = 0, w = 0;
	for (i = 1; i <= 8; i++)
	{
		for (j = 1; j <= 8; j++)
		{
			switch (board[i][j])					//��ӡ����
			{
				case AVAIL: 	putimage(37 * (i - 1), 37 * (j - 1), &img[0]); break;
				case BLACKONE: 	putimage(37 * (i - 1), 37 * (j - 1), &img[1]); b++; break;
				case WHITEONE: 	putimage(37 * (i - 1), 37 * (j - 1), &img[2]); w++; break;
			}
		}
	}
	putimage(-26, 340-26, &img[5]);
	putimage(37 * 4.8, 37 * 8.25, &img[6]);			//�������Ҫ��ס����
	putimage(37 * 5.9, 37 * 8.25, &img[7]);
	putimage(37 * 7, 37 * 8.25, &img[8]);
	TCHAR num_b[3] = _T(""), num_w[3] = _T(""), s[100] = _T("Black: ");
	_itow_s(b, num_b, 10);
	_itow_s(w, num_w, 10);
	lstrcat(s, num_b);
	lstrcat(s, _T("     White: "));
	lstrcat(s, num_w);
	outtextxy(37 * 0.2, 37 * 8.5, s);				//���������
}

void textclean()
{
	int i, j, b = 0, w = 0;
	for (i = 1; i <= 8; i++)
	{
		for (j = 1; j <= 8; j++)
		{
			switch (board[i][j])					//��ӡ����
			{
				case BLACKONE: 	b++; break;
				case WHITEONE: 	w++; break;
			}
		}
	}
	putimage(-26, 340 - 26, &img[5]);
	putimage(37 * 4.8, 37 * 8.25, &img[6]);			//�������Ҫ��ס����
	putimage(37 * 5.9, 37 * 8.25, &img[7]);
	putimage(37 * 7, 37 * 8.25, &img[8]);
	TCHAR num_b[3] = _T(""), num_w[3] = _T(""), s[100] = _T("Black: ");
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
	int key;
	int a, b, x0, y0, i, j, n;
	bool flag = false, regret = false;
	while (true)
	{
		while (true)
		{
			m = GetMouseMsg();						//��ȡ�����Ϣ
			if (m.uMsg == WM_LBUTTONDOWN)			//���������
			{
				if (m.x - 26 < 37 * 8 && m.y - 26 < 37 * 8)
					break;							//������ ������ѭ�������ж�
				else if (m.x - 26 > 37 * 4.8 && m.x - 26 < 37 * 5.8 && m.y - 26 > 37 * 8.25 && m.y - 26 < 37 * 9.25)
				{
					key = MessageBox(wnd, _T("��ȷ��Ҫ����֮ǰ�Ĵ浵��"), _T("�浵"), MB_YESNO | MB_ICONQUESTION);
					if (key == IDYES)
					{
						if (comside == WHITEONE)
						{
							ofstream fout("material\\white_saving.txt");
							for (i = 1; i <= 8; i++)	//�浵
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
							for (i = 1; i <= 8; i++)	//�浵
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
					key = MessageBox(wnd, _T("��ȷ��Ҫ������"), _T("����"), MB_YESNO | MB_ICONQUESTION);
					if (key == IDYES) return true;	//����
				}
				else if (m.x - 26 > 37 * 7 && m.x - 26 < 37 * 8 && m.y - 26 > 37 * 8.25 && m.y - 26 < 37 * 9.25)
				{
					if (regret)
					{
						MessageBox(wnd, _T("���Ѿ��ڹ�����~"), _T("����"), MB_OK );
						continue;
					}
					key = MessageBox(wnd, _T("��ȷ��Ҫ������һ��ֻ�ܻ�һ��Ŷ~"), _T("����"), MB_YESNO | MB_ICONQUESTION);
					if (key == IDYES)				//����
					{
						for (i = 1; i <= 8; i++)
							for (j = 1; j <= 8; j++)
								board[i][j] = lastBoard[i][j];
						moves = lastmoves;
						strcpy_s(gamerecord, lastrecord);
						output();
						avail(-comside, 0);			//���ߵ��������̶��ص���һ��
						regret = true;
					}
				}
			}
		}
		a = (m.x - 26) / 37 + 1;
		b = (m.y - 26) / 37 + 1;
		for (n = 0; n<top[0]; n++)
			if (availPos[0][n][0] == a && availPos[0][n][1] == b)//����ǿ������ӵĵط� 
			{
				for (i = 1; i <= 8; i++)
					for (j = 1; j <= 8; j++)
						lastBoard[i][j] = board[i][j];//������һ��������
				lastmoves = moves;
				strcpy_s(lastrecord, gamerecord);
				while (availPos[0][n][0] == a && availPos[0][n][1] == b)
				{
					y0 = (availPos[0][n][2] + 6) % 3;//��ԭ���ԳԱ��˵ķ�λ  
					if (y0 == 2) y0 = -1;
					x0 = (availPos[0][n][2] - y0) / 3;
					board[a][b] = -comside;
					for (i = a + x0, j = b + y0; board[i][j] == comside; i += x0, j += y0)
						board[i][j] = -comside;		//�������λ�ϳԱ��� 
					n++;
				}
				output();
				if (comside == WHITEONE) putimage(37 * (a - 1), 37 * (b - 1), &img[3]);//����ر�ʶ����ǰ���ӵ� 3Ϊ�� 4Ϊ��
				else putimage(37 * (a - 1), 37 * (b - 1), &img[4]);
				flag = true;
				gamerecord[2 * moves] = a + '0';
				gamerecord[2 * moves + 1] = b + '0';
				break;
			}
		if (flag) break;
	}
	return false;									//δ���䣬��������
}

bool comFirst()										//�鿪�ֿ�
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
			else return 0;							//�����һ������
		}
	}

	int i, j, maxn = 0, sum, n, x0, y0, a, b, indexX = availPos[step][0][0], indexY = availPos[step][0][1];//���ӵ��г�ֵ��ֹ����
	bool flag = false;
	int **t = new int *[8];
	for (i = 0; i<8; i++)
		t[i] = new int[8];
	for (i = 1; i <= 8; i++)
		for (j = 1; j <= 8; j++)
			t[i - 1][j - 1] = board[i][j];			//�ڶ�̬�����п���һ�� 

	for (n = 0; n<top[step]; )
	{
		a = availPos[step][n][0];
		b = availPos[step][n][1];
		sum = value[a - 1][b - 1];
		while (availPos[step][n][0] == a && availPos[step][n][1] == b)
		{
			y0 = (availPos[step][n][2] + 6) % 3;	//��ԭ���ԳԱ��˵ķ�λ  ��һ�����ظ�����2 
			if (y0 == 2) y0 = -1;
			x0 = (availPos[step][n][2] - y0) / 3;
			board[a][b] = side;
			for (i = a + x0, j = b + y0; board[i][j] == -side; i += x0, j += y0)
			{
				board[i][j] = side;					//�������λ�ϳԱ���
				sum += 2 * value[i - 1][j - 1];		//�˴�sum=ͳ��(a,b)�¼������ӵ�������
			}
			n++;
		}
		if (moves <= 20) sum += 4 * top[step];
		else sum += 2 * top[step];					//һ�����ԣ���α�ж����������
		if (flag == false || sum > maxn)			//�˴����Ǽ�֦��
		{
			moves++;
			sum -= com(-side, step + 1);
			moves--;
			if (flag == false || sum > maxn)
			{
				maxn = sum;
				indexX = a;
				indexY = b;
				flag = true;						//maxn�ҵ��˳�ֵ 
			}
		}
		for (i = 1; i <= 8; i++)
			for (j = 1; j <= 8; j++)
				board[i][j] = t[i - 1][j - 1];		//����������û��ʹ��λ����
	}
	for (i = 0; i<8; i++)
		delete[] t[i];
	delete[] t;										//�����ռ�

	if (step == 0)									//����ǵ�һ�����ʶ�������ӵ�
	{
		LastX = indexX;								//��ͷ��Ҫ��LastX,LastY������ 
		LastY = indexY;
	}

	return maxn;									//�������Ž�
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
			else return comside == side ? -4 : 4;	//���һ������ 
		}
	}

	int i, j, maxn = 0, sum, n, x0, y0, a, b, indexX = availPos[step][0][0], indexY = availPos[step][0][1];//���ӵ��г�ֵ��ֹ����
	bool flag = false;

	int **t = new int *[8];
	for (i = 0; i<8; i++)
		t[i] = new int[8];
	for (i = 1; i <= 8; i++)
		for (j = 1; j <= 8; j++)
			t[i - 1][j - 1] = board[i][j];			//�ڶ�̬�����п���һ�� 

	for (n = 0; n<top[step]; )
	{
		a = availPos[step][n][0];
		b = availPos[step][n][1];
		sum = 1;
		while (availPos[step][n][0] == a && availPos[step][n][1] == b)
		{
			y0 = (availPos[step][n][2] + 6) % 3;	//��ԭ���ԳԱ��˵ķ�λ  ��һ�����ظ�����2 
			if (y0 == 2) y0 = -1;
			x0 = (availPos[step][n][2] - y0) / 3;
			board[a][b] = side;
			for (i = a + x0, j = b + y0; board[i][j] == -side; i += x0, j += y0)
			{
				board[i][j] = side;					//�������λ�ϳԱ���
				sum += 2;							//��󼸲�ÿ���ӵĹ��۶�һ�� 
			}
			n++;
		}
		if (flag == false || sum+4 > maxn)			//�˴����Ǽ�֦��
		{
			moves++;
			sum -= comLast(-side, step + 1);
			moves--;
			if (flag == false || sum > maxn)
			{
				maxn = sum;
				indexX = a;
				indexY = b;
				flag = true;						//maxn�ҵ��˳�ֵ 
			}
		}
		for (i = 1; i <= 8; i++)
			for (j = 1; j <= 8; j++)
				board[i][j] = t[i - 1][j - 1];		//����������û��ʹ��λ����
	}

	for (i = 0; i<8; i++)
		delete[] t[i];
	delete[] t;										//�����ռ�

	if (step == 0)									//����ǵ�һ�����ʶ�������ӵ�
	{
		LastX = indexX;								//��ͷ��Ҫ��LastX,LastY������ 
		LastY = indexY;
	}

	return maxn;									//�������Ž�
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

		clock_t start = clock();					//�ü������������������֣��Ա���ҿ����
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
		case 0: key = MessageBox(wnd, _T("ƽ����ɫ~~~����һ����"), _T("�;�"), MB_YESNO | MB_ICONQUESTION); break;
		case 1: key = MessageBox(wnd, _T("��ϲ����Ӯ�ˣ�����һ����"), _T("ʤ��"), MB_YESNO | MB_ICONQUESTION); break;
		case -1: key = MessageBox(wnd, _T("���ź��������ˡ�����һ����"), _T("ʧ��"), MB_YESNO | MB_ICONQUESTION); break;
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
		if (top[0])									//����еط���
		{
			giveup = ply();
			if (giveup)								//���������
			{
				count(true);
				return;
			}
		}
		else
		{
			gamerecord[2 * moves] = '0';			//û�ط������¼0,0
			gamerecord[2 * moves + 1] = '0';
			flag = true;							//�Ȼ��ж��ǲ��ǵ���Ҳû�еط���
		}
		moves++;
		
		avail(comside, 0);
		if (top[0])
		{
			textclean();
			outtextxy(37 * 0.2, 37 * 9.5, thinking);//�������˼��
			clock_t start = clock();				//�ü�������ٵ�һ�������ӣ��Ա���ҿ����
			if (!comFirst())
			{
				checklib = false;
				if (moves <= 46) com(comside, 0);
				else comLast(comside, 0);
			}
			gamerecord[2 * moves] = LastX + '0';	//��¼����
			gamerecord[2 * moves + 1] = LastY + '0';
			_itow_s(clock() - start, thoughtTime, 10);
			while (clock() - start < CLOCKS_PER_SEC) ;//�ü�������ٵ�һ�������ӣ��Ա���ҿ����
			textclean();
			lstrcpy(s, _T("Computer has thought for "));
			lstrcat(s, thoughtTime);
			lstrcat(s, _T(" ms."));
			if (checklib) lstrcat(s, _T(" (BOOK)"));

			for (n = 0; n<top[0]; n++)				//�������ظ�����3 
				if (availPos[0][n][0] == LastX && availPos[0][n][1] == LastY)//����ǿ������ӵĵط� 
				{
					while (availPos[0][n][0] == LastX && availPos[0][n][1] == LastY)
					{
						y0 = (availPos[0][n][2] + 6) % 3;//��ԭ���ԳԱ��˵ķ�λ  
						if (y0 == 2) y0 = -1;
						x0 = (availPos[0][n][2] - y0) / 3;
						board[LastX][LastY] = comside;
						for (i = LastX + x0, j = LastY + y0; board[i][j] == -comside; i += x0, j += y0)
							board[i][j] = comside;	//�������λ�ϳԱ��� 
						n++;
					}
					output();
					if (comside == WHITEONE) putimage(37 * (LastX - 1), 37 * (LastY - 1), &img[4]);//����ر�ʶ����ǰ���ӵ�
					else putimage(37 * (LastX - 1), 37 * (LastY - 1), &img[3]);
					if (flag)
					{
						textclean();
						outtextxy(37 * 0.2, 37 * 9, _T("No move available for you."));
					}
					break;
				}
			outtextxy(37 * 0.2, 37 * 9.5, s);		//���˼��ʱ��
		}
		else
		{
			gamerecord[2 * moves] = '0';
			gamerecord[2 * moves + 1] = '0';		//�������ӿ���
			textclean();
			if (flag)								//˫�������ӿ���
			{
				outtextxy(37 * 0.2, 37 * 9.5, _T("No move available for you."));
				outtextxy(37 * 0.2, 37 * 10.5, _T("No move available for computer."));
				count(false);
				moves++;
				return;
			}
			else
			{										//���������ӿ���
				outtextxy(37 * 0.2, 37 * 9.5, _T("No move available for computer."));
			}
		}
		moves++;
	}
}