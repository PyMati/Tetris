#include "def.h"
#include "primlib.h"
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#define GAME_FIELD_X_MATRIX 10
#define GAME_FIELD_Y_MATRIX 20
#define GAME_FIELD_HALF_XSIZE 175
#define GAME_FIELD_YSIZE 625
#define PREVIEW_BOX_YPOINT (gfx_screenHeight() / 2)
#define SPEED 13
#define BLOCK_POINTS 10
#define ROTATIONSPEED 45

enum moveKeys{DOWN = 3, RIGHT = 2, LEFT = 1};

typedef struct {
	int coord[4][3];
	int index;
	int rotation;
} gameBlock;

void gfxInit()
{
	if (gfx_init())
		exit(3);
}

void drawBackground()
{
	gfx_filledRect(0, 0, gfx_screenWidth() - 1, gfx_screenHeight() - 1, BLACK);
}

void drawPreviewBlock(int nextBlockIndex, int margin, float x_start,
					  float y_start, float x_end, float y_end)
{
	x_start += margin;
	y_start += margin;
	x_end -= margin;
	y_end -= margin;
	float x_size = (x_end - x_start) / 4;
	float y_size = (y_end - y_start) / 4;
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			if (pieces[nextBlockIndex][0][i][j] == 1) {
				gfx_filledRect(x_start + (j * x_size), y_start + (i * y_size),
							   x_start + ((j + 1) * x_size),
							   y_start + ((i + 1) * y_size), RED);
			}
			else if (pieces[nextBlockIndex][0][i][j] == 2)
				gfx_filledRect(x_start + (j * x_size), y_start + (i * y_size),
							   x_start + ((j + 1) * x_size),
							   y_start + ((i + 1) * y_size), GREEN);
		}
	}
}

void drawPreviewBox(int nextBlockIndex)
{
	float margin = 10;
	float size = 150;
	float x_start = gfx_screenWidth() / 2 + GAME_FIELD_HALF_XSIZE + margin;
	float x_end = x_start + size;
	float y_start = PREVIEW_BOX_YPOINT - size / 2;
	float y_end = y_start + size;
	gfx_filledRect(x_start, y_start, x_end, y_end, BLUE);
	gfx_textout(x_start + ((x_end - x_start) / 2) - 39, y_start + 5,
				"NEXT BLOCK", YELLOW);
	drawPreviewBlock(nextBlockIndex, margin + 10, x_start, y_start, x_end,
					 y_end);
}

void drawGameBoard(float x_start, float y_start, float x_end, float y_end)
{
	gfx_line(x_start, y_start, x_start, y_end, CYAN);
	gfx_line(x_end, y_start, x_end, y_end, CYAN);
}

void drawGameBlockMatrix(float blockWH[], int gameMatrix[][GAME_FIELD_Y_MATRIX],
						 float x_start, float y_start)
{
	for (int i = 0; i < GAME_FIELD_X_MATRIX; i++) {
		for (int j = 0; j < GAME_FIELD_Y_MATRIX; j++) {
			if (gameMatrix[i][j] == 2) {
				gfx_filledRect(x_start + (i * blockWH[0]),
							   y_start + (j * blockWH[1]),
							   x_start + ((i + 1) * blockWH[0]),
							   y_start + ((j + 1) * blockWH[1]), GREEN);
			}
			if (gameMatrix[i][j] == 1) {
				gfx_filledRect(x_start + (i * blockWH[0]),
							   y_start + (j * blockWH[1]),
							   x_start + ((i + 1) * blockWH[0]),
							   y_start + ((j + 1) * blockWH[1]), RED);
			}
		}
	}
}

void drawGameBlock(gameBlock* block, float xstart, float ystart, float xend,
				   float yend, float xsize, float ysize)
{
	for (int i = 0; i < 4; i++) {
		int column = block->coord[i][0];
		int row = block->coord[i][1];
		int val = block->coord[i][2];
		if (val == 1) {
			gfx_filledRect(xstart + (column * xsize), yend + (row * ysize),
						   xstart + ((column + 1) * xsize),
						   yend + ((row + 1) * ysize), RED);
		}
		else {
			gfx_filledRect(xstart + (column * xsize), yend + (row * ysize),
						   xstart + ((column + 1) * xsize),
						   yend + ((row + 1) * ysize), GREEN);
		}
	}
}

