/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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


package com.nokia.microedition.media.control;

import org.eclipse.swt.*;
import org.eclipse.swt.widgets.*;
import javax.microedition.lcdui.*;
import org.eclipse.ercp.swt.mobile.MobileShell;
import org.eclipse.swt.graphics.Rectangle;
import com.nokia.microedition.media.control.VideoItem;
import com.nokia.mj.impl.utils.Logger;


public class MMACanvasDisplay extends BaseDisplay
{

    private int iEventSourceHandle;
    private MMAPIeSWTObserver eswtObserver;
    private int x;
    private int y;
    private int qwidgetHandle;
    private int eSWTcontrolLocationX;
    private int eSWTcontrolLocationY;

    // index 0 : x-coordinate of topleft corner of display
    // index 1 : y-coordinate of topleft corner of display
    // index 3 : width of display
    // index 4 : height of display

    private int[] displayboundarr = new int[ 4 ];
    // represents native side of display


    public MMACanvasDisplay(int aEventSourceHandle , javax.microedition.lcdui.Canvas canvas)
    {
        iEventSourceHandle = aEventSourceHandle;
        eswtObserver = new MMAPIeSWTObserver();
        iControl = com.nokia.mj.impl.nokialcdui.LCDUIInvoker.getEswtControl(canvas);
        //super.setESWTControl(eswtCanvasControl);
    }

    private void addListeners()
    {
        eswtObserver.addControlListenerToControl(iControl);
        eswtObserver.addShellListenerToControl(iControl);
        eswtObserver.addDisposeListenerToControl(iControl);
    }

    public void setNativeHandle(int handle)
    {
        Logger.LOG(Logger.EJavaMMAPI,Logger.EInfo,"MMACanvasDisplay.java : setNativeHandle :" + handle);
        nativeDisplayHandle = handle;
        eswtObserver.setDisplayToObserver(this);
        addListeners();
    }

    public void setDisplaySize(int aWidth, int aHeight)
    {
        final int width = aWidth;
        final int height = aHeight;
        try
        {
            final org.eclipse.swt.widgets.Display disp = com.nokia.mj.impl.nokialcdui.LCDUIInvoker.getEswtDisplay();
            disp.syncExec(new Runnable()
            {
                public void run()
                {
                    org.eclipse.swt.graphics.Point size = new org.eclipse.swt.graphics.Point(width,height);
                    iControl.redraw();

                    Logger.LOG(Logger.EJavaMMAPI,Logger.EInfo,"inside videoControl's setDisplaySize redraw called");
                }
            });
        }
        catch (Exception e)
        {
            Logger.LOG(Logger.EJavaMMAPI,Logger.EInfo,"inside videoControl's setDisplaySize....exception is " + e.toString());
        }


        Logger.LOG(Logger.EJavaMMAPI,Logger.EInfo,"inside videoControl's setDisplaySize +");
        _setDisplaySize(nativeDisplayHandle,
                        iEventSourceHandle,
                        aWidth,
                        aHeight);
        Logger.LOG(Logger.EJavaMMAPI,Logger.EInfo,"inside videoControl's setDisplaySize -");
    }

    public void setDisplayFullScreen(final boolean aFullScreenMode)
    {
        try
        {

            final org.eclipse.swt.widgets.Display disp = com.nokia.mj.impl.nokialcdui.LCDUIInvoker.getEswtDisplay();
            disp.syncExec(new Runnable()
            {
                public void run()
                {
                    iControl.setBounds(disp.getClientArea());
                    //new MobileShell(disp).setFullScreenMode(aFullScreenMode);
                    // instruct native to switch to full screen mode
                    //_setFullScreenMode(nativeDisplayHandle,aFullScreenMode);
                }
            });
        }
        catch (Exception e)
        {
            Logger.LOG(Logger.EJavaMMAPI,Logger.EInfo,"inside videoControl's setDisplaySize....exception is " + e.toString());
        }

    }

