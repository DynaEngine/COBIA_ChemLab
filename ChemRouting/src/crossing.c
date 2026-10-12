#include "chemrouting/chemrouting.h"
#include <math.h>

#define EPS 1e-3

/* ==========================================================================
 * 线段分解 — 对应 StreamItem::segments()
 * ========================================================================== */

int cr_segments_from_polyline(const cr_point_t* pts,
                              int npts,
                              cr_segment_t* out_segs,
                              int max_segs)
{
    int n = 0;
    for (int i = 0; i < npts - 1 && n < max_segs; i++) {
        cr_segment_t* s = &out_segs[n];
        s->a = pts[i];
        s->b = pts[i + 1];

        double dx = fabs(s->b.x - s->a.x);
        double dy = fabs(s->b.y - s->a.y);

        if (dx < EPS && dy < EPS) {
            s->valid      = 0;
            s->horizontal = -1;
        } else if (dy < EPS) {
            s->valid      = 1;
            s->horizontal = 1;
        } else if (dx < EPS) {
            s->valid      = 1;
            s->horizontal = 0;
        } else {
            s->valid      = 0;
            s->horizontal = -1;
        }
        n++;
    }
    return n;
}

/* ==========================================================================
 * 交叉检测 — 对应 StreamItem::findCrossings()
 *
 * 只检测 水平×竖直 的正交交叉。
 * hop_radius 控制交叉点必须离线段端点至少多远。
 * ========================================================================== */

int cr_find_crossings(const cr_segment_t* segs_a,
                      int na,
                      const cr_segment_t* segs_b,
                      int nb,
                      double hop_radius,
                      cr_crossing_t* out,
                      int max_out)
{
    int count = 0;

    for (int ia = 0; ia < na && count < max_out; ia++) {
        const cr_segment_t* sa = &segs_a[ia];
        if (!sa->valid) continue;

        for (int ib = 0; ib < nb && count < max_out; ib++) {
            const cr_segment_t* sb = &segs_b[ib];
            if (!sb->valid) continue;
            if (sa->horizontal == sb->horizontal) continue;

            if (sa->horizontal) {
                double y  = sa->a.y;
                double x  = sb->a.x;
                double sx_min = fmin(sa->a.x, sa->b.x);
                double sx_max = fmax(sa->a.x, sa->b.x);
                double sy_min = fmin(sb->a.y, sb->b.y);
                double sy_max = fmax(sb->a.y, sb->b.y);

                if (x > sx_min + hop_radius - 0.5 &&
                    x < sx_max - hop_radius + 0.5 &&
                    y > sy_min + hop_radius - 0.5 &&
                    y < sy_max - hop_radius + 0.5)
                {
                    out[count].pt.x          = x;
                    out[count].pt.y          = y;
                    out[count].on_horizontal = 1;
                    count++;
                }
            } else {
                double x  = sa->a.x;
                double y  = sb->a.y;
                double sy_min = fmin(sa->a.y, sa->b.y);
                double sy_max = fmax(sa->a.y, sa->b.y);
                double sx_min = fmin(sb->a.x, sb->b.x);
                double sx_max = fmax(sb->a.x, sb->b.x);

                if (y > sy_min + hop_radius - 0.5 &&
                    y < sy_max - hop_radius + 0.5 &&
                    x > sx_min + hop_radius - 0.5 &&
                    x < sx_max - hop_radius + 0.5)
                {
                    out[count].pt.x          = x;
                    out[count].pt.y          = y;
                    out[count].on_horizontal = 0;
                    count++;
                }
            }
        }
    }

    return count;
}