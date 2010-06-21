/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Implementation of CStorageBackupUtil
*
*/


#include "javastoragebackuputil.h"
#include "midp2backupplugin.h"
#include "midp2backupdataids.h"

#include "javastorageentry.h"
#include "javastorage.h"
#include "javastoragenames.h"

#include "logger.h"
#include "javauid.h"
#include "javaoslayer.h"
#include "javacommonutils.h"
#include "s60commonutils.h"
#include "javasymbianoslayer.h"

#include <memory>
#include <s32mem.h>

using namespace std;
using namespace java::storage;
using namespace java::backup;

// ======== MEMBER FUNCTIONS ========

CStorageBackupUtil::CStorageBackupUtil()
{
    LOG(EBackup, EInfo, "CStorageBackupUtil constructor");
}

void CStorageBackupUtil::ConstructL()
{
    LOG(EBackup, EInfo, "CStorageBackupUtil::ConstructL");

    iFirstCalltoBackupStorageData = ETrue;
    iFirstCalltoRestoreStorageData = ETrue;

    iBufferSpaceLeft = 0;

    iRemainingString = EFalse;
    iStrCount = 0;

    for (int tableNumber = 0; tableNumber < NUMBER_OF_TABLES; tableNumber++)
    {
        iTableSize[tableNumber] = 0;
    }
}

CStorageBackupUtil* CStorageBackupUtil::NewL()
{
    LOG(EBackup, EInfo, "CStorageBackupUtil::NewL");

    CStorageBackupUtil* self = new(ELeave) CStorageBackupUtil();
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop();

    return self;
}

CStorageBackupUtil::~CStorageBackupUtil()
{
    LOG(EBackup, EInfo, "CStorageBackupUtil Destructor");

    // clear the vectors to free all the heap data.
    iStringVector.clear();
}


void CStorageBackupUtil::BackupStorageDataL(RDesWriteStream& aStream, TBool& aBackupNotFinished, TInt& aBufferSpaceLeft)
{
    LOG(EBackup, EInfo, "CStorageBackupUtil::BackupStorageDataL");

    iBufferSpaceLeft = aBufferSpaceLeft;

    if (iFirstCalltoBackupStorageData)
    {
        int err = FillVectorWithStorageData();

        if (err != KErrNone)
        {
            User::Leave(err);
        }
        LOG1(EBackup, EInfo, "Total no of rows in vector: %d", iStringVector.size());

        // First write the total no of rows in the vector to the stream

        aStream.WriteInt32L(iStringVector.size());
        iBufferSpaceLeft -= sizeof(TInt32);

        /* Then write the number of rows in each table to the stream.
           This will be used while writing the data to storage. */

        for (int tableNumber = 0; tableNumber < NUMBER_OF_TABLES; tableNumber++)
        {
            aStream.WriteInt16L(iTableSize[tableNumber]);
            iBufferSpaceLeft -= sizeof(TInt16);
        }

        iFirstCalltoBackupStorageData = EFalse;
    }

    // Now write the actual string data into the stream.

    while (iBufferSpaceLeft > 0 && iStrCount < iStringVector.size())
    {
        WriteStringtoStreamL(aStream, iStringVector[iStrCount]);
        LOG1(EBackup, EInfo, "StrCount = %d", iStrCount);
    }

    if (iStrCount >= iStringVector.size())
    {
        LOG(EBackup, EInfo, "Backup of storage data finished");
        aBackupNotFinished = EFalse;
    }
}

void CStorageBackupUtil::RestoreStorageDataL(RDesReadStream& aStream, TInt& aRestoreState, TInt& aBufferSpaceLeft)
{
    LOG(EBackup, EInfo, "CStorageBackupUtil::RestoreStorageDataL()");

    iBufferSpaceLeft = aBufferSpaceLeft;

    if (iFirstCalltoRestoreStorageData)
    {
        // the no of strings in the stream
        iStrCount = aStream.ReadInt32L();
        iBufferSpaceLeft -= sizeof(TInt32);

        // read the number of rows in each table
        for (int tableNumber = 0; tableNumber < NUMBER_OF_TABLES; tableNumber++)
        {
            iTableSize[tableNumber] = aStream.ReadInt16L();
            iBufferSpaceLeft -= sizeof(TInt16);
        }
        iFirstCalltoRestoreStorageData = EFalse;
    }

    while (iBufferSpaceLeft > 0 && iStrCount > 0)
    {
        ReadStringfromStreamL(aStream);
    }

    if (iStrCount == 0)
    {
        LOG1(EBackup, EInfo, "Finished reading from stream, row count = %d", iStringVector.size());


        int err = WriteDataToStorage();

        if (err != KErrNone)
        {
            CleanupStack::PopAndDestroy(&aStream);
            User::Leave(KErrGeneral);
        }

        // Storage restore is over; Set state to EAppArc
        aRestoreState = EAppArc;
        aBufferSpaceLeft = iBufferSpaceLeft;
    }
}


void CStorageBackupUtil::WriteStringtoStreamL(RDesWriteStream& aStream, wstring aStr)
{
    iLenOfString = aStr.length();

    // if length of string is 0, do not write any string to the stream.
    if (iLenOfString == 0)
    {
        aStream.WriteInt16L(iLenOfString*2);
        iBufferSpaceLeft -= sizeof(TInt16);
        iStrCount++;
    }

    else
    {
        /* if space is not enough for writing the complete string,
           do not write it. Could be written next time.  */
        if (((iLenOfString*2) + sizeof(TInt16)) > iBufferSpaceLeft)
        {
            LOG(EBackup, EInfo, "Stream size is not enough to hold the string");
            // set the bufferspaceleft to zero
            iBufferSpaceLeft = 0;
        }
        // stream has enough space for the length and the string data.
        else
        {
            aStream.WriteInt16L(iLenOfString*2);
            iBufferSpaceLeft -= sizeof(TInt16);

            HBufC* tempstr = java::util::S60CommonUtils::wstringToDes(aStr.c_str());
            TPtrC tempStr = tempstr->Des();
            aStream.WriteL(tempStr);
            iBufferSpaceLeft -= (iLenOfString*2);
            delete tempstr;

            iStrCount++;
        }
    }
}

