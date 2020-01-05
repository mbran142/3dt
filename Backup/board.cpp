#include "3DT.h"

using namespace io;

Board::Board() {

	board = new int**[SIZE];
	newp = new bool**[SIZE];
	last_move = moves = NULL;

	for (int i = 0; i < SIZE; i++) {

		board[i] = new int*[SIZE];
		newp[i] = new bool*[SIZE];

		for (int j = 0; j < SIZE; j++) {
			board[i][j] = new int[SIZE];
			newp[i][j] = new bool[SIZE];
		}
	}

	this->clear();
}

Board::~Board() {

	for (int i = 0; i < SIZE; i++) {

		for (int j = 0; j < SIZE; j++) {
			delete [] board[i][j];
			delete [] newp[i][j];
		}

		delete [] board[i];
		delete [] newp[i];
	}

	delete [] board;
	delete [] newp;
}

void Board::print() {
	
	for (int i = 0; i < SIZE; i++) {

		for (int j = 0; j < SIZE; j++) {

			cout << ' ';

			for (int k = 0; k < SIZE; k++) {

				cout << (board[i][j][k] == 0 ? ' ' : (board[i][j][k] == HUMAN ? (newp[i][j][k] ? 'O' : 'o') : (newp[i][j][k] ? 'X' : 'x')));

				if (k != SIZE - 1)
					cout << " | ";
			}

			if (j != SIZE - 1)
				cout << "\n- - - - - - - -\n";
		}

		cout << "\n\n\n";
	}

	for (int i = 0; i < SIZE; i++)
		for (int j = 0; j < SIZE; j++)
			for (int k = 0; k < SIZE; k++)
				newp[i][j][k] = false;
}

void Board::clear() {
	
	for (int i = 0; i < SIZE; i++)
		for (int j = 0; j < SIZE; j++)
			for (int k = 0; k < SIZE; k++) {
				board[i][j][k] = 0;
				newp[i][j][k] = false;
			}

	node* p = moves;

	while (p != NULL) {
		moves = moves->next;
		delete p;
		p = moves;
	}

	last_move = NULL;
}

int Board::game_status() {

	int prod[6];
	int psize;
	register int i, j, k;

	//general form is a => all values {0...3}, n => circle though {0...3}
	
	//all 2a 1n
	psize = 3;
	for (i = 0; i < SIZE; i++) {
		for (j = 0; j < SIZE; j++) {

			for (int p = 0; p < psize; p++)
				prod[p] = 1;

			for (k = 0; k < SIZE; k++) {
				prod[0] *= board[i][j][k];
				prod[1] *= board[i][k][j];
				prod[2] *= board[k][i][j];
			}

			for (int p = 0; p < psize; p++) {
				if (prod[p] == 1) return HUMAN;
				if (prod[p] == 16) return COMPUTER; 
			}
		}
	}

	//all 1a 2n
	psize = 6;
	for (i = 0; i < SIZE; i++) {

		for (int p = 0; p < psize; p++)
			prod[p] = 1;

		for (j = 0; j < SIZE; j++) {
			prod[0] *= board[i][j][j];
			prod[1] *= board[j][i][j];
			prod[2] *= board[j][j][i];
			prod[3] *= board[i][j][CSIZE - j];
			prod[4] *= board[j][i][CSIZE - j];
			prod[5] *= board[j][CSIZE - j][i];
		}

		for (int p = 0; p < psize; p++) {
			if (prod[p] == 1) return HUMAN;
			if (prod[p] == 16) return COMPUTER; 
		}	
	}

	//all 3n
	psize = 4;
	{
		for (int p = 0; p < psize; p++)
			prod[p] = 1;

		for (i = 0; i < SIZE; i++) {
			prod[0] *= board[i][i][i];
			prod[1] *= board[i][i][CSIZE - i];
			prod[2] *= board[i][CSIZE - i][i];
			prod[3] *= board[CSIZE - i][i][i];
		}

		for (int p = 0; p < psize; p++) {
			if (prod[p] == 1) return HUMAN;
			if (prod[p] == 16) return COMPUTER; 
		}
	}

	//check full board
	for (i = 0; i < SIZE; i++)
		for (j = 0; j < SIZE; j++)
			for (k = 0; k < SIZE; k++)
				if (board[i][j][k] == 0)
					return INCOMPLETE;

	return DRAW;
}

