#include "Tree.h"

PTreeNode::PTreeNode()
{
	x = 0;
	y = 0;
	direction = NO_ENTRY;
	path_num = 0;
	parent = nullptr;
	child[0] = nullptr;
	child[1] = nullptr;
	child[2] = nullptr;
	child[3] = nullptr;
}

PTreeNode::PTreeNode(unsigned _x, unsigned _y, posi_t _direction, PTreeNode* _parent) :
	x(_x), y(_y), direction(_direction), path_num(0), parent(_parent)
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
	if( !child[count] )
		child[count] = new PTreeNode(_x, _y, _direction, this);
}



PTree::PTree() :
	root(nullptr), map(nullptr), active_path_num(0), current_node(nullptr)
	, found_exit(false), backtrack_needed(false), exit_node(nullptr)
{}

PTree::PTree(Map* _map) :
	root(nullptr), active_path_num(0), current_node(nullptr)
	, found_exit(false), backtrack_needed(false), exit_node(nullptr)
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

	rock_commands.clear();
	commands.clear();
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

PTreeNode* PTree::find_path_child(PTreeNode* node)
{
	if( !found_exit || active_path_num == 0 )
		return nullptr;

	for( int i = 0; i < 4; ++i )
	{
		if( node->child[i] )
		{
			if( node->child[i]->path_num == active_path_num )
				return node->child[i];
		}
	}

	return nullptr;
}

std::string PTree::next_command()
{
	current_node = find_path_child( current_node );
	map->move_rocks();

	if( commands.empty() )
		return std::string("WAIT");
	else
	{
		std::string current_command( commands.back() );
		std::stringstream temp( current_command );

		unsigned x, y;
		std::string rotation;
		temp >> x >> y >> rotation;
		if( rotation.compare("RIGHT") == 0 )
			map->rotate_right(x, y);
		if( rotation.compare("LEFT") == 0 )
			map->rotate_left(x, y);

		commands.pop_back();
		return current_command;
	}
}

int PTree::actions_left(PTreeNode* node)
{
	if( !node || !current_node )
		return -1;

	int count = 0;
	PTreeNode* temp = node;
	while( temp != current_node )
	{
		if( temp == temp->parent->child[0] )
		{
			if( rock_commands.find(temp) == rock_commands.end() )
				count++;
			else
				count--;
		}
		if( temp == temp->parent->child[3] )
			count--;
		temp = temp->parent;
	}
	return count;
}

int PTree::check_draw_path(PTreeNode* node)
{
	if( !node || !current_node )
		return -1;

	bool found_new_nodes = false;
	PTreeNode* temp = node;
	while( temp != current_node )
	{
		if( temp->path_num == 0 )
		{
			found_new_nodes = true;
			temp->path_num = active_path_num + 1;
		}
		temp = temp->parent;
	}

	if( found_new_nodes )
	{
		found_exit = true;
		active_path_num++;
		exit_node = node;
		temp = node;
		while( temp != current_node )
		{
			temp->path_num = active_path_num;
			temp = temp->parent;
		}

		return 0;
	}

	return 1;
}

