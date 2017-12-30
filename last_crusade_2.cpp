#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <algorithm>
#include <stack>

#define ABS(x) (((x) < 0) ? ((-1)*(x)) : (x))

typedef short room_type;
typedef enum posi { TOP, LEFT, RIGHT, STOP, NO_ENTRY = -1 } posi_t;
typedef std::stack<std::string> PStack;

class Map
{
	unsigned H, W;		// Height and width of the map.
	unsigned ex, ey;	// Coordinates of the exit.
	room_type* T;		// Types of rooms [-13; 13]; negatives for unrotatable rooms;
						// -1 reserved for returning errors, because
						// you can't rotate type 1 rooms anyway.

public:
	Map();
	Map(unsigned _W, unsigned _H, unsigned _ex, room_type* _T = nullptr);
	Map(const Map& );
	~Map();

	// Access to private members
	unsigned get_width();
	unsigned get_height();
	void set_width(unsigned width);
	void set_height(unsigned height);
	int init(room_type* _T = nullptr);
	room_type get_room_type(unsigned x, unsigned y);
	room_type set_room_type(unsigned x, unsigned y, room_type type = 0);
	void set_exit(unsigned x);
	unsigned get_exit_x();
	unsigned get_exit_y();

	// Functions for rotating the rooms
	room_type rotate_right(unsigned x, unsigned y);
	room_type rotate_left(unsigned x, unsigned y);
	room_type rotate_180(unsigned x, unsigned y);

	// Function for determining the path through the room
	posi_t path_find(unsigned x, unsigned y, posi_t direction);

	// Additional function for determining coordinates of the room
	// an object is about to enter
	int next_room(unsigned& x, unsigned& y, posi_t direction);
};

struct PTreeNode
{
public:
	unsigned x, y;
	posi_t direction;
	PTreeNode* parent;
	PTreeNode* child[4];

	PTreeNode();
	PTreeNode(unsigned _x, unsigned _y, posi_t _direction, PTreeNode* _parent = nullptr);
	~PTreeNode();

	int has_children();
	void add_child(unsigned count, unsigned _x, unsigned _y, posi_t _direction);
};

class PTree
{
	PTreeNode* root;
	Map* map;
	PStack commands;

	bool found_exit;
	PTreeNode* exit_node;

public:
	PTree();
	PTree(Map* _map);
	~PTree();

	// Function for deleting a node and all of its offsprings
private:
	void deleteSubTree(PTreeNode* node);

public:
	// Access to private members
	void set_map(Map* _map);

	// Function fetching the next command and executing it on the 'map'
	std::string next_command();

	int update_tree(unsigned xi, unsigned yi, std::string position);
	int update_tree(unsigned xi, unsigned yi, posi_t position);

private:
	int build_tree(PTreeNode* node, unsigned xi, unsigned yi, posi_t position);
	void backtrack();
	inline void check_add_child(unsigned count, PTreeNode* node, unsigned xi, unsigned yi, posi_t position);
	int actions_left(PTreeNode* node);
};


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

		if( tree.update_tree(XI, YI, POSI) == -1 )
			std::cerr << "Error updating tree" << std::endl;

		// Write an action using cout. DON'T FORGET THE "<< endl"
		// To debug: cerr << "Debug messages..." << endl;


		// One line containing on of three commands: 'X Y LEFT', 'X Y RIGHT' or 'WAIT'
		std::cout << tree.next_command() << std::endl;
	}
	
	return 0;
}


Map::Map() : 
	W(0), H(0), ex(0), ey(0), T(nullptr)
{}

/** 
* Fully initializing constructor
* with _T as a pointer to array of room types
* with which to fill T (unless _T is null-pointer)
*/
Map::Map(unsigned _W, unsigned _H, unsigned _ex, room_type* _T) : 
	W(_W), H(_H), ex(_ex)
{
	if( H>0 && W>0 )
	{
		T = new room_type[H*W];
		if(_T)
		{
			for(unsigned i = 0; i < H*W; ++i)
				T[i] = _T[i];
		}
		else
		{
			for(unsigned i = 0; i < H*W; ++i)
				T[i] = 0;
		}
		ey = H-1;
	}
	else
	{
		ey = 0;
		T = nullptr;
	}
}

Map::Map(const Map& arg) : 
	W(arg.W), H(arg.H)
{
	if( W>0 && H>0 && !(arg.T) )
	{
		T = new room_type[H*W];
		for( unsigned i = 0; i < H*W; ++i )
			T[i] = arg.T[i];
	}
	else
		T = nullptr;
}

