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
* Description:  Header file for CStorageBackupUtil class
*
*/


#ifndef JAVASTORAGEBACKUPUTIL_H
#define JAVASTORAGEBACKUPUTIL_H

#include <string>
#include <vector>

#include <e32base.h>
#include <wchar.h>
#include <s32strm.h>

#include "javastorageentry.h"
#include "javastorage.h"
#include "javastoragenames.h"

#define NUMBER_OF_TABLES 11

class RDesWriteStream;
class RDesReadStream;

namespace java
{
namespace backup
{


/**
--------------------------------------------------------------------------------

*  CStorageBackupUtil class is handling backup and restore of Java Storage data
*  This class is instantiated when a BUR operation is starting.

Example Data that could be present in JavaStorage which is backed up and restored by this class.

Application Package Data
ID        | PACKAGE_NAME | VENDOR | VERSION | ROOT_PATH                              | MEDIA_ID | INITIAL_SIZE | JAD_PATH                                              | JAR_PATH                                              | JAD_URL                          | JAR_URL                           | ACCESS_POINT | CONTENT_INFO | CONTENT_ID
788463512 | HelloWorld?  | Nokia  | 1.1     | \C\Private\102033E6\MIDlets\[101e59d8] | 123456   | 200          | \C\Private\102033E6\MIDlets\[101e59d8]\HelloWorld.jad | \C\Private\102033E6\MIDlets\[101e59d8]\HelloWorld.jar | http://getjar.com/MyGreatApp.jad | http://getjar.com/MyGreatApp2.jar | IAP:2        | 0            | 78d78sd89f789


Application Data
ID        | PACKAGE_ID | NAME       | MAIN_CLASS | AUTORUN
788463616 | 788463512  | HelloWorld | HelloWorld | 0


Application Package Attributes Data
ID        | NAME            | VALUE      | TRUSTED
788463512 | MIDlet-Name     | HelloWorld | 00
788463512 | MIDlet-Version  | 1.0        | 10
788463512 | MIDlet-Vendor   | Nokia      | 10


MIDP Application Package Data
ID        | TYPE         | SECURITY_DOMAIN | HASH      | CERT_HASH | RMS                                                | VALID_CERTS | ON_SCREEN_KEYPAD
788463512 | MIDletSuite? | Trusted         | 123456789 | 123456789 | \C\Private\102033E6\MIDlets\[101e59d8]\RMSdata.bin | 1,3,5       | 0


MIDP Permissions Data
ID        | CLASS                                        | NAME                     | ACTION | FUNCTION_GROUP
788463616 | javax.microedition.io.HttpProtocolPermission | http://server/index.html | read   | HTTP


MIDP Function Group Settings Data
ID        | FUNCTION_GROUP | ALLOWED_SETTINGS                     | CURRENT_SETTING | BLANKET_PROMPT
788463616 | HTTP           | 111 (e.g. BLANKET, SESSION, ONESHOT) | 001 (ONESHOT)   | 0

The allowed settings could be a an encoded integer based on the binary representation of 3 bits (e.g. 100 if only BLANKET is allowed, 011 if SESSION and ONESHOT are allowed).


MIDP Push Registrations Data
ID        | URL                                                                                       | NAME             | FILTER | REGISTRATION_TYPE
788463616 | sip:*;type=""application/test"", SIPPushReceiver?                                         | SIPPushReceiver? | *      | 1
788463616 | btspp://localhost:99999999999999999999999999999999;*;authenticated;blacklist=00E001234567 | BTPushReceiver?  | *      | 0


MIDP Alarm Push Registrations Data
ID        | ALARM_TIME
788463616 | 2008-09-13;15:20:30


MIDP Runtime Settings Data
EXTENSIONS
\C\Private\102033E6\Extenstions\12345678\Location.jar


Preinstall Data
NAME            | VENDOR     | VERSION | INSTALL_STATE
TestMIDletSuite | TestVendor | 1.2.3   | 2


MIDP OTA Status Data table
ID        | CREATION_TIME       | TYPE | OTA_CODE | URL                           | LATEST_RETRY_TIME   | RETRY_COUNT
788463616 | 2008-09-13;12:20:30 | 1    | 900      | http://www.moo.com/MIDLETS.js | 2008-09-13;15:20:30 | 2

-----------------------------------------------------------------------------------------------------------------------------
*/

class CStorageBackupUtil : public CBase
{
public:

