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
/* Takes a jlong which is the number of milliseconds from the Java epoch time
of 00:00 1st Jan 1970 and converts it into a EPOC TTime object */
EXPORT_C TTime JavaEpocTime::CreateEpocTTime(jlong aJavaTime)
{
    // Convert jlong to a TInt64
    TInt64 timeNum = *reinterpret_cast<TInt64*>(&aJavaTime);
    // Create a TTime object that represents the Java Date 'epoch' time of 00:00, 1 Jan 1970
    TInt64 javaEpocTimeNum = MAKE_TINT64(JavaUpperTimeFor1970, JavaLowerTimeFor1970);
    TTime time(javaEpocTimeNum);
    TTimeIntervalMicroSeconds delta(timeNum * 1000);
    return time + delta;
}
