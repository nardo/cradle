// The cradle library - copyright KAGR LLC. The use of this source code is governed by the license agreement(s) described in the "license.txt" file in this directory.

#define DefaultLineWidth 2.0f
class UserInterface;

struct Gamepad
{
    enum
    {
        InvalidGamepadId = 0x7FFFFFFF,
    };
    
    uint32 gamepad_id;
    string name;
    string mapping;
    uint32 label_south;
    uint32 label_east;
    uint32 label_west;
    uint32 label_north;
};

struct UIGlobals
{
    int32 windowWidth;
    int32 windowHeight;
    int32 canvasWidth;
    int32 canvasHeight;
    int32 vertMargin;
    int32 horizMargin;
    UserInterface *current;
    uint32 keyModifierState;
    array<Gamepad> gamepads;
    uint32 gamepadId;
    bool dropShadow;
    uint32 lastFrameStep;
    RenderSurfaceHandle screenSurface;
    RenderSurfaceHandle effectsSurface;
    SurfaceDeformer screenDeformer;

    UIGlobals()
    {
        lastFrameStep = 0;
        
        current = NULL;
#ifdef PLATFORM_XBOX
        horizMargin = 50;
        vertMargin = 38;
#else
        horizMargin = 15;
        vertMargin = 15;
#endif
        gamepadId = Gamepad::InvalidGamepadId;
        keyModifierState = 0;
        canvasWidth = 800;
        canvasHeight = 600;
        windowWidth = 1600;
        windowHeight = 1200;
        dropShadow = false;
    }
    void init()
    {
        screenSurface = new RenderSurface(64, 64);
        effectsSurface = new RenderSurface(64, 64);
    }
    bool gamepadEnabled()
    {
        return gamepadId != Gamepad::InvalidGamepadId;
    }
    Gamepad *getEnabledGamepad()
    {
        for(uint32 i = 0; i < gamepads.size(); i++)
        {
            if(gamepads[i].gamepad_id == gamepadId)
                return &gamepads[i];
        }
        return 0;
    }
    void toggleFullscreen()
    {
        bool _new_mode = !A.P.is_fullscreen();
        A.P.set_fullscreen(_new_mode);
        int _w, _h;
        A.P.get_canvas_size(_w, _h);
        windowWidth = _w;
        windowHeight = _h;
    }
} UI;

class UserInterface
{
public:
    static bool isShiftKeyDown()
    {
        return A.UI.keyModifierState & SDL_KMOD_SHIFT;
    }
    
    static bool isCtrlKeyDown()
    {
        return A.UI.keyModifierState & SDL_KMOD_CTRL;
    }
    
    static bool isAltKeyDown()
    {
        return A.UI.keyModifierState & SDL_KMOD_ALT;
    }

    virtual void render() {}
    virtual void idle(uint32 timeDelta) {}
    virtual void onActivate() {}
    virtual void onDeactivate() {}
    virtual void onMouseDown(int32 x, int32 y) {}
    virtual void onMouseUp(int32 x, int32 y) {}
    virtual void onRightMouseDown(int32 x, int32 y) {}
    virtual void onRightMouseUp(int32 x, int32 y) {}
    
    virtual void onMouseMoved(int32 x, int32 y, float32 rx, float32 ry) {}
    virtual void onMouseDragged(int32 x, int32 y, float32 rx, float32 ry) {}
    virtual void onKeyDown(uint32 key) {}
    virtual void onKeyUp(uint32 key) {}
    virtual void onSpecialKeyDown(uint32 key) {}
    virtual void onSpecialKeyUp(uint32 key) {}
    virtual void onModifierKeyDown(uint32 key) {}
    virtual void onModifierKeyUp(uint32 key) {}
    virtual void onGamepadButtonDown(uint32 buttonIndex)
    {
        switch(buttonIndex)
        {
            case SDL_GAMEPAD_BUTTON_SOUTH:
            case SDL_GAMEPAD_BUTTON_START:
                onKeyDown('\r');
                break;
            case SDL_GAMEPAD_BUTTON_EAST:
            case SDL_GAMEPAD_BUTTON_BACK:
                onKeyDown(27);
                break;
            case SDL_GAMEPAD_BUTTON_DPAD_UP:
                onSpecialKeyDown(SDLK_UP);
                break;
            case SDL_GAMEPAD_BUTTON_DPAD_DOWN:
                onSpecialKeyDown(SDLK_DOWN);
                break;
            case SDL_GAMEPAD_BUTTON_DPAD_LEFT:
                onSpecialKeyDown(SDLK_LEFT);
                break;
            case SDL_GAMEPAD_BUTTON_DPAD_RIGHT:
                onSpecialKeyDown(SDLK_RIGHT);
                break;
        }
    }
    virtual void onGamepadButtonUp(uint32 buttonIndex)
    {
        switch(buttonIndex)
        {
            case SDL_GAMEPAD_BUTTON_SOUTH:
            case SDL_GAMEPAD_BUTTON_START:
                onKeyUp('\r');
                break;
            case SDL_GAMEPAD_BUTTON_EAST:
            case SDL_GAMEPAD_BUTTON_BACK:
                onKeyUp(27);
                break;
            case SDL_GAMEPAD_BUTTON_DPAD_UP:
                onSpecialKeyUp(SDLK_UP);
                break;
            case SDL_GAMEPAD_BUTTON_DPAD_DOWN:
                onSpecialKeyUp(SDLK_DOWN);
                break;
            case SDL_GAMEPAD_BUTTON_DPAD_LEFT:
                onSpecialKeyUp(SDLK_LEFT);
                break;
            case SDL_GAMEPAD_BUTTON_DPAD_RIGHT:
                onSpecialKeyUp(SDLK_RIGHT);
                break;
        }
    }
    virtual void onControllerAxis(uint32 axis, float32 value)
    {
        
    }
    virtual void onGamepadConnected(uint32 gamepad_id) {}
    virtual void onGamepadDisconnected(uint32 gamepad_id) {}

