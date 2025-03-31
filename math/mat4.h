// The math library - copyright KAGR LLC, portions copyright Team Nutshell. This library is derived largely from nml - the C++ Nutshell Mathematics Library for Computer Graphics (https://team-nutshell.github.io/nml/). The use of this sorce code is governed by the MIT software license described in the "license.txt" file in this directory.

//  xx | yx | zx | wx
// ----|----|----|----
//  xy | yy | zy | wy
// ----|----|----|----
//  xz | yz | zz | wz
// ----|----|----|----
//  xw | yw | zw | ww
struct mat4 {
    vec4 x;
    vec4 y;
    vec4 z;
    vec4 w;

    mat4() : x(0.0f), y(0.0f), z(0.0f), w(0.0f) {}
    mat4(float _value) : x(_value), y(_value), z(_value), w(_value) {}
    mat4(float _xx, float _xy, float _xz, float _xw, float _yx, float _yy,
         float _yz, float _yw, float _zx, float _zy, float _zz, float _zw,
         float _wx, float _wy, float _wz, float _ww)
        : x(_xx, _xy, _xz, _xw), y(_yx, _yy, _yz, _yw), z(_zx, _zy, _zz, _zw),
          w(_wx, _wy, _wz, _ww) {}
    mat4(float _xx, float _xy, float _xz, float _xw, float _yx, float _yy,
         float _yz, float _yw, float _zx, float _zy, float _zz, float _zw,
         const vec4 &_w)
        : x(_xx, _xy, _xz, _xw), y(_yx, _yy, _yz, _yw), z(_zx, _zy, _zz, _zw),
          w(_w) {}
    mat4(float _xx, float _xy, float _xz, float _xw, float _yx, float _yy,
         float _yz, float _yw, const vec4 &_z, float _wx, float _wy, float _wz,
         float _ww)
        : x(_xx, _xy, _xz, _xw), y(_yx, _yy, _yz, _yw), z(_z),
          w(_wx, _wy, _wz, _ww) {}
    mat4(float _xx, float _xy, float _xz, float _xw, const vec4 &_y, float _zx,
         float _zy, float _zz, float _zw, float _wx, float _wy, float _wz,
         float _ww)
        : x(_xx, _xy, _xz, _xw), y(_y), z(_zx, _zy, _zz, _zw),
          w(_wx, _wy, _wz, _ww) {}
    mat4(const vec4 &_x, float _yx, float _yy, float _yz, float _yw, float _zx,
         float _zy, float _zz, float _zw, float _wx, float _wy, float _wz,
         float _ww)
        : x(_x), y(_yx, _yy, _yz, _yw), z(_zx, _zy, _zz, _zw),
          w(_wx, _wy, _wz, _ww) {}
    mat4(float _xx, float _xy, float _xz, float _xw, float _yx, float _yy,
         float _yz, float _yw, const vec4 &_z, const vec4 &_w)
        : x(_xx, _xy, _xz, _xw), y(_yx, _yy, _yz, _yw), z(_z), w(_w) {}
    mat4(float _xx, float _xy, float _xz, float _xw, const vec4 &_y, float _zx,
         float _zy, float _zz, float _zw, const vec4 &_w)
        : x(_xx, _xy, _xz, _xw), y(_y), z(_zx, _zy, _zz, _zw), w(_w) {}
    mat4(const vec4 &_x, float _yx, float _yy, float _yz, float _yw, float _zx,
         float _zy, float _zz, float _zw, const vec4 &_w)
        : x(_x), y(_yx, _yy, _yz, _yw), z(_zx, _zy, _zz, _zw), w(_w) {}
    mat4(float _xx, float _xy, float _xz, float _xw, const vec4 &_y,
         const vec4 &_z, float _wx, float _wy, float _wz, float _ww)
        : x(_xx, _xy, _xz, _xw), y(_y), z(_z), w(_wx, _wy, _wz, _ww) {}
    mat4(const vec4 &_x, float _yx, float _yy, float _yz, float _yw,
         const vec4 &_z, float _wx, float _wy, float _wz, float _ww)
        : x(_x), y(_yx, _yy, _yz, _yw), z(_z), w(_wx, _wy, _wz, _ww) {}
    mat4(const vec4 &_x, const vec4 &_y, float _zx, float _zy, float _zz,
         float _zw, float _wx, float _wy, float _wz, float _ww)
        : x(_x), y(_y), z(_zx, _zy, _zz, _zw), w(_wx, _wy, _wz, _ww) {}
    mat4(float _xx, float _xy, float _xz, float _xw, const vec4 &_y,
         const vec4 &_z, const vec4 &_w)
        : x(_xx, _xy, _xz, _xw), y(_y), z(_z), w(_w) {}
    mat4(const vec4 &_x, float _yx, float _yy, float _yz, float _yw,
         const vec4 &_z, const vec4 &_w)
        : x(_x), y(_yx, _yy, _yz, _yw), z(_z), w(_w) {}
    mat4(const vec4 &_x, const vec4 &_y, float _zx, float _zy, float _zz,
         float _zw, const vec4 &_w)
        : x(_x), y(_y), z(_zx, _zy, _zz, _zw), w(_w) {}
    mat4(const vec4 &_x, const vec4 &_y, const vec4 &_z, float _wx, float _wy,
         float _wz, float _ww)
        : x(_x), y(_y), z(_z), w(_wx, _wy, _wz, _ww) {}
    mat4(const vec4 &_x, const vec4 &_y, const vec4 &_z, const vec4 &_w)
        : x(_x), y(_y), z(_z), w(_w) {}
    mat4(const float *_ptr) : x(_ptr), y(_ptr + 4), z(_ptr + 8), w(_ptr + 12) {}

