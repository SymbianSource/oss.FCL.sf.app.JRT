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
* Description:  Converts PIM Event items and native calendar anniversaries
 *
*/


#ifndef CPIMAGNANNIVADAPTER_H
#define CPIMAGNANNIVADAPTER_H

// INCLUDES
#include "cpimagneventadapter.h"

// FORWARD DECLARATIONS
class MPIMEventItem;

// CLASS DECLARATION
/**
 * Anniversary Item Adapter
 * Converts PIM Event items and native calendar anniversaries
 */
NONSHARABLE_CLASS(CPIMAgnAnnivAdapter) : public CPIMAgnEventAdapter
{
public: // Constructor and destructor

    /**
     * Two-phased constructor.
     */
    static CPIMAgnAnnivAdapter* NewL(java::util::FunctionServer* aFuncServer);

    /**
     * Destructor.
     */
    virtual ~CPIMAgnAnnivAdapter();

public: // From CPIMAgnEventAdapter

    /**
     * Constructs a Agenda Model entry from the framework PIM item.
     *
     * @param aItem The item to convert
     *
     * @return an entry constructed according to framework item.
     * The \b ownership of the card is transferred to the caller.
     *
     * @par Leaving:
     * This method may leave.
     */
    CCalEntry* CreateItemToAgnL(const MPIMEventItem& aItem);

    /**
     * Converts from Agenda Model entry to the framework PIM item.
     *
     * @param aItem PIM Item data container store the entry data.
     * @param aEntry Agenda Model entry to convert.
     *
     * @par Leaving:
     * This method may leave.
     */
    void ReadAgnToItemL(MPIMEventItem& aItem,
                        CCalEntry& aEntry);

    /**
     * Updates the Agenda Model entry according to the framework PIM item.
     *
     * @param aItem PIM Item with the current data.
     * @param aEntry Agenda Model entry to update.
     *
     * @par Leaving:
     * This method may leave.
     */
    void UpdateItemToAgnL(const MPIMEventItem& aItem,
                          CCalEntry& aEntry);

private: // From CPIMAgnEventAdapter

    /**
     * Converts the start field of an Agenda Model entry into
     * the framework PIM item field.
     * Implements the pure virtual method from the base class.
     *
     * @param aItem Framework PIM Item.
     * @param aEntry Agenda Model entry
     *
     * @par Leaving:
     * This method may leave.
     */
    void ReadStartFromAgnL(MPIMEventItem& aItem, CCalEntry& aEntry);

    /**
     * Converts a PIM event date field to native calendar entry
     * This function supports only start field since anniversaries
     * do not support any other date fields
     * @param aItem Framework PIM Item.
     * @param aEntry Native Calendar entry
     * @param aField Date field which is to be converted
     *
     * @par Leaving:
     * This method may leave.
     */
    void ConvertDateFieldToAgnL(
        const MPIMEventItem& aItem,
        CCalEntry& aEntry,
        const TPIMEventField aField);

    /**
     * Converts the end field of an Agenda Model entry into
     * the framework PIM item field.
     * Implements the pure virtual method from the base class.
     *
     * @param aItem Framework PIM Item.
     * @param aEntry Agenda Model entry
     *
     * @par Leaving:
     * This method may leave.
     */
    void ReadEndFromAgnL(MPIMEventItem& aItem, CCalEntry& aEntry);

    /**
     * Adds default values to the native entry. This function MUST call
     * the version from the base class since it adds common values for all
     * event entries in the native database.
     *
     * @param aItem PIM API Event item
     * @param aEntry Native entry which will contain PIM data
     */
    void AddDefaultValuesToEntryL(
        const MPIMEventItem& aItem,
        CCalEntry& aEntry) const;

protected:

    void ConstructL();

private:

    /**
     * C++ constructor.
     */
    CPIMAgnAnnivAdapter(java::util::FunctionServer* aFuncServer);

};

#endif // CPIMAGNANNIVADAPTER_H
// End of file
