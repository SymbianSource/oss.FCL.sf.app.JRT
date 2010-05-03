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

#ifndef MIDPAUTHENTICATIONMODULE_H
#define MIDPAUTHENTICATIONMODULE_H

#include <list>
#include <stdio.h>
#include <string>

#include "javaosheaders.h"
#include "javauid.h"

namespace java
{
namespace security
{

class MIDPAuthenticationModule
{
public:

    /**
     * Creates an instance of the MIDPAuthenticationModule
     *
     * @return An instance of MIDPAuthenticationModule
     */
    OS_IMPORT static MIDPAuthenticationModule* createInstance();

    /**
     * Gets application suite certificate chains.
     *
     * @param aUid application suite uid.
     * @param aChains[out] populated certificate chain list. Empty if
     *                     application is not signed or application is not
     *                     identified by given attributes.
     */
    virtual void getCertChains(const java::util::Uid& aUid,
                               std::list<std::string>& aChains) = 0;

};

} // end namespace security
} // end namespace java

#endif // MIDPAUTHENTICATIONMODULE_H

