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
* Description:  To-do list specialization.
 *
*/


// INCLUDE FILES
#include  "cpimtodolist.h"
#include  "cpimtodovalidator.h"
#include  "mpimtodoadaptermanager.h"
#include  "mpimtodolistadapter.h"
#include  "cpimtodoitem.h"
#include  "mpimadaptermanager.h"
#include  "pimtodo.h"
#include  "javasymbianoslayer.h"
#include  "pimjnitools.h"
#include "pimutils.h"
#include  "s60commonutils.h"
#include "logger.h"

CPIMToDoList::CPIMToDoList(const CPIMToDoValidator& aValidator) :
        CPIMList(aValidator), iToDoValidator(aValidator)
{
    JELOG2(EPim);
}

void CPIMToDoList::ConstructL(MPIMToDoAdapterManager* aToDoAdapterManager,
                              MPIMToDoListAdapter* aToDoListAdapter,
                              MPIMLocalizationData* aLocalizationData)
{
    JELOG2(EPim);
    CPIMList::ConstructL(aLocalizationData,
                         aToDoAdapterManager->GetAdapterManager(),
                         aToDoListAdapter->GetPimListAdapter());

    iToDoAdapterManager = aToDoAdapterManager;
    iToDoListAdapter = aToDoListAdapter;
}

CPIMToDoList* CPIMToDoList::NewL(MPIMToDoAdapterManager* aToDoAdapterManager, // not owned
                                 MPIMToDoListAdapter* aToDoListAdapter, // owned
                                 MPIMLocalizationData* aLocalizationData, // not owned
                                 const CPIMToDoValidator& aValidator) // not owned
{
    JELOG2(EPim);
    CPIMToDoList* self = new(ELeave) CPIMToDoList(aValidator);

    CleanupStack::PushL(self);

    self->ConstructL(aToDoAdapterManager, aToDoListAdapter, aLocalizationData);

    CleanupStack::Pop(self);
    return self;
}

CPIMToDoList::~CPIMToDoList()
{
    JELOG2(EPim);
    delete iToDoListAdapter;
    delete iToDoAdapterManager;
}

RPointerArray<CPIMItem>* CPIMToDoList::ItemsL(const TPIMField& aField,
        const TPIMDate& aStartDate, const TPIMDate& aEndDate)
{
    JELOG2(EPim);
    // Check field, range and list state
    TPIMFieldDataType fieldType = iToDoValidator.FieldDataType(aField);

    if (fieldType != EPIMFieldDate)
    {
        User::Leave(KErrArgument);
    }

    if (aStartDate > aEndDate)
    {
        User::Leave(KErrArgument);
    }

    if (!iToDoListAdapter)
    {
        User::Leave(KErrSessionClosed);
    }

    RPointerArray<CPIMItem>* retArr = new(ELeave) RPointerArray<CPIMItem> ;
    CleanupStack::PushL(retArr);
    CleanupResetAndDestroyPushL(*retArr);

    const TInt numToDos = iItems.Count();
    for (TInt toDoIndex = 0; toDoIndex < numToDos; toDoIndex++)
    {
        CPIMItem* toDo = iItems[toDoIndex];

        // Deduce wheter this toDo belongs to the returned set

        if (toDo->CountValuesL(aField) != 0)
        {
            TPIMDate value = toDo->GetDateL(aField, 0);
            if (value >= aStartDate && value <= aEndDate)
            {
                User::LeaveIfError(retArr->Append(toDo));
            }
        }
    }

    if (aField == EPIMToDoDue)
    {
        retArr->Sort(TLinearOrder<CPIMItem> (ToDoCompareDue));
    }
    else if (aField == EPIMToDoCompletionDate)
    {
        retArr->Sort(TLinearOrder<CPIMItem> (ToDoCompareCompletionDate));
    }

    CleanupStack::Pop(); // retArr reset and destroy
    CleanupStack::Pop(retArr);
    return retArr;
}

jintArray CPIMToDoList::itemsByDate(TPIMField aField, jlong aStartDate,
                                    jlong aEndDate, JNIEnv* aJniEnv, jintArray aError)
{
    JELOG2(EPim);
    RPointerArray<CPIMItem>* items = NULL;
    TPIMDate startDate(java::util::S60CommonUtils::JavaTimeToTTime(aStartDate));
    TPIMDate endDate(java::util::S60CommonUtils::JavaTimeToTTime(aEndDate));
    TInt error = 0;
    TRAP(error, items = ItemsL(aField, startDate, endDate));
    SetJavaErrorCode(aJniEnv, aError, error);

    if (error != KErrNone)
    {
        // If a leave occurred the items array was never created
        return NULL;
    }

    // we now own the items array

    jintArray itemHandles = GetJavaItemHandles(*aJniEnv, *items);

    items->Close();
    delete items;

    if (itemHandles == NULL)
    {
        SetJavaErrorCode(aJniEnv, aError, KErrNoMemory);
    }
    return itemHandles;
}

