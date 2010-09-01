/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef CMIDTOOLKIT_H
#define CMIDTOOLKIT_H

#include <mevents.h>
#include <coecntrl.h>
//
#include <Lcdglue.h>
#include "CMIDEnv.h"
#include "MIDUtils.h"
#include "lcdui.h"
#include "lcdgr.h"

//
class CApaWindowGroupName;
class CMIDToolkit;
class MMIDDisplayable;

/**
Utility class providing access to the MIDlet suite attributes.
*/
class CMIDletSuite : public CBase
{
public:
    CMIDletSuite();
    ~CMIDletSuite();

    void SetAttributesL(RPointerArray<HBufC>& aAttributes);

    TInt GetAttribute(const TDesC& aName, TPtrC& aValue);

private:
    RPointerArray<HBufC> iAttributes;
};

/**
 *
 * Main class in the framework, one per MIDlet
 *
 * CMIDToolkit isA java event source and as such has an extremely complicated lifecycle with
 * several states:
 *
 * The source is constructed via a mechanism that ensures the first three phases of construction
 * succeed fully or the object is fully destroyed. This is handled by the New() static method in
 * CJavaEventSource<CMIDToolkit>.
 *
 * CMIDToolkit()        // PHASE 1
 * ConstructL()         // PHASE 2
 * SvrConstructL()      // PHASE 3
 *
 * FinalizeSvr()        // PHASE 3
 * FinalizeJni()        // PHASE 2
 * ~CMIDToolkit()       // PHASE 1
 *
 */
class CMIDToolkit : public CJavaEventSource<CMIDToolkit>, public MMIDObserver
{
public:
    TInt PostDisplayEvent(TEventType aEvent);
    TInt PostDisplayableEvent(MMIDComponent& aDisplayable, TEventType aEvent, TInt aParam0, TInt aParam1);
    TInt PostItemEvent(MMIDComponent& aItem, TEventType aEvent, TInt aParam0, TInt aParam1, TInt aParam2);

    TInt PostCanvasGraphicsItemPainterEvent(MMIDComponent& aCanvasGraphicsItemPainter,
                                            TEventType aEvent, TInt aParam0, TInt aParam1);

    inline MMIDDisplayable* Current();
    TPtrC MidletName() const;
    inline TUid MidletUid() const;
    inline TPtrC MidletHome() const;

    /**
     * Inform CMIDToolkit about deleting of object implementing MMIDDisplayable.
     *
     * @param displayable deleting displayble
     * @since Java 2.0
     */
    inline void DisplayableIsDestructed(const MMIDDisplayable* aDisplayable);

    TInt MidletAttribute(const TDesC& aAttributeName, TPtrC& aAttributeValue);

public:
    /**
     * Other server side methods
     */
    static void InvokeSvrConstructL(CMIDToolkit* aToolkit,const TDesC* aName);
    void SetCurrentL(MMIDDisplayable* aDisplayable);
    void ClearDisplayable();
    TBool PostSerialEvent(jobject aRunnable);
    void DisposeObject(MMIDComponent* aObject);

    // Returns pointer to last fullscreen displayble.
    inline const MMIDDisplayable* LastFullscreenDisplayable() const;

    //
    // Activate event sources
    //
    void ActivateL();
    TInt RegisterComponentL(MMIDComponent* aComponent, TJavaPeer aPeer);

public:
    // Factory
    static TInt New
    (
        JNIEnv&             aJni,
        jobject             aPeer,
        TJavaEventServer    aServer,
        const TDesC&        aAppName,
        TInt                aAppUid,
        const TDesC&        aAppHome,
        RPointerArray<HBufC>* aAttributes   // may be NULL
    );

    /**
     * PHASE 1 Constructor.
     */
    CMIDToolkit();

    /**
     * PHASE 1 Destructor
     */
    ~CMIDToolkit();

    /**
     * PHASE 2 Constructor
     */
    void ConstructL(JNIEnv&         aJni,
                    jobject         aPeer,
                    TJavaEventServer aServer,
                    const TDesC&    aAppName,
                    TInt            aAppUid,
                    const TDesC&    aHomeDir,
                    RPointerArray<HBufC>* aAttributes   // may be NULL
                   );

    inline MMIDComponentFactory* CMIDToolkit::ComponentFactory() const;
    inline MMIDUtils* Utils() const;
    inline MMIDEnv* Env() const;
    inline CApaWindowGroupName* WgName() const;     // _APIEXT_SASF_

public:
    inline MMIDGraphicsFactory& GraphicsFactory() const;
    inline static TJavaPeer ComponentPeer(MMIDComponent& aComponent);

    void BringToForeground();
    void SendToBackground();

public:
    /*
     * From MMIDObserver
     */
    void HandleExitL();
    void HandleForegroundL(TBool aForeground);
    void HandleSwitchOnEventL();
    void HandleResourceChangeL(TInt aType);
#ifdef RD_JAVA_NGA_ENABLED
    void HandleFullOrPartialForegroundL(TBool aFullOrPartialFg);
    void HandleFreeGraphicsMemory();
#endif

private:
    /**
     * Resets user inactivity timers
     */
    void ResetInactivityTimeL();

    static TInt CreateToolkitL
    (
        JNIEnv&             aJni,
        jobject             aPeer,
        TJavaEventServer    aServer,
        const TDesC&        aAppName,
        TInt                aAppUid,
        const TDesC&        aAppHome,
        RPointerArray<HBufC>* aAttributes       // may be NULL
    );

    /**
     * Phase 3 constructor.
     */
    void SvrConstructL(const TDesC& aName);