void Board::make_move(const Player& player, int turn) {

	if (player.get_id() == COMPUTER)
		this->ai_move(turn);

	else {

		int i, j, k;
		string* input;

		do {

			cout << "Enter move: ";
			input = get_msg('\0');

			if (input->size() < 3)
				i = j = k = -1;

			else {
				i = (int)(input->at(0) & 0xf);
				j = (int)(input->at(1) & 0xf);
				k = (int)(input->at(2) & 0xf);
			}

			if (i > 4 || j > 4 || k > 4)
				i = -1;

			delete input;

		} while (i < 0 || board[i - 1][j - 1][k - 1] != 0);

		board[i - 1][j - 1][k - 1] = HUMAN;
		newp[i - 1][j - 1][k - 1] = true;
			
		node* p = new node;
		p->info = (i * 100) + (j * 10) + k;
		p->next = NULL;

		if (moves == NULL)
			last_move = moves = p;
		else {
			last_move->next = p;
			last_move = p;
		}
	}
}

void Board::ai_move(int turn) {

	int max_depth = 5;
	int depth = 0;
	int status_prediction, temp;
	int moves[max_depth + 1][8];
	bool down = true;

	for (int i = 0; i <= max_depth; i++)
		moves[i][4] = (i & 0x1) ? 0x80000000 : 0x7FFFFFFF;

	//moves[i][0] is used for the current iteration in the 'node'
	//moves[i][1-3] are the i j k board placements
	//moves[i][4] is the minimax score
	//moves[i][5] is the accociated move with the minimax score
	//moves[i][0] always starts at 0
	//moves[i][7] is the current amount of moves made

	while (true) {

		if (down) {
			depth++;
			moves[depth][0] = (rand() % 64) - 1;
			//moves[depth][0] = -1;
			moves[depth][7] = -1;
		}

		down = true;

		do {

			moves[depth][0] = moves[depth][0] == ((SIZE * SIZE * SIZE) - 1) ? 0 : moves[depth][0] + 1;
			moves[depth][7]++;
			down = !(moves[depth][7] == SIZE * SIZE * SIZE);

			if (down) {
				moves[depth][1] = moves[depth][0] / (SIZE * SIZE);
				moves[depth][2] = (moves[depth][0] - (moves[depth][1] * SIZE * SIZE)) / SIZE;
				moves[depth][3] = moves[depth][0] - (moves[depth][1] * SIZE * SIZE) - (moves[depth][2] * SIZE);
			}

		} while (down && board[moves[depth][1]][moves[depth][2]][moves[depth][3]]);

		if (!down) {

			depth--;

			if ((!(depth & 0x1) && moves[depth][4] > moves[depth + 1][4]) ||
				(depth & 0x1 && moves[depth][4] < moves[depth + 1][4])) {
				moves[depth][4] = moves[depth + 1][4];
				moves[depth][5] = moves[depth + 1][5];
			}

			moves[depth + 1][4] = ((depth + 1) & 0x1) ? 0x80000000 : 0x7FFFFFFF;

			if (!depth) break;
			board[moves[depth][1]][moves[depth][2]][moves[depth][3]] = 0;
		}
	
		else {

			board[moves[depth][1]][moves[depth][2]][moves[depth][3]] = depth & 0x1 ? COMPUTER : HUMAN;

			status_prediction = this->game_status();

			//win / loss
			if (status_prediction != INCOMPLETE) {

				board[moves[depth][1]][moves[depth][2]][moves[depth][3]] = 0;
				depth--;

				if (!depth) {
					moves[depth][5] = moves[depth + 1][0];
					break;
				}

				board[moves[depth][1]][moves[depth][2]][moves[depth][3]] = 0;
				depth--;

				if (!depth) {
					moves[depth][5] = moves[depth + 2][0];
					break;
				}

				board[moves[depth][1]][moves[depth][2]][moves[depth][3]] = 0;
				moves[depth][4] = depth & 0x1 ? 1000000 : -1000000;
				moves[depth][5] = moves[depth][0];
				down = false;
			}

			else {

				temp = this->get_minimax();

				if (depth == max_depth) {

					if ((depth & 0x1 && temp > moves[depth][4]) ||
						(!(depth & 0x1) && temp < moves[depth][4])) {
						moves[depth][4] = temp;
						moves[depth][5] = moves[depth][0];
					}

					board[moves[depth][1]][moves[depth][2]][moves[depth][3]] = 0;
					down = false;
				}

				else {
					down = ((depth & 0x1 && temp > moves[depth][4]) || (!(depth & 0x1) && temp < moves[depth][4]));
					board[moves[depth][1]][moves[depth][2]][moves[depth][3]] = 0;
				}
			}
		}
	}
	//end while

	moves[depth][1] = moves[depth][5] / (SIZE * SIZE);
	moves[depth][2] = (moves[depth][5] - (moves[depth][1] * SIZE * SIZE)) / SIZE;
	moves[depth][3] = moves[depth][5] - (moves[depth][1] * SIZE * SIZE) - (moves[depth][2] * SIZE);

	board[moves[0][1]][moves[0][2]][moves[0][3]] = COMPUTER;
	newp[moves[0][1]][moves[0][2]][moves[0][3]] = true;
}

