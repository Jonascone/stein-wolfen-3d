#ifndef MOB_HPP
#define MOB_HPP

#include "wall.hpp"
#include "sprite.hpp"
#include "vec.hpp"
//#include "raytrace2.hpp"

#include <cstdlib>
#include <iostream>

class mob : public drawnObject {

    public:

        mob(int SpriteID, const vec2 &Pos, int Health):
            drawnObject(Pos, vec2(0.75f, 0.75f)), mobSprite(SpriteID < 0 ? 0 : sprites[SpriteID]), mobSpriteID(SpriteID), mobHealth(Health)
        {
            objectType = TYPE_MOB;
        }
        ~mob() {};

        virtual void draw() {

            if (mobSpriteID == -1) { return; }
            glPushMatrix();

                glTranslatef(objectPos.x, 1.25f, objectPos.y);
                glRotatef((mobSprite->globalRotate ? spriteRot : 0.f), 0.f, 1.f, 0.f);
                objectAng.y = (mobSprite->globalRotate ? spriteRot : objectAng.y);

                glEnable(GL_TEXTURE_2D);

                glBindTexture(GL_TEXTURE_2D, mobSprite->texture);
                glBegin(GL_QUADS);

                    glNormal3f(0.f, 0.f, -1.f);
                    glTexCoord2f(0.f, 1.f);
                    glVertex3f(-1.f, 1.25f, 0.f);
                    glTexCoord2f(1.f, 1.f);
                    glVertex3f(1.f, 1.25f, 0.f);
                    glTexCoord2f(1.f, 0.f);
                    glVertex3f(1.f, -1.25f, 0.f);
                    glTexCoord2f(0.f, 0.f);
                    glVertex3f(-1.f, -1.25f, 0.f);

                glEnd();

                glDisable(GL_TEXTURE_2D);

            glPopMatrix();

        }
        virtual void think() {}
        vec2 getEye() {

            float rad = deg2rad(objectAng.y);
            return vec2(-sin(rad), -cos(rad));

        }

        void adjHealth(int Value) { mobHealth+= Value; }
        int getHealth() { return mobHealth; }

    protected:

        int mobHealth, mobSpriteID;
        sprite *mobSprite;

};

// Decoration Class
class decor : public mob {

    public:

        decor(int SpriteID, const vec2 &Pos):
              mob(SpriteID, Pos, 0)
        {
            objectType = TYPE_DECOR;
        }
        ~decor() {}

};

// Player Class
enum weaponClasses {

    WEAPON_KNIFE = 0,
    WEAPON_PISTOL = 1,
    WEAPON_MACH = 2,
    WEAPON_CHAIN = 3

};
class player : public mob {

    public:

        player(const matrix<cube*> &MapMatrix, const int &MapX, const int &MapY, const std::vector<mob*> &Ents, const vec2 &Pos):
               mob(-1, Pos, 100), worldMapMatrix(MapMatrix), worldMapX(MapX), worldMapY(MapY), worldEnts(Ents), score(0), ammo(16),
               levelComplete(false), pistol(true), mach(false), chain(false), shooting(false), holding(false),
               activeWeapon(WEAPON_PISTOL), activeWeaponAnim(0), animDelay(0)
        {
            objectType = TYPE_PLAYER;
        }
        ~player() {}

        void adjScore(int Value) { score+= Value; }
        int getScore() { return score; }

        void adjAmmo(int Value) { ammo+= Value; }
        int getAmmo() { return ammo; }

