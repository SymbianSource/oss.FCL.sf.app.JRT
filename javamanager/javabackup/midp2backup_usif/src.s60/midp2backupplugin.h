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
* Description:  Header file for CMidp2BackupPlugin class
*
*/


#ifndef MIDP2BACKUPPLUGIN_H
#define MIDP2BACKUPPLUGIN_H

#include "backupplugin.h"
#include <apadef.h>
#include "logger.h"

class RDesWriteStream;
class RDesReadStream;



namespace java
{
namespace backup
{

class CAppArcBackupUtil;
class CJavaVersionBackupUtil;
class CStorageBackupUtil;



/**
 *  CMidp2BackupPlugin class is handling backup and restore operations
 *  for MIDlets
 *
 *  This class is instantiated when a BUR operation is starting.
 */
class CMidp2BackupPlugin : public CBackupPlugin
{

public:

    /**
     * Instantiates an object of this type
     */
    static CMidp2BackupPlugin* NewL();

    ~CMidp2BackupPlugin();


    // from base class CBackupPlugin

    /**
     * This method is called when a backup or restore operation is
     * starting. Preparations can be done to prepare for BUR.
     *
     * @param aBackupStateValue the value of the current backup state
     */
    void PrepareForBURL(TInt aBackupStateValue);

    /**
     * This method receives all or part of a snapshot of data to allow
     * calculation of an incremental backup.  The snapshot is one that
     * was previously supplied by the data owner.  The snapshot data
     * should be read from the location supplied. The snapshot data may
     * be larger than the location supplied in which case the routine
     * will be called repeatedly until all data has been supplied.
     *
     * Snapshot data will also be supplied as part of a restore operation
     *
     * @param aDrive the drive being backed up
     * @param aBuffer a pointer to the base of the location from whence
     *        data can be copied.
     * @param aLastSection ETrue if this is the last section of snapshot
     *        data, else EFalse.
     */
    void ReceiveSnapshotDataL(TDriveNumber aDrive,
                              TDesC8& aBuffer,
                              TBool aLastSection);

    /**
     * This method returns the expected size of backup data that will be
     * supplied. If an incremental backup is underway then this method
     * will not be called until after ReceiveSnapshotDataL().
     * The size data will be used for the purpose of tracking progess
     * during a backup. If it is inaccurate then the user may see
     * irregular progress but the actual backup data will not be
     * affected so it is acceptable to return an estimated value.
     *
     * @param aDrive the drive being backed up.
     * @return the size of the data that will be returned
     */
    TUint GetExpectedDataSize(TDriveNumber aDrive);

    /**
     * This method returns a snapshot of data to accompany a backup. The
     * snapshot is expected to contain details on files / data being
     * backed up. The format of the snapshot is only meaningful to the
     * data owner. The snapshot will be supplied if the data owner is
     * asked for an incremental backup and for a restore operation. The
     * snapshot data should be copied to the location supplied.
     *
     * The snapshot data may be larger than the location supplied in
     * which case the routine will be called repeatedly until all data
     * has been retrieved.
     *
     * @param aDrive the drive being backed up
     * @param aBuffer a pointer to the base of the location where data
     *        can be copied.
     * @param aFinished on return ETrue if all data has been returned
     *        for this drive, else EFalse.
     */
    void GetSnapshotDataL(TDriveNumber aDrive,
                          TPtr8& aBuffer,
                          TBool& aFinished);

    /**
     * This method prepares the implementor to return backup data. It
     * will be followed by a sequence of calls to request the actual
     * data.
     *
     * @param aDrive the drive being backed up.
     */
    void InitialiseGetBackupDataL(TDriveNumber aDrive);

    /**
     * This method requests a section of backup data.
     * InitialiseGetBackupDataL() will have been called previously to
     * specify the drive concerned.  The data returned may be base or
     * incremental depending on the type of backup and the capability of
     * the data owner.
     *
     * @param aBuffer a pointer to the base of the location where data
     *        can be copied.
     * @param aFinished on return ETrue if all data has been returned
     *        for this drive, else EFalse.
     */
    void GetBackupDataSectionL(TPtr8& aBuffer,
                               TBool& aFinished);

    /**
     * This method prepares the implementor to receive base restore data
     * for a drive. It will be followed by a sequence of calls to supply
     * the actual data.
     *
     * @param aDrive the drive being restored.
     */
    void InitialiseRestoreBaseDataL(TDriveNumber aDrive);

    /**
     * This method receives a section of base restore data.
     * InitialiseRestoreBaseDataL() will have been called previously to
     * specify the drive concerned.
     *
     * @param aBuffer a pointer to the base of the location whence data
     *        can be read.
     * @param aFinished ETrue if all data has been returned for this
     *        drive, else EFalse.
     */
    void RestoreBaseDataSectionL(TDesC8& aBuffer,
                                 TBool aFinished);

    /**
     * This method prepares the implementor to receive incremental
     * restore data for a drive. It will be followed by a sequence
     * of calls to supply the actual data.  If multiple increments
     * are supplied then this methid will be called before each increment
     *
     * @param aDrive the drive being restored.
     */
    void InitialiseRestoreIncrementDataL(TDriveNumber aDrive);

