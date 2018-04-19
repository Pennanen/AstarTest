#include "Search.h"



SearchLevel::SearchLevel()
{
}


SearchLevel::~SearchLevel()
{
}

bool SearchLevel::init(const uint8_t* inputData, int width, int height)
{
	m_levelData = inputData;
	if (m_levelData == 0)
		return false;
	m_levelWidth = width;
	m_levelHeight = height;

	return true;
}

float SearchLevel::getDeltaG(Node* fromNode, Node* toNode) // SearchNode * fromPos, SearchNode * toPos ?
{
	return (float)manhattanDist(toNode->pos.first, fromNode->pos.first, toNode->pos.second, fromNode->pos.second);
}

float SearchLevel::getH(Node * fromPos, Node * toPos)
{

	return (float)manhattanDist(toPos->pos.first, fromPos->pos.first, toPos->pos.second, fromPos->pos.second);
}

bool SearchLevel::isWalkable(int x, int y)
{
	const uint8_t* pix = &m_levelData[3 * (y*m_levelWidth + x)]; // get pixel
	uint8_t r = pix[0];
	uint8_t g = pix[1];
	uint8_t b = pix[2];
	if (0 == r && g == 255 && b == 0) // green?
	{
		return false;
	}
	return true;
}

std::vector<Position> SearchLevel::getAdjacentNodes(int posX, int posY)
{
	std::vector<Position> temp;

	if (isWalkable(posX, posY + 1)) // Up
		temp.push_back(Position(posX, posY + 1));

	if (isWalkable(posX + 1, posY + 1)) // Up-Right
		temp.push_back(Position(posX + 1, posY + 1));

	if (isWalkable(posX + 1, posY)) // Right
		temp.push_back(Position(posX + 1, posY));

	if (isWalkable(posX + 1, posY - 1)) // Down-Right
		temp.push_back(Position(posX + 1, posY - 1));

	if (isWalkable(posX, posY - 1)) // Down
		temp.push_back(Position(posX, posY - 1));

	if (isWalkable(posX - 1, posY - 1)) // Down-Left
		temp.push_back(Position(posX - 1, posY - 1));

	if (isWalkable(posX - 1, posY)) // Left
		temp.push_back(Position(posX - 1, posY));

	if (isWalkable(posX - 1, posY + 1)) // Up-Left
		temp.push_back(Position(posX - 1, posY + 1));

	return temp;
}
