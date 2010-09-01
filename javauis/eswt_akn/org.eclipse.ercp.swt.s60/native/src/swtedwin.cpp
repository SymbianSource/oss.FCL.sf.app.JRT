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


#include <swtlaffacade.h>
#include <eikedwin.h>
#include <AknNumEdwin.h>
#include <eikfpne.h>
#include "swtedwin.h"
#include "swtedwincustomdrawer.h"


// ==========================================================================
// CSwtEdwin MEMBER FUNCTIONS
// ==========================================================================


//---------------------------------------------------------------------------
// CSwtEdwin::CSwtEdwin
// ---------------------------------------------------------------------------
//
CSwtEdwin::CSwtEdwin()
        : CEikEdwin()
{
}

//---------------------------------------------------------------------------
// CSwtEdwin::SetClippingRect
// ---------------------------------------------------------------------------
//
void CSwtEdwin::SetClippingRect(const TRect& aRect)
{
    ASSERT(iClippingCustomDrawer);
    iClippingCustomDrawer->SetClippingRect(aRect);
}

//---------------------------------------------------------------------------
// CSwtEdwin::CreateCustomDrawL
// From CEikEdwin
// ---------------------------------------------------------------------------
//
CLafEdwinCustomDrawBase* CSwtEdwin::CreateCustomDrawL()
{
    // Custom draw object is deleted by CEikEdwin
    const MFormCustomDraw* edwinDrawer = CEikEdwin::CreateCustomDrawL();
    iClippingCustomDrawer = new(ELeave)CSwtEdwinCustomDrawer(
        iEikonEnv->LafEnv(), edwinDrawer, *this);
    return iClippingCustomDrawer;
}


// ==========================================================================
// CSwtIntegerEdwin MEMBER FUNCTIONS
// ==========================================================================


//---------------------------------------------------------------------------
// CSwtIntegerEdwin::CSwtIntegerEdwin
// ---------------------------------------------------------------------------
//
CSwtIntegerEdwin::CSwtIntegerEdwin()
{
}

//---------------------------------------------------------------------------
// CSwtIntegerEdwin::SetClippingRect
// ---------------------------------------------------------------------------
//
void CSwtIntegerEdwin::SetClippingRect(const TRect& aRect)
{
    ASSERT(iClippingCustomDrawer);
    iClippingCustomDrawer->SetClippingRect(aRect);
}

//---------------------------------------------------------------------------
// CSwtIntegerEdwin::CreateCustomDrawL
// From CEikEdwin
// ---------------------------------------------------------------------------
//
CLafEdwinCustomDrawBase* CSwtIntegerEdwin::CreateCustomDrawL()
{
    // Custom draw object is deleted by CEikEdwin
    const MFormCustomDraw* edwinDrawer = CEikEdwin::CreateCustomDrawL();
    iClippingCustomDrawer = new(ELeave)CSwtEdwinCustomDrawer(
        iEikonEnv->LafEnv(), edwinDrawer, *this);
    return iClippingCustomDrawer;
}

// ==========================================================================
// CSwtFloatingPointEdwin MEMBER FUNCTIONS
// ==========================================================================

//---------------------------------------------------------------------------
// CSwtFloatingPointEdwin::CSwtFloatingPointEdwin
// ---------------------------------------------------------------------------
//
CSwtFloatingPointEdwin::CSwtFloatingPointEdwin()
        : CEikFloatingPointEditor()
{
}

//---------------------------------------------------------------------------
// CSwtFloatingPointEdwin::SetClippingRect
// ---------------------------------------------------------------------------
//
void CSwtFloatingPointEdwin::SetClippingRect(const TRect& aRect)
{
    ASSERT(iClippingCustomDrawer);
    iClippingCustomDrawer->SetClippingRect(aRect);
}

//---------------------------------------------------------------------------
// CSwtFloatingPointEdwin::InputCapabilities
// From CEikEdwin
// ---------------------------------------------------------------------------
//
TCoeInputCapabilities CSwtFloatingPointEdwin::InputCapabilities() const
{
    if (IsReadOnly())
    {
        // Bypass CEikFloatingPointEditor
        return CEikEdwin::InputCapabilities();
    }
    else
    {
        return CEikFloatingPointEditor::InputCapabilities();
    }
}

//---------------------------------------------------------------------------
// CSwtFloatingPointEdwin::CreateCustomDrawL
// From CEikEdwin
// ---------------------------------------------------------------------------
//
CLafEdwinCustomDrawBase* CSwtFloatingPointEdwin::CreateCustomDrawL()
{
    // Custom draw object is deleted by CEikEdwin
    const MFormCustomDraw* edwinDrawer = CEikEdwin::CreateCustomDrawL();
    iClippingCustomDrawer = new(ELeave)CSwtEdwinCustomDrawer(
        iEikonEnv->LafEnv(), edwinDrawer, *this);
    return iClippingCustomDrawer;
}
