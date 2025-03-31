// The math library - copyright KAGR LLC, portions copyright Team Nutshell. This library is derived largely from nml - the C++ Nutshell Mathematics Library for Computer Graphics (https://team-nutshell.github.io/nml/). The use of this sorce code is governed by the MIT software license described in the "license.txt" file in this directory.

// a + bi + cj + dk
struct quat {
    float a;
    float b;
    float c;
    float d;

    // Constructors
    quat() : a(0.0f), b(0.0f), c(0.0f), d(0.0f) {}
    quat(float _a, float _b, float _c, float _d) : a(_a), b(_b), c(_c), d(_d) {}
    quat(const float *_ptr)
        : a(*_ptr), b(*(_ptr + 1)), c(*(_ptr + 2)), d(*(_ptr + 3)) {}

    quat &operator+=(const quat &other) {
        a += other.a;
        b += other.b;
        c += other.c;
        d += other.d;

        return *this;
    }

    quat &operator-=(const quat &other) {
        a -= other.a;
        b -= other.b;
        c -= other.c;
        d -= other.d;

        return *this;
    }

    quat &operator*=(const quat &other) {
        const quat tmp(
            (a * other.a) - (b * other.b) - (c * other.c) - (d * other.d),
            (a * other.b) + (b * other.a) + (c * other.d) - (d * other.c),
            (a * other.c) - (b * other.d) + (c * other.a) + (d * other.b),
            (a * other.d) + (b * other.c) - (c * other.b) + (d * other.a));

        a = tmp.a;
        b = tmp.b;
        c = tmp.c;
        d = tmp.d;

        return *this;
    }

    quat &operator*=(const float other) {
        a *= other;
        b *= other;
        c *= other;
        d *= other;

        return *this;
    }

    quat &operator/=(const float other) {
        a /= other;
        b /= other;
        c /= other;
        d /= other;

        return *this;
    }

    quat operator-() const { return quat(-a, -b, -c, -d); }

    float &operator[](size_t index) {
        assert_msg(index < 4, "index out of range");
        if (index == 0) {
            return a;
        } else if (index == 1) {
            return b;
        } else if (index == 2) {
            return c;
        } else {
            return d;
        }
    }

    const float operator[](size_t index) const {
        assert_msg(index < 4, "index out of range");
        if (index == 0) {
            return a;
        } else if (index == 1) {
            return b;
        } else if (index == 2) {
            return c;
        } else {
            return d;
        }
    }

    float length() const {
        return std::sqrt((a * a) + (b * b) + (c * c) + (d * d));
    }

    float *data() { return &a; }

    static quat identity() { return quat(1.0f, 0.0f, 0.0f, 0.0f); }

    quat operator+(const quat &rhs) const {
        quat lhs = *this;
        lhs += rhs;

        return lhs;
    }

    quat operator-(const quat &rhs) const {
        quat lhs = *this;
        lhs -= rhs;

        return lhs;
    }

    quat operator*(const quat &rhs) const {
        quat lhs = *this;
        lhs *= rhs;

        return lhs;
    }

    quat operator*(const float rhs) const {
        quat lhs = *this;
        lhs *= rhs;

        return lhs;
    }

    quat operator/(const float rhs) const {
        quat lhs = *this;
        lhs /= rhs;

        return lhs;
    }

    bool operator==(const quat &rhs) const {
        return ((a == rhs.a) && (b == rhs.b) && (c == rhs.c) && (d == rhs.d));
    }

    bool operator!=(const quat &rhs) const {
        return a != rhs.a || b != rhs.b || c != rhs.c || d != rhs.d;
    }

    static quat conjugate(const quat &qua) {
        return quat(qua.a, -qua.b, -qua.c, -qua.d);
    }

    static quat normalize(const quat &qua) {
        const float l = qua.length();

        return (qua / l);
    }

    float dot(const quat &qb) const {
        return ((a * qb.a) + (b * qb.b) + (c * qb.c) + (d * qb.d));
    }

    quat slerp(const quat &qa, const quat &qb, const float interpolationValue) {
        quat tmpB = qb;

        float aDotb = qa.dot(qb);

        if (aDotb < 0.0) {
            tmpB = tmpB * -1.0f;
            aDotb = -aDotb;
        }

        if (aDotb > 0.9995) {
            return quat::normalize(qa + (tmpB - qa) * interpolationValue);
        }

        const float theta0 = std::acos(aDotb);
        const float theta = interpolationValue * theta0;
        const float sinTheta0 = std::sin(theta0);
        const float sinTheta = std::sin(theta);

        float scaleA = std::cos(theta) - aDotb * (sinTheta / sinTheta0);
        float scaleB = sinTheta / sinTheta0;

        return (qa * scaleA + tmpB * scaleB);
    }
    
    /*std::string to_string(const quat& qua) {
        return std::to_string(qua.a) + " + " + std::to_string(qua.b) + "i + " +
    std::to_string(qua.c) + "j + " + std::to_string(qua.d) + "k";
    }*/
};
