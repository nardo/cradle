// The math library - copyright KAGR LLC, portions copyright Team Nutshell. This library is derived largely from nml - the C++ Nutshell Mathematics Library for Computer Graphics (https://team-nutshell.github.io/nml/). The use of this sorce code is governed by the MIT software license described in the "license.txt" file in this directory.

//  xx | yx | zx
// ----|----|----
//  xy | yy | zy
// ----|----|----
//  xz | yz | zz
struct mat3 {
    vec3 x;
    vec3 y;
    vec3 z;

    // Constructors
    mat3() : x(0.0f), y(0.0f), z(0.0f) {}
    mat3(float _value) : x(_value), y(_value), z(_value) {}
    mat3(float _xx, float _xy, float _xz, float _yx, float _yy, float _yz,
         float _zx, float _zy, float _zz)
        : x(_xx, _xy, _xz), y(_yx, _yy, _yz), z(_zx, _zy, _zz) {}
    mat3(float _xx, float _xy, float _xz, float _yx, float _yy, float _yz,
         const vec3 &_z)
        : x(_xx, _xy, _xz), y(_yx, _yy, _yz), z(_z) {}
    mat3(float _xx, float _xy, float _xz, const vec3 &_y, float _zx, float _zy,
         float _zz)
        : x(_xx, _xy, _xz), y(_y), z(_zx, _zy, _zz) {}
    mat3(const vec3 &_x, float _yx, float _yy, float _yz, float _zx, float _zy,
         float _zz)
        : x(_x), y(_yx, _yy, _yz), z(_zx, _zy, _zz) {}
    mat3(float _xx, float _xy, float _xz, const vec3 &_y, const vec3 &_z)
        : x(_xx, _xy, _xz), y(_y), z(_z) {}
    mat3(const vec3 &_x, const vec3 &_y, float _zx, float _zy, float _zz)
        : x(_x), y(_y), z(_zx, _zy, _zz) {}
    mat3(const vec3 &_x, float _yx, float _yy, float _yz, const vec3 &_z)
        : x(_x), y(_yx, _yy, _yz), z(_z) {}
    mat3(const vec3 &_x, const vec3 &_y, const vec3 &_z)
        : x(_x), y(_y), z(_z) {}
    mat3(const float *_ptr) : x(_ptr), y(_ptr + 3), z(_ptr + 6) {}
    mat3(const mat4 &_mat) : x(_mat.x), y(_mat.y), z(_mat.z) {}

    mat3 &operator+=(const mat3 &other) {
        x += other.x;
        y += other.y;
        z += other.z;

        return *this;
    }

    mat3 &operator-=(const mat3 &other) {
        x -= other.x;
        y -= other.y;
        z -= other.z;

        return *this;
    }

    mat3 &operator*=(const mat3 &other) {
        mat3 tmp(vec3(x.x * other.x.x + y.x * other.x.y + z.x * other.x.z,
                      x.y * other.x.x + y.y * other.x.y + z.y * other.x.z,
                      x.z * other.x.x + y.z * other.x.y + z.z * other.x.z),
                 vec3(x.x * other.y.x + y.x * other.y.y + z.x * other.y.z,
                      x.y * other.y.x + y.y * other.y.y + z.y * other.y.z,
                      x.z * other.y.x + y.z * other.y.y + z.z * other.y.z),
                 vec3(x.x * other.z.x + y.x * other.z.y + z.x * other.z.z,
                      x.y * other.z.x + y.y * other.z.y + z.y * other.z.z,
                      x.z * other.z.x + y.z * other.z.y + z.z * other.z.z));

        x = tmp.x;
        y = tmp.y;
        z = tmp.z;

        return *this;
    }

    mat3 &operator*=(const float other) {
        x *= other;
        y *= other;
        z *= other;

        return *this;
    }

    mat3 &operator/=(const float other) {
        x /= other;
        y /= other;
        z /= other;

        return *this;
    }

    vec3 &operator[](size_t index) {
        assert_msg(index < 3, "index out of range.");

        if (index == 0) {
            return x;
        } else if (index == 1) {
            return y;
        } else {
            return z;
        }
    }

    const vec3 &operator[](size_t index) const {
        assert_msg(index < 3, "index out of range.");

        if (index == 0) {
            return x;
        } else if (index == 1) {
            return y;
        } else {
            return z;
        }
    }

