#include <vector3.h>
#include <stdio.h>
#include <math.h>

Vector3 vector3_normalize(Vector3 a) {
    double mag = vector3_magnitude(a);
    if (mag < 1e-15) return (Vector3){0.0, 0.0, 0.0};
    return (Vector3){a.x / mag, a.y / mag, a.z / mag};
}

Vector3 vector3_normalize_safe(Vector3 a) {
    double mag = vector3_magnitude(a);
    if (mag < 1e-15) return (Vector3){1.0, 0.0, 0.0};
    return (Vector3){a.x / mag, a.y / mag, a.z / mag};
}

double vector3_angle(Vector3 a, Vector3 b) {
    double mag_a = vector3_magnitude(a);
    double mag_b = vector3_magnitude(b);
    if (mag_a < 1e-15 || mag_b < 1e-15) return 0.0;
    double cos_angle = vector3_dot(a, b) / (mag_a * mag_b);
    cos_angle = (cos_angle > 1.0) ? 1.0 : (cos_angle < -1.0) ? -1.0 : cos_angle;
    return acos(cos_angle);
}

Vector3 vector3_project(Vector3 a, Vector3 b) {
    double b_mag_sq = vector3_magnitude_squared(b);
    if (b_mag_sq < 1e-15) return (Vector3){0.0, 0.0, 0.0};
    double scalar = vector3_dot(a, b) / b_mag_sq;
    return vector3_scale(b, scalar);
}

Vector3 vector3_reflect(Vector3 v, Vector3 n) {
    double dot = vector3_dot(v, n);
    return (Vector3){v.x - 2.0 * dot * n.x, v.y - 2.0 * dot * n.y, v.z - 2.0 * dot * n.z};
}

Vector3 vector3_lerp(Vector3 a, Vector3 b, double t) {
    t = (t < 0.0) ? 0.0 : (t > 1.0) ? 1.0 : t;
    return (Vector3){a.x + t * (b.x - a.x), a.y + t * (b.y - a.y), a.z + t * (b.z - a.z)};
}

// double vector3_distance(Vector3 a, Vector3 b) {
//     Vector3 diff = vector3_sub(a, b);
//     return vector3_magnitude(diff);
// }

Matrix3x3 matrix3x3_identity(void) {
    return (Matrix3x3){{{1.0, 0.0, 0.0}, {0.0, 1.0, 0.0}, {0.0, 0.0, 1.0}}};
}

Vector3 matrix3x3_multiply_vector(Matrix3x3 m, Vector3 v) {
    return (Vector3){
        m.m[0][0] * v.x + m.m[0][1] * v.y + m.m[0][2] * v.z,
        m.m[1][0] * v.x + m.m[1][1] * v.y + m.m[1][2] * v.z,
        m.m[2][0] * v.x + m.m[2][1] * v.y + m.m[2][2] * v.z
    };
}

Matrix3x3 matrix3x3_transpose(Matrix3x3 m) {
    return (Matrix3x3){{{m.m[0][0], m.m[1][0], m.m[2][0]}, {m.m[0][1], m.m[1][1], m.m[2][1]}, {m.m[0][2], m.m[1][2], m.m[2][2]}}};
}

Matrix3x3 matrix3x3_rotation_x(double angle) {
    double c = cos(angle), s = sin(angle);
    return (Matrix3x3){{{1.0, 0.0, 0.0}, {0.0, c, -s}, {0.0, s, c}}};
}

Matrix3x3 matrix3x3_rotation_y(double angle) {
    double c = cos(angle), s = sin(angle);
    return (Matrix3x3){{{c, 0.0, s}, {0.0, 1.0, 0.0}, {-s, 0.0, c}}};
}

Matrix3x3 matrix3x3_rotation_z(double angle) {
    double c = cos(angle), s = sin(angle);
    return (Matrix3x3){{{c, -s, 0.0}, {s, c, 0.0}, {0.0, 0.0, 1.0}}};
}

Matrix3x3 matrix3x3_rotation(Vector3 axis, double angle) {
    axis = vector3_normalize(axis);
    double c = cos(angle), s = sin(angle), t = 1.0 - c;
    return (Matrix3x3){{{t*axis.x*axis.x + c, t*axis.x*axis.y - s*axis.z, t*axis.x*axis.z + s*axis.y},
                        {t*axis.x*axis.y + s*axis.z, t*axis.y*axis.y + c, t*axis.y*axis.z - s*axis.x},
                        {t*axis.x*axis.z - s*axis.y, t*axis.y*axis.z + s*axis.x, t*axis.z*axis.z + c}}};
}

Matrix3x3 matrix3x3_multiply(Matrix3x3 a, Matrix3x3 b) {
    Matrix3x3 result = {{{0}}};
    for (int i = 0; i < 3; i++)
        for (int j = 0; j < 3; j++)
            for (int k = 0; k < 3; k++)
                result.m[i][j] += a.m[i][k] * b.m[k][j];
    return result;
}

void vector3_print(const char *name, Vector3 v) {
    printf("%s: (%.6f, %.6f, %.6f)\n", name, v.x, v.y, v.z);
}

void matrix3x3_print(const char *name, Matrix3x3 m) {
    printf("%s:\n", name);
    for (int i = 0; i < 3; i++)
        printf("  [%.6f, %.6f, %.6f]\n", m.m[i][0], m.m[i][1], m.m[i][2]);
}