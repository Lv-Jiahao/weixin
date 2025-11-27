#include <kinematics.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

KinematicsEngine* kinematics_engine_create(SimulationConfig config) {
    KinematicsEngine *engine = (KinematicsEngine*)malloc(sizeof(KinematicsEngine));
    if (!engine) return NULL;
    
    engine->satellites = (Satellite**)malloc(sizeof(Satellite*) * 100);
    engine->satellite_count = 0;
    engine->satellite_capacity = 100;
    
    engine->dt_seconds = config.time_step;
    engine->current_time = 0;
    engine->step_count = 0;
    engine->config = config;
    
    engine->formations = (Formation**)malloc(sizeof(Formation*) * 10);
    engine->formation_count = 0;
    
    engine->state_file = NULL;
    engine->maneuver_file = NULL;
    engine->history_file = NULL;
    
    return engine;
}

void kinematics_engine_destroy(KinematicsEngine *engine) {
    if (!engine) return;
    
    for (int i = 0; i < engine->satellite_count; i++) {
        if (engine->satellites[i]) satellite_destroy(engine->satellites[i]);
    }
    free(engine->satellites);
    free(engine->formations);
    
    if (engine->state_file) fclose(engine->state_file);
    if (engine->maneuver_file) fclose(engine->maneuver_file);
    if (engine->history_file) fclose(engine->history_file);
    
    free(engine);
}

int kinematics_engine_add_satellite(KinematicsEngine *engine, Satellite *sat) {
    if (!engine || !sat) return -1;
    if (engine->satellite_count >= engine->satellite_capacity) return -2;
    
    engine->satellites[engine->satellite_count] = sat;
    engine->satellite_count++;
    return engine->satellite_count - 1;
}

Satellite* kinematics_engine_get_satellite(KinematicsEngine *engine, int sat_id) {
    if (!engine) return NULL;
    for (int i = 0; i < engine->satellite_count; i++) {
        if (engine->satellites[i] && engine->satellites[i]->id == sat_id) {
            return engine->satellites[i];
        }
    }
    return NULL;
}

Satellite** kinematics_engine_get_all_satellites(KinematicsEngine *engine, int *count) {
    if (!engine || !count) return NULL;
    *count = engine->satellite_count;
    return engine->satellites;
}

int kinematics_engine_remove_satellite(KinematicsEngine *engine, int sat_id) {
    if (!engine) return -1;
    for (int i = 0; i < engine->satellite_count; i++) {
        if (engine->satellites[i] && engine->satellites[i]->id == sat_id) {
            satellite_destroy(engine->satellites[i]);
            for (int j = i; j < engine->satellite_count - 1; j++) {
                engine->satellites[j] = engine->satellites[j + 1];
            }
            engine->satellite_count--;
            return 0;
        }
    }
    return -1;
}

int kinematics_engine_step(KinematicsEngine *engine) {
    if (!engine) return -1;
    
    engine->current_time += engine->dt_seconds;
    engine->step_count++;
    
    for (int i = 0; i < engine->satellite_count; i++) {
        if (!engine->satellites[i]) continue;
        Satellite *sat = engine->satellites[i];
        sat->state.time = engine->current_time;
    }
    
    return 0;
}

int kinematics_engine_run(KinematicsEngine *engine, uint32_t num_steps) {
    if (!engine) return -1;
    
    for (uint32_t i = 0; i < num_steps; i++) {
        if (kinematics_engine_step(engine) < 0) return -1;
    }
    
    return 0;
}

int kinematics_engine_init_satellites(KinematicsEngine *engine) {
    if (!engine) return -1;
    for (int i = 0; i < engine->satellite_count; i++) {
        if (engine->satellites[i]) {
            satellite_init_history(engine->satellites[i], 10000);
        }
    }
    return 0;
}

int kinematics_engine_init_formations(KinematicsEngine *engine) {
    if (!engine) return -1;
    return 0;
}

int kinematics_engine_init_transition_rules(KinematicsEngine *engine) {
    if (!engine) return -1;
    return 0;
}

double kinematics_engine_get_current_time(KinematicsEngine *engine) {
    if (!engine) return 0;
    return engine->current_time;
}

