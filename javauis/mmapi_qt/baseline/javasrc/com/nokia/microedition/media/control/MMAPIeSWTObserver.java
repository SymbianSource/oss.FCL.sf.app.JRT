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

import org.eclipse.swt.widgets.Control;
import org.eclipse.swt.widgets.Display;
import org.eclipse.swt.events.*;
import com.nokia.microedition.volumekeys.*;
import com.nokia.mj.impl.utils.Logger;


public class MMAPIeSWTObserver
{
    //MMAPILCDUIInvokerImpl mmapiLcduiInvoker;

    mmacontrollistener cntrlsner ;
    mmashelllistener shellsner ;
    mmadisposelistener displsner;

    BaseDisplay display;


    public MMAPIeSWTObserver()
    {
        //mmapiLcduiInvoker = new MMAPILCDUIInvokerImpl();
    }

    public void setDisplayToObserver(BaseDisplay display)
    {
        this.display = display;
        cntrlsner = new mmacontrollistener(display);
        shellsner = new mmashelllistener(display);
        displsner = new mmadisposelistener(display);
    }

    public void addControlListenerToControl(final Control eSWTControl)
    {

        Logger.LOG(Logger.EJavaMMAPI,Logger.EInfo,"MMAPIeSWTObserver.java :addControlListenerToControl +");
        //mmapiLcduiInvoker.AddControlListenerToControl(eSWTControl,(org.eclipse.swt.events.ControlListener)this);

        try
        {
            final org.eclipse.swt.widgets.Display disp = com.nokia.mj.impl.nokialcdui.LCDUIInvoker.getEswtDisplay();
            disp.syncExec(new Runnable()
            {
                public void run()
                {
                    eSWTControl.addControlListener(cntrlsner);
                }
            });
        }
        catch (Exception e)
        {
            Logger.LOG(Logger.EJavaMMAPI,Logger.EInfo,"addControlListenerToControl....exception is " + e.toString());
        }

        Logger.LOG(Logger.EJavaMMAPI,Logger.EInfo,"MMAPIeSWTObserver.java :addControlListenerToControl -");

    }

    public void addShellListenerToControl(final Control eSWTControl)
    {
        Logger.LOG(Logger.EJavaMMAPI,Logger.EInfo,"MMAPIeSWTObserver.java :addShellListenerToControl +");
        try
        {
            final org.eclipse.swt.widgets.Display disp = com.nokia.mj.impl.nokialcdui.LCDUIInvoker.getEswtDisplay();
            disp.syncExec(new Runnable()
            {
                public void run()
                {
                    eSWTControl.getShell().addShellListener(shellsner);
                }
            });
        }
        catch (Exception e)
        {
            Logger.LOG(Logger.EJavaMMAPI,Logger.EInfo,"addShellListenerToControl....exception is " + e.toString());
        }

        Logger.LOG(Logger.EJavaMMAPI,Logger.EInfo,"MMAPIeSWTObserver.java :addShellListenerToControl -");

    }


    public void addDisposeListenerToControl(final Control eSWTControl)
    {
        Logger.LOG(Logger.EJavaMMAPI,Logger.EInfo,"MMAPIeSWTObserver.java :addDisposeListenerToControl +");
        //mmapiLcduiInvoker.AddDisposeListenerToControl(eSWTControl,(org.eclipse.swt.events.DisposeListener)this);

        try
        {
            final org.eclipse.swt.widgets.Display disp = com.nokia.mj.impl.nokialcdui.LCDUIInvoker.getEswtDisplay();
            disp.syncExec(new Runnable()
            {
                public void run()
                {
                    eSWTControl.addDisposeListener(displsner);
                }
            });
        }
        catch (Exception e)
        {
            Logger.LOG(Logger.EJavaMMAPI,Logger.EInfo,"addShellListenerToControl....exception is " + e.toString());
        }
        Logger.LOG(Logger.EJavaMMAPI,Logger.EInfo,"MMAPIeSWTObserver.java :addDisposeListenerToControl -");

    }

