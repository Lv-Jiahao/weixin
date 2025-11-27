#ifndef FORMATION_BASE_H
#define FORMATION_BASE_H

#include "types.h"
#include "satellite.h"

/* ==================== 编队接口 ==================== */

/* 编队初始化 */
typedef int (*formation_init_func)(Formation *formation);

/* 编队更新（单步） */
typedef int (*formation_update_func)(Formation *formation, Satellite *chaser, Satellite *target);

/* 编队状态检查 */
typedef int (*formation_check_status_func)(Formation *formation, Satellite *chaser);

/* 编队销毁 */
typedef void (*formation_destroy_func)(Formation *formation);

/* ==================== 编队创建和销毁 ==================== */

/* 创建通用编队控制器 */
Formation* formation_create(
    const char *name,
    formation_init_func init,
    formation_update_func update,
    formation_check_status_func check_status,
    formation_destroy_func destroy
);

/* 销毁编队控制器 */
void formation_destroy(Formation *formation);

/* ==================== 编队执行 ==================== */

/* 初始化编队 */
int formation_init(Formation *formation);

/* 更新编队（单步） */
int formation_update(Formation *formation, Satellite *chaser, Satellite *target);

/* 检查编队状态 */
int formation_check_status(Formation *formation, Satellite *chaser);

/* ==================== 私有数据管理 ==================== */

/* 设置私有数据 */
void formation_set_private_data(Formation *formation, void *data);

/* 获取私有数据 */
void* formation_get_private_data(Formation *formation);

/* ==================== 编队类型工厂 ==================== */

/* 创建指定类型的编队控制器 */
Formation* formation_create_by_type(uint8_t formation_type);

/* 获取编队类型名称 */
const char* formation_get_type_name(uint8_t formation_type);

/* ==================== 编队通用参数 ==================== */

/* 编队控制数据结构 */
typedef struct {
    /* 状态机 */
    int state;                 // 0=初始化, 1=接近, 2=维持, 3=完成
    double phase_start_time;   // 当前阶段开始时间
    double phase_timer;        // 阶段计时器
    
    /* 轨道参数 */
    OrbitalElements initial_elements;
    OrbitalElements target_elements;
    HohmannTransfer hohmann_params;
    LambertTrajectory lambert_params;
    
    /* 编队参数 */
    double target_distance;    // 目标距离 (km)
    double min_distance;       // 最小距离 (km)
    double max_distance;       // 最大距离 (km)
    
    /* 计数器 */
    int circle_count;          // 完成的圈数
    double circle_progress;    // 当前圈进度 (0-1)
    int inspection_started;    // 侦查已开始
    
    /* 历史记录 */
    double *distance_history;  // 距离历史
    int distance_history_count;
    int distance_history_capacity;
} FormationControlData;

/* ==================== 编队工具函数 ==================== */

/* 计算编队接近度 (0-1) */
double formation_calculate_proximity(
    Satellite *chaser,
    Satellite *target,
    double target_distance
);

/* 计算编队质量指标 (0-1) */
double formation_calculate_quality(
    Satellite *chaser,
    Satellite *target,
    double target_distance
);

/* 检查是否接近目标 */
int formation_is_approaching_target(
    Satellite *chaser,
    Satellite *target,
    double threshold_distance
);

/* 检查是否在编队距离内 */
int formation_is_in_formation_distance(
    Satellite *chaser,
    Satellite *target,
    double min_distance,
    double max_distance
);

/* 计算相对速度 */
Vector3 formation_relative_velocity(
    Satellite *chaser,
    Satellite *target
);

/* 计算相对位置 */
Vector3 formation_relative_position(
    Satellite *chaser,
    Satellite *target
);

/* ==================== 编队状态管理 ==================== */

/* 创建编队控制数据 */
FormationControlData* formation_control_data_create(void);

/* 销毁编队控制数据 */
void formation_control_data_destroy(FormationControlData *data);

/* 重置编队控制数据 */
void formation_control_data_reset(FormationControlData *data);

/* 初始化距离历史缓冲 */
int formation_distance_history_init(FormationControlData *data, int capacity);

/* 记录距离到历史 */
int formation_distance_history_append(FormationControlData *data, double distance);

/* ==================== 编队转换 ==================== */

/* 检查编队转换条件 */
int formation_check_transition_condition(
    Satellite *chaser,
    Satellite *target,
    uint8_t from_formation,
    uint8_t to_formation
);

/* 获取转换所需最少时间 */
double formation_transition_min_time(
    uint8_t from_formation,
    uint8_t to_formation
);

/* ==================== 调试和信息 ==================== */

/* 打印编队信息 */
void formation_print_info(Formation *formation);

/* 打印编队控制数据 */
void formation_control_data_print(const char *name, FormationControlData *data);

/* 获取编队状态描述 */
const char* formation_get_state_description(int state);

#endif /* FORMATION_BASE_H */