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


// INCLUDE FILES
#include "cpimmanager.h"
#include <e32std.h>
#include <badesca.h>
#include <calsession.h>
#include "pimcommon.h"
#include "mpimcontactadaptermanager.h"
#include "mpimeventadaptermanager.h"
#include "mpimtodoadaptermanager.h"
#include "mpimcontactlistadapter.h"
#include "mpimeventlistadapter.h"
#include "mpimtodolistadapter.h"
#include "cpimversit.h"
#include "mpimlocalizationmanager.h"
#include "mpimlocalizationdata.h"
#include "cpimcontactlist.h"
#include "cpimeventlist.h"
#include "cpimtodolist.h"
#include "cpimcontactvalidator.h"
#include "cpimeventvalidator.h"
#include "cpimtodovalidator.h"
#include "mpimadapteraccess.h"
#include "cpimcmadapteraccess.h"
#include "cpimeventadapteraccess.h"
#include "cpimtodoadapteraccess.h"
#include "pimpanics.h"
#include "pimjnitools.h"
#include "pimutils.h"
#include "jstringutils.h"
#include "fs_methodcall.h"
#include "logger.h"
#include "cpimlocalizationmanager.h"

#include "s60commonutils.h"



// CONSTANTS

/**
 * Array granularity for arrays of managers, cached list names and
 * localization objects. Number of such objects in an array is quite
 * low, at the time of writing 1 to 4.
 */
const TInt KManagerArrayGranularity = 2;


CPIMManager::CPIMManager() :
        java::util::FunctionServer("CPIMManager"), iAdapterAccesses()
{
    JELOG2(EPim);
}

void CPIMManager::ConstructL()
{
    JELOG2(EPim);
    iContactValidator = CPIMContactValidator::NewL();
    iEventValidator = CPIMEventValidator::NewL();
    iToDoValidator = CPIMToDoValidator::NewL();

    iLocalizationManager
    = (MPIMLocalizationManager*)(CPIMLocalizationManager::NewL());

    createServerToNewThread();
        CallMethodL(this, &CPIMManager::CreateCalSessionL, this);
    }

void CPIMManager::CreateCalSessionL()
    {
    		iCalSession = CCalSession::NewL();    	   		
    		TRAPD(err, iCalSession->OpenL(iCalSession->DefaultFileNameL()));
        if ( KErrNotFound == err)
        {
          iCalSession->CreateCalFileL(iCalSession->DefaultFileNameL());
          iCalSession->OpenL(iCalSession->DefaultFileNameL());
        }
        else
        {
          User::LeaveIfError(err);
        }      			    			
    }

pimbasemanager* pimbasemanager::getInstance()
{
    JELOG2(EPim);
    // Create CPIMManager

    CPIMManager* self = NULL;
    TInt error = KErrNone;
    TRAP(error, self = CPIMManager::NewL());
    if (error != KErrNone)
        throw error;
    return self;
}

CPIMManager* CPIMManager::NewL()
{
    JELOG2(EPim);
    CPIMManager* self = new(ELeave) CPIMManager;

    CleanupStack::PushL(self);
    self->ConstructL();

    MPIMLocalizationManager* localizationManager = self->iLocalizationManager;

    // Create Adapter Access objects

    MPIMAdapterAccess* contactAdapterAccess = CPIMCMAdapterAccess::NewL(
                localizationManager, self);
    CleanupDeletePushL(contactAdapterAccess);
    self->AddAdapterAccessL(contactAdapterAccess);
    CleanupStack::Pop(contactAdapterAccess);

    MPIMAdapterAccess* eventAdapterAccess = CPIMEventAdapterAccess::NewL(
                                                localizationManager, self);
    CleanupDeletePushL(eventAdapterAccess);
    self->AddAdapterAccessL(eventAdapterAccess);
    CleanupStack::Pop(eventAdapterAccess);

    MPIMAdapterAccess* toDoAdapterAccess = CPIMToDoAdapterAccess::NewL(
                                               localizationManager, self);
    CleanupDeletePushL(toDoAdapterAccess);
    self->AddAdapterAccessL(toDoAdapterAccess);
    CleanupStack::Pop(toDoAdapterAccess);

    CleanupStack::Pop(self);

    return self;
}

