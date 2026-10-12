#include "chemrouting/chemrouting.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

static int g_failures = 0;
static int g_total    = 0;

#define CHECK(cond, msg) do {                                   \
    g_total++;                                                  \
    if (!(cond)) {                                              \
        fprintf(stderr, "  FAIL [%d] %s\n", __LINE__, (msg));   \
        g_failures++;                                           \
    }                                                           \
} while(0)

#define CHECK_EQ(a, b, msg) do {                                \
    g_total++;                                                  \
    if ((a) != (b)) {                                           \
        fprintf(stderr, "  FAIL [%d] %s: expected %d, got %d\n", \
                __LINE__, (msg), (int)(b), (int)(a));           \
        g_failures++;                                           \
    }                                                           \
} while(0)

#define CHECK_DOUBLE_EQ(a, b, tol, msg) do {                    \
    g_total++;                                                  \
    if (fabs((a) - (b)) > (tol)) {                              \
        fprintf(stderr, "  FAIL [%d] %s: expected %.4f, got %.4f\n", \
                __LINE__, (msg), (b), (a));                      \
        g_failures++;                                           \
    }                                                           \
} while(0)

/* ==========================================================================
 * 测试：上下文与参数
 * ========================================================================== */

static void test_context_create(void)
{
    printf("  test_context_create\n");

    cr_routing_ctx_t* ctx = cr_routing_create("orthogonal");
    CHECK(ctx != NULL, "create orthogonal context");
    cr_routing_destroy(ctx);

    ctx = cr_routing_create("midpoint");
    CHECK(ctx != NULL, "create midpoint context");
    cr_routing_destroy(ctx);

    cr_routing_ctx_t* bad = cr_routing_create("nonexistent");
    CHECK(bad == NULL, "create nonexistent returns NULL");
}

static void test_list_algorithms(void)
{
    printf("  test_list_algorithms\n");

    const char* names[8];
    int n = cr_routing_list_algorithms(names, 8);
    CHECK(n >= 2, "at least two algorithms");
    CHECK(strcmp(names[0], "orthogonal") == 0, "first algo is orthogonal");
    CHECK(strcmp(names[1], "midpoint") == 0, "second algo is midpoint");
}

static void test_params(void)
{
    printf("  test_params\n");

    cr_routing_ctx_t* ctx = cr_routing_create("orthogonal");
    CHECK(ctx != NULL, "create ctx");

    cr_routing_params_t p;
    cr_routing_get_params(ctx, &p);
    CHECK_DOUBLE_EQ(p.dist,   CR_DEFAULT_DIST,   1e-6, "default dist");
    CHECK_DOUBLE_EQ(p.margin, CR_DEFAULT_MARGIN, 1e-6, "default margin");

    cr_routing_params_t new_p = { 80.0, 15.0 };
    cr_routing_set_params(ctx, &new_p);
    cr_routing_get_params(ctx, &p);
    CHECK_DOUBLE_EQ(p.dist,   80.0, 1e-6, "modified dist");
    CHECK_DOUBLE_EQ(p.margin, 15.0, 1e-6, "modified margin");

    cr_routing_destroy(ctx);
}

/* ==========================================================================
 * 测试：正交路由算法
 * ========================================================================== */

static void test_orthogonal_east_west_direct(void)
{
    /* A 在左侧，B 在右侧：直接 E→W 连线
     *
     *   [ A ]            [ B ]
     *     E ------→------ W
     *
     * A-rect: (0,0,100,60), port E at (100,30), loc=0.5
     * B-rect: (300,0,100,60), port W at (300,30), loc=0.5
     * B.x > A.x → 直接连接
     */
    printf("  test_orthogonal_east_west_direct\n");

    cr_routing_ctx_t* ctx = cr_routing_create("orthogonal");
    cr_routing_params_t p = { 50.0, 10.0 };
    cr_routing_set_params(ctx, &p);

    cr_rect_t ra = { 0.0, 0.0, 100.0, 60.0 };
    cr_rect_t rb = { 300.0, 0.0, 100.0, 60.0 };
    cr_point_t pa = { 100.0, 30.0 };
    cr_point_t pb = { 300.0, 30.0 };

    cr_point_t out[16];
    int n = cr_routing_compute(ctx, &ra, &rb, &pa, &pb,
                               CR_SIDE_E, CR_SIDE_W, 0.5, 0.5,
                               out, 16);
    CHECK(n == 4, "4 waypoints");
    if (n == 4) {
        CHECK_DOUBLE_EQ(out[0].x, 100.0, 1e-6, "start x");
        CHECK_DOUBLE_EQ(out[1].x, 275.0, 1e-6, "turn1 x = pb.x - dist*lb");
        CHECK_DOUBLE_EQ(out[2].x, 275.0, 1e-6, "turn2 x same");
        CHECK_DOUBLE_EQ(out[3].x, 300.0, 1e-6, "end x");
    }

    cr_routing_destroy(ctx);
}