    public void removeControlListenerToControl(Control eSWTControl)
    {
        Logger.LOG(Logger.EJavaMMAPI,Logger.EInfo,"MMAPIeSWTObserver.java :removeControlListenerToControl +");
        //mmapiLcduiInvoker.RemoveControlListenerToControl(eSWTControl,(org.eclipse.swt.events.ControlListener)this);
        Logger.LOG(Logger.EJavaMMAPI,Logger.EInfo,"MMAPIeSWTObserver.java :removeControlListenerToControl -");

    }
    public void removeShellListenerToControl(Control eSWTControl)
    {
        Logger.LOG(Logger.EJavaMMAPI,Logger.EInfo,"MMAPIeSWTObserver.java :removeShellListenerToControl +");
        //mmapiLcduiInvoker.RemoveShellListenerToControl(eSWTControl,(org.eclipse.swt.events.ShellListener)this);
        Logger.LOG(Logger.EJavaMMAPI,Logger.EInfo,"MMAPIeSWTObserver.java :removeShellListenerToControl -");

    }
    public void removeDisposeListenerToControl(Control eSWTControl)
    {
        Logger.LOG(Logger.EJavaMMAPI,Logger.EInfo,"MMAPIeSWTObserver.java :removeDisposeListenerToControl +");
        //mmapiLcduiInvoker.RemoveDisposeListenerToControl(eSWTControl,(org.eclipse.swt.events.DisposeListener)this);
        Logger.LOG(Logger.EJavaMMAPI,Logger.EInfo,"MMAPIeSWTObserver.java :removeDisposeListenerToControl -");

    }

}


// Control Listener

class mmacontrollistener implements ControlListener
{

    private BaseDisplay display;
    mmacontrollistener(BaseDisplay display)
    {
        this.display = display;

    }
    // from ControlListener
    public void controlMoved(ControlEvent e)
    {
        Logger.LOG(Logger.EJavaMMAPI,Logger.EInfo,"MMAPIeSWTObserver.java :controlMoved callback");
    }

    public void controlResized(ControlEvent e)
    {
        Logger.LOG(Logger.EJavaMMAPI,Logger.EInfo,"MMAPIeSWTObserver.java :controlResized callback");
    }
}

// ShellListener
class mmashelllistener implements ShellListener
{
    private BaseDisplay display;
    mmashelllistener(BaseDisplay display)
    {
        this.display = display;

    }
    // from ShellListener
    public void shellActivated(ShellEvent e)
    {
        Logger.LOG(Logger.EJavaMMAPI,Logger.EInfo,"MMAPIeSWTObserver.java :shellActivated callback");
        display.setContainerVisibilityToNative(true);
    }

    public void shellClosed(ShellEvent e)
    {
        Logger.LOG(Logger.EJavaMMAPI,Logger.EInfo,"MMAPIeSWTObserver.java :shellClosed callback");
    }

    public void shellDeactivated(ShellEvent e)
    {
        Logger.LOG(Logger.EJavaMMAPI,Logger.EInfo,"MMAPIeSWTObserver.java :shellDeactivated callback");
        display.setContainerVisibilityToNative(false);
    }

    public void shellDeiconified(ShellEvent e)
    {
        Logger.LOG(Logger.EJavaMMAPI,Logger.EInfo,"MMAPIeSWTObserver.java :shellDeiconified callback");
    }

    public void shellIconified(ShellEvent e)
    {
        Logger.LOG(Logger.EJavaMMAPI,Logger.EInfo,"MMAPIeSWTObserver.java :shellIconified callback");
    }


}

// DisposeListener

class mmadisposelistener implements DisposeListener
{
    private BaseDisplay display;
    mmadisposelistener(BaseDisplay display)
    {
        this.display = display;
    }

    // from DisposeListener
    public void widgetDisposed(DisposeEvent e)
    {
        Logger.LOG(Logger.EJavaMMAPI,Logger.EInfo,"MMAPIeSWTObserver.java :widgetDisposed callback");
    }

}