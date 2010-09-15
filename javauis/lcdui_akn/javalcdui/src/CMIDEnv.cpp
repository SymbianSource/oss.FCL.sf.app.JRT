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

#include <coemain.h>
#include <eikenv.h>
#include <hal.h>
#include "CMIDEnv.h"
#include "CMIDToolkit.h"
#include "CMIDNotify.h"
#include "CMIDKeyTranslator.h"
#include "CMIDToLcduiObserver.h"

#ifdef RD_JAVA_NGA_ENABLED
#include <EGL/egl.h>
#include <GLES/gl.h>
#include <libc/string.h>
#endif // RD_JAVA_NGA_ENABLED

const TUint16 KCharComma = ',';

CMIDEnv::CMIDEnv(CMIDToolkit* aToolkit, const TSize& aSize)
        : iToolkit(aToolkit)
        , iObservers(EGranularity)
        , iCanvasAssumedSize(aSize)
        , iToLcduiObserver(NULL)
#ifdef RD_JAVA_NGA_ENABLED
        , iHardwareStatus(0)
#endif
{
    iCanvasData.iLock.CreateLocal();
}

void CMIDEnv::ConstructL()
{
    iKeyTranslator = new(ELeave) CMIDKeyTranslator(*this);
    iKeyTranslator->ConstructL();
    iToLcduiObserver = new(ELeave) CMIDToLcduiObserver();

    // On a non-full-screen (normal) mode Canvas the background image must be initially shown,
    // if the value of the Nokia-UI-Enhancement attribute is "CanvasHasBackground". The
    // attribute may be placed in the JAD or the manifest.
    iCanvasData.iHasBackground = MidletAttributeContainsVal(
                                     LcduiMidletAttributes::KAttribUIEnhancement,
                                     LcduiMidletAttributeValues::KUIEnhCanvasBackground);
#ifdef RD_JAVA_NGA_ENABLED
    InitHardwareStatusL();

    iVideoOverlayEnabled = MidletAttributeContainsVal(
                                    LcduiMidletAttributes::KAttribUIEnhancement,
                                    LcduiMidletAttributeValues::KUIEnhVideoOverlay);
#endif // RD_JAVA_NGA_ENABLED
    
    iMMAPILock.CreateLocal();
}

CMIDEnv::~CMIDEnv()
{
    if (iToLcduiObserver)
    {
        delete iToLcduiObserver;
        iToLcduiObserver = NULL;
    }
    delete iKeyTranslator;
    iObservers.Close();
    iCanvasData.iLock.Close();
    iMMAPILock.Close();
}

void CMIDEnv::SetUtils(MMIDUtils* aUtils)
{
    iKeyTranslator->SetUtils(aUtils);
}

TPtrC CMIDEnv::MidletName() const
{
    return iToolkit->MidletName();
}

TUid CMIDEnv::MidletUid() const
{
    return iToolkit->MidletUid();
}

TPtrC CMIDEnv::MidletHome() const
{
    return iToolkit->MidletHome();
}

#ifdef RD_SCALABLE_UI_V2
TUid CMIDEnv::MidletSuiteUid()
{
    if (!iMidletSuiteUid.iUid)
    {
        //Midlet suite uid is located inside brackets in the midlet home path.
        const TChar KUidStart = '[';
        const TChar KUidEnd = ']';

        const TPtrC midletHome = MidletHome();

        //Finding start and end positions
        TInt startPos = midletHome.Locate(KUidStart) + 1;
        TInt endPos = midletHome.LocateReverse(KUidEnd);

        if (startPos != KErrNotFound && endPos != KErrNotFound &&
                startPos < endPos && endPos <= midletHome.Length())
        {
            //Creating new pointer descriptor representing the midlet suite uid
            const TPtrC uidDesc = midletHome.Mid(startPos, endPos - startPos);

            if (uidDesc.Length())
            {
                //Creating unsigned integer out of the hex string.
                TUint uint;
                TLex lexer(uidDesc);
                TInt err = lexer.Val(uint, EHex);

                if (err == KErrNone)
                {
                    iMidletSuiteUid = TUid::Uid(uint);
                }
                else
                {
                    iMidletSuiteUid = TUid::Uid(KNullUidValue);
                }
            }
            else
            {
                iMidletSuiteUid = TUid::Uid(KNullUidValue);
            }
        }
        else
        {
            iMidletSuiteUid = TUid::Uid(KNullUidValue);
        }
    }
    return iMidletSuiteUid;
}
#endif // RD_SCALABLE_UI_V2


