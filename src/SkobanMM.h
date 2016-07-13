#ifndef SKOBANMM_H
#define SKOBANMM_H

#include <allegro.h>
#include <fstream>
#include <winalleg.h>

using namespace std;

#define WINDOW_WIDTH    1024
#define WINDOW_HEIGHT   480

#define MAP_WIDTH        32
#define MAP_HEIGHT       24

#define TILE_WIDTH       20
#define TILE_HEIGHT      20

#define TILESET_WIDTH     2
#define TILESET_HEIGHT    1

#define NUM_OF_MATERIALS  2
#define NUM_OF_TILESETS  10

#define SKY               1
#define BRICK             2
#define FLOOR             3

#define CURSOR          100

void Setup();
void LoadMap(const char *path);
void SaveMap(const char *path);
void SetTile(int material, int posX, int posY);
void SetObject(int material, int posX, int posY);
bool isCursorOnMap();
void ClearMap();
void DrawMap();
void DrawCursorPosition();
void SetCursor();
void CheckMouseInput();
void CheckKeyboardInput();

#endif
