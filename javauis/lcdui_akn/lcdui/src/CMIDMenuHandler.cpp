/*
* Copyright (c) 2003-2008 Nokia Corporation and/or its subsidiary(-ies).
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


// INCLUDE FILES
// CEikonEnv API for iEkonEnv :
// - to retrieving CEikAppUi
// - to obtain device screen size
#include <eikenv.h>
// API for iAppUi
#include <eikappui.h>
// API for iMenuBar
#include <eikmenub.h>
#include <e32cmn.h>
#include <AknUtils.h>
// using constants for soft keys
#include <avkon.hrh>
// mocros for avkon resources
#include <avkon.rsg>
// macros for resources
#include <lcdui.rsg>
// API needed for iDisplayable and iDefaultDisplayable members
#include "CMIDDisplayable.h"
#include "CMIDCanvas.h"
#include "CMIDMenuHandler.h"
// CMIDTickerController API for iTickerController (stored in tls structure)
#include "CMIDTicker.h"
// for setting key decoder in tls
#include "CMIDKeyDecoder.h"
#include "lcdui.h"
#include <j2me/jdebug.h>
#include "CMIDUIManager.h"


const TInt KResolutionStringLength = 4;

// ============================ MEMBER FUNCTIONS ===============================


// -----------------------------------------------------------------------------
// CMIDMenuHandler::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CMIDMenuHandler* CMIDMenuHandler::NewL(MMIDEnv& aEnv)
{
    CMIDMenuHandler* self = new(ELeave) CMIDMenuHandler(aEnv);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);

    return self;
}

// -----------------------------------------------------------------------------
// CMIDMenuHandler::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CMIDMenuHandler::ConstructL()
{
    iAppUi = iEikEnv.EikAppUi();
    iMenuBar = new(ELeave) CEikMenuBar;
    iMenuBar->ConstructL(this, 0, R_MIDP_MENUBAR_DEFAULT);

    iCba = CEikButtonGroupContainer::NewL(CEikButtonGroupContainer::ECba,
                                          CEikButtonGroupContainer::EHorizontal,
                                          this,
                                          R_AVKON_SOFTKEYS_EMPTY);
#ifdef RD_JAVA_S60_RELEASE_9_2
    static_cast<CEikCba*>(iCba->ButtonGroup())->EnableItemSpecificSoftkey(EFalse);
#endif
    const TSize screenSize = iEikEnv.ScreenDevice()->SizeInPixels();
    iCba->SetBoundingRect(TRect(screenSize));
    iViewRect = iAppUi->ClientRect();
    iCba->ReduceRect(iViewRect);
    iAppUi->AddToStackL(iMenuBar,ECoeStackPriorityMenu,ECoeStackFlagRefusesFocus);
    SetScalingFactors();
}


void CMIDMenuHandler::HideMenuIfVisible()
{
    if (iMenuBar->IsDisplayed())
    {
        iMenuBar->StopDisplayingMenuBar();
    }
}

void CMIDMenuHandler::UpdateMenuIfVisibleL()
{
    if (iMenuBar->IsDisplayed())
    { // Menu is currently displayed

        iMenuBar->StopDisplayingMenuBar();

        ShowMenuL(iMenuType);
    }
}


// C++ default constructor
CMIDMenuHandler::CMIDMenuHandler(MMIDEnv& aEnv)
        : iEnv(aEnv)
        ,iEikEnv(*(CEikonEnv::Static()))
{
}

// Destructor
CMIDMenuHandler::~CMIDMenuHandler()
{
    HideMenuIfVisible();
    if (iAppUi)
    {
        iAppUi->RemoveFromStack(iMenuBar);
    }
    delete iMenuBar;
    delete iCba;
    iMenuItems.Reset();
}

// -----------------------------------------------------------------------------
// CMIDMenuHandler::ShowMenuL
//
// @see DynInitMenuPaneL()
// -----------------------------------------------------------------------------
//
void CMIDMenuHandler::ShowMenuL(const TMenuType& aType)
{
    if (iDisplayable && aType != ENoMenu && !iAttemptingToDisplayMenu)
    {
        iMenuType = aType;
        iDisplayable->CreateMenuItemsL(iMenuType, iMenuItems);

        // Set the CEikMenuBar type according to internal type
        if (aType == EOptionsMenu)
        {
            iMenuBar->SetMenuType(CEikMenuBar::EMenuOptions);
        }
        else if (aType == EOkMenu || aType == EHelpMenu || aType == EScreenAndHelpMenu)
        {
            iMenuBar->SetMenuType(CEikMenuBar::EMenuContext);
        }

        // If there is anything to show, open the menu
        if (iMenuItems.Count() > 0 && !iMenuBar->IsDisplayed())
        {
            iAttemptingToDisplayMenu = ETrue;
            iMenuBar->TryDisplayMenuBarL();
        }
        // Reset the menu type always to options menu just in case
        iMenuBar->SetMenuType(CEikMenuBar::EMenuOptions);
    }
}

// -----------------------------------------------------------------------------
// CMIDMenuHandler::MenuItemsCount
//
//
// -----------------------------------------------------------------------------
//
TUint CMIDMenuHandler::MenuItemsCount() const
{
    return iMenuItems.Count();
}

// -----------------------------------------------------------------------------
// CMIDMenuHandler::SetDisplayable
//
//
// -----------------------------------------------------------------------------
//
void CMIDMenuHandler::SetDisplayable(CMIDDisplayable* aDisplayable)
{
    iDisplayable = aDisplayable;

    if (iDisplayable)
    {
        SendMultipleKeyPressedEvent();
    }
}
// -----------------------------------------------------------------------------
// CMIDMenuHandler::SendMultipleKeyPressedEventL
//
//
// -----------------------------------------------------------------------------
//
void CMIDMenuHandler::SendMultipleKeyPressedEvent()
{
    ASSERT(iDisplayable);

    RWsSession  wsSession = CCoeEnv::Static()->WsSession();
    TWsEvent event;

    // Check the current Display type if it is canvas enable the
    // multiple key events, other wise disable it.
    MMIDComponent* component = iDisplayable->Component();
    MMIDComponent::TType type = component->Type();

    if (type == MMIDComponent::ECanvas)
        event.SetType(EEnableMultipleKeyPressedEvent); // set event type enable multiple key pressed event
    else
        event.SetType(EDisableMultipleKeyPressedEvent); // set event type disable multiple key pressed event
    event.SetTimeNow(); // set the event time
    event.SetHandle(wsSession.WsHandle()); // set window server handle
    wsSession.SendEventToAllWindowGroups(event);
}


// -----------------------------------------------------------------------------
// CMIDMenuHandler::ProcessCommandL
//
//
// -----------------------------------------------------------------------------
//
void CMIDMenuHandler::ProcessCommandL(TInt aCommandId)
{
    HideMenuIfVisible();

    //
    if (aCommandId == EAknSoftkeyExit)
    {
        iAppUi->HandleCommandL(EEikCmdExit);
        return;
    }

    if (aCommandId == EEikCmdCanceled)
    {
        return;
    }

    if (aCommandId == EAknSoftkeyOptions)
    {
        ShowMenuL(EOptionsMenu);
        return;
    }

    if (aCommandId == EStdKeyDevice3)
    {
        ShowMenuL(EOkMenu);
        return;
    }

    if (iDisplayable)
    {
        iDisplayable->ProcessCommandL(aCommandId);
    }
}

// -----------------------------------------------------------------------------
// CMIDMenuHandler::SetEmphasis
//
//
// -----------------------------------------------------------------------------
//
void CMIDMenuHandler::SetEmphasis(CCoeControl* aMenuControl,TBool aEmphasis)
{
    if (!iDisplayable)
    {
        return;
    }

    if (aEmphasis)
    {
        if (MMIDComponent::ECanvas == iDisplayable->Component()->Type())
        {
            iEnv.PostMidletEvent(EBackground);
        }
    }
    else
    {
        if (MMIDComponent::ECanvas == iDisplayable->Component()->Type())
        {
            iEnv.PostMidletEvent(EForeground);
        }
    }

    iAppUi->UpdateStackedControlFlags(aMenuControl, aEmphasis? 0: ECoeStackFlagRefusesFocus,
                                      ECoeStackFlagRefusesFocus);
    iAppUi->HandleStackChanged();
}

// -----------------------------------------------------------------------------
// CMIDMenuHandler::DynInitMenuPaneL
//
//
// -----------------------------------------------------------------------------
void CMIDMenuHandler::DynInitMenuPaneL(TInt aResourceId, CEikMenuPane* aMenuPane)
{
    if (aResourceId == R_MIDP_MENUPANE_DISPLAYABLE)
    {
        TInt numItems = iMenuItems.Count();
        for (TInt i = 0; i < numItems; i++)
        {
            aMenuPane->AddMenuItemL(iMenuItems[i]);
        }

        iAttemptingToDisplayMenu = EFalse;
    }

}


/** SetScalingFactors

 Sets MIDlet's original size and target size resolutions if those are defined in JAD or manifest.
 If attribute is missing or it's not defined correctly, the value will be 0,0.
 This value is later used as an "error value".
 @param -
 @return -

 */
