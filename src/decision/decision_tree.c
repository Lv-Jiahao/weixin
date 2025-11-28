#include "decision/decision_tree.h"
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdio.h>

#define MAX_ITERATIONS 100
#define CONVERGENCE_THRESHOLD 1e-4

/**
 * 计算两个卫星之间的欧氏距离
 */
static double calculate_distance(Vector3 pos1, Vector3 pos2) {
    double dx = pos1.x - pos2.x;
    double dy = pos1. y - pos2.y;
    double dz = pos1.z - pos2.z;
    return sqrt(dx*dx + dy*dy + dz*dz);
}

/**
 * 向量相加
 */
static Vector3 vector_add(Vector3 a, Vector3 b) {
    return (Vector3){a.x + b.x, a.y + b.y, a.z + b.z};
}

/**
 * 向量数乘
 */
static Vector3 vector_scale(Vector3 v, double scale) {
    return (Vector3){v.x * scale, v.y * scale, v.z * scale};
}

/**
 * K-means聚类的核心算法
 */
static void kmeans_clustering(
    Satellite **satellites,
    int num_satellites,
    int num_groups,
    int *assignments,
    Vector3 *centers) {
    
    // 初始化聚类中心（选择前num_groups个卫星作为初始中心）
    for (int i = 0; i < num_groups; i++) {
        centers[i] = satellites[i % num_satellites]->state.position;
    }
    
    // 迭代聚类
    for (int iter = 0; iter < MAX_ITERATIONS; iter++) {
        // ===== Step 1: 分配卫星到最近的聚类中心 =====
        for (int i = 0; i < num_satellites; i++) {
            double min_distance = 1e100;
            int closest_center = 0;
            
            for (int k = 0; k < num_groups; k++) {
                double dist = calculate_distance(
                    satellites[i]->state.position,
                    centers[k]
                );
                
                if (dist < min_distance) {
                    min_distance = dist;
                    closest_center = k;
                }
            }
            
            assignments[i] = closest_center;
        }
        
        // ===== Step 2: 更新聚类中心 =====
        Vector3 old_centers[num_groups];
        memcpy(old_centers, centers, sizeof(Vector3) * num_groups);
        
        for (int k = 0; k < num_groups; k++) {
            Vector3 sum = {0, 0, 0};
            int count = 0;
            
            for (int i = 0; i < num_satellites; i++) {
                if (assignments[i] == k) {
                    sum = vector_add(sum, satellites[i]->state. position);
                    count++;
                }
            }
            
            if (count > 0) {
                centers[k] = vector_scale(sum, 1.0 / count);
            }
        }
        
        // ===== Step 3: 检查收敛 =====
        int converged = 1;
        for (int k = 0; k < num_groups; k++) {
            double center_shift = calculate_distance(centers[k], old_centers[k]);
            if (center_shift > CONVERGENCE_THRESHOLD) {
                converged = 0;
                break;
            }
        }
        
        if (converged) {
            printf("[决策树] K-means在第%d次迭代时收敛\n", iter);
            break;
        }
    }
}

/* ==================== 公开接口实现 ==================== */

GroupResult* decision_tree_group_satellites(
    Satellite **satellites,
    int num_satellites,
    int target_num_groups) {
    
    if (! satellites || num_satellites <= 0 || target_num_groups <= 0) {
        fprintf(stderr, "[错误] 决策树: 输入参数无效\n");
        return NULL;
    }
    
    // 限制分组数
    if (target_num_groups > num_satellites) {
        target_num_groups = num_satellites;
    }
    
    printf("[决策树] 开始分组: %d颗卫星分%d组\n", num_satellites, target_num_groups);
    
    // 分配结果结构
    GroupResult *result = (GroupResult*)malloc(sizeof(GroupResult));
    if (!result) {
        fprintf(stderr, "[错误] 内存分配失败\n");
        return NULL;
    }
    
    result->num_satellites = num_satellites;
    result->num_groups = target_num_groups;
    result->group_ids = (int*)malloc(sizeof(int) * num_satellites);
    result->group_centers = (Vector3*)malloc(sizeof(Vector3) * target_num_groups);
    result->group_sizes = (int*)malloc(sizeof(int) * target_num_groups);
    
    if (!result->group_ids || !result->group_centers || !result->group_sizes) {
        fprintf(stderr, "[错误] 内存分配失败\n");
        free(result->group_ids);
        free(result->group_centers);
        free(result->group_sizes);
        free(result);
        return NULL;
    }
    
    // 执行K-means聚类
    kmeans_clustering(
        satellites,
        num_satellites,
        target_num_groups,
        result->group_ids,
        result->group_centers
    );
    
    // 计算每组的卫星数量
    memset(result->group_sizes, 0, sizeof(int) * target_num_groups);
    for (int i = 0; i < num_satellites; i++) {
        result->group_sizes[result->group_ids[i]]++;
    }
    
    // 打印分组统计
    printf("[决策树] 分组完成:\n");
    for (int k = 0; k < target_num_groups; k++) {
        printf("  第%d组: %d颗卫星, 中心=(%.1f, %.1f, %.1f)km\n",
               k, result->group_sizes[k],
               result->group_centers[k].x / 1000.0,
               result->group_centers[k]. y / 1000.0,
               result->group_centers[k].z / 1000.0);
    }
    
    return result;
}

