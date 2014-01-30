#ifndef ENGINE_HPP
#define ENGINE_HPP

#include <string>

#include "audio.hpp"
#include "texloader.hpp"
#include "camera.hpp"
#include "world.hpp"

#define BUILD_FOR_WINDOWS

#ifdef BUILD_FOR_WINDOWS
#include <Windows.h>
#endif

/*

    TODO:

    Create AI/pathfinding.


*/

enum fontSizes {

    FONT_SMALL = 0,
    FONT_LARGE = 1

};

class engine {

    public:

        engine(int *argc, char **argv):
              fullScreen(false), inGame(false), inMenu(true), gunFlash(false), how2play(false), cam(videoMode, engineWorld), activeFont(FONT_SMALL), sleep(0)
        {

            glfwInit();

            alutInit(argc, argv);
            #ifdef BUILD_FOR_WINDOWS

                char str[128];
                if (alutGetError()) {

                    sprintf(str, "%s.\n%s", alutGetErrorString(alutGetError()), "You should probably install OpenAL 1.1.");
                    MessageBox(NULL, str, "Open AL Error :(", MB_OK | MB_ICONERROR);

                }

            #endif



            glfwGetDesktopMode(&videoMode);
            if (glfwOpenWindow(videoMode.Width, videoMode.Height, 8, 8, 8, 8, 8, 0, GLFW_WINDOW)) {

                srand(time(0));
                glfwDisable(GLFW_MOUSE_CURSOR);
                glfwSetWindowTitle("Steinwolfen 3D");
                glfwRestoreWindow();
                initGL();
                initAL();
                loadTextures();
                loadSprites();
                run();

            }


        }

        ~engine() {

            alutExit();
            glfwTerminate();

        }

    private:

        void initGL() {

            glClearColor(0.f, 0.25f, 0.25, 0.f);
            start3D();
            glEnable(GL_ALPHA_TEST);
            glAlphaFunc(GL_GREATER, 0.5);

        }
        void initAL() {

            // Music
            MUSIC_SPLASH = alutCreateBufferFromFile("music/splash.wav");
            MUSIC_MENU = alutCreateBufferFromFile("music/menu.wav");
            MUSIC_WIN = alutCreateBufferFromFile("music/win.wav");

            MENU_SWITCH = alutCreateBufferFromFile("sounds/M_SEL.wav");
            MENU_SELECT = alutCreateBufferFromFile("sounds/M_BACK.wav");
            MENU_BACK = alutCreateBufferFromFile("sounds/DSBO3ATK.wav");

            // Door Sounds
            SOUND_DOPEN = alutCreateBufferFromFile("sounds/DSDOROPN.wav");
            SOUND_DCLOSE = alutCreateBufferFromFile("sounds/DSDORCLS.wav");

            // Treasure Sounds
            SOUND_TCROSS = alutCreateBufferFromFile("sounds/T_CROSS.wav");
            SOUND_TCUP = alutCreateBufferFromFile("sounds/T_CUP.wav");
            SOUND_TCHEST = alutCreateBufferFromFile("sounds/T_CHEST.wav");
            SOUND_TCROWN = alutCreateBufferFromFile("sounds/T_CROWN.wav");

            // Health Sounds
            SOUND_HMEDKIT = alutCreateBufferFromFile("sounds/P_MEDKIT.wav");

            // Weapon Sounds
            SOUND_WKNIFE = alutCreateBufferFromFile("sounds/DSWKNIF.wav");
            SOUND_WPISTOL = alutCreateBufferFromFile("sounds/DSWPIST.wav");
            SOUND_WMACH = alutCreateBufferFromFile("sounds/DSWMGUN.wav");
            SOUND_WCHAIN = alutCreateBufferFromFile("sounds/DSWCGUN.wav");
            SOUND_PMACH = alutCreateBufferFromFile("sounds/P_MGUN.wav");
            SOUND_PCHAIN = alutCreateBufferFromFile("sounds/P_CGUN.wav");
            SOUND_PAMMO = alutCreateBufferFromFile("sounds/P_AMMO.wav");

            // NPC Sounds
            char str[64];
            for (int i = 0; i < 7; i++) {

                sprintf(str, "sounds/DSSOLDT%i.wav", i);
                SOUND_NGUARD_DED[i] = alutCreateBufferFromFile(str);

            }
            SOUND_NGUARD_AGG = alutCreateBufferFromFile("sounds/DSBRNSIT.wav");


            alGenSources(1, &SOURCE_MENU);
            alGenSources(1, &SOURCE_MUSIC);
            alGenSources(1, &SOURCE_DOOR);
            alGenSources(1, &SOURCE_PICKUP);
            alGenSources(1, &SOURCE_NPC);

        }