void endScreen(int gameMatrix[][GAME_FIELD_Y_MATRIX])
{
	int score = 0;
	for (int i = 0; i < GAME_FIELD_X_MATRIX; i++)
		for (int j = 0; j < GAME_FIELD_Y_MATRIX; j++) {
			if (gameMatrix[i][j] != 0)
				score++;
		}
	drawBackground();
	gfx_textout(gfx_screenWidth() / 2, gfx_screenHeight() / 2, "END", YELLOW);
}

void drawEnv(int nextBlockIndex, float blockWidthHeigth[],
			 int gameMatrix[][GAME_FIELD_Y_MATRIX], gameBlock* block)
{
	float xstart = gfx_screenWidth() / 2 - GAME_FIELD_HALF_XSIZE;
	float ystart = gfx_screenHeight();
	float yend = ystart - GAME_FIELD_YSIZE;
	float xend = gfx_screenWidth() / 2 + GAME_FIELD_HALF_XSIZE;
	float xsize = (xend - xstart) / GAME_FIELD_X_MATRIX;
	float ysize = (ystart - yend) / GAME_FIELD_Y_MATRIX;
	blockWidthHeigth[0] = xsize;
	blockWidthHeigth[1] = ysize;
	drawBackground();
	drawGameBlock(block, xstart, ystart, xend, yend, xsize, ysize);
	drawGameBlockMatrix(blockWidthHeigth, gameMatrix, xstart, yend);
	drawGameBoard(xstart, ystart, xend, yend);
	drawPreviewBox(nextBlockIndex);
}

void handleClick(int* display, int* newRotation, int* moveType)
{
	if (gfx_isKeyDown(SDLK_ESCAPE))
		*display = 0;
	if (gfx_isKeyDown(SDLK_DOWN)) {
		*moveType = DOWN;
	}
	if (gfx_isKeyDown(SDLK_LEFT)) {
		*moveType = LEFT;
	}
	if (gfx_isKeyDown(SDLK_RIGHT)) {
		*moveType = RIGHT;
	}
	if (gfx_isKeyDown(SDLK_SPACE))
		*newRotation = 1;
}

int randomIndex()
{
	int index = (rand() % 7);
	return index;
}

void initGameBlock(int* index, int* newIndex, gameBlock* block,
				   int rotationIndex)
{
	int xyindex = 0;
	int centerX = GAME_FIELD_X_MATRIX / 2;
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			if (pieces[*index][rotationIndex][j][i] > 0) {
				block->coord[xyindex][0] = i + centerX;
				block->coord[xyindex][1] = j;
				block->coord[xyindex][2] = pieces[*index][rotationIndex][j][i];
				block->index = *index;
				block->rotation = rotationIndex;
				xyindex++;
			}
		}
	}

	*index = *newIndex;
	*newIndex = randomIndex();
}

void shouldBeNewBlock(int gameMatrix[][GAME_FIELD_Y_MATRIX], gameBlock* block,
					  int* new)
{

	for (int i = 0; i < 4; i++) {
		if (block->coord[i][1] == GAME_FIELD_Y_MATRIX - 1)
			*new = 1;
		else if (gameMatrix[block->coord[i][0]][block->coord[i][1] + 1] != 0)
			*new = 1;
	}
}
void isLegalMove(int gameMatrix[][GAME_FIELD_Y_MATRIX], gameBlock* block,
				 int* new, int* should_blockl, int* should_blockr, int moveType)
{
	for (int i = 0; i < 4; i++) {
		if (block->coord[i][0] + 1 > GAME_FIELD_X_MATRIX - 1)
			*should_blockr = 1;
		else if (block->coord[i][0] == 0)
			*should_blockl = 1;
		shouldBeNewBlock(gameMatrix, block, new);
		if (gameMatrix[block->coord[i][0] - 1][block->coord[i][1]] != 0 &&
			moveType == 2) {
			if (!*should_blockl)
				*should_blockl = 1;
		}
		if (gameMatrix[block->coord[i][0] + 1][block->coord[i][1]] != 0 &&
			moveType == 1) {
			if (!*should_blockr)
				*should_blockr = 1;
		}
		if (block->coord[i][1] != GAME_FIELD_Y_MATRIX - 1) {
			if (gameMatrix[block->coord[i][0] + 1][block->coord[i][1] + 1] !=
					0 &&
				moveType == 1) {
				if (!*should_blockr)
					*should_blockr = 1;
			}
		}
		if (block->coord[i][1] != GAME_FIELD_Y_MATRIX - 1) {
			if (gameMatrix[block->coord[i][0] - 1][block->coord[i][1] + 1] !=
					0 &&
				moveType == 2) {
				if (!*should_blockl) {
					*should_blockl = 1;
				}
			}
		}
	}
}

