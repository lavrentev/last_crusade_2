#include "Map.h"

#define ABS(x) (((x) < 0) ? ((-1)*(x)) : (x))

Rock::Rock() :
	x(0), y(0), position(NO_ENTRY), status(UNKNOWN)
{}

Rock::Rock(unsigned _x, unsigned _y, posi_t _position, rock_t _status) :
	x(_x), y(_y), position(_position), status(_status)
{}

Rock::~Rock()
{}

bool Rock::equal(unsigned _x, unsigned _y, posi_t _position)
{
	if( x == _x && y == _y && position == _position )
		return true;
	else
		return false;
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
	rocks.clear();
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
 * Adds a rock to the map
 * 
 * @param coordinates of the rock x,y and direction of movement
 * @return -1 for error, 0 for success and
 * 1 if a rock with the same parameters already exists on the map
 */
int Map::add_rock(unsigned xr, unsigned yr, posi_t direction)
{
	if( xr >= W || yr >= H || direction == NO_ENTRY || direction == STOP )
		return -1;

	for( unsigned i = 0; i < rocks.size(); ++i )
	{
		if( rocks[i].equal(xr, yr, direction) )
			return 1;
	}
	rocks.push_back( Rock(xr, yr, direction) );

	return 0;
}

int Map::add_rock(unsigned xr, unsigned yr, std::string direction)
{
	posi_t posit;
	switch( direction[0] )
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
	return add_rock(xr, yr, posit);
}

/**
 * Returns the amount of rocks on the map
 */
unsigned Map::rocks_size()
{
	return rocks.size();
}

/**
 * Returns a reference to a rock indexed 'num'
 */
Rock& Map::get_rock(unsigned num)
{
	return rocks.at(num);
}

/**
 * Deletes a rock with an index 'num'
 */
void Map::delete_rock(unsigned num)
{
	if( num < rocks.size() )
		rocks.erase( rocks.begin() + num );
}

/**
 * Check if the map has rocks on it
 */
bool Map::has_rocks()
{
	if( rocks.empty() )
		return false;
	else
		return true;
}

/**
 * Changes the rocks according to their movement
 * like it's the next turn
 *
 * @return -1 for error, 0 for success
 */
int Map::move_rocks()
{
	posi_t next_posi;
	unsigned next_x, next_y;
	int status;

	for( int i = 0; i < rocks.size(); ++i )
	{
		next_x = rocks[i].x;
		next_y = rocks[i].y;
		next_posi = path_find( next_x, next_y, rocks[i].position );
		status = next_room( next_x, next_y, next_posi );
		if( status == -1 )
			return -1;
		if( status == 1 )
			rocks[i].status = DEAD;
		else
		{
			rocks[i].x = next_x;
			rocks[i].y = next_y;
			rocks[i].position = next_posi;
		}
	}

	return 0;
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