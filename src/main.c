#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include <constants.h>
#include <types.h>
#include <satellite.h>
#include <kinematics.h>
#include "config/config.h"
#include <decision/decision_tree.h>
#include <decision/differential_game.h>

// static const char *OUTPUT_DIR = "output";


Config Init_config = {
    .red_attack = 3,
    .red_recon = 3,
    .red_defense = 3,
    .blue_attack = 2,
    .blue_recon = 2,
    .blue_defense = 2,
    .strategy = "GJ",
    .max_time = 500000.0,
    .time_step = 1.0
};

void print_banner(void) {
    printf("\n");
    printf("╔════════════════════════════════════════════════════════════╗\n");
    printf("║       卫星编队控制系统 - C语言版本                          ║\n");
    printf("║                                                            ║\n");
    printf("║  核心功能：                                               ║\n");
    printf("║  • 多卫星轨道动力学仿真                                   ║\n");
    printf("║  • 4种编队构型控制                                        ║\n");
    printf("║  • 决策树分组与微分博弈策略分配                           ║\n");
    printf("║  • 轨道机动（Lambert、Hohmann、RK4）                     ║\n");
    printf("║  • 姿态跟踪与四元数控制                                  ║\n");
    printf("╚════════════════════════════════════════════════════════════╝\n");
    printf("\n");
}

void print_progress(uint32_t step_count, uint32_t max_steps, double simulation_time) {
    int bar_length = 50;
    double progress = (double)step_count / max_steps;
    int filled = (int)(progress * bar_length);
    
    printf("\r[");
    for (int i = 0; i < bar_length; i++) {
        if (i < filled) printf("=");
        else printf("-");
    }
    printf("] %.1f%% | 步数: %u/%u | 仿真时间: %.2f小时",
           progress * 100.0, step_count, max_steps, simulation_time / 3600.0);
    fflush(stdout);
}

int initialize_simulation(KinematicsEngine **engine_out) {
    printf("正在初始化仿真...\n");
    // todo 时间步
    SimulationConfig config = {
        .time_step = 10.0,
        .max_steps = 10000,
        .save_interval = 100,
        .hohmann_precision = 1e-6,
        .lambert_max_iterations = 100,
        .lambert_convergence = 1e-6,
        .lambert_target_distance = 1000000,
        .lambert_max_delta_v = 5000,
        .ellipse_min_distance = 6500000,
        .ellipse_max_distance = 8000000
    };
    
    KinematicsEngine *engine = kinematics_engine_create(config);
    if (!engine) {
        fprintf(stderr, "错误：无法创建运动学引擎\n");
        return -1;
    }
    
    printf("✓ 运动学引擎已创建\n");

    // 红星数
    int red_sum = Init_config.red_attack + Init_config.red_recon + Init_config.red_defense;
    int id = 1000;

    // 红色 攻击卫星
    for (size_t i = 0; i < Init_config.red_attack; i++) {
        Satellite *sat = satellite_create(id, 0, 0, 0);  // 团队0=红队，功能0=攻击
        if (!sat) {
            fprintf(stderr, "错误：无法创建卫星 %d\n", i);
            kinematics_engine_destroy(engine);
            return -1;
        }
        kinematics_engine_add_satellite(engine, sat);
        id++;
    }

    // 红色 侦察卫星
    for (size_t i = 0; i < Init_config.red_recon; i++) {
        Satellite *sat = satellite_create(id, 0, 0, 1);  // 团队0=红队，功能1=侦察
        if (!sat) {
            fprintf(stderr, "错误：无法创建卫星 %d\n", i);
            kinematics_engine_destroy(engine);
            return -1;
        }
        kinematics_engine_add_satellite(engine, sat);
        id++;
    }

    // 红色 防御卫星
    for (size_t i = 0; i < Init_config.red_defense; i++) {
        Satellite *sat = satellite_create(id, 0, 0, 2);  // 团队0=红队，功能2=防御
        if (!sat) {
            fprintf(stderr, "错误：无法创建卫星 %d\n", i);
            kinematics_engine_destroy(engine);
            return -1;
        }
        kinematics_engine_add_satellite(engine, sat);
        id++;
    }

    int blue_sum = Init_config.blue_attack + Init_config.blue_recon + Init_config.blue_defense;

    // 蓝色 攻击卫星
    for (size_t i = 0; i < Init_config.blue_attack; i++) {
        Satellite *sat = satellite_create(id, 1, 0, 0);  // 团队1=蓝队，功能0=攻击
        if (!sat) {
            fprintf(stderr, "错误：无法创建卫星 %d\n", i);
            kinematics_engine_destroy(engine);
            return -1;
        }
        kinematics_engine_add_satellite(engine, sat);
        id++;
    }

    // 蓝色 侦察卫星
    for (size_t i = 0; i < Init_config.blue_recon; i++) {
        Satellite *sat = satellite_create(id, 1, 0, 1);  // 团队1=蓝队，功能1=侦察
        if (!sat) {
            fprintf(stderr, "错误：无法创建卫星 %d\n", i);
            kinematics_engine_destroy(engine);
            return -1;
        }
        kinematics_engine_add_satellite(engine, sat);
        id++;
    }

    // 蓝色 防御卫星
    for (size_t i = 0; i < Init_config.blue_defense; i++) {
        Satellite *sat = satellite_create(id, 1, 0, 2);  // 团队1=蓝队，功能2=防御
        if (!sat) {
            fprintf(stderr, "错误：无法创建卫星 %d\n", i);
            kinematics_engine_destroy(engine);
            return -1;
        }
        kinematics_engine_add_satellite(engine, sat);
        id++;
    }
    // for (size_t i = 0; i < red_sum; i++)
    // {
    //     Satellite *sat = satellite_create(i, , i % 3);
    //     /* code */
    // }
    
    
    // // 人为？？？
    // for (int i = 0; i < 5; i++) {
    //     Satellite *sat = satellite_create(i, i % 2, i % 3);
    //     if (!sat) {
    //         fprintf(stderr, "错误：无法创建卫星 %d\n", i);
    //         kinematics_engine_destroy(engine);
    //         return -1;
    //     }
    //     kinematics_engine_add_satellite(engine, sat);
    // }
    
    printf("✓ 已加载 %d 颗卫星\n", engine->satellite_count);
    printf("✓ 卫星初始化完成\n");
    printf("✓ 编队控制器初始化完成\n");
    printf("✓ 编队转换规则配置完成\n");
    
    *engine_out = engine;
    printf("\n✓ 仿真初始化完成！\n\n");
    return 0;
}

