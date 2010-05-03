/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  THIS FILE IS NOT INCLUDED INTO ECLIPSE CVS DELIVERY
*
*/


// Includes
#include "swtdccontrol.h"
//

//
#include "mmmadirectContent.h"
#include "swtdcobserver.h"

#include <swtcontrolbase.h>
#include <swtuiutils.h>
#include <methodwrappers.h>
#include <AknUtils.h>
#include <eiklbi.h>
#include <eikenv.h>
#include <jdebug.h>
#include <eikcolor.hrh>
// MUiEventConsumer interface
#include <reflcdui.h>

enum TDcEvent
{
    ERedraw,
    ESetFullScreen,
    EUpdateFullScreen
};

///////////////////////////////////////////////////////////////////////////////
CSwtDCControl::CSwtDCControl(MSwtDisplay& aDisplay,
                             TSwtPeer aPeer,
                             MSwtComposite* aParent)
        : CCoeControl()
        , ASwtControlBase(aDisplay, aPeer, aParent, 0 /*style*/)
#ifdef SWTDCCONTROL_DSA_ENABLED
        , iDSAccess(NULL)
        , iDsaWasStartedAlready(EFalse)
#endif
        , iDcObserver(NULL)
{
    SetFocusing(EFalse);
}

CSwtDCControl::~CSwtDCControl()
{
    DEBUG("CSwtDCControl::~CSwtDCControl()+");

    iDisplay.RemoveAppFocusObserver(this);

    // delete iClient
    DeleteClient();

    if (iContent)
    {
        iContent->MdcContainerDestroyed();
    }
#ifdef SWTDCCONTROL_DSA_ENABLED
    if (iDSAccess)
    {
        iDSAccess->Cancel();
        delete iDSAccess;
        iDSAccess = NULL;
    }
#endif

    if (iDcObserver)
    {
        delete iDcObserver;
        iDcObserver = NULL;
    }

    DEBUG("CSwtDCControl::~CSwtDCControl()-");
}

/**
 * @brief 2nd phase constructor
 */
void CSwtDCControl::ConstructL()
{
    DEBUG("CSwtDCControl::ConstructL()+");

    CCoeControl& coeParent = iParent->Control()->CoeControl();
    SetContainerWindowL(coeParent);

    TBool isVisible = coeParent.IsVisible();
    DEBUG_INT("CSwtDCControl::ConstructL parent visible %d", isVisible);
    CCoeControl::MakeVisible(isVisible);
    CCoeControl::SetDimmed(coeParent.IsDimmed());

    ActivateL();

    iDcObserver = new(ELeave) CSwtDcObserver();

#ifdef SWTDCCONTROL_DSA_ENABLED
    // direct screen access is needed to receive notification when visibility
    // is changed
    CWsScreenDevice* screenDevice = ControlEnv()->ScreenDevice();
    RWsSession& wsSession = ControlEnv()->WsSession();
    iDSAccess = CDirectScreenAccess::NewL(
                    wsSession,
                    *screenDevice,
                    *DrawableWindow(),
                    *this
                );
    TRAPD(err, iDSAccess->StartL());
    if (err != KErrNone)
    {
        iDSAccess->Cancel();
        delete iDSAccess;
        iDSAccess = NULL;
        DEBUG("CSwtDCControl::ConstructL dsa not started");
    }
    else
    {
        DEBUG("CSwtDCControl::ConstructL dsa started");
    }
#endif

    iDisplay.AddAppFocusObserverL(this);

    DEBUG("CSwtDCControl::ConstructL()-");
}

/**
 * @brief 1st & 2nd phase constructors wrapper
 */
CSwtDCControl* CSwtDCControl::NewL(MSwtDisplay& aDisplay,
                                   TSwtPeer aPeer,
                                   MSwtComposite* aParent,
                                   MMMADirectContent* aContent)
{
    CSwtDCControl* self = new(ELeave) CSwtDCControl(aDisplay,
            aPeer,
            aParent);
    CleanupStack::PushL(self);
    self->ConstructL();
    self->RegisterWithParentL();

    // content cannot be set before RegisterWithParentL because it assumes
    // size 0,0
    self->iContent = aContent;
    aContent->MdcSetContainer(self);
    self->SetSize(self->iContent->MdcSourceSize());
    CleanupStack::Pop(self);

    MSwtShell* shell = &(self->GetShell());
    TRect rWindowRect = shell->Control()->GetBounds();
    self->iContent->MdcContainerWindowRectChanged(rWindowRect);

    return self;
}

