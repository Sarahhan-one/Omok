#include "Display.h"

void Display::move(int x, int y) {
	COORD pos = { x, y };
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);
}

void Display::drawBoard(const Board& board) {
	//system("cls");
	move(board.getWidth() - 2, 0);
	printf("Let's Play Omok\n");

	for (int i = 0; i < board.getHeight(); i++) {
		move(3, i + 3);
		for (int j = 0; j < board.getWidth(); j++) {
			printf("%c", board.getStoneAt(i, j));
		}
	}
}

void Display::drawStone(int row, int col, char stone) {
	move(3 + col * 2, 3 + row);
	printf("%c", stone);
}

void Display::setColor(int color) {
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);
}

void Display::updateScreen(const Board& board, int oldRow, int oldCol, int newRow, int newCol, bool isBlackTurn) {
	drawBoard(board);

	// erase the cursor from its old position
	move(3 + oldCol * 2, oldRow + 3); 
	setColor(7);
	printf("%c", board.getStoneAt(oldRow, oldCol));

	// draw the new cursor
	move(3 + newCol * 2, newRow + 3);
	isBlackTurn ? setColor(FOREGROUND_GREEN) : setColor(FOREGROUND_RED);
	printf("%c", board.getStoneAt(newRow, newCol)); // print the stone under the cursor
	
	setColor(7);

	move(0, board.getHeight() + 4);
	
	if (isBlackTurn) printf("Black's Turn | X , Y : %2d %2d", newCol + 1, newRow + 1);
	else printf("White's Turn | X , Y : %2d %2d", newCol + 1, newRow + 1);
	
	// Leave the cursor at the end of the line to prevent it from blinking
	move(0, board.getHeight() + 5);
}

void Display::showWinner(bool isBlackTurn) {
	move(0, 22); 
	if (isBlackTurn) printf("BLACK WINS!\n");
	else printf("WHITE WINS!\n");

}