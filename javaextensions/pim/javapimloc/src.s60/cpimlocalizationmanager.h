/*
* Copyright (c) 2004-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  A factory for Localization Data objects
*
*/


#ifndef CPIMLOCALIZATIONMANAGER_H
#define CPIMLOCALIZATIONMANAGER_H

// INTERNAL INCLUDES
#include "pimtypes.h"
#include "mpimlocalizationmanager.h"

// EXTERNAL INCLUDES
#include <e32base.h>
#include <f32file.h>

// Forward declarations
class MPIMLocalizationData;
class RResourceFile;

// CLASS DECLARATION
/**
 * A Concrete factory class for MPIMLocalizationData objects
 */
NONSHARABLE_CLASS(CPIMLocalizationManager) : public CBase,
        public MPIMLocalizationManager
{
public: // Constructors and a destructor

    /**
     * Two-phased constructor.
     */
    static CPIMLocalizationManager* NewL();

    /**
     * destructor
     */
    virtual ~CPIMLocalizationManager();

public: // From MPIMLocalizationManager

    /**
     * Creates an instance of MPIMLocalizationData.
     * Caller takes ownership of the returned object.
     *
     * @param aType Type of the list, which of data to localize.
     *              This can be KPIMLocalizationIdContact,
     *              KPIMLocalizationIdSIM, KPIMLocalizationIdEvent or
     *              KPIMLocalizationidToDo
     *
     * @return MPIMLocalizationData object for the requested list type.
     * @par Leaving:
     * The method leaves on error. Error codes should be interpreted as
     * follows:
     * @li \c KErrArgument - \a aType is not valid
     * @li \c KErrNotFound - The resource file cannot be read
     */
    MPIMLocalizationData* GetPIMLocalizationDataL(
        TPIMLocalizationDataID aType);

    /**
     * Creates an instance of MPIMLocalizationData.
     * Caller takes ownership of the returned object.
     *
     * @param aType Type of the list, which of data to localize.
     *              This can be KPIMLocalizationIdContact,
     *              KPIMLocalizationIdSIM, KPIMLocalizationIdEvent or
     *              KPIMLocalizationidToDo
     *
     * @param aSubType List name identifier. Currently only Event lists
     *              can have list name identifiers (all other list should
     *              use zero).
     *
     * @return MPIMLocalizationData object for the requested list type.
     * @par Leaving:
     * The method leaves on error. Error codes should be interpreted as
     * follows:
     * @li \c KErrArgument - \a aType is not valid, or \a aSubType is not
     * valid for \a aType.
     * @li \c KErrNotFound - The resource file cannot be read
     */
    MPIMLocalizationData* GetPIMLocalizationDataL(
        TPIMLocalizationDataID aType,
        TPIMLocalizationDataID aSubType);



protected:

    /**
     * C++ default constructor.
     */
    CPIMLocalizationManager();

    /**
     * Symbian 2nd phase constructor
     */
    void ConstructL();



private:

    // File system server. Owned
    RFs iRFs;

};



#endif // CPIMLOCALIZATIONMANAGER_H

// End of file