uint32_t kinematics_engine_get_step_count(KinematicsEngine *engine) {
    if (!engine) return 0;
    return engine->step_count;
}

int kinematics_engine_should_continue(KinematicsEngine *engine) {
    if (!engine) return 0;
    return (engine->step_count < engine->config.max_steps);
}

double kinematics_engine_avg_formation_quality(KinematicsEngine *engine) {
    if (!engine) return 0;
    return 0.5;
}

int kinematics_engine_count_formations(KinematicsEngine *engine) {
    if (!engine) return 0;
    return engine->formation_count;
}

double kinematics_engine_total_fuel_consumed(KinematicsEngine *engine) {
    if (!engine) return 0;
    double total = 0;
    for (int i = 0; i < engine->satellite_count; i++) {
        if (engine->satellites[i]) {
            total += engine->satellites[i]->total_fuel_used;
        }
    }
    return total;
}

int kinematics_engine_total_maneuvers(KinematicsEngine *engine) {
    if (!engine) return 0;
    return 0;
}

int kinematics_engine_open_state_file(KinematicsEngine *engine, const char *filename) {
    if (!engine || !filename) return -1;
    engine->state_file = fopen(filename, "w");
    return (engine->state_file) ? 0 : -1;
}

int kinematics_engine_open_maneuver_file(KinematicsEngine *engine, const char *filename) {
    if (!engine || !filename) return -1;
    engine->maneuver_file = fopen(filename, "w");
    return (engine->maneuver_file) ? 0 : -1;
}

int kinematics_engine_open_history_file(KinematicsEngine *engine, const char *filename) {
    if (!engine || !filename) return -1;
    engine->history_file = fopen(filename, "w");
    return (engine->history_file) ? 0 : -1;
}

int kinematics_engine_close_files(KinematicsEngine *engine) {
    if (!engine) return -1;
    if (engine->state_file) fclose(engine->state_file);
    if (engine->maneuver_file) fclose(engine->maneuver_file);
    if (engine->history_file) fclose(engine->history_file);
    engine->state_file = NULL;
    engine->maneuver_file = NULL;
    engine->history_file = NULL;
    return 0;
}

int kinematics_engine_write_state(KinematicsEngine *engine, uint32_t step, double time) {
    if (!engine || !engine->state_file) return -1;
    fprintf(engine->state_file, "Step %u, Time %.2f\n", step, time);
    return 0;
}

int kinematics_engine_write_maneuver(KinematicsEngine *engine, int sat_id, double delta_v, double time) {
    if (!engine || !engine->maneuver_file) return -1;
    fprintf(engine->maneuver_file, "Sat %d, DeltaV %.2f, Time %.2f\n", sat_id, delta_v, time);
    return 0;
}

int kinematics_engine_write_trajectory(KinematicsEngine *engine, int sat_id, Vector3 position, Vector3 velocity) {
    if (!engine || !engine->history_file) return -1;
    fprintf(engine->history_file, "Sat %d: Pos (%.0f, %.0f, %.0f), Vel (%.2f, %.2f, %.2f)\n",
            sat_id, position.x, position.y, position.z, velocity.x, velocity.y, velocity.z);
    return 0;
}

int kinematics_engine_print_status(KinematicsEngine *engine) {
    if (!engine) return -1;
    printf("KinematicsEngine Status:\n");
    printf("  Current Time: %.2f s\n", engine->current_time);
    printf("  Step Count: %u\n", engine->step_count);
    printf("  Satellites: %d\n", engine->satellite_count);
    printf("  Formations: %d\n", engine->formation_count);
    return 0;
}

int kinematics_engine_print_formation_summary(KinematicsEngine *engine) {
    if (!engine) return -1;
    printf("Formation Summary:\n");
    printf("  Average Quality: %.2f\n", kinematics_engine_avg_formation_quality(engine));
    printf("  Total Fuel Consumed: %.2f\n", kinematics_engine_total_fuel_consumed(engine));
    return 0;
}

int kinematics_engine_get_formation_summary(KinematicsEngine *engine) {
    if (!engine) return -1;
    return 0;
}