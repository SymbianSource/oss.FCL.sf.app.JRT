/*
* Copyright (c) 1999-2000 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:
*
*/


#include "jutils.h"

RJArray::RJArray(JNIEnv& aJni)
        : iJni(aJni)
{
}



RJArray::~RJArray()
{
}



void* RJArray::GetPrimitiveArrayCriticalLC(jarray aJavaArray, TBool aMutable)
{
    iJavaArray = aJavaArray;
    iArrayPtr = REINTERPRET_CAST(TUint8*, iJni.GetPrimitiveArrayCritical(aJavaArray, NULL));
    iMutable = aMutable;
    User::LeaveIfNull(iArrayPtr);
    CleanupStack::PushL(TCleanupItem(&CleanupArrayAccess, this));
    return iArrayPtr;
}



void RJArray::CleanupArrayAccess(TAny* aRJArray)
{
    RJArray* array = STATIC_CAST(RJArray*, aRJArray);
    array->ReleasePrimitiveArrayCritical();
}



void RJArray::ReleasePrimitiveArrayCritical()
{
    iJni.ReleasePrimitiveArrayCritical(iJavaArray, iArrayPtr, (iMutable ? 0 : JNI_ABORT));
}
