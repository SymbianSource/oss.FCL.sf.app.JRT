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


#include "swtbrowsercontainer.h"
#include "swtbrowser.h"


// ======== MEMBER FUNCTIONS ========


CSwtBrowserContainer* CSwtBrowserContainer::NewL(CSwtBrowser& aApiProvider)
{
    CSwtBrowserContainer* container = new(ELeave) CSwtBrowserContainer(aApiProvider);
    CleanupStack::PushL(container);
    container->ConstructL(&aApiProvider);
    CleanupStack::Pop(container);
    return container;
}

CSwtBrowserContainer::CSwtBrowserContainer(CSwtBrowser& aApiProvider)
        : iApiProvider(aApiProvider)
{
}

CSwtBrowserContainer::~CSwtBrowserContainer()
{
}

void CSwtBrowserContainer::ConstructL(CCoeControl* aParent)
{
    CreateWindowL(aParent);

    // Required by the Browser control. WSERV:43 raised if not provided.
    Window().AllocPointerMoveBuffer(256, 0);
    Window().DisablePointerMoveBuffer();
}

void CSwtBrowserContainer::FocusChanged(TDrawNow aDrawNow)
{
    iApiProvider.BrCtlInterface().SetFocus(IsFocused());
    CCoeControl::FocusChanged(aDrawNow);
}

#ifdef RD_SCALABLE_UI_V2
void CSwtBrowserContainer::HandlePointerEventL(const TPointerEvent& aPointerEvent)
{
    // From CSwtBrowserContainer coordinates to parent Shell coordinates
    TPointerEvent event(aPointerEvent);
    event.iPosition += iApiProvider.Rect().iTl;

    // Forward to Shell which will forward to CSwtBrowser. If not done this way,
    // CSwtBrowser will not get focused meaning users being unable to use the websites!
    MSwtShell& shell = iApiProvider.GetShell();
    shell.Control()->CoeControl().HandlePointerEventL(event);
}
#endif

void CSwtBrowserContainer::SizeChanged()
{
    if (iApiProvider.StartedUp())
    {
        iApiProvider.BrCtlInterface().SetRect(Rect());
    }
}

TInt CSwtBrowserContainer::CountComponentControls() const
{
    TInt ctrls = 0;
    if (iApiProvider.StartedUp())
    {
        ctrls++;
    }
    return ctrls;
}

CCoeControl* CSwtBrowserContainer::ComponentControl(TInt /*aIndex*/) const
{
    CCoeControl *ctrl = NULL;
    if (iApiProvider.StartedUp())
    {
        ctrl = &(iApiProvider.BrCtlInterface());
    }
    return ctrl;
}
