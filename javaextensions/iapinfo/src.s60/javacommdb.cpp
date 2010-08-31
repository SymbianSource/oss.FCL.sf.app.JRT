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
* Description:  JavaCommDB class implementation
 *
*/


#include "javacommdb.h"
#include "jstringutils.h"
#include "s60commonutils.h"
#include <e32base.h>
#include <e32debug.h>
#include "logger.h"
#include "javajniutils.h"

#include "fs_methodcall.h"
#include "com_nokia_mid_iapinfo_IAPInfoImpl.h"

using namespace CommsDat;
using namespace java::util;

// CONSTANTS
_LIT(KIAPTable, "IAP");
_LIT(KConnectionPreferencesTable, "ConnectionPreferences");
_LIT(KGlobalSettingsTable, "GlobalSettings");
//_LIT( KNetworkTable, "Network" );
_LIT(KFieldNameRecordId, "RecordId");
_LIT(KFieldNameName, "Name");

JNIEXPORT jint JNICALL Java_com_nokia_mid_iapinfo_IAPInfoImpl__1createSession
(JNIEnv *, jobject)
{
    IapInfoSession *sess = NULL;
    TRAPD(err,sess =  IapInfoSession::NewL(););
    if (err!=KErrNone)
    {
        return err;
    }
    return reinterpret_cast<int>(sess);

}


JNIEXPORT void JNICALL Java_com_nokia_mid_iapinfo_CommsTable__1destroy(JNIEnv* ,
        jclass,
        jint /*aHandle*/)
{
    LOG(EJavaIapInfo, EInfo, "CJAVACOMMDB + destroy()");
    // JavaCommDB* commdb = reinterpret_cast<JavaCommDB *>(aHandle);
    //  delete commdb;
    LOG(EJavaIapInfo, EInfo, "CJAVACOMMDB - destroy()");
}


JNIEXPORT jint JNICALL Java_com_nokia_mid_iapinfo_CommsTable__1open(JNIEnv* aJni,
        jclass, jint aSessionHandle,
        jstring aTableName)
{
    LOG(EJavaIapInfo, EInfo, "CJAVACOMMDB + open()gggggggggg");
    IapInfoSession* session = reinterpret_cast<IapInfoSession *>(aSessionHandle);
    JavaCommDB *cdb = new JavaCommDB(session);
    JStringUtils table(*aJni, aTableName);
    int err = -1;
    TRAP(err,CallMethodL(cdb, &JavaCommDB::OpenL,table,cdb->iSession););
    TInt handle = reinterpret_cast<jint>(cdb);
    if (err < 0)
        handle = err;
    LOG(EJavaIapInfo, EInfo, "CJAVACOMMDB - open()hhhhhh");
    return handle;
}


JNIEXPORT void JNICALL Java_com_nokia_mid_iapinfo_CommsTable__1close(JNIEnv* ,
        jclass, jint aHandle)
{
    LOG1(EJavaIapInfo, EInfo, "CJAVACOMMDB + close() handle: %D", aHandle);
    JavaCommDB* commdb = reinterpret_cast<JavaCommDB *>(aHandle);
    CallMethod(commdb, &JavaCommDB::Close,commdb->iSession);
    //TRAP_IGNORE(commdb->Close());
    LOG(EJavaIapInfo, EInfo, "CJAVACOMMDB - close()");
}

void JavaCommDB::Destroy(JavaCommDB* aObj)
{
    delete aObj;

}

JNIEXPORT jint JNICALL Java_com_nokia_mid_iapinfo_CommsTable__1getRecordCount(JNIEnv* ,
        jclass,
        jint aHandle)
{
    LOG(EJavaIapInfo, EInfo, "CJAVACOMMDB + getRecordCount()");
    JavaCommDB* commdb = reinterpret_cast<JavaCommDB *>(aHandle);
    TInt count = 0;
    CallMethod(count, commdb, &JavaCommDB::GetRecordCount,commdb->iSession);
    //count = commdb->GetRecordCount();
    LOG1(EJavaIapInfo, EInfo, "CJAVACOMMDB returning value: %D", count);
    LOG(EJavaIapInfo, EInfo, "CJAVACOMMDB - getRecordCount()");
    return count;
}

