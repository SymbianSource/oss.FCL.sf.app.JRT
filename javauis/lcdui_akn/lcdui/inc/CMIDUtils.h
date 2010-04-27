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
* Description:  ?Description
*
*/


#ifndef CMIDUTILS_H
#define CMIDUTILS_H

#include <coecntrl.h>
#include <gulcolor.h>
// MMIDUtils interface
#include <lcdui.h>

#ifdef RD_INTELLIGENT_TEXT_INPUT
#ifdef RD_JAVA_S60_RELEASE_5_0_IAD
#include "javaptivariation.h"
#endif //RD_JAVA_S60_RELEASE_5_0_IAD
#endif // RD_INTELLIGENT_TEXT_INPUT
// FORWARD DECLARATIONS
class CHWRMLight;
class CHWRMVibra;
class CMIDUIManager;
class CMIDKeyDecoder;
class CMIDMenuHandler;

//#include <dsyenums.h> // contains enums needed by DosSvrServices

#ifdef RD_INTELLIGENT_TEXT_INPUT
class CPtiEngine;
class CRepository;
#endif // RD_INTELLIGENT_TEXT_INPUT

NONSHARABLE_CLASS(CMIDUtils) : public CBase, public MMIDUtils
{
    /**
     * Enumeration defines values for every keyboard type.
     * Values come from repository:
     * 0 No Keyboard
     * 1 Keyboard with 12 key
     * 2 QWERTY 4x12 layout
     * 3 QWERTY 4x10 layout
     * 4 QWERTY 3 x 11 layout
     * 5 Half QWERTY
     * 6 Custom QWERTY
     */
    enum TKeyboardTypes
    {
        ENoKeyboard,
        EKeyboardWith12Key,
        EQWERTY4x12Layout,
        EQWERTY4x10Layout,
        EQWERTY3x11Layout,
        EHalfQWERTY,
        ECustomQWERTY
    };

    enum TStickyKeyStatus
    {
        EKeyNotPressed,
        EKeyWasPressed,
        EKeyWasReleased,
    };

    enum TLockableKeyStatus
    {
        EKeyNotActive,
        EKeyActive,
        EKeyNotActiveNext,
        EKeyLocked,
    };

    // This class is storage for data needed for scaling.
    class TScalingData
    {
    public:
        inline TScalingData();
        inline TBool operator==(const TScalingData& aData) const;

        TSize iOriginalSize;
        TSize iTargetSize;
        TSize iScreenSize;
        TSize iCanvasSize;
        TReal iRatioX;
        TReal iRatioY;
    };

public:
    enum TDirections
    {
        EHorizontal,
        EVertical
    };

private:
    class TStickyKeysHandler
    {
    public:
        TStickyKeysHandler();
        void  Reset();
        TBool HandleKey(const TKeyEvent& aKeyEvent, TEventCode aType);
        TInt  Modifiers() const;

    private:
        void  ResetState();
        TUint ModifierFromScanCode(TInt aScanCode);
        TBool IsSticky(const TKeyEvent& aKeyEvent);
        void HandleLockableKeys(TUint aModifiers);
        void HandleFnKey();
        void HandleShiftKey();
        void AddShiftToStickyModifiers();
        void AddFnToStickyModifiers();
        inline TBool IsFnModifierOn(const TUint aModifiers) const;
        inline TBool IsShiftModifierOn(const TUint aModifiers) const;

    private:
        TStickyKeyStatus iStickyKeyState;
        TLockableKeyStatus iFnKeyState;
        TLockableKeyStatus iShiftKeyState;
        TInt iLastStickyScanCode;
        TUint iStickyModifier;
        TUint iLastKeyModifier;
        TUint iActualFnModifierValue;
        TUint iActualShiftModifierValue;
    };

public:
    /**
     * Static method to create MMIDUtils instance
     * @param aEnv Environment
     * @param aUIManager Manager
     * @return MMIDUtils instance
     *
     * @since s60
     */
    static MMIDUtils* NewL(MMIDEnv& aEnv, CMIDUIManager* aUIManager);

    //
    // Utility methods used within lcdui plugin
    //
    static void MapJavaToETextChars(HBufC* aText,TBool aSupportLineBreaks=ETrue);
    static void MapETextToJavaChars(HBufC* aText);
    static TInt GameAction(TInt aKeyCode);
    static TBool IsStronglyRightToLeft(HBufC* aText);
    static void Reverse(HBufC* aText);
    static TBool IgnoreKeyEvent(TUint aKeyEvent);
    static CFbsBitmap* CopyBitmapL(CFbsBitmap* aSource);

    /**
     * Scaling of one integer (as height).
     *
     * @param aNonScaled A non-scaled value.
     * @param aDirection A direction of value. Values are in TDirections.
     * @return A scaled value.
     *
     * @since S60 5.0
     */
    TInt DoScaling(TInt aNonScaled, TInt aDirection);

    /**
     * Scaling of TPoint value.
     *
     * @param aNonScaled A non-scaled value.
     * @return A scaled value.
     *
     * @since S60 5.0
     */
    TPoint DoScaling(TPoint aNonScaled);

    /**
     * Scaling and positioning of TPoint value.
     *
     * @param aNonScaled A non-scaled value.
     * @return A scaled value.
     *
     * @since S60 5.0
     */
    TPoint DoScalingAndPositioning(TPoint aNonScaled);

    /**
     * Scaling of TRect value.
     *
     * @param aNonScaled A non-scaled value.
     * @return A scaled value.
     *
     * @since S60 5.0
     */
    TRect DoScaling(TRect aNonScaled);

    /**
     * Scaling and positioning of TRect value.
     *
     * @param aNonScaled A non-scaled value.
     * @return A scaled value.
     *
     * @since S60 5.0
     */
    TRect DoScalingAndPositioning(TRect aNonScaled);

    /**
     * Scaling of TSize value.
     *
     * @param aNonScaled A non-scaled value.
     * @return A scaled value.
     *
     * @since S60 5.0
     */
    TSize DoScaling(TSize aNonScaled);

    /**
     * Descaling of one integer (as height).
     *
     * @param aNonScaled A scaled value.
     * @param aDirection A direction of value. Values are in TDirections.
     * @return A original non-scaled value.
     *
     * @since S60 5.0
     */
    TInt DoDescaling(TInt aNonScaled, TInt aDirection);

    /**
     * Descaling of TPoint value.
     *
     * @param aNonScaled A scaled value.
     * @return A original non-scaled value.
     *
     * @since S60 5.0
     */
    TPoint DoDescaling(TPoint aNonScaled);

    /**
     * Descaling and position to originalposition of TPoint value.
     *
     * @param aNonScaled A scaled value.
     * @return A original non-scaled value.
     *
     * @since S60 5.0
     */
    TPoint DoDescalingAndPositioning(TPoint aNonScaled);

    /**
     * Descaling of TRect value.
     *
     * @param aNonScaled A scaled value.
     * @return A original non-scaled value.
     *
     * @since S60 5.0
     */
    TRect DoDescaling(TRect aNonScaled);

    /**
     * Descaling and position to originalposition of TRect value.
     *
     * @param aNonScaled A scaled value.
     * @return A original non-scaled value.
     *
     * @since S60 5.0
     */
    TRect DoDescalingAndPositioning(TRect aNonScaled);

    /**
     * Descaling of TSize value.
     *
     * @param aNonScaled A scaled value.
     * @return A original non-scaled value.
     *
     * @since S60 5.0
     */
    TSize DoDescaling(TSize aNonScaled);

    /**
     * Reset of iScalingData
     *
     * @since S60 5.0
     */
    void ResetScalingData();

    /**
     * @return ETrue if aChar is one of the following characters: 0x0a, 0x0d, 0x2028
     */
    static TBool IsLineSeparator(const TText aChar);

    //
    // From MMIDUtils
    //
    TBool PlaySound(TInt aType);
    //
    // key utils
    TBool IsJavaKey(TInt aScanCode);
    TInt  MapNonUnicodeKey(TUint aScanCode);
    void  GetKeyName(TDes& aText,TInt aKeyCode);
    TInt  GetKeyCode(TInt aGameAction);
    TInt  GetGameAction(TInt aKeyCode);
    TBool HasPointerEvents();
    TBool HasPointerMotionEvents();
    TBool HasRepeatEvents();
    void GetKeyboardTypeName(TDes* aText);
    TInt GetKeyScanCode();
    void SetLastKeyEvent(const TKeyEvent& aEvent);
    TInt GetKeyModifier();
    void MappingDataForKey(TKeyEvent& aEvent, TEventCode aType);
    void HandleResourceChangedL();
    void HandleForegroundL(TBool aForeground);
    //
    // Display utils
    TBool FlashBacklightL(const TTimeIntervalMicroSeconds32& aDuration);
    TBool Vibrate(const TTimeIntervalMicroSeconds32& aDuration);
    TInt Color(TColorType aColorSpecifier);
    TGraphicsType BorderStyle(TBool aHighlighted);
    TSize BestImageSize(TImageType aImageType) const;
    //
    // Font utils
    SFontSpec FontSpecifierSpecs(MMIDFont::TFontSpecifier aSpecifier);
    //
    // Destruction
    inline void Dispose();
    //

private:
    /**
     * Ctor
     * @param aEnv Environment
     * @param aUIManager Manager
     *
     * @since s60
     */
    CMIDUtils(MMIDEnv& aEnv, CMIDUIManager* aUIManager);

    ~CMIDUtils();
    void ConstructL();

    //
    TBool HasPen();
    TBool HasMouse();

    /**
     * Gets a sizes neeeded for futher function of scaling functions.
     *
     * @return Initialized instance of TScalingData with actual content.
     *
     * @since S60 5.0
     */
    CMIDUtils::TScalingData GetScalingData();

    void UpdateStickyKeyFlags(const TKeyEvent& aKeyEvent);
    void UpdatePTIEngineStatusL();

#ifdef RD_INTELLIGENT_TEXT_INPUT
#ifdef RD_JAVA_S60_RELEASE_5_0_IAD
    void CallToJavaPtiVariationL(TInt aType);
#endif //RD_JAVA_S60_RELEASE_5_0_IAD    
#endif //RD_INTELLIGENT_TEXT_INPUT

private:
    MMIDEnv* iEnv;

    // Light Client API, owned
    CHWRMLight* iLight;

    // Vibra API, owned
    CHWRMVibra* iVibra;
    CMIDUIManager* iUIManager;
    CMIDKeyDecoder* iKeyDecoder;

    TScalingData iScalingData;


#ifdef RD_INTELLIGENT_TEXT_INPUT
    /**
     * Predictive text input engine for canvas low level key events.
     * Own.
     */
    CPtiEngine* iPtiEngine;

    /**
     * Acces to a repository ( Fep input txt lang )
     * Own.
     */
    CRepository* iRepository;

#ifdef RD_JAVA_S60_RELEASE_5_0_IAD
    RLibrary iPtiSupportLib;
#endif //RD_JAVA_S60_RELEASE_5_0_IAD

#endif // RD_INTELLIGENT_TEXT_INPUT

    TInt iQwertyMode;

    TInt iStickyKey;

    // Stores value of the scan code of the latest key event.
    // Value is used for com.nokia.key.scancode system property.
    TInt iLastScanCode;

    TInt iModifier;

    TStickyKeysHandler iStickyHandler;

    // Instace of menu handler. Needed for scaling. Not owned.
    CMIDMenuHandler* iMenuHandler;
};

inline void CMIDUtils::Dispose()
{
    delete this;
}

inline CMIDUtils::TScalingData::TScalingData(): iOriginalSize(),
        iTargetSize(),
        iScreenSize(),
        iCanvasSize(),
        iRatioX(1.0f),
        iRatioY(1.0f)
{}

inline TBool CMIDUtils::TScalingData::operator==(const TScalingData& aData) const
{
    if (iOriginalSize == aData.iOriginalSize
            && iTargetSize == aData.iTargetSize
            && iScreenSize == aData.iScreenSize
            && iCanvasSize == aData.iCanvasSize
            && iRatioX == aData.iRatioX
            && iRatioY == aData.iRatioY)
    {
        return ETrue;
    }
    return EFalse;
}

#endif // CMIDUTILS_H
