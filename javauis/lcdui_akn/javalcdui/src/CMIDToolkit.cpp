/*
* Copyright (c) 1999 - 2003 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:
*
*/


#include <coecntrl.h>
#include <coeccntx.h>
#include <coecobs.h>
#include <apgwgnam.h>
#include <coemain.h>
#include <eikenv.h>
#include <eikappui.h>
#include <apgtask.h>
#include <bautils.h>
#include <s32stor.h>
#include <s32file.h>
#include <e32property.h>
#include <centralrepository.h>
#include <settingsinternalcrkeys.h>
#include <ScreensaverInternalPSKeys.h>      // to work with screensaver
#include <gfxtranseffect/gfxtranseffect.h>  // For transition effects
#include <akntranseffect.h>                                 // For transition effects
//
#include "lcdui.h"
#include "lcdgr.h"
#include "mevents.h"
#include "jutils.h"
#include "MIDUtils.h"
#include "CJavaEventServer.h"
#include "CMIDToolkit.h"
#include "CMIDEvent.h"
#include "CMIDBuffer.h"
#include "CMIDNotify.h"
#include <stdio.h>
#include "LcduiThread.h"

#include "coreuiavkonlcdui.h"
#include "coreuiappui.h"

#include "MMIDCanvasGraphicsItemPainter.h"

#include <jdebug.h>
#ifdef LCDUI_DEBUG_ON
#define LCDUI_DEBUG(msg) DEBUG(msg)
#define LCDUI_DEBUG_INT(msg, x) DEBUG_INT(msg, x)
#else
#define LCDUI_DEBUG(msg)
#define LCDUI_DEBUG_INT(msg, x)
#endif

/**
 * Macro controlling synthesis of AppArc foreground event when the
 * MIdlet requests that the Display be brought to the foreground.
 *
 * Some UI's may require this event, others may not.
 *
 * Generation of this event should be moved to the plugin.
 */
#define GENERATE_APPARC_FOREGROUND_EVENT 1

/**
 * Macro controlling whether the tasklist entry is cleared early in
 * CMIDToolkit::Close() or left in place until the window group is
 * destroyed (when the eikonenv is destroyed).
 *
 */
#define CLEAR_TASKLIST_IN_CLOSE 1

/**
 * Macro extracting the weak reference corresponding to a component.
 */
#define MIDJOBJECT(component)  ( reinterpret_cast<jobject>((component)->iPeer) )

/**
 * Graphics plugin library name.
 */
_LIT(KGrLibName,"lcdgr");

/**
 * Window group ordinal position (z-order) for sending to background.
 */
const TInt KWgOrdinalBackground = -1;

/**
 * Window group ordinal position (z-order) for bringing to foreground.
 */
const TInt KWgOrdinalForeground = 0;


/**
 * This timeout (microseconds) defines how long we prevent MIDlet from becoming
 * back to foregound after user has switched it to background.
 * Some MIDlets change the current displayable in hideNotify(), which (without this prevention)
 * would cause them to become back to foreground immediately.
 */
const TInt KChangingToBackgroundTimeout = 5000000;


/**
 * LCDGR graphics plugin factory function.
 */
typedef MMIDGraphicsFactory*(*TCreateGraphicsFactoryFunc)(RFs&, TDisplayMode);


/**
 * Exception safe reversion to previous displayable if CMIDToolkit::SetCurrentL fails.
 *
 */
class TRevertCurrent
{
public:
    TRevertCurrent(MMIDDisplayable*& aCurrentPointer)
            : iCurrentPointer(aCurrentPointer)
            , iPrevious(aCurrentPointer)
    {
    }
    static void Revert(TAny*);

private:
    MMIDDisplayable*& iCurrentPointer;
    MMIDDisplayable*  iPrevious;
};

/**
 * process entries on finalize queue.
 */
class CMIDFinalizeEvent : public CJavaEvent<CMIDToolkit>
{
private:
    void Dispatch(JNIEnv& aJni);
};


const static TInt KPayload = 16; // Number of weak references on queue before dispose event

/**
 * Register an MMIDComponent and its peer with the Toolkit.
 *
 *@return an object handle.
 */
