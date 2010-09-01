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
* Description:  Scans for matching jad files
*
*/


#ifndef JCFJADJARMATCHERSCANJADFILES_H
#define JCFJADJARMATCHERSCANJADFILES_H

#include <memory>

#include "jcfjadjarmatcherstates.h"

/**
* Scanner class.
*  Search matching JAD files.
*
*  @lib JcfJadJarMatcher.lib
*  @since S60 2.0
*/
class CJcfJadJarMatcherScanJadFiles : public CJcfJadJarMatcherState
{
public:
    /**
     * To construct new CJcfJadJarMatcherScanJadFiles.
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
    CJcfJadJarMatcherScanJadFiles(MJcfJadJarMatcherObserver* aObs, RFs& aFs);

    /**
     * To destruct CJcfJadJarMatcherState.
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
    virtual ~CJcfJadJarMatcherScanJadFiles();

    /**
     * To execute this state.
     *
     * Preconditions. The following conditions must be true prior to calling
     * this method; otherwise, its operation is undefined.
     * - aJadNamePtr.Length() >= 256.
     *
     * Postconditions. The following condition is true immediately after
     * returning from this method.
     * - iState == EOpenJarFile.
     *
     * @param aJarName - a name of the JAR file.
     * @param aDirName - a name of the directory to be scanned.
     * @param aJadNamePtr - a pointer to the descriptor, where name of the JAD file
     * is stored.
     * @return None.
     * @leave Creation of the JAR file fails.
     * @leave Creation of iDir descriptor fails.
     */
    virtual void ExecuteL(const TDesC& aJarName, const TDesC& aDirName, TDes* aJadNamePtr);

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
    TMode iMode;
    TInt iIndex;
    TDes* iJadFileNamePtr;
    HBufC* iDir;
    CDir* iFl;
    RPointerArray<Java::MJavaAttribute>iAttributes;
    std::auto_ptr<HBufC> iFullJarName;
    RFs& iFs;
};

#endif // JCFJADJARMATCHERSCANJADFILES_H

