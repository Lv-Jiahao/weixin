#include "formation/formation_circumnavigate.h"
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdio.h>

#define MU 3.986004418e5

static double point_distance(Vector3 p1, Vector3 p2) {
    double dx = p1.x - p2.x;
    double dy = p1. y - p2.y;
    double dz = p1. z - p2.z;
    return sqrt(dx*dx + dy*dy + dz*dz);
}

static double hohmann_delta_v(double a1, double a2) {
    if (fabs(a1 - a2) < 1.0) return 0.001;
    
    double v1 = sqrt(MU / a1);
    double transfer_a = (a1 + a2) / 2.0;
    double v_transfer_1 = sqrt(MU * (2.0 / a1 - 1.0 / transfer_a));
    double delta_v1 = fabs(v_transfer_1 - v1);
    
    double v2 = sqrt(MU / a2);
    double v_transfer_2 = sqrt(MU * (2.0 / a2 - 1.0 / transfer_a));
    double delta_v2 = fabs(v_transfer_2 - v2);
    
    return delta_v1 + delta_v2;
}

CircumnavigateFormationState* circumnavigate_formation_create(void) {
    CircumnavigateFormationState *state = 
        (CircumnavigateFormationState*)malloc(sizeof(CircumnavigateFormationState));
    if (!state) return NULL;
    
    state->capacity = 100;
    state->states = (CircumnavigateState*)calloc(state->capacity, sizeof(CircumnavigateState));
    state->num_states = 0;
    
    if (!state->states) {
        free(state);
        return NULL;
    }
    
    printf("[CIRCUMNAVIGATE] 环视编队已创建\n");
    return state;
}

void circumnavigate_formation_destroy(CircumnavigateFormationState *state) {
    if (!state) return;
    if (state->states) free(state->states);
    free(state);
}

int circumnavigate_formation_single(
    Satellite *chaser,
    Satellite *target,
    OrbitalElements *orbital_elements_out,
    double *delta_v_out) {
    
    if (!chaser || !target) return -1;
    
    printf("[CIRCUMNAVIGATE] 单对一环视: 红星%d → 蓝星%d\n", chaser->id, target->id);
    
    // Lambert接近：瞄准距离目标100km的位置
    double distance = point_distance(chaser->state.position, target->state.position);
    double target_distance = 100000.0;  // 100km
    
    printf("  当前距离: %.1f km\n", distance / 1000.0);
    printf("  目标距离: %.1f km\n", target_distance / 1000.0);
    
    // 简化：使用霍曼转移到目标轨道高度
    double target_a = target->orbital_elements.a;
    double chaser_a = chaser->orbital_elements.a;
    
    *delta_v_out = hohmann_delta_v(chaser_a, target_a);
    memcpy(orbital_elements_out, &target->orbital_elements, sizeof(OrbitalElements));
    orbital_elements_out->e = 0.001;  // 圆轨道维持
    
    printf("  轨道转移: a=%.1f→%.1f km, ΔV=%.4f km/s\n", 
           chaser_a, target_a, *delta_v_out);
    
    return 0;
}

int circumnavigate_formation_multi(
    Satellite **chasers,
    int num_chasers,
    Satellite *target,
    OrbitalElements *orbital_elements_out,
    double *delta_v_out) {
    
    if (!chasers || num_chasers <= 0 || !target) return -1;
    
    printf("[CIRCUMNAVIGATE] 多对一环视: %d个红星 → 蓝星%d\n", num_chasers, target->id);
    
    double target_a = target->orbital_elements.a;
    
    for (int i = 0; i < num_chasers; i++) {
        circumnavigate_formation_single(chasers[i], target, 
                                       &orbital_elements_out[i], &delta_v_out[i]);
    }
    
    printf("[CIRCUMNAVIGATE] 环视编队配置完成\n");
    return 0;
}

int circumnavigate_formation_update(
    CircumnavigateFormationState *state,
    int sat_id,
    Satellite **satellite_dict,
    int num_sats) {
    
    // 查找该卫星的状态
    CircumnavigateState *circ_state = NULL;
    for (int i = 0; i < state->num_states; i++) {
        if (state->states[i].sat_id == sat_id) {
            circ_state = &state->states[i];
            break;
        }
    }
    
    if (!circ_state && state->num_states < state->capacity) {
        circ_state = &state->states[state->num_states];
        circ_state->sat_id = sat_id;
        circ_state->phase = 1;
        circ_state->last_distance = 1e10;
        circ_state->maintenance_min = 40000.0;    // 40km
        circ_state->maintenance_max = 50000.0;    // 50km
        state->num_states++;
    }
    
    return 0;
}

const char* circumnavigate_formation_get_status(
    CircumnavigateFormationState *state,
    int sat_id) {
    
    static char status_buf[256];
    
    for (int i = 0; i < state->num_states; i++) {
        if (state->states[i].sat_id == sat_id) {
            const char *phase_name = "未知";
            switch (state->states[i].phase) {
                case 1: phase_name = "Lambert接近"; break;
                case 2: phase_name = "等待转移"; break;
                case 3: phase_name = "霍曼维持"; break;
                case 4: phase_name = "完成"; break;
            }
            snprintf(status_buf, sizeof(status_buf), 
                    "环视编队(%s, 距离:%.1f km)", 
                    phase_name, state->states[i].last_distance / 1000.0);
            return status_buf;
        }
    }
    
    snprintf(status_buf, sizeof(status_buf), "环视编队(未初始化)");
    return status_buf;
}