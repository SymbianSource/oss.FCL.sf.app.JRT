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
* Description:  PIM manager singleton native side, a starting point to PIM API.
 *
*/


#ifndef CPIMMANAGER_H
#define CPIMMANAGER_H

//  INCLUDES
#include <e32base.h>
#include <e32std.h>
#include <badesca.h>
#include "pimcommon.h"
#include "mpimcontactadaptermanager.h"
#include "mpimeventadaptermanager.h"
#include "mpimtodoadaptermanager.h"
#include "functionserver.h"
#include "pimbasemanager.h"

// FORWARD DECLARATIONS
class MPIMLocalizationManager;
class CPIMVersit;
class CPIMList;
class pimbaselist;
class CPIMContactList;
class CPIMEventList;
class CPIMToDoList;
class CPIMItem;
class CPIMContactValidator;
class CPIMEventValidator;
class CPIMToDoValidator;
class MPIMAdapterAccess;
class CCalSession;

// CLASS DECLARATION

/**
 *  PIM manager class. Provides access to PIM functionalities.
 */
NONSHARABLE_CLASS(CPIMManager): public CBase,
        public pimbasemanager,
        public java::util::FunctionServer
{
public: // Constructors and destructor

    /**
     * Creates a new PIMManager and initializes it.
     * Initialization includes:
     * @li Initializing Validators
     * @li Initializing Adapter Managers from Adapter Access objects
     * @li Initializing Localization Manager
     * @li Initializing proper Localizations for each list
     * @li Initializing Versit
     */

    /**
     * Two-phased constructor.
     * This constructor should be used in unit tests only.
     */
    //static pimbasemanager* CPIMManager::getInstance( ) ;

    static CPIMManager* NewL();

    /**
     * Destructor.
     */
    virtual ~CPIMManager();
	void DeleteSessions();
public: // New functions

    /**
     * LocalizationManager
     * Returns an interface for PIM Localization manager
     * Note that caller does not take the ownership of the instance
     *
     * @return PIM Localization Manager
     */
    MPIMLocalizationManager* localizationManager() const;

    /**
     * Appends a new Adapter Access to the end of the list of
     * Adapter Accesses.
     *
     * The order of Adapter Access objects and the lists provided by
     * them matters; default list of each type is the first list
     * of that type encountered when searched sequentially through
     * the Adapter Access objects and their lists.
     *
     * @param aAdapterAccess The new Adapter Access.
     *        Owhership of the argument is taken.
     *
     * @par Leaving:
     * @li Any - fatal error.
     */
    void AddAdapterAccessL(MPIMAdapterAccess* aAdapterAccess);

    /**
     * Opens a PIM list of given type.
     *
     * @param aPimListType List type.
     * @param aPimListName Name of the list. The name must be valid list
     *        name. If not present, name is resolved to the default list
     *        of the given type.
     *
     * @return PIM list. Type of the list corresponds to
     *         \a aPimListType argument.
     *
     * @par Leaving:
     * The method leaves on error. Error codes should be interpreted as
     * follows:
     * @li \c KErrArgument - \a aPimListType is invalid.
     * @li \c KErrNotSupported - \a aPimListType is not supported.
     * @li \c KErrNotFound - No list was found by \a aPimListName.
     * @li \c KErrAlreadyExists - The list was already opened and multiple
     *     instances of the list are not supported.
     * @li Other - Internal error.
     */
    pimbaselist* openPimList(
        const TPIMListType& aPimListType,
        jstring aPimListName,
        JNIEnv* aJniEnv);

    /**
     * Lists PIM lists of given type.
     * The first entry in the list is the default list of the given type.
     *
     * @param aPimListType List type.
     *
     * @return Array of list names.
     *         Caller takes the ownership of the returned object.
     *
     * @par Leaving:
     * @li \c KErrArgument - \a aPimListType is not valid PIM list type.
     */

    jobjectArray listPimLists(
        const TPIMListType& aPimListType,
        jintArray aError,
        JNIEnv* aJniEnv);

    /**
     * Provides a Contact validator.
     */
    const CPIMContactValidator& ContactValidator();

    /**
     * Provides an Event validator.
     */
    const CPIMEventValidator& EventValidator();

    /**
     * Provides a ToDo validator.
     */
    const CPIMToDoValidator& ToDoValidator();

protected: // New functions

    /**
     * Opens a contact list.
     *
     * @param aListName Name of the list or NULL to indicate
     *        default list.
     *
     * @return A new list or NULL if no matching list was found.
     */
    CPIMContactList* DoOpenContactListL(
        const TDesC* aListName);

    /**
     * Opens an event list.
     *
     * @param aListName Name of the list or NULL to indicate
     *        default list.
     *
     * @return A new list or NULL if no matching list was found.
     */
    CPIMEventList* DoOpenEventListL(
        const TDesC* aListName);

    /**
     * Opens a to-do list.
     *
     * @param aListName Name of the list or NULL to indicate
     *        default list.
     *
     * @return A new list or NULL if no matching list was found.
     */
    CPIMToDoList* DoOpenToDoListL(
        const TDesC* aListName);

    CDesCArray* DoListPimListsL(
        const TPIMListType& aPimListType);

    pimbaselist* DoOpenPimListL(
        const TPIMListType& aPimListType,
        const TDesC* aPimListName);
	void CreateCalSessionL();
    void dispose();

private: // Constructors

    /**
     * C++ default constructor.
     */
    CPIMManager();

    /**
     * Symbian 2nd phase constructor.
     */
    void ConstructL();

private: // Data

    /** Owned. */
    MPIMLocalizationManager* iLocalizationManager;

    /**
     * List of Adapter Access objects. Elements are owned.
     */
    RPointerArray< MPIMAdapterAccess> iAdapterAccesses;

    /** Owned. */
    CPIMContactValidator* iContactValidator;

    /** Owned. */
    CPIMEventValidator* iEventValidator;

    /** Owned. */
    CPIMToDoValidator* iToDoValidator;

    RLibrary iLocalizationLibrary;
	
    
    /** Session to calendar server. Owned. */
    CCalSession* iCalSession;
    
    

};

#endif // CPIMMANAGER_H
// End of File