    /**
     * Instantiates an object of this type
     */
    static CStorageBackupUtil* NewL();

    ~CStorageBackupUtil();

    /**
     * This method requests a section of Java Storage data.
     * Called by the function GetBackupDataSectionL from midp2backupplugin
     * The data is filled into the stream till it is full.
     * The data returned may be base or incremental depending
     * on the type of backup and the capability of the data owner.
     *
     * @param aBuffer a pointer to the base of the location where data
     *        can be copied.
     * @param aBackupNotFinished on return EFalse if all data has
     *        been returned for this drive, else ETrue.
     */
    void BackupStorageDataL(RDesWriteStream& aStream, TBool& aBackupNotFinished, TInt& aBufferSpaceLeft);

    /**
     * This method receives a section of base restore data.
     * Stores the restore data in the vector so that it can be
     * written to Java Storage when all data have come.
     *
     * @param stream holds the restore data
     * @param aRestoreState EFirstBuffer if all data has been returned
     *        for this drive, else remains at EStorage.
     * @param aBufferSpaceLeft the space left in the buffer
     */
    void RestoreStorageDataL(RDesReadStream& aStream, TInt& aRestoreState, TInt& aBufferSpaceLeft);

private:

    // Default constructor for the class

    CStorageBackupUtil();

    // Second phase constructor function

    void ConstructL();

    /**
     * This method fills the vector with Java Storage data.
     * Data is read from JavaStorage tables and put inside the vector
     * which will later be put into the buffer stream.
     * The reason for having vector to store the data is that connection to
     * Java Storage cannot be open for a long period of time
     * ie writing into buffer stream's time
     *
     * @return returns an integer value telling whether the operation
     *         was completed successfully or not.
     */
    int FillVectorWithStorageData();

    /**
     * A utility function which converts a wstring into a TDesC
     * and writes it into the stream.
     *
     * @param stream an RDesWriteStream into which the converted
     *        string is written
     * @param tempString the wstring which is to be converted.
     */
    void WriteStringtoStreamL(RDesWriteStream& aStream, std::wstring aTempString);

    /**
     * A utility function which reads a TDesC from the stream, converts it
     * to a wstring and writes it into the vector.
     *
     * @param stream an RDesReadStream from which the TDesC data is read
     */
    void ReadStringfromStreamL(RDesReadStream& aStream);

    /**
     * This method writes the data to storage.
     * Gets called after FillVectorsWithStreamDataL() function is called.
     * This function opens a transaction to Storage and writes
     * the vector's data into storage row-by-row, and then commits
     * the transaction.
     *
     * @return returns an integer specifying whether the operation
     *         has completed successfully or not.
     */
    int WriteDataToStorage();
public:
    /**
     * Utility function which fills the vector with data got from storage.
     * Handles data of only APPLICATION_PACKAGE_TABLE.
     *
     * @param afoundEntries the application entries which match
     *        the SQL query.
     * @return an integer representing the number of rows written
     *         into the vector.
     */
    int FillVectorwithAppPackageTableData(java::storage::JavaStorageApplicationList_t& afoundEntries);

    /**
     * Utility function which fills the vector with data got from storage.
     * Handles data of only APPLICATION_TABLE.
     *
     * @param afoundEntries the application entries which match
     *        the SQL query.
     * @return an integer representing the number of rows written
     *         into the vector.
     */
    int FillVectorwithAppTableData(java::storage::JavaStorageApplicationList_t& afoundEntries);

    /**
     * Utility function which fills the vector with data got from storage.
     * Handles data of only APPLICATION_PACKAGE_ATTRIBUTES_TABLE.
     *
     * @param afoundEntries the application entries which match
     *        the SQL query.
     * @return an integer representing the number of rows written
     *         into the vector.
     */
    int FillVectorwithAppPackageAttTableData(java::storage::JavaStorageApplicationList_t& afoundEntries);

    /**
     * Utility function which fills the vector with data got from storage.
     * Handles data of only MIDP_PACKAGE_TABLE.
     *
     * @param afoundEntries the application entries which match
     *        the SQL query.
     * @return an integer representing the number of rows written
     *         into the vector.
     */
    int FillVectorwithMidpPackageTableData(java::storage::JavaStorageApplicationList_t& afoundEntries);

