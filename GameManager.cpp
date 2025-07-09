#include "GameManager.h"
#include <Windows.h>
#include <conio.h>

constexpr int BOARD_SIZE = 15; 
constexpr int BLACK_STONE = 'B';
constexpr int WHITE_STONE = 'W';

GameManager::GameManager() : board(BOARD_SIZE, BOARD_SIZE), isBlackTurn(true), isBattleEnded(false) {
	currentRow = board.getHeight() / 2;
	currentCol = board.getWidth() / 2;

	prevRow = currentRow;
	prevCol = currentCol;
}

void GameManager::handleInput() {
	if (_kbhit()) { // key has been pressed
		int key = _getch();
		if (key == 224) {
			key = _getch();
			switch (key) {
			case 72: if (currentRow > 0) currentRow--; break; // Up
			case 80: if (currentRow < board.getHeight() - 1) currentRow++; break; // Down
			case 75: if (currentCol > 0) currentCol--; break; // Left
			case 77: if (currentCol < board.getWidth() - 1) currentCol++; break; // Right
			}
		}
		else if (key == 32) { // Spacebar
			placeStone();
		}
	}
}

bool GameManager::isWinningMove(int row, int col, char stone) {
    int directions[4][2] = {{1, 0}, {0, 1}, {1, 1}, {1, -1}}; 

    for (auto& dir : directions) {
        int count = 1;
        // Check in one direction
        for (int i = 1; i < 5; ++i) {
            if (board.getStoneAt(row + i * dir[0], col + i * dir[1]) == stone) count++;
            else break;
        }
        // Check in the opposite direction
        for (int i = 1; i < 5; ++i) {
            if (board.getStoneAt(row - i * dir[0], col - i * dir[1]) == stone) count++;
            else break;
        }
        if (count >= 5) return true;
    }
    return false;
}

void GameManager::placeStone() {
	if (board.isOccupied(currentRow, currentCol)) {
		return;
	}

	const char currentStone = isBlackTurn ? BLACK_STONE : WHITE_STONE;
	board.setStoneAt(currentRow, currentCol, currentStone);

	// After updating the board data, immediately draw the stone permanently.
	display.drawStone(currentRow, currentCol, currentStone);

	if (isWinningMove(currentRow, currentCol, currentStone)) {
		isBattleEnded = true;
	}
	else {
		switchTurn();
	}
}

void GameManager::switchTurn() {
	isBlackTurn = !isBlackTurn;
}

void GameManager::run() {
	system("cls");
	display.drawBoard(board);

	while (!isBattleEnded) {
		// Only redraw the cursor if it has moved.
		if (currentRow != prevRow || currentCol != prevCol) {
			display.updateScreen(board, prevRow, prevCol, currentRow, currentCol, isBlackTurn);
			prevRow = currentRow;
			prevCol = currentCol;
		}
		handleInput();
		Sleep(100);
	}
	
	displayWinner();
	_getch();
}

void GameManager::displayWinner() {
	display.showWinner(isBlackTurn);
}