        int getActiveWeapon() { return activeWeapon; }
        int getActiveWeaponAnim() { return activeWeaponAnim; }
        void giveWeapon(int Weapon) {

            switch (Weapon) {

                case WEAPON_MACH:

                    mach = true;
                    break;

                case WEAPON_CHAIN:

                    chain = true;
                    break;

            }

        }
        bool hasWeapon(int Weapon) {

            switch (Weapon) {

                case WEAPON_KNIFE:
                    return true;
                    break;

                case WEAPON_PISTOL:
                    return pistol;
                    break;
                case WEAPON_MACH:
                    return mach;
                    break;
                case WEAPON_CHAIN:
                    return chain;
                    break;

            }

        }
        ALuint getActiveWeaponSound() {

            switch (activeWeapon) {

                case WEAPON_KNIFE:

                    return SOUND_WKNIFE;
                    break;

                case WEAPON_PISTOL:

                    return SOUND_WPISTOL;
                    break;

                case WEAPON_MACH:

                    return SOUND_WMACH;
                    break;

                case WEAPON_CHAIN:

                    return SOUND_WCHAIN;
                    break;

            }

        }
        int getActiveWeaponFireRate() {

            switch (activeWeapon) {

                case WEAPON_KNIFE:

                    return 66;
                    break;

                case WEAPON_PISTOL:

                    return 125;
                    break;

                case WEAPON_MACH:

                    return 75;
                    break;

                case WEAPON_CHAIN:

                    return 20;
                    break;

            }

        }
        void setActiveWeapon(int Weapon) {

            if (shooting) { return; }
            activeWeapon = hasWeapon(Weapon) ? Weapon : activeWeapon;

        }
        int getWeaponDmg(int Weapon) {

            // WEAPON_KNIFE damage resolved under attack().
            switch (Weapon) {

                case WEAPON_PISTOL:

                    return 16;
                    break;

                case WEAPON_MACH:

                    return 20;
                    break;

                case WEAPON_CHAIN:

                    return 24;
                    break;

            }

        }
        bool hitWall() {

            for (int y = 0; y < worldMapY; y++) {

                for (int x = 0; x < worldMapX; x++) {

                    cube *wall = worldMapMatrix.data[x][y];
                    // insert magic way to detect if player is aiming at a wall.
                    // (probably using ray and plane magic)
                    // return true;

                }

            }

            return false;

        }
        void attack() {

            playSound(getActiveWeaponSound(), SOURCE_PICKUP, vec3(), false, true);
            ammo = activeWeapon != WEAPON_KNIFE && ammo > 0 ? ammo - 1 : ammo;

            mob *closest = 0;
            int lastDistance = 9e4;
            for (int i = 0; i < worldEnts.size(); i++){

                if (worldEnts[i]->getHealth() <= 0 || worldEnts[i]->getType() != TYPE_NPC) { continue; }
                vec2 entPos = worldEnts[i]->getPos();
                int curDist = distance(objectPos, entPos);

                // If we're aiming within the boundaries of the sprite and the target is closesr than the previous target, it must be the target the player is shooting at.
                float angle = angBetween(getEye(), entPos - objectPos);
                if (angle < 7.f && curDist < lastDistance) {

                    lastDistance = curDist;
                    closest = worldEnts[i];

                }

            }
            if (!closest) { return; }

            // Damage the enemy.
            closest->adjHealth(-(activeWeapon == WEAPON_KNIFE && lastDistance < 1.5f ? 20 : getWeaponDmg(activeWeapon)));

        }
        void shoot() {

            if (shooting || (activeWeapon != WEAPON_KNIFE && ammo <= 0)) {

                holding = (activeWeapon == WEAPON_KNIFE || ammo > 0) ? true : false;
                return;

            }
            shooting = true;
            attack();

        }
        void think() {

            if (shooting) {

                if (activeWeapon > WEAPON_PISTOL && holding && clock() > animDelay) {

                    if (activeWeaponAnim == (activeWeapon == WEAPON_CHAIN ? 3 : 2)) { activeWeaponAnim--; }
                    else {

                        activeWeaponAnim++;
                        attack();
                        if (ammo <= 0) { holding = false; }

                    }
                    animDelay = clock() + (activeWeapon == WEAPON_CHAIN ? 20 : 100);

                }
                else if (clock() > animDelay) {

                    animDelay = clock() + 120;
                    activeWeaponAnim++;

                }
                if (activeWeaponAnim > 4) {

                    activeWeaponAnim = 0;
                    shooting = false;

                }

            }
            holding = false;

        }
        const matrix<cube*>& getMapMatrix() { return worldMapMatrix; }
        int getMapX() { return worldMapX; }
        int getMapY() { return worldMapY; }
        void reset() {

            score = 0, ammo = 16;
            mach = chain = false;
            activeWeapon = WEAPON_PISTOL;

        }
        bool isLevelComplete() { return levelComplete; }
        /*

        Will rewrite this soon, understanding of euclidian vectors increasing.

        bool canSee(mob *Mob) {

            float dist = distance(objectPos, Mob->getPos());
            ray trace(objectPos, getEye());
            for (int y = 0; y < worldMapY; y++) {

                for (int x = 0; x < worldMapX; x++) {

                    if (!worldMapMatrix.data[x][y]) { continue; }
                    cube *wall = worldMapMatrix.data[x][y];
                    vec2 wallPos = wall->getPos(), minBox = wall->getMinBox(), maxBox = wall->getMaxBox();
                    vec2 vertex1 = (wallPos + maxBox) * vec2(-1.f, 1.f), vertex2 = (wallPos + maxBox),
                         vertex3 = (wallPos + minBox), vertex4 = (wallPos + minBox) * vec2(1.f, -1.f);
                    plane collisionPlane(-trace.getDir(), vertex1, vertex2, vertex3, vertex4);
                    bool result = trace.rayPlane(collisionPlane, dist);
                    if (result) { return false; } // The ray hit something, meaning we can't see Mob. So return false.

                }

            }
            return true;

        }
        cube* hitWall() {

            ray trace(objectPos, getEye());
            for (int y = 0; y < worldMapY; y++) {

                for (int x = 0; x < worldMapX; x++) {

                    if (!worldMapMatrix.data[x][y]) { continue; }
                    cube *wall = worldMapMatrix.data[x][y];
                    vec2 wallPos = wall->getPos(), minBox = wall->getMinBox(), maxBox = wall->getMaxBox();
                    vec2 vertex1 = (wallPos + maxBox) * vec2(-1.f, 1.f), vertex2 = (wallPos + maxBox),
                         vertex3 = (wallPos + minBox), vertex4 = (wallPos + minBox) * vec2(1.f, -1.f);
                    plane collisionPlane(-trace.getDir(), vertex1, vertex2, vertex3, vertex4);
                    bool result = trace.rayPlane(collisionPlane);
                    if (result) { return wall; } // The ray hit something, meaning we can't see Mob. So return false.

                }

            }
            return 0;

        }
        */

