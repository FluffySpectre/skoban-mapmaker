#include "SkobanMM.h"

BITMAP *buffer;
BITMAP *materials[NUM_OF_MATERIALS];
BITMAP *tilesets[NUM_OF_TILESETS][TILESET_HEIGHT][TILESET_WIDTH];
BITMAP *usedTile[NUM_OF_TILESETS][TILESET_HEIGHT][TILESET_WIDTH];

int map[MAP_HEIGHT][MAP_WIDTH];
int objMap[MAP_HEIGHT][MAP_WIDTH];

int tempCursorX;
int tempCursorY;

int actTileset;

//bool needsRefresh = true;

bool isBitmap(const char *file) {
    std::string tempFilename(file);
    std::string foundExt;
    
    if(tempFilename.find_last_of(".") != std::string::npos) {
        foundExt = tempFilename.substr(tempFilename.find_last_of(".") + 1);
        
        if(foundExt == "bmp") {
            return true;
        }
    }
    
    return false;
}

BITMAP *GetFrame(BITMAP *source, int width, int height, int startX, int startY, int i, int j) {
    BITMAP *tempBitmap = create_bitmap(width, height);
    
    int x = (startX + i) * width;
    int y = (startY + j) * height; 
    
    blit(source, tempBitmap, x, y, 0, 0, width, height);
    
    return tempBitmap;     
}

void LoadTilesets() {
    HANDLE fHandle;
    WIN32_FIND_DATA wfd;
    BITMAP *tempBitmap;    
    int actSet = 0;
    std::string path;
    
    // Erste Datei des Verzeichnisses finden
    fHandle = FindFirstFile("tilesets\\*", &wfd);
    
    // Zweites Ergebnis ignorieren
    FindNextFile(fHandle, &wfd);
    
    // Alle Dateien im Ordner durchlaufen
    while(FindNextFile(fHandle, &wfd)) {
        if(isBitmap(wfd.cFileName)) {
            std::string filename(wfd.cFileName);
            
            path = "tilesets\\" + filename;
            
            tempBitmap = load_bitmap(path.c_str(), NULL);
            
            if(tempBitmap != NULL) {  
                for(int j = 0; j < TILESET_HEIGHT; j++) {
                    for(int i = 0; i < TILESET_WIDTH; i++) {
                        tilesets[actSet][j][i] = GetFrame(tempBitmap, TILE_WIDTH, TILE_HEIGHT, 0, 0, i, j);
                    }
                }
        
                actSet++;
            }   
        }     
    }
    
    FindClose(fHandle);
}
  
void Setup() {
    allegro_init();
	install_keyboard();
	install_mouse();
	
	set_color_depth(24);
	set_gfx_mode(GFX_AUTODETECT_WINDOWED, WINDOW_WIDTH, WINDOW_HEIGHT, 0, 0);
	
	// Bitmap für Double-Buffering erstellen
	buffer = create_bitmap(WINDOW_WIDTH, WINDOW_HEIGHT); 
	
	LoadTilesets();
    actTileset = 0;
    
	// Grafiken laden
	materials[0] = load_bitmap("materials/brick.bmp", NULL);
	materials[1] = load_bitmap("materials/floor.bmp", NULL);
	
	// Karte leeren
	ClearMap();
}

void LoadMap(const char *path) {
    ifstream filein(path);
    char temp;
    
    if(filein) {
        for(int y = 0; y < MAP_HEIGHT; y++) {
            for(int x = 0; x < MAP_WIDTH; x++) {
                filein >> temp;
                
                map[y][x] = atoi(&temp);               
            }       
        }     
    }
}

void SaveMap(const char *path) {
    ofstream fileout(path);
    
    for(int y = 0; y < MAP_HEIGHT; y++) {
        for(int x = 0; x < MAP_WIDTH; x++) {
            fileout << map[y][x];               
        }
        
        fileout << "\n";        
    } 
    
    fileout.close();   
}

void SetTile(int material, int posX, int posY) {
     map[posY][posX] = material;
}

void SetObject(int material, int posX, int posY) {
     objMap[posY][posX] = material;
}

bool isCursorOnMap() {
    if((mouse_x / TILE_WIDTH) < MAP_WIDTH && (mouse_y / TILE_HEIGHT) < MAP_HEIGHT) {
        return true;
    }
    
    return false;     
}

void ClearMap() {
    for(int y = 0; y < MAP_HEIGHT; y++) {
        for(int x = 0; x < MAP_WIDTH; x++) {
            map[y][x] = SKY;           
        }        
    }     
}

