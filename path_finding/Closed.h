#pragma once
#include "Node.h"

#include <map>

class ClosedList
{
public:

	ClosedList();
	~ClosedList();

	void addToClosedList(Node* n);
	bool isInClosedList(Position pos);
	void clear();

	const std::map<Position, Node*>& getList() const { return m_closedList; };

private:
	std::map<Position, Node*> m_closedList;
};

