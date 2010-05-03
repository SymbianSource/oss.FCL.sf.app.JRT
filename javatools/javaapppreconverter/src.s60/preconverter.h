/*
* Copyright (c) 2008 - 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: Java platform 2.0 javaapppreconverter process
*
*/


#ifndef PRECONVERTER_H
#define PRECONVERTER_H

#include <f32file.h>
#include <e32base.h>

#include "midletlist.h"

/**
 * The main active object of javaapppreconverter that is executed once
 * to collect all necessary data of old S60 MIDlets so that they can
 * be reregistered to AppArc if OMJ is uninstalled.
 *
 * This active object is activated from the process main() after the
 * active scheduler has been created.
 */
class CPreConverter : public CActive
{
    enum TPreConversionState
    {
        EConversionDataAlreadyGathered,
        EFindOutInstalledMidlets,
        EFillDataFromSystemAmsDb,
        EStoreData,
        EStopMidlets,
        EUnregisterMidlets,
        EExit
    };

public:
    static CPreConverter* NewLC(RFs& aFs);

    void CompleteRequest();
    ~CPreConverter();
    void Start();

protected:
    // From CActive
    void RunL();
    void DoCancel();
    TInt RunError(TInt aError);

private:
    void Exit();
    CPreConverter(RFs& aFs);
    void ConstructL();
    void FullCleanup();

    TDriveNumber ExtractDriveNumberFromPathL(TFileName &aPathName);
    void ReadMidletsFromAppArcL();
    void AddDataFromSystemAmsDbL();
    void RemoveInvalidMidlets();
    void StoreUidsL();
    void UnregisterOldJavaAppsL();


private:
    RFs& iFs;
    CMidletList *iMidlets;
    TPreConversionState iState;

};

#endif // PRECONVERTER_H
