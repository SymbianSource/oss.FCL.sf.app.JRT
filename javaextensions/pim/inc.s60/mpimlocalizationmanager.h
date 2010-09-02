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
* Description:  Access interface to different list localization data sets.
*
*/

#ifndef MPIMLOCALIZATIONMANAGER_H
#define MPIMLOCALIZATIONMANAGER_H

// INCLUDES
#include "pimtypes.h"
#include <badesca.h>

// FORWARD DECLARATIONS
class MPIMLocalizationData;
class MPIMConfirmationDialogue;

/**
 * PIM Item and category operations
 */
enum TPIMOperationType
{
    EPIMOperationItemDeletion = 1,
    EPIMOperationItemCommit,
    EPIMOperationCategoryDeletion,
    EPIMOperationContactListRead,
    EPIMOperationContactListWrite,
    EPIMOperationEventListRead,
    EPIMOperationEventListWrite,
    EPIMOperationToDoListRead,
    EPIMOperationToDoListWrite
};

// CLASS DECLARATION
/**
 * A Factory class for MPIMLocalizationData objects
 */
class MPIMLocalizationManager
{
public: // destructor
    /**
     * Destructor is public virtual in order to allow deletion through
     * M-class
     */
    virtual ~MPIMLocalizationManager() { }

public:
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
    virtual MPIMLocalizationData* GetPIMLocalizationDataL(
        TPIMLocalizationDataID aType) = 0;

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
    virtual MPIMLocalizationData* GetPIMLocalizationDataL(
        TPIMLocalizationDataID aType,
        TPIMLocalizationDataID aSubType) = 0;



};



#endif // MPIMLOCALIZATIONMANAGER_H

// End of file
