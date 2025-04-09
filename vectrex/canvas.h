
struct RenderVertex2
{
    vec2 p;
};

struct RenderVertex2C
{
    vec2 p;
    ColorI c;
};

struct RenderVertex2CT
{
    vec2 p;
    ColorI c;
    vec2 t;
};

struct RenderVertex2T
{
    vec2 p;
    vec2 t;
};

struct RenderVertex2TT
{
    vec2 p;
    vec2 t1;
    vec2 t2;
};

struct RenderVertex3C
{
    vec3 p;
    ColorI c;
};

struct RenderVertex3T
{
    vec3 p;
    vec2 t;
};

struct RenderVertex3CT
{
    vec3 p;
    ColorI c;
    vec2 t;
};

struct RenderVertex3TT
{
    vec3 p;
    vec2 t1;
    vec2 t2;
};

enum BlendMode
{
    BlendModeNone,
    BlendModeBlend,
    BlendModeAddMulSrcAlpha,
    BlendModeAdd,
};

struct canvas
{
private:
    ColorI draw_color;
    SDL_FColor draw_color_sdl;
    SDL_Rect viewport;
    mat4 draw_matrix;
    array<vec2> vertex_buffer;
    array<int> index_buffer;
    bool dirty_viewport;
    bool dirty_blend;
    BlendMode blend_mode;
public:
    bool draw_outline;
    MatrixStack modelView;
    MatrixStack projection;

    enum BitmapFlipMode
    {
       BitmapFlip_None = 0,
       BitmapFlip_X    = 1 << 0,
       BitmapFlip_Y    = 1 << 1,
       BitmapFlip_XY   = BitmapFlip_X | BitmapFlip_Y
    };

    enum GFXTextureFilterType
    {
       GFXTextureFilter_FIRST = 0,
       GFXTextureFilterNone = 0,
       GFXTextureFilterPoint,
       GFXTextureFilterLinear,
       /*GFXTextureFilterAnisotropic,
       GFXTextureFilterPyramidalQuad,
       GFXTextureFilterGaussianQuad,
       GFXTextureFilter_COUNT*/
    };

    canvas() : draw_color(0,0,0,255)
    {
        draw_outline = false;
        blend_mode = BlendModeNone;
        dirty_blend = false;
        dirty_viewport = false;
        draw_matrix = mat4::identity();

    }
    
    void set_viewport(float32 x, float32 y, float32 w, float32 h)
    {
        dirty_viewport = true;
        
        viewport.x = x;
        viewport.y = y;
        viewport.w = w;
        viewport.h = h;
        //SDL_SetRenderViewport(renderer, &viewport);
        glViewport(x, y, w, h);
    }
    
    

    void color(const ColorI &_draw_color)
    {
        draw_color = _draw_color;
        const float32 scale = 1.0f / 255.0f;
        draw_color_sdl.r = draw_color.red * scale;
        draw_color_sdl.g = draw_color.green * scale;
        draw_color_sdl.b = draw_color.blue * scale;
        draw_color_sdl.a = draw_color.alpha * scale;
        glColor4ub(draw_color.red, draw_color.green, draw_color.blue, draw_color.alpha);
        
        //SDL_SetRenderDrawColor(renderer, draw_color.red, draw_color.green, draw_color.blue, draw_color.alpha);
    }
    
    void color(const Color &_draw_color, float32 _alpha = 1)
    {
        const float32 scale = 255.0f;
        draw_color.red = _draw_color.r * scale;
        draw_color.green = _draw_color.g * scale;
        draw_color.blue = _draw_color.b * scale;
        draw_color.alpha = _alpha * scale;
        draw_color_sdl.r = _draw_color.r;
        draw_color_sdl.g = _draw_color.g;
        draw_color_sdl.b = _draw_color.b;
        draw_color_sdl.a = _alpha;
        glColor4f(_draw_color.r, _draw_color.g, _draw_color.b, _alpha);
        
        //SDL_SetRenderDrawColor(renderer, draw_color.red, draw_color.green, draw_color.blue, draw_color.alpha);
    }

