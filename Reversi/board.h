//#pragma once
#ifndef _BOARD_H_
#define _BOARD_H_

#define BLACKONE 0
#define WHITEONE 1
#define POS(x, y) (1ll << ((x<<3)+y))
#define OUTOFBOARD(x, y) ((x)<0 || (x)>=8 || (y)<0 || (y)>=8)
#define MAX_BOOKSIZE 6000
#define BESTCHILDRATIO 1.0
//TODO
typedef long long bitboard;

class Board
{
private:
	bitboard board[2];
	int side;
	int validPos[32][2];//TODO
	bitboard newboard[32][2];
	int validTop;
	static const int dir[8][2];
	static int totNodes;
	static int trie[MAX_BOOKSIZE][64];

	void locateValid();//��¼�������ӵ�λ�ò�������validPos��
	int evaluate(int stage);//������������
	void makeMoveByIndex(int n, Board &newone);//����������availPos�е��������
	static int countOne(bitboard n)
	{
		n = (n & 0x5555555555555555ll) + ((n >> 1) & 0x5555555555555555ll);
		n = (n & 0x3333333333333333ll) + ((n >> 2) & 0x3333333333333333ll);
		n = (n & 0x0f0f0f0f0f0f0f0fll) + ((n >> 4) & 0x0f0f0f0f0f0f0f0fll);
		n = (n & 0x00ff00ff00ff00ffll) + ((n >> 8) & 0x00ff00ff00ff00ffll);
		n = (n & 0x0000ffff0000ffffll) + ((n >> 16) & 0x0000ffff0000ffffll);
		n = (n & 0x00000000ffffffffll) + ((n >> 32) & 0x00000000ffffffffll);
		return (int)n;
	}
public:
	Board() {}
	Board(int s, bitboard b, bitboard w);
	bool stalemate()//�Ƿ����ӿ���
	{
		return validTop == 0;
	}
	int getSide()
	{
		return side;
	}
	bitboard getBoard(int index)
	{
		return board[index];
	}
	int *getValidPos()
	{
		return &validPos[0][0];
	}
	int getValidTop()
	{
		return validTop;
	}
	bool makeMove(int a, int b, Board &newone);//���ӣ������¾��汣����newone�У���¼newone�������ӵ�λ��
	int ABNegaMax(int depth, int alpha, int beta, int lastmove[], int stage);//alphabeta����
	int searchFull(int depth, int &win, int &draw, int &lose, int lastmove[]);//�о���ȫ����������ѡ��ʤ�ʸߵ��߷�����������������¾�������
	bool searchBook(int *gamerecord, int moves, int lastmove[]);//���ҿ��ֿ⣬���ѡ���߷�
	int randToEnd();
	static bool loadBook();//���ؿ��ֿ�
	int countResult();
	//TODO:������ֿⱻ�۸�
};
class UCTNode//���ؿ��巨
{
private:
	int side;
	bitboard board[2];
	int count, value;
	int parent, parentStalemate, child[32], childTop;
	static int totNodes;
	static UCTNode* UCT;
	static const int dir[8][2];

	static int treePolicy();
	static void expand(int cur);
	inline int defaultPolicy();
	int bestChild();
	static void backUp(int cur, int delta);
public:
	static void UCTsearch(Board s, int lastmove[]);
};
#endif