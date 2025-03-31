// The math library - copyright KAGR LLC, portions copyright Team Nutshell. This library is derived largely from nml - the C++ Nutshell Mathematics Library for Computer Graphics (https://team-nutshell.github.io/nml/). The use of this sorce code is governed by the MIT software license described in the "license.txt" file in this directory.

// x | y | z | w
struct vec4 {
    float x;
    float y;
    float z;
    float w;

    // Constructors
    vec4() : x(0.0f), y(0.0f), z(0.0f), w(0.0f) {}
    vec4(float _value) : x(_value), y(_value), z(_value), w(_value) {}
    vec4(float _x, float _y, float _z, float _w) : x(_x), y(_y), z(_z), w(_w) {}
    vec4(float _x, const vec3 &_yzw) : x(_x), y(_yzw.x), z(_yzw.y), w(_yzw.z) {}
    vec4(const vec3 &_xyz, float _w) : x(_xyz.x), y(_xyz.y), z(_xyz.z), w(_w) {}
    vec4(float _x, float _y, const vec2 &_zw)
        : x(_x), y(_y), z(_zw.x), w(_zw.y) {}
    vec4(float _x, const vec2 &_yz, float _w)
        : x(_x), y(_yz.x), z(_yz.y), w(_w) {}
    vec4(const vec2 &_xy, float _z, float _w)
        : x(_xy.x), y(_xy.y), z(_z), w(_w) {}
    vec4(const vec2 &_xy, float _w) : x(_xy.x), y(_xy.y), z(0), w(_w) {}

    vec4(const vec2 &_xy, const vec2 &_zw)
        : x(_xy.x), y(_xy.y), z(_zw.x), w(_zw.y) {}
    vec4(const float *_ptr)
        : x(*_ptr), y(*(_ptr + 1)), z(*(_ptr + 2)), w(*(_ptr + 3)) {}

    vec4 &operator+=(const vec4 &other) {
        x += other.x;
        y += other.y;
        z += other.z;
        w += other.w;

        return *this;
    }

    vec4 &operator-=(const vec4 &other) {
        x -= other.x;
        y -= other.y;
        z -= other.z;
        w -= other.w;

        return *this;
    }

    vec4 &operator*=(const float other) {
        x *= other;
        y *= other;
        z *= other;
        w *= other;

        return *this;
    }

    vec4 &operator/=(const float other) {
        x /= other;
        y /= other;
        z /= other;
        w /= other;

        return *this;
    }

    vec4 operator-() const { return vec4(-x, -y, -z, -w); }

    float &operator[](size_t index) {
        assert_msg(index < 4, "index out of range");
        if (index == 0) {
            return x;
        } else if (index == 1) {
            return y;
        } else if (index == 2) {
            return z;
        } else {
            return w;
        }
    }

    const float operator[](size_t index) const {
        assert_msg(index < 4, "index out of range");
        if (index == 0) {
            return x;
        } else if (index == 1) {
            return y;
        } else if (index == 2) {
            return z;
        } else {
            return w;
        }
    }

    float length() const {
        return std::sqrt((x * x) + (y * y) + (z * z) + (w * w));
    }

    float *data() { return &x; }

    vec4 operator+(const vec4 &rhs) const {
        vec4 lhs = *this;
        lhs += rhs;

        return lhs;
    }

    vec4 operator-(const vec4 &rhs) const {
        vec4 lhs = *this;
        lhs -= rhs;

        return lhs;
    }

    vec4 operator*(const float rhs) const {
        vec4 lhs = *this;
        lhs *= rhs;

        return lhs;
    }

    vec4 operator/(const float rhs) const {
        vec4 lhs = *this;
        lhs /= rhs;

        return lhs;
    }

    bool operator==(const vec4 &rhs) const {
        return ((x == rhs.x) && (y == rhs.y) && (z == rhs.z) && (w == rhs.w));
    }

    bool operator!=(const vec4 &rhs) const {
        return x != rhs.x || y != rhs.y || z != rhs.z || w != rhs.w;
    }

    void normalize() {
        const float inv_l = length();
        *this *= inv_l;
    }

    float dot(const vec4 &b) const {
        return ((x * b.x) + (y * b.y) + (z * b.z) + (w * b.w));
    }

    static vec4 reflect(const vec4 &i, const vec4 &n) {
        return (i - n * (2.0f * n.dot(i)));
    }

    static vec4 refract(const vec4 &i, const vec4 &n, float ior) {
        const float ndoti = n.dot(i);
        const float k = 1.0f - ior * ior * (1.0f - ndoti * ndoti);
        if (k < 0.0f) {
            return vec4(0.0f);
        } else {
            return i * ior - n * (ior * ndoti + std::sqrt(k));
        }
    }

    /*std::string to_string(const vec4& vec) {
        return ("[" + std::to_string(vec.x) + ", " + std::to_string(vec.y) + ",
    " + std::to_string(vec.z) + ", " + std::to_string(vec.w) + "]");
    }*/
};