    /**
     * Utility function which fills the vector with data got from storage.
     * Handles data of only MIDP_PERMISSIONS_TABLE.
     *
     * @param afoundEntries the application entries which match
     *        the SQL query.
     * @return an integer representing the number of rows written
     *         into the vector.
     */
    int FillVectorwithMidpPermTableData(java::storage::JavaStorageApplicationList_t& afoundEntries);

    /**
     * Utility function which fills the vector with data got from storage.
     * Handles data of only MIDP_FUNCTION_GROUP_SETTINGS_TABLE.
     *
     * @param afoundEntries the application entries which match
     *        the SQL query.
     * @return an integer representing the number of rows written
     *         into the vector.
     */
    int FillVectorwithMidpFuncGrpSetTableData(java::storage::JavaStorageApplicationList_t& afoundEntries);

    /**
     * Utility function which fills the vector with data got from storage.
     * Handles data of only PUSH_REGISTRATION_TABLE.
     *
     * @param afoundEntries the application entries which match
     *        the SQL query.
     * @return an integer representing the number of rows written
     *         into the vector.
     */
    int FillVectorwithPushRegTableData(java::storage::JavaStorageApplicationList_t& afoundEntries);

    /**
     * Utility function which fills the vector with data got from storage.
     * Handles data of only ALARM_REGISTRATION_TABLE.
     *
     * @param afoundEntries the application entries which match
     *        the SQL query.
     * @return an integer representing the number of rows written
     *         into the vector.
     */
    int FillVectorwithAlarmRegTableData(java::storage::JavaStorageApplicationList_t& afoundEntries);

    /**
     * Utility function which fills the vector with data got from storage.
     * Handles data of only RUNTIME_SETTINGS_TABLE.
     *
     * @param afoundEntries the application entries which match
     *        the SQL query.
     * @return an integer representing the number of rows written
     *         into the vector.
     */
    int FillVectorwithRuntimeSetTableData(java::storage::JavaStorageApplicationList_t& afoundEntries);

    /**
     * Utility function which fills the vector with data got from storage.
     * Handles data of only PREINSTALL_TABLE.
     *
     * @param afoundEntries the application entries which match
     *        the SQL query.
     * @return an integer representing the number of rows written
     *         into the vector.
     */
    int FillVectorwithPreinstallTableData(java::storage::JavaStorageApplicationList_t& afoundEntries);

    /**
     * Utility function which fills the vector with data got from storage.
     * Handles data of only OTA_STATUS_TABLE.
     *
     * @param afoundEntries the application entries which match
     *        the SQL query.
     * @return an integer representing the number of rows written
     *         into the vector.
     */
    int FillVectorwithOtaStatusTableData(java::storage::JavaStorageApplicationList_t& afoundEntries);

    /**
     * Prints the vectors
     */
    void printVector();

private:

    /**
     * The actual wstring vector which holds the data during a B&R operation.
     * During a backup, this vector is filled with storage data and then
     * written to the stream.
     * During a restore, this vector is filled with data from the stream
     * before it is written to storage.
     * Own
     */
    std::vector< std::wstring > iStringVector;

    /**
     * An integer which holds the length of the string.
     * When only half the data is read from the stream, this holds the
     * length of the remaining data which is to be read.
     * Own
     */
    int iLenOfString;

    /**
     * An integer which holds the count of the number of strings
     * in the vector. Used during a B&R operation.
     * Own
     */
    int iStrCount;

    /**
     * An integer array which holds the number of rows of each table
     * present in storage.
     * This information is written to stream and is used when the
     * restored data is written back to storage.
     * Own
     */
    int iTableSize[NUMBER_OF_TABLES];

    /**
     * A Boolean value which says if a string was only
     * half read during a previous iteration.
     * Own
     */
    TBool iRemainingString;

    /**
     * A wstring which holds the half read string so that the
     * completed string can be appended to it during the next iteration
     * before it is put into the vector.
     * Own
     */
    std::wstring iHalfReadString;

    /**
     * Shows if it's the fill call to backup storage data.
     * Vectors filled only at the first call.
     * Own
     */
    TBool iFirstCalltoBackupStorageData;

    /**
     * Shows if it's the fill call to restore storage data.
     * Own
     */
    TBool iFirstCalltoRestoreStorageData;

    /**
     * Shows the remaining number of bytes available in the stream.
     * Own
     */
    int iBufferSpaceLeft;
};

} // namespace backup
} // namespace java

#endif // JAVASTORAGEBACKUPUTIL_H
