/*******************************************************************************
 * Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *
 * Contributors:
 *     Nokia Corporation - initial API and implementation
 *******************************************************************************/
#ifndef GFXLOG_H_
#define GFXLOG_H_

#include "swtlog.h"

using namespace Java::eSWT;

// Logging of a JNI native method call, JNI functions have this
//#define GFX_LOG_JNI_CALL() SwtScopeLog __swtScopeLogger(Q_FUNC_INFO, GfxLogJNIEnter, GfxLogJNIExit)
#define GFX_LOG_JNI_CALL()

// Logging of a function call other than the above
//#define GFX_LOG_FUNC_CALL() SwtScopeLog __swtScopeLogger(Q_FUNC_INFO, GfxLogFuncEnter, GfxLogFuncExit)
#define GFX_LOG_FUNC_CALL()

// Logging of a scope other than function call
//#define GFX_LOG_SCOPE_CALL() SwtScopeLog __swtScopeLogger(Q_FUNC_INFO, GfxLogScopeEnter, GfxLogScopeExit)
#define GFX_LOG_SCOPE_CALL()


#endif /*GFXLOG_H_*/
