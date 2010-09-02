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
#include <logger.h>
//#include <lcdui.h>

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
    delete iDisplay;

    // Event server takes event ownership
    if (iEventPoster)
    {
        if (iDeleteRefEvent)
        {
            iEventPoster->PostEvent(iDeleteRefEvent);
        }
    }
    if (iSnapshot)
    {
        iSnapshot->Cancel();
        delete iSnapshot;
    }
}


void CMMAVideoControl::ConstructL(CMMAVideoControl* aControl,
                                  jobject aControlObject,
                                  MMAFunctionServer* aEventSource,
                                  JNIEnv* aJni,
                                  MMMAEventPoster* aEventPoster/*,
                                  CMIDToolkit* aToolkit*/) // mobitv fix // 3.x QT based UI
{





    aControl->iListenerObject = aControlObject;
    aControl->iJni = aEventSource->getValidJniEnv();
    aControl->iEventPoster = aEventPoster;

    // snapshot ready method. Created here because jni cannot be used later.
    aControl->iSnapshotReadyMethod = aJni->GetMethodID(
                                         aJni->GetObjectClass(aControlObject),
                                         "snapshotReady",
                                         "(I[B)V");

    aControl->iSnapshot = CMMASnapshot::NewL(aControl->iGuiPlayer,
                          *aControl);
    // aControl->RegisterForegroundListenerL(aToolkit);   // 3.x QT based UI
}
/*   // 3.x QT based UI
// mobitv fix`
void CMMAVideoControl::RegisterForegroundListenerL(CMIDToolkit* aToolkit)
{
    LOG(EJavaMMAPI,EInfo,"CMMAVideoControl::RegisterForegroundListener + ");

    // Initialize IsForeground to true, we might have already missed the event (HandleForegroundL), events before observer registration
    iIsForeground = ETrue;


    iToolkit = aToolkit;

    if (iToolkit)
    {
        LOG(EJavaMMAPI,EInfo,"CMMAVideoControl::RegisterForegroundListenerL : iToolkit->Env(); +");
        iMidEnv = iToolkit->Env();
        LOG(EJavaMMAPI,EInfo,"CMMAVideoControl::RegisterForegroundListenerL : iToolkit->Env(); -");
    }

    if (iMidEnv)
    {
        LOG(EJavaMMAPI,EInfo,"CMMAVideoControl::RegisterForegroundListenerL : iMidEnv->AddObserverL + ");
        // register for getting the foreground change event
        iMidEnv->AddObserverL(*this);
        LOG(EJavaMMAPI,EInfo,"CMMAVideoControl::RegisterForegroundListenerL : iMidEnv->AddObserverL - ");
    }

    LOG(EJavaMMAPI,EInfo,"CMMAVideoControl::RegisterForegroundListenerL - ");
}
*/
void CMMAVideoControl::StaticInitL(CMMAVideoControl* aControl,
                                   jobject javaDisplayRef,
                                   MMAFunctionServer* aEventSource,
                                   TInt* aDisplayHandle,
                                   CMMAEvent* aDeleteRefEvent,
                                   TInt aGuiType)
{

    LOG(EJavaMMAPI, EInfo, "MMA::CMMAVideoControl::StaticInitL +");

    if (aGuiType == CMMAVideoControl::EDISPLAY_VIDEOITEM)
    {
        LOG(EJavaMMAPI, EInfo, "MMA::CMMAVideoControl::StaticInitL - aGuiType is customitem ");
        aControl->iDeleteRefEvent = aDeleteRefEvent;

        CMMAItemDisplay* itemDisplay =
            CMMAItemDisplay::NewLC(aEventSource,javaDisplayRef);
        aControl->iDisplay = itemDisplay;
        CleanupStack::Pop(itemDisplay);
        *aDisplayHandle = reinterpret_cast<TInt>(itemDisplay);
    }
    else if (aGuiType == CMMAVideoControl::EDISPLAY_CANVAS)
    {
        LOG(EJavaMMAPI, EInfo, "MMA::CMMAVideoControl::StaticInitL - aGuiType is canvas ");
        aControl->iDeleteRefEvent = aDeleteRefEvent;

        CMMACanvasDisplay* canvasDisplay =
            CMMACanvasDisplay::NewLC(aEventSource,javaDisplayRef);
        aControl->iDisplay = canvasDisplay;
        CleanupStack::Pop(canvasDisplay);
        *aDisplayHandle = reinterpret_cast<TInt>(canvasDisplay);
    }

    /*    MMIDComponent::TType componentType = aComponent->Type();
        switch (componentType)
        {
        case MMIDComponent::ECustomItem:
        {
            //MMIDCustomItem  // end of // 3.x QT based UI
           // MMIDCustomItem* customItem =
             //   reinterpret_cast< MMIDCustomItem* >(aComponent);

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

        }*/

    aControl->iGuiPlayer->SetDisplayL(aControl->iDisplay);
    LOG(EJavaMMAPI, EInfo, "MMA::CMMAVideoControl::StaticInitL - ");
}


