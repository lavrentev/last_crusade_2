#include "Tree.h"

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

std::string PTree::next_command()
{
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
		std::stack<std::string> reserve;
		PTreeNode* curr_node = exit_node;
		while( curr_node != root )
		{
			std::string str;
			std::stringstream ss( str );

			if( curr_node == curr_node->parent->child[0] )
			{
				if( reserve.empty() )
					commands.push_back(std::string("WAIT"));
				else
				{
					commands.push_back(reserve.top());
					reserve.pop();
				}
			}
			else if( curr_node == curr_node->parent->child[1] )
			{
				ss << curr_node->x << ' ' << curr_node->y << ' ' << "LEFT";
				std::getline(ss, str);
				commands.push_back(str);
			}
			else if( curr_node == curr_node->parent->child[2] )
			{
				ss << curr_node->x << ' ' << curr_node->y << ' ' << "RIGHT";
				std::getline(ss, str);
				commands.push_back(str);
			}
			else if( curr_node == curr_node->parent->child[3] )
			{
				ss << curr_node->x << ' ' << curr_node->y << ' ' << "RIGHT";
				std::getline(ss, str);
				commands.push_back(str);
				reserve.push(str);
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