#include <stdint.h>
#include <stdio.h>
#include <cstdlib>
#include <assert.h>
#include <glut/glut.h>
#include <memory.h>
#include <iostream>
#include <vector>
#include <map>
#include <algorithm>

#include "Node.h"
#include "Closed.h"
#include "Open.h"
#include "Search.h"

typedef std::pair<int, int> Position;

//#define DRAW //Comment to disable search visualization

void drawLevel();

// Global Variables-----------------------------------------------------------------------

// OpenGL texture ids for rendering.
GLuint  inputTexture = 0;
GLuint  outputTexture = 0;

// Input and output data in pixels. outputData is updated to outputTexture each frame
uint8_t* inputData = 0;
uint8_t* outputData = 0;

// width and height of the input and output datas
int levelWidth = 0;
int levelHeight = 0;
float maxF = 0;
float minF = 99999999;

// start and end position for path finding. These are found automatically from input file.
int startX = -1;
int startY = -1;
int endX = -1;
int endY = -1;
SearchLevel searchLevel;
ClosedList closedList;
OpenList openList;

bool isPathFound = false;
bool isPathingStarted = false;
bool isDone = false;
bool isDrawnOnce = false;

// setPixel-----------------------------------------------------------------------
void setPixel(uint8_t* data, int x, int y, int w, int h, uint8_t red, uint8_t green, uint8_t blue)
{
	uint8_t* pixel = &data[3 * (y*w + x)];
	pixel[0] = blue;
	pixel[1] = green;
	pixel[2] = red;
}
// get distance-------------------------------------------------------------------
double distance(double a, double b, double c, double d) 
{ // pythagorian distance
	return sqrt(double((a - c) * (a - c) + (b - d) * (b - d)));
}
namespace
{
	void doPathFinding(const uint8_t* inputData, int width, int height, uint8_t* outputData, int startX, int startY, int endX, int endY)
	{
		//Init-----------------------------------------------------------------------
		if (!searchLevel.init(inputData, width, height))
		{
			printf("failed to init searchlevel\n");
		}

		Node* current = nullptr;

		//Search Loop-----------------------------------------------------------------------

#ifndef DRAW
		while (!isDone)
		{
#endif

			// If first time in loop
			if (!isPathingStarted)
			{
				// create start Node with f = 0 and insert it to openlist
				Node* start = new Node(Position(startX, startY), distance(startX, startY, endX, endY), 0.0f, nullptr);
				openList.insertToOpenList(start);
				isPathingStarted = true;
			}

			// A* pathfinding
			if (!openList.isEmpty() && !isPathFound)
			{
				memcpy(outputData, inputData, 3 * width*height);

				// sort open list
				openList.sortOpenList();

				current = openList.RemoveSmallestFFromOpenList();
				// add the node with smallest f to closed list. 
				closedList.addToClosedList(current);

				if (current->prevNode != nullptr) {

					assert(current->prevNode->pos.first >= 0 && current->prevNode->pos.first < 1000);
					assert(current->prevNode->pos.second >= 0 && current->prevNode->pos.second < 1000);
				}
				//color openlist
				for (auto itr : openList.getList())
				{
					setPixel(outputData, itr->pos.first, itr->pos.second, width, height, 255, 0, 255);
				}
				//color closedlist
				for (auto itr : closedList.getList())
				{
					setPixel(outputData, itr.first.first, itr.first.second, width, height,255, 0, 0);
				}

				// if the current node is the goal
				if (current->pos.first == endX && current->pos.second == endY)
				{
					// path found, set flag
					isPathFound = true;
				}

				//////////////////////////////////////////

				if (!isPathFound) // check the neighbors only if the path has not been found yet
				{

					// find current nodes walkable neighbors 
					auto neighbors = searchLevel.getAdjacentNodes(current->pos.first, current->pos.second);

					for (auto n_itr : neighbors)
					{
						// is neighbor in closed list
						if (closedList.isInClosedList(n_itr))
						{
							continue; // to next neighbor
						}

						// neighbor is not in open list
						if (!openList.isInOpenList(n_itr))
						{
							// push into openlist
							openList.insertToOpenList(new Node(
								n_itr,
								SearchLevel::euclideanDist(n_itr.first, n_itr.second, endX, endY),
								SearchLevel::euclideanDist(current->pos.first, current->pos.second, n_itr.first, n_itr.second),
								current
								));

						}
						// neighbor is in open list
						else
						{
							/*
							->check if current.G + neighbor.H < neighbor.F
							true: update neigbor.F and reparent it to current.
							false : ignore and go to next neighbor
							*/
							auto evaluateThis = openList.findFromOpenList(n_itr);
							if ((current->G + evaluateThis->H) < evaluateThis->getF())
							{
								evaluateThis->resetPrev(current, 1.0f);
							}
							else
							{
								continue; // go to next neighbor
							}
						}
					}
				}


				// A* star pathfinding - end loop

				if (openList.isEmpty() && isPathingStarted && !isDone)
				{
					printf("No path found \n");
				}

				// traverse the path and set path pixels. 
				if (isPathFound && !isDone)
				{
					// clear the left side

					do {
						// color the current node black
						setPixel(outputData, current->pos.first, current->pos.second, width, height, 0, 20, 0);
						// make current point to previous node
						assert(current != current->prevNode);
						current = current->prevNode;
					} while (current != nullptr); // do, until we point to the start node, which has no parent

					openList.clear();
					closedList.clear();
					isDone = true; // if done, just draw
				}

#ifndef DRAW
			}
#endif
		}
	}

		
	}


	namespace
	{
		// Quick and dirty function for reading bmp-files to opengl textures.
		GLuint loadBMPTexture(const char *fileName, int* w, int* h, uint8_t** data)
		{
			assert(w != 0);
			assert(h != 0);
			assert(data != 0);
			FILE *file;
			if ((file = fopen(fileName, "rb")) == NULL)
				return 0;
			fseek(file, 18, SEEK_SET);

			int width = 0;
			fread(&width, 2, 1, file);
			fseek(file, 2, SEEK_CUR);
			int height = 0;
			fread(&height, 2, 1, file);
			printf("Image \"%s\" (%dx%d)\n", fileName, width, height);

			*data = new uint8_t[3 * width * height];
			assert(data != 0);
			fseek(file, 30, SEEK_CUR);
			fread(*data, 3, width * height, file);
			fclose(file);

			GLuint  texId;
			glGenTextures(1, &texId);
			glBindTexture(GL_TEXTURE_2D, texId);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			gluBuild2DMipmaps(GL_TEXTURE_2D, 3, width, height, GL_BGR_EXT, GL_UNSIGNED_BYTE, *data);
			glBindTexture(GL_TEXTURE_2D, 0);
			if (w) *w = width;
			if (h) *h = height;
			return texId;
		}

		// Initialization
		bool init()
		{

			// Load input file
			inputTexture = loadBMPTexture("input.bmp", &levelWidth, &levelHeight, &inputData);
			if (0 == inputTexture)
			{
				printf("Error! Cannot open file: \"input.bmp\"\n");
				return false;
			}

			glMatrixMode(GL_PROJECTION);

			glOrtho(0, levelWidth * 4 + 4, levelHeight * 2 + 2, 0, -1, 1); //for original input
			//glOrtho(0, 128 + 4, 64 + 2, 0, -1, 1);

			// Make outputTexture
			glGenTextures(1, &outputTexture);
			glBindTexture(GL_TEXTURE_2D, outputTexture);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glBindTexture(GL_TEXTURE_2D, 0);

			if (!searchLevel.init(inputData, levelWidth, levelHeight))
			{
				printf("failed to init searchlevel\n");
			}

			// Copy inputData also to outputData
			outputData = new uint8_t[3 * levelWidth*levelHeight];
			memcpy(outputData, inputData, 3 * levelWidth*levelHeight);

			// find start and end
			for (int y = 0; y < levelHeight; ++y)
			{
				for (int x = 0; x < levelWidth; ++x)
				{
					uint8_t* pix = &inputData[3 * (y*levelWidth + x)]; // get pixel
					uint8_t r = pix[0];
					uint8_t g = pix[1];
					uint8_t b = pix[2];
					if (255 == r && g == 0 && b == 0) // Red?
					{
						// Start
						startX = x;
						startY = y;
						printf("Start position: <%d,%d>\n", x, y);
					}
					if (255 == b && r == 0 && g == 0) // Blue?
					{
						// End
						endX = x;
						endY = y;
						printf("End position: <%d,%d>\n", x, y);
					}
				}
			}

			if (startX < 0 || startY < 0)
			{
				printf("Error! Start position not found\n");
				return false;
			}

			if (endX < 0 || endY < 0)
			{
				printf("Error! End position not found\n");
				return false;
			}

			return true;
		}

		// Draw/Render
		void draw()
		{
			doPathFinding(inputData, levelWidth, levelHeight, outputData, startX, startY, endX, endY);
			drawLevel();

		}
	} // end - anonymous namespace


	// Main
	int main(int argc, char ** argv) {
		glutInit(&argc, argv);
		glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
		glutInitWindowSize(2 * (512 + 4), 2 * (256 + 2));
		glutCreateWindow("Pathfinding Demo");
		glutDisplayFunc(draw);
		if (!init()) return -1;
		glutMainLoop();
		delete inputData;
		delete outputData;
		return 0;
	}

	void drawLevel()
	{
		// Copy outputData to outputTexture
		glBindTexture(GL_TEXTURE_2D, outputTexture);
		gluBuild2DMipmaps(GL_TEXTURE_2D, 3, levelWidth, levelHeight, GL_BGR_EXT, GL_UNSIGNED_BYTE, outputData);
		glBindTexture(GL_TEXTURE_2D, 0);

		glClear(GL_COLOR_BUFFER_BIT);

		// Draw input texture to left half of the screen
		glPushMatrix();
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, inputTexture);
		glBegin(GL_QUADS);

		glTexCoord2d(0, 1); glVertex2d(1, 1);
		glTexCoord2d(0, 0); glVertex2d(1, 1 + (levelHeight * 2));
		glTexCoord2d(1, 0); glVertex2d(1 + (levelWidth * 2), 1 + (levelHeight * 2));
		glTexCoord2d(1, 1); glVertex2d(1 + (levelWidth * 2), 1);
		/*
		glTexCoord2d(0, 1); glVertex2d(1, 1);
		glTexCoord2d(0, 0); glVertex2d(1, 1);
		glTexCoord2d(1, 0); glVertex2d(1 + 64, 1 + 64);
		glTexCoord2d(1, 1); glVertex2d(1 + 64, 1);
		*/
		glEnd();
		glDisable(GL_TEXTURE_2D);
		glPopMatrix();

		// Draw output texture to right half of the screen
		glPushMatrix();
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, outputTexture);
		glBegin(GL_QUADS);

		glTexCoord2d(0, 1); glVertex2d(2 + (levelWidth * 2), 1);
		glTexCoord2d(0, 0); glVertex2d(2 + (levelWidth * 2), 1 + (levelHeight * 2));
		glTexCoord2d(1, 0); glVertex2d(2 + (levelWidth * 4), 1 + (levelHeight * 2));
		glTexCoord2d(1, 1); glVertex2d(2 + (levelWidth * 4), 1);
		/*
		glTexCoord2d(0, 1); glVertex2d(2 + 64, 1);
		glTexCoord2d(0, 0); glVertex2d(2 + 64, 1 + 64);
		glTexCoord2d(1, 0); glVertex2d(2 + 128, 1 + 64);
		glTexCoord2d(1, 1); glVertex2d(2 + 128, 1);
		*/
		glEnd();
		glDisable(GL_TEXTURE_2D);
		glPopMatrix();

		glutSwapBuffers();
		glutPostRedisplay();
	}