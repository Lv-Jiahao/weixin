#ifndef DIFFERENTIAL_GAME_H
#define DIFFERENTIAL_GAME_H

#include <types.h>
#include <satellite.h>

/* 卫星分类结果 */
typedef struct {
    int *attack_satellites;
    int *recon_satellites;
    int *defense_satellites;
    int num_attack;
    int num_recon;
    int num_defense;
} ClassifiedSatellites;

/* 威胁评估 */
typedef struct {
    int *target_ids;
    double *threat_scores;
    int num_targets;
} ThreatAssessment;

/* 策略分配 */
typedef struct {
    StrategyType *strategies;
    int *target_assignments;
    int num_satellites;
} StrategyAssignment;

/* 分类函数 */
ClassifiedSatellites* classified_satellites_create(int num_satellites);
void classified_satellites_destroy(ClassifiedSatellites *cs);
int classify_satellites(Satellite **satellites, int num_satellites, ClassifiedSatellites *classified);

/* 威胁评估函数 */
ThreatAssessment* threat_assessment_create(int num_targets);
void threat_assessment_destroy(ThreatAssessment *ta);
int assess_threats(Satellite **red_satellites, int num_red, Satellite **blue_satellites, int num_blue, ThreatAssessment *assessment);

/* 策略分配函数 */
StrategyAssignment* strategy_assignment_create(int num_red);
void strategy_assignment_destroy(StrategyAssignment *sa);

int assign_attack_strategy(Satellite *red_sat, Satellite *blue_target, StrategyAssignment *assignment);
int assign_recon_strategy(Satellite *red_sat, Satellite *blue_target, StrategyAssignment *assignment);
int assign_defense_strategy(Satellite *red_sat, Satellite *blue_threat, StrategyAssignment *assignment);

/* 距离矩阵函数 */
double** calculate_distance_matrix(Satellite **red_satellites, int num_red, Satellite **blue_satellites, int num_blue);
void free_distance_matrix(double **matrix, int num_red);

/* 目标选择函数 */
int select_target_for_satellite(Satellite *red_sat, ThreatAssessment *threats, int *selected_target);
int select_global_host(Satellite **blue_satellites, int num_blue, ThreatAssessment *threats, int *host_id);
int allocate_fuel_resources(Satellite **red_satellites, int num_red, int num_missions);
int execute_cooperative_strategy(Satellite **red_satellites, int num_red, StrategyAssignment *assignment);
int evaluate_game_outcome(StrategyAssignment *red_strategy, StrategyAssignment *blue_strategy, double *red_payoff, double *blue_payoff);

int differential_game_get_role_weight(SatelliteFunctionType function_type);
int differential_game_calculate_payoff(Satellite *red_satellite, Satellite *blue_satellite, int red_strategy, int blue_strategy);
int differential_game_assign_strategies(Satellite **red_satellites, int num_red, Satellite **blue_satellites, int num_blue, StrategyAssignment *assignment);

#endif