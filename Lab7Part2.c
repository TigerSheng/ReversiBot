/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   Lab7Part1.c
 * Author: shengha5
 *
 * Created on March 9, 2017, 10:08 AM
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include <time.h>
#include <sys/time.h>
#include <sys/resource.h>
#include "lab7part2lib.h"

typedef struct {
    char board[26][26];
} Board;

/*
 * 
 */
bool positionInBounds(char board[][26], int n, char row, char col);
bool checkLegalInDirection(char board[][26], int n, char row, char col, char colour, int deltaRow, int deltaCol);
void printBoard(char board[][26], int n);
bool checkMove(char board[][26], int n, char seq[], char color);
void makeMove(char board[][26], int n, char seq[], char color);
void clearDirection(char board[][26], int n, char row, char col, char colour, int deltaRow, int deltaCol);
void endGame(char board[][26], int n);
bool checkLegalMoves(char board[][26], int n, char color);
bool checkEndGame(char board[][26], int n);
int getLegalMoves(char board[][26], int n, char avail[][2], char color);
int checkUnoccupied(char board[][26], int n);
bool isCorner(char seq[2], int n);
int pickBestMove(char board[][26], int n, char color, char seq[], double timeLimit);
void shuffle(char array[][2], int n);
bool isStable(char board[][26], int n, char color, int row, int col);
bool isBoundary(char board[][26], int n, int row, int col);
bool isInFilledRow(char board[][26], int n, int row, int col, int deltaRow, int deltaCol);