//
// Virtual methods from CCoeControl
//
void CSwtDCControl::Draw(const TRect& /*aRect*/) const
{
    // Draw only if there is content which has bitmap
    CFbsBitmap* bitmap = NULL;
    if (iContent)
    {
        bitmap = iContent->MdcFrameBuffer();
    } // else content is not set
    if (bitmap)
    {
        // The bitmap is compressed/stretched to fit the specified rectangle.
        if (iFullScreen)
        {
            TRect topRect = GetTopShellRect();

            CWindowGc& gc = SystemGc();
            gc.DrawBitmap(topRect, bitmap);
        }
        else
        {
            CWindowGc& gc = SystemGc();
            gc.DrawBitmap(Rect(), bitmap);
        }
    } // else content is using direct screen access
}

void CSwtDCControl::FocusChanged(TDrawNow aDrawNow)
{
    DEBUG("CSwtDCControl::FocusChanged");
    HandleFocusChanged(aDrawNow);
}

void CSwtDCControl::SetDimmed(TBool aDimmed)
{
    DEBUG("CSwtDCControl::SetDimmed");
    CCoeControl::SetDimmed(aDimmed);
}

void CSwtDCControl::HandleResourceChange(TInt aType)
{
    DEBUG("CSwtDCControl::HandleResourceChange()+");

    // Handle fading or content wont be updated properly when native
    // window pops out. Ignore the fading started by ancestor modal Shell.
    if (aType == KEikMessageUnfadeWindows ||
            aType == KEikMessageFadeAllWindows)
    {
        MSwtShell* fadingShell = UiUtils().FadingShell();
        if (!(fadingShell && this->IsDescentOf(*fadingShell->Control())))
        {
            iLastFadeMessage = aType;
        }
        else
        {
            iLastFadeMessage = 0;
        }
    }
    else if ((aType == KEikMessageWindowsFadeChange) &&
             ((iLastFadeMessage == KEikMessageUnfadeWindows) ||
              (iLastFadeMessage == KEikMessageFadeAllWindows)))
    {
        TBool isFaded = iLastFadeMessage == KEikMessageFadeAllWindows;

        DEBUG_INT("CSwtDCControl::HandleResourceChange(): isFaded=%d", isFaded);

        // Hide anytime, but show only when visibility conditions are fulfilled
        if (isFaded || IsContentVisibilityAllowed())

        {
            if (iContent)
            {
                iContent->MdcContainerVisibilityChanged(!isFaded);
            }
        }

        iLastFadeMessage = 0;
    }

    DEBUG("CSwtDCControl::HandleResourceChange()-");
}

void CSwtDCControl::MakeVisible(TBool aVisible)
{
    DEBUG_INT("CSwtDCControl::MakeVisible()+ %d", aVisible);

    CCoeControl::MakeVisible(aVisible);

    // Hide anytime, but show only when visibility conditions are fulfilled
    if (!aVisible || IsContentVisibilityAllowed())
    {
        if (iContent)
        {
            iContent->MdcContainerVisibilityChanged(aVisible);
        }
    }

    DEBUG("CSwtDCControl::MakeVisible()-");
}

void CSwtDCControl::PositionChanged()
{
    CCoeControl::PositionChanged();
    HandlePositionChanged();
    InformContentRect();
}

void CSwtDCControl::SizeChanged()
{
    CCoeControl::SizeChanged();

    DEBUG("CSwtDCControl::SizeChanged()+");

    TSize size(Size());
    DEBUG_INT2("CSwtDCControl::SizeChanged w = %d h = %d",
               size.iWidth,
               size.iHeight);
    HandleSizeChanged();
    InformContentRect();


    // When control is resized, it receives Restart() before its size is
    // changed. Therefore when sizing from larger to smaller, it can happen that
    // control doesn't fit in DSA drawing region and video is not resumed even when it
    // should be. Therefore we check here whether video can be resumed.
    // There is cross check wheter Restart() was successfully received at least once
    // to improve situation when midlet creates video during startup (video draws before
    // rest of the midlet's UI is drawn).
    if (
#ifdef SWTDCCONTROL_DSA_ENABLED
        iDsaWasStartedAlready &&
#endif
        IsContentVisibilityAllowed())
    {
        iContent->MdcContainerVisibilityChanged(ETrue);
    }


    DEBUG("CSwtDCControl::SizeChanged()-");
}

TTypeUid::Ptr CSwtDCControl::MopSupplyObject(TTypeUid aId)
{
    return ASwtControlBase::SwtMopSupplyObject(aId);
}

