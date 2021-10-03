#ifndef GAME_H
#define GAME_H

#include <string>
#include <vector>
#include <algorithm>
#include <stdlib.h>

struct Card {
	std::string suit;
	std::string rank;
	int val;
	Card();
};

class Game {
private:
	Card stdDeck[52];
	std::vector<int> faceDown;						//deck to be drawn from
	int numPlayers;									//number of players
	int turn;										//keeps track of who's turn it is
	int mother;										//if mother = 4 the turn ends
	int winner;										//corresponds to winning player
	bool reverse;									//reverses the direction the game is played
	bool seven;										//restricts drawing to sevens
	bool gameOver;									//true if game ends
	std::string suit;								//choice of wild card

	void tallyScores(int skip, int multiply = 1);	//tallies scores at the end of a round
	void reset();									//resets for next round
	void playSeven();								//handles sevens
public:
	std::vector<std::vector<int>> playerDecks;		//player decks represented as vectors of indices to stdDeck
	std::vector<int> scores;						//keeps track of scores
	std::vector<int> players;						//keeps track of players still in the game;
	std::vector<int> faceUp;						//deck to be played on

    Game(int num_players = 3);
	Card getCard(int index) { return stdDeck[index]; }		//returns card for a given turn and index
	int getTurn() { return turn; }
	int getWinner() { return winner; }
	bool gameIsOver() { return gameOver; }
	bool playCard(int c, std::string choice = "");
	bool drawCard();						//TEMPORARY COMMENT: I might need to change parameter
	
};

#endif
