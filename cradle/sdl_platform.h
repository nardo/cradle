// The cradle library - copyright KAGR LLC. The use of this source code is governed by the license agreement(s) described in the "license.txt" file in this directory.


struct sdl_platform
{
private:
    SDL_GLContext gl_context;
    SDL_Window *window;
    SDL_Renderer *renderer;
    cradle_journal *_journal;
    
    struct GamepadRef
    {
        SDL_Gamepad *gamepad;
        uint32 gamepad_id;
    };
    
    array<GamepadRef> gamepads;
public:
    bool fullscreen;
    
    sdl_platform()
    {
        _journal = NULL;
        fullscreen = false;
        window = 0;
        renderer = 0;

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
    
} P;
