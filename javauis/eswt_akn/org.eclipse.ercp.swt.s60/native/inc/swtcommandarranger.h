/*******************************************************************************
 * Copyright (c) 2005, 2010 Nokia Corporation and/or its subsidiary(-ies).
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *
 * Contributors:
 *     Nokia Corporation - S60 implementation
 *******************************************************************************/


#ifndef SWTCOMMANDARRANGER_H
#define SWTCOMMANDARRANGER_H


#include <eikcmobs.h>
#include "eswtmobileextensions.h"


class CSwtCommandContainer;
class CSwtSubCommandContainer;
class CSwtCommandMenu;


static const TInt KCbaButtonCountMax = 3;


/**
 * CSwtCommandArranger
 * @lib eswt
 */
NONSHARABLE_CLASS(CSwtCommandArranger)
        : public CBase
        , public MSwtCommandArranger
        , public MEikCommandObserver
{
// Methods
public:
    static CSwtCommandArranger* NewL(MSwtDisplay& aDisplay);
    virtual ~CSwtCommandArranger();

private:
    inline CSwtCommandArranger(MSwtDisplay& aDisplay);
    void ConstructL();
    void ClearCba();
    void ClearMenu();
    void ComputeAndRefreshCbaAndMenuL(TBool aFocusedControlOnly);
    void CleanCbaAndMenuAfterLeaveInCompute();
    void AddSubCommandsToCommandMenuL(const MSwtControl* aControl,
                                      const CSwtSubCommandContainer& aSubCommandContainer,
                                      CSwtCommandMenu& aCommandMenu);
    static TInt CommandContainerCompare(const CSwtCommandContainer& aFirst,
                                        const CSwtCommandContainer& aSecond);
    static TInt CommandCompare(const MSwtCommand& aFirst, const MSwtCommand& aSecond);
    TBool ControlIsInFocusHierarchy(const MSwtControl& aControl);
    static TBool CbaCategoryIsAvailable(TInt aCategory,
                                        const TFixedArray<TBool, KCbaButtonCountMax>& aCbaAvailability,
                                        TInt aNeutralCommandCurrentNumber);
    static TInt CbaCategoryOfCommand(const MSwtCommand& aCommand);
    static TInt FindAvailableNeutralButtonInCba(
        const TFixedArray<TBool, KCbaButtonCountMax>& aCbaAvailability);
    void AssignCommandToCbaAndMenuL(TFixedArray<TBool, KCbaButtonCountMax>& aCbaIsAvailable,
                                    const MSwtCommand& aCommand, const MSwtControl& aControl, const TInt& aIndexOfContainer);
    void AssignMSKToCbaL();
    void ComputeNumberOfContextOptionsMenuItems();
    const MSwtControl* GetFocusedControl() const;
    const MSwtControl* GetFirstFocusedControl() const;

#ifdef RD_SCALABLE_UI_V2
    /**
     * Checks (recursively for command groups) if there is at least one OK/SELECT command that should
     * be shown in stylus popup menu. aCmdContainer or aSubCmdContainer must be a valid pointer and
     * the other one must be set to NULL!
     *
     * @param aCmdContainer     Pointer to CSwtCommandContainer or NULL
     * @param aSubCmdContainer  Pointer to CSwtSubCommandContainer or NULL
     * @return  ETrue, if there is at least command that should be added to stylus popup menu.
     */
    TBool ContainsStylusPopupMenuCommands(CSwtCommandContainer* aCmdContainer,
                                          CSwtSubCommandContainer* aSubCmdContainer) const;

    /**
     * Adds recursively commands from command containers to the stylus popup menu. Only OK and SELECT type of
     * commands are added. aCmdContainer or aSubCmdContainer must be a valid pointer and the other one must be set to NULL!
     * iStylusPopupMenuCommands->Reset() must be called before calling this method.
     *
     * @param aCmdContainer     Pointer to CSwtCommandContainer or NULL
     * @param aSubCmdContainer  Pointer to CSwtSubCommandContainer or NULL
     */
    void AddCommandsToStylusPopupCommandMenu(CSwtCommandContainer* aCmdContainer,
            CSwtSubCommandContainer* aSubCmdContainer);


    /**
     * Return the index of the CSwtCommandContainer that contains the commands for aControl.
     * @param aControl      The control whose command container is returned.
     * @return  Then index of the aControl's command container
     */
    TInt GetIndexOfCommandContainer(const MSwtControl& aControl) const;
#endif //RD_SCALABLE_UI_V2    

// From MSwtCommandArranger
public:
    void ControlFocusLost();
    void ControlFocusGained(const MSwtControl& aControl);
    void ActiveShellLost();
    void ActiveShellGained(const MSwtShell& aShell);
    void CommandAddedL(const MSwtCommand& aCommand);
    void CommandRemoved(const MSwtCommand& aCommand);
    void CommandContentUpdated(const MSwtCommand& aCommand);
    void CommandEnablingUpdated(const MSwtCommand& aCommand);
    void CommandSetAsDefault(const MSwtCommand& aCommand);
    TInt NumberOfCommandsInCba() const;
    TBool ShowMenuL(TSwtMenuType aMenuType);
    TBool DoContextSensitiveOperationL();
    TBool DoClearOperationL();
    TBool DoHelpOperationL();
    TBool IsContextSensitiveOperationSet() const;
    TBool IsClearOperationSet() const;
    TBool IsDefaultCommandSet() const;
    void RefreshCba(TBool aFocusedControlOnly = EFalse);
    void UpdateMSKLabelL();
    void UpdateRightSoftkeyCommandAndObserver(TInt aCommandId,
            const TDesC& aText, MEikCommandObserver& aCommandObserver);
    TBool HandleEnterKeyL();
#ifdef RD_SCALABLE_UI_V2
    TBool ControlHasStylusPopupMenu(const MSwtControl& aControl) const;
    const MSwtMenu* GetStylusPopupCommandsMenu(const MSwtControl& aControl);
#endif //RD_SCALABLE_UI_V2        

// From MEikCommandObserver
public:
    void ProcessCommandL(TInt aCommandId);

// Data
private:
    static const TInt KTypeWeight[];      // Importance of each type of command
    MSwtDisplay& iDisplay;                // Related display
    CEikButtonGroupContainer& iCba;
    RPointerArray<CSwtCommandContainer> iCommands; // Commands sorted by parent control
    CSwtCommandContainer* iSearchedContainer; // Utility container used to perform find operation
    TFixedArray<const MSwtCommand*, KCbaButtonCountMax> iCbaCommands; // Commands mapped to CBA
    TInt iButtonCount;                    // Button count
    TInt iCbaIndexWhenComputeLeave;       // Used to clean CBA if ComputeAndRefreshCbaAndMenuL Leave
    CSwtCommandMenu* iCommandMenu;        // Menu for commands
    CSwtCommandMenu* iCommandMenuOK;      // Menu for OK and SELECT commands
    RPointerArray<CSwtCommandMenu> iCommandCascadeMenus; // Cascade menu panes related to commandgroup commands
    const MSwtCommand* iDefaultCommand;   // The default command accessed by the selection key
    const MSwtCommand* iClearCommand;     // The clear command accessed by the backup key
    const MSwtCommand* iSelectionCommand; // The top selection type command associated with a focused control
    /**
     * Pointer to the help command triggered by the Help key.
     * Can be NULL.
     * @Not Own
     */
    const MSwtCommand* iHelpCommand;
    TBool iCbaIsBeingComputed;
    TInt  iNumberOfContextOptionMenuItems;
    TBool iRightSoftkeyObserverAddedByControl;
    TInt  iRightSoftkeyCommandIdFromControl;

#ifdef RD_SCALABLE_UI_V2
    CSwtCommandMenu* iStylusPopupMenuCommands; // Commands for stylus popup menu, used when stylus poup is opened for nonfocused control
#endif
};


#endif // SWTCOMMANDARRANGER_H
