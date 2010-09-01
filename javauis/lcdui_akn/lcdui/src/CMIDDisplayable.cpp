/*
* Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Implementation of the CMIDDisplayable class
*
*/


#include <bldvariant.hrh>

#include <memory>
#include <apgwgnam.h>
#include <eikenv.h>
#include <coecntrl.h>
#include <uikon.hrh>
// using BaflUtils::CopyWithTruncation in PopulateMenuItemsWithListL function
#include <bautils.h>
// using MMIDImage and MMIDBitmapImage
#include <lcdgr.h>
// macros definitions for resources
#include <lcdui.rsg>
// using TMifLcdui enumeration
#include <lcdui.mbg>
// Used for Avkon toolbar disabling
#include <akntoolbar.h>
// Layout data
#include <layoutmetadata.cdl.h>
#include <aknlayoutscalable_avkon.cdl.h>
#include <e32property.h>
//#include <SysStartup.h>
#include <aknappui.h>
// using CAknTitlePane in ShowTitleL() function
#include <akntitle.h>
// using CAknContextPane in SetMIDletIconL() function to show midlet icon
#include <akncontext.h>
// used for label converting for commands (in PopulateMenuItemsWithListL function)
#include <AknBidiTextUtils.h>
#include <avkon.hrh>
#include <avkon.rsg>
#include <AvkonInternalCRKeys.h>
#include <aknstyluspopupmenu.h>
#include <eiklabel.h>
#ifdef RD_SCALABLE_UI_V2
// For requiring available keyboards in the device
#include <PtiEngine.h>
#include <PtiDefs.h>
#endif // RD_SCALABLE_UI_V2

// OMJ storage
#include <applicationinfo.h>
#include <javastorageentry.h>
#include <javastorage.h>
#include <javastoragenames.h>
#include <javauid.h>
#include <javasymbianoslayer.h>

#include "CMIDDisplayable.h"
// API used in several places (iAppUi is member)
#include "CMIDAppUi.h"
#include "CMIDCanvas.h"
// used in SetComponentL function
#include "CMIDDefaultBackground.h"
// API for ticker on displayable
#include "CMIDTicker.h"
#include "CMIDComponentFactory.h"
// used for setting iMenuHandler from iUIManager (MenuHandler)
#include "CMIDUIManager.h"
#include "CMIDCommand.h"
#include "CMIDForm.h"
// using CMIDUtils::CopyBitmapL in SetMIDletIconL() function
#include "CMIDUtils.h"
// used in SetComponentL function
// and used in several place for actions specific for Alert (SetTitleL, ContentSize, HandleCurrentL)
#include "CMIDAlert.h"
#include "CMIDAlertDialog.h"
#include "CMIDEdwin.h"
// used in SetComponentL function
// used in HandleCurrentL function
#include "CMIDTextBoxControl.h"
#include "CMIDTextBoxDialogControl.h"
// used in SetComponentL function
#include "CMIDList.h"
// using API for softkeys and their commands
#include "CMIDSoftKey.h"
#include "CMIDCanvasKeypad.h"
#include "CMIDTicker.h"

#include "coreuiavkonlcdui.h"
#include "coreuiappui.h"

#include <j2me/jdebug.h>

/** This macro is executed each time a trapped call returns an error code different than KErrNone */
#undef  TRAP_INSTRUMENTATION_LEAVE
#define TRAP_INSTRUMENTATION_LEAVE(aResult) DEBUG_INT2("In CMIDDisplayable.cpp, trapped method was called at line %D and got exception %D", __LINE__, aResult);

const TGulBorder::TBorderType KMIDLetBorder = TGulBorder::EThickDeepRaisedWithOutline;

const TInt KCommandIdNotFound = -10000;
const TInt KCommandIdBase = 7000;
const TInt KItemCommandIdBase = 8000; // must be greater than KCommandIdBase
// Special ID for built-in MSK commands. Handling of them is different because
// built-in commands are not visible in menus, only in MSK. Built-in MSK commands
// are commands that are set using SetMSKCommand method, but are not found in
// command lists.
const TInt KBuiltInMSKCommandId = 9000;

// Index of the MSK in CBA, to be used when adding MSK command to CBA
const TInt KMSKPositionInCBA = 3;

/** The maximum number of softkeys on the device.
*/
const TInt KMaxNumberOfSoftKeys = 2;
const TInt KListLongTapAnimationDelay = 400000;
const TInt KListLongTapDelay = 800000;


/*
 * The allowed command types for softkeys,
 *   -1 means we have run out of allowed types.
 * More restrictive sks, eg the right sk, should come
 * before more permissive sks. This array is used to
 * initialise iSoftkeys.
 */
const TInt KAllowedCommandTypesForSoftkeys[KMaxNumberOfSoftKeys][9] =
{
    // Right softkey
    {
        MMIDCommand::EStop,
        MMIDCommand::ECancel,
        MMIDCommand::EBack,
        MMIDCommand::EExit,
        -1,
        -1,
        -1,
        -1,
        -1
    },

    // Left softkey
    {
        MMIDCommand::EStop,
        MMIDCommand::EOk,
        MMIDCommand::ECancel,
        MMIDCommand::EItem,
        MMIDCommand::EScreen,
        MMIDCommand::EBack,
        MMIDCommand::EHelp,
        MMIDCommand::EExit,
        -1
    }
};

/**
  The CBA that can have an options menu if commands are available.
  This is used as a zero-based index in iSoftkeys and must be
  less than KMaxNumberOfSoftKeys.
*/
const TInt KOptionsMenuCBAIndex = 1; //left sk

/**
  The sk position in the CBA. Must use the same ordering as
  KAllowedCommandTypesForSoftkeys and is used to initialise
  iSoftkeys.
*/
const TInt KPositionsInCBAForSoftkeys[KMaxNumberOfSoftKeys] =
{
    2, //Right softkey
    0 //Left softkey
};

// One ticker can be added more than one displayable. So keep track of all of them
const TInt KAddDisplayable = 1; // add the displable to the list in ticker
const TInt KRemoveDisplayable = 0; // Remove the displable from the list in ticker

const TInt KSoftKeyLabelPropertyPositionsMSKIndex = 2;
const TInt KSoftKeyLabelPropertyPositionsInCBA[ KSoftKeyLabelPropertyNumberOfSoftKeys ] =
{
    0,  //LSK
    2,  //RSK
    3   //MSK
};

// class CMIDDisplayable

CMIDDisplayable* CMIDDisplayable::NewL(MMIDEnv& aEnv,CMIDUIManager& aUIManager)
{
    CMIDDisplayable* displayable = new(ELeave)CMIDDisplayable(aEnv,aUIManager);
    CleanupStack::PushL(displayable);
    displayable->ConstructL();
    CleanupStack::Pop(displayable);
    return displayable;
}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CMIDDisplayable::ConstructL()
{
    DEBUG("+ CMIDDisplayable::ConstructL");

    // Menu/commands
    // NOTE: Menu handler member variable has to be valid before
    // the <code>CCoeContol::MakeVisible</code> method is called
    // because the <code>CMIDDisplayable::MopSupplyObject</code> method
    // might be called because of that.
    iMenuHandler = iUIManager->OpenMenuHandlerL();
    iCommandList = new(ELeave) CMIDCommandList();
    iCommandList->SetCommandOffset(KCommandIdBase);
    iMSKCommand = NULL;
    iDisplayableBehindPopup = NULL;

    iEnv.AddObserverL(*this);
    CreateWindowL(&iEikonEnv->RootWin());
    SetMopParent(java::ui::CoreUiAvkonLcdui::getInstance().getJavaAknAppUi());

    //All windows are created invisible and then made visible
    //in SetComponentL(), except for Alerts (dialogs)
    MakeVisible(EFalse);
    Window().SetOrdinalPosition(-1);

    Window().EnableOnEvents(EEventControlOnlyWhenVisible);
    EnableDragEvents();

    // Long tap detection
    iLongTapDetector = CAknLongTapDetector::NewL(this);

    // SoftKeys
    for (TInt i = 0; i < KMaxNumberOfSoftKeys; i++)
    {
        CMIDSoftKey* softKey = CMIDSoftKey::NewLC(i, &(KAllowedCommandTypesForSoftkeys[i][0]),
                               KPositionsInCBAForSoftkeys[i]);
        User::LeaveIfError(iSoftKeys.Append(softKey));
        CleanupStack::Pop(softKey);
    }

    // CBA
    iCba = iMenuHandler->Cba();

    UpdateDisplayableRect();
    SetRect(iDisplayableRect);

    iBorder = TGulBorder::ENone;

    SetComponentsToInheritVisibility();
    // for Skin Support, Create background control context:
    iBackGroundControlContext = CAknsBasicBackgroundControlContext::NewL(
                                    KAknsIIDQsnBgAreaMain,  // Default mainpane skinning
                                    Rect(),                 // Layout to the entire client rect
                                    EFalse);

    iAppUi->Displayables().AppendL(this);
    iAppUi->SetEnv(iEnv);
    ActivateL();

    //Read JAD attribute value for On-Screen Keypad softkeys visual appearence with HW QWERTY keypad
    if (iEnv.MidletAttributeIsSetToVal(
                LcduiMidletAttributes::KAttribOskSoftkeysInQwerty,
                LcduiMidletAttributeValues::KPositionBottom))
    {
        iSKPositionWithQwerty = ESoftkeysBottom;
    }
    else
    {
        //default value
        iSKPositionWithQwerty = ESoftkeysRight;
    }

    // initialization of keypad information
    ReadOnScreenKeypadTypeFromSuiteSettings();

    DEBUG("- CMIDDisplayable::ConstructL");
}

CMIDDisplayable::CMIDDisplayable(MMIDEnv& aEnv,CMIDUIManager& aUIManager)
        :CEikBorderedControl(TGulBorder(KMIDLetBorder)),
        iUIManager(&aUIManager),iEnv(aEnv),
        iIsFullScreenMode(EFalse),iActive(EFalse), iSelectCommand(NULL), iSelectCommandEnabled(ETrue),
        iFullscreenCanvasLabelCacheIsValid(EFalse)
#ifdef RD_TACTILE_FEEDBACK
        ,iPenInputServerConnected(EFalse)
#endif //RD_TACTILE_FEEDBACK
        ,iIdOfMSKCommand(KErrNotFound)
        ,iRestoreOrientation(EFalse)
        ,iReleaseCnt(0)
{
#ifdef RD_JAVA_S60_RELEASE_9_2
    iSplitScreenKeyboard = EFalse;
#endif // RD_JAVA_S60_RELEASE_9_2
    iAppUi = (CMIDAppUi*)java::ui::CoreUiAvkonLcdui::getInstance().getJavaUiAppUi()->getLcduiChild();
}

CMIDDisplayable::~CMIDDisplayable()
{
    DEBUG("+ CMIDDisplayable::CMIDDisplayable");

    // iPropertyWatch is CPropertyWatch object,
    // delete member call desctructor of CPropertyWatch
    // and remove active object form active scheduler
    if (iPropertyWatch)
    {
        delete iPropertyWatch;
        iPropertyWatch = NULL;
    }

    // Informing iEnv about deleting this
    iEnv.DisplayableIsDestructed(this);

    // Remove this from iEnv observer
    iEnv.RemoveObserver(*this);

    if (iAppUi)
    {
        iAppUi->RemoveFromStack(iContentControl);
        TInt index = iAppUi->Displayables().Find(this);
        if (index != KErrNotFound)
        {
            iAppUi->Displayables().Remove(index);
        }
    }

    delete iLongTapDetector;
    delete iStylusPopupMenu;
    delete iTitle;

#ifdef RD_SCALABLE_UI_V2
    iUIManager->CloseCanvasKeypad(iCanvasKeypad);
    iCanvasKeypad = NULL;
#endif //RD_SCALABLE_UI_V2

#ifdef RD_TACTILE_FEEDBACK
    if (iPenInputServerConnected)
    {
        iPenInputServer.Close();
        iPenInputServerConnected = EFalse;
    }
#endif //RD_TACTILE_FEEDBACK

    // if ticker is present in this displayable and displayable is
    // deleted but ticker is not, tell the ticker that displayable is
    // garbage collected.
    if (iTicker)
    {
        TRAP_IGNORE(iTicker->SetDisplayableL(this, KRemoveDisplayable));
    }

    delete iBackGroundControlContext;
    iBackGroundControlContext = NULL;

    iSoftKeys.ResetAndDestroy();
    delete iCommandList;

    if (iMenuHandler->GetDisplayable() == this)
    {
        //this may happen when the application is exiting
        iMenuHandler->SetDisplayable(NULL);
    }

    DEBUG("- CMIDDisplayable::CMIDDisplayable");
}

/**
 * This means we are being deleted java side. We delete ourselves.
 * However before doing this we call ReplaceBehindAlertIfNeeded().
 **/
void CMIDDisplayable::Dispose()
{
    ReplaceBehindAlertIfNeeded();

    delete this;
}

/**
 * This method is called when we are deleted java side, from Dispose().
 * This code should only execute when java side they decide to delete the
 * displayable immeditately behind a showing alert. We check if we are a
 * displayable behind an alert. If so we find another displayable (the
 * one with lowest ordinal position, ie the top most displayable except us
 * and the alert). If we find one we make it visible so that the alert will
 * not end up with a white background.
 **/
void CMIDDisplayable::ReplaceBehindAlertIfNeeded()
{
    if ((this != iUIManager->GetDefaultDisplayable())
            && IsVisible()
            && (iMenuHandler->GetDisplayable())
            && (iMenuHandler->GetDisplayable()->Component()->Type() == EAlert))
    {
        // In this case we are a faded displayable behind a dialog
        TInt numDisplayables = iAppUi->Displayables().Count();
        TInt index = -1;
        TInt currentPosition = 999;
        for (TInt i = 0; i < numDisplayables; i++)
        {
            CMIDDisplayable* disp = iAppUi->Displayables()[i];
            if ((disp != this) && (disp->DrawableWindow()->OrdinalPosition() < currentPosition)
                    && (disp->Component()->Type() != EAlert))
            {
                currentPosition = disp->DrawableWindow()->OrdinalPosition();
                index = i;
            }
        }

        if (index != -1)
        {
            CMIDDisplayable* disp = iAppUi->Displayables()[index];
            disp->MakeVisible(ETrue);
        }
    }
}

void CMIDDisplayable::Draw(const TRect& aRect) const
{
    CWindowGc& gc = SystemGc();

    // Set up update region - preventing DSA to be destroyed by redrawing
    if (!iDirectContentsRegion.IsEmpty())
    {
        gc.CancelClippingRect();
        iUpdateRegion.Clear();
        iUpdateRegion.AddRect(aRect);
        // Remove occupied areas out from update region
        iUpdateRegion.SubRegion(iDirectContentsRegion);
        // Set the update region for the context
        gc.SetClippingRegion(iUpdateRegion);
    }

    if (iContent->Type() != ECanvas)
    {
        // Draw background
        MAknsSkinInstance* skin = AknsUtils::SkinInstance();
        AknsDrawUtils::Background(skin, iBackGroundControlContext, this, gc, aRect);
    }
    iBorder.Draw(gc, Rect());
}

