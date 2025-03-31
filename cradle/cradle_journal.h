// The cradle library - copyright KAGR LLC. The use of this source code is governed by the license agreement(s) described in the "license.txt" file in this directory.

struct cradle_journal : public journal
{
    cradle_journal()
    {
        register_callable(&cradle_journal::windowResized);
        register_callable(&cradle_journal::mouseDragged);
        register_callable(&cradle_journal::mouseMoved);
        register_callable(&cradle_journal::key);
        register_callable(&cradle_journal::mouseButton);
        register_callable(&cradle_journal::gamepad_added);
        register_callable(&cradle_journal::gamepad_removed);
        register_callable(&cradle_journal::gamepad_buttonup);
        register_callable(&cradle_journal::gamepad_buttondown);
        register_callable(&cradle_journal::gamepad_axis);
    }
    virtual void windowResized(int32 newWidth, int32 newHeight)
    {
    }
    
    virtual void mouseDragged(int32 x, int32 y, float32 xRel, float32 yRel)
    {
    }
    
    virtual void mouseMoved(int32 x, int32 y, float32 xRel, float32 yRel)
    {
    }
    
    virtual void key(uint32 key, bool down, uint32 modState)
    {
    }
    
    virtual void mouseButton (int32 button, bool buttonDown, int32 x, int32 y)
    {
    }
    
    virtual void gamepad_added (uint32 gamepad_id, string name, string mapping, uint32 label_south, uint32 label_east, uint32 label_west, uint32 label_north)
    {
    }
    virtual void gamepad_removed(uint32 gamepad_id)
    {
    }
    
    virtual void gamepad_buttondown (uint32 gamepad_id, uint32 button)
    {
    }
    
    virtual void gamepad_buttonup (uint32 gamepad_id, uint32 button)
    {
    }
    
    virtual void gamepad_axis (uint32 gamepad_id, uint32 axis, float32 value)
    {
    }
};