//
// Virtual methods from MSwtControl
//
CCoeControl& CSwtDCControl::CoeControl()
{
    return *this;
}


const CCoeControl& CSwtDCControl::CoeControl() const
{
    return *this;
}


void CSwtDCControl::ProcessKeyEventL(const TKeyEvent& /*aKeyEvent*/, TEventCode /*aType*/)
{
    // Do nothing
}

TRect CSwtDCControl::ClientRect() const
{
    return Rect();
}

TSize CSwtDCControl::ComputeSizeL(TInt aWHint, TInt aHHint)
{
    DEBUG_INT2("CSwtDCControl::ComputeSizeL w = %d h = %d", aWHint, aHHint);
    DEBUG_INT2("CSwtDCControl::ComputeSizeL rw = %d rh = %d", Rect().Size().iWidth, Rect().Size().iHeight);

    TSize size;
    if (iContent)
    {
        size = iContent->MdcSourceSize();
    }

    if (aWHint != KSwtDefault)
    {
        size.iWidth = aWHint;
    }
    if (aHHint != KSwtDefault)
    {
        size.iHeight = aHHint;
    }

    DEBUG_INT2("CSwtDCControl::ComputeSizeL retw = %d reth = %d", size.iWidth, size.iHeight);

    // return 0,0 if source size is not known, when size is known MdcInvalidate
    // will be called
    return size;
}

void CSwtDCControl::SetVisible(TBool aVisible)
{
    DEBUG_INT("CSwtDCControl::SetVisible()+ %d", aVisible);

    ASwtControlBase::SetVisible(aVisible);
    if (iContent)
    {
        iContent->MdcContainerVisibilityChanged(aVisible);
    }

    DEBUG("CSwtDCControl::SetVisible()-");
}

TSwtPeer CSwtDCControl::Dispose()
{
    DEBUG("CSwtDCControl::Dispose");
    // client is not needed after control is disposed
    DeleteClient();
    return ASwtControlBase::Dispose();
}

/**
 * From MSwtAppFocusObserver
 * If app has lost focus, then there is need to stop DSA
 * otherwise the parent control want be faded.
 * After gaining focus, we can start DSA again.
 */
void CSwtDCControl::HandleAppFocusChangeL(TBool aFocused)
{
#ifdef SWTDCCONTROL_DSA_ENABLED
    if (iDSAccess)
    {
        MSwtShell& parentShell = GetShell();
        MSwtShell& topShell = GetTopShell();
        MSwtControl* control = parentShell.Control();
        if (!aFocused)
        {
            AbortNow(RDirectScreenAccess::ETerminateCancel);
            iDSAccess->Cancel();
            // No need to redraw parent shell, if he is the top shell
            if (control && &parentShell != &topShell)
            {
                control->CoeControl().DrawDeferred();
            }
        }
        else if (!iDSAccess->IsActive())
        {
            Restart(RDirectScreenAccess::ETerminateCancel);
            // No need to redraw parent shell, if he is the top shell
            if (control && &parentShell != &topShell)
            {
                control->CoeControl().DrawDeferred();
            }
        }
    }
#else
   (void)aFocused; //Supresses compilation warning
#endif
}

TInt CSwtDCControl::GetBorderWidth() const
{
    // direct content has no borders
    return 0;
}

void CSwtDCControl::MdcRemoveContent()
{
    // This just causes that content's method will not be called
    iContent = NULL;
    delete iClient;
    iClient = NULL;
}

TBool CSwtDCControl::MdcContainerVisibility() const
{
    return IsVisible();
}

void CSwtDCControl::MdcRepaint() const
{
    DEBUG("CSwtDCControl::MdcRepaint()+");

    iDcObserver->InvokeDcEvent(*const_cast<CSwtDCControl*>(this),
                               ERedraw);

    DEBUG("CSwtDCControl::MdcRepaint()-");
}

void CSwtDCControl::MdcGetContentRect(TRect& aControlRect,
                                      TRect& aParentRect) const
{
    if (!iFullScreen)
    {
        aControlRect.SetRect(PositionRelativeToScreen(),
                             Size());

        CCoeControl& coeParent = iParent->Control()->CoeControl();

        aParentRect.SetRect(coeParent.PositionRelativeToScreen(),
                            coeParent.Size());

        // If the parent is bigger than the screen, video can get drawn outside
        // of application area (e.g. on top of CBA). That's why we need to clip
        // the parent rect to application area.
        aParentRect.Intersection(GetTopShellRect());
    }
    else
    {
        TRect topRect = GetTopShellRect();

        aControlRect = topRect;
        aParentRect = topRect;
    }
}