int main(int argc, char** argv) {
    //declare variables
    int n, numMoves, bSteps = 0, wSteps = 0;
    char board[26][26];
    char seq[2];
    char AIColor, humanColor;
    int row = 0, col = 0;
    struct rusage usage; // a structure to hold "resource usage" (including time)
    struct timeval start, end; // will hold the start and end times
    double stepTime = 0.0, timeLimit = 0.0;

    bool done = false, valid, hAvail = false, cAvail = false;

    //get n input from user
    printf("Enter the board dimension: ");
    scanf("%d", &n);

    //initialize available move array and score array
    char availMoves[n * n - 4][2];
    int score[n * n - 4];
    for (int i = 0; i < n * n - 4; i++) {
        availMoves[i][0] = '0';
        availMoves[i][1] = '0';
        score[i] = 0;
    }

    //get computer color
    printf("Computer plays (B/W) : ");
    scanf(" %c", &AIColor);

    //initialize human color variable
    if (AIColor == 'W')
        humanColor = 'B';
    else if (AIColor == 'B')
        humanColor = 'W';

    //initialize and print board
    //row loop
    for (int i = 0; i < n; i++) {
        //column loop
        for (int j = 0; j < n; j++) {
            //put W's at [n/2][n/2] and [n/2-1][n/2-1]
            if ((i == n / 2 && j == n / 2) || (i == n / 2 - 1 && j == n / 2 - 1))
                board[i][j] = 'W';
                //put B's at [n/2-1][n/2] and [n/2][n/2-1]
            else if ((i == n / 2 - 1 && j == n / 2) || (i == n / 2 && j == n / 2 - 1))
                board[i][j] = 'B';
                //put U's at all other positions
            else
                board[i][j] = 'U';
        }
    }
    printBoard(board, n);

    //first step for human player if AI plays white
    if (AIColor == 'W' && n != 2) {
        //test code
        findSmartestMove(board, n, humanColor, &row, &col);
        printf("Testing AI move (row, col): %c%c\n", row + 'a', col + 'a');
        seq[0] = (char) row + 'a';
        seq[1] = (char) col + 'a';
//        //prompt user to enter move
//                printf("Enter move for colour B (RowCol): ");
//                for (int i = 0; i < 2; i++)
//                    scanf(" %c", &seq[i]);
        //check if this move is valid
//        valid = checkMove(board, n, seq, humanColor);
//        if (valid) { //valid branch
//            //make this move
//            wSteps++;
//            makeMove(board, n, seq, humanColor);
//            //print board
//            printBoard(board, n);
//        } else { //not valid branch
//            //end the game
//            printf("Invalid move.\n");
//            done = true;
//        }
    } else if (n == 2) //if n is 2
        done = true;

    bool timeUp = false;
    int finishedInd = 0;
    //main game loop
    while (!done) {
        getrusage(RUSAGE_SELF, &usage);
        start = usage.ru_utime;
        double timeStart = start.tv_sec +
                start.tv_usec / 1000000.0; // in seconds
        //check available moves for white and if game is ended            
        cAvail = checkLegalMoves(board, n, AIColor);
        done = checkEndGame(board, n);

        //computer move
        if (cAvail && !done) { //if there is a valid move for black
            //AI TODO
            //get number of moves
            numMoves = getLegalMoves(board, n, availMoves, AIColor);
            Board *boards;
            int moveIndex = -1;
            boards = malloc(numMoves * sizeof (Board));
            //check corner moves
            for (int i = 0; i <= numMoves; i++) {
                if (isCorner(availMoves[i], n) == true)
                    moveIndex = i;
            }
            //if there is any corner moves available
            if (moveIndex != -1) {
                //take corner move
                seq[0] = availMoves[moveIndex][0];
                seq[1] = availMoves[moveIndex][1];
            }
            else { //if there is no corner move available
                //shuffle all moves
                shuffle(availMoves, numMoves);
                //calculate time for each step
                timeLimit = 1000.0 / (double) numMoves;
                //loop through all moves to look for best solution
                for (int i = 0; i < numMoves && !timeUp; i++) {
                    //timing code
                    getrusage(RUSAGE_SELF, &usage);
                    start = usage.ru_utime;
                    double timeStart = start.tv_sec +
                            start.tv_usec / 1000000.0; // in seconds
                    //initialize a temporary board                
                    for (int j = 0; j < n; j++) {
                        for (int k = 0; k < n; k++) {
                            (boards + i)->board[j][k] = board[j][k];
                        }
                    }
                    //recurse this function to mark all potential moves
                    //initialize the potential move sequence
                    seq[0] = availMoves[i][0];
                    seq[1] = availMoves[i][1];

                    //get score for each move
                    score[i] = pickBestMove((boards + i)->board, n, AIColor, seq, timeLimit);
//                    printf("%c%c, score: %d\n", seq[0], seq[1], score[i]);
                    //timing
                    getrusage(RUSAGE_SELF, &usage);
                    end = usage.ru_utime;
                    double timeEnd = end.tv_sec +
                            end.tv_usec / 1000000.0; // in seconds
                    double totalTime = timeEnd - timeStart;
                    stepTime += totalTime;
                    if (stepTime + totalTime > 1.0)
                        timeUp = true;
                    finishedInd = i;
                }
                timeUp = false;
                //            printf("This move took %.3lf seconds.\n", stepTime);
                //sort all scores the AI got and find out the best move
                int tempMax = score[0], tempIndex = 0;
                if (finishedInd < 1) {
                    srand(time(0));
                    tempIndex = rand() % numMoves;
                } else {
                    for (int i = 0; i <= finishedInd; i++) {
                        if (score[i] > tempMax) {
                            tempMax = score[i];
                            tempIndex = i;
                        }
                    }
                }
                //initialize the corresponding move array 
                seq[0] = availMoves[tempIndex][0];
                seq[1] = availMoves[tempIndex][1];  
            }

            //make the best move
            makeMove(board, n, seq, AIColor);
            bSteps++;
            getrusage(RUSAGE_SELF, &usage);
            end = usage.ru_utime;
            double timeEnd = end.tv_sec +
                    end.tv_usec / 1000000.0; // in seconds
            double totalTime = timeEnd - timeStart;

            //print the move
            printf("Computer places %c at %c%c.\n", AIColor, seq[0], seq[1]);
//            printf("This move took %.3lf seconds.\n", totalTime);
            //print the new board
            printBoard(board, n);
            //reset best move, score array
            for (int i = 0; i < n * n; i++) {
                availMoves[i][0] = '0';
                availMoves[i][1] = '0';
                score[i] = 0;
            }
            seq[0] = 0;
            seq[1] = 0;  
            free(boards);
            stepTime = 0.0;
        }//if there is no valid move
        else if (!cAvail && !done)
            //print there is no valid black move
            printf("%c player has no valid move.\n", AIColor);

        if (!done) {
            //check available moves for white and if game is ended
            hAvail = checkLegalMoves(board, n, humanColor);
            done = checkEndGame(board, n);

            if (hAvail && !done) {
//                //if there is a valid move for white
//                //prompt white user to make move
//                printf("Enter move for colour %c (RowCol): ", humanColor);
//                for (int i = 0; i < 2; i++)
//                    scanf(" %c", &seq[i]);
//                //check if this move is valid
//                valid = checkMove(board, n, seq, humanColor);
//                if (valid) { //valid branch
//                    //make this move
//                    makeMove(board, n, seq, humanColor);
//                    //print board
//                    printBoard(board, n);
//                } else { //not valid branch
//                    //end the game
//                    printf("Invalid move.\n");
//                    done = true;
//                }
                
                //AI test code
                findSmartestMove(board, n, humanColor, &row, &col);
                printf("Testing AI move (row, col): %c%c\n", row + 'a', col + 'a');
                seq[0] = (char) row + 'a';
                seq[1] = (char) col + 'a';

                //make this move
                makeMove(board, n, seq, humanColor);
                wSteps++;
                //print board
                printBoard(board, n);
            } else if (!hAvail && !done)
                //print there is no valid white move
                printf("%c player has no valid move.\n", humanColor);
        }
    }
    //print endgame message
    endGame(board, n);
    return (EXIT_SUCCESS);
}