TInt CMIDToolkit::RegisterComponentL(MMIDComponent* aComponent, TJavaPeer aPeer)
{
    ASSERT(aComponent);
    TObjectEntry* entry = new(ELeave) TObjectEntry(aComponent);
    CleanupStack::PushL(entry);
    User::LeaveIfError(iObjects.Append(entry));
    CleanupStack::Pop(entry);
    aComponent->iPeer = aPeer;
    return MIDHandle(aComponent);
}

/**
 * CONSTRUCTION PHASE 1 . Java side.
 */
CMIDToolkit::CMIDToolkit()
        : iPhase(EPhase1)
        , iOldFullScreenDisplayable(NULL), iObjects(EGranularity), iSentToBgTime(0), mFirst(ETrue)
{
    LCDUI_DEBUG_INT("CMIDToolkit::CMIDToolkit(%d) CONSTRUCTION PHASE 1", (TInt)this);
    iFinalizeMutex.CreateLocal();
}

/**
 * DESTRUCTION PHASE 1 . Java side
 */
CMIDToolkit::~CMIDToolkit()
{
    LCDUI_DEBUG_INT("CMIDToolkit::~CMIDToolkit(%d): DESTRUCTION PHASE 1 <BEGIN>", (TInt)this);
    ASSERT(iPhase == EPhase1);

    iFinalizeMutex.Close();
    iObjects.Close();

    //
    // We cannot delete any server side objects in the
    // destructor since it runs Java side. Therefore if
    // any of the following invariants fail, we have
    // leaked or failed to zero the pointer.
    //

    ASSERT(0    == iObjects.Count());
    ASSERT(NULL == iDisposedQueue);
    ASSERT(0    == iDisposedCount);
    ASSERT(NULL == iFinalizeQueue);
    ASSERT(NULL == iFinalizeCount);

    // Not owned but worth checking we clean everything.
    ASSERT(NULL == iCoeEnv);

    // Owned - if these are non-null we leaked.
    ASSERT(NULL == iWgName);
    ASSERT(NULL == iHomeDir);
    ASSERT(NULL == iEnv);
    ASSERT(NULL == iMidletSuite);
    ASSERT(NULL == iUtils);
    ASSERT(NULL == iUiFactory);
    ASSERT(NULL == iGrFactory);

    ASSERT(0    == iGrLibrary.Handle());        // Did we unload the gr library
    ASSERT(NULL == iCurrentDisplayable);        // Did we clean up correctly

    LCDUI_DEBUG_INT("CMIDToolkit::~CMIDToolkit(%d): DESTRUCTION PHASE 1 <END>", (TInt)this);
}

void CMIDToolkit::HandleExitL()
{
    PostDisplayEvent(EExit);
}


void CMIDToolkit::HandleForegroundL(TBool aForeground)
{
    if (aForeground)
    {
        // reset flag
        iMidletRequestedBg = EFalse;
        iSentToBgTime = 0;
        LCDUI_DEBUG("**FE**");
    }
    else
    {
        iSentToBgTime.HomeTime();
        LCDUI_DEBUG("**BE**");
    }
    iEnv->HandleForegroundL(aForeground);
    PostDisplayEvent(aForeground?EForeground:EBackground);
}

void CMIDToolkit::HandleResourceChangeL(TInt aType)
{
    iEnv->HandleResourceChangeL(aType);
}

void CMIDToolkit::HandleSwitchOnEventL()
{
    iEnv->HandleSwitchOnL(ETrue);
}

/**
 * CONSTRUCTION PHASE 2.
 * Java Side.
 */
