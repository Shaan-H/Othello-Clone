/* Author: Shaan Hudani
 * Description: Single Player Reversi against a simple opponent
 * */
#include <stdio.h>
#include <stdbool.h>

void printBoard(char board[][26],int n);
bool legalPosition(char board[][26], char row, char column, char color, int n);
bool checkLegalInDirection(char board[][26], int n, char row, char col, char color, int deltaRow, int deltaCol);
bool positionInBounds(int n, char row, char col);
void flipPiecesInDirection(char board[][26], char row, char col, char color, int deltaRow, int deltaCol);
bool availableMoves(char board[][26],int n,char color);
void computerTurn(char color,char board[][26],int boardSize);
int pointsInDirection(char board[][26], int n, char row, char col, char color, int deltaRow, int deltaCol);

int main(int argc, char **argv)
{
	int boardSize;
	printf("Enter the board dimension: ");
	scanf("%d",&boardSize);
	char board[26][26];
	
	for(int i=0;i<boardSize;i++){
		for(int j=0;j<boardSize;j++){
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
			computerTurn(compMove,board,boardSize);
			printBoard(board,boardSize);
		}
		//checks if it is the computer's Move
		else{
			printf("\nEnter move for colour %c (RowCol): ",turn);
			scanf("%*c%c%c",&row,&col);
			
			validMove = legalPosition(board,row,col,turn,boardSize);
			if(!validMove){
				printf("\nInvalid Move");
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
		for(int i = 0;i<boardSize;i++){
			for(int j = 0;j<boardSize;j++){
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
	int rowNum = row-'a';
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
	}while(positionInBounds(n, newRow, newCol));
}

bool legalPosition(char board[][26], char row, char column, char color,int n){
	int rowNum = row - 'a';
	int colNum = column - 'a';
	
	if(board[rowNum][colNum] != 'U'){
		return false;
	}
	
	bool piecesFlipped = false;
	int directions[16] = {0};
	int counter = 0;
	
	for(int k = -1;k<2;k++){
		for(int l = -1;l<2;l++){
			if(checkLegalInDirection(board,n,row,column,color,k,l)){
				directions[counter] = k;
				directions[counter+1] = l;
				counter+=2;
				piecesFlipped = true;
			}
		}
	}
	for(int i =0;i<16;i+=2){
		if(directions[i] != 0 || directions[i+1] != 0){
			flipPiecesInDirection(board, row,column,color,directions[i],directions[i+1]);
		}
	}
	
	if(piecesFlipped){
		return true;
	}
	else{
		return false;
	}
}

void printBoard(char board[][26],int n){
	
	printf("  ");
	int Letter = 97;
	for(int i = 0;i<n;i++){
		printf("%c",Letter);
		Letter++;
	}
	
	Letter -= n;
	
	for(int i = 0;i<n;i++){
		printf("\n%c ", Letter);
		Letter++;
		for(int j = 0;j<n;j++){
			printf("%c",board[i][j]);
		}
	}
}

bool availableMoves(char board[][26],int n,char color){
	for(int i = 0; i<n;i++){
		for(int j = 0; j<n;j++){
			for(int k = -1;k<2;k++){
				for(int l = -1;l<2;l++){
					if(checkLegalInDirection(board,n,i+'a',j+'a',color,k,l)){
						return true;
					}
				}
			}
		}
	}
	return false;
}

void computerTurn(char color,char board[][26], int n){
	int row1,col1,total1 = 0,row2,col2,total2 = 0;
	
	for(int i = 0; i<n;i++){
		for(int j = 0; j<n;j++){
			row2 = i;
			col2 = j;
			total2 = 0;
			for(int k = -1;k<2;k++){
				for(int l = -1;l<2;l++){
					if(checkLegalInDirection(board,n,i+'a',j+'a',color,k,l)){
						total2 += pointsInDirection(board,n,i+'a',j+'a',color,k,l);
					}
				}
			}
			if(total2>total1){
				row1 = row2;
				col1 = col2;
				total1 = total2;
			}
		}
	}
	//printf("\nCompTurn");
	legalPosition(board,row1+'a',col1+'a',color,n);
	printf("\nComputer places %c at %c%c\n",color,row1+'a',col1+'a');
}

int pointsInDirection(char board[][26], int n, char row, char col, char color, int deltaRow, int deltaCol){
	int rowNum = row-'a';
	int colNum = col - 'a';
	int points = 0;
	
	while(board[rowNum][colNum] != color){
		points++;
		rowNum += deltaRow;
		colNum += deltaCol;
	}
	return points;
}