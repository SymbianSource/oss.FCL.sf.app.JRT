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
* Description:  This class provides container for message.
*
*/


#ifndef JAVARUNTIMEPRIVATECRKEYS_H
#define JAVARUNTIMEPRIVATECRKEYS_H

#include <e32base.h>

/**
 * Java Runtime environment Central Repository UID
 */
const TUid    KCRUidJavaRuntime             = { 0x10282DFD };

/**
 * Indicates classpath for MIDP runtimes. This key value is generated
 * from the values defined by extension libraries.
 */
const TUint32 KJavaRuntimeMIDPClasspath     = 0x00000001;


/**
 * Indicates classpath for CDC main runtimes. This key value is generated
 * from the values defined by extension libraries.
 */
const TUint32 KJavaRuntimeCDCMainClasspath  = 0x00000002;

#endif // JAVARUNTIMEPRIVATECRKEYS_H
