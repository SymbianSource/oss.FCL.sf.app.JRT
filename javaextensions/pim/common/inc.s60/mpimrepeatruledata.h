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
* Description:  Access interface to RepeatRule data representation.
 *
*/


#ifndef MPIMREPEATRULEDATA_H
#define MPIMREPEATRULEDATA_H

//  INCLUDES
#include <e32base.h>
#include "pimcommon.h"
#include "pimrepeatrule.h"

//  FORWARD DECLARATIONS
class MPIMItemData;

/**
 *  Interface for accessing the repeat rule data associated with an item.
 *  See \ref MPIMEventItemData class.
 */
class MPIMRepeatRuleData
{
public:
    // Destructor

    /**
     * Destructor.
     */
    virtual ~MPIMRepeatRuleData()
    {
    }

public:
    // New functions

    /**
     * Add an exception date. If the date has a time portion, it is ignored
     * and also cleared when the item is committed to the Agenda Model.
     * Note that even it is possible to set an exception date that
     * is outside the agenda model's valid date range
     * (from 1st January 1980 to 31st December 2100), when committing an
     * item with such a repeat rule into the agenda model those non-valid
     * exception dates are discarded.
     *
     * @param aDate Exception date.
     *
     * @par Leaving:
     * The method leaves on error. Error codes should be interpreted as
     * follows:
     * @li Any - The date could not be added.
     */
    virtual void AddExceptDateL(TPIMDate aDate) = 0;

    /**
     * Remove an exception date.
     *
     * @param aDate Exception date.
     *
     * @par Leaving:
     * The method leaves on error. Error codes should be interpreted as
     * follows:
     * @li Any - The date could not be removed.
     */
    virtual void RemoveExceptDate(TPIMDate aDate) = 0;

    /**
     * Provides all exception dates set for this repeat rule.
     * The ownership of the array stays with the repeat rule.
     *
     * @return Array of exception dates.
     *
     * @par Leaving:
     * The method leaves on error. Error codes should be interpreted as
     * follows:
     * @li Any - The array could not be created.
     */
    virtual const CArrayFix<TPIMDate>& GetExceptDatesL() const = 0;

    /**
     * Gets the value in an integer field.
     *
     * @param aField The field.
     *
     * @return Integer value in \a aField.
     *
     * @par Leaving:
     * The method leaves on error. Error codes should be interpreted as
     * follows:
     * @li \c KErrArgument - \a aField is not valid.
     * @li \c KErrNotFound - \a aField has no value.
     */
    virtual TInt GetIntL(TPIMField aField) const = 0;

    /**
     * Sets the value of an integer field.
     *
     * @param aField The field.
     * @param aValue The value to be set for \a aField.
     *
     * @par Leaving:
     * The method leaves on error. Error codes should be interpreted as
     * follows:
     * @li \c KErrArgument - \a aField is not valid.
     * @li \c KErrTooBig - \a aValue is not valid for the field.
     * @li Other - The value could not be set.
     */
    virtual void SetIntL(TPIMField aField, TInt aValue) = 0;

    /**
     * Gets the value of a date field.
     *
     * @param aField The field.
     *
     * @return Date value in \a aField.
     *
     * @par Leaving:
     * The method leaves on error. Error codes should be interpreted as
     * follows:
     * @li \c KErrArgument - \a aField is not valid.
     * @li \c KErrNotFound - \a aField has no value.
     */
    virtual TPIMDate GetDateL(TPIMField aField) const = 0;

    /**
     * Sets the value of a date field.
     * Note that the given dates have to be in the agenda model's valid
     * date range (from 1st January 1980 to 31st December 2100).
     *
     * @param aField The field.
     * @param aDate The value to be set for \a aField.
     *
     * @par Leaving:
     * The method leaves on error. Error codes should be interpreted as
     * follows:
     * @li \c KErrArgument - \a aField is not valid.
     * @li \c KErrTooBig - \a aValue is not valid for the field.
     * @li Other - The value could not be set.
     */
    virtual void SetDateL(TPIMField aField, TPIMDate aDate) = 0;

    /**
     * Provides an array of fields currently in use.
     * The ownership of the array is transferred to the caller.
     *
     * @return An array of fields.
     * The \b ownership of the array is transferred to the caller.
     *
     * @par Leaving:
     * The method leaves on error. Error codes should be interpreted as
     * follows:
     * @li Any - The array could not be created.
     */
    virtual CArrayFix<TPIMField>* GetFieldsL() const = 0;

    /**
     * Clears a field. The value in that field is discarded.
     *
     * @param aField The field.
     *
     * @par Leaving:
     * The method leaves on error. Error codes should be interpreted as
     * follows:
     * @li \c KErrArgument - \a aField is not valid.
     */
    virtual void ClearFieldL(TPIMField aField) = 0;

    /**
     * Clears all fields. The values in all fields are discarded.
     */
    virtual void clear() = 0;

    /**
     * Copies all the fields from the Agenda model's Repeat definition.
     */
    virtual void BackupRepeatL() = 0;

    /**
     * Checks if the Repeat rule has been changed or not.
     */
    virtual TBool IsModified() const = 0;

protected:
    // Non-public operations

    // Allow derivation with protected default constructor.
    MPIMRepeatRuleData()
    {
    }

private:
    // Prohibited operations
    // Prohibit copy constructor.
    MPIMRepeatRuleData(const MPIMRepeatRuleData&)
    {
    }

    // Prohibit assigment operator.
    MPIMRepeatRuleData& operator=(const MPIMRepeatRuleData&)
    {
        return *this;
    }

};

#endif // MPIMREPEATRULEDATA_H
// End of File
