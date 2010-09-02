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

#include <e32std.h>
#include <badesca.h>
#include <calsession.h>
#include <flogger.h>
#ifdef RD_JAVA_PIM_MULTICAL_ENABLED
#include <calcalendarinfo.h>
#include <calenmulticaluids.hrh>
#endif
#include "cpimmanager.h"
#include "cpimcontactlist.h"
#include "cpimcontactvalidator.h"
#include "cpimeventvalidator.h"
#include "cpimtodovalidator.h"
#include "cpimeventlist.h"
#include "cpimtodolist.h"
#include "cpimversit.h"
#include "cpimcmadapteraccess.h"
#include "cpimeventadapteraccess.h"
#include "cpimtodoadapteraccess.h"
#include "cpimlocalizationmanager.h"
#include "fs_methodcall.h"
#include "jstringutils.h"
#include "logger.h"
#include "mpimcontactadaptermanager.h"
#include "mpimeventadaptermanager.h"
#include "mpimtodoadaptermanager.h"
#include "mpimcontactlistadapter.h"
#include "mpimeventlistadapter.h"
#include "mpimtodolistadapter.h"
#include "mpimlocalizationmanager.h"
#include "mpimlocalizationdata.h"
#include "mpimadapteraccess.h"
#include "pimcommon.h"
#include "pimpanics.h"
#include "pimjnitools.h"
#include "pimutils.h"
#include "s60commonutils.h"

// CONSTANTS

/**
 * Array granularity for arrays of managers, cached list names and
 * localization objects. Number of such objects in an array is quite
 * low, at the time of writing 1 to 4.
 */
const TInt KManagerArrayGranularity = 2;
#ifdef RD_JAVA_PIM_MULTICAL_ENABLED
const TInt KBuffLength = 24;
#endif

CPIMManager::CPIMManager() :
        java::util::FunctionServer("CPIMManager"), iAdapterAccesses(), iCalSessions()
{
    JELOG2(EPim);
}

void CPIMManager::ConstructL()
{
    JELOG2(EPim);
    iContactValidator = CPIMContactValidator::NewL();
    iEventValidator = CPIMEventValidator::NewL();
    iToDoValidator = CPIMToDoValidator::NewL();
    iDesCArray = new(ELeave) CDesCArrayFlat(KManagerArrayGranularity);
    iLocalizationManager
    = (MPIMLocalizationManager*)(CPIMLocalizationManager::NewL());

    createServerToNewThread();
    CallMethodL(this, &CPIMManager::createCalSessionL, this);
}

void CPIMManager::createCalSessionL()
{
    iCalSession = CCalSession::NewL();
    //CleanupStack::PushL(iCalSession);
    iCalSessionArray = iCalSession->ListCalFilesL();

    TInt index = 0;
    TInt find = iCalSessionArray->Find(iCalSession->DefaultFileNameL(),index);

    if (!find)
    {
        iCalSessionArray->Delete(index);
    }

    TRAPD(err, iCalSession->OpenL(iCalSession->DefaultFileNameL()));
    if (KErrNotFound == err)
    {
        iCalSession->CreateCalFileL(iCalSession->DefaultFileNameL());
        iCalSession->OpenL(iCalSession->DefaultFileNameL());
    }
    else
    {
        User::LeaveIfError(err);
    }
    //CleanupStack::Pop(iCalSession);

#ifdef RD_JAVA_PIM_MULTICAL_ENABLED
    TInt iterate = 0;
    TBool softDelete = EFalse;

    while (iterate < iCalSessionArray->Count())
    {
        CCalSession* aCalSession = CCalSession::NewL(*iCalSession);
        CleanupStack::PushL(aCalSession);
        aCalSession->OpenL(iCalSessionArray->MdcaPoint(iterate));
        CCalCalendarInfo* caleninfo = aCalSession->CalendarInfoL();
        CleanupStack::PushL(caleninfo);
        aCalSession->SetCalendarInfoL(*caleninfo);

        TBuf8<KBuffLength> keyBuff;
        keyBuff.Zero();
        keyBuff.AppendNum(EMarkAsDelete);
        TPckgC<TBool> pkgSoftDelete(softDelete);
        TRAPD(err,pkgSoftDelete.Set(caleninfo->PropertyValueL(keyBuff)));
        if (KErrNone == err)
        {
            softDelete = pkgSoftDelete();
        }

        if (softDelete)
        {

            CleanupStack::PopAndDestroy(caleninfo);
            CleanupStack::PopAndDestroy(aCalSession);
        }
        else
        {
            iCalSessions.AppendL(aCalSession);
            iDesCArray->AppendL(iCalSessionArray->MdcaPoint(iterate));
            CleanupStack::PopAndDestroy(caleninfo);
            CleanupStack::Pop(aCalSession);
        }
        iterate++;
    }
    delete iCalSessionArray;
    iCalSessionArray = NULL;
#endif
}