static void test_orthogonal_west_east_via_mirror(void)
{
    /* W→E 通过 mirror 复用 E→W
     *
     *   [ B ]            [ A ]
     *     E ←------←------ W
     */
    printf("  test_orthogonal_west_east_via_mirror\n");

    cr_routing_ctx_t* ctx = cr_routing_create("orthogonal");
    cr_routing_params_t p = { 50.0, 10.0 };
    cr_routing_set_params(ctx, &p);

    cr_rect_t ra = { 100.0, 0.0, 100.0, 60.0 };
    cr_rect_t rb = { -100.0, 0.0, 100.0, 60.0 };
    cr_point_t pa = { 100.0, 30.0 };
    cr_point_t pb = { -100.0, 30.0 };

    cr_point_t out[16];
    int n = cr_routing_compute(ctx, &ra, &rb, &pa, &pb,
                               CR_SIDE_W, CR_SIDE_E, 0.5, 0.5,
                               out, 16);
    CHECK(n == 4, "4 waypoints W->E");
    if (n == 4) {
        CHECK_DOUBLE_EQ(out[0].x, 100.0, 1e-6, "start x");
        CHECK_DOUBLE_EQ(out[3].x, -100.0, 1e-6, "end x");
    }

    cr_routing_destroy(ctx);
}

static void test_orthogonal_route_around(void)
{
    /* B 在 A 左侧且上方 → 向上绕行 (pb.x < pa.x && pb.y < pa.y)
     *
     *    ┌────────┐
     *    │   B    │  (x=200, y=0)
     *    │   W    │
     *    └───┬────┘
     *        │
     *   ┌──────┐
     *   │  A   │  (x=300, y=200)
     *   │   E──┼──┐
     *   └──────┘  │
     *        ┌────┘
     *        │ 绕行路径
     */
    printf("  test_orthogonal_route_around\n");

    cr_routing_ctx_t* ctx = cr_routing_create("orthogonal");
    cr_routing_params_t p = { 50.0, 10.0 };
    cr_routing_set_params(ctx, &p);

    /* A 在右下 (300,200)，B 在左上 (200,0)
     * pb.x(200) < pa.x(300) → 需要绕行
     * pb.y(30) < pa.y(230) → 向上绕行
     */
    cr_rect_t ra = { 300.0, 200.0, 100.0, 60.0 };
    cr_rect_t rb = { 200.0, 0.0, 100.0, 60.0 };
    cr_point_t pa = { 400.0, 230.0 };  /* A 的东侧中点 */
    cr_point_t pb = { 200.0, 30.0 };   /* B 的西侧中点 */

    cr_point_t out[16];
    int n = cr_routing_compute(ctx, &ra, &rb, &pa, &pb,
                               CR_SIDE_E, CR_SIDE_W, 0.5, 0.5,
                               out, 16);
    CHECK(n == 6, "6 waypoints for route-around");
    if (n == 6) {
        CHECK_DOUBLE_EQ(out[0].x, 400.0, 1e-6, "start x");
        CHECK_DOUBLE_EQ(out[0].y, 230.0, 1e-6, "start y");
        CHECK_DOUBLE_EQ(out[5].x, 200.0, 1e-6, "end x");
        CHECK_DOUBLE_EQ(out[5].y, 30.0,  1e-6, "end y");
        /* 中间拐点 y 应小于 ra_top (=200) - 即绕到 A 上方 */
        int has_above = 0;
        for (int i = 1; i < n - 1; i++) {
            if (out[i].y < 200.0) has_above = 1;
        }
        CHECK(has_above == 1, "route goes above rect A");
    }

    cr_routing_destroy(ctx);
}

