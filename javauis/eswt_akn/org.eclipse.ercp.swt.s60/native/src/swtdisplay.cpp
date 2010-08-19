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


#include <apgwgnam.h>
#include <swtlaffacade.h>
#include <akntitle.h>
#include "swtdisplay.h"
#include "eswtwidgetscore.h"
#include "methodwrappers.h"
#include "swtevents.h"
#include "swtkeymapper.h"
#include "swttimer.h"
#include "utils.h"
#include "swtdialogbroker.h"
#include "swtserver.h"
#include "swtmidremconobserver.h"
#include "swtuiutils.h"
#include "methodcall.h"
#include "swtfont.h"

// Default name displayed in the tasklist
_LIT(KAppDisplayName, "eSWT");

#define ASSERT_JAVAUITHREAD() ASSERT( IsCurrentThreadJavaUi() )
#define ASSERT_NATIVEUITHREAD() ASSERT( IsCurrentThreadNativeUi() )


// ======== MEMBER FUNCTIONS ========


// Java Ui Thread
CSwtDisplay* CSwtDisplay::NewL(JNIEnv& aJniEnv, jobject aPeer, TInt aDisplayParameter)
{
    jweak globalPeer = aJniEnv.NewWeakGlobalRef(aPeer);
    if (globalPeer == NULL)
        User::Leave(KErrNoMemory);

    CSwtDisplay* self = NULL;
    TRAPD(error, (self=DoNewL(aJniEnv, globalPeer, aDisplayParameter)));
    if (error)
    {
        aJniEnv.DeleteWeakGlobalRef(globalPeer);
        User::Leave(error);
    }

    return self;
}

// Java Ui Thread
void CSwtDisplay::Dispose(JNIEnv& aJniEnv)
{
    iDisposing = ETrue;

    if (iEventQueue)
        iEventQueue->PrepareForDestruction();

    TMethodWrapper0<CSwtDisplay> uiDestructor(*this, &CSwtDisplay::DestroyInNativeUiThread);
    ExecuteInNativeUiThread(uiDestructor);

    jweak globalPeer = static_cast<jweak>(iPeer);
    aJniEnv.DeleteWeakGlobalRef(globalPeer);

    delete iEventQueue;
    iEventQueue = NULL;

    iJavaUiThread.Close();
    iClient.Close();

    // No more Window Server events.
    java::ui::CoreUiAvkonEswt::getInstance().getJavaUiAppUi()->removeChild(this);

    // This will callback in CoreUiAvkonEswtSupportImpl::dispose,
    // the core ui will dissapear and native ui thread will exit.
    java::ui::CoreUiAvkonEswt::getInstance().dispose();
}

// Java Ui Thread
void CSwtDisplay::RequestRunDialogL(TSwtPeer aPeer, TInt aDialogType,
                                    TInt aStyle, const TDesC& aTitle, const TDesC& aText1, const TDesC& aText2,
                                    const TInt aInt1, const TInt aInt2, const TInt aInt3)
{
    TMethodWrapper0<CSwtDisplay> fp(*this, &CSwtDisplay::DialogAboutToOpen);
    ExecuteInNativeUiThread(fp);
    CSwtDialogBroker* dlgBroker(NULL);
    CallMethodL(dlgBroker, this, &CSwtDisplay::CreateDialogBrokerL);
    dlgBroker->SetExtraData(aInt1, aInt2, aInt3);
    CallMethodL(dlgBroker, &CSwtDialogBroker::RequestRunDialogL, aPeer, aDialogType,
                aStyle, aTitle, aText1, aText2);
}