void CMIDToolkit::ConstructL
(
    JNIEnv&             aJni,
    jobject             aPeer,
    TJavaEventServer    aServer,
    const TDesC&        aAppName,
    TInt                aAppUid,
    const TDesC&        aAppHome,
    RPointerArray<HBufC>* aAttributes
)
{
    LCDUI_DEBUG_INT("CMIDToolkit::ConstructL(%x) CONSTRUCTION PHASE 2 ", (TInt)this);
    ASSERT(iPhase == EPhase1);
    iPhase = EPhase2;

    CJavaEventSourceBase::ConstructL(aJni,aPeer,aServer);

    jclass clz = aJni.GetObjectClass(aPeer);
    iHandleItemEvent = aJni.GetMethodID(clz, "handleItemEvent", "(Ljavax/microedition/lcdui/Item;IIII)V");
    iHandleDisplayableEvent = aJni.GetMethodID(clz, "handleDisplayableEvent", "(Ljavax/microedition/lcdui/Displayable;IIII)V");
    iHandleDisplayEvent = aJni.GetMethodID(clz, "handleDisplayEvent", "(Ljavax/microedition/lcdui/Toolkit;IIII)V");
    iHandleNotifyMethod = aJni.GetMethodID(clz, "handleAsyncEvent", "(Ljava/lang/Object;I)V");
    iHandleCanavsGraphicsItemPainterEvent = aJni.GetMethodID(
                                                clz,
                                                "handleCanvasGraphicsItemPainterEvent",
                                                "(Ljavax/microedition/lcdui/CanvasGraphicsItemPainter;IIII)V");

    aJni.DeleteLocalRef(clz);

    if (0 == iHandleNotifyMethod       ||
            0 == iHandleDisplayEvent       ||
            0 == iHandleDisplayableEvent   ||
            0 == iHandleItemEvent
       )
    {
        User::Leave(KErrGeneral);
    }

    iHomeDir=aAppHome.AllocL();
    iAppUid=TUid::Uid(aAppUid);

    iMidletSuite = new(ELeave) CMIDletSuite;
    if (aAttributes)
    {
        iMidletSuite->SetAttributesL(*aAttributes);
    }

    CMIDToolkit* toolkit = this;
    const TDesC* appName = &aAppName;

    TInt err = ExecuteTrap(&CMIDToolkit::InvokeSvrConstructL, toolkit, appName);
    User::LeaveIfError(err);
}

void CMIDToolkit::InvokeSvrConstructL(CMIDToolkit* aToolkit, const TDesC* aName)
{
    User::LeaveIfError(RLcdui::Get()->CreateAppUi());

    aToolkit->SvrConstructL(*aName);
}


/**
 * CONSTRUCTION PHASE 3.
 * Server Side.
 */
void CMIDToolkit::SvrConstructL(const TDesC& aAppName)
{
    LCDUI_DEBUG_INT("CMIDToolkit::SvrConstructL(%x) CONSTRUCTION PHASE 3", (TInt)this);
    ASSERT(iPhase == EPhase2);
    iPhase = EPhase3;   // fully constructed

    //
    // This is the first method called server side, so the first place
    // we can safely obtain these pointers.
    //
    iCoeEnv = CCoeEnv::Static();

    iEnv = new(ELeave) CMIDEnv(this, TSize());
    iEnv->ConstructL();

    CreateTaskListEntryL(aAppName);
    SetTaskListEntry(EFalse);

    //
    //
    //
    LoadLibrariesL();
}

void CMIDToolkit::CreateTaskListEntryL(const TDesC& aAppName)
{
    ASSERT(NULL == iWgName);
    iWgName = java::ui::CoreUiAvkonLcdui::getInstance().getWindowGroupName();
    iWgName->SetRespondsToSwitchFilesEvent(EFalse);
    iWgName->SetCaptionL(aAppName);
}

void CMIDToolkit::LoadLibrariesL()
{
    ASSERT(NULL == iGrFactory);
    ASSERT(NULL == iUiFactory);

    RFs& fs = iCoeEnv->FsSession();
    TDisplayMode screenMode=iCoeEnv->ScreenDevice()->DisplayMode();
#ifdef RD_JAVA_S60_RELEASE_9_2
    if (screenMode == EColor16MAP)
    {
        screenMode = EColor16MA;
    }
#endif

    //
    // Load the GR dll
    //
    User::LeaveIfError(iGrLibrary.Load(KGrLibName));
    TCreateGraphicsFactoryFunc CreateGraphicsFactoryL;
    CreateGraphicsFactoryL = (TCreateGraphicsFactoryFunc)iGrLibrary.Lookup(1);
    if (!CreateGraphicsFactoryL)
    {
        User::Leave(KErrBadLibraryEntryPoint);
    }
    iGrFactory = (*CreateGraphicsFactoryL)(fs, screenMode);

    MLcduiPlugin* plugin = RLcdui::Get()->Plugin();
    if (!plugin)
    {
        User::Leave(KErrGeneral);
    }

    iUiFactory = plugin->CreateComponentFactoryL();
    iUiFactory->ConstructL(*iEnv);

    iUtils = iUiFactory->CreateUtilsL();
    iEnv->SetUtils(iUtils);
}