Map::~Map(void)
{
	if(T)
		delete[] T;
}

unsigned Map::get_width()
{
	return W;
}

unsigned Map::get_height()
{
	return H;
}

void Map::set_width(unsigned width)
{
	// Changes the rooms to accomodate
	// the new width of the map.
	// Fills the new rooms with type 0 rooms.
	if( H>0 && width > W && T )
	{
		room_type* temp_T = new room_type[H*W];
		unsigned i, j;
		for( i = 0; i < H*W; ++i )
			temp_T[i] = T[i];
		delete[] T;
		T = new room_type[H*width];
		for( i = 0; i < H; ++i )
			for( j = 0; j < W; ++j )
				T[j + i*width] = temp_T[j + i*W];
		for( i = 0; i < H; ++i )
			for( j = W; j < width; ++j )
				T[j + i*width] = 0;
	}
	W = width;
}

void Map::set_height(unsigned height)
{
	// Changes the rooms to accomodate
	// the new height of the map.
	// Fills the new rooms with type 0 rooms.
	if( height > H && W>0 && T )
	{
		room_type* temp_T = new room_type[H*W];
		unsigned i, j;
		for( i = 0; i < H*W; ++i )
			temp_T[i] = T[i];
		delete[] T;
		T = new room_type[height*W];
		for( i = 0; i < H; ++i )
			for( j = 0; j < W; ++j )
				T[j + i*W] = temp_T[j + i*W];
		for( i = H; i < height; ++i )
			for( j = 0; j < W; ++j )
				T[j + i*W] = 0;
	}
	H = height;
	ey = H-1;
}

/**
 * Initializes or re-initializes the rooms array
 * 
 * @param pointer to rooms array
 * @return 0 for success or -1 for error
 */
int Map::init(room_type* _T)
{
	if( H>0 && W>0 )
	{
		delete[] T;
		T = new room_type[H*W];
		if(_T)
		{
			for(unsigned i = 0; i < H*W; ++i)
				T[i] = _T[i];
		}
		else
		{
			for(unsigned i = 0; i < H*W; ++i)
				T[i] = 0;
		}
		return 0;
	}
	else
		return -1;
}

room_type Map::get_room_type(unsigned x, unsigned y)
{
	if( x>=W || y>=H || !T )
		return -1;
	return T[x + y*W];
}

room_type Map::set_room_type(unsigned x, unsigned y, room_type type)
{
	if( x>=W || y>=H || !T )
		return -1;
	if( type < -13 || type > 13)
		return -1;
	T[x + y*W] = type;
	return type;
}

void Map::set_exit(unsigned x)
{
	ex = x;
	if( H>0 )
		ey = H-1;
}

unsigned Map::get_exit_x()
{
	return ex;
}

unsigned Map::get_exit_y()
{
	return ey;
}

/**
 * Rotates the room at (x,y) clockwise if possible
 * 
 * @param coordinates of the room x,y
 * @return -1 for error, current type of a room
 * if couldn't rotate it, or new type of a rotated room
 */
room_type Map::rotate_right(unsigned x, unsigned y)
{
	if( x>=W || y>=H || !T )
		return -1;

	room_type type = T[x + y*W];

	// Cannot rotate these rooms
	if( type < 2 )
		return type;

	switch( type )
	{
		case 2:
			type = 3;
			break;
		case 3:
			type = 2;
			break;
		case 4:
			type = 5;
			break;
		case 5:
			type = 4;
			break;
		case 6:
			type = 7;
			break;
		case 7:
			type = 8;
			break;
		case 8:
			type = 9;
			break;
		case 9:
			type = 6;
			break;
		case 10:
			type = 11;
			break;
		case 11:
			type = 12;
			break;
		case 12:
			type = 13;
			break;
		case 13:
			type = 10;
			break;
		default:
			break;
	}

	T[x + y*W] = type;
	return type;
}

/**
 * Rotates the room at (x,y) counter-clockwise if possible
 * 
 * @param coordinates of the room x,y
 * @return -1 for error, current type of a room
 * if couldn't rotate it, or new type of a rotated room
 */
