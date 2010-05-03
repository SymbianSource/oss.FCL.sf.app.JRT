/*
* Copyright (c) 1999-2001 Nokia Corporation and/or its subsidiary(-ies).
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


#include <mevents.h>

EXPORT_C void CJavaEventSourceBase::Panic(TInt aVal)
//
// Runtime check for oversized parameter lists in the Execute...() API
//
{
    _LIT(KJsClientPanic,"JES-interface");
    User::Panic(KJsClientPanic,aVal);
}
