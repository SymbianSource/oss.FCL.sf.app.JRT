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
* Description:  Base class for Event item adapters
 *
*/


#ifndef CPIMAGNEVENTADAPTER_H
#define CPIMAGNEVENTADAPTER_H

// INTERNAL INCLUDES
#include "cpimagnitemadapter.h"
#include "pimevent.h"

// FORWARD DECLARATIONS
class MPIMEventItem;
class MPIMRepeatRuleData;
class CCalEntry;

// CLASS DECLARATION
/**
 * Event Item Adapter
 * Does basic conversions between PIM Event items and native calendar
 * entries. This is a base class for all Event item adapters
 */
NONSHARABLE_CLASS(CPIMAgnEventAdapter) : public CPIMAgnItemAdapter
{
public: // Destructor

    /**
     * Destructor.
     */
    virtual ~CPIMAgnEventAdapter();

public: // New abstract methods

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
    virtual CCalEntry* CreateItemToAgnL(const MPIMEventItem& aItem) = 0;

    /**
     * Converts from Agenda Model entry to the framework PIM item.
     *
     * @param aItem PIM Item data container store the entry data.
     * @param aEntry Agenda Model entry to convert.
     *
     * @par Leaving:
     * This method may leave.
     */
    virtual void ReadAgnToItemL(MPIMEventItem& aItem,
                                CCalEntry& aEntry) = 0;

    /**
     * Updates the Agenda Model entry according to the framework PIM item.
     *
     * @param aItem PIM Item with the current data.
     * @param aEntry Agenda Model entry to update.
     *
     * @par Leaving:
     * This method may leave.
     */
    virtual void UpdateItemToAgnL(const MPIMEventItem& aItem,
                                  CCalEntry& aEntry) = 0;

protected: // New pure virtual functions

    /**
     * Converts the start field of an Agenda Model entry into
     * the framework PIM item field
     *
     * @param aItem Framework PIM Item.
     * @param aEntry Agenda Model entry
     *
     * @par Leaving:
     * This method may leave.
     */
    virtual void ReadStartFromAgnL(
        MPIMEventItem& aItem,
        CCalEntry& aEntry) = 0;

    /**
     * Converts the end field of an Agenda Model entry into
     * the framework PIM item field
     *
     * @param aItem Framework PIM Item.
     * @param aEntry Agenda Model entry
     *
     * @par Leaving:
     * This method may leave.
     */
    virtual void ReadEndFromAgnL(
        MPIMEventItem& aItem,
        CCalEntry& aEntry) = 0;

protected: // New virtual functions

    /**
     * Converts a date field of a framework PIM item into
     * a native calendar entry.
     * @param aItem Framework PIM Item.
     * @param aEntry Native Calendar entry
     * @param aField Date field which is to be converted
     *
     * @par Leaving:
     * This method may leave.
     */
    virtual void ConvertDateFieldToAgnL(
        const MPIMEventItem& aItem,
        CCalEntry& aEntry,
        const TPIMEventField aField);

    /**
     * Converts a Event string field from a framework PIM item
     * into an Agenda model entry field and adds it to the entry
     *
     * @param aItem Framework PIM item
     * @param aEntry Agenda Model entry
     * @param aField PIM field which is to be converted
     */
    virtual void ConvertStringFieldToAgnL(
        const MPIMEventItem& aItem,
        CCalEntry& aEntry,
        const TPIMEventField aField) const;

    /**
     * Converts the summary field of an Agenda Model entry into
     * the framework PIM item field
     *
     * @param aItem Framework PIM Item.
     * @param aEntry Agenda Model entry
     *
     * @par Leaving:
     * This method may leave.
     */
    virtual void ReadSummaryFromAgnL(
        MPIMEventItem& aItem,
        CCalEntry& aEntry);

    /**
     * Converts the location field of an Agenda Model entry into
     * the framework PIM item field
     *
     * @param aItem Framework PIM Item.
     * @param aEntry Agenda Model entry
     *
     * @par Leaving:
     * This method may leave.
     */
    virtual void ReadLocationFromAgnL(
        MPIMEventItem& aItem,
        CCalEntry& aEntry);

    /**
     * Converts the note field of an Agenda Model entry into
     * the framework PIM item field
     *
     * @param aItem Framework PIM Item.
     * @param aEntry Agenda Model entry
     *
     * @par Leaving:
     * This method may leave.
     */
    virtual void ReadNoteFromAgnL(
        MPIMEventItem& aItem,
        CCalEntry& aEntry);

    /**
     * Converts the alarm field of an Agenda Model entry into
     * the framework PIM item field
     *
     * @param aItem Framework PIM Item.
     * @param aEntry Agenda Model entry
     *
     * @par Leaving:
     * This method may leave.
     */
    virtual void ReadAlarmFromAgnL(
        MPIMEventItem& aItem,
        CCalEntry& aEntry);

    /**
     * Converts the alarm field of a framework PIM item into
     * native calendar entry
     *
     * @param aItem Framework PIM Item.
     * @param aEntry Calendar entry
     *
     * @par Leaving:
     * This method may leave.
     */
    virtual void ConvertAlarmToAgnL(
        const MPIMEventItem& aItem,
        CCalEntry& aEntry);

    /**
     * Adds common default values for the native calendar entries
     * Base classes must call this function if they overwrite
     * this method
     *
     * @param aItem PIM API item from which the data is extracted
     * @param aEntry Native entry which will contain the data
     */
    virtual void AddDefaultValuesToEntryL(
        const MPIMEventItem& aItem,
        CCalEntry& aEntry) const;

protected: // New functions

    /**
     * Converts the fields of an Agenda Model entry into
     * the framework PIM item fields
     *
     * @param aItem Framework PIM Item.
     * @param aEntry Agenda Model entry
     * @param aFields Array of supported field types
     * @param aSize Size of the array of supported field types
     *
     * @par Leaving:
     * This method may leave.
     */
    void ReadFieldsFromAgnL(
        MPIMEventItem& aItem,
        CCalEntry& aEntry,
        const TPIMField aSupportedFields[],
        TInt aSize);

    /**
     * Converts the fields of an Agenda Model entry into
     * the framework PIM item fields
     *
     * @param aItem Framework PIM Item.
     * @param aEntry Agenda Model entry
     *
     * @par Leaving:
     * This method may leave.
     */
    void ConvertFieldsToAgnL(
        const MPIMEventItem& aItem,
        CCalEntry& aEntry);

    /**
     * Clears the fields supported by the PIM API from this
     * Agenda Model entry.
     *
     * @param aEntry Agenda Model entry
     * @param aFields Array of supported field types
     * @param aSize Size of the array of supported field types
     *
     * @par Leaving:
     * This method may leave.
     */
    void ClearPIMFieldsL(
        CCalEntry& aEntry,
        const TPIMField aSupportedFields[],
        TInt aSize);

    /**
     * Converts the repeat information of an Agenda Model entry into
     * the framework PIM item
     *
     * @param aItem Framework PIM Item.
     * @param aEntry Agenda Model entry
     *
     * @par Leaving:
     * This method may leave.
     */
    void ReadRepeatFromAgnL(
        MPIMEventItem& aItem,
        CCalEntry& aEntry);

    /**
     * Converts the repeat information of the framework PIM item into
     * an Agenda Model entry
     *
     * @param aItem Framework PIM Item.
     * @param aEntry Agenda Model entry
     *
     * @par Leaving:
     * This method may leave.
     */
    void ConvertRepeatToAgnL(
        const MPIMEventItem& aItem,
        CCalEntry& aEntry);

    /**
     * Reads entry's class details and converts them into PIM class.
     *
     * @param aItem Framework PIM Item.
     * @param aEntry Agenda Model entry
     *
     * @par Leaving:
     * This method may leave.
     */
    void ReadClassFromAgnL(
        MPIMEventItem& aItem,
        CCalEntry& aEntry);

    /**
     * Converts the class information of the framework PIM item into
     * an Agenda Model entry
     *
     * @param aItem Framework PIM Item.
     * @param aEntry Agenda Model entry
     *
     * @par Leaving:
     * This method may leave.
     */
    void ConvertClassToAgnL(
        const MPIMEventItem& aItem,
        CCalEntry& aEntry);

    /**
     * Sets the native entry alarm field.
     *
     * @param aEntry Native calendar entry
     * @param aValue Value of the alarm field
     */
    void SetAlarmToEntryL(CCalEntry& aEntry, const TInt aValue) const;

    /**
     * Checks that length of given string does not exceed
     * maximum length.
     *
     * @param aStringValue String value which is to be checked
     */
    void EnsureValidStringValueL(const TDesC& aStringValue) const;

protected: // Protected constructor

    /**
     * C++ constructor.
     */
    CPIMAgnEventAdapter(java::util::FunctionServer* aFuncServer);

};

#endif // CPIMAGNEVENTADAPTER_H
// End of file