TInt CPIMToDoList::ToDoCompareDue(const CPIMItem& aFirst,
                                  const CPIMItem& aSecond)
{
    JELOG2(EPim);
    TInt retVal = 0;
    TRAPD(err, retVal = ToDoCompareDateFieldL(aFirst, aSecond, EPIMToDoDue));

    if (err != KErrNone)
    {
        retVal = 0;
    }

    return retVal;
}

TInt CPIMToDoList::ToDoCompareCompletionDate(const CPIMItem& aFirst,
        const CPIMItem& aSecond)
{
    JELOG2(EPim);
    TInt retVal = 0;
    TRAPD(err, retVal = ToDoCompareDateFieldL(aFirst, aSecond,
                        EPIMToDoCompletionDate));

    if (err != KErrNone)
    {
        retVal = 0;
    }

    return retVal;
}

TInt CPIMToDoList::ToDoCompareDateFieldL(const CPIMItem& aFirst,
        const CPIMItem& aSecond, const TPIMField& aField)
{
    JELOG2(EPim);
    TInt retVal = 0;

    TPIMDate firstDate = aFirst.GetDateL(aField, 0);
    TPIMDate secondDate = aSecond.GetDateL(aField, 0);

    if (firstDate < secondDate)
    {
        retVal = -1;
    }
    else if (firstDate > secondDate)
    {
        retVal = 1;
    }
    // else leave retVal 0

    return retVal;
}

CPIMToDoItem* CPIMToDoList::DoCreateToDoL()
{
    JELOG2(EPim);
    // Create
    CPIMToDoItem* newToDo = CPIMToDoItem::NewL(iToDoValidator);

    // Associate with list
    newToDo->SetToDoAdapterAssociation(iToDoAdapterManager, iToDoListAdapter);

    return newToDo;
}

void CPIMToDoList::close()
{
    JELOG2(EPim);
    if (!iToDoListAdapter)
    {
        throw KErrSessionClosed ;
    }

    CPIMList::close();

    delete iToDoListAdapter;
    iToDoListAdapter = NULL;
}

// ---------------------------------------------------------------------------
// CPIMToDoList::DoCreateItemL
// ToDo list doesn't care about matching item because it doesn't support
// any performance improvement mechanisms
// ---------------------------------------------------------------------------
//
CPIMItem* CPIMToDoList::DoCreateItemL(const TPIMItemID& aItemId, CPIMItem* /*aMatchingItem*/)
{
    JELOG2(EPim);
    CPIMToDoItem* newToDo = DoCreateToDoL();

    if (aItemId != KPIMNullItemID)
    {
        CleanupStack::PushL(newToDo);
        __ASSERT_ALWAYS(iToDoListAdapter, User::Leave(KErrSessionClosed));

        // Load new item from the native database
        newToDo->SetToDoItemIdL(aItemId);
        iToDoListAdapter->ReadToDoItemL(*newToDo);
        newToDo->UpdateUidFieldL(EPIMToDoUid, aItemId);
        CleanupStack::Pop(newToDo);
    }

    return newToDo;
}

void CPIMToDoList::DoUpdateItemL(CPIMItem& aItem)
{
    JELOG2(EPim);
    if (!iToDoListAdapter)
    {
        User::Leave(KErrSessionClosed);
    }

    CPIMToDoItem& toDoItem = static_cast<CPIMToDoItem&>(aItem);
    iToDoListAdapter->ReadToDoItemL(toDoItem);
    toDoItem.UpdateUidFieldL(EPIMToDoUid, aItem.GetId());
}

void CPIMToDoList::DoDeleteItemL(CPIMItem& aItem)
{
    JELOG2(EPim);
    if (!iToDoListAdapter)
    {
        User::Leave(KErrSessionClosed);
    }

    iToDoListAdapter->RemoveToDoItemL(aItem.GetId());
    aItem.UpdateUidFieldL(EPIMToDoUid, KPIMNullItemID());
}

//  End of File
