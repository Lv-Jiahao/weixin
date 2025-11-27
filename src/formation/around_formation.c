#include <formation/formation_base.h>

int around_formation_init(Formation *f) {
    if (!f) return -1;
    return 0;
}

int around_formation_update(Formation *f, Satellite *c, Satellite *t) {
    if (!f || !c) return -1;
    return 0;
}

int around_formation_check_status(Formation *f, Satellite *c) {
    if (!f || !c) return -1;
    return 0;
}

void around_formation_destroy(Formation *f) {
    if (f) formation_destroy(f);
}