void CStorageBackupUtil::ReadStringfromStreamL(RDesReadStream& aStream)
{
    wstring emptyString;

    /* if only part of the string was read last time,
       read the remaining and append it to that string.  */
    if (iRemainingString)
    {
        // if the whole string cannot be read, read part of it and store it.
        if (iLenOfString > iBufferSpaceLeft)
        {
            LOG(EBackup, EInfo, "Cant read the full string; read only part of it");

            HBufC* data = HBufC::NewL(iBufferSpaceLeft/2+1);
            TPtr pdata = data->Des();

            aStream.ReadL(pdata, iBufferSpaceLeft/2);
            iLenOfString -= iBufferSpaceLeft;
            iBufferSpaceLeft = 0;

            wchar_t* str = desToWstring(pdata);
            wstring stringData(str);
            iHalfReadString += stringData;

            delete data;
            iRemainingString = ETrue;
        }
        // else read the complete string
        else
        {
            HBufC* data = HBufC::NewL(iLenOfString/2+1);
            TPtr pdata = data->Des();

            aStream.ReadL(pdata, iLenOfString/2);
            iBufferSpaceLeft -= (iLenOfString);

            wchar_t* str = desToWstring(pdata);
            wstring stringData(str);
            iHalfReadString += stringData;

            iStringVector.push_back(iHalfReadString);

            delete data;
            iStrCount--;
            iRemainingString = EFalse;
            iHalfReadString = emptyString;
        }
    }

    else
    {
        iLenOfString = aStream.ReadInt16L();
        iBufferSpaceLeft -= sizeof(TInt16);

        if (iLenOfString > 0)
        {
            // if full string cannot be read, read only part of it.
            if (iLenOfString > iBufferSpaceLeft)
            {
                /* if the stream does not have even part of the string,
                   just set the iRemainingFlag to ETrue so that the
                   string can be read next time. */

                if (iBufferSpaceLeft == 0)
                {
                    iRemainingString = ETrue;
                    iHalfReadString = emptyString;
                }
                // else stream contains part of the stream. Read it
                else
                {
                    LOG(EBackup, EInfo, "Cant read the full string; read only part of it");

                    HBufC* data = HBufC::NewL(iBufferSpaceLeft/2+1);
                    TPtr pdata = data->Des();

                    aStream.ReadL(pdata, iBufferSpaceLeft/2);
                    iLenOfString -= iBufferSpaceLeft;
                    iBufferSpaceLeft = 0;

                    wchar_t* str = desToWstring(pdata);
                    wstring stringData(str);
                    iHalfReadString = stringData;
                    delete data;
                    iRemainingString = ETrue;
                }
            }
            // else full string can be read
            else
            {
                HBufC* data = HBufC::NewL(iLenOfString/2+1);
                TPtr pdata = data->Des();

                aStream.ReadL(pdata, iLenOfString/2);
                iBufferSpaceLeft -= (iLenOfString);

                wchar_t* str = desToWstring(pdata);
                wstring stringData(str);

                iStringVector.push_back(stringData);
                iStrCount--;
                delete data;
            }
        }
        /* if length of string is 0, do not read anything from the stream;
           just push an empty string into the vector */
        else
        {
            iStringVector.push_back(emptyString);
            iStrCount--;
        }
    }
}


int CStorageBackupUtil::FillVectorWithStorageData()
{
    // get data from storage

    LOG(EBackup, EInfo, "CStorageBackupUtil::FillVectorsWithData");

    auto_ptr<JavaStorage> js(JavaStorage::createInstance());
    LOG(EBackup, EInfo, "Opening connection to JAVA_DATABASE");

    try
    {
        js->open(JAVA_DATABASE_NAME);
        LOG(EBackup, EInfo, "Opening commection succeeded");
    }
    catch (JavaStorageException jse)
    {
        ELOG1(EBackup, "Opening JAVA_DATABASE failed; %S", jse.toString().c_str());
        return KErrGeneral;
    }

    JavaStorageEntry attribute;
    JavaStorageApplicationEntry_t findPattern;
    JavaStorageApplicationList_t foundEntries;


    // table 1 : Application package table
    {
        LOG(EBackup, EInfo, "Read entries from APPLICATION_PACKAGE_TABLE");

        try
        {
            js->search(APPLICATION_PACKAGE_TABLE, findPattern, foundEntries);
            LOG(EBackup, EInfo, "Search in Java Storage succeded");
        }
        catch (JavaStorageException jse)
        {
            ELOG1(EBackup, "Search in Java Storage failed; %S", jse.toString().c_str());
            js->close();
            return KErrGeneral;
        }

        iTableSize[0] = FillVectorwithAppPackageTableData(foundEntries);
        foundEntries.clear();

        if (iTableSize[0] == 0)
        {
            LOG(EBackup, EInfo,  "No data in Storage; So no midlets in system;");
            return KErrGeneral;
        }
    }


    // table 2: Application table
    {
        LOG(EBackup, EInfo,  "Read entries from APPLICATION_TABLE");

        try
        {
            js->search(APPLICATION_TABLE, findPattern, foundEntries);
            LOG(EBackup, EInfo,  "Search in Java Storage succeded");
        }
        catch (JavaStorageException jse)
        {
            ELOG1(EBackup, "Search in Java Storage failed; %S", jse.toString().c_str());
            js->close();
            return KErrGeneral;
        }

        iTableSize[1] = FillVectorwithAppTableData(foundEntries);

        foundEntries.clear();
    }

    // table 3: Application package attributes table

    /* for this table, since there are very large number of rows,
       read the data from the table suite-by-suite
       ie. read entries for the first suite, then read entries for the
       second suite. This would prevent stack overflow  */
    {
        LOG(EBackup, EInfo,  "Read entries from APPLICATION_PACKAGE_ATTRIBUTES_TABLE");

        int rowCount = 0;

        for (int suiteNumber = 0; suiteNumber < iTableSize[0]; suiteNumber++)
        {
            attribute.setEntry(ID, iStringVector[suiteNumber*14]);
            findPattern.insert(attribute);

            try
            {
                js->search(APPLICATION_PACKAGE_ATTRIBUTES_TABLE, findPattern, foundEntries);
                LOG(EBackup, EInfo,  "Search in Java Storage succeded");
            }
            catch (JavaStorageException jse)
            {
                ELOG1(EBackup, "Search in Java Storage failed; %S", jse.toString().c_str());
                js->close();
                return KErrGeneral;
            }

            rowCount += FillVectorwithAppPackageAttTableData(foundEntries);

            foundEntries.clear();
            findPattern.clear();
        }
        iTableSize[2] = rowCount;
    }

    // table 4: Midp package table
    {
        LOG(EBackup, EInfo,  "Read entries from MIDP_PACKAGE_TABLE");

        try
        {
            js->search(MIDP_PACKAGE_TABLE, findPattern, foundEntries);
            LOG(EBackup, EInfo,  "Search in Java Storage succeded");
        }
        catch (JavaStorageException jse)
        {
            ELOG1(EBackup, "Search in Java Storage failed; %S", jse.toString().c_str());
            js->close();
            return KErrGeneral;
        }

        iTableSize[3] = FillVectorwithMidpPackageTableData(foundEntries);

        foundEntries.clear();
    }

    // table 5: Midp permissions table

    /* for this table, since there are very large number of rows,
       read the data from the table suite-by-suite
       ie. read entries for the first suite, then read entries for the
       second suite. This would prevent stack overflow  */
    {
        LOG(EBackup, EInfo,  "Read entries from MIDP_PERMISSIONS_TABLE");

        int rowCount = 0;

        for (int suiteNumber = 0; suiteNumber < iTableSize[0]; suiteNumber++)
        {
            attribute.setEntry(ID, iStringVector[suiteNumber*14]);
            findPattern.insert(attribute);

            try
            {
                js->search(MIDP_PERMISSIONS_TABLE, findPattern, foundEntries);
                LOG(EBackup, EInfo,  "Search in Java Storage succeded");
            }
            catch (JavaStorageException jse)
            {
                ELOG1(EBackup, "Search in Java Storage failed; %S", jse.toString().c_str());
                js->close();
                return KErrGeneral;
            }

            rowCount += FillVectorwithMidpPermTableData(foundEntries);

            foundEntries.clear();
            findPattern.clear();
        }
        iTableSize[4] = rowCount;
    }

    // table 6: Midp function group settings table
    {
        LOG(EBackup, EInfo,  "Read entries from MIDP_FUNC_GRP_SETTINGS_TABLE");

        try
        {
            js->search(MIDP_FUNC_GRP_SETTINGS_TABLE, findPattern, foundEntries);
            LOG(EBackup, EInfo,  "Search in Java Storage succeded");
        }
        catch (JavaStorageException jse)
        {
            ELOG1(EBackup, "Search in Java Storage failed; %S", jse.toString().c_str());
            js->close();
            return KErrGeneral;
        }

        iTableSize[5] = FillVectorwithMidpFuncGrpSetTableData(foundEntries);

        foundEntries.clear();
    }

    // table 7: push registration table
    {
        LOG(EBackup, EInfo,  "Read entries from PUSH_REGISTRATIONS_TABLE");

        try
        {
            js->search(PUSH_REGISTRATIONS_TABLE, findPattern, foundEntries);
            LOG(EBackup, EInfo,  "Search in Java Storage succeded");
        }
        catch (JavaStorageException jse)
        {
            ELOG1(EBackup, "Search in Java Storage failed; %S", jse.toString().c_str());
            js->close();
            return KErrGeneral;
        }

        iTableSize[6] = FillVectorwithPushRegTableData(foundEntries);

        foundEntries.clear();
    }

    // table 8: alarm registration table
    {
        LOG(EBackup, EInfo,  "Read entries from ALARM_REGISTRATIONS_TABLE");

        try
        {
            js->search(ALARM_REGISTRATIONS_TABLE, findPattern, foundEntries);
            LOG(EBackup, EInfo,  "Search in Java Storage succeded");
        }
        catch (JavaStorageException jse)
        {
            ELOG1(EBackup, "Search in Java Storage failed; %S", jse.toString().c_str());
            js->close();
            return KErrGeneral;
        }

        iTableSize[7] = FillVectorwithAlarmRegTableData(foundEntries);

        foundEntries.clear();
    }

    // table 9: runtime settings table
    {
        LOG(EBackup, EInfo,  "Read entries from RUNTIME_SETTINGS_TABLE");

        try
        {
            js->search(RUNTIME_SETTINGS_TABLE, findPattern, foundEntries);
            LOG(EBackup, EInfo,  "Search in Java Storage succeded");
        }
        catch (JavaStorageException jse)
        {
            ELOG1(EBackup, "Search in Java Storage failed; %S", jse.toString().c_str());
            js->close();
            return KErrGeneral;
        }

        iTableSize[8] = FillVectorwithRuntimeSetTableData(foundEntries);

        foundEntries.clear();
    }

    // table 10: pre-install table
    {
        LOG(EBackup, EInfo,  "Read entries from PREINSTALL_TABLE");

        try
        {
            js->search(PREINSTALL_TABLE, findPattern, foundEntries);
            LOG(EBackup, EInfo,  "Search in Java Storage succeded");
        }
        catch (JavaStorageException jse)
        {
            ELOG1(EBackup, "Search in Java Storage failed; %S", jse.toString().c_str());
            js->close();
            return KErrGeneral;
        }

        iTableSize[9] = FillVectorwithPreinstallTableData(foundEntries);

        foundEntries.clear();
    }

    // Data read from JAVA_DATABASE; So close the connection
    js->close();

    // table 11: ota status table
    /* Stored in another database called javaotadatabase.
       so open a new connection to that database.  */
    {
        LOG(EBackup, EInfo,  "Read entries from OTA_STATUS_TABLE");

        auto_ptr<JavaStorage> jos(JavaStorage::createInstance());

        try
        {
            jos->open(JAVA_OTA_DATABASE_NAME);
            ILOG(EBackup, "Opening connection to OTA database succeeded");
        }
        catch (JavaStorageException jse)
        {
            ELOG1(EBackup, "Opening OTA database failed: %s", jse.toString().c_str());
            return KErrGeneral;
        }

        try
        {
            jos->search(OTA_STATUS_TABLE, findPattern, foundEntries);
            LOG(EBackup, EInfo,  "Search in Java Storage succeded");
        }
        catch (JavaStorageException jse)
        {
            ELOG1(EBackup, "Search in Java Storage failed; %s", jse.toString().c_str());
            jos->close();
            return KErrGeneral;
        }

        iTableSize[10] = FillVectorwithOtaStatusTableData(foundEntries);

        foundEntries.clear();
        jos->close();
    }

    //################ printing the vectors #############
    //printVector();
    //###################################################

    ILOG(EBackup, "Vector filled with data");
    return KErrNone;
}