    void colorf(float32 _r, float32 _g, float32 _b, float32 _a = 1)
    {
        Color _c(_r, _g, _b);
        color(_c, _a);
    }
    
    void colori(uint8 _r, uint8 _g, uint8 _b, uint8 _a = 255)
    {
        ColorI c(_r, _g, _b, _a);
        color(c);
    }
    
    void clear()
    {
        //SDL_RenderClear(renderer);
        glClearColor(draw_color_sdl.r, draw_color_sdl.g, draw_color_sdl.b, draw_color_sdl.a);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }
    
    void set_blend_mode(BlendMode _mode)
    {
        if(_mode != blend_mode)
        {
            dirty_blend = true;
            blend_mode = _mode;
        }
    }

    
    void draw_prepare()
    {
        if(dirty_viewport || modelView.is_dirty() || projection.is_dirty())
        {
            float32 vw = viewport.w * 0.5;
            float32 vh = viewport.h * 0.5;
            
//            draw_matrix = mat4::translate(vec3(vw, vh, 0));
//            draw_matrix *= mat4::scale(vec3(vw, vh, 1));
            draw_matrix = projection.top();
            draw_matrix *= modelView.top();
            dirty_viewport = false;
            modelView.clear_dirty();
            projection.clear_dirty();
        }
        if(dirty_blend)
        {
            dirty_blend = false;
            switch(blend_mode)
            {
                case BlendModeNone:
                    glDisable(GL_BLEND);
                    break;
                case BlendModeBlend:
                    glEnable(GL_BLEND);
                    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
                    break;
                case BlendModeAddMulSrcAlpha:
                    glEnable(GL_BLEND);
                    glBlendFunc(GL_SRC_ALPHA, GL_ONE);
                    break;
                case BlendModeAdd:
                    glEnable(GL_BLEND);
                    glBlendFunc(GL_ONE, GL_ONE);
                    break;

            }
        }
    }

private:
    template<class V> void _enable_vertex_arrays(const V *_v, uint32 _vert_count)
    {
        vertex_buffer.setSize(_vert_count);
        for(uint32 i = 0; i < _vert_count; i++)
        {
            vec4 p(_v[i].p, 1);
            vec4 o = draw_matrix * p;
            vertex_buffer[i] = vec2(o.x/o.w, o.y/o.w);
        }
        glEnableClientState(GL_VERTEX_ARRAY);
        glVertexPointer(2, GL_FLOAT, 0, &vertex_buffer[0]);
    }
    
    void _enable_vertex_arrays(const vec2 *_v, uint32 _vert_count)
    {
        vertex_buffer.setSize(_vert_count);
        for(uint32 i = 0; i < _vert_count; i++)
        {
            vec4 p(_v[i], 1);
            vec4 o = draw_matrix * p;
            vertex_buffer[i] = vec2(o.x/o.w, o.y/o.w);
        }
        glEnableClientState(GL_VERTEX_ARRAY);
        glVertexPointer(2, GL_FLOAT, 0, &vertex_buffer[0]);
    }
    void _enable_array_properties(const vec2 *_v) {}
    void _enable_array_properties(const RenderVertex2 *_v) {}

