#include "3DT.h"

Player::Player(int id) {
	this->id = id;
	wins = 0;
}

int Player::get_id() const {
	return id;
}

int Player::get_wins() const {
	return wins;
}

void Player::win() {
	wins++;
}