#ifndef FORMATION_CIRCUMNAVIGATE_H
#define FORMATION_CIRCUMNAVIGATE_H

#include "types.h"

/* ==================== 环视编队(CIRCUMNAVIGATE) ==================== */

typedef struct {
    int sat_id;
    int target_id;
    int phase;  // 1=Lambert接近, 2=等待, 3=霍曼维持, 4=完成
    double last_distance;
    double maintenance_min;  // 40km
    double maintenance_max;  // 50km
} CircumnavigateState;

typedef struct {
    CircumnavigateState *states;
    int num_states;
    int capacity;
} CircumnavigateFormationState;

/**
 * 创建环视编队
 */
CircumnavigateFormationState* circumnavigate_formation_create(void);

/**
 * 销毁环视编队
 */
void circumnavigate_formation_destroy(CircumnavigateFormationState *state);

/**
 * 单对一环视
 */
int circumnavigate_formation_single(
    Satellite *chaser,
    Satellite *target,
    OrbitalElements *orbital_elements_out,
    double *delta_v_out
);

/**
 * 多对一环视
 */
int circumnavigate_formation_multi(
    Satellite **chasers,
    int num_chasers,
    Satellite *target,
    OrbitalElements *orbital_elements_out,
    double *delta_v_out
);

/**
 * 更新环视状态
 */
int circumnavigate_formation_update(
    CircumnavigateFormationState *state,
    int sat_id,
    Satellite **satellite_dict,
    int num_sats
);

/**
 * 获取环视状态文本
 */
const char* circumnavigate_formation_get_status(
    CircumnavigateFormationState *state,
    int sat_id
);

#endif /* FORMATION_CIRCUMNAVIGATE_H */