// Native Ui Thread
void CSwtDisplay::OfferWsEventL(const TSwtWsEvent& aEvent, CCoeControl* aDestination /*= NULL*/)
{
    ASSERT_NATIVEUITHREAD();

    switch (aEvent)
    {
    case SwtWsEventEndKeyPressed:
    {
        // At this point the application is already sent to background by Avkon
        // - Send close event to display ( happening before anything has been disposed )
        // - Close event can be modified doIt=false
        // - If doIt==false then we don't do anything
        // - If doIt==true then we notify the runtime of exit
        if (!iCloseEventDispatchTimer)
        {
            iCloseEventDispatchTimer = CPeriodic::NewL(CActive::EPriorityStandard);
        }
        if (!iCloseEventDispatchTimer->IsActive())
        {
            iCloseEventDispatched = EFalse;
            PostCloseEventL(iPeer, iCloseEventDispatched);
            iCloseEventDispatchTimer->Start(5000000, 0, TCallBack(CloseEventDispatchTimerCallback, this));
        }
    }
    break;

    case SwtWsEventShutdown:
    {
        // MIDP's wrapper asks the runtime to exit the process. It is the MIDlet's
        // responsibilty to call Display.dispose inside destroyApp().
        // eRCP's wrapper does not exit the process and does not dispose the display.
        void* env = NULL;
        void* args = NULL;
        if (iVM && iVM->AttachCurrentThread(&env, args) >= 0)
        {
            JNIEnv* jniEnv = reinterpret_cast<JNIEnv*>(env);
            TBool failed;
            CallStaticVoidJavaMethod(failed,
                                     jniEnv,
                                     "org/eclipse/swt/internal/symbian/ExitNotificationWrapper",
                                     "notifyExit",
                                     "(I)V",
                                     iApplicationUid);
            iVM->DetachCurrentThread();
        }
    }
    break;

    case SwtWsEventAppFocusGained:
    case SwtWsEventAppFocusLost:
    {
        TBool focused = aEvent == SwtWsEventAppFocusGained;

        // UiUtils intentionally handled separately although it is an app focus observer also.
        if (iUiUtils)
        {
            iUiUtils->HandleAppFocusChangeL(focused);
        }

        // Inform other observers.
        const TInt count = iAppFocusObservers.Count();
        for (TInt i = 0; i < count; i++)
        {
            iAppFocusObservers[i]->HandleAppFocusChangeL(focused);
        }

        // Inform Java
        PostForegroundEventL(iPeer, focused);
    }
    break;

    case SwtWsEventWindowVisibilityChanged:
    {
        if (iMenuArranger)
        {
            iMenuArranger->HandleWindowVisibilityChangeL(aDestination);
        }
    }
    break;

    default:
        break;
    }

    if (aEvent == CSwtLafFacade::GetConstant(CSwtLafFacade::EBrowserFreeRam)
            ||  aEvent == CSwtLafFacade::GetConstant(CSwtLafFacade::EBrowserMemoryGood))
    {
        if (iUiUtils)
        {
            iUiUtils->HandleFreeRamEventL(aEvent);
        }
    }
}

// Native Ui Thread
void CSwtDisplay::RemoveDialogBroker(CSwtDialogBroker* aBroker)
{
    ASSERT(aBroker);
    for (TInt i = iDialogBrokers.Count()-1; i > -1; i--)
    {
        if (iDialogBrokers[i] == aBroker)
        {
            iDialogBrokers.Remove(i);
        }
    }
    delete aBroker;
    aBroker = NULL;
}

// From java::ui::CoreAppUiChild
// Native Ui Thread
void CSwtDisplay::HandleForegroundEventL(TBool /*aForeground*/)
{
}

// From java::ui::CoreAppUiChild
// Native Ui Thread
void CSwtDisplay::HandleSwitchOnEventL(CCoeControl* /*aDestination*/)
{
}

// From java::ui::CoreAppUiChild
// Native Ui Thread
void CSwtDisplay::HandleApplicationSpecificEventL(TInt /*aType*/, const TWsEvent& /*aEvent*/)
{
}

// From java::ui::CoreAppUiChild
// Native Ui Thread
void CSwtDisplay::HandleCommandL(TInt aCommand)
{
    ASSERT_NATIVEUITHREAD();

    // System request to exit the application ( e.g. close from task list or out-of-memory )
    if (aCommand == EEikCmdExit)
    {
        // Exiting is handled in HandleWsEventL. Let's not do anything here
        // to prevent duplicates.
    }

    if (aCommand == EAknCmdHelp)
    {
        MSwtCommandArranger* commandArranger = CommandArranger();
        if (commandArranger)
        {
            commandArranger->DoHelpOperationL();
        }
    }
}

