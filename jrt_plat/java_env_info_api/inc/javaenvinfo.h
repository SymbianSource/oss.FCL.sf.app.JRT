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
* Description:  Java environment information utilities
*
*/

#ifndef JAVAENVINFO_H
#define JAVAENVINFO_H

//  INCLUDES
#include <e32cmn.h>

// CLASS DECLARATION

namespace Java
{
/**
 * Class provides information about Java environment version.
 *
 * @lib javaenvinfo.lib
 * @since S60 5.1
 */
NONSHARABLE_CLASS(JavaEnvInfo)
{
public:

    /**
      * Returns the Java environment version
      *
      * @since S60 5.1
      * @return TVersion instance representing the Java environment version
      */
    IMPORT_C static TVersion Version();

    /**
      * Returns the part of the user agent header describing the Java environment.
      * This string can be used by Browser or any other HTTP client when sending
      * HTTP requests to describe the supported Java environment.
      *
      * The returned string could be for example
      * "Configuration/CLDC-1.1 Profile/MIDP-2.1".
      *
      * @since S60 5.1
      * @return User Agent header string. Caller is responsible for deleting
      * this when done with it.
      */
    IMPORT_C static HBufC* GetUserAgentHeaderL();

    /**
      * Returns the Java configuration and profile information in human
      * readable format.
      *
      * The used format is <profile> ", " <configuration> - as an example
      * "MIDP 2.1, CLDC 1.1".
      *
      * @since S60 5.1
      * @return String in the format. Caller is responsible for deleting
      * this when done with it.
      */
    IMPORT_C static HBufC* GetPlatformInfoL();
private:

    static TVersion JavaEnvInfo::GetJavaVersionL();
};

}

#endif // JAVAENVINFO_H