//
// Enables events
//
void CMIDToolkit::ActivateL()
{
    iOpen=ETrue;
    SetTaskListEntry(ETrue);
    RLcdui::Get()->Plugin()->SetObserver(this);
    iCoeEnv->RootWin().EnableReceiptOfFocus(ETrue);
}

void CMIDToolkit::SetTaskListEntry(TBool aVisible)
{
    iWgName->SetRespondsToShutdownEvent(aVisible);
    iWgName->SetHidden(!aVisible);
    iWgName->SetAppUid(iAppUid);
    iWgName->SetWindowGroupName(iCoeEnv->RootWin());
}

TPtrC CMIDToolkit::MidletName() const
{
    return iWgName->Caption();
}

void CMIDToolkit::SetCurrentL(MMIDDisplayable* aDisplayable)
{
    MMIDDisplayable* newDisplayable = aDisplayable;
    MMIDDisplayable* oldDisplayable = iCurrentDisplayable;

    // Prepare switch
    TRevertCurrent revert(iCurrentDisplayable);
    CleanupStack::PushL(TCleanupItem(TRevertCurrent::Revert, &revert));

    ASSERT(newDisplayable != oldDisplayable);

    if (NULL == newDisplayable)
    {
        ASSERT(oldDisplayable);
        ASSERT(oldDisplayable->Component()->Type() == MMIDComponent::EAlert);
    }

    ResetInactivityTimeL();

    // Set it, revert will rollback if required.
    iCurrentDisplayable = newDisplayable;

    // Activate new displayable first. If this leaves the java toolkit
    // will have to cope with an unwanted pending switch event.
    if (newDisplayable)
    {
        newDisplayable->HandleCurrentL(ETrue);
    }
    // When new Displayable is EAlert and old Displayable is ECanvas then
    // variable iOldFullScreenDisplayable will have the value ECanvas.
    // This applies for Canvas in Normal mode and in Full Screen mode.
    // Deactivate old displayable - if this fails the
    // displayable is itself responsible for cleanup.
    // Deactivate also old Full Screen displayable.
    if (oldDisplayable)
    {
        TInt ignore1;
        TRAP(ignore1, oldDisplayable->HandleCurrentL(EFalse));
        MMIDComponent::TType newType = newDisplayable->Component()->Type();
        if ((newType == MMIDComponent::EAlert || (newType == MMIDComponent::ETextBox &&
                newDisplayable->IsPopupTextBox())))
        {
            if (!iOldFullScreenDisplayable)
            {
                iOldFullScreenDisplayable = oldDisplayable;
            }
        }
        else
        {
            if (newDisplayable == iOldFullScreenDisplayable)
            {
                iOldFullScreenDisplayable = NULL;
            }
            else
            {
                if (iOldFullScreenDisplayable)
                {
                    TRAP(ignore1,
                         iOldFullScreenDisplayable->HandleCurrentL(EFalse));
                    iOldFullScreenDisplayable = NULL;
                }
            }
        }

        MMIDComponent::TType oldType = oldDisplayable->Component()->Type();

        if (newType == MMIDComponent::ECanvas && newType == oldType)
        {
            // When old and new displayable type is canvas during
            // deactivation of old displayable osk is set to background
            // and no pointer events goes to new displayable.
            // Change OSK background state to false
            newDisplayable->ChangeOSKBackgroundState(EFalse);
        }
    }

    CleanupStack::Pop();    // revert
}

