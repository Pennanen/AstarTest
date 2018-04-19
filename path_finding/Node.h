#pragma once
#include <utility> // std::pair

typedef std::pair<int, int> Position;

class Node
{
public:

	Position pos;		  // x and y position in the map
	float G;			  // Actual distance from the beginning point (A)
	float H;			  // Estimated heurestic distance to end point (B)
	Node* prevNode; // Tells us the node, where we arrived to this node (previous/parent Node)
	const float getF() const { return G + H; };	// G + H

public:
	Node(Position _position, float _H, float deltaG, Node* prev = nullptr);
	~Node();

	void resetPrev(Node* prev, float deltaG);
	Node* getPrev() { return this->prevNode; };

};

