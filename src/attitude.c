#include <attitude.h>
#include <stdlib.h>
#include <math.h>

AttitudeTracker* attitude_tracker_create(double max_rate, double max_accel, Vector3 body_axis) {
    AttitudeTracker *tracker = (AttitudeTracker*)malloc(sizeof(AttitudeTracker));
    if (!tracker) return NULL;
    
    tracker->q = (Quaternion){0, 0, 0, 1};
    tracker->omega = (Vector3){0, 0, 0};
    tracker->alpha = (Vector3){0, 0, 0};
    tracker->max_rate = max_rate;
    tracker->max_accel = max_accel;
    tracker->body_axis = vector3_normalize_safe(body_axis);
    tracker->target_id = -1;
    tracker->step_count = 0;
    
    return tracker;
}

void attitude_tracker_destroy(AttitudeTracker *tracker) {
    if (tracker) free(tracker);
}

int attitude_tracker_init(AttitudeTracker *tracker, double max_rate, double max_accel, Vector3 body_axis) {
    if (!tracker) return -1;
    tracker->q = (Quaternion){0, 0, 0, 1};
    tracker->omega = (Vector3){0, 0, 0};
    tracker->alpha = (Vector3){0, 0, 0};
    tracker->max_rate = max_rate;
    tracker->max_accel = max_accel;
    tracker->body_axis = vector3_normalize_safe(body_axis);
    tracker->step_count = 0;
    return 0;
}

void attitude_tracker_reset(AttitudeTracker *tracker) {
    if (!tracker) return;
    tracker->q = (Quaternion){0, 0, 0, 1};
    tracker->omega = (Vector3){0, 0, 0};
    tracker->alpha = (Vector3){0, 0, 0};
    tracker->step_count = 0;
}

int attitude_tracker_step_towards(AttitudeTracker *tracker, Vector3 target_direction, Vector3 omega_desired, double dt) {
    if (!tracker || dt <= 0) return -1;
    
    target_direction = vector3_normalize_safe(target_direction);
    
    Vector3 omega_error = vector3_sub(omega_desired, tracker->omega);
    Vector3 alpha_cmd = vector3_scale(omega_error, 0.1);
    
    if (vector3_magnitude(alpha_cmd) > tracker->max_accel) {
        alpha_cmd = vector3_scale(vector3_normalize_safe(alpha_cmd), tracker->max_accel);
    }
    
    tracker->alpha = alpha_cmd;
    
    Vector3 omega_new = vector3_add(tracker->omega, vector3_scale(tracker->alpha, dt));
    if (vector3_magnitude(omega_new) > tracker->max_rate) {
        omega_new = vector3_scale(vector3_normalize_safe(omega_new), tracker->max_rate);
    }
    tracker->omega = omega_new;
    
    tracker->step_count++;
    return 0;
}

int attitude_euler_to_quaternion(double roll, double pitch, double yaw, Quaternion *q) {
    if (!q) return -1;
    *q = quat_from_euler_zyx(yaw, pitch, roll);
    return 0;
}

int attitude_set_pointing_nadir(AttitudeTracker *tracker, Vector3 position) {
    if (!tracker) return -1;
    Vector3 nadir = vector3_scale(position, -1.0);
    tracker->body_axis = vector3_normalize_safe(nadir);
    return 0;
}

int attitude_set_pointing_anti_nadir(AttitudeTracker *tracker, Vector3 position) {
    if (!tracker) return -1;
    tracker->body_axis = vector3_normalize_safe(position);
    return 0;
}

int attitude_set_pointing_sun(AttitudeTracker *tracker, Vector3 sun_direction) {
    if (!tracker) return -1;
    tracker->body_axis = vector3_normalize_safe(sun_direction);
    return 0;
}

int attitude_set_pointing_custom(AttitudeTracker *tracker, Vector3 direction) {
    if (!tracker) return -1;
    tracker->body_axis = vector3_normalize_safe(direction);
    return 0;
}

Vector3 attitude_saturate_omega(Vector3 omega, double max_rate) {
    double mag = vector3_magnitude(omega);
    if (mag > max_rate) {
        return vector3_scale(omega, max_rate / mag);
    }
    return omega;
}

Vector3 attitude_saturate_alpha(Vector3 alpha, double max_accel) {
    double mag = vector3_magnitude(alpha);
    if (mag > max_accel) {
        return vector3_scale(alpha, max_accel / mag);
    }
    return alpha;
}

int attitude_print(AttitudeTracker *tracker) {
    if (!tracker) return -1;
    printf("AttitudeTracker:\n");
    printf("  Quaternion: (%.6f, %.6f, %.6f, %.6f)\n", tracker->q.x, tracker->q.y, tracker->q.z, tracker->q.w);
    printf("  Angular Velocity: (%.6f, %.6f, %.6f)\n", tracker->omega.x, tracker->omega.y, tracker->omega.z);
    printf("  Step Count: %u\n", tracker->step_count);
    return 0;
}

void attitude_tracker_set_quaternion(AttitudeTracker *tracker, Quaternion q) {
    if (!tracker) return;
    tracker->q = quat_normalize(q);
}

Vector3 attitude_tracker_get_body_direction(AttitudeTracker *tracker) {
    if (!tracker) return (Vector3){0, 0, 0};
    return tracker->body_axis;
}

void attitude_tracker_get_euler_angles_xyz(AttitudeTracker *tracker, double *roll, double *pitch, double *yaw) {
    if (!tracker || !roll || !pitch || !yaw) return;
    *roll = 0;
    *pitch = 0;
    *yaw = 0;
}

Matrix3x3 attitude_tracker_get_rotation_matrix(AttitudeTracker *tracker) {
    if (!tracker) return matrix3x3_identity();
    return matrix3x3_identity();
}

double attitude_time_to_reach(Quaternion current, Quaternion target, double max_rate) {
    return 1.0;
}

Quaternion attitude_quat_from_axis_angle(Vector3 axis, double angle) {
    return quat_from_axis_angle(axis, angle);
}

void attitude_axis_angle_from_quat(Quaternion q, Vector3 *axis, double *angle) {
    if (!axis || !angle) return;
    quat_to_axis_angle(q, axis, angle);
}

int attitude_tracker_set_pointing_direction(AttitudeTracker *tracker, Vector3 direction) {
    if (!tracker) return -1;
    tracker->body_axis = vector3_normalize_safe(direction);
    return 0;
}

Vector3 attitude_pd_control(Vector3 error_axis, double error_angle, double kp, double kd) {
    return vector3_scale(error_axis, kp * error_angle);
}