// int run_simulation(KinematicsEngine *engine, uint32_t max_steps, int verbose) {
//     printf("开始仿真循环...\n");
//     printf("━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n\n");
    
//     clock_t start_time = clock();
//     uint32_t step = 0;
    
//     while (step < max_steps && kinematics_engine_should_continue(engine)) {
//         if (kinematics_engine_step(engine) != 0) {
//             fprintf(stderr, "\n错误：第 %u 步仿真失败\n", step);
//             break;
//         }
        
//         step++;
//         double current_time = kinematics_engine_get_current_time(engine);
        
//         if (step % 100 == 0) {
//             print_progress(step, max_steps, current_time);
//         }
        
//         if (verbose && step % 1000 == 0) {
//             printf("\n");
//             printf("[第 %u 步] 仿真时间: %.2f小时\n", step, current_time / 3600.0);
//         }
//     }
    
//     double current_time = kinematics_engine_get_current_time(engine);
//     print_progress(step, max_steps, current_time);
//     printf("\n");
    
//     clock_t end_time = clock();
//     double elapsed = (double)(end_time - start_time) / CLOCKS_PER_SEC;
    
//     printf("\n━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n");
//     printf("仿真完成！\n");
//     printf("  仿真步数: %u\n", step);
//     printf("  仿真时长: %.2f小时\n", current_time / 3600.0);
//     printf("  执行耗时: %.2f秒\n", elapsed);
//     printf("  性能: %.2f 步/秒\n", step / elapsed);
//     printf("\n");
    
//     return 0;
// }

// int run_simulation(KinematicsEngine *engine, uint32_t max_steps, int verbose) {
//     printf("开始仿真循环...\n");
//     printf("━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n\n");
    
//     // ===== 打开输出文件 =====
//     FILE *output_file = fopen("red_satellites_trajectory.csv", "w");
//     if (! output_file) {
//         fprintf(stderr, "错误：无法打开输出文件\n");
//         return -1;
//     }
    
//     // 写入CSV头
//     fprintf(output_file, "step,time(s),sat_id,pos_x(km),pos_y(km),pos_z(km),"
//                         "vel_x(km/s),vel_y(km/s),vel_z(km/s),fuel(kg),formation,strategy\n");
    
//     clock_t start_time = clock();
//     uint32_t step = 0;
    
//     // 用于记录上一次的编队和策略
//     uint8_t *last_formations = (uint8_t*)calloc(engine->satellite_count, sizeof(uint8_t));
//     int *last_strategies = (int*)calloc(engine->satellite_count, sizeof(int));
    
