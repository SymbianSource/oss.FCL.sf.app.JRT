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
* Description:  Handles PIM API todo item <-> Agenda Model todo conversions
 *
*/


#ifndef CPIMAGNTODOADAPTER_H
#define CPIMAGNTODOADAPTER_H

// INCLUDES
#include "cpimagnitemadapter.h"
#include "pimtodo.h"

// FORWARD DECLARATIONS
class MPIMToDoItem;
class MPIMItemData;
class CCalEntry;

// CLASS DECLARATION

/**
 * ToDo Item Adapter
 * Does conversion of Todo items between framework representation and
 * Agenda Model representation.
 */
NONSHARABLE_CLASS(CPIMAgnToDoAdapter): public CPIMAgnItemAdapter
{
public: // Constructor and destructor
    /**
     * Two-phased constructor.
     */
    static CPIMAgnToDoAdapter* NewL(java::util::FunctionServer* aFuncServer);

    /**
     * Destructor.
     */
    virtual ~CPIMAgnToDoAdapter();

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
    CCalEntry* CreateItemToAgnL(const MPIMToDoItem& aItem);

    /**
     * Converts from Agenda Model entry to the framework PIM item.
     *
     * @param aItem PIM Item data container store the entry data.
     * @param aEntry Agenda Model entry to convert.
     *
     * @par Leaving:
     * This method may leave.
     */
    void ReadAgnToItemL(MPIMToDoItem& aItem,
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
    void UpdateItemToAgnL(const MPIMToDoItem& aItem,
                          CCalEntry& aEntry);

private:

    /**
     * ExportItemL
     * Exports PIM ToDo item to native Calendar ToDo entry
     *
     * @param aItem The exported PIM item
     * @param aEntry The native entry in which the data is exported
     * @param aResetEntry Indicates that should the native entry
     *        be reseted or not
     */
    void ExportItemL(
        const MPIMToDoItem& aItem,
        CCalEntry& aEntry,
        TBool aResetEntry);

    /**
     * AddDefaultValuesToEntryL
     * Adds default values to the native entry which are required
     * by the native Calendar application
     *
     * @param aData PIM Item data
     * @param aEntry The native entry in which the data is exported
     */
    void AddDefaultValuesToEntryL(
        const MPIMItemData& aData,
        CCalEntry& aEntry) const;

    /**
     * ReadStringFieldsL
     * Read string fields from native Calendar ToDo entry and imports
     * the data to PIM API ToDo item
     *
     * @param aData PIM Item data
     * @param aEntry Calendar entry from which the data is imported
     */
    void ReadStringFieldsL(MPIMItemData& aData, CCalEntry& aEntry);

    /**
     * ReadIntFieldsL
     * Read int fields from native Calendar ToDo entry and imports
     * the data to PIM API ToDo item
     *
     * @param aData PIM Item data
     * @param aEntry Calendar entry from which the data is imported
     */
    void ReadIntFieldsL(MPIMItemData& aData, CCalEntry& aEntry);

    /**
     * ReadDateFieldsL
     * Read date fields from native Calendar ToDo entry and imports
     * the data to PIM API ToDo item
     *
     * @param aData PIM Item data
     * @param aEntry Calendar entry from which the data is imported
     */
    void ReadDateFieldsL(MPIMItemData& aData, CCalEntry& aEntry);

    /**
     * Converts the field of the framework PIM item into the field of
     * an Agenda Model entry
     *
     * @param aField PIM Item field.
     * @param aEntry Agenda Model entry.
     * @param aItem PIM Item with the current data.
     *
     * @par Leaving:
     * This method may leave.
     */
    void ConvertToAgnL(
        TPIMToDoField aField,
        CCalEntry& aEntry,
        const MPIMItemData& aItem);

    /**
     * Converts the date field of the framework PIM item into the field of
     * an Agenda Model entry
     *
     * @param aField PIM Item field.
     * @param aIndex Index of the field.
     * @param aTodo Agenda Model entry typecasted to a todo.
     * @param aItem PIM Item with the current data.
     *
     * @par Leaving:
     * This method may leave.
     */
    void ConvertDateToAgnL(
        TPIMToDoField aField,
        TInt aIndex,
        CCalEntry& aEntry,
        const MPIMItemData& aItem);

    /**
     * Converts the string field of the framework PIM item into the field of
     * an Agenda Model entry
     *
     * @param aField PIM Item field.
     * @param aIndex Index of the field.
     * @param aEntry Agenda Model entry.
     * @param aItem PIM Item with the current data.
     *
     * @par Leaving:
     * This method may leave.
     */
    void ConvertStringToAgnL(
        TPIMToDoField aField,
        TInt aIndex,
        CCalEntry& aEntry,
        const MPIMItemData& aItem);

    /**
     * Converts the int field of the framework PIM item into the field of
     * an Agenda Model entry
     *
     * @param aField PIM Item field.
     * @param aIndex Index of the field.
     * @param aTodo Agenda Model entry typecasted to a todo.
     * @param aItem PIM Item with the current data.
     *
     * @par Leaving:
     * This method may leave.
     */
    void ConvertIntToAgnL(
        TPIMToDoField aField,
        TInt aIndex,
        CCalEntry& aEntry,
        const MPIMItemData& aItem);

    /**
     * Converts the boolean field of the framework PIM item into the field
     * of an Agenda Model entry
     *
     * @param aField PIM Item field.
     * @param aIndex Index of the field.
     * @param aTodo Agenda Model entry typecasted to a todo.
     * @param aItem PIM Item with the current data.
     *
     * @par Leaving:
     * This method may leave.
     */
    void ConvertBooleanToAgnL(
        TPIMToDoField aField,
        TInt aIndex,
        CCalEntry& aEntry,
        const MPIMItemData& aItem);

    /**
     * Returns native alarm field offset in TTimeIntervalMinutes. The value
     * is calculated from PIM API alarm field value and from the difference
     * between that value and due date
     *
     * @param aItemData Rererence to item data
     * @param aEntry Native ToDo Calendar entry
     * @return Native alarm offset in minutes
     */
    TTimeIntervalMinutes AlarmOffsetL(
        const MPIMItemData& aItemData,
        CCalEntry& aEntry);

private:

    /**
     * C++ constructor.
     */
    CPIMAgnToDoAdapter(java::util::FunctionServer* aFuncServer);

};

#endif // CPIMAGNTODOADAPTER_H
// End of file
