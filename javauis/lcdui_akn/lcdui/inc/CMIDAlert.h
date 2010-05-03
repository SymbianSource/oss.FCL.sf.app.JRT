/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Alert implementation for Series 60
*
*/


#ifndef CMIDALERT_H
#define CMIDALERT_H

// INCLUDES
// MMIDAlert interface in class declaration
// and some other MMID classes used as parameters in several functions
#include <lcdui.h>
// using for CCoeControl class in inheritance
// using for CEikImage class for CEikImage* CreateImageLC() function
// using TTone enumeration
#include <AknPopupform.h>
// CONSTANTS
const TInt KAlertModalCommandCount = 2;

// FORWARD DECLARATIONS
class CMIDAlertDialog;
class MMIDBitmapImage;
class CMIDGaugeItem;
class CMIDDisplayable;

// CLASS DECLARATION
NONSHARABLE_CLASS(CMIDAlert) : public CCoeControl, public MMIDAlert
{

public:
    static CMIDAlert* NewL(
        MMIDEnv& aEnv,
        TAlertType aType,
        MMIDDisplayable& aDisplayable,
        const TDesC& aString,
        MMIDImage* aImage,
        MMIDUtils* aUtils);

public: // Called by the displayable, activates and destroies the alert
    void HandleCurrentL(TBool aCurrent);
    inline CMIDAlertDialog* Dialog() const;

public: // MMIDAlert
    void SetStringL(const TDesC& aString);
    void SetImageL(MMIDImage* aImage);
    void SetIndicatorL(MMIDGauge* aGauge);
    void SetTypeL(TAlertType aType);
    TInt DefaultTimeout();
    void SetTimeoutL(TInt aTime);
    void SetModalL();
    TBool IsModal();

    void SetTitleL(const TDesC* aTitle);
    TBool IsScrollable();

    TInt PlaySound();

public: // MMIDComponent
    inline void Dispose();

public: // Called by CMIDAlertDialog
    void TryToDismissDialog();

#ifdef RD_SCALABLE_UI_V2
    /**
    * Used by CMIDAlertDialog in detecting long taps.
    * This function call is forwarded to CMIDDisplayable which contains
    * the actual implementation.
    * @param    aPointerEvent The pointer event.
    * @return   ETrue, if the long tap was detected.
    */
    TBool TryDetectLongTapL(const TPointerEvent& aPointerEvent);
#endif

private:
    CMIDAlert(MMIDEnv& aEnv,TAlertType aType, MMIDUtils* aUtils);
    ~CMIDAlert();
    void ConstructL(MMIDDisplayable& aDisplayable,const TDesC& aString,MMIDImage* aImage);

private:
    TInt SetAlertResourceIdsL();
    void SetImageOrAnimToDialogL();

    CEikImage* CreateImageLC() const;
    void ResizeImage(CFbsBitmap* aImage, CFbsBitmap* aMask) const;

    void CreateDialogL();

private:
    static TInt TimerCallBack(TAny* aThis);
    void DoTimerCallBack();

protected:

    void HandleResourceChange(TInt aType);

private:
    MMIDEnv* iEnv;
    CMIDDisplayable* iDisplayable;

    /** The type of alert */
    TAlertType iAlertType;

    /** For timed alerts, the timeout in seconds */
    TInt iTimeout;

    /** The dialog itself */
    CMIDAlertDialog* iDialog;

    /** A user given dialog text */
    HBufC* iUserText;

    /** The dialog default text */
    HBufC* iDefaultText;

    /** The dialog image */
    MMIDBitmapImage* iBitmapImage;


    /** The dialog tone */
    TTone iAlertTone;

    /** Timed alerts are controlled by this timeout, we cannot use
    the dialog timeout because dialogs should not be automatically
    dismissed, only java side dismisses dialogs */
    CPeriodic* iDialogTimer;

    /** Used for playing a sound */
    MMIDUtils* iUtils;

    /** The dialog animation resource id */
    TInt iAlertAnimation;
};


inline CMIDAlertDialog* CMIDAlert::Dialog() const
{
    return iDialog;
}
#endif // CMIDALERT_H

// End of File