    void _enable_array_properties(const RenderVertex2T *_v)
    {
        glEnableClientState(GL_TEXTURE_COORD_ARRAY);
        glTexCoordPointer(2, GL_FLOAT, sizeof(RenderVertex2T), &_v[0].t);
    }
    void _enable_array_properties(const RenderVertex2C *_v)
    {
        glEnableClientState(GL_COLOR_ARRAY);
        glColorPointer(4, GL_UNSIGNED_BYTE, sizeof(RenderVertex2C), &_v[0].c);
    }
    void _enable_array_properties(const RenderVertex2CT *_v)
    {
        glEnableClientState(GL_COLOR_ARRAY);
        glEnableClientState(GL_TEXTURE_COORD_ARRAY);
        glTexCoordPointer(2, GL_FLOAT, sizeof(RenderVertex2CT), &_v[0].t);
        glColorPointer(4, GL_UNSIGNED_BYTE, sizeof(RenderVertex2CT), &_v[0].c);
    }
    void _enable_array_properties(const RenderVertex3T *_v)
    {
        glEnableClientState(GL_TEXTURE_COORD_ARRAY);
        glTexCoordPointer(2, GL_FLOAT, sizeof(RenderVertex3T), &_v[0].t);
    }
    void _enable_array_properties(const RenderVertex3C *_v)
    {
        glEnableClientState(GL_COLOR_ARRAY);
        glColorPointer(4, GL_UNSIGNED_BYTE, sizeof(RenderVertex3C), &_v[0].c);
    }
    void _enable_array_properties(const RenderVertex3CT *_v)
    {
        glEnableClientState(GL_COLOR_ARRAY);
        glEnableClientState(GL_TEXTURE_COORD_ARRAY);
        glTexCoordPointer(2, GL_FLOAT, sizeof(RenderVertex3CT), &_v[0].t);
        glColorPointer(4, GL_UNSIGNED_BYTE, sizeof(RenderVertex3CT), &_v[0].c);
    }

    void _disable_vertex_arrays()
    {
        glDisableClientState(GL_VERTEX_ARRAY);
    }
    void _disable_array_properties(const vec2 *) {}
    void _disable_array_properties(const RenderVertex2 *) {}

    void _disable_array_properties(const RenderVertex2T *)
    {
        glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    }
    void _disable_array_properties(const RenderVertex2C *)
    {
        glDisableClientState(GL_COLOR_ARRAY);
    }
    void _disable_array_properties(const RenderVertex2CT *)
    {
        glDisableClientState(GL_COLOR_ARRAY);
        glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    }
    void _disable_array_properties(const RenderVertex3T *)
    {
        glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    }
    void _disable_array_properties(const RenderVertex3C *)
    {
        glDisableClientState(GL_COLOR_ARRAY);
    }
    void _disable_array_properties(const RenderVertex3CT *)
    {
        glDisableClientState(GL_COLOR_ARRAY);
        glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    }

    template<class V> void _transform_and_emit_vertex(const V *_v)
    {
        vec4 p(_v->p, 1);
        vec4 o = draw_matrix * p;
        glVertex2f(o.x/o.w, o.y/o.w);
    }
    void _transform_and_emit_vertex(const vec2 *_v)
    {
        vec4 p(*_v, 1);
        vec4 o = draw_matrix * p;
        glVertex2f(o.x/o.w, o.y/o.w);
    }
    
    void _emit_vertex_properties(const vec2 *_v) {}
    
    void _emit_vertex_properties(const RenderVertex2T *_v)
    {
        glTexCoord2f(_v->t.x, _v->t.y);
    }
    
    void _emit_vertex_properties(const RenderVertex3T *_v)
    {
        glTexCoord2f(_v->t.x, _v->t.y);
    }
    
    void _emit_vertex_properties(const RenderVertex2TT *_v)
    {
        glTexCoord2f(_v->t1.x, _v->t1.y);
        glMultiTexCoord2f(GL_TEXTURE1, _v->t2.x, _v->t2.y);
    }
    
    void _emit_vertex_properties(const RenderVertex2C *_v)
    {
        glColor4ub(_v->c.red, _v->c.green, _v->c.blue, _v->c.alpha);
    }
    