//     while (step < max_steps && kinematics_engine_should_continue(engine)) {
//         // ===== 1. 获取所有卫星 =====
//         int num_sats = 0;
//         Satellite **all_sats = kinematics_engine_get_all_satellites(engine, &num_sats);
        
//         if (! all_sats || num_sats <= 0) break;
        
//         // ===== 2. 分离红蓝星 =====
//         Satellite **red_sats = (Satellite**)malloc(sizeof(Satellite*) * num_sats);
//         Satellite **blue_sats = (Satellite**)malloc(sizeof(Satellite*) * num_sats);
//         int num_red = 0, num_blue = 0;
        
//         for (int i = 0; i < num_sats; i++) {
//             if (all_sats[i]->team == 0) {
//                 red_sats[num_red++] = all_sats[i];
//             } else {
//                 blue_sats[num_blue++] = all_sats[i];
//             }
//         }
        
//         // ===== 3. 每100步执行一次决策 =====
//         if (step % 100 == 0 && num_red > 0 && num_blue > 0) {
//             printf("\n[Step %u] 执行决策和编队分配...\n", step);
            
//             // 决策树分组
//             GroupResult *groups = decision_tree_group_satellites(red_sats, num_red, 3);
            
//             // 微分博弈分配
//             GameResult *game = differential_game_assign_strategies(
//                 red_sats, num_red, blue_sats, num_blue, "GJ");
            
//             if (groups && game) {
//                 // 更新编队和策略
//                 for (int r = 0; r < num_red; r++) {
//                     uint8_t formation = decision_tree_select_formation(
//                         red_sats, num_red, groups, groups->group_ids[r]);
                    
//                     red_sats[r]->current_formation = formation;
//                     last_formations[r] = formation;
//                     last_strategies[r] = game->strategy_assignments[r];
//                 }
                
//                 printf("✓ 决策完成: %d个红星, %d组编队\n", num_red, groups->num_groups);
//             }
            
//             if (groups) decision_tree_free_result(groups);
//             if (game) differential_game_free_result(game);
//         }
        
//         // ===== 4. 仿真步进 =====
//         if (kinematics_engine_step(engine) != 0) {
//             fprintf(stderr, "\n错误：第 %u 步仿真失败\n", step);
//             break;
//         }
        
//         // ===== 5. 输出所有红星数据到CSV =====
//         for (int r = 0; r < num_red; r++) {
//             Satellite *sat = red_sats[r];
//             fprintf(output_file, "%u,%.2f,%d,%.6f,%.6f,%.6f,"
//                                 "%.6f,%.6f,%.6f,%.2f,%u,%d\n",
//                 step,
//                 engine->current_time,
//                 sat->id,
//                 sat->state.position.x,
//                 sat->state.position.y,
//                 sat->state.position.z,
//                 sat->state.velocity.x,
//                 sat->state.velocity.y,
//                 sat->state.velocity.z,
//                 sat->fuel,
//                 sat->current_formation,
//                 last_strategies[r]
//             );
//         }
        
//         // 清理临时数组
//         free(red_sats);
//         free(blue_sats);
        
//         step++;
//         double current_time = kinematics_engine_get_current_time(engine);
        
//         if (step % 100 == 0) {
//             print_progress(step, max_steps, current_time);
//         }
        
//         if (verbose && step % 1000 == 0) {
//             printf("\n");
//             printf("[第 %u 步] 仿真时间: %.2f小时\n", step, current_time / 3600.0);
//         }
//     }
    
//     // ===== 6. 关闭文件和清理 =====
//     fclose(output_file);
//     free(last_formations);
//     free(last_strategies);
    
//     double current_time = kinematics_engine_get_current_time(engine);
//     print_progress(step, max_steps, current_time);
//     printf("\n");
    
//     clock_t end_time = clock();
//     double elapsed = (double)(end_time - start_time) / CLOCKS_PER_SEC;
    
//     printf("\n━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n");
//     printf("仿真完成！\n");
//     printf("  仿真步数: %u\n", step);
//     printf("  仿真时长: %.2f小时\n", current_time / 3600.0);
//     printf("  执行耗时: %.2f秒\n", elapsed);
//     printf("  性能: %.2f 步/秒\n", step / elapsed);
//     printf("\n✓ 输出文件: red_satellites_trajectory.csv\n");
//     printf("\n");
    
//     return 0;
// }

