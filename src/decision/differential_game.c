#include "decision/differential_game.h"
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdio.h>

#define MU 3.986004418e5  // 地球重力参数 km³/s²

/**
 * 计算两个卫星之间的距离
 */
static double calculate_distance(Vector3 pos1, Vector3 pos2) {
    double dx = pos1.x - pos2.x;
    double dy = pos1. y - pos2.y;
    double dz = pos1. z - pos2.z;
    return sqrt(dx*dx + dy*dy + dz*dz);
}

/**
 * 计算向量的模长
 */
static double vector_magnitude(Vector3 v) {
    return sqrt(v.x*v.x + v.y*v.y + v.z*v.z);
}

/**
 * - 贪心最大权匹配
 */
static void hungarian_assignment_greedy(
    const double *payoff_matrix,
    int num_red,
    int num_blue,
    int *assignments) {
    
    // 初始化：所有红星无分配（-1）
    memset(assignments, -1, sizeof(int) * num_red);
    
    int *used_blue = (int*)calloc(num_blue, sizeof(int));
    if (!used_blue) return;
    
    // 按收益从高到低贪心分配
    // 最多分配 min(num_red, num_blue) 对
    int num_pairs = (num_red < num_blue) ? num_red : num_blue;
    
    for (int pair = 0; pair < num_pairs; pair++) {
        double max_payoff = -1e100;
        int best_red = -1;
        int best_blue = -1;
        
        // 找到最大未分配收益
        for (int red = 0; red < num_red; red++) {
            if (assignments[red] != -1) continue;  // 已分配
            
            for (int blue = 0; blue < num_blue; blue++) {
                if (used_blue[blue]) continue;  // 已被占用
                
                double payoff = payoff_matrix[red * num_blue + blue];
                if (payoff > max_payoff) {
                    max_payoff = payoff;
                    best_red = red;
                    best_blue = blue;
                }
            }
        }
        
        // 分配最大收益的红蓝星对
        if (best_red >= 0 && best_blue >= 0) {
            assignments[best_red] = best_blue;
            used_blue[best_blue] = 1;
        } else {
            break;  // 无更多有效分配
        }
    }
    
    free(used_blue);
}

/* ==================== 公开接口实现 ==================== */

double differential_game_calculate_threat(
    Satellite *red_sat,
    Satellite *blue_sat) {
    
    if (!red_sat || !blue_sat) return 0;
    
    // 距离因素 (km)
    double distance = calculate_distance(
        red_sat->state.position,
        blue_sat->state.position
    );
    double distance_factor = 100.0 / (1.0 + distance / 10000.0);  // 距离越近，威胁越大
    
    // 燃料因素
    double fuel_factor = (red_sat->fuel / 1000.0) * 20.0;  // 燃料越多，威胁越大
    
    // 相对速度因素
    double vel_x = red_sat->state.velocity. x - blue_sat->state.velocity. x;
    double vel_y = red_sat->state.velocity.y - blue_sat->state.velocity.y;
    double vel_z = red_sat->state.velocity.z - blue_sat->state.velocity.z;
    double rel_vel = sqrt(vel_x*vel_x + vel_y*vel_y + vel_z*vel_z);
    double velocity_factor = rel_vel * 5.0;  // 相对速度越大，威胁越大
    
    // 功能因素
    double function_factor = 0;
    if (red_sat->function_type == 0) function_factor = 30.0;  // 攻击威胁大
    else if (red_sat->function_type == 1) function_factor = 15.0;  // 侦察威胁中
    else function_factor = 10.0;  // 防御威胁小
    
    double threat_level = distance_factor + fuel_factor + velocity_factor + function_factor;
    return (threat_level < 100.0) ? threat_level : 100.0;  // 限制在0-100
}

double differential_game_calculate_payoff(
    Satellite *red_sat,
    Satellite *blue_sat,
    int strategy) {
    
    if (!red_sat || !blue_sat) return 0;
    
    double distance = calculate_distance(
        red_sat->state.position,
        blue_sat->state.position
    );
    
    double threat = differential_game_calculate_threat(red_sat, blue_sat);
    
    // 收益计算 (基于策略和距离)
    double payoff = 0;
    
    switch (strategy) {
        case 0:  // STRATEGY_ATTACK
            // 攻击策略：距离越近、威胁越大，收益越高
            payoff = 50.0 - (distance / 100000.0) + (threat / 2.0);
            break;
            
        case 1:  // STRATEGY_RECON
            // 侦察策略：中距离最优
            payoff = 30.0 - fabs(distance - 50000.0) / 10000.0 + (threat / 3.0);
            break;
            
        case 2:  // STRATEGY_DEFENSE
            // 防御策略：远距离最优
            payoff = 20.0 + (distance / 50000.0);
            break;
            
        default:
            payoff = 0;
    }
    
    // 燃料充足度修正
    double fuel_ratio = red_sat->fuel / 1000.0;
    if (fuel_ratio < 0.3) payoff *= 0.5;  // 燃料少则收益减半
    
    return payoff;
}

