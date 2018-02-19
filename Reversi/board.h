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

	void locateValid();//记录可以落子的位置并保存在validPos中
	int evaluate(int stage);//局面评估函数
	void makeMoveByIndex(int n, Board &newone);//按给定的在availPos中的序号落子
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
	bool stalemate()//是否无子可落
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
	bool makeMove(int a, int b, Board &newone);//落子，并把新局面保存在newone中，记录newone可以落子的位置
	int ABNegaMax(int depth, int alpha, int beta, int lastmove[], int stage);//alphabeta搜索
	int searchFull(int depth, int &win, int &draw, int &lose, int lastmove[]);//残局完全搜索，优先选择胜率高的走法，可以在落后的情况下尽量搅局
	bool searchBook(int *gamerecord, int moves, int lastmove[]);//查找开局库，随机选择走法
	int randToEnd();
	static bool loadBook();//加载开局库
	int countResult();
	//TODO:如果开局库被篡改
};
class UCTNode//蒙特卡洛法
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