#include <formation/formation_base.h>

int inspect_formation_init(Formation *f) {
    if (!f) return -1;
    return 0;
}

int inspect_formation_update(Formation *f, Satellite *c, Satellite *t) {
    if (!f || !c) return -1;
    return 0;
}

int inspect_formation_check_status(Formation *f, Satellite *c) {
    if (!f || !c) return -1;
    return 0;
}

void inspect_formation_destroy(Formation *f) {
    if (f) formation_destroy(f);
}