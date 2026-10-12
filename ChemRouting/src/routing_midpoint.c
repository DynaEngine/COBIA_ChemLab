#include "chemrouting/chemrouting.h"
#include <math.h>

/* ==========================================================================
 * 中点拐弯路由算法
 *
 * 策略：选择 A→B 之间较长的那条轴，在中点处拐弯
 *   - 若 |dx| > |dy|：先水平走到中点 X，再竖直走到 B
 *   - 若 |dx| <= |dy|：先竖直走到中点 Y，再水平走到 B
 *
 * 这种"在中间拐弯"的方式在流程图中非常常见且直观。
 * ========================================================================== */

static int copy_point(cr_point_t* out, int idx, int max_pts,
                       double x, double y)
{
    if (idx >= max_pts) return idx;
    out[idx].x = x;
    out[idx].y = y;
    return idx + 1;
}

int cr_routing_midpoint(const cr_routing_params_t* params,
                         const cr_rect_t* rect_a,
                         const cr_rect_t* rect_b,
                         const cr_point_t* pt_a,
                         const cr_point_t* pt_b,
                         cr_side_t side_a,
                         cr_side_t side_b,
                         double loc_a,
                         double loc_b,
                         cr_point_t* out_pts,
                         int max_pts)
{
    (void)params;
    (void)rect_a;
    (void)rect_b;
    (void)side_a;
    (void)side_b;
    (void)loc_a;
    (void)loc_b;

    double dx = pt_b->x - pt_a->x;
    double dy = pt_b->y - pt_a->y;

    int n = 0;
    n = copy_point(out_pts, n, max_pts, pt_a->x, pt_a->y);

    if (fabs(dx) > fabs(dy)) {
        double mid_x = (pt_a->x + pt_b->x) * 0.5;
        n = copy_point(out_pts, n, max_pts, mid_x, pt_a->y);
        n = copy_point(out_pts, n, max_pts, mid_x, pt_b->y);
    } else {
        double mid_y = (pt_a->y + pt_b->y) * 0.5;
        n = copy_point(out_pts, n, max_pts, pt_a->x, mid_y);
        n = copy_point(out_pts, n, max_pts, pt_b->x, mid_y);
    }

    n = copy_point(out_pts, n, max_pts, pt_b->x, pt_b->y);
    return n;
}