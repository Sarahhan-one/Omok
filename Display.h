#pragma once
#include "Board.h"
#include <Windows.h>
#include <iostream>

class Display {
public:
	void drawBoard(const Board& board);
	void updateScreen(const Board& board, int oldRow, int oldCol, int newRow, int newCol, bool isBlackTurn);
	void showWinner(bool isBlackTurn);
	void drawStone(int row, int col, char stone);

	static void setColor(int color);
	static void move(int x, int y);
};