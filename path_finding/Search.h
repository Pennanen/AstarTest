#pragma once
#include <vector>
#include "Closed.h"
#include "Node.h"
#include <stdint.h>


class SearchLevel
{
public:
	SearchLevel();
	~SearchLevel();

	bool init(const uint8_t* inputData, int width, int height);

	float getDeltaG(Node* fromNode, Node* toNode);
	float getH(Node* fromPos, Node* toPos);

	std::vector<Position> getAdjacentNodes(int posX, int posY);

	static double euclideanDist(double a, double b, double c, double d) { // pythagorian distance
		return sqrt(double((a - c) * (a - c) + (b - d) * (b - d)));
	}

	static double manhattanDist(double aX, double aY, double bX, double bY) { // block distance

		float x = (float)fabs(aX - bX);
		float y = (float)fabs(aY - bY);
		return x + y;
	}
private:
	bool isWalkable(int x, int y);


	const uint8_t* m_levelData;
	int m_levelWidth;
	int m_levelHeight;

	Node *m_start = nullptr;
	Node *m_end = nullptr;
};