room_type Map::rotate_left(unsigned x, unsigned y)
{
	if( x>=W || y>=H || !T )
		return -1;

	room_type type = T[x + y*W];

	// Cannot rotate these rooms
	if( type < 2 )
		return type;

	switch( type )
	{
		case 2:
			type = 3;
			break;
		case 3:
			type = 2;
			break;
		case 4:
			type = 5;
			break;
		case 5:
			type = 4;
			break;
		case 6:
			type = 9;
			break;
		case 7:
			type = 6;
			break;
		case 8:
			type = 7;
			break;
		case 9:
			type = 8;
			break;
		case 10:
			type = 13;
			break;
		case 11:
			type = 10;
			break;
		case 12:
			type = 11;
			break;
		case 13:
			type = 12;
			break;
		default:
			break;
	}

	T[x + y*W] = type;
	return type;
}

/**
 * Rotates the room at (x,y) 180 degrees if possible
 * 
 * @param coordinates of the room x,y
 * @return -1 for error, current type of a room
 * if couldn't rotate it, or new type of a rotated room
 */
room_type Map::rotate_180(unsigned x, unsigned y)
{
	if( x>=W || y>=H || !T )
		return -1;

	room_type type = T[x + y*W];

	// Cannot rotate these rooms
	if( type < 6 )
		return type;

	switch( type )
	{
		case 6:
			type = 8;
			break;
		case 7:
			type = 9;
			break;
		case 8:
			type = 6;
			break;
		case 9:
			type = 7;
			break;
		case 10:
			type = 12;
			break;
		case 11:
			type = 13;
			break;
		case 12:
			type = 10;
			break;
		case 13:
			type = 11;
			break;
		default:
			break;
	}

	T[x + y*W] = type;
	return type;
}

/**
 * Finds a path through a room at (x,y) if any exists
 * 
 * @param coordinates of the room x,y
 *		  and direction of entry
 * @return NO_ENTRY for error, STOP if no path exists,
 * TOP if entering the *next room* from the top,
 * LEFT - from the left, RIGHT - from the right
 */
posi_t Map::path_find(unsigned x, unsigned y, posi_t direction)
{
	if( x>=W || y>=H || !T || direction == NO_ENTRY || direction == STOP )
		return NO_ENTRY;

	room_type type = ABS(T[x + y*W]);
	posi_t ret;

	switch( type )
	{
		case 0:
			ret = STOP;
			break;
		case 1:
			ret = TOP;
			break;
		case 2:
		case 6:
			{
				if( direction == TOP )
					ret = STOP;
				else
					ret = direction;
			}
			break;
		case 3:
			{
				if( direction == TOP )
					ret = TOP;
				else ret = STOP;
			}
			break;
		case 4:
			{
				if( direction == TOP )
					ret = RIGHT;
				else if( direction == RIGHT )
					ret = TOP;
				else ret = STOP;
			}
			break;
		case 5:
			{
				if( direction == TOP )
					ret = LEFT;
				else if( direction == LEFT )
					ret = TOP;
				else ret = STOP;
			}
			break;
		case 7:
			{
				if( direction == LEFT )
					ret = STOP;
				else
					ret = TOP;
			}
			break;
		case 8:
			{
				if( direction == TOP )
					ret = STOP;
				else
					ret = TOP;
			}
			break;
		case 9:
			{
				if( direction == RIGHT )
					ret = STOP;
				else
					ret = TOP;
			}
			break;
		case 10:
			{
				if( direction == TOP )
					ret = RIGHT;
				else
					ret = STOP;
			}
			break;
		case 11:
			{
				if( direction == TOP )
					ret = LEFT;
				else
					ret = STOP;
			}
			break;
		case 12:
			{
				if( direction == RIGHT )
					ret = TOP;
				else
					ret = STOP;
			}
			break;
		case 13:
			{
				if( direction == LEFT )
					ret = TOP;
				else
					ret = STOP;
			}
			break;
		default:
			ret = NO_ENTRY;
			break;
	}

	return ret;
}

/**
 * Gives the coordinates of the next room on the path
 * found by path_find
 * 
 * @param coordinates of the last room x,y 
 *        [will be changed to next on success],
 *		  and direction of entry (from path_find)
 * @return -1 for error, 0 for success, 1 for getting
 *		   out of bounds and for STOP
 */
int Map::next_room(unsigned& x, unsigned& y, posi_t direction)
{
	switch( direction )
	{
		case TOP:
			if( y == H-1 )
				return 1;
			y++;
			break;
		case RIGHT:
			if( x == 0 )
				return 1;
			x--;
			break;
		case LEFT:
			if( x == W-1 )
				return 1;
			x++;
			break;
		case STOP:
			return 1;
		default:
			return -1;
	}
	return 0;
}

