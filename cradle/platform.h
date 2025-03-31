// The cradle library - copyright KAGR LLC. The use of this source code is governed by the license agreement(s) described in the "license.txt" file in this directory.

enum RMatrixMode {
    RProjection = 0,
    RModelView = 1,
};

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

struct CorePlatform
{
private:
    SDL_GLContext gl_context;
    SDL_Window *window;
    SDL_Renderer *renderer;
    ColorI draw_color;
    SDL_FColor draw_color_sdl;
    SDL_Rect viewport;
    mat4 draw_matrix;
    array<vec2> vertex_buffer;
    array<int> index_buffer;
    bool dirty_viewport;
    bool dirty_blend;
    BlendMode blend_mode;
    cradle_journal *_journal;
    
    struct GamepadRef
    {
        SDL_Gamepad *gamepad;
        uint32 gamepad_id;
    };
    
    array<GamepadRef> gamepads;
public:
    bool fullscreen;
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

    CorePlatform() : draw_color(0,0,0,255)
    {
        _journal = NULL;
        draw_outline = false;
        blend_mode = BlendModeNone;
        dirty_blend = false;
        dirty_viewport = false;
        fullscreen = false;
        window = 0;
        renderer = 0;
        draw_matrix = mat4::identity();

        gl_context = NULL;
    }
    bool init(const char *window_name, bool _fullscreen, cradle_journal *j)
    {
        _journal = j;
#if defined (PLATFORM_WEB)
        if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMEPAD)) {
            return false;
        }
        if(!SDL_GL_LoadLibrary(NULL))
        {
            logf(cradle, ("Error loading GL library."));
            return false;
        }
        window = SDL_CreateWindow("Athena Core", A.UI.windowWidth, A.UI.windowHeight, SDL_WINDOW_OPENGL);
        //SDL_SetWindowPosition(A.G.window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
        gl_context = SDL_GL_CreateContext(window);
        if (gl_context == nullptr)
        {
            logf(cradle, ("Error: SDL_GL_CreateContext(): %s\n", SDL_GetError()));
            return SDL_APP_FAILURE;
        }
