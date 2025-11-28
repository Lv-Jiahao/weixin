#ifndef DIFFERENTIAL_GAME_H
#define DIFFERENTIAL_GAME_H

#include "types.h"

/* ==================== 博弈结果结构 ==================== */

typedef struct {
    int *strategy_assignments;      // strategy_assignments[i] = 第i个红星的策略
    int *target_assignments;        // target_assignments[i] = 第i个红星的目标蓝星索引
    int num_red_satellites;
    int num_blue_satellites;
    double *payoff_matrix;          // 博弈收益矩阵 [红x蓝]
} GameResult;

/* ==================== 微分博弈接口函数 ==================== */

/**
 * 基于微分博弈理论分配红蓝卫星之间的策略和目标
 * @param red_satellites 红方卫星数组
 * @param num_red 红方卫星数量
 * @param blue_satellites 蓝方卫星数组
 * @param num_blue 蓝方卫星数量
 * @param strategy_type 策略类型 ("GJ"=攻击, "ZC"=侦察, "FY"=防御)
 * @return 博弈分配结果指针，NULL表示失败
 */
GameResult* differential_game_assign_strategies(
    Satellite **red_satellites,
    int num_red,
    Satellite **blue_satellites,
    int num_blue,
    const char *strategy_type
);

/**
 * 计算两颗卫星之间的威胁等级（基于距离、速度、燃料）
 * @param red_sat 红方卫星
 * @param blue_sat 蓝方卫星
 * @return 威胁等级 (0-100)
 */
double differential_game_calculate_threat(
    Satellite *red_sat,
    Satellite *blue_sat
);

/**
 * 基于策略类型计算单次博弈的收益
 * @param red_sat 红方卫星
 * @param blue_sat 蓝方卫星
 * @param strategy 红方采用的策略 (0=攻击, 1=侦察, 2=防御)
 * @return 收益值（负值表示损失）
 */
double differential_game_calculate_payoff(
    Satellite *red_sat,
    Satellite *blue_sat,
    int strategy
);

/**
 * 采用贪心最大权匹配算法进行最优分配
 * @param payoff_matrix 收益矩阵 [红 x 蓝]
 * @param num_red 红方数量
 * @param num_blue 蓝方数量
 * @param assignments 分配结果（大小为num_red，值为-1表示无分配）
 */
void differential_game_hungarian_assignment(
    const double *payoff_matrix,
    int num_red,
    int num_blue,
    int *assignments
);

/**
 * 释放博弈结果内存
 * @param result 要释放的结果
 */
void differential_game_free_result(GameResult *result);

/**
 * 打印博弈结果（用于调试）
 * @param result 博弈结果
 */
void differential_game_print_result(const GameResult *result);

#endif /* DIFFERENTIAL_GAME_H */