TBool CMIDEnv::PostJavaEvent
(
    MMIDComponent& aSource,
    TSourceType aSourceType,
    TEventType  aEventType,
    TInt        aEventData,
    TInt        aEventData1,
    TInt        aEventData2
)
{
#ifdef TRACE_EVENTS
    RDebug::Print(_L("CMIDEnv::PostJavaEvent(%x,%d,%d,%d,%d,%d)"),
                  aSource,
                  aSourceType,
                  aEventType,
                  aEventData,aEventData1, aEventData2);
#endif
    switch (aSourceType)
    {
    case EItem:
        return iToolkit->PostItemEvent(aSource, aEventType, aEventData, aEventData1, aEventData2);
    case EDisplayable:
        return iToolkit->PostDisplayableEvent(aSource, aEventType, aEventData, aEventData1);
    case ECanvasGraphicsItemPainterEvent:
        return iToolkit->PostCanvasGraphicsItemPainterEvent(aSource, aEventType, aEventData, aEventData1);
    default:
        ASSERT(EFalse);
    }
    return EFalse;
}

TBool CMIDEnv::PostJavaEvent(MMIDComponent& aSource,TSourceType aSourceType,TEventType aEventType,TInt aEventData)
{
    return PostJavaEvent(aSource, aSourceType, aEventType, aEventData, 0, 0);
}

TBool CMIDEnv::PostJavaEvent(MMIDComponent& aSource,TSourceType aSourceType,TInt aEventData)
{
    return PostJavaEvent(aSource, aSourceType, ENoType, aEventData, 0, 0);
}

TBool CMIDEnv::PostMidletEvent(TEventType aEventType)
{
    return iToolkit->PostDisplayEvent(aEventType);
}

MMIDNotifyEvent* CMIDEnv::NewNotifyL(MMIDComponent& aSource)
{
    return new(ELeave) CMIDNotifyEvent(iToolkit->ComponentPeer(aSource));
}

TBool   CMIDEnv::PostJavaNotify(MMIDNotifyEvent* aNotifyEvent)
{
    CMIDNotifyEvent* notify = static_cast<CMIDNotifyEvent*>(aNotifyEvent);
    return iToolkit->PostEvent(notify, CJavaEventBase::ENotifyPriority);
}

TBool CMIDEnv::TranslateKeyL(TMIDKeyEvent& aEvent, const TKeyEvent& aWsEvent, TEventCode aType)
{
    return iKeyTranslator->TranslateL(aEvent, aWsEvent, aType);
}

TBool CMIDEnv::PostKeyEvent(MMIDComponent& aSource, TMIDKeyEvent& aEvent)
{
    return iKeyTranslator->PostKeyEvent(aSource, aEvent);
}

void CMIDEnv::ResetKeys()
{
    iKeyTranslator->Reset();
}

MMIDDisplayable* CMIDEnv::Current()
{
    return iToolkit->Current();
}

/**
 * Set the zoom size which may be queried by Canvas in the
 * plugin. This is a relic from when Canvas was implemented
 * in the framework. Should be deprecated.
 */
void CMIDEnv::SetCanvasZoomSize(const TSize& aSize)
{
    iCanvasZoomSize=aSize;
}

/**
 * Get the zoom size - used by the Canvas in the plugin.
 * Relic from when Canvas was implementd in the framework.
 * Should be deprecated.
 */
TSize CMIDEnv::CanvasZoomSize()
{
    return iCanvasZoomSize;
}

TSize CMIDEnv::CanvasAssumedSize()
{
    return iCanvasAssumedSize;
}

TInt CMIDEnv::NumColors()
{
    TDisplayMode mode = DisplayMode();
    return TDisplayModeUtils::NumDisplayModeColors(mode);
}

TDisplayMode CMIDEnv::DisplayMode()
{
    MMIDGraphicsFactory& factory = iToolkit->GraphicsFactory();
    return factory.DisplayMode();
}

void CMIDEnv::AddObserverL(MMIDEnvObserver& aObserver)
{
    ASSERT(iObservers.Find(&aObserver) == KErrNotFound);
    //
    User::LeaveIfError(iObservers.Append(&aObserver));
}

void CMIDEnv::RemoveObserver(MMIDEnvObserver& aObserver)
{
    TInt index = iObservers.Find(&aObserver);
    if (index != KErrNotFound)
    {
        iObservers.Remove(index);
    }
}

void CMIDEnv::HandleSwitchOnL(TBool aSwitchOn)
{
    for (TInt i = iObservers.Count(); i--;)
        iObservers[i]->HandleSwitchOnL(aSwitchOn);
}

void CMIDEnv::HandleForegroundL(TBool aForeground)
{
#ifdef RD_JAVA_NGA_ENABLED
    if (aForeground)
    {
        iFullOrPartialFg = ETrue;
    }
#endif // RD_JAVA_NGA_ENABLED    

    for (TInt i = iObservers.Count(); i--;)
    {
        iObservers[i]->HandleForegroundL(aForeground);
    }
    if (aForeground && iToolkit && iToolkit->Utils())
    {
        iToolkit->Utils()->HandleForegroundL(aForeground);
    }
}

