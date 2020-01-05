#include "3DT.h"

using namespace io;

int main() {

	const int HUMAN_PLAYER = 0;
	const int COMPUTER_PLAYER = 1;

	srand(time(NULL));

	int status, turn, current_player;
	int game_count = 0;
	bool done = false;
	string* input;
	Board* board = new Board();
	Player** player = new Player*[NUM_PLAYERS];
	player[HUMAN_PLAYER] = new Player(HUMAN);
	player[COMPUTER_PLAYER] = new Player(COMPUTER);

	//replay loop
	do {

		game_count++;
		cout << "Game #" << game_count << " - Human wins: " << player[0]->get_wins()
			 << " | Computer wins: " << player[1]->get_wins() << endl;

		turn = 0;
		current_player = 0;
		board->print();

		//game loop
		do {

			turn++;
			cout << "Turn #" << turn << " - " << (current_player ? "Computer's turn.\n" : "Human's turn. "); 

			board->make_move(*player[current_player], turn);

			board->print();
			status = board->game_status();
			current_player = current_player == HUMAN_PLAYER ? COMPUTER_PLAYER : HUMAN_PLAYER;

		} while (status == INCOMPLETE);
		//end game loop

		if (status == HUMAN) {
			cout << "Human wins!\n" << endl;
			player[HUMAN_PLAYER]->win();
		}
		else if (status == COMPUTER) {
			cout << "Computer wins!\n" << endl;
			player[COMPUTER_PLAYER]->win();
		}
		else
			cout << "Draw game!\n" << endl;

		board->highlight_win(status);
		board->print();
		board->print_moves();
		board->clear();

		cout << "play again? [y/n]: ";
		input = get_msg('\0');

		done = !(((*input)[0] | ' ') == 'y');
		delete input;

	} while (!done);
	//end replay loop
 
	delete board;
	delete player[HUMAN - 1];
	delete player[COMPUTER - 1];
	delete [] player;

	return 0;
}

string* get_msg(char delim) {

	char temp;
	string* out = new string;

	out->push_back(getc(stdin));
	temp = getc(stdin);

	while (temp != '\n' || (delim && out->at(out->size()) != delim)) {
		out->push_back(temp);
		temp = getc(stdin);
	}

	return out;
}