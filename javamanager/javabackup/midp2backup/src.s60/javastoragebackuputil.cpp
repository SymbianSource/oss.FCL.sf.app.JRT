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
#include "mediaidupdater.h"

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

const wchar_t * const JBNULLSTRING = L"ABBAABBA_NULL";
const int JBNULLSTRINGLENGTH = -1;

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
    if (iBufForJavaStorageItemsPtr)
    {
        delete iBufForJavaStorageItemsPtr;
        iBufForJavaStorageItemsPtr = 0;
    }

}


void CStorageBackupUtil::BackupStorageDataL(RDesWriteStream& aStream, TBool& aBackupNotFinished, TInt& aBufferSpaceLeft)
{
    ILOG(EBackup, "CStorageBackupUtil::BackupStorageDataL()");

    iBufferSpaceLeft = aBufferSpaceLeft;

    if (iFirstCalltoBackupStorageData)
    {
        ILOG(EBackup, "First call to BackupStorageData()");
        int err = FillVectorWithStorageData();

        if (err != KErrNone)
        {
            ELOG1(EBackup, "Error (%d) in filling wstring vector", err);
            User::Leave(err);
        }
        ILOG1(EBackup, "Total no of rows in vector: %d", iStringVector.size());

        // 1. Find out the size of the buffer needed for containing JavaStorage
        //    data in "streamed" format.
        TUint totalStringLengthInBytes = 0;
        for (int i = 0; i < iStringVector.size(); ++i)
        {
            if (iStringVector[i] == JBNULLSTRING ){
                continue;
            }
            totalStringLengthInBytes += iStringVector[i].length()*sizeof(wchar_t);
        }
        ILOG1(EBackup, "Total string length calculated: %d", totalStringLengthInBytes);

        // Calculate the total length of the buffer.
        // The content of the buffer will be as follows:

        TUint totalBuffSize = sizeof(TInt32) + NUMBER_OF_TABLES*sizeof(TInt16)
            + iStringVector.size()*sizeof(TInt16) + totalStringLengthInBytes;

        // 2. Reserve the buffer with adequate space
        iBufForJavaStorageItemsPtr = HBufC8::NewL(totalBuffSize);
        ILOG1(EBackup, "javaStorage Buffer(size %d) allocated SUCCESSFULLY", totalBuffSize);

        // 3. Create temporary stream operator and with it write stuff to buffer
        TPtr8 buffPtr(iBufForJavaStorageItemsPtr->Des());
        RDesWriteStream buffStream(buffPtr);
        CleanupClosePushL(buffStream);
        buffStream.WriteInt32L(iStringVector.size());
        for (int tableNumber = 0; tableNumber < NUMBER_OF_TABLES; ++tableNumber)
        {
            buffStream.WriteInt16L(iTableSize[tableNumber]);
        }
        ILOG(EBackup, "JavaStorage table sizes writen to buffer");
        TUint writenStringLength = 0;
        for (int i = 0; i < iStringVector.size(); ++i)
        {
            TInt16 lenOf8byteString = JBNULLSTRINGLENGTH;
            if ( iStringVector[i] == JBNULLSTRING )
            {
                buffStream.WriteInt16L(lenOf8byteString);
                continue;
            }
            lenOf8byteString = iStringVector[i].length()*sizeof(wchar_t);
            buffStream.WriteInt16L(lenOf8byteString);
            if (lenOf8byteString > 0 )
            {
                HBufC* tempstring = java::util::S60CommonUtils::wstringToDes(
                    iStringVector[i].c_str());
                if (!tempstring)
                {
                    ELOG(EBackup, "Out of memory in JavaStorage backup(in wstring -> des conv)!");
                    User::Leave(KErrNoMemory);
                }
                CleanupStack::PushL(tempstring);
                TPtrC tempStr = tempstring->Des();
                writenStringLength += tempStr.Size();
                buffStream.WriteL(tempStr); //length of the string will not be written
                CleanupStack::PopAndDestroy(tempstring);
            }
        }
        ILOG1(EBackup, "Total string length writen: %d", writenStringLength);
        ILOG(EBackup, "Whole Java Storage String vector writen to streambuffer");

        // 4. Clear not needed resources
        iStringVector.clear();
        CleanupStack::PopAndDestroy(&buffStream);
        ILOG(EBackup, "Not needed resources cleared");

        // 5. Set the read pointer to the beginning of the buffer data
        //    Note that the length of the HBufC8 buffer is exact.
        iBuffReadPointer.Set(iBufForJavaStorageItemsPtr->Des());
        iFirstCalltoBackupStorageData = EFalse;
    }

    // 6. Start to provide data to SBE from the buffer.
    ILOG(EBackup, "Extracting data from buffer to SBE");
    ILOG1(EBackup, "Length of the data in stream buffer: %d", iBuffReadPointer.Length());
    ILOG1(EBackup, "Space available in SBE buffer: %d", aBufferSpaceLeft);
    if (iBuffReadPointer.Length() <= aBufferSpaceLeft )
    {
        aStream.WriteL(iBuffReadPointer);
        aBufferSpaceLeft -= iBuffReadPointer.Length();
        iBuffReadPointer.Set(NULL,0);
        delete iBufForJavaStorageItemsPtr;
        iBufForJavaStorageItemsPtr = 0;
        ILOG(EBackup, "BACKUP OF STORAGE DATA FINISHED");
        aBackupNotFinished = EFalse; // Indicate to caller that we are ready
    }
    else // All data from internal buffer does not fit at once to buffer received from SBE
    {
        aStream.WriteL(iBuffReadPointer, aBufferSpaceLeft);
        TInt lengthOfWritenData = aBufferSpaceLeft;
        iBuffReadPointer.Set(iBuffReadPointer.Ptr() + lengthOfWritenData,
                                       iBuffReadPointer.Length() - lengthOfWritenData);
        aBufferSpaceLeft = 0;
        ILOG(EBackup, "Not all of the storage data fit into SBE buffer, new buffer from SBE needed.");
    }
}