TInt CMIDDisplayable::CountComponentControls() const
{
    TInt count=0;
    if (iContentControl)
    {
        ++count;
    }
    return count;
}

CCoeControl* CMIDDisplayable::ComponentControl(TInt aIndex) const
{
    switch (aIndex)
    {
    case 0:
        return iContentControl ? iContentControl: 0;
    default:
        return 0;
    }
}

void CMIDDisplayable::SizeChanged()
{
    if (iBackGroundControlContext)
    {
        iBackGroundControlContext->SetRect(Rect());
    }
    Layout();
}

/**
 * Use the commands in aCommandList to create menu items and add them to aItems.
 */
void CMIDDisplayable::PopulateMenuItemsWithListL
(
    const CMIDMenuHandler::TMenuType& aMenuType, //The type of menu
    RArray<CEikMenuPaneItem::SData>& aItems, //The items that will be added to the menu
    CMIDCommandList* aCommandList, //The list of commands to be examined
    TBool aSeparator
)
{
    if (!aCommandList)
    {
        return;
    }
    const TBool isItemCommands = (aCommandList == iItemCommandList);

    CMIDCommandList* list = aCommandList;
    CEikMenuPaneItem::SData item;

#ifdef RD_JAVA_S60_RELEASE_9_2
    // implicitList is used when creating options menu content.
    TBool implicitList = EFalse;
    if (iContent && iContent->Type() == EList && iContentControl)
    {
        CMIDList* list = static_cast<CMIDList*>(iContentControl);
        implicitList = list->ListChoiceType() == MMIDChoiceGroup::EImplicit;
    }
#endif // RD_JAVA_S60_RELEASE_9_2

    const TInt count = list->Count();
    for (TInt ii=0; ii<count; ii++)
    {
        const CMIDCommand* command = (list->At(ii).iCommand);
#ifdef RD_JAVA_S60_RELEASE_9_2
        // If command is mapped to any sk, do not add it to menu (pop-up menu is an exception).
        if (!iIsFullScreenMode && CommandIsMappedToSk(command) && aMenuType != CMIDMenuHandler::EPopUpMenu)
#else
        // If command is mapped to any sk, do not add it to menu.
        if (!iIsFullScreenMode && CommandIsMappedToSk(command))
#endif // RD_JAVA_S60_RELEASE_9_2               
        {
            continue;
        }
        // Only commands of type OK and ITEM are visible in context menu.
        // TextBox/TextField device-provided commands:
        // - "Fetch number"
        // - "Call"
        // - "Fetch e-mail address"
        // are exception. Those are visible ONLY in Options menu so here
        // they are not added to context menu.
#ifdef RD_JAVA_S60_RELEASE_9_2
        if (aMenuType == CMIDMenuHandler::EOkMenu || aMenuType == CMIDMenuHandler::EPopUpMenu)
#else
        if (aMenuType == CMIDMenuHandler::EOkMenu)
#endif // RD_JAVA_S60_RELEASE_9_2            
        {
            if ((!isItemCommands &&
            (command->CommandType() != MMIDCommand::EOk) &&
            (command->CommandType() != MMIDCommand::EItem)) ||
            (command->Id() == CMIDEdwinUtils::EMenuCommandFetchPhoneNumber) ||
            (command->Id() == CMIDEdwinUtils::EMenuCommandFetchEmailAddress) ||
            (command->Id() == CMIDEdwinUtils::EMenuCommandCreatePhoneCall))
            {
                continue;
            }
        }

        if (aMenuType == CMIDMenuHandler::EHelpMenu)
        {
            if (command->CommandType() != MMIDCommand::EHelp)
            {
                continue;
            }
        }

        TPtrC shortLabel = const_cast<CMIDCommand*>(command)->ShortLabel();
        TPtrC longLabel  = const_cast<CMIDCommand*>(command)->Label();

        item.iCommandId = ii + list->CommandOffset();
        item.iCascadeId = 0;

        if (ii == (count-1) && aSeparator)
        {
            item.iFlags = EEikMenuItemSeparatorAfter;
        }
        else
        {
            item.iFlags = 0;
        }

        // Find out whether the long label fits into the menu without clipping,
        // and use the long label if it fits; otherwise use short label.
        //
        // In some scripts the visual presentation may differ from the logical text,
        // so we need to convert the text to visual presentation first.
        // The clipped clipped string is not used, only the return value is.
        //
        const CFont& font =
            *AknLayoutUtils::FontFromId(AKN_LAYOUT_TEXT_List_pane_texts__menu_single__Line_1(0).iFont);
        TInt maxMenuItemLength = CEikMenuPaneItem::SData::ENominalTextLength;

        TRect parentRect = Rect();
        TAknLayoutText prompt1;
        prompt1.LayoutText(parentRect, AKN_LAYOUT_TEXT_Data_query_pop_up_window_texts_Line_1(0));
        TInt layoutWidth = prompt1.TextRect().Width();

        HBufC* visual = HBufC::NewMaxLC(maxMenuItemLength + KAknBidiExtraSpacePerLine);
        TPtr visualPtr = visual->Des();
        TBool clip = AknBidiTextUtils::ConvertToVisualAndClip(
                         longLabel,
                         visualPtr,
                         font,
                         layoutWidth,
                         layoutWidth,
                         AknBidiTextUtils::EImplicit,
                         0xFFFF);
        CleanupStack::PopAndDestroy(visual);

        if (clip)
        {
            BaflUtils::CopyWithTruncation(item.iText, shortLabel);
        }
        else
        {
            BaflUtils::CopyWithTruncation(item.iText, longLabel);
        }

        if ((iMSKCommand) && (iMSKCommand == list->At(ii).iCommand))
        {
            User::LeaveIfError(aItems.Insert(item, 0));
        }
        else
        {
#ifdef RD_JAVA_S60_RELEASE_9_2
            if (aMenuType == CMIDMenuHandler::EOptionsMenu)
            {
                TBool implicitCmd = (implicitList && (command->CommandType() == MMIDCommand::EOk ||
                                                      command->CommandType() == MMIDCommand::EItem));

                if (!implicitCmd)
                {
                    User::LeaveIfError(aItems.Append(item));
                }
                else if (implicitCmd && iContentControl &&
                         static_cast<CMIDList*>(iContentControl)->IsHighlighted())
                {
                    // If highlight in implicit list is False then there is no focus
                    // Item/Ok should be visible in options menu only when there is focus on item
                    User::LeaveIfError(aItems.Append(item));
                }
            }
            else
            {
                User::LeaveIfError(aItems.Append(item));
            }
#else
            User::LeaveIfError(aItems.Append(item));
#endif // RD_JAVA_S60_RELEASE_9_2
        }
    }
}


/**
 * Create a list of menu items based on the type of menu and the
 * existing command lists.
 *
 * This method is executed each time the menu is displayed to the user.
 * There are two types of menus that can be displayed: context sensitive
 * - activated by the select key also known as ok-options menu -
 * or standard options menu - activated by the LSK. In the standard options
 * menu we display item and standard commands.In the context menu we display
 * item commands (if there are any) and standard commands if they are
 * of type OK or ITEM, this is taken care of by PopulateMenuItemsWithListL().
 *
 * @see PopulateMenuItemsWithListL(), CMIDMenuHandler::ShowMenuL()
 **/
void CMIDDisplayable::CreateMenuItemsL
(
    const CMIDMenuHandler::TMenuType& aMenuType, // the type of menu
    RArray<CEikMenuPaneItem::SData>& aMenuItems // the list of menu items
)
{
    aMenuItems.Reset();

    if (aMenuType == CMIDMenuHandler::EOkMenu)
    {
        if (iItemCommandList && iItemCommandList->Count() > 0)
        {
            PopulateMenuItemsWithListL(aMenuType, aMenuItems, iItemCommandList, EFalse);
        }

        // Add form commands always
        PopulateMenuItemsWithListL(aMenuType, aMenuItems, iCommandList, EFalse);
    }
    else if (aMenuType == CMIDMenuHandler::EHelpMenu)
    {
        // Add the Help commands only
        PopulateMenuItemsWithListL(aMenuType, aMenuItems, iCommandList, EFalse);
    }
    else if (aMenuType == CMIDMenuHandler::EOptionsMenu)
    {
        PopulateMenuItemsWithListL(aMenuType, aMenuItems, iItemCommandList, EFalse);
        PopulateMenuItemsWithListL(aMenuType, aMenuItems, iCommandList, ETrue);
    }
}

/**
 * iMenuHandler gets the commands first and handles the default ones (exit, options...).
 * The rest of the commands are passed here.
 **/
void CMIDDisplayable::ProcessCommandL(TInt aCommandId)
{
    if (!iActive)
    {
        return;
    }

    if (aCommandId == EAknSoftkeyContextOptions)
    {
        // MSK command to show context sensitive menu -> open it
        ShowOkOptionsMenuL();
    }
    else if (aCommandId == KBuiltInMSKCommandId)
    {
        // Handle built-in command that is not accessible from menus, only in MSK.
        // Send a notification to the observer; there always should be one in built-in
        // commands, if not it is considered to be an error.
        ASSERT(iMSKCommand->Observer());
        iMSKCommand->Observer()->ProcessCommandL(iMSKCommand);
    }
    else if (aCommandId >= KItemCommandIdBase
             && iItemCommandList->IsValidIndex(aCommandId-KItemCommandIdBase))
    {
        HandleItemCommandL(iItemCommandList->At(aCommandId-KItemCommandIdBase));
    }
    else if (aCommandId >= KCommandIdBase
             && iCommandList->IsValidIndex(aCommandId-KCommandIdBase))
    {
        HandleStandardCommandL(iCommandList->At(aCommandId-KCommandIdBase));
    }
#ifdef RD_JAVA_S60_RELEASE_9_2
    // If command has come from pop-up menu, ensure that
    // possible pending up event is forwarded to the UI component.
    if (iContent && iContentControl)
    {
        if (iContent->Type() == EList)
        {
            CMIDList* list = static_cast<CMIDList*>(iContentControl);
            list->PostPendingUpEventL();
        }
        else if (iContent->Type() == EForm)
        {
            CMIDForm* form = static_cast<CMIDForm*>(iContentControl);
            form->PostPendingUpEventL();
        }
    }
#endif // RD_JAVA_S60_RELEASE_9_2
}

#ifdef RD_SCALABLE_UI_V2
/** */
// This function can be moved out from RD_SCALABLE_UI_V2 flag if needed.
// It is behind this flag because currently it is used only by Touch.
void CMIDDisplayable::ProcessCommandL(CMIDCommand* aCommand)
{
    TInt internalID = GetInternalCommandIdFor(aCommand);
    if (internalID != KCommandIdNotFound)
    {
        ProcessCommandL(internalID);
    }
}
#endif //RD_SCALABLE_UI_V2

/** */
void CMIDDisplayable::HandleStandardCommandL(const TCommandEntry& aCmdEntry)
{
    TBool postJavaEvent = ETrue;
    if (aCmdEntry.iCommand->Id() < 0)
    {
        //non-midlet command, see if there is an observer
        if (aCmdEntry.iCommand->Observer())
        {
            postJavaEvent = !(aCmdEntry.iCommand->Observer()->ProcessCommandL(aCmdEntry.iCommand));
        }
    }

    if (postJavaEvent)
    {
        iEnv.PostJavaEvent(*this, EDisplayable, ECommand, aCmdEntry.iKey);
    }
}

/** */
void CMIDDisplayable::HandleItemCommandL(const TCommandEntry& aCmdEntry)
{
    TBool postJavaEvent = ETrue;
    if (aCmdEntry.iCommand->Id() < 0)
    {
        //non-midlet command, see if there is an observer
        if (aCmdEntry.iCommand->Observer())
        {
            postJavaEvent = !(aCmdEntry.iCommand->Observer()->ProcessCommandL(aCmdEntry.iCommand));
        }
    }

    if (postJavaEvent && iContent->Type() == EForm)
    {
        CMIDForm* form = static_cast<CMIDForm*>(iContentControl);
        ASSERT(form);

        MMIDItem* item = form->CurrentItem();
        if (item)
        {
            iEnv.PostJavaEvent(*item, EItem, ECommand, aCmdEntry.iKey);
        }
    }
}

void CMIDDisplayable::HandleHelpCommandL()
{
    // Get how many java help commands is registered
    TInt numCommands = NumCommandsForHelpOptionsMenu();

    // If there is only one help command, invoke the Java help event directly
    if (numCommands == 1)
    {
        TCommandEntry cmdEntry;
        cmdEntry.iCommand = NULL;

        if (iCommandList)
        {
            TInt index = iCommandList->HighestPriorityCommand(MMIDCommand::EHelp);
            if (index != KErrNotFound)
            {
                cmdEntry = iCommandList->At(index);
                iEnv.PostJavaEvent(*this, EDisplayable, ECommand, cmdEntry.iKey);
            }
        }
    }
    // If there is two or more help commands, let display the menu with these commands
    else if (numCommands > 0)
    {
        iMenuHandler->ShowMenuL(CMIDMenuHandler::EHelpMenu);
    }
}

TInt CMIDDisplayable::NumCommandsForHelpOptionsMenu() const
{
    TInt ret = 0;

    // Add always HELP commands from form
    TInt numCommands = iCommandList->Count();
    for (TInt i = 0; i < numCommands; i++)
    {
        const CMIDCommand& command = *(iCommandList->At(i).iCommand);
        if (command.CommandType() == MMIDCommand::EHelp)
        {
            ret++;
        }
    }
    return ret;
}

/**
 * Find the screen or help command with highest priority
 * and if exists invokes it
 **/
void CMIDDisplayable::HandleHighestPriorityScreenOrHelpCommandL()
{
    // Find the highest priority screen or help command
    TInt screenOrHelpCmdIndex = GetHighestPriorityScreenOrHelpCommand();
    if (screenOrHelpCmdIndex != KErrNotFound)
    {
        HandleStandardCommandL(iCommandList->At(screenOrHelpCmdIndex));
    }
}

//
// From MMIDDisplayable
//
void CMIDDisplayable::SetTitleL(const TDesC* aTitle)
{
    delete iTitle;
    iTitle = NULL;

    if (aTitle)
    {
        iTitle = aTitle->AllocL();
    }

    if (iTitle)
    {
        iHasTitle = ETrue;
    }
    else
    {
        iHasTitle = EFalse;
    }

    if (iActive)
    {
        ShowTitleL();
    }

    if (iContent && iContent->Type() == EAlert)
    {
        CMIDAlert* alert = (CMIDAlert*) iContent;
        alert->SetTitleL(iTitle);
    }
    // Title must be forwarded to Pop-up TextBox
    else if (iContent && iContent->Type() == ETextBox && iIsPopupTextBox)
    {
        CMIDTextBoxDialogControl* textbox = (CMIDTextBoxDialogControl*) iContent;
        textbox->SetTitleL(iTitle);
    }
}

