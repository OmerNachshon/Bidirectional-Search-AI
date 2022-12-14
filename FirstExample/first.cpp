#include "GLUT.h"

#include <time.h>
#include <vector>
#include "Cell.h"
#include <iostream>

using namespace std;


const int MSZ = 100; // maze size
const int WALL = 0;
const int SPACE = 1;
const int START = 2;
const int TARGET = 3;
const int GRAY = 4;
const int BLACK = 5;
const int PATH = 6;
const int BLACK_TARGET = 7;
const int GRAY_TARGET = 8;

int maze[MSZ][MSZ] = {0};
bool bfs_is_running = false;
bool dfs_is_running = false;

// defines queue of Cells
vector<Cell*> grays;
vector<Cell*> graysTarget;
void InitMaze();

void init()
{
	srand(time(0)); // seed random numbers
	glClearColor(0, 0, 0.2, 0); // RGB - sets background color
	glOrtho(0, MSZ, 0, MSZ, -1, 1); // setup coordinate system
	InitMaze();
}

void InitMaze()
{
	int i, j;

	// the border is WALL by default 

	for(i=1;i<MSZ-1;i++)
		for (j = 1; j < MSZ - 1; j++)
		{
			if (i % 2 == 1) // more SPACES
			{
				if (rand() % 100 < 85)
					maze[i][j] = SPACE;
				else
					maze[i][j] = WALL;
			}
			else // i is even: more walls
			{
				if (rand() % 100 < 70)
					maze[i][j] = WALL;
				else
					maze[i][j] = SPACE;
			}
		}
	maze[MSZ / 2][MSZ / 2] = START;
	// add Start Point to grays
	Cell* pc = new Cell(MSZ / 2, MSZ / 2);
	grays.push_back(pc);
	int rTarget = rand() % MSZ;
	int cTarget = rand() % MSZ;
	maze[rTarget][cTarget] = TARGET;
	Cell* pcTarget = new Cell(rTarget, cTarget);
	graysTarget.push_back(pcTarget);

}


void DrawMaze()
{
	int i, j;

	for(i=0;i<MSZ;i++)
		for (j = 0; j < MSZ; j++)
		{
			// set color
			switch (maze[i][j])
			{
			case WALL:
				glColor3d(0.3, 0, 0);// dark red
				break;
			case SPACE:
				glColor3d(1, 1, 1); // white
				break;
			case START:
				glColor3d(0.5, 0.7, 1);// light blue
				break;
			case TARGET:
				glColor3d(1, 0, 0); // red
				break;
			case GRAY:
				glColor3d(0, 0.5, 0); // dark green
				break;
			case BLACK:
				glColor3d(0.7, 1, 0.7); // light green
				break;
			case PATH:
				glColor3d(0.7, 0, 0.7); // magenta
				break;
			case BLACK_TARGET:
				glColor3d(0.2, 0, 0.9); // dark purple
				break;
			case GRAY_TARGET:
				glColor3d(0.7, 0, 0); // dark red
				break;
			}
			glBegin(GL_POLYGON);
			glVertex2d(j, i);
			glVertex2d(j, i+1);
			glVertex2d(j+1, i+1);
			glVertex2d(j+1, i);
			glEnd();
		}
}

void RestorePath(Cell* pc)
{
	while (pc != nullptr)
	{
		maze[pc->getRow()][pc->getCol()] = PATH;
		pc = pc->getParent();
	}
}

void CheckNeighbor(int row, int col, Cell* pcurrent)
{
	Cell* pneighbor;
	// is maze[row][col] a TARGET
	if (maze[row][col] == TARGET || maze[row][col]==BLACK_TARGET || maze[row][col] == GRAY_TARGET)
	{
		cout << "the solution has been found\n";
		bfs_is_running = false;
		dfs_is_running = false;
		RestorePath(pcurrent);
		//find neighbor cell
		for (int i = 0; i < graysTarget.size(); i++)
		{
			if (graysTarget.at(i)->getCol() == col && graysTarget.at(i)->getRow() == row)
			{
				RestorePath(graysTarget.at(i));
				return;
			}
		}
	}
	else // this is white neighbor
	{
		pneighbor = new Cell(row, col,pcurrent); // and pcurrent as parent
		grays.push_back(pneighbor);
		maze[row][col] = GRAY;
	}
}


void CheckNeighborTarget(int row, int col, Cell* pcurrent)
{
	Cell* pneighbor;
	// is maze[row][col] a START
	if (maze[row][col] == START || maze[row][col] == BLACK || maze[row][col] == GRAY)
	{
		cout << "the solution has been found\n";
		bfs_is_running = false;
		dfs_is_running = false;
		RestorePath(pcurrent);
		//find neighbor cell
		for (int i = 0; i < grays.size(); i++)
		{
			if (grays.at(i)->getCol() == col && grays.at(i)->getRow() == row)
			{
				RestorePath(grays.at(i));
				return;
			}
		}
	}
	else // this is white neighbor
	{
		pneighbor = new Cell(row, col, pcurrent); // and pcurrent as parent
		graysTarget.push_back(pneighbor);
		maze[row][col] = GRAY_TARGET;
	}
}


