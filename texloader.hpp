#ifndef TEXLOADER_HPP
#define TEXLOADER_HPP

#include <GL/glfw.h>
#include <cstdio>

typedef unsigned char byte;

// "that pink colour" -> RGB(152, 0, 136)

// Typical RGB helper function.
void glColour(double R, double G, double B) {

    glColor3d(R / 256.0, G / 256.0, B / 256.0);

}
void alphaImage(GLFWimage *image) {

    if (image->Format != GL_RGBA) { return; }

    // Loops through every RGB channel and checks if it is the same as "that pink colour". Setting the alpha channel of that pixel to 0.
    for (int y = 0; y < image->Height; y++) {

        for (int x = 3; x < image->Width * 4; x+= 4) {

            int i = (y * image->Width * 4) + x;
            if (image->Data[i - 3] == 152 && image->Data[i - 2] == 0 && image->Data[i - 1] == 136) { image->Data[i] = 0; }

        }

    }

}
void loadChar(char *FilePath, GLuint *Dest) {

    // Read in the data from the filepath.
    GLFWimage image;
    glfwReadImage(FilePath, &image, GLFW_NO_RESCALE_BIT);

    // Generate a new texture id.
    glGenTextures(1, Dest);
    glBindTexture(GL_TEXTURE_2D, *Dest);

    // Use nearest-neighbour interpolation.
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    alphaImage(&image);

    // Write the pixel data into RAM.
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image.Width, image.Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, (void*)image.Data);
    glfwFreeImage(&image);

}
void loadFont(char *Name, GLuint *Font) {

    char str[64];
    sprintf(str, "textures/fonts/%s/quest.tga", Name);
    loadChar(str, &(Font['?']));
    for (int i = 0; i < 256; i++) { Font[i] = Font['?']; }

    for (char i = '0'; i <= '9'; i++) {

        // Retrieve the filepath for each character.
        sprintf(str, "textures/fonts/%s/%c.tga", Name, i);
        loadChar(str, &(Font[i]));

    }

    for (char i = 'A'; i <= 'Z'; i++) {

        // Retrieve the filepath for each character.
        sprintf(str, "textures/fonts/%s/upper/%c.tga", Name, i);
        loadChar(str, &(Font[i]));

    }

    for (char i = 'a'; i <= 'z'; i++) {

        // Retrieve the filepath for each character.
        sprintf(str, "textures/fonts/%s/lower/%c.tga", Name, i);
        loadChar(str, &(Font[i]));

    }

    // Loading the other characters.
    sprintf(str, "textures/fonts/%s/space.tga", Name); // Space
    loadChar(str, &(Font[' ']));

    sprintf(str, "textures/fonts/%s/%c.tga", Name, '%'); // Percent
    loadChar(str, &(Font['%']));

    sprintf(str, "textures/fonts/%s/..tga", Name); // Fullstop
    loadChar(str, &(Font['.']));

    sprintf(str, "textures/fonts/%s/-.tga", Name); // Hyphen
    loadChar(str, &(Font['-']));

    sprintf(str, "textures/fonts/%s/,.tga", Name); // Comma
    loadChar(str, &(Font[',']));

    sprintf(str, "textures/fonts/%s/colon.tga", Name); // Colon
    loadChar(str, &(Font[':']));

    sprintf(str, "textures/fonts/%s/;.tga", Name); // Semi-Colon
    loadChar(str, &(Font[';']));

}
GLuint loadImage(char *filePath) {

    // Loads in a 32-bit tga image, replacing "that pink colour" with alpha.
    GLFWimage image;
    glfwReadImage(filePath, &image, GLFW_NO_RESCALE_BIT);

    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);


    alphaImage(&image);

    glTexImage2D(GL_TEXTURE_2D, 0, image.Format, image.Width, image.Height, 0, image.Format, GL_UNSIGNED_BYTE, (void*)image.Data);
    glfwFreeImage(&image);

    return texture;

}
int loadTexture(char *filePath, GLuint Filter = GL_NEAREST) {

    // Makes OpenGL to generate an ID for our texture.
    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    // Loads the texture into memory and applies filtering.
    glfwLoadTexture2D(filePath, GLFW_BUILD_MIPMAPS_BIT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, Filter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, Filter);

    // Returns the ID the texture has been assigned.
    return texture;

}
void loadWeapon(char *FilePath, GLuint *Dest) {

    GLFWimage image;
    glfwReadImage(FilePath, &image, GLFW_NO_RESCALE_BIT);
    glGenTextures(5, Dest);

    alphaImage(&image);

    // Place each image (every 64 pixels) into its own pixel data array.
    // pixelData[imageNum][bytes]

    // Setting the size of the array according to the width * num channels (RGBA) * height.
    byte pixelData[5][(64 * 4) * 64];

    // Let's say 'i' is (i)mage.
    for (int i = 0; i < 5; i++) {

        glBindTexture(GL_TEXTURE_2D, Dest[i]);

        // Column offset
        int offset = (64 * 4) * i;

        for (int height = 0; height < 64; height++) {

            for (int width = 0; width < (64 * 4); width++) {

                // Writes the pixels from each image (every 64 pixels) into the pixelData array.
                // pixelData[i][x, y] - where width is X and width * height is Y.
                // image.Data[offset + x, y] - where offset is every 64 pixels * num channels * image number, width is X and width * height is Y.
                pixelData[i][width + (64 * 4) * height] = image.Data[offset + width + (image.Width * 4) * height];

            }

        }

        // Use nearest-neighbour interpolation.
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

        // Tell OpenGL to create a texture out of the pixelData[i].
        glTexImage2D(GL_TEXTURE_2D, 0, image.Format, 64, 64, 0, image.Format, GL_UNSIGNED_BYTE, (void*)pixelData[i]);

    }
    glfwFreeImage(&image);

}
void loadNPC(char *FilePath, GLuint Dest[7][8]) { // This is buggy, and I need to fix it. The maths below is wrong and it's offsetting it incorrectly.

    GLFWimage image;
    glfwReadImage(FilePath, &image, GLFW_NO_RESCALE_BIT);
    alphaImage(&image);

    byte pixelData[7][8][(64 * 4) * 64];

    for (int r = 0; r < 7; r++) {

        for (int i = 0; i < 8; i++) {

            glGenTextures(1, &(Dest[r][i]));
            glBindTexture(GL_TEXTURE_2D, Dest[r][i]);

            // Column offset
            int offset = (64 * 4) * i + (8 * 64 * 4 * 64 * r); // Make sure we're on the next row. (8 columns * 64 pixels * 4 channels * row number).

            for (int height = 0; height < 64; height++) {

                for (int width = 0; width < (64 * 4); width++) {

                    pixelData[r][i][width + (64 * 4) * height] = image.Data[offset + width + (image.Width * 4) * height];

                }

            }

            // Use nearest-neighbour interpolation.
            glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

            // Tell OpenGL to create a texture out of the pixelData[r][i].
            glTexImage2D(GL_TEXTURE_2D, 0, image.Format, 64, 64, 0, image.Format, GL_UNSIGNED_BYTE, (void*)pixelData[r][i]);

        }

    }
    glfwFreeImage(&image);

}
#endif // TEXLOADER_HPP