/**
 * Sets the Ticker
 *
 * StartL is called on the current displayable's ticker in these circumstances:
 * - When the MIDlet is brought to the foreground (in HandleForegroundL(ETrue))
 * - When the device is switched on (in HandleSwitchOnL(ETrue))
 * - If the MIDlet has been paused due to a CSaveNotifier event, StartL is
 *   called when the MIDlet is next brought to the foreground.(in HandleSwitchOnL(ETrue))
 * - When the displayable is made current (StartL called by framework)
 *
 * Stop is called on the current displayable's ticker in these circumstances:
 * - When the MIDlet is placed in the background.(in HandleForegroundL(EFalse) )
 * - The implementation uses the CSaveNotifier framework to provide MIDLet state changes.
 *   When the following events are received ESaveAll,ESaveQuick,ESaveData,EReleaseRAM
 *   the MIDlet is paused and the ticker is stopped.(in HandleSwitchOnL(EFalse))
 * - When the Displayable is no longer current. (Stop is called by framework)
 **/
void CMIDDisplayable::SetTickerL(MMIDTicker* aTicker)
{
    CMIDTicker* newTicker = static_cast< CMIDTicker* >(aTicker);
    // if it is the same do nothing
    if (iTicker == aTicker)
    {
        return;
    }

    if (iTicker)
    {
        // Tell the current Ticker that it has no displayable
        iTicker->SetDisplayableL(this, KRemoveDisplayable);
    }

    iTicker = newTicker;
    if (iTicker)
    {
        iTicker->SetDisplayableL(this, KAddDisplayable);
    }

    // Request ticker timer if a valid ticker has been set and
    // this is the current displayable.
    UpdateTickerL();
}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
MMIDTicker* CMIDDisplayable::Ticker() const
{
    return iTicker;
}

// ---------------------------------------------------------------------------
// Ensures that ticker update events are generated when appropriate
// (e.g. when active) and that this displayables ticker control is notified
// whenever it needs to redraw.
//
// ResumeTicker() and SuspendTicker() deal with the ticker timer whereas
// PauseTicker() is actually sending the ticker to the background makeing it
// disappear.
// ---------------------------------------------------------------------------
void CMIDDisplayable::UpdateTickerL()
{
    if (iActive)
    {
        iUIManager->OpenNaviPaneControllerL()->SetTickerL(iTicker);
        if (iIsFullScreenMode || iIsPopupTextBox)
        {
            iUIManager->OpenNaviPaneControllerL()->ShowTickerL(EFalse);
        }
        else
        {
            iUIManager->OpenNaviPaneControllerL()->ShowTickerL(ETrue);
        }
    }
}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CMIDDisplayable::HandleSwitchOnL(TBool /*aSwitchOn*/)
{
}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CMIDDisplayable::HandleForegroundL(TBool aForeground)
{
    DEBUG("+ CMIDDisplayable::HandleForegroundL");

    // send HandleForegroundL notification to Form
    if (iContent && (iContent->Type() == MMIDComponent::EForm) && iContentControl)
    {
        CMIDForm* form = static_cast<CMIDForm*>(iContentControl);
        ASSERT(form);
        form->HandleForegroundL(aForeground);
    }

    // Notify canvas about gained foreground only if it is the current displayable
    if (!aForeground || iActive)
    {
        HandleCanvasForeground(aForeground);
    }

    if (aForeground)
    {
        // If MIDlet is sent to foreground and JAD-attribute BackgroundEvent=Pause,
        // then call startApp() method for the MIDlet.

        if (iEnv.MidletAttributeIsSetToVal(LcduiMidletAttributes::KAttribBackgroundEvent,
                                           LcduiMidletAttributeValues::KPauseValue))

        {
            iEnv.PostMidletEvent(EStart);
            iAppUi->SetPauseAppState(EFalse);
        }
    }
    else //to background
    {

        // If MIDlet is sent to background and JAD-attribute BackgroundEvent=Pause,
        // and window is not faded yet or window is already faded but pauseApp was not called yet,
        // then call pauseApp() method for the MIDlet.
        TBool isfaded = this->DrawableWindow()->IsFaded();

        if (iEnv.MidletAttributeIsSetToVal(LcduiMidletAttributes::KAttribBackgroundEvent,
                                           LcduiMidletAttributeValues::KPauseValue) && (!isfaded ||
                                                   (isfaded && !iAppUi->GetPauseAppState())))

        {
            iEnv.PostMidletEvent(EPause);
            iAppUi->SetPauseAppState(ETrue);
        }
    }
    DEBUG("- CMIDDisplayable::HandleForegroundL");
}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CMIDDisplayable::HandleResourceChangeL(TInt aType)
{
    DEBUG("+ CMIDDisplayable::HandleResourceChangeL");

    if (aType == KEikDynamicLayoutVariantSwitch)
    {
        // dynamic orientation change

        // Correct rect is set for displayable
        if (iActive && iCanvasKeypad)
        {
            //Update correct On-Screen Keypad type
            UpdateOnScreenKeypadSettings();
        }
        UpdateDisplayableRect();
        SetRect(iDisplayableRect);

        // MIDlet icon is resized in cpane SizeChanged()
        if (!iActive && iContentControl)
        {
            //The active displayable already gets this call by the CONE framework but
            //background displayables don't, so for example a background form won't be
            //re-laid-out if we don't call this here
            iContentControl->HandleResourceChange(aType);
        }
#ifdef RD_SCALABLE_UI_V2
        // When current Displayble is Canvas or popup Displayable and the last
        // Displayble was Canvas, then OSK visual appearance is needed
        //  to be change.
        if (iUseOnScreenKeypad && iCanvasKeypad && (iActive || this->DrawableWindow()->IsFaded()))
        {
            if (iActive)
            {
                // Current is Canvas.
                iCanvasKeypad->UpdateVisualAppearanceL(*iCanvas, iOnScreenKeyboardType, *this);
            }
            else
            {
                // Get current Displayable and the last fullscreen Displayble.
                MMIDDisplayable* current = iEnv.Current();
                const MMIDDisplayable* last = iEnv.LastFullscreenDisplayable();
                if (current && last && current->Component()
                        && last->Component())
                {
                    // Get types of Displayables.
                    MMIDComponent::TType lastType = last->Component()->Type();
                    MMIDComponent::TType currentType = current->Component()->Type();
                    if (lastType == ECanvas)
                    {
                        if (currentType == EAlert)
                        {
                            // Current is Alert and the last fullscreen
                            // Displayable was Canvas.
                            iCanvasKeypad->UpdateVisualAppearanceL(
                                *iCanvas, iOnScreenKeyboardType, *this);
                        }
                        else if (currentType == ETextBox && current->IsPopupTextBox())
                        {
                            // Current is popup TextBox and the last fullscreen
                            // Displayable was Canvas.
                            iCanvasKeypad->UpdateVisualAppearanceL(
                                *iCanvas, iOnScreenKeyboardType, *this);
                        }
                    }
                }
            }
        }
#endif // RD_SCALABLE_UI_V2
        iFullscreenCanvasLabelCacheIsValid = EFalse;
    }
    else if (aType == KEikColorResourceChange ||
             aType == KAknsMessageSkinChange  ||
             aType == KUidValueCoeColorSchemeChangeEvent)
    {
        //skin or color change, may need to recreate ctx icon
        // send skin change event to non-active controls so they can also
        // reload graphics
        if (!iActive && iContentControl)
        {
            iContentControl->HandleResourceChange(aType);
        }
#ifdef RD_SCALABLE_UI_V2
        if (iUseOnScreenKeypad && iCanvasKeypad && iActive)
        {
            iCanvasKeypad->UpdateVisualAppearanceL(*iCanvas, iOnScreenKeyboardType, *this);
        }
#endif // RD_SCALABLE_UI_V2
        iFullscreenCanvasLabelCacheIsValid = EFalse;
    }

    DEBUG("- CMIDDisplayable::HandleResourceChangeL");

    // Language input change, needed by TextEditor.
    if ((aType == KEikInputLanguageChange) && iContentControl)
    {
        iContentControl->HandleResourceChange(aType);
    }
}

#ifdef RD_JAVA_NGA_ENABLED
void CMIDDisplayable::HandleFullOrPartialForegroundL(TBool aFullOrPartialFg)
{
    CMIDCanvas* canvas = GetContentCanvas();
    if (canvas)
    {
        canvas->HandleFullOrPartialForegroundL(aFullOrPartialFg, iActive);
    }
}

void CMIDDisplayable::HandleFreeGraphicsMemory()
{
    CMIDCanvas* canvas = GetContentCanvas();
    if (canvas)
    {
        canvas->FreeGraphicsMemory(ETrue);
    }
}
#endif // RD_JAVA_NGA_ENABLED

/**
 * The container has changed it available drawing space, so resize
 * the displayable to fit and post a size changed event.
 **/
void CMIDDisplayable::Layout()
{
    TBool changed(EFalse);

    //
    // Get client area of window (within border).
    //
    TRect rect(iBorder.InnerRect(Rect()));

    if (iContentControl)
    {
        if (iContentControl->Rect() != rect)
        {
            // Anything left in rect is space for the content.
            // It is arguable that the content should post the resize event as
            // it knows its true size. Certainly that is required for Canvas.
            iContentControl->SetRect(rect);
            changed = ETrue;
            TRAP_IGNORE(iUIManager->OpenNaviPaneControllerL()->LayoutTickerL());
        }

        MMIDComponent::TType type = iContent->Type();
        //
        // We assume that the content width/height == control width height
        // arguably not a valid assumption for any content - but especially
        // invalid for Canvas when the JAD specifies an assumed size.
        //
        // Let Canvas's post their own size, but
        // use the content rect size for all other displayables.
        //
        if (type != ECanvas && type != EGameCanvas)
        {
            //
            // Only post this event for non-canvas components.
            //
            TSize size(rect.Size());
            iEnv.PostJavaEvent(*this, EDisplayable, ESizeChanged, size.iWidth, size.iHeight, 0);
        }
    }

    if (changed)
    {
        DrawDeferred();
    }
}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
TInt CMIDDisplayable::GetSKPositionForOSK()
{
    return iSKPositionWithQwerty;
}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
TRect CMIDDisplayable::GetCanvasRectFromLaf()
{
    DEBUG("+ CMIDDisplayable::GetCanvasRectFromLaf");

    TRect resultRect = TRect(0,0,0,0);
    AknLayoutUtils::LayoutMetricsRect(AknLayoutUtils::EScreen, resultRect);
    TAknLayoutRect canvasRect;
    TAknLayoutRect mainPane;
    TAknLayoutRect mainMidpPane;

#ifdef RD_JAVA_S60_RELEASE_9_2
    // If split screen is open, then it is needed return current size of Canvas.
    if (iSplitScreenKeyboard)
    {
        CMIDCanvas* canvas = GetContentCanvas();
        if (canvas)
        {
            resultRect = canvas->ViewRect();
        }
    }
    else
    {
#endif // RD_JAVA_S60_RELEASE_9_2    
        //Read canvasRect from LAF depending on keyboard type
        switch (iOnScreenKeyboardType)
        {
        case EOnScreenKeypadValueNo:
            break;
        case EOnScreenKeypadValueNavigationKeys:
        {
            if (iIsFullScreenMode)
            {
                if (!Layout_Meta_Data::IsLandscapeOrientation())  //portrait
                {
                    canvasRect.LayoutRect(resultRect, AknLayoutScalable_Avkon::midp_canvas_pane(1).LayoutLine());
                }
                else //landscape
                {
                    canvasRect.LayoutRect(resultRect, AknLayoutScalable_Avkon::midp_canvas_pane(8).LayoutLine());
                }
            }
            else //normal mode
            {
                if (!Layout_Meta_Data::IsLandscapeOrientation())  //portrait
                {
                    mainPane.LayoutRect(resultRect, AknLayoutScalable_Avkon::main_pane(3).LayoutLine());
                    mainMidpPane.LayoutRect(mainPane.Rect(), AknLayoutScalable_Avkon::main_midp_pane(0).LayoutLine());
                    canvasRect.LayoutRect(mainMidpPane.Rect(), AknLayoutScalable_Avkon::midp_canvas_pane(4).LayoutLine());
                }
                else //landscape
                {
                    mainPane.LayoutRect(resultRect, AknLayoutScalable_Avkon::main_pane(4).LayoutLine());
                    mainMidpPane.LayoutRect(mainPane.Rect(), AknLayoutScalable_Avkon::main_midp_pane(1).LayoutLine());
                    canvasRect.LayoutRect(mainMidpPane.Rect(), AknLayoutScalable_Avkon::midp_canvas_pane(7).LayoutLine());
                }
            }
            resultRect = canvasRect.Rect();
            break;
        }
        case EOnScreenKeypadValueGameActions:
        {
            if (iIsFullScreenMode)
            {
                if (!Layout_Meta_Data::IsLandscapeOrientation())  //portrait
                {
                    canvasRect.LayoutRect(resultRect, AknLayoutScalable_Avkon::midp_canvas_pane(2).LayoutLine());
                }
                else //landscape
                {
                    canvasRect.LayoutRect(resultRect, AknLayoutScalable_Avkon::midp_canvas_pane(3).LayoutLine());
                }
            }
            else //normal mode
            {
                if (!Layout_Meta_Data::IsLandscapeOrientation())  //portrait
                {
                    mainPane.LayoutRect(resultRect, AknLayoutScalable_Avkon::main_pane(3).LayoutLine());
                    mainMidpPane.LayoutRect(mainPane.Rect(), AknLayoutScalable_Avkon::main_midp_pane(0).LayoutLine());
                    canvasRect.LayoutRect(mainMidpPane.Rect(), AknLayoutScalable_Avkon::midp_canvas_pane(5).LayoutLine());
                }
                else//landscape
                {
                    mainPane.LayoutRect(resultRect, AknLayoutScalable_Avkon::main_pane(4).LayoutLine());
                    mainMidpPane.LayoutRect(mainPane.Rect(), AknLayoutScalable_Avkon::main_midp_pane(1).LayoutLine());
                    canvasRect.LayoutRect(mainMidpPane.Rect(), AknLayoutScalable_Avkon::midp_canvas_pane(6).LayoutLine());
                }
            }
            resultRect = canvasRect.Rect();
            break;
        }
        case EOnScreenKeypadValueLskRsk:
        {
            if (iIsFullScreenMode)
            {
                if (!Layout_Meta_Data::IsLandscapeOrientation())  //portrait
                {
                    canvasRect.LayoutRect(resultRect, AknLayoutScalable_Avkon::midp_canvas_pane(11).LayoutLine());
                }
                else //landscape
                {
                    if (iSKPositionWithQwerty == ESoftkeysRight)
                    {
                        canvasRect.LayoutRect(resultRect, AknLayoutScalable_Avkon::midp_canvas_pane(10).LayoutLine());
                    }
                    else
                    {
#ifdef RD_JAVA_S60_RELEASE_9_2
                        canvasRect.LayoutRect(resultRect, AknLayoutScalable_Avkon::midp_canvas_pane(9).LayoutLine());
#else
                        resultRect = TRect(80,0,560,360); // Layout data not defined in older releases.
                        DEBUG("- CMIDDisplayable::GetCanvasRectFromLaf");
                        return resultRect;
#endif // RD_JAVA_S60_RELEASE_9_2
                    }
                }
            }
            else//normal mode
            {
                //no need to present LSK&RSK in OSK when in normal mode Canvas
                resultRect = TRect(0,0,0,0);

                DEBUG("- CMIDDisplayable::GetCanvasRectFromLaf");
                return resultRect;
            }
            resultRect = canvasRect.Rect();
            break;
        }
        }
#ifdef RD_JAVA_S60_RELEASE_9_2
    }
#endif // RD_JAVA_S60_RELEASE_9_2

    DEBUG("- CMIDDisplayable::GetCanvasRectFromLaf");
    return resultRect;
}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CMIDDisplayable::AddCommandL(MMIDCommand* aCommand)
{
    iCommandList->AddL(aCommand);
    InitializeCbasL();
    iMenuHandler->UpdateMenuIfVisibleL();
}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CMIDDisplayable::RemoveCommand(MMIDCommand* aCommand)
{
    iCommandList->Remove(aCommand);
    TRAP_IGNORE(InitializeCbasL());
    TRAP_IGNORE(iMenuHandler->UpdateMenuIfVisibleL());
}

