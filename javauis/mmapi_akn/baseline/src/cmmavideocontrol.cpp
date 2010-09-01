/*
* Copyright (c) 2002-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  This class is a VideoControl.
*
*/


//  INCLUDE FILES
#include <jdebug.h>
#include <lcdui.h>
#include <JniEnvWrapper.h>

#include "com_nokia_microedition_media_control_VideoControl.h"

#include "cmmavideocontrol.h"
#include "cmmacameraplayer.h"
#include "mmmaeventposter.h"
#include "cmmacanvasdisplay.h"
#include "cmmaitemdisplay.h"
#include "cmmasnapshotevent.h"
#include "mmmaguiplayer.h"
#include "cmmasnapshot.h"
#include "mmmadisplaywindow.h"
#include "cmmaplayerproperties.h"
#include "cmmadcdisplay.h"
#include "cmmavideocontrol.h"

_LIT(KControlName, "VideoControl");

EXPORT_C CMMAVideoControl::CMMAVideoControl(MMMAGuiPlayer* aGuiPlayer):
        CMMAControl()
{
    iGuiPlayer = aGuiPlayer;
    iIsForeground = 1;

    iDisplay = 0;
}

EXPORT_C CMMAVideoControl::~CMMAVideoControl()
{
    if (iMidEnv)
    {
        // unregister for getting the foreground change event
        DEBUG("CMMAVideoControl::~CMMAVideoControl() : iMidEnv RemoveObserver +");
        iMidEnv->RemoveObserver(*this);
        DEBUG("CMMAVideoControl::~CMMAVideoControl() : iMidEnv RemoveObserver -");
    }
    delete iDisplay;

    // Event server takes event ownership
    if (iEventPoster)
    {
        iEventPoster->PostEvent(iDeleteRefEvent);
    }
    if (iSnapshot)
    {
        iSnapshot->Cancel();
        delete iSnapshot;
    }
}


void CMMAVideoControl::ConstructL(CMMAVideoControl* aControl,
                                  jobject aControlObject,
                                  JNIEnv* aJni,
                                  MMMAEventPoster* aEventPoster,
                                  CMIDToolkit* aToolkit) // mobitv fix
{

    // JNI interface pointer can't be passed to different thread, so
    // it is needed to get valid JNI interface pointer for Event Server thread
    aJni = JniEnvWrapper::GetValidJniRef();

    aControl->iListenerObject = aControlObject;
    aControl->iJni = aJni;
    aControl->iEventPoster = aEventPoster;

    // snapshot ready method. Created here because jni cannot be used later.
    aControl->iSnapshotReadyMethod = aJni->GetMethodID(
                                         aJni->GetObjectClass(aControlObject),
                                         "snapshotReady",
                                         "(I[B)V");

    aControl->iSnapshot = CMMASnapshot::NewL(aControl->iGuiPlayer,
                          *aControl);
    aControl->RegisterForegroundListenerL(aToolkit);
}

// mobitv fix
void CMMAVideoControl::RegisterForegroundListenerL(CMIDToolkit* aToolkit)
{
    DEBUG("CMMAVideoControl::RegisterForegroundListener + ");

    // Initialize IsForeground to true, we might have already missed the event (HandleForegroundL), events before observer registration
    iIsForeground = ETrue;


    iToolkit = aToolkit;

    if (iToolkit)
    {
        DEBUG("CMMAVideoControl::RegisterForegroundListenerL : iToolkit->Env(); +");
        iMidEnv = iToolkit->Env();
        DEBUG("CMMAVideoControl::RegisterForegroundListenerL : iToolkit->Env(); -");
    }

    if (iMidEnv)
    {
        DEBUG("CMMAVideoControl::RegisterForegroundListenerL : iMidEnv->AddObserverL + ");
        // register for getting the foreground change event
        iMidEnv->AddObserverL(*this);
        DEBUG("CMMAVideoControl::RegisterForegroundListenerL : iMidEnv->AddObserverL - ");
    }

    DEBUG("CMMAVideoControl::RegisterForegroundListenerL - ");
}
void CMMAVideoControl::StaticInitL(CMMAVideoControl* aControl,
                                   MMIDComponent* aComponent,
                                   TInt* aDisplayHandle,
                                   CMMAEvent* aDeleteRefEvent)
{

    DEBUG("MMA::CMMAVideoControl::StaticInitL +");

    aControl->iDeleteRefEvent = aDeleteRefEvent;

    MMIDComponent::TType componentType = aComponent->Type();
    switch (componentType)
    {
    case MMIDComponent::ECustomItem:
    {
        //MMIDCustomItem
        MMIDCustomItem* customItem =
            reinterpret_cast< MMIDCustomItem* >(aComponent);

        CMMAItemDisplay* itemDisplay =
            CMMAItemDisplay::NewLC(customItem);

        aControl->iDisplay = itemDisplay;
        CleanupStack::Pop(itemDisplay);
        *aDisplayHandle = JavaMakeHandle(itemDisplay);
        break;
    }
    case MMIDComponent::ECanvas:
    {
        //MMIDCanvas
        MMIDCanvas* canvas = reinterpret_cast< MMIDCanvas* >(aComponent);

        CMMACanvasDisplay* canvasDisplay =
            CMMACanvasDisplay::NewLC(canvas);

        aControl->iDisplay = canvasDisplay;
        CleanupStack::Pop(canvasDisplay);
        *aDisplayHandle = JavaMakeHandle(canvasDisplay);
        break;
    }
    default:
    {
        // other component types are not supported
        User::Leave(KErrNotSupported);
    }
    }

    aControl->iGuiPlayer->SetDisplayL(aControl->iDisplay);
    DEBUG("MMA::CMMAVideoControl::StaticInitL - ");
}

