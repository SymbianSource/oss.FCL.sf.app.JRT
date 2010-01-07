/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Java Debug API stub implementation.
*
*/


#include <e32base.h>
#include <javadebugapi.h>
#include <javadiagnostic.h>

EXPORT_C
TBool java::debug::installApp(HBufC& /*aFilename*/,
                              TUid& /*aSuiteUid*/,
                              RArray<TUid>& /*aApplicationUid*/)
{
    return EFalse;
}

EXPORT_C
TBool java::debug::uninstallApp(TUid /*aSuiteUid*/)
{
    return EFalse;
}

EXPORT_C
TBool java::debug::startApp(TUid /*aApplicationUid*/, HBufC& /*aUeiParameters*/)
{
    return EFalse;
}

EXPORT_C
TBool java::debug::stopApp(TUid /*aApplicationUid*/)
{
    return EFalse;
}

EXPORT_C
java::debug::JavaDiagnostic* java::debug::JavaDiagnostic::createInstance()
{
    return 0;
}
