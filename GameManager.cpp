#include "GameManager.h"
#include <Windows.h>
#include <conio.h>
#include <iostream>
using namespace sf;

const int BOARD_SIZE = 15; 
const int BLACK_STONE = 'B';
const int WHITE_STONE = 'W';

const int PORT = 53000;

GameManager::GameManager(Role role) : board(BOARD_SIZE, BOARD_SIZE), role(role), isBlackTurn(true), isBattleEnded(false) {
	currentRow = board.getHeight() / 2;
	currentCol = board.getWidth() / 2;

	prevRow = currentRow;
	prevCol = currentCol;

	// setup roles and turns 
	if (role == Role::SERVER) {
		setupServer();
		isMyTurn = true; 
	}
	else {
		setupClient();
		isMyTurn = false;
	}
}

void GameManager::setupServer() {
	cout << "Starting as [Server]. Waiting for a client to connect...\n";

	if (listener.listen(PORT) != Socket::Done) {
		cerr << "Error: Could not listen on port " << PORT << '\n';
		exit(1);
	}
	
	if (listener.accept(socket) != Socket::Done) {
		cerr << "Error: Could not accept client connection.\n";
		exit(1);
	}

	cout << "Client connected from " << socket.getRemoteAddress() << '\n';
}

void GameManager::setupClient() {
	cout << "Starting as [Client]. Enter the server's IP address: ";
	cin >> serverIp;

	Socket::Status status = socket.connect(serverIp, PORT);

	if (status != Socket::Done) {
		cerr << "Error: Could not connect to server at " << serverIp.toString() << " : " << PORT << '\n';
		exit(1);
	}

	cout << "Successfully connected to the server!\n";
}

void GameManager::waitForOpponent() {
	Display::move(0, board.getHeight() + 4);
	cout << "Waiting for opponent's move...          " << flush;

	Packet packet; 
	if (socket.receive(packet) != Socket::Done) {
		cerr << "Error receiving move or opponent disconnected.\n";
		isBattleEnded = true;
		return;
	}

	int receivedRow, receivedCol;
	if (packet >> receivedRow >> receivedCol) {
		const char opponentStone = isBlackTurn ? BLACK_STONE : WHITE_STONE;
		board.setStoneAt(receivedRow, receivedCol, opponentStone);
		display.drawStone(receivedRow, receivedCol, opponentStone);

		// The cursor should follow the last move
		currentRow = receivedRow;
		currentCol = receivedCol;
		prevRow = receivedRow;
		prevCol = receivedCol;

		if (isWinningMove(receivedRow, receivedCol, opponentStone)) {
			isBattleEnded = true;
		}
		else {
			switchTurn();
			isMyTurn = true; // end of my turn
		}
	}

}

void GameManager::handleInput() {
	if (_kbhit()) { // key has been pressed
		int key = _getch();
		if (key == 224) {
			key = _getch();
			switch (key) {
			case 72: if (currentRow > 0) { currentRow--; break; } // Up
			case 80: if (currentRow < board.getHeight() - 1) { currentRow++; break; } // Down
			case 75: if (currentCol > 0) { currentCol--; break; } // Left
			case 77: if (currentCol < board.getWidth() - 1) { currentCol++; break; } // Right
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

	// send move to opponent
	Packet packet; 
	packet << currentRow << currentCol; 
	if (socket.send(packet) != Socket::Done) {
		cerr << "Error sending move\n";
		isBattleEnded = true;
		return;
	}


	if (isWinningMove(currentRow, currentCol, currentStone)) {
		isBattleEnded = true;
		display.drawBoard(board);
		display.updateScreen(board, prevRow, prevCol, currentRow, currentCol, isBlackTurn);
	}
	else {
		switchTurn();
		isMyTurn = false; // end of my turn
	}
}

void GameManager::switchTurn() {
	isBlackTurn = !isBlackTurn;
}

void GameManager::run() {

	while (1) {
		system("cls");
		display.drawBoard(board);

		while (!isBattleEnded) {
			if (isMyTurn) {
				// Only redraw the cursor if it has moved.
				if (currentRow != prevRow || currentCol != prevCol) {
					display.updateScreen(board, prevRow, prevCol, currentRow, currentCol, isBlackTurn);
					prevRow = currentRow;
					prevCol = currentCol;
				}
				handleInput();
			}
			else {
				waitForOpponent();
				display.updateScreen(board, prevRow, prevCol, currentRow, currentCol, isBlackTurn);
			}
			Sleep(100);
		}

		displayWinner();

		if (playAgain()) {
			resetGame();
		}
		else break;
	}
	
	//_getch();
}

void GameManager::displayWinner() {
	display.showWinner(isBlackTurn);
}

void GameManager::resetGame() {
	Board newBoard(BOARD_SIZE, BOARD_SIZE);
	board = newBoard;
	board.init();

	currentRow = board.getHeight() / 2;
	currentCol = board.getWidth() / 2;

	prevRow = currentRow;
	prevCol = currentCol;

	isBattleEnded = false;
	isBlackTurn = true;

	isMyTurn = (role == Role::SERVER);

	system("cls");
}

bool GameManager::playAgain() {
	Display::move(0, 24); 
	cout << "One More Game? Press any key to accept...\n";

	_getch();

	cout << "You agreed to a rematch. Waiting for Opponent...\n";

	// send my confirmation 
	Packet packet; 
	bool wantPlayAgain = true;
	packet << wantPlayAgain;

	if (socket.send(packet) != Socket::Done) {
		cerr << "Failed to send rematch confirmation.\n";
		return false;
	}

	// wait for opponent's confirmation 
	Packet receivePacket; 
	bool alsoWantPlayAgain = false;
	if (socket.receive(receivePacket) != Socket::Done) {
		cout << "Did not receive rematch confirmation.\n";
		return false;
	}

	receivePacket >> alsoWantPlayAgain;

	if (wantPlayAgain && alsoWantPlayAgain) {
		cout << "Opponent agreed! Starting a new game...\n";
		Sleep(1500);
		return true;
	}
	else {
		cout << "Opponent declined or disconnected. Thanks for playing!\n";
		return false;
	}
}

 