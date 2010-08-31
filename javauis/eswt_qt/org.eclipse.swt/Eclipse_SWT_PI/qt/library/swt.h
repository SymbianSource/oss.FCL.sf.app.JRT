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

#include "swterror.h"

// Pointer-handle conversion macros

#define POINTER_TO_HANDLE(pointer) reinterpret_cast<jint>(static_cast<QObject*>(pointer))
#define HANDLE_TO_POINTER(type, variable, handle) type variable = qobject_cast<type>(static_cast<type>(reinterpret_cast<QObject*>( handle )))

#define QCOLOR_TO_HANDLE(pointer) reinterpret_cast<jint>(static_cast<QColor*>(pointer))
#define HANDLE_TO_QCOLOR(variable, handle) QColor* variable = static_cast<QColor*>(reinterpret_cast<QColor*>( handle ))

#define QTABLEWIDGETITEM_TO_HANDLE(pointer) reinterpret_cast<jint>(static_cast<QTableWidgetItem*>(pointer))
#define HANDLE_TO_QTABLEWIDGETITEM(variable, handle) QTableWidgetItem* variable = static_cast<QTableWidgetItem*>(reinterpret_cast<QTableWidgetItem*>( handle ))
#define QTREEWIDGETITEM_TO_HANDLE(pointer) reinterpret_cast<jint>(static_cast<QTreeWidgetItem*>(pointer))
#define HANDLE_TO_QTREEWIDGETITEM(variable, handle) QTreeWidgetItem* variable = static_cast<QTreeWidgetItem*>(reinterpret_cast<QTreeWidgetItem*>( handle ))

// Try-catch macros for to be used in the JNI functions

#define SWT_TRY try
#define SWT_CATCH \
catch(std::bad_alloc const&)\
    {\
    swtApp->jniUtils().Throw( aJniEnv, ESwtErrorNoHandles );\
    }\
catch(std::exception const&)\
    {\
    swtApp->jniUtils().Throw( aJniEnv, ESwtErrorUnspecified );\
    }
#define SWT_CATCH_1(err) \
catch(std::exception const&)\
    {\
    swtApp->jniUtils().Throw( aJniEnv, err );\
    }

#endif // _SWT_H_