// ---------------------------------------------------------------------------
// This method reports the total space available to the
// content class. Displayable subclasses may report a
// different value as the width/height in which case they
// will use a differnt mechanism.
// @see MMIDCanvas::ContentSize
// @see MMIDForm::Width
// @see MMIDForm::Height
// ---------------------------------------------------------------------------
TSize CMIDDisplayable::ContentSize() const
{
    if (iContent->Type() == MMIDComponent::EAlert)
    {
        return static_cast<CMIDAlert*>(iContent)->Dialog()->Size();
    }
    else if (iContent->Type() == MMIDComponent::ETextBox && iIsPopupTextBox)
    {
        return static_cast<CMIDTextBoxDialogControl*>(iContent)->Dialog()->ContentSize();
    }
    else if (iContent->Type() == MMIDComponent::EForm)
    {
        // return the size that is available for the form items
        // as stated in MIDP specification. This is smaller size than would be
        // returned by the iContentControl->Size().
        CMIDForm* form = static_cast<CMIDForm*>(iContent);
        return TSize(form->Width(), form->Height());
    }
    /**
    Size really depends on the content class, however
    javax.microedition.lcdui.Form and javax.microedition.lcdui.Canvas
    contain overrides to return the correct size - so polymorphism is
    only required here for List,TextBox and Alert.

    We return the size of the content control, so as long as the content
    control keeps its size up to date (eg alerts must have the same size as the dialog)
    the correct size will be returned even if there is no polymorphism
    provided by the framework. Now, for alerts UpdateSizeL() must have
    been called, in fact when the dialog is not yet displaying they need
    to create one, set the size and then delete the dialog immediately.
    */
    if (iContentControl)
    {
        return iContentControl->Size();
    }

    //If there is no content then we shouldn't really exist
    //because java side Displayable is abstract
    return TSize(0,0);
}

// ---------------------------------------------------------------------------
// Called by the framework when current displayable was changed
// in case that old and new displayable type is canvas to change osk state
// ---------------------------------------------------------------------------
void CMIDDisplayable::ChangeOSKBackgroundState(TBool aOSKBackgroundState)
{
    if (iContent && iContent->Type() == ECanvas)
    {
        if (iCanvasKeypad)
        {
            iCanvasKeypad->OSKInBackground(aOSKBackgroundState);
        }
    }
}

// ---------------------------------------------------------------------------
// Called by the framework when we become or stop being the active displayable.
// See HandleActivated(), HandleDeactivated().
// ---------------------------------------------------------------------------
void CMIDDisplayable::HandleCurrentL(TBool aCurrent)
{
    DEBUG("+ CMIDDisplayable::HandleCurrentL");
    ASSERT(iContentControl);
    ASSERT(iContent);

    iActive = aCurrent;
    const TType type = iContent->Type();

    if (aCurrent)
    {
        HandleCanvasForeground(aCurrent);

        // when setting displayable as current remember to deactivate
        // the default displayable if it has not been done yet
        CMIDDisplayable* defaultDisplayable =
            iUIManager->GetDefaultDisplayable();
        if (defaultDisplayable &&
                (this != defaultDisplayable) &&
                defaultDisplayable->IsActive())
        {
            defaultDisplayable->HandleCurrentL(EFalse);
        }
        // "Whole screen size"-displayable behind popup type displayable is stored.
        if (type == EAlert || (type == ETextBox && iIsPopupTextBox))
        {
            if (!(iMenuHandler->GetDisplayable() && (iMenuHandler->GetDisplayable()->IsPopupTextBox() ||
                    iMenuHandler->GetDisplayable()->Component()->Type() == EAlert)))
            {
                iDisplayableBehindPopup    = iMenuHandler->GetDisplayable();
            }
        }

        iMenuHandler->SetDisplayable(this);
        // Tell the iAppUi about setting this as active
        iAppUi->SetCurrentDisplayable(this);
        HandleActivatedL();
    }
    else
    {
        HandleDeactivated();
    }

    if (type == EAlert)
    {
        // alerts do their on thing, they rely on sleeping dialogs in fact
        CMIDAlert* alert = static_cast<CMIDAlert*>(iContentControl);
        TRAP_IGNORE(alert->HandleCurrentL(aCurrent));

        UpdateTickerL();
    }
    else if (type == ETextBox && iIsPopupTextBox)
    {
        // Pop-up TextBox do their on thing, they rely on sleeping dialogs in fact
        CMIDTextBoxDialogControl* textBoxDialogControl =
            static_cast<CMIDTextBoxDialogControl*>(iContentControl);
        TRAP_IGNORE(textBoxDialogControl->HandleCurrentL(aCurrent));
    }
    else
    {
        if (aCurrent)
        {
            UpdateVisualAppearanceL();
        }
        else
        {
            // Hide the displayable except the current displayable is Alert
            // or pop-up TextBox.
            if ((this != iUIManager->GetDefaultDisplayable()) &&
                    (iMenuHandler->GetDisplayable()) &&
                    (iMenuHandler->GetDisplayable()->Component()->Type() != EAlert)  &&
                    !(iMenuHandler->GetDisplayable()->Component()->Type() == ETextBox &&
                      iMenuHandler->GetDisplayable()->IsPopupTextBox()))
            {
                MakeVisible(EFalse);
            }

            // Notify canvas after changing the visibility
            HandleCanvasForeground(aCurrent);
        }

        iContentControl->SetFocus(aCurrent);

        switch (type)
        {
        case EForm:
        {
            CMIDForm* form = static_cast<CMIDForm*>(iContentControl);
            TRAP_IGNORE(form->HandleCurrentL(aCurrent));
            break;
        }
        case ETextBox:
        {
            CMIDTextBoxControl* textBoxControl =
                static_cast<CMIDTextBoxControl*>(iContentControl);
            TRAP_IGNORE(textBoxControl->HandleCurrentL(aCurrent));
            break;
        }
        }
    }

    DEBUG("- CMIDDisplayable::HandleCurrentL");
}

/**
 * Called when we become the active displayable, see HandleCurrent().
 * Put icontentControl on the app UI stack and update the title.
 **/
void CMIDDisplayable::HandleActivatedL()
{
    DEBUG("+ CMIDDisplayable::HandleActivatedL");
    ASSERT(iContentControl);

    if (iContentControl->IsNonFocusing()) // i.e it's a Form
    {
        iAppUi->AddToStackL(
            iContentControl,
            (ECoeStackPriorityDefault - 1),
            ECoeStackFlagRefusesFocus);
    }
    else
    {
        iAppUi->AddToStackL(iContentControl);
    }

    ShowTitleL();

    /* In case Canvas is in fullcsreen mode iDisplaybleRect needs to be updated.
     * The Canvas rect has been reduced if Casvas has been displayed before (See HandleDeactivated).
     */
    if (iContent && iContent->Type() == ECanvas)
    {
        if (iCanvasKeypad)
        {
            UpdateOnScreenKeypadSettings();
        }
        UpdateDisplayableRect();
        SetRect(iDisplayableRect);
    }

#ifdef RD_SCALABLE_UI_V2
    // Hide CanvasKeypad if the current displayable is not Alert nor pop-up TextBox.
    if (iUIManager->GetCanvasKeypad() && !iCanvasKeypad &&
            (iContent->Type() != EAlert &&
             !(iContent->Type() == ETextBox && iIsPopupTextBox)))
    {
        iUIManager->GetCanvasKeypad()->MakeVisible(EFalse);
    }
#endif //RD_SCALABLE_UI_V2

    DEBUG("- CMIDDisplayable::HandleActivatedL");
}

// ---------------------------------------------------------------------------
// Called when we stop being the active displayable, see HandleCurrent().
// Remove iContentControl from the app UI stack. If a menu was showing then
// hide it.
// ---------------------------------------------------------------------------
void CMIDDisplayable::HandleDeactivated()
{
    DEBUG("+ CMIDDisplayable::HandleDeactivated");
    ASSERT(iContentControl);

    // Make displayable behind popup/Alert non-visible if displayable behind
    // popup/Alert is changed to Canvas when dismissing the popup/Alert.
    // Use case: Canvas on-screen keypad visibility after displayable changes.
    if (iDisplayableBehindPopup && iContent && (iContent->Type() == EAlert || (iContent->Type() == ETextBox && iIsPopupTextBox))
            && iDisplayableBehindPopup != iMenuHandler->GetDisplayable())
    {
        if (iMenuHandler->GetDisplayable()->Component()->Type() == ECanvas ||
                iMenuHandler->GetDisplayable()->Component()->Type() == EGameCanvas)
        {
            iDisplayableBehindPopup->MakeVisible(EFalse);
        }
    }

    // Stop displaying commands of old displayable
    iMenuHandler->HideMenuIfVisible();

    // Stop repeat timer if OSK set to background
    if (iContent && iContent->Type() == ECanvas && iCanvasKeypad)
    {
        iCanvasKeypad->OSKInBackground(ETrue);
    }

    iAppUi->RemoveFromStack(iContentControl);

    /* Enable status pane when Canvas in fullscreen mode is deactivated.
     * In case there is Canvas in fullscreen mode and then setCurrent(Alert,normal mode displayable)
     * is called, status pane stays behind the Canvas window.
     * First we make status pane and cbas visible according to whether we
     * are full screen or not. This determines the rect returned by iAppUi->ClientRect()
     */
    if (iContent && iContent->Type() == ECanvas && iIsFullScreenMode)
    {
        CEikStatusPane* pane = iAppUi->StatusPane();
        pane->MakeVisible(ETrue);
        // set right Width from APPUI client rect
        TRect displayableRectT = iDisplayableRect;
        TRect iDisplayableRect = iAppUi->ClientRect();
        iDisplayableRect.iBr.iY = displayableRectT.iBr.iY;

        TRAPD(err, SetFullScreenModeL(ETrue));
        if (err != KErrNone)
        {
            DEBUG_INT("CMIDDisplayable::HandleDeactivated - SetFullScreenModeL error %d", err);
        }
    }

    DEBUG("- CMIDDisplayable::HandleDeactivated");
}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CMIDDisplayable::UpdateVisualAppearanceL()
{
    DEBUG("+ CMIDDisplayable::UpdateVisualAppearanceL");

    Window().SetOrdinalPosition(0);
    MakeVisible(ETrue);

    // First we make status pane and cbas visible according to whether we
    // are full screen or not. This determines the rect returned by
    // iAppUi->ClientRect()
    CEikStatusPane* pane = iAppUi->StatusPane();
    pane->MakeVisible(!iIsFullScreenMode);

    java::ui::CoreUiAvkonAppUi* appUi =
        java::ui::CoreUiAvkonLcdui::getInstance().getJavaUiAppUi();
    if (!iIsFullScreenMode && appUi && appUi->hidesIndicators())
    {
        HideIndicators();
    }

    if (iCba)
    {
        iCba->MakeVisible(!iIsFullScreenMode);

#ifdef RD_JAVA_S60_RELEASE_9_2
        // To enable clock pane in landscape after MIDlet was started
        iCba->SetBoundingRect(TRect(0, 0, 0, 0));
#endif // RD_JAVA_S60_RELEASE_9_2
    }

    // Close fixed toolbar for full screen Canvas.
    CAknToolbar* toolbar = iAppUi->CurrentFixedToolbar();
    if (toolbar)
    {
        toolbar->SetToolbarVisibility(!iIsFullScreenMode);
    }

#ifdef RD_SCALABLE_UI_V2
    if ((iActive && iCanvasKeypad)
            || (!iActive && this->DrawableWindow()->IsFaded()
                && iCanvasKeypad))
    {
        if (iUseOnScreenKeypad)
        {
            iCanvasKeypad->UpdateVisualAppearanceL(
                *iCanvas, iOnScreenKeyboardType, *this);

            // Because keypad changed its appearance we have to assure that
            // values for soft key label locations are updated.
            iFullscreenCanvasLabelCacheIsValid = EFalse;
        }
        else
        {
            // Canvas keypad has to be set invisible if On-Screen keypad is
            // not to be used. Otherwise, if the visibility not handled here,
            // status pane and CBA area's visual appearance can be corrupted.
            if (!iIsFullScreenMode)
            {
                iCanvasKeypad->MakeVisible(EFalse);
            }
        }
    }
#endif //RD_SCALABLE_UI_V2

    // At this point we can set the rect.
    if (!iIsFullScreenMode && iCba)
    {
        InitializeCbasL();
        iCba->DrawableWindow()->SetOrdinalPosition(0);
    }

    UpdateTickerL();
    DrawDeferred();
    DEBUG("- CMIDDisplayable::UpdateVisualAppearanceL");
}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CMIDDisplayable::SetComponentL(MMIDComponent& aComponent)
{
    ASSERT(!iContentControl);

    iContent = &aComponent;
    iContentControl = NULL;

    MMIDComponent::TType type = aComponent.Type();
    switch (type)
    {
    case MMIDComponent::EForm:
        iContentControl = static_cast<CMIDForm*>(iContent);
        break;
    case MMIDComponent::EAlert:
        iContentControl = static_cast<CMIDAlert*>(iContent);
        break;
    case MMIDComponent::ECanvas:
    case MMIDComponent::EGameCanvas:
    {
        iContentControl = &(static_cast<MMIDCanvas*>(iContent)->Control());
#ifdef RD_SCALABLE_UI_V2
        iCanvas = static_cast<CMIDCanvas*>(iContentControl);
        if (AknLayoutUtils::PenEnabled())
        {
            iUseOnScreenKeypad = ETrue;
            iCanvasKeypad = iUIManager->OpenCanvasKeypadL(this);

            // initialize iPropertyWatch as CPropertyWatch
            iPropertyWatch = CPropertyWatch::NewL(this);//Property observer
            UpdateOnScreenKeypadSettings();
        }
        UpdateDisplayableRect();
        SetRect(iDisplayableRect);
#endif //RD_SCALABLE_UI_V2
    }
    break;
    case MMIDComponent::ETextBox:
        if (iIsPopupTextBox) // pop-up TextBox
        {
            iContentControl = static_cast<CMIDTextBoxDialogControl*>(iContent);
        }
        else                // normal TextBox
        {
            iContentControl = static_cast<CMIDTextBoxControl*>(iContent);
        }
        break;
    case MMIDComponent::EList:
        iContentControl = static_cast<CMIDList*>(iContent);
        // Lengthen long tap delay to get better usability.
        iLongTapDetector->SetTimeDelayBeforeAnimation(KListLongTapAnimationDelay);
        iLongTapDetector->SetLongTapDelay(KListLongTapAnimationDelay + KListLongTapDelay);
        break;
    case MMIDComponent::EDefaultBackground:
        iContentControl = static_cast<CMIDDefaultBackground*>(iContent);
        break;
    default:
        ASSERT(iContentControl);
    }
    Layout();

    ASSERT(iContentControl);
    iContentControl->ActivateL();

    DEBUG("- CMIDDisplayable::SetComponentL");
}


