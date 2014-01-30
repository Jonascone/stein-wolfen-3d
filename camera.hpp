#ifndef CAMERA_HPP
#define CAMERA_HPP

#include <GL/glfw.h>

#include <iostream>

#include "audio.hpp"
#include "vec.hpp"
#include "base.hpp"
#include "world.hpp"

class camera {

    public:

        camera(GLFWvidmode &VideoMode, world &World):
               videoMode(VideoMode), engineWorld(World), inScreen(true), collisionObj(camPos, vec2(0.75f, 0.75f))
        {}

        vec2 getVel(float Dist, float Dir) {

            float rad = deg2rad(camAng.y + Dir);
            vec2 tmp = camPos;
            tmp.x-= sin(rad) * Dist;
            tmp.y-= cos(rad) * Dist;
            return (tmp - camPos);

        }
        vec2 getNext(float Dist, float Dir) {

            return (camPos + getVel(Dist, Dir));

        }
        void moveCamera(float Dist, float Dir) {

            float rad = deg2rad(camAng.y + Dir);
            camPos.x-= sin(rad) * Dist;
            camPos.y-= cos(rad) * Dist;

        }
        void lockCamera() {

            // Clamp the view angles of the camera.
            //camAng.x = (camAng.x > 90.0 ? 90.0 : camAng.x < -90.0 ? -90.0 : camAng.x);
            spriteRot = camAng.y = (camAng.y < 0.0 ? camAng.y + 360.0 : camAng.y > 360.0 ? camAng.y - 360.0 : camAng.y);

        }
        void updateCamera() {

            glTranslatef(-camPos.x, -1.25f, -camPos.y);
            alListener3f(AL_POSITION, camPos.x, 0.f, camPos.y);
            float rad = deg2rad(camAng.y);
            float orientation[] = {-sin(rad), 0.f, -cos(rad), 0.f, 1.f, 0.f};
            alListenerfv(AL_ORIENTATION, orientation);

        }

        cube* checkCollisions(float MoveVel, float Dir) {

            const matrix<cube*> &mapMatrix = engineWorld.getMapMatrix();
            cube* ent = 0;
            for (int y = 0; y < engineWorld.getMapY(); y++) {

                for (int x = 0; x < engineWorld.getMapX(); x++) {

                    ent = mapMatrix.data[x][y];
                    if (ent && ent->collidesWith(&collisionObj, getVel(MoveVel, Dir))) {

                        return ent;

                    }

                }

            }
            return 0;

        }

        void controlCamera(float MoveVel, float MouseVel) {

            if (glfwGetWindowParam(GLFW_ACTIVE) == GL_FALSE) {

                //glRotatef(-camAng.x, 1.0, 0.0, 0.0);
                glRotatef(-camAng.y, 0.0, 1.0, 0.0);
                return;

            }
            if (glfwGetKey(GLFW_KEY_PAUSE) == GLFW_PRESS) {

                inScreen = !inScreen;

            }
            if (!inScreen) {

                glfwEnable(GLFW_MOUSE_CURSOR);
                glRotatef(-camAng.y, 0.f, 1.f, 0.f);
                return;

            }
            glfwDisable(GLFW_MOUSE_CURSOR);

            int midX = videoMode.Width / 2, midY = videoMode.Height / 2, mouseX, mouseY;
            glfwGetMousePos(&mouseX, &mouseY);

            camAng.y+= MouseVel * (midX - mouseX);
            float mouseDir = (midY - mouseY < 0 ? 180.f : 0.f), mouseMove = std::min<float>(0.35f, std::abs(midY - mouseY) / 100.f);
            if (!checkCollisions(mouseMove, mouseDir)) { moveCamera(mouseMove, mouseDir); }
            //camAng.x+= MouseVel * (midY - mouseY);
            lockCamera();
            //spriteRot = camAng.y;
            glfwSetMousePos(midX, midY);

            collisionObj.setPos(camPos);

            // Manage key input.
            if (glfwGetKey(GLFW_KEY_UP) == GLFW_PRESS) {

                if (!checkCollisions(MoveVel, 0.f)) { moveCamera(MoveVel, 0.f); }

            }
            else if (glfwGetKey(GLFW_KEY_DOWN) == GLFW_PRESS) {

                if (!checkCollisions(MoveVel, 180.f)) { moveCamera(MoveVel, 180.f); }

            }
            if (glfwGetKey(GLFW_KEY_LEFT) == GLFW_PRESS) {

                if (!checkCollisions(MoveVel, 90.f)) { moveCamera(MoveVel, 90.f); }

            }
            else if (glfwGetKey(GLFW_KEY_RIGHT) == GLFW_PRESS) {

                if (!checkCollisions(MoveVel, 270.f)) { moveCamera(MoveVel, 270.f); }

            }

            if (glfwGetKey(GLFW_KEY_SPACE) == GLFW_PRESS || glfwGetMouseButton(GLFW_MOUSE_BUTTON_3)) {

                cube *ent = checkCollisions(MoveVel, 0.f);
                if (ent) { ent->use(); }

            }

            //glRotatef(-camAng.x, 1.0, 0.0, 0.0);
            glRotatef(-camAng.y, 0.0, 1.0, 0.0);

        }
        void setPos(vec2 Pos) { camPos = Pos; }
        vec2 getPos() { return camPos; }
        vec2 getEye(){

            float rad = deg2rad(camAng.y);
            return vec2(-sin(rad), -cos(rad));

        }
        vec3 getAng() { return camAng; }

    private:

        world &engineWorld;
        GLFWvidmode &videoMode;
        bool inScreen;
        vec2 camPos;
        vec3 camAng;
        cube collisionObj;


};

#endif // CAMERA_HPP
