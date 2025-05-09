// The vectrex library - copyright KAGR LLC. The use of this source code is governed by the license agreement(s) described in the "license.txt" file in this directory.


class TextureObject : public ref_object
{
    public:
    GLuint         m_textureID;   // The GL texture ID
   uint32         m_width;       // Image width
   uint32         m_height;      // Image height
   uint32         m_format;      // Image format type (GL_RGB, GL_RGBA)
   uint32         m_type;        // Data type for pixel data (GL_BYTE, ...)
   uint32         m_target;      // GL_TEXTURE_2D, ...
    void glSet(uint32 index)
    {
        if(index == 0)
            glBindTexture(GL_TEXTURE_2D, m_textureID);
        else
            glBindTexture(GL_TEXTURE0 + index, m_textureID);

        // sets this as the active texture on unit index
    }
    void release()
    {
        if(m_textureID)
        {
            glDeleteTextures(1, &m_textureID);
            m_textureID = 0;
        }
    }
    TextureObject()
    {
        m_textureID = 0;
        m_width = m_height = 0;
    }
    ~TextureObject()
    {
        if(m_textureID)
            release();
    }
    uint32 getWidth() { return m_width; }
    uint32 getHeight() { return m_height; }
};

typedef ref_ptr<TextureObject> TextureHandle;

class TextureManager
{
public:
    /**
     * Create a texture of given dimension.  Assumed that format and
     * glTexImage2D's internalFormat paramater can be the same.
     *
     * @param width
     * @param height
     * @param format
     * @param type
     * @param pixels
     *
     * @return
     */
    TextureHandle createTexture(uint32 width, uint32 height, uint32 format = GL_RGB, uint32 type = GL_UNSIGNED_BYTE, void * pixels = 0)
    {
       ref_ptr<TextureObject> texture = new TextureObject;

       glGenTextures(1, &texture->m_textureID);

       uint32 target = GL_TEXTURE_2D;

       if(!IsPow2(width) || !IsPow2(height))
          target = GL_TEXTURE_RECTANGLE_ARB;

       glBindTexture(target, texture->m_textureID);
       glTexImage2D(target, 0, format, width, height, 0, format, type, pixels);

       glTexParameteri(target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
       glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

       texture->m_width = width;
       texture->m_height = height;
       texture->m_format = format;
       texture->m_type = type;
       texture->m_target = target;

       return texture;
    }

   TextureHandle loadTexture(const char * file)
    {
       if(!file)
          return 0;

       SDL_Surface *texSurface = IMG_Load(file);
       uint8 *modPixels = 0;
       
       logf(cradle, ("Image load \"%s\" %d x %d fmt = %d", file, texSurface->w, texSurface->h, texSurface->format));
       GLuint format = 0;
       if(texSurface->format == SDL_PIXELFORMAT_RGB24)
           format = GL_RGB;
       else if(texSurface->format == SDL_PIXELFORMAT_RGBA32 || texSurface->format == SDL_PIXELFORMAT_RGBA8888)
           format = GL_RGBA;
       else if(texSurface->format == SDL_PIXELFORMAT_XRGB8888 || texSurface->format == SDL_PIXELFORMAT_ARGB8888)
       {
           uint32 *srcPixels = (uint32 *) texSurface->pixels;
           modPixels = new uint8[texSurface->w * texSurface->h * 4];
           uint8 *dstPixels = modPixels;
           for(uint32 y = 0; y < texSurface->h; y++)
           {
               for(uint32 x = 0; x < texSurface->w; x++)
               {
                   uint32 src = *srcPixels++;
                   big_endian_to_host(src);
                   dstPixels[0] = uint8(src >> 8);
                   dstPixels[1] = uint8(src >> 16);
                   dstPixels[2] = uint8(src >> 24);
                   dstPixels[3] = uint8(src);
                   dstPixels += 4;
               }
           }
           format = GL_RGBA;
       }
       ref_ptr<TextureObject> texture;
       
       if(format)
           texture = createTexture(texSurface->w, texSurface->h, format, GL_UNSIGNED_BYTE, modPixels ? modPixels : texSurface->pixels);

       if(modPixels)
           delete[] modPixels;
       SDL_DestroySurface(texSurface);
       
       return texture;
    }

};

TextureManager TX;
