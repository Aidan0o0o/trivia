#include "ofApp.h"
#include <iostream>
#include <string>
#include <vector>
#include <algorithm> //algorithm for sorting vector
#include <cstdlib>   //pseudo random number generation
#include <windows.h> //for time delay
#include <regex>     //text replacement in string
using namespace std;

//--------------------------------------------------------------

// Sets up openFrameworks application
void ofApp::setup()
{
    numPlayers = 4;            // default number of players
    appState = START;          // inital app state
    totalQuestions = 12;       // default number of questions
    numOfQuestionsIndex = 0;   // default number of question selection button index
    correctAnswerBtnIndex = 3; // 3 because correct answers original index always 3 before shufflings

    font.load("BebasNeue-Regular.ttf", 20); // loads font in 2 sizes
    headerFont.load("BebasNeue-Regular.ttf", 40);
    startGame(); // starts game routine
}

// Starts game routine
void ofApp::startGame()
{
    // reseting application state and components
    resetAnswerButtons();
    resetPlayerCountButtons();
    resetNumOfQuestionsButtons();
    resetScores();
    setNumOfQuestions();
    end.set(0, 0, 0, 0); // ensures end button is out the way
    appState = START;
    currentPlayer = 0; // index of player 1
    questionIndex = 0;
}

void ofApp::resetAnswerButtons()
{
    answerButtons.clear();                    // clears vector containing buttons
    answerButtons = {btn1, btn2, btn3, btn4}; // recreates vector of buttons
}

void ofApp::resetPlayerCountButtons()
{
    playerCountBtns.clear(); // clears vector containg buttons
    for (int i = 0; i < 4; i++)
    {
        playerCountBtns.push_back(ofRectangle(0, 0, 0, 0)); // generating buttons
    }
}

void ofApp::resetNumOfQuestionsButtons()
{
    numOfQuestionsBtns.clear();
    for (int i = 0; i < 4; i++)
    {
        numOfQuestionsBtns.push_back(ofRectangle(0, 0, 0, 0));
    }
}

void ofApp::resetScores()
{
    scores.clear(); // clears vector containing player scores
    for (int i = 0; i < numPlayers; i++)
    {
        scores.push_back({0, i}); // resets all player scores to 0
    }
}

// calculates total number of questions needed from the API based on number of questions selected for each player as well as how many players
void ofApp::setNumOfQuestions()
{
    totalQuestions = ((numOfQuestionsIndex + 1) * 3) * numPlayers; // uses the button index and number of players to generate total number of questions
}

// gets JSON data from opentdb API
void ofApp::getData()
{
    json.open("https://opentdb.com/api.php?amount=" + to_string(totalQuestions));
    // API is rate limited to a request every 5 seconds
}

// gets answers from JSON and puts both correct and incorrect answers into a single vector
vector<string> ofApp::prepAnswer()
{
    answers.clear();                                                               // clear the answers vector
    int numOfAnswers = json["results"][questionIndex]["incorrect_answers"].size(); // the number of incorrect answers (in this case 3 or 1)

    for (int i = 0; i < numOfAnswers; i++)
    {
        answers.push_back(json["results"][questionIndex]["incorrect_answers"][i].asString()); // puts incorrect answers into vector
    }

    answers.push_back(json["results"][questionIndex]["correct_answer"].asString()); // adds correct answer at the of the vector as API stores it seperatly to incorrect answers
    shuffleVector(answers);                                                         // shuffles the answer vector to change up the order each time

    // Locates the index of the correct answer after the answer vector is shuffled
    for (int i = 0; i < numOfAnswers; i++)
    {
        if (answers.at(i) == json["results"][questionIndex]["correct_answer"].asString())
        {
            correctAnswerBtnIndex = i;
            break;
        }
    }

    return answers;
}

// shuffles vector of answers so order is different everytime
void ofApp::shuffleVector(vector<string> &answers)
{
    srand(time(NULL)); // seeding the random number so its different everytime the code is run
    for (int i = answers.size() - 1; i > 0; i--)
    { // Loop swaps values in the vector into random positions using random numbers
        int randomNum1 = rand() % i;
        string temp = answers[i];
        answers.at(i) = answers[randomNum1];
        answers.at(randomNum1) = temp;
    }
}

// displays the set up menu to the user
void ofApp::drawMenu()
{
    ofSetColor(255);
    headerFont.drawString("Welcome to the trivia game!", 325, 150);
    start.set(450, 650, 200, 80);
    ofDrawRectangle(start); // start button created
    ofSetColor(0);
    font.drawString("START", 520, 695);
    getPlayerCount();    // draws the buttons for the number of players selection
    getNumOfQuestions(); // draws the buttons for the number of questions selection
}