void CMMAVideoControl::StaticInitDynamicModeL(
    CMMAVideoControl* aVideoControl,
    TInt* aContentHandle,
    CMMAEventSource* aEventSource,
    jobject aGUIObject,
    CMMAEvent* aDeleteRefEvent)
{
    DEBUG("MMA::CMMAVideoControl::StaticInitDynamicModeL+");
    aVideoControl->iDeleteRefEvent = aDeleteRefEvent;

    CMMADCDisplay* dcDisplay = CMMADCDisplay::NewLC(aVideoControl->iGuiPlayer,
                               aEventSource,
                               aGUIObject);

    MMMADirectContent* content = dcDisplay;
    *aContentHandle = reinterpret_cast< TInt >(content);
    aVideoControl->iDisplay = dcDisplay;
    aVideoControl->iGuiPlayer->SetDisplayL(aVideoControl->iDisplay);
    CleanupStack::Pop(dcDisplay);
    DEBUG("MMA::CMMAVideoControl::StaticInitDynamicModeL-");
}

void CMMAVideoControl::StaticGetPropertyL(CMMAVideoControl* aControl,
        TInt aPropertyType,
        TInt* aReturnValue)
{
    DEBUG_INT("MMA::CMMAVideoControl::StaticGetPropertyL property %d",
              aPropertyType);

    MMMADisplay* display = aControl->iDisplay;

    if (aPropertyType != com_nokia_microedition_media_control_VideoControl_PROPERTY_SOURCE_WIDTH &&
            aPropertyType != com_nokia_microedition_media_control_VideoControl_PROPERTY_SOURCE_HEIGHT &&
            !display)
    {
        DEBUG("MMA::CMMAVideoControl::StaticGetPropertyL not initialized yet");
        // init is not done yet, returning 0
        *aReturnValue = 0;
        return;
    }

    switch (aPropertyType)
    {
    case com_nokia_microedition_media_control_VideoControl_PROPERTY_DISPLAY_WIDTH:
    {
        *aReturnValue = display->DisplaySize().iWidth;
        break;
    }
    case com_nokia_microedition_media_control_VideoControl_PROPERTY_DISPLAY_HEIGHT:
    {
        *aReturnValue = display->DisplaySize().iHeight;
        break;
    }
    case com_nokia_microedition_media_control_VideoControl_PROPERTY_DISPLAY_X:
    {
        *aReturnValue = display->DisplayLocation().iX;
        break;
    }
    case com_nokia_microedition_media_control_VideoControl_PROPERTY_DISPLAY_Y:
    {
        *aReturnValue = display->DisplayLocation().iY;
        break;
    }
    case com_nokia_microedition_media_control_VideoControl_PROPERTY_SOURCE_WIDTH:
    {
        DEBUG("MMA::CMMAVideoControl::StaticGetPropertyL get source width");
        *aReturnValue = aControl->iGuiPlayer->SourceSize().iWidth;
        break;
    }
    case com_nokia_microedition_media_control_VideoControl_PROPERTY_SOURCE_HEIGHT:
    {
        DEBUG("MMA::CMMAVideoControl::StaticGetPropertyL get source height");
        *aReturnValue = aControl->iGuiPlayer->SourceSize().iHeight;
        break;
    }
    default:
    {
        *aReturnValue = KErrNotFound;
        User::Leave(KErrNotFound);
        break;
    }
    }
}