void CMIDDisplayable::UpdateOnScreenKeypadSettings()
{
    DEBUG("+ CMIDDisplayable::UpdateOnScreenKeypadSettings");
    if (iCanvasKeypad)
    {
        TInt hwKeyboardLayout = 0;

        if (AknLayoutUtils::PenEnabled())
        {
            // touch screen
            ReadOnScreenKeypadTypeFromSuiteSettings();

            if (iOnScreenKeyboardType == EOnScreenKeypadValueNo)
            {
                iUseOnScreenKeypad = EFalse;

                DEBUG("- CMIDDisplayable::UpdateOnScreenKeypadSettings");
                return;
            }

            RProperty::Get(
                KCRUidAvkon, KAknKeyBoardLayout, hwKeyboardLayout);

            // Avoid display of on-screen keypad on screen
            // in landscape mode for QWERTY layout 3x11 (4) and 4x10 (3)
            if (hwKeyboardLayout == EPtiKeyboardQwerty4x10 ||
                    hwKeyboardLayout == EPtiKeyboardQwerty3x11)
            {
                // HW keyboard is opened
                if (iIsFullScreenMode)
                {
                    iOnScreenKeyboardType = EOnScreenKeypadValueLskRsk;
                    iUseOnScreenKeypad = ETrue;
                }
                else
                {
                    // normal mode
                    iOnScreenKeyboardType = EOnScreenKeypadValueNo;
                    iUseOnScreenKeypad = EFalse;
                }
            }
            else
            {
                // Keypadless device or HW keyboard is closed
                iUseOnScreenKeypad = ETrue;
            }
        }
        else
        {
            // no touch screen
            iUseOnScreenKeypad = EFalse;
            iOnScreenKeyboardType = EOnScreenKeypadValueNo;
        }
    }

    DEBUG("- CMIDDisplayable::UpdateOnScreenKeypadSettings");
}

void CMIDDisplayable::ReadOnScreenKeypadTypeFromSuiteSettings()
{
    DEBUG("+ CMIDDisplayable::ReadOnScreenKeypadTypeFromSuiteSettings");

    // Read required keyboard type
    // 0=not defined,  1=no, 2=navigationkeys, 3=gameactions(default)
    TRAP_IGNORE(iOnScreenKeyboardType = OnScreenKeypadL());
    if (iOnScreenKeyboardType == EOnScreenKeypadValueUndefined)
    {
        // if keypad value cannot be read correctly
        // set gameactions keypad active (ignore error silently)
        iOnScreenKeyboardType = EOnScreenKeypadValueGameActions;
    }

    DEBUG("- CMIDDisplayable::ReadOnScreenKeypadTypeFromSuiteSettings");
}

void CMIDDisplayable::HandleOnScreenKeypadVisual()
{
    DEBUG("+ CMIDDisplayable::HandleOnScreenKeypadVisual");
#ifdef RD_JAVA_S60_RELEASE_9_2
    if (iActive && !iSplitScreenKeyboard)
#else
    if (iActive)
#endif // RD_JAVA_S60_RELEASE_9_2
    {
        UpdateOnScreenKeypadSettings();
        UpdateDisplayableRect();
        SetRect(iDisplayableRect);
        TRAP_IGNORE(UpdateVisualAppearanceL());
    }
    DEBUG("- CMIDDisplayable::HandleOnScreenKeypadVisual");
}


// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
CCoeControl& CMIDDisplayable::ContentWindow()
{
    return *this;
}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CMIDDisplayable::SetCommandListenerExistence(TBool aExistence)
{
    iCommandListenerExistence = aExistence;
}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
TBool CMIDDisplayable::IsCommandListenerSet() const
{
    return iCommandListenerExistence;
}

// ---------------------------------------------------------------------------
// Gets a softkey label location
// This can be got fot fullscreen canvas only
// ---------------------------------------------------------------------------
//
TBool CMIDDisplayable::SoftKeyLabelLocation(TInt aSoftKeyId, TPoint& aPosition, TSize& aSize)
{
    if ((iContent->Type() == ECanvas && iIsFullScreenMode) &&
            (aSoftKeyId > 0) &&
            (aSoftKeyId <= KSoftKeyLabelPropertyNumberOfSoftKeys))
    {
        // Refresh the properties cache
        // (can be trapped - checked by iFullscreenCanvasLabelCacheIsValid)
        TRAPD(result, RenewFullscreenCanvasLabelCacheL());
        if (result != KErrNone)
        {
            DEBUG_INT("CMIDDisplayable::SoftKeyLabelLocation - Exception from CMIDDisplayable::RenewFullscreenCanvasLabelCacheL. Error = %d", result);
        }
        else
        {
            TInt skIndex = aSoftKeyId - 1;
            if (iFullscreenCanvasLabelCacheIsValid &&
                    iFullscreenCanvasLabelCache[ skIndex ].iIsOn)
            {

                aPosition = iFullscreenCanvasLabelCache[ skIndex ].iPosition;
                aSize = iFullscreenCanvasLabelCache[ skIndex ].iSize;
                return ETrue;
            }
        }
    }
    return EFalse;
}

// ---------------------------------------------------------------------------
// Force sync draw.
// ---------------------------------------------------------------------------
//
void CMIDDisplayable::DrawNow()
{
    CEikBorderedControl::DrawNow();
    if (iCba)
        iCba->DrawNow();
    iAppUi->StatusPane()->DrawNow();
}

// ---------------------------------------------------------------------------
// Gets a softkey label anchor
// This can be got fot fullscreen canvas only
// ---------------------------------------------------------------------------
//
TInt CMIDDisplayable::SoftKeyLabelAnchor(TInt aSoftKeyId)
{
    if ((iContent->Type() == ECanvas && iIsFullScreenMode) &&
            (aSoftKeyId > 0) &&
            (aSoftKeyId <= KSoftKeyLabelPropertyNumberOfSoftKeys))
    {
        // Refresh the properties cache
        // (can be trapped - checked by iFullscreenCanvasLabelCacheIsValid)
        TRAPD(result, RenewFullscreenCanvasLabelCacheL());
        if (result != KErrNone)
        {
            DEBUG_INT("CMIDDisplayable::SoftKeyLabelAnchor - Exception from CMIDDisplayable::RenewFullscreenCanvasLabelCacheL. Error = %d", result);
        }
        else
        {
            TInt skIndex = aSoftKeyId - 1;
            if (iFullscreenCanvasLabelCacheIsValid &&
                    iFullscreenCanvasLabelCache[ skIndex ].iIsOn)
            {
                return iFullscreenCanvasLabelCache[ skIndex ].iAnchor;
            }
        }
    }
    return 0;
}

void CMIDDisplayable::SetS60SelectionKeyCompatibility(TBool aS60SelectionKeyCompatibility)
{
    iS60SelectionKeyCompatibility = aS60SelectionKeyCompatibility;
}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
CMIDUIManager* CMIDDisplayable::GetUIManager() const
{
    return iUIManager;
}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CMIDDisplayable::UpdateDisplayableRect()
{
    DEBUG("+ CMIDDisplayable::UpdateDisplayableRect");

#ifdef RD_SCALABLE_UI_V2
    TRect canvasRect;
    if (iContent)
    {
        if (iUseOnScreenKeypad)
        {
            MMIDComponent::TType type = iContent->Type();
            if (type == ECanvas || type == EGameCanvas)
            {
                canvasRect = GetCanvasRectFromLaf();
            }
        }
    }
#endif //RD_SCALABLE_UI_V2
    if (iIsFullScreenMode)
    {
        iDisplayableRect = iAppUi->ApplicationRect();

        // Update Cba because of softkey location properties
        // (see RenewFullscreenCanvasLabelCacheL())
        if (iCba && iActive)
        {
            // enable cba so that the layout is performed correctly
            iCba->MakeVisible(ETrue);
            iCba->SetBoundingRect(iDisplayableRect);
            // iCba is always invisible in fullscreen
            iCba->MakeVisible(EFalse);
        }

#ifdef RD_SCALABLE_UI_V2
        if (iUseOnScreenKeypad && (canvasRect != TRect(0,0,0,0)))
        {
            iDisplayableRect = canvasRect;
        }
#endif //RD_SCALABLE_UI_V2
    }
    else
    {
        // In case we are changing from FullScreen mode to Normal mode
        // the status pane has to be enabled before quering client area rect.
        CEikStatusPane* pane = iAppUi->StatusPane();
        // Store pane visibility
        TBool paneVisible = pane->IsVisible();
        pane->MakeVisible(ETrue);

        iDisplayableRect = iAppUi->ClientRect();

#ifdef RD_SCALABLE_UI_V2
        if (iUseOnScreenKeypad && (canvasRect != TRect(0,0,0,0)))
        {
            iDisplayableRect = canvasRect;
        }
#endif //RD_SCALABLE_UI_V2
        if (!iActive)
        {
            // Restore pane visibility. There is one pane for all displayables.
            // Only active displayable can decide about the pane visibility.
            // Others should not change it.
            pane->MakeVisible(paneVisible);
        }

        if (iCba)
        {
            // Store cba visibility
            TBool cbaVisible = iCba->IsVisible();

            // enable cba so that the layout is performed correctly
            iCba->MakeVisible(ETrue);

            iCba->SetBoundingRect(iDisplayableRect);
            // Control pane area has to be reduced
            iCba->ReduceRect(iDisplayableRect);

            if (!iActive)
            {
                // Restore cba visibility. There is one cba for all displayables.
                // Only active displayable can decide about the cba visibility.
                // Others should not change it.
                iCba->MakeVisible(cbaVisible);
            }
        }
    }

    DEBUG("- CMIDDisplayable::UpdateDisplayableRect");
}

// ---------------------------------------------------------------------------
// When we are full screen (currently only Canvas supports this) we need to
// get rid of status pane and CBAs. We do this by calling SetRect() to the full
// screen rect. We also set a flag in canvas, which determines its behaviour.
// See CMIDCanvas::FullScreen(). We store the full screen status in iIsFullScreenMode.
// Note that this method is called by java side.
// ---------------------------------------------------------------------------
void CMIDDisplayable::SetFullScreenModeL(TBool aFullScreen)
{
    DEBUG("+ CMIDDisplayable::SetFullScreenModeL");

    iIsFullScreenMode = aFullScreen;

    if (iContent->Type() == MMIDComponent::ECanvas)
    {
        static_cast<CMIDCanvas*>(iContent)->FullScreen(iIsFullScreenMode);
    }
    if (iCanvasKeypad)
    {
        UpdateOnScreenKeypadSettings();
    }

    // Displayable rect is updated and set
    UpdateDisplayableRect();
    SetRect(iDisplayableRect);

    if (iActive)
    {
        //this means we are the active displayable
        UpdateVisualAppearanceL();
    }

    if (aFullScreen)
    {
        iFullscreenCanvasLabelCacheIsValid = EFalse;
    }

    DEBUG("- CMIDDisplayable::SetFullScreenModeL");
}

#ifdef RD_JAVA_S60_RELEASE_9_2
// ---------------------------------------------------------------------------
// When partial screen keyboard is opened we need to
// get rid of status pane and resize Displayable.
// ---------------------------------------------------------------------------
void CMIDDisplayable::HandleSplitScreenKeyboard(TBool aOpened)
{
    iSplitScreenKeyboard = aOpened;
    if (aOpened)
    {
        iAppUi->StatusPane()->MakeVisible(EFalse);
        TRect clientRect = iAppUi->ClientRect();
        SetRect(clientRect);
        if (iActive)
        {
            DrawDeferred();
        }
    }
    else
    {
        if (iActive)
        {
            UpdateOnScreenKeypadSettings();
            UpdateDisplayableRect();
            SetRect(iDisplayableRect);
            // Ignoring leave
            // The method was already called at least once
            // and must be called here.
            TRAP_IGNORE(UpdateVisualAppearanceL());
        }
    }
}
#endif // RD_JAVA_S60_RELEASE_9_2

// ---------------------------------------------------------------------------
// Return true if we are in full screen mode (cavas), false otherwise.
// @see iIsFullScreenMode
// @see SetFullScreenModeL().
// ---------------------------------------------------------------------------
TBool CMIDDisplayable::IsFullScreenMode() const
{
    return iIsFullScreenMode;
}

// ---------------------------------------------------------------------------
// Sets a new command to the middle soft key (MSK). This can be used e.g. by
// ListBox to set the selection command to the MSK. Context sensitive menu
// opening command is handled automatically by displayable, there is no need
// to set a command for that. Setting MSK via this method, overrides
// automatic behavior of displayable e.g. to open context menu. To unset the
// MSK command, set it to NULL, then the automatic behavior is effective again.
// ---------------------------------------------------------------------------
void CMIDDisplayable::SetMSKCommand(CMIDCommand* aMSKCommand)
{
    CMIDCommand* oldMSKCommand = iMSKCommand;
    iMSKCommand = aMSKCommand;
    if (oldMSKCommand != iMSKCommand)
    {
        // avoid unnecessary updating to avoid flickering
        TRAP_IGNORE(InitializeCbasL());
    }
}

void CMIDDisplayable::SetSelectCommand(CMIDCommand* aSelectCommand)
{
    if (aSelectCommand != iSelectCommand)
    {
        // avoid unnecessary updating to avoid flickering
        iSelectCommand = aSelectCommand;
        TRAP_IGNORE(InitializeCbasL());
    }
}

void CMIDDisplayable::SetSelectCommandState(TBool aEnableSelectCommand)
{
    iSelectCommandEnabled = aEnableSelectCommand;
}

