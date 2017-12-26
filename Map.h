#pragma once

typedef short room_type;
typedef enum posi { TOP, LEFT, RIGHT, STOP, NO_ENTRY = -1 } posi_t;

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