#else
        SDL_SetHint(SDL_HINT_JOYSTICK_HIDAPI, "1");
        SDL_SetHint(SDL_HINT_JOYSTICK_ENHANCED_REPORTS, "auto");
        SDL_SetHint(SDL_HINT_JOYSTICK_HIDAPI_STEAM, "1");
        SDL_SetHint(SDL_HINT_JOYSTICK_ROG_CHAKRAM, "1");
        SDL_SetHint(SDL_HINT_JOYSTICK_ALLOW_BACKGROUND_EVENTS, "1");
        SDL_SetHint(SDL_HINT_JOYSTICK_LINUX_DEADZONES, "1");
        if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK | SDL_INIT_GAMEPAD))
        {
            logf(cradle, ("Error: SDL/GL init failed: %s.", SDL_GetError()));
            return false;
        }
        if(SDL_AddGamepadMappingsFromFile("gamecontrollerdb.txt") == -1){
            logf(cradle, ("Error: SDL Gamepad Mappings failed: %s.", SDL_GetError()));
        }
        SDL_SetGamepadEventsEnabled(true);


        if(!SDL_GL_LoadLibrary(NULL))
        {
            logf(cradle, ("Error loading GL library."));
            return false;
        }
        if((window = SDL_CreateWindow(window_name, A.UI.windowWidth, A.UI.windowHeight, SDL_WINDOW_OPENGL|SDL_WINDOW_RESIZABLE)) == NULL || (gl_context = SDL_GL_CreateContext(window)) == nullptr)
        {
            logf(cradle, ("Error: SDL/GL init failed: %s.", SDL_GetError()));
            return false;
        }
        gl_context = SDL_GL_CreateContext(window);
        if (gl_context == nullptr)
        {
            logf(cradle, ("Error: SDL_GL_CreateContext(): %s\n", SDL_GetError()));
            return SDL_APP_FAILURE;
        }
        SDL_SetWindowPosition(window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
        SDL_GL_SetSwapInterval(1); // Enable vsync
        SDL_SetWindowResizable(window, true);
        SDL_ShowWindow(window);
        fullscreen = _fullscreen;
        if (_fullscreen)
            SDL_SetWindowFullscreen(window, true);
#endif
        loadGLExtensions();
        SDL_GL_MakeCurrent(window, gl_context);
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glLineWidth(2);
        
        int w, h;
        get_canvas_size(w, h);
        A.UI.windowWidth = w;
        A.UI.windowHeight = h;

        return true;
    }
    
    void set_mouse_lock(bool lock)
    {
        SDL_SetWindowRelativeMouseMode(window, lock);
    }
    
    SDL_AppResult process_event(SDL_Event *event)
    {
        switch (event->type)
        {
            case SDL_EVENT_QUIT:
                return SDL_APP_SUCCESS;
            case SDL_EVENT_KEY_DOWN:
            case SDL_EVENT_KEY_UP:
                _journal->call(&cradle_journal::key, event->key.key, event->type == SDL_EVENT_KEY_DOWN, event->key.mod);
                break;
            case SDL_EVENT_MOUSE_MOTION:
                if(event->motion.state & SDL_BUTTON_LMASK)
                    _journal->call(&cradle_journal::mouseDragged, event->motion.x, event->motion.y, event->motion.xrel, event->motion.yrel);
                else
                    _journal->call(&cradle_journal::mouseMoved, event->motion.x, event->motion.y, event->motion.xrel, event->motion.yrel);
                break;
            case SDL_EVENT_MOUSE_BUTTON_DOWN:
            case SDL_EVENT_MOUSE_BUTTON_UP:
                _journal->call(&cradle_journal::mouseButton, event->button.button, event->type == SDL_EVENT_MOUSE_BUTTON_DOWN, event->button.x, event->button.y);
                break;
            case SDL_EVENT_GAMEPAD_ADDED:
            {
                uint32 i;
                for(i = 0; i < gamepads.size(); i++)
                {
                    if(gamepads[i].gamepad_id == event->gdevice.which)
                        return SDL_APP_CONTINUE;
                }
                GamepadRef g;
                g.gamepad = SDL_OpenGamepad(event->gdevice.which);
                g.gamepad_id = event->gdevice.which;
                gamepads.push_back(g);
                
                const char *gamepad_name = SDL_GetGamepadName(g.gamepad);
                SDL_GamepadButtonLabel bl_south = SDL_GetGamepadButtonLabel(g.gamepad, SDL_GAMEPAD_BUTTON_SOUTH);
                SDL_GamepadButtonLabel bl_east = SDL_GetGamepadButtonLabel(g.gamepad, SDL_GAMEPAD_BUTTON_EAST);
                SDL_GamepadButtonLabel bl_west = SDL_GetGamepadButtonLabel(g.gamepad, SDL_GAMEPAD_BUTTON_WEST);
                SDL_GamepadButtonLabel bl_north = SDL_GetGamepadButtonLabel(g.gamepad, SDL_GAMEPAD_BUTTON_NORTH);

                char *mapping;
                mapping = SDL_GetGamepadMapping(g.gamepad);
                string gp_name(gamepad_name);
                string gp_mapping(mapping);

                _journal->call(&cradle_journal::gamepad_added, g.gamepad_id, gp_name, gp_mapping, uint32(bl_south), uint32(bl_east), uint32(bl_west), uint32(bl_north));
                break;
            }
            case SDL_EVENT_GAMEPAD_REMOVED:
            {
                for(uint32 i = 0; i < gamepads.size(); i++)
                {
                    if(event->gdevice.which == gamepads[i].gamepad_id)
                    {
                        SDL_CloseGamepad(gamepads[i].gamepad);
                        gamepads.erase(i);
                        _journal->call(&cradle_journal::gamepad_removed, event->gdevice.which);
                        break;
                    }
                }
                break;
            }
            case SDL_EVENT_GAMEPAD_BUTTON_DOWN:
                _journal->call(&cradle_journal::gamepad_buttondown, event->gdevice.which, event->gbutton.button);
                break;
            case SDL_EVENT_GAMEPAD_BUTTON_UP:
                _journal->call(&cradle_journal::gamepad_buttonup, event->gdevice.which, event->gbutton.button);
                break;
            case SDL_EVENT_GAMEPAD_AXIS_MOTION:
            {
                int32 axis = event->gaxis.axis;
                _journal->call(&cradle_journal::gamepad_axis, event->gdevice.which, axis, event->gaxis.value < 0 ? float32(event->gaxis.value)/32768.0f : float32(event->gaxis.value)/32767.0f);
                break;
            }
            case SDL_EVENT_WINDOW_RESIZED:
                {
                int w = event->window.data1;
                int h = event->window.data2;
                //logf(cradle, ("resize %d %d", w, h));
                _journal->call(&cradle_journal::windowResized, w, h);
                }
                break;
            case SDL_EVENT_JOYSTICK_AXIS_MOTION:
            case SDL_EVENT_JOYSTICK_BUTTON_DOWN:
            case SDL_EVENT_JOYSTICK_BUTTON_UP:
            case SDL_EVENT_JOYSTICK_HAT_MOTION:
            case SDL_EVENT_GAMEPAD_REMAPPED:
                //logf(cradle, ("GP event: %d", event->type));

                break;

        }
        if(A.G.quit)
            return SDL_APP_SUCCESS;
        else
            return SDL_APP_CONTINUE;
    }

    void shutdown()
    {
        if(window)
            SDL_DestroyWindow(window);
    }

    void get_canvas_size(int &_w, int &_h)
    {
        if(!window)
            _w = _h = 0;
        else
            SDL_GetWindowSize(window, &_w, &_h);
    }
    
    bool is_fullscreen()
    {
        return fullscreen;
    }
    
    bool set_fullscreen(bool _fullscreen)
    {
        if(!window)
            return false;
        
        fullscreen = _fullscreen;
        SDL_SetWindowFullscreen(window, fullscreen);
        if (!fullscreen)
            SDL_SetWindowResizable(window, true);
        return true;
    }
    
    void present()
    {
        if(renderer)
            SDL_RenderPresent(renderer);
        else if(window)
            SDL_GL_SwapWindow(window);
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
    
    template<class V> void draw_triangles(V *_verts, uint32 _vert_count)
    {
        draw_prepare();
        _enable_vertex_arrays(_verts, _vert_count);
        _enable_array_properties(_verts);
        glDrawArrays(GL_TRIANGLES, 0, _vert_count);
        _disable_vertex_arrays();
        _disable_array_properties(_verts);
    }

    template<class V> void draw_triangle_strip(V *_verts, uint32 _vert_count)
    {
        draw_prepare();
        _enable_vertex_arrays(_verts, _vert_count);
        _enable_array_properties(_verts);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, _vert_count);
        _disable_vertex_arrays();
        _disable_array_properties(_verts);
    }
    
    template<class V> void draw_triangles_indexed(V *_verts, uint32 _vert_count, GLuint *indices, uint32 _index_count)
    {
        draw_prepare();
        _enable_vertex_arrays(_verts, _vert_count);
        _enable_array_properties(_verts);
        glDrawElements(GL_TRIANGLES, _index_count, GL_UNSIGNED_INT, indices);
        _disable_vertex_arrays();
        _disable_array_properties(_verts);
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


    void draw_bitmap_stretch_SR( TextureHandle &texture, Point topLeft, Point topRight, Point bottomLeft, Point bottomRight, Rect srcRect, BitmapFlipMode in_flip = BitmapFlip_None, GFXTextureFilterType filter = GFXTextureFilterPoint, bool in_wrap = true)
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
        RenderVertex2T verts[4];
        
        const float32 fillConv = 0; //mDevice->getFillConventionOffset();
       verts[0].p.set( topLeft.x     - fillConv, topLeft.y     - fillConv);
        verts[0].t.set(texLeft, texTop);
        verts[1].p.set( topRight.x    - fillConv, topRight.y    - fillConv);
        verts[1].t.set(texRight, texTop);
        verts[2].p.set( bottomRight.x  - fillConv, bottomRight.y  - fillConv);
        verts[2].t.set(texRight, texBottom);
        verts[3].t.set(texLeft, texBottom);
        verts[3].p.set( bottomLeft.x - fillConv, bottomLeft.y - fillConv );
        
        glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
        set_blend_mode(BlendModeBlend);
        draw_quad(verts);
        set_blend_mode(BlendModeNone);
        glDisable(texture->m_target);
    }
    
} P;