// ---------------------------------------------------------------------------
// Sets the list of form item commands associated with this Displayable on a
// given moment. Calling this will cause the CBA to be updated accordingly and
// usually it is called when focus has moved to a new Item.
//
// If the MSK command is not NULL, it will be presented in the middle soft key
// (MSK). Setting the MSK command here is basically equivalent to setting it with
// SetMSKCommand. The parameter is added here to remind the programmer and to
// gain a small performance benefit of not updating the CBA twice.
// ---------------------------------------------------------------------------
void CMIDDisplayable::SetItemCommandList(CMIDCommandList* aList, CMIDCommand* aMSKCommand)
{
    iItemCommandList = aList;
    if (iItemCommandList)
    {
        iItemCommandList->SetCommandOffset(KItemCommandIdBase);
    }
    CMIDCommand* oldMSKCommand = iMSKCommand;
    iMSKCommand = aMSKCommand;

    TInt newCount = iItemCommandList ? iItemCommandList->Count() : 0;

    // To avoid flicker don't update the CBA if the old and the new command list
    // both have 0 items (very likely).
    if (!(iItemCommandsCount==0 && newCount==0) ||
            oldMSKCommand != iMSKCommand
            || iS60SelectionKeyCompatibility
       )
    {
        TRAP_IGNORE(InitializeCbasL());
    }

    // set current items commands count
    iItemCommandsCount = newCount;
}

// ---------------------------------------------------------------------------
// Called to show the MIDlet title in the Status Pane.
// ---------------------------------------------------------------------------
void CMIDDisplayable::ShowTitleL()
{
    CEikStatusPane* pane = iAppUi->StatusPane();
    CAknTitlePane* titlePane = (CAknTitlePane*)pane->ControlL(TUid::Uid(EEikStatusPaneUidTitle));


    if ((iContent && iContent->Type() == ETextBox) && iIsPopupTextBox)
    {
        // No need to update status pane for pop-up TextBox, because it is not using it.
    }
    else if ((iContent && iContent->Type() == EAlert) || !(this->iTitle) ||
             (this->iTitle->Length() < 1))
    {
        const TDesC& titleText = iEnv.MidletName();
        titlePane->SetTextL(titleText);
    }
    else
    {
        titlePane->SetTextL(*iTitle);
    }
}
// ---------------------------------------------------------------------------
// Get Tilte to
// ---------------------------------------------------------------------------
HBufC* CMIDDisplayable::Title()
{
    return iTitle;
}

// ---------------------------------------------------------------------------
// First map the first eligible command to every sk. Then for the
// sk that accepts the options menu, change the mapped command to
// the options menu if there are still commands that have not been
// mapped. ASSUMPTION 1: Only one sk accepts a menu and this menu is
// always the options menu. ASSUMPTION 2: sks are ordered so that
// sks that are more restrictive in the type of commands they accept
// come before less restrictive sks, eg the right sk comes before the
// left sk.
// Some components, for example, List, do not want MSK set if list is empty,
// decision is done based on enableDefaultMSK parameter value
//
// Finally draw the CBA.
// ---------------------------------------------------------------------------
void CMIDDisplayable::InitializeCbasL()
{
    if (!iActive)
    {
        return;
    }

    iCba->SetCommandSetL(R_AVKON_SOFTKEYS_EMPTY);

    // Initialize command lists
    RArray<CMIDCommandList*> lists;
    CleanupClosePushL(lists);
    User::LeaveIfError(lists.Append(iCommandList));
    User::LeaveIfError(lists.Append(iItemCommandList));

#ifdef RD_JAVA_S60_RELEASE_9_2
    if (iSelectCommand)
#else
    if (!iSelectCommandEnabled && iSelectCommand)
#endif // RD_JAVA_S60_RELEASE_9_2
    {
        // remove the select command
        for (TInt j = 0; j < lists.Count(); j++)
        {
            if (lists[j])
            {
                CMIDCommandList* itemCommandList = lists[j];
                if (itemCommandList)
                {
                    TInt index = itemCommandList->FindCommandIndex(iSelectCommand);
                    if (index != KErrNotFound)
                    {
                        itemCommandList->Remove(iSelectCommand);
                    }
                }
            }
        }
    }

    TInt numLists = lists.Count();
    TInt numSoftKeys = iSoftKeys.Count();
    ResetSoftKeysAndCommands(lists);

    // Variable implicitList is used when mapping commands to soft keys
    // In case of IMPLICIT List with only OK or ITEM commands,
    // no command is mapped to soft key and these commands are not
    // populated to Options menu
    TBool implicitList = EFalse;
#ifdef RD_JAVA_S60_RELEASE_9_2
    if (iContent && iContent->Type() == EList && iContentControl)
    {
        CMIDList* list = static_cast<CMIDList*>(iContentControl);
        // Important: Set to ETrue only if List is IMPLICIT and there is no highlight
        implicitList = (list->ListChoiceType() == MMIDChoiceGroup::EImplicit) &&
                       !list->IsHighlighted();
    }
#endif // RD_JAVA_S60_RELEASE_9_2

    // First map one command to every softkey if possible,
    // sks must have been ordered correctly, ie right key before
    // left key or else left key might get BACK cmd if not other
    // cmds are available.
    for (TInt i = 0; i < numSoftKeys; i++)
    {
        for (TInt j = 0; j < numLists && !iSoftKeys[i]->HasAMappedCommand(); j++)
        {
            if (lists[j])
            {
                TInt index = lists[j]->FindCommandForSoftKey(*iSoftKeys[i], implicitList);
                if (index != KErrNotFound)
                {
                    TInt commandId = lists[j]->CommandOffset() + index;
                    CMIDCommand* cmd = lists[j]->At(index).iCommand;
                    iSoftKeys[i]->Map(cmd);
                    iCba->SetCommandL(iSoftKeys[i]->PositionInCBA(),
                                      commandId,
                                      lists[j]->ShortLabel(index));
                }
            }
        }
    }

    // Then for the sk that can potentially accept an options menu,
    // see if the mapped command must be replaced by the options menu
    // when there is at least one command that has not been mapped to any sk.
    // Important: Do not set Options menu in case of IMPLICIT List
    // with only OK or ITEM commands
    TBool needToDisplayMenu = EFalse;
    for (TInt j = 0; j < numLists && !needToDisplayMenu; j++)
    {
        if (lists[j])
        {
            // Looping trough all commands
            for (TInt i = 0; i < lists[j]->Count() && !needToDisplayMenu; i++)
            {
                CMIDCommand* cmd = lists[j]->At(i).iCommand;
                // In case that displayable is IMPLICIT List, we need to know,
                // if it has only OK or ITEM commands
                if (!(implicitList && (cmd->CommandType() == MMIDCommand::EOk ||
                                       cmd->CommandType() == MMIDCommand::EItem)))
                {
                    // There is at least one command, which is not mapped yet,
                    // so it will be populated to Options menu.
                    needToDisplayMenu = !CommandIsMappedToSk(cmd);
                }
            }
        }
    }
    if (needToDisplayMenu)
    {
        iSoftKeys[KOptionsMenuCBAIndex]->MappedCommand()->SetMappedToSoftKey(EFalse);
        iSoftKeys[KOptionsMenuCBAIndex]->Map(NULL);
        iCba->SetCommandL(iSoftKeys[KOptionsMenuCBAIndex]->PositionInCBA(),R_MIDP_SOFTKEY_OPTIONS);
    }

    // Setup the MSK command
    TInt contextMenuSize = NumCommandsForOkOptionsMenu();
    // first reset stored id of command mapped to MSK
    iIdOfMSKCommand = KErrNotFound;
    if (!iS60SelectionKeyCompatibility)
    {
        if (iMSKCommand)
        {
            // There is a explicitly set MSK command -> it takes the precedence and gets MSK
            TInt commandId = GetInternalCommandIdFor(iMSKCommand);
            if (commandId == KCommandIdNotFound)
            {
                // it must be a built-in command, it is the only command not contained in the command lists
                commandId = KBuiltInMSKCommandId;
            }
            iCba->SetCommandL(KMSKPositionInCBA, commandId, iMSKCommand->ShortLabel());
            iIdOfMSKCommand = commandId;
        }
        else if (!iMSKCommand && (contextMenuSize == 1))
        {
            // There is no explicitly set MSK command and just one for the context menu.
            // Instead of a menu, put the command to MSK directly.
            RPointerArray<CMIDCommand> commands;
            GetOkOptionsMenuCommandsL(commands);
            ASSERT(commands.Count() == 1);
            CMIDCommand* command = commands[0];
            commands.Close();
            iCba->SetCommandL(KMSKPositionInCBA,
                              GetInternalCommandIdFor(command),
                              command->ShortLabel());
            iIdOfMSKCommand = GetInternalCommandIdFor(command);
        }
        else if (!iMSKCommand && (contextMenuSize > 1))
        {
            // There is no explicitly set MSK command, but there is a
            // context sensitive menu -> display MSK command & icon for opening the menu
            _LIT(KEmptyLabel, ""); // the label is never shown in MSK, but an icon is
            iCba->SetCommandL(KMSKPositionInCBA, EAknSoftkeyContextOptions, KEmptyLabel);
            iIdOfMSKCommand = EAknSoftkeyContextOptions;
        }
        else if (!iMSKCommand && (contextMenuSize == 0))
        {
            // There is no explicitly set MSK command and no OK or ITEM command defined
            // Try to map SCREEN or HELP command with the highest priority
            TInt screenOrHelpCmdIndex = GetHighestPriorityScreenOrHelpCommand();
            if (screenOrHelpCmdIndex != KErrNotFound)
            {
                CMIDCommand *command = iCommandList->At(screenOrHelpCmdIndex).iCommand;
                if (command)
                {
                    // At least one SCREEN or HELP command is set, map to MSK
                    iCba->SetCommandL(KMSKPositionInCBA,
                                      GetInternalCommandIdFor(command),
                                      command->ShortLabel());
                    iIdOfMSKCommand = GetInternalCommandIdFor(command);
                }
            }
        }
    }

    CleanupStack::PopAndDestroy(&lists);
    iCba->DrawDeferred();
}

// ---------------------------------------------------------------------------
// Returns an internal command id for a command. Assumes that
// command is not NULL.
// ---------------------------------------------------------------------------
TInt CMIDDisplayable::GetInternalCommandIdFor(CMIDCommand* aCommand) const
{
    ASSERT(aCommand);
    // See if command is part on command list, if the list exists
    if (iCommandList)
    {
        // To identify the command, first find it from the list, this is based on
        // command ids. However, commands in iCommandList and iItemCommandList may
        // have same ids (because of Java side implementation) and thus we compare
        // the pointer of the command in the second phase.
        TInt index = iCommandList->FindCommandIndex(aCommand);
        if (index != KErrNotFound && iCommandList->At(index).iCommand == aCommand)
        {
            return iCommandList->CommandOffset() + index;
        }
    }
    // See if command is part of item command list, if the list exists
    if (iItemCommandList)
    {
        // To identify the command, first find it from the list, this is based on
        // command ids. However, commands in iCommandList and iItemCommandList may
        // have same ids (because of Java side implementation) and thus we compare
        // the pointer of the command in the second phase.
        TInt index = iItemCommandList->FindCommandIndex(aCommand);
        if (index != KErrNotFound && iItemCommandList->At(index).iCommand == aCommand)
        {
            return iItemCommandList->CommandOffset() + index;
        }
    }
    // Did not find the requested command
    return KCommandIdNotFound;
}

// ---------------------------------------------------------------------------
// Return true if the command is mapped to any softkey. The same java side command seem
// to exist on more than one list so we use the command Id to make sure the command is not
// a repeatition of an existing command.
// ---------------------------------------------------------------------------
TBool CMIDDisplayable::CommandIsMappedToSk(const CMIDCommand* aCommand) const
{
    DEBUG("+ CMIDDisplayable::CommandIsMappedToSk");

    TBool commandIsMapped = aCommand->IsMappedToSoftKey();
    for (TInt k = 0; k < iSoftKeys.Count() && !commandIsMapped; k++)
    {
        const CMIDCommand* skCmd = iSoftKeys[k]->MappedCommand();
        if (skCmd && skCmd == aCommand && skCmd->Id() == aCommand->Id())
        {
            commandIsMapped = ETrue;
        }
    }

    DEBUG("- CMIDDisplayable::CommandIsMappedToSk");
    return commandIsMapped;
}

// ---------------------------------------------------------------------------
// See how many commands eligible for the ok-optins menu we have. If we
// have only one command then post java event directly. Otherwise show
// ok-options menu. In these two cases return ETrue. If zero or negative
// commands do nothing and return EFalse. See also NumCommandsForOkOptionsMenu()
// and CMIDMenuHandler::ShowMenuL().
// ---------------------------------------------------------------------------
TBool CMIDDisplayable::ShowOkOptionsMenuL()
{
    TInt numCommands = NumCommandsForOkOptionsMenu();

    if (numCommands == 1)
    {
        TCommandEntry cmd;
        cmd.iCommand = NULL;

        if (iItemCommandList && iItemCommandList->Count() > 0)
        {
            // there can only be one so pick the first one
            cmd = iItemCommandList->At(0);

            if (cmd.iCommand->IsMappedToSoftKey())
            {
                HandleItemCommandL(cmd);
                return ETrue;
            }
        }
        else if (iCommandList && iCommandList->Count() > 0)
        {
            TInt index = iCommandList->HighestPriorityCommand(MMIDCommand::EOk);
            if (index == KErrNotFound)
            {
                // there can only be one so if the other one was KErrNotFound, this must be it
                index = iCommandList->HighestPriorityCommand(MMIDCommand::EItem);
            }
            cmd = iCommandList->At(index);

            if (cmd.iCommand->IsMappedToSoftKey())
            {
                HandleStandardCommandL(cmd);
                return ETrue;
            }
        }
    }

    if (numCommands > 0)
    {
        iMenuHandler->ShowMenuL(CMIDMenuHandler::EOkMenu);
        return ETrue;
    }

    return EFalse;
}

// ---------------------------------------------------------------------------
// See how many commands eligible for the screen or help - optins menu we have.
// If we have only one command call ProcessCommandL. Otherwise show
// options menu. In these two cases return ETrue. If zero or negative
// commands do nothing and return EFalse.
// ---------------------------------------------------------------------------
TBool CMIDDisplayable::ShowScreenOrHelpOptionsMenuL()
{
    TBool ret = EFalse;
    TInt numOkCommands = NumCommandsForOkOptionsMenu();
    TInt numScreenOrHelpCommands = NumCommandsForScreenOrHelpOptionsMenu();
    TInt screenOrHelpCmdIndex = GetHighestPriorityScreenOrHelpCommand();

    // There is no OK or ITEM command defined on form
    // There are SCREEN or HELP commands
    if (iCommandList && screenOrHelpCmdIndex != KErrNotFound && numOkCommands == 0)
    {
        CMIDCommand *command = NULL;
        if (iCommandList->IsValidIndex(screenOrHelpCmdIndex))
        {
            command = iCommandList->At(screenOrHelpCmdIndex).iCommand;
        }

        if (command && (command->CommandType() == MMIDCommand::EScreen ||
                        command->CommandType() == MMIDCommand::EHelp) && iMenuHandler)
        {
            if (numScreenOrHelpCommands > 1)
            {
                iMenuHandler->ShowMenuL(CMIDMenuHandler::EOptionsMenu);
                ret = ETrue;
            }
            else if (numScreenOrHelpCommands == 1)
            {
                ProcessCommandL(iCommandList->CommandOffset());
                ret = ETrue;
            }
        }
    }

    return ret;
}

