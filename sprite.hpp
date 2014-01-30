#ifndef SPRITE_HPP
#define SPRITE_HPP

#include <vector>

int SPRITE_TCROSS = -1, SPRITE_TCUP = -1, SPRITE_TCHEST = -1, SPRITE_TCROWN = -1,
    SPRITE_HMEDKIT = -1,
    SPRITE_PMACH = -1, SPRITE_PCHAIN = -1, SPRITE_PAMMO = -1,
    SPRITE_NGUARD = -1;

struct sprite {

    sprite(char *Texture, bool GlobalRotate = true):
           globalRotate(GlobalRotate)
    {
        texture = (Texture ? loadImage(Texture) : -1);
    }
    GLuint texture;
    bool globalRotate;

};
struct spriteNPC : public sprite {

    spriteNPC(char *FilePath):
              sprite(0, false)
    {
        loadNPC(FilePath, textures);
    }
    GLuint textures[7][8];

};
float spriteRot = 0.f;

std::vector<sprite*> sprites;

#endif // SPRITE_HPP
