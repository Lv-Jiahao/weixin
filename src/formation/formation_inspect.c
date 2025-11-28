#include "formation/formation_inspect.h"
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdio.h>

#define MU 3.986004418e5
#define EARTH_RADIUS 6371000.0
#define PI 3.14159265359

/**
 * 计算两点间距离
 */
static double point_distance(Vector3 p1, Vector3 p2) {
    double dx = p1.x - p2. x;
    double dy = p1.y - p2.y;
    double dz = p1.z - p2.z;
    return sqrt(dx*dx + dy*dy + dz*dz);
}

/**
 * 计算Hohmann转移速度增量
 */
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

/* ==================== 公开接口实现 ==================== */

InspectFormationState* inspect_formation_create(void) {
    InspectFormationState *state = (InspectFormationState*)malloc(sizeof(InspectFormationState));
    if (!state) return NULL;
    
    state->capacity = 100;
    state->states = (InspectionState*)calloc(state->capacity, sizeof(InspectionState));
    state->num_states = 0;
    
    if (!state->states) {
        free(state);
        return NULL;
    }
    
    printf("[INSPECT] 巡视编队已创建\n");
    return state;
}

void inspect_formation_destroy(InspectFormationState *state) {
    if (!state) return;
    if (state->states) free(state->states);
    free(state);
}

int inspect_formation_multi_inspect_single(
    Satellite **inspectors,
    int num_inspectors,
    Satellite *target,
    OrbitalElements *orbital_elements_out,
    double *delta_v_out) {
    
    if (!inspectors || num_inspectors <= 0 || !target) return -1;
    
    printf("[INSPECT] 多对一巡视: %d个红星 → 蓝星%d\n", num_inspectors, target->id);
    
    double target_a = target->orbital_elements.a;
    double ellipse_altitude = 30000.0;  // ±30km
    double ellipse_e = ellipse_altitude / target_a;  // 偏心率
    double ellipse_a = target_a;  // 半长轴相同
    
    printf("  统一椭圆轨道: a=%.1f km, e=%.6f\n", ellipse_a, ellipse_e);
    
    // 为每个巡视卫星分配轨道参数
    for (int i = 0; i < num_inspectors; i++) {
        Satellite *inspector = inspectors[i];
        
        // 所有巡视卫星进入相同的椭圆轨道
        orbital_elements_out[i] = target->orbital_elements;
        orbital_elements_out[i].a = ellipse_a;
        orbital_elements_out[i]. e = ellipse_e;
        orbital_elements_out[i].m0 = target->orbital_elements.m0 + (i * 0.0);  // 相位对齐
        
        // 计算速度增量
        delta_v_out[i] = hohmann_delta_v(inspector->orbital_elements.a, ellipse_a);
        
        printf("  WX%d: 轨道变更 a=%.1f→%.1f km, e=%.1f→%.6f, ΔV=%.4f km/s\n",
               inspector->id,
               inspector->orbital_elements. a, ellipse_a,
               inspector->orbital_elements.e, ellipse_e,
               delta_v_out[i]);
    }
    
    printf("[INSPECT] 巡视编队配置完成\n");
    return 0;
}

int inspect_formation_update_phase(
    InspectFormationState *state,
    int inspector_id,
    Satellite **satellite_dict,
    int num_sats) {
    
    // 简化实现
    return 0;
}

void inspect_formation_update_circle_count(
    InspectFormationState *state,
    int inspector_id,
    Satellite *inspector_sat,
    Satellite *target_sat) {
    
    if (!state || !inspector_sat || !target_sat) return;
    
    // 查找或创建该巡视的状态
    InspectionState *insp_state = NULL;
    for (int i = 0; i < state->num_states; i++) {
        if (state->states[i].inspector_id == inspector_id) {
            insp_state = &state->states[i];
            break;
        }
    }
    
    if (!insp_state && state->num_states < state->capacity) {
        insp_state = &state->states[state->num_states];
        insp_state->inspector_id = inspector_id;
        insp_state->circle_count = 0;
        insp_state->circle_progress = 0.0;
        insp_state->inspection_started = 0;
        state->num_states++;
    }
    
    if (! insp_state) return;
    
    // 计算距离
    double distance = point_distance(inspector_sat->state.position, target_sat->state.position);
    
    // 更新最小距离
    if (distance < insp_state->min_distance_reached) {
        insp_state->min_distance_reached = distance;
    }
    
    // 检查是否开始巡视
    if (! insp_state->inspection_started && distance < 2000000.0) {
        insp_state->inspection_started = 1;
        printf("[INSPECT] WX%d 开始圈数统计, 距离=%.1f km\n", 
               inspector_id, distance / 1000.0);
    }
}

const char* inspect_formation_get_status(
    InspectFormationState *state,
    int sat_id) {
    
    static char status_buf[256];
    
    if (!state) {
        snprintf(status_buf, sizeof(status_buf), "巡视编队(未初始化)");
        return status_buf;
    }
    
    for (int i = 0; i < state->num_states; i++) {
        if (state->states[i].inspector_id == sat_id) {
            snprintf(status_buf, sizeof(status_buf), 
                    "巡视编队(圈:%u, 进度:%.0f%%)",
                    state->states[i].circle_count,
                    state->states[i].circle_progress * 100.0);
            return status_buf;
        }
    }
    
    snprintf(status_buf, sizeof(status_buf), "巡视编队中");
    return status_buf;
}