static void test_orthogonal_east_east(void)
{
    /* E→E: 复用 link_ew with locB flipped */
    printf("  test_orthogonal_east_east\n");

    cr_routing_ctx_t* ctx = cr_routing_create("orthogonal");
    cr_routing_params_t p = { 50.0, 10.0 };
    cr_routing_set_params(ctx, &p);

    cr_rect_t ra = { 0.0, 0.0, 100.0, 60.0 };
    cr_rect_t rb = { 200.0, 100.0, 100.0, 60.0 };
    cr_point_t pa = { 100.0, 30.0 };
    cr_point_t pb = { 300.0, 130.0 };

    cr_point_t out[16];
    int n = cr_routing_compute(ctx, &ra, &rb, &pa, &pb,
                               CR_SIDE_E, CR_SIDE_E, 0.5, 0.5,
                               out, 16);
    CHECK(n >= 4, "at least 4 waypoints for E->E");

    cr_routing_destroy(ctx);
}

static void test_orthogonal_midline_fallback(void)
{
    /* N/S fallback: simple midline */
    printf("  test_orthogonal_midline_fallback\n");

    cr_routing_ctx_t* ctx = cr_routing_create("orthogonal");
    cr_routing_params_t p = { 50.0, 10.0 };
    cr_routing_set_params(ctx, &p);

    cr_rect_t ra = { 0.0, 0.0, 100.0, 60.0 };
    cr_rect_t rb = { 200.0, 0.0, 100.0, 60.0 };
    cr_point_t pa = { 50.0, 60.0 };
    cr_point_t pb = { 250.0, 0.0 };

    cr_point_t out[16];
    int n = cr_routing_compute(ctx, &ra, &rb, &pa, &pb,
                               CR_SIDE_S, CR_SIDE_N, 0.5, 0.5,
                               out, 16);
    CHECK(n == 4, "4 waypoints for midline");
    if (n == 4) {
        CHECK_DOUBLE_EQ(out[0].x, 50.0,  1e-6, "start");
        CHECK_DOUBLE_EQ(out[3].x, 250.0, 1e-6, "end");
    }

    cr_routing_destroy(ctx);
}

static void test_params_effect_on_routing(void)
{
    /* 改变 dist 参数应影响路径偏移 */
    printf("  test_params_effect_on_routing\n");

    cr_routing_ctx_t* ctx = cr_routing_create("orthogonal");

    cr_rect_t ra = { 0.0, 100.0, 100.0, 60.0 };
    cr_rect_t rb = { 200.0, 0.0, 100.0, 60.0 };
    cr_point_t pa = { 100.0, 130.0 };
    cr_point_t pb = { 200.0, 30.0 };

    cr_routing_params_t p_small = { 30.0, 10.0 };
    cr_routing_set_params(ctx, &p_small);
    cr_point_t out_small[16];
    int n_small = cr_routing_compute(ctx, &ra, &rb, &pa, &pb,
                                     CR_SIDE_E, CR_SIDE_W, 0.5, 0.5,
                                     out_small, 16);

    cr_routing_params_t p_large = { 80.0, 10.0 };
    cr_routing_set_params(ctx, &p_large);
    cr_point_t out_large[16];
    int n_large = cr_routing_compute(ctx, &ra, &rb, &pa, &pb,
                                     CR_SIDE_E, CR_SIDE_W, 0.5, 0.5,
                                     out_large, 16);

    CHECK_EQ(n_small, n_large, "same waypoint count");
    /* 大 dist 的绕行范围更大 → 上方点更靠上 */
    if (n_small == n_large && n_small == 6) {
        CHECK(out_small[2].y > out_large[2].y,
              "smaller dist → higher route-around top (less negative y)");
    }

    cr_routing_destroy(ctx);
}

/* ==========================================================================
 * 测试：中点拐弯路由算法
 * ========================================================================== */

