/*
* Copyright (c) 2003, 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Menu pane handling, common across all Displayables.
*
*/


#ifndef CMIDMENUHANDLER_H
#define CMIDMENUHANDLER_H

//  INCLUDES
#include <eikbctrl.h>
// CEikMenuPaneItem (this is actually from eikmenup.h)
#include <eikmenub.h>
// MEikMenuObserver interface in class declaration
#include <eikmobs.h>

#include "CMIDTicker.h"

// FORWARD DECLARATIONS
class CMIDDisplayable;
class CMIDCanvas;
class CMIDDefaultBackground;
class CMIDUIManager;
class CEikMenuBar;
class CEikButtonGroupContainer;
class CEikAppUi;
class CMIDMenuHandler;
class CMIDTickerController;
class CMIDKeyDecoder;
class CMIDScaler;
class CEikonEnv;

// CLASS DECLARATION

// make a common place for the CMIDDisplayable and this const
const TInt KMenuBarResourceId = 0xfffd;
/**
*  CMIDMenuHandler handles the menu and CBA for all Displayables
*
*  @lib lcdui.dll
*  @since Series60 2.0
*/
NONSHARABLE_CLASS(CMIDMenuHandler) : public CBase, public MEikMenuObserver
{
public:
    enum TMenuType
    {
        ENoMenu=0,
        EOptionsMenu,
        EOkMenu,
        EHelpMenu,
#ifdef RD_JAVA_S60_RELEASE_9_2
        EScreenAndHelpMenu,
        EPopUpMenu
#else
        EScreenAndHelpMenu
#endif // RD_JAVA_S60_RELEASE_9_2          
    };

public:  // Constructors and destructor

    /**
    * Two-phased constructor.
    * Used to create the first instance, the use GetInstance to get pointer
    */
    static CMIDMenuHandler* NewL(MMIDEnv& aEnv);

    /**
    * Destructor.
    */
    ~CMIDMenuHandler();

public: // New functions

    /**
     * @since s60
     */
    TUint MenuItemsCount() const;

    /**
     * @since s60
     */
    void ShowMenuL(const TMenuType& aType);

    /**
     * @since s60
     */
    inline CEikMenuBar* MenuBar() const;

    /**
     * @since s60
     */
    inline CEikButtonGroupContainer* Cba();

    /**
     * @since s60
     */
    void SetDisplayable(CMIDDisplayable* aView);

    /**
     * @since s60
     */
    inline CMIDDisplayable* GetDisplayable();

    /**
     * Stops displaying menu
     * @since s60
     */
    void HideMenuIfVisible();

    /**
    * Contents of the menu is updated if the menu is currently displayed
    * @since ?Series60_version 3.1
    */
    void UpdateMenuIfVisibleL();

    /**
    * Checks if MIDlet's full-screen (game)canvas scaling is enabled.
    * @return   ETrue if scaling is enabled else EFalse.
     * @since s60
    */
    TBool IsScalingEnabled() const;

    /**
    * Returns Nokia-MIDlet-Original-Display-Size attribute size
    * as it is defined in jad file.
    * @return   Original MIDlet size if attribute is defined else
    *           zero-size.
    */
    inline TSize GetScalingParameterOrgMIDletScrSize() const;

    /**
    * Returns Nokia-MIDlet-Taregt-Display-Size attribute size
    * as it is defined in jad file.
    * @return   Target MIDlet size if attribute is defined else
    *           zero-size.
    */
    inline TSize GetScalingParameterTargetMIDletScrSize() const;

    /**
    * Checks if "MIDlet scaling on orientation switch" is allowed by
    * setting Nokia-MIDlet-allow-scaling-on-orientation-switch attribute
    * to true value.
    * @return   ETrue if feature is enabled else EFalse.
    */
    inline TBool GetScalingParameterScaleMIDletOnOrientSwitch() const;

    /**
     * Checks if the scaling is enabled in the current device screen.
     * Checks that Nokia-MIDlet-Original-Display-Size is defined and its
     * width and height are smaller than the current screen width and heigth.
     *
     * @return ETrue if scaling is enabled and effective in current screen.
     * @since S60 5.0
     */
    TBool IsScalingEffectiveInCurrentScreen() const;

    /**
    * This function returns current screen size.
    * @return size of screen
    */
    TSize GetScreenSize() const;

public: // From MEikMenuObserver

    /**
     *
     * @since s60
     */
    void ProcessCommandL(TInt aCommandId);

    /**
     *
     * @since s60
     */
    void SetEmphasis(CCoeControl* /*aMenuControl*/,TBool aEmphasis);

    /**
     *
     * @since s60
     */
    void DynInitMenuPaneL(TInt /*aResourceId*/, CEikMenuPane* /*aMenuPane*/);

private: // Constructors

    /**
     * Ctor
     * @param aEnv Java environment
     *
     * @since s60
     */
    CMIDMenuHandler(MMIDEnv& aEnv);

    /**
     * By default Symbian 2nd phase constructor is private.
     */
    void ConstructL();

    /**
     * Prohibit copy constructor if not deriving from CBase.
     * @since s60
     */
    CMIDMenuHandler(const CMIDMenuHandler&);

    /**
     * Prohibit assigment operator if not deriving from CBase.
     * @since s60
     */
    CMIDMenuHandler& operator=(const CMIDMenuHandler&);

    /**
     * Updating of scaling factors
     */
    void SetScalingFactors();

    /**
     * @since s60
     */
    TSize DesToTSize(TPtrC scrSize);

    /**
     * Enable Multiple key events for Canvas
     *
     * @param aGaugeItemData Data pointer
     * @since s60
     */
    void SendMultipleKeyPressedEvent();


private: // Data

    MMIDEnv& iEnv;

    CEikMenuBar* iMenuBar;
    CEikButtonGroupContainer* iCba;

    CMIDDisplayable* iDisplayable;
    CEikAppUi* iAppUi;
    TRect iViewRect;

    /** The array of menu items to be added next time we display a menu */
    RArray<CEikMenuPaneItem::SData> iMenuItems;

    /** Prevents trying to display a menu twice in case the user clicks twice
    on options CBA whilst menu is displaying - this causes a crash in current
    3.0 builds */
    TBool iAttemptingToDisplayMenu;

    const CEikonEnv& iEikEnv;

    /** Menutype of currently present menu */
    TMenuType iMenuType;

    /** Parameters for scaling by JAD attribute */
    TSize iOrgMIDletScrSize;
    TSize iTargetMIDletScrSize;
    TBool iScaleMIDletOnOrientSwitch;

};


