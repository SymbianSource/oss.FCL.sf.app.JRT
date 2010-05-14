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
    EnableDragEvents();
#ifdef RD_JAVA_S60_RELEASE_9_2
#ifndef __WINSCW__
    Window().EnableAdvancedPointers();
#endif
#endif // RD_JAVA_S60_RELEASE_9_2
    ActivateL();
}

void CSwtBrowserContainer::FocusChanged(TDrawNow aDrawNow)
{
    iApiProvider.BrCtlInterface().SetFocus(IsFocused());
    CCoeControl::FocusChanged(aDrawNow);
}

#ifdef RD_SCALABLE_UI_V2
void CSwtBrowserContainer::HandlePointerEventL(const TPointerEvent& aPointerEvent)
{
    // Capturing the pointer improves considerably the frequency of drag events
    // and the usability of the pinch zooming. It also shortens the route of
    // the event back to the browser.
    MSwtUiUtils& utils = iApiProvider.Display().UiUtils();
    MSwtControl* prevCapturingCtrl = utils.PointerCaptureControl();
    utils.SetPointerCaptureControl(&iApiProvider);

    MSwtControl* shell = iApiProvider.GetShell().Control();

#ifdef RD_JAVA_S60_RELEASE_9_2
    if (aPointerEvent.AdvancedPointerEvent())
    {
        TAdvancedPointerEvent event = *(static_cast<const TAdvancedPointerEvent *>(&aPointerEvent));
        event.iPosition += iApiProvider.Rect().iTl;
        shell->HandlePointerEventL(event);
    }
    else
#endif
    {
        TPointerEvent event = *(static_cast<const TPointerEvent *>(&aPointerEvent));
        event.iPosition += iApiProvider.Rect().iTl;
        shell->HandlePointerEventL(event);
    }

    utils.SetPointerCaptureControl(prevCapturingCtrl);
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