// ---------------------------------------------------------------------------
// Return the number of commands that can be displayed in the ok-options menu.
// If form has set any item commands in iItemCommandList this means that there is
// a form item focused and the commands for this item should be displayed in
// the ok-options menu along with form OK and ITEM commands.
//
// If we have no item cmds we analyse the standard command list and
// select only the commands of type OK or ITEM.
//
// TextBox/TextField device-provided commands:
// - "Fetch number"
// - "Call"
// - "Fetch e-mail address"
// are exception. Those are visible ONLY in Options menu so here they are
// removed from context menu commands count.
// ---------------------------------------------------------------------------
TInt CMIDDisplayable::NumCommandsForOkOptionsMenu() const
{
    TInt ret = 0;

    if (iItemCommandList && iItemCommandList->Count() > 0)
    {
        TInt numItemCommands = iItemCommandList->Count();
        for (TInt i = 0; i < numItemCommands; i++)
        {
            const CMIDCommand& command = *(iItemCommandList->At(i).iCommand);

            if ((command.Id() != CMIDEdwinUtils::EMenuCommandFetchPhoneNumber) &&
                    (command.Id() != CMIDEdwinUtils::EMenuCommandFetchEmailAddress) &&
                    (command.Id() != CMIDEdwinUtils::EMenuCommandCreatePhoneCall))
            {
                ret++;
            }
        }
    }

    // Add always OK and ITEM commands from form
    TInt numCommands = iCommandList->Count();
    for (TInt i = 0; i < numCommands; i++)
    {
        const CMIDCommand& command = *(iCommandList->At(i).iCommand);

        if (((command.CommandType() == MMIDCommand::EOk) ||
                (command.CommandType() == MMIDCommand::EItem)) &&
                (command.Id() != CMIDEdwinUtils::EMenuCommandFetchPhoneNumber) &&
                (command.Id() != CMIDEdwinUtils::EMenuCommandFetchEmailAddress) &&
                (command.Id() != CMIDEdwinUtils::EMenuCommandCreatePhoneCall))
        {
            TBool selectCommand = (&command == iSelectCommand);
            if (selectCommand && !iSelectCommandEnabled)
            {
                continue;
            }
            ret++;
        }
    }

    return ret;
}

// ---------------------------------------------------------------------------
// Return the number of commands that can be displayed in the screen or help - options menu.
// Form item screen or help commands are ignored
//
// TextBox/TextField device-provided commands:
// - "Fetch number"
// - "Call"
// - "Fetch e-mail address"
// are exception. Those are visible ONLY in Options menu so here they are
// removed from context menu commands count.
// ---------------------------------------------------------------------------
TInt CMIDDisplayable::NumCommandsForScreenOrHelpOptionsMenu() const
{
    TInt ret = 0;

    // Add SCREEN and HELP commands from form
    if (iCommandList)
    {
        TInt numCommands = iCommandList->Count();
        for (TInt i = 0; i < numCommands; i++)
        {
            const CMIDCommand& command = *(iCommandList->At(i).iCommand);

            if (((command.CommandType() == MMIDCommand::EScreen) ||
                    (command.CommandType() == MMIDCommand::EHelp)) &&
                    (command.Id() != CMIDEdwinUtils::EMenuCommandFetchPhoneNumber) &&
                    (command.Id() != CMIDEdwinUtils::EMenuCommandFetchEmailAddress) &&
                    (command.Id() != CMIDEdwinUtils::EMenuCommandCreatePhoneCall))
            {
                TBool selectCommand = (&command == iSelectCommand);
                if (selectCommand && !iSelectCommandEnabled)
                {
                    continue;
                }
                ret++;
            }
        }
    }
    return ret;
}

// ---------------------------------------------------------------------------
// Returns a pointer to the command in the iCommandList with the specified
// ID number. If such command is not found, returns NULL.
// ---------------------------------------------------------------------------
CMIDCommand* CMIDDisplayable::FindCommandWithId(TInt aCommandId) const
{
    TInt numCommands = iCommandList->Count();
    for (TInt i = 0; i < numCommands; i++)
    {
        CMIDCommand* command = iCommandList->At(i).iCommand;
        if (command->Id() == aCommandId)
            return command;
    }
    return NULL;
}

// ---------------------------------------------------------------------------
// Retrieves the commands to be placed in the context (ok-options) menu.
// The commands are returned in the array, provided by the calling code.
// This function will empty the commands array before filling it.
//
// Only commands of type OK and ITEM are visible in context menu.
// TextBox/TextField device-provided commands:
// - "Fetch number"
// - "Call"
// - "Fetch e-mail address"
// are exception. Those are visible ONLY in Options menu so here
// they are not added to context menu commands array.
//
// If there are item commands, there are placed first. Form commands of ITEM
// and OK type are then included always.
// ---------------------------------------------------------------------------
void CMIDDisplayable::GetOkOptionsMenuCommandsL(RPointerArray<CMIDCommand>& aCommands) const
{
    aCommands.Reset();
    if (iItemCommandList && iItemCommandList->Count() > 0)
    {
        for (TInt i = 0; i < iItemCommandList->Count(); i++)
        {
            const CMIDCommand* command = iItemCommandList->At(i).iCommand;
            if ((command->Id() != CMIDEdwinUtils::EMenuCommandFetchPhoneNumber) &&
                    (command->Id() != CMIDEdwinUtils::EMenuCommandFetchEmailAddress) &&
                    (command->Id() != CMIDEdwinUtils::EMenuCommandCreatePhoneCall))
            {
                aCommands.AppendL(command);
            }
        }
    }

    // add Form commands always
    TInt numCommands = iCommandList->Count();
    for (TInt i = 0; i < numCommands; i++)
    {
        const CMIDCommand* command = iCommandList->At(i).iCommand;
        if (((command->CommandType() == MMIDCommand::EOk) ||
                (command->CommandType() == MMIDCommand::EItem)) &&
                (command->Id() != CMIDEdwinUtils::EMenuCommandFetchPhoneNumber) &&
                (command->Id() != CMIDEdwinUtils::EMenuCommandFetchEmailAddress) &&
                (command->Id() != CMIDEdwinUtils::EMenuCommandCreatePhoneCall))
        {
            if (!iSelectCommandEnabled && command == iSelectCommand)
            {
                continue;
            }
            aCommands.AppendL(command);
        }
    }
}

/**
 * Retrieves index of the highest priority SCREEN command
 * where there is no SCREEN command it retreives index
 * of the highest priority HELP command number.
 *
 * When there is no such command type, it returns KErrNotFound
 **/
TInt CMIDDisplayable::GetHighestPriorityScreenOrHelpCommand() const
{
    TInt cmdNum = iCommandList->HighestPriorityCommand(MMIDCommand::EScreen);
    if (cmdNum == KErrNotFound)
        cmdNum = iCommandList->HighestPriorityCommand(MMIDCommand::EHelp);
    return cmdNum;
}


// ---------------------------------------------------------------------------
// Removes the title, effectively showing the midlet name as the title.
// Used by popup-style TextBox which maintains its own title.
// ---------------------------------------------------------------------------
void CMIDDisplayable::ClearTitleL()
{
    delete iTitle;
    iTitle = NULL;
    if (this->iActive)
    {
        this->ShowTitleL();
    }
    iHasTitle = EFalse;
}

// ---------------------------------------------------------------------------
// Replaces the CEikButtonGroupContainer.
// ---------------------------------------------------------------------------
void CMIDDisplayable::SetCba(CEikButtonGroupContainer* aCba)
{
    DEBUG("+ CMIDDisplayable::SetCba");

    ASSERT(aCba);
    iCba = aCba;
    iCba->DrawDeferred();

    DEBUG("- CMIDDisplayable::SetCba");
}

// ---------------------------------------------------------------------------
// Reset the status of all sks and commands so that no command is mapped to any sk
// ---------------------------------------------------------------------------
void CMIDDisplayable::ResetSoftKeysAndCommands(const RArray<CMIDCommandList*>& aLists)
{
    TInt numSoftKeys = iSoftKeys.Count();
    for (TInt i = 0; i < numSoftKeys; i++)
    {
        iSoftKeys[i]->Map(NULL);
    }

    TInt numLists = aLists.Count();
    for (TInt j = 0; j < numLists; j++)
    {
        if (aLists[j])
        {
            aLists[j]->UnMapCommands();
        }
    }
}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
TBool CMIDDisplayable::TryDetectLongTapL(const TPointerEvent& aPointerEvent)
{
    //Following code disables the cancelling of long tap to be determined as a normal tap.
    //Cancelling of long tap (up event during long tap animation) shouldn't do anything
    //else than cancel the animation.
    if (iLongTapDetector->IsAnimationRunning() &&
            aPointerEvent.iType == TPointerEvent::EButton1Up)
    {
        iLongTapDetector->PointerEventL(aPointerEvent);
        return ETrue;
    }
    // If there is a context menu available, forward events to long tap detector
#ifdef RD_JAVA_S60_RELEASE_9_2
    if (NumCommandsForOkOptionsMenu() > 0)
#else
    if (NumCommandsForOkOptionsMenu() > 1)
#endif // RD_JAVA_S60_RELEASE_9_2
    {
        iLongTapDetector->PointerEventL(aPointerEvent);
    }
    if (aPointerEvent.iType == TPointerEvent::EButton1Up && iLongTapDetected)
    {
        iLongTapDetected = EFalse;
        return ETrue;
    }
    else
    {
        return EFalse;
    }
}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
void CMIDDisplayable::HandleLongTapEventL(const TPoint& /*aPenEventLocation*/,
        const TPoint& aPenEventScreenLocation)
{
    // Long tap was detected -> show context menu. However, make sure that the
    // context sensitive menu is still there; it could be that the menu items
    // have been changed while waiting for the long tap event
    RArray<CEikMenuPaneItem::SData> menuItems; // the list of menu items
    menuItems.Reset();

    if (iItemCommandList && iItemCommandList->Count() > 0)
    {
#ifdef RD_JAVA_S60_RELEASE_9_2
        PopulateMenuItemsWithListL(CMIDMenuHandler::EPopUpMenu, menuItems, iItemCommandList, EFalse);
#else
        PopulateMenuItemsWithListL(CMIDMenuHandler::EOkMenu, menuItems, iItemCommandList, EFalse);
#endif // RD_JAVA_S60_RELEASE_9_2
    }

    // Add form commands always
#ifdef RD_JAVA_S60_RELEASE_9_2
    PopulateMenuItemsWithListL(CMIDMenuHandler::EPopUpMenu, menuItems, iCommandList, EFalse);
#else
    PopulateMenuItemsWithListL(CMIDMenuHandler::EOkMenu, menuItems, iCommandList, EFalse);
#endif // RD_JAVA_S60_RELEASE_9_2
#ifdef RD_JAVA_S60_RELEASE_9_2
    if (menuItems.Count() > 0)
#else
    if (menuItems.Count() > 1)
#endif // RD_JAVA_S60_RELEASE_9_2
    {
        // recreate stylus popup menu because it does not have method
        // for clearing the menu items
        delete iStylusPopupMenu;
        iStylusPopupMenu = NULL;
        iStylusPopupMenu = CAknStylusPopUpMenu::NewL(this, aPenEventScreenLocation);
        for (TInt i = 0; i < menuItems.Count(); i++)
        {
            iStylusPopupMenu->AddMenuItemL(menuItems[i].iText, menuItems[i].iCommandId);
        }

        iStylusPopupMenu->SetPosition(aPenEventScreenLocation);
        iStylusPopupMenu->ShowMenu();
    }

    iLongTapDetected = ETrue;
}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
MAknsControlContext* CMIDDisplayable::BackGroundControlContext()
{
    return iBackGroundControlContext;
}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
CMIDCommandList* CMIDDisplayable::MainCommandList() const
{
    return iCommandList;
}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
MMIDComponent* CMIDDisplayable::Component() const
{
    return iContent;
}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
TInt CMIDDisplayable::CommandCount()
{
    return iCommandList->Count();
}

// ---------------------------------------------------------------------------
// Returns a boolean indicating whether a non-null title has been set by
// calling the Displayable's SetTitle method.
// ---------------------------------------------------------------------------
TBool CMIDDisplayable::HasTitle() const
{
    return iHasTitle;
}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
CMIDMenuHandler* CMIDDisplayable::MenuHandler() const
{
    return iMenuHandler;
}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
TBool CMIDDisplayable::IsActive() const
{
    return iActive;
}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
TTypeUid::Ptr CMIDDisplayable::MopSupplyObject(TTypeUid aId)
{
    if (aId.iUid == MAknsControlContext::ETypeId && iBackGroundControlContext)
    {
        return MAknsControlContext::SupplyMopObject(aId, iBackGroundControlContext);
    }
    ASSERT(iMenuHandler);
    return SupplyMopObject(aId, iMenuHandler->Cba(), iMenuHandler->MenuBar());
}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
void CMIDDisplayable::NotifyContentDestroyed()
{
    iContent = NULL;
    iContentControl = NULL;

    iEnv.RemoveObserver(*this);
}

/** Check virtual keyboard status. Only for touchscreen.
  *  Returns a boolean variable:
  *  ETrue  - VKB is opened,
  *  EFalse - VKB is hidden.
 **/
#ifdef RD_TACTILE_FEEDBACK
TBool CMIDDisplayable::IsVKBOnScreen()
{
    TBool vkbOpen = EFalse;

    if (!iPenInputServerConnected && AknLayoutUtils::PenEnabled())
    {
        TInt err = iPenInputServer.Connect();
        iPenInputServerConnected = (err == KErrNone);
    }

    if (iPenInputServerConnected)
    {
        if (iPenInputServer.IsVisible())
        {
            vkbOpen = ETrue;
        }
    }

    return vkbOpen;
}
#endif //RD_TACTILE_FEEDBACK


