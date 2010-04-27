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


#ifndef SWTTASKTIP_H
#define SWTTASKTIP_H


#include "eswtmobileextensions.h"


/**
 * CSwtTaskTip
 * @lib eswt
 */
NONSHARABLE_CLASS(CSwtTaskTip)
        : public CBase
        , public MSwtTaskTip
        , public MSwtResourceChangeObserver
{
// Own methods
public:
    static CSwtTaskTip* NewL(MSwtDisplay& aDisplay, TSwtPeer aPeer, TInt aStyle);

// From MSwtTaskTip
public:
    TSwtPeer Dispose();
    TSwtPeer JavaPeer() const;
    TRect GetBarDefaultBounds(TBool aHasText) const;
    TRect GetLabelDefaultBounds(TBool aHasText) const;
    TRect GetShellDefaultBounds(TBool aHasText) const;

// From MSwtResourceChangeObserver
public:
    void OfferResourceChangeL(TInt aType);

// Own methods
private:
    CSwtTaskTip(MSwtDisplay& aDisplay, TSwtPeer aPeer, TInt aStyle);
    ~CSwtTaskTip();
    void ConstructL();
    void UpdateLayout();

// Data
private:
    MSwtDisplay& iDisplay;
    const TSwtPeer iPeer;
    TInt iStyle;
    TRect iRectShell;
    TRect iRectShellBarOrLabelOnly;
    TRect iRectLabel;
    TRect iRectLabelOnly;
    TRect iRectBar;
    TRect iRectBarOnly;
};

#endif // SWTTASKTIP_H
