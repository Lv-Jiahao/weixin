#ifndef VECTOR3_H
#define VECTOR3_H

#include "types.h"
#include <math.h>

/* ==================== 向量构造和基本操作 ==================== */

/* 创建向量 */
static inline Vector3 vector3_create(double x, double y, double z) {
    return (Vector3){x, y, z};
}

/* 零向量 */
static inline Vector3 vector3_zero(void) {
    return (Vector3){0.0, 0.0, 0.0};
}

/* ==================== 向量算术 ==================== */

/* 向量加法: c = a + b */
static inline Vector3 vector3_add(Vector3 a, Vector3 b) {
    return (Vector3){a.x + b.x, a.y + b.y, a.z + b.z};
}

/* 向量减法: c = a - b */
static inline Vector3 vector3_sub(Vector3 a, Vector3 b) {
    return (Vector3){a.x - b.x, a.y - b.y, a.z - b.z};
}

/* 向量标量乘法: c = s * a */
static inline Vector3 vector3_scale(Vector3 a, double s) {
    return (Vector3){a.x * s, a.y * s, a.z * s};
}

/* 向量点积: c = a · b */
static inline double vector3_dot(Vector3 a, Vector3 b) {
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

/* 向量叉积: c = a × b */
static inline Vector3 vector3_cross(Vector3 a, Vector3 b) {
    return (Vector3){
        a.y * b.z - a.z * b.y,
        a.z * b.x - a.x * b.z,
        a.x * b.y - a.y * b.x
    };
}

/* ==================== 向量范数 ==================== */

/* 向量长度: ||a|| */
static inline double vector3_magnitude(Vector3 a) {
    return sqrt(a.x * a.x + a.y * a.y + a.z * a.z);
}

/* 向量长度平方: ||a||² (避免sqrt，更快) */
static inline double vector3_magnitude_squared(Vector3 a) {
    return a.x * a.x + a.y * a.y + a.z * a.z;
}

/* ==================== 向量规范化 ==================== */

/* 向量规范化: c = a / ||a|| */
Vector3 vector3_normalize(Vector3 a);

/* 向量规范化（带安全检查） */
Vector3 vector3_normalize_safe(Vector3 a);

/* ==================== 距离计算 ==================== */

/* 向量间距离 */
static inline double vector3_distance(Vector3 a, Vector3 b) {
    Vector3 diff = vector3_sub(a, b);
    return vector3_magnitude(diff);
}

/* 向量间距离平方 */
static inline double vector3_distance_squared(Vector3 a, Vector3 b) {
    Vector3 diff = vector3_sub(a, b);
    return vector3_magnitude_squared(diff);
}

/* ==================== 高级运算 ==================== */

/* 向量角度: arccos(a·b / (||a|| * ||b||)) */
double vector3_angle(Vector3 a, Vector3 b);

/* 向量投影: proj_b(a) = (a·b / ||b||²) * b */
Vector3 vector3_project(Vector3 a, Vector3 b);

/* 向量反射: r = v - 2(v·n)n */
Vector3 vector3_reflect(Vector3 v, Vector3 n);

/* 线性插值: c = a + t(b - a) */
Vector3 vector3_lerp(Vector3 a, Vector3 b, double t);

/* ==================== 矩阵操作 ==================== */

/* 单位矩阵 */
Matrix3x3 matrix3x3_identity(void);

/* 矩阵-向量乘法: b = M * a */
Vector3 matrix3x3_multiply_vector(Matrix3x3 m, Vector3 v);

/* 矩阵转置 */
Matrix3x3 matrix3x3_transpose(Matrix3x3 m);

/* X轴旋转矩阵 (角度单位: 弧度) */
Matrix3x3 matrix3x3_rotation_x(double angle);

/* Y轴旋转矩阵 */
Matrix3x3 matrix3x3_rotation_y(double angle);

/* Z轴旋转矩阵 */
Matrix3x3 matrix3x3_rotation_z(double angle);

/* 通用旋转矩阵 (Rodrigues公式) */
Matrix3x3 matrix3x3_rotation(Vector3 axis, double angle);

/* 矩阵乘法: C = A * B */
Matrix3x3 matrix3x3_multiply(Matrix3x3 a, Matrix3x3 b);

/* ==================== 实用工具 ==================== */

/* 打印向量（调试用） */
void vector3_print(const char *name, Vector3 v);

/* 打印矩阵（调试用） */
void matrix3x3_print(const char *name, Matrix3x3 m);

#endif /* VECTOR3_H */