void CMIDEnv::HandleResourceChangeL(TInt aType)
{
    for (TInt i = iObservers.Count(); i--;)
    {
        iObservers[i]->HandleResourceChangeL(aType);
    }
    if (iToolkit && iToolkit->Utils())
    {
        iToolkit->Utils()->HandleResourceChangedL();
    }
}

TBool CMIDEnv::VideoOverlayEnabled() const
{
    return iVideoOverlayEnabled;
}

RCriticalSection& CMIDEnv::GetMMAPILock()
{
    return iMMAPILock;
}

#ifdef RD_JAVA_NGA_ENABLED
void CMIDEnv::HandleFullOrPartialForegroundL(TBool aForeground)
{
    iFullOrPartialFg = aForeground;
    for (TInt i = iObservers.Count(); i--;)
    {
        iObservers[i]->HandleFullOrPartialForegroundL(aForeground);
    }
}

void CMIDEnv::HandleFreeGraphicsMemory()
{
    for (TInt i = iObservers.Count(); i--;)
    {
        iObservers[i]->HandleFreeGraphicsMemory();
    }
}

TBool CMIDEnv::HasFullOrPartialForeground() const
{
    return iFullOrPartialFg;
}
#endif //RD_JAVA_NGA_ENABLED

TInt CMIDEnv::MidletAttribute(const TDesC& aAttributeName, TPtrC& aAttributeValue)
{
    return iToolkit->MidletAttribute(aAttributeName,aAttributeValue);
}

TBool CMIDEnv::MidletAttributeIsSetToVal(const TDesC& aAttributeName,
        const TDesC& aAttributeValue)
{
    TPtrC attributeValue;

    if (MidletAttribute(aAttributeName, attributeValue) == KErrNone &&
            attributeValue.CompareF(aAttributeValue) == 0)
    {
        return ETrue;
    }
    else
    {
        return EFalse;
    }
}

TBool CMIDEnv::MidletAttributeContainsVal(const TDesC& aAttributeName,
        const TDesC& aAttributeValue)
{
    TPtrC attributeValue;

    if (MidletAttribute(aAttributeName, attributeValue) == KErrNone)
    {
        TInt startIdx = attributeValue.FindF(aAttributeValue);
        if (startIdx != KErrNotFound)
        {
            if (aAttributeName.FindF(LcduiMidletAttributes::KAttribUIEnhancement) == KErrNotFound)
            {
                // If there is a preceding character before the value substring, it must be a comma
                if (startIdx == 0 || attributeValue[ startIdx - 1 ] == KCharComma)
                {
                    // Check what comes after the found value substring.
                    TInt endIdx = startIdx + aAttributeValue.Length();
                    if (endIdx < attributeValue.Length())
                    {
                        // only comma is ok
                        if (attributeValue[ endIdx ] == KCharComma)
                        {
                            return ETrue;
                        }
                    }
                }
                else // the found substring was right at the end of attributeValue string
                {
                    return ETrue;
                }
            }
            else // it is "Nokia-UI-Enhancement" - it is enough just to identify the substring in list of values
            {
                return ETrue;
            }
        }
    }

    return EFalse;
}

void CMIDEnv::SetCanvasAssumedSize(const TSize& aSize)
{
    iCanvasAssumedSize = aSize;
}
CFbsBitmap* CMIDEnv::ReserveCanvasFrameBufferL(MMIDCanvas& aCanvas, const TSize& aSz)
{
    CMIDEnv::TMutexAutoLock lock(&iCanvasData.iLock);
    if (!aCanvas.IsGameCanvas())
    {
        if (!iCanvasData.iFrameBuffer)
        {
            iCanvasData.iFrameBuffer = CreateCanvasFrameBufferL(aCanvas, ETrue, aSz);
        }
        iCanvasData.iRefCount++;
        return iCanvasData.iFrameBuffer;
    }
    return CreateCanvasFrameBufferL(aCanvas, ETrue, aSz);
}

void CMIDEnv::ReleaseCanvasFrameBuffer(MMIDCanvas& aCanvas, CFbsBitmap*& aFrameBuffer)
{
    CMIDEnv::TMutexAutoLock lock(&iCanvasData.iLock);
    if (!aCanvas.IsGameCanvas())
    {
        if (iCanvasData.iFrameBuffer)
        {
            iCanvasData.iRefCount--;
            if (iCanvasData.iRefCount <= 0)
            {
                iCanvasData.Reset();
            }
        }
    }
    else
    {
        delete aFrameBuffer;
        aFrameBuffer = NULL;
    }
}

