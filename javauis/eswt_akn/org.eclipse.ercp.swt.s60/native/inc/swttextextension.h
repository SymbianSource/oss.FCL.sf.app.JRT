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


#ifndef SWTTEXTEXTENSION_H
#define SWTTEXTEXTENSION_H


#include <swttext.h>
#include <aknnavide.h>
#include "eswtmobileextensions.h"


class CSwtControlMenu;
class CSwtMenuItem;


/**
 * CSwtTextExtension
 * @lib eswt
 */
NONSHARABLE_CLASS(CSwtTextExtension)
        : public CSwtText
        , public MSwtTextExtension
        , public MEikCommandObserver
{
public:
    /**
    * The 1st and 2nd phase constructor wrapper
    * @param aDisplay The Display to associate with
    * @param aPeer The peer to its java counterpart
    * @param aParent Parent composite
    * @param aStyle The style
    */
    static CSwtTextExtension* NewL(MSwtDisplay& aDisplay, TSwtPeer aPeer,
    MSwtComposite& aParent, TInt aStyle);


private:
    /**
    * Contructor.
    * @param aDisplay The Display to associate with
    * @param aPeer The peer to its java counterpart
    * @param aParent Parent composite
    * @param aStyle The style
    */
    inline CSwtTextExtension(MSwtDisplay& aDisplay, TSwtPeer aPeer,
                             MSwtComposite& aParent, TInt aStyle);

    /**
    * Destructor
    */
    virtual ~CSwtTextExtension();

    /**
    * Second phase constructor
    */
    void ConstructL();


public:
    /**
    * From MSwtControlMenu
    * Gets control menu to menu arranger
    * @return a pointer to MSwtMenu
    */
    const MSwtMenu* GetControlMenu() const;

#ifdef RD_SCALABLE_UI_V2
    const MSwtMenu* GetStylusPopupControlMenu() const;
#endif //RD_SCALABLE_UI_V2

public:
    /**
    * From MSwtTextExtension
    * Returns to MSwtcontrol interface
    * @return Pointer to MSwtText
    */
    MSwtText* Text();

    /**
    * From MSwtTextExtension
    * Sets the type style for the TextExtension
    * @param aTypeStyle the style of the TextExtension
    */
    void SetTypeStyleL(TInt aTypeStyle);

    /**
    * From MSwtTextExtension
    * Sets the inputmode, casingmodifiers and local languages
    * @param aCasingModifiers casing modifiers for the text
    * @param aMode text input mode
    */
    void SetInitialInputModeL(TCasingModifier aCasingModifier, const TDesC* aMode);


public:
    /**
    * From MEikCommandObserver
    * Get called when command pressed
    * @param aCmdId id of the command
    */
    void ProcessCommandL(TInt aCmdId);

private:
    /**
    * Read the Email Address
    */
    void FetchEmailL();

    /**
     * Read the URL
     */
    void FetchUrlL();

    /**
     * Select the Language
     * @param aCharSubset the selected character subset
     * @param aDefaultLanguage the cached language
     * @return language
     */
    TLanguage SelectLanguage(TInt aCharSubset, TLanguage aDefaultLanguage);

    /**
     * checks the Languages which has same character subset
     * @param aDefaultLanguage the cached language
     * @param aCharSubset the selected character subset
     * @return ETrue if @param aLanguage belongs to @param aCharSubset otherwise EFalse
     */
    TBool IsLanguageInCharSubset(TLanguage aLanguage, TInt aCharSubset);

    /**
     * From ASwtControlBase
     * Sets the InitialInputMode active
     */
    void FocusChanged(TDrawNow aDrawNow);


    /**
     * Makes sure that status pane is visible. Needed because used dialogs
     * don't work correctly without a status pane.
     */
    void MakeStatusPaneLargeL(MSwtShell* aShell,
                              TBool& aFullScreenMode,
                              MSwtShell::TSwtStatusPaneStyle& aStyle);

    /**
     * Restores the status pane after a call To MakeStatusPaneLargeL.
     */
    void RestoreStatusPaneL(MSwtShell* aShell,
                            const TBool& aFullScreenMode,
                            const MSwtShell::TSwtStatusPaneStyle& aStyle);

// Data
private:
    /**
     * Pointer to the control menu
     * Own
     */
    CSwtControlMenu* iMenu;

    /**
     * Pointer to the menu item
     * Own
     */
    CSwtMenuItem* iMenuItem;

    /**
     * Current Casing modifier for the characters
     */
    TInt iInitialCurrentCase;

    /**
     * Current inputMode for the widget
     */
    TInt iInitialCurrentInputMode;

    /**
     * Current selected laguage
     */
    TLanguage iInitialCurrentLanguage;

    /**
     * Content style for TextExtension
     */
    TInt iTypeStyle;

    /**
     * Pointer to the navi decorator
     * Own
     */
    CAknNavigationDecorator* iNaviDecorator;
};


inline CSwtTextExtension::CSwtTextExtension(MSwtDisplay& aDisplay, TSwtPeer aPeer,
        MSwtComposite& aParent, TInt aStyle)
        : CSwtText(aDisplay, aPeer, aParent, aStyle)
        , iInitialCurrentCase(0), iInitialCurrentInputMode(0)
        , iInitialCurrentLanguage(ELangTest), iTypeStyle(0)
{
}

#endif // SWTTEXTEXTENSION_H