void createNewBlock(int new, gameBlock* block, int* rotationIndex,
					int* newIndex, int gameMatrix[][GAME_FIELD_Y_MATRIX],
					int* index)
{
	if (new) {
		for (int i = 0; i < 4; i++)
			gameMatrix[block->coord[i][0]][block->coord[i][1]] = 1;
		initGameBlock(index, newIndex, block, 0);
		*rotationIndex = 0;
	}
}

void moveSides(int moveType, gameBlock* block, int should_blockl,
			   int should_blockr, int new)
{
	if (!new) {
		for (int i = 0; i < 4; i++) {
			if (moveType == 0) {
				block->coord[i][1] = block->coord[i][1] + 1;
			}
			else if (moveType == RIGHT) {
				block->coord[i][1] = block->coord[i][1] + 1;
				if (!should_blockr) {
					block->coord[i][0] = block->coord[i][0] + 1;
				}
			}
			else if (moveType == LEFT) {
				block->coord[i][1] = block->coord[i][1] + 1;
				if (!should_blockl) {
					block->coord[i][0] = block->coord[i][0] - 1;
				}
			}
		}
	}
}

void moveBlock(gameBlock* block, int moveType,
			   int gameMatrix[][GAME_FIELD_Y_MATRIX], int* index, int* newIndex,
			   int* rotationIndex)
{
	int should_blockl = 0;
	int should_blockr = 0;
	int new = 0;
	isLegalMove(gameMatrix, block, &new, &should_blockl, &should_blockr,
				moveType);
	if (moveType == DOWN) {
		while (1) {
			isLegalMove(gameMatrix, block, &new, &should_blockl, &should_blockr,
						moveType);
			if (new)
				break;
			for (int i = 0; i < 4; i++) {
				block->coord[i][1] = block->coord[i][1] + 1;
			}
		}
	}
	moveSides(moveType, block, should_blockl, should_blockr, new);
	createNewBlock(new, block, rotationIndex, newIndex, gameMatrix, index);
}

void findPlaceOfGreenBlock(gameBlock* block, int greenPlace[])
{
	for (int i = 0; i < 4; i++) {
		if (block->coord[i][2] == 2) {
			greenPlace[0] = block->coord[i][0];
			greenPlace[1] = block->coord[i][1];
			break;
		}
	}
}

int isRotationLegal(gameBlock* block, int gameMatrix[][GAME_FIELD_Y_MATRIX],
					int col, int row, int* newRotation)
{
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			if (pieces[block->index][*newRotation][i][j]) {
				if (i + col < 0)
					return 0;
				if (j + row > GAME_FIELD_Y_MATRIX - 1)
					return 0;
				if (i + col > GAME_FIELD_X_MATRIX - 1)
					return 0;
				if (i + row < 0)
					return 0;
				if (gameMatrix[i + col][j + row] != 0)
					return 0;
			}
		}
	}
	return 1;
}

void trueRotation(int greenPlace[], gameBlock* block, int* newRotation, int col,
				  int row)
{
	int index = 0;
	for (int i = 0; i < 4; i++)
		for (int j = 0; j < 4; j++) {
			if (pieces[block->index][*newRotation][i][j] != 0) {
				if (pieces[block->index][*newRotation][i][j] == 2) {
					col = greenPlace[0] - i;
					row = greenPlace[1] - j;
				}
				block->coord[index][0] = i;
				block->coord[index][1] = j;
				block->coord[index][2] =
					pieces[block->index][*newRotation][i][j];
				index++;
			}
		}

	for (int i = 0; i < 4; i++) {
		block->coord[i][0] = block->coord[i][0] + col;
		block->coord[i][1] = block->coord[i][1] + row;
	}
	block->rotation = *newRotation;
}

