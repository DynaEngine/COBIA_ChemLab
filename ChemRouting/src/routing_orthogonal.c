#include "chemrouting/chemrouting.h"
#include <math.h>
#include <string.h>

/* ==========================================================================
 * 正交路由算法 — 从 StreamItem::routePath / linkEW 提取
 *
 * 处理四种端口边组合:
 *   E→W, W→E, E→E, W→W  使用 link_ew + mirror
 *   其余 (含 N/S)         使用简单中线折线
 * ========================================================================== */

/* ---- 内部辅助 ---------------------------------------------------------- */

static int copy_point(cr_point_t* out, int idx, int max_pts,
                       double x, double y)
{
    if (idx >= max_pts) return idx;
    out[idx].x = x;
    out[idx].y = y;
    return idx + 1;
}

/* link_ew: 东→西路由核心
 *   若 B 在 A 右侧 → 直接水平拐折
 *   若 B 在 A 左侧 → 向上或向下绕行
 */
static int link_ew(const cr_routing_params_t* params,
                   const cr_rect_t* ra, const cr_rect_t* rb,
                   const cr_point_t* pa, const cr_point_t* pb,
                   double la, double lb,
                   cr_point_t* out, int max_pts)
{
    (void)rb;  /* 保留以与原始 linkEW 签名一致，后续算法可能会使用 */
    double D  = params->dist;
    double ra_top    = cr_rect_top(ra);
    double ra_bottom = cr_rect_bottom(ra);
    int n = 0;

    n = copy_point(out, n, max_pts, pa->x, pa->y);

    if (pb->x > pa->x) {
        n = copy_point(out, n, max_pts, pb->x - D * lb, pa->y);
        n = copy_point(out, n, max_pts, pb->x - D * lb, pb->y);
    } else {
        if (pb->y < pa->y) {
            n = copy_point(out, n, max_pts, pa->x + D * la, pa->y);
            n = copy_point(out, n, max_pts, pa->x + D * la, ra_top - D * la);
            n = copy_point(out, n, max_pts, pb->x - D * lb, ra_top - D * la);
            n = copy_point(out, n, max_pts, pb->x - D * lb, pb->y);
        } else {
            n = copy_point(out, n, max_pts, pa->x + D * la, pa->y);
            n = copy_point(out, n, max_pts, pa->x + D * la, ra_bottom + D * la);
            n = copy_point(out, n, max_pts, pb->x - D * lb, ra_bottom + D * la);
            n = copy_point(out, n, max_pts, pb->x - D * lb, pb->y);
        }
    }

    n = copy_point(out, n, max_pts, pb->x, pb->y);
    return n;
}

/* mirror_x: 将矩形关于 Y 轴镜像
 *   QRectF(-r.right(), r.y(), r.w, r.h)
 *   right() = x + w
 */
static void mirror_rect_x(const cr_rect_t* src, cr_rect_t* dst)
{
    dst->x = -(src->x + src->w);
    dst->y =  src->y;
    dst->w =  src->w;
    dst->h =  src->h;
}

/* 对输出结果做 X 镜像 */
static void mirror_result_x(cr_point_t* pts, int n)
{
    for (int i = 0; i < n; i++)
        pts[i].x = -pts[i].x;
}

/* ==========================================================================
 * 正交路由主入口
 * ========================================================================== */

int cr_routing_orthogonal(const cr_routing_params_t* params,
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
    int n;

    /* ---- E → W ---- */
    if (side_a == CR_SIDE_E && side_b == CR_SIDE_W) {
        return link_ew(params, rect_a, rect_b,
                       pt_a, pt_b, loc_a, loc_b,
                       out_pts, max_pts);
    }

    /* ---- W → E : mirror-X → link_ew → mirror-X ---- */
    if (side_a == CR_SIDE_W && side_b == CR_SIDE_E) {
        cr_rect_t ma, mb;
        cr_point_t mpa, mpb;
        mirror_rect_x(rect_a, &ma);
        mirror_rect_x(rect_b, &mb);
        mpa.x = -pt_a->x; mpa.y = pt_a->y;
        mpb.x = -pt_b->x; mpb.y = pt_b->y;
        n = link_ew(params, &ma, &mb, &mpa, &mpb,
                    loc_a, loc_b, out_pts, max_pts);
        mirror_result_x(out_pts, n);
        return n;
    }

    /* ---- E → E : 复用 link_ew，B 侧 loc 翻转 ---- */
    if (side_a == CR_SIDE_E && side_b == CR_SIDE_E) {
        return link_ew(params, rect_a, rect_b,
                       pt_a, pt_b, loc_a, 1.0 - loc_b,
                       out_pts, max_pts);
    }

    /* ---- W → W : mirror-X → link_ew(..., locA, 1-locB) → mirror-X ---- */
    if (side_a == CR_SIDE_W && side_b == CR_SIDE_W) {
        cr_rect_t ma, mb;
        cr_point_t mpa, mpb;
        mirror_rect_x(rect_a, &ma);
        mirror_rect_x(rect_b, &mb);
        mpa.x = -pt_a->x; mpa.y = pt_a->y;
        mpb.x = -pt_b->x; mpb.y = pt_b->y;
        n = link_ew(params, &ma, &mb, &mpa, &mpb,
                    loc_a, 1.0 - loc_b, out_pts, max_pts);
        mirror_result_x(out_pts, n);
        return n;
    }

    /* ---- 其它 (N/S 等) : 简单中线折线 ---- */
    {
        double mid_x = (pt_a->x + pt_b->x) * 0.5;
        n = 0;
        n = copy_point(out_pts, n, max_pts, pt_a->x,       pt_a->y);
        n = copy_point(out_pts, n, max_pts, mid_x,         pt_a->y);
        n = copy_point(out_pts, n, max_pts, mid_x,         pt_b->y);
        n = copy_point(out_pts, n, max_pts, pt_b->x,       pt_b->y);
        return n;
    }
}