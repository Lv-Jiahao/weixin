#include <quaternion.h>
#include <math.h>
#include <stdio.h>

Quaternion quat_normalize(Quaternion q) {
    double mag = sqrt(q.x*q.x + q.y*q.y + q.z*q.z + q.w*q.w);
    if (mag < 1e-15) return (Quaternion){0, 0, 0, 1};
    return (Quaternion){q.x/mag, q.y/mag, q.z/mag, q.w/mag};
}

// Quaternion quat_conjugate(Quaternion q) {
//     return (Quaternion){-q.x, -q.y, -q.z, q.w};
// }

Quaternion quat_multiply(Quaternion q1, Quaternion q2) {
    return (Quaternion){
        q1.w*q2.x + q1.x*q2.w + q1.y*q2.z - q1.z*q2.y,
        q1.w*q2.y - q1.x*q2.z + q1.y*q2.w + q1.z*q2.x,
        q1.w*q2.z + q1.x*q2.y - q1.y*q2.x + q1.z*q2.w,
        q1.w*q2.w - q1.x*q2.x - q1.y*q2.y - q1.z*q2.z
    };
}

Vector3 quat_rotate_vector(Quaternion q, Vector3 v) {
    Quaternion v_quat = (Quaternion){v.x, v.y, v.z, 0};
    Quaternion result = quat_multiply(quat_multiply(q, v_quat), quat_conjugate(q));
    return (Vector3){result.x, result.y, result.z};
}

Quaternion quat_from_axis_angle(Vector3 axis, double angle) {
    double half_angle = angle / 2.0;
    double sin_half = sin(half_angle);
    return (Quaternion){axis.x * sin_half, axis.y * sin_half, axis.z * sin_half, cos(half_angle)};
}

Quaternion quat_from_two_vectors(Vector3 v1, Vector3 v2) {
    v1 = vector3_normalize(v1);
    v2 = vector3_normalize(v2);
    
    double dot = vector3_dot(v1, v2);
    if (dot > 0.9995) return (Quaternion){0, 0, 0, 1};
    if (dot < -0.9995) {
        Vector3 axis = (fabs(v1.x) < 0.9) ? (Vector3){1, 0, 0} : (Vector3){0, 1, 0};
        axis = vector3_normalize(vector3_cross(v1, axis));
        return quat_from_axis_angle(axis, M_PI);
    }
    
    Vector3 axis = vector3_cross(v1, v2);
    return quat_normalize((Quaternion){axis.x, axis.y, axis.z, 1.0 + dot});
}

void quat_to_axis_angle(Quaternion q, Vector3 *axis, double *angle) {
    q = quat_normalize(q);
    double sin_half = sqrt(q.x*q.x + q.y*q.y + q.z*q.z);
    if (sin_half < 1e-10) {
        *axis = (Vector3){0, 0, 1};
        *angle = 0;
    } else {
        *axis = (Vector3){q.x/sin_half, q.y/sin_half, q.z/sin_half};
        *angle = 2.0 * acos(fmax(-1.0, fmin(1.0, q.w)));
        if (*angle > M_PI) *angle = 2*M_PI - *angle;
    }
}

Quaternion quat_lerp(Quaternion q1, Quaternion q2, double t) {
    t = (t < 0) ? 0 : (t > 1) ? 1 : t;
    Quaternion result = {
        q1.x + t * (q2.x - q1.x),
        q1.y + t * (q2.y - q1.y),
        q1.z + t * (q2.z - q1.z),
        q1.w + t * (q2.w - q1.w)
    };
    return quat_normalize(result);
}

Quaternion quat_slerp(Quaternion q1, Quaternion q2, double t) {
    t = (t < 0) ? 0 : (t > 1) ? 1 : t;
    Quaternion q1_norm = quat_normalize(q1);
    Quaternion q2_norm = quat_normalize(q2);
    
    double dot = q1_norm.x*q2_norm.x + q1_norm.y*q2_norm.y + q1_norm.z*q2_norm.z + q1_norm.w*q2_norm.w;
    if (dot < 0) {
        q2_norm = (Quaternion){-q2_norm.x, -q2_norm.y, -q2_norm.z, -q2_norm.w};
        dot = -dot;
    }
    
    dot = (dot > 1.0) ? 1.0 : (dot < -1.0) ? -1.0 : dot;
    double theta = acos(dot);
    double sin_theta = sin(theta);
    
    if (sin_theta < 1e-10) return quat_lerp(q1_norm, q2_norm, t);
    
    double w1 = sin((1-t) * theta) / sin_theta;
    double w2 = sin(t * theta) / sin_theta;
    
    return (Quaternion){
        w1*q1_norm.x + w2*q2_norm.x,
        w1*q1_norm.y + w2*q2_norm.y,
        w1*q1_norm.z + w2*q2_norm.z,
        w1*q1_norm.w + w2*q2_norm.w
    };
}

Quaternion quat_from_euler_zyx(double z, double y, double x) {
    double cy = cos(y * 0.5), sy = sin(y * 0.5);
    double cp = cos(x * 0.5), sp = sin(x * 0.5);
    double cr = cos(z * 0.5), sr = sin(z * 0.5);
    
    return (Quaternion){
        sr*cp*cy - cr*sp*sy,
        cr*sp*cy + sr*cp*sy,
        cr*cp*sy - sr*sp*cy,
        cr*cp*cy + sr*sp*sy
    };
}

