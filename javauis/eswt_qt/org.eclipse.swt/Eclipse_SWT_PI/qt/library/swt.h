/*******************************************************************************
 * Copyright (c) 2009, 2010 Nokia Corporation and/or its subsidiary(-ies).
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *
 * Contributors:
 *     Nokia Corporation - initial implementation
 *******************************************************************************/

#ifndef _SWT_H_
#define _SWT_H_

namespace Java { namespace eSWT {

enum SwtError
    {
    ESwtErrorNone                =  0,
    ESwtErrorUnspecified         =  1,
    ESwtErrorNoHandles           =  2,
    ESwtErrorNoMoreCallbacks     =  3,
    ESwtErrorNullArgument        =  4,
    ESwtErrorInvalidArgument     =  5,
    ESwtErrorInvalidRange        =  6,
    ESwtErrorCannotBeZero        =  7,
    ESwtErrorCannotGetItem       =  8,
    ESwtErrorCannotGetSelection  =  9,
    ESwtErrorCannotGetItemHeight = 11,
    ESwtErrorCannotGetText       = 12,
    ESwtErrorCannotSetText       = 13,
    ESwtErrorItemNotAdded        = 14,
    ESwtErrorItemNotRemoved      = 15,
    ESwtErrorNotImplemented      = 20,
    ESwtErrorMenuNotDropDown     = 21,
    ESwtErrorThreadInvalidAccess = 22,
    ESwtErrorWidgetDisposed      = 24,
    ESwtErrorMenuItemNotCascade  = 27,
    ESwtErrorCannotSetSelection  = 28,
    ESwtErrorCannotSetMenu       = 29,
    ESwtErrorCannotSetEnabled    = 30,
    ESwtErrorCannotGetEnabled    = 31,
    ESwtErrorInvalidParent       = 32,
    ESwtErrorMenuNotBar          = 33,
    ESwtErrorCannotGetCount      = 36,
    ESwtErrorMenuNotPopUp        = 37,
    ESwtErrorUnsupportedDepth    = 38,
    ESwtErrorIO                  = 39,
    ESwtErrorInvalidImage        = 40,
    ESwtErrorUnsupportedFormat   = 42,
    ESwtErrorInvalidSubclass     = 43,
    ESwtErrorGraphicDisposed     = 44,
    ESwtErrorDeviceDisposed      = 45,
    ESwtErrorFailedExec          = 46,
    ESwtErrorFailedLoadLibrary   = 47
    };

}}

#endif // _SWT_H_