//function to find the best move

int pickBestMove(char board[][26], int n, char color, char seq[], double timeLimit) {
    struct rusage usage; // a structure to hold "resource usage" (including time)
    struct timeval start, end; // will hold the start and end times

    getrusage(RUSAGE_SELF, &usage);
    start = usage.ru_utime;
    double timeStart = start.tv_sec +
            start.tv_usec / 1000000.0; // in seconds         

    //judge if this move is a best move by counting total number of tiles of different colors
    int bCount = 0, wCount = 0, numUn, numMoves, result = 0;
    //initialize the next calling color
    char callColor;
    char callSeq[2];
    if (color == 'W')
        callColor = 'B';
    else if (color == 'B')
        callColor = 'W';

    //make this move
    makeMove(board, n, seq, color);
    //        printf("Branch %c%c for color %c\n", seq[0], seq[1], color);
    //                printBoard(board, n);
    //                printf("\n");


    //see if it is end of game
    bool endGame = checkEndGame(board, n);
    if (timeLimit <= 0.75 || endGame) {
//    if (endGame) {
        //TODO: new heuristic needed
        for (int i = 0; i < n; i++) {
            for (int j = 0; j < n; j++) {
                if (board[i][j] == 'B' && isStable(board, n, 'B', i, j))
                    bCount++;
                else if (board[i][j] == 'W' && isStable(board, n, 'W', i, j))
                    wCount++;
            }
        }

        //pick best move for white
        if (color == 'W') {
            if (wCount > bCount) {
                //good move when b < w
                result = 1;
                return result;
            } else if (wCount < bCount) {
                //not good
                result = -1;
                return result;
            }
        }//pick best move for black
        else if (color == 'B') {
            if (bCount > wCount) {
                //good move when b > w
                result = 1;
                return result;
            } else if (bCount < wCount) {
                //not good
                result = -1;
                return result;
            }
        }
        if (bCount == wCount) {
            result += 0;
            //draw
            return result;
        }
    }

    //if the game is not done, get number of unoccupied grids
    numUn = checkUnoccupied(board, n);
    //initialize potential move array and score array
    char potMoves[numUn][2];
    int score[numUn];
    for (int i = 0; i < numUn; i++) {
        potMoves[i][0] = 0;
        potMoves[i][1] = 0;
        score[i] = 0;
    }
    //get all potential moves
    numMoves = getLegalMoves(board, n, potMoves, callColor);
    Board *boards;
    boards = malloc(numMoves * sizeof (Board));

    if(numMoves < 10)
        result -= numMoves;
    else
        result -= 10;

    
    //if there is any potential moves for the opponent
    if (numMoves != 0) {
        //calculate new time limit
        timeLimit = timeLimit / (double) numMoves;
        //check for each potential move
        for (int i = 0; i < numMoves; i++) {
            //initialize call sequence
            callSeq[0] = potMoves[i][0];
            callSeq[1] = potMoves[i][1];

            for (int j = 0; j < n; j++) {
                for (int k = 0; k < n; k++) {
                    (boards + i)->board[j][k] = board[j][k];
                }
            }
            //recurse this function to mark all potential moves
            score[i] = pickBestMove((boards + i)->board, n, callColor, callSeq, timeLimit);
            //            printf("branch %c%c for color %c, score: %d", callSeq[0], callSeq[1], callColor, score[i]);
        }
        //find the highest score of the opponent in this move   
        int tempMax = score[0];
        for (int i = 0; i < numMoves; i++) {
            if (score[i] > tempMax)
                tempMax = score[i];
        }
        //assume the enemy will take the best move for them
        //the better their move is, the worse for us
            result -= tempMax;
        } else {
        result += numMoves;
        //directly score for potential friendly moves
        numMoves = getLegalMoves(board, n, potMoves, color);
        //calculate new time limit
        timeLimit = timeLimit / (double) numMoves;
        //check for each potential move
        boards = malloc(numMoves * sizeof (Board));
        for (int i = 0; i < numMoves; i++) {
            //initialize call sequence
            callSeq[0] = potMoves[i][0];
            callSeq[1] = potMoves[i][1];

            for (int j = 0; j < n; j++) {
                for (int k = 0; k < n; k++) {
                    (boards + i)->board[j][k] = board[j][k];
                }
            }
            //recurse this function to mark all potential moves
            score[i] = pickBestMove((boards + i)->board, n, color, callSeq, timeLimit);
            //            printf("branch %c%c for color %c, score: %d", callSeq[0], callSeq[1], color, score[i]);
        }
        int tempMin = score[0];
        //find the highest score of us in this move
        for (int i = 0; i < numMoves; i++) {
            if (score[i] < tempMin)
                tempMin = score[i];
        }
        //be prepared for the worse case scenario of friendlies
        result += tempMin;

    }
    free(boards);
    //        printf("Branch %c%c for color %c, score: %d\n", seq[0], seq[1], color, sumScore);

    getrusage(RUSAGE_SELF, &usage);
    end = usage.ru_utime;
    double timeEnd = end.tv_sec +
            end.tv_usec / 1000000.0; // in seconds
    double totalTime = timeEnd - timeStart;

//    printf("%.3lf seconds\n", totalTime);
    return result;
}


