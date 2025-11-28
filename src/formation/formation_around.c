#include "formation/formation_around.h"
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdio.h>

#define MU 3.986004418e5  // 地球重力参数
#define EARTH_RADIUS 6371000. 0

/**
 * 计算两个向量之间的欧氏距离
 */
static double vector_distance(Vector3 v1, Vector3 v2) {
    double dx = v1.x - v2.x;
    double dy = v1.y - v2. y;
    double dz = v1.z - v2. z;
    return sqrt(dx*dx + dy*dy + dz*dz);
}

/**
 * 向量规范化
 */
static Vector3 vector_normalize(Vector3 v) {
    double mag = sqrt(v.x*v.x + v.y*v.y + v.z*v.z);
    if (mag < 1e-10) return v;
    return (Vector3){v.x/mag, v. y/mag, v.z/mag};
}

/**
 * 向量点积
 */
static double vector_dot(Vector3 a, Vector3 b) {
    return a.x*b. x + a.y*b. y + a.z*b. z;
}

/**
 * 计算Hohmann转移的速度增量
 */
static double calculate_hohmann_delta_v(double a1, double a2) {
    if (fabs(a1 - a2) < 1.0) return 0.001;
    
    double v1 = sqrt(MU / a1);  // 初始轨道速度
    double transfer_a = (a1 + a2) / 2.0;
    double v_transfer_at_a1 = sqrt(MU * (2.0 / a1 - 1.0 / transfer_a));
    double delta_v1 = fabs(v_transfer_at_a1 - v1);
    
    double v2 = sqrt(MU / a2);
    double v_transfer_at_a2 = sqrt(MU * (2.0 / a2 - 1.0 / transfer_a));
    double delta_v2 = fabs(v_transfer_at_a2 - v2);
    
    return delta_v1 + delta_v2;
}

/**
 * 计算球形位置（简化版，使用立方体顶点分布）
 */
static void calculate_sphere_positions(int num_sats, Vector3 *positions) {
    int cube_positions[8][3] = {
        {-1, -1, -1}, {-1, -1, 1},  {-1, 1, -1}, {-1, 1, 1},
        { 1, -1, -1}, { 1, -1, 1},  { 1, 1, -1}, { 1, 1, 1}
    };
    
    double radius = 100000.0;  // 100,000 km
    
    for (int i = 0; i < num_sats && i < 8; i++) {
        positions[i]. x = cube_positions[i][0] * radius;
        positions[i]. y = cube_positions[i][1] * radius;
        positions[i].z = cube_positions[i][2] * radius;
    }
}

/* ==================== 公开接口实现 ==================== */

AroundFormationState* around_formation_create(void) {
    AroundFormationState *state = (AroundFormationState*)malloc(sizeof(AroundFormationState));
    if (!state) return NULL;
    
    memset(state, 0, sizeof(AroundFormationState));
    printf("[AROUND] 球形围观编队已创建\n");
    
    return state;
}

void around_formation_destroy(AroundFormationState *state) {
    if (state) free(state);
}

double around_formation_single_vs_single(
    Satellite *chaser,
    Satellite *target,
    OrbitalElements *orbital_elements_out) {
    
    if (!chaser || !target || !orbital_elements_out) return 0;
    
    printf("[AROUND] 单对一编队: 红星%d → 蓝星%d\n", chaser->id, target->id);
    
    // 获取当前位置
    if (!chaser->state. position. x) {
        printf("  警告: 卫星位置未初始化\n");
    }
    
    // 计算距离
    double distance = vector_distance(chaser->state.position, target->state.position);
    printf("  当前距离: %.1f km\n", distance / 1000.0);
    
    // 计算Hohmann转移参数
    double target_a = target->orbital_elements.a;
    double chaser_a = chaser->orbital_elements.a;
    double delta_v = calculate_hohmann_delta_v(chaser_a, target_a);
    
    // 输出目标轨道参数
    memcpy(orbital_elements_out, &target->orbital_elements, sizeof(OrbitalElements));
    orbital_elements_out->e = 0.001;  // 圆轨道
    
    printf("  目标轨道: a=%.1f km, 速度增量=%.4f km/s\n", target_a, delta_v);
    
    return delta_v;
}

int around_formation_multi_vs_single_sphere(
    Satellite **chasers,
    int num_chasers,
    Satellite *target,
    OrbitalElements *orbital_elements_out,
    double *delta_v_out) {
    
    if (!chasers || num_chasers <= 0 || !target) return -1;
    
    printf("[AROUND] 多对一球形编队: %d个红星 → 蓝星%d\n", num_chasers, target->id);
    
    // 计算球形位置
    Vector3 sphere_positions[100];
    calculate_sphere_positions(num_chasers, sphere_positions);
    
    // 为每个追踪卫星分配位置和计算轨道参数
    for (int i = 0; i < num_chasers; i++) {
        Satellite *chaser = chasers[i];
        
        // 计算目标位置（球体中心 + 偏移）
        double target_a = target->orbital_elements.a;
        
        // 高度偏移：±200km
        double delta_a = (i % 2 == 0) ?  200000.0 : -200000.0;
        double orbit_a = target_a + delta_a / 1000.0;
        
        // 输出轨道参数
        orbital_elements_out[i] = target->orbital_elements;
        orbital_elements_out[i].a = orbit_a;
        orbital_elements_out[i].e = 0.001;  // 圆轨道
        
        // 计算速度增量
        delta_v_out[i] = calculate_hohmann_delta_v(chaser->orbital_elements.a, orbit_a);
        
        printf("  WX%d: a=%.1f km → %.1f km, ΔV=%.4f km/s\n",
               chaser->id, chaser->orbital_elements.a, orbit_a, delta_v_out[i]);
    }
    
    printf("[AROUND] 球形编队配置完成\n");
    return 0;
}

OrbitalElements* around_formation_update_phase(
    AroundFormationState *state,
    int chaser_id,
    Satellite **satellite_dict,
    int num_sats) {
    
    // 简化实现：返回NULL表示维持当前轨道
    return NULL;
}

const char* around_formation_get_status(
    AroundFormationState *state,
    int sat_id) {
    
    static char status_buf[256];
    snprintf(status_buf, sizeof(status_buf), "球形编队中");
    return status_buf;
}