pimbasemanager* pimbasemanager::getInstance()
{
    JELOG2(EPim);
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
    delete iDesCArray;
    iToDoValidator = NULL;
    stopServer();
}
void CPIMManager::DeleteSessions()
{
    JELOG2(EPim);
    iCalSessions.ResetAndDestroy();
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
                                      jstring aPimListName, jstring aCalName,
                                      JNIEnv* aJniEnv)
{
    JELOG2(EPim);
    const JStringUtils listName(*aJniEnv, aPimListName);
    const TDesC* listNameArg = (aPimListName ? &listName : NULL);
    const JStringUtils calName(*aJniEnv, aCalName);
    const TDesC* calNameArg = (aCalName ? &calName : NULL);
    pimbaselist* list = NULL;
    TInt error = KErrNone;
    TRAP(error, list = DoOpenPimListL(aPimListType, listNameArg, calNameArg));
    if (error != KErrNone)
        throw error;
    return list;
}

pimbaselist* CPIMManager::DoOpenPimListL(const TPIMListType& aPimListType,
        const TDesC* aPimListName, const TDesC* aCalNameArg)
{
    JELOG2(EPim);
    pimbaselist* list = NULL;

    if (aPimListType == EPIMContactList)
    {
        list = DoOpenContactListL(aPimListName);
    }
    else if (aPimListType == EPIMEventList)
    {
        list = DoOpenEventListL(aPimListName, aCalNameArg);
    }
    else if (aPimListType == EPIMToDoList)
    {
        list = DoOpenToDoListL(aPimListName, aCalNameArg);
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





void CPIMManager::DoListCalendarsL()
{

#ifdef RD_JAVA_PIM_MULTICAL_ENABLED
    iCalList = iCalSession->ListCalFilesL();

    TInt index = 0;
    TInt find = iCalList->Find(iCalSession->DefaultFileNameL(),index);

    if (!find)
    {
        iCalList->Delete(index);
    }
    iCalSessionArray = iCalList;
    TInt iterate = 0;
    TInt aPosition = 0;
    while (iterate < iCalList->Count())
    {
        aPosition = 0;

        TInt findInCurrentArray = iDesCArray->Find(iCalList->MdcaPoint(iterate),aPosition, ECmpNormal16);
        CCalSession* aCalSession;
        if (findInCurrentArray == 0)
        {
            aCalSession = iCalSessions[aPosition];
        }
        else
        {
            aCalSession = CCalSession::NewL(*iCalSession);
            CleanupStack::PushL(aCalSession);
            aCalSession->OpenL(iCalList->MdcaPoint(iterate));
        }
        // check for soft deleted calendars, incase of calendars being held by other applications, this will be followed.
        CCalCalendarInfo* caleninfo = aCalSession->CalendarInfoL();
        CleanupStack::PushL(caleninfo);
        aCalSession->SetCalendarInfoL(*caleninfo);
        TBool softDelete = EFalse;
        TBuf8<KBuffLength> keyBuff;
        keyBuff.Zero();
        keyBuff.AppendNum(EMarkAsDelete);

        TPckgC<TBool> pkgSoftDelete(softDelete);

        TRAPD(err,pkgSoftDelete.Set(caleninfo->PropertyValueL(keyBuff)));

        if (KErrNone == err)
        {
            softDelete = pkgSoftDelete();
        }

        if (!softDelete && findInCurrentArray != 0)
        {
            iCalSessions.AppendL(aCalSession);
            CleanupStack::Pop(aCalSession);
            iDesCArray->AppendL(iCalList->MdcaPoint(iterate));
        }
        else if (softDelete && findInCurrentArray == 0)
        {
            CleanupStack::PopAndDestroy(caleninfo);
            caleninfo = NULL;
            delete aCalSession;
            iCalSessions[aPosition] = NULL;
            iCalSessions.Remove(aPosition);
            iDesCArray->Delete(aPosition);

        }
        else if (softDelete && findInCurrentArray != 0)
        {
            CleanupStack::PopAndDestroy(caleninfo);
            CleanupStack::PopAndDestroy(aCalSession);
            caleninfo=NULL;
            aCalSession = NULL;
        }
        if (caleninfo != NULL)
        {
            CleanupStack::PopAndDestroy(caleninfo);
            caleninfo = NULL;
        }
        iterate++;

    }

    iCalList = iDesCArray;
    iCalList->AppendL(iCalSession->DefaultFileNameL());
    delete iCalSessionArray;
    iCalSessionArray = NULL;
#endif

}


jobjectArray CPIMManager::listCalendars(jintArray aError,
                                        JNIEnv* aJniEnv)
{

    jobjectArray javaStringArray = NULL;
    TInt error = KErrNone;
    TRAP(error, CallMethodL(this, &CPIMManager::DoListCalendarsL, this));
    if (error == KErrNone)
    {
        javaStringArray = CreateJavaStringArray(aJniEnv, *iCalList, EFalse);
        if (!javaStringArray)
        {
            SetJavaErrorCode(aJniEnv, aError, KErrNoMemory);
        }
        iCalList = NULL;
        TInt index = 0;
        TInt find = 0;
        TRAPD(error,find = iDesCArray->Find(iCalSession->DefaultFileNameL(),index));
        if (error == KErrNone)
            if (!find)
            {
                iDesCArray->Delete(index);
            }
            else
            {
                SetJavaErrorCode(aJniEnv, aError, error);
            }
    }
    else
    {

        SetJavaErrorCode(aJniEnv, aError, error);
    }
    return javaStringArray;
}

jobjectArray CPIMManager::listCalendarNames(jintArray aError,
        JNIEnv* aJniEnv)
{
    jobjectArray javaStringArray = NULL;
    TInt error = KErrNone;
    TRAP(error, CallMethodL(this, &CPIMManager::DoListCalendarNamesL, this));
    if (error == KErrNone)
    {
        javaStringArray = CreateJavaStringArray(aJniEnv, *iCalListName, EFalse);
        if (!javaStringArray)
        {
            SetJavaErrorCode(aJniEnv, aError, KErrNoMemory);
        }
        delete iCalListName;
        iCalListName = NULL;
    }
    else
    {
        SetJavaErrorCode(aJniEnv, aError, error);
    }
    return javaStringArray;
}

void CPIMManager::DoListCalendarNamesL()
{
#ifdef RD_JAVA_PIM_MULTICAL_ENABLED
    iCalListName = new(ELeave) CDesCArrayFlat(KManagerArrayGranularity);
    //CleanupStack::PushL(iCalListName);
    for (int iterate = 0; iterate < iDesCArray->Count(); iterate++)
    {
        CCalCalendarInfo* caleninfo = iCalSessions[iterate]->CalendarInfoL();
        CleanupStack::PushL(caleninfo);
        iCalListName->AppendL(caleninfo->NameL());

        CleanupStack::PopAndDestroy(caleninfo);
    }
    CCalCalendarInfo* caleninfo = iCalSession->CalendarInfoL();
    CleanupStack::PushL(caleninfo);
    iCalListName->AppendL(caleninfo->NameL());

    CleanupStack::PopAndDestroy(caleninfo);
    //CleanupStack::Pop(iCalListName);
#endif
}
void CPIMManager::createCalendar(jstring aCalName,jstring aDisplayName, JNIEnv* aJniEnv)
{
    const JStringUtils calName(*aJniEnv, aCalName);
    const JStringUtils displayName(*aJniEnv, aDisplayName);
    const TDesC* displayNameArg = (aDisplayName ? &displayName : NULL);
    const TDesC* calNameArg = (aCalName ? &calName : NULL);
    TInt error = KErrNone;
    TRAP(error, CallMethodL(this, &CPIMManager::DoCreateCalFileL, *calNameArg,*displayNameArg, this));
    if (error != KErrNone)
        throw error;
}

void CPIMManager::DoCreateCalFileL(const TDesC& aFileName, const TDesC& aDisplayName)
{
    #ifdef RD_JAVA_PIM_MULTICAL_ENABLED

    CCalSession* aCalSession = CCalSession::NewL(*iCalSession);
    CleanupStack::PushL(aCalSession);
    CCalCalendarInfo* calendarInfo = CCalCalendarInfo::NewL();
    CleanupStack::PushL(calendarInfo);

    aCalSession->CreateCalFileL(aFileName,*calendarInfo);
    CleanupStack::PopAndDestroy(calendarInfo);
    aCalSession->OpenL(aFileName);
    CCalCalendarInfo* calendarinfostack = aCalSession->CalendarInfoL();
    CleanupStack::PushL(calendarinfostack);

    calendarinfostack->SetNameL(aDisplayName);
    calendarinfostack->SetEnabled(ETrue);
    aCalSession->SetCalendarInfoL(*calendarinfostack);
    CleanupStack::PopAndDestroy(calendarinfostack);
    iCalSessions.AppendL(aCalSession);
    iDesCArray->AppendL(aFileName);
    CleanupStack::Pop(aCalSession);

    #endif
}

void CPIMManager::deleteCalendar(jstring aCalName, JNIEnv* aJniEnv)
{
    const JStringUtils calName(*aJniEnv, aCalName);
    const TDesC* calNameArg = (aCalName ? &calName : NULL);
    TInt error = KErrNone;
    TRAP(error, CallMethodL(this, &CPIMManager::DoDeleteCalFileL, *calNameArg, this));
    if (error != KErrNone)
        throw error;

}


void CPIMManager::DoDeleteCalFileL(const TDesC& aFileName)
{
	  #ifdef RD_JAVA_PIM_MULTICAL_ENABLED

    if (aFileName != iCalSession->DefaultFileNameL())
    {
        TInt aPosition = 0;
        TInt findInCurrentArray = iDesCArray->Find(aFileName, aPosition, ECmpNormal16);
        if (findInCurrentArray == 0)
        {
            CCalCalendarInfo* caleninfo = iCalSessions[aPosition]->CalendarInfoL();
            CleanupStack::PushL(caleninfo);

            caleninfo->SetEnabled(EFalse);

            TBuf8<KBuffLength> keyBuff;

            keyBuff.Zero();
            keyBuff.AppendNum(EMarkAsDelete);
            TPckgC<TBool> pkgSoftDelete(ETrue);
            caleninfo->SetPropertyL(keyBuff, pkgSoftDelete);

            CleanupStack::PopAndDestroy(caleninfo);
            TRAP_IGNORE(iCalSessions[aPosition]->DeleteCalFileL(aFileName));

            delete iCalSessions[aPosition];
            iCalSessions[aPosition]= NULL;
            iCalSessions.Remove(aPosition);

            iDesCArray->Delete(aPosition);
        }
        else
        {
            User::Leave(KErrNotFound);
        }

    }
    else
    {
        User::Leave(KErrAccessDenied);
    }

    #endif
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
            CleanupDeletePushL(contactAdapterManager);
            CleanupDeletePushL(contactListAdapter);
            contactList = CPIMContactList::NewL(contactAdapterManager,
                                                contactListAdapter, localizationData, *iContactValidator);
            CleanupStack::Pop(contactListAdapter);
            CleanupStack::Pop(contactAdapterManager);
            break;
        }
    }

    return contactList;
}

