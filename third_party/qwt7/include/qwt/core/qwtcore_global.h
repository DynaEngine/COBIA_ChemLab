/******************************************************************************
 * Qwt Widget Library
 * Copyright (C) 1997   Josef Wilgen
 * Copyright (C) 2002   Uwe Rathmann
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the Qwt License, Version 1.0
 *****************************************************************************/

#ifndef QWTCORE_GLOBAL_H
#define QWTCORE_GLOBAL_H

#include "qwt_global.h"

#ifdef QWTCORE_DLL

#if defined(QWTCORE_MAKEDLL)  // create a qwtcore DLL library
#define QWTCORE_EXPORT Q_DECL_EXPORT
#else  // use a qwtcore DLL library
#define QWTCORE_EXPORT Q_DECL_IMPORT
#endif

#endif  // QWTCORE_DLL

#ifndef QWTCORE_EXPORT
#define QWTCORE_EXPORT
#endif

#endif