int CStorageBackupUtil::WriteDataToStorage()
{
    JELOG2(EBackup);
    ILOG(EBackup, "Create instance of JAVA_DATABASE_NAME and open the database");

    auto_ptr<JavaStorage> js(JavaStorage::createInstance());

    try
    {
        js->open(JAVA_DATABASE_NAME);
        ILOG(EBackup, "Opening database succeded");
    }
    catch (JavaStorageException jse)
    {
        ELOG1(EBackup, "Opening database failed: %S", jse.toString().c_str());
        return KErrGeneral;
    }

    try
    {
        js->startTransaction();
        ILOG(EBackup, "Transaction started");
    }
    catch (JavaStorageException jse)
    {
        ELOG1(EBackup, "startTransaction() failed: %S", jse.toString().c_str());
        js->close();
        return KErrGeneral;
    }

    ILOG(EBackup, "Before writing data to storage, existing data must be cleared");

    // call delete * from table one-by-one
    {
        JavaStorageApplicationEntry_t emptyEntry;

        try
        {
            js->remove(APPLICATION_PACKAGE_TABLE, emptyEntry);
            js->remove(APPLICATION_TABLE, emptyEntry);
            js->remove(APPLICATION_PACKAGE_ATTRIBUTES_TABLE, emptyEntry);
            js->remove(MIDP_PACKAGE_TABLE, emptyEntry);
            js->remove(MIDP_PERMISSIONS_TABLE, emptyEntry);
            js->remove(MIDP_FUNC_GRP_SETTINGS_TABLE, emptyEntry);
            js->remove(PUSH_REGISTRATIONS_TABLE, emptyEntry);
            js->remove(ALARM_REGISTRATIONS_TABLE, emptyEntry);
            js->remove(RUNTIME_SETTINGS_TABLE, emptyEntry);
            js->remove(PREINSTALL_TABLE, emptyEntry);

            ILOG(EBackup, "Data removed successfully from table");
        }
        catch (JavaStorageException jse)
        {
            ELOG1(EBackup, "Failed during removal of entries from table: %S", jse.toString().c_str());
            js->close();
            return KErrGeneral;
        }
    }

    JavaStorageEntry attribute;
    JavaStorageApplicationEntry_t insertEntry;

    ILOG(EBackup, "Start transaction for writing into the database");

    int count = 0;
    // table 1 : Application package table
    {
        LOG(EBackup, EInfo,  "Writing to APPLICATION_PACKAGE_TABLE");

        for (int rowNumber = 0; rowNumber < iTableSize[0]; rowNumber++)
        {
            attribute.setEntry(ID, iStringVector[count++]);
            insertEntry.insert(attribute);

            attribute.setEntry(PACKAGE_NAME, iStringVector[count++]);
            insertEntry.insert(attribute);

            attribute.setEntry(VENDOR, iStringVector[count++]);
            insertEntry.insert(attribute);

            attribute.setEntry(VERSION, iStringVector[count++]);
            insertEntry.insert(attribute);

            attribute.setEntry(ROOT_PATH, iStringVector[count++]);
            insertEntry.insert(attribute);

            attribute.setEntry(MEDIA_ID, iStringVector[count++]);
            insertEntry.insert(attribute);

            attribute.setEntry(INITIAL_SIZE, iStringVector[count++]);
            insertEntry.insert(attribute);

            attribute.setEntry(JAD_PATH, iStringVector[count++]);
            insertEntry.insert(attribute);

            attribute.setEntry(JAR_PATH, iStringVector[count++]);
            insertEntry.insert(attribute);

            attribute.setEntry(JAD_URL, iStringVector[count++]);
            insertEntry.insert(attribute);

            attribute.setEntry(JAR_URL, iStringVector[count++]);
            insertEntry.insert(attribute);

            attribute.setEntry(ACCESS_POINT, iStringVector[count++]);
            insertEntry.insert(attribute);

            attribute.setEntry(CONTENT_INFO, iStringVector[count++]);
            insertEntry.insert(attribute);

            attribute.setEntry(CONTENT_ID, iStringVector[count++]);
            insertEntry.insert(attribute);

            try
            {
                js->write(APPLICATION_PACKAGE_TABLE, insertEntry);
                ILOG(EBackup, "Writing to table succeded");
            }
            catch (JavaStorageException jse)
            {
                ELOG1(EBackup, "Writing to table failed: %S", jse.toString().c_str());
                js->close();
                return KErrGeneral;
            }
            insertEntry.clear();
        }
    }

    // table 2: Application table
    {
        LOG(EBackup, EInfo,  "Writing to APPLICATION_TABLE");

        for (int rowNumber = 0; rowNumber < iTableSize[1]; rowNumber++)
        {
            attribute.setEntry(ID, iStringVector[count++]);
            insertEntry.insert(attribute);

            attribute.setEntry(PACKAGE_ID, iStringVector[count++]);
            insertEntry.insert(attribute);

            attribute.setEntry(NAME, iStringVector[count++]);
            insertEntry.insert(attribute);

            attribute.setEntry(MAIN_CLASS, iStringVector[count++]);
            insertEntry.insert(attribute);

            attribute.setEntry(AUTORUN, iStringVector[count++]);
            insertEntry.insert(attribute);

            try
            {
                js->write(APPLICATION_TABLE, insertEntry);
                ILOG(EBackup, "Writing to table succeded");
            }
            catch (JavaStorageException jse)
            {
                ELOG1(EBackup, "Writing to table failed: %S", jse.toString().c_str());
                js->close();
                return KErrGeneral;
            }
            insertEntry.clear();
        }
    }

    // table 3: Application package attributes table
    {
        LOG(EBackup, EInfo,  "Writing to APPLICATION_PACKAGE_ATTRIBUTES_TABLE");

        for (int rowNumber = 0; rowNumber < iTableSize[2]; rowNumber++)
        {
            attribute.setEntry(ID, iStringVector[count++]);
            insertEntry.insert(attribute);

            attribute.setEntry(NAME, iStringVector[count++]);
            insertEntry.insert(attribute);

            attribute.setEntry(VALUE, iStringVector[count++]);
            insertEntry.insert(attribute);

            attribute.setEntry(TRUSTED, iStringVector[count++]);
            insertEntry.insert(attribute);

            try
            {
                js->write(APPLICATION_PACKAGE_ATTRIBUTES_TABLE, insertEntry);
                ILOG(EBackup, "Writing to table succeded");
            }
            catch (JavaStorageException jse)
            {
                ELOG1(EBackup, "Writing to table failed: %S", jse.toString().c_str());
                js->close();
                return KErrGeneral;
            }
            insertEntry.clear();
        }
    }

    // table 4: Midp package table
    {
        LOG(EBackup, EInfo,  "Writing to MIDP_PACKAGE_TABLE");

        for (int rowNumber = 0; rowNumber < iTableSize[3]; rowNumber++)
        {
            attribute.setEntry(ID, iStringVector[count++]);
            insertEntry.insert(attribute);

            attribute.setEntry(TYPE, iStringVector[count++]);
            insertEntry.insert(attribute);

            attribute.setEntry(SECURITY_DOMAIN, iStringVector[count++]);
            insertEntry.insert(attribute);

            attribute.setEntry(SECURITY_DOMAIN_CATEGORY, iStringVector[count++]);
            insertEntry.insert(attribute);

            attribute.setEntry(HASH, iStringVector[count++]);
            insertEntry.insert(attribute);

            attribute.setEntry(CERT_HASH, iStringVector[count++]);
            insertEntry.insert(attribute);

            attribute.setEntry(RMS, iStringVector[count++]);
            insertEntry.insert(attribute);

            attribute.setEntry(VALID_CERTS, iStringVector[count++]);
            insertEntry.insert(attribute);

            attribute.setEntry(ON_SCREEN_KEYPAD, iStringVector[count++]);
            insertEntry.insert(attribute);
            
            attribute.setEntry(SECURITY_WARNINGS, iStringVector[count++]);
            insertEntry.insert(attribute);

            try
            {
                js->write(MIDP_PACKAGE_TABLE, insertEntry);
                ILOG(EBackup, "Writing to table succeded");
            }
            catch (JavaStorageException jse)
            {
                ELOG1(EBackup, "Writing to table failed: %S", jse.toString().c_str());
                js->close();
                return KErrGeneral;
            }
            insertEntry.clear();
        }
    }

    // table 5: Midp permissions table
    {
        LOG(EBackup, EInfo,  "Writing to MIDP_PERMISSIONS_TABLE");

        for (int rowNumber = 0; rowNumber < iTableSize[4]; rowNumber++)
        {
            attribute.setEntry(ID, iStringVector[count++]);
            insertEntry.insert(attribute);

            attribute.setEntry(CLASS, iStringVector[count++]);
            insertEntry.insert(attribute);

            attribute.setEntry(NAME, iStringVector[count++]);
            insertEntry.insert(attribute);

            attribute.setEntry(ACTION, iStringVector[count++]);
            insertEntry.insert(attribute);

            attribute.setEntry(FUNCTION_GROUP, iStringVector[count++]);
            insertEntry.insert(attribute);

            try
            {
                js->write(MIDP_PERMISSIONS_TABLE, insertEntry);
                ILOG(EBackup, "Writing to table succeded");
            }
            catch (JavaStorageException jse)
            {
                ELOG1(EBackup, "Writing to table failed: %S", jse.toString().c_str());
                js->close();
                return KErrGeneral;
            }
            insertEntry.clear();
        }
    }

    // table 6: Midp function group settings table
    {
        LOG(EBackup, EInfo,  "MIDP_FUNC_GRP_SETTINGS_TABLE");

        for (int rowNumber = 0; rowNumber < iTableSize[5]; rowNumber++)
        {
            attribute.setEntry(ID, iStringVector[count++]);
            insertEntry.insert(attribute);

            attribute.setEntry(FUNCTION_GROUP, iStringVector[count++]);
            insertEntry.insert(attribute);

            attribute.setEntry(ALLOWED_SETTINGS, iStringVector[count++]);
            insertEntry.insert(attribute);

            attribute.setEntry(CURRENT_SETTING, iStringVector[count++]);
            insertEntry.insert(attribute);

            attribute.setEntry(BLANKET_PROMPT, iStringVector[count++]);
            insertEntry.insert(attribute);

            try
            {
                js->write(MIDP_FUNC_GRP_SETTINGS_TABLE, insertEntry);
                ILOG(EBackup, "Writing to table succeded");
            }
            catch (JavaStorageException jse)
            {
                ELOG1(EBackup, "Writing to table failed: %S", jse.toString().c_str());
                js->close();
                return KErrGeneral;
            }
            insertEntry.clear();
        }
    }

    // table 7: push registration table
    {
        LOG(EBackup, EInfo,  "Writing to PUSH_REGISTRATIONS_TABLE");

        for (int rowNumber = 0; rowNumber < iTableSize[6]; rowNumber++)
        {
            attribute.setEntry(ID, iStringVector[count++]);
            insertEntry.insert(attribute);

            attribute.setEntry(URL, iStringVector[count++]);
            insertEntry.insert(attribute);

            attribute.setEntry(NAME, iStringVector[count++]);
            insertEntry.insert(attribute);

            attribute.setEntry(FILTER, iStringVector[count++]);
            insertEntry.insert(attribute);

            attribute.setEntry(REGISTRATION_TYPE, iStringVector[count++]);
            insertEntry.insert(attribute);

            try
            {
                js->write(PUSH_REGISTRATIONS_TABLE, insertEntry);
                ILOG(EBackup, "Writing to table succeded");
            }
            catch (JavaStorageException jse)
            {
                ELOG1(EBackup, "Writing to table failed: %S", jse.toString().c_str());
                js->close();
                return KErrGeneral;
            }
            insertEntry.clear();
        }
    }

    // table 8: alarm registration table
    {
        LOG(EBackup, EInfo,  "Writing to ALARM_REGISTRATIONS_TABLE");

        for (int rowNumber = 0; rowNumber < iTableSize[7]; rowNumber++)
        {
            attribute.setEntry(ID, iStringVector[count++]);
            insertEntry.insert(attribute);

            attribute.setEntry(ALARM_TIME, iStringVector[count++]);
            insertEntry.insert(attribute);

            try
            {
                js->write(ALARM_REGISTRATIONS_TABLE, insertEntry);
                ILOG(EBackup, "Writing to table succeded");
            }
            catch (JavaStorageException jse)
            {
                ELOG1(EBackup, "Writing to table failed: %S", jse.toString().c_str());
                js->close();
                return KErrGeneral;
            }
            insertEntry.clear();
        }
    }


    // table 9: runtime settings table
    {
        LOG(EBackup, EInfo,  "Writing to RUNTIME_SETTINGS_TABLE");

        for (int rowNumber = 0; rowNumber < iTableSize[8]; rowNumber++)
        {
            attribute.setEntry(EXTENSIONS, iStringVector[count++]);
            insertEntry.insert(attribute);

            try
            {
                js->write(RUNTIME_SETTINGS_TABLE, insertEntry);
                ILOG(EBackup, "Writing to table succeded");
            }
            catch (JavaStorageException jse)
            {
                ELOG1(EBackup, "Writing to table failed: %S", jse.toString().c_str());
                js->close();
                return KErrGeneral;
            }
            insertEntry.clear();
        }
    }

    // table 10: pre-install table
    {
        LOG(EBackup, EInfo,  "Writing to PREINSTALL_TABLE");

        for (int rowNumber = 0; rowNumber < iTableSize[9]; rowNumber++)
        {
            attribute.setEntry(NAME, iStringVector[count++]);
            insertEntry.insert(attribute);

            attribute.setEntry(VENDOR, iStringVector[count++]);
            insertEntry.insert(attribute);

            attribute.setEntry(VERSION, iStringVector[count++]);
            insertEntry.insert(attribute);

            attribute.setEntry(INSTALL_STATE, iStringVector[count++]);
            insertEntry.insert(attribute);

            try
            {
                js->write(PREINSTALL_TABLE, insertEntry);
                ILOG(EBackup, "Writing to table succeded");
            }
            catch (JavaStorageException jse)
            {
                ELOG1(EBackup, "Writing to table failed: %S", jse.toString().c_str());
                js->close();
                return KErrGeneral;
            }
            insertEntry.clear();
        }
    }

    ILOG(EBackup, "Data written to JAVA_DATABASE; So Commit the transaction and close the database");
    ILOG(EBackup, "Committing the transaction");

    try
    {
        js->commitTransaction();
        ILOG(EBackup, "Transaction committed successfully");
    }
    catch (JavaStorageException jse)
    {
        ELOG1(EBackup, "Committing the transaction failed: %S", jse.toString().c_str());
        js->close();
        return KErrGeneral;
    }

    ILOG(EBackup, "Close the database");

    // close the JAVA_DATABASE
    js->close();


    // table 11: ota status table
    /* stored in another database called javaotadatabase.
       so open a new connection to that database   */

    {
        ILOG(EBackup, "Create instance of JAVA_OTA_DATABASE_NAME and open the database");
        auto_ptr<JavaStorage> jos(JavaStorage::createInstance());

        try
        {
            jos->open(JAVA_OTA_DATABASE_NAME);
            ILOG(EBackup, "Opening database succeded");
        }
        catch (JavaStorageException jse)
        {
            ELOG1(EBackup, "Opening database failed: %S", jse.toString().c_str());
            return KErrGeneral;
        }

        ILOG(EBackup, "Start a transaction for writing to this database");

        try
        {
            jos->startTransaction();
            ILOG(EBackup, "Transaction started");
        }
        catch (JavaStorageException jse)
        {
            ELOG1(EBackup, "startTransaction() failed: %S", jse.toString().c_str());
            jos->close();
            return KErrGeneral;
        }

        ILOG(EBackup, "Before writing data to this database, existing data must be cleared");

        try
        {
            JavaStorageApplicationEntry_t emptyEntry;
            jos->remove(OTA_STATUS_TABLE, emptyEntry);
        }
        catch (JavaStorageException jse)
        {
            ELOG1(EBackup, "Removing data from OTA database failed: %S", jse.toString().c_str());
            jos->close();
            return KErrGeneral;
        }

        LOG(EBackup, EInfo,  "Writing to OTA_STATUS_TABLE");

        for (int rowNumber = 0; rowNumber < iTableSize[10]; rowNumber++)
        {
            attribute.setEntry(ID, iStringVector[count++]);
            insertEntry.insert(attribute);

            attribute.setEntry(CREATION_TIME, iStringVector[count++]);
            insertEntry.insert(attribute);

            attribute.setEntry(TYPE, iStringVector[count++]);
            insertEntry.insert(attribute);

            attribute.setEntry(OTA_CODE, iStringVector[count++]);
            insertEntry.insert(attribute);

            attribute.setEntry(URL, iStringVector[count++]);
            insertEntry.insert(attribute);

            attribute.setEntry(LATEST_RETRY_TIME, iStringVector[count++]);
            insertEntry.insert(attribute);

            attribute.setEntry(RETRY_COUNT, iStringVector[count++]);
            insertEntry.insert(attribute);

            try
            {
                jos->write(OTA_STATUS_TABLE, insertEntry);
                LOG(EBackup, EInfo,  "Writing to table succeded");
            }
            catch (JavaStorageException jse)
            {
                ELOG1(EBackup, "Writing to table failed: %S", jse.toString().c_str());
                jos->close();
                return KErrGeneral;
            }
            insertEntry.clear();
        }

        ILOG(EBackup, "Data written to JAVA_OTA_DATABASE; commit the transaction and close the database");
        ILOG(EBackup, "Committing the transaction");

        try
        {
            jos->commitTransaction();
            LOG(EBackup, EInfo, "Transaction committed successfully");
        }
        catch (JavaStorageException jse)
        {
            ELOG1(EBackup, "Committing the transaction failed: %S", jse.toString().c_str());
            jos->close();
            return KErrGeneral;
        }

        LOG(EBackup, EInfo, "Close the database");

        // close the OTA_DATABASE
        jos->close();
    }

    LOG(EBackup, EInfo, "Data written to Storage");
    return KErrNone;
}