void CStorageBackupUtil::RestoreStorageDataL(RDesReadStream& aStream, TInt& aRestoreState, TInt& aBufferSpaceLeft)
{
    ILOG(EBackup, "+CStorageBackupUtil::RestoreStorageDataL()");

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

        // ensure that storage contains correct removable media ids
        MediaIdUpdater updater;
        updater.update();

        // Storage restore is over; Set state to EAppArc
        ILOG(EBackup, "JAVASTORAGE RESTORED SUCCESSFULLY");
        aRestoreState = EAppArc;
        aBufferSpaceLeft = iBufferSpaceLeft;
    }
    ILOG(EBackup, "-CStorageBackupUtil::RestoreStorageDataL()");
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

    else /* handling new string */
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
        /* */
        else if (iLenOfString == JBNULLSTRINGLENGTH )
        {
            iStringVector.push_back(JBNULLSTRING);
            iStrCount--;
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


void CStorageBackupUtil::WriteItemToStorageEntry(
    const std::wstring& aEntryName,
    const std::wstring& aEntryValue,
    JavaStorageApplicationEntry_t& aInsertEntry
    )
{
    JavaStorageEntry attribute;
    if (aEntryValue != JBNULLSTRING )
    {
        attribute.setEntry(aEntryName, aEntryValue);
        aInsertEntry.insert(attribute);
    }
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

    JavaStorageApplicationEntry_t insertEntry;

    ILOG(EBackup, "Start transaction for writing into the database");

    int count = 0;
    // table 1 : Application package table
    {
        LOG(EBackup, EInfo,  "Writing to APPLICATION_PACKAGE_TABLE");

        for (int rowNumber = 0; rowNumber < iTableSize[0]; rowNumber++)
        {
            WriteItemToStorageEntry(ID, iStringVector[count++], insertEntry);
            WriteItemToStorageEntry(PACKAGE_NAME, iStringVector[count++], insertEntry);
            WriteItemToStorageEntry(VENDOR, iStringVector[count++], insertEntry);
            WriteItemToStorageEntry(VERSION, iStringVector[count++], insertEntry);
            WriteItemToStorageEntry(ROOT_PATH, iStringVector[count++], insertEntry);
            WriteItemToStorageEntry(MEDIA_ID, iStringVector[count++], insertEntry);
            WriteItemToStorageEntry(INITIAL_SIZE, iStringVector[count++], insertEntry);
            WriteItemToStorageEntry(JAD_PATH, iStringVector[count++], insertEntry);
            WriteItemToStorageEntry(JAR_PATH, iStringVector[count++], insertEntry);
            WriteItemToStorageEntry(JAD_URL, iStringVector[count++], insertEntry);
            WriteItemToStorageEntry(JAR_URL, iStringVector[count++], insertEntry);
            WriteItemToStorageEntry(ACCESS_POINT, iStringVector[count++], insertEntry);
            WriteItemToStorageEntry(CONTENT_INFO, iStringVector[count++], insertEntry);
            WriteItemToStorageEntry(CONTENT_ID, iStringVector[count++], insertEntry);

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
            WriteItemToStorageEntry(ID, iStringVector[count++], insertEntry);
            WriteItemToStorageEntry(PACKAGE_ID, iStringVector[count++], insertEntry);
            WriteItemToStorageEntry(NAME, iStringVector[count++], insertEntry);
            WriteItemToStorageEntry(MAIN_CLASS, iStringVector[count++], insertEntry);
            WriteItemToStorageEntry(AUTORUN, iStringVector[count++], insertEntry);

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
            WriteItemToStorageEntry(ID, iStringVector[count++], insertEntry);
            WriteItemToStorageEntry(NAME, iStringVector[count++], insertEntry);
            WriteItemToStorageEntry(VALUE, iStringVector[count++], insertEntry);
            WriteItemToStorageEntry(TRUSTED, iStringVector[count++], insertEntry);

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
            WriteItemToStorageEntry(ID, iStringVector[count++], insertEntry);
            WriteItemToStorageEntry(TYPE, iStringVector[count++], insertEntry);
            WriteItemToStorageEntry(SECURITY_DOMAIN, iStringVector[count++], insertEntry);
            WriteItemToStorageEntry(SECURITY_DOMAIN_CATEGORY, iStringVector[count++], insertEntry);
            WriteItemToStorageEntry(HASH, iStringVector[count++], insertEntry);
            WriteItemToStorageEntry(CERT_HASH, iStringVector[count++], insertEntry);
            WriteItemToStorageEntry(RMS, iStringVector[count++], insertEntry);
            WriteItemToStorageEntry(VALID_CERTS, iStringVector[count++], insertEntry);
            WriteItemToStorageEntry(ON_SCREEN_KEYPAD, iStringVector[count++], insertEntry);
            WriteItemToStorageEntry(SECURITY_WARNINGS, iStringVector[count++], insertEntry);

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
            WriteItemToStorageEntry(ID, iStringVector[count++], insertEntry);
            WriteItemToStorageEntry(CLASS, iStringVector[count++], insertEntry);
            WriteItemToStorageEntry(NAME, iStringVector[count++], insertEntry);
            WriteItemToStorageEntry(ACTION, iStringVector[count++], insertEntry);
            WriteItemToStorageEntry(FUNCTION_GROUP, iStringVector[count++], insertEntry);

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
            WriteItemToStorageEntry(ID, iStringVector[count++], insertEntry);
            WriteItemToStorageEntry(FUNCTION_GROUP, iStringVector[count++], insertEntry);
            WriteItemToStorageEntry(ALLOWED_SETTINGS, iStringVector[count++], insertEntry);
            WriteItemToStorageEntry(CURRENT_SETTING, iStringVector[count++], insertEntry);
            WriteItemToStorageEntry(BLANKET_PROMPT, iStringVector[count++], insertEntry);

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
            WriteItemToStorageEntry(ID, iStringVector[count++], insertEntry);
            WriteItemToStorageEntry(URL, iStringVector[count++], insertEntry);
            WriteItemToStorageEntry(NAME, iStringVector[count++], insertEntry);
            WriteItemToStorageEntry(FILTER, iStringVector[count++], insertEntry);
            WriteItemToStorageEntry(REGISTRATION_TYPE, iStringVector[count++], insertEntry);

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
            WriteItemToStorageEntry(ID, iStringVector[count++], insertEntry);
            WriteItemToStorageEntry(ALARM_TIME, iStringVector[count++], insertEntry);

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
            WriteItemToStorageEntry(EXTENSIONS, iStringVector[count++], insertEntry);

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
            WriteItemToStorageEntry(NAME, iStringVector[count++], insertEntry);
            WriteItemToStorageEntry(VENDOR, iStringVector[count++], insertEntry);
            WriteItemToStorageEntry(VERSION, iStringVector[count++], insertEntry);
            WriteItemToStorageEntry(INSTALL_STATE, iStringVector[count++], insertEntry);

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
            WriteItemToStorageEntry(ID, iStringVector[count++], insertEntry);
            WriteItemToStorageEntry(CREATION_TIME, iStringVector[count++], insertEntry);
            WriteItemToStorageEntry(TYPE, iStringVector[count++], insertEntry);
            WriteItemToStorageEntry(OTA_CODE, iStringVector[count++], insertEntry);
            WriteItemToStorageEntry(URL, iStringVector[count++], insertEntry);
            WriteItemToStorageEntry(LATEST_RETRY_TIME, iStringVector[count++], insertEntry);
            WriteItemToStorageEntry(RETRY_COUNT, iStringVector[count++], insertEntry);

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
        FetchStorageEntryToStringVector(attribute, applications);

        attribute.setEntry(PACKAGE_NAME, L"");
        FetchStorageEntryToStringVector(attribute, applications);

        attribute.setEntry(VENDOR, L"");
        FetchStorageEntryToStringVector(attribute, applications);

        attribute.setEntry(VERSION, L"");
        FetchStorageEntryToStringVector(attribute, applications);

        attribute.setEntry(ROOT_PATH, L"");
        FetchStorageEntryToStringVector(attribute, applications);

        attribute.setEntry(MEDIA_ID, L"");
        FetchStorageEntryToStringVector(attribute, applications);

        attribute.setEntry(INITIAL_SIZE, L"");
        FetchStorageEntryToStringVector(attribute, applications);

        attribute.setEntry(JAD_PATH, L"");
        FetchStorageEntryToStringVector(attribute, applications);

        attribute.setEntry(JAR_PATH, L"");
        FetchStorageEntryToStringVector(attribute, applications);

        attribute.setEntry(JAD_URL, L"");
        FetchStorageEntryToStringVector(attribute, applications);

        attribute.setEntry(JAR_URL, L"");
        FetchStorageEntryToStringVector(attribute, applications);

        attribute.setEntry(ACCESS_POINT, L"");
        FetchStorageEntryToStringVector(attribute, applications);

        attribute.setEntry(CONTENT_INFO, L"");
        FetchStorageEntryToStringVector(attribute, applications);

        attribute.setEntry(CONTENT_ID, L"");
        FetchStorageEntryToStringVector(attribute, applications);

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
        FetchStorageEntryToStringVector(attribute, applications);

        attribute.setEntry(PACKAGE_ID, L"");
        FetchStorageEntryToStringVector(attribute, applications);

        attribute.setEntry(NAME, L"");
        FetchStorageEntryToStringVector(attribute, applications);

        attribute.setEntry(MAIN_CLASS, L"");
        FetchStorageEntryToStringVector(attribute, applications);

        attribute.setEntry(AUTORUN, L"");
        FetchStorageEntryToStringVector(attribute, applications);

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
        FetchStorageEntryToStringVector(attribute, applications);

        attribute.setEntry(NAME, L"");
        FetchStorageEntryToStringVector(attribute, applications);

        attribute.setEntry(VALUE, L"");
        FetchStorageEntryToStringVector(attribute, applications);

        attribute.setEntry(TRUSTED, L"");
        FetchStorageEntryToStringVector(attribute, applications);

        rowsCount++;
    }
    ILOG1(EBackup, "for loop crossed with i = %d", rowsCount);
    return rowsCount;
}

void CStorageBackupUtil::FetchStorageEntryToStringVector(const JavaStorageEntry& aAttribute,
    JavaStorageApplicationList_t::const_iterator& aApplicationsIter)
{
    const wstring emptyString;
    wstring str;
    JavaStorageApplicationEntry_t::const_iterator findIterator;
    str = emptyString;
    findIterator = (*aApplicationsIter).find(aAttribute);

    if (findIterator != (*aApplicationsIter).end())
    {
        str = (*findIterator).entryValue();
        iStringVector.push_back(str);
    } else {
        iStringVector.push_back(JBNULLSTRING);
    }
}

int CStorageBackupUtil::FillVectorwithMidpPackageTableData(JavaStorageApplicationList_t& afoundEntries)
{
    const wstring emptyString;
    wstring str;

    JavaStorageEntry attribute;

    /* Initialise Iterators to iterate through all applications
       matched with search patterns.  */
    JavaStorageApplicationList_t::const_iterator applications;

    int rowsCount=0;
    for (applications = afoundEntries.begin(); applications != afoundEntries.end(); applications++)
    {
        attribute.setEntry(ID, L"");
        FetchStorageEntryToStringVector(attribute, applications);

        attribute.setEntry(TYPE, L"");
        FetchStorageEntryToStringVector(attribute, applications);

        attribute.setEntry(SECURITY_DOMAIN, L"");
        FetchStorageEntryToStringVector(attribute, applications);

        attribute.setEntry(SECURITY_DOMAIN_CATEGORY, L"");
        FetchStorageEntryToStringVector(attribute, applications);

        attribute.setEntry(HASH, L"");
        FetchStorageEntryToStringVector(attribute, applications);

        attribute.setEntry(CERT_HASH, L"");
        FetchStorageEntryToStringVector(attribute, applications);

        attribute.setEntry(RMS, L"");
        FetchStorageEntryToStringVector(attribute, applications);

        attribute.setEntry(VALID_CERTS, L"");
        FetchStorageEntryToStringVector(attribute, applications);

        attribute.setEntry(ON_SCREEN_KEYPAD, L"");
        FetchStorageEntryToStringVector(attribute, applications);

        attribute.setEntry(SECURITY_WARNINGS, L"");
        FetchStorageEntryToStringVector(attribute, applications);

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
        FetchStorageEntryToStringVector(attribute, applications);

        attribute.setEntry(CLASS, L"");
        FetchStorageEntryToStringVector(attribute, applications);

        attribute.setEntry(NAME, L"");
        FetchStorageEntryToStringVector(attribute, applications);

        attribute.setEntry(ACTION, L"");
        FetchStorageEntryToStringVector(attribute, applications);

        attribute.setEntry(FUNCTION_GROUP, L"");
        FetchStorageEntryToStringVector(attribute, applications);

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
        FetchStorageEntryToStringVector(attribute, applications);

        attribute.setEntry(FUNCTION_GROUP, L"");
        FetchStorageEntryToStringVector(attribute, applications);

        attribute.setEntry(ALLOWED_SETTINGS, L"");
        FetchStorageEntryToStringVector(attribute, applications);

        attribute.setEntry(CURRENT_SETTING, L"");
        FetchStorageEntryToStringVector(attribute, applications);

        attribute.setEntry(BLANKET_PROMPT, L"");
        FetchStorageEntryToStringVector(attribute, applications);

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
        FetchStorageEntryToStringVector(attribute, applications);

        attribute.setEntry(URL, L"");
        FetchStorageEntryToStringVector(attribute, applications);

        attribute.setEntry(NAME, L"");
        FetchStorageEntryToStringVector(attribute, applications);

        attribute.setEntry(FILTER, L"");
        FetchStorageEntryToStringVector(attribute, applications);

        attribute.setEntry(REGISTRATION_TYPE, L"");
        FetchStorageEntryToStringVector(attribute, applications);

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
        FetchStorageEntryToStringVector(attribute, applications);

        attribute.setEntry(ALARM_TIME, L"");
        FetchStorageEntryToStringVector(attribute, applications);

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
        FetchStorageEntryToStringVector(attribute, applications);

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
        FetchStorageEntryToStringVector(attribute, applications);

        attribute.setEntry(VENDOR, L"");
        FetchStorageEntryToStringVector(attribute, applications);

        attribute.setEntry(VERSION, L"");
        FetchStorageEntryToStringVector(attribute, applications);

        attribute.setEntry(INSTALL_STATE, L"");
        FetchStorageEntryToStringVector(attribute, applications);

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
        FetchStorageEntryToStringVector(attribute, applications);

        attribute.setEntry(CREATION_TIME, L"");
        FetchStorageEntryToStringVector(attribute, applications);

        attribute.setEntry(TYPE, L"");
        FetchStorageEntryToStringVector(attribute, applications);

        attribute.setEntry(OTA_CODE, L"");
        FetchStorageEntryToStringVector(attribute, applications);

        attribute.setEntry(URL, L"");
        FetchStorageEntryToStringVector(attribute, applications);

        attribute.setEntry(LATEST_RETRY_TIME, L"");
        FetchStorageEntryToStringVector(attribute, applications);

        attribute.setEntry(RETRY_COUNT, L"");
        FetchStorageEntryToStringVector(attribute, applications);

        rowsCount++;
    }
    ILOG1(EBackup, "for loop crossed with i = %d", rowsCount);
    return rowsCount;
}