void CMIDMenuHandler::SetScalingFactors()
{
    TPtrC orgSize;
    // If JAD attribute Nokia-MIDlet-Original-Display-Size is defined, it will be used to scale Canvas.
    if (iEnv.MidletAttribute(LcduiMidletAttributes::KAttribOrigDisplaySize, orgSize) == KErrNone)
    {
        iOrgMIDletScrSize = DesToTSize(orgSize);
        TPtrC targetSize;
        // If JAD attribute Nokia-MIDlet-Original-Display-Size and Nokia-MIDlet-Target-Display-Size are defined,
        // those will be used to scale Canvas.
        // If Nokia-MIDlet-Target-Display-Size is invalid, Canvas will still be scaled,
        // if Nokia-MIDlet-Original-Display-Size is correct.
        if (iOrgMIDletScrSize.iHeight != 0 && iOrgMIDletScrSize.iWidth != 0 &&
                iEnv.MidletAttribute(LcduiMidletAttributes::KAttribTargetDisplaySize,
                                     targetSize) == KErrNone)
        {
            iTargetMIDletScrSize = DesToTSize(targetSize);
        }

        // Load Nokia-MIDlet-allow-scaling-on-orientation-switch attribute
        // setting. This attribute is valid only when original MIDlet size
        // is defined.
        if (iOrgMIDletScrSize.iHeight != 0 && iOrgMIDletScrSize.iWidth != 0)
        {
            iScaleMIDletOnOrientSwitch = iEnv.MidletAttributeIsSetToVal(
                                             LcduiMidletAttributes::KAttribAllowScalingOnOrientationSwitch,
                                             LcduiMidletAttributeValues::KTrueValue);
        }
    }
}