CPIMManager::~CPIMManager()
{
    JELOG2(EPim);
    delete iLocalizationManager;
    iLocalizationManager = NULL;
    iAdapterAccesses.ResetAndDestroy();
    delete iContactValidator;
    iContactValidator = NULL;
    delete iEventValidator;
    iEventValidator = NULL;
    delete iToDoValidator;    
    CallMethod(this, &CPIMManager::DeleteSessions, this);   

    iToDoValidator = NULL;
    stopServer();
}
void CPIMManager::DeleteSessions()
	{
    delete iCalSession;
    iCalSession = NULL;
	}

void CPIMManager::dispose()
{
    JELOG2(EPim);
    delete iContactValidator;
    iContactValidator = NULL;
    delete iEventValidator;
    iEventValidator = NULL;
    delete iToDoValidator;
    iToDoValidator = NULL;
}

MPIMLocalizationManager* CPIMManager::localizationManager() const
{
    JELOG2(EPim);
    return iLocalizationManager;
}

void CPIMManager::AddAdapterAccessL(MPIMAdapterAccess* aAdapterAccess)
{
    JELOG2(EPim);
    User::LeaveIfError(iAdapterAccesses.Append(aAdapterAccess));
}

pimbaselist* CPIMManager::openPimList(const TPIMListType& aPimListType,
                                      jstring aPimListName, JNIEnv* aJniEnv)
{
    JELOG2(EPim);
    const JStringUtils listName(*aJniEnv, aPimListName);
    const TDesC* listNameArg = (aPimListName ? &listName : NULL);
    pimbaselist* list = NULL;
    TInt error = KErrNone;
    TRAP(error, list = DoOpenPimListL(aPimListType, listNameArg));
    if (error != KErrNone)
        throw error;
    return list;
}

pimbaselist* CPIMManager::DoOpenPimListL(const TPIMListType& aPimListType,
        const TDesC* aPimListName)
{
    JELOG2(EPim);
    pimbaselist* list = NULL;

    if (aPimListType == EPIMContactList)
    {
        list = DoOpenContactListL(aPimListName);
    }
    else if (aPimListType == EPIMEventList)
    {
        list = DoOpenEventListL(aPimListName);
    }
    else if (aPimListType == EPIMToDoList)
    {
        list = DoOpenToDoListL(aPimListName);
    }
    else
    {
        // invalid list type
        User::Leave(KErrArgument);
    }

    // Check results

    if (!list)
    {
        if (aPimListName)
        {
            // list name was given but no such list exists
            User::Leave(KErrNotFound);
        }
        else
        {
            // default list was not found
            User::Leave(KErrNotSupported);
        }
    }

    return list;
}

CDesCArray* CPIMManager::DoListPimListsL(const TPIMListType& aPimListType)
{
    JELOG2(EPim);
    CDesCArray* retVal = new(ELeave) CDesCArrayFlat(KManagerArrayGranularity);
    CleanupStack::PushL(retVal);

    // Loop through the Adapter Accesses

    const TInt numAccesses = iAdapterAccesses.Count();
    for (TInt accessIndex = 0; accessIndex < numAccesses; accessIndex++)
    {
        // Loop through the lists provided by an Adapter Access

        const CDesCArray& lists = iAdapterAccesses[accessIndex]->ListNamesL(
                                      aPimListType);

        const TInt numLists = lists.Count();
        for (TInt listIndex = 0; listIndex < numLists; listIndex++)
        {
            retVal->AppendL(lists[listIndex]);
        }
    }

    // Done

    CleanupStack::Pop(retVal);
    return retVal;

}

