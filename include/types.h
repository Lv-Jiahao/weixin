#ifndef TYPES_H
#define TYPES_H

#include <stdio.h>
#include <math.h>
#include <stdint.h>
#include <time.h>


/* ===================== 系统初始化 卫星数据信息 ===================== */
typedef struct {
    int red_attack, red_recon, red_defense;   //红星 ：攻击、侦查、防御
    int blue_attack, blue_recon, blue_defense;  //蓝星 ：攻击、侦查、防御
    char strategy[32];
    double max_time, time_step;
} Config;



/* ===================== 基础数据类型 ===================== */

/* 三维向量 */
typedef struct {
    double x, y, z;
} Vector3;

/* 3x3矩阵 */
typedef struct {
    double m[3][3];
} Matrix3x3;

/* 四元数 (x, y, z, w) */
typedef struct {
    double x, y, z, w;
} Quaternion;

/* ===================== 轨道参数 ===================== */

/* 轨道六根数 */
typedef struct {
    double a;           // 半长轴 (km)
    double e;           // 偏心率 (0-1)
    double i;           // 轨道倾角 (度)
    double omega_big;   // 升交点赤经Ω (度)
    double omega_small; // 近地点幅角ω (度)
    double m0;          // 平近点角M (度)
} OrbitalElements;

/* 位置和速度状态 */
typedef struct {
    Vector3 position;   // 位置 (km)
    Vector3 velocity;   // 速度 (km/s)
    double time;        // 时间戳 (秒)
} StateVector;

/* ===================== 轨道机动参数 ===================== */

/* Hohmann转移参数 */
typedef struct {
    double delta_v1;           // 第一次燃烧速度增量 (km/s)
    double delta_v2;           // 第二次燃烧速度增量 (km/s)
    double total_delta_v;      // 总速度增量 (km/s)
    double transfer_time;      // 转移时间 (秒)
    Vector3 v1_burn_direction; // 第一次燃烧方向
    Vector3 v2_burn_direction; // 第二次燃烧方向
} HohmannTransfer;

/* Lambert轨迹参数 */
typedef struct {
    Vector3 position_target;   // 目标位置 (km)
    Vector3 velocity_target;   // 目标速度 (km/s)
    double delta_v;            // 速度增量 (km/s)
    double transfer_time;      // 转移时间 (秒)
    int num_revolutions;       // 圆周数 (0=短路径, 1=长路径)
    int converged;             // 收敛标志
} LambertTrajectory;

/* ===================== 姿态参数 ===================== */

/* 姿态跟踪器 */
typedef struct {
    Quaternion q;              // 当前姿态四元数
    Vector3 omega;             // 角速度 (rad/s) - 体坐标系
    Vector3 alpha;             // 角加速度 (rad/s²) - 体坐标系
    double max_rate;           // 最大角速度约束 (rad/s)
    double max_accel;          // 最大角加速度约束 (rad/s²)
    Vector3 body_axis;         // 机体前向轴 (体坐标系)
    int target_id;             // 指向目标ID (-1表示无目标)
    uint32_t step_count;       // 更新步数
} AttitudeTracker;

/* ===================== 卫星数据结构 ===================== */

typedef struct Satellite {
    /* 基本信息 */
    int id;                    // 卫星ID
    uint8_t team;              // 0 红、1 蓝      
    uint8_t type;              // 卫星类型 (HF/LF)
    uint8_t function_type;     // 功能类型 (攻击/侦察/防护)
    
    /* 轨道参数 */
    OrbitalElements orbital_elements;
    StateVector state;         // 当前位置和速度
    
    /* 姿态控制 */
    AttitudeTracker attitude;
    
    /* 燃料和消耗 */
    double fuel;               // 剩余燃料 (kg)
    double fuel_consumption_rate; // 燃料消耗率 (kg/s)
    double total_fuel_used;    // 累计燃料消耗 (kg)
    
    /* 编队信息 */
    uint8_t current_formation; // 当前编队类型
    int target_id;             // 目标卫星ID (-1表示无目标)
    double distance_to_target; // 到目标距离 (km)
    double min_distance_reached; // 到达过的最小距离 (km)
    
    /* 巡视计数器（用于编队阶段追踪）*/
    uint32_t circle_count;     // 完成的圈数
    double circle_progress;    // 当前圈进度 (0-1)
    int inspection_started;    // 是否开始侦查 (0/1)
    
    /* 历史数据 */
    Vector3 *position_history; // 位置历史
    Vector3 *velocity_history; // 速度历史
    int *formation_history;    // 编队类型历史
    int history_count;         // 当前历史点数
    int history_capacity;      // 历史缓冲容量
    
    /* 时间信息 */
    double creation_time;      // 创建时间 (秒)
    double last_maneuver_time; // 最后一次机动时间 (秒)
    
} Satellite;

/* ===================== 编队基类 ===================== */

typedef struct Formation {
    const char *name;
    int (*init)(struct Formation*);
    int (*update)(struct Formation*, Satellite*, Satellite*);
    int (*check_status)(struct Formation*, Satellite*);
    void (*destroy)(struct Formation*);
    void *private_data;        // 编队特有数据
} Formation;

/* 编队控制器私有数据基类 */
typedef struct {
    int state;                 // 当前状态 (0=初始化, 1=接近, 2=绕飞, 3=完成)
    double phase_start_time;   // 当前阶段开始时间
    HohmannTransfer last_hohmann;
    LambertTrajectory last_lambert;
    double phase_timer;        // 阶段计时器
} FormationPrivateData;

/* ===================== 编队管理器 ===================== */

typedef struct {
    int sat_id;
    uint8_t current_formation;
    uint32_t time_in_formation;
    double distance_to_target;
    double min_distance_reached;
    double formation_quality;
    uint32_t circle_count;
    double circle_progress;
    int inspection_started;
    int custom_condition;
} FormationTrigger;

/* ===================== 策略参数 ===================== */

/* 策略类型 */
typedef enum {
    STRATEGY_ATTACK = 0,      // 攻击策略
    STRATEGY_RECON = 1,       // 侦察策略  
    STRATEGY_DEFENSE = 2      // 防御策略
} StrategyType;

/* 卫星功能类型 */
typedef enum {
    FUNCTION_ATTACK = 1,      // 攻击功能
    FUNCTION_RECON = 2,       // 侦察功能
    FUNCTION_DEFENSE = 3      // 防御功能
} SatelliteFunctionType;

typedef struct {
    int attack_distance;
    int inspect_distance;
    int defense_distance;
    int critical_distance;
    int warning_distance;
} StrategyThresholds;

/* ===================== 配置结构 ===================== */

typedef struct {
    /* 仿真参数 */
    double time_step;          // 时间步长 (分钟)
    uint32_t max_steps;        // 最大步数
    uint32_t save_interval;    // 保存间隔
    
    /* 轨道参数 */
    double hohmann_precision;
    uint32_t lambert_max_iterations;
    double lambert_convergence;
    
    /* 编队参数 */
    double lambert_target_distance;
    double lambert_max_delta_v;
    double ellipse_min_distance;
    double ellipse_max_distance;
    
    /* 策略参数 */
    StrategyThresholds strategy;
} SimulationConfig;


/* ===================== 仿真引擎 ===================== */
/* KinematicsEngine定义在kinematics.h中 */


#endif /* TYPES_H */