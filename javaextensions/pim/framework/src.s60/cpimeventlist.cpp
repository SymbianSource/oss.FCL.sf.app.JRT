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
* Description:  Event list specialization.
 *
*/


// INCLUDE FILES
#include  "cpimeventlist.h"
#include  "cpimeventvalidator.h"
#include  "mpimeventadaptermanager.h"
#include  "cpimeventlistadapter.h"
#include  "cpimeventitem.h"
#include  "cpimrepeatrule.h"
#include  "cpimeventmatcher.h"
#include  "pimpanics.h"
#include  "pimrepeatrule.h"
#include  "pimjnitools.h"
#include  "pimutils.h"
#include  "s60commonutils.h"
#include  "javasymbianoslayer.h"
#include  "logger.h"

CPIMEventList::CPIMEventList(const CPIMEventValidator& aValidator) :
        CPIMList(aValidator), iEventValidator(aValidator)
{
    JELOG2(EPim);
}

void CPIMEventList::ConstructL(MPIMEventAdapterManager* aEventAdapterManager,
                               MPIMEventListAdapter* aEventListAdapter,
                               MPIMLocalizationData* aLocalizationData)
{
    JELOG2(EPim);
    CPIMList::ConstructL(aLocalizationData,
                         aEventAdapterManager->GetAdapterManager(),
                         aEventListAdapter->GetPimListAdapter());

    iEventAdapterManager = aEventAdapterManager;
    iEventListAdapter = aEventListAdapter;
}

CPIMEventList* CPIMEventList::NewL(
    MPIMEventAdapterManager* aEventAdapterManager, // owned
    MPIMEventListAdapter* aEventListAdapter, // owned
    MPIMLocalizationData* aLocalizationData, // not owned
    const CPIMEventValidator& aValidator) // not owned
{
    JELOG2(EPim);
    CPIMEventList* self = new(ELeave) CPIMEventList(aValidator);

    CleanupStack::PushL(self);

    self->ConstructL(aEventAdapterManager, aEventListAdapter, aLocalizationData);

    CleanupStack::Pop(self);
    return self;
}

CPIMEventList::~CPIMEventList()
{
    JELOG2(EPim);
    delete iEventListAdapter;
    delete iEventAdapterManager;
}

RPointerArray<CPIMItem>* CPIMEventList::ItemsL(
    const TPIMEventSearchType& aSearchType, const TPIMDate& aStartDate,
    const TPIMDate& aEndDate, TBool aInitialEventOnly)
{
    JELOG2(EPim);
    if (!iEventListAdapter)
    {
        User::Leave(KErrSessionClosed);
    }

    CPIMEventMatcher
    * eventMatcher =
        new(ELeave) CPIMEventMatcher(aSearchType, aStartDate, aEndDate, aInitialEventOnly);

    CleanupStack::PushL(eventMatcher);

    if (!eventMatcher->SaneOptions())
    {
        User::Leave(KErrArgument);
    }

    // Temporary result array
    RArray<TPIMItemOccurrence> itemOccurrences;

    const TInt n = iItems.Count();
    for (TInt i = 0; i < n; i++)
    {
        CPIMEventItem* event = static_cast<CPIMEventItem*>(iItems[i]);

        TPIMDate occurrenceStart(0);
        if (eventMatcher->MatchL(*event, occurrenceStart))
        {
            TPIMItemOccurrence itemOccurrence(event, occurrenceStart);
            User::LeaveIfError(itemOccurrences.Append(itemOccurrence));
        }
    }

    RPointerArray<CPIMItem>* retArr = new(ELeave) RPointerArray<CPIMItem> ;

    CleanupStack::PushL(retArr);
    CleanupResetAndDestroyPushL(*retArr);

    itemOccurrences.Sort(TLinearOrder<TPIMItemOccurrence> (
                             CompareItemOccurrences));

    const TInt numOccs = itemOccurrences.Count();
    for (TInt occIndex = 0; occIndex < numOccs; occIndex++)
    {
        User::LeaveIfError(retArr->Append(itemOccurrences[occIndex].iItem));
    }

    CleanupStack::Pop();
    CleanupStack::Pop(retArr);
    itemOccurrences.Close();
    CleanupStack::PopAndDestroy(eventMatcher);
    return retArr;
}

jintArray CPIMEventList::itemsByDate(jint aSearchType, jlong aStartDate,
                                     jlong aEndDate, jboolean aInitialEventOnly, JNIEnv* aJniEnv,
                                     jintArray aError)
{
    JELOG2(EPim);
    TPIMDate startDate(java::util::S60CommonUtils::JavaTimeToTTime(aStartDate));
    TPIMDate endDate(java::util::S60CommonUtils::JavaTimeToTTime(aEndDate));

    RPointerArray<CPIMItem>* items = NULL;
    TInt error = KErrNone;
    TRAP(error, items = ItemsL(static_cast<TPIMEventSearchType>(aSearchType),
                               startDate, endDate, static_cast<TBool>(aInitialEventOnly)));

    SetJavaErrorCode(aJniEnv, aError, error);

    if (error != KErrNone)
    {
        // If a leave occurred the array of items was never created.
        return NULL;
    }

    // We now own the items array

    jintArray itemHandles = GetJavaItemHandles(*aJniEnv, *items);

    items->Close();
    delete items;

    if (!itemHandles)
    {
        SetJavaErrorCode(aJniEnv, aError, KErrNoMemory);
    }

    return itemHandles;
}

