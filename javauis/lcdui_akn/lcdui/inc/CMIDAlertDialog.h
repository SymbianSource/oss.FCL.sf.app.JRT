/*
* Copyright (c) 2003 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Alert dialog implementation for Series 60
*
*/


#ifndef CMIDALERTDIALOG_H
#define CMIDALERTDIALOG_H

// INCLUDES
// using MMIDGaugeToAlertListner interface in class declaration
// and CMIDNonInteractiveGauge as iGauge member
#include "CMIDGaugeItem.h"
#include <lcdui.h>
#include <AknPopupform.h>

// FORWARD DECLARATIONS
class CMIDAlert;

// CLASS DECLARATION
NONSHARABLE_CLASS(CMIDAlertDialog) : public CAknPopupForm, public MMIDGaugeToAlertListner
{

public:

    static CMIDAlertDialog* NewL(CMIDAlert* aAlert,CMIDDisplayable* aDisplayable);

    CMIDAlertDialog(CMIDAlert* aAlert,CMIDDisplayable* aDisplayable);
    ~CMIDAlertDialog();
    void SetGaugeL(CMIDGaugeItem* aGauge);
    void PreLayoutDynInitL();
    void PostLayoutDynInitL();

    void UpdateCbasL();
    TKeyResponse OfferKeyEventL(const TKeyEvent& aKeyEvent,TEventCode aType);
    TBool OkToExitL(TInt aCommandId);

    void InsertGaugeIntoDialogL();
    void CheckAndSetAnimationModeL();

    // from  MMIDGaugeToAlertListner
    void UpdateGaugeInAlert(TInt aValue);
    void GaugeTypeInAlertChangedL();

    void ShowL(TBool aShow);
    void DisableVisibilityChange(TBool aDisable);

public: // from CCoeControl
    void MakeVisible(TBool aVisible);
    void SizeChanged();
#ifdef RD_SCALABLE_UI_V2
    void HandlePointerEventL(const TPointerEvent& aPointerEvent);
#endif

protected: //MMIDGaugeToAlertListner
    void UpdateGaugeInAlertL(TInt aValue);

    void HandleResourceChange(TInt aType);

#ifdef RD_SCALABLE_UI_V2
private:
    /**
    * Checks if the point given as a parameter belongs to the dialog region.
    * @param  aPoint   The point
    * @return TBool indicating if the point is in the dialog region.
    */
    TBool AlertRegionContains(const TPoint& aPoint);
#endif

private:
    CMIDAlert* iParent;
    CMIDDisplayable* iDisplayable;

    CMIDNonInteractiveGauge* iGauge;
    TPopupFormControlType iGaugeResourceType;

    TInt iGaugeFrameCount;
    TInt iIncrementalFrameCounter;

    TBool iShowing;

    TBool iVisibilityChangeDisabled;
#ifdef RD_SCALABLE_UI_V2
    // When closing timed alerts, this boolean is needed in tracking
    // that at least one EButton1Down has been received
    TBool iButtonDownReceived;
#endif
};

#endif // CMIDALERTDIALOG_H

// End of File
