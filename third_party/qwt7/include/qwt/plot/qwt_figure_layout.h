/******************************************************************************
 * Qwt Widget Library
 * Copyright (C) 2024   ChenZongYan <czy.t@163.com>
 *****************************************************************************/
#ifndef QWT_FIGURE_LAYOUT_H
#define QWT_FIGURE_LAYOUT_H
// stl
#include <memory>

// Qt
#include <QLayout>

// qwt
#include "qwt_global.h"

/**
 * @class QwtFigureLayout
 *
 * @brief Custom layout manager for QwtFigureWidget that handles both normalized coordinates and grid layouts
 */
class QWT_EXPORT QwtFigureLayout : public QLayout
{
    Q_OBJECT
    QWT_DECLARE_PRIVATE(QwtFigureLayout)
public:
    QwtFigureLayout();
    explicit QwtFigureLayout(QWidget* parent);
    ~QwtFigureLayout() override;

    virtual void addItem(QLayoutItem* item) override;
    virtual QLayoutItem* itemAt(int index) const override;
    virtual QLayoutItem* takeAt(int index) override;
    virtual int count() const override;
    virtual QSize sizeHint() const override;
    virtual QSize minimumSize() const override;
    virtual void setGeometry(const QRect& rect) override;

    // Add a widget with normalized coordinates
    void addAxes(QWidget* widget, const QRectF& rect);

    // Add a widget with normalized coordinates using separate parameters
    void addAxes(QWidget* widget, qreal left, qreal top, qreal width, qreal height);

    // Add a widget by grid layout
    void addGridAxes(QWidget* widget,
                     int rowCnt,
                     int colCnt,
                     int row,
                     int col,
                     int rowSpan  = 1,
                     int colSpan  = 1,
                     qreal wspace = 0.0,
                     qreal hspace = 0.0);

    // Change the normalized position of an already added widget
    void setAxesNormPos(QWidget* widget, const QRectF& rect);

    // Get the normalized rectangle for a widget
    QRectF widgetNormRect(QWidget* widget) const;

    // Calculate normalized coordinates of rect relative to parentRect
    static QRectF calcNormRect(const QRect& parentRect, const QRect& rect);
    // Calculate actual rectangle from normalized coordinates
    QRect calcActualRect(const QRect& parentRect, const QRectF& normRect);

protected:
    // Calculate the normalized rectangle for a grid cell
    QRectF calcGridRect(int rowCnt,
                        int colCnt,
                        int row,
                        int col,
                        int rowSpan  = 1,
                        int colSpan  = 1,
                        qreal wspace = 0.0,
                        qreal hspace = 0.0) const;
};

#endif  // QWT_FIGURE_LAYOUT_H
