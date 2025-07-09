#include "Display.h"

void Display::move(int x, int y) {
	COORD pos = { x, y };
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);
}

void Display::drawBoard(const Board& board) {
	//system("cls");
	move(board.getWidth() - 2, 0);
	printf("Omok\n");

	for (int i = 0; i < board.getHeight(); i++) {
		move(3, i + 3);
		for (int j = 0; j < board.getWidth(); j++) {
			printf("%c", board.getStoneAt(i, j));
		}
	}
}

void Display::drawStone(int row, int col, char stone) {
	move(3 + col, 3 + row);
	printf("%c", stone);
}

void Display::setColor(int color) {
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);
}

void Display::updateScreen(const Board& board, int oldRow, int oldCol, int newRow, int newCol, bool isBlackTurn) {
	drawBoard(board);

	// erase the cursor from its old position
	move(3 + oldCol, 3 + oldRow); 
	setColor(7); //default color
	printf("%c", board.getStoneAt(oldRow, oldCol));

	// draw the new cursor
	move(3 + newCol, newRow + 3);
	isBlackTurn ? setColor(FOREGROUND_GREEN) : setColor(FOREGROUND_RED);
	printf("%c", board.getStoneAt(newRow, newCol)); // print the stone under the cursor
	
	setColor(7);

	move(0, board.getHeight() + 4);
	
	if (isBlackTurn) cout << "Black's Turn | X, Y : " << newCol + 1 << ", " << newRow + 1 << "  " << flush;
	else cout << "White's Turn | X, Y : " << newCol + 1 << ", " << newRow + 1 << "  " << flush;
	
	//// Leave the cursor at the end of the line to prevent it from blinking on the board 
	//move(0, board.getHeight() + 5);
}

void Display::showWinner(bool isBlackTurn) {
	move(0, 22); 
	if (isBlackTurn) printf("BLACK WINS!\n");
	else printf("WHITE WINS!\n");

}