JNIEXPORT jint JNICALL Java_com_nokia_mid_iapinfo_CommsTable__1readIntFieldValue(JNIEnv* aJni,
        jclass,
        jint aHandle,
        jstring aFieldName,
        jintArray aError)
{
    LOG(EJavaIapInfo, EInfo, "CJAVACOMMDB + readIntFieldValue()");
    TInt value = 0;

    JavaCommDB* commdb = reinterpret_cast<JavaCommDB *>(aHandle);
    JStringUtils field(*aJni, aFieldName);

    //LOG1(EJavaIapInfo,  EInfo,  "CJAVACOMMDB  reading field value: %S", field );

    TRAPD(err,CallMethodL(value, commdb, &JavaCommDB::GetIntFieldValueL,field,commdb->iSession););
    //TRAPD(err, value = commdb->GetIntFieldValueL(field));

    // Put the native error code into the Java error array
    jint javaError[1] =
        {   err};
    aJni->SetIntArrayRegion(aError, 0, 1, javaError);

    LOG1(EJavaIapInfo, EInfo, "CJAVACOMMDB  returning value: %D", value);
    LOG(EJavaIapInfo, EInfo, "CJAVACOMMDB - readIntFieldValue()");
    return value;
}

JNIEXPORT jstring JNICALL Java_com_nokia_mid_iapinfo_CommsTable__1readStringFieldValue(JNIEnv* aJni,
        jclass,
        jint aHandle,
        jstring aFieldName,
        jintArray aError)
{
    LOG(EJavaIapInfo, EInfo, "CJAVACOMMDB + readStringFieldValue()");

    HBufC* value = NULL;
    jstring str = NULL;

    JavaCommDB* commdb = reinterpret_cast<JavaCommDB *>(aHandle);
    JStringUtils field(*aJni, aFieldName);

    //LOG1(EJavaIapInfo,  EInfo,  "CJAVACOMMDB  reading field value: %S", field );
    TRAPD(err,CallMethodL(value, commdb, &JavaCommDB::GetStringFieldValueL,field,commdb->iSession););
    // TRAPD(err, value = commdb->GetStringFieldValueL(field));
    if (NULL != value && KErrNone == err)
    {
        str = S60CommonUtils::NativeToJavaString(*aJni, value->Des());
        //LOG1(EJavaIapInfo, EInfo, "CJAVACOMMDB  returning value: %S", value->Des() );
        delete value;
    }

    // Put the native error code into the Java error array
    jint javaError[1] =
        {   err};
    aJni->SetIntArrayRegion(aError, 0, 1, javaError);

    LOG(EJavaIapInfo, EInfo, "CJAVACOMMDB - readStringFieldValue()");
    return str;
}

JNIEXPORT jint JNICALL Java_com_nokia_mid_iapinfo_CommsTable__1next(JNIEnv* ,
        jclass, jint aHandle)
{
    LOG(EJavaIapInfo, EInfo, "CJAVACOMMDB + next()");
    JavaCommDB* commdb = reinterpret_cast<JavaCommDB *>(aHandle);
    TInt rec;
    CallMethod(rec, commdb, &JavaCommDB::Next,commdb->iSession);
    //TInt rec = commdb->Next();
    LOG(EJavaIapInfo, EInfo, "CJAVACOMMDB - next()");
    return rec;
}

JNIEXPORT jint JNICALL Java_com_nokia_mid_iapinfo_CommsTable__1previous(JNIEnv* ,
        jclass,
        jint /*aHandle*/)
{
    return 0;
}

JNIEXPORT jint JNICALL Java_com_nokia_mid_iapinfo_CommsTable__1findById(JNIEnv* ,
        jclass,
        jint aHandle,
        jint aId)
{
    LOG(EJavaIapInfo, EInfo, "CJAVACOMMDB + findById()");
    TInt recId = 0;

    JavaCommDB* commdb = reinterpret_cast<JavaCommDB *>(aHandle);
    CallMethod(recId, commdb, &JavaCommDB::FindById,aId,commdb->iSession);
    //recId = commdb->FindById(aId);
    LOG(EJavaIapInfo, EInfo, "CJAVACOMMDB - findById()");
    return recId;
}

JNIEXPORT jint JNICALL Java_com_nokia_mid_iapinfo_CommsTable__1findByName(JNIEnv* aJni,
        jclass,
        jint aHandle,
        jstring aName)
{
    LOG(EJavaIapInfo, EInfo, "CJAVACOMMDB + findByName()");
    TInt recId = 0;
    JStringUtils name(*aJni, aName);

    JavaCommDB* commdb = reinterpret_cast<JavaCommDB *>(aHandle);
    TRAPD(err,CallMethodL(recId, commdb, &JavaCommDB::FindByNameL,name,commdb->iSession););
    if (err < KErrNone)
        recId = err;
    //TRAP(recId, recId = commdb->FindByNameL(name));
    LOG(EJavaIapInfo, EInfo, "CJAVACOMMDB - findByName()");
    return recId;
}

