/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Java platform 2.0 javaappbackconverter process
*
*/


#ifndef BACKCONVERTER_H
#define BACKCONVERTER_H

#include <f32file.h>
#include <e32base.h>
#include <apgcli.h>

#include "midletlist.h"

/**
 * The main active object of javaappbackconverter that is executed once
 * if OMJ is uninstalled to reregister the old S60 MIDlets back to AppArc.
 *
 * This active object is activated from the process main() after the
 * active scheduler has been created.
 */
class CBackConverter : public CActive
{
    enum TBackConversionState
    {
        EInitialize,
        EReadConversionData,
        EUnregisterOldMidletData,
        EReregisterMidlets,
        EExit
    };

public:
    static CBackConverter* NewLC(RFs& aFs);

    void CompleteRequest();
    ~CBackConverter();
    void Start();

protected:
    // From CActive
    void RunL();
    void DoCancel();
    TInt RunError(TInt aError);

private:
    void Exit();
    CBackConverter(RFs& aFs);
    void ConstructL();
    void FullCleanup();

    void RegisterMidletsL();
    void RegisterOneMidletL(RApaLsSession *aSession, CMidletInfo *aMidlet);
    void UnregisterOldMidletData();
    void RemoveDataFile();

private:
    RFs& iFs;
    CMidletList *iMidlets;
    TBackConversionState iState;

};

#endif // BACKCONVERTER_H