int Board::get_minimax() {

	const int MULTIPLIER = 4;
	const int P_CONST = -1;
	const int C_CONST = 1;
	int psize, temp, line, minimax, c3, p3;
	c3 = p3 = minimax = 0;
	register int i, j, k;
	
	//all 2a 1n
	psize = 3;
	for (i = 0; i < SIZE; i++) {
		for (j = 0; j < SIZE; j++) {

			for (int p = 0; p < psize; p++) {

				line = 0;

				for (k = 0; k < SIZE; k++) {

					switch(p) {
						case 0 : temp = board[i][j][k]; break;
						case 1 : temp = board[i][k][j]; break;
						default: temp = board[k][i][j];
					}

					if (line) {
						if ((line > 0 && temp == COMPUTER) || (line < 0 && temp == HUMAN))
							line *= MULTIPLIER;

						else if (temp) {
							line = 0;
							break;
						} 
					}

					else if (temp)
						line = temp == COMPUTER ? C_CONST : P_CONST;
					}

				minimax += line;

				if (line == MULTIPLIER * MULTIPLIER * C_CONST) c3++;
				else if (line == MULTIPLIER * MULTIPLIER * P_CONST) p3++;
			}
		}
	}

	//all 1a 2n
	psize = 6;
	for (i = 0; i < SIZE; i++) {

		for (int p = 0; p < psize; p++) {

			line = 0;

			for (j = 0; j < SIZE; j++) {

				switch (p) {
					case 0 : temp = board[i][j][j]; break;
					case 1 : temp = board[j][i][j]; break;
					case 2 : temp = board[j][j][i]; break;
					case 3 : temp = board[i][j][CSIZE - j]; break;
					case 4 : temp = board[j][i][CSIZE - j]; break;
					default: temp = board[j][CSIZE - j][i];
				}

				if (line) {
					if ((line > 0 && temp == COMPUTER) || (line < 0 && temp == HUMAN))
						line *= MULTIPLIER;

					else if (temp) {
						line = 0;
						break;
					} 
				}

				else if (temp)
					line = temp == COMPUTER ? C_CONST : P_CONST;
			}

			minimax += line;

			if (line == MULTIPLIER * MULTIPLIER * C_CONST) c3++;
			else if (line == MULTIPLIER * MULTIPLIER * P_CONST) p3++;			
		}
	}

	//all 3n
	psize = 4;
	for (int p = 0; p < psize; p++) {

		line = 0;

		for (i = 0; i < SIZE; i++) {

			switch (p) {
				case 0 : temp = board[i][i][i]; break;
				case 1 : temp = board[i][i][CSIZE - i]; break;
				case 2 : temp = board[i][CSIZE - i][i]; break;
				default: temp = board[CSIZE - i][i][i];
			}

			if (line) {
				if ((line > 0 && temp == COMPUTER) || (line < 0 && temp == HUMAN))
					line *= MULTIPLIER;

				else if (temp) {
					line = 0;
					break;
				} 
			}

			else if (temp)
				line = temp == COMPUTER ? C_CONST : P_CONST;
		}

		minimax += line;

		if (line == MULTIPLIER * MULTIPLIER * C_CONST) c3++;
		else if (line == MULTIPLIER * MULTIPLIER * P_CONST) p3++;
	}

	if (!p3 && c3 == 2) return 5000;
	else if (!c3 && p3 == 2) return -5000;

	return minimax;
}