PTreeNode::PTreeNode()
{
	x = 0;
	y = 0;
	direction = NO_ENTRY;
	parent = nullptr;
	child[0] = nullptr;
	child[1] = nullptr;
	child[2] = nullptr;
	child[3] = nullptr;
}

PTreeNode::PTreeNode(unsigned _x, unsigned _y, posi_t _direction, PTreeNode* _parent) :
	x(_x), y(_y), direction(_direction), parent(_parent)
{
	child[0] = nullptr;
	child[1] = nullptr;
	child[2] = nullptr;
	child[3] = nullptr;
}

PTreeNode::~PTreeNode()
{}

/**
 * Checks if the current node has children
 * 
 * @return Amount of children
 */
int PTreeNode::has_children()
{
	int count = 0;
	for( int i = 0; i < 4; ++i )
	{
		if(child[i])
			count++;
	}
	return count;
}

/**
 * Adds a child to current node
 * 
 * @param count Index of the child
 */
void PTreeNode::add_child(unsigned count, unsigned _x, unsigned _y, posi_t _direction)
{
	child[count] = new PTreeNode(_x, _y, _direction, this);
}



PTree::PTree() :
	root(nullptr), map(nullptr), found_exit(false), exit_node(nullptr)
{}

PTree::PTree(Map* _map) :
	root(nullptr), found_exit(false), exit_node(nullptr)
{
	map = _map;
}

PTree::~PTree()
{
	if(root)
	{
		deleteSubTree(root->child[0]);
		deleteSubTree(root->child[1]);
		deleteSubTree(root->child[2]);
		deleteSubTree(root->child[3]);

		delete root;
	}
}

void PTree::deleteSubTree(PTreeNode* node)
{
	if(node)
	{
		deleteSubTree(node->child[0]);
		deleteSubTree(node->child[1]);
		deleteSubTree(node->child[2]);
		deleteSubTree(node->child[3]);

		delete node;
	}
}

std::string PTree::next_command()
{
	if( commands.empty() )
		return std::string("WAIT");
	else
	{
		std::string current_command( commands.top() );
		std::stringstream temp( current_command );

		unsigned x, y;
		std::string rotation;
		temp >> x >> y >> rotation;
		if( rotation.compare("RIGHT") == 0 )
			map->rotate_right(x, y);
		if( rotation.compare("LEFT") == 0 )
			map->rotate_left(x, y);

		commands.pop();
		return current_command;
	}
}

int PTree::actions_left(PTreeNode* node)
{
	int count = 0;
	PTreeNode* temp = node;
	while( temp != root )
	{
		if( temp == temp->parent->child[0] )
			count++;
		if( temp == temp->parent->child[3] )
			count--;
		temp = temp->parent;
	}
	return count;
}

void PTree::check_add_child(unsigned count, PTreeNode* node, unsigned xi, unsigned yi, posi_t position)
{
	if( node->child[count] )
	{
		if( node->child[count]->x != xi || node->child[count]->y != yi )
			deleteSubTree( node->child[count] );
	}
	node->add_child(count, xi, yi, position);
}