int CStorageBackupUtil::FillVectorwithAppPackageTableData(JavaStorageApplicationList_t& foundEntries)
{
    const wstring emptyString;
    wstring str;

    JavaStorageEntry attribute;

    /* Initialise Iterators to iterate through all applications
       matched with search patterns.  */
    JavaStorageApplicationList_t::const_iterator applications;
    JavaStorageApplicationEntry_t::const_iterator findIterator;

    int rowsCount=0;
    for (applications = foundEntries.begin(); applications != foundEntries.end(); applications++)
    {
        attribute.setEntry(ID, L"");
        str = emptyString;
        findIterator = (*applications).find(attribute);

        if (findIterator != (*applications).end())
        {
            str = (*findIterator).entryValue();
        }
        iStringVector.push_back(str);

        attribute.setEntry(PACKAGE_NAME, L"");
        str = emptyString;
        findIterator = (*applications).find(attribute);

        if (findIterator != (*applications).end())
        {
            str = (*findIterator).entryValue();
        }
        iStringVector.push_back(str);

        attribute.setEntry(VENDOR, L"");
        str = emptyString;
        findIterator = (*applications).find(attribute);

        if (findIterator != (*applications).end())
        {
            str = (*findIterator).entryValue();
        }
        iStringVector.push_back(str);

        attribute.setEntry(VERSION, L"");
        str = emptyString;
        findIterator = (*applications).find(attribute);

        if (findIterator != (*applications).end())
        {
            str = (*findIterator).entryValue();
        }
        iStringVector.push_back(str);

        attribute.setEntry(ROOT_PATH, L"");
        str = emptyString;
        findIterator = (*applications).find(attribute);

        if (findIterator != (*applications).end())
        {
            str = (*findIterator).entryValue();
        }
        iStringVector.push_back(str);

        attribute.setEntry(MEDIA_ID, L"");
        str = emptyString;
        findIterator = (*applications).find(attribute);

        if (findIterator != (*applications).end())
        {
            str = (*findIterator).entryValue();
        }
        iStringVector.push_back(str);

        attribute.setEntry(INITIAL_SIZE, L"");
        str = emptyString;
        findIterator = (*applications).find(attribute);

        if (findIterator != (*applications).end())
        {
            str = (*findIterator).entryValue();
        }
        iStringVector.push_back(str);

        attribute.setEntry(JAD_PATH, L"");
        str = emptyString;
        findIterator = (*applications).find(attribute);

        if (findIterator != (*applications).end())
        {
            str = (*findIterator).entryValue();
        }
        iStringVector.push_back(str);

        attribute.setEntry(JAR_PATH, L"");
        str = emptyString;
        findIterator = (*applications).find(attribute);

        if (findIterator != (*applications).end())
        {
            str = (*findIterator).entryValue();
        }
        iStringVector.push_back(str);

        attribute.setEntry(JAD_URL, L"");
        str = emptyString;
        findIterator = (*applications).find(attribute);

        if (findIterator != (*applications).end())
        {
            str = (*findIterator).entryValue();
        }
        iStringVector.push_back(str);

        attribute.setEntry(JAR_URL, L"");
        str = emptyString;
        findIterator = (*applications).find(attribute);

        if (findIterator != (*applications).end())
        {
            str = (*findIterator).entryValue();
        }
        iStringVector.push_back(str);

        attribute.setEntry(ACCESS_POINT, L"");
        str = emptyString;
        findIterator = (*applications).find(attribute);

        if (findIterator != (*applications).end())
        {
            str = (*findIterator).entryValue();
        }
        iStringVector.push_back(str);

        attribute.setEntry(CONTENT_INFO, L"");
        str = emptyString;
        findIterator = (*applications).find(attribute);

        if (findIterator != (*applications).end())
        {
            str = (*findIterator).entryValue();
        }
        iStringVector.push_back(str);

        attribute.setEntry(CONTENT_ID, L"");
        str = emptyString;
        findIterator = (*applications).find(attribute);

        if (findIterator != (*applications).end())
        {
            str = (*findIterator).entryValue();
        }
        iStringVector.push_back(str);

        rowsCount++;
    }
    ILOG1(EBackup, "for loop crossed with i = %d", rowsCount);
    return rowsCount;
}

