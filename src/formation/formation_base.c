#include <formation/formation_base.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

Formation* formation_create(
    const char *name,
    formation_init_func init,
    formation_update_func update,
    formation_check_status_func check_status,
    formation_destroy_func destroy
) {
    Formation *f = (Formation*)malloc(sizeof(Formation));
    if (!f) return NULL;
    
    char *name_copy = (char*)malloc(strlen(name) + 1);
    if (name_copy) {
        strcpy(name_copy, name);
        f->name = (const char*)name_copy;
    } else {
        f->name = "";
    }
    
    f->init = init;
    f->update = update;
    f->check_status = check_status;
    f->destroy = destroy;
    f->private_data = NULL;
    
    return f;
}

void formation_destroy(Formation *f) {
    if (!f) return;
    
    if (f->destroy) {
        f->destroy(f);
    }
    
    if (f->name) {
        char *name_ptr = (char*)f->name;
        free(name_ptr);
    }
    
    free(f);
}

int formation_init(Formation *formation) {
    if (!formation) return -1;
    if (formation->init) {
        return formation->init(formation);
    }
    return 0;
}

int formation_update(Formation *f, Satellite *chaser, Satellite *target) {
    if (!f || !chaser || !target) return -1;
    if (f->update) {
        return f->update(f, chaser, target);
    }
    return 0;
}

int formation_check_status(Formation *f, Satellite *chaser) {
    if (!f || !chaser) return -1;
    if (f->check_status) {
        return f->check_status(f, chaser);
    }
    return 0;
}

void formation_set_private_data(Formation *f, void *data) {
    if (!f) return;
    f->private_data = data;
}

void* formation_get_private_data(Formation *f) {
    if (!f) return NULL;
    return f->private_data;
}

Formation* formation_create_by_type(uint8_t formation_type) {
    Formation *f = formation_create("formation", NULL, NULL, NULL, NULL);
    if (!f) return NULL;
    return f;
}

const char* formation_get_type_name(uint8_t formation_type) {
    switch (formation_type) {
        case 0: return "INSPECT";
        case 1: return "AROUND";
        case 2: return "CIRCUMNAVIGATE";
        case 3: return "RETREAT";
        case 4: return "FREE";
        default: return "UNKNOWN";
    }
}

double formation_calculate_proximity(
    Satellite *chaser,
    Satellite *target,
    double target_distance
) {
    if (!chaser || !target) return 0;
    
    double distance = vector3_distance(chaser->state.position, target->state.position);
    if (distance < target_distance * 0.1) return 1.0;
    if (distance > target_distance * 10) return 0.0;
    
    return 1.0 - (distance - target_distance * 0.1) / (target_distance * 9.9);
}

double formation_calculate_quality(
    Satellite *chaser,
    Satellite *target,
    double target_distance
) {
    if (!chaser || !target) return 0;
    
    double proximity = formation_calculate_proximity(chaser, target, target_distance);
    double velocity_sim = 0.9;
    
    return proximity * 0.6 + velocity_sim * 0.4;
}

int formation_is_approaching_target(
    Satellite *chaser,
    Satellite *target,
    double threshold_distance
) {
    if (!chaser || !target) return 0;
    
    double distance = vector3_distance(chaser->state.position, target->state.position);
    return (distance < threshold_distance) ? 1 : 0;
}

int formation_check_transition_condition(
    Satellite *chaser,
    Satellite *target,
    uint8_t from_formation,
    uint8_t to_formation
) {
    if (!chaser || !target) return 0;
    return 0;
}

double formation_transition_min_time(uint8_t from_formation, uint8_t to_formation) {
    return 0;
}