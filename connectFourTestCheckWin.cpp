#include <iostream>
#include <cstdlib>
#include <fstream>
using namespace std;

const int ROW = 6;
const int COLUMN = 7;
int connect4Board[ROW][COLUMN] = {0};

bool checkWin(int player);

int main()
{
	int numFail = 0;
	ifstream test("test.txt");
	if (!test)
	{
		cout << "Invalid file." << endl;
		return EXIT_FAILURE;
	}
	
	int numTests = 0;
	test >> numTests;
	for (int tests = 0; tests < numTests; tests++)
	{
		for (int x = 0; x < ROW; x++)
			for (int y = 0; y < COLUMN; y++)
				test >> connect4Board[x][y];
		
		bool expected = true;
		cout << "\nTest " << tests + 1 << endl;
		
		for (int player = 1; player < 3; player++)
		{
			test >> expected;
			if (expected == checkWin(player))
				cout << "Pass" << endl;
			else
			{
				cout << "Fail" << endl;
				numFail ++;
			}
		}
	}
	
	cout << "\nNumber Tests Failed: " << numFail << endl;
	return 0;
}

bool checkWin(int player)
{
	for (int i=0; i<COLUMN; i++)
	{
		for (int j=0; j<ROW; j++)
		{
			if (j<3)
			{
				//for vertical
				if (connect4Board[j][i] == player && connect4Board[j + 1][i] == player && connect4Board[j + 2][i] == player && connect4Board[j + 3][i] == player)
					return true;
				if (i<4)
					//for diagonal with positive slope
					if (connect4Board[j][i]==player && connect4Board[j+1][i+1] == player && connect4Board[j+2][i+2] == player && connect4Board[j+3][i+3] == player)						
						return true;
			}

			if (i < 4)
			{
				//for horizontal
				if (connect4Board[j][i] == player && connect4Board[j][i+1] == player && connect4Board[j][i+2] == player && connect4Board[j][i+3] == player)
					return true;
				if (j >= 3)
					//for diagonal with negative slope
					if (connect4Board[j][i] == player && connect4Board[j-1][i+1] == player && connect4Board[j-2][i+2] == player && connect4Board[j-3][i+3] == player)
						return true;
			}
		}
	}
	return false;
}
