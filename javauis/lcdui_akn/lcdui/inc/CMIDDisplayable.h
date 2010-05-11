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
* Description:  ?Description
*
*/


#ifndef CMIDDISPLAYABLE_H
#define CMIDDISPLAYABLE_H

// CEikBorderedControl inheritance in class declaration
#include <eikbctrl.h>
#include <eikmobs.h>
// needed for calling function SupplyMopObject in CMIDControlItem.cpp, CMIDDisplayable.cpp, CMIDForm.cpp
#include <eikmop.h>
#include <lcdui.h>
#include <eikmenub.h>
// used MAknLongTapDetectorCallBack interface in class declaration
// using CAknLongTapDetector* iLongTapDetector member
#include <aknlongtapdetector.h>

// Skin
// using API for icons in CMIDCanvas.cpp
// using API for skin in CMIDDisplayable.cpp
// using API for icons and skin in CMIDChoiceGroupModel.cpp
#include <AknsDrawUtils.h>// skin
// for backgroud control context (iBackGroundControlContext)
#include <AknsBasicBackgroundControlContext.h> //skin

#include "CMIDCommand.h"
// API needed for working with commands and command list (setting, getting, handling)
#include "CMIDCommandList.h"
// using CMIDMenuHandler::TMenuType
#include "CMIDMenuHandler.h"
// using TMidpCustomEvents enumeration
#include "S60LCDUICustomEvents.h"
#include "CMIDCanvasKeypad.h"
#include <e32property.h>

#ifdef RD_TACTILE_FEEDBACK
#include <touchfeedback.h>
#include <peninputclient.h>
#endif //RD_TACTILE_FEEDBACK

// Constants
const TInt KSoftKeyLabelPropertyNumberOfSoftKeys = 3;

// Forward declarations
class CMIDAppUi;
class CMIDSoftKey;
class CMIDUIManager;
class CAknAppUi;
class CMIDTicker;
class CCommandList;
class MMIDDisplayableObserver;
class CEikButtonGroupContainer;
class CAknStylusPopUpMenu;
class CPropertyWatch;

