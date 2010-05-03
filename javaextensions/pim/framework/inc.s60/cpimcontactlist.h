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
* Description:  Contact list specialization.
 *
*/


#ifndef CPIMCONTACTLIST_H
#define CPIMCONTACTLIST_H

//  INCLUDES
#include "cpimlist.h"


// FORWARD DECLARATIONS
class MPIMContactAdapterManager;
class MPIMContactListAdapter;
class MPIMLocalizationData;
class CPIMContactValidator;
class CPIMContactItem;

// CLASS DECLARATION

/**
 *  Contact PIM list specialization.
 */
NONSHARABLE_CLASS(CPIMContactList): public CPIMList
{
public: // Constructors and destructor

    /**
     * Two-phased constructor.
     *
     * @param aContactAdapterManager Associated contact adapter manager.
     *        Ownership of the object is taken.
     * @param aContactListAdapter Associated list adapter.
     *        Ownership of the object is taken.
     * @param aLocalizationData Associated list adapter, not owned.
     * @param aValidator Contact validator, not owned.
     *
     * @return New instance of this class.
     */
    static CPIMContactList* NewL(
        MPIMContactAdapterManager* aContactAdapterManager,
        MPIMContactListAdapter* aContactListAdapter,
        MPIMLocalizationData* aLocalizationData,
        const CPIMContactValidator& aValidator);

    // functions from pimbaselist
    jintArray itemsByDate(
        jint /*aSearchType*/, jlong /*aStartDate*/,
        jlong /*aEndDate*/, jboolean /*aInitialEventOnly*/,
        JNIEnv* /*aJniEnv*/,jintArray /*aError*/)
    {
        return NULL;
    }

    jintArray getSupportedRepeatRuleFields(
        jint /*aFrequency*/, JNIEnv* /*aJniEnv*/, jintArray /*aError*/)
    {
        return NULL;
    }

    jintArray itemsByDate(
        TPIMField /*aField*/, jlong /*aStartDate*/,jlong /*aEndDate*/,
        JNIEnv* /*aJniEnv*/, jintArray /*aError*/)
    {
        return NULL;
    }

    /**
     * Destructor.
     */
    virtual ~CPIMContactList();

protected: // New functions

    /**
     * Creates a new contact and associates it with the list.
     */
    CPIMContactItem* DoCreateContactL();

protected: // From CPIMList

    void close();

    CPIMItem* DoCreateItemL(const TPIMItemID& aItemId,
                            CPIMItem* aMatchingItem);
    void DoUpdateItemL(CPIMItem& aItem);
    void DoDeleteItemL(CPIMItem& aItem);

protected:

    /**
     * Constructor.
     */
    CPIMContactList(const CPIMContactValidator& aValidator);

    /**
     * 2nd phase constructor.
     * \b Must be called from derived classes during their instantiation.
     */
    void ConstructL(
        MPIMContactAdapterManager* aContactAdapterManager,
        MPIMContactListAdapter* aContactListAdapter,
        MPIMLocalizationData* aLocalizationdata);

private: // Data

    /**
     * Contact adapter manager, owned.
     * Adapter manager must always be present.
     */
    MPIMContactAdapterManager* iContactAdapterManager;

    /**
     * Contact list adapter, owned.
     * May be NULL, which indicates that the list is closed.
     */
    MPIMContactListAdapter* iContactListAdapter;

    /** Contact validator, not owned. */
    const CPIMContactValidator& iContactValidator;

};

#endif // CPIMCONTACTLIST_H
// End of File