int PTree::build_tree(PTreeNode* node, unsigned xi, unsigned yi, posi_t position)
{
	unsigned next_x, next_y;
	posi_t next_posi;
	int status;
	room_type room[4];

	room[0] = map->get_room_type(xi, yi);

	next_x = xi;
	next_y = yi;
	next_posi = map->path_find(xi, yi, position);
	status = map->next_room(next_x, next_y, next_posi);
	if( status == -1 )
		return -1;
	if( status == 0 )
	{
		check_add_child(0, node, xi, yi, position);

		if( next_x == map->get_exit_x() && next_y == map->get_exit_y() )
		{
			found_exit = true;
			exit_node = node->child[0];
			map->set_room_type(xi, yi, room[0]);
			return 0;
		}

		status = build_tree(node->child[0], next_x, next_y, next_posi);
		if( status != 1 )
			return status;
	}

	next_x = xi;
	next_y = yi;
	room[1] = map->rotate_left(xi, yi);
	if( room[1] != room[0] )
	{
		next_posi = map->path_find(xi, yi, position);
		status = map->next_room(next_x, next_y, next_posi);
		if( status == -1 )
		{
			map->set_room_type(xi, yi, room[0]);
			return -1;
		}
		if( status == 0 )
		{
			check_add_child(1, node, xi, yi, position);

			if( next_x == map->get_exit_x() && next_y == map->get_exit_y() )
			{
				found_exit = true;
				exit_node = node->child[1];
				map->set_room_type(xi, yi, room[0]);
				return 0;
			}

			status = build_tree(node->child[1], next_x, next_y, next_posi);
			if( status != 1 )
			{
				map->set_room_type(xi, yi, room[0]);
				return status;
			}
		}
	}

	next_x = xi;
	next_y = yi;
	room[2] = map->rotate_180(xi, yi);
	if( room[2] != room[1] && room[2] != room[0] )
	{
		next_posi = map->path_find(xi, yi, position);
		status = map->next_room(next_x, next_y, next_posi);
		if( status == -1 )
		{
			map->set_room_type(xi, yi, room[0]);
			return -1;
		}
		if( status == 0 )
		{
			check_add_child(2, node, xi, yi, position);

			if( next_x == map->get_exit_x() && next_y == map->get_exit_y() )
			{
				found_exit = true;
				exit_node = node->child[2];
				map->set_room_type(xi, yi, room[0]);
				return 0;
			}

			status = build_tree(node->child[2], next_x, next_y, next_posi);
			if( status != 1 )
			{
				map->set_room_type(xi, yi, room[0]);
				return status;
			}
		}
	}

	next_x = xi;
	next_y = yi;
	room[3] = map->rotate_right(xi, yi);
	if( room[3] != room[2] && room[3] != room[1] && room[3] != room[0] )
	{
		if( actions_left(node) > 0 )
		{
			next_posi = map->path_find(xi, yi, position);
			status = map->next_room(next_x, next_y, next_posi);
			if( status == -1 )
			{
				map->set_room_type(xi, yi, room[0]);
				return -1;
			}
			if( status == 0 )
			{
				check_add_child(3, node, xi, yi, position);

				if( next_x == map->get_exit_x() && next_y == map->get_exit_y() )
				{
					found_exit = true;
					exit_node = node->child[3];
					map->set_room_type(xi, yi, room[0]);
					return 0;
				}

				status = build_tree(node->child[3], next_x, next_y, next_posi);
				if( status != 1 )
				{
					map->set_room_type(xi, yi, room[0]);
					return status;
				}
			}
		}
	}

	map->set_room_type(xi, yi, room[0]);
	return 1;
}

void PTree::backtrack()
{
	if( exit_node )
	{
		PTreeNode* curr_node = exit_node;
		while( curr_node != root )
		{
			std::string str;
			std::stringstream ss( str );

			if( curr_node == curr_node->parent->child[1] )
			{
				ss << curr_node->x << ' ' << curr_node->y << ' ' << "LEFT";
				std::getline(ss, str);
				commands.push(str);
			}
			else if( curr_node == curr_node->parent->child[2] )
			{
				ss << curr_node->x << ' ' << curr_node->y << ' ' << "RIGHT";
				std::getline(ss, str);
				commands.push(str);
			}
			else if( curr_node == curr_node->parent->child[3] )
			{
				ss << curr_node->x << ' ' << curr_node->y << ' ' << "RIGHT";
				std::getline(ss, str);
				commands.push(str);
				commands.push(str);
			}

			curr_node = curr_node->parent;
		}
	}
}

int PTree::update_tree(unsigned xi, unsigned yi, posi_t position)
{
	if(!map)
		return -1;

	unsigned map_height = map->get_height();
	unsigned map_width = map->get_width();
	if( xi > map_width || yi > map_height || position == NO_ENTRY || position == STOP )
		return -1;

	unsigned next_x, next_y;
	posi_t next_posi;

	if(root)
	{
		if(!found_exit)
			return 1;
	}
	else
	{
		root = new PTreeNode(xi, yi, position);
		next_x = xi;
		next_y = yi;
		next_posi = map->path_find(next_x, next_y, position);
		if( map->next_room(next_x, next_y, next_posi) != 0 )
			return -1;

		if( build_tree(root, next_x, next_y, next_posi) == -1 )
			return -1;
		if( !found_exit )
			return 1;

		backtrack();
	}
	return 0;
}

int PTree::update_tree(unsigned xi, unsigned yi, std::string position)
{
	posi_t posit;
	switch( position[0] )
	{
		case 'T':
			posit = TOP;
			break;
		case 'L':
			posit = LEFT;
			break;
		case 'R':
			posit = RIGHT;
			break;
		default:
			posit = NO_ENTRY;
			break;
	}
	return update_tree(xi, yi, posit);
}