#include "stdafx.h"
#include "board.h"
#include <string.h>
#include <stdio.h>
#include <time.h>
#include <math.h>

const int Board::dir[8][2] = 
{ 
	{-1, -1}, {-1, 0}, {-1, 1}, {0, -1}, {0, 1}, {1, -1}, {1, 0}, {1, 1} 
};

Board::Board(int s, bitboard b, bitboard w)
{
	side = s;
	board[BLACKONE] = b;
	board[WHITEONE] = w;
	locateValid();
}
void Board::locateValid()
{
	bitboard avail = ~(board[BLACKONE] | board[WHITEONE]);
	validTop = 0;

	int i, j;
	bitboard temp, sum;
	for (int i0 = 0; i0 < 8; ++i0)
		for (int j0 = 0; j0 < 8; ++j0)
		{
			if (avail & 1ll)								//这个位置可以走 
			{
				sum = 0;
				for (int d = 0; d < 8; ++d)
				{
					temp = 0;
					i = i0 + dir[d][0];
					j = j0 + dir[d][1];
					if (OUTOFBOARD(i, j) || !(board[!side] & POS(i, j))) continue;//某个方位上第一颗得是别人的棋子 
					do
					{
						temp |= POS(i, j);
						i += dir[d][0];
						j += dir[d][1];
					} while (!OUTOFBOARD(i, j) && (board[!side] & POS(i, j)));
					if (!OUTOFBOARD(i, j) && (board[side] & POS(i, j)))		//之后一直是别人的棋子 直到出现自己的棋子 
					{
						sum |= temp;					//记录下这个方向上能吃的子
						validPos[validTop][0] = i0;
						validPos[validTop][1] = j0;
					}
				}
				if (sum)
				{
					newboard[validTop][side] = board[side] | sum | POS(i0, j0);
					newboard[validTop][!side] = board[!side] & ~sum;
					++validTop;
				}
			}
			avail >>= 1;
		}
}
bool Board::makeMove(int a, int b, Board &newone)
{
	if (a != -1 && b != -1)
	{
		for (int n = 0; n < validTop; ++n)
			if (validPos[n][0] == a && validPos[n][1] == b)//如果是可以落子的地方 
			{
				newone.side = !side;
				newone.board[BLACKONE] = newboard[n][BLACKONE];
				newone.board[WHITEONE] = newboard[n][WHITEONE];
				newone.locateValid();
				return true;
			}
		return false;
	}
	else
	{
		newone.side = !side;
		newone.board[BLACKONE] = board[BLACKONE];
		newone.board[WHITEONE] = board[WHITEONE];
		newone.locateValid();
		return true;
	}
}
void Board::makeMoveByIndex(int n, Board &newone)
{
	newone.side = !side;
	if (n != -1)
	{
		newone.board[BLACKONE] = newboard[n][BLACKONE];
		newone.board[WHITEONE] = newboard[n][WHITEONE];
	}
	else
	{
		newone.board[BLACKONE] = board[BLACKONE];
		newone.board[WHITEONE] = board[WHITEONE];
	}
	newone.locateValid();
}
int Board::ABNegaMax(int depth, int alpha, int beta, int lastmove[], int stage)
{
	static const int maxDepth[2] = { 8, 8 };
	if (stage < 2 && depth == maxDepth[stage])
		return evaluate(stage);

	int val;
	Board newone;
	if (stalemate())
	{
		makeMoveByIndex(-1, newone);
		if (depth == 0)
		{
			lastmove[0] = -1;
			lastmove[1] = -1;
		}
		if (newone.stalemate())
		{
			val = countOne(board[side]) - countOne(board[!side]);
			if (stage != 2) return val > 0 ? (1000000 - 1) : -(1000000 - 1);
			else return val;
		}
		else
			return -newone.ABNegaMax(depth + 1, -beta, -alpha, lastmove, stage);
	}
	for (int n = 0; n < validTop; ++n)
	{
		makeMoveByIndex(n, newone);
		val = -newone.ABNegaMax(depth + 1, -beta, -alpha, lastmove, stage);
		if (val >= beta) return val;
		if (val > alpha)
		{
			alpha = val;
			if (depth == 0)
			{
				lastmove[0] = validPos[n][0];
				lastmove[1] = validPos[n][1];
			}
		}
	}
	return alpha;
}
int Board::evaluate(int stage)
{
	static const int corner[4][2] =
	{
		{0, 0}, {0, 7}, {7, 0}, {7, 7}
	};
	static const int cornerNeighbor[4][4][2] =
	{
		{ {0, 1}, {1, 0}, {1, 1}, {1, 1} },
		{ {0, 6}, {1, 7}, {1, 6}, {1, 6} },
		{ {6, 0}, {7, 1}, {6, 1}, {6, 1} },
		{ {6, 7}, {7, 6}, {6, 6}, {6, 6} }
	};
	static const int cornerValue[2] = { 50, 40 }, cornerNeighborValue[2] = { -12, -3 }, edgeValue[2] = { 2, 3 },
		stableValue[2] = { 0, 0 }, unstableValue[2] = { -3, -2 }, mobilityValue[2] = { 4, 2 };
	int sum = 0;

	for (int t = 0; t < 4; ++t)
	{
		int s = -1;
		if (board[side] & POS(corner[t][0], corner[t][1]))
		{
			s = side;
		}
		else if (board[!side] & POS(corner[t][0], corner[t][1]))
		{
			s = !side;
		}
		if (s != -1)
		{
			int temp = cornerValue[stage];
			if (corner[t][0] == 0)
			{
				for (int i = 0; i < 8; ++i)
				{
					if (board[s] & POS(corner[t][0], i))
						temp += edgeValue[stage];
					else break;
				}
			}
			else
			{
				for (int i = 7; i >= 0; --i)
				{
					if (board[s] & POS(corner[t][0], i))
						temp += edgeValue[stage];
					else break;
				}
			}
			if (corner[t][1] == 0)
			{
				for (int i = 0; i < 8; ++i)
				{
					if (board[s] & POS(i, corner[t][1]))
						temp += edgeValue[stage];
					else break;
				}
			}
			else
			{
				for (int i = 7; i >= 0; --i)
				{
					if (board[s] & POS(i, corner[t][1]))
						temp += edgeValue[stage];
					else break;
				}
			}

			if (s == side) sum += temp;
			else sum -= temp;
		}
		else
		{
			for (int i = 0; i < 4; ++i)
			{
				if (board[side] & POS(cornerNeighbor[t][i][0], cornerNeighbor[t][i][1]))
					sum += cornerNeighborValue[stage];
				else if (board[!side] & POS(cornerNeighbor[t][i][0], cornerNeighbor[t][i][1]))
					sum -= cornerNeighborValue[stage];
			}
		}
	}

	bitboard avail = ~(board[side] | board[!side]);
	for (int i0 = 1; i0 < 7; ++i0)
		for (int j0 = 1; j0 < 7; ++j0)
		{
			if (avail & POS(i0, j0))
			{
				for (int d = 0; d < 8; ++d)
					if (board[side] & POS(i0 + dir[d][0], j0 + dir[d][1]))
					{
						sum += unstableValue[stage];
						break;
					}
				for (int d = 0; d < 8; ++d)
					if (board[!side] & POS(i0 + dir[d][0], j0 + dir[d][1]))
					{
						sum -= unstableValue[stage];
						break;
					}
			}
		}

	Board newone;
	makeMoveByIndex(-1, newone);
	sum += (validTop - newone.validTop) * mobilityValue[stage];
	
	return sum;
}
int Board::searchFull(int depth, int &win, int &draw, int &lose, int lastmove[])
{
	win = draw = lose = 0;
	int newwin, newdraw, newlose, result;
	Board newone;

	if (stalemate())
	{
		makeMoveByIndex(-1, newone);
		if (depth == 0)
		{
			lastmove[0] = -1;
			lastmove[1] = -1;
		}
		if (newone.stalemate())
			result = countResult();
		else
			result = -newone.searchFull(depth + 1, newwin, newdraw, newlose, lastmove);
		switch (result)
		{
			case 1: win++; break;
			case 0: draw++; break;
			case -1: lose++; break;
		}
		return result;
	}
	else
	{
		double loseratio = 1.1, drawratio = 1.1;
		bool flag = false;
		for (int n = 0; n < validTop; ++n)
		{
			makeMoveByIndex(n, newone);//遍历所有子局面
			result = newone.searchFull(depth + 1, newwin, newdraw, newlose, lastmove);
			if (result == 1)//是一个先手胜的子局面
			{
				win++;
				if (!flag && (double)newlose / max(newone.validTop, 1) < loseratio)
				{//如果全是先手胜的子局面，也要尽量找到这样一个先手胜的子局面，它的先手败的子局面的比例较小
					loseratio = (double)newlose / max(newone.validTop, 1);
					if (depth == 0)
					{
						lastmove[0] = validPos[n][0];
						lastmove[1] = validPos[n][1];
					}
				}
			}
			else if (result == 0)//是一个先手和的子局面
			{
				draw++;
				flag = true;//如果没有先手败的子局面，也要尽量找到这样一个先手和的子局面，它的先手和的子局面的比例较小
				if ((double)newdraw / max(newone.validTop, 1) < drawratio)
				{
					drawratio = (double)newdraw / max(newone.validTop, 1);
					if (depth == 0)
					{
						lastmove[0] = validPos[n][0];
						lastmove[1] = validPos[n][1];
					}
				}
			}
			else //是一个先手败的子局面，那么停止搜索，就这么走
			{
				if (depth == 0)
				{
					lastmove[0] = validPos[n][0];
					lastmove[1] = validPos[n][1];
				}
				return 1;
			}
		}
		if (flag) return 0;
		else return -1;
	}
}

