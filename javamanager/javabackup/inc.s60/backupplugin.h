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
* Description:  Backup Plugin interface
*
*/


#ifndef BACKUPPLUGIN_H
#define BACKUPPLUGIN_H

#include <e32base.h>
#include <ecom/ecom.h>
#include "javauids.h"

namespace java
{
namespace backup
{

// UID of this interface
const TUid KBackupEcomIfTUid = { KBackupEcomIfUid };


/**
 * Backup Plugin abstract class.
 */
class CBackupPlugin : public CBase
{

public:

    /**
     * Instantiates an object of this type
     * using the aMatchString as the resolver parameters.
     *
     * @param aMatchString String identifier of the implementation
     */
    inline static CBackupPlugin*  NewL(const TDesC8& aMatchString);

    /**
     * Lists all implementations of this interface
     *
     * @param aImplInfoArray Array containing the info of implementations
     */
    inline static void ListAllImplementationsL(RImplInfoPtrArray& aImplInfoArray);

    inline virtual ~CBackupPlugin();

    /**
     * This method is called when a backup or restore operation is
     * starting. Preparations can be done to prepare for BUR.
     *
     * @param aBackupStateValue the value of the current backup state
     */
    virtual void PrepareForBURL(TInt aBackupStateValue) = 0;

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
    virtual void ReceiveSnapshotDataL(TDriveNumber aDrive,
                                      TDesC8& aBuffer,
                                      TBool aLastSection) = 0;

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
    virtual TUint GetExpectedDataSize(TDriveNumber aDrive) = 0;

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
    virtual void GetSnapshotDataL(TDriveNumber aDrive,
                                  TPtr8& aBuffer,
                                  TBool& aFinished) = 0;

    /**
     * This method prepares the implementor to return backup data. It
     * will be followed by a sequence of calls to request the actual
     * data.
     *
     * @param aDrive the drive being backed up.
     */
    virtual void InitialiseGetBackupDataL(TDriveNumber aDrive) = 0;

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
    virtual void GetBackupDataSectionL(TPtr8& aBuffer,
                                       TBool& aFinished) = 0;

    /**
     * This method prepares the implementor to receive base restore data
     * for a drive. It will be followed by a sequence of calls to supply
     * the actual data.
     *
     * @param aDrive the drive being restored.
     */
    virtual void InitialiseRestoreBaseDataL(TDriveNumber aDrive) = 0;

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
    virtual void RestoreBaseDataSectionL(TDesC8& aBuffer,
                                         TBool aFinished) = 0;

    /**
     * This method prepares the implementor to receive incremental
     * restore data for a drive. It will be followed by a sequence
     * of calls to supply the actual data.  If multiple increments
     * are supplied then this methid will be called before each increment
     *
     * @param aDrive the drive being restored.
     */
    virtual void InitialiseRestoreIncrementDataL(TDriveNumber aDrive) = 0;

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
    virtual void RestoreIncrementDataSectionL(TDesC8& aBuffer,
            TBool aFinished) = 0;

    /**
     * This method is called when all data to be restored has been
     * supplied.
     *
     * @param aDrive the drive being restored.
     */
    virtual void RestoreComplete(TDriveNumber aDrive) = 0;

private:

    // Unique instance identifier key
    TUid iDtor_ID_Key;
};

} //namespace backup
} //namespace java

#include "backupplugin.inl"

#endif // BACKUPPLUGIN_H