//function to print the board

void printBoard(char board[][26], int n) {
    //loop for rows
    for (int i = 0; i < n + 1; i++) {
        //print row coordinates
        if (i == 0)
            printf("  ");
        else
            printf("%c ", 'a' - 1 + i);
        //loop for columns
        for (int j = 0; j < n; j++) {
            if (i == 0)
                //print column coordinates on first row
                printf("%c", 'a' + j);
            else
                //print board rows on the following rows
                printf("%c", board[i - 1][j]);
        }
        //print line breakers at the end of each line
        printf("\n");
    }
    return;
}

//function to check if one spot is a legal move in direction (deltaRow, deltaCol)

bool checkLegalInDirection(char board[][26], int n, char row, char col, char colour, int deltaRow, int deltaCol) {
    //initialize variables
    bool legal = false;
    //initialize the row number and column number of the first position
    //on direction (deltaRow, deltaCol) after the position being checked
    int i = (int) (row - 'a') + deltaRow;
    int j = (int) (col - 'a') + deltaCol;
    //check if this first position is in bounds
    bool inbound = positionInBounds(board, n, (char) 'a' + i, (char) 'a' + j);
    //start looping if this position is in bounds and the move is not legal yet
    while (inbound && !legal) {
        //if checking for white
        if (colour == 'W') {
            //if current position (i,j) is unoccupied on this direction
            //this move is not legal
            if (board[i][j] == 'U')
                return legal;
                //if there is another white tile on the line
            else if (board[i][j] == 'W') {
                //if this white tile is not right next to the position being evaluated
                //this move is legal
                if (i != (int) (row - 'a') + deltaRow || j != (int) (col - 'a') + deltaCol)
                    legal = true;
                    //otherwise this move is not legal
                else
                    return legal;
            }//if there is a black tile, keep incrementing
        }            //if checking for black
        else if (colour == 'B') {
            if (board[i][j] == 'U')
                //if current position (i,j) is unoccupied on this direction
                //this move is not legal
                return legal;
            else if (board[i][j] == 'B') {
                //if this black tile is not right next to the position being evaluated
                //this move is legal
                if (i != (int) (row - 'a') + deltaRow || j != (int) (col - 'a') + deltaCol)
                    legal = true;
                    //otherwise this move is not legal
                else
                    return legal;
            }//if there is a black tile, keep incrementing
        }
        //increase i and j by deltaRow and deltaCol, respectively
        i = i + deltaRow;
        j = j + deltaCol;
        //check if new position is in bounds
        inbound = positionInBounds(board, n, (char) 'a' + i, (char) 'a' + j);
    }
    return legal;
}

