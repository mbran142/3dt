#ifndef _3DT_H
#define _3DT_H

#include <iostream>
#include <string>
#include <stdlib.h>
#include <time.h>

namespace io
{
  using std::cout;
  using std::cin;
  using std::endl;
  using std::string;
};

const int SIZE =		4;
const int CSIZE =  		SIZE - 1;
const int HUMAN = 		1;
const int COMPUTER =  	2;
const int DRAW = 		2;
const int NUM_PLAYERS = 2;
const int INCOMPLETE = -1;

struct node {
	int info;
	node* next;
};

class Player {
	private:
		int id;
		int wins;
	public:
		Player(int);
		int get_id() const;
		int get_wins() const;
		void win();
};

class Board {
	private:
		int*** board;
		bool*** newp;
		node* moves;
		node* last_move;
	public:
		Board();
		~Board();
		void print();
		void clear();
		void make_move(const Player&, int);
		int game_status();
		void ai_move(int);
		int get_minimax();
		void highlight_win(int);
		void print_moves();
};

std::string* get_msg(char);

#endif