    float det() const {
        return ((x.x * ((y.y * z.z) - (z.y * y.z))) -
                (y.x * ((x.y * z.z) - (z.y * x.z))) +
                (z.x * ((x.y * y.z) - (y.y * x.z))));
    }

    void eigen(float eigenvalues[3], vec3 eigenvectors[3]) const {
        const float epsilon = std::numeric_limits<float>::epsilon();

        mat3 scaledMatrix = *this;
        const float shift = (x.x + y.y + z.z) / 3.0f;
        scaledMatrix.x.x -= shift;
        scaledMatrix.y.y -= shift;
        scaledMatrix.z.z -= shift;
        const float scale = std::max(
            std::abs(scaledMatrix.x.x),
            std::max(std::abs(scaledMatrix.x.y),
                     std::max(std::abs(scaledMatrix.x.z),
                              std::max(std::abs(scaledMatrix.y.y),
                                       std::max(std::abs(scaledMatrix.y.z),
                                                std::abs(scaledMatrix.z.z))))));
        if (scale > 0.0f) {
            scaledMatrix /= scale;
        }

        const bool xyIsZero =
            (scaledMatrix.x.y > -epsilon) && (scaledMatrix.x.y < epsilon);
        const bool yzIsZero =
            (scaledMatrix.y.z > -epsilon) && (scaledMatrix.y.z < epsilon);
        const bool xzIsZero =
            (scaledMatrix.x.z > -epsilon) && (scaledMatrix.x.z < epsilon);

        if (xyIsZero && yzIsZero && xzIsZero) {
            eigenvalues[0] = scaledMatrix.x.x;
            eigenvalues[1] = scaledMatrix.y.y;
            eigenvalues[2] = scaledMatrix.z.z;

            eigenvectors[0] = vec3(1.0f, 0.0f, 0.0f);
            eigenvectors[1] = vec3(0.0f, 1.0f, 0.0f);
            eigenvectors[2] = vec3(0.0f, 0.0f, 1.0f);
        } else if (xyIsZero && xzIsZero) {
            const float halfyyMinuszz =
                (scaledMatrix.y.y - scaledMatrix.z.z) / 2.0f;

            eigenvalues[0] = scaledMatrix.x.x;
            eigenvalues[1] = ((scaledMatrix.y.y + scaledMatrix.z.z) / 2.0f) +
                             std::sqrt((halfyyMinuszz * halfyyMinuszz) +
                                       (scaledMatrix.y.z * scaledMatrix.y.z));
            eigenvalues[2] = ((scaledMatrix.y.y + scaledMatrix.z.z) / 2.0f) -
                             std::sqrt((halfyyMinuszz * halfyyMinuszz) +
                                       (scaledMatrix.y.z * scaledMatrix.y.z));

            const float byy1 = scaledMatrix.y.y - eigenvalues[1];
            const float byy2 = scaledMatrix.y.y - eigenvalues[2];

            eigenvectors[0] = vec3(1.0f, 0.0f, 0.0f);
            eigenvectors[1] =
                vec3(0.0f,
                     -(scaledMatrix.y.z /
                       std::sqrt((byy1 * byy1) +
                                 (scaledMatrix.y.z * scaledMatrix.y.z))),
                     (scaledMatrix.y.y /
                      std::sqrt((byy1 * byy1) +
                                (scaledMatrix.y.z * scaledMatrix.y.z))));
            eigenvectors[2] =
                vec3(0.0f,
                     -(scaledMatrix.y.y /
                       std::sqrt((byy2 * byy2) +
                                 (scaledMatrix.y.z * scaledMatrix.y.z))),
                     -(scaledMatrix.y.z /
                       std::sqrt((byy2 * byy2) +
                                 (scaledMatrix.y.z * scaledMatrix.y.z))));
        } else if (xyIsZero && yzIsZero) {
            const float halfxxMinuszz =
                (scaledMatrix.x.x - scaledMatrix.z.z) / 2.0f;

            eigenvalues[0] = ((scaledMatrix.x.x + scaledMatrix.z.z) / 2.0f) +
                             std::sqrt((halfxxMinuszz * halfxxMinuszz) +
                                       (scaledMatrix.x.z * scaledMatrix.x.z));
            eigenvalues[1] = scaledMatrix.y.y;
            eigenvalues[2] = ((scaledMatrix.x.x + scaledMatrix.z.z) / 2.0f) -
                             std::sqrt((halfxxMinuszz * halfxxMinuszz) +
                                       (scaledMatrix.x.z * scaledMatrix.x.z));

            const float bxx0 = scaledMatrix.x.x - eigenvalues[0];
            const float bxx2 = scaledMatrix.x.x - eigenvalues[2];

            eigenvectors[0] =
                vec3(-(scaledMatrix.x.z /
                       std::sqrt((bxx0 * bxx0) +
                                 (scaledMatrix.x.z * scaledMatrix.x.z))),
                     0.0f,
                     (bxx0 / std::sqrt((bxx0 * bxx0) +
                                       (scaledMatrix.x.z * scaledMatrix.x.z))));
            eigenvectors[1] = vec3(0.0f, 1.0f, 0.0f);
            eigenvectors[2] =
                vec3((bxx2 / std::sqrt((bxx2 * bxx2) +
                                       (scaledMatrix.x.z * scaledMatrix.x.z))),
                     0.0f,
                     (scaledMatrix.x.z /
                      std::sqrt((bxx2 * bxx2) +
                                (scaledMatrix.x.z * scaledMatrix.x.z))));
        } else if (yzIsZero && xzIsZero) {
            const float halfxxMinusyy =
                (scaledMatrix.x.x - scaledMatrix.y.y) / 2.0f;

            eigenvalues[0] = ((scaledMatrix.x.x + scaledMatrix.y.y) / 2.0f) +
                             std::sqrt((halfxxMinusyy * halfxxMinusyy) +
                                       (scaledMatrix.x.y * scaledMatrix.x.y));
            eigenvalues[1] = ((scaledMatrix.x.x + scaledMatrix.y.y) / 2.0f) -
                             std::sqrt((halfxxMinusyy * halfxxMinusyy) +
                                       (scaledMatrix.x.y * scaledMatrix.x.y));
            eigenvalues[2] = scaledMatrix.z.z;

            const float bxx0 = scaledMatrix.x.x - eigenvalues[0];
            const float bxx1 = scaledMatrix.x.x - eigenvalues[1];

            eigenvectors[0] =
                vec3(-(scaledMatrix.x.y /
                       std::sqrt((bxx0 * bxx0) +
                                 (scaledMatrix.x.y * scaledMatrix.x.y))),
                     (bxx0 / std::sqrt((bxx0 * bxx0) +
                                       (scaledMatrix.x.y * scaledMatrix.x.y))),
                     0.0f);
            eigenvectors[1] =
                vec3(-(bxx1 / std::sqrt((bxx1 * bxx1) +
                                        (scaledMatrix.x.y * scaledMatrix.x.y))),
                     -(scaledMatrix.x.y /
                       std::sqrt((bxx1 * bxx1) +
                                 (scaledMatrix.x.y * scaledMatrix.x.y))),
                     0.0f);
            eigenvectors[2] = vec3(0.0f, 0.0f, 1.0f);
        } else { // General case
            const float alpha =
                scaledMatrix.x.x + scaledMatrix.y.y + scaledMatrix.z.z;
            const float beta = (scaledMatrix.x.y * scaledMatrix.x.y) +
                               (scaledMatrix.x.z * scaledMatrix.x.z) +
                               (scaledMatrix.y.z * scaledMatrix.y.z) -
                               (scaledMatrix.x.x * scaledMatrix.y.y) -
                               (scaledMatrix.y.y * scaledMatrix.z.z) -
                               (scaledMatrix.z.z * scaledMatrix.x.x);
            const float gamma =
                (scaledMatrix.x.x * scaledMatrix.y.y * scaledMatrix.z.z) +
                (2.0f * scaledMatrix.x.y * scaledMatrix.y.z *
                 scaledMatrix.x.z) -
                (scaledMatrix.x.x * scaledMatrix.y.z * scaledMatrix.y.z) -
                (scaledMatrix.x.y * scaledMatrix.x.y * scaledMatrix.z.z) -
                (scaledMatrix.x.z * scaledMatrix.x.z * scaledMatrix.y.y);

            const float alphaOver3 = alpha / 3.0f;

            const float p = -(((3.0f * beta) + (alpha * alpha)) / 3.0f);
            const float q = -(gamma + ((2.0f * alpha * alpha * alpha) / 27.0f) +
                              ((alpha * beta) / 3.0f));

            const float pOver3 = std::abs(p) / 3.0f;

            const float theta =
                std::acos(-(q / (2.0f * std::sqrt(pOver3 * pOver3 * pOver3))));

            eigenvalues[0] = alphaOver3 + (2.0f * std::sqrt(pOver3) *
                                           std::cos(theta / 3.0f));
            eigenvalues[1] = alphaOver3 - (2.0f * std::sqrt(pOver3) *
                                           std::cos((theta - PI) / 3.0f));
            eigenvalues[2] = alphaOver3 - (2.0f * std::sqrt(pOver3) *
                                           std::cos((theta + PI) / 3.0f));

            for (uint8_t i = 0; i < 2; i++) {
                const mat3 eigenvalueMatrix =
                    mat3(vec3(eigenvalues[i], 0.0f, 0.0f),
                         vec3(0.0f, eigenvalues[i], 0.0f),
                         vec3(0.0f, 0.0f, eigenvalues[i]));
                const mat3 b = scaledMatrix - eigenvalueMatrix;

                const float case1One =
                    ((b.x.x * b.y.z) - (b.x.z * b.x.y)) * b.x.z;
                const float case1Two =
                    ((b.x.y * b.x.y) - (b.x.x * b.y.y)) * b.x.z;
                const bool case1OneIsZero =
                    (case1One > -epsilon) && (case1One < epsilon);
                const bool case1TwoIsZero =
                    (case1Two > -epsilon) && (case1Two < epsilon);

                const float case2One =
                    ((b.x.x * b.z.z) - (b.x.z * b.x.z)) * b.x.y;
                const float case2Two =
                    ((b.x.y * b.x.z) - (b.x.x * b.y.z)) * b.x.y;
                const bool case2OneIsZero =
                    (case2One > -epsilon) && (case2One < epsilon);
                const bool case2TwoIsZero =
                    (case2Two > -epsilon) && (case2Two < epsilon);

                const float case3One =
                    ((b.x.y * b.z.z) - (b.y.z * b.x.z)) * b.x.x;
                const float case3Two =
                    ((b.y.y * b.x.z) - (b.x.y * b.y.z)) * b.x.x;
                const bool case3OneIsZero =
                    (case3One > -epsilon) && (case3One < epsilon);
                const bool case3TwoIsZero =
                    (case3Two > -epsilon) && (case3Two < epsilon);

                const float case4One =
                    ((b.x.y * b.y.z) - (b.x.z * b.y.y)) * b.y.z;
                const float case4Two =
                    ((b.x.x * b.y.y) - (b.x.y * b.x.y)) * b.y.z;
                const bool case4OneIsZero =
                    (case4One > -epsilon) && (case4One < epsilon);
                const bool case4TwoIsZero =
                    (case4Two > -epsilon) && (case4Two < epsilon);

                const float case5One =
                    ((b.x.y * b.z.z) - (b.x.z * b.y.z)) * b.y.y;
                const float case5Two =
                    ((b.x.x * b.y.z) - (b.x.y * b.x.z)) * b.y.y;
                const bool case5OneIsZero =
                    (case5One > -epsilon) && (case5One < epsilon);
                const bool case5TwoIsZero =
                    (case5Two > -epsilon) && (case5Two < epsilon);

                const float case6One =
                    ((b.y.y * b.z.z) - (b.y.z * b.y.z)) * b.x.y;
                const float case6Two =
                    ((b.x.y * b.y.z) - (b.y.y * b.x.z)) * b.x.y;
                const bool case6OneIsZero =
                    (case6One > -epsilon) && (case6One < epsilon);
                const bool case6TwoIsZero =
                    (case6Two > -epsilon) && (case6Two < epsilon);

                const float case7One =
                    ((b.x.z * b.y.y) - (b.x.y * b.y.z)) * b.z.z;
                const float case7Two =
                    ((b.x.x * b.y.z) - (b.x.z * b.x.y)) * b.z.z;
                const bool case7OneIsZero =
                    (case7One > -epsilon) && (case7One < epsilon);
                const bool case7TwoIsZero =
                    (case7Two > -epsilon) && (case7Two < epsilon);

                const float case8One =
                    ((b.x.z * b.y.z) - (b.x.y * b.z.z)) * b.y.z;
                const float case8Two =
                    ((b.x.x * b.z.z) - (b.x.z * b.x.z)) * b.y.z;
                const bool case8OneIsZero =
                    (case8One > -epsilon) && (case8One < epsilon);
                const bool case8TwoIsZero =
                    (case8Two > -epsilon) && (case8Two < epsilon);

                const float case9One =
                    ((b.y.z * b.y.z) - (b.y.y * b.z.z)) * b.x.z;
                const float case9Two =
                    ((b.x.y * b.z.z) - (b.y.z * b.x.z)) * b.x.z;
                const bool case9OneIsZero =
                    (case9One > -epsilon) && (case9One < epsilon);
                const bool case9TwoIsZero =
                    (case9Two > -epsilon) && (case9Two < epsilon);

                if (!case1OneIsZero || !case1TwoIsZero) {
                    const float Q = ((b.x.x * b.y.z) - (b.x.z * b.x.y)) /
                                    ((b.x.y * b.x.y) - (b.x.x * b.y.y));
                    const float Pn = -(((b.y.z * Q) + b.z.z) / b.x.z);

                    const float n =
                        1.0f / std::sqrt((Pn * Pn) + (Q * Q) + 1.0f);

                    eigenvectors[i] = vec3(Pn * n, Q * n, n);
                } else if (!case2OneIsZero || !case2TwoIsZero) {
                    const float Q = ((b.x.x * b.z.z) - (b.x.z * b.x.z)) /
                                    ((b.x.y * b.x.z) - (b.x.x * b.y.z));
                    const float Pn = -(((b.y.y * Q) + b.y.z) / b.x.y);

                    const float n =
                        1.0f / std::sqrt((Pn * Pn) + (Q * Q) + 1.0f);

                    eigenvectors[i] = vec3(Pn * n, Q * n, n);
                } else if (!case3OneIsZero || !case3TwoIsZero) {
                    const float Q = ((b.x.y * b.z.z) - (b.y.z * b.x.z)) /
                                    ((b.y.y * b.x.z) - (b.x.y * b.y.z));
                    const float Pn = -(((b.x.y * Q) + b.x.z) / b.x.x);

                    const float n =
                        1.0f / std::sqrt((Pn * Pn) + (Q * Q) + 1.0f);

                    eigenvectors[i] = vec3(Pn * n, Q * n, n);
                } else if (!case4OneIsZero || !case4TwoIsZero) {
                    const float P = ((b.x.y * b.y.z) - (b.x.z * b.y.y)) /
                                    ((b.x.x * b.y.y) - (b.x.y * b.x.y));
                    const float Qn = -(((b.x.z * P) + b.z.z) / b.y.z);

                    const float n =
                        1.0f / std::sqrt((P * P) + (Qn * Qn) + 1.0f);

                    eigenvectors[i] = vec3(P * n, Qn * n, n);
                } else if (!case5OneIsZero || !case5TwoIsZero) {
                    const float P = ((b.x.y * b.z.z) - (b.x.z * b.y.z)) /
                                    ((b.x.x * b.y.z) - (b.x.y * b.x.z));
                    const float Qn = -(((b.x.y * P) + b.y.z) / b.y.y);

                    const float n =
                        1.0f / std::sqrt((P * P) + (Qn * Qn) + 1.0f);

                    eigenvectors[i] = vec3(P * n, Qn * n, n);
                } else if (!case6OneIsZero || !case6TwoIsZero) {
                    const float P = ((b.y.y * b.z.z) - (b.y.z * b.y.z)) /
                                    ((b.x.y * b.y.z) - (b.y.y * b.x.z));
                    const float Qn = -(((b.x.x * P) + b.x.z) / b.x.y);

                    const float n =
                        1.0f / std::sqrt((P * P) + (Qn * Qn) + 1.0f);

                    eigenvectors[i] = vec3(P * n, Qn * n, n);
                } else if (!case7OneIsZero || !case7TwoIsZero) {
                    const float P = ((b.x.z * b.y.y) - (b.x.y * b.y.z)) /
                                    ((b.x.x * b.y.z) - (b.x.z * b.x.y));
                    const float Rm = -(((b.x.z * P) + b.y.z) / b.z.z);

                    const float m =
                        1.0f / std::sqrt((P * P) + 1.0f + (Rm * Rm));

                    eigenvectors[i] = vec3(P * m, m, Rm * m);
                } else if (!case8OneIsZero || !case8TwoIsZero) {
                    const float P = ((b.x.z * b.y.z) - (b.x.y * b.z.z)) /
                                    ((b.x.x * b.z.z) - (b.x.z * b.x.z));
                    const float Rm = -(((b.x.y * P) + b.y.y) / b.y.z);

                    const float m =
                        1.0f / std::sqrt((P * P) + 1.0f + (Rm * Rm));

                    eigenvectors[i] = vec3(P * m, m, Rm * m);
                } else if (!case9OneIsZero || !case9TwoIsZero) {
                    const float P = ((b.y.z * b.y.z) - (b.y.y * b.z.z)) /
                                    ((b.x.y * b.z.z) - (b.y.z * b.x.z));
                    const float Rm = -(((b.x.x * P) + b.x.y) / b.x.z);

                    const float m =
                        1.0f / std::sqrt((P * P) + 1.0f + (Rm * Rm));

                    eigenvectors[i] = vec3(P * m, m, Rm * m);
                }
            }

            eigenvectors[2] = eigenvectors[0].cross(eigenvectors[1]);
        }

        eigenvalues[0] *= scale;
        eigenvalues[0] += shift;
        eigenvalues[1] *= scale;
        eigenvalues[1] += shift;
        eigenvalues[2] *= scale;
        eigenvalues[2] += shift;
    }

