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
* Description:  Header file for CAppArcBackupUtil class
*
*/


#ifndef APPARCBACKUPUTIL_H
#define APPARCBACKUPUTIL_H

#include <e32base.h>
#include <apgcli.h>
#include <apadef.h>

#include "javastorage.h"
#include "javastorageentry.h"
#include "javastoragenames.h"

namespace java
{
namespace backup
{

/**
 *  CAppArcBackupUtil class is handling AppArc registrations during backup
 *  and restore of Java Domain.
 */
class CAppArcBackupUtil : public CBase
{

public:

    /**
     * Instantiates an object of this type
     */
    static CAppArcBackupUtil* NewL(RFs& aFs);

    ~CAppArcBackupUtil();

    /**
     * This method registers applications to AppArc during restore.
     *
     * @param restoreIconPtr the buffer which contains the Uid and the
     *        icon file to be restored
     * @param aDrive current drive to be restored
     */
    void RegisterAppL(TPtr8& aRestoreIconPtr, TDriveNumber& aDrive);

    /**
     * This method deregisters leftover applications from AppArc during
     * restore.
     * @param aDrive current drive to be restored
     */
    void DeregisterAppsL(TDriveNumber aDrive);

    /**
     * This method builds an array of the registered apps in AppArc.
     *
     * @param [out] aUidArray array of Midlet uids
     * @param aDrive current drive to be backed up
     */
    void GetMidletsFromAppArcL(RArray<TUid>& aUidArray, TDriveNumber aDrive);

    /**
     * This method looks up the icon filename of the given midlet
     *
     * @param aUid Uid of Midlet
     * @param [out] aFullFilename filename of icon file
     */
    void GetIconFilename(TUid aUid, HBufC*& aFullFilename);

    /**
     * This method gets the number of MIDlet icons registered to AppArc
     * restore.
     *
     * @return iNumberOfIcons
     */
    TInt NumberOfIcons();

    /**
     * Gets the number of icons defined for a midlet from AppArc
     *
     * @param aUid Midlet Id
     * @param [out] aIconCount number of icons defined for the midlet
     */
    void NumberOfOwnDefinedIcons(TUid aUid, TInt& aIconCount);

    /**
     * Gets an entry from Java Storage for the midlet uid specified
     *
     * @param midletUid Uid of the midlet
     *
     * @return returns the entry of the midlet
     */
    java::storage::JavaStorageApplicationEntry_t StorageEntryL(const TUid& aMidletUID);

    /**
     * Gets the Drive of the midlet suite for the specified UID
     *
     * @param packageID the ID of the application package
     *
     * @return returns the drive number of the application package
     */

    TDriveNumber GetDrivefromStorageL(std::wstring& aPackageID);

    /**
     * Gets the group name of the midlet for the pecified UID
     *
     * @param aUid the UID of the midlet
     *
     * @return returns the group name of the midlet
     */

    TApaAppGroupName GetMidletGroupName(TUid aUid);

private:

    CAppArcBackupUtil();

    void ConstructL(RFs& aFs);

    /**
     * Checks if midlet is already registered to AppArc.
     *
     * @param aUid Uid of Midlet
     * @return ETrue if midlet is already registered to AppArc
     */
    TBool CheckIfAlreadyRegisteredL(TInt& aUid);

    /**
     * Gets the drive of the Midlet based on the path
     *
     * @param aPath Full path of Midlet
     * @param [out] aDrive the drive where the midlet is registered to
     */
    void GetDriveOfApp(TFileName aPath, TInt& aDrive);

    /**
     * Gets the index of the midlet in its midlet suite
     *
     * @param aMidletSuiteEntry Midlet suite
     * @param aUid Midlet Id
     * @return index of midlet in the suite
     */
    //TInt MidletIndexL( CJavaRegistrySuiteEntry &aMidletSuiteEntry,
    //                   TUid aUid );


private: // data

    /**
     * File session
     * Not own.
     */
    RFs iFs;

    /**
     * Number of MIDlet icons registered to AppArc
     * Own.
     */
    TInt iNumberOfIcons;

    /**
     * ApparcSession
     * Own.
     */
    RApaLsSession iApparcServer;

};

} //namespace backup
} //namespace java

#endif // APPARCBACKUPUTIL_H