    static void getViewport(float32 &x, float32 &y, float32 &vw, float32 &vh)
    {
        x = 0;
        y = 0;
        vw = A.UI.windowWidth;
        vh = A.UI.windowHeight;
        float32 aspect = float32(A.UI.windowWidth) / float32(A.UI.windowHeight);
        float32 targetAspect = 800.0f / 600.0f;

        if (aspect > targetAspect)
        {
            // the window is wider rather than taller than ideal
            vw = targetAspect * float32(A.UI.windowHeight);
            x = (float32(A.UI.windowWidth) - vw) * 0.5;
        }
        else if (aspect < targetAspect)
        {
            vh = float32(A.UI.windowWidth) / targetAspect;
            y = (float32(A.UI.windowHeight) - vh) * 0.5;
        }
    }
    static void renderCurrent(uint32 integerTime)
    {
        A.UI.lastFrameStep = integerTime;
        A.C.set_viewport(0, 0, A.UI.windowWidth, A.UI.windowHeight);
        int c = 0;
        A.C.color(ColorI(c,c,c,255));
        A.C.clear();

       float32 x, y, vw, vh;
        uint32 vw_i, vh_i;
       getViewport(x, y, vw, vh);
        vw_i = uint32(vw);
        vh_i = uint32(vh);
        if(vw_i && vh_i)
        {
            A.UI.screenSurface->resize(vw_i, vh_i);
            A.UI.effectsSurface->resize(vw_i, vh_i);
            A.UI.screenDeformer.updateScreenExtent(vw_i, vh_i);
        }
        A.C.set_viewport(x, y, vw, vh);
        
        float32 projFOV = 60.f;
        float32 projNear = 100.f;  // Should adjust to actual screen dimension
        float32 projFar = 5000.f;  // ""

        double left, right;
        double bottom, top;
        top = tan(DegToRad(projFOV * 0.5f)) * projNear;
        double aspect = double(A.UI.canvasWidth) / double(A.UI.canvasHeight);
        bottom = -top;
        left = aspect * bottom;
        right = aspect * top;

        float32 projOffset = ((A.UI.canvasWidth * 0.5f) / aspect) / tan(DegToRad(projFOV * 0.5));
        mat4 gl_proj = mat4::frustum(left, right, bottom, top, projNear, projFar);
        
        A.C.projection.set(gl_proj);
                           
        setModelView();
        A.C.colorf(1,1,1);

       if(A.UI.current)
          A.UI.current->render();
    }
    
    static void setModelView()
    {
        float32 projFOV = 60.f;

        double aspect = double(A.UI.canvasWidth) / double(A.UI.canvasHeight);
        float32 projOffset = ((A.UI.canvasWidth * 0.5f) / aspect) / tan(DegToRad(projFOV * 0.5));
        A.C.modelView.identity();
        A.C.modelView.scale(1.f,-1.f,1.f);
        A.C.modelView.translate(vec3(-(A.UI.canvasWidth * 0.5f), -(A.UI.canvasHeight * 0.5f), -projOffset));
    }

    void activate()
    {
        if(A.UI.current)
            A.UI.current->onDeactivate();
       A.UI.current = this;
       onActivate();
    }
    
    static void drawString(int32 x, int32 y, int32 size, const char *string)
    {
#ifndef AC_DEDICATED
       float32 scaleFactor = size / 120.0f;
        A.C.modelView.push();
        A.C.modelView.translate(vec3(x, y + size, 0));
        A.C.modelView.scale(vec3(scaleFactor, -scaleFactor, 1));
        float32 offset = 0;
       for(int32 i = 0; string[i]; i++)
           offset += StrokeFont::strokeCharacter(string[i], offset);
        A.C.modelView.pop();
#endif
    }
    static void drawRightString(int32 x, int32 y, int32 size, const char *string)
    {
        int32 rx = int32( x - getStringWidth(size, string));
        drawString(rx, y, size, string);
    }

    static void drawCenteredString(int32 y, int32 size, const char *string)
    {
       int32 x = int32( (A.UI.canvasWidth - getStringWidth(size, string)) / 2);
       drawString(x, y, size, string);
    }

    static void drawStringf(int32 x, int32 y, int32 size, const char *format, ...)
    {
       va_list args;
       va_start(args, format);
       char buffer[2048];

       vsnprintf(buffer, sizeof(buffer), format, args);
       drawString(x, y, size, buffer);
    }

    static void drawCenteredStringf(int32 y, int32 size, const char *format, ...)
    {
       va_list args;
       va_start(args, format);
       char buffer[2048];

       vsnprintf(buffer, sizeof(buffer), format, args);
       drawCenteredString(y, size, buffer);
    }

    static uint32 getStringWidth(int32 size, const char *string, uint32 len = 0)
    {
       int32 width = 0;
#ifndef AC_DEDICATED
       if(!len)
          len = (uint32) strlen(string);
       while(len--)
       {
          width += StrokeFont::strokeWidth( *string);
          string++;
       }
#endif
       return uint32((width * size) / 120.0f);

    }
    static float32 getCharWidth(int32 size, char c)
    {
       int32 width = 0;
#ifndef AC_DEDICATED
       width = StrokeFont::strokeWidth(c);
#endif
       return (width * size) / 120.0f;
    }
};