    public void setDisplayLocation(int aX, int aY)
    {

        final int videoControlLocationX = aX;
        final int videoControlLocationY = aY;

        try
        {
            final org.eclipse.swt.widgets.Display disp = com.nokia.mj.impl.nokialcdui.LCDUIInvoker.getEswtDisplay();
            disp.syncExec(new Runnable()
            {
                public void run()
                {
                    org.eclipse.swt.graphics.Point topleftposition = iControl.getLocation();
                    eSWTcontrolLocationX = topleftposition.x;
                    eSWTcontrolLocationY = topleftposition.y;

                }
            });
        }
        catch (Exception e)
        {
            Logger.LOG(Logger.EJavaMMAPI,Logger.EInfo,"inside videoControl's setDisplaySize....exception is " + e.toString());
        }
        // To Avoid UI thread block
        Logger.LOG(Logger.EJavaMMAPI,Logger.EInfo,"inside videoControl's setDisplaySize before calling _setPosition ");
        _setPosition(nativeDisplayHandle,
                     iEventSourceHandle,
                     eSWTcontrolLocationX,
                     eSWTcontrolLocationY,
                     videoControlLocationX,
                     videoControlLocationY);
        Logger.LOG(Logger.EJavaMMAPI,Logger.EInfo,"inside videoControl's setDisplaySize after calling _setPosition ");

        try
        {
            final org.eclipse.swt.widgets.Display disp = com.nokia.mj.impl.nokialcdui.LCDUIInvoker.getEswtDisplay();
            disp.syncExec(new Runnable()
            {
                public void run()
                {
                    Logger.LOG(Logger.EJavaMMAPI,Logger.EInfo,"inside videoControl's setDisplaySize before redraw() ");
                    iControl.redraw();
                    Logger.LOG(Logger.EJavaMMAPI,Logger.EInfo,"inside videoControl's setDisplaySize after redraw() ");
                }
            });
        }
        catch (Exception e)
        {
            Logger.LOG(Logger.EJavaMMAPI,Logger.EInfo,"inside videoControl's setDisplayLocation in redraw()....exception is " + e.toString());
        }


        Logger.LOG(Logger.EJavaMMAPI,Logger.EInfo,"inside videoControl's setDisplayLocation coming out of setDisplayLocation()");
    }


    public void setVisible(boolean aVisible)
    {
        try
        {
            final boolean visible = aVisible;
            final org.eclipse.swt.widgets.Display disp = com.nokia.mj.impl.nokialcdui.LCDUIInvoker.getEswtDisplay();
            disp.asyncExec(new Runnable()
            {
                public void run()
                {
                    //eswtCanvasControl.setVisible(visible);
                    // call native side to set the visibiity
                    _setVisible(nativeDisplayHandle,visible);
                }
            });
        }
        catch (Exception e)
        {
            Logger.LOG(Logger.EJavaMMAPI,Logger.EInfo,"inside videoControl's setDisplaySize....exception is " + e.toString());
        }
        //System.out.println("inside videoControl's setVisible before native call");
        // _setVisible(nativeDisplayHandle,aVisible);
        // System.out.println("inside videoControl's setVisible after native call");
    }

    public int getDisplayHeight()
    {


        try
        {
            final org.eclipse.swt.widgets.Display disp = com.nokia.mj.impl.nokialcdui.LCDUIInvoker.getEswtDisplay();
            disp.syncExec(new Runnable()
            {

                public void run()
                {
                    y = iControl.getSize().y;
                }
            });
        }
        catch (Exception e)
        {
            Logger.LOG(Logger.EJavaMMAPI,Logger.EInfo,"inside videoControl's setDisplaySize....exception is " + e.toString());
        }
        return y;

    }

    public int getDisplayWidth()
    {
        try
        {
            final org.eclipse.swt.widgets.Display disp = com.nokia.mj.impl.nokialcdui.LCDUIInvoker.getEswtDisplay();
            disp.syncExec(new Runnable()
            {
                public void run()
                {
                    x = iControl.getSize().x;

                }
            });
        }
        catch (Exception e)
        {
            Logger.LOG(Logger.EJavaMMAPI,Logger.EInfo,"inside videoControl's setDisplaySize....exception is " + e.toString());
        }

        return x;

    }
    public int getDisplayX()
    {
        try
        {
            final org.eclipse.swt.widgets.Display disp = com.nokia.mj.impl.nokialcdui.LCDUIInvoker.getEswtDisplay();
            disp.syncExec(new Runnable()
            {
                public void run()
                {
                    x = iControl.getLocation().x;

                }
            });
        }
        catch (Exception e)
        {
            Logger.LOG(Logger.EJavaMMAPI,Logger.EInfo,"inside videoControl's setDisplaySize....exception is " + e.toString());
        }
        return x;

    }

    public int getDisplayY()
    {



        try
        {
            final org.eclipse.swt.widgets.Display disp = com.nokia.mj.impl.nokialcdui.LCDUIInvoker.getEswtDisplay();
            disp.syncExec(new Runnable()
            {
                public void run()
                {
                    y = iControl.getLocation().y;

                }
            });
        }
        catch (Exception e)
        {
            Logger.LOG(Logger.EJavaMMAPI,Logger.EInfo,"inside videoControl's setDisplaySize....exception is " + e.toString());
        }
        return y;
    }

    public int getSourceWidth()
    {
        // return width of the source video

        return 0;

    }

    public int getSourceHeight()
    {
        // return height of the source video

        return 0;
    }