int CStorageBackupUtil::FillVectorwithAppTableData(JavaStorageApplicationList_t& afoundEntries)
{
    const wstring emptyString;
    wstring str;

    JavaStorageEntry attribute;

    /* Initialise Iterators to iterate through all applications
       matched with search patterns.  */
    JavaStorageApplicationList_t::const_iterator applications;
    JavaStorageApplicationEntry_t::const_iterator findIterator;

    int rowsCount=0;
    for (applications = afoundEntries.begin(); applications != afoundEntries.end(); applications++)
    {
        attribute.setEntry(ID, L"");
        str = emptyString;
        findIterator = (*applications).find(attribute);

        if (findIterator != (*applications).end())
        {
            str = (*findIterator).entryValue();
        }
        iStringVector.push_back(str);

        attribute.setEntry(PACKAGE_ID, L"");
        str = emptyString;
        findIterator = (*applications).find(attribute);

        if (findIterator != (*applications).end())
        {
            str = (*findIterator).entryValue();
        }
        iStringVector.push_back(str);

        attribute.setEntry(NAME, L"");
        str = emptyString;
        findIterator = (*applications).find(attribute);

        if (findIterator != (*applications).end())
        {
            str = (*findIterator).entryValue();
        }
        iStringVector.push_back(str);

        attribute.setEntry(MAIN_CLASS, L"");
        str = emptyString;
        findIterator = (*applications).find(attribute);

        if (findIterator != (*applications).end())
        {
            str = (*findIterator).entryValue();
        }
        iStringVector.push_back(str);

        attribute.setEntry(AUTORUN, L"");
        str = emptyString;
        findIterator = (*applications).find(attribute);

        if (findIterator != (*applications).end())
        {
            str = (*findIterator).entryValue();
        }
        iStringVector.push_back(str);

        rowsCount++;
    }
    ILOG1(EBackup, "for loop crossed with i = %d", rowsCount);
    return rowsCount;
}

