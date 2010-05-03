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


#ifndef CPIMEVENTLIST_H
#define CPIMEVENTLIST_H

//  INCLUDES
#include "cpimlist.h"
#include "pimevent.h"
#include "pimrepeatrule.h"

// FORWARD DECLARATIONS
class MPIMEventAdapterManager;
class MPIMEventListAdapter;
class MPIMLocalizationData;
class CPIMEventValidator;
class CPIMEventItem;

// CLASS DECLARATION

/**
 *  Event PIM list specialization.
 */
NONSHARABLE_CLASS(CPIMEventList): public CPIMList
{
protected: // Type definitions

    /**
     * Gathers information about a single (Event) item and its
     * occurrence date/time (perhaps a repeating instance).
     */
    struct TPIMItemOccurrence
    {
        TPIMItemOccurrence(CPIMItem* aItem,
        const TPIMDate& aOccurrenceStart):
                iItem(aItem),
                iOccurrenceStart(aOccurrenceStart) {}

        CPIMItem* iItem;
        TPIMDate iOccurrenceStart;
    };

public: // Constructors and destructor

    /**
     * Two-phased constructor.
     *
     * @param aEventAdapterManager Associated event adapter manager.
     *        Ownership of the object is taken.
     * @param aEventListAdapter Associated list adapter.
     *        Ownership of the object is taken.
     * @param aLocalizationData Associated list adapter, not owned.
     * @param aValidator Event validator, not owned.
     *
     * @return New instance of this class.
     */
    static CPIMEventList* NewL(
        MPIMEventAdapterManager* aEventAdapterManager,
        MPIMEventListAdapter* aEventListAdapter,
        MPIMLocalizationData* aLocalizationData,
        const CPIMEventValidator& aValidator);

    /**
     * Destructor.
     */
    virtual ~CPIMEventList();

public: // New functions

    /**
     * Provides event items that start, end or occur in the specified
     * date range according to \a aSearchType and \a aInitialEventOnly.
     *
     * @param aSearchType Indicates whether events that start, end or
     *        occur in the specified range are returned.
     * @param aStartDate Inclusive start date of the range.
     * @param aEndDate Inclusive end date of the range.
     * @param aInitialEventOnly If \c ETrue, repeating is not taken into
     *        account, but only the start and end date of the event itself.
     *        If \c EFalse, repeating occurrences are examined.
     *
     * @return List of items.
     *         \b Ownership if the returned value is transferred to caller.
     *
     * @par Leaving:
     * @li \c KErrArgument - aSearchType is invalid or \a aStartDate is
     *     later than \a aEndDate.
     * @li \c KErrSessionClosed - list is closed.
     * @li Other - internal error.
     */
    RPointerArray< CPIMItem>* ItemsL(
        const TPIMEventSearchType& aSearchType,
        const TPIMDate& aStartDate,
        const TPIMDate& aEndDate,
        TBool aInitialEventOnly);

    jintArray itemsByDate(
        jint aSearchType,
        jlong aStartDate,
        jlong aEndDate,
        jboolean aInitialEventOnly,
        JNIEnv* aJniEnv,
        jintArray aError);

    /**
     * Provides supported repeat rule fields for given repeat rule
     * frequency. The array may be empty.
     *
     * @par Leaving:
     * @li \c KErrArgument - \a aFrequency is not a valid frequency.
     */
    const CArrayFix< TPIMField>& GetSupportedRepeatRuleFieldsL(
        const TPIMRepeatRuleFrequency& aFrequency);

    jintArray getSupportedRepeatRuleFields(
        jint aFrequency, JNIEnv* aJniEnv, jintArray aError);

    // functions from pimbaselist
    jintArray itemsByDate(
        TPIMField /*aField*/, jlong /*aStartDate*/, jlong /*aEndDate*/,
        JNIEnv* /*aJniEnv*/, jintArray /*aError*/)
    {
        return NULL;
    }

protected: // New functions

    /**
     * Compares item occurrences. The comparison is based on
     * occurrence start date and item EPIMEventSummary field
     * content, in this order.
     *
     * @return Negative, if aFirst precedes aSecond;
     *         positive, if aSecond precedes aFirst;
     *         zero, of aFirst and aSecond are equal.
     */
    static TInt CompareItemOccurrences(
        const TPIMItemOccurrence& aFirst,
        const TPIMItemOccurrence& aSecond);

    /**
     * Helper function to CompareItemOccurrences().
     */
    static TInt CompareSummariesL(
        const CPIMItem& aFirst,
        const CPIMItem& aSecond);

    /**
     * Creates new Event item and associates it with the list.
     */
    CPIMEventItem* DoCreateEventL();

protected: // Functions CPIMList

    void close();

    CPIMItem* DoCreateItemL(const TPIMItemID& aItemId,
                            CPIMItem* aMatchingItem);
    void DoUpdateItemL(CPIMItem& aItem);
    void DoDeleteItemL(CPIMItem& aItem);

protected:

    /**
     * Constructor.
     */
    CPIMEventList(const CPIMEventValidator& aValidator);

    /**
     * 2nd phase constructor.
     * \b Must be called from derived classes during their instantiation.
     */
    void ConstructL(
        MPIMEventAdapterManager* aEventAdapterManager,
        MPIMEventListAdapter* aEventListAdapter,
        MPIMLocalizationData* aLocalizationData);

private: // Data

    /**
     * Event adapter manager, owned.
     * Adapter manager must always be present.
     */
    MPIMEventAdapterManager* iEventAdapterManager;

    /**
     * Event list adapter, \b owned by the object.
     * May be NULL, which indicates that the list is closed.
     */
    MPIMEventListAdapter* iEventListAdapter;

    /** Event validator, not owned. */
    const CPIMEventValidator& iEventValidator;

};

#endif // CPIMEVENTLIST_H
// End of File
