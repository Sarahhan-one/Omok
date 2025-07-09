#pragma once
#include "Board.h"
#include "Display.h"
#include <SFML/Network.hpp>
using namespace sf;

class GameManager {
public:
	enum class Role { SERVER, CLIENT };
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

	// networking members and methods
	Role role; 
	TcpSocket socket;
	TcpListener listener; 
	IpAddress serverIp;
	bool isMyTurn;

	void setupServer();
	void setupClient(); 
	void waitForOpponent();

	bool playAgain();
	void resetGame();


public:
	GameManager(Role role);
	void run();
};