/* Author: Shaan Hudani
 * Description: Single Player Reversi against a fairly advanced opponent
 * */
#include <stdio.h>
#include <stdbool.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <math.h>

void printBoard(char board[][26],int n);
bool legalPosition(char board[][26], char row, char column, char color,int n,int directions[]);
bool checkLegalInDirection(char board[][26], int n, char row, char col, char color, int deltaRow, int deltaCol);
bool positionInBounds(int n, char row, char col);
void flipPiecesInDirection(char board[][26], char row, char col, char color, int deltaRow, int deltaCol);
bool availableMoves(char board[][26],int n,char color);
void computerTurn(char color,char board[][26], int n);
void undoMove(char row, char col,char board[][26],int n);
int alphaBetaLoop(char board[][26],int boardSize, char* row, char* col,int depth,bool maxingPlayer,char color,int alpha,int beta);

struct rusage usage; // a structure to hold "resource usage" (including time)
struct timeval start, end; // will hold the start and end times

int main(int argc, char **argv)
{
	int boardSize;
	printf("Enter the board dimension: ");
	scanf("%d",&boardSize);
	char board[26][26];
	
	int i,j;
	for( i=0;i<boardSize;i++){
		for(j=0;j<boardSize;j++){
			board[i][j] = 'U';
		}
	}
	//initilizes the board with everyting empty
	
	int startPos1 = boardSize/2;
	int startPos2 = startPos1-1;
	board[startPos1][startPos1] = 'W';
	board[startPos1][startPos2] = 'B';
	board[startPos2][startPos2] = 'W';
	board[startPos2][startPos1] = 'B';
	//starting positions
	
	char compMove, turn;
	printf("Computer plays (B/W): ");
	scanf("%*c%c",&compMove);
	printBoard(board, boardSize);
	//asks for what turn the computer will play as
	
	turn = 'B';
	bool validMove = true;
	bool availMvsBlack = true;
	bool availMvsWhite = true;
	bool availMvs = true;
	//initilize some variables for the loop
	
	do{
		char row, col;
		
		if(turn == compMove){
			
			getrusage(RUSAGE_SELF, &usage);
			start = usage.ru_utime;
			double timeStart = start.tv_sec + start.tv_usec / 1000000.0; // in seconds
			
			computerTurn(compMove,board,boardSize);
			printBoard(board,boardSize);
			
			getrusage(RUSAGE_SELF, &usage);
			end = usage.ru_utime;
			double timeEnd = end.tv_sec +
			end.tv_usec / 1000000.0; // in seconds
			double totalTime = timeEnd - timeStart;
			printf("\nTotal Time Taken by Comp: %lf",totalTime);
			
		}
		//checks if it is the computer's Move
		else{
			printf("\nEnter move for colour %c (RowCol): ",turn);
			scanf("%*c%c%c",&row,&col);
			
			int directions[16] = {0};
			validMove = legalPosition(board,row,col,turn,boardSize,directions);
			
			int i;
			for(i =0;i<16;i+=2){
				if(directions[i] != 0 || directions[i+1] != 0){
					flipPiecesInDirection(board, row,col,turn,directions[i],directions[i+1]);
				}
			}
			//flips all the legal directions of the square
			
			if(!validMove){
				printf("Invalid Move");
			}
			else{
				printBoard(board,boardSize);
			}
		}
		availMvsBlack = availableMoves(board,boardSize,'B');
		availMvsWhite = availableMoves(board,boardSize,'W');
		
		if(validMove){
				if(turn == 'B' && availMvsWhite){
				turn = 'W';
			}
			else if(turn == 'W' && availMvsBlack){
				turn = 'B';
			}
			else if(turn == 'B' && availMvsBlack){
				printf("W player has no valid move.");
				turn = 'B';
			}
			else if(turn == 'W' && availMvsWhite){
				printf("B player has no valid move.");
				turn = 'W';
			}
			else{
				availMvs = false;
			}
		}
		
	}while(availMvs&&validMove);
	//main game loop
	if(validMove){
		int blackPieces = 0;
		int whitePieces = 0;
		int i,j;
		for( i = 0;i<boardSize;i++){
			for(j = 0;j<boardSize;j++){
				if(board[i][j] == 'W'){
					whitePieces++;
				}
				else if(board[i][j] == 'B'){
					blackPieces++;
				}
			}
		}
		
		if(blackPieces > whitePieces){
			printf("\nB player wins.");
		}
		else if(whitePieces > blackPieces){
			printf("\nW player wins.");
		}
		else{
			printf("\nDraw!");
		}
	}
	else{
		if(turn == 'W'){
			printf("\nB player wins.");
		}
		else{
			printf("\nW player wins.");
		}
	}
	
	
	
	return 0;
}

