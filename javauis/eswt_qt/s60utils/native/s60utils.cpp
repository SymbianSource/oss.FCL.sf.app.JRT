/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
#include <e32debug.h>
#include <e32base.h>
#include <e32event.h>
#include <eikenv.h>
#include <coemain.h>
#include <e32property.h>
#include <screensaverinternalpskeys.h>
#include <e32keys.h>

//#include <QApplication>

#include "s60utils.h"
// Note following modifilers are from X11 instead of Symbian
static const TInt Shift                           = (1<<0);
static const TInt CapsLock                        = (1<<1);
static const TInt Control                         = (1<<2);
static const TInt Alt                             = (1<<3);
static const TInt NumLock                         = (1<<4);
static const TInt ScrllLock                       = (1<<4);
static const TInt AltGr                           = (1<<7);


static TInt symbianKeyCode(TInt aX11Modifier)
    {
    TInt keyCode(0);
    if( (aX11Modifier & Shift) > 0 )
        {
        keyCode = EStdKeyLeftShift;
        }
    else if( (aX11Modifier & CapsLock) > 0 )
        {
        keyCode = EStdKeyCapsLock;
        }
    else if( (aX11Modifier & Control) > 0 )
        {
        keyCode = EStdKeyLeftCtrl;
        }
    else if( (aX11Modifier & Alt) > 0 )
        {
        keyCode = EStdKeyLeftAlt;
        }
    else if( (aX11Modifier & NumLock) > 0 )
        {
        keyCode = EStdKeyNumLock;
        }
    else if( (aX11Modifier & ScrllLock) > 0 )
        {
        keyCode = EStdKeyScrollLock;
        }
    else if( (aX11Modifier & AltGr) > 0 )
        {
        keyCode = EStdKeyRightAlt;
        }
    return keyCode;
    }

JNIEXPORT jint JNICALL Java_com_nokia_mj_impl_uitestutils_S60Utils_S60SendKeyToFocusWindow
  (JNIEnv *, jclass, jint aKeyCode, jint aModifier, jint aPressOrRelease)
    {
    RDebug::Print(_L("S60UITestUtils : S60SendKeyToFocusWindow, keycode=%d"), aKeyCode);
    
    User::ResetInactivityTime();// Reset user inactivity timers

    if(aModifier > 0 && aPressOrRelease == com_nokia_mj_impl_uitestutils_S60Utils_PRESS)
        {
        TRawEvent mevent;
        TInt keyCode = symbianKeyCode( aModifier);
        mevent.Set(TRawEvent::EKeyDown, keyCode);
        UserSvr::AddEvent(mevent);
        User::After(100000);
        }
    
	TRawEvent event;
	
	if (aPressOrRelease == com_nokia_mj_impl_uitestutils_S60Utils_PRESS)
	    {
	    event.Set(TRawEvent::EKeyDown, aKeyCode);
	    }
	else
	    {
	    event.Set(TRawEvent::EKeyUp, aKeyCode);
	    }
	
	UserSvr::AddEvent(event);
    User::After(100000);
    if(aModifier > 0 && aPressOrRelease == com_nokia_mj_impl_uitestutils_S60Utils_RELEASE)
        {
        TRawEvent mevent;
        TInt keyCode = symbianKeyCode(aModifier);
        mevent.Set(TRawEvent::EKeyUp, keyCode);
        UserSvr::AddEvent(mevent);
        User::After(100000);
        }

	return 0;
    }

JNIEXPORT jint JNICALL Java_com_nokia_mj_impl_uitestutils_S60Utils_S60KeysymToKeycode
  (JNIEnv *, jclass, jint)
    {
    return 0;
    }

JNIEXPORT jint JNICALL Java_com_nokia_mj_impl_uitestutils_S60Utils_S60SendPointerEvent
  (JNIEnv *, jclass, jint aX, jint aY, jint /*aButton*/, jint aPressOrRelease)
    {
    User::ResetInactivityTime();// Reset user inactivity timers
	
	TRawEvent event;
	
	if (aPressOrRelease == com_nokia_mj_impl_uitestutils_S60Utils_PRESS) {
	    event.Set(TRawEvent::EButton1Down, aX, aY);
	} else {
	    event.Set(TRawEvent::EButton1Up, aX, aY);
    }   
	UserSvr::AddEvent(event);
	return 0;
    }

JNIEXPORT jint JNICALL Java_com_nokia_mj_impl_uitestutils_S60Utils_S60SendPointerMoveEvent
  (JNIEnv *, jclass, jint aX, jint aY, jint)
    {
    User::ResetInactivityTime();// Reset user inactivity timers
	TRawEvent event;
	event.Set(TRawEvent::EPointerMove, aX, aY);
	UserSvr::AddEvent(event);
    return 0;
    }

JNIEXPORT void JNICALL Java_com_nokia_mj_impl_uitestutils_S60Utils_S60DisableScreensaver
  (JNIEnv *, jclass)
    {
    RProperty::Set( KPSUidScreenSaver, KScreenSaverAllowScreenSaver, 1 );
    }
    
JNIEXPORT void JNICALL Java_com_nokia_mj_impl_uitestutils_S60Utils_S60SetDoubleclickInterval
  (JNIEnv *, jclass, jint aInterval)
    {
    // Commented out for now. This can be taken into use once Qt doesn't need WSD to be allowed
    /*QApplication* app = static_cast<QApplication*>(QCoreApplication::instance());
    if ( app )
        {
        app->setDoubleClickInterval( aInterval );
        }*/
    }
    