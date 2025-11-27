#ifndef CONSTANTS_H
#define CONSTANTS_H

/* ===================== 物理常数 ===================== */
#define EARTH_RADIUS        6371.0          // 地球半径 (km)
#define EARTH_MASS          5.972e24        // 地球质量 (kg)
#define MU                  398600.4418     // 地心引力常数 (km³/s²)
#define GEO_ALTITUDE        35786.0         // 地球同步轨道高度 (km)
#define GEO_SEMIMAJOR       42164.0         // 地球同步轨道半长轴 (km)

/* ===================== 仿真参数 ===================== */
#define MAX_SATELLITES      100             // 最大卫星数
#define MAX_HISTORY_PER_SAT 10000          // 每颗卫星最大历史长度
#define TIME_STEP_MINUTES   1.0             // 时间步长 (分钟)
#define TIME_STEP_SECONDS   60.0            // 时间步长 (秒)

/* ===================== 轨道机动参数 ===================== */
#define HOHMANN_DEFAULT_PRECISION  1e-10    // Hohmann精度
#define LAMBERT_MAX_ITERATIONS     100      // Lambert最大迭代次数
#define LAMBERT_CONVERGENCE        1e-12    // Lambert收敛条件
#define RK4_TIME_STEP              1.0      // RK4积分步长 (秒)

/* ===================== 编队控制参数 ===================== */
#define LAMBERT_TARGET_DISTANCE    50.0     // Lambert目标距离 (km)
#define LAMBERT_MAX_DELTA_V        0.100    // Lambert最大速度增量 (km/s)
#define LAMBERT_TOF_MIN_FACTOR     0.15     // Lambert最小转移时间系数
#define LAMBERT_TOF_MAX_FACTOR     0.40     // Lambert最大转移时间系数

#define ELLIPSE_MIN_DISTANCE       50.0     // 椭圆绕飞最小距离 (km)
#define ELLIPSE_MAX_DISTANCE       70.0     // 椭圆绕飞最大距离 (km)
#define CIRCLE_COMPLETE_THRESHOLD  0.95     // 圈数完成阈值 (0-1)

/* ===================== 策略参数 ===================== */
#define ATTACK_DISTANCE    3000.0            // 攻击触发距离 (km)
#define INSPECT_DISTANCE   2000.0            // 侦察触发距离 (km)
#define DEFENSE_DISTANCE   4000.0            // 防护触发距离 (km)
#define CRITICAL_DISTANCE  1000.0            // 紧急距离 (km)
#define WARNING_DISTANCE   5000.0            // 监视距离 (km)

/* ===================== 姿态控制参数 ===================== */
#define ATT_DEFAULT_MAX_RATE   0.01          // 默认最大角速度 (rad/s)
#define ATT_DEFAULT_MAX_ACCEL  0.002         // 默认最大角加速度 (rad/s²)
#define ATT_CONTROL_KP         2.0           // 比例增益
#define ATT_CONTROL_KD         1.0           // 微分增益

/* ===================== 数值精度 ===================== */
#define EPSILON            1e-12             // 最小数值
#define DEG_TO_RAD         3.14159265358979 / 180.0
#define RAD_TO_DEG         180.0 / 3.14159265358979
#define PI                 3.14159265358979323846
#define M_PI                 3.14159265358979323846

/* ===================== 编队类型枚举 ===================== */
typedef enum {
    FORMATION_INSPECT = 0,         // 巡视ZC
    FORMATION_AROUND = 1,          // 环绕HS
    FORMATION_CIRCUMNAVIGATE = 2,  // 环绕HSFY
    FORMATION_RETREAT = 3,         // 撤离CT
    FORMATION_FREE = 4             // 自由飞行
} FormationType;

/* ===================== 卫星类型枚举 ===================== */
typedef enum {
    SAT_TYPE_HF = 0,  // HF卫星
    SAT_TYPE_LF = 1   // LF卫星
} SatelliteType;

/* ===================== 卫星功能类型枚举 ===================== */
// typedef enum {
//     SAT_FUNC_ATTACK = 1,    // 攻击
//     SAT_FUNC_RECON = 2,     // 侦察
//     SAT_FUNC_DEFENSE = 3    // 防护
// } SatelliteFunctionType;

/* ===================== 机动类型枚举 ===================== */
typedef enum {
    MANEUVER_HOHMANN = 0,
    MANEUVER_LAMBERT = 1,
    MANEUVER_INCLINATION = 2,
    MANEUVER_PLANE_CHANGE = 3
} ManeuverType;

/* ===================== 策略类型枚举 ===================== */
// typedef enum {
//     STRATEGY_ATTACK = 0,
//     STRATEGY_INSPECT = 1,
//     STRATEGY_DEFENSE = 2
// } StrategyType;

#endif /* CONSTANTS_H */
