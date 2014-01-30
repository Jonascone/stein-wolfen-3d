#ifndef TEXT_HPP
#define TEXT_HPP

#include "texloader.hpp"

struct drawnText {

    drawnText():
              textureID(0)
    {}
    const GLuint textureID;

};

void writeString(float x, float y, const std::string &string)

#endif // TEXT_HPP
