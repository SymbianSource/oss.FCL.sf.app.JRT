/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Interface for all player factories.
*
*/


#ifndef MMMAPLAYERFACTORY_H
#define MMMAPLAYERFACTORY_H

//  INCLUDES
#include <badesca.h> // CDesC16Array

//  CONSTANTS
_LIT(KMMAHttpProtocol, "http");
_LIT(KMMAHttpsProtocol, "https");
_LIT(KMMAFileProtocol, "file");
_LIT(KMMACaptureProtocol, "capture");
_LIT(KMMADeviceProtocol, "device");

//  CLASS DEFINITION
class CMMAPlayer;

/*
-----------------------------------------------------------------------------

    DESCRIPTION
    Interface for all player factories.

-----------------------------------------------------------------------------
*/
NONSHARABLE_CLASS(MMMAPlayerFactory)
{
public:

    /**
     * Allow delete through this interface.
     */
    virtual ~MMMAPlayerFactory() {};

    /**
     * Creates new player according to a content type.
     *
     * @param aContentType Player's content type.
     * @return New instance of CMMAPlayer or null if factory cannot
     *         create it
     */
    virtual CMMAPlayer* CreatePlayerL(const TDesC& aContentType) = 0;

    /**
     * Creates new player according to a locator
     *
     * @param aProtocol protocol part from the locator.
     * @param aMiddlePart middle part from the locator.
     * @param aParameters parameters part from the locator.
     * @return New instance of CMMAPlayer or null if factory cannot
     *         create it
     */
    virtual CMMAPlayer* CreatePlayerL(const TDesC& aProtocol,
                                      const TDesC& aMiddlePart,
                                      const TDesC& aParameters) = 0;

    /**
     * Creates new player according to a header data
     *
     * @param aHeaderData Header data from the content
     * @return New instance of CMMAPlayer or null if factory cannot
     *         create it
     */
    virtual CMMAPlayer* CreatePlayerL(const TDesC8& aHeaderData) = 0;

    /**
     * Gets player's supported content types.
     *
     * @param aProtocol The input protocol for the supported content types.
     * @param aMimeTypeArray Will contain supported mime types for protocol.
     */
    virtual void GetSupportedContentTypesL(const TDesC& aProtocol,
                                           CDesC16Array& aMimeTypeArray) = 0;

    /**
     * Gets player's supported protocols for the content type.
     *
     * @param aContentType The content type for the supported protocols.
     * @param aProtocolArray Will contain supported protocols for content type.
     */
    virtual void GetSupportedProtocolsL(const TDesC& aContentType,
                                        CDesC16Array& aProtocolArray) = 0;
};

#endif // MMMAPLAYERFACTORY_H