int Board::totNodes = 0;
int Board::trie[MAX_BOOKSIZE][64] = { 0 };
bool Board::loadBook()
{
	srand((unsigned)time(0));
	FILE *fp;
	char s[100];
	fopen_s(&fp, "material\\book.txt", "r");
	if (fp)
	{
		int now, len, index;
		while (fscanf_s(fp, "%s", s, 100) > 0)
		{
			now = 0;	// now is the current position of the trie. 
			len = strlen(s);
			for (int i = 0; i < len; i += 2)
			{
				index = (s[i] - '1') * 8 + s[i + 1] - '1';
				if (!trie[now][index]) trie[now][index] = ++totNodes;
				// if there is no node exist, we need to create one 
				now = trie[now][index];
			}
		}
		fclose(fp);
		return true;
	}
	else
		return false;
}
bool Board::searchBook(int *gamerecord, int moves, int lastmove[])
{
	int now = 0, index;
	for (int i = 0; i < moves; ++i)
	{
		index = gamerecord[2 * i] * 8 + gamerecord[2 * i + 1];
		if (!trie[now][index]) return false;
		now = trie[now][index];
	}
	index = rand() % validTop;
	int i = 0;
	while (!trie[now][validPos[index][0] * 8 + validPos[index][1]])
	{
		index = (index + 1) % validTop;
		++i;
		if (i == validTop) return false;
	}
	lastmove[0] = validPos[index][0];
	lastmove[1] = validPos[index][1];
	return true;
}
int Board::randToEnd()
{
	bool flag;
	Board newone;
	int myside = side;
	while (true)
	{
		flag = false;
		if (!stalemate())
		{
			makeMoveByIndex(rand() % validTop, newone);
		}
		else
		{
			makeMoveByIndex(-1, newone);
			flag = true;
		}
		if (!newone.stalemate())
		{
			newone.makeMoveByIndex(rand() % newone.validTop, *this);
		}
		else
		{
			newone.makeMoveByIndex(-1, *this);
			if (flag)
			{
				int result = countResult();
				if (getSide() != myside) result = -result;
				return result > 0;
			}
		}
	}
}
int Board::countResult()
{
	int n = countOne(board[side]) - countOne(board[!side]);
	if (n > 0) return 1;
	else if (n == 0) return 0;
	else return -1;
}