/** DesToTSize

 Utility to parse TPtrC to TSize.
 This util is made for graphics scaling feature.
 @param TPtrC
 @return TSize

 */
TSize CMIDMenuHandler::DesToTSize(TPtrC scrSize)
{
    TSize newSize;
    TSize errorSize(0, 0);
    TInt newIntWidth;
    TInt newIntHeight;
    // Assumed max. 9999 pixels in width.
    TBuf<KResolutionStringLength> newWidth;
    // Assumed max. 9999 pixels in height.
    TBuf<KResolutionStringLength> newHeight;
    TChar tmpChar;
    _LIT(lComma, ",");
    TChar comma(',');

    // The digits of width and height must be divided by comma
    if (scrSize.Find(lComma) != KErrNotFound)
    {
        TLex16 lex(scrSize);
        while (lex.Peek() != comma)
        {
            tmpChar = lex.Get();
            if (tmpChar.IsDigit() && newWidth.Length() < KResolutionStringLength)
            {
                newWidth.Append(tmpChar);
            }
            else if (!tmpChar.IsSpace())
            {
                // If character is not digit or space
                return errorSize;
            }
        }
        lex.Get();
        while (!lex.Eos())
        {
            tmpChar = lex.Get();
            if (tmpChar.IsDigit() && newHeight.Length() < KResolutionStringLength)
            {
                newHeight.Append(tmpChar);
            }
            else if (!tmpChar.IsSpace())
            {
                // If character is not digit or space
                return errorSize;
            }
        }

        // Width and height must have at least one digit each
        if (newWidth.Length() > 0 && newHeight.Length() > 0)
        {
            TLex16 widthLex(newWidth);
            widthLex.Val(newIntWidth);
            TLex16 heightLex(newHeight);
            heightLex.Val(newIntHeight);
            newSize.SetSize(TInt(newIntWidth), TInt(newIntHeight));
            return newSize;
        }
    }
    // If there is no comma between the digits or there is no digits in width or height
    return errorSize;
}

TSize CMIDMenuHandler::GetScreenSize() const
{
    // It get size of screen from iEikEnv's screen device.
    const CEikonEnv* env = &iEikEnv;
    return env->ScreenDevice()->SizeInPixels();
}

TBool CMIDMenuHandler::IsScalingEnabled() const
{
    //If iOrgMIDletScrSize has been initialized then scaling is on.
    //It's enough to check either height or width only.
    return (iOrgMIDletScrSize.iHeight != 0);
}

TBool CMIDMenuHandler::IsScalingEffectiveInCurrentScreen() const
{
    if (IsScalingEnabled())
    {
        TRect screen;
        AknLayoutUtils::LayoutMetricsRect(AknLayoutUtils::EScreen, screen);
        TSize screenSize = screen.Size();

        TSize origResolution = iOrgMIDletScrSize;
        if (iScaleMIDletOnOrientSwitch)
        {
            // ensure that origResolution and screenSize are in portrait
            origResolution = TSize(Min(iOrgMIDletScrSize.iHeight, iOrgMIDletScrSize.iWidth),
                                   Max(iOrgMIDletScrSize.iHeight, iOrgMIDletScrSize.iWidth));
            screenSize = TSize(Min(screen.Height(), screen.Width()),
                               Max(screen.Height(), screen.Width()));
        }

        // If origResolution and screenSize are different, then
        // scaling is effective.
        return (screenSize.iHeight != origResolution.iHeight ||
                screenSize.iWidth != origResolution.iWidth);
    }
    else
    {
        return EFalse;
    }
}
// End of File