void Board::highlight_win(int winner) {

	int psize, temp;
	register int i, j, k;
	
	//all 2a 1n
	psize = 3;
	for (i = 0; i < SIZE; i++) {
		for (j = 0; j < SIZE; j++) {
			for (int p = 0; p < psize; p++) {
				for (k = 0; k < SIZE; k++) {

					switch(p) {
						case 0 : temp = board[i][j][k]; break;
						case 1 : temp = board[i][k][j]; break;
						default: temp = board[k][i][j];
					}

					if (temp != winner) break;
					if (k == SIZE - 1) {
						for (k = 0; k < SIZE; k++) {

							switch(p) {
								case 0 : newp[i][j][k] = true; break;
								case 1 : newp[i][k][j] = true; break;
								default: newp[k][i][j] = true;
							}
						}

						return;
					}
				}
			}
		}
	}

	//all 1a 2n
	psize = 6;
	for (i = 0; i < SIZE; i++) {
		for (int p = 0; p < psize; p++) {
			for (j = 0; j < SIZE; j++) {

				switch (p) {
					case 0 : temp = board[i][j][j]; break;
					case 1 : temp = board[j][i][j]; break;
					case 2 : temp = board[j][j][i]; break;
					case 3 : temp = board[i][j][CSIZE - j]; break;
					case 4 : temp = board[j][i][CSIZE - j]; break;
					default: temp = board[j][CSIZE - j][i];
				}


				if (temp != winner) break;
				if (j == SIZE - 1) {
					for (j = 0; j < SIZE; j++) {

						switch(p) {
							case 0 : newp[i][j][j] = true; break;
							case 1 : newp[j][i][j] = true; break;
							case 2 : newp[j][j][i] = true; break;
							case 3 : newp[i][j][CSIZE - j] = true; break;
							case 4 : newp[j][i][CSIZE - j] = true; break;
							default: newp[j][CSIZE - j][i] = true;
						}
					}
					
					return;
				}
			}
		}
	}

	//all 3n
	psize = 4;
	for (int p = 0; p < psize; p++) {
		for (i = 0; i < SIZE; i++) {

			switch (p) {
				case 0 : temp = board[i][i][i]; break;
				case 1 : temp = board[i][i][CSIZE - i]; break;
				case 2 : temp = board[i][CSIZE - i][i]; break;
				default: temp = board[CSIZE - i][i][i];
			}

			if (temp != winner) break;
			if (i == SIZE - 1) {
				for (i = 0; i < SIZE; i++) {

					switch(p) {
						case 0 : newp[i][i][i] = true; break;
						case 1 : newp[i][i][CSIZE - i] = true; break;
						case 2 : newp[i][CSIZE - i][i] = true; break;
						default: newp[CSIZE - i][i][i] = true;
					}
				}
				
				return;
			}
		}
	}
}

void Board::print_moves() {

	cout << "Move sequence: ";
	node* p = moves;

	while (p != NULL) {
		cout << p->info << ", ";
		p = p->next;
	}
}