    /**
     * This method receives a section of increment restore data.
     * InitialiseRestoreIncrementDataL() will have been called
     * previously to specify the drive concerned.
     *
     * @param aBuffer a pointer to the base of the location whence data
     *        can be read.
     * @param aFinished ETrue if all data has been returned for this
     *        increment, else EFalse.
     */
    void RestoreIncrementDataSectionL(TDesC8& aBuffer,
                                      TBool aFinished);

    /**
     * This method is called when all data to be restored has been
     * supplied.
     *
     * @param aDrive the drive being restored.
     */
    void RestoreComplete(TDriveNumber aDrive);

private:

    CMidp2BackupPlugin();

    void ConstructL();

    /**
     * This method is called to get the unique Id of the current MMC
     *
     * @return The Id of the MMC, or 0, if no MMC is inserted
     */
    TUint MmcIdL();

    /**
     * This method is called to process the content of the buffer
     * provided in active phase of restore
     *
     * @param aBuffer the buffer to process
     */
    void ProcessBufferL(RDesReadStream& stream);

    /**
     * This method is called to restore the Java Registry entries
     * on the MMC, if needed
     */
    void RestoreMmcRegistryL();

    /**
     * This method creates the path for the Java Registry entries on
     * the specified MMC
     *
     * @param [out] aPathName path of the entries of the MMC
     * @param aDrive drive of the MMC
     * @param aUniqueId Unique Id of the MMC
     */
    void CreateMmcPath(TDes& aPathName,
                       TInt aDrive,
                       TUint aUniqueId);

    /**
     * This method gets the path for the next icon file to be backed up
     *
     * @param [out] aCurrentUid Uid of the next icon
     * @param [out] aFullFileName path of the next icon
     * @return EFalse if there are no more icons in the array, ETrue
     *         otherwise
     */
    TBool NextIcon(TUid& aCurrentUid, HBufC*& aFullFileName);

    /**
     * Process buffer when restore state is EFirstBuffer
     *
     * @param aStream readstream of buffer to process
     * @param aBufferSpaceLeft space left of buffer to be processed
     */
    void ProcessFirstBufferL(RDesReadStream& aStream, TInt& aBufferSpaceLeft);

    /**
     * Process buffer when restore state is EInIcon
     *
     * @param aStream readstream of buffer to process
     * @param aBufferSpaceLeft space left of buffer to be processed
     */
    void ProcessInIconL(RDesReadStream& aStream, TInt& aBufferSpaceLeft);

    /**
     * Process buffer when restore state is EInSize
     *
     * @param aStream readstream of buffer to process
     * @param aBufferSpaceLeft space left of buffer to be processed
     */
    void ProcessInSizeL(RDesReadStream& aStream, TInt& aBufferSpaceLeft);

    /**
     * Resets the array of icons to be backed up
     */
    void ResetIconArray();

private: // data

    /**
     * File session
     * Own.
     */
    RFs iFs;

    /**
     * AppArcBackupUtil object for AppArc operations
     * Own.
     */
    CAppArcBackupUtil* iAppArcUtil;

    /**
     * StorageBackupUtil object for storage B&R
     * Own.
     */
    CStorageBackupUtil* iStorageBackupUtil;
    /**
     * The drive that is backed up or restored currently
     * Own.
     */
    TDriveNumber iDrive;

    /**
     * Id of backed up drive, gets its value from restored data
     * Own.
     */
    TInt32 iBackupDrive;

    /**
     * Id of backed up MMC, gets its value from restored data
     * Own.
     */
    TUint32 iBackupMmc;

    /**
     * State of streaming during restore
     * Own.
     */
    TInt iRestoreState;

    /**
     * Shows if it's the first call to GetBackupDataSectionL
     * Own.
     */
    TBool iFirstCallToGetBackupDataSection;

    /**
     * Shows if Backup of storage data is finished or not
     * Own.
     */
    TBool iStorageDataBackup;

    /**
     * Shows if Restore of storage data is finished or not
     * Own.
     */
    TBool iStorageDataRestore;

    /**
     * Shows if Java version information is already written or not
     * Own
     */
    TBool iJavaVersionInfoWritten;

    /**
     * Shows if its the first call to restore data or not.
     * Own.
     */
    TBool iFirstCallToRestoreBackupDataSection;

    /**
     * Stores the uids of the midlets whose icons are to be backed up.
     * Own.
     */
    RArray<TUid> iIconUidArray;

    /**
     * The current index in the icon file array
     * Own.
     */
    TInt iIconFilePointer;

    /**
     * Stores the current icon file for streaming
     * This descriptor will be streamed at subsequent calls of
     * GetBackupDataSectionL()
     * Own.
     */
    HBufC8* iIconDesc;

    /**
     * Stores the current icon file being restored
     * Own.
     */
    HBufC8* iRestoreIconDesc;

    /**
     * Size of the icon to be still read during restore
     * Own.
     */
    TInt32 iIconFileSizeLeft;

    /**
     * Current index of iIconDesc
     * Own.
     */
    TInt iIconDescIndex;

    /**
     * Holds the remaining bytes in the buffer
     * Own
     */

    TInt iBufferSpaceLeft;

    /**
     * Buffer for storing data size in restore
     * Own.
     */
    TBuf8<4> iSizeBuffer;
};


 
} //namespace backup
} //namespace java

#endif // MIDP2BACKUPPLUGIN_H