    mat4 &operator+=(const mat4 &other) {
        x += other.x;
        y += other.y;
        z += other.z;
        w += other.w;

        return *this;
    }

    mat4 &operator-=(const mat4 &other) {
        x -= other.x;
        y -= other.y;
        z -= other.z;
        w -= other.w;

        return *this;
    }

    mat4 &operator*=(const mat4 &other) {
        const mat4 tmp(vec4(x.x * other.x.x + y.x * other.x.y +
                                z.x * other.x.z + w.x * other.x.w,
                            x.y * other.x.x + y.y * other.x.y +
                                z.y * other.x.z + w.y * other.x.w,
                            x.z * other.x.x + y.z * other.x.y +
                                z.z * other.x.z + w.z * other.x.w,
                            x.w * other.x.x + y.w * other.x.y +
                                z.w * other.x.z + w.w * other.x.w),
                       vec4(x.x * other.y.x + y.x * other.y.y +
                                z.x * other.y.z + w.x * other.y.w,
                            x.y * other.y.x + y.y * other.y.y +
                                z.y * other.y.z + w.y * other.y.w,
                            x.z * other.y.x + y.z * other.y.y +
                                z.z * other.y.z + w.z * other.y.w,
                            x.w * other.y.x + y.w * other.y.y +
                                z.w * other.y.z + w.w * other.y.w),
                       vec4(x.x * other.z.x + y.x * other.z.y +
                                z.x * other.z.z + w.x * other.z.w,
                            x.y * other.z.x + y.y * other.z.y +
                                z.y * other.z.z + w.y * other.z.w,
                            x.z * other.z.x + y.z * other.z.y +
                                z.z * other.z.z + w.z * other.z.w,
                            x.w * other.z.x + y.w * other.z.y +
                                z.w * other.z.z + w.w * other.z.w),
                       vec4(x.x * other.w.x + y.x * other.w.y +
                                z.x * other.w.z + w.x * other.w.w,
                            x.y * other.w.x + y.y * other.w.y +
                                z.y * other.w.z + w.y * other.w.w,
                            x.z * other.w.x + y.z * other.w.y +
                                z.z * other.w.z + w.z * other.w.w,
                            x.w * other.w.x + y.w * other.w.y +
                                z.w * other.w.z + w.w * other.w.w));

        x = tmp.x;
        y = tmp.y;
        z = tmp.z;
        w = tmp.w;

        return *this;
    }

    mat4 &operator*=(const float other) {
        x *= other;
        y *= other;
        z *= other;
        w *= other;

        return *this;
    }

    mat4 &operator/=(const float other) {
        x /= other;
        y /= other;
        z /= other;
        w /= other;

        return *this;
    }