    template<class V> void _draw_gl_primitive(V *_verts, uint32 _vert_count, uint32 _gl_primitive)
    {
        draw_prepare();
        glBegin(_gl_primitive);
        for(uint32 i = 0; i < _vert_count; i++)
        {
            _emit_vertex_properties(_verts + i);
            _transform_and_emit_vertex(_verts + i);
        }
        glEnd();
    }
    template<class V> void _draw_gl_indexed_primitive(V *_verts, uint32 _vert_count, GLuint *indices, uint32 _index_count, uint32 _gl_primitive)
    {
        draw_prepare();
        _enable_vertex_arrays(_verts, _vert_count);
        _enable_array_properties(_verts);
        glDrawElements(_gl_primitive, _index_count, GL_UNSIGNED_INT, indices);
        _disable_vertex_arrays();
        _disable_array_properties(_verts);

        draw_prepare();
        glBegin(_gl_primitive);
        for(uint32 i = 0; i < _vert_count; i++)
        {
            _emit_vertex_properties(_verts + i);
            _transform_and_emit_vertex(_verts + i);
        }
        glEnd();
    }

public:
    template<class V> void draw_line(const V &v1, const V &v2)
    {
        draw_prepare();
        glBegin(GL_LINES);
        _emit_vertex_properties(&v1);
        _transform_and_emit_vertex(&v1);
        _emit_vertex_properties(&v2);
        _transform_and_emit_vertex(&v2);
        glEnd();
    }

    template<class V> void draw_line_strip(V *_verts, uint32 _vert_count)
        { _draw_gl_primitive(_verts, _vert_count, GL_LINE_STRIP); }
    
    template<class V> void draw_line_loop(V *_verts, uint32 _vert_count)
        { _draw_gl_primitive(_verts, _vert_count, GL_LINE_LOOP); }

    template<class V> void draw_lines(V *_verts, uint32 _vert_count)
        { _draw_gl_primitive(_verts, _vert_count, GL_LINES); }

    template<class V> void draw_poly(V *_verts, uint32 _vert_count)
        { _draw_gl_primitive(_verts, _vert_count, GL_TRIANGLE_FAN); }

    template<class V> void draw_quad(V *_verts)
        { _draw_gl_primitive(_verts, 4, GL_QUADS); }

    template<class V> void enable_vertex_array(V *_verts, uint32 _vert_count)
    {
        draw_prepare();
        _enable_vertex_arrays(_verts, _vert_count);
        _enable_array_properties(_verts);
    }
    
    template<class V> void disable_vertex_array(V *_verts)
    {
        _disable_vertex_arrays();
        _disable_array_properties(_verts);
    }
    
    void draw_array_triangles(uint32 _vert_count)
    {
        glDrawArrays(GL_TRIANGLES, 0, _vert_count);
    }

    void draw_array_triangle_strip(uint32 _vert_count)
    {
        glDrawArrays(GL_TRIANGLE_STRIP, 0, _vert_count);
    }
    
    void draw_array_triangles_indexed(GLuint *indices, uint32 _index_count)
    {
        glDrawElements(GL_TRIANGLES, _index_count, GL_UNSIGNED_INT, indices);
    }

    void draw_array_quads_indexed(GLuint *indices, uint32 _index_count)
    {
        glDrawElements(GL_QUADS, _index_count, GL_UNSIGNED_INT, indices);
    }

    void draw_array_line_loop_indexed(GLuint *indices, uint32 _index_count)
    {
        glDrawElements(GL_LINE_LOOP, _index_count, GL_UNSIGNED_INT, indices);
    }

    void draw_array_line_strip_indexed(GLuint *indices, uint32 _index_count)
    {
        glDrawElements(GL_LINE_STRIP, _index_count, GL_UNSIGNED_INT, indices);
    }

    // _glVertex2fXF and _glVertexXF are temporary hacks for easing the porting process
    // these functions are intended to go away once all of the glBegin/glEnd bits are removed from AC
    void _glVertex2fXF(float32 x, float32 y)
    {
        vec4 p(x, y, 0, 1);
        vec4 o = draw_matrix * p;
        glVertex2f(o.x/o.w, o.y/o.w);
    }
    void _glVertexXF(const vec2 &v)
    {
        vec4 p(v, 0, 1);
        vec4 o = draw_matrix * p;
        glVertex2f(o.x/o.w, o.y/o.w);
    }
    