        void loadTextures() {

            // Load Menu textures
            TEXTURE_SPLASH = loadTexture("textures/menu/splash.tga");
            TEXTURE_NAVIGATE = loadTexture("textures/menu/navigate.tga");
            TEXTURE_OPTIONS = loadTexture("textures/menu/options.tga");
            TEXTURE_GUN = loadTexture("textures/menu/gun.tga");
            TEXTURE_GUNBLINK = loadTexture("textures/menu/gunblink.tga");

            // Load Game textures
            TEXTURE_DOOR = loadTexture("textures/door.tga");
            TEXTURE_DOORWALL = loadTexture("textures/doorwall.tga");
            TEXTURE_WALL = loadTexture("textures/wall.tga");

            loadWeapon("textures/weapons/knife.tga", TEXTURES_WEAPONS[0]);
            loadWeapon("textures/weapons/pistol.tga", TEXTURES_WEAPONS[1]);
            loadWeapon("textures/weapons/mach.tga", TEXTURES_WEAPONS[2]);
            loadWeapon("textures/weapons/chain.tga", TEXTURES_WEAPONS[3]);

            loadFont("small", font[FONT_SMALL]);

        }
        void loadSprites() {

            sprites.push_back(new sprite("textures/tcross.tga")); SPRITE_TCROSS = sprites.size() - 1;
            sprites.push_back(new sprite("textures/tcup.tga")); SPRITE_TCUP = sprites.size() - 1;
            sprites.push_back(new sprite("textures/tchest.tga")); SPRITE_TCHEST = sprites.size() - 1;
            sprites.push_back(new sprite("textures/tcrown.tga")); SPRITE_TCROWN = sprites.size() - 1;
            sprites.push_back(new sprite("textures/hmedkit.tga")); SPRITE_HMEDKIT = sprites.size() - 1;
            sprites.push_back(new sprite("textures/pmach.tga")); SPRITE_PMACH = sprites.size() - 1;
            sprites.push_back(new sprite("textures/pchain.tga")); SPRITE_PCHAIN = sprites.size() - 1;
            sprites.push_back(new sprite("textures/pammo.tga")); SPRITE_PAMMO = sprites.size() - 1;
            sprites.push_back(new spriteNPC("textures/npcs/guard.tga")); SPRITE_NGUARD = sprites.size() - 1;

        }
        void unloadSprites() {

            for (int i = 0; i < sprites.size(); i++) {

                if (sprites[i]) { delete sprites[i]; }

            }

        }

