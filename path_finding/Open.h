#pragma once
#include "Node.h"
#include <vector>

class OpenList
{
public:
	OpenList();
	~OpenList();

	Node* findFromOpenList(Position pos);
	void insertToOpenList(Node* n);
	void sortOpenList();
	bool isInOpenList(Position pos);
	Node* RemoveSmallestFFromOpenList();
	bool isEmpty();
	void clear();

	const std::vector<Node*>& getList() const { return m_openList; };

private:
	std::vector<Node*> m_openList;
};