/* ---------------------------------------------------------------------------
 *    JavaCommDB class implementation ;
 * ---------------------------------------------------------------------------
 */

JavaCommDB::JavaCommDB(IapInfoSession *aSession)
{
    iSession = aSession;


}

// ---------------------------------------------------------------------------
//    Delete allocated member objects.
// -----------------------------------------------------------------------------
JavaCommDB::~JavaCommDB()
{
    LOG(EJavaIapInfo, EInfo, "CJAVACOMMDB  + ~JavaCommDB()");
    Close();
    LOG(EJavaIapInfo, EInfo, "CJAVACOMMDB  - ~JavaCommDB()");
}

// ---------------------------------------------------------------------------
//    Delete allocated member objects.
// -----------------------------------------------------------------------------
void JavaCommDB::Close()
{
    LOG(EJavaIapInfo, EInfo, "CJAVACOMMDB + Close()");
    iCurrentRecord = 0;
    if (iTable)
        delete iTable;

    iTable = NULL;

    LOG(EJavaIapInfo, EInfo, "CJAVACOMMDB - Close()");

}

// ---------------------------------------------------------------------------
//    Opens the given table.
//    @param aTablename name of the CommDB table to be opened
//    @return error code
// -----------------------------------------------------------------------------
void JavaCommDB::OpenL(const TDesC& aTableName)
{
    //LOG1(EJavaIapInfo, EInfo, "CJAVACOMMDB + OpenL(): open table: %S",   aTableName);
    //iDb = CMDBSession::NewL(KCDVersion1_1);
    iCurrentRecord = 0;

    if (0 == aTableName.Compare(KIAPTable))
    {
        // Create IAP table recordset
        iTable = (CMDBRecordSet<CCDRecordBase> *)(new(ELeave) CMDBRecordSet<
                 CCDIAPRecord> (KCDTIdIAPRecord));
        LOG(EJavaIapInfo, EInfo, "CJAVACOMMDB  OpenL(): IAP table opened.");
    }
    else if (0 == aTableName.Compare(KGlobalSettingsTable))
    {
        iTable = (CMDBRecordSet<CCDRecordBase> *) new(ELeave) CMDBRecordSet<
                 CCDGlobalSettingsRecord> (KCDTIdGlobalSettingsRecord);
        LOG(EJavaIapInfo, EInfo,
            "CJAVACOMMDB  OpenL(): GlobalSettings table opened.");
    }
    else if (0 == aTableName.Compare(KConnectionPreferencesTable))
    {
        iTable = (CMDBRecordSet<CCDRecordBase> *) new(ELeave) CMDBRecordSet<
                 CCDConnectionPrefsRecord> (KCDTIdConnectionPrefsRecord);
        LOG(EJavaIapInfo, EInfo,
            "CJAVACOMMDB  OpenL(): ConnectionPreferences table opened.");
    }
    // Load the table

    iTable->LoadL(*(iSession->iDb));
    LOG(EJavaIapInfo, EInfo, "CJAVACOMMDB - OpenL()");
}

// ---------------------------------------------------------------------------
//    Get number of records in the current table.
//    @return record count
// -----------------------------------------------------------------------------
TInt JavaCommDB::GetRecordCount()
{
    return iTable->iRecords.Count();
}

// ---------------------------------------------------------------------------
//    Retrieves the value of the given field from the actual record.
//    @param aFieldName name of the field.
//    @return integer value of the field
// -----------------------------------------------------------------------------
TInt JavaCommDB::GetIntFieldValueL(const TDesC& aFieldName)
{
    TPtrC fieldName(aFieldName);
    TInt valueType;
    TUint32 value = 0;

    // If the field is the RecordId we have to use RecordId() function
    if (0 == aFieldName.Compare(KFieldNameRecordId))
    {
        value
        = (static_cast<CCDRecordBase*>(iTable->iRecords[iCurrentRecord]))->RecordId();
        LOG1(EJavaIapInfo, EInfo,
             "CJAVACOMMDB  GetIntFieldValueL(): getting RecordId: %D", value);
        return value;
    }

    // Get reference to the field by its name
    CMDBField<TUint32>
    * field =
        (CMDBField<TUint32> *)(static_cast<CCDRecordBase*>(iTable->iRecords[iCurrentRecord]))->GetFieldByNameL(
            fieldName, valueType);

    if (field->IsNull())
    {
        // The field is not initialized with a value
        LOG(EJavaIapInfo, EInfo,
            "CJAVACOMMDB  GetIntFieldValueL(): CMDBField is NULL!");
    }
    else
    {
        value = *field;
        LOG1(EJavaIapInfo, EInfo,
             "CJAVACOMMDB  GetIntFieldValueL(): getting int value: %D",
             value);
    }

    return value;
}

