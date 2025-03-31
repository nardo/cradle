// CradleTest - copyright KAGR LLC. The use of this source code is governed by the license agreement(s) described in the "license.txt" file in this directory.

struct cradle_test_journal : public cradle_journal
{
    bool quit;
    
    cradle_test_journal()
    {
        quit = false;
        register_callable(&cradle_test_journal::idle);
        register_callable(&cradle_test_journal::init);

        /*
        register_callable(&cradle_test_journal::windowResized);
        register_callable(&cradle_test_journal::mouseDragged);
        register_callable(&cradle_test_journal::mouseMoved);
        register_callable(&cradle_test_journal::key);
        register_callable(&cradle_test_journal::mouseButton);
        register_callable(&cradle_test_journal::gamepad_added);
        register_callable(&cradle_test_journal::gamepad_removed);
        register_callable(&cradle_test_journal::gamepad_buttonup);
        register_callable(&cradle_test_journal::gamepad_buttondown);
        register_callable(&cradle_test_journal::gamepad_axis);
        register_callable(&cradle_test_journal::idle);
        register_callable(&cradle_test_journal::init);*/
    }
    void windowResized(int32 newWidth, int32 newHeight)
    {
        A.UI.windowWidth = newWidth;
        A.UI.windowHeight = newHeight;
    }
    
    void mouseDragged(int32 x, int32 y, float32 xRel, float32 yRel)
    {
        if(A.UI.current)
            A.UI.current->onMouseDragged(x, y, xRel, yRel);
    }
    
    void mouseMoved(int32 x, int32 y, float32 xRel, float32 yRel)
    {
        if(A.UI.current)
            A.UI.current->onMouseMoved(x, y, xRel, yRel);
    }
    
    void key(uint32 key, bool down, uint32 modState)
    {
        A.UI.keyModifierState = modState;
        UserInterface *ui = A.UI.current;
        if(!ui)
            return;
        
        if(down)
        {
            if(key == SDLK_UP || key == SDLK_DOWN || key == SDLK_LEFT || key == SDLK_RIGHT)
                ui->onSpecialKeyDown(key);
            else if(key == SDLK_LSHIFT || key == SDLK_RSHIFT)
                ui->onModifierKeyDown(0);
            else if(key == SDLK_LCTRL || key == SDLK_RCTRL)
                ui->onModifierKeyDown(1);
            else if(key == SDLK_LALT || key == SDLK_RALT)
                ui->onModifierKeyDown(2);
            else
            {
                if(key == '\r' && (modState & SDL_KMOD_ALT))
                    A.UI.toggleFullscreen();
                else
                {
                    if(modState & SDL_KMOD_SHIFT)
                        key = toupper(key);
                    ui->onKeyDown(key);
                }
            }
        }
        else
        {
            if(key == SDLK_UP || key == SDLK_DOWN || key == SDLK_LEFT || key == SDLK_RIGHT)
                ui->onSpecialKeyUp(key);
            else if(key == SDLK_LSHIFT || key == SDLK_RSHIFT)
                ui->onModifierKeyUp(0);
            else if(key == SDLK_LCTRL || key == SDLK_RCTRL)
                ui->onModifierKeyUp(1);
            else if(key == SDLK_LALT || key == SDLK_RALT)
                ui->onModifierKeyUp(2);
            else
                ui->onKeyUp(key);
        }
    }
    
    void mouseButton (int32 button, bool buttonDown, int32 x, int32 y)
    {
        static bool mouseState[2] = { false, false};
        if(!A.UI.current)
            return;
        
        if(button == SDL_BUTTON_LEFT)
        {
            if(!buttonDown && mouseState[0])
            {
                A.UI.current->onMouseUp(x, y);
                mouseState[0] = false;
            }
            else
            {
                mouseState[0] = true;
                A.UI.current->onMouseDown(x, y);
            }
        }
        else if(button == SDL_BUTTON_RIGHT)
        {
            if(!buttonDown && !mouseState[1])
            {
                A.UI.current->onRightMouseUp(x, y);
                mouseState[1] = false;
            }
            else
            {
                mouseState[1] = true;
                A.UI.current->onRightMouseDown(x, y);
            }
        }
    }
    
    void gamepad_added (uint32 gamepad_id, string name, string mapping, uint32 label_south, uint32 label_east, uint32 label_west, uint32 label_north)
    {
        logf(LogGamepad, ("gamepad added %d %s", gamepad_id, name.c_str()));
        A.UI.gamepads.increment();
        Gamepad &g = A.UI.gamepads.last();
        g.name = name;
        g.gamepad_id = gamepad_id;
        g.mapping = mapping;
        g.label_south = label_south;
        g.label_east = label_east;
        g.label_west = label_west;
        g.label_north = label_north;
        
        A.UI.gamepadId = g.gamepad_id;
        if(A.UI.current)
            A.UI.current->onGamepadConnected(gamepad_id);

    }
    void gamepad_removed(uint32 gamepad_id)
    {
        logf(LogGamepad, ("gamepad removed %d", gamepad_id));
        uint32 i;
        for(i = 0; i < A.UI.gamepads.size(); i++)
        {
            if(A.UI.gamepads[i].gamepad_id == gamepad_id)
            {
                A.UI.gamepads.erase(i);
                if(A.UI.gamepadId == gamepad_id)
                {
                    if(A.UI.gamepads.size())
                    {
                        A.UI.gamepadId = A.UI.gamepads[0].gamepad_id;
                    }
                    else
                    {
                        A.UI.gamepadId = Gamepad::InvalidGamepadId;
                        if(A.UI.current)
                            A.UI.current->onGamepadDisconnected(gamepad_id);
                    }
                }
                break;
            }
        }
    }
    void gamepad_buttondown (uint32 gamepad_id, uint32 button)
    {
        logf(LogGamepad, ("gamepad buttondown %d", button));
        if(A.UI.current && gamepad_id == A.UI.gamepadId)
            A.UI.current->onGamepadButtonDown(button);
    }
    
    void gamepad_buttonup (uint32 gamepad_id, uint32 button)
    {
        logf(LogGamepad, ("gamepad buttonup %d", button));
        if(A.UI.current && gamepad_id == A.UI.gamepadId)
            A.UI.current->onGamepadButtonUp(button);
    }
    void gamepad_axis (uint32 gamepad_id, uint32 axis, float32 value)
    {
        logf(LogGamepadAxis, ("gamepad axis %d %g", axis, value));
        if(A.UI.current && gamepad_id == A.UI.gamepadId)
            A.UI.current->onControllerAxis(axis, value);
    }

    void idle (uint32 integerTime)
    {
        if(A.UI.current)
            A.UI.current->idle(integerTime);

        glFlush();
            UserInterface::renderCurrent(integerTime);
            A.P.present();
    }
    void init()
    {
        A.UI.init();
        A.UITest.activate();
    }
} G;
