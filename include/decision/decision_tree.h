#ifndef DECISION_TREE_H
#define DECISION_TREE_H

#include "types.h"

/* ==================== 决策树分组结果 ==================== */

typedef struct {
    int *group_ids;              // group_ids[i] = 卫星i属于哪一组 (0 to num_groups-1)
    int num_groups;              // 总共分了几组
    int num_satellites;          // 卫星总数
    Vector3 *group_centers;      // 每组的中心位置
    int *group_sizes;            // 每组的卫星数量
} GroupResult;

/* ==================== 决策树接口函数 ==================== */

/**
 * 基于卫星位置进行K-means聚类分组
 * @param satellites 卫星数组
 * @param num_satellites 卫星数量
 * @param target_num_groups 目标分组数
 * @return 分组结果指针，NULL表示失败
 */
GroupResult* decision_tree_group_satellites(
    Satellite **satellites,
    int num_satellites,
    int target_num_groups
);

/**
 * 根据分组和卫星功能类型选择合适的编队
 * @param satellites 卫星数组
 * @param num_satellites 卫星总数
 * @param groups 分组结果
 * @param group_id 要选择编队的组ID
 * @return 编队类型 (0=AROUND, 1=INSPECT, 2=CIRCUMNAVIGATE, 3=RETREAT)
 */
uint8_t decision_tree_select_formation(
    Satellite **satellites,
    int num_satellites,
    const GroupResult *groups,
    int group_id
);

/**
 * 自动确定最优分组数（使用肘部法则）
 * @param satellites 卫星数组
 * @param num_satellites 卫星数量
 * @param max_groups 最大分组数
 * @return 最优分组数
 */
int decision_tree_auto_determine_groups(
    Satellite **satellites,
    int num_satellites,
    int max_groups
);

/**
 * 释放分组结果内存
 * @param result 要释放的分组结果
 */
void decision_tree_free_result(GroupResult *result);

/**
 * 打印分组信息（用于调试）
 * @param result 分组结果
 */
void decision_tree_print_groups(const GroupResult *result);

#endif /* DECISION_TREE_H */