// From java::ui::CoreAppUiChild
// Native Ui Thread
void CSwtDisplay::HandleResourceChangeL(TInt aType)
{
    if (iDisposing)
    {
        return;
    }

    TBool colorChange(EFalse);
    switch (aType)
    {
        // KAknsMessageSkinChange and KEikColorResourceChange are always sent in tandem.
        // ( See AknsAppSkinInstance.cpp ) Unfortunately we have to pass them both events
        // to the Avkon components as some do not react to them in the same matter.
        // eSWT components must pass both events to their Avkon contained or inherited
        // components and try to handle other jobs ( like font update ) only once.
    case KAknsMessageSkinChange:
    case KEikColorResourceChange:
        colorChange = ETrue;
        // Intentional fall through;
    case KEikDynamicLayoutVariantSwitch:
        if (iMenuArranger)
        {
            iMenuArranger->HandleResolutionChangeL();
        }
        // Intentional fall through;
    case KEikMessageFontChange:
        // Update system font
    {
        if (iSystemFont)
        {
            iSystemFont->RemoveRef();
            iSystemFont = NULL;
        }
        iSystemFont = CSwtFont::NewL(Device(), *AknLayoutUtils::FontFromId(
                                         CSwtLafFacade::GetFontId(CSwtLafFacade::EForm2MidpLabelPaneT1Font, 0)));
        // Send the Settings Event
        // KAknsMessageSkinChange and KEikColorResourceChange are always sent in tandem.
        if (aType != KEikColorResourceChange && iEventQueue)
        {
            iEventQueue->PushL(new(ELeave) CSwtSettingsEvent(iPeer, colorChange));
        }
    }
    break;
    // Not really needed to be handled here.
    case KEikMessageFadeAllWindows:
    case KEikMessageUnfadeWindows:
        break;
    default:
        ASSERT(ETrue);
        break;
    }

    if (iUiUtils)
    {
        iUiUtils->HandleResourceChangedL(aType);
    }

    // Forward HandleResourceChange to observers
    const TInt observerNb = iResourceChangeObservers.Count();
    for (TInt observerIndex=0; observerIndex<observerNb; ++observerIndex)
    {
        if (iResourceChangeObservers[observerIndex])
        {
            iResourceChangeObservers[observerIndex]->OfferResourceChangeL(aType);
        }
    }
}

// From java::ui::CoreAppUiChild
// Native Ui Thread
TBool CSwtDisplay::HandleWsEventL(const TWsEvent& aEvent, CCoeControl* aDestination)
{
    TSwtWsEvent swtEventType = SwtWsEventUnknown;
    TInt eventType = aEvent.Type();

    switch (eventType)
    {
    case KAknUidValueEndKeyCloseEvent:
        // Let CSwtDisplay::OfferWsEventL handle end ( red ) key.
        // CAknAppUi::HandleWsEventL should not handle it.
        swtEventType = SwtWsEventEndKeyPressed;
        break;

    case EEventUser:
        if ((*reinterpret_cast<TApaSystemEvent*>(aEvent.EventData())) == EApaSystemEventShutdown)
        {
            // Oom or exit from task-list:
            // - Send close event to display ( happening before anything has been disposed )
            // - Close event's doIt is ignored
            // - After calling the close event listeners then we notify the runtime of exit
            if (!iCoeEnv->IsSystem())
            {
                swtEventType = SwtWsEventShutdown;
            }
        }
        break;

#ifdef RD_SCALABLE_UI_V2
        // In versions higher than 3.23 different event is sent, when closing
        // midlet using the C-key.
    case KAknShutOrHideApp:
        swtEventType = SwtWsEventShutdown;
        break;
#endif //RD_SCALABLE_UI_V2

    case EEventFocusGained:
        swtEventType = SwtWsEventAppFocusGained;
        break;

    case EEventFocusLost:
        swtEventType = SwtWsEventAppFocusLost;
        break;

    case EEventWindowVisibilityChanged:
        swtEventType = SwtWsEventWindowVisibilityChanged;
        break;

    default:
        break;
    }

    if (swtEventType != SwtWsEventUnknown)
    {
        OfferWsEventL(swtEventType, aDestination);
    }

    // Returning ETrue here means that CAknAppUi will not handle the event.
    return (eventType == KAknUidValueEndKeyCloseEvent);
}

