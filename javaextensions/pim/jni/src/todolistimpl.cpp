/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:  ToDoListImpl JNI wrapper.
 *
*/


//  INCLUDE FILES

#include "com_nokia_mj_impl_pim_ToDoListImpl.h"
#include "pimcommon.h"
#include "pimbaseitem.h"
#include "pimbaselist.h"
#include "pimutils.h"
#include "logger.h"

JNIEXPORT jintArray
JNICALL Java_com_nokia_mj_impl_pim_ToDoListImpl__1itemsByDate(
    JNIEnv* aJniEnv,
    jobject /*aPeer*/,
    jint aToDoListHandle,
    jint aField,
    jlong aStartDate,
    jlong aEndDate,
    jintArray aError)
{
    JELOG2(EPim);
    pimbaselist* toDoList = reinterpret_cast< pimbaselist *>(aToDoListHandle);

    jintArray itemHandles = toDoList->itemsByDate(
                                aField, aStartDate, aEndDate, aJniEnv, aError);

    return itemHandles;
}

//  End of File
