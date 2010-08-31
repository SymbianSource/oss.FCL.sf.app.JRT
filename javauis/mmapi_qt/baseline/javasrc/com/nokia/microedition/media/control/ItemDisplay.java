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

import javax.microedition.lcdui.Item;
import com.nokia.mj.impl.utils.Logger;
import org.eclipse.swt.*;
import org.eclipse.swt.widgets.*;
import javax.microedition.lcdui.*;
import org.eclipse.ercp.swt.mobile.MobileShell;
import org.eclipse.swt.graphics.Rectangle;
import com.nokia.mj.impl.nokialcdui.ItemControlStateChangeListener;
import com.nokia.microedition.media.control.VideoItem;




public class ItemDisplay extends BaseDisplay implements ItemControlStateChangeListener

{

    private int iEventSourceHandle;

    private MMAPIeSWTObserver iEswtObserver;
    private int x;
    private int y;
    private int qwidgetHandle;
    Rectangle iFormRect;
    private boolean iQWidgetHandleSet = false;

    // index 0 : x-coordinate of topleft corner of display
    // index 1 : y-coordinate of topleft corner of display
    // index 3 : width of display
    // index 4 : height of display

    private int[] displayboundarr = new int[ 4 ];
    // represents native side of display

    private VideoItem iVideoItem = null;




    public ItemDisplay(int aEventSourceHandle , VideoItem aVideoItem)
    {
        Logger.LOG(Logger.EJavaMMAPI,Logger.EInfo,"ItemDisplay.java + ");
        iEventSourceHandle = aEventSourceHandle;

        iVideoItem = aVideoItem;


        iEswtObserver = new MMAPIeSWTObserver();
        //iControl = com.nokia.mj.impl.nokialcdui.LCDUIInvoker.getEswtControl(item);
        com.nokia.mj.impl.nokialcdui.LCDUIInvoker.setItemControlStateChangeListener((ItemControlStateChangeListener)this,(javax.microedition.lcdui.Item) aVideoItem);
        Logger.LOG(Logger.EJavaMMAPI,Logger.EInfo,"ItemDisplay.java - ");


    }

    private void addListeners()
    {
        iEswtObserver.addControlListenerToControl(iControl);
        iEswtObserver.addShellListenerToControl(iControl);
        iEswtObserver.addDisposeListenerToControl(iControl);
    }

    public void setNativeHandle(int handle)
    {
        Logger.LOG(Logger.EJavaMMAPI,Logger.EInfo,"ItemDisplay.java : setNativeHandle :" + handle);
        nativeDisplayHandle = handle;
        //iVideoItem = vitem;

    }

    public void addObservers()
    {
        iEswtObserver.setDisplayToObserver(this);
        addListeners();

    }

