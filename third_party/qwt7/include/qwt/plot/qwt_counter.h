/******************************************************************************
 * Qwt Widget Library
 * Copyright (C) 1997   Josef Wilgen
 * Copyright (C) 2002   Uwe Rathmann
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the Qwt License, Version 1.0
 *
 * Modified by ChenZongYan in 2024 <czy.t@163.com>
 *   Summary of major modifications (see ChangeLog.md for full history):
 *   1. CMake build system & C++11 throughout.
 *   2. Core panner/ zoomer refactored:
 *        - QwtPanner → QwtCachePanner (pixmap-cache version)
 *        - New real-time QwtPlotPanner derived from QwtPicker.
 *   3. Zoomer supports multi-axis.
 *   4. Parasite-plot framework:
 *        - QwtFigure, QwtPlotParasiteLayout, QwtPlotTransparentCanvas,
 *        - QwtPlotScaleEventDispatcher, built-in pan/zoom on axis.
 *   5. New picker: QwtPlotSeriesDataPicker (works with date axis).
 *   6. Raster & color-map extensions:
 *        - QwtGridRasterData (2-D table + interpolation)
 *        - QwtLinearColorMap::stopColors(), stopPos() API rename.
 *   7. Bar-chart: expose pen/brush control.
 *   8. Amalgamated build: single QwtPlot.h / QwtPlot.cpp pair in src-amalgamate.
 *****************************************************************************/

#ifndef QWT_COUNTER_H
#define QWT_COUNTER_H

#include "qwt_global.h"
#include <qwidget.h>

/**
 * @brief The Counter Widget
 * @details A Counter consists of a label displaying a number and
 *          one or more (up to three) push buttons on each side
 *          of the label which can be used to increment or decrement
 *          the counter's value.
 *          A counter has a range from a minimum value to a maximum value
 *          and a step size. When the wrapping property is set
 *          the counter is circular.
 *          The number of steps by which a button increments or decrements the value
 *          can be specified using setIncSteps(). The number of buttons can be
 *          changed with setNumButtons().
 *          Example:
 *          @code
 *          #include <qwt_counter.h>
 *          QwtCounter *counter = new QwtCounter(parent);
 *          counter->setRange(0.0, 100.0);                  // From 0.0 to 100
 *          counter->setSingleStep( 1.0 );                  // Step size 1.0
 *          counter->setNumButtons(2);                      // Two buttons each side
 *          counter->setIncSteps(QwtCounter::Button1, 1);   // Button 1 increments 1 step
 *          counter->setIncSteps(QwtCounter::Button2, 20);  // Button 2 increments 20 steps
 *          connect(counter, SIGNAL(valueChanged(double)), myClass, SLOT(newValue(double)));
 *          @endcode
 */

class QWT_EXPORT QwtCounter : public QWidget
{
    Q_OBJECT

    Q_PROPERTY(double value READ value WRITE setValue NOTIFY valueChanged USER true)
    Q_PROPERTY(double minimum READ minimum WRITE setMinimum)
    Q_PROPERTY(double maximum READ maximum WRITE setMaximum)
    Q_PROPERTY(double singleStep READ singleStep WRITE setSingleStep)

    Q_PROPERTY(int numButtons READ numButtons WRITE setNumButtons)
    Q_PROPERTY(int stepButton1 READ stepButton1 WRITE setStepButton1)
    Q_PROPERTY(int stepButton2 READ stepButton2 WRITE setStepButton2)
    Q_PROPERTY(int stepButton3 READ stepButton3 WRITE setStepButton3)

    Q_PROPERTY(bool readOnly READ isReadOnly WRITE setReadOnly)
    Q_PROPERTY(bool wrapping READ wrapping WRITE setWrapping)

public:
    /**
     * @brief Button index
     */
    enum Button
    {
        /// Button intended for minor steps
        Button1,

        /// Button intended for medium steps
        Button2,

        /// Button intended for large steps
        Button3,

        /// Number of buttons
        ButtonCnt
    };

    /// Constructor
    explicit QwtCounter(QWidget* parent = nullptr);
    /// Destructor
    ~QwtCounter() override;

    /// Set the counter to be in valid/invalid state
    void setValid(bool);
    /// Return true if the value is valid
    bool isValid() const;

    /// Enable or disable wrapping
    void setWrapping(bool);
    /// Return true if wrapping is enabled
    bool wrapping() const;

    /// Return true if the counter is read only
    bool isReadOnly() const;
    /// Set the counter to read only mode
    void setReadOnly(bool);

    /// Set the number of buttons on each side
    void setNumButtons(int);
    /// Return the number of buttons
    int numButtons() const;

    /// Set the number of increment steps for a button
    void setIncSteps(QwtCounter::Button, int numSteps);
    /// Return the number of increment steps for a button
    int incSteps(QwtCounter::Button) const;

    /// Return a size hint
    virtual QSize sizeHint() const override;

    /// Return the single step size
    double singleStep() const;
    /// Set the single step size
    void setSingleStep(double stepSize);

    /// Set the range of the counter
    void setRange(double min, double max);

    /// Return the minimum value
    double minimum() const;
    /// Set the minimum value
    void setMinimum(double);

    /// Return the maximum value
    double maximum() const;
    /// Set the maximum value
    void setMaximum(double);

    /// Set the number of increment steps for button 1
    void setStepButton1(int nSteps);
    /// Return the number of increment steps for button 1
    int stepButton1() const;

    /// Set the number of increment steps for button 2
    void setStepButton2(int nSteps);
    /// Return the number of increment steps for button 2
    int stepButton2() const;

    /// Set the number of increment steps for button 3
    void setStepButton3(int nSteps);
    /// Return the number of increment steps for button 3
    int stepButton3() const;

    /// Return the current value
    double value() const;

public Q_SLOTS:
    void setValue(double);

Q_SIGNALS:
    /**
     * @brief Signal emitted when a button has been released
     * @param value The new value
     */
    void buttonReleased(double value);

    /**
     * @brief Signal emitted when the counter's value has changed
     * @param value The new value
     */
    void valueChanged(double value);

protected:
    virtual bool event(QEvent*) override;
    virtual void wheelEvent(QWheelEvent*) override;
    virtual void keyPressEvent(QKeyEvent*) override;

private Q_SLOTS:
    void btnReleased();
    void btnClicked();
    void textChanged();

private:
    void incrementValue(int numSteps);
    void initCounter();
    void updateButtons();
    void showNumber(double);

    QWT_DECLARE_PRIVATE(QwtCounter)
};

#endif