#ifdef RD_SCALABLE_UI_V2
TUint CMIDDisplayable::OnScreenKeypadL()
{
    DEBUG("+ CMIDDisplayable::OnScreenKeypadL");

    if (RProcess().SecureId().iId != 0x102033E6)
    {
        // For standalone type apps we don't show keypad.
        return EOnScreenKeypadValueNo;
    }
    TUint onScreenKeypadValue = EOnScreenKeypadValueUndefined;

    std::auto_ptr<java::storage::JavaStorage> js(java::storage::JavaStorage::createInstance());
    java::storage::JavaStorageApplicationEntry_t entries;
    try
    {
        js->open();
        java::util::Uid uid;
        TUidToUid(iEnv.MidletSuiteUid(), uid);
        js->read(java::storage::MIDP_PACKAGE_TABLE, uid, entries);
        js->close();
    }
    catch (java::storage::JavaStorageException& ex)
    {
        DEBUG_INT("CMIDCanvas::IsNetworkIndicatorEnabledL: JavaStorage error: \
            reading MIDP_PACKAGE_TABLE failed, error code = %D", ex.mStatus);
    }
    java::storage::JavaStorageEntry attribute;
    attribute.setEntry(java::storage::ON_SCREEN_KEYPAD, L"");
    java::storage::JavaStorageApplicationEntry_t::const_iterator findIterator = entries.find(attribute);
    std::wstring res = L"";
    if (findIterator != entries.end())
    {
        res = (*findIterator).entryValue();
    }
    entries.clear();

    if (res == L"0")
    {
        onScreenKeypadValue = EOnScreenKeypadValueNo;
    }
    else if (res == L"1")
    {
        onScreenKeypadValue = EOnScreenKeypadValueGameActions;
    }
    else if (res == L"2")
    {
        onScreenKeypadValue = EOnScreenKeypadValueNavigationKeys;
    }

    DEBUG("- CMIDDisplayable::OnScreenKeypadL");

    return onScreenKeypadValue;
}
#endif // RD_SCALABLE_UI_V2

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
void CMIDDisplayable::SetEmphasis(CCoeControl* /*aMenuControl*/,TBool /*aEmphasis*/)
{

}

// ---------------------------------------------------------------------------
// Adds a rectangle to be excluded from redrawing (for DSA)
// ---------------------------------------------------------------------------
//
void CMIDDisplayable::AddDirectContentArea(const TRect& aRect)
{
    TDirectContentsRect rect(aRect);
    TInt index = iDirectContentsRects.Find(
                     rect, TIdentityRelation< TDirectContentsRect >(CMIDDisplayable::MatchDirectContentsRects));
    if (index == KErrNotFound)
    {
        TInt err = iDirectContentsRects.Append(rect);
        if (KErrNone == err)
        {
            UpdateDirectContentsRegion();
        }
        else
        {
            DEBUG_INT("CMIDDisplayable::AddDirectContentArea - RArray append error %d", err);
        }
    }
    else
    {
        iDirectContentsRects[ index ].iRefCount++;
    }
}

// ---------------------------------------------------------------------------
// Removes a rectangle to be excluded from redrawing (for DSA)
// ---------------------------------------------------------------------------
//
void CMIDDisplayable::RemoveDirectContentArea(const TRect& aRect)
{
    TDirectContentsRect rect(aRect);
    TInt index = iDirectContentsRects.Find(
                     rect, TIdentityRelation< TDirectContentsRect >(CMIDDisplayable::MatchDirectContentsRects));
    if (index != KErrNotFound)
    {
        iDirectContentsRects[ index ].iRefCount--;
        if (iDirectContentsRects[ index ].iRefCount <= 0)
        {
            iDirectContentsRects.Remove(index);
            UpdateDirectContentsRegion();
        }
    }
}


// ---------------------------------------------------------------------------
// Matches two direct content rectangles and returns the result
// ---------------------------------------------------------------------------
//
TBool CMIDDisplayable::MatchDirectContentsRects
(
    const TDirectContentsRect& aLhs,
    const TDirectContentsRect& aRhs
)
{
    return (aLhs.iRect == aRhs.iRect);
}

// ---------------------------------------------------------------------------
// Update the direct content regions
// This should be called just after addition or removal of dc rectangle
// ---------------------------------------------------------------------------
//
void CMIDDisplayable::UpdateDirectContentsRegion()
{
    iDirectContentsRegion.Clear();
    TInt count = iDirectContentsRects.Count();
    for (int index = 0; index < count; index++)
    {
        iDirectContentsRegion.AddRect(iDirectContentsRects[ index ].iRect);
    }
}


TBool CMIDDisplayable::NoDirectContentAreaDefined()
{
    return iDirectContentsRegion.IsEmpty();
}

void CMIDDisplayable::SetPopupTextBox(TBool aPopup)
{
    iIsPopupTextBox = aPopup;
}

TBool CMIDDisplayable::IsPopupTextBox()
{
    return iIsPopupTextBox;
}

// ---------------------------------------------------------------------------
// Gets the location and anchor for all the softkey buttons
// for fullscreen canvas and stores it for later usage.
// The data have to marked as invalid in iFullscreenCanvasLabelCacheIsValid
// if any resolution / skin change is proceeding.
// ---------------------------------------------------------------------------
//
void CMIDDisplayable::RenewFullscreenCanvasLabelCacheL()
{
    DEBUG("+ CMIDDisplayable::RenewFullscreenCanvasLabelCacheL");

    if ((iCba)   &&
            (!iFullscreenCanvasLabelCacheIsValid) &&
            (iCommandList))
    {
        TPoint move(0, 0);
        if (iUseOnScreenKeypad)
        {
            // SK location have to be moved when OnScreenKeypad is active
            TSize diff(iDisplayableRect.Size() -
                       iAppUi->ApplicationRect().Size());
            move = diff.AsPoint();
        }

        for (TInt softKeyId = 0; softKeyId < KSoftKeyLabelPropertyNumberOfSoftKeys; softKeyId++)
        {
            iFullscreenCanvasLabelCache[softKeyId].iIsOn = EFalse;
            TBool SKVisibility = true;

            // Check if the MSK is used in Avkon
            if (softKeyId == KSoftKeyLabelPropertyPositionsMSKIndex)
            {
                TBool MSKEnabledInPlatform;
                CRepository* cenRep = NULL;
                TRAPD(err, cenRep = CRepository::NewL(KCRUidAvkon));
                if (!err)
                {
                    err = cenRep->Get(KAknMiddleSoftkeyEnabled, MSKEnabledInPlatform);
                    delete cenRep;
                }

                // Check the cases, in which the MSK is off
                if (!AknLayoutUtils::MSKEnabled() || !MSKEnabledInPlatform ||
                        !Layout_Meta_Data::IsMSKEnabled() ||
                        Layout_Meta_Data::IsLandscapeOrientation())
                {
                    SKVisibility = false;
                }

            }

            if (SKVisibility)
            {
                iCba->SetCommandSetL(R_AVKON_SOFTKEYS_EMPTY);
                TInt softKeyCbaPosition = KSoftKeyLabelPropertyPositionsInCBA[ softKeyId ];
                iCba->SetCommandL(softKeyCbaPosition, R_MIDP_SOFTKEY_OPTIONS);
                TInt cmdId = EAknSoftkeyOptions;
                TInt position = iCba->PositionById(cmdId);
                if ((position == softKeyCbaPosition) && iCba->IsCommandVisible(cmdId))
                {
                    CEikCommandButton *cmdButton = (CEikCommandButton*)iCba->ControlOrNull(cmdId);
                    if (cmdButton && cmdButton->IsVisible()  && !iCba->IsCommandDimmed(cmdId))
                    {
                        CEikLabel *label = (CEikLabel*)cmdButton->ComponentControl(0);
                        if (label && label->IsVisible())
                        {
                            iFullscreenCanvasLabelCache[softKeyId].iPosition
                            = label->PositionRelativeToScreen() + move;

                            iFullscreenCanvasLabelCache[softKeyId].iSize = label->Size();
                            TGulAlignment align = label->iAlignment;
                            TGulHAlignment hAlign = align.HAlignment();
                            switch (hAlign)
                            {
                            case EHLeft:
                                iFullscreenCanvasLabelCache[softKeyId].iAnchor = TSoftkeyLabel::EJavaLeft;
                                break;
                            case EHCenter:
                                iFullscreenCanvasLabelCache[softKeyId].iAnchor = TSoftkeyLabel::EJavaHCenter;
                                break;
                            case EHRight:
                                iFullscreenCanvasLabelCache[softKeyId].iAnchor = TSoftkeyLabel::EJavaRight;
                                break;
                            }
                            TGulVAlignment vAlign = align.VAlignment();
                            switch (vAlign)
                            {
                            case EVTop:
                                iFullscreenCanvasLabelCache[softKeyId].iAnchor |= TSoftkeyLabel::EJavaTop;
                                break;
                            case EVCenter:
                                iFullscreenCanvasLabelCache[softKeyId].iAnchor |= TSoftkeyLabel::EJavaVCenter;
                                break;
                            case EVBottom:
                                iFullscreenCanvasLabelCache[softKeyId].iAnchor |= TSoftkeyLabel::EJavaBottom;
                                break;
                            }
                            iFullscreenCanvasLabelCache[softKeyId].iIsOn = ETrue;
                        }
                    }
                }
            }
        }
    }

    iFullscreenCanvasLabelCacheIsValid = ETrue;

    DEBUG("- CMIDDisplayable::RenewFullscreenCanvasLabelCacheL");
}

void CMIDDisplayable::HandleCanvasForeground(TBool aForeground)
{
    CMIDCanvas* canvas = GetContentCanvas();
    if (canvas)
    {
        canvas->HandleForeground(aForeground);
    }
}

void CMIDDisplayable::HandleApplicationBackground()
{
    // If iCanvasKeypad exists, it notifies On-Screen Keypad
    // about current application focus lost.
    if (iCanvasKeypad)
    {
        iCanvasKeypad->HandleApplicationBackground();
    }

    HandleCanvasForeground(EFalse);
}

void CMIDDisplayable::ProcessMSKCommandL()
{
    if (iIdOfMSKCommand != KErrNotFound)
    {
        ProcessCommandL(iIdOfMSKCommand);
    }
}

void CMIDDisplayable::DisplayableBehindPopupIsDestroyed()
{
    // Old fullscreen Displayable is destroyed.
    iDisplayableBehindPopup = NULL;
}

void CMIDDisplayable::HideIndicator(CEikStatusPane* aSp, TInt aId)
{
    TUid uid = TUid::Uid(aId);
    CEikStatusPaneBase::TPaneCapabilities subPane = aSp->PaneCapabilities(uid);
    if (subPane.IsPresent() && subPane.IsAppOwned())
    {
        CCoeControl* ctrl = NULL;
        TRAP_IGNORE(ctrl = aSp->ControlL(uid));
        if (ctrl)
        {
            ctrl->MakeVisible(EFalse);
        }
    }
}

void CMIDDisplayable::HideIndicators()
{
    CEikStatusPane* pane = iAppUi->StatusPane();
    if (!pane)
        return;

    HideIndicator(pane, EEikStatusPaneUidSignal);
    HideIndicator(pane, EEikStatusPaneUidBattery);
    HideIndicator(pane, EEikStatusPaneUidIndic);
    HideIndicator(pane, EEikStatusPaneUidMessage);
    HideIndicator(pane, EEikStatusPaneUidClock);
    HideIndicator(pane, EEikStatusPaneUidDigitalClock);
}

CMIDCanvas* CMIDDisplayable::GetContentCanvas()
{
    CMIDCanvas* ret = NULL;
    if (iContent && iContentControl &&
            (iContent->Type() == MMIDComponent::ECanvas ||
             iContent->Type() == MMIDComponent::EGameCanvas))
    {
        ret = static_cast<CMIDCanvas*>(iContentControl);
    }
    return ret;
}

CPropertyWatch* CPropertyWatch::NewL(MMIDDisplayable* aDisplayable)
{
    DEBUG("+ CPropertyWatch::NewL");

    CPropertyWatch* self = new(ELeave) CPropertyWatch;
    CleanupStack::PushL(self);
    self->ConstructL(aDisplayable);
    CleanupStack::Pop(self);

    DEBUG("- CPropertyWatch::NewL");

    return self;
}

void CMIDDisplayable::FixOrientation()
{
    TBool tmpRestoreOrientation;

    if (!iAvkonAppUi)
    {
        return;
    }

    iOldUiOrientation = iAvkonAppUi->Orientation();
    tmpRestoreOrientation = (iOldUiOrientation == CAknAppUiBase::EAppUiOrientationUnspecified) ? ETrue : EFalse;

    // Fix the orientation when was set to unspecified only
    if (tmpRestoreOrientation)
    {
        TRAP_IGNORE(iAvkonAppUi->SetOrientationL(CAknAppUiBase::EAppUiOrientationLandscape));

        iRestoreOrientation = ETrue;
        ++iReleaseCnt;
    }

}

void CMIDDisplayable::ReleaseOrientation()
{
    if (!iAvkonAppUi)
    {
        return;
    }

    if (iReleaseCnt > 0)
    {
        --iReleaseCnt;
        if (iReleaseCnt == 0 && iRestoreOrientation)
        {
            TRAP_IGNORE(iAvkonAppUi->SetOrientationL(iOldUiOrientation));
            iRestoreOrientation = EFalse;
        }

    }
}

CPropertyWatch::CPropertyWatch()
        : CActive(0)
{
}

void CPropertyWatch::ConstructL(MMIDDisplayable* aDisplayable)
{
    DEBUG("+ CPropertyWatch::ConstructL");

    iDisplayable = static_cast< CMIDDisplayable* >(aDisplayable);
    User::LeaveIfError(iProperty.Attach(KCRUidAvkon, KAknKeyBoardLayout));
    CActiveScheduler::Add(this);
    iDelayTimer = CPeriodic::NewL(CActive::EPriorityStandard);
    RunL();

    DEBUG("- CPropertyWatch::ConstructL");
}

CPropertyWatch::~CPropertyWatch()
{
    DEBUG("+ CPropertyWatch::~CPropertyWatch");

    Cancel();
    iProperty.Close();

    if (iDelayTimer)
    {
        iDelayTimer->Cancel();
        delete iDelayTimer;
    }

    DEBUG("- CPropertyWatch::~CPropertyWatch");
}

void CPropertyWatch::DoCancel()
{
    iProperty.Cancel();
}

void CPropertyWatch::HandleDelayTimerEventL()
{
    iDelayTimer->Cancel();
    if (Layout_Meta_Data::IsLandscapeOrientation())
    {
        iDisplayable->HandleOnScreenKeypadVisual();
    }
}

TInt CPropertyWatch::DelayTimerCallbackL(TAny* aThis)
{
    CPropertyWatch* observer = static_cast<CPropertyWatch*>(aThis);
    observer->HandleDelayTimerEventL();
    return 0;
}

void CPropertyWatch::RunL()
{
    DEBUG("+ CPropertyWatch::RunL");

    //Resubscribe before processing new value to prevent missing updates
    iProperty.Subscribe(iStatus);
    SetActive();

    if (Layout_Meta_Data::IsLandscapeOrientation())
    {
        iDelayTimer->Cancel();
        iDelayTimer->Start(KTimerDelayValue, KTimerDelayValue,
                           TCallBack(DelayTimerCallbackL, this));
    }

    DEBUG("- CPropertyWatch::RunL");
}


// End of File