// Container for MIDlet Displayables.
NONSHARABLE_CLASS(CMIDDisplayable) : public CEikBorderedControl, public MMIDDisplayable,
        public MMIDEnvObserver, public MAknLongTapDetectorCallBack, public MEikMenuObserver
{
public:

#ifdef RD_SCALABLE_UI_V2
    /**
     * On screen keypad options for devices that do not have physical keypads.
     */
    enum TOnScreenKeypadValue
    {
        EOnScreenKeypadValueUndefined = 0,
        EOnScreenKeypadValueNo,
        EOnScreenKeypadValueNavigationKeys,
        EOnScreenKeypadValueGameActions,
        EOnScreenKeypadValueLskRsk
    };

    // is VKB on screen
    TBool IsVKBOnScreen();
#endif // RD_SCALABLE_UI_V2

    /**
     * On-Screen Keypad SoftKey positions(used when hw-qwerty does not have LSK&RSK)
     */
    enum TOnScreenKeypadSoftKeysPosition
    {
        ESoftkeysBottom = 0,//default value
        ESoftkeysRight
    };

    /**
     * Returns SoftKey position defined by JAD parameter(default position is bottom)
     */
    TInt GetSKPositionForOSK();

    // Construction
    static CMIDDisplayable* NewL(MMIDEnv& aEnv,CMIDUIManager& aUIManager);

    // From MMIDDisplayable
    void SetTitleL(const TDesC* aTitle);
    void SetTickerL(MMIDTicker* aTicker);
    MMIDTicker* Ticker() const;
    void AddCommandL(MMIDCommand* aCommand);
    void RemoveCommand(MMIDCommand* aCommand);
    TSize ContentSize() const;
    void HandleCurrentL(TBool aCurrent);
    void SetComponentL(MMIDComponent& aComponent);
    MMIDComponent* Component() const;
    void SetFullScreenModeL(TBool aFullScreen);
    CCoeControl& ContentWindow();
    TBool SoftKeyLabelLocation(TInt aSoftKeyId, TPoint& aPosition, TSize& aSize);
    TInt SoftKeyLabelAnchor(TInt aSoftKeyId);
    void DrawNow();
    void ChangeOSKBackgroundState(TBool aOSKBackgroundState);

    /**
     * Sets a command listener existence info.
     * @since Series S60 3.2
     * @param aExistence ETrue if command listener is set
     */
    void SetCommandListenerExistence(TBool aExistence);

    // From MMIDComponent
    void Dispose();

    // From MMIDEnvObserver
    void HandleSwitchOnL(TBool aSwitchOn);
    void HandleForegroundL(TBool aForeground);
    void HandleResourceChangeL(TInt aType);

    // Title handling
    TBool HasTitle() const;
    void ShowTitleL();
    HBufC*  Title();
    void ClearTitleL();

    // Command handling
    CMIDCommandList* MainCommandList() const;
    CMIDCommand* FindCommandWithId(TInt aCommandId) const;

    /**
     * Find the correct softkey commands, if any,  and draw the CBA.
     * Finally draw the CBA.
     *
     * @see RestoreMenuL()
     **/
    void InitializeCbasL();
    void SetCba(CEikButtonGroupContainer* aCba);
    TBool ShowOkOptionsMenuL();
    TInt NumCommandsForOkOptionsMenu() const;
    TInt GetHighestPriorityScreenOrHelpCommand() const;
    void SetItemCommandList(CMIDCommandList* aList, CMIDCommand* aMSKCommand);
    void SetMSKCommand(CMIDCommand* aMSKCommand);
    /**
     * Stores select command to Displayable
     * @param aSelectCommand a command used in MSK and in menu
     */
    void SetSelectCommand(CMIDCommand* aSelectCommand);
    /**
     * Disables or enables select command
     * @aEnableSelectCommand ETrue enables select command and EFalse disables select command
     */
    void SetSelectCommandState(TBool aEnableSelectCommand);

    TInt CommandCount();
    void ProcessCommandL(TInt aCommandId);
    void DisplayableBehindPopupIsDestroyed();
    // Processes native Help command event to java
    void HandleHelpCommandL();
    // Get number of inserted help commands
    TInt NumCommandsForHelpOptionsMenu() const;
    void HandleHighestPriorityScreenOrHelpCommandL();

#ifdef RD_SCALABLE_UI_V2
    // This function can be moved out from RD_SCALABLE_UI_V2 flag if needed.
    // It is behind this flag because currently it is used only by Touch.
    /**
     * Passes call to ProcessCommandL (TInt aCommandId) function.
     * This is useful in situations in which the internal Command ID is not available.
     *
     * @param aCommand to be executed.
     * @since S60 5.0
     */
    void ProcessCommandL(CMIDCommand* aCommand);
#endif // RD_SCALABLE_UI_V2

    void CreateMenuItemsL(const CMIDMenuHandler::TMenuType& aMenuType,
                          RArray<CEikMenuPaneItem::SData>& aMenuItems);
    // Skin
    MAknsControlContext* BackGroundControlContext();

    /** From MAknLongTapDetectorCallBack */
    void HandleLongTapEventL(const TPoint& /*aPenEventLocation*/, const TPoint& /*aPenEventScreenLocation*/);

    /**
     * Way to report pointer event to displayable so that the component will get the
     * default long tap functionality. This method should be called when ever a pointer
     * event occurs in an ui component. Returns true if the event is consumed and should
     * not be forwarded any more (e.g. pointer up event after a detected long tap).
     **/
    TBool TryDetectLongTapL(const TPointerEvent& aPointerEvent);

    // Misc.
    TBool IsFullScreenMode() const;
    CMIDMenuHandler* MenuHandler() const;
    TBool IsActive() const;

    /**
     * Content component notifies Displayable about its deletion.
     *
     * @note It might happen that content is destroyed before displayable
     * and that's why we need to notify displayable about its content deletion.
     * @see Displayable.java: registeredFinalize()
     **/
    void NotifyContentDestroyed();

    /**
     * Gets a command listener existence info.
     * @since Series S60 3.2
     * @return ETrue if a command listener is set
     */
    TBool IsCommandListenerSet() const;
    /**
     * Get UI manager
     * @return CMIDUIManager instance
     *
     * @since s60
     */
    virtual CMIDUIManager* GetUIManager() const;

    /**
     * Sets S60 Selection Key Compatibility
     */
    void SetS60SelectionKeyCompatibility(TBool aS60SelectionKeyCompatibility);

    /**
    Adds a rectangle which is excluded from redrawing (for DSA)
    @param "aRect" Rentangle to be exluded from redrawing.
    */
    void AddDirectContentArea(const TRect& aRect);

    /**
    Removes a rectangle which is excluded from redrawing (for DSA)
    @param "aRect" Rentangle to be removed from exclusion list.
    */
    void RemoveDirectContentArea(const TRect& aRect);

    /**
    Checks if there were some direct content area added
    @return ETrue if there no DC Area has been added
    */
    TBool NoDirectContentAreaDefined();

    /**
     * Sets popup style TextBox boolean value. Called by CMIDTextBoxDialogControl when
     * TextBox presentation is pop-up.
     *
     * @param "aPopup" boolean value indicating whether the textbox is popup or not.
     *
     * @since S60 5.0
     */
    void SetPopupTextBox(TBool aPopup);

    /**
     * Returns boolean value indicating whether this displayable is pop-up TextBox.
     *
     * @return TBool
     *
     * @since S60 5.0
     */
    virtual TBool IsPopupTextBox();

    /**
    * Handles the On-Screen Keypad visual appearance.
    *
    * @return void
    *
    * @since S60 5.0
    */
    void HandleOnScreenKeypadVisual();

    /**
    * Notify a CMIDDisplayable then application is switch to background.
    *
    * @return void
    *
    * @since S60 5.0
    */
    void HandleApplicationBackground();

    /**
     * Function process command which is mapped to MSK
     *
     * @return void
     *
     * @since S60 5.0
     */
    void ProcessMSKCommandL();

private:
    // Construction and destruction
    CMIDDisplayable(MMIDEnv& aEnv,CMIDUIManager& aUIManager);
    void ConstructL();
    ~CMIDDisplayable();

    // From CCoeControl
    void Draw(const TRect& aRect) const;
    virtual TInt CountComponentControls() const;
    CCoeControl* ComponentControl(TInt aIndex) const;
    void SizeChanged();
    TTypeUid::Ptr MopSupplyObject(TTypeUid aId);

    // Private command handling
    void PopulateMenuItemsWithListL(const CMIDMenuHandler::TMenuType& aMenuType,
                                    RArray<CEikMenuPaneItem::SData>& aItems, CMIDCommandList* aCommandList, TBool aSeparator);
    TBool CommandIsMappedToSk(const CMIDCommand* aCommand) const;
    void HandleStandardCommandL(const TCommandEntry& aCmdEntry);
    void HandleItemCommandL(const TCommandEntry& aCmdEntry);
    TInt GetInternalCommandIdFor(CMIDCommand* aCommand) const;
    void GetOkOptionsMenuCommands(RPointerArray<CMIDCommand>& aCommands) const;
    void ResetSoftKeysAndCommands(const RArray<CMIDCommandList*>& aLists);

    // Visual layouting and updating
    void UpdateVisualAppearanceL();
    void UpdateTickerL();
    void Layout();
    void SizeWindowAndToolBar();
    void ReplaceBehindAlertIfNeeded();
    void UpdateDisplayableRect();

    // Activation handling
    void HandleActivatedL();
    void HandleDeactivated();


    // The structure to store the Direct content bounds and its identification number
    struct TDirectContentsRect
    {
        /**
         * Default ctor
         */
        TDirectContentsRect()
                : iRefCount(0) {}

        /**
         * Ctor
         * @param aRect Rectangle
         */
        TDirectContentsRect(const TRect& aRect)
                : iRect(aRect), iRefCount(1) {}

        TRect iRect;
        TInt iRefCount;
    };

    /**
     * Determines whether two TDirectContentsRect type objects match
     * @param aLhs First object
     * @param aRhs Second object
     * @return True if matched
     * @see RArray::Find
     */
    static TBool MatchDirectContentsRects(
        const TDirectContentsRect& aLhs, const TDirectContentsRect& aRhs);

    /**
     * Updates <code>iDirectContentsRegion</code> according to
     * the <code>iDirectContentsRects</code> array.
     */
    void UpdateDirectContentsRegion();


    // From MEikMenuObserver
    void SetEmphasis(CCoeControl* aMenuControl,TBool aEmphasis);
    /**
     * OnScreenKeypad
     * Returns canvas rect from LAF when using OnScreenKeypad.
     *
     * @return TRect                    Canvas Rect
     *
     * @since S60 5.0
     */
    TRect GetCanvasRectFromLaf();

    /**
    * Set correct On-Screen keyboard type.
    *
    * @return void
    *
    * @since S60 5.0
    */
    void UpdateOnScreenKeypadSettings();

    /**
    * Read On-Screen keyboard MIDlet suite settings
    *
    * @return void
    *
    * @since S60 5.0
    */
    void ReadOnScreenKeypadTypeFromSuiteSettings();

#ifdef RD_SCALABLE_UI_V2
    /**
     * Returns midlet suite setting for on screen keypad.
     * The setting is retrieved from the User preferences API and it requires file accessing, thus
     * this function should not be called repeatedly because of performance issues.
     *
     * @return TOnScreenKeypadValue     Current setting value.
     *                                  Returns EOnScreenKeypadValueUndefined if reading of the setting fails.
     *
     * @since S60 5.0
     */
    TUint OnScreenKeypadL();
#endif // RD_SCALABLE_UI_V2

    void RenewFullscreenCanvasLabelCacheL();

    /**
     * In case content is Canvas, notifies it about changed
     * foreground status.
     *
     * @param aForeground   ETrue, if displayable is on foreground,
     *                      EFalse otherwise.
     * @since S60 9.2
     */
    void HandleCanvasForeground(TBool aForeground);

    void HideIndicator(CEikStatusPane* aSp, TInt aId);
    void HideIndicators();

private:
    CMIDAppUi* iAppUi;
    CMIDUIManager* iUIManager;
    MMIDEnv& iEnv;
    CAknLongTapDetector* iLongTapDetector;
    TBool iLongTapDetected;
    CAknStylusPopUpMenu* iStylusPopupMenu;

    // Controls
    CCoeControl* iContentControl;
    CEikButtonGroupContainer* iCba;
    // Peers
    CMIDTicker* iTicker;
    MMIDComponent* iContent;

    //The main displayable commands
    CMIDCommandList* iCommandList;

    //Displayable behind Alert and PopupTextBox
    CMIDDisplayable* iDisplayableBehindPopup;

    //The commands associated with form items
    CMIDCommandList* iItemCommandList;

    // The command associated with middle soft key, may be NULL
    CMIDCommand* iMSKCommand;

    //Sofkey information. For each softkey on the device it
    //stores the allowed commands types, the mapped command
    //or default CBA button
    RPointerArray<CMIDSoftKey> iSoftKeys;

    CMIDMenuHandler* iMenuHandler;
    HBufC* iTitle;
    TBool iHasTitle;

    //Skin
    CAknsBasicBackgroundControlContext* iBackGroundControlContext;

    // State
    TBool iIsFullScreenMode;
    TBool iActive;
    TBool iCommandListenerExistence;
    TRect iDisplayableRect;
    // current number of item commands
    TInt  iItemCommandsCount;

    CMIDCommand* iSelectCommand;
    TBool iSelectCommandEnabled;

    TBool iS60SelectionKeyCompatibility;

    //OnScreenKeypad specific members
    CMIDCanvasKeypad*    iCanvasKeypad;
    CMIDCanvas* iCanvas;
    TBool iUseOnScreenKeypad;
    TInt iOnScreenKeyboardType;
    TInt iSKPositionWithQwerty;

    mutable RRegion iUpdateRegion;
    RRegion iDirectContentsRegion;
    // Direct content bounds list
    RArray< TDirectContentsRect > iDirectContentsRects;

    // Structure to store the softkey label position and anchor
    struct TSoftkeyLabel
    {
        TPoint iPosition;
        TSize  iSize;
        // Java anchor constant as defined in class Graphics
        enum TJavaAnchor
        {
            EJavaHCenter = 1,
            EJavaVCenter = 2,
            EJavaLeft    = 4,
            EJavaRight   = 8,
            EJavaTop     = 16,
            EJavaBottom  = 32
        };
        TInt  iAnchor;
        // Shows if the sk is used
        TBool iIsOn;
    };

    // The storage of softkey labels data
    // This is used for the increasing of the performace of getting SK properties
    // The data validity is stored in iFullscreenCanvasLabelCacheIsValid
    // The data are renewed/revalidated by RenewFullscreenCanvasLabelCache()
    TSoftkeyLabel iFullscreenCanvasLabelCache[ KSoftKeyLabelPropertyNumberOfSoftKeys ];

    // Shows if the data in iFullscreenCanvasLabelCache are valid
    TBool iFullscreenCanvasLabelCacheIsValid;

    // VKB status
#ifdef RD_TACTILE_FEEDBACK
    RPeninputServer iPenInputServer;
    TBool iPenInputServerConnected;
#endif

    // Used by TextBox indicating whether the textbox is popup or not.
    TBool iIsPopupTextBox;

    /**
     *  Initialize iPropertyWatch as CPropertyWatch. Active Object will be
     *  removed from Active Scheduler in CMIDDisplayable (destructor).
     */
    CPropertyWatch* iPropertyWatch;

    //stores id of command mapped to the MSK
    TInt iIdOfMSKCommand;

};




// Timer delay
const TInt KTimerDelayValue = 200000;//0.2s

/**
An active object that tracks changes to the active keyboard properties
*/
NONSHARABLE_CLASS(CPropertyWatch) : public CActive
{
public:
    static CPropertyWatch* NewL(MMIDDisplayable* aDisplayable);
    ~CPropertyWatch();
private:
    CPropertyWatch();
    void ConstructL(MMIDDisplayable* aDisplayable);
    void RunL();
    void DoCancel();
    void HandleDelayTimerEventL();
    static TInt DelayTimerCallbackL(TAny* aThis);

    CMIDDisplayable* iDisplayable;
    RProperty iProperty;

    /**
    * Timer delay for reacting to keypad opening/closing.
    */
    CPeriodic*     iDelayTimer;

};



#endif // CMIDDISPLAYABLE_H

// End of File