void CMMAVideoControl::StaticSetPropertyL(CMMAVideoControl* aControl,
        TInt aPropertyType,
        TInt aPropertyA,
        TInt aPropertyB)
{
    DEBUG_INT("MMA::CMMAVideoControl::StaticSetPropertyL property type %d",
              aPropertyType);
    DEBUG_INT("MMA::CMMAVideoControl::StaticSetPropertyL a property %d",
              aPropertyA);
    DEBUG_INT("MMA::CMMAVideoControl::StaticSetPropertyL b property %d",
              aPropertyB);

    MMMADisplay* display = aControl->iDisplay;

    __ASSERT_DEBUG(display != NULL,
                   User::Panic(_L("display not initialized"), KErrNotReady));

    switch (aPropertyType)
    {
    case com_nokia_microedition_media_control_VideoControl_SET_DISPLAY_SIZE:
    {
        TSize displaySize(aPropertyA, aPropertyB);
        display->SetDisplaySizeL(displaySize);

        // inform java side
        if (!display->IsFullScreen())
        {
            aControl->iGuiPlayer->NotifyWithStringEvent(
                CMMAPlayerEvent::ESizeChanged, KControlName);
        }
        break;
    }
    case com_nokia_microedition_media_control_VideoControl_SET_DISPLAY_LOCATION:
    {
        TPoint displayLocation(aPropertyA, aPropertyB);
        display->SetDisplayLocationL(displayLocation);
        break;
    }
    case com_nokia_microedition_media_control_VideoControl_SET_DISPLAY_VISIBLE_TRUE:
    {
        display->SetVisible(ETrue);
        break;
    }
    case com_nokia_microedition_media_control_VideoControl_SET_DISPLAY_VISIBLE_FALSE:
    {
        display->SetVisible(EFalse);
        break;
    }
    case com_nokia_microedition_media_control_VideoControl_SET_DISPLAY_FULLSCREEN_TRUE:
    {
        // store old user rect to determine if SIZE_CHANGED event
        // has to be delivered when full screen mode is turned off.
        aControl->iOldDisplaySize = display->DisplaySize();

        display->SetFullScreenL(ETrue);
        break;
    }
    case com_nokia_microedition_media_control_VideoControl_SET_DISPLAY_FULLSCREEN_FALSE:
    {
        display->SetFullScreenL(EFalse);

        // Send SIZE_CHANGED event when fullscreen is turned off if
        // size of the video display has changed. Possible position
        // change is however disregarded
        if (aControl->iOldDisplaySize != display->DisplaySize())
        {
            aControl->iGuiPlayer->NotifyWithStringEvent(
                CMMAPlayerEvent::ESizeChanged, KControlName);
        }

        break;
    }
    default:
    {
        User::Leave(KErrNotFound);
        break;
    }
    }
}


void CMMAVideoControl::TakeSnapShotL(CMMAVideoControl* aControl,
                                     const TDesC* aProperties)
{
    __ASSERT_DEBUG(
        aControl,
        User::Panic(_L("CMMAVideoControl::TakeSnapShotL CMMAVideoControl is NULL"),
                    KErrArgument)
    );

    ASSERT(aProperties);
    aControl->iSnapshot->TakeSnapShotL(*aProperties);
}

EXPORT_C void CMMAVideoControl::SnapshotReady()
{
    DEBUG("MMA::CMMAVideoControl::SnapshotReady()");

    // now this class owns the buffer
    HBufC8* imageBuffer = iSnapshot->ImageBuffer();
    // Use standard new to avoid useless trap.
    CMMASnapshotEvent* event =
        new CMMASnapshotEvent(iListenerObject,
                              iSnapshotReadyMethod,
                              iSnapshot->iStatus.Int(),
                              imageBuffer,
                              CMMAEvent::EDisposableEvent);
    if (event)
    {
        // ok, ownership of buffer transfered to event

        // event poster takes the ownership of the event
        iEventPoster->PostEvent(event, CJavaEventBase::ENotifyPriority);
    }
    else
    {
        // we own the buffer
        delete imageBuffer;
    }
}

EXPORT_C const TDesC& CMMAVideoControl::ClassName() const
{
    return KControlName;
}
TBool CMMAVideoControl::IsForeground()
{
    DEBUG_INT("CMMAVideoControl::IsForeground() %d",iIsForeground);

    return iIsForeground;
}

void CMMAVideoControl::StaticSetForegroundL(CMMAVideoControl* aControl,
        TInt aForeground)
{
    DEBUG_INT("MMA::CMMAVideoControl::StaticSetForegroundL + aForeground %d",
              aForeground);

    __ASSERT_DEBUG(
        aControl,
        User::Panic(_L("MMA::CMMAVideoControl::StaticSetForegroundL : CMMAVideoControl is NULL"),
                    KErrArgument)
    );

    if (aControl)
    {
        aControl->SetForeground(aForeground, EFalse);
    }

    DEBUG("MMA::CMMAVideoControl::StaticSetForegroundL - ");
}
// Implement MMIDEnvObserver

EXPORT_C void CMMAVideoControl::HandleSwitchOnL(TBool /*aSwitchOn*/)
{
    // Dummy implementation, no intent to do anything
}

/**
 * Handles the case when the MIDlet is brought to the foreground.
 */
EXPORT_C void CMMAVideoControl::HandleForegroundL(TBool aForeground)
{

    SetForeground(aForeground, ETrue);

}
void CMMAVideoControl::SetForeground(TBool aForeground, TBool aUseEventServer)
{
    iIsForeground = aForeground;
    if (iDisplay)
    {
        iDisplay->SetForeground(iIsForeground, aUseEventServer);
    }
}

/**
 * Handles a change to resources which are shared accross the environment.
 */
EXPORT_C void CMMAVideoControl::HandleResourceChangeL(TInt /*aType*/)
{
    // Dummy implementation, no intent to do anything
}
//  END OF FILE