void flipPiecesInDirection(char board[][26], char row, char col, char color, int deltaRow, int deltaCol){
	int rowNum = row - 'a';
	int colNum = col - 'a';
	do{
		board[rowNum][colNum] = color;
		rowNum += deltaRow;
		colNum += deltaCol;
	}while(board[rowNum][colNum] != color);
}

bool positionInBounds(int n, char row, char col){
	int rowNum = row - 'a';
	int colNum = col - 'a';
	if(rowNum < 0 || colNum < 0){
		return false;
	}
	if(rowNum < n && colNum < n){
		return true;
	}
	else{
		return false;
	}
}

bool checkLegalInDirection(char board[][26], int n, char row, char col, char color, int deltaRow, int deltaCol){
	if(deltaCol==0&&deltaRow==0){
		return false;
	}
	//printf("\nDeltaRow: %d, DeltaCol: %d",deltaRow,deltaCol);
	char newRow = row;
	char newCol = col;
	int newRowNum = newRow - 'a';
	int newColNum = newCol - 'a';
	int counter = 0;
	
	if(board[newRowNum][newColNum] != 'U'){
		//printf("\nRow %c Col %c False1",row,col);
		return false;
	}
	 
	do{
		if(!positionInBounds(n, newRow, newCol)){
			//printf("\nFalse2");
			return false;
		}
		else if(counter > 0 && board[newRowNum][newColNum] == 'U'){
			//printf("\nFalse3");
			return false;
		}
		else if(counter == 1 && color==board[newRowNum][newColNum]){
			//printf("\nFalse4");
			return false;
		}
		else if(color==board[newRowNum][newColNum]){
			//printf("\nDeltaCol %d DeltaRow %d, Color = %c, counter = %d, newColNum = %d, newRowNum = %d",deltaCol,deltaRow,board[newRowNum][newColNum],counter,newColNum,newRowNum);
			//printf("\nTruther");
			return true;
		}
		
		newRow += deltaRow;
		newCol += deltaCol;
		newRowNum = newRow - 'a';
		newColNum = newCol - 'a';
		counter++;
	}while(true);
}

bool legalPosition(char board[][26], char row, char column, char color,int n,int directions[]){
	int rowNum = row - 'a';
	int colNum = column - 'a';
	
	if(board[rowNum][colNum] != 'U'){
		return false;
	}
	
	bool legal = false;
	int counter = 0;
	
	int k,l;
	for(k = -1;k<2;k++){
		for(l = -1;l<2;l++){
			if(checkLegalInDirection(board,n,row,column,color,k,l)){
				directions[counter] = k;
				directions[counter+1] = l;
				counter+=2;
				legal = true;
			}
		}
	}
	/*
	int i;
	
	for(i =0;i<16;i+=2){
		if(directions[i] != 0 || directions[i+1] != 0){
			flipPiecesInDirection(board, row,column,color,directions[i],directions[i+1]);
		}
	}
	 */
	
	if(legal){
		return true;
	}
	else{
		return false;
	}
}

void printBoard(char board[][26],int n){
	
	printf("  ");
	int Letter = 97;
	int i,j;
	for(i = 0;i<n;i++){
		printf("%c",Letter);
		Letter++;
	}
	
	Letter -= n;
	
	for(i = 0;i<n;i++){
		printf("\n%c ", Letter);
		Letter++;
		for(j = 0;j<n;j++){
			printf("%c",board[i][j]);
		}
	}
}

bool availableMoves(char board[][26],int n,char color){
	int i,j;
	for(i = 0; i<n;i++){
		for(j = 0; j<n;j++){
			int directions[16] = {0};
			if(legalPosition(board,i+'a',j+'a',color,n,directions)){
				return true;
			}
		}
	}
	return false;
}

