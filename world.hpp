#ifndef WORLD_HPP
#define WORLD_HPP

#include <iostream>

#include <vector>
#include <fstream>
#include <ctime>

#include "vec.hpp"
#include "base.hpp"
#include "mob.hpp"
#include "audio.hpp"

class world {

    public:

        world():
              ply(mapMatrix, mapX, mapY, ents, vec2()), mapX(0), mapY(0), mapLoaded(false), levelComplete(false)
        {

            // Initialise mapMatrix with NULL pointers.
            for (int y = 0; y < 128; y++) {

                for (int x = 0; x < 128; x++) {

                    mapMatrix.data[x][y] = 0;

                }

            }

        }
        ~world() { cleanMap(); }

        void cleanMap() {

            for (int y = 0; y < mapY; y++) {

                for (int x = 0; x < mapX; x++) {

                    // If the element at [x][y] is NULL, continue.
                    if (mapMatrix.data[x][y] == 0) { continue; }

                    // Otherwise it's allocated memory, so we'll want to delete (re-allocate) it.
                    delete mapMatrix.data[x][y];

                }

            }
            for (int i = 0; i < ents.size(); i++) {

                // Re-allocate memory dedicated to an entity.
                delete ents[i];

            }
            ents.clear();
            ply.reset();
            mapLoaded = false;

        }
        void loadMap(char *FileName) {

            cleanMap();

            std::ifstream data(FileName, std::ios_base::in);
            mapX = mapY = 0;
            std::vector<std::string> tmpData;

            if (!data.is_open()) { return; }
            while (!data.eof()) {

                std::string tmpStr;
                std::getline(data, tmpStr);
                tmpData.push_back(tmpStr);

                // Get new size of map.
                mapX = tmpStr.length() > mapX ? tmpStr.length() : mapX;
                mapY++;

            }

            GLuint tmpFBTex = -1, tmpLRTex = -1;
            vec3 tmpAng;

            // Fill mapMatrix with walls/doors, and ents vector with entities.
            for (int y = 0; y < mapY; y++) {

                for (int x = 0; x < tmpData[y].length(); x++) {

                    vec2 pos(x * 2.f, y * 2.f), tile(x, y);
                    switch (tmpData[y][x]) {

                        case '*':

                            tmpFBTex = (y > 0 && tmpData[y - 1][x] == 'D' || y + 1 < mapY && tmpData[y + 1][x] == 'D' ? TEXTURE_DOORWALL : TEXTURE_WALL);
                            tmpLRTex = (x > 0 && tmpData[y][x - 1] == 'D' || x + 1 < mapX && tmpData[y][x + 1] == 'D' ? TEXTURE_DOORWALL : TEXTURE_WALL);
                            setMapTile(new cube(pos, vec2(2.f, 2.f), vec2(2.f, 2.f), vec3(), tmpFBTex, tmpLRTex), tile);
                            break;

                        case 'D':

                            tmpAng = (tmpData[y][x - 1] == '*' && tmpData[y][x + 1] == '*' ? vec3() : tmpData[y - 1][x] == '*' && tmpData[y + 1][x] == '*' ? vec3(0.f, -90.f, 0.f) : vec3());
                            setMapTile(new door(pos, tmpAng), tile);
                            break;

                        case 'P':

                            ply.setPos(pos);
                            break;

                        case '1':

                            ents.push_back(new tCross(ply, pos));
                            break;

                        case '2':

                            ents.push_back(new tCup(ply, pos));
                            break;

                        case '3':

                            ents.push_back(new tChest(ply, pos));
                            break;

                        case '4':

                            ents.push_back(new tCrown(ply, pos));
                            break;

                        case 'H':

                            ents.push_back(new hMedkit(ply, pos));
                            break;

                        case 'M':

                            ents.push_back(new pMach(ply, pos));
                            break;

                        case 'C':

                            ents.push_back(new pChain(ply, pos));
                            break;

                        case 'A':

                            ents.push_back(new pAmmo(ply, pos));
                            break;

                        case 'G':

                            ents.push_back(new nGuard(ply, pos));
                            break;

                    }

                }

            }
            data.close();
            mapLoaded = true;

        }
        void drawWorld() { // Self-explanatory.

            glPushMatrix();

                glColour(112.f, 122.f, 112.f);
                glBegin(GL_QUADS);

                    glVertex3f(-1.f, 0.f, -1.f);
                    glVertex3f(mapX * 2.f, 0.f, -1.f);
                    glVertex3f(mapX * 2.f, 0.f, mapY * 2.f);
                    glVertex3f(-1.f, 0.f, mapY * 2.f);

                glEnd();
                glColour(56.f, 56.f, 56.f);
                glBegin(GL_QUADS);

                    glVertex3f(-1.f, 2.5f, -1.f);
                    glVertex3f(mapX * 2.f, 2.5f, -1.f);
                    glVertex3f(mapX * 2.f, 2.5f, mapY * 2.f);
                    glVertex3f(-1.f, 2.5f, mapY * 2.f);

                glEnd();

            glPopMatrix();
            glColor3f(1.f, 1.f, 1.f);
            for (int y = 0; y < mapY; y++) {

                for (int x = 0; x < mapX; x++) {

                    cube *ent = mapMatrix.data[x][y];
                    if (ent == 0) { continue; }
                    else {
                        switch (ent->getType()) {
                            case TYPE_EXIT:
                                levelComplete = ((exit*)ent)->isLevelComplete();
                                break;
                        }
                    }
                    ent->draw();
                    ent->think();

                }

            }
            for (int i = 0; i < ents.size(); i++) {

                mob *ent = ents[i];
                if (ent == 0) { continue; }
                ent->draw();
                ent->think();

            }
            ply.think();

        }
        bool isMapLoaded() { return mapLoaded; }
        const int& getMapX() { return mapX; }
        const int& getMapY() { return mapY; }
        const matrix<cube*>& getMapMatrix() { return mapMatrix; }
        const std::vector<mob*>& getEnts() { return ents; }
        cube* getMapTile(const vec2 &Tile) {

            if (Tile.x < 0 || Tile.x > mapX || Tile.y < 0 || Tile.y > mapY) { return 0; }
            return mapMatrix.data[(int)round(Tile.x)][(int)round(Tile.y)];

        }
        bool setMapTile(cube *Object, const vec2 &Tile) {

            if (Tile.x < 0 || Tile.x > mapX || Tile.y < 0 || Tile.y > mapY) { return false; }
            mapMatrix.data[(int)round(Tile.x)][(int)round(Tile.y)] = Object;

        }
        bool isLevelComplete() { return levelComplete; }
        player& getPly() { return ply; }


    protected:

        int mapX, mapY;
        matrix<cube*> mapMatrix;
        std::vector<mob*> ents;
        player ply;
        bool mapLoaded, levelComplete;

};

#endif // WORLD_HPP