int UCTNode::totNodes = 0;
UCTNode* UCTNode::UCT = NULL;
const int UCTNode::dir[8][2] = 
{
	{ -1, -1 }, { -1, 0 }, { -1, 1 }, { 0, -1 }, { 0, 1 }, { 1, -1 }, { 1, 0 }, { 1, 1 }
};

void UCTNode::UCTsearch(Board s, int lastmove[])
{
	const int t = 2000000;
	UCT = new UCTNode[t + 100];
	totNodes = 1;
	UCT[1].side = s.getSide();
	UCT[1].board[BLACKONE] = s.getBoard(BLACKONE);
	UCT[1].board[WHITEONE] = s.getBoard(WHITEONE);
	UCT[1].count = UCT[1].value = 0;
	UCT[1].parent = UCT[1].parentStalemate = 0;
	UCT[1].expand(1);
	int index, delta;
	clock_t start = clock();
	while ((clock() - start) < 10 * CLOCKS_PER_SEC && 
		totNodes < t)
	{
		index = treePolicy();
		delta = UCT[index].defaultPolicy();
		backUp(index, delta);
	}

	double ans, maxn = -1000000;
	for (int i = 0; i < UCT[1].childTop; ++i)
	{
		ans = (double)UCT[UCT[1].child[i]].value / UCT[UCT[1].child[i]].count;
		if (ans > maxn)
		{
			maxn = ans;
			index = i;
		}
	}
	index = UCT[1].child[index];
	bitboard pos = UCT[index].board[BLACKONE] | UCT[index].board[WHITEONE];
	pos &= ~(UCT[1].board[BLACKONE] | UCT[1].board[WHITEONE]);
	int sum = 0;
	while ((pos & 1ll) == 0)
	{
		sum++;
		pos >>= 1;
	}
	lastmove[0] = sum / 8;
	lastmove[1] = sum % 8;
	delete[] UCT;
}
int UCTNode::treePolicy()
{
	int cur = 1;
	while (UCT[cur].parentStalemate < 2)
	{
		for (int i = 0; i < UCT[cur].childTop; ++i)
			if (UCT[UCT[cur].child[i]].count == 0)
			{
				expand(UCT[cur].child[i]);
				return UCT[cur].child[i];
			}
		cur = UCT[cur].bestChild();
	}
	return cur;
}
void UCTNode::expand(int cur)
{
	bitboard avail = ~(UCT[cur].board[BLACKONE] | UCT[cur].board[WHITEONE]);
	UCT[cur].childTop = 0;

	int i, j;
	bitboard temp, sum;
	for (int i0 = 0; i0 < 8; ++i0)
		for (int j0 = 0; j0 < 8; ++j0)
		{
			if (avail & 1ll)								//这个位置可以走 
			{
				sum = 0;
				for (int d = 0; d < 8; ++d)
				{
					temp = 0;
					i = i0 + dir[d][0];
					j = j0 + dir[d][1];
					if (OUTOFBOARD(i, j) || !(UCT[cur].board[!UCT[cur].side] & POS(i, j))) continue;//某个方位上第一颗得是别人的棋子 
					do
					{
						temp |= POS(i, j);
						i += dir[d][0];
						j += dir[d][1];
					} while (!OUTOFBOARD(i, j) && (UCT[cur].board[!UCT[cur].side] & POS(i, j)));
					if (!OUTOFBOARD(i, j) && (UCT[cur].board[UCT[cur].side] & POS(i, j)))		//之后一直是别人的棋子 直到出现自己的棋子 
					{
						sum |= temp;
					}
				}
				if (sum)
				{
					++totNodes;
					UCT[totNodes].side = !UCT[cur].side;
					UCT[totNodes].board[UCT[cur].side] = UCT[cur].board[UCT[cur].side] | sum | POS(i0, j0);
					UCT[totNodes].board[!UCT[cur].side] = UCT[cur].board[!UCT[cur].side] & ~sum;
					UCT[totNodes].count = UCT[totNodes].value = 0;
					UCT[totNodes].parent = cur;
					UCT[totNodes].parentStalemate = 0;

					UCT[cur].child[UCT[cur].childTop++] = totNodes;
				}
			}
			avail >>= 1;
		}
	if (UCT[cur].childTop == 0)
	{
		++totNodes;
		UCT[totNodes].side = !UCT[cur].side;
		UCT[totNodes].board[BLACKONE] = UCT[cur].board[BLACKONE];
		UCT[totNodes].board[WHITEONE] = UCT[cur].board[WHITEONE];
		UCT[totNodes].count = UCT[totNodes].value = 0;
		UCT[totNodes].parent = cur;
		UCT[totNodes].parentStalemate = UCT[cur].parentStalemate + 1;

		UCT[cur].child[UCT[cur].childTop++] = totNodes;
	}
}
inline int UCTNode::defaultPolicy()
{
	Board temp(side, board[BLACKONE], board[WHITEONE]);
	return temp.randToEnd();
}
int UCTNode::bestChild()
{
	int index = 0;
	double maxn = -1000000, ans, doublelogcount = 2 * log(count);
	for (int i = 0; i < childTop; ++i)
	{
		ans = (double)UCT[child[i]].value / UCT[child[i]].count + BESTCHILDRATIO * sqrt(doublelogcount / UCT[child[i]].count);
		if (ans > maxn)
		{
			maxn = ans;
			index = i;
		}
	}
	return child[index];
}
void UCTNode::backUp(int cur, int delta)
{
	while (cur)
	{
		UCT[cur].count++;
		UCT[cur].value += delta;
		delta = !delta;
		cur = UCT[cur].parent;
	}
}