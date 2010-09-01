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


#ifndef CMIDSPACER_H
#define CMIDSPACER_H

#include <lcdui.h>
// CMIDControlItem inheritance in class declaration
#include "CMIDControlItem.h"


NONSHARABLE_CLASS(CMIDSpacer) : public CMIDControlItem, public MMIDSpacer
{
public:

    /**
     * Double ctor
     * @param aMinimumSize Min size
     * @param aUIManager UI manager
     * @return CMIDSpacer instance
     *
     * @since s60
     */
    static CMIDSpacer* NewL(const TSize& aMinimumSize, CMIDUIManager* aUIManager);

    //
    // From MMIDSpacer
    //
    void SetMinimumSizeL(const TSize& aSize);
    //
    // From MMIDItem
    //
    void SetLabelL(const TDesC& aLabel);
    void SetLayoutL(TLayout aLayout);
    void SetPreferredSizeL(const TSize& aSize);
    TSize PreferredSize() const;
    TSize MinimumSize() const;
    void AddCommandL(MMIDCommand* aCommand);
    void RemoveCommand(MMIDCommand* aCommand);
    void SetDefaultCommand(MMIDCommand* aCommand);
    TBool IsSelectable() const;
    //
    // From MMIDComponent
    //
    void Dispose();
    //
    // From CCoeControl
    //
    TSize MinimumSize();
    virtual void Draw(const TRect& /*aRect*/) const;
    //
    // From CMIDItem
    //
    TInt ItemPreferredHeightWithoutLabel();
    void SetUsedForStringFormatting(TBool iVal);
    TBool UsedForStringFormatting();
public:
    /**
     * Ctor
     * @param aUIManager UI manager
     *
     * @since s60
     */
    CMIDSpacer(CMIDUIManager* aUIManager);

    void ConstructL(const TSize& aMinimumSize);
private:
    TSize iMinimumSize;

    // if this is true, this spacer is just used for string formatting
    // and should be deleted when reformatting the form
    TBool iUsedForStringFormatting;
};

#endif // CMIDSPACER_H
