#include "Map.h"
#include "Tree.h"

#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <algorithm>


int main()
{
	int W; // number of columns
	int H; // number of rows
	std::cin >> W >> H; std::cin.ignore();
	room_type* T = new room_type[H*W];
	for (int i = 0; i < H; i++)
	{
		std::string LINE; // each line represents a line in the grid and contains W integers T. The absolute value of T specifies the type of the room. If T is negative, the room cannot be rotated.
		getline(std::cin, LINE);
		std::stringstream ss(LINE);
		for (int j = 0; j < W; j++)
			ss >> T[i*W + j];
	}
	int EX; // the coordinate along the X axis of the exit.
	std::cin >> EX; std::cin.ignore();

	Map maze(W, H, EX, T);
	PTree tree( &maze );

	// game loop
	int status;
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
			maze.add_rock(XR, YR, POSR);
		}

		status = tree.update_tree(XI, YI, POSI);

		if( status == -1 )
			std::cerr << "Error updating tree." << std::endl;
		if( status == 1 )
			std::cerr << "Couldn't find a solution." << std::endl;

		std::cout << tree.next_command() << std::endl;
	}
	
	return 0;
}