// -----------------------------------------------------------------------------
// CMIDMenuHandler::MenuBar
//
//
// -----------------------------------------------------------------------------
//
inline CEikMenuBar* CMIDMenuHandler::MenuBar() const
{
    return iMenuBar;
}

// -----------------------------------------------------------------------------
// CMIDMenuHandler::Cba
//
//
// -----------------------------------------------------------------------------
//
inline CEikButtonGroupContainer* CMIDMenuHandler::Cba()
{
    return iCba;
}

// -----------------------------------------------------------------------------
// CMIDMenuHandler::GetDisplayable
//
//
// -----------------------------------------------------------------------------
//
inline CMIDDisplayable* CMIDMenuHandler::GetDisplayable()
{
    return iDisplayable;
}

// -----------------------------------------------------------------------------
// CMIDMenuHandler::GetScalingParameterOrgMIDletScrSize
// -----------------------------------------------------------------------------
//
inline TSize CMIDMenuHandler::GetScalingParameterOrgMIDletScrSize() const
{
    return iOrgMIDletScrSize;
}

// -----------------------------------------------------------------------------
// CMIDMenuHandler::GetScalingParameterTargetMIDletScrSize
// -----------------------------------------------------------------------------
//
inline TSize CMIDMenuHandler::GetScalingParameterTargetMIDletScrSize() const
{
    return iTargetMIDletScrSize;
}

// -----------------------------------------------------------------------------
// CMIDMenuHandler::GetScalingParameterScaleMIDletOnOrientSwitch
// -----------------------------------------------------------------------------
//
inline TBool CMIDMenuHandler::GetScalingParameterScaleMIDletOnOrientSwitch() const
{
    return iScaleMIDletOnOrientSwitch;
}

#endif // CMIDMENUHANDLER_H

