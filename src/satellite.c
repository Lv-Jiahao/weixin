#include <satellite.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

Satellite* satellite_create(int id, uint8_t type, uint8_t function_type) {
    Satellite *sat = (Satellite*)malloc(sizeof(Satellite));
    if (!sat) return NULL;
    
    sat->id = id;
    sat->type = type;
    sat->function_type = function_type;
    sat->orbital_elements.a = 7000000;
    sat->orbital_elements.e = 0.01;
    sat->orbital_elements.i = 0.1;
    sat->orbital_elements.omega_big = 0;
    sat->orbital_elements.omega_small = 0;
    sat->orbital_elements.m0 = 0;
    sat->fuel = 1000.0;
    sat->fuel_consumption_rate = 0.1;
    sat->total_fuel_used = 0;
    sat->current_formation = 255;
    sat->target_id = -1;
    sat->distance_to_target = 1e10;
    sat->min_distance_reached = 1e10;
    sat->circle_count = 0;
    sat->circle_progress = 0;
    sat->inspection_started = 0;
    
    sat->attitude.q = (Quaternion){0, 0, 0, 1};
    sat->attitude.omega = (Vector3){0, 0, 0};
    sat->attitude.alpha = (Vector3){0, 0, 0};
    sat->attitude.max_rate = 0.1;
    sat->attitude.max_accel = 0.01;
    sat->attitude.body_axis = (Vector3){0, 0, 1};
    sat->attitude.target_id = -1;
    sat->attitude.step_count = 0;
    
    sat->state.position = (Vector3){7000000, 0, 0};
    sat->state.velocity = (Vector3){0, 7546, 0};
    sat->state.time = 0;
    
    sat->position_history = NULL;
    sat->velocity_history = NULL;
    sat->formation_history = NULL;
    sat->history_count = 0;
    sat->history_capacity = 0;
    
    return sat;
}

void satellite_destroy(Satellite *sat) {
    if (!sat) return;
    if (sat->position_history) free(sat->position_history);
    if (sat->velocity_history) free(sat->velocity_history);
    if (sat->formation_history) free(sat->formation_history);
    free(sat);
}

void satellite_init_history(Satellite *sat, int capacity) {
    if (!sat) return;
    sat->history_capacity = capacity;
    sat->position_history = (Vector3*)malloc(sizeof(Vector3) * capacity);
    sat->velocity_history = (Vector3*)malloc(sizeof(Vector3) * capacity);
    sat->formation_history = (int*)malloc(sizeof(int) * capacity);
    sat->history_count = 0;
}

void satellite_append_history(Satellite *sat) {
    if (!sat || sat->history_count >= sat->history_capacity) return;
    sat->position_history[sat->history_count] = sat->state.position;
    sat->velocity_history[sat->history_count] = sat->state.velocity;
    sat->formation_history[sat->history_count] = sat->current_formation;
    sat->history_count++;
}

void satellite_clear_history(Satellite *sat) {
    if (!sat) return;
    sat->history_count = 0;
}

void satellite_update_formation(Satellite *sat, uint8_t formation_type) {
    if (!sat) return;
    sat->current_formation = formation_type;
    sat->circle_count = 0;
    sat->circle_progress = 0;
}

void satellite_increment_circle(Satellite *sat) {
    if (!sat) return;
    sat->circle_count++;
    sat->circle_progress = 0;
}

double satellite_get_circle_progress(Satellite *sat) {
    if (!sat) return 0;
    return sat->circle_progress;
}

double satellite_distance_to_satellite(Satellite *sat1, Satellite *sat2) {
    if (!sat1 || !sat2) return 1e10;
    return vector3_distance(sat1->state.position, sat2->state.position);
}

Vector3 satellite_direction_to_satellite(Satellite *sat1, Satellite *sat2) {
    if (!sat1 || !sat2) return (Vector3){0, 0, 0};
    Vector3 diff = vector3_sub(sat2->state.position, sat1->state.position);
    return vector3_normalize(diff);
}

Vector3 satellite_direction_to_target(Satellite *sat, Satellite *target) {
    if (!sat || !target) return (Vector3){0, 0, 0};
    Vector3 diff = vector3_sub(target->state.position, sat->state.position);
    return vector3_normalize(diff);
}

int satellite_consume_fuel(Satellite *sat, double delta_v) {
    if (!sat) return -1;
    double fuel_needed = fabs(delta_v) / 100.0;
    if (sat->fuel < fuel_needed) return 0;
    sat->fuel -= fuel_needed;
    sat->total_fuel_used += fuel_needed;
    return 1;
}

int satellite_has_sufficient_fuel(Satellite *sat, double delta_v) {
    if (!sat) return 0;
    double fuel_needed = fabs(delta_v) / 100.0;
    return (sat->fuel >= fuel_needed);
}

double satellite_get_fuel_percentage(Satellite *sat) {
    if (!sat) return 0;
    return (sat->fuel / 1000.0);
}

void satellite_set_target(Satellite *sat, int target_id) {
    if (!sat) return;
    sat->target_id = target_id;
}

void satellite_print(Satellite *sat) {
    if (!sat) return;
    printf("Satellite #%d:\n", sat->id);
    printf("  Type: %d, Function: %d\n", sat->type, sat->function_type);
    printf("  Position: (%.0f, %.0f, %.0f)\n", sat->state.position.x, sat->state.position.y, sat->state.position.z);
    printf("  Velocity: (%.2f, %.2f, %.2f)\n", sat->state.velocity.x, sat->state.velocity.y, sat->state.velocity.z);
    printf("  Fuel: %.1f%%\n", satellite_get_fuel_percentage(sat) * 100.0);
}

int satellite_elements_to_state(Satellite *sat) {
    if (!sat) return -1;
    return 0;
}

int satellite_state_to_elements(Satellite *sat) {
    if (!sat) return -1;
    return 0;
}

void satellite_set_formation_progress(Satellite *sat, double progress) {
    if (!sat) return;
    sat->circle_progress = (progress < 0) ? 0 : (progress > 1) ? 1 : progress;
}