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


#ifndef SWTDISPLAY_H
#define SWTDISPLAY_H


// In emulator debug builds, uncomment the following line to compile eSWT with
// memory leak checking enabled. Besides checks for leaked Java objects, this will
// cause eSWT to create a separate heap for its native UI thread. In this heap it
// will be checked when the Display object is disposed that all the allocated
// heap cells have been freed. Failing the check will cause the thread to panic.
// Note: Browser widget has problems when this flag is enabled.
// Pay attention when using in other files! Include "swtdisplay.h"!!!
//#define ESWT_NATIVE_UITHREAD_OWN_HEAP

// In emulator debug builds, uncomment the following line to compile eSWT with
// checking for situations where Java application is disposing native resources
// ( e.g. Images, Fonts ) too soon while they are still being used by the widgets.
//#define ESWT_EARLY_DISPOSAL_CHECKING_ENABLED


#include <aknapp.h>
#include <AknDoc.h>
#include <aknappui.h>

#include <coreuiappui.h>
#include <coreuiavkoneswt.h>
#include <coreuiappuichild.h>

#include "swtdisplaybase.h"
#include "swtclient.h"
#include "swteventqueue.h"

class CEikonEnv;
class CApaWindowGroupName;
class CSwtServer;
class CSwtDialogBroker;


/**
 * C++ counterpart to the org.eclipse.swt.widgets.Display Java class.
 *
 * Additionally to implementing Display's features, this class
 * also serves as the hub for all communications between the main thread and
 * the UI thread. Actually, it is it that creates the AppUi's thread.
 *
 * If the macro ESWT_OWN_APPUI macro is defined, the AppUi will be
 * created by the Display ( instead of the launcher ). This must not be used for
 * release builds as it yields a broken behaviour, but it enables memory leaks
 * detection by allocating a separate heap for the UI.
 *
 * @lib eswt
 */
NONSHARABLE_CLASS(CSwtDisplay)
        : public CBase
        , public ASwtDisplayBase
        , public java::ui::CoreAppUiChild
{
// Any thread
public:
    static inline CSwtDisplay* CurrentOrNull();
    static inline CSwtDisplay& Current();

// Java Ui Thread
public:
    static CSwtDisplay* NewL(JNIEnv& aJniEnv, jobject aPeer, TInt aDisplayParameter);
    void Dispose(JNIEnv& aJniEnv);
    void RequestRunDialogL(TSwtPeer aPeer, TInt aDialogType, TInt aStyle, const TDesC& aTitle,
                           const TDesC& aText1 = KNullDesC(), const TDesC& aText2 = KNullDesC(),
                           const TInt aInt1 = 0, const TInt aInt2 = 0, const TInt aInt3 = 0);
    inline TInt ExecuteInNativeUiThread(const MSwtFunctor& aFunctor);

// Native Ui Thread
public:
    void OfferWsEventL(const TSwtWsEvent& aEvent, CCoeControl* aDestination = NULL);
    void RemoveDialogBroker(CSwtDialogBroker* aBroker);

// From java::ui::CoreAppUiChild
// Native Ui Thread
public:
    void HandleForegroundEventL(TBool aForeground);
    void HandleSwitchOnEventL(CCoeControl* aDestination);
    void HandleApplicationSpecificEventL(TInt aType,const TWsEvent& aEvent);
    void HandleCommandL(TInt aCommand);
    void HandleResourceChangeL(TInt aType);
    TBool HandleWsEventL(const TWsEvent& aEvent, CCoeControl* aDestination);

// From MSwtDisplay
// Any thread
public:
#ifdef _DEBUG
    TBool IsCurrentThreadNativeUi() const;
#endif
    TSwtPeer JavaPeer();

// From MSwtDisplay
// Native UI Thread
public:
    void SetNameInTaskListL(const TDesC* aNewName);
    void SetUIDInTaskList(TInt aNewUID);
    void SetAppVisible(TBool aVisible);
    void SetUiReady(TBool aFullScreenUi);
    TBool IsUiReady() const;

// Java Ui Thread
private:
    static CSwtDisplay* DoNewL(JNIEnv& aJniEnv, TSwtPeer aPeer, TInt aApplicationUid);
    inline CSwtDisplay(TSwtPeer aPeer);
    void ConstructInJavaUiThreadL(JNIEnv& aJniEnv);

// Native Ui Thread
private:
    static void DoExecuteInNativeUiThreadL(const MSwtFunctor* aFunctor);
    static TInt CloseEventDispatchTimerCallback(TAny* aThis);
    ~CSwtDisplay();
    void ConstructInNativeUiThreadL();
    void DestroyInNativeUiThread();
    CSwtDialogBroker* CreateDialogBrokerL();
    void HandleCloseEventDispatchTimerCallbackL();
    void DialogAboutToOpen();

// Any thread data
private:
    TSwtPeer iPeer;
    RThread iNativeUiThread;

// Java Ui Thread data
private:
    RSwtClient iClient; // own

// Native Ui Thread data
private:
    TBool iCloseEventDispatched;
    CPeriodic* iCloseEventDispatchTimer; // own
    RPointerArray<CSwtDialogBroker> iDialogBrokers; // own
    TBool iUiReady;
    JavaVM* iVM;
    TBool iDisposing;
};

/**
 * Returns the current Display or NULL if there is none.
 */
inline CSwtDisplay* CSwtDisplay::CurrentOrNull()
{
    if (java::ui::CoreUiAvkonEswt::isCoreUiCreated())
    {
        java::ui::CoreUiAvkonEswt& ui = java::ui::CoreUiAvkonEswt::getInstance();
        return ui.getEswtSupport().display();
    }
    else
    {
        return 0;
    }
}

/**
 * Returns the current Display.
 * Can only be called after a Display has been created
 */
inline CSwtDisplay& CSwtDisplay::Current()
{
    CSwtDisplay* display = CurrentOrNull();
    ASSERT(display);
    return *display;
}

/**
 * Executes a wrapped function in the UI's thread
 */
inline TInt CSwtDisplay::ExecuteInNativeUiThread(const MSwtFunctor& aFunctor)
{
    ASSERT(iClient.Handle()!=0);
    return iClient.Execute(aFunctor);
}

/**
 * C'tor
 */
inline CSwtDisplay::CSwtDisplay(TSwtPeer aPeer)
        : ASwtDisplayBase()
        , iPeer(aPeer)
{
}

#endif // SWTDISPLAY_H
