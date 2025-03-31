// The cradle library - copyright KAGR LLC. The use of this source code is governed by the license agreement(s) described in the "license.txt" file in this directory.

enum GamepadButtonInfo {
    GamepadButton1 = 1 << 0,
    GamepadButton2 = 1 << 1,
    GamepadButton3 = 1 << 2,
    GamepadButton4 = 1 << 3,
    GamepadButton5 = 1 << 4,
    GamepadButton6 = 1 << 5,
    GamepadButtonLeftTrigger = 1 << 6,
    GamepadButtonRightTrigger = 1 << 7,
    ControllerGameButtonCount = 8,
    GamepadButtonStart = 1 << 8,
    GamepadButtonBack = 1 << 9,
    GamepadButtonDPadUp = 1 << 10,
    GamepadButtonDPadDown = 1 << 11,
    GamepadButtonDPadLeft = 1 << 12,
    GamepadButtonDPadRight = 1 << 13,
};

enum {
    ButtonTypeCircle = 1,
    ButtonTypeRect = 2,
    ButtonTypeDPad = 3,
};

struct GamepadButtonRenderDetails
{
    uint32 buttonIndex;
    uint32 buttonType;
    float32 color[3];
    uint32 fontSize;
    const char *text;
};

static void renderGamepadButton(float32 x, float32 y, uint32 buttonIndex, uint32 keyIndex)
{
    if(!A.UI.gamepadEnabled())
        UserInterface::drawStringf(x, y, 15, "%c", keyIndex);
    else
    {
        enum {
            NumButtonLabels = 12,
            NumRemapLables = 9,
        };
        Gamepad *gp = A.UI.getEnabledGamepad();
        GamepadButtonRenderDetails *d;

        static GamepadButtonRenderDetails remapGamepadButtonInfo[NumRemapLables] = {
            {SDL_GAMEPAD_BUTTON_LABEL_UNKNOWN, ButtonTypeCircle, { 0, 1, 0 }, 12, "?"},
            {SDL_GAMEPAD_BUTTON_LABEL_A, ButtonTypeCircle, { 1, 0, 0 }, 12, "A"},
            {SDL_GAMEPAD_BUTTON_LABEL_B, ButtonTypeCircle, { 0, 0, 1 }, 12, "B"},
            {SDL_GAMEPAD_BUTTON_LABEL_X, ButtonTypeCircle, { 1, 1, 0 }, 12, "X"},
            {SDL_GAMEPAD_BUTTON_LABEL_Y, ButtonTypeCircle, { 0, 1, 0 }, 12, "Y"},
            {SDL_GAMEPAD_BUTTON_LABEL_CROSS, ButtonTypeCircle, { 1, 0, 0 }, 12, "+"},
            {SDL_GAMEPAD_BUTTON_LABEL_CIRCLE, ButtonTypeCircle, { 0, 0, 1 }, 12, "O"},
            {SDL_GAMEPAD_BUTTON_LABEL_SQUARE, ButtonTypeCircle, { 1, 1, 0 }, 12, "["},
            {SDL_GAMEPAD_BUTTON_LABEL_TRIANGLE, ButtonTypeCircle, { 1, 1, 0 }, 12, "T"},
        };

        static GamepadButtonRenderDetails buttonInfo[NumButtonLabels] = {
            {SDL_GAMEPAD_BUTTON_SOUTH, ButtonTypeCircle, { 0, 1, 0 }, 12, "A"},
            {SDL_GAMEPAD_BUTTON_EAST, ButtonTypeCircle, { 1, 0, 0 }, 12, "B"},
            {SDL_GAMEPAD_BUTTON_WEST, ButtonTypeCircle, { 0, 0, 1 }, 12, "X"},
            {SDL_GAMEPAD_BUTTON_NORTH, ButtonTypeCircle, { 1, 1, 0 }, 12, "Y"},
            {SDL_GAMEPAD_BUTTON_BACK, ButtonTypeCircle, { 0.25, 0.25, 0.25 }, 9, "BK"},
            {SDL_GAMEPAD_BUTTON_START, ButtonTypeCircle, { 0.25, 0.25, 0.25 }, 9, "ST"},
            {SDL_GAMEPAD_BUTTON_LEFT_SHOULDER, ButtonTypeRect, { 0.25, 0.25, 0.25 }, 9, "LS"},
            {SDL_GAMEPAD_BUTTON_RIGHT_SHOULDER, ButtonTypeRect, { 0.25, 0.25, 0.25 }, 9, "RS"},
            {SDL_GAMEPAD_BUTTON_DPAD_UP, ButtonTypeCircle, { 0.25, 0.25, 0.25 }, 12, "U"},
            {SDL_GAMEPAD_BUTTON_DPAD_DOWN, ButtonTypeCircle, { 0.25, 0.25, 0.25 }, 12, "D"},
            {SDL_GAMEPAD_BUTTON_DPAD_LEFT, ButtonTypeCircle, { 0.25, 0.25, 0.25 }, 12, "L"},
            {SDL_GAMEPAD_BUTTON_DPAD_RIGHT, ButtonTypeCircle, { 0.25, 0.25, 0.25 }, 12, "R"},
        };
        if(gp && (buttonIndex == SDL_GAMEPAD_BUTTON_SOUTH || buttonIndex == SDL_GAMEPAD_BUTTON_EAST ||
                  buttonIndex == SDL_GAMEPAD_BUTTON_WEST || buttonIndex == SDL_GAMEPAD_BUTTON_NORTH))
        {
            uint32 label;
            switch(buttonIndex)
            {
                case SDL_GAMEPAD_BUTTON_SOUTH:
                    label = gp->label_south;
                    break;
                case SDL_GAMEPAD_BUTTON_EAST:
                    label = gp->label_east;
                    break;
                case SDL_GAMEPAD_BUTTON_WEST:
                    label = gp->label_west;
                    break;
                case SDL_GAMEPAD_BUTTON_NORTH:
                    label = gp->label_north;
                    break;
            }
            uint32 index;
            for(index = 0; index < NumRemapLables; index++)
                if(remapGamepadButtonInfo[index].buttonIndex == label)
                    break;
            if(index == NumRemapLables)
                return;
            d = remapGamepadButtonInfo + index;
        }
        else
        {
            uint32 index;
            for(index = 0; index < NumButtonLabels; index++)
                if(buttonInfo[index].buttonIndex == buttonIndex)
                    break;
            if(index == NumButtonLabels)
                return;
            d = buttonInfo + index;
        }
        Color c(d->color[0], d->color[1], d->color[2]);
        
        A.P.color(c * 0.5f);
        switch(d->buttonType)
        {
            case ButtonTypeCircle:
                A.P.fill_circle(Point(x + 8, y + 8), 8);
                glColor3f(1,1,1);
                A.P.draw_circle(Point(x + 8, y + 8), 8);
                UserInterface::drawStringf(x + 4, y + 2, d->fontSize, d->text);
                break;
            case ButtonTypeRect:
                A.P.draw_rect(Point(x, y), Point(20, 16));
                glColor3f(1,1,1);
                A.P.draw_rect(Point(x, y), Point(20, 16));
                UserInterface::drawStringf(x + 4, y + 2, d->fontSize, d->text);
                break;
        }
    }
}