        // Rendering modes.
        void start3D() {

            // Store current width/height of window.
            int X, Y;
            glfwGetWindowSize(&X, &Y);
            glViewport(0, Y * 0.125, (GLsizei)X, (GLsizei)Y - Y * 0.125);
            glMatrixMode(GL_PROJECTION);
            glLoadIdentity();
            gluPerspective(90.0, videoMode.Width / videoMode.Height, 0.01, 50.0);
            glMatrixMode(GL_MODELVIEW);
            glLoadIdentity();
            glEnable(GL_DEPTH_TEST);

        }
        void start2D() {

            // Clearing the depth buffer, this means our 2D drawings will overlap the 3D.
            int X, Y;
            glfwGetWindowSize(&X, &Y);
            glViewport(0, 0, (GLsizei)X, (GLsizei)Y);
            glClear(GL_DEPTH_BUFFER_BIT);
            glMatrixMode(GL_PROJECTION);
            glLoadIdentity();
            gluOrtho2D(0.0, videoMode.Width, videoMode.Height, 0.0);
            glMatrixMode(GL_MODELVIEW);
            glLoadIdentity();
            glDisable(GL_DEPTH_TEST);

        }

        // Various rendering bits and bobs.
        void setFont(int Font) { activeFont = Font; }
        void writeString(float x, float y, const std::string &str, float scale = 1.f, vec3 colour = vec3() + 255.f) {

            if (str.length() == 0) { return; }

            // Allocate enough memory to hold the pixel data dedicated to the amount of chars in the string.
            GLuint strHeight = charHeight(str[0]), oStrWidth = strWidth(str.c_str()) * 4;
            byte pixelData[oStrWidth * strHeight];

            for (int curHeight = 0; curHeight < strHeight; curHeight++) {

                int offset = 0;
                for (int c = 0; c < str.length(); c++) {

                    // Get pixel data of character at index c of string.
                    int cWidth = charWidth(str[c]) * 4;
                    byte charPixelData[cWidth * strHeight];
                    glBindTexture(GL_TEXTURE_2D, font[activeFont][str[c]]);
                    glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, charPixelData);

                    // Fill each row with pixel data from each character.
                    // offset moves to the next character.
                    for (int i = 0; i < cWidth; i++) {

                        int address = (i + offset) + (curHeight * oStrWidth);
                        pixelData[address] = charPixelData[i + (curHeight * cWidth)];

                    }
                    offset+= cWidth;

                }

            }

            GLuint texture;

            glGenTextures(1, &texture);
            glBindTexture(GL_TEXTURE_2D, texture);

            glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, oStrWidth / 4, strHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, (void*)pixelData);

            glEnable(GL_TEXTURE_2D);
            glColour(colour.x, colour.y, colour.z);
            glBegin(GL_QUADS);

                glTexCoord2f(0.f, 1.f);
                glVertex2f(x, y);
                glTexCoord2f(1.f, 1.f);
                glVertex2f(x + oStrWidth / 4 * scale, y);
                glTexCoord2f(1.f, 0.f);
                glVertex2f(x + oStrWidth / 4 * scale, y + strHeight * scale);
                glTexCoord2f(0.f, 0.f);
                glVertex2f(x, y + strHeight * scale);

            glEnd();
            glDisable(GL_TEXTURE_2D);
            glDeleteTextures(1, &texture);

        }
        void drawRect(vec2 Pos, vec2 Scale, double R, double G, double B) {

            glColour(R, G, B);
            glBegin(GL_QUADS);

                glVertex2f(Pos.x, Pos.y);
                glVertex2f(Pos.x + Scale.x, Pos.y);
                glVertex2f(Pos.x + Scale.x, Pos.y + Scale.y);
                glVertex2f(Pos.x, Pos.y + Scale.y);

            glEnd();

        }
        void drawTexRect(vec2 Pos, GLuint Texture, float size = 1.f) {

            glEnable(GL_TEXTURE_2D);
            glColor3f(1.f, 1.f, 1.f);
            glBindTexture(GL_TEXTURE_2D, Texture);

            float texW, texH;
            glGetTexLevelParameterfv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &texW);
            glGetTexLevelParameterfv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &texH);
            texW*= size, texH*= size;

            glPushMatrix();

                glTranslatef(Pos.x, Pos.y, 0.f);
                glBegin(GL_QUADS);

                    glTexCoord2f(0.f, 1.f);
                    glVertex2f(-texW / 2.f, -texH / 2.f);
                    glTexCoord2f(1.f, 1.f);
                    glVertex2f(texW / 2.f, -texH / 2.f);
                    glTexCoord2f(1.f, 0.f);
                    glVertex2f(texW / 2.f, texH / 2.f);
                    glTexCoord2f(0.f, 0.f);
                    glVertex2f(-texW / 2.f, texH / 2.f);

                glEnd();

            glPopMatrix();
            glDisable(GL_TEXTURE_2D);

        }
        vec2 getTexScale(GLuint Texture) {

            vec2 scale;
            glBindTexture(GL_TEXTURE_2D, Texture);
            glGetTexLevelParameterfv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &scale.x);
            glGetTexLevelParameterfv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &scale.y);
            return scale;

        }
        float charHeight(char Char) {

            glBindTexture(GL_TEXTURE_2D, font[activeFont][Char]);
            int tmp;
            glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &tmp);
            return (float)tmp;

        }
        float charWidth(char Char) {

            glBindTexture(GL_TEXTURE_2D, font[activeFont][Char]);
            int tmp;
            glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &tmp);
            return (float)tmp;

        }
        float strWidth(char *Str) {

            int i = 0;
            float width = 0.f;
            while (true) {

                if (Str[i] == '\0') { return width; }
                width+= charWidth(Str[i]);
                i++;

            }
            return width;

        }
        float strWidth(const char *Str) {

            int i = 0;
            float width = 0.f;
            while (true) {

                if (Str[i] == '\0') { return width; }
                width+= charWidth(Str[i]);
                i++;

            }
            return width;

        }
        void startGame() {

            engineWorld.loadMap("map1.map");
            cam.setPos(engineWorld.getPly().getPos());
            inMenu = false;
            inGame = true;
            alSourceStop(SOURCE_MUSIC);

        }

        void menuSelect() {

            switch (menuSwitch) {

                // New Game
                case 0:

                    startGame();
                    break;

                // How to play
                case 1:

                    how2play = true;
                    break;

                // Back to Game
                case 2:

                    if (inGame) { inMenu = false; }
                    break;

                // Exit
                case 3:

                    runGame = false;
                    break;

            }

        }
        void drawMenu() {

            start2D();

            // Manage the background music, check whether it is already playing.
            ALint playing = 0;
            alGetSourcei(SOURCE_MUSIC, AL_BUFFER, &playing);
            if (playing != MUSIC_MENU) { playMusic(MUSIC_MENU); }

            // Draw pretty gradient background.
            glColour(136.0, 0.0, 0.0);
            glBegin(GL_QUADS);

                glVertex2f(0.f, 0.f);
                glVertex2f(videoMode.Width, 0.f);
                glVertex2f(videoMode.Width, videoMode.Height);
                glVertex2f(0.f, videoMode.Height);

            glEnd();


            drawTexRect(vec2(videoMode.Width / 2.f, getTexScale(TEXTURE_OPTIONS).y / 2.f), TEXTURE_OPTIONS);
            if (how2play) {

                writeString(64.f, 96.f, "How to play:");
                writeString(64.f, 128.f, "Use the mouse to look around or move.");
                writeString(64.f, 146.f, "Alternatively, you can use the arrow-keys to move.");
                writeString(64.f, 164.f, "You can use the middle mouse button or space to open doors.");

                if (glfwGetKey(GLFW_KEY_ESC) == GLFW_PRESS) {

                    how2play = false;
                    playSound(MENU_BACK, SOURCE_MENU, vec3(), false, true);

                }
                return;

            }
            drawTexRect(vec2(videoMode.Width / 2.f, videoMode.Width - getTexScale(TEXTURE_NAVIGATE).y), TEXTURE_NAVIGATE);

            if (clock() > gunFlashTick) {

                gunFlashTick = clock() + (gunFlash ? 2000 : 500);
                gunFlash = !gunFlash;

            }
            drawTexRect(vec2(videoMode.Width / 2.f - 160.f, 141.f + menuSwitch * 17.5f), gunFlash ? TEXTURE_GUN : TEXTURE_GUNBLINK, 2.f);

            // Draw ugly boring grey text.
            if (clock() > menuDelayTick) {

                if (glfwGetKey(GLFW_KEY_UP) == GLFW_PRESS) {

                    menuSwitch = menuSwitch <= 0 ? 3 : menuSwitch - 1 == 2 && !inGame ? menuSwitch - 2 : menuSwitch - 1;
                    playSound(MENU_SWITCH, SOURCE_MENU, vec3(), false, true);
                    menuDelayTick = clock() + 250;

                }
                else if (glfwGetKey(GLFW_KEY_DOWN) == GLFW_PRESS) {

                    menuSwitch = menuSwitch >= 3 ? 0 : menuSwitch + 1 == 2 && !inGame ? menuSwitch + 2 : menuSwitch + 1;
                    playSound(MENU_SWITCH, SOURCE_MENU, vec3(), false, true);
                    menuDelayTick = clock() + 250;

                }
                else if (glfwGetKey(GLFW_KEY_ENTER) == GLFW_PRESS) {

                    playSound(MENU_SELECT, SOURCE_MENU, vec3(), false, true);
                    menuDelayTick = clock() + 500;
                    menuSelect();
                    return;

                }

            }

            writeString(videoMode.Width / 2.f - 96.f, 128.f, "New Game", 1.f, (menuSwitch == 0 ? vec3() + 255.f : vec3() + 128.f));
            writeString(videoMode.Width / 2.f - 96.f, 146.f, "How to play", 1.f, (menuSwitch == 1 ? vec3() + 255.f : vec3() + 128.f));
            writeString(videoMode.Width / 2.f - 96.f, 164.f, "Back to Game", 1.f, !inGame ? vec3(128.f, 64.f, 0.f) : (menuSwitch == 2 ? vec3() + 255.f : vec3() + 128.f));
            writeString(videoMode.Width / 2.f - 96.f, 182.f, "Quit", 1.f, (menuSwitch == 3 ? vec3() + 255.f : vec3() + 128.f));

        }
        void game() {

            if (glfwGetKey(GLFW_KEY_ESC) == GLFW_PRESS) { inMenu = true; playMusic(MUSIC_MENU); }

            start3D();

            cam.controlCamera(0.125f, 0.25f);
            cam.updateCamera();

            player &ply = engineWorld.getPly();

            ply.setPos(cam.getPos());
            ply.setAng(cam.getAng());

            // Render World
            engineWorld.drawWorld();

            // Render HUD Elements
            start2D();
            char xyz[64];
            mob *tmp = 0;
            for (int i = 0; i < engineWorld.getEnts().size(); i++) {

                if (engineWorld.getEnts()[i]->getType() == TYPE_NPC) {

                    tmp = engineWorld.getEnts()[i];
                    break;

                }

            }

            /*sprintf(xyz, "Dot Product: %f", (dot(tmp->getEye(), normalise(tmp->getPos() - ply.getPos()))));
            writeString(0.f, 0.f, xyz);*/

            // Manage weapon viewmodel drawing.

            // Manage input.
            if (glfwGetMouseButton(GLFW_MOUSE_BUTTON_1)) { ply.shoot(); }
            else if (glfwGetKey(GLFW_KEY_KP_1)) { ply.setActiveWeapon(WEAPON_KNIFE); }
            else if (glfwGetKey(GLFW_KEY_KP_2)) { ply.setActiveWeapon(WEAPON_PISTOL); }
            else if (glfwGetKey(GLFW_KEY_KP_3)) { ply.setActiveWeapon(WEAPON_MACH); }
            else if (glfwGetKey(GLFW_KEY_KP_4)) { ply.setActiveWeapon(WEAPON_CHAIN); }

            // Get textures of player's current active weapon.
            GLuint *wepTextures = TEXTURES_WEAPONS[ply.getActiveWeapon()];

            glPushMatrix();

                glEnable(GL_TEXTURE_2D);

                glBindTexture(GL_TEXTURE_2D, wepTextures[ply.getActiveWeaponAnim()]);
                glTranslatef(videoMode.Width / 2.f, videoMode.Height * 0.875 - 256, 0.f);
                glBegin(GL_QUADS);

                    glTexCoord2f(0.f, 1.f);
                    glVertex2f(-256.f, -256.f);
                    glTexCoord2f(1.f, 1.f);
                    glVertex2f(256.f, -256.f);
                    glTexCoord2f(1.f, 0.f);
                    glVertex2f(256.f, 256.f);
                    glTexCoord2f(0.f, 0.f);
                    glVertex2f(-256.f, 256.f);

                glEnd();

                glDisable(GL_TEXTURE_2D);

            glPopMatrix();

            // Draw Boxes
            char str[64];
            drawRect(vec2(0.f, videoMode.Height * 0.875f), vec2(videoMode.Width, videoMode.Height * 0.125f), 0.0, 0.0, 128.0);

            sprintf(str, "%s", "Score");
            writeString(videoMode.Width * 0.25f - strWidth(str) / 2.f, videoMode.Height * 0.875 + 12.f, str);

            sprintf(str, "%i", ply.getScore());
            writeString(videoMode.Width * 0.25 - strWidth(str) / 2.f, videoMode.Height * 0.875 + 32.f, str);

            sprintf(str, "%s", "Ammo");
            writeString(videoMode.Width * 0.75f - strWidth(str) / 2.f, videoMode.Height * 0.875 + 12.f, str);

            sprintf(str, "%i", ply.getAmmo());
            writeString(videoMode.Width * 0.75 - strWidth(str) / 2.f, videoMode.Height * 0.875 + 32.f, str);

            sprintf(str, "%s", "Health");
            writeString(videoMode.Width * 0.5f - strWidth(str) / 2.f, videoMode.Height * 0.875 + 12.f, str);

            sprintf(str, "%i%c", ply.getHealth(), '%');
            writeString(videoMode.Width * 0.5f - strWidth(str) / 2.f, videoMode.Height * 0.875 + 32.f, str);

        }

        void run() {

            runGame = true;
            while (runGame) {

                if (clock() - sleep < 10) { continue; }
                sleep = clock();
                if (glfwGetKey(GLFW_KEY_F11) == GLFW_PRESS) {

                    fullScreen = !fullScreen;
                    glfwCloseWindow();
                    glfwOpenWindow(videoMode.Width, videoMode.Height, 8, 8, 8, 8, 8, 0, fullScreen ? GLFW_FULLSCREEN : GLFW_WINDOW);
                    initGL();
                    loadTextures();
                    loadSprites();

                }

                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
                glLoadIdentity();

                if (inMenu) { drawMenu();  }
                else { game(); }

                glfwSwapBuffers();
                //glfwSleep(0.005);

            }

        }

        GLFWvidmode videoMode;
        bool fullScreen, runGame, inMenu, inGame;

        // Menu related
        bool gunFlash, how2play;
        unsigned int gunFlashTick, menuDelayTick, sleep;
        int menuSwitch;

        // Various textures.
        GLuint TEXTURE_SPLASH, TEXTURE_NAVIGATE, TEXTURE_OPTIONS,
               TEXTURE_GUN, TEXTURE_GUNBLINK, TEXTURE_HUD;

        GLuint TEXTURES_WEAPONS[4][5];

        // Font related
        int activeFont;
        GLuint font[2][256];

        // Game related
        world engineWorld;
        camera cam;

};

#endif // ENGINE_HPP