void CMIDToolkit::ResetInactivityTimeL()
{
    TInt status = KErrNotFound;
    RProperty::Get(KPSUidScreenSaver, KScreenSaverAllowScreenSaver,status);

    // If Screen Saver is enabled and is inactive reset timers
    // Keep lights on and screensaver disabled. When status is >0 it means
    // that screen saver is not allowed to be activated
    if (!status)
    {
        TInt isTimeoutEnabled = KErrNone;
        TInt errPCenrep = KErrNone;
        CRepository* pCenrep = CRepository::NewLC(KCRUidPersonalizationSettings);
        if (pCenrep)
        {
            errPCenrep = pCenrep->Get(
                             KSettingsScreensaverTimeoutItemVisibility,
                             isTimeoutEnabled);
        }
        CleanupStack::PopAndDestroy(pCenrep);

#if defined(__WINSCW__)
        if (!isTimeoutEnabled)
        {
            isTimeoutEnabled = 1;
        }
#endif

        // Screen Saver Time out value
        TInt screenSaverTimeout = KErrNone;
        TInt errSCenrep = KErrNone;
        CRepository* securityCenrep = CRepository::NewLC(KCRUidSecuritySettings);
        if (securityCenrep)
        {
            errSCenrep = securityCenrep->Get(
                             KSettingsAutomaticKeyguardTime, screenSaverTimeout);
        }
        CleanupStack::PopAndDestroy(securityCenrep);

        // Inactivity time in seconds
        TInt userInactivity = User::InactivityTime().Int();

        // Check if screen saver is inactive, if so reset timers
        if (errPCenrep == KErrNone && errSCenrep == KErrNone &&
                isTimeoutEnabled && userInactivity < screenSaverTimeout)
        {
            User::ResetInactivityTime();
        }
    }
}

void CMIDToolkit::BringToForeground()
{
    LCDUI_DEBUG("**RF**");

    // Block the foreground granting to MIDlet immediately after user has switched
    // MIDlet to background. If the background was requested by the MIDlet,
    // the foreground request made by MIDlet should not be prevented.
    TTime now;
    now.HomeTime();
    TTimeIntervalMicroSeconds elapsedTime = now.MicroSecondsFrom(iSentToBgTime);
    if (elapsedTime > TTimeIntervalMicroSeconds(KChangingToBackgroundTimeout) ||
            iMidletRequestedBg)
    {
        if (mFirst)
        {
            TRAP_IGNORE(HandleForegroundL(ETrue));
        }

        SetOrdinalPosition(KWgOrdinalForeground);

        if (mFirst)
        {
            mFirst = EFalse;
        }
    }

    // Stop the start screen if it is still active.
    java::ui::CoreUiAvkonAppUi* appUi = java::ui::CoreUiAvkonLcdui::getInstance().getJavaUiAppUi();
    if (appUi && appUi->hasStartScreen())
    {
        MMIDComponent* content = iCurrentDisplayable ? iCurrentDisplayable->Component() : NULL;

        TBool isCanvas = EFalse;
        TBool isCanvasReadyToBlit = EFalse;
        if (content)
        {
            if (content->Type() == MMIDComponent::ECanvas)
            {
                isCanvas = ETrue;
                MMIDCanvas* canvas = static_cast<MMIDCanvas*>(content);
                isCanvasReadyToBlit = canvas->ReadyToBlit();
            }
        }

        if (!content || !isCanvas || isCanvasReadyToBlit)
        {
            if (iCurrentDisplayable)
            {
                iCurrentDisplayable->DrawNow();
            }
            appUi->stopStartScreen();
        }
    }
}

void CMIDToolkit::SendToBackground()
{
    LCDUI_DEBUG("**RB**");

    if (mFirst)
    {
        java::ui::CoreUiAvkonAppUi* appUi = java::ui::CoreUiAvkonLcdui::getInstance().getJavaUiAppUi();
        appUi->stopStartScreen(false); // no screenshot
        mFirst = EFalse;
    }
    iMidletRequestedBg = ETrue;
    SetOrdinalPosition(KWgOrdinalBackground);
}

void CMIDToolkit::SetOrdinalPosition(TInt aPos)
{
    RWindowGroup& group = iCoeEnv->RootWin();
    group.SetOrdinalPosition(aPos);
}