    void CreateTaskListEntryL(const TDesC& aName);
    void SetTaskListEntry(TBool);

    /**
     * Phase 3 destructor
     */
    void FinalizeSvr();

    /**
     * Phase 2 destructor
     */
    void FinalizeJni(JNIEnv& aJni);

    /**
     * Cleanup an object
     */
    void DisposeEntry(TInt aIndex, TBool aPostEvent);

    /**
     * Set window group ordinal position.
     */
    void SetOrdinalPosition(TInt aOrdinal);

    /**
     * Filter events - actually more of an on/off tap
     */
    TBool CheckEvent(CJavaEventBase*);

    void LoadLibrariesL();

    /**
     * If current displayable is canvas,
     * returns pointer to it.
     *
     * @return  pointer to the current canvas, if current displayable is canvas.
     *          NULL, if current displayable is not canvas.
     */
    MMIDCanvas* GetCurrentCanvas() const;

public:
    /*@ deprecated */
    jmethodID iHandleNotifyMethod;

private:
    jmethodID iHandleItemEvent;
    jmethodID iHandleDisplayableEvent;
    jmethodID iHandleDisplayEvent;
    jmethodID iHandleCanavsGraphicsItemPainterEvent;

private:
    enum {EGranularity=4};

private:
    enum TPhase
    {
        ENone,
        EPhase1,
        EPhase2,
        EPhase3
    };

    struct TObjectEntry
    {
    public:
        TObjectEntry(MMIDComponent* aComponent);
        union
        {
            MMIDComponent*  iComponent;
            TObjectEntry*   iNextEntry;
        };
        TJavaPeer       iDisposed;
    };

private:
    friend class CMIDFinalizeEvent;

    /**
     * Delete weak refs - called from CMIDDisposeEvent only..
     */
    void FinalizeReferences(JNIEnv& aEnv);

    void DestroyUi();

private:
    TPhase                  iPhase;
    TUid                    iAppUid;
    HBufC*                  iHomeDir;
    CMIDletSuite*           iMidletSuite;
    TObjectEntry*           iFinalizeQueue;             // finalization queue of weak ref entries
    TInt                    iFinalizeCount;
    RCriticalSection        iFinalizeMutex;

private:
    /**
     * Server fields
     */
    TObjectEntry*           iDisposedQueue;             // Disposed entry queue - pending finalization
    TInt                    iDisposedCount;

    CCoeEnv*                iCoeEnv;                    // Not Owned.
    MMIDDisplayable*        iCurrentDisplayable;        // Not Owned.
    MMIDDisplayable*        iOldFullScreenDisplayable;  // Not Owned.

    CApaWindowGroupName*    iWgName;
    CMIDEnv*                iEnv;

    RArray<TObjectEntry*>   iObjects;

    MMIDComponentFactory*   iUiFactory;
    MMIDUtils*              iUtils;

    RLibrary                iGrLibrary;
    MMIDGraphicsFactory*    iGrFactory;

    TBool                   iOpen;
    TBool                   iMidletRequestedBg; // indicates if MIDlet itself has requested sending to background
    TTime                   iSentToBgTime;      // timestamp when MIDlet was sent to background
    TBool                   mFirst;
};

/**
 * Useful functions:
 */
inline void CleanupDisposePushL(MMIDComponent* aComponent);
inline void CleanupPopComponent(MMIDComponent* aComponent);

inline MMIDDisplayable* CMIDToolkit::Current()
{
    return iCurrentDisplayable;
}

inline TUid CMIDToolkit::MidletUid() const
{
    return iAppUid;
}

inline TPtrC CMIDToolkit::MidletHome() const
{
    return *iHomeDir;
}

inline MMIDComponentFactory* CMIDToolkit::ComponentFactory() const
{
    return iUiFactory;
}

inline MMIDUtils* CMIDToolkit::Utils() const
{
    return iUtils;
}

inline MMIDEnv* CMIDToolkit::Env() const
{
    return iEnv;
}

// --> _APIEXT_SASF_
inline CApaWindowGroupName* CMIDToolkit::WgName() const
{
    return iWgName;
}
// <-- _APIEXT_SASF_

inline MMIDGraphicsFactory& CMIDToolkit::GraphicsFactory() const
{
    return *iGrFactory;
}

inline TJavaPeer CMIDToolkit::ComponentPeer(MMIDComponent& aComponent)
{
    return aComponent.iPeer;
}

inline void DisposeComponent(TAny* aPtr)
{
    MMIDComponent* component = static_cast<MMIDComponent*>(aPtr);
    component->Dispose();
}

inline void CleanupDisposePushL(MMIDComponent* aComponent)
{
    CleanupStack::PushL(TCleanupItem(DisposeComponent, aComponent));
}

inline void CleanupPopComponent(MMIDComponent* aComponent)
{
    CleanupStack::Pop(aComponent);
}

inline void CMIDToolkit::DisplayableIsDestructed(const MMIDDisplayable* aDisplayable)
{
    // If this is set as iOldFullScreenDisplayable, we need set this to NULL.
    if (aDisplayable == iOldFullScreenDisplayable)
    {
        iOldFullScreenDisplayable = NULL;

        // We need to inform current Displayable about destroying
        //  old full screen Displayable.
        if (iCurrentDisplayable)
        {
            iCurrentDisplayable->DisplayableBehindPopupIsDestroyed();
        }
    }
}

inline const MMIDDisplayable* CMIDToolkit::LastFullscreenDisplayable() const
{
    return iOldFullScreenDisplayable;
}

#endif // CMIDTOOLKIT_H
