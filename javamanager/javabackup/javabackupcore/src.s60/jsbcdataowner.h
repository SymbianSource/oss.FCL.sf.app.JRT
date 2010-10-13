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
* Description:  Header file for Java Secure Backup Core JsbcDataOwner class
*
*/


#ifndef JSBCDATAOWNER_H
#define JSBCDATAOWNER_H

#include <abclient.h>

namespace java
{
namespace backup
{

// FORWARD DECLARATIONS
class CBackupPlugin;
class CJsbcActive;


/**
 *  CJsbcDataOwner class is implemeting the active backup of Java Domain
 */
class CJsbcDataOwner : public CBase,
        public conn::MActiveBackupDataClient
{

public:

    static CJsbcDataOwner* NewL(CJsbcActive* aJsbcActive);
    static CJsbcDataOwner* NewLC(CJsbcActive* aJsbcActive);
    ~CJsbcDataOwner();

    /**
     * This method is called when a backup or restore operation is
     * starting. Preparations can be done to prepare for BUR.
     *
     * @param aBackupStateValue the value of the current backup state
     */
    void PrepareForBURL(TInt aBackupStateValue);

    // from base class MActiveBackupDataClient

    /**
     * This method informs the active backup data client that all
     * snapshots have been supplied. If the client has not
     * received a snapshot then it should perform a base backup
     */
    void AllSnapshotsSuppliedL();

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
     * then this method will not be called until after
     * ReceiveSnapshotDataL(). The size data will be used for the purpose
     * of tracking progess during a backup. If it is inaccurate then the
     * user may see irregular progress but the actual backup data will
     * not be affected so it is acceptable to return an estimated value.
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

    /**
     * This method is called if copying of data is terminated prematurely
     * to allow the implementor to tidy up.  The same method applies to
     * all types of data and to backup and restore.
     */
    void TerminateMultiStageOperation();

    /**
     * Gets a 32-bit checksum for its private data.
     * This routine is for test purposes.  It must be implemented but an
     * invariant checksum value can be provided.  Some tests may cause
     * checksum values to be compared.
     *
     * @param aDrive the drive containing data being checksummed
     * @return the 32-bit checksum
     */
    TUint GetDataChecksum(TDriveNumber aDrive);

private:

    CJsbcDataOwner();

    void ConstructL(CJsbcActive* aJsbcActive);

private: // data

    /**
     * MIDP2 Backup Plugin
     * Own.
     */
    CBackupPlugin* iPlugin;

    /**
     * JSBCActive object (which instantiated this object)
     * Not own.
     */
    CJsbcActive* iJsbcActive;
};

} // namespace backup
} // namespace java

#endif // JSBCDATAOWNER_H