const CArrayFix<TPIMField>& CPIMEventList::GetSupportedRepeatRuleFieldsL(
    const TPIMRepeatRuleFrequency& aFrequency)
{
    JELOG2(EPim);
    return iEventAdapterManager->GetSupportedRepeatRuleFieldsL(aFrequency);
}

jintArray CPIMEventList::getSupportedRepeatRuleFields(jint aFrequency,
        JNIEnv* aJniEnv, jintArray aError)
{
    JELOG2(EPim);
    const CArrayFix<TPIMField>* fields = NULL;
    TInt error = KErrNone;
    TRAP(error, fields = &(GetSupportedRepeatRuleFieldsL(
                               static_cast<TPIMRepeatRuleFrequency>(aFrequency))));
    SetJavaErrorCode(aJniEnv, aError, error);

    if (error != KErrNone)
    {
        return NULL;
    }

    // The cast works because TPIMField is really TInt
    jintArray javaFields = ConvertToJavaIntArray(*aJniEnv,
                           *reinterpret_cast<const CArrayFix<TInt>*>(fields));

    if (!javaFields)
    {
        SetJavaErrorCode(aJniEnv, aError, KErrNoMemory);
    }

    return javaFields;
}

TInt CPIMEventList::CompareItemOccurrences(const TPIMItemOccurrence& aFirst,
        const TPIMItemOccurrence& aSecond)
{
    JELOG2(EPim);
    TInt retVal = 0;

    if (aFirst.iOccurrenceStart < aSecond.iOccurrenceStart)
    {
        retVal = -1;
    }
    else if (aFirst.iOccurrenceStart > aSecond.iOccurrenceStart)
    {
        retVal = 1;
    }
    else
    {
        TRAPD(err, retVal
              = CompareSummariesL(*(aFirst.iItem), *(aSecond.iItem)));

        if (err != KErrNone)
        {
            retVal = 0;
        }
    }

    return retVal;
}

TInt CPIMEventList::CompareSummariesL(const CPIMItem& aFirst,
                                      const CPIMItem& aSecond)
{
    JELOG2(EPim);
    TInt retVal = 0;

    const TDesC& firstSummary = aFirst.GetStringL(EPIMEventSummary, 0);
    const TDesC& secondSummary = aSecond.GetStringL(EPIMEventSummary, 0);

    if (firstSummary < secondSummary)
    {
        retVal = -1;
    }
    else if (firstSummary > secondSummary)
    {
        retVal = 1;
    }
    // else leave retVal 0

    return retVal;
}

CPIMEventItem* CPIMEventList::DoCreateEventL()
{
    JELOG2(EPim);
    // Create
    CPIMEventItem* newEvent = CPIMEventItem::NewL(iEventValidator);

    // Associate with list
    newEvent->SetEventAdapterAssociation(iEventAdapterManager,
                                         iEventListAdapter);

    return newEvent;
}

void CPIMEventList::close()
{
    JELOG2(EPim);
    if (!iEventListAdapter)
    {
        throw KErrSessionClosed ;
    }

    CPIMList::close();

    delete iEventListAdapter;
    iEventListAdapter = NULL;

}

// ---------------------------------------------------------------------------
// CPIMEventList::DoCreateItemL
// Event list doesn't care about matching item because it doesn't support
// any performance improvement mechanisms
// ---------------------------------------------------------------------------
//
CPIMItem* CPIMEventList::DoCreateItemL(const TPIMItemID& aItemId, CPIMItem* /*aMatchingItem*/)
{
    JELOG2(EPim);
    CPIMEventItem* newEvent = DoCreateEventL();

    if (aItemId != KPIMNullItemID)
    {
        CleanupStack::PushL(newEvent);
        __ASSERT_ALWAYS(iEventListAdapter, User::Leave(KErrSessionClosed));
        // Load new item from the native database
        newEvent->SetEventItemIdL(aItemId);
        iEventListAdapter->ReadEventItemL(*newEvent);
        newEvent->UpdateUidFieldL(EPIMEventUid, aItemId);
        CleanupStack::Pop(newEvent);
    }

    return newEvent;
}

void CPIMEventList::DoUpdateItemL(CPIMItem& aItem)
{
    JELOG2(EPim);
    if (!iEventListAdapter)
    {
        User::Leave(KErrSessionClosed);
    }

    CPIMEventItem& eventItem = static_cast<CPIMEventItem&>(aItem);
    iEventListAdapter->ReadEventItemL(eventItem);
    eventItem.UpdateUidFieldL(EPIMEventUid, aItem.GetId());
}

void CPIMEventList::DoDeleteItemL(CPIMItem& aItem)
{
    JELOG2(EPim);
    if (!iEventListAdapter)
    {
        User::Leave(KErrSessionClosed);
    }

    iEventListAdapter->RemoveEventItemL(aItem.GetId());
    aItem.UpdateUidFieldL(EPIMEventUid, TPtrC8(KPIMNullItemID));
}

//  End of File
