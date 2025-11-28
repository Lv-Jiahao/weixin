#ifndef FORMATION_RETREAT_H
#define FORMATION_RETREAT_H

#include "types.h"

/* ==================== 撤退编队(RETREAT) ==================== */

typedef struct {
    int sat_id;
    int blue_id;
    int phase;  // 1=脉冲1, 2=滑行, 3=脉冲2, 4=维持
    double a_transfer;
    double e_transfer;
    double r_apoapsis;
    double delta_v2_magnitude;
    double start_distance;
    double total_delta_v;
} RetreatState;

typedef struct {
    RetreatState *states;
    int num_states;
    int capacity;
} RetreatFormationState;

/**
 * 创建撤退编队
 */
RetreatFormationState* retreat_formation_create(void);

/**
 * 销毁撤退编队
 */
void retreat_formation_destroy(RetreatFormationState *state);

/**
 * 启动快速撤退（Hohmann转移）
 */
int retreat_formation_initiate_rapid(
    Satellite *red_sat,
    Satellite *blue_sat,
    OrbitalElements *orbital_elements_out,
    double *delta_v_out
);

/**
 * 启动渐进撤退
 */
int retreat_formation_initiate_gradual(
    Satellite *red_sat,
    Satellite *blue_sat,
    OrbitalElements *orbital_elements_out,
    double *delta_v_out
);

/**
 * 更新撤退阶段
 */
int retreat_formation_update_phase(
    RetreatFormationState *state,
    int sat_id,
    Satellite **satellite_dict,
    int num_sats
);

/**
 * 获取撤退状态
 */
const char* retreat_formation_get_status(
    RetreatFormationState *state,
    int sat_id
);

#endif /* FORMATION_RETREAT_H */