    private:

        bool shooting, holding, levelComplete;
        unsigned int animDelay;
        int score, ammo, activeWeapon, activeWeaponAnim;
        bool pistol, mach, chain;
        const std::vector<mob*> &worldEnts;
        const int &worldMapX, &worldMapY;
        const matrix<cube*> &worldMapMatrix;

};
class pickup : public decor {

    public:

        pickup(player &Ply, int SpriteID, const vec2 &Pos, ALuint Sound):
               decor(SpriteID, Pos), ply(Ply), sound(Sound)
        {
            objectType = TYPE_PICKUP;
        }
        ~pickup() {}

        void think() {

            if (mobSpriteID == -1) { return; }

            vec2 plyPos = ply.getPos();
            if (shouldPickup() && plyPos.x > objectPos.x - 1.25f && plyPos.x < objectPos.x + 1.25f && plyPos.y > objectPos.y - 1.25f && plyPos.y < objectPos.y + 1.25f) {

                playSound(sound, SOURCE_PICKUP, vec3(), false, true);
                onPickup();
                mobSpriteID = -1;

            }

        }

    protected:

        virtual void onPickup() = 0;
        virtual bool shouldPickup() { return true; }
        ALuint sound;
        player &ply;

};
class treasure : public pickup {

    public:

        treasure(player &Ply, int SpriteID, const vec2 &Pos, int Score, ALuint Sound):
                 pickup(Ply, SpriteID, Pos, Sound), treasureScore(Score)
        {}
        ~treasure() {}

    protected:

        void onPickup() {

            ply.adjScore(treasureScore);

        }
        int treasureScore;

};
class tCross : public treasure {

    public:

        tCross(player &Ply, const vec2 &Pos):
               treasure(Ply, SPRITE_TCROSS, Pos, 100, SOUND_TCROSS)
        {}
        ~tCross() {}

};
class tCup : public treasure {

    public:

        tCup(player &Ply, const vec2 &Pos):
             treasure(Ply, SPRITE_TCUP, Pos, 500, SOUND_TCUP)
        {}
        ~tCup() {}

};
class tChest : public treasure {

    public:

        tChest(player &Ply, const vec2 &Pos):
               treasure(Ply, SPRITE_TCHEST, Pos, 1000, SOUND_TCHEST)
        {}
        ~tChest() {}

};
class tCrown : public treasure {

    public:

        tCrown(player &Ply, const vec2 &Pos):
               treasure(Ply, SPRITE_TCROWN, Pos, 5000, SOUND_TCROWN)
        {}
        ~tCrown() {}

};