void rotateBlock(gameBlock* block, int* newRotation,
				 int gameMatrix[][GAME_FIELD_Y_MATRIX])
{
	int greenPlace[2];
	findPlaceOfGreenBlock(block, greenPlace);
	int col = 0;
	int row = 0;
	for (int i = 0; i < 4; i++)
		for (int j = 0; j < 4; j++) {
			if (pieces[block->index][*newRotation][i][j] != 0) {
				if (pieces[block->index][*newRotation][i][j] == 2) {
					col = greenPlace[0] - i;
					row = greenPlace[1] - j;
				}
			}
		}

	if (isRotationLegal(block, gameMatrix, col, row, newRotation)) {
		trueRotation(greenPlace, block, newRotation, col, row);
		*newRotation = *newRotation + 1;
	}
}

void resetRotationIndex(int* rotationi)
{
	if (*rotationi >= 4) {
		*rotationi = 0;
		return;
	}
}

void checkEndGame(int gameMatrix[][GAME_FIELD_Y_MATRIX], int* bool)
{
	for (int i = 0; i < GAME_FIELD_X_MATRIX; i++) {
		if (gameMatrix[i][0] != 0)
			*bool = 1;
	}
}

void enableMove(int* waiter, int* shouldMove)
{
	*waiter = 0;
	*shouldMove = 1;
}

void enableRotation(int* rotationCounter, int* canRotate)
{
	*rotationCounter = 0;
	*canRotate = 1;
}

void handleGameValues(int* waiter, int* rotationCounter, int* shouldMove,
					  int* moveType, int* canRotate)
{
	*waiter = *waiter + 1;
	*rotationCounter = *rotationCounter + 1;
	*shouldMove = 0;
	*moveType = 0;
	*canRotate = 0;
}

void deleteFullRow(int gameMatrix[][GAME_FIELD_Y_MATRIX])
{
	for (int y = 0; y < GAME_FIELD_Y_MATRIX; y++) {
		int counter = 0;
		for (int x = 0; x < GAME_FIELD_X_MATRIX; x++) {
			if (gameMatrix[x][y] != 0) {
				counter++;
				if (counter == GAME_FIELD_X_MATRIX) {
					for (int h = y; h > 0; h--) {
						for (int z = 0; z < GAME_FIELD_X_MATRIX; z++) {
							gameMatrix[z][h] = 0;
							gameMatrix[z][h] = gameMatrix[z][h - 1];
						}
					}
				}
			}
		}
	}
}

int main()
{
	gfxInit();
	srand(time(0));
	float blockWidthHeight[2];
	int gameMatrix[GAME_FIELD_X_MATRIX][GAME_FIELD_Y_MATRIX] = {0};
	int display = 1, index = randomIndex(), nextBlockIndex = randomIndex(),
		newRotation = 0, nxRotInd = 1, shouldMove = 0, waiter = 0, moveType = 0,
		endgame = 0, rotationCounter = 0, canRotate = 0;
	gameBlock block;
	initGameBlock(&index, &nextBlockIndex, &block, 0);
	while (display) {
		deleteFullRow(gameMatrix);
		handleClick(&display, &newRotation, &moveType);
		checkEndGame(gameMatrix, &endgame);
		if (waiter > SPEED)
			enableMove(&waiter, &shouldMove);
		if (rotationCounter > ROTATIONSPEED)
			enableRotation(&rotationCounter, &canRotate);
		if (!endgame)
			drawEnv(index, blockWidthHeight, gameMatrix, &block);
		if (endgame)
			endScreen(gameMatrix);
		if (newRotation && canRotate) {
			newRotation = 0;
			resetRotationIndex(&nxRotInd);
			rotateBlock(&block, &nxRotInd, gameMatrix);
		}
		if (shouldMove)
			moveBlock(&block, moveType, gameMatrix, &index, &nextBlockIndex,
					  &nxRotInd);
		handleGameValues(&waiter, &rotationCounter, &shouldMove, &moveType,
						 &canRotate);
		gfx_updateScreen();
		SDL_Delay(20);
	}
}
