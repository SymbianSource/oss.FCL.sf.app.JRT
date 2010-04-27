/*******************************************************************************
 * Copyright (c) 2007, 2010 Nokia Corporation and/or its subsidiary(-ies).
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *
 * Contributors:
 *     Nokia Corporation - S60 implementation
 *******************************************************************************/


#ifndef SWTMULTIPAGEDIALOG_H
#define SWTMULTIPAGEDIALOG_H


#include <AknTabObserver.h>
#include "swtcontrolbase.h"
#include "swteditindicator.h"
#include "eswtmobileextensions.h"


class CAknTabGroup;
class CAknNavigationDecoratorContainer;


/**
 * CSwtMultiPageDialog
 * @lib eswt.dll
 */
NONSHARABLE_CLASS(CSwtMultiPageDialog)
        : public CCoeControl
        , public ASwtControlBase
        , public MSwtMultiPageDialog
        , public MAknTabObserver
{
public:
    /**
     * Destructor
     */
    ~CSwtMultiPageDialog();

    /**
     * First and second phase constructor wrapper.
     * @param aDisplay current display
     * @param aPeer java peer
     * @param aParent parent control
     * @return pointer to created control
     */
    static CSwtMultiPageDialog* NewL(MSwtDisplay& aDisplay,
                                     TSwtPeer aPeer, MSwtShell& aParent);


private:
    /**
     * Constructor.
     * @param aDisplay current display
     * @param aPeer java peer
     * @param aParent parent control
     */
    CSwtMultiPageDialog(MSwtDisplay& aDisplay,
                        TSwtPeer aPeer, MSwtShell& aParent);

    /**
     * Second phase constructor
     */
    void ConstructL();

private:
    /**
     * private class to hold the data for a single Tab.
     */
    class TTabItem
    {
    public:
        //Pointer to composite cannot be NULL
        MSwtComposite* iComposite;

        // id
        TInt iTabId;

        // flag if tab item has image on title
        TBool iHasTitle;
    };

private:
    // From CCoeControl
    void                SizeChanged();
    TTypeUid::Ptr       MopSupplyObject(TTypeUid aId);

private:
    // From MSwtControl
    TRect              ClientRect() const;
    CCoeControl&       CoeControl();
    const CCoeControl& CoeControl() const;
    TSize              ComputeSizeL(TInt aWHint, TInt aHHint);
    TInt               GetBorderWidth() const;
    void               ProcessKeyEventL(const TKeyEvent& aKeyEvent, TEventCode aType);

public:
    // From MSwtMultiPageDialog
    MSwtControl* Control();
    void Close();
    void CreatePageL(MSwtComposite* aComposite, const TDesC& aTitle, MSwtImage* aImage);
    void DeletePageL(TInt aIndex);
    TInt GetSelectedIndex();
    void OpenL();
    void SetSelection(TInt aIndex);
    void SetTitleAndImageL(TInt aIndex, const TDesC& aText, MSwtImage* aImage);
    void SetTitleL(TInt aIndex, const TDesC& aText);

public:
    // From MAknTabObserver
    void TabChangedL(TInt aIndex);

// New methods.
private:
    /**
     * Changes the visible tab.
     * @param aIndex zero relative index of the tab
     */
    void ShowTab(TInt aIndex);

    /**
     * Convenience method for supporting dynamic tab changes
     */
    void RefreshStatusPane() const;

    /**
     * Compute the tab style to use according to tab numbers and title content.
     * @return style
     */
    TInt ComputeTabStyle() const;

// Data
private:
    /**
     * Pointer to navigation control container.
     * Not Own
     */
    CAknNavigationControlContainer* iNaviCtrlCtnr;

    /**
     * Pointer to the navigation decorator.
     * Own
     */
    CAknNavigationDecorator* iNaviDecorator;

    /**
     * Pointer to the tabgroup
     */
    CAknTabGroup* iTabGroup;

    /**
     * The tab items array.
     */
    RArray<TTabItem> iTabItems;

    /**
     * ETrue if the dialog is open.
     */
    TBool iIsOpen;

    /**
     * Visible tab index.
     */
    TInt iVisibleTab;

    /**
     * Counter for tabs with a title
     */
    TInt iTitleCount;

    /**
     * Edit indicator redirection flag can not be NULL
     * Own
     */

    CSwtEditIndicatorRedictor* iRedirector;
};

#endif // SWTMULTIPAGEDIALOG_H
