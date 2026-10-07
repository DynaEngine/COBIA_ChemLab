#ifndef QWT3D_ENRICHMENT_STD_H
#define QWT3D_ENRICHMENT_STD_H

#include "qwt3d_enrichment.h"

namespace Qwt3D
{

class Plot3D;

/**
 * @brief The Cross Hair Style
 */
class QWT3D_EXPORT CrossHair : public VertexEnrichment
{
public:
    CrossHair();
    CrossHair(double rad, double linewidth, bool smooth, bool boxed);
    CrossHair(const CrossHair& other);
    ~CrossHair() override;

    Qwt3D::Enrichment* clone() const override;

    void configure(double rad, double linewidth, bool smooth, bool boxed);
    void drawBegin() override;
    void drawEnd() override;
    void draw(Qwt3D::Triple const&) override;

private:
    QWT_DECLARE_PRIVATE(CrossHair)
};

/**
 * @brief The Point Style
 */
class QWT3D_EXPORT Dot : public VertexEnrichment
{
public:
    Dot();
    Dot(double pointsize, bool smooth);
    Dot(const Dot& other);
    ~Dot() override;

    Qwt3D::Enrichment* clone() const override;

    void configure(double pointsize, bool smooth);
    void drawBegin() override;
    void drawEnd() override;
    void draw(Qwt3D::Triple const&) override;

private:
    QWT_DECLARE_PRIVATE(Dot)
};

/**
 * @brief The Cone Style
 */
class QWT3D_EXPORT Cone : public VertexEnrichment
{
public:
    Cone();
    Cone(double rad, unsigned quality);
    Cone(const Cone& other);
    ~Cone() override;

    Qwt3D::Enrichment* clone() const override;

    void configure(double rad, unsigned quality);
    void draw(Qwt3D::Triple const&) override;

private:
    QWT_DECLARE_PRIVATE(Cone)
};

/**
 * @brief 3D vector field
 * @details The class encapsulates a vector field including its OpenGL representation as arrow field.
 *          The arrows can be configured in different aspects (color, shape, painting quality).
 */
class QWT3D_EXPORT Arrow : public VertexEnrichment
{
public:
    Arrow();
    Arrow(const Arrow& other);
    ~Arrow() override;

    Qwt3D::Enrichment* clone() const override;

    void configure(int segs, double relconelength, double relconerad, double relstemrad);
    void setQuality(int val);
    void draw(Qwt3D::Triple const&) override;

    void setTop(Qwt3D::Triple t);
    void setColor(Qwt3D::RGBA rgba);

private:
    QWT_DECLARE_PRIVATE(Arrow)

    double calcRotation(Qwt3D::Triple& axis, Qwt3D::FreeVector const& vec);
};

}  // ns

#endif  // QWT3D_ENRICHMENT_STD_H
