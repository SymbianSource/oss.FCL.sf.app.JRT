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
* Description:  Midlet plugin information interface
*
*/


#ifndef APPMNGR2MIDLETINFO_H
#define APPMNGR2MIDLETINFO_H

/**
 * Midlet runtime interface for common used midlet details
 *
 * @lib appmngr2midletplugin.dll
 * @since S60 v9.1
 */
class MAppMngr2MidletInfo
{
public:
    /**
     * Path to jar file
     *
     * @since S60 9.1
     * @return Path to jar file
     */
    virtual const TDesC& JarFileName() const = 0;

    /**
     * Path to jad file
     *
     * @since S60 9.1
     * @return Path to jad file
     */
    virtual const TDesC& JadFileName() const = 0;

    /**
     * Installed application MIME type
     *
     * @since S60 9.1
     * @return Application MIME type
     */
    virtual const TDesC8& MimeType() const = 0;
};

#endif // APPMNGR2MIDLETINFO_H
