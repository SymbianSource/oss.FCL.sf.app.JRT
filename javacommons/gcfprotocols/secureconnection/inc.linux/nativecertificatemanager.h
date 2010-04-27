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
* Description:  Provides functionality for adding certificate ino cert store
*
*/


#ifndef NATIVECERTIFICATEMANAGER_H
#define NATIVECERTIFICATEMANAGER_H


// CLASS DECLARATION

/**
*  Provides functionality for verifying a X509 certificate.
*/


class NativeCertificateManager
{
public:

    /*
    * validateX509Certificate is called by nativesecureconnection when the ssl handshake happens.
    * In case of symbian when an X509 certificate is received, the fields are validated.
    * For linux currently there are not apis available for validation, so this function returns valid always.
    */
    static int validateX509Certificate(X509 *aCert)
    {
        return 0;
    }

};


#endif // NATIVECERTIFICATEMANAGER_H
