#ifndef DECISION_TREE_H
#define DECISION_TREE_H

#include <types.h>
#include <satellite.h>

/* ==================== K-Means聚类 ==================== */

/* K-Means聚类结果 */
typedef struct {
    int *assignments;          // 每颗卫星的分组ID
    Vector3 *centers;          // 分组中心
    int num_groups;            // 分组数
    int num_satellites;        // 卫星数
    double *inertia_values;    // 各K值的惯性
} KMeansResult;

/* 创建K-Means结果结构 */
KMeansResult* kmeans_result_create(int num_satellites, int num_groups);

/* 销毁K-Means结果结构 */
void kmeans_result_destroy(KMeansResult *result);

/* ==================== 聚类算法 ==================== */

/* 执行K-Means聚类 */
int kmeans_cluster(
    Satellite **satellites,
    int num_satellites,
    int num_groups,
    KMeansResult *result,
    int max_iterations
);

/* 组卫星进行分组 */
int decision_tree_group_satellites(
    Satellite **satellites,
    int num_satellites,
    int num_groups,
    KMeansResult *result
);

/* 获取每个分组中的卫星ID */
int* decision_tree_get_group_members(
    KMeansResult *result,
    int group_id,
    int *member_count
);

/* ==================== 自动分组 ==================== */

/* 自动确定最优分组数 */
int decision_tree_auto_determine_groups(
    Satellite **satellites,
    int num_satellites,
    int *num_groups
);

/* 计算肘点（使用二阶导数） */
int decision_tree_find_elbow_point(
    double *inertia_values,
    int num_values
);

/* Elbow方法 */
int decision_tree_elbow_method(
    Satellite **satellites,
    int num_satellites,
    int max_k,
    int *optimal_k
);

/* ==================== 球形编队分配 ==================== */

/* 球形编队轨道配置 */
typedef struct {
    double a;              // 轨道半长轴
    double e;              // 偏心率
    double i;              // 倾角
    double omega_big;      // 升交点赤经
    double omega_small;    // 近地点幅角
    double phase;          // 轨道相位 (度)
} OrbitConfig;

/* 为球形编队分配位置 */
int decision_tree_assign_sphere_positions(
    Satellite **satellites,
    int num_satellites,
    Vector3 target_position,
    OrbitConfig *orbit_configs,
    int num_configs,
    int *position_assignment  // 输出：WXID -> 配置索引
);

/* 按燃料分配位置（优先燃料充足的） */
int decision_tree_assign_by_fuel(
    Satellite **satellites,
    int num_satellites,
    int num_positions,
    int *assignment  // 输出：WXID -> 位置索引
);

/* ==================== 任务规划 ==================== */

/* 任务分配结果 */
typedef struct {
    int *assignments;      // 卫星ID -> 任务ID
    int *task_counts;      // 每个任务分配的卫星数
    int num_tasks;         // 任务数
} TaskAssignment;

/* 创建任务分配结构 */
TaskAssignment* task_assignment_create(int num_satellites, int num_tasks);

/* 销毁任务分配结构 */
void task_assignment_destroy(TaskAssignment *assignment);

/* 分配编队任务 */
int decision_tree_assign_formation_tasks(
    Satellite **satellites,
    int num_satellites,
    Satellite **targets,
    int num_targets,
    TaskAssignment *assignment
);

/* ==================== 全局策略决策 ==================== */

/* 全局决策上下文 */
typedef struct {
    Satellite **red_satellites;
    int num_red;
    
    Satellite **blue_satellites;
    int num_blue;
    
    /* 决策结果 */
    uint8_t *formation_assignments;  // 每颗红星的编队类型
    int *target_assignments;         // 每颗红星的目标ID
    StrategyType *strategy_assignments; // 每颗红星的策略类型
} DecisionContext;

/* 创建决策上下文 */
DecisionContext* decision_context_create(int num_red, int num_blue);

/* 销毁决策上下文 */
void decision_context_destroy(DecisionContext *ctx);

/* 全局决策树执行 */
int decision_tree_execute(
    DecisionContext *ctx
);

/* Silhouette检查 */
int decision_tree_silhouette_check(KMeansResult *result, double threshold);

/* ==================== 聚类评估 ==================== */

/* Silhouette系数 */
int decision_tree_calculate_silhouette(
    Satellite **satellites,
    int num_satellites,
    KMeansResult *result
);

/* Davies-Bouldin指数 */
double decision_tree_calculate_davies_bouldin(
    KMeansResult *result
);

/* ==================== 调试和信息 ==================== */

/* 打印K-Means结果 */
void kmeans_result_print(KMeansResult *result);

/* 打印分组信息 */
void decision_tree_print_groups(KMeansResult *result);

/* 打印任务分配 */
void decision_tree_print_task_assignment(TaskAssignment *assignment);

#endif /* DECISION_TREE_H */