jobjectArray CPIMManager::listPimLists(const TPIMListType& aPimListType,
                                       jintArray aError, JNIEnv* aJniEnv)
{
    JELOG2(EPim);
    jobjectArray javaStringArray = NULL;
    CDesCArray* lists = NULL;
    TInt error = KErrNone;
    TRAP(error, lists = DoListPimListsL(aPimListType));
    if (error == KErrNone)
    {
        javaStringArray = CreateJavaStringArray(aJniEnv, *lists, EFalse); // (do not handle KPIMNullArrayElement elements specially)

        if (!javaStringArray)
        {
            SetJavaErrorCode(aJniEnv, aError, KErrNoMemory);
        }

        delete lists;
    }
    else
    {
        // list array was not created
        SetJavaErrorCode(aJniEnv, aError, error);
    }
    return javaStringArray;
}

const CPIMContactValidator& CPIMManager::ContactValidator()
{
    JELOG2(EPim);
    return *iContactValidator;
}

const CPIMEventValidator& CPIMManager::EventValidator()
{
    JELOG2(EPim);
    return *iEventValidator;
}

const CPIMToDoValidator& CPIMManager::ToDoValidator()
{
    JELOG2(EPim);
    return *iToDoValidator;
}

CPIMContactList* CPIMManager::DoOpenContactListL(const TDesC* aListName)
{
    JELOG2(EPim);
    CPIMContactList* contactList = NULL;

    MPIMContactAdapterManager* contactAdapterManager = NULL;
    MPIMContactListAdapter* contactListAdapter = NULL;
    MPIMLocalizationData* localizationData = NULL;

    const TInt n = iAdapterAccesses.Count();
    for (TInt i = 0; i < n; i++)
    {
        if (iAdapterAccesses[i]->OpenContactListL(aListName,
                &contactAdapterManager, &contactListAdapter, &localizationData))
        {
            // got one
            CleanupDeletePushL(contactAdapterManager);
            CleanupDeletePushL(contactListAdapter);

            contactList = CPIMContactList::NewL(contactAdapterManager,
                                                contactListAdapter, localizationData, *iContactValidator);

            CleanupStack::Pop(contactListAdapter);
            CleanupStack::Pop(contactAdapterManager);

            break;
        }

        // else try next Adapter Access
    }

    return contactList;
}

CPIMEventList* CPIMManager::DoOpenEventListL(const TDesC* aListName)
{
    JELOG2(EPim);
    CPIMEventList* eventList = NULL;

    MPIMEventAdapterManager* eventAdapterManager = NULL;
    MPIMEventListAdapter* eventListAdapter = NULL;
    MPIMLocalizationData* localizationData = NULL;    
    const TInt n = iAdapterAccesses.Count();
    for (TInt i = 0; i < n; i++)
    {
        if (iAdapterAccesses[i]->OpenEventListL(iCalSession,aListName,
                                                &eventAdapterManager, &eventListAdapter, &localizationData))
        {
            // got one
            CleanupDeletePushL(eventAdapterManager);
            CleanupDeletePushL(eventListAdapter);

            eventList = CPIMEventList::NewL(eventAdapterManager,
                                            eventListAdapter, localizationData, *iEventValidator);

            CleanupStack::Pop(eventListAdapter);
            CleanupStack::Pop(eventAdapterManager);

            break;
        }

        // else try next Adapter Access
    }

    return eventList;
}

CPIMToDoList* CPIMManager::DoOpenToDoListL(const TDesC* aListName)
{
    JELOG2(EPim);
    CPIMToDoList* toDoList = NULL;

    MPIMToDoAdapterManager* toDoAdapterManager = NULL;
    MPIMToDoListAdapter* toDoListAdapter = NULL;
    MPIMLocalizationData* localizationData = NULL;    
    const TInt n = iAdapterAccesses.Count();
    for (TInt i = 0; i < n; i++)
    {
        if (iAdapterAccesses[i]->OpenToDoListL(iCalSession,aListName, &toDoAdapterManager,
                                               &toDoListAdapter, &localizationData))
        {
            // got one
            CleanupDeletePushL(toDoAdapterManager);
            CleanupDeletePushL(toDoListAdapter);

            toDoList = CPIMToDoList::NewL(toDoAdapterManager, toDoListAdapter,
                                          localizationData, *iToDoValidator);

            CleanupStack::Pop(toDoListAdapter);
            CleanupStack::Pop(toDoAdapterManager);

            break;
        }

        // else try next Adapter Access
    }

    return toDoList;
}

//  End of File
