// The math library - copyright KAGR LLC, portions copyright Team Nutshell. This library is derived largely from nml - the C++ Nutshell Mathematics Library for Computer Graphics (https://team-nutshell.github.io/nml/). The use of this sorce code is governed by the MIT software license described in the "license.txt" file in this directory.

//  xx | yx
// ----|----
//  xy | yy

struct mat2 {
    vec2 x;
    vec2 y;

    mat2() : x(0.0f), y(0.0f) {}
    mat2(float _value) : x(_value), y(_value) {}
    mat2(float _xx, float _xy, float _yx, float _yy)
        : x(_xx, _xy), y(_yx, _yy) {}
    mat2(float _xx, float _xy, const vec2 &_y) : x(_xx, _xy), y(_y) {}
    mat2(const vec2 &_x, float _yx, float _yy) : x(_x), y(_yx, _yy) {}
    mat2(const vec2 &_x, const vec2 &_y) : x(_x), y(_y) {}
    mat2(const float *_ptr) : x(_ptr), y(_ptr + 2) {}
    mat2(const mat3 &_mat) : x(_mat.x), y(_mat.y) {}
    mat2(const mat4 &_mat) : x(_mat.x), y(_mat.y) {}

    mat2 &operator+=(const mat2 &other) {
        x += other.x;
        y += other.y;

        return *this;
    }

    mat2 &operator-=(const mat2 &other) {
        x -= other.x;
        y -= other.y;

        return *this;
    }

    mat2 &operator*=(const mat2 &other) {
        const mat2 tmp(vec2(x.x * other.x.x + y.x * other.x.y,
                            x.y * other.x.x + y.y * other.x.y),
                       vec2(x.x * other.y.x + y.x * other.y.y,
                            x.y * other.y.x + y.y * other.y.y));

        x = tmp.x;
        y = tmp.y;

        return *this;
    }

    mat2 &operator*=(const float other) {
        x *= other;
        y *= other;

        return *this;
    }

    mat2 &operator/=(const float other) {
        x /= other;
        y /= other;

        return *this;
    }

    vec2 &operator[](size_t index) {
        assert_msg(index < 2, "index out of range");
        return index == 0 ? x : y;
    }

    const vec2 &operator[](size_t index) const {
        assert_msg(index < 2, "index out of range");
        return index == 0 ? x : y;
    }

    float det() const { return (x.x * y.y - y.x * x.y); }

    float *data() { return x.data(); }

    static mat2 identity() { return mat2(1.0f, 0.0f, 0.0f, 1.0f); }

    mat2 operator+(const mat2 &rhs) const {
        mat2 lhs = *this;
        lhs += rhs;

        return lhs;
    }

    mat2 operator-(const mat2 &rhs) const {
        mat2 lhs = *this;
        lhs -= rhs;

        return lhs;
    }

    mat2 operator*(const mat2 &rhs) const {
        mat2 lhs = *this;
        lhs *= rhs;

        return lhs;
    }

    vec2 operator*(const vec2 &rhs) const {
        return vec2(x.x * rhs.x + y.x * rhs.y, x.y * rhs.x + y.y * rhs.y);
    }

    mat2 operator*(const float rhs) const {
        mat2 lhs = *this;
        lhs *= rhs;

        return lhs;
    }

    mat2 operator/(const float rhs) const {
        mat2 lhs = *this;
        lhs /= rhs;

        return lhs;
    }

    bool operator==(const mat2 &rhs) const {
        return ((x == rhs.x) && (y == rhs.y));
    }

    bool operator!=(const mat2 &rhs) const { return x != rhs.x || y != rhs.y; }

    mat2 transpose() const { return mat2(x.x, y.x, x.y, y.y); }

    mat2 inverse() const {
        const float determinant = det();
        return mat2(y.y, -x.y, -y.x, x.x) * (1.0f / determinant);
    }

    /*std::string to_string(const mat2& mat) {
        return ("[" + to_string(mat.x) + ", " + to_string(mat.y) + "]");
    }*/
};