static void test_midpoint_horizontal_dominant(void)
{
    /* |dx| > |dy| → 先水平走到中点 X，再竖直
     *
     *   A ●────────┐
     *              │
     *   B ●────────┘
     */
    printf("  test_midpoint_horizontal_dominant\n");

    cr_routing_ctx_t* ctx = cr_routing_create("midpoint");

    cr_rect_t ra = { 0.0, 0.0, 60.0, 60.0 };
    cr_rect_t rb = { 300.0, 50.0, 60.0, 60.0 };
    cr_point_t pa = { 60.0, 30.0 };
    cr_point_t pb = { 300.0, 80.0 };

    cr_point_t out[16];
    int n = cr_routing_compute(ctx, &ra, &rb, &pa, &pb,
                               CR_SIDE_E, CR_SIDE_W, 0.5, 0.5,
                               out, 16);
    CHECK(n == 4, "4 waypoints for midpoint horizontal");
    if (n == 4) {
        double midX = (60.0 + 300.0) * 0.5;
        CHECK_DOUBLE_EQ(out[0].x, 60.0, 1e-6, "start x");
        CHECK_DOUBLE_EQ(out[0].y, 30.0, 1e-6, "start y");
        CHECK_DOUBLE_EQ(out[1].x, midX, 1e-6, "turn1 x = midpoint");
        CHECK_DOUBLE_EQ(out[1].y, 30.0, 1e-6, "turn1 y stays");
        CHECK_DOUBLE_EQ(out[2].x, midX, 1e-6, "turn2 x = midpoint");
        CHECK_DOUBLE_EQ(out[2].y, 80.0, 1e-6, "turn2 y = B.y");
        CHECK_DOUBLE_EQ(out[3].x, 300.0, 1e-6, "end x");
        CHECK_DOUBLE_EQ(out[3].y, 80.0, 1e-6, "end y");
    }

    cr_routing_destroy(ctx);
}

static void test_midpoint_vertical_dominant(void)
{
    /* |dy| > |dx| → 先竖直走到中点 Y，再水平
     *
     *   A ●
     *     │
     *   B ●
     */
    printf("  test_midpoint_vertical_dominant\n");

    cr_routing_ctx_t* ctx = cr_routing_create("midpoint");

    cr_rect_t ra = { 100.0, 0.0, 60.0, 60.0 };
    cr_rect_t rb = { 80.0, 300.0, 60.0, 60.0 };
    cr_point_t pa = { 130.0, 60.0 };
    cr_point_t pb = { 110.0, 300.0 };

    cr_point_t out[16];
    int n = cr_routing_compute(ctx, &ra, &rb, &pa, &pb,
                               CR_SIDE_S, CR_SIDE_N, 0.5, 0.5,
                               out, 16);
    CHECK(n == 4, "4 waypoints for midpoint vertical");
    if (n == 4) {
        double midY = (60.0 + 300.0) * 0.5;
        CHECK_DOUBLE_EQ(out[0].x, 130.0, 1e-6, "start x stays");
        CHECK_DOUBLE_EQ(out[0].y, 60.0,  1e-6, "start y");
        CHECK_DOUBLE_EQ(out[1].x, 130.0, 1e-6, "turn1 x stays");
        CHECK_DOUBLE_EQ(out[1].y, midY,  1e-6, "turn1 y = midpoint");
        CHECK_DOUBLE_EQ(out[2].x, 110.0, 1e-6, "turn2 x = B.x");
        CHECK_DOUBLE_EQ(out[2].y, midY,  1e-6, "turn2 y = midpoint");
        CHECK_DOUBLE_EQ(out[3].x, 110.0, 1e-6, "end x");
        CHECK_DOUBLE_EQ(out[3].y, 300.0, 1e-6, "end y");
    }

    cr_routing_destroy(ctx);
}

static void test_midpoint_diagonal(void)
{
    /* 对角 → 看哪条轴更长就拐哪条 */
    printf("  test_midpoint_diagonal\n");

    cr_routing_ctx_t* ctx = cr_routing_create("midpoint");

    cr_rect_t ra = { 0.0, 0.0, 60.0, 60.0 };
    cr_rect_t rb = { 400.0, 5.0, 60.0, 60.0 };
    cr_point_t pa = { 60.0, 30.0 };
    cr_point_t pb = { 400.0, 35.0 };

    cr_point_t out[16];
    int n = cr_routing_compute(ctx, &ra, &rb, &pa, &pb,
                               CR_SIDE_E, CR_SIDE_W, 0.5, 0.5, out, 16);
    CHECK(n == 4, "4 waypoints for diagonal");
    if (n == 4) {
        CHECK_DOUBLE_EQ(out[0].x, 60.0,  1e-6, "start x");
        CHECK_DOUBLE_EQ(out[3].x, 400.0, 1e-6, "end x");
        CHECK_DOUBLE_EQ(out[3].y, 35.0,  1e-6, "end y");
    }

    cr_routing_destroy(ctx);
}

