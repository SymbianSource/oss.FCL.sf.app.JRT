/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Utility class
*
*/


#ifndef PIMLOCALIZATIONUTILS_H
#define PIMLOCALIZATIONUTILS_H

// EXTERNAL INCLUDES
#include <e32std.h>

// FORWARD DECLARATIONS
class RResourceFile;
class RFs;

/**
 * PIM static utility class
 * @since S60 3.2
 */
NONSHARABLE_CLASS(PIMLocalizationUtils)
{
public: // New static methods

    /**
     * OpenResourceFileLC
     * Opens a specified resource file and confirms the signature
     *
     * @param aRfs Opened file server session
     * @param aResourceFile Reference to the resource opened resource
     * @param aFileName Name of the resource file. The name doesn't countain
     *        drive nor path, only plain resource file name
     */
    static void OpenResourceFileLC(
        RFs& aRfs,
        RResourceFile& aResourceFile,
        const TDesC& aFileName);

};

#endif // PIMLOCALIZATIONUTILS_H

// End of file
