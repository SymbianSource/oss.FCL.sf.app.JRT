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
* Description:  Converts PIM Event items and native calendar appointments
 *
*/


#ifndef CPIMAGNAPPTADAPTER_H
#define CPIMAGNAPPTADAPTER_H

// INCLUDES
#include "cpimagneventadapter.h"

// FORWARD DECLARATIONS
class MPIMEventItem;

// CLASS DECLARATION
/**
 * Appointment Item Adapter
 * Converts PIM Event items and native calendar appointments
 */
NONSHARABLE_CLASS(CPIMAgnApptAdapter): public CPIMAgnEventAdapter
{
public: // Constructor and destructor

    /**
     * Two-phased constructor.
     */
    static CPIMAgnApptAdapter* NewL(java::util::FunctionServer* aFuncServer);

    /**
     * Destructor.
     */
    virtual ~CPIMAgnApptAdapter();

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
    CPIMAgnApptAdapter(java::util::FunctionServer* aFuncServer);

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
     * Adds common default values for the native calendar entries
     * Base classes must call this function if they overwrite
     * this method
     *
     * @param aItem PIM API item from which the data is extracted
     * @param aEntry Native entry which will contain the data
     */
    void AddDefaultValuesToEntryL(
        const MPIMEventItem& aItem,
        CCalEntry& aEntry) const;

protected:

    void ConstructL();
};

#endif // CPIMAGNAPPTADAPTER_H
// End of file
