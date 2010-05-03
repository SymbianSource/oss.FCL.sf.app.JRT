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
* Description:  This class is used for creating EMC-based players.
*
*/


#ifndef CMMAEMCPLAYERFACTORY_H
#define CMMAEMCPLAYERFACTORY_H

//  INCLUDES
// #include <mmf/common/mmfcontrollerpluginresolver.h>        Remove and replace with proper header file
#include "mmmaplayerfactory.h"

class CMMAEMCResolver;

//  CLASS DECLARATION
/**
*   This class is used for creating EMC-based players.
*
*
*/
NONSHARABLE_CLASS(CMMAEMCPlayerFactory): public CBase, public MMMAPlayerFactory
{
public: // Constructor and destructor
    CMMAEMCPlayerFactory();
    ~CMMAEMCPlayerFactory();

public: // From MMMAPlayerFactory
    CMMAPlayer* CreatePlayerL(const TDesC& aContentType);

    CMMAPlayer* CreatePlayerL(const TDesC& aProtocol,
                              const TDesC& aMiddlePart,
                              const TDesC& aParameters);

    CMMAPlayer* CreatePlayerL(const TDesC8& aHeaderData);

    void GetSupportedContentTypesL(const TDesC& aProtocol,
                                   CDesC16Array& aMimeTypeArray);

    void GetSupportedProtocolsL(const TDesC& aContentType,
                                CDesC16Array& aProtocolArray);

protected:
    /**
      * Creates new CMMAPlayer instance with given resolver.
      * @param aResolver Resolver that contains needed infos
      * for creating player.
      */
    virtual CMMAPlayer* CreatePlayerL(CMMAEMCResolver* aResolver) = 0;

    /**
     * Returns list of allowed media IDs
     */
    virtual void MediaIdsL(RArray<TUid>& aMediaIds) = 0;

    /**
     * Checks that is this protocol supported by this factory
     */
    TBool IsSupportedProtocolL(const TDesC& aProtocol);

    /**
     * Checks that is this content-type supported by this factory
     */
    TBool IsSupportedContentTypeL(const TDesC& aContentType);
private:
    /**
     * File version of creating player from content type
     */

    CMMAPlayer* CreatePlayerL(const TDesC& aContentType,
                              const TDesC* aFileName);
#ifndef RD_JAVA_OMA_DRM_V2
    /**
     * Tries to open DRM file
     */
    CMMAPlayer* TryOpenDRMFileL(const TDesC& aFileName);
#endif // RD_JAVA_OMA_DRM_V2

};

#endif // CMMAEMCPLAYERFACTORY_H
