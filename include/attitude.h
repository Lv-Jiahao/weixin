#ifndef ATTITUDE_H
#define ATTITUDE_H

#include "types.h"
#include "vector3.h"
#include "quaternion.h"

/* ==================== 姿态初始化 ==================== */

/* 创建姿态跟踪器 */
AttitudeTracker* attitude_tracker_create(
    double max_rate,
    double max_accel,
    Vector3 body_axis
);

/* 销毁姿态跟踪器 */
void attitude_tracker_destroy(AttitudeTracker *tracker);

/* 初始化姿态跟踪器（已分配的结构） */
int attitude_tracker_init(
    AttitudeTracker *tracker,
    double max_rate,
    double max_accel,
    Vector3 body_axis
);

/* 重置姿态跟踪器 */
void attitude_tracker_reset(AttitudeTracker *tracker);

/* ==================== 姿态更新 ==================== */

/* 单步更新：使机体轴指向目标位置 */
int attitude_tracker_step_towards(
    AttitudeTracker *tracker,
    Vector3 current_pos,
    Vector3 target_pos,
    double dt_sec
);

/* 直接设置姿态 */
void attitude_tracker_set_quaternion(AttitudeTracker *tracker, Quaternion q);

/* 设置指向目标ID */
void attitude_tracker_set_target_id(AttitudeTracker *tracker, int target_id);

/* ==================== 姿态查询 ==================== */

/* 获取当前姿态四元数 */
Quaternion attitude_tracker_get_quaternion(AttitudeTracker *tracker);

/* 获取当前角速度 */
Vector3 attitude_tracker_get_angular_velocity(AttitudeTracker *tracker);

/* 获取机体轴在世界系中的方向 */
Vector3 attitude_tracker_get_body_direction(AttitudeTracker *tracker);

/* 获取前向轴方向 */
Vector3 attitude_tracker_get_forward_direction(AttitudeTracker *tracker);

/* 获取上向轴方向 */
Vector3 attitude_tracker_get_up_direction(AttitudeTracker *tracker);

/* 获取右向轴方向 */
Vector3 attitude_tracker_get_right_direction(AttitudeTracker *tracker);

/* ==================== 四元数运算（高级） ==================== */

/* 从两个向量计算所需旋转四元数 */
Quaternion attitude_quat_from_two_vectors(Vector3 a, Vector3 b);

/* 从轴和角度创建四元数 */
Quaternion attitude_quat_from_axis_angle(Vector3 axis, double angle);

/* 提取轴和角度 */
void attitude_axis_angle_from_quat(
    Quaternion q,
    Vector3 *axis,
    double *angle
);

/* ==================== 欧拉角转换 ==================== */

/* 从欧拉角初始化（ZYX约定：yaw-pitch-roll） */
int attitude_tracker_set_euler_angles_zyx(
    AttitudeTracker *tracker,
    double yaw,
    double pitch,
    double roll
);

/* 提取欧拉角（ZYX约定） */
void attitude_tracker_get_euler_angles_zyx(
    AttitudeTracker *tracker,
    double *yaw,
    double *pitch,
    double *roll
);

/* 从欧拉角初始化（XYZ约定：roll-pitch-yaw） */
int attitude_tracker_set_euler_angles_xyz(
    AttitudeTracker *tracker,
    double roll,
    double pitch,
    double yaw
);

/* 提取欧拉角（XYZ约定） */
void attitude_tracker_get_euler_angles_xyz(
    AttitudeTracker *tracker,
    double *roll,
    double *pitch,
    double *yaw
);

/* ==================== 旋转矩阵 ==================== */

/* 获取旋转矩阵 */
Matrix3x3 attitude_tracker_get_rotation_matrix(AttitudeTracker *tracker);

/* 从旋转矩阵设置姿态 */
int attitude_tracker_set_rotation_matrix(AttitudeTracker *tracker, Matrix3x3 m);

/* ==================== 角速度约束 ==================== */

/* 设置最大角速度 */
void attitude_tracker_set_max_rate(AttitudeTracker *tracker, double max_rate);

/* 设置最大角加速度 */
void attitude_tracker_set_max_accel(AttitudeTracker *tracker, double max_accel);

/* 获取最大角速度 */
double attitude_tracker_get_max_rate(AttitudeTracker *tracker);

/* 获取最大角加速度 */
double attitude_tracker_get_max_accel(AttitudeTracker *tracker);

/* 应用角速度饱和 */
Vector3 attitude_saturate_angular_velocity(
    Vector3 omega,
    double max_rate
);

/* 应用角加速度饱和 */
Vector3 attitude_saturate_angular_acceleration(
    Vector3 alpha,
    double max_accel
);

/* ==================== 姿态指令生成 ==================== */

/* PD控制律：计算所需角速度 */
Vector3 attitude_pd_control(
    Vector3 error_axis,    // 误差轴
    double error_angle,    // 误差角
    double kp,             // 比例增益
    double kd              // 微分增益
);

/* 计算到达姿态所需时间 */
double attitude_time_to_reach(
    Quaternion current,
    Quaternion target,
    double max_rate
);

/* ==================== 调试和信息 ==================== */

/* 打印姿态信息 */
void attitude_tracker_print(const char *name, AttitudeTracker *tracker);

/* 打印四元数 */
void attitude_tracker_print_quat(const char *name, AttitudeTracker *tracker);

/* 检查姿态四元数是否有效 */
int attitude_tracker_is_valid(AttitudeTracker *tracker);

/* ==================== 特殊姿态 ==================== */

/* 设置为NADIR指向（指向地心） */
int attitude_tracker_set_nadir_pointing(AttitudeTracker *tracker, Vector3 position);

/* 设置为ANTI-NADIR指向（指向远地方向） */
int attitude_tracker_set_anti_nadir_pointing(AttitudeTracker *tracker, Vector3 position);

/* 设置为指向太阳 */
int attitude_tracker_set_sun_pointing(AttitudeTracker *tracker);

/* 设置为指向特定方向 */
int attitude_tracker_set_pointing_direction(AttitudeTracker *tracker, Vector3 direction);

#endif /* ATTITUDE_H */