int run_simulation(KinematicsEngine *engine, uint32_t max_steps, int verbose) {
    printf("开始仿真循环...\n");
    printf("━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n\n");
    
    // ===== 打开输出文件 =====
    FILE *output_file = fopen("red_satellites_trajectory.csv", "w");
    if (! output_file) {
        fprintf(stderr, "错误：无法打开输出文件\n");
        return -1;
    }
    
    // 写入CSV头
    fprintf(output_file, "step,time(s),sat_id,pos_x(km),pos_y(km),pos_z(km),"
                        "vel_x(km/s),vel_y(km/s),vel_z(km/s),fuel(kg),formation,strategy\n");
    
    clock_t start_time = clock();
    uint32_t step = 0;
    
    // 用于记录上一次的编队和策略
    uint8_t *last_formations = (uint8_t*)calloc(engine->satellite_count, sizeof(uint8_t));
    int *last_strategies = (int*)calloc(engine->satellite_count, sizeof(int));
    
    while (step < max_steps && kinematics_engine_should_continue(engine)) {
        // ===== 1. 获取所有卫星 =====
        int num_sats = 0;
        Satellite **all_sats = kinematics_engine_get_all_satellites(engine, &num_sats);
        
        if (! all_sats || num_sats <= 0) break;
        
        // ===== 2. 分离红蓝星 =====
        Satellite **red_sats = (Satellite**)malloc(sizeof(Satellite*) * num_sats);
        Satellite **blue_sats = (Satellite**)malloc(sizeof(Satellite*) * num_sats);
        int num_red = 0, num_blue = 0;
        
        for (int i = 0; i < num_sats; i++) {
            if (all_sats[i]->team == 0) {
                red_sats[num_red++] = all_sats[i];
            } else {
                blue_sats[num_blue++] = all_sats[i];
            }
        }
        
        // ===== 3. 每100步执行一次决策 =====
        if (step % 100 == 0 && num_red > 0 && num_blue > 0) {
            printf("\n[Step %u] 执行决策和编队分配...\n", step);
            
            // 决策树分组
            GroupResult *groups = decision_tree_group_satellites(red_sats, num_red, 3);
            
            if (groups) {
                // 微分博弈分配 - 正确的函数调用方式
                GameResult *game = differential_game_assign_strategies(
                    red_sats, num_red, blue_sats, num_blue, "GJ");
                
                if (game) {
                    // 更新编队和策略
                    for (int r = 0; r < num_red; r++) {
                        uint8_t formation = decision_tree_select_formation(
                            red_sats, num_red, groups, groups->group_ids[r]);
                        
                        red_sats[r]->current_formation = formation;
                        last_formations[r] = formation;
                        last_strategies[r] = game->strategy_assignments[r];
                    }
                    
                    printf("✓ 决策完成: %d个红星, %d组编队\n", num_red, groups->num_groups);
                    
                    differential_game_free_result(game);  // 释放游戏结果
                }
                
                decision_tree_free_result(groups);  // 释放分组结果
            }
        }
        
        // ===== 4. 仿真步进 =====
        if (kinematics_engine_step(engine) != 0) {
            fprintf(stderr, "\n错误：第 %u 步仿真失败\n", step);
            break;
        }
        
        // ===== 5.  输出所有红星数据到CSV =====
        for (int r = 0; r < num_red; r++) {
            Satellite *sat = red_sats[r];
            fprintf(output_file, "%u,%.2f,%d,%.6f,%.6f,%.6f,"
                                "%.6f,%.6f,%.6f,%.2f,%u,%d\n",
                step,
                engine->current_time,
                sat->id,
                sat->state.position.x,
                sat->state.position. y,
                sat->state. position.z,
                sat->state.velocity.x,
                sat->state.velocity.y,
                sat->state.velocity. z,
                sat->fuel,
                sat->current_formation,
                last_strategies[r]
            );
        }
        
        // 清理临时数组
        free(red_sats);
        free(blue_sats);
        
        step++;
        double current_time = kinematics_engine_get_current_time(engine);
        
        if (step % 100 == 0) {
            print_progress(step, max_steps, current_time);
        }
        
        if (verbose && step % 1000 == 0) {
            printf("\n");
            printf("[第 %u 步] 仿真时间: %.2f小时\n", step, current_time / 3600.0);
        }
    }
    
    // ===== 6. 关闭文件和清理 =====
    fclose(output_file);
    free(last_formations);
    free(last_strategies);
    
    double current_time = kinematics_engine_get_current_time(engine);
    print_progress(step, max_steps, current_time);
    printf("\n");
    
    clock_t end_time = clock();
    double elapsed = (double)(end_time - start_time) / CLOCKS_PER_SEC;
    
    printf("\n━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n");
    printf("仿真完成！\n");
    printf("  仿真步数: %u\n", step);
    printf("  仿真时长: %.2f小时\n", current_time / 3600.0);
    printf("  执行耗时: %.2f秒\n", elapsed);
    printf("  性能: %.2f 步/秒\n", step / elapsed);
    printf("\n✓ 输出文件: red_satellites_trajectory.csv\n");
    printf("\n");
    
    return 0;
}