// ---------------------------------------------------------------------------
//    Retrieves the value of the given field from the actual record.
//    @param aFieldName name of the field.
//    @return String value of the field
// -----------------------------------------------------------------------------
HBufC* JavaCommDB::GetStringFieldValueL(const TDesC& aFieldName)
{
    TPtrC fieldName(aFieldName);
    HBufC* strValue = NULL;
    TInt valueType;

    // Get reference to the field by its name
    CMDBField<TDesC>
    * descField =
        (CMDBField<TDesC> *)(static_cast<CCDRecordBase*>(iTable->iRecords[iCurrentRecord]))->GetFieldByNameL(
            fieldName, valueType);

    if (descField->IsNull())
    {
        LOG(EJavaIapInfo, EInfo,
            "CJAVACOMMDB  GetStringFieldValueL(): CMDBField is NULL!");
    }
    else
    {
        TPtrC value = descField->GetL();
        strValue = value.AllocL();
        //LOG1(EJavaIapInfo, EInfo,"CJAVACOMMDB  GetStringFieldValue(): getting string value: %S", value);
    }

    return strValue;
}

// ---------------------------------------------------------------------------
//    Changes the actual record pointer to the next record.
//    @return number of the actual record
// -----------------------------------------------------------------------------
TInt JavaCommDB::Next()
{
    if (iCurrentRecord < GetRecordCount() - 1)
    {
        ++iCurrentRecord;
        return iCurrentRecord;
    }
    else
    {
        return KErrEof;
    }
}

// ---------------------------------------------------------------------------
//    Changes the actual record pointer to the previous record.
//    @return number of the actual record
// -----------------------------------------------------------------------------
TInt JavaCommDB::Previous()
{
    return NULL;
}

// ---------------------------------------------------------------------------
//    Finds a record in the current table by its name. The search is case
//    sensitive. If the record exists the current record will be changed to
//    this record.
//    @param aName name of the record
//    @return with the RecordId of the found record, KErrNotFound if no record
//    found with the given name.
// -----------------------------------------------------------------------------
TInt JavaCommDB::FindByNameL(const TDesC& aName)
{
    TInt ret = KErrNotFound;
    TInt count = GetRecordCount();
    TInt i = 0;
    HBufC* strValue = NULL;
    TPtrC FName(KFieldNameName);
    TInt valueType;

    while ((i < count) && (ret == KErrNotFound))
    {
        // Get reference to the field by its name
        CMDBField<TDesC>
        * descField =
            (CMDBField<TDesC> *)(static_cast<CCDRecordBase*>(iTable->iRecords[i]))->GetFieldByNameL(
                FName, valueType);

        if (!(descField->IsNull()))
        {
            TPtrC value = descField->GetL();

            strValue = value.AllocL();
            //LOG1(EJavaIapInfo, EInfo, "CJAVACOMMDB  FindByName(): getting string value: %S", value);
            if (0 == aName.Compare(strValue->Des()))
            {
                LOG(EJavaIapInfo, EInfo,
                    "CJAVACOMMDB  FindByName(): record found.");
                iCurrentRecord = i;
                ret
                = (static_cast<CCDRecordBase*>(iTable->iRecords[i])->RecordId());
            }

            delete strValue;
        }

        i++;
    }

    return ret;
}

// ---------------------------------------------------------------------------
//    Finds a record in the current table by its RecordId. If the record exists
//    the current record will be changed to this record.
//    @param aId RecordId of the record
//    @return with the RecordId of the found record, KErrNotFound if no record
//    found with the given RecordId.
// -----------------------------------------------------------------------------
TInt JavaCommDB::FindById(TInt aId)
{
    TInt ret = KErrNotFound;
    TInt count = GetRecordCount();
    TInt i = 0;

    while ((i < count) && (ret == KErrNotFound))
    {
        if (aId
                == (static_cast<CCDRecordBase*>(iTable->iRecords[i])->RecordId()))
        {
            LOG(EJavaIapInfo, EInfo, "CJAVACOMMDB  FindById(): record found.");
            iCurrentRecord = i;
            ret
            = (static_cast<CCDRecordBase*>(iTable->iRecords[i])->RecordId());
        }

        i++;
    }

    return ret;
}