int CStorageBackupUtil::FillVectorwithAppPackageAttTableData(JavaStorageApplicationList_t& afoundEntries)
{
    const wstring emptyString;
    wstring str;

    JavaStorageEntry attribute;

    /* Initialise Iterators to iterate through all applications
       matched with search patterns.  */
    JavaStorageApplicationList_t::const_iterator applications;
    JavaStorageApplicationEntry_t::const_iterator findIterator;

    int rowsCount=0;
    for (applications = afoundEntries.begin(); applications != afoundEntries.end(); applications++)
    {
        attribute.setEntry(ID, L"");
        str = emptyString;
        findIterator = (*applications).find(attribute);

        if (findIterator != (*applications).end())
        {
            str = (*findIterator).entryValue();
        }
        iStringVector.push_back(str);

        attribute.setEntry(NAME, L"");
        str = emptyString;
        findIterator = (*applications).find(attribute);

        if (findIterator != (*applications).end())
        {
            str = (*findIterator).entryValue();
        }
        iStringVector.push_back(str);

        attribute.setEntry(VALUE, L"");
        str = emptyString;
        findIterator = (*applications).find(attribute);

        if (findIterator != (*applications).end())
        {
            str = (*findIterator).entryValue();
        }
        iStringVector.push_back(str);

        attribute.setEntry(TRUSTED, L"");
        str = emptyString;
        findIterator = (*applications).find(attribute);

        if (findIterator != (*applications).end())
        {
            str = (*findIterator).entryValue();
        }
        iStringVector.push_back(str);

        rowsCount++;
    }
    ILOG1(EBackup, "for loop crossed with i = %d", rowsCount);
    return rowsCount;
}

