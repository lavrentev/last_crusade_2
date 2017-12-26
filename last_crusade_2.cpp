#include <iostream>
#include <string>
#include <vector>
#include <algorithm>


int main()
{
	int W; // number of columns
	int H; // number of rows
	std::cin >> W >> H; std::cin.ignore();
	for (int i = 0; i < H; i++)
	{
		std::string LINE; // each line represents a line in the grid and contains W integers T. The absolute value of T specifies the type of the room. If T is negative, the room cannot be rotated.
		getline(std::cin, LINE);
	}
	int EX; // the coordinate along the X axis of the exit.
	std::cin >> EX; std::cin.ignore();

	// game loop
	while (1)
	{
		int XI;
		int YI;
		std::string POSI;
		std::cin >> XI >> YI >> POSI; std::cin.ignore();
		int R; // the number of rocks currently in the grid.
		std::cin >> R; std::cin.ignore();
		for (int i = 0; i < R; i++)
		{
			int XR;
			int YR;
			std::string POSR;
			std::cin >> XR >> YR >> POSR; std::cin.ignore();
		}

		// Write an action using cout. DON'T FORGET THE "<< endl"
		// To debug: cerr << "Debug messages..." << endl;


		// One line containing on of three commands: 'X Y LEFT', 'X Y RIGHT' or 'WAIT'
		std::cout << "WAIT" << std::endl;
	}
	
	return 0;
}