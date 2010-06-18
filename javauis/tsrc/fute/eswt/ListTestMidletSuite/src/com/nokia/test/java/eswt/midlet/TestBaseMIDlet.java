/*
* Copyright (c) 2006-2010 Nokia Corporation and/or its subsidiary(-ies).
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
package com.nokia.test.java.eswt.midlet;

import javax.microedition.midlet.MIDlet;
import javax.microedition.midlet.MIDletStateChangeException;

import org.eclipse.swt.widgets.Display;
import org.eclipse.swt.widgets.Shell;

import org.eclipse.swt.events.SelectionEvent;
import org.eclipse.swt.events.SelectionListener;

import org.eclipse.ercp.swt.mobile.Command;

public abstract class TestBaseMIDlet extends MIDlet
        implements Runnable, SelectionListener
{

    /** */
    protected Display display = null;
    /** */
    protected Shell shell     = null;
    /** */
    protected Command cmdExit = null;

    /*  (non-Javadoc)
     * @see javax.microedition.midlet.MIDlet#startApp()
     */
    protected void startApp() throws MIDletStateChangeException
    {
        new Thread(this).start();
    }

    /*  (non-Javadoc)
     * @see java.lang.Runnable#run()
     */
    public void run()
    {
        try
        {
            this.display = new Display();
            this.shell = new Shell(this.display);

            // custom initialization
            init();

            this.cmdExit = new Command(this.shell, Command.EXIT, 0);
            this.cmdExit.setText("Exit");
            this.cmdExit.addSelectionListener(this);

            this.shell.layout();
            this.shell.open();

            while (!this.shell.isDisposed())
                if (!this.display.readAndDispatch())
                    this.display.sleep();

            this.display.dispose();

        }
        catch (Throwable e)
        {
            System.out.println("Exception: " + e.getMessage());
        }

        this.notifyDestroyed();
    }

    /*  (non-Javadoc)
     * @see javax.microedition.midlet.MIDlet#destroyApp(boolean)
     */
    protected void destroyApp(boolean arg0) throws MIDletStateChangeException
    {
    }

    /*  (non-Javadoc)
     * @see javax.microedition.midlet.MIDlet#pauseApp()
     */
    protected void pauseApp()
    {
    }

    public void setExit(boolean enable)
    {

        if (this.cmdExit.isDisposed() && enable)
        {
            this.cmdExit = new Command(this.shell, Command.EXIT, 0);
            this.cmdExit.setText("Exit");
            this.cmdExit.addSelectionListener(this);
        }
        else if (!enable)
        {
            if (!this.cmdExit.isDisposed())
                this.cmdExit.dispose();
        }
    }
    /* (non-Javadoc)
     * @see org.eclipse.swt.events.SelectionListener#widgetSelected(org.eclipse.swt.events.SelectionEvent)
     */
    public void widgetSelected(SelectionEvent event)
    {
        if (event.widget == this.cmdExit)
        {
            this.cmdExit.dispose();
            this.shell.dispose();
        }
    }

    /* (non-Javadoc)
     * @see org.eclipse.swt.events.SelectionListener#widgetDefaultSelected(org.eclipse.swt.events.SelectionEvent)
     */
    public void widgetDefaultSelected(SelectionEvent event)
    {
    }

    /**
     * Initialisation
     */
    protected abstract void init();
}
