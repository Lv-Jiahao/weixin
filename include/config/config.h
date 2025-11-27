#ifndef CONFIG_H
#define CONFIG_H

#include "types.h"

/* ==================== 配置文件路径 ==================== */

#define DEFAULT_CONFIG_FILE   "config/config.json"
#define OUTPUT_STATE_FILE     "output/state.json"
#define OUTPUT_MANEUVER_FILE  "output/maneuver.json"
#define OUTPUT_HISTORY_FILE   "output/trajectory.json"

/* ==================== 配置读取 ==================== */

/* 从JSON文件读取仿真配置 */
int config_load_simulation(const char *config_file, SimulationConfig *config);

/* 从JSON文件读取卫星配置 */
int config_load_satellites(
    const char *config_file,
    Satellite ***satellites_out,
    int *num_satellites_out
);

/* 从JSON文件读取策略阈值 */
int config_load_strategy_thresholds(
    const char *config_file,
    StrategyThresholds *thresholds
);

/* ==================== 配置保存 ==================== */

/* 将仿真配置保存到JSON文件 */
int config_save_simulation(
    const char *output_file,
    SimulationConfig *config
);

/* 将卫星配置保存到JSON文件 */
int config_save_satellites(
    const char *output_file,
    Satellite **satellites,
    int num_satellites
);

/* ==================== 卫星配置JSON解析 ==================== */

/* 从JSON对象解析单颗卫星 */
int config_parse_satellite_json(
    void *json_obj,  // 实际类型取决于JSON库
    Satellite *sat
);

/* 创建卫星的JSON对象 */
void* config_satellite_to_json(Satellite *sat);

/* ==================== 验证配置 ==================== */

/* 验证仿真配置的有效性 */
int config_validate_simulation(SimulationConfig *config);

/* 验证卫星配置的有效性 */
int config_validate_satellite(Satellite *sat);

/* 验证所有卫星 */
int config_validate_all_satellites(Satellite **satellites, int num_satellites);

/* ==================== 配置打印 ==================== */

/* 打印仿真配置 */
void config_print_simulation(SimulationConfig *config);

/* 打印卫星配置 */
void config_print_satellite(Satellite *sat);

/* 打印所有卫星配置 */
void config_print_all_satellites(Satellite **satellites, int num_satellites);

/* 打印策略阈值 */
void config_print_strategy_thresholds(StrategyThresholds *thresholds);

/* ==================== 配置模板 ==================== */

/* 生成默认配置JSON字符串 */
char* config_generate_default_json_template(void);

/* 创建默认仿真配置 */
SimulationConfig config_create_default_simulation(void);

/* 创建默认策略阈值 */
StrategyThresholds config_create_default_strategy_thresholds(void);

#endif /* CONFIG_H */