void CMMAVideoControl::SetDisplayHandleToJavaPeer(MMAFunctionServer* eventSource ,jobject aJavaVideoControlPeer)
{
    JNIEnv* validJNI = eventSource->getValidJniEnv();

    jmethodID jmid = validJNI->GetMethodID(validJNI->GetObjectClass(aJavaVideoControlPeer),
                                           "setNativeDisplayHandleToJavaPeer",
                                           "(I)V");
    // DEBUG_INT("CMMADisplay::GetCallbackInUiThread getCallBackMethodID = %d",getCallBackMethodID);
    TInt handle = reinterpret_cast<TInt>(iDisplay);
    validJNI->CallVoidMethod(aJavaVideoControlPeer,jmid,handle);
    TRAP_IGNORE(iGuiPlayer->SetDisplayL(iDisplay));
}

void CMMAVideoControl::StaticInitDynamicModeL(
    CMMAVideoControl* aVideoControl,
    JNIEnv* aJni,
    TInt* aContentHandle,
    MMAFunctionServer* aEventSource,
    jobject aGUIObject,
    CMMAEvent* aDeleteRefEvent)
{
    LOG(EJavaMMAPI,EInfo,"MMA::CMMAVideoControl::StaticInitDynamicModeL+");
    aVideoControl->iDeleteRefEvent = aDeleteRefEvent;

    CMMADCDisplay* dcDisplay = CMMADCDisplay::NewLC(aVideoControl->iGuiPlayer,
                               aEventSource,
                               aJni,
                               aGUIObject);

    //MMMADirectContent* content = dcDisplay;
    *aContentHandle = reinterpret_cast< TInt >(dcDisplay);
    aVideoControl->iDisplay = dcDisplay;
    aVideoControl->iGuiPlayer->SetDisplayL(aVideoControl->iDisplay);
    CleanupStack::Pop(dcDisplay);
    LOG(EJavaMMAPI, EInfo, "MMA::CMMAVideoControl::StaticInitDynamicModeL-");
}

