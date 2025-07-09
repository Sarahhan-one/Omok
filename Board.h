#pragma once

#include <vector>
using namespace std;

class Board {
private:
	vector<vector<char>> grid; 
	int width;
	int height;

public:
	Board(int width, int height);

	void init(); 
	bool isOccupied(int row, int col) const; 
	char getStoneAt(int row, int col) const;
	void setStoneAt(int row, int col, char stone);
	
	int getWidth() const { return width; }
	int getHeight() const { return height; }
};