    float *data() { return x.data(); }

    static mat3 identity() {
        return mat3(1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f);
    }

    mat3 operator+(const mat3 &rhs) const {
        mat3 lhs = *this;
        lhs += rhs;

        return lhs;
    }

    mat3 operator-(const mat3 &rhs) const {
        mat3 lhs = *this;
        lhs -= rhs;

        return lhs;
    }

    mat3 operator*(const mat3 &rhs) const {
        mat3 lhs = *this;
        lhs *= rhs;

        return lhs;
    }

    vec3 operator*(const vec3 &rhs) const {
        mat3 lhs = *this;
        return vec3(lhs.x.x * rhs.x + lhs.y.x * rhs.y + lhs.z.x * rhs.z,
                    lhs.x.y * rhs.x + lhs.y.y * rhs.y + lhs.z.y * rhs.z,
                    lhs.x.z * rhs.x + lhs.y.z * rhs.y + lhs.z.z * rhs.z);
    }

    mat3 operator*(const float rhs) const {
        mat3 lhs = *this;
        lhs *= rhs;

        return lhs;
    }

    mat3 operator/(const float rhs) const {
        mat3 lhs = *this;
        lhs /= rhs;

        return lhs;
    }

    bool operator==(const mat3 &rhs) const {
        return ((x == rhs.x) && (y == rhs.y) && (z == rhs.z));
    }