//function to check if position is in bounds

bool positionInBounds(char board[][26], int n, char row, char col) {
    //initialize variable
    bool inbound;

    //if row and column numbers are greater than or equal to 0
    //and less than n
    //this position is in bounds
    if ((row - 'a' >= 0 && row - 'a' < n) && (col - 'a' >= 0 && col - 'a' < n))
        inbound = true;
        //otherwise this position is not in bounds
    else
        inbound = false;

    return inbound;
}

//function to check if user move is valid

bool checkMove(char board[][26], int n, char seq[], char color) {
    //initialize variables
    bool valid = false;
    int row = (int) seq[0] - 'a', col = (int) seq[1] - 'a';

    //check if this position indicated by user is unoccupied first
    //move will only be valid if this position is unoccupied
    if (board[row][col] == 'U') {
        //if move is done by white
        if (color == 'W') {
            for (int r = -1; r <= 1 && !valid; r++) { //row loop for checking directions, breaks if move is valid
                for (int c = -1; c <= 1 && !valid; c++) { //column loop for checking directions, breaks if move is valid
                    //checking direction must not be (0,0)
                    if (!(r == 0 && c == 0))
                        //move is valid if it is a legal move in direction checking
                        valid = checkLegalInDirection(board, n, (char) 'a' + row, (char) 'a' + col, color, r, c);
                }
            }
        }            //if move is done by black
        else if (color == 'B') {
            for (int r = -1; r <= 1 && !valid; r++) { //row loop for checking directions, breaks if move is valid
                for (int c = -1; c <= 1 && !valid; c++) { //column loop for checking directions, breaks if move is valid
                    //checking direction must not be (0,0)
                    if (!(r == 0 && c == 0))
                        //move is valid if it is a legal move in direction checking
                        valid = checkLegalInDirection(board, n, (char) 'a' + row, (char) 'a' + col, color, r, c);
                }
            }
        }
    }
    return valid;
}

//function to make a user's move on the board and make all flips

void makeMove(char board[][26], int n, char seq[], char color) {
    //initialize variables
    bool legal = false;
    int row = (int) seq[0] - 'a', col = (int) seq[1] - 'a';

    //make the move base on the sequence for white or black
    //at position (row, col)
    if (color == 'W')
        board[row][col] = 'W';
    else if (color == 'B')
        board[row][col] = 'B';

    //loop through each direction to flip all opposite colors between 2 friendlies
    for (int r = -1; r <= 1; r++) { //row loop
        for (int c = -1; c <= 1; c++) { //column loop
            if (!(r == 0 && c == 0)) { //checking direction must not be (0,0)
                //check if a particular direction is legal
                legal = checkLegalInDirection(board, n, (char) 'a' + row, (char) 'a' + col, color, r, c);
                if (legal) { //if one direction is legal, clear that direction
                    clearDirection(board, n, (char) 'a' + row, (char) 'a' + col, color, r, c);
                    //reset legal boolean
                    legal = false;
                }
            }
        }
    }
    return;
}

