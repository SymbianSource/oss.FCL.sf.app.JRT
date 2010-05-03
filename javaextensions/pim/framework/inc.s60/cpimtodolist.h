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


#ifndef CPIMTODOLIST_H
#define CPIMTODOLIST_H

//  INCLUDES
#include "cpimlist.h"

// FORWARD DECLARATIONS
class MPIMAdapterManager;
class MPIMListAdapter;
class MPIMLocalizationData;
class MPIMToDoAdapterManager;
class MPIMToDoListAdapter;
class CPIMToDoValidator;
class CPIMToDoItem;

// CLASS DECLARATION

/**
 *  ToDo PIM list specialization.
 */
NONSHARABLE_CLASS(CPIMToDoList): public CPIMList
{
public: // Constructors and destructor

    /**
     * Two-phased constructor.
     *
     * @param aToDoAdapterManager Associated toDo adapter manager.
     *        Ownership of the object is taken.
     * @param aToDoListAdapter Associated list adapter.
     *        Ownership of the object is taken.
     * @param aLocalizationData Associated list adapter, not owned.
     * @param aValidator ToDo validator, not owned.
     *
     * @return New instance of this class.
     */
    static CPIMToDoList* NewL(
        MPIMToDoAdapterManager* aToDoAdapterManager,
        MPIMToDoListAdapter* aToDoListAdapter,
        MPIMLocalizationData* aLocalizationData,
        const CPIMToDoValidator& aValidator);

    /**
     * Destructor.
     */
    virtual ~CPIMToDoList();

public: // New functions

    /**
     * Provides event items that start, end or occur in the specified
     * date range according to \a aSearchType and \a aInitialEventOnly.
     *
     * @param aField The field in items for which the range is applied.
     * @param aStartDate Inclusive start date of the range.
     * @param aEndDate Inclusive end date of the range.
     *
     * @return List of items.
     *         \b Ownership if the returned value is transferred to caller.
     *
     * @par Leaving:
     * @li \c KErrArgument - aField is not a date field in to-do items,
     *     \a aStartDate is later than \aEndDate or \a aField is not
     *     a date field.
     * @li \c KErrSessionClosed - list is closed.
     * @li Other - internal error.
     */
    RPointerArray< CPIMItem>* ItemsL(
        const TPIMField& aField,
        const TPIMDate& aStartDate,
        const TPIMDate& aEndDate);

    jintArray itemsByDate(
        TPIMField aField, jlong aStartDate, jlong aEndDate,
        JNIEnv* aJniEnv, jintArray aError);

    // functions from pimbaselist
    jintArray itemsByDate(
        jint /*aSearchType*/, jlong /*aStartDate*/, jlong /*aEndDate*/,
        jboolean /*aInitialEventOnly*/, JNIEnv* /*aJniEnv*/,
        jintArray /*aError*/)
    {
        return NULL;
    }

    jintArray getSupportedRepeatRuleFields(
        jint /*aFrequency*/, JNIEnv* /*aJniEnv*/, jintArray /*aError*/)
    {
        return NULL;
    }

public: // Functions from base classes


protected: // New functions

    /**
     * Creates a new toDo and associates it with the list.
     */
    CPIMToDoItem* DoCreateToDoL();

    static TInt ToDoCompareDue(
        const CPIMItem& aFirst,
        const CPIMItem& aSecond);

    static TInt ToDoCompareCompletionDate(
        const CPIMItem& aFirst,
        const CPIMItem& aSecond);

    static TInt ToDoCompareDateFieldL(
        const CPIMItem& aFirst,
        const CPIMItem& aSecond,
        const TPIMField& aField);

protected: // Functions from CPIMList

    void close();

    CPIMItem* DoCreateItemL(const TPIMItemID& aItemId,
                            CPIMItem* aMatchingItem);
    void DoUpdateItemL(CPIMItem& aItem);
    void DoDeleteItemL(CPIMItem& aItem);

protected:

    /**
     * Constructor.
     */
    CPIMToDoList(const CPIMToDoValidator& aValidator);

    /**
     * 2nd phase constructor.
     * \b Must be called from derived classes during their instantiation.
     */
    void ConstructL(
        MPIMToDoAdapterManager* aToDoAdapterManager,
        MPIMToDoListAdapter* aToDoListAdapter,
        MPIMLocalizationData* aLocalizationdata);

private: // Data

    /**
     * ToDo adapter manager, owned.
     * Adapter manager must always be present.
     */
    MPIMToDoAdapterManager* iToDoAdapterManager;

    /**
     * ToDo list adapter, \b owned by the object.
     * May be NULL, which indicates that the list is closed.
     */
    MPIMToDoListAdapter* iToDoListAdapter;

    /** ToDo validator, not owned. */
    const CPIMToDoValidator& iToDoValidator;

};

#endif // CPIMTODOLIST_H
// End of File
