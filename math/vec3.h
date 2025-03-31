// The math library - copyright KAGR LLC, portions copyright Team Nutshell. This library is derived largely from nml - the C++ Nutshell Mathematics Library for Computer Graphics (https://team-nutshell.github.io/nml/). The use of this sorce code is governed by the MIT software license described in the "license.txt" file in this directory.

// x | y | z
struct vec3 {
    float x;
    float y;
    float z;

    // Constructors
    vec3() : x(0.0f), y(0.0f), z(0.0f) {}
    vec3(float _value) : x(_value), y(_value), z(_value) {}
    vec3(float _x, float _y, float _z) : x(_x), y(_y), z(_z) {}
    vec3(float _x, const vec2 &_yz) : x(_x), y(_yz.x), z(_yz.y) {}
    vec3(const vec2 &_xy, float _z) : x(_xy.x), y(_xy.y), z(_z) {}
    vec3(const float *_ptr) : x(*_ptr), y(*(_ptr + 1)), z(*(_ptr + 2)) {}
    vec3(const vec4 &_xyzw) : x(_xyzw.x), y(_xyzw.y), z(_xyzw.z) {}

    void set(float _x, float _y, float _z) { x = _x; y = _y; z = _z; }
    
    vec3 &operator+=(const vec3 &other) {
        x += other.x;
        y += other.y;
        z += other.z;

        return *this;
    }

    vec3 &operator-=(const vec3 &other) {
        x -= other.x;
        y -= other.y;
        z -= other.z;

        return *this;
    }

    vec3 &operator*=(const float other) {
        x *= other;
        y *= other;
        z *= other;

        return *this;
    }

    vec3 &operator/=(const float other) {
        x /= other;
        y /= other;
        z /= other;

        return *this;
    }

    vec3 operator-() const { return vec3(-x, -y, -z); }

    float &operator[](size_t index) {
        assert_msg(index < 3, "index out of range");
        if (index == 0) {
            return x;
        } else if (index == 1) {
            return y;
        } else {
            return z;
        }
    }

    const float operator[](size_t index) const {
        assert_msg(index < 3, "index out of range");
        if (index == 0) {
            return x;
        } else if (index == 1) {
            return y;
        } else {
            return z;
        }
    }

    float length() const { return std::sqrt((x * x) + (y * y) + (z * z)); }

    float *data() { return &x; }

    vec3 operator+(const vec3 &rhs) const {
        vec3 lhs = *this;
        lhs += rhs;

        return lhs;
    }

    vec3 operator-(const vec3 &rhs) const {
        vec3 lhs = *this;
        lhs -= rhs;

        return lhs;
    }

    vec3 operator*(const float rhs) const {
        vec3 lhs = *this;
        lhs *= rhs;

        return lhs;
    }

    vec3 operator/(const float rhs) const {
        vec3 lhs = *this;
        lhs /= rhs;

        return lhs;
    }

    bool operator==(const vec3 &rhs) const {
        return ((x == rhs.x) && (y == rhs.y) && (z == rhs.z));
    }

    bool operator!=(const vec3 &rhs) const {
        return x != rhs.x || y != rhs.y || z != rhs.z;
    }

    void normalize() {
        const float inv_l = 1 / length();
        x *= inv_l;
        y *= inv_l;
        z *= inv_l;
    }
    static vec3 normalize(const vec3 &m)
    {
        vec3 temp = m;
        temp.normalize();
        return temp;
    }

    float dot(const vec3 &b) const {
        return ((x * b.x) + (y * b.y) + (z * b.z));
    }

    vec3 cross(const vec3 &b) const {
        return vec3(y * b.z - z * b.y, z * b.x - x * b.z, x * b.y - y * b.x);
    }

    static vec3 reflect(const vec3 &i, const vec3 &n) {
        return (i - n * n.dot(i) * 2.0f);
    }

    static vec3 refract(const vec3 &i, const vec3 &n, float ior) {
        const float ndoti = n.dot(i);
        const float k = 1.0f - ior * ior * (1.0f - ndoti * ndoti);
        if (k < 0.0f) {
            return vec3(0.0f);
        } else {
            return i * ior - n * (ior * ndoti + std::sqrt(k));
        }
    }

    /*std::string to_string(const vec3& vec) {
        return ("[" + std::to_string(vec.x) + ", " + std::to_string(vec.y) + ",
    " + std::to_string(vec.z) + "]");
    }*/
};
