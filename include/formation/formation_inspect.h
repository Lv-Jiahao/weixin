#ifndef FORMATION_INSPECT_H
#define FORMATION_INSPECT_H

#include "types.h"

/* ==================== 巡视编队(INSPECT) ==================== */

typedef struct {
    int inspector_id;
    int target_id;
    int phase;  // 1=接近, 2=相位追赶, 3=椭圆巡视
    double initial_distance;
    uint32_t circle_count;
    double circle_progress;
    int inspection_started;
    double min_distance_reached;
} InspectionState;

typedef struct {
    InspectionState *states;
    int num_states;
    int capacity;
} InspectFormationState;

/**
 * 创建巡视编队
 */
InspectFormationState* inspect_formation_create(void);

/**
 * 销毁巡视编队
 */
void inspect_formation_destroy(InspectFormationState *state);

/**
 * 多对一巡视编队
 * @param inspectors 巡视卫星数组
 * @param num_inspectors 巡视卫星数量
 * @param target 目标卫星
 * @param orbital_elements_out 输出轨道参数
 * @param delta_v_out 输出速度增量
 * @return 成功返回0
 */
int inspect_formation_multi_inspect_single(
    Satellite **inspectors,
    int num_inspectors,
    Satellite *target,
    OrbitalElements *orbital_elements_out,
    double *delta_v_out
);

/**
 * 更新巡视阶段
 */
int inspect_formation_update_phase(
    InspectFormationState *state,
    int inspector_id,
    Satellite **satellite_dict,
    int num_sats
);

/**
 * 更新圈数统计
 */
void inspect_formation_update_circle_count(
    InspectFormationState *state,
    int inspector_id,
    Satellite *inspector_sat,
    Satellite *target_sat
);

/**
 * 获取巡视状态
 */
const char* inspect_formation_get_status(
    InspectFormationState *state,
    int sat_id
);

#endif /* FORMATION_INSPECT_H */