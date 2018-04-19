#include "Node.h"
#include <assert.h>

Node::Node(Position _position, float _H, float deltaG, Node* prev /*= nullptr*/) :
prevNode(nullptr),
pos(_position),
G(0.0f),
H(_H)
{
	assert(G >= 0 && H >= 0);
	resetPrev(prev, deltaG);
}

void
Node::resetPrev(Node* prev, float deltaG)
{
	prevNode = prev;

	assert(this != prevNode);
	if (prevNode == nullptr)
		G = 0.0f;
	else {
		float sum = deltaG + prevNode->G;
		G = sum;
	}

}

Node::~Node()
{
}