static void test_midpoint_same_point(void)
{
    /* 相同端点 → 直线 */
    printf("  test_midpoint_same_point\n");

    cr_routing_ctx_t* ctx = cr_routing_create("midpoint");

    cr_rect_t ra = { 0.0, 0.0, 60.0, 60.0 };
    cr_rect_t rb = { 0.0, 0.0, 60.0, 60.0 };
    cr_point_t pa = { 30.0, 30.0 };
    cr_point_t pb = { 30.0, 30.0 };

    cr_point_t out[16];
    int n = cr_routing_compute(ctx, &ra, &rb, &pa, &pb,
                               CR_SIDE_E, CR_SIDE_E, 0.5, 0.5, out, 16);
    CHECK(n == 4, "still produces 4 points for degenerate case");
    CHECK_DOUBLE_EQ(out[0].x, 30.0, 1e-6, "start");
    CHECK_DOUBLE_EQ(out[n-1].x, 30.0, 1e-6, "end");

    cr_routing_destroy(ctx);
}

/* ==========================================================================
 * 测试：线段分解
 * ========================================================================== */

static void test_segments(void)
{
    printf("  test_segments\n");

    cr_point_t pts[] = {
        { 0.0, 0.0 },
        { 100.0, 0.0 },   /* H */
        { 100.0, 50.0 },  /* V */
        { 200.0, 50.0 },  /* H */
        { 200.0, 100.0 }, /* V */
    };
    cr_segment_t segs[8];
    int n = cr_segments_from_polyline(pts, 5, segs, 8);
    CHECK_EQ(n, 4, "4 segments");

    CHECK_EQ(segs[0].horizontal, 1,  "seg0 horizontal");
    CHECK_EQ(segs[0].valid,      1,  "seg0 valid");
    CHECK_EQ(segs[1].horizontal, 0,  "seg1 vertical");
    CHECK_EQ(segs[1].valid,      1,  "seg1 valid");
    CHECK_EQ(segs[2].horizontal, 1,  "seg2 horizontal");
    CHECK_EQ(segs[3].horizontal, 0,  "seg3 vertical");
}

static void test_segments_invalid(void)
{
    printf("  test_segments_invalid\n");

    /* degenerate: zero-length segment */
    cr_point_t pts[] = {
        { 10.0, 20.0 },
        { 10.0, 20.0 },  /* degenerate */
        { 10.0, 40.0 },  /* V */
    };
    cr_segment_t segs[8];
    int n = cr_segments_from_polyline(pts, 3, segs, 8);
    CHECK_EQ(n, 2, "2 segments");
    CHECK_EQ(segs[0].valid, 0, "seg0 invalid (degenerate)");
    CHECK_EQ(segs[1].valid, 1, "seg1 valid");
}

/* ==========================================================================
 * 测试：交叉检测
 * ========================================================================== */

static void test_crossings(void)
{
    printf("  test_crossings\n");

    /* 水平线 y=50, x in [0, 200] */
    cr_segment_t segs_a[2];
    segs_a[0].a.x = 0.0;    segs_a[0].a.y = 50.0;
    segs_a[0].b.x = 200.0;  segs_a[0].b.y = 50.0;
    segs_a[0].horizontal = 1;
    segs_a[0].valid      = 1;

    /* 竖直线 x=100, y in [0, 100] */
    segs_a[1].a.x = 100.0;  segs_a[1].a.y = 0.0;
    segs_a[1].b.x = 100.0;  segs_a[1].b.y = 100.0;
    segs_a[1].horizontal = 0;
    segs_a[1].valid      = 1;

    /* 另一组: 竖直线 x=80, y in [30, 70] */
    cr_segment_t segs_b[1];
    segs_b[0].a.x = 80.0;   segs_b[0].a.y = 30.0;
    segs_b[0].b.x = 80.0;   segs_b[0].b.y = 70.0;
    segs_b[0].horizontal = 0;
    segs_b[0].valid      = 1;

    cr_crossing_t out[8];
    int n = cr_find_crossings(segs_a, 2, segs_b, 1, 4.0, out, 8);
    CHECK_EQ(n, 1, "1 crossing");
    if (n >= 1) {
        CHECK_DOUBLE_EQ(out[0].pt.x, 80.0, 1e-6, "crossing x=80");
        CHECK_DOUBLE_EQ(out[0].pt.y, 50.0, 1e-6, "crossing y=50");
        CHECK_EQ(out[0].on_horizontal, 1, "crossing on horizontal");
    }
}

