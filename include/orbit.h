#ifndef ORBIT_H
#define ORBIT_H

#include "types.h"
#include "vector3.h"
#include "constants.h"

/* ==================== 轨道六根数转换 ==================== */

/* 轨道六根数转换为位置和速度（PQW坐标系 -> ECI坐标系） */
int orbit_elements_to_state(
    OrbitalElements *elements,
    StateVector *state
);

/* 位置和速度转换为轨道六根数 */
int orbit_state_to_elements(
    StateVector *state,
    OrbitalElements *elements
);

/* ==================== Kepler方程求解 ==================== */

/* 求解Kepler方程: E - e*sin(E) = M */
double orbit_solve_kepler_equation(double M, double e, double tolerance);

/* 从偏近地点角E计算真近地点角nu */
double orbit_eccentric_anomaly_to_true_anomaly(double E, double e);

/* 从真近地点角nu计算偏近地点角E */
double orbit_true_anomaly_to_eccentric_anomaly(double nu, double e);

/* 从平近点角M计算真近地点角nu */
double orbit_mean_anomaly_to_true_anomaly(double M, double e);

/* ==================== 轨道参数计算 ==================== */

/* 计算轨道能量 */
double orbit_specific_energy(double a);

/* 计算轨道周期 (秒) */
double orbit_period(double a);

/* 计算轨道离心向量 */
Vector3 orbit_eccentricity_vector(Vector3 r, Vector3 v);

/* 计算轨道角动量向量 */
Vector3 orbit_angular_momentum_vector(Vector3 r, Vector3 v);

/* 计算轨道倾角 */
double orbit_inclination_from_h(Vector3 h);

/* ==================== Hohmann转移 ==================== */

/* 计算Hohmann转移 */
int orbit_hohmann_transfer(
    double a1,        // 初始轨道半长轴
    double a2,        // 目标轨道半长轴
    HohmannTransfer *transfer
);

/* 计算圆轨道Hohmann转移的速度增量 */
double orbit_hohmann_delta_v(double a1, double a2);

/* 计算Hohmann转移时间 */
double orbit_hohmann_transfer_time(double a1, double a2);

/* ==================== Lambert轨迹求解 ==================== */

/* Lambert轨迹求解（最简版本） */
int orbit_lambert_solve_simple(
    Vector3 r1,           // 起始位置
    Vector3 r2,           // 目标位置
    double tof,           // 转移时间
    double mu,            // 引力常数
    Vector3 *v1,          // 起始速度（输出）
    Vector3 *v2           // 目标速度（输出）
);

/* Lambert轨迹求解（完整版本，支持多圈） */
int orbit_lambert_solve(
    Vector3 r1,           // 起始位置
    Vector3 r2,           // 目标位置
    double tof,           // 转移时间
    int num_revolutions,  // 圆周数 (0=短路径, 1=长路径)
    double mu,            // 引力常数
    double max_iterations, // 最大迭代次数
    double convergence,   // 收敛阈值
    Vector3 *v1,          // 起始速度（输出）
    Vector3 *v2           // 目标速度（输出）
);

/* 计算最优转移时间范围 */
int orbit_lambert_find_optimal_tof(
    Vector3 r1,
    Vector3 r2,
    double tof_min,
    double tof_max,
    double mu,
    double *optimal_tof
);

/* ==================== 轨道外推 ==================== */

/* RK4积分一步（单位时间） */
int orbit_rk4_step(
    StateVector *state,
    double dt,
    Vector3 *acceleration  // 可选的非万有引力加速度
);

/* 从当前状态外推到未来时刻 */
int orbit_propagate(
    StateVector *initial_state,
    double propagation_time,
    StateVector *final_state,
    double time_step
);

/* ==================== 轨道操纵参数计算 ==================== */

/* 计算速度增量大小 */
double orbit_delta_v_magnitude(Vector3 v1, Vector3 v2);

/* 计算倾角变化所需速度增量 */
double orbit_delta_v_inclination_change(double a, double di);

/* 计算升交点赤经变化所需速度增量 */
double orbit_delta_v_raan_change(double a, double e, double i, double dOmega);

/* 计算偏心率变化所需速度增量 */
double orbit_delta_v_eccentricity_change(double a, double de);

/* ==================== 轨道接近和碰撞检测 ==================== */

/* 计算两条轨道的最近接近距离 */
double orbit_closest_approach_distance(
    OrbitalElements *orb1,
    OrbitalElements *orb2
);

/* 检测两条轨道是否相交 */
int orbit_orbits_intersect(
    OrbitalElements *orb1,
    OrbitalElements *orb2,
    double *intersection_distance
);

/* ==================== 轨道维持 ==================== */

/* 计算轨道衰减率 */
double orbit_decay_rate(double altitude, double ballistic_coefficient);

/* 计算维持轨道所需的轨道升高 */
double orbit_maintenance_delta_v(double decay_rate, double maintenance_period);

/* ==================== 相对轨道 ==================== */

/* 计算相对轨道根数 */
int orbit_calculate_relative_elements(
    OrbitalElements *chief_orb,
    OrbitalElements *deputy_orb,
    OrbitalElements *relative_orb
);

/* 从相对轨道根数计算绝对轨道 */
int orbit_absolute_from_relative(
    OrbitalElements *chief_orb,
    OrbitalElements *relative_orb,
    OrbitalElements *deputy_orb
);

/* ==================== 临时参数计算 ==================== */

/* 从半长轴计算轨道高度 */
static inline double orbit_altitude_from_semimajor_axis(double a) {
    return a - EARTH_RADIUS;
}

/* 从轨道高度计算半长轴 */
static inline double orbit_semimajor_axis_from_altitude(double altitude) {
    return altitude + EARTH_RADIUS;
}

/* 计算圆轨道速度 */
static inline double orbit_circular_velocity(double a) {
    return sqrt(MU / a);
}

/* 计算逃逸速度 */
static inline double orbit_escape_velocity(double a) {
    return sqrt(2.0 * MU / a);
}

/* ==================== 工具函数 ==================== */

/* 打印轨道六根数 */
void orbit_print_elements(const char *name, OrbitalElements *elements);

/* 打印状态向量 */
void orbit_print_state(const char *name, StateVector *state);

/* 打印Hohmann转移参数 */
void orbit_print_hohmann(const char *name, HohmannTransfer *transfer);

#endif /* ORBIT_H */
