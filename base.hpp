#ifndef BASE_HPP
#define BASE_HPP

#include "vec.hpp"

class drawnObject {

    public:

        drawnObject(const vec2 &Pos, const vec2 &BoundingBox, const vec3 &Ang = vec3()):
                    objectPos(Pos), objectBoundingBox(BoundingBox), objectAng(Ang), objectType(0) {};

        vec2 getMinBox() { return -(objectBoundingBox); }
        vec2 getMaxBox() { return (objectBoundingBox); }
        vec2 getPos() { return objectPos; }

        void setPos(const vec2 &Pos) { objectPos = Pos; }
        void setAng(const vec3 &Ang) { objectAng = Ang; }

        bool collidesWith(drawnObject *Object, const vec2 &Vel = vec2()) {

            vec2 pos = Object->getPos();
            vec2 ObjectPos = Object->getPos() + Vel;
            return ObjectPos + Object->getMinBox() >= objectPos + getMinBox() && ObjectPos + Object->getMaxBox() <= objectPos + getMaxBox() ? true : false;

        }
        vec2 getTile() {

            return vec2(round(objectPos.x / 2.f), round(objectPos.y / 2.f));

        }
        vec2 getTile(const vec2 &Pos) {

            return vec2(round(Pos.x / 2.f), round(Pos.y / 2.f));

        }
        virtual void draw() = 0;
        virtual void think() = 0;
        int getType() { return objectType; }

    protected:

        vec2 objectPos, objectBoundingBox;
        vec3 objectAng;
        int objectType;

};

enum worldTypes {

    TYPE_WALL = 0,
    TYPE_DOOR = 1,
    TYPE_EXIT = 2,
    TYPE_MOB = 3,
    TYPE_DECOR = 4,
    TYPE_PICKUP = 5,
    TYPE_PLAYER = 6,
    TYPE_NPC = 7

};

template<typename t>
struct matrix {

    t data[128][128];

};

#endif // BASE_HPP
