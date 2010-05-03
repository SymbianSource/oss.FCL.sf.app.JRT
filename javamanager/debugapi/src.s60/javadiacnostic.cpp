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
* Description:  JavaDiagnostic factory
*
*/

#include "javadiagnostic.h"
#include "diagnosticimpl.h"

#include "javaosheaders.h"
using namespace java::debug;

OS_EXPORT JavaDiagnostic* JavaDiagnostic::createInstance()
{
    JavaDiagnostic* jd = new DiagnosticImpl();
    return jd;
}