static void test_crossings_no_cross(void)
{
    printf("  test_crossings_no_cross\n");

    /* 两条平行线不应有交叉 */
    cr_segment_t segs_a[1];
    segs_a[0].a.x = 0.0;    segs_a[0].a.y = 10.0;
    segs_a[0].b.x = 100.0;  segs_a[0].b.y = 10.0;
    segs_a[0].horizontal = 1;
    segs_a[0].valid      = 1;

    cr_segment_t segs_b[1];
    segs_b[0].a.x = 0.0;    segs_b[0].a.y = 30.0;
    segs_b[0].b.x = 100.0;  segs_b[0].b.y = 30.0;
    segs_b[0].horizontal = 1;
    segs_b[0].valid      = 1;

    cr_crossing_t out[8];
    int n = cr_find_crossings(segs_a, 1, segs_b, 1, 4.0, out, 8);
    CHECK_EQ(n, 0, "no crossing for parallel lines");
}

static void test_crossings_hop_radius(void)
{
    printf("  test_crossings_hop_radius\n");

    /* 交叉点在端点附近，hop_radius 大时应被过滤 */
    cr_segment_t segs_a[1];
    segs_a[0].a.x = 0.0;    segs_a[0].a.y = 10.0;
    segs_a[0].b.x = 20.0;   segs_a[0].b.y = 10.0;
    segs_a[0].horizontal = 1;
    segs_a[0].valid      = 1;

    cr_segment_t segs_b[1];
    segs_b[0].a.x = 10.0;   segs_b[0].a.y = 0.0;
    segs_b[0].b.x = 10.0;   segs_b[0].b.y = 20.0;
    segs_b[0].horizontal = 0;
    segs_b[0].valid      = 1;

    cr_crossing_t out[8];

    int n_small = cr_find_crossings(segs_a, 1, segs_b, 1, 2.0, out, 8);
    CHECK_EQ(n_small, 1, "crossing with small hop_radius");

    int n_large = cr_find_crossings(segs_a, 1, segs_b, 1, 12.0, out, 8);
    CHECK_EQ(n_large, 0, "no crossing with large hop_radius");
}

/* ==========================================================================
 * 主入口
 * ========================================================================== */

int main(void)
{
    setvbuf(stdout, NULL, _IONBF, 0);
    setvbuf(stderr, NULL, _IONBF, 0);

    printf("=== ChemRouting Test Suite ===\n\n");

    printf("[context & params]\n");
    test_context_create();
    test_list_algorithms();
    test_params();

    printf("\n[routing — orthogonal]\n");
    test_orthogonal_east_west_direct();
    test_orthogonal_west_east_via_mirror();
    test_orthogonal_route_around();
    test_orthogonal_east_east();
    test_orthogonal_midline_fallback();
    test_params_effect_on_routing();

    printf("\n[routing — midpoint]\n");
    test_midpoint_horizontal_dominant();
    test_midpoint_vertical_dominant();
    test_midpoint_diagonal();
    test_midpoint_same_point();

    printf("\n[segments]\n");
    test_segments();
    test_segments_invalid();

    printf("\n[crossings]\n");
    test_crossings();
    test_crossings_no_cross();
    test_crossings_hop_radius();

    printf("\n==============================\n");
    printf("Total: %d, Passed: %d, Failed: %d\n",
           g_total, g_total - g_failures, g_failures);

    return g_failures > 0 ? 1 : 0;
}