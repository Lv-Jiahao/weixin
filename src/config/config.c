#ifndef CONFIG_H
#define CONFIG_H

#include <stdbool.h>

// 团队属性结构体
typedef struct {
    int attack;
    int recon;
    int defense;
} TeamConfig;

// 模拟配置结构体
typedef struct {
    int max_time;
    int time_step;
} SimulationConfig;

// 主配置结构体
typedef struct {
    TeamConfig red;
    TeamConfig blue;
    char strategy[50];
    SimulationConfig simulation;
} GameConfig;

// 函数声明
bool config_init(const char* config_path);
void config_cleanup(void);
const GameConfig* config_get(void);

// 便捷访问函数
const TeamConfig* config_get_red(void);
const TeamConfig* config_get_blue(void);
const char* config_get_strategy(void);
const SimulationConfig* config_get_simulation(void);

// 调试函数
void config_print(void);

#endif // CONFIG_H