// From MSwtDisplay
// Any thread
#ifdef _DEBUG
TBool CSwtDisplay::IsCurrentThreadNativeUi() const
{
    return (RThread().Id() == iNativeUiThread.Id());
}
#endif

// From MSwtDisplay
// Any thread
TSwtPeer CSwtDisplay::JavaPeer()
{
    return iPeer;
}

// From MSwtDisplay
// Native UI Thread
void CSwtDisplay::SetNameInTaskListL(const TDesC* aNewName)
{
    // This code seems to have no effect on the task list.
    CApaWindowGroupName* wgName = java::ui::CoreUiAvkonEswt::getInstance().getWindowGroupName();
    if (wgName)
    {
        if (aNewName)
        {
            wgName->SetCaptionL(*aNewName);
        }
        else
        {
            wgName->SetCaptionL(KAppDisplayName);
        }
        // Refresh displayed name in tasklist
        wgName->SetWindowGroupName(iCoeEnv->RootWin());
    }
}

// From MSwtDisplay
// Native UI Thread
// For eRCP
void CSwtDisplay::SetUIDInTaskList(TInt aNewUID)
{
    CApaWindowGroupName* wgName = java::ui::CoreUiAvkonEswt::getInstance().getWindowGroupName();
    if (wgName)
    {
        wgName->SetAppUid((TUid::Uid(aNewUID)));
        // Refresh displayed name in tasklist
        wgName->SetWindowGroupName(iCoeEnv->RootWin());
    }
}

// From MSwtDisplay
// Native UI Thread
// For eRCP
void CSwtDisplay::SetAppVisible(TBool aVisible)
{
    CApaWindowGroupName* wgName = java::ui::CoreUiAvkonEswt::getInstance().getWindowGroupName();
    if (wgName)
    {
        wgName->SetHidden(!aVisible);
        // Refresh displayed name in tasklist
        wgName->SetWindowGroupName(iCoeEnv->RootWin());
    }
}

// From MSwtDisplay
// Native UI Thread
void CSwtDisplay::SetUiReady(TBool aFullScreenUi)
{
    if (!iUiReady)
    {
        iUiReady = ETrue;
        java::ui::CoreUiAvkonAppUi* appUi = java::ui::CoreUiAvkonEswt::getInstance().getJavaUiAppUi();
        if (appUi)
        {
            if (!appUi->isStartupCancelled())
            {
                // Bring app to foreground
                RWindowGroup& group = iCoeEnv->RootWin();
                group.SetOrdinalPosition(0);
            }
            appUi->stopStartScreen(aFullScreenUi);
        }
    }
}

// From MSwtDisplay
// Native UI Thread
TBool CSwtDisplay::IsUiReady() const
{
    return iUiReady;
}

// Java Ui Thread
CSwtDisplay* CSwtDisplay::DoNewL(JNIEnv& aJniEnv, TSwtPeer aPeer, TInt aApplicationUid)
{
    CSwtDisplay* self = new(ELeave) CSwtDisplay(aPeer);
    CleanupStack::PushL(self);
    self->iApplicationUid = aApplicationUid;
    self->ConstructInJavaUiThreadL(aJniEnv);
    CleanupStack::Pop(self);
    return self;
}

