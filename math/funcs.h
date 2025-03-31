// The math library - copyright KAGR LLC, portions copyright Team Nutshell. This library is derived largely from nml - the C++ Nutshell Mathematics Library for Computer Graphics (https://team-nutshell.github.io/nml/). The use of this sorce code is governed by the MIT software license described in the "license.txt" file in this directory.

static quat eulerAnglesToQuat(const vec3 &eulerAngles) {
    const float cosHalfX = std::cos(eulerAngles.x / 2.0f);
    const float sinHalfX = std::sin(eulerAngles.x / 2.0f);
    const float cosHalfY = std::cos(eulerAngles.y / 2.0f);
    const float sinHalfY = std::sin(eulerAngles.y / 2.0f);
    const float cosHalfZ = std::cos(eulerAngles.z / 2.0f);
    const float sinHalfZ = std::sin(eulerAngles.z / 2.0f);

    return quat(
        cosHalfX * cosHalfY * cosHalfZ - sinHalfX * sinHalfY * sinHalfZ,
        sinHalfX * cosHalfY * cosHalfZ + cosHalfX * sinHalfY * sinHalfZ,
        cosHalfX * sinHalfY * cosHalfZ - sinHalfX * cosHalfY * sinHalfZ,
        cosHalfX * cosHalfY * sinHalfZ + sinHalfX * sinHalfY * cosHalfZ);
}

static quat rotationMatrixToQuat(const mat4 &mat) {
    quat quaternion;

    const float trace = mat.x.x + mat.y.y + mat.z.z;
    if (trace > 0.0f) {
        const float S = std::sqrt(1.0f + trace) * 2.0f;
        quaternion.a = S * 0.25f;
        quaternion.b = (mat.y.z - mat.z.y) / S;
        quaternion.c = (mat.z.x - mat.x.z) / S;
        quaternion.d = (mat.x.y - mat.y.x) / S;
    } else if ((mat.x.x > mat.y.y) && (mat.x.x > mat.z.z)) {
        const float S =
            std::sqrt(1.0f + mat.x.x - mat.y.y - mat.z.z) * 2.0f;
        quaternion.a = (mat.y.z - mat.z.y) / S;
        quaternion.b = S * 0.25f;
        quaternion.c = (mat.y.x + mat.x.y) / S;
        quaternion.d = (mat.z.x + mat.x.z) / S;
    } else if (mat.y.y > mat.z.z) {
        const float S =
            std::sqrt(1.0f + mat.y.y - mat.x.x - mat.z.z) * 2.0f;
        quaternion.a = (mat.z.x - mat.x.z) / S;
        quaternion.b = (mat.y.x + mat.x.y) / S;
        quaternion.c = S * 0.25f;
        quaternion.d = (mat.z.y + mat.y.z) / S;
    } else {
        const float S =
            std::sqrt(1.0f + mat.z.z - mat.x.x - mat.y.y) * 2.0f;
        quaternion.a = (mat.x.y - mat.y.x) / S;
        quaternion.b = (mat.z.x + mat.x.z) / S;
        quaternion.c = (mat.z.y + mat.y.z) / S;
        quaternion.d = S * 0.25f;
    }

    return quaternion;
}


static void decomposeTransform(const mat4 &m, vec3 &translation, quat &rotation, vec3 &scale) {
    translation = vec3(m.w);
    scale = vec3(m.x.length(), m.y.length(), m.z.length());

    const mat4 rotationMatrix =
        mat4(vec4(vec3(m.x) / scale.x, 0.0f), vec4(vec3(m.y) / scale.y, 0.0f),
             vec4(vec3(m.z) / scale.z, 0.0f), vec4(0.0f, 0.0f, 0.0f, 1.0f));
    rotation = rotationMatrixToQuat(rotationMatrix);
}

static float32 clamp(float32 f, float32 min, float32 max)
{
    if (f < min)
        return min;
    if (f > max)
        return max;
    return f;
}
static vec3 matrixToEulerAngles(const mat4 &m) {
    vec3 eulerAngles;
    eulerAngles.y = std::asin(clamp(m.z.x, -1.0f, 1.0f));
    if (std::abs(m.z.x) < (1.0f - std::numeric_limits<float>::epsilon())) {
        eulerAngles.x = std::atan2(-m.z.y, m.z.z);
        eulerAngles.z = std::atan2(-m.y.x, m.x.x);
    } else {
        eulerAngles.x = std::atan2(m.y.z, m.y.y);
        eulerAngles.z = 0.0f;
    }
    return eulerAngles;
}

static mat4 quatToMat4(const quat &qua) {
    const float ab = qua.a * qua.b;
    const float ac = qua.a * qua.c;
    const float ad = qua.a * qua.d;
    const float bb = qua.b * qua.b;
    const float bc = qua.b * qua.c;
    const float bd = qua.b * qua.d;
    const float cc = qua.c * qua.c;
    const float cd = qua.c * qua.d;
    const float dd = qua.d * qua.d;

    return mat4(1.0f - 2.0f * (cc + dd), 2.0f * (bc + ad), 2.0f * (bd - ac),
                0.0f, 2.0f * (bc - ad), 1.0f - 2.0f * (bb + dd),
                2.0f * (cd + ab), 0.0f, 2.0f * (bd + ac), 2.0f * (cd - ab),
                1.0f - 2.0f * (bb + cc), 0.0f, 0.0f, 0.0f, 0.0f, 1.0f);
}

static vec3 quatToEulerAngles(const quat &qua) {
    mat4 rotationMatrix = quatToMat4(qua);
    return matrixToEulerAngles(rotationMatrix);
}
