#ifndef WALL_HPP
#define WALL_HPP

#include "base.hpp"
#include "audio.hpp"

// I'll just dump these here.
GLuint TEXTURE_WALL = 0, TEXTURE_DOOR = 0, TEXTURE_DOORWALL = 0,
       TEXTURE_EXIT_UP = 0, TEXTURE_EXIT_DOWN = 0;

class cube : public drawnObject {

    public:

        cube(const vec2 &Pos, const vec2 &BoundingBox = vec2(2.f, 2.f), const vec2 &Scale = vec2(2.f, 2.f), const vec3 &Ang = vec3(), GLuint FBTex = TEXTURE_WALL, GLuint LRTex = TEXTURE_WALL):
             drawnObject(Pos, BoundingBox, Ang), scale(Scale), frontBackTex(FBTex), leftRightTex(LRTex)
        {
            objectType = TYPE_WALL;
        }
        ~cube() {}

        virtual void use() {}
        void draw() {

            glPushMatrix();

                glTranslatef(objectPos.x, 1.25f, objectPos.y);
                glRotatef(objectAng.x, 1.f, 0.f, 0.f);
                glRotatef(objectAng.y, 0.f, 1.f, 0.f);
                glRotatef(objectAng.z, 0.f, 0.f, 1.f);

                glEnable(GL_TEXTURE_2D);

                // Front Face
                glBindTexture(GL_TEXTURE_2D, frontBackTex);
                glBegin(GL_QUADS);

                    glNormal3f(0.f, 0.f, -1.f);
                    glTexCoord2f(0.f, 1.f);
                    glVertex3f(-scale.x / 2.f, 1.25f, -scale.y / 2.f);
                    glTexCoord2f(1.f, 1.f);
                    glVertex3f(scale.x / 2.f, 1.25f, -scale.y / 2.f);
                    glTexCoord2f(1.f, 0.f);
                    glVertex3f(scale.x / 2.f, -1.25f, -scale.y / 2.f);
                    glTexCoord2f(0.f, 0.f);
                    glVertex3f(-scale.x / 2.f, -1.25f, -scale.y / 2.f);

                glEnd();

                // Back Face
                glBegin(GL_QUADS);

                    glNormal3f(0.f, 0.f, 1.f);
                    glTexCoord2f(0.f, 1.f);
                    glVertex3f(-scale.x / 2.f, 1.25f, scale.y / 2.f);
                    glTexCoord2f(1.f, 1.f);
                    glVertex3f(scale.x / 2.f, 1.25f, scale.y / 2.f);
                    glTexCoord2f(1.f, 0.f);
                    glVertex3f(scale.x / 2.f, -1.25f, scale.y / 2.f);
                    glTexCoord2f(0.f, 0.f);
                    glVertex3f(-scale.x / 2.f, -1.25f, scale.y / 2.f);

                glEnd();

                glBindTexture(GL_TEXTURE_2D, leftRightTex);
                glBegin(GL_QUADS);

                    glNormal3f(-1.f, 0.f, 0.f);
                    glTexCoord2f(0.f, 0.f);
                    glVertex3f(-scale.x / 2.f, -1.25f, -scale.y / 2.f);
                    glTexCoord2f(1.f, 0.f);
                    glVertex3f(-scale.x / 2.f, -1.25f, scale.y / 2.f);
                    glTexCoord2f(1.f, 1.f);
                    glVertex3f(-scale.x / 2.f, 1.25f, scale.y / 2.f);
                    glTexCoord2f(0.f, 1.f);
                    glVertex3f(-scale.x / 2.f, 1.25f, -scale.y / 2.f);

                glEnd();

                glBegin(GL_QUADS);

                    glNormal3f(1.f, 0.f, 0.f);
                    glTexCoord2f(0.f, 0.f);
                    glVertex3f(scale.x / 2.f, -1.25f, -scale.y / 2.f);
                    glTexCoord2f(1.f, 0.f);
                    glVertex3f(scale.x / 2.f, -1.25f, scale.y / 2.f);
                    glTexCoord2f(1.f, 1.f);
                    glVertex3f(scale.x / 2.f, 1.25f, scale.y / 2.f);
                    glTexCoord2f(0.f, 1.f);
                    glVertex3f(scale.x / 2.f, 1.25f, -scale.y / 2.f);

                glEnd();

                glDisable(GL_TEXTURE_2D);

            glPopMatrix();

        }
        virtual void think() {}
    protected:

        vec2 scale;
        GLuint frontBackTex, leftRightTex;

};
class door : public cube {

    public:

        door(vec2 &Pos, vec3 &Ang):
             cube(Pos, vec2(2.f, 2.f), vec2(2.f, 0.2f), Ang, TEXTURE_DOOR, TEXTURE_DOOR), openPos(Ang.y == 0.f ? (Pos + vec2(2.5f, 0.f)) : Ang.y == -90.f ? (Pos + vec2(0.f, 2.5f)) : Pos), closePos(Pos), dir(Ang.y == 0.f ? vec2(0.f, -1.f) : vec2(-1.f, 0.f)), bOpen(false), closeTick(0)
        {
            objectType = TYPE_DOOR;
        }
        ~door() {}

        void open() {

            if (bOpen) { return; }
            bOpen = true;
            playSound(SOUND_DOPEN, SOURCE_DOOR, closePos);

        }
        void close() {

            if (!bOpen) { return; }
            bOpen = false;
            playSound(SOUND_DCLOSE, SOURCE_DOOR, closePos);

        }
        void toggle() { bOpen = !bOpen; }
        void use() { open(); }
        void think() {

            if (bOpen == true) {

                closeTick = closeTick == 0 ? clock() + 7500 : closeTick;
                if (clock() > closeTick) {

                    close();
                    closeTick = 0;

                }
                else if (objectPos.x <= openPos.x && objectAng.y == 0.f) {

                    objectPos.x+= 0.015625f * 4;

                }
                else if (objectPos.y <= openPos.y && objectAng.y == -90.f) {

                    objectPos.y+= 0.015625f * 4;

                }

            }
            else if (objectPos.x > closePos.x) { objectPos.x-= 0.015625f * 4; }
            else if (objectPos.y > closePos.y) { objectPos.y-= 0.015625f * 4; }

        }
        bool isOpen() { return bOpen; }

    protected:

        bool bOpen;
        vec2 dir, openPos, closePos;
        unsigned int closeTick;

};
class exit : public cube {
    public:
        exit(vec2 &Pos):
             cube(Pos, vec2(2.f, 2.f), vec2(2.f, 0.2f), vec3(), TEXTURE_EXIT_UP, TEXTURE_EXIT_UP), openPos(Ang.y == 0.f ? (Pos + vec2(2.5f, 0.f)) : Ang.y == -90.f ? (Pos + vec2(0.f, 2.5f)) : Pos), closePos(Pos), dir(Ang.y == 0.f ? vec2(0.f, -1.f) : vec2(-1.f, 0.f)), levelCompleted(false)
        void use() {
            levelCompleted = true;
            playSound(SOUND_EXIT, SOURCE_DOOR, objectPos);
        }
        bool isLevelComplete() { return levelCompleted; }
    protected:
        bool levelCompleted;
};

#endif // WALL_HPP