void PTree::check_add_child(unsigned count, PTreeNode* node, unsigned xi, unsigned yi, posi_t position)
{
	if( node->child[count] )
	{
		if( node->child[count]->x != xi || node->child[count]->y != yi
				|| node->child[count]->direction != position )
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
			status = check_draw_path( node->child[0] );
			map->set_room_type(xi, yi, room[0]);
			return status;
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
				status = check_draw_path( node->child[1] );
				map->set_room_type(xi, yi, room[0]);
				return status;
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
				status = check_draw_path( node->child[2] );
				map->set_room_type(xi, yi, room[0]);
				return status;
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
					status = check_draw_path( node->child[3] );
					map->set_room_type(xi, yi, room[0]);
					return status;
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
	if( exit_node && current_node && backtrack_needed )
	{
		commands.clear();

		std::stack<std::string> reserve;
		RStack::iterator it;
		PTreeNode* node = exit_node;
		while( node != current_node )
		{
			std::string str;
			std::stringstream ss( str );

			if( node == node->parent->child[0] )
			{
				it = rock_commands.find(node);
				if( it != rock_commands.end() )
				{
					commands.push_back(it->second);
					rock_commands.erase(it);
				}
				else if( !reserve.empty() )
				{
					commands.push_back(reserve.top());
					reserve.pop();
				}
				else
					commands.push_back(std::string("WAIT"));
			}
			else if( node == node->parent->child[1] )
			{
				ss << node->x << ' ' << node->y << ' ' << "LEFT";
				std::getline(ss, str);
				commands.push_back(str);
			}
			else if( node == node->parent->child[2] )
			{
				ss << node->x << ' ' << node->y << ' ' << "RIGHT";
				std::getline(ss, str);
				commands.push_back(str);
			}
			else if( node == node->parent->child[3] )
			{
				ss << node->x << ' ' << node->y << ' ' << "RIGHT";
				std::getline(ss, str);
				commands.push_back(str);
				reserve.push(str);
			}

			node = node->parent;
		}
	}

	backtrack_needed = false;
}

/**
 * Checks if rock is going to cross Indy's path or not
 * 
 * @param rock in question
 * @return 0 if the rock is not dangerous, 1 if it is and
 * -1 for error
 */
int PTree::check_rock(Rock& rock)
{
	if( !found_exit || !exit_node || !current_node )
		return -1;

	int status;
	posi_t next_posi;
	unsigned next_x, next_y;
	PTreeNode* temp_node = current_node;

	next_x = rock.x;
	next_y = rock.y;
	next_posi = rock.position;

	while( temp_node != exit_node )
	{
		if( next_x == temp_node->x && next_y == temp_node->y )
		{
			room_type temp_room = map->get_room_type( next_x, next_y );
			if( temp_room == 4 )
			{
				if( temp_node->direction == TOP && next_posi == LEFT )
				{
					rock.status = DANGER;
					return 1;
				}
			}
			else if( temp_room == 5 )
			{
				if( temp_node->direction == TOP && next_posi == RIGHT )
				{
					rock.status = DANGER;
					return 1;
				}
			}
			else
			{
				rock.status = DANGER;
				return 1;
			}
		}

		next_posi = map->path_find(next_x, next_y, next_posi);
		status = map->next_room(next_x, next_y, next_posi);

		if( status == -1 )
			return -1;
		if( status == 1 )
			break;

		temp_node = find_path_child( temp_node );
	}

	rock.status = SAFE;
	return 0;
}

/**
 * Checks if rock is going to cross Indy's path or not
 * and if it does marks the tree node at which they meet
 * (overload of the previous function)
 *
 * @param rock in question and potentially dangerous node
 * @return 0 if the rock is not dangerous, 1 if it is and
 * -1 for error
 */
int PTree::check_rock(Rock& rock, PTreeNode*& danger_node)
{
	if( !found_exit || !exit_node || !current_node )
		return -1;

	int status;
	posi_t next_posi;
	unsigned next_x, next_y;
	PTreeNode* temp_node = current_node;

	next_x = rock.x;
	next_y = rock.y;
	next_posi = rock.position;

	while( temp_node != exit_node )
	{
		if( next_x == temp_node->x && next_y == temp_node->y )
		{
			room_type temp_room = map->get_room_type( next_x, next_y );
			if( temp_room == 4 )
			{
				if( temp_node->direction == TOP && next_posi == LEFT )
				{
					rock.status = DANGER;
					danger_node = temp_node;
					return 1;
				}
			}
			else if( temp_room == 5 )
			{
				if( temp_node->direction == TOP && next_posi == RIGHT )
				{
					rock.status = DANGER;
					danger_node = temp_node;
					return 1;
				}
			}
			else
			{
				rock.status = DANGER;
				danger_node = temp_node;
				return 1;
			}
		}

		next_posi = map->path_find(next_x, next_y, next_posi);
		status = map->next_room(next_x, next_y, next_posi);

		if( status == -1 )
			return -1;
		if( status == 1 )
			break;

		temp_node = find_path_child( temp_node );
	}

	rock.status = SAFE;
	danger_node = nullptr;
	return 0;
}

/**
 * Tries to stop a rock by rotating the rooms in front
 * so it either stops or goes out of bounds of the map
 *
 * @param rock in question and dangerous node
 * @return 0 if the rock was successfully stopped,
 * 1 if it wasn't and -1 for error
 */
int PTree::stop_rock(Rock& rock, PTreeNode* danger_node)
{
	if( !found_exit || !exit_node || !current_node || !danger_node
		|| danger_node->path_num != active_path_num )
		return -1;

	if( actions_left(danger_node) > 0 )
	{
		int status;
		unsigned next_x = rock.x;
		unsigned next_y = rock.y;
		posi_t next_posi = rock.position;
		room_type room[4];
		
		PTreeNode* temp_node = current_node;
		while( temp_node != danger_node )
		{
			next_posi = map->path_find(next_x, next_y, next_posi);
			status = map->next_room(next_x, next_y, next_posi);
			
			if( status == -1 )
				return -1;
			if( status == 1 )
				return 0;

			temp_node = find_path_child( temp_node );
			if( temp_node == temp_node->parent->child[0] )
			{
				std::string str;
				std::stringstream ss( str );

				room[0] = map->get_room_type(next_x, next_y);

				room[1] = map->rotate_left(next_x, next_y);
				if( room[1] != room[0] )
				{
					status = check_rock(rock);
					if( status == -1 )
						return -1;
					if( status == 0 )
					{
						ss << next_x << ' ' << next_y << " LEFT";
						rock_commands[temp_node] = str;
						map->set_room_type(next_x, next_y, room[0]);
						return 0;
					}
				}
				
				room[2] = map->rotate_180(next_x, next_y);
				if( room[2] != room[1] && room[2] != room[0] )
				{
					status = check_rock(rock);
					if( status == -1 )
						return -1;
					if( status == 0 )
					{
						ss << next_x << ' ' << next_y << " RIGHT";
						rock_commands[temp_node] = str;
						map->set_room_type(next_x, next_y, room[0]);
						return 0;
					}
				}

				room[3] = map->rotate_right(next_x, next_y);
				if( room[3] != room[2] && room[3] != room[1] && room[3] != room[0] )
				{
					if( actions_left(temp_node->parent) > 0 )
					{
						status = check_rock(rock);
						if( status == -1 )
							return -1;
						if( status == 0 )
						{
							ss << next_x << ' ' << next_y << " RIGHT";
							rock_commands[temp_node] = str;

							PTreeNode* counter_node = temp_node;
							while( counter_node != current_node )
							{
								if( counter_node == counter_node->parent->child[0]
									&& rock_commands.find(counter_node) == rock_commands.end() )
								{
									rock_commands[counter_node] = str;
									break;
								}
								counter_node = counter_node->parent;
							}

							map->set_room_type(next_x, next_y, room[0]);
							return 0;
						}
					}

					map->set_room_type(next_x, next_y, room[0]);
				}
			}
		}
	}

	return 1;
}

int PTree::update_tree(unsigned xi, unsigned yi, posi_t position)
{
	if( !map )
		return -1;

	unsigned map_height = map->get_height();
	unsigned map_width = map->get_width();
	if( xi > map_width || yi > map_height || position == NO_ENTRY || position == STOP )
		return -1;

	unsigned next_x, next_y;
	posi_t next_posi;

	if(root)
	{
		if( !found_exit )
			return 1;
		if( !current_node )
			return -1;
		if( current_node->x != xi || current_node->y != yi || current_node->direction != position )
			return -1;
	}
	else
	{
		root = new PTreeNode(xi, yi, position);
		current_node = root;
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