GameResult* differential_game_assign_strategies(
    Satellite **red_satellites,
    int num_red,
    Satellite **blue_satellites,
    int num_blue,
    const char *strategy_type) {
    
    if (!red_satellites || ! blue_satellites || num_red <= 0 || num_blue <= 0) {
        fprintf(stderr, "[错误] 微分博弈: 输入参数无效\n");
        return NULL;
    }
    
    printf("[微分博弈] 开始策略分配: %d红vs%d蓝, 策略=%s\n",
           num_red, num_blue, strategy_type ?  strategy_type : "未指定");
    
    // 分配结果结构
    GameResult *result = (GameResult*)malloc(sizeof(GameResult));
    if (!result) {
        fprintf(stderr, "[错误] 内存分配失败\n");
        return NULL;
    }
    
    result->num_red_satellites = num_red;
    result->num_blue_satellites = num_blue;
    result->strategy_assignments = (int*)malloc(sizeof(int) * num_red);
    result->target_assignments = (int*)malloc(sizeof(int) * num_red);
    result->payoff_matrix = (double*)malloc(sizeof(double) * num_red * num_blue);
    
    if (!result->strategy_assignments || !result->target_assignments || !result->payoff_matrix) {
        fprintf(stderr, "[错误] 内存分配失败\n");
        free(result->strategy_assignments);
        free(result->target_assignments);
        free(result->payoff_matrix);
        free(result);
        return NULL;
    }
    
    // ===== Step 1: 确定每个红星的策略 =====
    int default_strategy = 0;  // STRATEGY_ATTACK
    if (strategy_type) {
        if (strcmp(strategy_type, "ZC") == 0) {
            default_strategy = 1;  // STRATEGY_RECON
        } else if (strcmp(strategy_type, "FY") == 0) {
            default_strategy = 2;  // STRATEGY_DEFENSE
        }
    }
    
    // 根据卫星功能类型分配策略
    for (int i = 0; i < num_red; i++) {
        Satellite *sat = red_satellites[i];
        
        // 优先使用卫星本身的功能类型
        if (sat->function_type == 0) {
            result->strategy_assignments[i] = 0;  // 攻击卫星 -> 攻击策略
        } else if (sat->function_type == 1) {
            result->strategy_assignments[i] = 1;  // 侦察卫星 -> 侦察策略
        } else {
            result->strategy_assignments[i] = 2;  // 防御卫星 -> 防御策略
        }
    }
    
    // ===== Step 2: 计算收益矩阵 =====
    printf("[微分博弈] 计算收益矩阵...\n");
    
    for (int r = 0; r < num_red; r++) {
        for (int b = 0; b < num_blue; b++) {
            double payoff = differential_game_calculate_payoff(
                red_satellites[r],
                blue_satellites[b],
                result->strategy_assignments[r]
            );
            result->payoff_matrix[r * num_blue + b] = payoff;
        }
    }
    
    // ===== Step 3: 最优分配 =====
    printf("[微分博弈] 执行最优分配...\n");
    hungarian_assignment_greedy(
        result->payoff_matrix,
        num_red,
        num_blue,
        result->target_assignments
    );
    
    // ===== 打印分配结果 =====
    printf("[微分博弈] 分配完成:\n");
    for (int r = 0; r < num_red; r++) {
        int target_id = result->target_assignments[r];
        if (target_id >= 0 && target_id < num_blue) {
            double payoff = result->payoff_matrix[r * num_blue + target_id];
            const char *strat_name = 
                result->strategy_assignments[r] == 0 ? "攻击" :
                result->strategy_assignments[r] == 1 ? "侦察" : "防御";
            
            printf("  红星%d -> 蓝星%d [%s] 收益=%.2f\n",
                   red_satellites[r]->id,
                   blue_satellites[target_id]->id,
                   strat_name,
                   payoff);
        } else {
            printf("  红星%d -> 无目标\n", red_satellites[r]->id);
        }
    }
    
    return result;
}

void differential_game_hungarian_assignment(
    const double *payoff_matrix,
    int num_red,
    int num_blue,
    int *assignments) {
    
    hungarian_assignment_greedy(payoff_matrix, num_red, num_blue, assignments);
}

void differential_game_free_result(GameResult *result) {
    if (! result) return;
    
    if (result->strategy_assignments) free(result->strategy_assignments);
    if (result->target_assignments) free(result->target_assignments);
    if (result->payoff_matrix) free(result->payoff_matrix);
    
    free(result);
}

void differential_game_print_result(const GameResult *result) {
    if (!result) {
        fprintf(stderr, "[错误] 博弈结果为空\n");
        return;
    }
    
    printf("\n=== 微分博弈结果 ===\n");
    printf("红方卫星数: %d\n", result->num_red_satellites);
    printf("蓝方卫星数: %d\n", result->num_blue_satellites);
    
    printf("\n收益矩阵 (红x蓝):\n");
    for (int r = 0; r < result->num_red_satellites; r++) {
        printf("Red %d: ", r);
        for (int b = 0; b < result->num_blue_satellites; b++) {
            printf("%.2f ", result->payoff_matrix[r * result->num_blue_satellites + b]);
        }
        printf("\n");
    }
    
    printf("\n最优分配:\n");
    for (int r = 0; r < result->num_red_satellites; r++) {
        if (result->target_assignments[r] >= 0) {
            printf("Red %d -> Blue %d\n", r, result->target_assignments[r]);
        }
    }
}