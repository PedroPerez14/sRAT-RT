#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H

/// Credits for this file: Néstor Monzón (gracias néstor)

#include <array>
#include <algorithm>

#include <glad/gl.h>
#include <GLFW/glfw3.h>

template <GLuint INTERNAL_FORMAT, GLuint FORMAT, size_t N_TEXTURES>
class GLFrameBuffer {
    /*
    OpenGL Framebuffer wrapper
    */

    GLuint m_frameBufferID;
    GLuint m_depthTextureID;
    std::array<GLuint, N_TEXTURES> m_textureIDs;

    unsigned int m_width, m_height;

public:
    GLFrameBuffer()
        : m_frameBufferID(0), m_width(0), m_height(0)
    {
    }
    
    void init(unsigned int width, unsigned int height) {
        std::cout << "[Framebuffer] initializing... ";
        m_width = width; m_height = height;
        GL_CHECK(glGenFramebuffers(1, &m_frameBufferID));
        GL_CHECK(glBindFramebuffer(GL_FRAMEBUFFER, m_frameBufferID));
        
        GLuint attachments[N_TEXTURES];
        for (size_t i = 0; i < N_TEXTURES; i++)
        {
            GL_CHECK(glGenTextures(1, &m_textureIDs[i]));
            GL_CHECK(glBindTexture(GL_TEXTURE_2D, m_textureIDs[i]));
            GL_CHECK(glTexImage2D(GL_TEXTURE_2D, 0, INTERNAL_FORMAT, width, height, 0, FORMAT, GL_FLOAT, NULL));
            GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
            GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
            GL_CHECK(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, m_textureIDs[i], 0));
            
            // GL_CHECK(glActiveTexture(GL_TEXTURE0 + i));
            // GL_CHECK(glBindTexture(GL_TEXTURE_2D, 0));

            attachments[i] = GL_COLOR_ATTACHMENT0 + i;
        }

        // - tell OpenGL which color attachments we'll use (of this framebuffer) for rendering 
        glDrawBuffers(N_TEXTURES, attachments); 

        /// DEPTH ///
        // create a renderbuffer object for depth and stencil attachment (we won't be sampling these) 
        
        ///////////////////////////////////////////////////////////////////////////glGenRenderbuffers(1, &m_depthBufferID); 
        glGenTextures(1, &m_depthTextureID);
        glBindTexture(GL_TEXTURE_2D, m_depthTextureID);
        GL_CHECK(glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, width, height, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, NULL));
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_NONE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
        glBindTexture(GL_TEXTURE_2D, 0);
        ///////////////////////////////////////////////////////////////////////////

        std::cout << "SWPTH BUFFER ID: " << m_depthTextureID << std::endl; 
        ///////////////////////////////////////////////////////////////////////////glBindRenderbuffer(GL_RENDERBUFFER, m_depthBufferID); 
        ///////////////////////////////////////////////////////////////////////////glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, m_width, m_height); // use a single renderbuffer object for both a depth AND stencil buffer. 
        /////////////////////////////////////////////////////////////////////////// glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_depthBufferID);	// now actually attach it 
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_depthTextureID, 0);

        // finally check if framebuffer is complete
        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            std::cout << "Framebuffer not complete!" << std::endl;
        else 
            std::cout << "[" << width << "x"<< height << "] done\n";
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        
    }

    GLuint getID() const { return m_frameBufferID; }

    void bind() const {
        glBindFramebuffer(GL_FRAMEBUFFER, m_frameBufferID);
    }
    
    void bindForReading() const {
        glBindFramebuffer(GL_READ_FRAMEBUFFER, m_frameBufferID);
    }

    void unbind() const {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }
    
    void clear() const {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }

    GLuint getTextureID(size_t i) const { return m_textureIDs[i]; }

    GLuint getDepthTextureID() const { return m_depthTextureID; }

    void bindTextures() const {
        for (size_t i = 0; i < N_TEXTURES; i++)
        {
            std::cout << "binding texture: " << i << " " << m_textureIDs[i] << std::endl;
            GL_CHECK(glActiveTexture(GL_TEXTURE0 + i));
            GL_CHECK(glBindTexture(GL_TEXTURE_2D, m_textureIDs[i]));
        }
    }

    void unbindTextures() const {
        for (size_t i = 0; i < N_TEXTURES; i++)
        {
            GL_CHECK(glActiveTexture(GL_TEXTURE0 + i));
            GL_CHECK(glBindTexture(GL_TEXTURE_2D, 0));
        }
    }

    void resize(unsigned int width, unsigned int height) {
        m_width = width;
        m_height = height;
        for (size_t i = 0; i < N_TEXTURES; i++)
        {
            GL_CHECK(glBindTexture(GL_TEXTURE_2D, m_textureIDs[i]));
            GL_CHECK(glTexImage2D(GL_TEXTURE_2D, 0, INTERNAL_FORMAT, m_width, m_height, 0, FORMAT, GL_FLOAT, NULL));
            //////////////////////glBindRenderbuffer(GL_RENDERBUFFER, m_depthBufferID); 
            //////////////////////glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, m_width, m_height);
        }
        glBindTexture(GL_TEXTURE_2D, m_depthTextureID);
        GL_CHECK(glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, m_width, m_height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL));
        glBindTexture(GL_TEXTURE_2D, 0);
    }
    
    bool isSameSize(GLuint w, GLuint h) const {
        return w == m_width && h == m_height;
    }

    unsigned int getWidth() const { return m_width; }
    unsigned int getHeight() const { return m_height; }

};

// RGB Framebuffer:
template <size_t N_TEXTURES>
using GLFrameBufferRGB = GLFrameBuffer<GL_RGB32F, GL_RGB, N_TEXTURES>;

// RGBA Framebuffer:
template <size_t N_TEXTURES>
using GLFrameBufferRGBA = GLFrameBuffer<GL_RGBA32F, GL_RGBA, N_TEXTURES>;

// using GLFrameBufferRGBALDR = GLFrameBuffer<GL_RGBA8, GL_RGBA, 1>;

#endif