    public void getBoundRect()
    {
        Logger.LOG(Logger.EJavaMMAPI,Logger.EInfo,"MMACanvasDisplay.java :getBoundRect()");

        try
        {
            final org.eclipse.swt.widgets.Display disp = com.nokia.mj.impl.nokialcdui.LCDUIInvoker.getEswtDisplay();
            disp.syncExec(new Runnable()
            {
                public void run()
                {
                    Rectangle boundrect  = iControl.getBounds();
                    displayboundarr[0] = boundrect.x ;
                    displayboundarr[1] = boundrect.y ;
                    displayboundarr[2] = boundrect.width ;
                    displayboundarr[3] = boundrect.height ;

                }
            });
        }
        catch (Exception e)
        {
            Logger.LOG(Logger.EJavaMMAPI,Logger.EInfo,"inside videoControl's setDisplaySize....exception is " + e.toString());
        }
        Logger.LOG(Logger.EJavaMMAPI,Logger.EInfo,"MMACanvasDisplay.java :getBoundRect() x =" + displayboundarr[0] + "y ="+ displayboundarr[1] +"width ="+ displayboundarr[2] +"height =" + displayboundarr[3]);
    }


    public void getContainerRect()
    {
        // this function is not used for canvas case assuming that in case of canvas Bound rect and container rect is same.
        // to do: need to confirm
        getBoundRect();

    }

    public void setContainerVisibilityToNative(final boolean active)
    {
        Logger.LOG(Logger.EJavaMMAPI,Logger.EInfo,"MMACanvasDisplay.java : SetContainerVisibilityToNative + ");
        new Thread()
        {
            public void run()
            {
                Logger.LOG(Logger.EJavaMMAPI,Logger.EInfo,"MMACanvasDisplay.java : SetContainerVisibilityToNative execute the native function in new thread ");
                _setContainerVisible(iEventSourceHandle,nativeDisplayHandle,active);
            }
        } .start();

        Logger.LOG(Logger.EJavaMMAPI,Logger.EInfo,"MMACanvasDisplay.java : SetContainerVisibilityToNative - ");
    }

    public void setWindowResources(VideoItem aVideoItem)
    {
        Logger.LOG(Logger.EJavaMMAPI,Logger.EInfo,"MMACanvasDisplay.java: setWindowResources windowHandle ");
        try
        {
            final org.eclipse.swt.widgets.Display disp = com.nokia.mj.impl.nokialcdui.LCDUIInvoker.getEswtDisplay();

            disp.syncExec(new Runnable()
            {
                public void run()
                {
                    Shell shell = iControl.getShell();
                    qwidgetHandle = Internal_PackageSupport.topHandle(shell);
                    x = iControl.getSize().x;
                    y = iControl.getSize().y;
                    _setWindowToNative(nativeDisplayHandle,qwidgetHandle);
                    Logger.LOG(Logger.EJavaMMAPI,Logger.EInfo,"MMACanvasDisplay.java: setWindowResources qwidgetHandle is " + qwidgetHandle);
                }
            });
        }
        catch (Exception e)
        {
            Logger.LOG(Logger.EJavaMMAPI,Logger.EInfo,"inside videoControl's setDisplaySize....exception is " + e.toString());
        }
        _setNativeWindowSize(nativeDisplayHandle,iEventSourceHandle,x,y);

    }

    /*
    // dummy

    public void dummy()
    {
        _nativeDummy();
    }
    */
    /*
        public void getDisplaySize(int width, int height)
        {

            try{
                final org.eclipse.swt.widgets.Display disp = com.nokia.mj.impl.nokialcdui.LCDUIInvoker.getEswtDisplay();
                 disp.syncExec(new Runnable() {
                                 public void run() {
                                      x = eswtCanvasControl.getSize().x;
                                      y = eswtCanvasControl.getSize().y;

                                 }
                    });
                }
           catch(Exception e)
                           {
                            System.out.println("inside videoControl's setDisplaySize....exception is " + e.toString());
                   }

                   width = x;
                   height = y;
        }

        */
    // Native methods


    private native void _setVisible(int nativeDisplayHandle,
                                    boolean value);

    private native void _setContainerVisible(int iEventSourceHandle,
            int nativeDisplayHandle,
            boolean value);
    private native void _setFullScreenMode(int nativeDisplayHandle, boolean value);
    private native void _setWindowToNative(int nativeDisplayHandle,int qwidgetHandle);
    private native void _setNativeWindowSize(int nativeDisplayHandle,int iEventSourceHandle, int width,int height);
    private native void _setPosition(int nativeDisplayHandle,
                                     int iEventSourceHandle,
                                     int uiControlLocationX,
                                     int uiControlLocationY,
                                     int videoControlLocationX,
                                     int videoControlLocationY);
    private native void _setDisplaySize(int nativeDisplayHandle,
                                        int iEventSourceHandle,
                                        int eSWTcontrolLocationX,
                                        int eSWTcontrolLocationY);
}
