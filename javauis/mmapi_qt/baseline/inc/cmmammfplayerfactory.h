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
* Description:  This class is used for creating MMF-based players.
*
*/


#ifndef CMMAMMFPLAYERFACTORY_H
#define CMMAMMFPLAYERFACTORY_H

//  INCLUDES
#include <mmf/common/mmfcontrollerpluginresolver.h>
#include "mmmaplayerfactory.h"

class CMMAMMFResolver;

//  CLASS DECLARATION
/**
*   This class is used for creating MMF-based players.
*
*
*/
class CMMAMMFPlayerFactory: public CBase, public MMMAPlayerFactory
{
public: // Constructor and destructor
    IMPORT_C CMMAMMFPlayerFactory();
    IMPORT_C ~CMMAMMFPlayerFactory();

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
     * @param aResolver Resolver that contains needed mmf controller infos
     * for creating player.
     */
    virtual CMMAPlayer* CreatePlayerL(CMMAMMFResolver* aResolver) = 0;

    /**
     * Returns list of allowed media IDs
     */
    virtual void MediaIdsL(RArray<TUid>& aMediaIds) = 0;

    /**
     * Returns Match type for media IDs.
     * CMMFPluginSelectionParameters::EAllowOnlySuppliedMediaIds is the default.
     */
    virtual CMMFPluginSelectionParameters::TMediaIdMatchType
    MediaIdMatchType();

    /**
     * This method is called before mmf controller implementation array
     * is created. With this the actual factories can e.g. select either
     * set required play or record format support. Play format support is
     * the default.
     */
    IMPORT_C virtual void PreparePluginSelectionParametersL(
        CMMAMMFResolver* aResolver,
        CMMFFormatSelectionParameters* aFormatSelection);

    /**
     * Creates CMMAPlayer with given format selection parameters
     */
    IMPORT_C virtual CMMAPlayer* CreatePlayerL(CMMFFormatSelectionParameters* aFormatSelect,
            const TDesC* aFileName = NULL);

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

#endif // CMMAMMFPLAYERFACTORY_H
