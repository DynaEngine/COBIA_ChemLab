#include "chemrouting/chemrouting.h"
#include <stdlib.h>
#include <string.h>

/* ---- 算法函数签名 ---- */
typedef int (*cr_routing_func_t)(const cr_routing_params_t*,
                                  const cr_rect_t*, const cr_rect_t*,
                                  const cr_point_t*, const cr_point_t*,
                                  cr_side_t, cr_side_t,
                                  double, double,
                                  cr_point_t*, int);

/* ---- 算法注册项 ---- */
typedef struct cr_routing_algo {
    const char*       name;
    const char*       description;
    cr_routing_func_t func;
} cr_routing_algo_t;

/* ---- 前向声明各算法入口 ---- */
extern int cr_routing_orthogonal(const cr_routing_params_t*,
                                  const cr_rect_t*, const cr_rect_t*,
                                  const cr_point_t*, const cr_point_t*,
                                  cr_side_t, cr_side_t,
                                  double, double,
                                  cr_point_t*, int);

extern int cr_routing_midpoint(const cr_routing_params_t*,
                                const cr_rect_t*, const cr_rect_t*,
                                const cr_point_t*, const cr_point_t*,
                                cr_side_t, cr_side_t,
                                double, double,
                                cr_point_t*, int);

/* ---- 算法注册表 ---- */
static const cr_routing_algo_t g_algos[] = {
    { "orthogonal", "Manhattan orthogonal routing with E/W priority", cr_routing_orthogonal },
    { "midpoint",   "Midpoint turn — turn at the center between two ports", cr_routing_midpoint },
};
static const int g_algo_count = (int)(sizeof(g_algos) / sizeof(g_algos[0]));

/* ---- 上下文结构 ---- */
struct cr_routing_ctx {
    cr_routing_algo_t algo;
    cr_routing_params_t params;
};

/* ==========================================================================
 * 公共 API
 * ========================================================================== */

cr_routing_ctx_t* cr_routing_create(const char* algorithm)
{
    const cr_routing_algo_t* found = NULL;
    for (int i = 0; i < g_algo_count; i++) {
        if (strcmp(g_algos[i].name, algorithm) == 0) {
            found = &g_algos[i];
            break;
        }
    }
    if (!found) return NULL;

    cr_routing_ctx_t* ctx = (cr_routing_ctx_t*)malloc(sizeof(cr_routing_ctx_t));
    if (!ctx) return NULL;

    ctx->algo = *found;
    ctx->params.dist   = CR_DEFAULT_DIST;
    ctx->params.margin = CR_DEFAULT_MARGIN;
    return ctx;
}

void cr_routing_destroy(cr_routing_ctx_t* ctx)
{
    free(ctx);
}

void cr_routing_set_params(cr_routing_ctx_t* ctx, const cr_routing_params_t* params)
{
    if (ctx && params)
        ctx->params = *params;
}

void cr_routing_get_params(const cr_routing_ctx_t* ctx, cr_routing_params_t* params)
{
    if (ctx && params)
        *params = ctx->params;
}

int cr_routing_list_algorithms(const char** names, int max_count)
{
    int n = 0;
    for (int i = 0; i < g_algo_count && n < max_count; i++, n++)
        names[n] = g_algos[i].name;
    return g_algo_count;
}

int cr_routing_compute(const cr_routing_ctx_t* ctx,
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
    if (!ctx || !ctx->algo.func || !out_pts || max_pts <= 0)
        return 0;

    return ctx->algo.func(&ctx->params,
                          rect_a, rect_b,
                          pt_a, pt_b,
                          side_a, side_b,
                          loc_a, loc_b,
                          out_pts, max_pts);
}