//flip all tiles between 2 same color tiles on a straight line

void clearDirection(char board[][26], int n, char row, char col, char colour, int deltaRow, int deltaCol) {
    //initialize first position to check after the position of the move
    int i = (int) (row - 'a') + deltaRow;
    int j = (int) (col - 'a') + deltaCol;
    //check if this position is in bounds
    bool inbound = positionInBounds(board, n, (char) 'a' + i, (char) 'a' + j);
    //initialize done variable
    bool done = false;

    //loop will be executed only if position is in bounds and clearing is not done
    while (inbound && !done) {
        //if doing move for white
        if (colour == 'W') {
            //if encountered a black tile, flip it to white
            if (board[i][j] == 'B')
                board[i][j] = 'W';
                //if encountered another white tile, terminate
            else if (board[i][j] == 'W')
                done = true;
        }            //if doing move for black
        else if (colour == 'B') {
            //if encountered a white tile, flip it to black
            if (board[i][j] == 'W')
                board[i][j] = 'B';
                //if encountered another black tile, terminate
            else if (board[i][j] == 'B')
                done = true;
        }
        //increase i and j by deltaRow and deltaCol, respectively 
        i = i + deltaRow;
        j = j + deltaCol;
        //check if this new position is in bounds
        inbound = positionInBounds(board, n, (char) 'a' + i, (char) 'a' + j);
    }
    return;
}

//function to end this game

void endGame(char board[][26], int n) {
    int bCount = 0, wCount = 0;
    //count number of black and white tiles on the board
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            if (board[i][j] == 'B')
                bCount++;
            else if (board[i][j] == 'W')
                wCount++;
        }
    }
    //final message
    if (bCount < wCount)
        printf("W player wins.\n");
    else if (bCount > wCount)
        printf("B player wins.\n");
    else if (bCount == wCount)
        printf("Draw!\n");

    return;
}

//function to store all legal moves for one color

bool checkLegalMoves(char board[][26], int n, char color) {
    bool legalMove = false;
    int numMoves = 0;

    //check how many moves are available
    for (int i = 0; i < n; i++) { //row loop
        for (int j = 0; j < n; j++) { //column loop
            if (board[i][j] == 'U') { //if position is unoccupied
                for (int r = -1; r <= 1 && !legalMove; r++) { //check each direction
                    for (int c = -1; c <= 1 && !legalMove; c++) {
                        if (!(r == 0 && c == 0))
                            legalMove = checkLegalInDirection(board, n, 'a' + i, 'a' + j, color, r, c);
                    }
                }
                if (legalMove) { //if move is legal increase count by 1
                    numMoves = numMoves + 1;
                    //reset legal variable
                    legalMove = false;
                }
            }
        }
    }
    //if no move is available return false
    if (numMoves == 0)
        return false;
        //if there is any available move
    else {
        return true;
    }
}

//function to check if this game is ended

bool checkEndGame(char board[][26], int n) {
    bool gameEnded = false, legal1, legal2;
    int uCount;
    //check if board is full
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            if (board[i][j] == 'U')
                uCount++;
        }
    }
    //if all positions are occupied
    if (uCount == 0)
        return true;
    //check if there is no legal moves for both
    legal1 = checkLegalMoves(board, n, 'B');
    legal2 = checkLegalMoves(board, n, 'W');
    //if there is no legal move, game is ended
    if (legal1 == false && legal2 == false)
        gameEnded = true;

    return gameEnded;
}

//function to get and store legal move

