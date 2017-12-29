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

PTreeNode::PTreeNode(unsigned _x, unsigned _y, posi_t _direction, PTreeNode* _parent = nullptr) :
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
	root(nullptr), map(nullptr)
{}

PTree::PTree(Map* _map) :
	root(nullptr)
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