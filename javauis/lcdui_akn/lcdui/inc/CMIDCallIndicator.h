/*
* Copyright (c) 2008-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  ?Description
*
*/


#ifndef CMIDCALLINDICATOR_H
#define CMIDCALLINDICATOR_H

#include <e32base.h>
//using CTelephony class
#include <etel3rdparty.h>
//using CMIDCanvas class - needed for CMIDNetworkIndicator
class CMIDCanvas;
//using CMIDNetworkIndicator class - iNetworkIndicator
class CMIDNetworkIndicator;

NONSHARABLE_CLASS(CMIDCallIndicator)  : public CActive
{
public:
    /**
     * Constructor with parameters.
     *
     * @see CMIDCallIndicator::NewL()
     */
    CMIDCallIndicator(CMIDNetworkIndicator *aNetworkIndicator);

    /**
     * Two-phased constructor.
     *
     * @param aNetworkIndicator     Pointer to a NetworkIndicator.
     */
    static CMIDCallIndicator* NewL(CMIDNetworkIndicator *aNetworkIndicator);

    /**
     * Two-phased constructor.
     *
     * @see CMIDCallIndicator::NewL().
     */
    static CMIDCallIndicator* NewLC(CMIDNetworkIndicator *aNetworkIndicator);

    /**
     * Destructor.
     */
    ~CMIDCallIndicator();

    /**
     * Activates the AO and register for notification by the CTelephony
     */
    void SetActiveLocal();

    /**
     * Finds out if call is ongoing when opening canvas and sets members.
     */
    void CMIDCallIndicator::Init();

    /**
     * Returns the call activity.
     *
     * @return Member iView. ETrue when active call indicator is shown =
     *       call is ongoing and EFalse otherwise.
     */
    TBool CMIDCallIndicator::GetCallActivity();

protected:
    /**
     * From CActive.
     */
    void RunL();

    /**
     * From CActive.
     */
    TInt RunError(TInt anError);

    /**
     * From CActive.
     */
    void DoCancel();

private:
    /**
     * Symbian 2-phase constructor.
     */
    void ConstructL();

    /*
     * Check the call status. If there is call established
     * return true orwise return false.
     *
     * @param   aCallStatus     call status.
     * @return  ETrue if calling else EFalse.
     */
    TBool CallInProgress(CTelephony::TCallStatus aCallStatus);

private: // Data

    /**
     * Not own.
     */
    CMIDNetworkIndicator* iNetworkIndicator;
    /**
     * Own.
     */
    CTelephony* iTelephony;
    CTelephony::TCallStatusV1 iLineStatus;
    CTelephony::TCallStatusV1Pckg iLineStatusPckg;

    /**
     * Indicates the call and indicator status
     * ETrue  - call is ongoing and icon is shown
     * EFalse - no active call, no active call icon is shown
     */
    TBool iView;

};

#endif // CMIDCALLINDICATOR_H