void CSwtDCControl::MdcInvalidate(const TSize& /*aPreferredSize*/)
{
    // Done on the Java side.
}

void CSwtDCControl::MdcSetDisplayFullScreen(TBool aFullScreen)
{
    if (iClient)
    {
        if (aFullScreen != iFullScreen)
        {
            iFullScreen = aFullScreen;
            iDcObserver->InvokeDcEvent(*this, ESetFullScreen);
        }
        else if (aFullScreen && iFullScreen)
        {
            iDcObserver->InvokeDcEvent(*this, EUpdateFullScreen);
        }
    }
}

void CSwtDCControl::MdcGetDSAResources(
    MUiEventConsumer& aConsumer,
    TBool aIsInUiThread)
{
    DEBUG_INT("CSwtDCControl::MdcGetDSAResources()+ eswt_thread=%d", aIsInUiThread);

    iDcObserver->InvokeDSAResourcesCallback(*this,
                                            aConsumer);
    DEBUG("CSwtDCControl::MdcGetDSAResources()-");

}

void CSwtDCControl::MdcGetUICallback(
    MUiEventConsumer& aConsumer,
    TInt aCallbackId)
{
    DEBUG_INT("CSwtDCControl::MdcGetUICallback()+ ID=%d", aCallbackId);

    iDcObserver->InvokeUICallback(aConsumer, aCallbackId);
    DEBUG("CSwtDCControl::MdcGetUICallback()-");
}

TRect CSwtDCControl::MdcContainerWindowRect() const
{
    return GetShell().Control()->GetBounds();
}

#ifdef SWTDCCONTROL_DSA_ENABLED
void CSwtDCControl::Restart(RDirectScreenAccess::TTerminationReasons /*aReason*/)
{
    // DSA needs to be started in order to get correct drawing region
    TRAPD(err, iDSAccess->StartL());

    DEBUG_INT("CSwtDCControl::Restart error = %d", err);

    // If DSA events cannot be received its better to hide content
    if (err != KErrNone && iContent)
    {
        iContent->MdcContainerVisibilityChanged(EFalse);
        return;
    }

    // If DSA was started correctly
    //   and control should be visible
    //   and DSA region is valid
    // make content visible.
    if (IsControlActive() && IsDsaRegionValid())
    {
        iContent->MdcContainerVisibilityChanged(ETrue);
        iDsaWasStartedAlready = ETrue;
    }
}

void CSwtDCControl::AbortNow(RDirectScreenAccess::TTerminationReasons /*aReason*/)
{
    if (iContent)
    {
        iContent->MdcContainerVisibilityChanged(EFalse);
    }
}
#endif

//
// Own methods
//

void CSwtDCControl::SetClient(MSwtClient* aClient, RHeap& aClientHeap)
{
    iClient = aClient;
    iClientHeap = &aClientHeap;
}

void CSwtDCControl::DeleteClient()
{
    DEBUG("CSwtDCControl::DeleteClient+");
    // switch heap if eswt has own heap
    if (iClient && (User::Heap().Base() != iClientHeap->Base()))
    {
        RHeap* oldHeap = User::SwitchHeap(iClientHeap);
        DEBUG_INT2("CSwtDCControl::DeleteClient() switch heap %d to %d",
                   (TInt)oldHeap->Base(), (TInt)iClientHeap->Base());
        delete iClient;
        User::SwitchHeap(oldHeap);
    }
    else
    {
        // no need to switch heap
        delete iClient;
    }
    iClient = NULL;

    DEBUG("CSwtDCControl::DeleteClient-");
}

void CSwtDCControl::InformContentRect()
{
    if (iContent)
    {
        TRect controlRect, parentRect;

        MdcGetContentRect(controlRect, parentRect);

        iContent->MdcContentRectChanged(controlRect, parentRect);
        iContent->MdcContainerWindowRectChanged(MdcContainerWindowRect());

        if (!iFullScreen && iParent)
        {
            // If the content changed size/location, the parent needs
            // to redraw to paint over the old content area.
            iParent->Control()->Redraw();
        }
    }
}

TRect CSwtDCControl::GetTopShellRect() const
{
    MSwtShell* topLevelShell = &GetShell();

    ASSERT(topLevelShell);

    MSwtShell* parentShell = topLevelShell->GetParentShell();

    while (parentShell != 0)
    {
        topLevelShell = parentShell;
        parentShell = topLevelShell->GetParentShell();
    }

    ASSERT(topLevelShell);

    TRect shellRect = TRect(topLevelShell->Control()->GetLocation(),
                            topLevelShell->Control()->GetBounds().Size());

    return shellRect;
}

