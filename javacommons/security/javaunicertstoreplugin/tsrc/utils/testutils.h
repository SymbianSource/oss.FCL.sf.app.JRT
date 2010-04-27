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
* Description:
*
*/

#ifndef TESTUTILS_H
#define TESTUTILS_H

#include <list>
#include <cctcertinfo.h>
#include <ct/rmpointerarray.h>
#include "testdata.h"

class CCertAttributeFilter;

class TestUtils
{
public :

    /**
     * Starts java captain.
     */
    static int StartJavaCaptain();

    /**
     * @return Ownership of returned object is give to the user.
     */
    static CCertAttributeFilter* GetFilterObj();

    /**
     * This verification based on fact that size of content of each certificate
     * is unique.
     */
    static void CheckResultOfList(std::list<CertMetaData>& aExpected,
                                  RMPointerArray<CCTCertInfo>& aResult);

    static void CheckContentOfCert(std::list<CertMetaData>& aExpected,
                                   HBufC8* aContent);

private :

    static bool sCaptainRunning;

};

#endif // TESTUTILS_H

