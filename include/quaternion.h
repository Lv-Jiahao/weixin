#ifndef QUATERNION_H
#define QUATERNION_H

#include "constants.h"
#include "types.h"
#include "vector3.h"
#include <math.h>


/* ==================== 四元数构造 ==================== */

/* 创建四元数 (x, y, z, w) */
static inline Quaternion quat_create(double x, double y, double z, double w) {
    return (Quaternion){x, y, z, w};
}

/* 单位四元数 (恒等旋转) */
static inline Quaternion quat_identity(void) {
    return (Quaternion){0.0, 0.0, 0.0, 1.0};
}

/* ==================== 四元数规范化 ==================== */

/* 四元数规范化: q' = q / ||q|| */
Quaternion quat_normalize(Quaternion q);

/* 四元数规范化（安全版本） */
Quaternion quat_normalize_safe(Quaternion q);

/* ==================== 四元数基本操作 ==================== */

/* 四元数共轭: q* = (-x, -y, -z, w) */
static inline Quaternion quat_conjugate(Quaternion q) {
    return (Quaternion){-q.x, -q.y, -q.z, q.w};
}

/* 四元数范数: ||q|| */
static inline double quat_magnitude(Quaternion q) {
    return sqrt(q.x * q.x + q.y * q.y + q.z * q.z + q.w * q.w);
}

/* 四元数逆: q⁻¹ = q* / ||q||² */
static inline Quaternion quat_inverse(Quaternion q) {
    double norm_sq = q.x * q.x + q.y * q.y + q.z * q.z + q.w * q.w;
    if (norm_sq < 1e-12) {
        return quat_identity();
    }
    Quaternion conj = quat_conjugate(q);
    return (Quaternion){
        conj.x / norm_sq, conj.y / norm_sq, conj.z / norm_sq, conj.w / norm_sq
    };
}

/* ==================== 四元数乘法 ==================== */

/* 四元数乘法: q1 * q2 */
Quaternion quat_multiply(Quaternion q1, Quaternion q2);

/* 四元数与向量乘法（作为四元数）: q * [v, 0] */
Quaternion quat_multiply_vector(Quaternion q, Vector3 v);

/* ==================== 四元数与向量的关系 ==================== */

/* 向量旋转: v' = q * [v, 0] * q* */
Vector3 quat_rotate_vector(Quaternion q, Vector3 v);

/* 从两个向量构造四元数: 旋转使a转向b */
Quaternion quat_from_two_vectors(Vector3 a, Vector3 b);

/* 从轴和角度构造四元数 */
Quaternion quat_from_axis_angle(Vector3 axis, double angle);

/* 从欧拉角构造四元数 (ZYX约定：yaw-pitch-roll) */
Quaternion quat_from_euler_zyx(double yaw, double pitch, double roll);

/* 从欧拉角构造四元数 (XYZ约定) */
Quaternion quat_from_euler_xyz(double roll, double pitch, double yaw);

/* ==================== 四元数转换为其他表示 ==================== */

/* 从四元数提取轴和角度 */
void quat_to_axis_angle(Quaternion q, Vector3 *axis, double *angle);

/* 从四元数提取欧拉角 (ZYX约定) */
void quat_to_euler_zyx(Quaternion q, double *yaw, double *pitch, double *roll);

/* 从四元数提取欧拉角 (XYZ约定) */
void quat_to_euler_xyz(Quaternion q, double *roll, double *pitch, double *yaw);

/* 从四元数提取旋转矩阵 */
Matrix3x3 quat_to_matrix(Quaternion q);

/* 从旋转矩阵构造四元数 */
Quaternion quat_from_matrix(Matrix3x3 m);

/* ==================== 四元数插值 ==================== */

/* 线性插值: (1-t)q1 + t*q2 */
Quaternion quat_lerp(Quaternion q1, Quaternion q2, double t);

/* 球面线性插值 (SLERP): 使用二阶插值保持恒定角速度 */
Quaternion quat_slerp(Quaternion q1, Quaternion q2, double t);

/* ==================== 四元数微分 ==================== */

/* 四元数导数: dq/dt = 0.5 * [omega, 0] * q */
Quaternion quat_derivative(Quaternion q, Vector3 omega);

/* 从四元数和角速度积分一个时间步: q(t+dt) */
Quaternion quat_integrate(Quaternion q, Vector3 omega, double dt);

/* ==================== 四元数距离和角度 ==================== */

/* 四元数间的角距离 (弧度) */
double quat_angular_distance(Quaternion q1, Quaternion q2);

/* 两个四元数的夹角 (弧度) */
double quat_angle_between(Quaternion q1, Quaternion q2);

/* ==================== 实用工具 ==================== */

/* 打印四元数（调试用） */
void quat_print(const char *name, Quaternion q);

/* 检验四元数是否规范化 */
int quat_is_normalized(Quaternion q);

#endif /* QUATERNION_H */