void BfsIteration()
{
	///////////////////////////////////////////////////////////////////////////// start step
	Cell* pcurrent;
	int r, c;
	
	if (grays.empty())
	{
		cout << "There is no solution\n";
		bfs_is_running = false;
		return;
	}
	else // grays is not empty
	{
		pcurrent = *grays.begin();
		// remove pcurrent from grays and paint it black
		grays.erase(grays.begin());
		r = pcurrent->getRow();
		c = pcurrent->getCol();
		maze[r][c] = BLACK;
		// add non-visited neighbors
		// up
		if (maze[r + 1][c] == SPACE || maze[r + 1][c] == TARGET || maze[r+1][c]==BLACK_TARGET || maze[r + 1][c] == GRAY_TARGET)
			CheckNeighbor(r + 1, c, pcurrent);
		// down
		if (bfs_is_running && (maze[r - 1][c] == SPACE || maze[r - 1][c] == TARGET || maze[r - 1][c]==BLACK_TARGET || maze[r - 1][c] == GRAY_TARGET))
			CheckNeighbor(r - 1, c, pcurrent);
			// left
		if (bfs_is_running && (maze[r ][c- 1] == SPACE || maze[r ][c- 1] == TARGET || maze[r][c - 1]==BLACK_TARGET || maze[r][c - 1] == GRAY_TARGET))
			CheckNeighbor(r, c - 1, pcurrent);
		// right
		if (bfs_is_running && (maze[r][c + 1] == SPACE || maze[r][c + 1] == TARGET || maze[r][c + 1]==BLACK_TARGET || maze[r][c + 1] == GRAY_TARGET))
			CheckNeighbor(r, c + 1, pcurrent);
	}

	/////////////////////////////////////////////////////////////////////////////// target step

	Cell* pcurrentTarget;
	int r2, c2;

	if (graysTarget.empty())
	{
		cout << "There is no solution\n";
		bfs_is_running = false;
		return;
	}
	else // graysTarget is not empty
	{
		pcurrentTarget = *graysTarget.begin();
		// remove pcurrentTarget from graysTarget and paint it black
		graysTarget.erase(graysTarget.begin());
		r2 = pcurrentTarget->getRow();
		c2 = pcurrentTarget->getCol();
		maze[r2][c2] = BLACK_TARGET;
		// add non-visited neighbors
		// up
		if (bfs_is_running && (maze[r2 + 1][c2] == SPACE || maze[r2 + 1][c2] == START || maze[r2 + 1][c2] == BLACK || maze[r2 + 1][c2] == GRAY))
			CheckNeighborTarget(r2 + 1, c2, pcurrentTarget);
		// down
		if (bfs_is_running && (maze[r2 - 1][c2] == SPACE || maze[r2 - 1][c2] == START || maze[r2 - 1][c2] == BLACK || maze[r2 - 1][c2] == GRAY))
			CheckNeighborTarget(r2 - 1, c2, pcurrentTarget);
		// left
		if (bfs_is_running && (maze[r2][c2 - 1] == SPACE || maze[r2][c2 - 1] == START || maze[r2][c2 - 1] == BLACK || maze[r2][c2 - 1] == GRAY))
			CheckNeighborTarget(r2, c2 - 1, pcurrentTarget);
		// right
		if (bfs_is_running && (maze[r2][c2 + 1] == SPACE || maze[r2][c2 + 1] == START || maze[r2][c2 + 1] == BLACK || maze[r2][c2 + 1] == GRAY))
			CheckNeighborTarget(r2, c2 + 1, pcurrentTarget);
	}

}

void DfsIteration()
{
	Cell* pcurrent;
	int row, col,i;

	if (grays.empty())
	{
		cout << "There is no solution\n";
		dfs_is_running = false;
		return;
	}
	else // grays is not empty
	{// pick the last element from grays
		pcurrent = grays.back();
		grays.pop_back();
		row = pcurrent->getRow();
		col = pcurrent->getCol();
		maze[row][col] = BLACK;

		// choose random direction (In DFS random direction is important)
		int direction[4] = { -1,-1,-1,-1 };
		int place;
		for (i = 0; i < 4; i++) // i stands for direction
		{
			do
			{
				place = rand() % 4;
			} while (direction[place]!=-1);
			direction[place] = i;
		}

		// pick neighbors in random order
		for (i = 0; i < 4 && dfs_is_running; i++)
		{
			switch (direction[i])
			{
			case 0:  // up
				if (maze[row + 1][col] == SPACE || maze[row + 1][col] == TARGET)
					CheckNeighbor(row + 1, col, pcurrent);
				break;
			case 1: //down
				if (maze[row - 1][col] == SPACE || maze[row - 1][col] == TARGET)
					CheckNeighbor(row - 1, col, pcurrent);
				break;
			case 2: // left 
				if (maze[row ][col - 1] == SPACE || maze[row][col - 1] == TARGET)
					CheckNeighbor(row, col - 1, pcurrent);
				break;
			case 3: // right
				if (maze[row][col + 1] == SPACE || maze[row][col + 1] == TARGET)
					CheckNeighbor(row, col + 1, pcurrent);
				break;
			}
		}

	}
}

void display()
{
	glClear(GL_COLOR_BUFFER_BIT); // fills all the frame buffer with background color

	DrawMaze();
	glutSwapBuffers();// show what was drawn in "frame buffer"
}

void idle()
{
	if (bfs_is_running)
		BfsIteration();
	if (dfs_is_running)
		DfsIteration();

	glutPostRedisplay(); // calls indirectly to display
}

void menu(int choice)
{
	switch (choice)
	{
	case 1: // bfs
		bfs_is_running = true;
		break;
	case 2: // dfs
		dfs_is_running = true;
		break;
	}
}

void main(int argc, char* argv[])
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE); // double means it uses double buffer
	glutInitWindowSize(600, 600);
	glutInitWindowPosition(600, 100);
	glutCreateWindow("BFS");

	glutDisplayFunc(display); // refresh function
	glutIdleFunc(idle); // runs all the time in the background
// add menu
	glutCreateMenu(menu);
	glutAddMenuEntry("Run BFS", 1);
	glutAddMenuEntry("Run DFS", 2);
	glutAttachMenu(GLUT_RIGHT_BUTTON);

	init();

	glutMainLoop();
}