void DrawMap() {
    clear_bitmap(buffer);
    
    for(int y = 0; y < MAP_HEIGHT; y++) {
        for(int x = 0; x < MAP_WIDTH; x++) {
            if(map[y][x] == SKY) rectfill(buffer, x * TILE_WIDTH, y * TILE_HEIGHT, (x + 1) * TILE_WIDTH, 
                                         (y + 1) * TILE_HEIGHT, makecol(0, 128, 250));
                                         
            if(map[y][x] == BRICK) draw_sprite(buffer, materials[0], x * TILE_WIDTH, y * TILE_HEIGHT);
            if(map[y][x] == FLOOR) draw_sprite(buffer, materials[1], x * TILE_WIDTH, y * TILE_HEIGHT);            
        }        
    }
}

void DrawCursorPosition() {
    textprintf_ex(buffer, font, (MAP_WIDTH * TILE_WIDTH) + 20, 60, makecol(255, 255, 255),
                  -1, "X: %d", mouse_x / TILE_WIDTH);
    textprintf_ex(buffer, font, (MAP_WIDTH * TILE_WIDTH) + 20, 75, makecol(255, 255, 255),
                  -1, "Y: %d", mouse_y / TILE_HEIGHT);
}

void DrawTileset() {
    for(int j = 0; j < TILESET_HEIGHT; j++) {
        for(int i = 0; i < TILESET_WIDTH; i++) {
            draw_sprite(buffer, tilesets[actTileset][j][i], 650 + i * TILE_WIDTH, 10 + j * TILE_HEIGHT);
        }
    }
}

void SetCursor() {
    // Cursor an letzter Position löschen
    SetObject(0, tempCursorX, tempCursorY);
    
    // Prüfen. ob sich der Cursor innerhalb der Karte befindet
    if(isCursorOnMap()) {
        // Position des Cursors zwischenspeichern
        tempCursorX = mouse_x / TILE_WIDTH;
        tempCursorY = mouse_y / TILE_HEIGHT;
    }
    
    DrawCursorPosition();
    SetObject(CURSOR, tempCursorX, tempCursorY);
    
    // Cursor zeichnen
    for(int y = 0; y < MAP_HEIGHT; y++) {
        for(int x = 0; x < MAP_WIDTH; x++) {
            if(objMap[y][x] == CURSOR) rect(buffer, x * TILE_WIDTH, y * TILE_HEIGHT, 
                                           (x + 1) * TILE_WIDTH, (y + 1) * TILE_HEIGHT, makecol(255, 255, 255)); 
        }
    }        
}

void CheckMouseInput() {
    if(isCursorOnMap()) {
        if(mouse_b & 1) {
            SetTile(FLOOR, mouse_x / TILE_WIDTH, mouse_y / TILE_HEIGHT);    
        }else if(mouse_b & 2) {
            SetTile(BRICK, mouse_x / TILE_WIDTH, mouse_y / TILE_HEIGHT);
        }
    }               
}

void CheckKeyboardInput() {
    // Karte löschen
    if(key[KEY_C]) {
        ClearMap();
    }else if(key[KEY_S]) {
        SaveMap("map/map1.dat");
    }else if(key[KEY_L]) {
        LoadMap("map/map1.dat");
    }else if(key[KEY_SPACE]) {
        if(isCursorOnMap()) {
            SetTile(SKY, mouse_x / TILE_WIDTH, mouse_y / TILE_HEIGHT);
        }
    }/*else if(key[KEY_RIGHT]) {
        if(actTileset < sizeof(tilesets) - 1) {
            actTileset += 1;
        }
    }else if(key[KEY_LEFT]) {
        if(actTileset > 0) {
            actTileset -= 1;
        }
    }*/
}

int main() {	
	Setup();
	
	while(!key[KEY_ESC]) {
        // Eingaben checken
        CheckMouseInput();
        CheckKeyboardInput();
        
        // Karte erneuern
        DrawMap();
        DrawTileset();
        
        // Cursor setzen
        SetCursor();
        
        // Puffer auf den Bildschirm zeichnen
        draw_sprite(screen, buffer, 0, 0);
        clear_bitmap(buffer);
        
        rest(1);
    }
    
    // Bitmaps aus dem Speicher löschen
    for(int material = 0; material < NUM_OF_MATERIALS; material++) {
        destroy_bitmap(materials[material]);        
    } 
              
	destroy_bitmap(buffer);
	
	// Engine beenden
	allegro_exit();
	
	return 0;
}
END_OF_MAIN();