uint8_t decision_tree_select_formation(
    Satellite **satellites,
    int num_satellites,
    const GroupResult *groups,
    int group_id) {
    
    if (! satellites || ! groups || group_id < 0 || group_id >= groups->num_groups) {
        fprintf(stderr, "[错误] 决策树: 输入参数无效\n");
        return 0;  // 默认返回球形编队
    }
    
    // 统计该组中各类型卫星的数量
    int attack_count = 0;
    int recon_count = 0;
    int defense_count = 0;
    int total_fuel = 0;
    
    for (int i = 0; i < num_satellites; i++) {
        if (groups->group_ids[i] == group_id) {
            Satellite *sat = satellites[i];
            
            switch (sat->function_type) {
                case 0:  // 攻击
                    attack_count++;
                    break;
                case 1:  // 侦察
                    recon_count++;
                    break;
                case 2:  // 防御
                    defense_count++;
                    break;
            }
            total_fuel += (int)sat->fuel;
        }
    }
    
    printf("[决策树] 第%d组编队选择: 攻击%d 侦察%d 防御%d\n",
           group_id, attack_count, recon_count, defense_count);
    
    // 编队选择规则
    if (attack_count >= 2) {
        printf("  → 选择: 球形围观(AROUND)\n");
        return 0;  // AROUND
    } else if (recon_count >= 1) {
        printf("  → 选择: 巡视编队(INSPECT)\n");
        return 1;  // INSPECT
    } else if (defense_count >= 1 && attack_count >= 1) {
        printf("  → 选择: 环视编队(CIRCUMNAVIGATE)\n");
        return 2;  // CIRCUMNAVIGATE
    } else if (total_fuel < 50000) {
        printf("  → 选择: 撤退编队(RETREAT) - 燃料不足\n");
        return 3;  // RETREAT
    } else {
        printf("  → 选择: 球形围观(AROUND) - 默认\n");
        return 0;  // AROUND (默认)
    }
}

int decision_tree_auto_determine_groups(
    Satellite **satellites,
    int num_satellites,
    int max_groups) {
    
    if (!satellites || num_satellites <= 0 || max_groups <= 0) {
        return 1;  // 默认返回1组
    }
    
    if (num_satellites <= 2) {
        return num_satellites;
    }
    
    printf("[决策树] 使用肘部法则自动确定分组数.. .\n");
    
    // 简化的肘部法则
    double *wcss_values = (double*)malloc(sizeof(double) * max_groups);
    if (!wcss_values) return 1;
    
    for (int k = 1; k <= max_groups; k++) {
        int *assignments = (int*)malloc(sizeof(int) * num_satellites);
        Vector3 *centers = (Vector3*)malloc(sizeof(Vector3) * k);
        
        if (!assignments || !centers) {
            free(assignments);
            free(centers);
            free(wcss_values);
            return 1;
        }
        
        // 执行K-means
        kmeans_clustering(satellites, num_satellites, k, assignments, centers);
        
        // 计算WCSS (Within-Cluster Sum of Squares)
        double wcss = 0;
        for (int i = 0; i < num_satellites; i++) {
            int cluster = assignments[i];
            double dist = calculate_distance(
                satellites[i]->state.position,
                centers[cluster]
            );
            wcss += dist * dist;
        }
        wcss_values[k-1] = wcss;
        
        printf("  K=%d: WCSS=%. 2f\n", k, wcss);
        
        free(assignments);
        free(centers);
    }
    
    // 查找肘点
    int optimal_k = 1;
    double max_decrease_rate = 0;
    
    for (int k = 2; k < max_groups; k++) {
        double decrease_rate = (wcss_values[k-2] - wcss_values[k-1]) / wcss_values[k-2];
        if (decrease_rate < max_decrease_rate * 0.5 && optimal_k > 1) {
            break;
        }
        max_decrease_rate = decrease_rate;
        optimal_k = k;
    }
    
    free(wcss_values);
    
    printf("[决策树] 自动分组数: %d\n", optimal_k);
    return optimal_k;
}

void decision_tree_free_result(GroupResult *result) {
    if (! result) return;
    
    if (result->group_ids) free(result->group_ids);
    if (result->group_centers) free(result->group_centers);
    if (result->group_sizes) free(result->group_sizes);
    
    free(result);
}

void decision_tree_print_groups(const GroupResult *result) {
    if (!result) {
        fprintf(stderr, "[错误] 分组结果为空\n");
        return;
    }
    
    printf("\n=== 分组统计 ===\n");
    printf("总卫星数: %d\n", result->num_satellites);
    printf("分组数: %d\n", result->num_groups);
    
    for (int k = 0; k < result->num_groups; k++) {
        printf("\n第%d组:\n", k);
        printf("  卫星数量: %d\n", result->group_sizes[k]);
        printf("  群心位置: (%.1f, %. 1f, %.1f) km\n",
               result->group_centers[k].x / 1000.0,
               result->group_centers[k]. y / 1000.0,
               result->group_centers[k].z / 1000.0);
    }
}