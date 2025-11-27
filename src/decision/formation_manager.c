#include <decision/formation_manager.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

FormationManager* formation_manager_create(void) {
    FormationManager *fm = (FormationManager*)malloc(sizeof(FormationManager));
    if (!fm) return NULL;
    
    memset(fm, 0, sizeof(FormationManager));
    
    fm->satellites = (Satellite**)calloc(100, sizeof(Satellite*));
    fm->triggers = (FormationTrigger*)calloc(100, sizeof(FormationTrigger));
    fm->transition_rules = (FormationTransitionRule*)calloc(50, sizeof(FormationTransitionRule));
    
    if (!fm->satellites || !fm->triggers || !fm->transition_rules) {
        formation_manager_destroy(fm);
        return NULL;
    }
    
    fm->num_satellites = 0;
    fm->num_rules = 0;
    
    return fm;
}

void formation_manager_destroy(FormationManager *fm) {
    if (!fm) return;
    free(fm->satellites);
    free(fm->triggers);
    free(fm->transition_rules);
    free(fm);
}

int formation_manager_register_satellite(FormationManager *fm, Satellite *sat) {
    if (!fm || !sat) return -1;
    if (fm->num_satellites >= 100) return -1;
    
    fm->satellites[fm->num_satellites] = sat;
    memset(&fm->triggers[fm->num_satellites], 0, sizeof(FormationTrigger));
    fm->triggers[fm->num_satellites].sat_id = sat->id;
    
    fm->num_satellites++;
    return 0;
}

int formation_manager_remove_satellite(FormationManager *fm, int sat_id) {
    if (!fm) return -1;
    
    for (int i = 0; i < fm->num_satellites; i++) {
        if (fm->satellites[i] && fm->satellites[i]->id == sat_id) {
            fm->satellites[i] = NULL;
            return 0;
        }
    }
    
    return -1;
}

int formation_manager_update_trigger(FormationManager *fm, int sat_id, uint32_t circle_count, 
                                     double circle_progress, double distance) {
    if (!fm) return -1;
    
    for (int i = 0; i < fm->num_satellites; i++) {
        if (fm->triggers[i].sat_id == sat_id) {
            fm->triggers[i].circle_count = circle_count;
            fm->triggers[i].circle_progress = circle_progress;
            fm->triggers[i].distance_to_target = distance;
            return 0;
        }
    }
    
    return -1;
}

double formation_manager_get_trigger(FormationManager *fm, int sat_id) {
    if (!fm) return 0.0;
    
    for (int i = 0; i < fm->num_satellites; i++) {
        if (fm->triggers[i].sat_id == sat_id) {
            return fm->triggers[i].distance_to_target;
        }
    }
    
    return 0.0;
}

int formation_manager_add_transition_rule(FormationManager *fm, uint8_t from_formation, 
                                          uint8_t to_formation, const char *condition_name) {
    if (!fm || fm->num_rules >= 50) return -1;
    
    fm->transition_rules[fm->num_rules].from_formation = from_formation;
    fm->transition_rules[fm->num_rules].to_formation = to_formation;
    fm->transition_rules[fm->num_rules].enabled = 1;
    
    if (condition_name) {
        strncpy(fm->transition_rules[fm->num_rules].condition_name, condition_name, 63);
    }
    
    fm->num_rules++;
    return 0;
}

int formation_manager_remove_transition_rule(FormationManager *fm, uint8_t from_formation, 
                                             uint8_t to_formation) {
    if (!fm) return -1;
    
    for (int i = 0; i < fm->num_rules; i++) {
        if (fm->transition_rules[i].from_formation == from_formation &&
            fm->transition_rules[i].to_formation == to_formation) {
            fm->transition_rules[i].enabled = 0;
            return 0;
        }
    }
    
    return -1;
}

uint8_t formation_manager_check_transition(FormationManager *fm, uint8_t current_formation, 
                                           double trigger_value) {
    if (!fm) return current_formation;
    
    for (int i = 0; i < fm->num_rules; i++) {
        if (fm->transition_rules[i].from_formation == current_formation &&
            fm->transition_rules[i].enabled) {
            return fm->transition_rules[i].to_formation;
        }
    }
    
    return current_formation;
}

int formation_manager_get_formation_type(FormationManager *fm, int sat_id) {
    if (!fm) return -1;
    
    for (int i = 0; i < fm->num_satellites; i++) {
        if (fm->triggers[i].sat_id == sat_id) {
            return fm->triggers[i].current_formation;
        }
    }
    
    return -1;
}

int formation_manager_get_formation_count(FormationManager *fm) {
    if (!fm) return 0;
    return fm->num_satellites;
}

Satellite** formation_manager_get_formation_satellites(FormationManager *fm, int formation_id, 
                                                      int *count) {
    if (!fm || !count) return NULL;
    *count = 0;
    return fm->satellites;
}

void formation_manager_print_status(FormationManager *mgr) {
    if (!mgr) return;
    printf("FormationManager: %d satellites, %d rules\n", mgr->num_satellites, mgr->num_rules);
}