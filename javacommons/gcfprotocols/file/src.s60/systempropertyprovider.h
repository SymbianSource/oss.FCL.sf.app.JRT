/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Provides System properties such as Memory Card drive name.
 *                Paths to specific system properties based on the device.
 *
 *
*/


#ifndef SYSTEMPROPERTYPROVIDER_H
#define SYSTEMPROPERTYPROVIDER_H

#include <e32cmn.h>

namespace java
{
namespace file
{

class SystemPropertyProvider
{
public:

    /**
     * Fetches the path of property specified.
     */
    static void GetPathOfProperty(TInt aProperty, TFileName& aPath,
                                  TBool aAddRoot);

    static void GetRestrictedPathsL(HBufC*& aRetBuf);

    static void GetForbiddenPaths(HBufC*& aRetBuf);

    static void GetMemoryCardPath(TFileName& aPath);

    static void GetDefaultRoot(TFileName& aPath);

    static void GetTemporaryDrive(TFileName& aPath);

    static void GetRomDrive(TFileName& aPath);
};

} // namespace file
} // namespace java

#endif // SYSTEMPROPERTYPROVIDER_H
