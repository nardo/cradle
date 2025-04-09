// The vectrex library - copyright KAGR LLC. The use of this source code is governed by the license agreement(s) described in the "license.txt" file in this directory.

struct RenderSurface : public ref_object
{
    uint32 width;
    uint32 height;
    GLuint frameBuffer;
    GLuint depthBuffer;
    GLuint stencilBuffer;
    uint32 format;
    bool has_zbuffer;
    bool has_stencil;
    
    TextureHandle texture;
    
    RenderSurface(uint32 _width, uint32 _height, uint32 _format = GL_RGB, bool _has_zbuffer = true, bool _has_stencil = false)
    {
        frameBuffer = 0;
        depthBuffer = 0;
        stencilBuffer = 0;
        width = _width;
        height = _height;
        format = _format;
        has_zbuffer = _has_zbuffer;
        has_stencil = _has_stencil;
        allocBuffers();
    }
    
    ~RenderSurface()
    {
        freeBuffers();
    }
    
    void resize(uint32 _w, uint32 _h)
    {
        if(_w != width || _h != height)
        {
            freeBuffers();
            width = _w;
            height = _h;
            allocBuffers();
        }
    }
    
    void allocBuffers()
    {
        texture = A.TX.createTexture(width, height, format);
        glGenFramebuffers(1, &frameBuffer);
        glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);

        if(has_zbuffer)
        {
            glGenRenderbuffers(1, &depthBuffer);
            glBindRenderbuffer(GL_RENDERBUFFER, depthBuffer);
            glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
            glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthBuffer);
        }
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, texture->m_target, texture->m_textureID, 0);
        GLenum status;
        
        status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
        
        if(status != GL_FRAMEBUFFER_COMPLETE)
        {
            freeBuffers();
        }
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }
        
    void freeBuffers()
    {
        if(depthBuffer)
            glDeleteRenderbuffers(1, &depthBuffer);
        if(frameBuffer)
            glDeleteFramebuffers(1, &frameBuffer);
        depthBuffer = frameBuffer = 0;
        texture = 0;
    }
    
    bool is_valid()
    {
        return frameBuffer != 0;
    }
    
    void activate()
    {
        glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
    }
    
    static void deactivate()
    {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }
    
    /*
    void drawFullscreen(Texture * texture)
    {
       glEnable(texture->m_target);
       glBindTexture(texture->m_target, texture->m_textureID);

       glBegin(GL_QUADS);

       float32 width;
       float32 height;

       if(texture->m_target == GL_TEXTURE_RECTANGLE_ARB)
       {
          width = texture->m_width;
          height = texture->m_height;
       }
       else
          width = height = 1.f;

       glTexCoord2f(0, 0);
       glVertex3f(0, 1, -1);
       glTexCoord2f(0, height);
       glVertex3f(0, 0, -1);
       glTexCoord2f(width, height);
       glVertex3f(1, 0, -1);
       glTexCoord2f(width, 0);
       glVertex3f(1, 1, -1);

       glEnd();
       glDisable(texture->m_target);
    }*/
};

typedef ref_ptr<RenderSurface> RenderSurfaceHandle;