void CMIDToolkit::ClearDisplayable()
{
    MMIDDisplayable* current = iCurrentDisplayable;
    iCurrentDisplayable = NULL;
    if (current)
    {
        TInt ignore;
        TRAP(ignore, current->HandleCurrentL(EFalse));
    }
    SendToBackground();
}

/**
 * SERVER SIDE
 */
void CMIDToolkit::DisposeObject(MMIDComponent* aObject)
{
    for (TInt i = iObjects.Count(); i--;)
    {
        TObjectEntry* entry = iObjects[i];
        if (entry->iComponent == aObject)
        {
            DisposeEntry(i, ETrue);
            return;
        }
    }
    ASSERT(NULL == aObject);
}

void CMIDToolkit::DestroyUi()
{
    //
    // Cleanup any components which have not been finalized yet.
    //
    // This is the last change to finalize them.
    //
    const TInt count = iObjects.Count();
    for (TInt i=count-1; i>=0; --i)
    {
        DisposeEntry(i, EFalse);
    }

    if (iGrFactory)
    {
        iGrFactory->Dispose();
        iGrFactory = NULL;
    }

    if (iUtils)
    {
        if (iEnv)
        {
            iEnv->SetUtils(NULL);
        }
        iUtils->Dispose();
        iUtils = NULL;
    }

    if (iUiFactory)
    {
        iUiFactory->Dispose();
        iUiFactory = NULL;
    }

    iGrLibrary.Close();

    delete iEnv;
    iEnv = NULL;
}

/**
 * DESTRUCTION PHASE 3 - SERVER SIDE
 *
 * This method is called when the event source has no more references
 * and is about to be deleted. It is called in the context of the
 * server thread, allowing server side resources to be  cleaned up.
 *
 * It is called before FinalizeJni() so there may still be live JNI
 * weak references contained in MMIDComponent objects referenced by
 * the iObjects array.
 *
 * If SvrConstructL fails, the calling thread is responsible for calling
 * Dispose() on the toolkit, which will lead to FinalizeSvr being called
 * in the context of the server thread. This is therefore the appropriate
 * place to cleanup server side resources.
 *
 * Like any normal destructor it should be able to cope with cleaning up
 * partically constructed objects. Unlike a normal destructor, additional
 * code (including FinalizeJni and the real destructor) will be run *after*
 * this method - so any instance variables that could be queried by these
 * methods should be cleaned up appropriately.
 *
 * Here we take down instance members in reverse order that they were
 * allocated/constructed.
 *
 */
void CMIDToolkit::FinalizeSvr()
{
    LCDUI_DEBUG_INT("CMIDToolkit::FinalizeSvr(%d) DESTRUCTOR PHASE 3", (TInt)this);

    switch (iPhase)
    {
    case ENone:
        ASSERT(iPhase != ENone);
        break;

    case EPhase1:
    case EPhase2:
        return;

    case EPhase3:
        iPhase = EPhase2;
        break;
    }

    // Always use exit effect when exiting midlet
    GfxTransEffect::BeginFullScreen(AknTransEffect::EApplicationExit, TRect(),
                                    AknTransEffect::EParameterType, AknTransEffect::GfxTransParam(iAppUid));


    //
    // Send MIDlet to background.
    //
    ClearDisplayable();

#ifdef CLEAR_TASKLIST_IN_CLOSE
    //
    // Remove tasklist entry.
    //
    iAppUid.iUid=0;
    SetTaskListEntry(EFalse);
#endif

    DestroyUi();

    if (iCoeEnv)
    {
        //
        // Refuse focus
        //
        iCoeEnv->RootWin().EnableReceiptOfFocus(EFalse);
        iCoeEnv = NULL;
    }

    if (RLcdui::Get()->Plugin())
    {
        RLcdui::Get()->Plugin()->SetObserver(NULL);
    }

    DestroyUi();

    iWgName = NULL;

    iCurrentDisplayable = NULL;

}

/**
 * DESTRUCTION PHASE 2 - Java Side.
 *
 * Multi-thead note:
 *
 * Although this method runs Java side, there should be no remaining server
 * side references to the object by the time this method runs. It is safe
 * to access server side data structures - such as the disposed reference
 * queue.
 *
 */
