/*
* Copyright (c) 2005-2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Scans for matching jar files
*
*/


#ifndef JCFJADJARMATCHERSCANJARFILES_H
#define JCFJADJARMATCHERSCANJARFILES_H

#include <memory>

#include "jcfjadjarmatcherstates.h"

/**
* Scanner class.
*  Search matching JAR files.
*
*  @lib JcfJadJarMatcher.lib
*  @since S60 2.0
*/
class CJcfJadJarMatcherScanJarFiles : public CJcfJadJarMatcherState
{
public:
    /**
     * To construct new CJcfJadJarMatcherScanJarFiles.
     *
     * Preconditions. The following conditions must be true prior to calling
     * this method; otherwise, its operation is undefined.
     * - None.
     *
     * Postconditions. The following condition is true immediately after
     * returning from this method.
     * - None.
     *
     * @param aObs - an observer of this object.
     * @param aFs - a reference to the file server session.
     */
    CJcfJadJarMatcherScanJarFiles(MJcfJadJarMatcherObserver* aObs, RFs& aFs);

    /**
     * To destruct CJcfJadJarMatcherScanJarFiles.
     *
     * Preconditions. The following conditions must be true prior to calling
     * this method; otherwise, its operation is undefined.
     * - None.
     *
     * Postconditions. The following condition is true immediately after
     * returning from this method.
     * - No pending request.
     *
     */
    virtual ~CJcfJadJarMatcherScanJarFiles();

    /**
     * To execute this state.
     *
     * Preconditions. The following conditions must be true prior to calling
     * this method; otherwise, its operation is undefined.
     * - aJarNamePtr.Length() >= 256.
     *
     * Postconditions. The following condition is true immediately after
     * returning from this method.
     * - iState == EOpenJadFile.
     *
     * @param aJadName - a name of the JAD file.
     * @param aDirName - a name of the directory to be scanned.
     * @param aJarNamePtr - a pointer to the descriptor, where name of the JAR file
     * is stored.
     * @leave Creation of the JAD file fails.
     * @leave Creation of iDir descriptor fails.
     */
    virtual void ExecuteL(const TDesC& aJadName, const TDesC& aDirName, TDes* aJarNamePtr);

protected:
    /**
     * To match JAR and JAD files from public folders.
     *
     * Preconditions. The following conditions must be true prior to calling
     * this method; otherwise, its operation is undefined.
     * - Successfull call of ExecuteL.
     *
     * Postconditions. The following condition is true immediately after
     * returning from this method.
     * - None.
     *
     * @leave If abnormal execution, e.g., allocation fails.
     */
    virtual void RunL();

    /**
     * To cancel this object activity.
     *
     * Preconditions. The following conditions must be true prior to calling
     * this method; otherwise, its operation is undefined.
     * - None.
     *
     * Postconditions. The following condition is true immediately after
     * returning from this method.
     * - No pending requests.
     *
     */
    virtual void DoCancel();

    /**
     * To check whether a file is possible JAR file or not.
     *
     * Definition. Operation returns false if at least one of following
     * statements is true:
     *
     * 1) The MIME type is defined and it is not MIME for JAR file.
     *
     * Otherwise operation returns true.
     *
     * Preconditions. The following conditions must be true prior to calling
     * this method; otherwise, its operation is undefined.
     * - None.
     *
     * Postconditions. The following condition is true immediately after
     * returning from this method.
     * - None.
     *
     * @param aAttaInfo - an attachment to be checked.
     * @return False if file is not JAR file.
     */
    //TBool IsFilePossibleJarFileL( const TDesC& fileName );

private:
    /**
     * To do clean-up for next round.
     *
     * Preconditions. The following conditions must be true prior to calling
     * this method; otherwise, its operation is undefined.
     * - None.
     *
     * Postconditions. The following condition is true immediately after
     * returning from this method.
     * - iState == EBegin.
     *
     */
    void Cleanup();

    /**
     * To complete pending request and set this object active.
     *
     * Preconditions. The following conditions must be true prior to calling
     * this method; otherwise, its operation is undefined.
     * - None.
     *
     * Postconditions. The following condition is true immediately after
     * returning from this method.
     * - iActive == ETrue.
     * - iStatus == KErrNone.
     *
     */
    void SetObjectActive();

private:
    TFindFile iFf;
    TBool iAdHocGuess;
    TBool iIsAbsolute;
    TInt iIndex;
    TDes* iJarFileNamePtr;
    HBufC* iDir;
    CDir* iFl;
    RPointerArray<Java::MJavaAttribute>iAttributes;

    RFs& iFs;
    std::auto_ptr<HBufC> iFullJadName;

};

#endif // JCFJADJARMATCHERSCANJARFILES_H
