#ifndef FORMATION_AROUND_H
#define FORMATION_AROUND_H

#include "types.h"

/* ==================== 球形围观编队(AROUND) ==================== */

/**
 * 球形围观编队控制器
 * 用于多个红星围观单个或多个蓝星
 */
typedef struct {
    int sat_ids[100];              // 参与编队的卫星ID
    int num_sats;                  // 编队卫星数
    int target_ids[10];            // 目标卫星ID
    int num_targets;
    int phase[100];                // 每颗卫星的机动阶段
    double phase_start_time[100];  // 每阶段的开始时间
} AroundFormationState;

/**
 * 创建球形围观编队
 */
AroundFormationState* around_formation_create(void);

/**
 * 销毁球形围观编队
 */
void around_formation_destroy(AroundFormationState *state);

/**
 * 单对一球形围观
 * @param chaser 追踪卫星
 * @param target 目标卫星
 * @param orbital_elements_out 输出的轨道参数
 * @return 速度增量 (km/s)
 */
double around_formation_single_vs_single(
    Satellite *chaser,
    Satellite *target,
    OrbitalElements *orbital_elements_out
);

/**
 * 多对一球形围观（整个编队围观一个目标）
 * @param chasers 追踪卫星数组
 * @param num_chasers 追踪卫星数量
 * @param target 目标卫星
 * @param orbital_elements_out 输出数组
 * @param delta_v_out 速度增量数组
 * @return 成功返回0
 */
int around_formation_multi_vs_single_sphere(
    Satellite **chasers,
    int num_chasers,
    Satellite *target,
    OrbitalElements *orbital_elements_out,
    double *delta_v_out
);

/**
 * 更新编队阶段
 * @param state 编队状态
 * @param chaser_id 追踪卫星ID
 * @param satellite_dict 所有卫星
 * @param num_sats 卫星总数
 * @return 新的轨道参数或NULL
 */
OrbitalElements* around_formation_update_phase(
    AroundFormationState *state,
    int chaser_id,
    Satellite **satellite_dict,
    int num_sats
);

/**
 * 获取编队状态文本
 */
const char* around_formation_get_status(
    AroundFormationState *state,
    int sat_id
);

#endif /* FORMATION_AROUND_H */