void CMIDToolkit::FinalizeJni(JNIEnv& aJni)
{
    LCDUI_DEBUG_INT("CMIDToolkit::FinalizeJni(%d) DESTRUCTION PHASE 2", (TInt)this);

    switch (iPhase)
    {
    case ENone:
        ASSERT(iPhase != ENone);
        break;

    case EPhase1:
        // We didn't even get as far as running ConstructL()
        return;

    case EPhase2:
        // We got at least part way through ConstructL().
        iPhase = EPhase1;
        break;

    case EPhase3:
        ASSERT(iPhase != EPhase3);
        break;
    }

    //
    // There should be no more entries in the object map, only
    // entries on the finalization queue.
    //
    ASSERT(0 == iObjects.Count());

    FinalizeReferences(aJni);   // dispose finalize queue

    //
    // MUTEX NOTE: FinalizeMutex not needed as the calling
    // thread holds the last reference.
    //
    iFinalizeQueue = iDisposedQueue;
    iFinalizeCount = iDisposedCount;
    iDisposedQueue  = NULL;
    iDisposedCount  = 0;

    FinalizeReferences(aJni);   // dispose finalize queue

    delete iHomeDir;
    iHomeDir = NULL;

    delete iMidletSuite;
    iMidletSuite = NULL;
}

void CMIDToolkit::DisposeEntry(TInt aIndex, TBool aPostEvent)
{
    TObjectEntry* entry = iObjects[aIndex];
    iObjects.Remove(aIndex);

    if (entry->iComponent)
    {
        /*
         * Make weak ref available for cleanup.
         */
        entry->iDisposed = entry->iComponent->iPeer;
        entry->iComponent->iPeer = NULL;
        entry->iComponent->Dispose();
        entry->iComponent = NULL;
    }

    if (entry->iDisposed)
    {
        //
        // Park entry on the dispose queue.
        //
        entry->iNextEntry = iDisposedQueue;
        iDisposedQueue = entry;
        iDisposedCount++;

        // MUTEX NOTE - can test iFinalizeQueue as we only write it when NULL
        // in which case no-one else will be reading/writing.
        if (aPostEvent && (iDisposedCount > KPayload) && (NULL == iFinalizeQueue))
        {
            iFinalizeMutex.Wait();
            iFinalizeQueue = iDisposedQueue;
            iFinalizeCount = iDisposedCount;
            PostEvent(new CMIDFinalizeEvent);
            iDisposedQueue  = NULL;
            iDisposedCount  = 0;
            iFinalizeMutex.Signal();
        }
    }
    else
    {
        //
        // There is no peer to dispose, so we are free to
        // delete the entry immediately, or part it on
        // on a free list.
        //
        delete entry;
    }
}

void TRevertCurrent::Revert(TAny* aPtr)
{
    TRevertCurrent& revert = *static_cast<TRevertCurrent*>(aPtr);
    revert.iCurrentPointer = revert.iPrevious;
}


void CMIDFinalizeEvent::Dispatch(JNIEnv& aJni)
{
    Object().FinalizeReferences(aJni);
}

CMIDToolkit::TObjectEntry::TObjectEntry(MMIDComponent* aComponent)
        : iComponent(aComponent)
        , iDisposed(NULL)
{
}

TInt CMIDToolkit::New
(
    JNIEnv&             aJni,
    jobject             aPeer,
    TJavaEventServer    aServer,
    const TDesC&        aAppName,
    TInt                aAppUid,
    const TDesC&        aAppHome,
    RPointerArray<HBufC>* aAttributes
)
{
    TRAPD(handle, handle=CreateToolkitL(aJni, aPeer, aServer, aAppName, aAppUid, aAppHome, aAttributes));
    return handle;
}

TInt CMIDToolkit::CreateToolkitL
(
    JNIEnv&             aJni,
    jobject             aPeer,
    TJavaEventServer    aServer,
    const TDesC&        aAppName,
    TInt                aAppUid,
    const TDesC&        aAppHome,
    RPointerArray<HBufC>* aAttributes
)
{
    TConstructor self(aJni);
    self->ConstructL(aJni,aPeer,aServer,aAppName,aAppUid,aAppHome,aAttributes);
    return self.GetHandle();
}

