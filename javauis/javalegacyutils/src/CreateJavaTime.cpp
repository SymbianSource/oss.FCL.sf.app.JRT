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

//----------------------------------------------------------------------------
/* Takes a TTime and returns the number of milliseconds since the Java epoch time
of 00:00 1st Jan 1970 */
EXPORT_C jlong JavaEpocTime::CreateJavaTime(TTime aEpocTime)
{
    // Create a TTime object that represents the Java Date 'epoch' time of 00:00, 1 Jan 1970
    TInt64 javaEpocTimeNum = MAKE_TINT64(JavaUpperTimeFor1970, JavaLowerTimeFor1970);
    TTime javaEpochTime(javaEpocTimeNum);
    // Find difference in microseconds between 'epoch' and EPOC date and adjust to milliseconds
    TTimeIntervalMicroSeconds microInterval = aEpocTime.MicroSecondsFrom(javaEpochTime);
    TInt64 intervalNum = microInterval.Int64();
    intervalNum /= 1000;
    jlong jInterval = *reinterpret_cast<jlong*>(&intervalNum);
    return jInterval;
}
