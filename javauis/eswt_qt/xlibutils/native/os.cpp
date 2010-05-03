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
// Description: Testing utility for emulating pointer/key input events in X. 

#include <QApplication>
#include <Xlib.h>
#include <new>

#include "os.h"

namespace Java {
    namespace XlibUtils {
        typedef enum
        {
            NO_ERROR        = 0,
            NO_DISPLAY      = 1,
            NO_FOCUSWINDOW  = 2,
            NO_ROOTWINDOW   = 3,
            NO_MEMORY       = 4,
            SENDEVENT_ERR   = 5
        } XlibUtilsErr;
    }
}

JNIEXPORT jint JNICALL Java_com_nokia_mj_impl_uitestutils_XlibUtils_XSendKeyToFocusWindow
  ( JNIEnv*, jclass, jint aKeycode, jint aModifiers, jint aPressOrRelease )
{
    int keycode      = static_cast<int>( aKeycode );
    int modifiers    = static_cast<int>( aModifiers );
    bool press       = ( aPressOrRelease == 0 ? true : false );
    jint retVal      = 0;
    Display* display = NULL;
    
    try
    {
        display = ::XOpenDisplay( NULL ); 
        if( !display )
        {
            throw Java::XlibUtils::NO_DISPLAY;
        }
        
        Window focusWindow  = 0;
        int focusState      = 0;
        ::XGetInputFocus( display,  &focusWindow, &focusState );
        
        Window rootWindow = ::XDefaultRootWindow( display );

        if( focusWindow == None )
        {
            throw Java::XlibUtils::NO_FOCUSWINDOW;
        }
        else if( rootWindow == None )
        {
            throw Java::XlibUtils::NO_ROOTWINDOW;
        }
        else
        {
            XKeyEvent* event = new(std::nothrow) XKeyEvent();
            if( !event )
            {
                throw Java::XlibUtils::NO_MEMORY;
            }
            event->type        = ( press ? KeyPress : KeyRelease );
            event->serial      = None;
            event->send_event  = false;
            event->display     = display;
            event->window      = focusWindow;
            event->root        = rootWindow;
            event->subwindow   = None;
            event->time        = CurrentTime;
            event->x           = 0;
            event->y           = 0;
            event->x_root      = 0;
            event->y_root      = 0;
            event->state       = modifiers;
            event->keycode     = keycode;
            event->same_screen = True;

            if( ::XSendEvent( display,
                              event->window,
                              false,
                              0,
                              reinterpret_cast<XEvent*>(event)) == 0 )
            {
                throw Java::XlibUtils::SENDEVENT_ERR;
            }
            ::XFlush( display );
        }
     }
     catch(Java::XlibUtils::XlibUtilsErr e)
     {
        retVal = static_cast<jint>( e );
     }
    if( retVal != Java::XlibUtils::NO_DISPLAY )
    {
        ::XCloseDisplay( display );
    }
    return retVal;
}

JNIEXPORT jint JNICALL Java_com_nokia_mj_impl_uitestutils_XlibUtils_XKeysymToKeycode
  (JNIEnv*, jclass, jint aKeysym)
    {
    int keysym      = static_cast<int>(aKeysym);
    KeyCode keycode = 0;
    
    Display* display = ::XOpenDisplay( NULL );
    if( display )
    {
        keycode = ::XKeysymToKeycode( display, keysym );
        ::XCloseDisplay( display );
    }
    return keycode;
    }