    bool operator!=(const mat3 &rhs) const {
        return x != rhs.x || y != rhs.y || z != rhs.z;
    }

    mat3 transpose() const {
        return mat3(x.x, y.x, z.x, x.y, y.y, z.y, x.z, y.z, z.z);
    }

    mat3 inverse() const {
        const float determinant = det();

        const mat3 t = transpose();
        const float a = mat2(t.y.y, t.y.z, t.z.y, t.z.z).det();
        const float b = mat2(t.y.x, t.y.z, t.z.x, t.z.z).det() * -1.0f;
        const float c = mat2(t.y.x, t.y.y, t.z.x, t.z.y).det();
        const float d = mat2(t.x.y, t.x.z, t.z.y, t.z.z).det() * -1.0f;
        const float e = mat2(t.x.x, t.x.z, t.z.x, t.z.z).det();
        const float f = mat2(t.x.x, t.x.y, t.z.x, t.z.y).det() * -1.0f;
        const float g = mat2(t.x.y, t.x.z, t.y.y, t.y.z).det();
        const float h = mat2(t.x.x, t.x.z, t.y.x, t.y.z).det() * -1.0f;
        const float i = mat2(t.x.x, t.x.y, t.y.x, t.y.y).det();

        const mat3 adj = mat3(a, b, c, d, e, f, g, h, i);

        return adj * (1.0f / determinant);
    }

    static mat3 translate(const vec2 &translation) {
        return mat3(1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, translation.x,
                    translation.y, 1.0f);
    }

    static mat3 rotate(const float angle) {
        const float cosTheta = std::cos(angle);
        const float sinTheta = std::sin(angle);

        return mat3(cosTheta, sinTheta, 0.0f, -sinTheta, cosTheta, 0.0f, 0.0f,
                    0.0f, 1.0f);
    }

    static mat3 scale(const vec2 &scaling) {
        return mat3(scaling.x, 0.0f, 0.0f, 0.0f, scaling.y, 0.0f, 0.0f, 0.0f,
                    1.0f);
    }

    /*std::string to_string(const mat3& mat) {
        return ("[" + to_string(mat.x) + ", " + to_string(mat.y) + ", " +
    to_string(mat.z) + "]");
    }*/
};