CPIMEventList* CPIMManager::DoOpenEventListL(const TDesC* aListName, const TDesC* aCalNameArg)
{
    JELOG2(EPim);
    CPIMEventList* eventList = NULL;

    MPIMEventAdapterManager* eventAdapterManager = NULL;
    MPIMEventListAdapter* eventListAdapter = NULL;
    MPIMLocalizationData* localizationData = NULL;


    CCalSession* calSession = NULL;
    TInt findInCurrentArray = 0;
    TInt aPosition = 0;
    if (aCalNameArg == NULL || *aCalNameArg == iCalSession->DefaultFileNameL())
    {
        calSession = iCalSession;
    }
    else
    {
        findInCurrentArray = iDesCArray->Find(*aCalNameArg, aPosition);

        if (findInCurrentArray == 0)
        {
            calSession = iCalSessions[aPosition];
        }
        else
        {
            User::Leave(KErrNotFound);
        }
    }

    const TInt n = iAdapterAccesses.Count();
    for (TInt i = 0; i < n; i++)
    {
        if (iAdapterAccesses[i]->OpenEventListL(calSession, aListName,
                                                &eventAdapterManager, &eventListAdapter, &localizationData))
        {
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

CPIMToDoList* CPIMManager::DoOpenToDoListL(const TDesC* aListName, const TDesC* aCalNameArg)
{
    JELOG2(EPim);
    CPIMToDoList* toDoList = NULL;

    MPIMToDoAdapterManager* toDoAdapterManager = NULL;
    MPIMToDoListAdapter* toDoListAdapter = NULL;
    MPIMLocalizationData* localizationData = NULL;
    TInt aPosition = 0;
    TInt findInCurrentArray = 0;


    CCalSession* calSession = NULL;

    if (aCalNameArg == NULL || *aCalNameArg == iCalSession->DefaultFileNameL())
    {
        calSession = iCalSession;
    }
    else
    {
        findInCurrentArray = iDesCArray->Find(*aCalNameArg, aPosition);

        if (findInCurrentArray == 0)
        {
            calSession = iCalSessions[aPosition];
        }
        else
        {
            User::Leave(KErrNotFound);
        }
    }
    const TInt n = iAdapterAccesses.Count();
    for (TInt i = 0; i < n; i++)
    {
        if (iAdapterAccesses[i]->OpenToDoListL(calSession, aListName,
                                               &toDoAdapterManager, &toDoListAdapter,&localizationData))
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
