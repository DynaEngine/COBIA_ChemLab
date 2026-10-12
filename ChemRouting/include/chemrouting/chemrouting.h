#ifndef CHEMROUTING_H
#define CHEMROUTING_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>

/* ==========================================================================
 * 公共数据类型
 * ========================================================================== */

/** 2D 点 */
typedef struct cr_point {
    double x;
    double y;
} cr_point_t;

/** 矩形 (x, y, width, height)，与 QRectF 语义一致 */
typedef struct cr_rect {
    double x;
    double y;
    double w;
    double h;
} cr_rect_t;

/** 端口所在边 */
typedef enum cr_side {
    CR_SIDE_E = 0,  /**< 东 (右侧) */
    CR_SIDE_W = 1,  /**< 西 (左侧) */
    CR_SIDE_N = 2,  /**< 北 (上侧) */
    CR_SIDE_S = 3   /**< 南 (下侧) */
} cr_side_t;

/** 水平/竖直线段 */
typedef struct cr_segment {
    cr_point_t a;
    cr_point_t b;
    int horizontal;  /**< 1 = 水平，0 = 竖直，-1 = 无效 */
    int valid;        /**< 1 = 有效，0 = 无效 */
} cr_segment_t;

/** 正交交叉点 */
typedef struct cr_crossing {
    cr_point_t pt;
    int on_horizontal;  /**< 1 = 交叉发生在水平线段上 */
} cr_crossing_t;

/** 路由算法可配置参数 */
typedef struct cr_routing_params {
    double dist;       /**< 路由偏移距离，默认 50.0 */
    double margin;     /**< 边距保留量，默认 10.0 */
} cr_routing_params_t;

/** 默认参数 */
#define CR_DEFAULT_DIST   50.0
#define CR_DEFAULT_MARGIN 10.0

/* ==========================================================================
 * 矩形辅助函数 (内联)
 * ========================================================================== */

static inline double cr_rect_left(const cr_rect_t* r)   { return r->x; }
static inline double cr_rect_right(const cr_rect_t* r)  { return r->x + r->w; }
static inline double cr_rect_top(const cr_rect_t* r)    { return r->y; }
static inline double cr_rect_bottom(const cr_rect_t* r) { return r->y + r->h; }

/* ==========================================================================
 * 路由上下文 (不透明句柄)
 * ========================================================================== */

typedef struct cr_routing_ctx cr_routing_ctx_t;

/* ==========================================================================
 * 上下文生命周期
 * ========================================================================== */

/**
 * 根据算法名称创建路由上下文
 * @param algorithm  算法名称，如 "orthogonal"
 * @return 上下文句柄，失败返回 NULL
 */
cr_routing_ctx_t* cr_routing_create(const char* algorithm);

/**
 * 销毁路由上下文
 */
void cr_routing_destroy(cr_routing_ctx_t* ctx);

/* ==========================================================================
 * 参数配置
 * ========================================================================== */

/**
 * 设置路由参数
 */
void cr_routing_set_params(cr_routing_ctx_t* ctx, const cr_routing_params_t* params);

/**
 * 获取当前路由参数
 */
void cr_routing_get_params(const cr_routing_ctx_t* ctx, cr_routing_params_t* params);

/* ==========================================================================
 * 算法发现
 * ========================================================================== */

/**
 * 列出所有可用的路由算法名称
 * @param names     输出缓冲区，用于存放名称字符串指针
 * @param max_count 缓冲区最大容量
 * @return 可用算法总数 (可能大于 max_count)
 */
int cr_routing_list_algorithms(const char** names, int max_count);

/* ==========================================================================
 * 路由计算
 * ========================================================================== */

/**
 * 计算从端口 A 到端口 B 的正交路由路径
 *
 * @param ctx     路由上下文 (含算法和参数)
 * @param rect_a  源单元操作的包围矩形
 * @param rect_b  目标单元操作的包围矩形
 * @param pt_a    源端口坐标
 * @param pt_b    目标端口坐标
 * @param side_a  源端口所在边
 * @param side_b  目标端口所在边
 * @param loc_a   源端口在边上的相对位置 (0.0 ~ 1.0)
 * @param loc_b   目标端口在边上的相对位置 (0.0 ~ 1.0)
 * @param out_pts 输出拐点数组 (调用者分配)
 * @param max_pts 最大拐点数
 * @return 实际拐点数，<= 0 表示失败
 */
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
                       int max_pts);

/* ==========================================================================
 * 线段分解与交叉检测
 * ========================================================================== */

/**
 * 将多段线分解为水平和竖直线段
 * @param pts      多段线顶点数组
 * @param npts     顶点数
 * @param out_segs 输出线段数组 (调用者分配)
 * @param max_segs 最大线段数
 * @return 实际线段数 (含无效线段)
 */
int cr_segments_from_polyline(const cr_point_t* pts,
                              int npts,
                              cr_segment_t* out_segs,
                              int max_segs);

/**
 * 检测两组正交线段之间的交叉点
 * @param segs_a    第一组线段
 * @param na        第一组线段数
 * @param segs_b    第二组线段数
 * @param nb        第二组线段数
 * @param hop_radius 交叉跳弧半径 (控制离端点多远才算交叉)
 * @param out       输出交叉点数组 (调用者分配)
 * @param max_out   最大输出数
 * @return 实际交叉点数
 */
int cr_find_crossings(const cr_segment_t* segs_a,
                      int na,
                      const cr_segment_t* segs_b,
                      int nb,
                      double hop_radius,
                      cr_crossing_t* out,
                      int max_out);

#ifdef __cplusplus
}
#endif

#endif /* CHEMROUTING_H */