void computerTurn(char color,char board[][26], int n){
	
	int depth = 200;
	char row,col;
	alphaBetaLoop(board,n,&row,&col,depth,true,color,-INFINITY,INFINITY);
	
	int directions[16] = {0};
	legalPosition(board,row,col,color,n,directions);
	
	int i;
	for(i =0;i<16;i+=2){
		if(directions[i] != 0 || directions[i+1] != 0){
			flipPiecesInDirection(board,row,col,color,directions[i],directions[i+1]);
		}
	}
	
	printf("\nComputer places %c at %c%c\n",color,row,col);
}

int alphaBetaLoop(char board[][26],int boardSize, char *row, char *col,int depth,bool maxingPlayer,char color,int alpha,int beta){
	bool whiteMoves = availableMoves(board,boardSize,'W');
	bool blackMoves = availableMoves(board,boardSize,'B');
	if(depth == 0 || (!whiteMoves && !blackMoves)){
		int blackPieces = 0;
		int whitePieces = 0;
		int i,j;
		for( i = 0;i<boardSize;i++){
			for(j = 0;j<boardSize;j++){
				if(board[i][j] == 'W'){
					whitePieces++;
				}
				else if(board[i][j] == 'B'){
					blackPieces++;
				}
			}
		}
		if(color == 'W'){
			return whitePieces - blackPieces;
		}
		else if(color == 'B'){
			return blackPieces - whitePieces;
		}
	}
	//checks if at the end node, if it is, then it returns the value of the board in the current state
	
	if(color == 'W' && !whiteMoves && maxingPlayer){
		maxingPlayer = false;
	}
	else if(color == 'B' && !blackMoves && maxingPlayer){
		maxingPlayer = false;
	}
	else if(color == 'W' && !blackMoves && !maxingPlayer){
		maxingPlayer = true;
	}
	else if(color == 'B' && !whiteMoves && !maxingPlayer){
		maxingPlayer = true;
	}
	//checks to see if the current player can actually play and switches the player if the current player has no moves
	
	if(maxingPlayer){
		int value = -INFINITY;
		
		int i,j;
		for(i = 0;i<boardSize;i++){
			for(j = 0;j<boardSize;j++){
				int directions[16] = {0};
				if(legalPosition(board,i+'a',j+'a',color,boardSize,directions)){
					char boardClone[26][26] = {0};
					
					int k,l;
					for(k = 0;k<boardSize;k++){
						for(l = 0;l<boardSize;l++){
							boardClone[k][l] = board[k][l];
						}
					}
					//copies the board to a temp board clone
					
					int m;
					for(m =0;m<16;m+=2){
						if(directions[m] != 0 || directions[m+1] != 0){
							flipPiecesInDirection(boardClone, i + 'a',j + 'a',color,directions[m],directions[m+1]);
						}
					}
					char row1,col1;
					int score;
					score = alphaBetaLoop(boardClone,boardSize,&row1,&col1,depth-1,false,color,alpha,beta);
					
					if(score>value){
						value = score;
						*row = i+'a';
						*col = j +'a';
					}
					
					if(value>alpha){
						alpha = value;
					}
					
					if(alpha>=beta){
						goto RETURNING;
					}
				}
				//checks if the board position is a child
			}
		}
		RETURNING:return value;
	}
	else{
		int value = INFINITY;
		
		char turnColor;
		if(color == 'W'){
			turnColor = 'B';
		}
		else{
			turnColor = 'W';
		}
		
		int i,j;
		for(i = 0;i<boardSize;i++){
			for(j = 0;j<boardSize;j++){
				int directions[16] = {0};
				if(legalPosition(board,i+'a',j+'a',turnColor,boardSize,directions)){
					char boardClone[26][26] = {0};
					
					int k,l;
					for(k = 0;k<boardSize;k++){
						for(l = 0;l<boardSize;l++){
							boardClone[k][l] = board[k][l];
						}
					}
					//copies the board to a temp board clone
					
					int m;
					for(m =0;m<16;m+=2){
						if(directions[m] != 0 || directions[m+1] != 0){
							flipPiecesInDirection(boardClone, i + 'a',j + 'a',turnColor,directions[m],directions[m+1]);
						}
					}
					
					char row1,col1;
					int score;
					score = alphaBetaLoop(boardClone,boardSize,&row1,&col1,depth-1,true,color,alpha,beta);
					
					if(score<value){
						value = score;
					}
					
					if(value<beta){
						beta = value;
					}
					
					if(alpha>=beta){
						goto RETURNS;
					}
				}
			}
		}
		RETURNS: return value;
	}
}