int getLegalMoves(char board[][26], int n, char avail[][2], char color) {
    bool legalMove = false;
    int count = 0;
    //check how many moves are available
    for (int i = 0; i < n; i++) { //row loop
        for (int j = 0; j < n; j++) { //column loop
            if (board[i][j] == 'U') { //if position is unoccupied
                for (int r = -1; r <= 1 && !legalMove; r++) { //check each direction
                    for (int c = -1; c <= 1 && !legalMove; c++) {
                        if (!(r == 0 && c == 0))
                            legalMove = checkLegalInDirection(board, n, 'a' + i, 'a' + j, color, r, c);
                    }
                }
                if (legalMove) {
                    //store x-coordinate of potential move
                    avail[count][0] = 'a' + i;
                    //store y-coordinate of potential move
                    avail[count][1] = 'a' + j;
                    //count++
                    count = count + 1;
                    legalMove = false;
                }
            }
        }
    }
    //return number of potential moves
    return count;
}

//function to check number of unoccupied grids

int checkUnoccupied(char board[][26], int n) {
    int number = 0;
    //loop through board to find number of unoccupied grids
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            if (board[i][j] == 'U')
                number++;
        }
    }
    return number;
}

//function to check if a move is at corner

bool isCorner(char seq[2], int n) {
    int row = seq[0] - 'a', col = seq[1] - 'a';
    if (row == 0 || row == n - 1) {
        if (col == 0 || col == n - 1)
            return true;
    }
    return false;
}

//shuffling arrays
void shuffle(char array[][2], int n){
    srand(time(0));
    if (n > 1) 
    {
        int i;
        for (i = 0; i < n - 1; i++) {
          int j = i + rand() / (RAND_MAX / (n - i) + 1);
          char t[2] = {array[j][0], array[j][1]};
          array[j][0] = array[i][0];
          array[j][1] = array[i][1];
          array[i][0] = t[0];
          array[i][1] = t[1];
        }
    }
}

//function to check number of stable disks for one color on the board
bool isStable(char board[][26], int n, char color, int row, int col){
    bool stable = true;

    for (int r = -1; r <= 1 && stable; r++) {
        for (int c = -1; c <= 1 && stable; c++) {
            if(!(r == 0 && c == 0)){
                if (positionInBounds(board, n, row + r + 'a', col + c + 'a')) {
                    if (!isBoundary(board, n, row + r, col + c)) {
                        if (!isInFilledRow(board, n, row + r, col + c, r, c)) {
                            stable = false;  
                        }
                    }
                } 
            }
        }
    }

    return stable;
}

//check if a tile is on boundary
bool isBoundary(char board[][26], int n, int row, int col){
    if(row == 0 || row == n || col == 0 || col == n)
        return true;
    else
        return false;
}

//function to check if this tile is in a filled row
bool isInFilledRow(char board[][26], int n, int row, int col, int deltaRow, int deltaCol){
    bool inFilledRow = false;
    char color = board[row][col], oppoColor;
    if(color == 'W')
        oppoColor = 'B';
    else if(color == 'B')
        oppoColor = 'W';
    
    bool changeDir = false, hitBound1 = false, hitBound2 = false, hitOppo1 = false, hitOppo2 = false;
    int tempRow = row + deltaRow, tempCol = col + deltaCol;
    bool inBounds = positionInBounds(board, n, tempRow + 'a', tempCol + 'a');

    while (inBounds && !changeDir) {
        if (board[tempRow][tempCol] == oppoColor || board[tempRow][tempCol] == 'U') {
            hitOppo1 = true;
            changeDir = true;
        } else if (isBoundary(board, n, tempRow, tempCol) == true) {
            hitBound1 = true;
            changeDir = true;
        }
        tempRow = tempRow + deltaRow, tempCol = tempCol + deltaCol;
        inBounds = positionInBounds(board, n, tempRow + 'a', tempCol + 'a');
    }

    tempRow = row - deltaRow, tempCol = col - deltaCol;
    while (inBounds) {
        if (board[tempRow][tempCol] == oppoColor || board[tempRow][tempCol] == 'U') {
            hitOppo2 = true;
        } else if (isBoundary(board, n, tempRow, tempCol) == true) {
            hitBound2 = true;
        }
        tempRow = tempRow - deltaRow, tempCol = tempCol - deltaCol;
        inBounds = positionInBounds(board, n, tempRow + 'a', tempCol + 'a');
    }

    if (!(hitOppo1 == true && hitOppo2 == true))
        inFilledRow = true;
    
    return inFilledRow;
}