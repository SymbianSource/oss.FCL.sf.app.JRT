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
* Description:  An access interface to an adapter module.
 *
*/


#ifndef MPIMADAPTERACCESS_H
#define MPIMADAPTERACCESS_H

//  INCLUDES
#include <badesca.h>
#include "pimcommon.h"

// FORWARD DECLARATIONS
class MPIMContactAdapterManager;
class MPIMContactListAdapter;
class MPIMEventAdapterManager;
class MPIMEventListAdapter;
class MPIMToDoAdapterManager;
class MPIMToDoListAdapter;
class MPIMLocalizationData;
class CCalSession;

// CLASS DECLARATION

/**
 *  Provides the elementary access the PIM lists provided by a single adapter
 *  module.
 */
class MPIMAdapterAccess
{
public:
    // Destructor

    /**
     * Destructor.
     */
    virtual ~MPIMAdapterAccess()
    {
    }

public:
    // New functions

    /**
     * Gets list names of given list type from this Adapter.
     *
     * @param aListType One of EPIMContactList, EPIMEventList
     *        and EPIMToDoList.
     *
     * @return Array of list names of given type. A zero-length
     *         array is returned if the list type is not supported
     *         by this adapter or the list type is invalid.
     *         Caller takes the ownership of the returned object.
     */
    virtual const CDesCArray& ListNamesL(const TPIMListType aListType) = 0;

    /**
     * Opens a list managed by this adapter and provides instances of
     * Adapter Manager, List Adapter and associated Localization Data.
     * The objects are returned through arguments. If no matching list
     * exists, the arguments are left untouch.
     *
     * If the adapter does not support multiple simultaneous list instances
     * and an instance already exists, the method leaves.
     *
     * @param aListName List name. If NULL, first list if opened (if any).
     * @param aRetAdapterManager The returned Adapter Manager.
     *        Caller takes the ownership of the returned object.
     * @param aRetListAdapter The returned List Adapter.
     *        Caller takes the ownership of the returned object.
     * @param aRetLocalizationData The returned Localization Data.
     *
     * @return ETrue, if a Contact list with given name exists and
     *         was successfully opened, EFalse otherwise.
     *         Caller takes the ownership of the returned object.
     *
     * @par Leaving:
     * The method leaves on error. Error codes should be interpreted as
     * follows:
     * @li \c KErrAlreadyExists - An instance of the list adapter has already
     *        been opened and multiple simultaneous instances are not
     *        supported.
     * @li \c KErrNotFound - The native database does not exist any more.
     * @li Other - The system is non-functional.
     */
    virtual TBool OpenContactListL(const TDesC* aListName,
                                   MPIMContactAdapterManager** aRetAdapterManager,
                                   MPIMContactListAdapter** aRetListAdapter,
                                   MPIMLocalizationData** aRetLocalizationData) = 0;

    /**
     * Opens a list managed by this adapter and provides instances of
     * Adapter Manager, List Adapter and associated Localization Data.
     * The objects are returned through arguments. If no matching list
     * exists, the arguments are left untouch.
     *
     * If the adapter does not support multiple simultaneous list instances
     * and an instance already exists, the method leaves.
     *
     * @param aListName List name. If NULL, first list if opened (if any).
     * @param aRetAdapterManager The returned Adapter Manager.
     *        Caller takes the ownership of the returned object.
     * @param aRetListAdapter The returned List Adapter.
     *        Caller takes the ownership of the returned object.
     * @param aRetLocalizationData The returned Localization Data.
     *
     * @return ETrue, if a Event list with given name exists and
     *         was successfully opened, EFalse otherwise.
     *         Caller takes the ownership of the returned object.
     *
     * @par Leaving:
     * The method leaves on error. Error codes should be interpreted as
     * follows:
     * @li \c KErrAlreadyExists - An instance of the list adapter has already
     *        been opened and multiple simultaneous instances are not
     *        supported.
     * @li \c KErrNotFound - The native database does not exist any more.
     * @li Other - The system is non-functional.
     */
    virtual TBool OpenEventListL(CCalSession* aCalSession,
                                 const TDesC* aListName,
                                 //const TDesC* aCalNameArg,
                                 MPIMEventAdapterManager** aRetAdapterManager,
                                 MPIMEventListAdapter** aRetListAdapter,
                                 MPIMLocalizationData** aRetLocalizationData) = 0;

    /**
     * Opens a list managed by this adapter and provides instances of
     * Adapter Manager, List Adapter and associated Localization Data.
     * The objects are returned through arguments. If no matching list
     * exists, the arguments are left untouch.
     *
     * If the adapter does not support multiple simultaneous list instances
     * and an instance already exists, the method leaves.
     *
     * @param aListName List name. If NULL, first list if opened (if any).
     * @param aRetAdapterManager The returned Adapter Manager.
     *        Caller takes the ownership of the returned object.
     * @param aRetListAdapter The returned List Adapter.
     *        Caller takes the ownership of the returned object.
     * @param aRetLocalizationData The returned Localization Data.
     *
     * @return ETrue, if a ToDo list with given name exists and
     *         was successfully opened, EFalse otherwise.
     *         Caller takes the ownership of the returned object.
     *
     * @par Leaving:
     * The method leaves on error. Error codes should be interpreted as
     * follows:
     * @li \c KErrAlreadyExists - An instance of the list adapter has already
     *        been opened and multiple simultaneous instances are not
     *        supported.
     * @li \c KErrNotFound - The native database does not exist any more.
     * @li Other - The system is non-functional.
     */
    virtual TBool OpenToDoListL(CCalSession* aCalSession,
                                const TDesC* aListName,
                                //const TDesC* aCalNameArg,
                                MPIMToDoAdapterManager** aRetAdapterManager,
                                MPIMToDoListAdapter** aRetListAdapter,
                                MPIMLocalizationData** aRetLocalizationData) = 0;

protected:
    // Protected constructor

    // Default constructor is protected to enable derivation
    MPIMAdapterAccess()
    {
    }

private:
    // Prohibited operations

    // Prohibit copy constructor..
    MPIMAdapterAccess(const MPIMAdapterAccess&)
    {
    }

    // Prohibit assigment operator.
    MPIMAdapterAccess& operator=(const MPIMAdapterAccess&)
    {
        return *this;
    }

};

#endif // MPIMADAPTERACCESS_H
// End of File
