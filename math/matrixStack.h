// The math library - copyright KAGR LLC, portions copyright Team Nutshell. This library is derived largely from nml - the C++ Nutshell Mathematics Library for Computer Graphics (https://team-nutshell.github.io/nml/). The use of this sorce code is governed by the MIT software license described in the "license.txt" file in this directory.

class MatrixStack
{
    array<mat4> stack;
    bool dirty;
    
public:
    MatrixStack()
    {
        stack.push_back(mat4::identity());
        dirty = false;
    }
    ~MatrixStack()
    {
        assert_msg(stack.size() == 1, "imbalanced matrix stack");
    }
    bool is_dirty()
    {
        return dirty;
    }
    void clear_dirty()
    {
        dirty = false;
    }
    uint32 size()
    {
        return stack.size();
    }
    void push()
    {
        mat4 m = stack.last();
        stack.push_back(m);
    }
    void pop()
    {
        assert_msg(stack.size() > 1, "can't pop an empty stack");
        stack.pop_back();
        dirty = true;
    }
    void translate(const vec3 &xl)
    {
        mat4 m = mat4::translate(xl);
        stack.last() *= m;
        dirty = true;
    }

    void translate(float32 _x, float32 _y, float32 _z)
    {
        mat4 m = mat4::translate(vec3(_x, _y, _z));
        stack.last() *= m;
        dirty = true;
    }

    void scale(const vec3 &sv)
    {
        mat4 m = mat4::scale(sv);
        stack.last() *= m;
        dirty = true;
    }

    void scale(float32 _x, float32 _y, float32 _z)
    {
        mat4 m = mat4::scale(vec3(_x, _y, _z));
        stack.last() *= m;
        dirty = true;
    }

    void rotate(float32 angle, const vec3 &axis)
    {
        float32 radians = mDegToRad(angle);
        mat4 m = mat4::rotate(radians, axis);
        stack.last() *= m;
        dirty = true;
    }
    void rotate(float32 angle, float32 axis_x, float32 axis_y, float32 axis_z)
    {
        rotate(angle, vec3(axis_x, axis_y, axis_z));
    }
    void set(const mat4 &m)
    {
        stack.last() = m;
        dirty = true;
    }
    void mul(const mat4 &m)
    {
        stack.last() *= m;
        dirty = true;
    }
    void identity()
    {
        stack.last() = mat4::identity();
        dirty = true;
    }
    const mat4& top()
    {
        return stack.last();
    }
};

