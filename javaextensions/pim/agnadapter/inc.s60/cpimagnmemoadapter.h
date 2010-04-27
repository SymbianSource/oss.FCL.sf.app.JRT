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
* Description:  Converts PIM Event items and native calendar day notes
 *
*/


#ifndef CPIMAGNMEMOADAPTER_H
#define CPIMAGNMEMOADAPTER_H

// INCLUDES
#include "cpimagneventadapter.h"

// FORWARD DECLARATIONS
class MPIMEventItem;

// CLASS DECLARATION
/**
 * Day note Item Adapter
 * Converts PIM Event items and native calendar day notes
 */
NONSHARABLE_CLASS(CPIMAgnMemoAdapter) : public CPIMAgnEventAdapter
{
public: // Constructor and destructor

    /**
     * Two-phased constructor.
     */
    static CPIMAgnMemoAdapter* NewL(java::util::FunctionServer* aFuncServer);

    /**
     * Destructor.
     */
    virtual ~CPIMAgnMemoAdapter();

public: // new methods

    /**
     * Constructs a Agenda Model entry from the framework PIM item.
     *
     * @param aItem The item to convert
     *
     * @return A CCalEntry constructed according to framework item.
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

private:

    /**
     * C++ constructor.
     */
    CPIMAgnMemoAdapter(java::util::FunctionServer* aFuncServer);

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
    void ReadStartFromAgnL(MPIMEventItem& aItem,
                           CCalEntry& aEntry);

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
    void ReadEndFromAgnL(MPIMEventItem& aItem,
                         CCalEntry& aEntry);

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

};

#endif // CPIMAGNMEMOADAPTER_H
// End of file
