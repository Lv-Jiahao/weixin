#include "formation/formation_retreat.h"
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

static double vector_magnitude(Vector3 v) {
    return sqrt(v.x*v.x + v.y*v.y + v.z*v.z);
}

RetreatFormationState* retreat_formation_create(void) {
    RetreatFormationState *state = (RetreatFormationState*)malloc(sizeof(RetreatFormationState));
    if (!state) return NULL;
    
    state->capacity = 100;
    state->states = (RetreatState*)calloc(state->capacity, sizeof(RetreatState));
    state->num_states = 0;
    
    if (!state->states) {
        free(state);
        return NULL;
    }
    
    printf("[RETREAT] 撤退编队已创建\n");
    return state;
}

void retreat_formation_destroy(RetreatFormationState *state) {
    if (!state) return;
    if (state->states) free(state->states);
    free(state);
}

int retreat_formation_initiate_rapid(
    Satellite *red_sat,
    Satellite *blue_sat,
    OrbitalElements *orbital_elements_out,
    double *delta_v_out) {
    
    if (!red_sat || !blue_sat) return -1;
    
    printf("[RETREAT] 快速撤退: 红星%d 远离 蓝星%d\n", red_sat->id, blue_sat->id);
    
    // Hohmann转移参数
    double r_magnitude = vector_magnitude(red_sat->state.position);
    double retreat_altitude_gain = 1500000.0;  // 1500km
    
    double r_periapsis = r_magnitude;
    double r_apoapsis = r_magnitude + retreat_altitude_gain;
    
    double a_transfer = (r_periapsis + r_apoapsis) / 2.0;
    double e_transfer = (r_apoapsis - r_periapsis) / (r_apoapsis + r_periapsis);
    
    // 计算第一次脉冲
    double v_circular = sqrt(MU / r_magnitude);
    double v_transfer = sqrt(MU * (2.0 / r_magnitude - 1.0 / a_transfer));
    double delta_v1 = v_transfer - v_circular;
    
    // 计算第二次脉冲
    double v_transfer_apo = sqrt(MU * (2.0 / r_apoapsis - 1.0 / a_transfer));
    double v_circular_apo = sqrt(MU / r_apoapsis);
    double delta_v2 = v_circular_apo - v_transfer_apo;
    
    double total_delta_v = fabs(delta_v1) + fabs(delta_v2);
    
    printf("  Hohmann转移参数:\n");
    printf("    近地点: %. 1f km\n", r_periapsis / 1000.0);
    printf("    远地点: %.1f km (增益+%. 1f km)\n", 
           r_apoapsis / 1000.0, retreat_altitude_gain / 1000.0);
    printf("    ΔV1: %.4f km/s (切向加速)\n", delta_v1);
    printf("    ΔV2: %.4f km/s (远地点圆化)\n", delta_v2);
    printf("    总ΔV: %.4f km/s\n", total_delta_v);
    
    // 输出新轨道（基于当前位置但速度改变）
    memcpy(orbital_elements_out, &red_sat->orbital_elements, sizeof(OrbitalElements));
    orbital_elements_out->a = a_transfer;
    orbital_elements_out->e = e_transfer;
    
    *delta_v_out = total_delta_v;
    
    // 记录撤退状态
    // （在实际应用中应将状态添加到 RetreatFormationState）
    
    printf("[RETREAT] 快速撤退配置完成\n");
    return 0;
}

int retreat_formation_initiate_gradual(
    Satellite *red_sat,
    Satellite *blue_sat,
    OrbitalElements *orbital_elements_out,
    double *delta_v_out) {
    
    if (! red_sat || !blue_sat) return -1;
    
    printf("[RETREAT] 渐进撤退: 红星%d 缓慢远离 蓝星%d\n", red_sat->id, blue_sat->id);
    
    // 小幅度提升
    double r_magnitude = vector_magnitude(red_sat->state.position);
    double altitude_gain = 1200000.0;  // 1200km
    
    double r_periapsis = r_magnitude;
    double r_apoapsis = r_magnitude + altitude_gain;
    
    double a_transfer = (r_periapsis + r_apoapsis) / 2.0;
    double e_transfer = (r_apoapsis - r_periapsis) / (r_apoapsis + r_periapsis);
    
    double v_circular = sqrt(MU / r_magnitude);
    double v_transfer = sqrt(MU * (2.0 / r_magnitude - 1.0 / a_transfer));
    double delta_v = v_transfer - v_circular;
    
    printf("  渐进撤退参数:\n");
    printf("    高度提升: +%.1f km\n", altitude_gain / 1000.0);
    printf("    ΔV: %.4f km/s\n", delta_v);
    
    memcpy(orbital_elements_out, &red_sat->orbital_elements, sizeof(OrbitalElements));
    orbital_elements_out->a = a_transfer;
    orbital_elements_out->e = e_transfer;
    
    *delta_v_out = fabs(delta_v) * 2.0;  // 估算总ΔV
    
    printf("[RETREAT] 渐进撤退配置完成\n");
    return 0;
}

int retreat_formation_update_phase(
    RetreatFormationState *state,
    int sat_id,
    Satellite **satellite_dict,
    int num_sats) {
    
    // 查找该卫星的撤退状态
    RetreatState *retreat_state = NULL;
    for (int i = 0; i < state->num_states; i++) {
        if (state->states[i].sat_id == sat_id) {
            retreat_state = &state->states[i];
            break;
        }
    }
    
    if (!retreat_state) return -1;
    
    // 简化实现：根据相位更新
    if (retreat_state->phase == 1) {
        // Phase 1: 脉冲1完成，进入滑行
        retreat_state->phase = 2;
        printf("[RETREAT] WX%d: 第一次脉冲完成，进入滑行\n", sat_id);
    } else if (retreat_state->phase == 2) {
        // Phase 2: 滑行到远地点
        // 简化：10步后进入Phase 3
        retreat_state->phase = 3;
        printf("[RETREAT] WX%d: 到达远地点，准备第二次脉冲\n", sat_id);
    } else if (retreat_state->phase == 3) {
        // Phase 3: 施加第二次脉冲
        retreat_state->phase = 4;
        printf("[RETREAT] WX%d: 施加第二次脉冲，进入高轨维持\n", sat_id);
    }
    
    return 0;
}

const char* retreat_formation_get_status(
    RetreatFormationState *state,
    int sat_id) {
    
    static char status_buf[256];
    
    for (int i = 0; i < state->num_states; i++) {
        if (state->states[i].sat_id == sat_id) {
            const char *phase_name = "未知";
            switch (state->states[i].phase) {
                case 1: phase_name = "脉冲1-稳定"; break;
                case 2: phase_name = "滑行到远地点"; break;
                case 3: phase_name = "脉冲2-圆化"; break;
                case 4: phase_name = "高轨维持"; break;
            }
            snprintf(status_buf, sizeof(status_buf), 
                    "撤退编队(%s)", phase_name);
            return status_buf;
        }
    }
    
    snprintf(status_buf, sizeof(status_buf), "撤退编队(未开始)");
    return status_buf;
}