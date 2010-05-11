/*
* Copyright (c) 2003-2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Implementation class for TextBox
*
*/


#ifndef CMIDTEXTBOXEDWIN_H
#define CMIDTEXTBOXEDWIN_H

// ========== INCLUDE FILES ================================
#include <fepbase.h>    // for MCoeCaptionRetrieverForFep
#include "CMIDEdwin.h"

// ========== FORWARD DECLARATIONS =========================

class CMIDTextBoxEdwinCustomDraw;

// ========== CLASS DECLARATION ============================

NONSHARABLE_CLASS(CMIDTextBoxEdwin) : public CMIDEdwin, public MCoeCaptionRetrieverForFep
{
public:
    static CMIDTextBoxEdwin* NewL(TInt aConstraints,const TDesC& aText,TInt aMaxSize,
    MMIDDisplayable* aDisplayable);
    ~CMIDTextBoxEdwin();

    // From CEikEdwin
    /**
     * Creates a new @c CLafEdwinCustomDrawBase instance.
     *
     * @return Pointer to new custom @c CLafEdwinCustomDrawBase object.
     */
    virtual CLafEdwinCustomDrawBase* CreateCustomDrawL();

#ifdef RD_SCALABLE_UI_V2
    void HandlePointerEventL(const TPointerEvent &aPointerEvent);
#endif // RD_SCALABLE_UI_V2

    // From CCoeControl
    TCoeInputCapabilities InputCapabilities() const;

    // From MCoeCaptionRetrieverForFep
    void GetCaptionForFep(TDes& aCaption) const;

protected: // from CMIDEdwin
    /**
     * Layouts the CMIDTextBoxEdwin relative to the parent.
     * @note The margins are added by CMIDTextBoxControl::SizeChanged()
     * @see CMIDTextBoxControl::SizeChanged()
     * <p>
     * @param aRect the parent rect
     * @param aLayout multi line layout info
     */
    virtual void DoLayout(const TRect& aRect);
    void AddCommandToEdwinL(MMIDCommand& aCommand);

private: // Constructors
    /**
     * Constructor
     */
    CMIDTextBoxEdwin(TUint aConstraints, MMIDDisplayable* aDisplayable);

    /**
     * Second phase constructor
     * @param aText a text to be set in the field
     * @param aMaxSize maximum number of characters to allow
     */
    void ConstructL(const TDesC& aText,TInt aMaxSize);

    /**
     * Get the layout for the Edwin
     *
     * @return multi line layout structure
     */
    TAknMultiLineTextLayout GetLayout() const;

public:

    /**
     * Returning pointer to owned CMIDTextBoxEdwinCustomDraw
     *
     * @return Pointer to iCustomDraw.
     */
    CMIDTextBoxEdwinCustomDraw* GetCustomDraw() const;

private: // data
    // reference on the latest instance created, EikEdwin owns the instance
    CMIDTextBoxEdwinCustomDraw* iCustomDraw;
};

#endif // CMIDTEXTBOXEDWIN_H
