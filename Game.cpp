#include "Game.h"
#include <iostream>
#include <random>
#include <chrono>

Card::Card() {
	suit = "";
	rank = "";
	val = 0;
}

Game::Game(int num_players) {

	numPlayers = num_players;
	turn = 0;
	mother = 1;
	winner = -1;
	suit = "";
	reverse = false;
	seven = false;
	gameOver = false;

	//reference deck goes from lowest to highest value
	//assign rank and value to reference deck

	for (int i = 0; i < 4; i++) {
		stdDeck[i].rank = "A";
		stdDeck[i].val = 11;
	}
	for (int i = 2; i <= 10; i++) {
		int j;
        for (j = 4 * (i - 1); j < 4 * i; j++) {
			stdDeck[j].rank = std::to_string(i);
			stdDeck[j].val = i;
        }
	}
	for (int i = 40; i < 44; i++) {
		stdDeck[i].rank = "J";
		stdDeck[i].val = 20;
	}
	for (int i = 44; i < 48; i++) {
		stdDeck[i].rank = "Q";
		stdDeck[i].val = 30;
	}
	for (int i = 48; i < 52; i++) {
		stdDeck[i].rank = "K";
		stdDeck[i].val = 40;
	}

	//assign Suit to reference deck

	for (int i = 0; i < 52; i++) {
		switch (i % 4) {
		case 0:
			stdDeck[i].suit = "DIAMONDS";
			break;
		case 1:
			stdDeck[i].suit = "CLUBS";
			break;
		case 2:
			stdDeck[i].suit = "HEARTS";
			break;
		case 3:
			stdDeck[i].suit = "SPADES";
			break;
		}
	}

	//shuffle and deal cards

	for (int i = 0; i < 52; i++) {
		faceDown.push_back(i);
	}

    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    std::shuffle(faceDown.begin(), faceDown.end(), std::default_random_engine(seed));

	for (int i = 0; i < numPlayers; i++) {
		std::vector<int> newPlayer;

		for (int j = 0; j < 5; j++) {
			newPlayer.push_back(faceDown[faceDown.size() - 1]); 
			faceDown.pop_back();
		}

		playerDecks.push_back(newPlayer);
		scores.push_back(0);			//initializing score values to 0
		players.push_back(i + 1);		//initializing player list
	}

	faceUp.push_back(faceDown[faceDown.size() - 1]);
	faceDown.pop_back();
}

void Game::reset() {

	//clear decks

	faceDown.clear();
	faceUp.clear();
	for (int i = 0; i < numPlayers; i++) {
		playerDecks[i].clear();
	}

	//shuffle and deal cards

	for (int i = 0; i < 52; i++) {
		faceDown.push_back(i);
	}

    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    std::shuffle(faceDown.begin(), faceDown.end(), std::default_random_engine(seed));

	for (int i = 0; i < numPlayers; i++) {
		for (int j = 0; j < 5; j++) {
			playerDecks[i].push_back(faceDown[faceDown.size() - 1]);
			faceDown.pop_back();
		}
	}

	faceUp.push_back(faceDown[faceDown.size() - 1]);
	faceDown.pop_back();
}

void Game::tallyScores(int skip, int multiply) {
	std::vector<int> roundScores(numPlayers, 0);
	for (int i = 0; i < numPlayers; i++) {
		if (i == skip)
			continue;
		for (unsigned int j = 0; j < playerDecks[i].size(); j++) {
			scores[i] += stdDeck[playerDecks[i][j]].val;
		}
		if (scores[i] == 500)
			scores[i] = 200;
		else if (scores[i] == 250)
			scores[i] = 50;
		else if (scores[i] > 500) {
			playerDecks.erase(playerDecks.begin() + i);		//delete player's deck
			scores.erase(scores.begin() + i);				//delete player's score
			roundScores.erase(roundScores.begin() + i);		//delete player's round score
			players.erase(players.begin() + i);				//delete player
			numPlayers -= 1;								//update number of players
		}
	}
	if (numPlayers == 1) {									//check for game over conditions
		winner = players[1];
		gameOver = true;

		faceDown.clear();									//clear cards
		faceUp.clear();
		for (int i = 0; i < numPlayers; i++) {
			playerDecks[i].clear();
		}

		return;
	}
	int minP = 0;											//the player next to the lowest scoring player goes next
	for (int i = 1; i < numPlayers; i++) {
		if (roundScores[i] < roundScores[minP])
			minP = i;
	}
	turn = minP + 1;
	if (turn >= numPlayers)
		turn = turn % numPlayers;

	reset();
	return;
}