int CStorageBackupUtil::FillVectorwithMidpPackageTableData(JavaStorageApplicationList_t& afoundEntries)
{
    const wstring emptyString;
    wstring str;

    JavaStorageEntry attribute;

    /* Initialise Iterators to iterate through all applications
       matched with search patterns.  */
    JavaStorageApplicationList_t::const_iterator applications;
    JavaStorageApplicationEntry_t::const_iterator findIterator;

    int rowsCount=0;
    for (applications = afoundEntries.begin(); applications != afoundEntries.end(); applications++)
    {
        attribute.setEntry(ID, L"");
        str = emptyString;
        findIterator = (*applications).find(attribute);

        if (findIterator != (*applications).end())
        {
            str = (*findIterator).entryValue();
        }
        iStringVector.push_back(str);

        attribute.setEntry(TYPE, L"");
        str = emptyString;
        findIterator = (*applications).find(attribute);

        if (findIterator != (*applications).end())
        {
            str = (*findIterator).entryValue();
        }
        iStringVector.push_back(str);

        attribute.setEntry(SECURITY_DOMAIN, L"");
        str = emptyString;
        findIterator = (*applications).find(attribute);

        if (findIterator != (*applications).end())
        {
            str = (*findIterator).entryValue();
        }
        iStringVector.push_back(str);

        attribute.setEntry(SECURITY_DOMAIN_CATEGORY, L"");
        str = emptyString;
        findIterator = (*applications).find(attribute);

        if (findIterator != (*applications).end())
        {
            str = (*findIterator).entryValue();
        }
        iStringVector.push_back(str);

        attribute.setEntry(HASH, L"");
        str = emptyString;
        findIterator = (*applications).find(attribute);

        if (findIterator != (*applications).end())
        {
            str = (*findIterator).entryValue();
        }
        iStringVector.push_back(str);

        attribute.setEntry(CERT_HASH, L"");
        str = emptyString;
        findIterator = (*applications).find(attribute);

        if (findIterator != (*applications).end())
        {
            str = (*findIterator).entryValue();
        }
        iStringVector.push_back(str);

        attribute.setEntry(RMS, L"");
        str = emptyString;
        findIterator = (*applications).find(attribute);

        if (findIterator != (*applications).end())
        {
            str = (*findIterator).entryValue();
        }
        iStringVector.push_back(str);

        attribute.setEntry(VALID_CERTS, L"");
        str = emptyString;
        findIterator = (*applications).find(attribute);

        if (findIterator != (*applications).end())
        {
            str = (*findIterator).entryValue();
        }
        iStringVector.push_back(str);

        attribute.setEntry(ON_SCREEN_KEYPAD, L"");
        str = emptyString;
        findIterator = (*applications).find(attribute);

        if (findIterator != (*applications).end())
        {
            str = (*findIterator).entryValue();
        }
        iStringVector.push_back(str);
        
        attribute.setEntry(SECURITY_WARNINGS, L"");
        str = emptyString;
        findIterator = (*applications).find(attribute);

        if (findIterator != (*applications).end())
        {
            str = (*findIterator).entryValue();
        }
        iStringVector.push_back(str);

        rowsCount++;
    }
    ILOG1(EBackup, "for loop crossed with i = %d", rowsCount);
    return rowsCount;
}

int CStorageBackupUtil::FillVectorwithMidpPermTableData(JavaStorageApplicationList_t& afoundEntries)
{
    const wstring emptyString;
    wstring str;

    JavaStorageEntry attribute;

    /* Initialise Iterators to iterate through all applications
       matched with search patterns.  */
    JavaStorageApplicationList_t::const_iterator applications;
    JavaStorageApplicationEntry_t::const_iterator findIterator;

    int rowsCount=0;
    for (applications = afoundEntries.begin(); applications != afoundEntries.end(); applications++)
    {
        attribute.setEntry(ID, L"");
        str = emptyString;
        findIterator = (*applications).find(attribute);

        if (findIterator != (*applications).end())
        {
            str = (*findIterator).entryValue();
        }
        iStringVector.push_back(str);

        attribute.setEntry(CLASS, L"");
        str = emptyString;
        findIterator = (*applications).find(attribute);

        if (findIterator != (*applications).end())
        {
            str = (*findIterator).entryValue();
        }
        iStringVector.push_back(str);

        attribute.setEntry(NAME, L"");
        str = emptyString;
        findIterator = (*applications).find(attribute);

        if (findIterator != (*applications).end())
        {
            str = (*findIterator).entryValue();
        }
        iStringVector.push_back(str);

        attribute.setEntry(ACTION, L"");
        str = emptyString;
        findIterator = (*applications).find(attribute);

        if (findIterator != (*applications).end())
        {
            str = (*findIterator).entryValue();
        }
        iStringVector.push_back(str);

        attribute.setEntry(FUNCTION_GROUP, L"");
        str = emptyString;
        findIterator = (*applications).find(attribute);

        if (findIterator != (*applications).end())
        {
            str = (*findIterator).entryValue();
        }
        iStringVector.push_back(str);

        rowsCount++;
    }
    ILOG1(EBackup, "for loop crossed with i = %d", rowsCount);
    return rowsCount;
}

int CStorageBackupUtil::FillVectorwithMidpFuncGrpSetTableData(JavaStorageApplicationList_t& afoundEntries)
{
    const wstring emptyString;
    wstring str;

    JavaStorageEntry attribute;

    /* Initialise Iterators to iterate through all applications
       matched with search patterns.  */
    JavaStorageApplicationList_t::const_iterator applications;
    JavaStorageApplicationEntry_t::const_iterator findIterator;

    int rowsCount=0;
    for (applications = afoundEntries.begin(); applications != afoundEntries.end(); applications++)
    {
        attribute.setEntry(ID, L"");
        str = emptyString;
        findIterator = (*applications).find(attribute);

        if (findIterator != (*applications).end())
        {
            str = (*findIterator).entryValue();
        }
        iStringVector.push_back(str);

        attribute.setEntry(FUNCTION_GROUP, L"");
        str = emptyString;
        findIterator = (*applications).find(attribute);

        if (findIterator != (*applications).end())
        {
            str = (*findIterator).entryValue();
        }
        iStringVector.push_back(str);

        attribute.setEntry(ALLOWED_SETTINGS, L"");
        str = emptyString;
        findIterator = (*applications).find(attribute);

        if (findIterator != (*applications).end())
        {
            str = (*findIterator).entryValue();
        }
        iStringVector.push_back(str);

        attribute.setEntry(CURRENT_SETTING, L"");
        str = emptyString;
        findIterator = (*applications).find(attribute);

        if (findIterator != (*applications).end())
        {
            str = (*findIterator).entryValue();
        }
        iStringVector.push_back(str);

        attribute.setEntry(BLANKET_PROMPT, L"");
        str = emptyString;
        findIterator = (*applications).find(attribute);

        if (findIterator != (*applications).end())
        {
            str = (*findIterator).entryValue();
        }
        iStringVector.push_back(str);

        rowsCount++;
    }
    ILOG1(EBackup, "for loop crossed with i = %d", rowsCount);
    return rowsCount;
}

