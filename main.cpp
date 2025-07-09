#include "GameManager.h"
#include <iostream>
#include <string>

int main() {
	char role;
	cout << "Welcome to Omok!\n";
	cout << "Choose your role: [S]erver or [C]lient? \n";
	cin >> role;

	if (toupper(role) == 'S') {
		GameManager game(GameManager::Role::SERVER);
		game.run();
	}
	else if (toupper(role) == 'C') {
		GameManager game(GameManager::Role::CLIENT);
		game.run();
	}
	else {
		cout << "Invalid Choice.\n";
	}

	return 0;
}