CFbsBitmap* CMIDEnv::CreateCanvasFrameBufferL(
    MMIDCanvas& /* aCanvas */, TBool aWsBitmap, const TSize& aSz)
{
    CFbsBitmap* bitmap = new(ELeave) CFbsBitmap;
    CleanupStack::PushL(bitmap);
    TInt err = KErrGeneral;
    err = bitmap->CreateHardwareBitmap(aSz, DisplayMode(), MidletUid());
    if (KErrNone != err)
    {
        // Failed to create hardware bitmap. Fall back to normal software only bitmaps
        User::LeaveIfError(bitmap->Create(aSz, DisplayMode()));
    }
    if (!aWsBitmap)
    {
        CleanupStack::Pop();
        return bitmap;
    }
    CWsBitmap* wsBitmap =
        new(ELeave) CWsBitmap(CCoeEnv::Static()->WsSession());
    err = wsBitmap->Duplicate(bitmap->Handle());
    if (KErrNone != err)
    {
        delete wsBitmap;
        User::Leave(err);
    }
    CleanupStack::PopAndDestroy(bitmap);
    return wsBitmap;
}

TBool CMIDEnv::CanvasHasBackground(const MMIDCanvas& /* aCanvas */) const
{
    return iCanvasData.iHasBackground;
}

#ifdef RD_JAVA_NGA_ENABLED
// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
TBool CMIDEnv::IsHardwareAcceleratedL(MMIDEnv::THardwareType aHardwareType)
{
    ASSERT(iHardwareStatus);
    return (iHardwareStatus & aHardwareType);
}

// ---------------------------------------------------------------------------
// Checks from OpenGL ES API, if it is HW accelerated
// ---------------------------------------------------------------------------
//
TInt CMIDEnv::InitHardwareStatusL()
{
    iHardwareStatus = 1;

    EGLDisplay eglDisplay = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    eglInitialize(eglDisplay, NULL, NULL);

    EGLConfig config;
    EGLint numOfConfigs = 0;
    // Choose suitable display config
    const EGLint attribs[] =
    {
        EGL_SURFACE_TYPE, EGL_PBUFFER_BIT,
        EGL_BUFFER_SIZE, 32,
        EGL_DEPTH_SIZE, 16,
        EGL_NONE
    };
    eglChooseConfig(eglDisplay, attribs, &config, 1, &numOfConfigs);
    // Create a surface
    const EGLint pBuffAttribs[] =
    {
        EGL_WIDTH, 1,
        EGL_HEIGHT, 1,
        EGL_NONE
    };
    EGLSurface surface = eglCreatePbufferSurface(eglDisplay, config, pBuffAttribs);
    // Create a context into this thread.
    EGLContext context = eglCreateContext(eglDisplay, config, EGL_NO_CONTEXT, NULL);
    // Try to bind surface to context
    if (context != EGL_NO_CONTEXT && surface != EGL_NO_SURFACE &&
            eglMakeCurrent(eglDisplay, surface, surface, context))
    {
        if (strstr((const char *)glGetString(GL_RENDERER), "HW"))
        {
            iHardwareStatus |= MMIDEnv::EHardware3D;
        }
        eglMakeCurrent(eglDisplay, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
    }

    if (context != EGL_NO_CONTEXT)
    {
        eglDestroyContext(eglDisplay, context);
    }

    if (surface != EGL_NO_SURFACE)
    {
        eglDestroySurface(eglDisplay, surface);
    }

    eglTerminate(eglDisplay);

    return iHardwareStatus;
}

#endif // RD_JAVA_NGA_ENABLED

void CMIDEnv::MappingDataForKey(TKeyEvent& aEvent, TEventCode aType)
{
    iToolkit->Utils()->MappingDataForKey(aEvent, aType);
}

void CMIDEnv::SetLastKeyEvent(const TKeyEvent& aEvent)
{
    iToolkit->Utils()->SetLastKeyEvent(aEvent);
}


// ---------------------------------------------------------------------------
// CMIDEnv::ToLcduiObserver
// Gets an instance of ToLcduiObserver.
// ---------------------------------------------------------------------------
//
MMIDToLcduiObserver& CMIDEnv::ToLcduiObserver()
{
    return *iToLcduiObserver;
}

void CMIDEnv::DisplayableIsDestructed(const MMIDDisplayable* aDisplayable)
{
    // Inform CMIDToolkit about deleting of displayble. This is needed
    // for prevereting of panic during changing to new Displayable
    iToolkit->DisplayableIsDestructed(aDisplayable);
}

const MMIDDisplayable* CMIDEnv::LastFullscreenDisplayable() const
{
    return iToolkit->LastFullscreenDisplayable();
}