    vec4 &operator[](size_t index) {
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

    const vec4 &operator[](size_t index) const {
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

    float det() const {
        return (
            x.x * ((y.y * z.z * w.w) - (y.y * w.z * z.w) - (z.y * y.z * w.w) +
                   (z.y * w.z * y.w) + (w.y * y.z * z.w) - (w.y * z.z * y.w)) -
            y.x * ((x.y * z.z * w.w) - (x.y * w.z * z.w) - (z.y * x.z * w.w) +
                   (z.y * w.z * x.w) + (w.y * x.z * z.w) - (w.y * z.z * x.w)) +
            z.x * ((x.y * y.z * w.w) - (x.y * w.z * y.w) - (y.y * x.z * w.w) +
                   (y.y * w.z * x.w) + (w.y * x.z * y.w) - (w.y * y.z * x.w)) -
            w.x * ((x.y * y.z * z.w) - (x.y * z.z * y.w) - (y.y * x.z * z.w) +
                   (y.y * z.z * x.w) + (z.y * x.z * y.w) - (z.y * y.z * x.w)));
    }

    float *data() { return x.data(); }

    static mat4 identity() {
        return mat4(1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
                    1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f);
    }

    mat4 operator+(const mat4 &rhs) const {
        mat4 lhs = *this;
        lhs += rhs;

        return lhs;
    }

    mat4 operator-(const mat4 &rhs) const {
        mat4 lhs = *this;
        lhs -= rhs;

        return lhs;
    }

    mat4 operator*(const mat4 &rhs) const {
        mat4 lhs = *this;
        lhs *= rhs;

        return lhs;
    }

    vec4 operator*(const vec4 &rhs) const {
        mat4 lhs = *this;
        return vec4(lhs.x.x * rhs.x + lhs.y.x * rhs.y + lhs.z.x * rhs.z +
                        lhs.w.x * rhs.w,
                    lhs.x.y * rhs.x + lhs.y.y * rhs.y + lhs.z.y * rhs.z +
                        lhs.w.y * rhs.w,
                    lhs.x.z * rhs.x + lhs.y.z * rhs.y + lhs.z.z * rhs.z +
                        lhs.w.z * rhs.w,
                    lhs.x.w * rhs.x + lhs.y.w * rhs.y + lhs.z.w * rhs.z +
                        lhs.w.w * rhs.w);
    }

    mat4 operator*(const float rhs) const {
        mat4 lhs = *this;
        lhs *= rhs;

        return lhs;
    }

    mat4 operator/(const float rhs) const {
        mat4 lhs = *this;
        lhs /= rhs;

        return lhs;
    }

    bool operator==(const mat4 &rhs) const {
        return ((x == rhs.x) && (y == rhs.y) && (z == rhs.z) && (w == rhs.w));
    }

    bool operator!=(const mat4 &rhs) const {
        return x != rhs.x || y != rhs.y || z != rhs.z || w != rhs.w;
    }

    static mat4 transpose(const mat4 &mat) {
        return mat4(mat.x.x, mat.y.x, mat.z.x, mat.w.x, mat.x.y, mat.y.y,
                    mat.z.y, mat.w.y, mat.x.z, mat.y.z, mat.z.z, mat.w.z,
                    mat.x.w, mat.y.w, mat.z.w, mat.w.w);
    }

    static mat4 inverse(const mat4 &mat) {
        const float determinant = mat.det();

        const mat4 t = transpose(mat);
        const float a =
            mat3(t.y.y, t.y.z, t.y.w, t.z.y, t.z.z, t.z.w, t.w.y, t.w.z, t.w.w)
                .det();
        const float b =
            mat3(t.y.x, t.y.z, t.y.w, t.z.x, t.z.z, t.z.w, t.w.x, t.w.z, t.w.w)
                .det() *
            -1.0f;
        const float c =
            mat3(t.y.x, t.y.y, t.y.w, t.z.x, t.z.y, t.z.w, t.w.x, t.w.y, t.w.w)
                .det();
        const float d =
            mat3(t.y.x, t.y.y, t.y.z, t.z.x, t.z.y, t.z.z, t.w.x, t.w.y, t.w.z)
                .det() *
            -1.0f;
        const float e =
            mat3(t.x.y, t.x.z, t.x.w, t.z.y, t.z.z, t.z.w, t.w.y, t.w.z, t.w.w)
                .det() *
            -1.0f;
        const float f =
            mat3(t.x.x, t.x.z, t.x.w, t.z.x, t.z.z, t.z.w, t.w.x, t.w.z, t.w.w)
                .det();
        const float g =
            mat3(t.x.x, t.x.y, t.x.w, t.z.x, t.z.y, t.z.w, t.w.x, t.w.y, t.w.w)
                .det() *
            -1.0f;
        const float h =
            mat3(t.x.x, t.x.y, t.x.z, t.z.x, t.z.y, t.z.z, t.w.x, t.w.y, t.w.z)
                .det();
        const float i =
            mat3(t.x.y, t.x.z, t.x.w, t.y.y, t.y.z, t.y.w, t.w.y, t.w.z, t.w.w)
                .det();
        const float j =
            mat3(t.x.x, t.x.z, t.x.w, t.y.x, t.y.z, t.y.w, t.w.x, t.w.z, t.w.w)
                .det() *
            -1.0f;
        const float k =
            mat3(t.x.x, t.x.y, t.x.w, t.y.x, t.y.y, t.y.w, t.w.x, t.w.y, t.w.w)
                .det();
        const float l =
            mat3(t.x.x, t.x.y, t.x.z, t.y.x, t.y.y, t.y.z, t.w.x, t.w.y, t.w.z)
                .det() *
            -1.0f;
        const float m =
            mat3(t.x.y, t.x.z, t.x.w, t.y.y, t.y.z, t.y.w, t.z.y, t.z.z, t.z.w)
                .det() *
            -1.0f;
        const float n =
            mat3(t.x.x, t.x.z, t.x.w, t.y.x, t.y.z, t.y.w, t.z.x, t.z.z, t.z.w)
                .det();
        const float o =
            mat3(t.x.x, t.x.y, t.x.w, t.y.x, t.y.y, t.y.w, t.z.x, t.z.y, t.z.w)
                .det() *
            -1.0f;
        const float p =
            mat3(t.x.x, t.x.y, t.x.z, t.y.x, t.y.y, t.y.z, t.z.x, t.z.y, t.z.z)
                .det();

        const mat4 adj = mat4(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p);

        return adj * (1.0f / determinant);
    }

    static mat4 translate(const vec3 &translation) {
        return mat4(1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
                    1.0f, 0.0f, translation.x, translation.y, translation.z,
                    1.0f);
    }

    static mat4 rotate(const float angle, const vec3 &axis) {
        const float cosTheta = std::cos(angle);
        const float oMCT = 1.0f - cosTheta;
        const float sinTheta = std::sin(angle);

        return mat4(cosTheta + ((axis.x * axis.x) * oMCT),
                    ((axis.y * axis.x) * oMCT) + (axis.z * sinTheta),
                    ((axis.z * axis.x) * oMCT) - (axis.y * sinTheta), 0.0f,
                    ((axis.x * axis.y) * oMCT) - (axis.z * sinTheta),
                    cosTheta + ((axis.y * axis.y) * oMCT),
                    ((axis.z * axis.y) * oMCT) + (axis.x * sinTheta), 0.0f,
                    ((axis.x * axis.z) * oMCT) + (axis.y * sinTheta),
                    ((axis.y * axis.z) * oMCT) - (axis.x * sinTheta),
                    cosTheta + ((axis.z * axis.z) * oMCT), 0.0f, 0.0f, 0.0f,
                    0.0f, 1.0f);
    }

    static mat4 scale(const vec3 &scaling) {
        return mat4(scaling.x, 0.0f, 0.0f, 0.0f, 0.0f, scaling.y, 0.0f, 0.0f,
                    0.0f, 0.0f, scaling.z, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f);
    }

    static mat4 frustum(double left, double right, double bottom, double top, double nearDist, double farDist)
    {
        mat4 ret = identity();
        double A = (right + left) / (right - left);
        double B = (top + bottom) / (top - bottom);
        double C = (farDist + nearDist) / (farDist - nearDist);
        double D = (2 * farDist * nearDist) / (farDist - nearDist);
        ret.x.x = 2 * nearDist / (right - left);
        ret.y.y = 2 * nearDist / (top - bottom);
        ret.z.x = A;
        ret.z.y = B;
        ret.z.z = C;
        ret.z.w = -1;
        ret.w.z = D;
        ret.w.w = 0;
        return ret;
    }
    
    static mat4 lookAtLH(const vec3 &from, const vec3 &to, const vec3 &up) {
        const vec3 forward = vec3::normalize(to - from);
        const vec3 right = vec3::normalize(up.cross(forward));
        const vec3 realUp = forward.cross(right);

        return mat4(right.x, realUp.x, forward.x, 0.0, right.y, realUp.y,
                    forward.y, 0.0, right.z, realUp.z, forward.z, 0.0,
                    -right.dot(from), -realUp.dot(from), -forward.dot(from),
                    1.0);
    }

    static mat4 lookAtRH(const vec3 &from, const vec3 &to, const vec3 &up) {
        const vec3 forward = vec3::normalize(to - from);
        const vec3 right = vec3::normalize(forward.cross(up));
        const vec3 realUp = right.cross(forward);

        return mat4(right.x, realUp.x, -forward.x, 0.0f, right.y, realUp.y,
                    -forward.y, 0.0f, right.z, realUp.z, -forward.z, 0.0f,
                    -right.dot(from), -realUp.dot(from), forward.dot(from),
                    1.0f);
    }

    static mat4 orthoLH(const float left, const float right, const float bottom,
                        const float top, const float nearDist, const float farDist) {
        const float rightPlusLeft = right + left;
        const float rightMinusLeft = right - left;
        const float topPlusBottom = top + bottom;
        const float topMinusBottom = top - bottom;
        const float farMinusNear = farDist - nearDist;

        return mat4(
            2.0f / rightMinusLeft, 0.0f, 0.0f, 0.0f, 0.0f,
            2.0f / topMinusBottom, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f / farMinusNear,
            0.0f, -(rightPlusLeft / rightMinusLeft),
            -(topPlusBottom / topMinusBottom), -nearDist / farMinusNear, 1.0f);
    }

    static mat4 orthoRH(const float left, const float right, const float bottom,
                        const float top, const float nearDist, const float farDist) {
        const float rightPlusLeft = right + left;
        const float rightMinusLeft = right - left;
        const float topPlusBottom = top + bottom;
        const float topMinusBottom = top - bottom;
        const float farMinusNear = farDist - nearDist;

        return mat4(
            2.0f / rightMinusLeft, 0.0f, 0.0f, 0.0f, 0.0f,
            2.0f / topMinusBottom, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f / farMinusNear,
            0.0f, -(rightPlusLeft / rightMinusLeft),
            -(topPlusBottom / topMinusBottom), -nearDist / farMinusNear, 1.0f);
    }

    static mat4 perspectiveLH(const float fovY, const float aspectRatio,
                              const float nearDist, const float farDist) {
        const float tanHalfFovY = std::tan(fovY / 2.0f);
        const float farMinusNear = farDist - nearDist;

        return mat4(1.0f / (aspectRatio * tanHalfFovY), 0.0f, 0.0f, 0.0f, 0.0f,
                    1.0f / tanHalfFovY, 0.0f, 0.0f, 0.0f, 0.0f,
                    farDist / farMinusNear, 1.0f, 0.0f, 0.0f,
                    -(farDist * nearDist) / farMinusNear, 0.0f);
    }

    static mat4 perspectiveRH(const float fovY, const float aspectRatio,
                              const float nearDist, const float farDist) {
        const float tanHalfFovY = std::tan(fovY / 2.0f);
        const float farMinusNear = farDist - nearDist;
        const float nearMinusFar = nearDist - farDist;

        return mat4(1.0f / (aspectRatio * tanHalfFovY), 0.0f, 0.0f, 0.0f, 0.0f,
                    1.0f / tanHalfFovY, 0.0f, 0.0f, 0.0f, 0.0f,
                    farDist / nearMinusFar, -1.0f, 0.0f, 0.0f,
                    -(farDist * nearDist) / farMinusNear, 0.0f);
    }
    /*
     std::string to_string(const mat4& mat) {
     return ("[" + to_string(mat.x) + ", " + to_string(mat.y) + ", " +
     to_string(mat.z) + ", " + to_string(mat.w) + "]");
     }*/
};