void quat_to_euler_zyx(Quaternion q, double *z, double *y, double *x) {
    double test = q.x*q.y + q.z*q.w;
    if (test > 0.499) {
        *z = 2 * atan2(q.x, q.w);
        *y = M_PI / 2;
        *x = 0;
        return;
    }
    if (test < -0.499) {
        *z = -2 * atan2(q.x, q.w);
        *y = -M_PI / 2;
        *x = 0;
        return;
    }
    
    double sqx = q.x*q.x, sqy = q.y*q.y, sqz = q.z*q.z;
    *z = atan2(2*q.y*q.w - 2*q.x*q.z, 1 - 2*sqy - 2*sqz);
    *y = asin(2*test);
    *x = atan2(2*q.x*q.w - 2*q.y*q.z, 1 - 2*sqx - 2*sqz);
}

Quaternion quat_from_matrix(Matrix3x3 m) {
    double trace = m.m[0][0] + m.m[1][1] + m.m[2][2];
    Quaternion q;
    
    if (trace > 0) {
        double s = 0.5 / sqrt(trace + 1.0);
        q.w = 0.25 / s;
        q.x = (m.m[2][1] - m.m[1][2]) * s;
        q.y = (m.m[0][2] - m.m[2][0]) * s;
        q.z = (m.m[1][0] - m.m[0][1]) * s;
    } else if (m.m[0][0] > m.m[1][1] && m.m[0][0] > m.m[2][2]) {
        double s = 2.0 * sqrt(1.0 + m.m[0][0] - m.m[1][1] - m.m[2][2]);
        q.w = (m.m[2][1] - m.m[1][2]) / s;
        q.x = 0.25 * s;
        q.y = (m.m[0][1] + m.m[1][0]) / s;
        q.z = (m.m[0][2] + m.m[2][0]) / s;
    } else if (m.m[1][1] > m.m[2][2]) {
        double s = 2.0 * sqrt(1.0 + m.m[1][1] - m.m[0][0] - m.m[2][2]);
        q.w = (m.m[0][2] - m.m[2][0]) / s;
        q.x = (m.m[0][1] + m.m[1][0]) / s;
        q.y = 0.25 * s;
        q.z = (m.m[1][2] + m.m[2][1]) / s;
    } else {
        double s = 2.0 * sqrt(1.0 + m.m[2][2] - m.m[0][0] - m.m[1][1]);
        q.w = (m.m[1][0] - m.m[0][1]) / s;
        q.x = (m.m[0][2] + m.m[2][0]) / s;
        q.y = (m.m[1][2] + m.m[2][1]) / s;
        q.z = 0.25 * s;
    }
    return quat_normalize(q);
}

Matrix3x3 quat_to_matrix(Quaternion q) {
    q = quat_normalize(q);
    double xx = q.x*q.x, yy = q.y*q.y, zz = q.z*q.z;
    double xy = q.x*q.y, xz = q.x*q.z, yz = q.y*q.z;
    double wx = q.w*q.x, wy = q.w*q.y, wz = q.w*q.z;
    
    return (Matrix3x3){{{1-2*(yy+zz), 2*(xy-wz), 2*(xz+wy)},
                       {2*(xy+wz), 1-2*(xx+zz), 2*(yz-wx)},
                       {2*(xz-wy), 2*(yz+wx), 1-2*(xx+yy)}}};
}

Quaternion quat_integrate(Quaternion q, Vector3 omega, double dt) {
    Vector3 scaled_omega = vector3_scale(omega, 0.5 * dt);
    Quaternion omega_quat = (Quaternion){scaled_omega.x, scaled_omega.y, scaled_omega.z, 0};
    Quaternion dq = quat_multiply(omega_quat, q);
    
    return quat_normalize((Quaternion){
        q.x + dq.x,
        q.y + dq.y,
        q.z + dq.z,
        q.w + dq.w
    });
}

Quaternion quat_derivative(Quaternion q, Vector3 omega) {
    Quaternion omega_quat = (Quaternion){omega.x, omega.y, omega.z, 0};
    Quaternion dq = quat_multiply(omega_quat, q);
    return (Quaternion){dq.x*0.5, dq.y*0.5, dq.z*0.5, dq.w*0.5};
}

double quat_angular_distance(Quaternion q1, Quaternion q2) {
    q1 = quat_normalize(q1);
    q2 = quat_normalize(q2);
    double dot = q1.x*q2.x + q1.y*q2.y + q1.z*q2.z + q1.w*q2.w;
    dot = (dot > 1.0) ? 1.0 : (dot < -1.0) ? -1.0 : dot;
    return 2.0 * acos(fabs(dot));
}

int quat_is_normalized(Quaternion q) {
    double mag_sq = q.x*q.x + q.y*q.y + q.z*q.z + q.w*q.w;
    return (fabs(mag_sq - 1.0) < 1e-6);
}

void quat_print(const char *name, Quaternion q) {
    printf("%s: (%.6f, %.6f, %.6f, %.6f)\n", name, q.x, q.y, q.z, q.w);
}