/* 卫星数据结构和轨道计算 */

#ifndef SATELLITE_H
#define SATELLITE_H

#include "types.h"
#include "vector3.h"
#include "quaternion.h"

/* ==================== 卫星创建和销毁 ==================== */

/* 创建卫星 */
Satellite* satellite_create(int id, uint8_t team, uint8_t type, uint8_t function_type);

/* 创建卫星（完整参数） */
Satellite* satellite_create_full(
    int id, uint8_t type, uint8_t function_type,
    double a, double e, double i, 
    double omega_big, double omega_small, double m0,
    double fuel
);

/* 销毁卫星 */
void satellite_destroy(Satellite *sat);

/* ==================== 轨道计算 ==================== */

/* 轨道六根数转换为位置和速度 */
int satellite_orbital_elements_to_state(Satellite *sat);

/* 位置和速度转换为轨道六根数 */
int satellite_state_to_orbital_elements(Satellite *sat);

/* 计算卫星的平均角速度 (度/秒) */
double satellite_calculate_mean_motion(Satellite *sat);

/* 更新卫星位置（基于当前轨道参数和时间） */
int satellite_update_position(Satellite *sat, double dt_minutes);

/* 求解Kepler方程 */
double satellite_solve_kepler_equation(double M, double e, double tolerance);

/* ==================== 相对轨道计算 ==================== */

/* 计算两颗卫星之间的距离 */
double satellite_distance_to(Satellite *sat1, Satellite *sat2);

/* 计算卫星到目标点的距离 */
double satellite_distance_to_point(Satellite *sat, Vector3 target);

/* 获取指向目标的方向向量 */
Vector3 satellite_direction_to_target(Satellite *sat, Satellite *target);

/* ==================== 轨道机动计算 ==================== */

/* 计算Hohmann转移的速度增量 */
double satellite_calculate_hohmann_delta_v(double a1, double a2);

/* 计算变轨所需燃料 */
double satellite_calculate_fuel_for_maneuver(Satellite *sat, double delta_v);

/* 检查卫星燃料是否足够进行机动 */
int satellite_has_sufficient_fuel(Satellite *sat, double delta_v);

/* ==================== 轨道参数计算 ===================== */

/* 计算轨道能量 */
double satellite_orbital_energy(Satellite *sat);

/* 计算轨道周期 (秒) */
double satellite_orbital_period(Satellite *sat);

/* 计算特征能量 */
double satellite_specific_orbital_energy(Satellite *sat);

/* 计算近地点和远地点 */
void satellite_calculate_periapsis_apoapsis(
    Satellite *sat, 
    double *periapsis,
    double *apoapsis
);

/* ==================== 太阳照射 ==================== */

/* 计算太阳高度角 */
double satellite_calculate_sun_elevation(Satellite *sat);

/* 检查与另一卫星的相对照射情况 */
int satellite_check_illumination(Satellite *observer, Satellite *target);

/* ==================== 历史轨迹管理 ==================== */

// /* 初始化历史缓冲 */
// int satellite_history_init(Satellite *sat, int capacity);

/* 初始化历史（兼容函数） */
void satellite_init_history(Satellite *sat, int capacity);

/* 记录当前位置到历史 */
int satellite_history_append(Satellite *sat);

/* 获取历史数据 */
Vector3 satellite_history_get_position(Satellite *sat, int index);

/* 清空历史 */
void satellite_history_clear(Satellite *sat);

/* ==================== 编队状态管理 ==================== */

/* 设置编队类型 */
void satellite_set_formation(Satellite *sat, uint8_t formation_type);

/* 获取当前编队类型 */
uint8_t satellite_get_formation(Satellite *sat);

/* 设置目标卫星ID */
void satellite_set_target(Satellite *sat, int target_id);

/* 更新圈数计数器 */
void satellite_update_circle_count(Satellite *sat, uint32_t count);

/* 更新圈进度 */
void satellite_update_circle_progress(Satellite *sat, double progress);

/* 更新距离信息 */
void satellite_update_distance_info(
    Satellite *sat,
    double distance_to_target,
    double min_distance_reached
);

/* 标记侦查状态 */
void satellite_set_inspection_started(Satellite *sat, int started);

/* ==================== 燃料管理 ==================== */

/* 消耗燃料 */
int satellite_consume_fuel(Satellite *sat, double delta_v);

/* 获取剩余燃料百分比 */
double satellite_get_fuel_percentage(Satellite *sat);

/* ==================== 调试和信息 ==================== */

/* 打印卫星信息 */
void satellite_print_info(Satellite *sat);

/* 打印卫星轨道参数 */
void satellite_print_orbital_elements(Satellite *sat);

/* 打印卫星状态向量 */
void satellite_print_state(Satellite *sat);

/* 输出卫星状态到JSON格式字符串 */
char* satellite_to_json_string(Satellite *sat);

/* ==================== 便利函数 ==================== */

/* 随机生成卫星初始姿态 */
void satellite_randomize_attitude(Satellite *sat);

/* 设置卫星指向目标 */
void satellite_point_at_target(Satellite *sat, Satellite *target);

/* 计算编队质量指标 (0-1) */
double satellite_formation_quality(Satellite *sat);

#endif /* SATELLITE_H */
