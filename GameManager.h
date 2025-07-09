#pragma once
#include "Board.h"
#include "Display.h"

class GameManager {
private:
	Board board; 
	Display display; 
	bool isBlackTurn; 
	int currentRow;
	int currentCol; 
	int prevRow;
	int prevCol;
	bool isBattleEnded; 

	void handleInput();
	void placeStone();
	bool isWinningMove(int row, int col, char stone);
	void switchTurn();
	void displayWinner();

public:
	GameManager();
	void run();
};