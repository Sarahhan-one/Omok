#include "Board.h"

const char EMPTY_SLOT = '+';

Board::Board(int width, int height) : width(width), height(height) {
	init();
}

void Board::init() {
	grid.resize(height, vector<char>(width, EMPTY_SLOT));
}

char Board::getStoneAt(int row, int col) const {
	//check for boundary
	if (row < 0 || row >= height || col < 0 || col >= width) {
		return '\0';
	}
	return grid[row][col];
}

void Board::setStoneAt(int row, int col, char stone) {
	if (row >= 0 && row < height && col >= 0 && col < width) {
		grid[row][col] = stone;
	}
}

bool Board::isOccupied(int row, int col) const {
	char stone = getStoneAt(row, col); 
	return (stone != EMPTY_SLOT && stone != '\0');
}