void print_final_statistics(KinematicsEngine *engine) {
    printf("╔════════════════════════════════════════════════════════════╗\n");
    printf("║                    仿真统计结果                            ║\n");
    printf("╠════════════════════════════════════════════════════════════╣\n");
    
    printf("│ 卫星信息:\n");
    printf("│   总卫星数: %d\n", engine->satellite_count);
    printf("│\n");
    printf("│ 机动统计:\n");
    printf("│   总机动次数: %d\n", kinematics_engine_total_maneuvers(engine));
    printf("│   总燃料消耗: %.2f kg\n", kinematics_engine_total_fuel_consumed(engine));
    
    printf("│\n");
    printf("│ 编队质量:\n");
    double avg_quality = kinematics_engine_avg_formation_quality(engine);
    printf("│   平均编队质量: %.2f%%\n", avg_quality * 100.0);
    
    printf("│\n");
    printf("│ 时间信息:\n");
    printf("│   仿真总时间: %.2f小时\n", engine->current_time / 3600.0);
    printf("│   仿真总步数: %u\n", engine->step_count);
    
    printf("╚════════════════════════════════════════════════════════════╝\n");
    printf("\n");
}

void print_satellite_details(KinematicsEngine *engine) {
    printf("╔════════════════════════════════════════════════════════════╗\n");
    printf("║                    卫星详细信息                            ║\n");
    printf("╠════════════════════════════════════════════════════════════╣\n");
    
    for (int i = 0; i < engine->satellite_count; i++) {
        Satellite *sat = engine->satellites[i];
        if (!sat) continue;
        
        printf("│ 卫星ID: %d\n", sat->id);
        printf("│   类型: %s\n", sat->type == 0 ? "HF" : "LF");
        printf("│   功能: %s\n", 
               sat->function_type == 1 ? "攻击" : 
               sat->function_type == 2 ? "侦察" : "防护");
        printf("│   燃料: %.1f kg\n", sat->fuel);
        printf("│   位置: (%.1f, %.1f, %.1f) km\n",
               sat->state.position.x / 1000.0, sat->state.position.y / 1000.0, sat->state.position.z / 1000.0);
        printf("│\n");
    }
    
    printf("╚════════════════════════════════════════════════════════════╝\n");
    printf("\n");
}

void print_usage(const char *program_name) {
    printf("使用方法: %s [选项]\n", program_name);
    printf("\n选项:\n");
    printf("  -s STEPS       仿真最大步数 (默认: 10000)\n");
    printf("  -v             启用详细日志输出\n");
    printf("  -h             显示本帮助信息\n");
    printf("\n例子:\n");
    printf("  %s -s 50000 -v\n", program_name);
    printf("\n");
}

int main(int argc, char *argv[]) {
    


    print_banner();
    
    uint32_t max_steps = 10000;
    int verbose = 0;
    
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-s") == 0 && i + 1 < argc) {
            max_steps = (uint32_t)atoi(argv[++i]);
        } else if (strcmp(argv[i], "-v") == 0) {
            verbose = 1;
        } else if (strcmp(argv[i], "-h") == 0) {
            print_usage(argv[0]);
            return 0;
        } else {
            fprintf(stderr, "未知选项: %s\n", argv[i]);
            print_usage(argv[0]);
            return 1;
        }
    }
    
    printf("配置参数:\n");
    printf("  最大步数: %u\n", max_steps);
    printf("  详细输出: %s\n", verbose ? "是" : "否");
    printf("\n");
    
    KinematicsEngine *engine = NULL;
    if (initialize_simulation(&engine) != 0) {
        fprintf(stderr, "仿真初始化失败！\n");
        return 1;
    }
    
    if (run_simulation(engine, max_steps, verbose) != 0) {
        fprintf(stderr, "仿真运行失败！\n");
        kinematics_engine_destroy(engine);
        return 1;
    }
    
    print_final_statistics(engine);
    print_satellite_details(engine);
    
    printf("正在清理资源...\n");
    kinematics_engine_destroy(engine);
    printf("✓ 所有资源已释放\n\n");
    
    printf("━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n");
    printf("仿真程序结束。\n");
    printf("━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n\n");
    
    return 0;
}