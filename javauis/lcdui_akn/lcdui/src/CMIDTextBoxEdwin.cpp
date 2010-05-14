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


#include <lafmain.h>
// LAF API use in GetLayout function
#include <aknlayoutscalable_avkon.cdl.h>

#include "CMIDEdwin.h"
#include "CMIDTextBoxEdwin.h"
// API for iCustomDraw
#include "CMIDTextBoxEdwinCustomDraw.h"
#include "CMIDDisplayable.h"

// class CMIDTextBoxEdwin

CMIDTextBoxEdwin* CMIDTextBoxEdwin::NewL(TInt aConstraints,
        const TDesC& aText,
        TInt aMaxSize,
        MMIDDisplayable* aDisplayable)
{
    CMIDTextBoxEdwin* textBox = new(ELeave)CMIDTextBoxEdwin(aConstraints, aDisplayable);
    CleanupStack::PushL(textBox);
    textBox->ConstructL(aText,aMaxSize);
    CleanupStack::Pop(textBox);
    return textBox;
}

void CMIDTextBoxEdwin::ConstructL(const TDesC& aText,TInt aMaxSize)
{
    // just call superclass, for now
    CMIDEdwin::ConstructL(aText, aMaxSize);
}

// ----------------------------------------------------------------------------
// CMIDTextBoxEdwin::GetLayout()
// ----------------------------------------------------------------------------
TAknMultiLineTextLayout CMIDTextBoxEdwin::GetLayout() const
{
    TAknMultiLineTextLayout         edwinLayout;
    TAknTextComponentLayout         textLayout;
    RArray<TAknTextComponentLayout> edwinLayoutLines;

    // TextBox case
    TAknWindowComponentLayout   rowPaneLayout;  // row pane
    TAknTextComponentLayout     textRowLayout;  // text within a row pane

    textLayout = AknLayoutScalable_Avkon::list_single_midp_pane_t1();

    // get the number of layout lines for panel rows
    TAknLayoutScalableParameterLimits limits =
        AknLayoutScalable_Avkon::list_single_midp_pane_ParamLimits();
    TInt linesCount = limits.LastRow() + 1;

    for (TInt i = 0; i < linesCount; i++)
    {
        // translate one row of text layout inside a row pane
        textRowLayout = TAknWindowComponentLayout::ComposeText(
                            AknLayoutScalable_Avkon::list_single_midp_pane(i),
                            textLayout);

        TRAP_IGNORE(edwinLayoutLines.AppendL(textRowLayout));
    }

    edwinLayout = TAknTextComponentLayout::Multiline(edwinLayoutLines);
    edwinLayoutLines.Reset();

    return edwinLayout;
}


CMIDTextBoxEdwin::CMIDTextBoxEdwin(TUint aConstraints, MMIDDisplayable* aDisplayable)
        : CMIDEdwin(aConstraints, aDisplayable, NULL)
{
}

CMIDTextBoxEdwin::~CMIDTextBoxEdwin()
{
    // just calls superclass destructor
}

CLafEdwinCustomDrawBase* CMIDTextBoxEdwin::CreateCustomDrawL()
{
    // set custom drawing to the editor
    const MFormCustomDraw* customDraw = CEikEdwin::CreateCustomDrawL(); // we will take ownership of the original custom draw
    iCustomDraw = CMIDTextBoxEdwinCustomDraw::NewL(iEikonEnv->LafEnv(), customDraw, this);
    return iCustomDraw;
}

void CMIDTextBoxEdwin::DoLayout(const TRect& aRect)
{
    TAknMultiLineTextLayout layout;

    /** The skin text color overrides the text color in LAF. */
    TAknsQsnTextColorsIndex skinTextColor;

    if (iConstraints & MMIDTextField::EUneditable)
    { // skinning colour groups for uneditable text
        skinTextColor = EAknsCIQsnTextColorsCG52;
    }
    else
    { // skinning colour groups for editable text
        skinTextColor = EAknsCIQsnTextColorsCG6;
    }

    // get layout
    layout = GetLayout();

    // layout the control
    AknLayoutUtils::LayoutEdwin(this, aRect, layout, skinTextColor);
    // layout the custom draw
    if (iCustomDraw != NULL) // the custom draw is created by us, so if it is defined we have the right version
    {
        iCustomDraw->CalculateLayout(aRect);
    }
}


#ifdef RD_SCALABLE_UI_V2
void CMIDTextBoxEdwin::HandlePointerEventL(const TPointerEvent &aPointerEvent)
{
    if (AknLayoutUtils::PenEnabled())
    {
        TBool consumed = STATIC_CAST(CMIDDisplayable*, iDisplayable)->TryDetectLongTapL(aPointerEvent);
        if (!consumed)
        {
            CEikEdwin::HandlePointerEventL(aPointerEvent);
        }
    }

}
#endif // RD_SCALABLE_UI_V2


TCoeInputCapabilities CMIDTextBoxEdwin::InputCapabilities() const
{
    TCoeInputCapabilities inputCapabilities(TCoeInputCapabilities::ENone, NULL,
                                            const_cast<CMIDTextBoxEdwin*>(this));
    inputCapabilities.MergeWith(CMIDEdwin::InputCapabilities());
    return inputCapabilities;
}


void CMIDTextBoxEdwin::GetCaptionForFep(TDes& aCaption) const
{
    ASSERT(iDisplayable);
    HBufC* title = static_cast<CMIDDisplayable*>(iDisplayable)->Title();

    if (title)
    {
        const TInt maximumLength = aCaption.MaxLength();
        if (title->Length() > maximumLength)
        {
            aCaption = title->Left(maximumLength);
        }
        else
        {
            aCaption = *title;
        }
    }
    else
    {
        aCaption = KNullDesC;
    }
}


void CMIDTextBoxEdwin::AddCommandToEdwinL(MMIDCommand& aCommand)
{
    STATIC_CAST(CMIDDisplayable*, iDisplayable)->AddCommandL(&aCommand);
}

// ---------------------------------------------------------
// CMIDTextBoxEdwin::GetCustomDraw()
// ---------------------------------------------------------
//
CMIDTextBoxEdwinCustomDraw* CMIDTextBoxEdwin::GetCustomDraw() const
{
    return iCustomDraw;
}

