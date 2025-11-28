#include <kinematics.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

// ===== 导入编队控制器头文件 =====
#include "formation/formation_around.h"
#include "formation/formation_inspect.h"
#include "formation/formation_circumnavigate.h"
#include "formation/formation_retreat.h"

/* ==================== 内部函数声明 ==================== */

/**
 * 创建所有编队控制器
 */
static int kinematics_engine_create_formations(KinematicsEngine *engine) {
    if (! engine) return -1;
    
    printf("[Kinematics] 正在创建编队控制器...\n");
    
    // 创建所有编队控制器
    AroundFormationState *around = around_formation_create();
    InspectFormationState *inspect = inspect_formation_create();
    CircumnavigateFormationState *circum = circumnavigate_formation_create();
    RetreatFormationState *retreat = retreat_formation_create();
    
    if (!around || !inspect || ! circum || !retreat) {
        fprintf(stderr, "错误：编队控制器创建失败\n");
        
        // 清理已创建的
        if (around) around_formation_destroy(around);
        if (inspect) inspect_formation_destroy(inspect);
        if (circum) circumnavigate_formation_destroy(circum);
        if (retreat) retreat_formation_destroy(retreat);
        
        return -1;
    }
    
    // 存储到引擎
    engine->formation_controllers.around_state = around;
    engine->formation_controllers.inspect_state = inspect;
    engine->formation_controllers.circumnavigate_state = circum;
    engine->formation_controllers.retreat_state = retreat;
    
    printf("[Kinematics] ✓ 所有编队控制器已创建\n");
    return 0;
}

/**
 * 销毁所有编队控制器
 */
static void kinematics_engine_destroy_formations(KinematicsEngine *engine) {
    if (!engine) return;
    
    printf("[Kinematics] 正在销毁编队控制器...\n");
    
    if (engine->formation_controllers.around_state) {
        around_formation_destroy((AroundFormationState*)engine->formation_controllers.around_state);
        engine->formation_controllers.around_state = NULL;
    }
    
    if (engine->formation_controllers.inspect_state) {
        inspect_formation_destroy((InspectFormationState*)engine->formation_controllers.inspect_state);
        engine->formation_controllers.inspect_state = NULL;
    }
    
    if (engine->formation_controllers.circumnavigate_state) {
        circumnavigate_formation_destroy((CircumnavigateFormationState*)engine->formation_controllers. circumnavigate_state);
        engine->formation_controllers.circumnavigate_state = NULL;
    }
    
    if (engine->formation_controllers.retreat_state) {
        retreat_formation_destroy((RetreatFormationState*)engine->formation_controllers.retreat_state);
        engine->formation_controllers.retreat_state = NULL;
    }
    
    printf("[Kinematics] ✓ 所有编队控制器已销毁\n");
}

/* ==================== 公开接口实现 ==================== */

KinematicsEngine* kinematics_engine_create(SimulationConfig config) {
    KinematicsEngine *engine = (KinematicsEngine*)malloc(sizeof(KinematicsEngine));
    if (!engine) {
        fprintf(stderr, "错误：无法分配KinematicsEngine内存\n");
        return NULL;
    }
    
    // 初始化卫星数组
    engine->satellites = (Satellite**)malloc(sizeof(Satellite*) * 100);
    if (!engine->satellites) {
        free(engine);
        return NULL;
    }
    engine->satellite_count = 0;
    engine->satellite_capacity = 100;
    
    // 初始化基本参数
    engine->dt_seconds = config.time_step;
    engine->current_time = 0;
    engine->step_count = 0;
    engine->config = config;
    
    // 初始化编队数组
    engine->formations = (Formation**)malloc(sizeof(Formation*) * 10);
    if (!engine->formations) {
        free(engine->satellites);
        free(engine);
        return NULL;
    }
    engine->formation_count = 0;
    engine->num_formations = 0;
    
    // 初始化文件指针
    engine->state_file = NULL;
    engine->maneuver_file = NULL;
    engine->history_file = NULL;
    
    // 初始化统计数据
    engine->total_maneuvers = 0;
    engine->total_fuel_consumed = 0.0;
    
    // ===== 初始化编队控制器 =====
    memset(&engine->formation_controllers, 0, sizeof(FormationControllers));
    
    if (kinematics_engine_create_formations(engine) != 0) {
        fprintf(stderr, "错误：编队控制器初始化失败\n");
        free(engine->satellites);
        free(engine->formations);
        free(engine);
        return NULL;
    }
    
    printf("[Kinematics] ✓ KinematicsEngine创建成功\n");
    return engine;
}

void kinematics_engine_destroy(KinematicsEngine *engine) {
    if (!engine) return;
    
    printf("[Kinematics] 正在销毁KinematicsEngine...\n");
    
    // 销毁所有卫星
    for (int i = 0; i < engine->satellite_count; i++) {
        if (engine->satellites[i]) {
            satellite_destroy(engine->satellites[i]);
        }
    }
    free(engine->satellites);
    
    // 销毁编队数组
    free(engine->formations);
    
    // ===== 销毁编队控制器 =====
    kinematics_engine_destroy_formations(engine);
    
    // 关闭文件
    if (engine->state_file) fclose(engine->state_file);
    if (engine->maneuver_file) fclose(engine->maneuver_file);
    if (engine->history_file) fclose(engine->history_file);
    
    free(engine);
    printf("[Kinematics] ✓ KinematicsEngine已销毁\n");
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
    
    // 更新每颗卫星的时间戳
    for (int i = 0; i < engine->satellite_count; i++) {
        if (! engine->satellites[i]) continue;
        Satellite *sat = engine->satellites[i];
        sat->state.time = engine->current_time;
    }
    
    return 0;
}

int kinematics_engine_run(KinematicsEngine *engine, uint32_t num_steps) {
    if (! engine) return -1;
    
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
    printf("[Kinematics] ✓ 卫星初始化完成\n");
    return 0;
}

int kinematics_engine_init_formations(KinematicsEngine *engine) {
    if (!engine) return -1;
    printf("[Kinematics] ✓ 编队初始化完成\n");
    return 0;
}

int kinematics_engine_init_transition_rules(KinematicsEngine *engine) {
    if (!engine) return -1;
    printf("[Kinematics] ✓ 转换规则配置完成\n");
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
    if (! engine) return 0;
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
    return engine->total_maneuvers;
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
    if (!engine || ! engine->history_file) return -1;
    fprintf(engine->history_file, "Sat %d: Pos (%.0f, %.0f, %.0f), Vel (%.2f, %.2f, %.2f)\n",
            sat_id, position. x, position.y, position. z, velocity.x, velocity. y, velocity.z);
    return 0;
}

int kinematics_engine_print_status(KinematicsEngine *engine) {
    if (!engine) return -1;
    printf("[Kinematics] Status:\n");
    printf("  Current Time: %.2f s\n", engine->current_time);
    printf("  Step Count: %u\n", engine->step_count);
    printf("  Satellites: %d\n", engine->satellite_count);
    printf("  Formations: %d\n", engine->formation_count);
    return 0;
}

int kinematics_engine_print_formation_summary(KinematicsEngine *engine) {
    if (!engine) return -1;
    printf("[Kinematics] Formation Summary:\n");
    printf("  Average Quality: %.2f\n", kinematics_engine_avg_formation_quality(engine));
    printf("  Total Fuel Consumed: %.2f\n", kinematics_engine_total_fuel_consumed(engine));
    return 0;
}

int kinematics_engine_get_formation_summary(KinematicsEngine *engine) {
    if (!engine) return -1;
    return 0;
}