void CSwtDCControl::SetFullScreenState()
{
    if (!iContent)
    {
        return;
    }

    MSwtShell* topShell = UiUtils().GetParentTopShellOfActiveShell();
    MSwtShell* shell = &GetShell();

    ASSERT(shell);

    if (!IsFrameBufferUsed() && topShell)
    {
        // Stop updating the parent shells in fullscreen mode
        // if the content is using DSA to avoid flickering.
        topShell->Control()->SetRedraw(!iFullScreen);

        // A fullscreen video that is not using DSA will fill
        // only the parent shell area. In such a case we don't
        // want to mess around with areas that won't get updated.
        topShell->SetFullScreen(iFullScreen);
    }

    shell->SetFullScreen(iFullScreen);

    if (iFullScreen)
    {
        // Save normal state size and position of the control
        iNormalPos = iPosition;
        iNormalSize = iSize;

        SetExtentToWholeScreen();
    }
    else
    {
        SetExtent(iNormalPos, iNormalSize);

        if (iParent)
        {
            iParent->Control()->Redraw();
        }
    }

    InformContentRect();
    Redraw();

    if (!IsFrameBufferUsed() && topShell)
    {
        // Now we can allow shell to update itself
        // Otherwise shell background will be drawn over the whole screen
        topShell->Control()->SetRedraw(iFullScreen);
    }
}

void CSwtDCControl::UpdateFullScreenState()
{
    SetExtentToWholeScreen();
    InformContentRect();
    Redraw();
}


void CSwtDCControl::DoDraw()
{
    Redraw();
}


/**
  * Returns true when control is active (app is focused and no menu is shown)
  */
TBool CSwtDCControl::IsControlActive() const
{
    // Get app focused state
    const TBool appFocused = (CCoeEnv::Static()->RootWin().Identifier() ==
                              ControlEnv()->WsSession().GetFocusWindowGroup());

    // Get menu state
    const TBool menuDisplayed = iDisplay.MenuArranger().EikMenuBar()->
                                IsDisplayed();

    DEBUG_INT2("CSwtDCControl::IsControlActive appFocused %d menuDisplayed %d",
               appFocused, menuDisplayed);
    DEBUG_INT("CSwtDCControl::IsControlActive isVisible %d",
              IsVisible());

    // Put content visible only if
    // application's window group is focused
    // menu is not shown
    return appFocused && !menuDisplayed &&
           iContent && IsVisible();
}

#ifdef SWTDCCONTROL_DSA_ENABLED
/**
 * Returns true when DSA region is large enough to draw the content.
 */
TBool CSwtDCControl::IsDsaRegionValid() const
{
    RRegion &dsaRegion = *iDSAccess->DrawingRegion();
    RRegion videoRegion;
    TRect controlRect;
    TRect parentRect;
    MdcGetContentRect(controlRect, parentRect);
    videoRegion.AddRect(controlRect);
    videoRegion.Intersect(dsaRegion);

    // When there is only one rectangle in intersection,
    // and it matches the content's rectangle
    videoRegion.Tidy();
    const TRect boundingRect = videoRegion.BoundingRect();
    const TBool isValidRegion = videoRegion.Count() == 1 &&
                                boundingRect == controlRect;

    videoRegion.Close();
    return isValidRegion;
}
#endif

/**
 * Returns true when content can be shown.
 */
TBool CSwtDCControl::IsContentVisibilityAllowed() const
{
    TBool ret = iContent && IsControlActive() &&
#ifdef SWTDCCONTROL_DSA_ENABLED
                (IsFrameBufferUsed() ||
                 (iDSAccess && IsDsaRegionValid() &&
                  GetShell().Control()->GetVisible())) &&
#endif

                IsVisible();

    DEBUG_INT("CSwtDCControl::IsContentVisibilityAllowed(): ret=%d", ret);

    return ret;
}


/**
 * If another drawing method (like DSA or NGA) is used by the content
 * the framebuffer of content is NULL.
 */
TBool CSwtDCControl::IsFrameBufferUsed() const
{
    return iContent && iContent->MdcFrameBuffer() != 0;
}

/**
 * Receives asynchronous events from iDcObserver
 */
void CSwtDCControl::HandleDcEvent(int aType)
{
    switch (aType)
    {
    case ERedraw:
        DoDraw();
        break;
    case ESetFullScreen:
        SetFullScreenState();
        break;
    case EUpdateFullScreen:
        UpdateFullScreenState();
        break;
    }
}

// End of file
