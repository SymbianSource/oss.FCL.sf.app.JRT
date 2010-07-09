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

import org.eclipse.ercp.swt.mobile.MobileShell;
import org.eclipse.swt.graphics.Rectangle;
import org.eclipse.swt.widgets.Display;
import org.eclipse.swt.widgets.Internal_PackageSupport;
import org.eclipse.swt.widgets.Shell;
import com.nokia.mj.impl.utils.Logger;

public class MMAeSWTDisplay extends BaseDisplay
{
    private int iEventSourceHandle;
    private int qwidgetHandle;
    private int x;
    private int y;
    private int eSWTcontrolLocationX;
    private int eSWTcontrolLocationY;

    // index 0 : x-coordinate of topleft corner of display
    // index 1 : y-coordinate of topleft corner of display
    // index 3 : width of display
    // index 4 : height of display

    private int[] displayboundarr = new int[ 4 ];

    //private org.eclipse.swt.widgets.Control iControl;
    private MMAPIeSWTObserver eswtObserver;

    public MMAeSWTDisplay(int aEventServerHandle, org.eclipse.swt.widgets.Control aControl)
    {
        iEventSourceHandle = aEventServerHandle;
        iControl = aControl;
        eswtObserver = new MMAPIeSWTObserver();
        iseSWT = true;
    }

    private void addListeners()
    {
        eswtObserver.addControlListenerToControl(iControl);
        eswtObserver.addShellListenerToControl(iControl);
        eswtObserver.addDisposeListenerToControl(iControl);
    }

    /**
     * Inherited from BaseDisplay.
     */
    /*public void GetCallbackInUiThread(int placeholder)
    {
        Logger.LOG(Logger.EJavaMMAPI,Logger.EInfo,"inside BaseDisplay : GetCallbackInUiThread +");
        final int val = placeholder;
        try{
            Logger.LOG(Logger.EJavaMMAPI,Logger.EInfo,"MMAPI: BaseDisplay.java: GetCallbackInUiThread(): before Display.getDefault()");
            final org.eclipse.swt.widgets.Display disp = org.eclipse.swt.widgets.Display.getDefault();
            if(disp == null)
            {
                Logger.LOG(Logger.EJavaMMAPI,Logger.EInfo,"MMAPI: BaseDisplay.java: GetCallbackInUiThread(): disp is null");
            }
            disp.syncExec(new Runnable() {
             public void run() {
                 Logger.LOG(Logger.EJavaMMAPI,Logger.EInfo,"MMAPI: BaseDisplay.java: GetCallbackInUiThread(): Before native call");
                          _nativeMethodCallInUiThread(nativeDisplayHandle , val);
                        Logger.LOG(Logger.EJavaMMAPI,Logger.EInfo,"MMAPI: BaseDisplay.java: GetCallbackInUiThread(): After native call");
                             }
                         });
           }
         catch(Exception e)
         {
            Logger.LOG(Logger.EJavaMMAPI,Logger.EInfo,"inside BaseDisplay : GetCallbackInUiThread....exception is " + e.toString());
         }
        Logger.LOG(Logger.EJavaMMAPI,Logger.EInfo,"inside BaseDisplay : GetCallbackInUiThread -");
    }*/

    public void setNativeHandle(int handle)
    {
        nativeDisplayHandle = handle;
        //eswtObserver.setDisplayToObserver(this);
        //addListeners();
    }