// draws the buttons for the number of players selection
void ofApp::getPlayerCount()
{
    int x = 100;
    int i = 0;
    ofSetColor(255);
    font.drawString("Select Number of PLayers", 100, 320);
    for (ofRectangle btn : playerCountBtns)
    { // shorthand
        if (i + 1 == numPlayers)
        {
            ofSetColor(255, 215, 0); // sets colour of button to gold if the user selects it
        }
        else
        {
            ofSetColor(255); // sets other button colours to white
        }
        playerCountBtns[i].set(x, 350, 150, 80); // sets each buttons position
        ofDrawRectangle(playerCountBtns[i]);     // draws each button
        ofSetColor(0);
        font.drawString(to_string(i + 1), x + 70, 400); // draws text onto button
        x += 250;
        i++;
    }
}

// draws the buttons for the number of questions selection
void ofApp::getNumOfQuestions()
{
    int x = 100;
    int i = 0;
    ofSetColor(255);
    font.drawString("Select Number of Questions", 100, 480);
    for (ofRectangle btn : numOfQuestionsBtns)
    { // shorthand
        if (i == numOfQuestionsIndex)
        { // sets colour of button to gold if user selects it
            ofSetColor(255, 215, 0);
        }
        else
        {
            ofSetColor(255);
        }
        numOfQuestionsBtns[i].set(x, 500, 150, 80);
        ofDrawRectangle(numOfQuestionsBtns[i]);
        ofSetColor(0);
        font.drawString(to_string((i + 1) * 3), x + 70, 550);
        x += 250;
        i++;
    }
}

// Renders questions and answers buttons to user
void ofApp::drawQuestion(vector<string> &answers)
{ // passes the vector of answers by reference to stop them mutating
    ofSetColor(255);
    string question = json["results"][questionIndex]["question"].asString(); // takes single question from JSON
    font.drawString(regex_replace(question, regex("&quot;"), "'"), 10, 110); // displays user with question
    drawQuestionCounter();                                                   // draws counter for current question in corner of screen
    int y = 130;
    int i = 0; // counts current button

    // sets and draws answerButtons and answers
    for (string answer : answers)
    { // shorthand
        ofSetColor(255);
        answerButtons[i].set(10, y, 500, 80);
        ofDrawRectangle(answerButtons[i]);
        ofSetColor(0);
        font.drawString(answer, 15, y + 50);
        y += 100;
        i++;
    }
    appState = IN_GAME; // change app state to in game
}

// draws a counter to keep track of the current question the user is on
void ofApp::drawQuestionCounter()
{
    headerFont.drawString(to_string(questionIndex + 1) + " / " + to_string(totalQuestions), 960, 50);
}

// page to display that the user got the answer correct
void ofApp::drawCorrectResult()
{
    resetAnswerButtons();
    setGradientBackground(0, 255, 0); // sets background to green gradient
    headerFont.drawString("CORRECT!", 460, 360);
    if ((ofGetElapsedTimef() - timer) >= 1.0)
    {                   // checks to see if a second has passed since the page has been displayed
        cyclePlayers(); // moves on to the next question and player if a second has passed
    }
}
// page to display that the user got the answer incorrect
void ofApp::drawIncorrectResult()
{
    resetAnswerButtons();
    setGradientBackground(255, 0, 0); // sets background to red gradient
    headerFont.drawString("INCORRECT!", 460, 360);
    if ((ofGetElapsedTimef() - timer) >= 1.0)
    {
        cyclePlayers();
    }
}

// cycles the question and the players turn
void ofApp::cyclePlayers()
{
    currentPlayer++; // adds to the indexes of the current player and question index to iterate the vectors
    questionIndex++;
    if (currentPlayer > (numPlayers - 1))
    { // minus one as currentPlayer is index
        currentPlayer = 0;
    }
    if (questionIndex >= totalQuestions)
    { // checks if the current quesiton is the last or not and if it is, will end the game after
        appState = END_GAME;
    }
    else
    {
        appState = PREP_QUESTIONS; // preps another question for users
    }
}

// render the current scores to the users in the bottom right of the page
void ofApp::drawScores()
{
    ofSetColor(255); // white
    int x = 660;
    headerFont.drawString("Scores: ", 480, 750);
    for (int i = 0; i < numPlayers; i++)
    {
        headerFont.drawString("P" + to_string(i + 1) + "=" + to_string(scores[i].score), x, 750);
        x += 100;
    }
}
// function to compare 2 scores - used in the sort function to put scores in descending order
bool ofApp::compareScores(const playerScore &a, const playerScore &b)
{                             // static function so the sort function can use it
    return a.score > b.score; //> for descending order as returns true if A is bigger than B
}

