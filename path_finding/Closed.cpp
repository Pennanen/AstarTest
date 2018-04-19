#include "Closed.h"

ClosedList::ClosedList()
{
}

ClosedList::~ClosedList()
{
}

void ClosedList::addToClosedList(Node * n)
{
	m_closedList.insert(std::pair<Position, Node*>(n->pos, n));
}

bool ClosedList::isInClosedList(Position pos)
{
	if (m_closedList.find(pos) != m_closedList.end())
		return true;
	else
		return false;
}

void ClosedList::clear()
{
	m_closedList.clear();
}