class pMach : public pickup {

    public:

        pMach(player &Ply, const vec2 &Pos):
              pickup(Ply, SPRITE_PMACH, Pos, SOUND_PMACH)
        {}

    protected:

        void onPickup() {

            ply.giveWeapon(WEAPON_MACH);
            ply.setActiveWeapon(WEAPON_MACH);

        }

};
class pChain : public pickup {

    public:

        pChain(player &Ply, const vec2 &Pos):
                pickup(Ply, SPRITE_PCHAIN, Pos, SOUND_PCHAIN)
        {}

    protected:

        void onPickup() {

            ply.giveWeapon(WEAPON_CHAIN);
            ply.setActiveWeapon(WEAPON_CHAIN);

        }

};
class pAmmo : public pickup {

    public:

        pAmmo(player &Ply, const vec2 &Pos):
              pickup(Ply, SPRITE_PAMMO, Pos, SOUND_PAMMO)
        {}

    protected:

        void onPickup() {

            ply.adjAmmo(16 > (100 - ply.getAmmo()) ? (100 - ply.getAmmo()) : 16);

        }
        bool shouldPickup() { return ply.getAmmo() == 100 ? false : true; }


};

class health : public pickup {

    public:

        health(player &Ply, int SpriteID, const vec2 &Pos, int Restore, ALuint Sound):
               pickup(Ply, SpriteID, Pos, Sound), healthRestore(Restore)
        {}
        ~health() {}

    protected:

        bool shouldPickup() { return ply.getHealth() == 100 ? false : true; }
        void onPickup() {

            ply.adjHealth(healthRestore > (100 - ply.getHealth()) ? (100 - ply.getHealth()) : healthRestore);

        }
        int healthRestore;

};
class hMedkit : public health {

    public:

        hMedkit(player &Ply, const vec2 &Pos):
                health(Ply, SPRITE_HMEDKIT, Pos, 25, SOUND_HMEDKIT)
        {}
        ~hMedkit() {}

};

/*
    TODO:
        Make mr npc follow generated path.
*/
class npc : public mob {

    public:

        npc(player &Ply, int SpriteID, const vec2 &Pos, int Health, int Damage):
            mob(SpriteID, Pos, Health), npcSprite((spriteNPC*)mobSprite), npcDamage(Damage), lastHealth(Health), ply(Ply), aggro(false), alive(true), aggroRadius(4.f),
            movementSprite(2), spriteTexture(npcSprite->textures[2][0]), spriteDelay(0), deathSprite(-1)
        {
            objectType = TYPE_NPC;
        }
        void draw() {

            if (mobSpriteID == -1) { return; }
            glPushMatrix();

                glTranslatef(objectPos.x, 1.25f, objectPos.y);
                glRotatef(spriteRot, 0.f, 1.f, 0.f);

                glEnable(GL_TEXTURE_2D);

                glBindTexture(GL_TEXTURE_2D, spriteTexture);
                glBegin(GL_QUADS);

                    glNormal3f(0.f, 0.f, -1.f);
                    glTexCoord2f(0.f, 1.f);
                    glVertex3f(-1.f, 1.25f, 0.f);
                    glTexCoord2f(1.f, 1.f);
                    glVertex3f(1.f, 1.25f, 0.f);
                    glTexCoord2f(1.f, 0.f);
                    glVertex3f(1.f, -1.25f, 0.f);
                    glTexCoord2f(0.f, 0.f);
                    glVertex3f(-1.f, -1.25f, 0.f);

                glEnd();

                glDisable(GL_TEXTURE_2D);

            glPopMatrix();

        }
        bool hitWall() {

            int worldMapX = ply.getMapX(), worldMapY = ply.getMapY();
            const matrix<cube*> &worldMapMatrix = ply.getMapMatrix();

            for (int y = 0; y < worldMapY; y++) {

                for (int x = 0; x < worldMapX; x++) {

                    cube *wall = worldMapMatrix.data[x][y];

                    // insert magic way to detect if player is aiming at a wall.
                    // (probably using ray and plane magic)
                    // return true;

                }

            }

            return false;

        }
        vec2 getVel(float Dist) {

            return (getEye() * Dist);

        }
        cube* checkCollisions(float MoveVel) {

            int worldMapX = ply.getMapX(), worldMapY = ply.getMapY();
            const matrix<cube*> &worldMapMatrix = ply.getMapMatrix();
            cube* ent = 0;
            for (int y = 0; y < worldMapY; y++) {

                for (int x = 0; x < worldMapX; x++) {

                    ent = worldMapMatrix.data[x][y];
                    if (ent && ent->collidesWith(this, -getVel(MoveVel))) {

                        return ent;

                    }

                }

            }
            return 0;

        }
        bool canSeePly() {

            // Determine whether player is within the FOV of NPC.

            if (!angBetween(getEye(), ply.getPos() - objectPos)) {
                return false;
            }

            const matrix<cube*> &mapMatrix = ply.getMapMatrix();
            vec2 plyTile = ply.getTile(), objectTile = getTile(), dir = getEye(), line = objectTile, lineTile;
            while (true) {

                line+= vec2(round(dir.x), round(dir.y));
                lineTile = getTile(line);
                if (lineTile == plyTile) { return true; }
                else if (mapMatrix.data[(int)lineTile.x][(int)lineTile.y]) {

                    std::cout << lineTile.x << ", " << lineTile.y << std::endl;
                    return false;

                }
                break;

            }


            /*
            // Determine whether player is behind a wall.
            int nearest = 9e4, curDist = 0;
            cube *wall = 0, *target;
            for (int y = 0; y < 128; y++) {

                for (int x = 0; x < 128; x++) {

                    if (!ply.getMapMatrix().data[x][y]) { continue; }
                    wall = ply.getMapMatrix().data[x][y];
                    curDist = distance(objectPos, wall->getPos());
                    if (angBetween(getEye(), wall->getPos() - objectPos) < 7.f && curDist < nearest) {

                        nearest = curDist;
                        target = wall;

                    }

                }

            }
            return (nearest > distance(ply.getPos(), target->getPos()) ? false : true);*/

        }
        void think() {

            // Insert clever code which controls sprite rotation (switching) and AI/pathfinding (to player) here.
            if (!alive) { return; }
            else if (mobHealth <= 0) {

                // Play death sound.
                if (deathSprite == -1) {

                    onDeath();
                    deathSprite++;

                }
                else if (clock() > spriteDelay) {

                    spriteDelay = clock() + 100;
                    spriteTexture = npcSprite->textures[1][deathSprite];
                    deathSprite++;

                }
                else if (deathSprite > 4) { alive = false; }
                return;

            }
            else if (mobHealth != lastHealth) {

                // Switch to "taken damage" sprite and play hurt sound.
                spriteDelay = clock() + 250;
                spriteTexture = npcSprite->textures[1][7];

            }
            else if (clock() >= spriteDelay) {

                spriteDelay = clock() + 66;
                float angle = angBetweenN(getEye(), objectPos - ply.getPos());
                // Needs more negative ... well, less. Q.Q
                if (angle > 135.f) { rotationSprite = 4; }
                else if (angle > 90.f) { rotationSprite = 5; }
                else if (angle > 45.f) { rotationSprite = 6; }
                else if (angle > 0.f) { rotationSprite = 7; }
                else if (angle > -45.f) { rotationSprite = 0; }
                else if (angle > -90.f) { rotationSprite = 1; }
                else if (angle > -135.f) { rotationSprite = 2; }
                else if (angle > -180.f) { rotationSprite = 3; }
                spriteTexture = npcSprite->textures[movementSprite][rotationSprite];

            }

            // AI.
            lastHealth = mobHealth;
            std::cout << "Can see ply: " << canSeePly() << std::endl;


        }
        bool isAlive() { return alive; }

    protected:

        player &ply;
        spriteNPC *npcSprite;
        GLuint spriteTexture;
        bool aggro, alive; // aggression (seen player)
        unsigned int spriteDelay;
        int npcDamage, lastHealth, curPath, deathSprite, movementSprite, rotationSprite;
        float aggroRadius;
        virtual void onDeath() {}

};
class nGuard : public npc {

    public:

        nGuard(player &Ply, const vec2 &Pos):
               npc(Ply, SPRITE_NGUARD, Pos, 50, 15)
        {}

    protected:

        void onDeath() {

            playSound(SOUND_NGUARD_DED[rand() % 7], SOURCE_NPC, objectPos);

        }

};

#endif // MOB_HPP
