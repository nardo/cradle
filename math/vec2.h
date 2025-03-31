// The math library - copyright KAGR LLC, portions copyright Team Nutshell. This library is derived largely from nml - the C++ Nutshell Mathematics Library for Computer Graphics (https://team-nutshell.github.io/nml/). The use of this sorce code is governed by the MIT software license described in the "license.txt" file in this directory.

// x | y

struct vec2 {
    typedef float member_type;
    
    float x;
    float y;
    vec2() : x(0.0f), y(0.0f) {}
    vec2(float _value) : x(_value), y(_value) {}
    vec2(float _x, float _y) : x(_x), y(_y) {}
    vec2(const float *_ptr) : x(*_ptr), y(*(_ptr + 1)) {}
    vec2(const vec2 &_xy) : x(_xy.x), y(_xy.y) {}
    vec2(const vec3 &_xyz) : x(_xyz.x), y(_xyz.y) {}
    vec2(const vec4 &_xyzw) : x(_xyzw.x), y(_xyzw.y) {}

    void set(float _x, float _y) { x = _x; y = _y; }
    void set(const vec2 &_xy) { x = _xy.x; y = _xy.y; }
    void setMin(const vec2 &p) { if (x > p.x) x = p.x; if(y > p.y) y = p.y; }
    void setMax(const vec2 &p) { if (x < p.x) x = p.x; if(y < p.y) y = p.y; }

    vec2 &operator+=(const vec2 &other) {
        x += other.x;
        y += other.y;

        return *this;
    }

    vec2 &operator-=(const vec2 &other) {
        x -= other.x;
        y -= other.y;

        return *this;
    }

    vec2 &operator*=(const float other) {
        x *= other;
        y *= other;

        return *this;
    }

    vec2 &operator/=(const float other) {
        x /= other;
        y /= other;

        return *this;
    }

    vec2 operator-() const { return vec2(-x, -y); }

    float &operator[](size_t index) {
        assert_msg(index < 2, "index out of range.");
        if (index == 0)
            return x;
        else
            return y;
    }

    const float operator[](size_t index) const {
        assert_msg(index < 2, "index out of range.");
        return index == 0 ? x : y;
    }

    float length() const { return std::sqrt((x * x) + (y * y)); }
    float len() const { return std::sqrt((x * x) + (y * y)); }
    float lenSquared() const { return x * x + y * y; }

    float *data() { return &x; }

    vec2 operator+(const vec2 &rhs) const {
        vec2 lhs = *this;
        lhs += rhs;

        return lhs;
    }

    vec2 operator-(const vec2 &rhs) const {
        vec2 lhs = *this;
        lhs -= rhs;

        return lhs;
    }

    vec2 operator*(const float rhs) const {
        vec2 lhs = *this;
        lhs *= rhs;

        return lhs;
    }

    vec2 operator*(const vec2 &rhs) const {
        vec2 lhs = *this;
        lhs.x *= rhs.x;
        lhs.y *= rhs.y;
        return lhs;
    }

    vec2 operator/(const float rhs) const {
        vec2 lhs = *this;
        lhs /= rhs;

        return lhs;
    }

    bool operator==(const vec2 &rhs) const {
        return ((x == rhs.x) && (y == rhs.y));
    }

    bool operator!=(const vec2 &rhs) const { return x != rhs.x || y != rhs.y; }

    void normalize() {
        float l = length();
        if(l == 0)
        {
            x = 1; y = 0;
        }
        else
        {
            l = 1 / l;
            x *= l;
            y *= l;
        }
    }
    void normalize(float new_len) {
        float l = length();
        if(l == 0)
        {
            x = new_len; y = 0;
        }
        else
        {
            l = new_len / l;
            x *= l;
            y *= l;
        }
    }
    
    void scaleFloorDiv(float scaleFactor, float divFactor)
    {
        x = (float) floor(x * scaleFactor + 0.5) * divFactor;
        y = (float) floor(y * scaleFactor + 0.5) * divFactor;
    }

    float dot(const vec2 &b) const { return ((x * b.x) + (y * b.y)); }
    float cross(const vec2 &p) const { return x * p.y - y * p.x; }
    vec2 getCross() const { return vec2(y, -x); }
    vec2 & cross() { float tmp = x; x = y; y = -tmp; return *this; }
    float getAngleBetween(const vec2 &p) const { float d = dot(p); float c = cross(p); return RadToDeg(atan2(c, d)); }
    float getAngle() const {return RadToDeg(atan2(y, x)); }   // ccw angle from (1,0)
    vec2 & rotate(float angle) { angle = DegToRad(angle); float tempX = x; x = x * cos(angle) - y * sin(angle); y = tempX * sin(angle) + y * cos(angle); return *this; }
    vec2 & rotate(const vec2 & center, float angle) { vec2 temp = *this - center; temp.rotate(angle); *this = center + temp; return *this; }
    bool isZero() const { return x == 0 && y == 0; }

    static vec2 reflect(const vec2 &i, const vec2 &n) {
        return (i - n * (2.0f * n.dot(i)));
    }

    static vec2 refract(const vec2 &i, const vec2 &n, float ior) {
        const float ndoti = n.dot(i);
        const float k = 1.0f - ior * ior * (1.0f - ndoti * ndoti);
        if (k < 0.0f) {
            return vec2(0.0f);
        } else {
            return i * ior - n * (ior * ndoti + std::sqrt(k));
        }
    }
    void read(const char **argv) { x = (float) atof(argv[0]); y = (float) atof(argv[1]); }

    /*std::string to_string(const vec2& vec) {
        return ("[" + std::to_string(vec.x) + ", " + std::to_string(vec.y) +
    "]");
    }*/
};

typedef vec2 Point;
