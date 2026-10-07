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

#ifndef QWT_GLOBAL_H
#define QWT_GLOBAL_H

#include <qglobal.h>
#include <memory>
#include <utility>
#include "qwt_version_info.h"

#if defined(_MSC_VER) /* MSVC Compiler */
/* template-class specialization 'identifier' is already instantiated */
#pragma warning(disable : 4660)
/* inherits via dominance */
#pragma warning(disable : 4250)
#endif  // _MSC_VER

#ifdef QWT_DLL

#if defined(QWT_MAKEDLL)  // create a Qwt DLL library
#define QWT_EXPORT Q_DECL_EXPORT
#else  // use a Qwt DLL library
#define QWT_EXPORT Q_DECL_IMPORT
#endif

#endif  // QWT_DLL

#ifndef QWT_EXPORT
#define QWT_EXPORT
#endif

#ifndef QWT_DEBUG_DRAW
#define QWT_DEBUG_DRAW 0
#endif

#ifndef QWT_DEBUG_PRINT
#define QWT_DEBUG_PRINT 0
#endif
/**
 * @def QWT_DECLARE_PRIVATE
 * @brief Imitates Q_DECLARE_PRIVATE but uses an inner class instead of forward declaration
 *
 * For example:
 *
 * @code
 * //header
 * class A
 * {
 *  QWT_DECLARE_PRIVATE(A)
 * };
 * @endcode
 *
 * It expands to:
 *
 * @code
 * class A{
 *  class PrivateData;
 *  friend class A::PrivateData;
 *  std::unique_ptr< PrivateData > m_data;
 * }
 * @endcode
 *
 * This forward declares an inner PrivateData class, which is implemented in the cpp file
 *
 * @code
 * //cpp
 * class A::PrivateData{
 *  QWT_DECLARE_PUBLIC(A)
 *  PrivateData(A* p):m_data(p){
 *  }
 * };
 *
 * A::A():m_data(new PrivateData(this)){
 * }
 * @endcode
 */
#ifndef QWT_DECLARE_PRIVATE
#define QWT_DECLARE_PRIVATE(classname)                                                                                 \
    class PrivateData;                                                                                                 \
    friend class classname::PrivateData;                                                                               \
    std::unique_ptr< PrivateData > m_data;                                                                             \
    inline PrivateData* d_func()                                                                                       \
    {                                                                                                                  \
        return (m_data.get());                                                                                         \
    }                                                                                                                  \
    inline const PrivateData* d_func() const                                                                           \
    {                                                                                                                  \
        return (m_data.get());                                                                                         \
    }
#endif

/**
 * @def QWT_DECLARE_PUBLIC
 * @brief Imitates Q_DECLARE_PUBLIC
 *
 * Used with QWT_DECLARE_PRIVATE
 */
#ifndef QWT_DECLARE_PUBLIC
#define QWT_DECLARE_PUBLIC(classname)                                                                                  \
    friend class classname;                                                                                            \
    classname* q_ptr { nullptr };                                                                                      \
    inline classname* q_func()                                                                                         \
    {                                                                                                                  \
        return (static_cast< classname* >(q_ptr));                                                                     \
    }                                                                                                                  \
    inline const classname* q_func() const                                                                             \
    {                                                                                                                  \
        return (static_cast< const classname* >(q_ptr));                                                               \
    }
#endif

/**
 * @def  QWT_PIMPL_CONSTRUCT
 *
 * Used with QWT_DECLARE_PRIVATE to construct private data in constructor
 */
#ifndef QWT_PIMPL_CONSTRUCT
#define QWT_PIMPL_CONSTRUCT m_data(qwt_make_unique< PrivateData >(this))
#endif

/**
 * @def  QWT_PIMPL_CONSTRUCT_INIT
 *
 * Used with QWT_DECLARE_PRIVATE to construct private data in constructor (alternative form)
 */
#ifndef QWT_PIMPL_CONSTRUCT_INIT
#define QWT_PIMPL_CONSTRUCT_INIT()                                                                                     \
    do {                                                                                                               \
        m_data = qwt_make_unique< PrivateData >(this);                                                                 \
    } while (0)
#endif

/**
 * @def QWT_D
 * @brief Get private implementation pointer, similar to Q_D
 */
#ifndef QWT_D
#define QWT_D(pointerName) PrivateData* pointerName = d_func()
#endif

/**
 * @def QWT_DC
 * @brief Get const private implementation pointer, similar to Q_D
 */
#ifndef QWT_DC
#define QWT_DC(pointerName) const PrivateData* pointerName = d_func()
#endif

/**
 * @def QWT_Q
 * @brief Get public class pointer from private implementation, similar to Q_Q
 */
#ifndef QWT_Q
#define QWT_Q(classname, pointerName) classname* pointerName = q_func()
#endif

/**
 * @def QWT_QC
 * @brief Get const public class pointer from private implementation, similar to Q_Q
 */
#ifndef QWT_QC
#define QWT_QC(classname, pointerName) const classname* pointerName = q_func()
#endif

#if __cplusplus >= 201402L
// C++14 or later, use standard library make_unique
template< typename T, typename... Args >
std::unique_ptr< T > qwt_make_unique(Args&&... args)
{
    return std::make_unique< T >(std::forward< Args >(args)...);
}

template< typename T >
std::unique_ptr< T > qwt_make_unique(std::size_t size)
{
    return std::make_unique< T >(size);
}

#else
// C++11 custom implementation

// Basic version - normal object
template< typename T, typename... Args >
std::unique_ptr< T > qwt_make_unique(Args&&... args)
{
    return std::unique_ptr< T >(new T(std::forward< Args >(args)...));
}

// Array specialization version - dynamic array
template< typename T >
std::unique_ptr< T > qwt_make_unique(std::size_t size)
{
    return std::unique_ptr< T >(new typename std::remove_extent< T >::type[ size ]());
}

#endif

#if (__cplusplus >= 201703L) || (defined(_MSVC_LANG) && _MSVC_LANG >= 201703L)
#ifndef qwt_as_const
#define qwt_as_const std::as_const
#endif
#else
// C++14 and below use Qt's qwt_as_const
#ifndef qwt_as_const
#define qwt_as_const qAsConst
#endif
#endif

#endif