// displays the final leaderboard
void ofApp::drawLeaderboard()
{
    sort(scores.begin(), scores.end(), compareScores); // sorts the score vector in descending order using the compareScores function made previously
    int y = 340;
    for (int i = 0; i < numPlayers; i++)
    {
        headerFont.drawString("Player " + to_string(scores[i].player + 1) + " :  " + to_string(scores[i].score) + " / " + to_string((numOfQuestionsIndex + 1) * 3), 430, y);
        y += 50;
    }

    end.set(450, 650, 200, 80); // end button set
    ofDrawRectangle(end);       // end button drawn
    ofSetColor(0);
    font.drawString("RESTART", 510, 695); // text added ontop of button
}

// handy function to set the gradient background
void ofApp::setGradientBackground(int r, int g, int b)
{ // r = red, g = green, b = blue
    ofColor colorOne(r, g, b);
    ofColor colorTwo(0, 0, 0); // gradient always fades to black

    ofBackgroundGradient(colorOne, colorTwo, OF_GRADIENT_CIRCULAR); // of function to create gradient with circular type
}

// main draw function - handles gamestates - called by openframeworks in a loop
void ofApp::draw()
{
    ofSetColor(255); // set colour to white (removes tint from any images)
    setGradientBackground(0, 0, 255);

    if (appState == START)
    { // if start of game, draw menu
        drawMenu();
    }
    else if (appState == PREP_QUESTIONS)
    { // if in prep state, prep questions then change to ingame state
        prepAnswer();
        appState = IN_GAME;
    }
    else if (appState == IN_GAME)
    {                                                                                    // display user with main game - questions, answers, scores, player turn
        headerFont.drawString("Player's Turn: " + to_string(currentPlayer + 1), 10, 50); // plus 1 as currentPLayer is index
        drawQuestion(answers);
        drawScores();
    }
    else if (appState == END_GAME)
    { // if in end state, draws leaderboard for user
        drawLeaderboard();
    }
    else if (appState == CORRECT_ANSWER)
    {
        drawCorrectResult(); // if in correct state draw correct page
    }
    else if (appState == INCORRECT_ANSWER)
    {
        drawIncorrectResult(); // if in incorrect state draw incorrect page
    }
}

// adds 1 to the user score each time function called
void ofApp::updateScore()
{
    scores[currentPlayer].score++;
}

// function to handle all mouse presses
void ofApp::mousePressed(int x, int y, int button)
{ // params are x and y coordinates of mouse click as well which mouse button (left,right ect)
    // variables to act as counters
    int c = 0; // answers buttonindex
    int i = 0; // player count button index
    int z = 0; // num of questions index

    if (appState == CORRECT_ANSWER || appState == INCORRECT_ANSWER)
    {
        return; // wont check clicks when in either app state to avoid ghost clicks
    }

    if (end.inside(x, y))
    { // if end button pressed, startgame() function called to reset game
        startGame();
        return; // empty return as function void to leave function
    }

    // start button
    if (start.inside(x, y))
    {
        resetPlayerCountButtons(); // resets all the button positions so they dont get in the way
        resetNumOfQuestionsButtons();
        setNumOfQuestions();
        getData();                 // gets data from JSON when the user presses start ( called only here to avoid rate limiting)
        appState = PREP_QUESTIONS; // preps questions
        return;
    }

    // checks which button user pressed for num of questions
    for (ofRectangle btn : numOfQuestionsBtns)
    {
        if (btn.inside(x, y))
        {
            // set question counter
            numOfQuestionsIndex = z;
            setNumOfQuestions(); // updates the total questions based on the users choice
            return;
        };
        z++;
    }

    // checks which button user pressed for player count
    for (ofRectangle btn : playerCountBtns)
    {
        if (btn.inside(x, y))
        {
            numPlayers = i + 1;
            resetScores();       // resets scores to 0 each time to update vector containing them
            setNumOfQuestions(); // updates the total questions based on the users choice
            return;
        };
        i++;
    }

    // answer button check
    for (ofRectangle btn : answerButtons)
    {
        if (btn.inside(x, y))
        {
            if (c == correctAnswerBtnIndex)
            {                              // checks if the button that was pressed had an index equal to the correct answer button
                updateScore();             // updates score accordingly
                appState = CORRECT_ANSWER; // changes state to correct answer state
            }
            else
            {
                appState = INCORRECT_ANSWER;
            } // set timer for frame delay in correct/incorrect screens
            timer = ofGetElapsedTimef(); // elapsed time returns float time in seconds - called here as this when user pressed button
            break;
        }
        c++;
    }
}
