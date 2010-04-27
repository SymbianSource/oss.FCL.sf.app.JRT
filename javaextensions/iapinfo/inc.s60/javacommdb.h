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
* Description:  JavaCommDB definition
 *
*/


#ifndef JAVACOMMDB_H
#define JAVACOMMDB_H

#include <commsdattypesv1_1.h>
#include <commsdat.h>
#include <metadatabase.h>
#include "com_nokia_mid_iapinfo_CommsTable.h"

using namespace CommsDat;

/**
 *  Class implements the native part of Java CommDB API.
 *
 *  @lib j9_23_commdb.lib
 *  @since S60 v3.0
 */
class JavaCommDB
//:
//    public CJavaPeer<JavaCommDB>

{

public:

    ~ JavaCommDB();

    /**
     * Opens the given table.
     * @since S60 v3.0
     * @param aTablename name of the CommDB table to be opened
     * @return error code
     */
    void OpenL(const TDesC&);

    /**
     * Close the table and delete allocated member objects.
     * @since S60 v3.0
     * @return error code
     */
    TInt Close();

    /**
     * Get number of records in the current table.
     * @since S60 v3.0
     * @return record count
     */
    TInt GetRecordCount();

    /**
     * Retrieves the value of the given field from the actual record.
     * The field contains integer type value.
     * @since S60 v3.0
     * @param aFieldName name of the field.
     * @return integer value of the field
     */
    TInt GetIntFieldValueL(const TDesC& aFieldName);

    /**
     * Retrieves the value of the given field from the actual record.
     * The field contains string type value.
     * @since S60 v3.0
     * @param aFieldName name of the field.
     * @return String value of the field
     */
    HBufC* GetStringFieldValueL(const TDesC& aFieldName);

    /**
     * Moves the actual record pointer to the next record.
     * @since S60 v3.0
     * @return number of the actual record
     */
    TInt Next();

    /**
     * Moves the actual record pointer to the previous record.
     * @since S60 v3.0
     * @return number of the actual record
     */
    TInt Previous();

    /**
     * Finds a record in the current table by its name.
     * The search is case sensitive.
     * If the record exists the current record will be changed to this record.
     * @since S60 v3.0
     * @param aFieldName name of the record
     * @return with the RecordId of the found record, KErrNotFound if no record found with the given name.
     */
    TInt FindByNameL(const TDesC& aFieldName);

    /**
     * Finds a record in the current table by its RecordId. If the record exists
     * the current record will be changed to this record.
     * @since S60 v3.0
     * @param aId RecordId of the record
     * @return with the RecordId of the found record, KErrNotFound if no record found with the given RecordId.
     */
    TInt FindById(TInt aId);

private:

    /**
     * Stores the number of the current record.
     */
    TInt iCurrentRecord;

    /**
     * Pointer to the actual DB Session object.
     * Initialized in the Open() function.
     */
    CMDBSession* iDb;

    /**
     * Pointer to the current RecordSet object.
     * Initialized in the Open() function.
     */
    CMDBRecordSet<CCDRecordBase>* iTable;
};

#endif // JAVACOMMDB_H

