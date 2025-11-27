/* 编队状态机和切换管理 */
#ifndef FORMATION_MANAGER_H
#define FORMATION_MANAGER_H

#include <types.h>
#include <satellite.h>

typedef struct {
    uint8_t from_formation;
    uint8_t to_formation;
    int enabled;
    char condition_name[64];
} FormationTransitionRule;

typedef struct {
    int num_satellites;
    Satellite **satellites;
    FormationTrigger *triggers;
    FormationTransitionRule *transition_rules;
    int num_rules;
} FormationManager;

/* 创建和销毁 */
FormationManager* formation_manager_create(void);
void formation_manager_destroy(FormationManager *fm);

/* 卫星管理 */
int formation_manager_register_satellite(FormationManager *fm, Satellite *sat);
int formation_manager_remove_satellite(FormationManager *fm, int sat_id);

/* 触发器管理 */
int formation_manager_update_trigger(FormationManager *fm, int sat_id, uint32_t circle_count, double circle_progress, double distance);
double formation_manager_get_trigger(FormationManager *fm, int sat_id);

/* 转换规则 */
int formation_manager_add_transition_rule(FormationManager *fm, uint8_t from_formation, uint8_t to_formation, const char *condition_name);
int formation_manager_remove_transition_rule(FormationManager *fm, uint8_t from_formation, uint8_t to_formation);
uint8_t formation_manager_check_transition(FormationManager *fm, uint8_t current_formation, double trigger_value);

/* 查询 */
int formation_manager_get_formation_type(FormationManager *fm, int sat_id);
int formation_manager_get_formation_count(FormationManager *fm);
Satellite** formation_manager_get_formation_satellites(FormationManager *fm, int formation_id, int *count);

/* 调试 */
void formation_manager_print_status(FormationManager *mgr);

#endif