void CMMAVideoControl::StaticGetPropertyL(CMMAVideoControl* aControl,
        TInt aPropertyType,
        TInt* aReturnValue)
{
    LOG1(EJavaMMAPI, EInfo, "MMA::CMMAVideoControl::StaticGetPropertyL property %d",
         aPropertyType);

// MMAPI UI 3.x req.
// remove the below return and uncomment the below code once  display is implemented in java
//    return;


    MMMADisplay* display = aControl->iDisplay;

    if (aPropertyType != com_nokia_microedition_media_control_VideoControl_PROPERTY_SOURCE_WIDTH &&
            aPropertyType != com_nokia_microedition_media_control_VideoControl_PROPERTY_SOURCE_HEIGHT &&
            !display)
    {
        LOG(EJavaMMAPI, EInfo, "MMA::CMMAVideoControl::StaticGetPropertyL not initialized yet");
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
        LOG1(EJavaMMAPI, EInfo, "MMA::CMMAVideoControl::StaticGetPropertyL aReturnValue %d",
             *aReturnValue);
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
        LOG(EJavaMMAPI, EInfo, "MMA::CMMAVideoControl::StaticGetPropertyL get source width");
        *aReturnValue = aControl->iGuiPlayer->SourceSize().iWidth;
        break;
    }
    case com_nokia_microedition_media_control_VideoControl_PROPERTY_SOURCE_HEIGHT:
    {
        LOG(EJavaMMAPI, EInfo, "MMA::CMMAVideoControl::StaticGetPropertyL get source height");
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
    //LOG1( EJavaMMAPI, EInfo, "MMA::CMMAVideoControl::StaticSetPropertyL property type %d",
    //          aPropertyType);
    //LOG1( EJavaMMAPI, EInfo, "MMA::CMMAVideoControl::StaticSetPropertyL a property %d",
    //         aPropertyA);
    // LOG1( EJavaMMAPI, EInfo, "MMA::CMMAVideoControl::StaticSetPropertyL b property %d",
    //          aPropertyB);
// MMAPI UI 3.x req.
// remove the below return and uncomment the below code; once  display is implemented in java
    // return;


    MMMADisplay* display = aControl->iDisplay;

    __ASSERT_DEBUG(display != NULL,
                   User::Panic(_L("display not initialized"), KErrNotReady));

    switch (aPropertyType)
    {
    case com_nokia_microedition_media_control_VideoControl_SET_DISPLAY_SIZE:
    {
        TSize displaySize(aPropertyA, aPropertyB);
        LOG1(EJavaMMAPI, EInfo, "CMMAVideoControl::statepropertyl %d",displaySize);
        display->SetDisplaySizeL(displaySize);
        LOG(EJavaMMAPI, EInfo, "CMMAVideoControl::SetDisplayProperty:: setDisplaySizeL Completed");
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
        LOG1(EJavaMMAPI, EInfo, "MMA::CMMAVideoControl::StaticSetPropertyL a property %d",
             aPropertyA);
        LOG1(EJavaMMAPI, EInfo, "MMA::CMMAVideoControl::StaticSetPropertyL b property %d",
             aPropertyB);
        TSize fullscreenSize(aPropertyA, aPropertyB);
        display->SetFullscreenSize(fullscreenSize);
        //display->SetDisplaySizeL(fullsclreenSize);
        display->SetFullScreenL(ETrue);
        //display->SourceSizeChanged(aPropertyA, aPropertyB,0,0,TRect(0,0,0,0));
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
    LOG(EJavaMMAPI, EInfo, "MMA::CMMAVideoControl::SnapshotReady()");

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
        iEventPoster->PostEvent(event, CMMAEvent::ENotifyPriority);
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
    LOG1(EJavaMMAPI, EInfo, "CMMAVideoControl::IsForeground() %d",iIsForeground);

    return iIsForeground;
}

void CMMAVideoControl::StaticSetForegroundL(CMMAVideoControl* aControl,
        TInt aForeground)
{
    LOG1(EJavaMMAPI, EInfo, "MMA::CMMAVideoControl::StaticSetForegroundL + aForeground %d",
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

    LOG(EJavaMMAPI, EInfo, "MMA::CMMAVideoControl::StaticSetForegroundL - ");
}

/*   // 3.x QT based UI
// Implement MMIDEnvObserver

void CMMAVideoControl::HandleSwitchOnL(TBool aSwitchOn)
{
    // Dummy implementation, no intent to do anything
}
*/
/**
 * Handles the case when the MIDlet is brought to the foreground.
 */

/*
void CMMAVideoControl::HandleForegroundL(TBool aForeground)
{

    SetForeground(aForeground, ETrue);

}

*/
void CMMAVideoControl::SetForeground(TBool aForeground, TBool aUseEventServer)
{
    iIsForeground = aForeground;
    if (iDisplay)
    {
        iDisplay->SetForeground(iIsForeground, aUseEventServer);
    }
}

/*
MMMAGuiPlayer* CMMAVideoControl::GuiPlayer()
{
    return iGuiPlayer;
}
*/
/**
 * Handles a change to resources which are shared accross the environment.
 */
/*
void CMMAVideoControl::HandleResourceChangeL(TInt aType)
{
    // Dummy implementation, no intent to do anything
}
*/  // end of // 3.x QT based UI
//  END OF FILE
