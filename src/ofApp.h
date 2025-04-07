#pragma once

#include "ofMain.h"
#include "ofxJSON.h"
#include <string>
#include <vector>

using namespace std;

class ofApp : public ofBaseApp
{

public:
	void setup();
	void draw();

	ofTrueTypeFont font;
	ofRectangle btn1, btn2, btn3, btn4, end; // 5 rectangles for buttons
	int correctAnswerBtnIndex;
	vector<ofRectangle> buttons;

	ofxJSONElement json; // object to read json data

	int numPlayers, currentPlayer; // variables to store number of players and page number
	// vector to contian each players score
	int count;
	int appState;
	int questionIndex;
	string temp;
	int totalQuestions;

	struct playerScore
	{
		int score;
		int player;
	};
	vector<playerScore> scores;

	void cyclePlayers();
	void updateScore();
	void startGame();

	static bool compareScores(const playerScore &, const playerScore &);

	void resetScores();
	void getPlayerCount();
	void shuffleArray(vector<string> &);
	void drawQuestion(vector<string> &);
	void drawAnswers();
	void drawScores();
	void drawLeaderboard();
	vector<string> prepAnswer();
	vector<string> answers;

	void getData(); // function to obtain API data
	void mousePressed(int x, int y, int button);
};
