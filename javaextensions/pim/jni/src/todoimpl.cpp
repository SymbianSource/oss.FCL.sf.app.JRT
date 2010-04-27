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
* Description:  ToDoImpl JNI wrapper.
 *
*/


//  INCLUDE FILES

#include "com_nokia_mj_impl_pim_ToDoImpl.h"
#include "pimbasemanager.h"
#include "pimbaseitem.h"
#include "pimcommon.h"
#include "pimutils.h"
#include "logger.h"

JNIEXPORT jint
JNICALL Java_com_nokia_mj_impl_pim_ToDoImpl__1createNativePeer(
    JNIEnv* aJniEnv,
    jobject /*aPeer*/,
    jint aManagerHandle,
    jintArray aError)
{
    JELOG2(EPim);
    pimbasemanager* pimManager = reinterpret_cast< pimbasemanager *>(aManagerHandle);
    pimbaseitem* todoItem = NULL;
    int error = 0;
    try
    {
        todoItem = pimbaseitem::getToDoItemInstance(pimManager);
    }
    catch (int aError)
    {
        error = aError;
    }
    SetJavaErrorCode(aJniEnv, aError, error);
    if (error != 0)
        return 0;
    return reinterpret_cast<int>(todoItem);

}

// End of File