    public void setDisplaySize(int aWidth, int aHeight)
    {
        Logger.LOG(Logger.EJavaMMAPI,Logger.EInfo,"ItemDisplay.java: setDisplaySize + ");
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
                    iControl.setSize(size);
                }
            });
        }
        catch (Exception e)
        {
            Logger.LOG(Logger.EJavaMMAPI,Logger.EInfo,"inside videoControl's setDisplaySize....exception is " + e.toString());
            e.printStackTrace();
        }
        Logger.LOG(Logger.EJavaMMAPI,Logger.EInfo,"ItemDisplay.java: setDisplaySize - ");
    }

    public void setDisplayFullScreen(final boolean aFullScreenMode)
    {
        Logger.LOG(Logger.EJavaMMAPI,Logger.EInfo,"ItemDisplay.java: setDisplayFullScreen + ");
        try
        {

            final org.eclipse.swt.widgets.Display disp = com.nokia.mj.impl.nokialcdui.LCDUIInvoker.getEswtDisplay();
            disp.syncExec(new Runnable()
            {
                public void run()
                {
                    iControl.setBounds(disp.getClientArea());
                    new MobileShell(disp).setFullScreenMode(aFullScreenMode);
                    // instruct native to switch to full screen mode
                    _setFullScreenMode(nativeDisplayHandle,aFullScreenMode);
                }
            });
        }
        catch (Exception e)
        {
            Logger.LOG(Logger.EJavaMMAPI,Logger.EInfo,"inside videoControl's setDisplaySize....exception is " + e.toString());
        }
        Logger.LOG(Logger.EJavaMMAPI,Logger.EInfo,"ItemDisplay.java: setDisplayFullScreen - ");

    }

    public void setDisplayLocation(int aX, int aY)
    {

        Logger.LOG(Logger.EJavaMMAPI,Logger.EInfo,"ItemDisplay.java: setDisplayLocation + ");
        final int x = aX;
        final int y = aY;

        try
        {

            final org.eclipse.swt.widgets.Display disp = com.nokia.mj.impl.nokialcdui.LCDUIInvoker.getEswtDisplay();
            disp.syncExec(new Runnable()
            {
                public void run()
                {
                    iControl.setLocation(x , y);
                }
            });
        }
        catch (Exception e)
        {
            Logger.LOG(Logger.EJavaMMAPI,Logger.EInfo,"inside videoControl's setDisplaySize....exception is " + e.toString());
            e.printStackTrace();
        }
        Logger.LOG(Logger.EJavaMMAPI,Logger.EInfo,"ItemDisplay.java: setDisplayLocation - ");

    }


    public void setVisible(boolean aVisible)
    {
        Logger.LOG(Logger.EJavaMMAPI,Logger.EInfo,"ItemDisplay.java: setVisible + ");
        // call native side to set the visibiity
        _setVisible(nativeDisplayHandle,aVisible);
        Logger.LOG(Logger.EJavaMMAPI,Logger.EInfo,"ItemDisplay.java: setVisible after _setVisible ");
        try
        {
            final boolean visible = aVisible;
            final org.eclipse.swt.widgets.Display disp = com.nokia.mj.impl.nokialcdui.LCDUIInvoker.getEswtDisplay();
            disp.syncExec(new Runnable()
            {
                public void run()
                {


                    Logger.LOG(Logger.EJavaMMAPI,Logger.EInfo,"before calling iControl.setVisible");
                    iControl.setVisible(visible);
                }
            });
        }
        catch (Exception e)
        {
            Logger.LOG(Logger.EJavaMMAPI,Logger.EInfo,"inside videoControl's setDisplaySize....exception is " + e.toString());
            e.printStackTrace();
        }
        Logger.LOG(Logger.EJavaMMAPI,Logger.EInfo,"ItemDisplay.java: setVisible - ");
    }

    public int getDisplayHeight()
    {

        Logger.LOG(Logger.EJavaMMAPI,Logger.EInfo,"ItemDisplay.java: getDisplayHeight + ");
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
            e.printStackTrace();
        }
        Logger.LOG(Logger.EJavaMMAPI,Logger.EInfo,"ItemDisplay.java: getDisplayHeight - ");
        return y;

    }

    public int getDisplayWidth()
    {
        Logger.LOG(Logger.EJavaMMAPI,Logger.EInfo,"ItemDisplay.java: getDisplayWidth + ");
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
            e.printStackTrace();
        }
        Logger.LOG(Logger.EJavaMMAPI,Logger.EInfo,"ItemDisplay.java: getDisplayWidth - ");
        return x;

    }
    public int getDisplayX()
    {
        Logger.LOG(Logger.EJavaMMAPI,Logger.EInfo,"ItemDisplay.java: getDisplayX + ");
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
            e.printStackTrace();
        }
        Logger.LOG(Logger.EJavaMMAPI,Logger.EInfo,"ItemDisplay.java: getDisplayX - ");
        return x;

    }

    public int getDisplayY()
    {
        Logger.LOG(Logger.EJavaMMAPI,Logger.EInfo,"ItemDisplay.java: getDisplayY + ");


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
            e.printStackTrace();
        }
        Logger.LOG(Logger.EJavaMMAPI,Logger.EInfo,"ItemDisplay.java: getDisplayY - ");
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
        Logger.LOG(Logger.EJavaMMAPI,Logger.EInfo,"ItemDisplay.java :getBoundRect()");

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

                    Shell shell = iControl.getShell();
                    iFormRect = shell.getBounds();


                    Logger.LOG(Logger.EJavaMMAPI,Logger.EInfo,"MMAitemDisplay.java: form bounds - "+iFormRect.x +","+iFormRect.y+","+iFormRect.width+","+iFormRect.height);

                    iFormRect = shell.getClientArea();
                    Logger.LOG(Logger.EJavaMMAPI,Logger.EInfo,"MMAitemDisplay.java: client area - "+iFormRect.x +","+iFormRect.y+","+iFormRect.width+","+iFormRect.height);

                }
            });


        }
        catch (Exception e)
        {
            Logger.LOG(Logger.EJavaMMAPI,Logger.EInfo,"inside videoControl's setDisplaySize....exception is " + e.toString());
            e.printStackTrace();
        }
        Logger.LOG(Logger.EJavaMMAPI,Logger.EInfo,"ItemDisplay.java :getBoundRect() x =" + displayboundarr[0] + "y ="+ displayboundarr[1] +"width ="+ displayboundarr[2] +"height =" + displayboundarr[3]);
    }


    public void getContainerRect()
    {
        // this function is not used for canvas case assuming that in case of canvas Bound rect and container rect is same.
        // to do: need to confirm
        getBoundRect();

    }

    public void setContainerVisibilityToNative(final boolean active)
    {
        Logger.LOG(Logger.EJavaMMAPI,Logger.EInfo,"ItemDisplay.java : SetContainerVisibilityToNative + ");
        new Thread()
        {
            public void run()
            {
                Logger.LOG(Logger.EJavaMMAPI,Logger.EInfo,"ItemDisplay.java : SetContainerVisibilityToNative execute the native function in new thread ");
                _setContainerVisible(iEventSourceHandle,nativeDisplayHandle,active);
            }
        } .start();
        //_setContainerVisible(nativeDisplayHandle,active);
        Logger.LOG(Logger.EJavaMMAPI,Logger.EInfo,"ItemDisplay.java : SetContainerVisibilityToNative - ");
    }

    public void setWindowResources(VideoItem aVideoItem)
    {
        Logger.LOG(Logger.EJavaMMAPI,Logger.EInfo,"ItemDisplay.java: setWindowResources windowHandle ");

        iQWidgetHandleSet = true;
        if (aVideoItem == null)
            Logger.LOG(Logger.EJavaMMAPI,Logger.EInfo,"ItemDisplay.java: setWindowResources aVideoItem is null");
        // set the item size to be that of the video size if the video size is known.
        int[] size = aVideoItem.getSourceSize();
        Logger.LOG(Logger.EJavaMMAPI,Logger.EInfo,"ItemDisplay.java: after getting sourcesize ");
        setDisplaySize(size[0],size[1]);

        Logger.LOG(Logger.EJavaMMAPI,Logger.EInfo,"ItemDisplay.java: after setting the size of item to source size ");

        if (iControl == null)
        {
            Logger.LOG(Logger.EJavaMMAPI,Logger.EInfo,"+ItemDisplay() -  iControl is null");
        }
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
                    Logger.LOG(Logger.EJavaMMAPI,Logger.EInfo,"ItemDisplay.java: setWindowResources qwidgetHandle is " + qwidgetHandle);
                }
            });
        }
        catch (Exception e)
        {
            Logger.LOG(Logger.EJavaMMAPI,Logger.EInfo,"inside videoControl's setDisplaySize....exception is " + e.toString());
            e.printStackTrace();
        }
        Logger.LOG(Logger.EJavaMMAPI,Logger.EInfo,"ItemDisplay.java: setWindowResources qwidgetHandle(x,y) " +x+"," +y);
        //int width = iVideoItem.getMinContentWidth();
        //int height = iVideoItem.getMinContentHeight();
        //Logger.LOG(Logger.EJavaMMAPI,Logger.EInfo,"ItemDisplay.java: preffered content h(height,width) - (" +height+"," +width+")");
        getBoundRect();
        //_setNativeWindowSize(nativeDisplayHandle,iEventSourceHandle,width,height);
        int formArray[] = new int[4];
        formArray[0] = iFormRect.x;
        formArray[1] = iFormRect.y;
        formArray[2] = iFormRect.width;
        formArray[3] = iFormRect.height;
        _setNativeWindowSize(nativeDisplayHandle,iEventSourceHandle,displayboundarr[2],displayboundarr[3],displayboundarr[0],displayboundarr[1],formArray);
        Logger.LOG(Logger.EJavaMMAPI,Logger.EInfo,"ItemDisplay.java: setWindowResources - ");

    }


    public void setNativeResources(VideoItem aVideoitem)
    {
        Logger.LOG(Logger.EJavaMMAPI,Logger.EInfo,"ItemDisplay.java : nativeResources  + ");
        addObservers();
        setWindowResources(aVideoitem);
        Logger.LOG(Logger.EJavaMMAPI,Logger.EInfo,"ItemDisplay.java : nativeResources  - ");
    }

    /*
    this method is called once the eSWTControl for Item
    is available
    */
    public void notifyControlAvailable(org.eclipse.swt.widgets.Control ctrl,Item item)
    {
        Logger.LOG(Logger.EJavaMMAPI,Logger.EInfo,"ItemDisplay.java : notifyControlAvailable + ");
        iControl = ctrl;
        /* now use the control and addobserver to the control
        and setwindowresources
        */
        if (iQWidgetHandleSet == false)
        {
            NativeResources nativeres = new NativeResources(this,iVideoItem);
            nativeres.start();
        }

        Logger.LOG(Logger.EJavaMMAPI,Logger.EInfo,"ItemDisplay.java : notifyControlAvailable - ");
    }


    public void notifyControlDisposed(Item item)
    {
        Logger.LOG(Logger.EJavaMMAPI,Logger.EInfo,"ItemDisplay.java : notifyControlAvailable + ");
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
                                      x = iControl.getSize().x;
                                      y = iControl.getSize().y;

                                 }
                    });
                }
           catch(Exception e)
                           {
                            Logger.LOG(Logger.EJavaMMAPI,Logger.EInfo,"inside videoControl's setDisplaySize....exception is " + e.toString());
                   }

                   width = x;
                   height = y;
        }

        */
    // Native methods


    private native void _setVisible(int nativeDisplayHandle,
                                    boolean value);

    private native void _setContainerVisible(int iEventSourceHandle,int nativeDisplayHandle,
            boolean value);
    private native void _setFullScreenMode(int nativeDisplayHandle, boolean value);
    private native void _setWindowToNative(int nativeDisplayHandle,int qwidgetHandle);
    private native void _setNativeWindowSize(int nativeDisplayHandle,int iEventSourceHandle,int width,int height,int x, int y, int[] formArray);
//  private native void _nativeDummy();

}

class NativeResources implements Runnable
{
    private VideoItem iVItem;
    private ItemDisplay iItemDis;
    NativeResources(ItemDisplay aParent, VideoItem aVideoItem)
    {
        iVItem = aVideoItem;
        iItemDis = aParent;
    }
    public void start()
    {
        Thread t = new Thread(this);
        t.start();
    }

    public void run()
    {
        iItemDis.setNativeResources(iVItem);
    }
}