TBool CMIDToolkit::CheckEvent(CJavaEventBase* /* aEvent */)
{
    return iOpen;
}

void CMIDToolkit::FinalizeReferences(JNIEnv& aEnv)
{
    TObjectEntry* queue = NULL;
    TInt          count = 0;

    iFinalizeMutex.Wait();

    queue = iFinalizeQueue;
    count = iFinalizeCount;

    iFinalizeQueue = NULL;
    iFinalizeCount = 0;

    iFinalizeMutex.Signal();

    while (queue)
    {
        TObjectEntry* next = queue->iNextEntry;
        ASSERT(queue->iDisposed);
        aEnv.DeleteWeakGlobalRef((jweak)(queue->iDisposed));
        delete queue;
        queue = next;
        --count;
    }

    ASSERT(0 == count);
}

CMIDletSuite::CMIDletSuite()
        :iAttributes()
{
}

/**
 * Takes ownership of pointed objects.
 */
void CMIDletSuite::SetAttributesL(RPointerArray<HBufC>& aAttributes)
{
    const TInt count = aAttributes.Count();
    for (TInt i=0; i<count; i++)
    {
        HBufC* entry = aAttributes[i];
        iAttributes.InsertL(entry, i);
        aAttributes[i]=NULL;
    }
    aAttributes.Reset();
}

CMIDletSuite::~CMIDletSuite()
{
    iAttributes.ResetAndDestroy();
    iAttributes.Close();
}

TInt CMIDletSuite::GetAttribute(const TDesC& aName, TPtrC& aValue)
{
    const TInt length = iAttributes.Count();
    for (TInt ii=0; ii<length; ii+=2)
    {
        if (aName.CompareC(*iAttributes[ii]) == 0)
        {
            aValue.Set(*iAttributes[ii+1]);
            return KErrNone;
        }
    }
    return KErrNotFound;
}

TInt CMIDToolkit::MidletAttribute(const TDesC& aAttributeName, TPtrC& aAttributeValue)
{
    return iMidletSuite->GetAttribute(aAttributeName, aAttributeValue);
}


TInt CMIDToolkit::PostDisplayEvent(TEventType aEvent)
{
    //
    // We pass the toolkit peer as CMIDEvent checks the source weak ref and
    // will nop any events to expired objects.
    //
    jweak toolkit = Peer();

    CMIDEvent* event = new CMIDEvent(iHandleDisplayEvent, toolkit, aEvent);

    if (event)
    {
        return PostEvent(event);
    }
    return KErrNoMemory;
}

TInt CMIDToolkit::PostDisplayableEvent(MMIDComponent& aDisplayable, TEventType aEvent, TInt aParam0, TInt aParam1)
{
    jobject displayable = MIDJOBJECT(&aDisplayable);

// Even if displayable is null it is safe to post the event as the event will
// dispose itself without posting itself.
    CMIDEvent* event = new CMIDEvent(iHandleDisplayableEvent, displayable, aEvent, aParam0, aParam1);
    if (event)
    {
        return PostEvent(event);
    }
    return KErrNoMemory;
}

TInt CMIDToolkit::PostItemEvent(MMIDComponent& aItem, TEventType aEvent, TInt aParam0, TInt aParam1, TInt aParam2)
{
    jobject item = MIDJOBJECT(&aItem);
    ASSERT(item);
    CMIDEvent* event = new CMIDEvent(iHandleItemEvent, item, aEvent, aParam0, aParam1, aParam2);
    if (event)
    {
        return PostEvent(event);
    }
    return KErrNoMemory;
}

TInt CMIDToolkit::PostCanvasGraphicsItemPainterEvent(MMIDComponent& aCanvasGraphicsItemPainter,
        TEventType aEvent, TInt aParam0, TInt aParam1)
{
    jobject canvasgraphicsitempainter = MIDJOBJECT(&aCanvasGraphicsItemPainter);
    CMIDEvent* event = new CMIDEvent(iHandleCanavsGraphicsItemPainterEvent, canvasgraphicsitempainter,
                                     aEvent, aParam0, aParam1);
    if (event)
    {
        return PostEvent(event);
    }
    return KErrNoMemory;
}