    void draw_circle(const vec2& pos, float32 radius)
    {
        draw_prepare();
       glBegin(GL_LINE_LOOP);

       for(float32 theta = 0; theta < 2 * 3.1415; theta += 0.2)
           _glVertex2fXF(pos.x + cos(theta) * radius, pos.y + sin(theta) * radius);

       glEnd();
    }
    
    void draw_rect(const vec2& pos, const vec2& ext)
    {
        draw_prepare();

        glBegin(GL_LINE_LOOP);
        _glVertex2fXF(pos.x, pos.y);
        _glVertex2fXF(pos.x + ext.x, pos.y);
        _glVertex2fXF(pos.x + ext.x, pos.y + ext.y);
        _glVertex2fXF(pos.x, pos.y + ext.y);
        glEnd();
    }

    void fill_rect(const vec2& pos, const vec2& ext)
    {
        draw_prepare();
        
        glBegin(GL_POLYGON);
        _glVertex2fXF(pos.x, pos.y);
        _glVertex2fXF(pos.x + ext.x, pos.y);
        _glVertex2fXF(pos.x + ext.x, pos.y + ext.y);
        _glVertex2fXF(pos.x, pos.y + ext.y);
        glEnd();
    }

    void fill_circle(const vec2& pos, float32 radius)
    {
        draw_prepare();
        
       glBegin(GL_POLYGON);

       for(float32 theta = 0; theta < 2 * 3.1415; theta += 0.2)
           _glVertex2fXF(pos.x + cos(theta) * radius, pos.y + sin(theta) * radius);

       glEnd();
    }


    void draw_bitmap_stretch_SR( TextureHandle &texture, Point topLeft, Point topRight, Point bottomLeft, Point bottomRight, Rect srcRect, float32 depth = 0.0f, BitmapFlipMode in_flip = BitmapFlip_None, GFXTextureFilterType filter = GFXTextureFilterPoint, bool in_wrap = true)
    {
        float32 texLeft   = srcRect.min.x;
       float32 texRight  = srcRect.max.x;
        float32 texTop    = srcRect.min.y;
       float32 texBottom = srcRect.max.y;
        
       if( in_flip & BitmapFlip_X )
       {
          float32 temp = texLeft;
          texLeft = texRight;
          texRight = temp;
       }
       if( in_flip & BitmapFlip_Y )
       {
          float32 temp = texTop;
          texTop = texBottom;
          texBottom = temp;
       }
        glEnable(texture->m_target);
        glBindTexture(texture->m_target, texture->m_textureID);
        glTexParameteri(texture->m_target, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(texture->m_target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(texture->m_target, GL_TEXTURE_WRAP_S, GL_CLAMP);
        glTexParameteri(texture->m_target, GL_TEXTURE_WRAP_T, GL_CLAMP);
    /*
        switch(filter)
        {
            case GFXTextureFilterNone:
            case GFXTextureFilterPoint:
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
                break;
            case GFXTextureFilterLinear:
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                break;
        }*/
        RenderVertex3T verts[4];
        
        const float32 fillConv = 0; //mDevice->getFillConventionOffset();
       verts[0].p.set( topLeft.x     - fillConv, topLeft.y     - fillConv, depth);
        verts[0].t.set(texLeft, texTop);
        verts[1].p.set( topRight.x    - fillConv, topRight.y    - fillConv, depth);
        verts[1].t.set(texRight, texTop);
        verts[2].p.set( bottomRight.x  - fillConv, bottomRight.y  - fillConv, depth);
        verts[2].t.set(texRight, texBottom);
        verts[3].t.set(texLeft, texBottom);
        verts[3].p.set( bottomLeft.x - fillConv, bottomLeft.y - fillConv , depth);
        
        glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
        set_blend_mode(BlendModeBlend);
        draw_quad(verts);
        set_blend_mode(BlendModeNone);
        glDisable(texture->m_target);
    }
} C;