// Java Ui Thread
void CSwtDisplay::ConstructInJavaUiThreadL(JNIEnv& aJniEnv)
{
    iDisposing = EFalse;

    // Step 1 : Get CAknAppUi
    java::ui::CoreUiAvkonEswt& ui = java::ui::CoreUiAvkonEswt::getInstance();
    ui.ensureInitialized(TUid::Uid(iApplicationUid));
    User::LeaveIfError(iJavaUiThread.Open(RThread().Id()));
#ifdef _DEBUG
    iNativeUiThread = ui.getEswtSupport().thread();
#endif //_DEBUG
    ui.getEswtSupport().setDisplay(this);

    // Step 2: Construct display base in java ui thread
    ASwtDisplayBase::ConstructInJavaUiThreadL();

    // Step 3: Open a session to the function server
    User::LeaveIfError(iClient.Connect());

    // Step 4: Construct in native ui thread
    TMethodWrapper0<CSwtDisplay> uiConstructor(*this, &CSwtDisplay::ConstructInNativeUiThreadL);
    TInt error = ExecuteInNativeUiThread(uiConstructor);
    if (error)   // Cannot use LeaveIfError(), it wouldn't leave on errors >0
        User::Leave(error);

    // Step 5: Store the Jni
    User::LeaveIfError(aJniEnv.GetJavaVM(&iVM));

    // Step 6: Set CoreUi child. Ready to receive Window Server events.
    //         CoreUI takes the ownership of the CSwtDisplay object.
    ui.getJavaUiAppUi()->setEswtChild(this);
}

// Native Ui Thread
void CSwtDisplay::DoExecuteInNativeUiThreadL(const MSwtFunctor* aFunctor)
{
    (*aFunctor)();   // It doesn't look like it but this may leave
}

// Native Ui Thread
TInt CSwtDisplay::CloseEventDispatchTimerCallback(TAny* aThis)
{
    CSwtDisplay* self = static_cast< CSwtDisplay* >(aThis);
    ASSERT(self);

    TRAPD(err, self->HandleCloseEventDispatchTimerCallbackL());
    return err;
}

// Native Ui Thread
CSwtDisplay::~CSwtDisplay()
{
    // Cannot have anything here. Use Dispose().
    // The object will be deleted from CoreUi.
}

// Native Ui Thread
void CSwtDisplay::ConstructInNativeUiThreadL()
{
    ASwtDisplayBase::ConstructInNativeUiThreadL();

    if (java::ui::CoreUiAvkonEswt::getInstance().getJavaUiAppUi()->isForeground())
    {
        iUiUtils->HandleAppFocusChangeL(ETrue);
    }
    else
    {
        iUiUtils->HandleAppFocusChangeL(EFalse);
    }

    // iAppFocusObservers empty at this point
}

// Native Ui Thread
void CSwtDisplay::DestroyInNativeUiThread()
{
    ASSERT_NATIVEUITHREAD();

    ASwtDisplayBase::DestroyInNativeUiThread();

    if (iCloseEventDispatchTimer)
    {
        if (iCloseEventDispatchTimer->IsActive())
        {
            iCloseEventDispatchTimer->Cancel();
        }
        delete iCloseEventDispatchTimer;
        iCloseEventDispatchTimer = NULL;
    }

    CSwtDialogBroker* dlgBroker(NULL);
    for (TInt i = iDialogBrokers.Count() - 1; i > -1; i--)
    {
        dlgBroker = iDialogBrokers[ i ];
        iDialogBrokers.Remove(i);
        delete dlgBroker;
    }
    iDialogBrokers.Close();
}

// Native Ui Thread
CSwtDialogBroker* CSwtDisplay::CreateDialogBrokerL()
{
    CSwtDialogBroker* dlgBroker =  new(ELeave) CSwtDialogBroker(this, CSwtDialogBroker::ENoRequest);
    iDialogBrokers.AppendL(dlgBroker);
    return dlgBroker;
}

// Native Ui Thread
void CSwtDisplay::HandleCloseEventDispatchTimerCallbackL()
{
    iCloseEventDispatchTimer->Cancel();
    if (iCloseEventDispatched)
    {
        return;
    }

    // The event did not dispatch in 5 seconds. Destroy the UI.
    OfferWsEventL(SwtWsEventShutdown);
}

// Native Ui Thread
void CSwtDisplay::DialogAboutToOpen()
{
    ASSERT_NATIVEUITHREAD();
    if (!IsUiReady())
    {
        SetUiReady(EFalse);
    }
}



