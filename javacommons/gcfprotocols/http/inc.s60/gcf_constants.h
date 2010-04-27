/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef GCF_CONSTANTS_H
#define GCF_CONSTANTS_H

#include <e32std.h>

/*
* Value returned by javax.microedition.io SecurityInfo getProtocolVersion()
* for all secure connection types
*/
_LIT(KMIDSecureProtocolVersion, "3.1");

/*
* Value returned by javax.microedition.io SecurityInfo getProtocolName()
* for all secure connection types
*/
_LIT(KMIDSecureProtocolName, "TLS");

/*
* Secure Protocol layer used by Java Secure Sockets.
*/
_LIT(KMIDSecureSocketProtocol, "TLS1.0");

#endif // GCF_CONSTANTS_H