bool Game::playCard(int c, std::string choice) {

	bool play = false;		
	int multiply = 1;		//in case player goes out with a jack
	unsigned int i;

	for (i = 0; i < playerDecks[turn].size(); i++) {
		
		if (seven && stdDeck[c].rank != "7")
			return false;

		if (playerDecks[turn][i] == c) {
			if (stdDeck[c].rank == "J") {		//check if card is a jack (wild)
				if (playerDecks[turn][i] / 4 == faceUp[faceUp.size() - 1] / 4)
					mother++;
				multiply = mother;
				suit = choice;
				play = true;
			}
			else if (c / 4 == faceUp[faceUp.size() - 1] / 4) {
				mother++;
				play = true;
			}
			else if (suit == "" && c % 4 == faceUp[faceUp.size() - 1] % 4) {
				mother = 1;
				play = true;
			}
			else if (stdDeck[c].suit == suit) {
				mother = 1;
				play = true;
			}
			break;
		}
	}
	if (play) {
		faceUp.push_back(playerDecks[turn][i]);						//copy player card into the face up deck
		playerDecks[turn].erase(playerDecks[turn].begin() + i);		//delete player card

		//handling of special cards

		std::string r = stdDeck[c].rank;

		if (r == "7")												//7s are the draw card
			playSeven();											//their behavior is more complicated

		if (mother == 4 || playerDecks[turn].size() == 0) {
			tallyScores(turn, multiply);
			return true;
		}


		//these cards only matter if the player didn't go out

		if (r == "K" || r == "8") {									//kings and 8s are skips (add an extra turn)
			if (reverse)
				turn -= 1;
			else
				turn += 1;
		}
		else if (r == "Q")											//queens are reverse
			reverse = true;

		//manage next turn

		if (reverse)
			turn -= 1;
		else
			turn += 1;

		if (turn >= numPlayers)
			turn = turn % numPlayers;
		else if (turn < 0)
			turn += 4;

		return true;
	}
	else 
		return false;
}

bool Game::drawCard() {

    Card c = stdDeck[faceUp[faceUp.size() - 1]];

	for (unsigned int i = 0; i < playerDecks[turn].size(); i++) {					//players are not allowed to draw if a non jack card can be played
		if (seven && stdDeck[playerDecks[turn][i]].rank == "7") {					//if the draw was called by a seven only a seven can be played
			return false;
		}
		if (!seven && stdDeck[playerDecks[turn][i]].rank == "J")
			continue;
		if (!seven && (stdDeck[playerDecks[turn][i]].rank == c.rank || stdDeck[playerDecks[turn][i]].suit == c.suit)) {
			return false;
		}
	}
	playerDecks[turn].push_back(faceDown[faceDown.size() - 1]);						//draw from deck
	faceDown.pop_back();

	//if deck is empty, reshuffle

	if (faceDown.size() == 0) {
		faceDown.insert(faceDown.end(), faceUp.begin(), faceUp.end() - mother);		//leave top card and previous cards if they have the same rank
		faceUp.erase(faceUp.begin(), faceUp.end() - mother);
        unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
        std::shuffle(faceDown.begin(), faceDown.end(), std::default_random_engine(seed));
	}

	int idx = playerDecks[turn][playerDecks[turn].size() - 1];						//manage turns
	if (stdDeck[idx].rank != c.rank && stdDeck[idx].suit != c.suit) {
		if (reverse)
			turn -= 1;
		else
			turn += 1;

		if (turn >= numPlayers)
			turn = turn % numPlayers;
		else if (turn < 0)
			turn += 4;
	}
	return true;
}

void Game::playSeven() {

	seven = true;
	if (reverse)
		turn -= 1;
	else
		turn += 1;

    if (turn >= numPlayers)
        turn = turn % numPlayers;
    else if (turn < 0)
        turn += 4;

	for (int i = 0; i < mother; i++) {
		if (!drawCard()) {
			if (reverse)
				turn += 1;
			else
				turn -= 1;
			seven = false;
			break;
		}
	}

	if (turn >= numPlayers)
		turn = turn % numPlayers;
	else if (turn < 0)
		turn += 4;

	seven = !seven;
}