int CStorageBackupUtil::FillVectorwithPushRegTableData(JavaStorageApplicationList_t& afoundEntries)
{
    const wstring emptyString;
    wstring str;

    JavaStorageEntry attribute;

    /* Initialise Iterators to iterate through all applications
       matched with search patterns.  */
    JavaStorageApplicationList_t::const_iterator applications;
    JavaStorageApplicationEntry_t::const_iterator findIterator;

    int rowsCount=0;
    for (applications = afoundEntries.begin(); applications != afoundEntries.end(); applications++)
    {
        attribute.setEntry(ID, L"");
        str = emptyString;
        findIterator = (*applications).find(attribute);

        if (findIterator != (*applications).end())
        {
            str = (*findIterator).entryValue();
        }
        iStringVector.push_back(str);

        attribute.setEntry(URL, L"");
        str = emptyString;
        findIterator = (*applications).find(attribute);

        if (findIterator != (*applications).end())
        {
            str = (*findIterator).entryValue();
        }
        iStringVector.push_back(str);

        attribute.setEntry(NAME, L"");
        str = emptyString;
        findIterator = (*applications).find(attribute);

        if (findIterator != (*applications).end())
        {
            str = (*findIterator).entryValue();
        }
        iStringVector.push_back(str);

        attribute.setEntry(FILTER, L"");
        str = emptyString;
        findIterator = (*applications).find(attribute);

        if (findIterator != (*applications).end())
        {
            str = (*findIterator).entryValue();
        }
        iStringVector.push_back(str);

        attribute.setEntry(REGISTRATION_TYPE, L"");
        str = emptyString;
        findIterator = (*applications).find(attribute);

        if (findIterator != (*applications).end())
        {
            str = (*findIterator).entryValue();
        }
        iStringVector.push_back(str);

        rowsCount++;
    }
    ILOG1(EBackup, "for loop crossed with i = %d", rowsCount);
    return rowsCount;
}

int CStorageBackupUtil::FillVectorwithAlarmRegTableData(JavaStorageApplicationList_t& afoundEntries)
{
    const wstring emptyString;
    wstring str;

    JavaStorageEntry attribute;

    /* Initialise Iterators to iterate through all applications
       matched with search patterns.  */
    JavaStorageApplicationList_t::const_iterator applications;
    JavaStorageApplicationEntry_t::const_iterator findIterator;

    int rowsCount=0;
    for (applications = afoundEntries.begin(); applications != afoundEntries.end(); applications++)
    {
        attribute.setEntry(ID, L"");
        str = emptyString;
        findIterator = (*applications).find(attribute);

        if (findIterator != (*applications).end())
        {
            str = (*findIterator).entryValue();
        }
        iStringVector.push_back(str);

        attribute.setEntry(ALARM_TIME, L"");
        str = emptyString;
        findIterator = (*applications).find(attribute);

        if (findIterator != (*applications).end())
        {
            str = (*findIterator).entryValue();
        }
        iStringVector.push_back(str);

        rowsCount++;
    }
    ILOG1(EBackup, "for loop crossed with i = %d", rowsCount);
    return rowsCount;
}

int CStorageBackupUtil::FillVectorwithRuntimeSetTableData(JavaStorageApplicationList_t& afoundEntries)
{
    const wstring emptyString;
    wstring str;

    JavaStorageEntry attribute;

    /* Initialise Iterators to iterate through all applications
       matched with search patterns.  */
    JavaStorageApplicationList_t::const_iterator applications;
    JavaStorageApplicationEntry_t::const_iterator findIterator;

    int rowsCount=0;
    for (applications = afoundEntries.begin(); applications != afoundEntries.end(); applications++)
    {
        attribute.setEntry(EXTENSIONS, L"");
        str = emptyString;
        findIterator = (*applications).find(attribute);

        if (findIterator != (*applications).end())
        {
            str = (*findIterator).entryValue();
        }
        iStringVector.push_back(str);

        rowsCount++;
    }
    ILOG1(EBackup, "for loop crossed with i = %d", rowsCount);
    return rowsCount;
}

int CStorageBackupUtil::FillVectorwithPreinstallTableData(JavaStorageApplicationList_t& afoundEntries)
{
    const wstring emptyString;
    wstring str;

    JavaStorageEntry attribute;

    /* Initialise Iterators to iterate through all applications
       matched with search patterns.  */
    JavaStorageApplicationList_t::const_iterator applications;
    JavaStorageApplicationEntry_t::const_iterator findIterator;

    int rowsCount=0;
    for (applications = afoundEntries.begin(); applications != afoundEntries.end(); applications++)
    {
        attribute.setEntry(NAME, L"");
        str = emptyString;
        findIterator = (*applications).find(attribute);

        if (findIterator != (*applications).end())
        {
            str = (*findIterator).entryValue();
        }
        iStringVector.push_back(str);

        attribute.setEntry(VENDOR, L"");
        str = emptyString;
        findIterator = (*applications).find(attribute);

        if (findIterator != (*applications).end())
        {
            str = (*findIterator).entryValue();
        }
        iStringVector.push_back(str);

        attribute.setEntry(VERSION, L"");
        str = emptyString;
        findIterator = (*applications).find(attribute);

        if (findIterator != (*applications).end())
        {
            str = (*findIterator).entryValue();
        }
        iStringVector.push_back(str);

        attribute.setEntry(INSTALL_STATE, L"");
        str = emptyString;
        findIterator = (*applications).find(attribute);

        if (findIterator != (*applications).end())
        {
            str = (*findIterator).entryValue();
        }
        iStringVector.push_back(str);

        rowsCount++;
    }
    ILOG1(EBackup, "for loop crossed with i = %d", rowsCount);
    return rowsCount;
}

int CStorageBackupUtil::FillVectorwithOtaStatusTableData(JavaStorageApplicationList_t& afoundEntries)
{
    const wstring emptyString;
    wstring str;

    JavaStorageEntry attribute;

    /* Initialise Iterators to iterate through all applications
       matched with search patterns.  */
    JavaStorageApplicationList_t::const_iterator applications;
    JavaStorageApplicationEntry_t::const_iterator findIterator;

    int rowsCount=0;
    for (applications = afoundEntries.begin(); applications != afoundEntries.end(); applications++)
    {
        attribute.setEntry(ID, L"");
        str = emptyString;
        findIterator = (*applications).find(attribute);

        if (findIterator != (*applications).end())
        {
            str = (*findIterator).entryValue();
        }
        iStringVector.push_back(str);

        attribute.setEntry(CREATION_TIME, L"");
        str = emptyString;
        findIterator = (*applications).find(attribute);

        if (findIterator != (*applications).end())
        {
            str = (*findIterator).entryValue();
        }
        iStringVector.push_back(str);

        attribute.setEntry(TYPE, L"");
        str = emptyString;
        findIterator = (*applications).find(attribute);

        if (findIterator != (*applications).end())
        {
            str = (*findIterator).entryValue();
        }
        iStringVector.push_back(str);

        attribute.setEntry(OTA_CODE, L"");
        str = emptyString;
        findIterator = (*applications).find(attribute);

        if (findIterator != (*applications).end())
        {
            str = (*findIterator).entryValue();
        }
        iStringVector.push_back(str);

        attribute.setEntry(URL, L"");
        str = emptyString;
        findIterator = (*applications).find(attribute);

        if (findIterator != (*applications).end())
        {
            str = (*findIterator).entryValue();
        }
        iStringVector.push_back(str);

        attribute.setEntry(LATEST_RETRY_TIME, L"");
        str = emptyString;
        findIterator = (*applications).find(attribute);

        if (findIterator != (*applications).end())
        {
            str = (*findIterator).entryValue();
        }
        iStringVector.push_back(str);

        attribute.setEntry(RETRY_COUNT, L"");
        str = emptyString;
        findIterator = (*applications).find(attribute);

        if (findIterator != (*applications).end())
        {
            str = (*findIterator).entryValue();
        }
        iStringVector.push_back(str);

        rowsCount++;
    }
    ILOG1(EBackup, "for loop crossed with i = %d", rowsCount);
    return rowsCount;
}

