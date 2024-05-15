#ifndef _UTILS_H_
#define _UTILS_H_

#include <string>
#include <iostream>

#include <glad/gl.h>
#include <sRAT-RT/stb_image.h>
#include <sRAT-RT/gl_check.h>

inline unsigned int texture_from_file(const std::string& full_path)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    unsigned char* data = stbi_load(full_path.c_str(), &width, &height, &nrComponents, 0);
    if (data)
    {
        GLenum format;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;
        
        GL_CHECK(glBindTexture(GL_TEXTURE_2D, textureID));
        GL_CHECK(glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data));
        GL_CHECK(glGenerateMipmap(GL_TEXTURE_2D));

        GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT));
        GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT));
        GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR));
        GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));

    }
    else
    {
        std::cout << "Failed to load texture: " << full_path << std::endl;
    }
    stbi_image_free(data);

    return textureID;
}

#endif