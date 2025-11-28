#ifndef KINEMATICS_H
#define KINEMATICS_H

#include <types.h>
#include <satellite.h>
#include <orbit.h>
#include <attitude.h>
#include <decision/formation_manager.h>

/* ==================== 编队控制器结构 ==================== */
typedef struct {
    void *around_state;              // AroundFormationState*
    void *inspect_state;             // InspectFormationState*
    void *circumnavigate_state;      // CircumnavigateFormationState*
    void *retreat_state;             // RetreatFormationState*
} FormationControllers;

/* ==================== 运动学引擎结构 ==================== */
typedef struct KinematicsEngine {
    Satellite **satellites;
    int satellite_count;
    int satellite_capacity;
    int formation_count;
    
    double current_time;
    uint32_t step_count;
    double dt_seconds;
    
    SimulationConfig config;
    
    FormationManager *formation_manager;
    Formation **formations;
    int num_formations;
    
    StrategyThresholds strategy_thresholds;
    
    // ===== 新增：编队控制器 =====
    FormationControllers formation_controllers;
    
    FILE *state_file;
    FILE *maneuver_file;
    FILE *history_file;
    
    uint32_t total_maneuvers;
    double total_fuel_consumed;
} KinematicsEngine;

/* ==================== 基本引擎函数 ==================== */
KinematicsEngine* kinematics_engine_create(SimulationConfig config);
void kinematics_engine_destroy(KinematicsEngine *engine);

int kinematics_engine_add_satellite(KinematicsEngine *engine, Satellite *sat);
Satellite* kinematics_engine_get_satellite(KinematicsEngine *engine, int sat_id);
Satellite** kinematics_engine_get_all_satellites(KinematicsEngine *engine, int *count);
int kinematics_engine_remove_satellite(KinematicsEngine *engine, int sat_id);

int kinematics_engine_step(KinematicsEngine *engine);
int kinematics_engine_run(KinematicsEngine *engine, uint32_t num_steps);

int kinematics_engine_init_satellites(KinematicsEngine *engine);
int kinematics_engine_init_formations(KinematicsEngine *engine);
int kinematics_engine_init_transition_rules(KinematicsEngine *engine);

double kinematics_engine_get_current_time(KinematicsEngine *engine);
uint32_t kinematics_engine_get_step_count(KinematicsEngine *engine);
int kinematics_engine_should_continue(KinematicsEngine *engine);

double kinematics_engine_avg_formation_quality(KinematicsEngine *engine);
int kinematics_engine_count_formations(KinematicsEngine *engine);
double kinematics_engine_total_fuel_consumed(KinematicsEngine *engine);
int kinematics_engine_total_maneuvers(KinematicsEngine *engine);

int kinematics_engine_open_state_file(KinematicsEngine *engine, const char *filename);
int kinematics_engine_open_maneuver_file(KinematicsEngine *engine, const char *filename);
int kinematics_engine_open_history_file(KinematicsEngine *engine, const char *filename);
int kinematics_engine_close_files(KinematicsEngine *engine);

int kinematics_engine_write_state(KinematicsEngine *engine, uint32_t step, double time);
int kinematics_engine_write_maneuver(KinematicsEngine *engine, int sat_id, double delta_v, double time);
int kinematics_engine_write_trajectory(KinematicsEngine *engine, int sat_id, Vector3 position, Vector3 velocity);

int kinematics_engine_print_status(KinematicsEngine *engine);
int kinematics_engine_print_formation_summary(KinematicsEngine *engine);
int kinematics_engine_get_formation_summary(KinematicsEngine *engine);

#endif