JNIEXPORT jint JNICALL Java_com_nokia_mj_impl_uitestutils_XlibUtils_XSendPointerEvent
  (JNIEnv*, jclass, jint aX, jint aY, jint aButton, jint aPressOrRelease)
    {
    bool press       = ( aPressOrRelease == 0 ? true : false );
    int x            = static_cast<int>(aX);
    int y            = static_cast<int>(aY);
    jint retVal      = 0;
    Display* display = NULL;
    int buttonNMask  = 1 << (7 + aButton); // Button1Mask, Button2Mask, ..., Button5Mask
    
    try
    {
        display = ::XOpenDisplay( NULL ); 
        if( !display )
        {
            throw Java::XlibUtils::NO_DISPLAY;
        }
        
        Window rootWindow = ::XDefaultRootWindow( display );
        if( rootWindow == None )
        {
            throw Java::XlibUtils::NO_ROOTWINDOW;
        }

        // Release events don't seem to work if we do pointer warp. 
        // Pointer is warped only for press events. Releases use the
        // current cursor position to determine the window where to 
        // send the event to. 
        if( press )
        {
            ::XWarpPointer( display, None, rootWindow, 0, 0, 0, 0, x, y );
        }

        XButtonEvent* event = new(std::nothrow) XButtonEvent();
        if( !event )
        {
            throw Java::XlibUtils::NO_MEMORY;
        }
                
        event->type         = ( press ? ButtonPress : ButtonRelease );
        event->serial       = None;
        event->send_event   = False;
        event->display      = display;
//      event->window       = ;
//      event->root         = ;
//      event->subWindow    = ;
        event->time         = CurrentTime;
//      event->x            = ;
//      event->y            = ;
//      event->x_root       = ;
//      event->y_root       = ;
//      event->state        = ;
        event->button       = aButton;
        event->same_screen  = True;
        
        ::XQueryPointer( display, 
                         rootWindow, 
                         &event->root, 
                         &event->window,
                         &event->x_root, 
                         &event->y_root, 
                         &event->x, 
                         &event->y, 
                         &event->state );
        
        event->subwindow    = event->window;
        event->state        = buttonNMask;
        
        // Find the top window at cursor position. 
        while( event->subwindow )
        {
            event->window = event->subwindow;
            
            ::XQueryPointer( display, 
                             event->window,
                             &event->root, 
                             &event->subwindow,
                             &event->x_root, 
                             &event->y_root,
                             &event->x, 
                             &event->y,
                             &event->state );
        }

        // For release event just adjust the coordinates in the event. 
        // Cursor is actually never moved to the release event location. 
        if( !press )
        {
            int delta_x = x - event->x_root;
            int delta_y = y - event->y_root;
            
            event->x      += delta_x;
            event->y      += delta_y;
            event->x_root += delta_x;
            event->y_root += delta_y;
        }

        // Double click interval is set to zero in QApplication to avoid getting
        // unwanted double-click events. It's common that test cases just keep
        // clicking in the middle of a widget and tests execute in rapidly. 
        QApplication* app = qApp;
        if(app) app->setDoubleClickInterval( 0 );
        
        event->time         = CurrentTime;
        event->state        = (press ? buttonNMask : 0);

        // Unless the window is a tooltip or other window which will be ignored
        // by the window manager, set input focus to make window activate like 
        // clicking with real mouse. 
        XWindowAttributes attr;
        ::memset(&attr, 0, sizeof(attr));
        ::XGetWindowAttributes(display, event->window, &attr);
        if(attr.override_redirect == False) 
        {
            ::XSetInputFocus(display, event->window, RevertToNone, CurrentTime);
        }
        
        if( ::XSendEvent( display, 
                          PointerWindow, 
                          True, 
                          0, 
                          reinterpret_cast<XEvent*>(event) ) == 0)
        {
            throw Java::XlibUtils::SENDEVENT_ERR;
        }

        ::XFlush( display );
     }
     catch(Java::XlibUtils::XlibUtilsErr e)
     {
        retVal = static_cast<jint>( e );
     }
     
    if( retVal != Java::XlibUtils::NO_DISPLAY )
    {
        ::XCloseDisplay( display );
    }
      
    return retVal;
    }


JNIEXPORT jint JNICALL Java_com_nokia_mj_impl_uitestutils_XlibUtils_XSendPointerMoveEvent
  (JNIEnv *, jclass, jint aX, jint aY, jint aButton)
    {
    int x            = static_cast<int>(aX);
    int y            = static_cast<int>(aY);
    jint retVal      = 0;
    Display* display = NULL;
    int buttonNMask  = 1 << (7 + aButton); // Button1Mask, Button2Mask, ..., Button5Mask
    
    try
    {
        display = ::XOpenDisplay( NULL ); 
        if( !display )
        {
            throw Java::XlibUtils::NO_DISPLAY;
        }
        
        Window rootWindow = ::XDefaultRootWindow( display );
        if( rootWindow == None )
        {
            throw Java::XlibUtils::NO_ROOTWINDOW;
        }

        XMotionEvent* event = new(std::nothrow) XMotionEvent();
        if( !event )
        {
            throw Java::XlibUtils::NO_MEMORY;
        }
                
        event->type         = MotionNotify;
        event->serial       = None;
        event->send_event   = False;
        event->display      = display;
//      event->window       = ;
//      event->root         = ;
//      event->subWindow    = ;
        event->time         = CurrentTime;
//      event->x            = ;
//      event->y            = ;
//      event->x_root       = ;
//      event->y_root       = ;
//      event->state        = ;
        event->is_hint      = NotifyNormal;
        event->same_screen  = True;
        
        ::XQueryPointer( display, 
                         rootWindow, 
                         &event->root, 
                         &event->window,
                         &event->x_root, 
                         &event->y_root, 
                         &event->x, 
                         &event->y, 
                         &event->state );
        
        event->subwindow    = event->window;
        event->state        = buttonNMask;
        
        // Find the top window at cursor position. 
        while( event->subwindow )
        {
            event->window = event->subwindow;
            
            ::XQueryPointer( display, 
                             event->window,
                             &event->root, 
                             &event->subwindow,
                             &event->x_root, 
                             &event->y_root,
                             &event->x, 
                             &event->y,
                             &event->state );
        }

        // For motion event just adjust the coordinates in the event. 
        // Cursor is actually never moved to the motion event location. 
        int delta_x = x - event->x_root;
        int delta_y = y - event->y_root;

        event->x      += delta_x;
        event->y      += delta_y;
        event->x_root += delta_x;
        event->y_root += delta_y;

        event->state   = buttonNMask;

        if( ::XSendEvent( display, 
                          PointerWindow, 
                          True, 
                          0, 
                          reinterpret_cast<XEvent*>(event) ) == 0)
        {
            throw Java::XlibUtils::SENDEVENT_ERR;
        }

        ::XFlush( display );
     }
     catch(Java::XlibUtils::XlibUtilsErr e)
     {
        retVal = static_cast<jint>( e );
     }
     
    if( retVal != Java::XlibUtils::NO_DISPLAY )
    {
        ::XCloseDisplay( display );
    }
      
    return retVal;
    }