    public void getBoundRect()
    {
        Logger.LOG(Logger.EJavaMMAPI,Logger.EInfo,"MMACanvasDisplay.java :getBoundRect()");

        try
        {
            final org.eclipse.swt.widgets.Display display = org.eclipse.swt.widgets.Display.getDefault();
            display.syncExec(new Runnable()
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

    public int getDisplayHeight()
    {
        try
        {
            final org.eclipse.swt.widgets.Display display = org.eclipse.swt.widgets.Display.getDefault();

            display.syncExec(new Runnable()
            {
                public void run()
                {
                    y = iControl.getSize().y;

                }
            });
        }
        catch (Exception e)
        {
            Logger.LOG(Logger.EJavaMMAPI,Logger.EInfo,"MMAPI: MMAeSWTDisplay.java: getDisplayHeight() ....exception is "
                       + e.toString());
        }

        return y;
    }

    public int getDisplayWidth()
    {
        try
        {
            final org.eclipse.swt.widgets.Display display = org.eclipse.swt.widgets.Display.getDefault();

            display.syncExec(new Runnable()
            {

                public void run()
                {
                    x = iControl.getSize().x;
                }
            });
        }
        catch (Exception e)
        {
            Logger.LOG(Logger.EJavaMMAPI,Logger.EInfo,"MMAPI: MMAeSWTDisplay.java: getDisplayWidth() ....exception is "
                       + e.toString());
        }
        return x;
    }

    public int getDisplayX()
    {

        try
        {
            final org.eclipse.swt.widgets.Display display = org.eclipse.swt.widgets.Display.getDefault();
            display.syncExec(new Runnable()
            {
                public void run()
                {
                    x = iControl.getLocation().x;

                }
            });
        }
        catch (Exception e)
        {
            Logger.LOG(Logger.EJavaMMAPI,Logger.EInfo,"inside MMAeSWTDisplay.java's getDisplayX....exception is " + e.toString());
        }
        return x;
    }

    protected int getDisplayY()
    {
        try
        {
            final org.eclipse.swt.widgets.Display display = org.eclipse.swt.widgets.Display.getDefault();
            display.syncExec(new Runnable()
            {
                public void run()
                {
                    y = iControl.getLocation().y;

                }
            });
        }
        catch (Exception e)
        {
            Logger.LOG(Logger.EJavaMMAPI,Logger.EInfo,"inside MMAeSWTDisplay.java's getDisplayY....exception is " + e.toString());
        }
        return y;
    }

    public int getSourceHeight()
    {
        // TODO Auto-generated method stub
        return 0;
    }

    public int getSourceWidth()
    {
        // TODO Auto-generated method stub
        return 0;
    }

    public void setDisplayFullScreen(final boolean aFullScreenMode)
    {
        try
        {

            final org.eclipse.swt.widgets.Display display = org.eclipse.swt.widgets.Display.getDefault();

            //display.syncExec(new Runnable() {
            //public void run() {
            iControl.setBounds(display.getClientArea());
            System.out.println("enetr in to the full screen mode setDisplayFullScreen %d"+iControl.getBounds());
//                  new MobileShell(display).setFullScreenMode(aFullScreenMode);
            //((MobileShell)iControl.getShell()).setFullScreenMode(aFullScreenMode);
            //  new Shell(display).setFullScreenMode(aFullScreenMode);
            //((Shell)iControl.getShell()).setFullScreenMode(aFullScreenMode);
            //TODO
            // instruct native to switch to full screen mode
            //_setFullScreenMode(nativeDisplayHandle, aFullScreenMode);
            //}
            //});
        }
        catch (Exception e)
        {
            System.out
            .println("inside MMAeSWTDisplay.java: setDisplayFullScreen....exception is "
                     + e.toString());
            e.printStackTrace();
        }


    }

    public void setDisplayLocation(int aX, int aY)
    {

        //Not required for eSWT since this function does not work for USE_GUI_PRIMITIVE

        //Logger.LOG(Logger.EJavaMMAPI,Logger.EInfo,"MMAPI: MMAeSWTDisplay.java: setDisplayLocation() +");
        //final int videoControlLocationX = aX;
        //final int videoControlLocationY = aY;

        //try {
        //  final org.eclipse.swt.widgets.Display display = org.eclipse.swt.widgets.Display.getDefault();
        //  display.syncExec(new Runnable() {
        //      public void run() {
        ///         org.eclipse.swt.graphics.Point topleftposition = iControl
        //                  .getLocation();
        //          eSWTcontrolLocationX = topleftposition.x;
        //          eSWTcontrolLocationY = topleftposition.y;
        //          Logger.LOG(Logger.EJavaMMAPI,Logger.EInfo,"MMAPI: MMAeSWTDisplay.java: setDisplayLocation(): eSWTcontrolLocationX="+eSWTcontrolLocationX +
        //                  " eSWTcontrolLocationY=" + eSWTcontrolLocationY);
        //  }
        //});
        //} catch (Exception e) {
        //  System..println("inside MMAeSWTDisplay's setDisplayLocation....exception is "
        //+ e.toString());
        //}
        // To Avoid UI thread block
        //System.out
        //      .println("inside MMAeSWTDisplay's setDisplayLocation before calling _setPosition ");
        //_setPosition(nativeDisplayHandle, iEventSourceHandle,
        //      eSWTcontrolLocationX, eSWTcontrolLocationY,
        //      videoControlLocationX, videoControlLocationY);
        //System.out
        //.println("inside MMAeSWTDisplay's setDisplayLocation after calling _setPosition ");

        //try {
        //  final org.eclipse.swt.widgets.Display display = org.eclipse.swt.widgets.Display.getDefault();
        //  display.syncExec(new Runnable() {
        //      public void run() {
        //          //System.out
        //.println("inside MMAeSWTDisplay's setDisplayLocation before redraw() ");
        //iControl.redraw();
        //  System.out
        //.println("inside MMAeSWTDisplay's setDisplayLocation after redraw() ");
        //  }
        //  });
        //} catch (Exception e) {
        //System.out
        //.println("inside MMAeSWTDisplay's setDisplayLocation in redraw()....exception is "
        //+ e.toString());
        //  }

        //Logger.LOG(Logger.EJavaMMAPI,Logger.EInfo,"MMAPI: MMAeSWTDisplay.java: setDisplayLocation() -");



    }

    public void setDisplaySize(int aWidth, int aHeight)
    {
        final int width = aWidth;
        final int height = aHeight;
        try
        {
            final org.eclipse.swt.widgets.Display disp = org.eclipse.swt.widgets.Display.getDefault();

            //disp.syncExec(new Runnable() {
            //  public void run() {
            org.eclipse.swt.graphics.Point size = new org.eclipse.swt.graphics.Point(
                width, height);
            //eswtCanvasControl.setSize(size);
            iControl.setSize(size);
            iControl.redraw();

            System.out
            .println("inside MMAeSWTDisplay.java's setDisplaySize redraw called");
            //}
            //});
        }
        catch (Exception e)
        {
            System.out
            .println("inside MMAeSWTDisplay.java's setDisplaySize....exception is "
                     + e.toString());
        }

        Logger.LOG(Logger.EJavaMMAPI,Logger.EInfo,"inside MMAeSWTDisplay.java's setDisplaySize");
        //_setDisplaySize(nativeDisplayHandle, iEventSourceHandle, aWidth,
        //      aHeight);
        Logger.LOG(Logger.EJavaMMAPI,Logger.EInfo,"inside MMAeSWTDisplay.java's setDisplaySize -");

    }



    public void setVisible(boolean aVisible)
    {
        Logger.LOG(Logger.EJavaMMAPI,Logger.EInfo,"MMAPI: MMAeSWTDisplay.java: setVisible() + " + aVisible);
        try
        {
            final boolean visible = aVisible;
            final org.eclipse.swt.widgets.Display display = org.eclipse.swt.widgets.Display.getDefault();
            display.asyncExec(new Runnable()
            {
                public void run()
                {
                    Logger.LOG(Logger.EJavaMMAPI,Logger.EInfo,"MMAPI: MMAeSWTDisplay.java: setVisible() inside asyncExec run()");
                    //eswtCanvasControl.setVisible(visible);
                    // call native side to set the visibiity
                    _setVisible(nativeDisplayHandle, visible);
                    Logger.LOG(Logger.EJavaMMAPI,Logger.EInfo,"MMAPI: MMAeSWTDisplay.java: _setVisible() native call completed");
                }
            });
        }
        catch (Exception e)
        {
            System.out
            .println("MMAPI: MMAeSWTDisplay.java: setVisible() ....exception is "
                     + e.toString());
        }
        Logger.LOG(Logger.EJavaMMAPI,Logger.EInfo,"MMAPI: MMAeSWTDisplay.java: setVisible() -");
    }

    public void setWindowResources(VideoItem aVideoItem)
    {

        Logger.LOG(Logger.EJavaMMAPI,Logger.EInfo,"MMAeSWTDisplay.java: setWindowResources windowHandle + " + Thread.currentThread().getName());
        try
        {
            final org.eclipse.swt.widgets.Display display = org.eclipse.swt.widgets.Display.getDefault();
            display.syncExec(new Runnable()
            {
                public void run()
                {
                    Logger.LOG(Logger.EJavaMMAPI,Logger.EInfo,"MMAeSWTDisplay.java: setWindowResources() run() runnable = " + this.hashCode());
                    Shell shell = iControl.getShell();
                    qwidgetHandle = Internal_PackageSupport.topHandle(shell);
                    x = iControl.getSize().x;
                    y = iControl.getSize().y;
                    _setWindowToNative(nativeDisplayHandle, qwidgetHandle);
                    Logger.LOG(Logger.EJavaMMAPI,Logger.EInfo,"MMAPI: MMAeSWTDisplay.java: setWindowResources(): calling _setNativeWindowSize() + " + Thread.currentThread().getName());
                    _setNativeWindowSize(nativeDisplayHandle, iEventSourceHandle, x, y);
                    Logger.LOG(Logger.EJavaMMAPI,Logger.EInfo,"MMAeSWTDisplay.java: setWindowResources last statement of syncExec");
                }
            });
        }
        catch (Exception e)
        {
            Logger.LOG(Logger.EJavaMMAPI,Logger.EInfo,"inside MMAeSWTDisplay.java's setWindowResources() ....exception is "
                       + e.toString());
        }
        //Removed from here and added to syncexec above, since setWindowResources is called from new Thread this
        //function is no longer in UI THREAD.

        //Logger.LOG(Logger.EJavaMMAPI,Logger.EInfo,"MMAPI: MMAeSWTDisplay.java: setWindowResources(): calling _setNativeWindowSize() + " + Thread.currentThread().getName() );
        //_setNativeWindowSize(nativeDisplayHandle, iEventSourceHandle, x, y);
        Logger.LOG(Logger.EJavaMMAPI,Logger.EInfo,"MMAPI: MMAeSWTDisplay.java: setWindowResources() - ");
    }

    public void getCallbackSourceSizeChanged(int width, int height)
    {
        Logger.LOG(Logger.EJavaMMAPI,Logger.EInfo,"MMAPI: MMAeSWTDisplay.java: getCallbackSourceSizeChanged() +");
        Logger.LOG(Logger.EJavaMMAPI,Logger.EInfo,"MMAPI: MMAeSWTDisplay.java: getCallbackSourceSizeChanged() width = " + width + " height = " + height);
        iControl.setSize(width, height);
        Logger.LOG(Logger.EJavaMMAPI,Logger.EInfo,"MMAPI: MMAeSWTDisplay.java: getCallbackSourceSizeChanged() +");
    }

    public void setContainerVisibilityToNative(boolean active)
    {
        // TODO Auto-generated method stub

    }

    private native void _setVisible(int nativeDisplayHandle, boolean value);

    private native void _setContainerVisible(int nativeDisplayHandle,
            boolean value);

    private native void _setFullScreenMode(int nativeDisplayHandle,
                                           boolean value);

    private native void _setWindowToNative(int nativeDisplayHandle,
                                           int qwidgetHandle);

    private native void _setNativeWindowSize(int nativeDisplayHandle,
            int iEventSourceHandle, int width, int height);

    private native void _setPosition(int nativeDisplayHandle,
                                     int iEventSourceHandle, int uiControlLocationX,
                                     int uiControlLocationY, int videoControlLocationX,
                                     int videoControlLocationY);

    private native void _setDisplaySize(int nativeDisplayHandle,
                                        int iEventSourceHandle, int eSWTcontrolLocationX,
                                        int eSWTcontrolLocationY);



}