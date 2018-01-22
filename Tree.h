#pragma once
#include <string>
#include <sstream>
#include <stack>

#include "Map.h"

typedef std::vector<std::string> PStack;

struct PTreeNode
{
public:
	unsigned x, y;
	posi_t direction;
	unsigned path_num;
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
	unsigned active_path_num;
	bool found_exit;
	PTreeNode* exit_node;
	PTreeNode* current_node;

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
	// Also it updates the state of the tree and map to that of next turn of the game
	std::string next_command();

	int update_tree(unsigned xi, unsigned yi, std::string position);
	int update_tree(unsigned xi, unsigned yi, posi_t position);

private:
	int build_tree(PTreeNode* node, unsigned xi, unsigned yi, posi_t position);
	void backtrack();
	inline void check_add_child(unsigned count, PTreeNode* node, unsigned xi, unsigned yi, posi_t position);
	int actions_left(PTreeNode* node);
	int check_draw_path(PTreeNode* node);
	PTreeNode* find_path_child(PTreeNode* node);

	// Function checking if the rock is dangerous for Indy
	int check_rock(Rock& rock);
};

