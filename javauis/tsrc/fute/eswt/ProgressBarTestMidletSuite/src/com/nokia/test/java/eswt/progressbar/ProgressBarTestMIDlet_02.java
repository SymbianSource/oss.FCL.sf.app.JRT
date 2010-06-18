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
package com.nokia.test.java.eswt.progressbar;


import org.eclipse.swt.SWT;
import org.eclipse.swt.events.SelectionEvent;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.widgets.Control;
import org.eclipse.swt.widgets.Menu;
import org.eclipse.swt.widgets.MenuItem;
import org.eclipse.swt.widgets.MessageBox;
import org.eclipse.swt.widgets.Shell;
import org.eclipse.swt.widgets.ProgressBar;
import org.eclipse.swt.widgets.Display;
import org.eclipse.swt.widgets.Label;
import org.eclipse.ercp.swt.mobile.Command;
import java.util.*;

import com.nokia.test.java.eswt.midlet.TestBaseMIDlet;

/* **
 * this class is intended to test the progress bar component
 *
 *
 */

public class ProgressBarTestMIDlet_02 extends TestBaseMIDlet
{

    /**  smooth progress bar */
    private ProgressBar smooth = null;

    /**  indeterminate progress bar*/
    private ProgressBar indeterminate = null;

    private Shell dialog = null;

    public ProgressBar runnable = null;
    /** Add runnable command */
    private String cmdRunnable = "Runnable";
    /** Add smooth command */
    private String cmdSmooth = "Smooth in Dialog Shell";
    /** Add indeterminate command */
    private String cmdIndeterminate = "Indeterminate";
    /** Add smooth vertical command */
    private String cmdSmoothVertical = "Smooth - vertical";
    /** Add indeterminate vertical command */
    private String cmdIndeterminateVertical = "Indeterminate in dialog shell";
    /** Add runnable command */
    private String cmdRun = "Run it";

    private Command cmdClose = null;
    private Command cmdAgain = null;
    private Label smoothLabel = null;
    Timer timer;
    private ProgressTimerTask progTimerTask;
    int KTime;
    volatile int timeout;
    boolean addItemsInEvent = false;

    synchronized int timeout()
    {
        return timeout;
    }

    synchronized void setTimeout(int val)
    {
        timeout = val;
    }
    /**
     * show a messagebox with the thrown exception
     * @param the text to be displayed in the messagebox
     */
    protected void showException(String msg)
    {
        MessageBox mb = new MessageBox(this.shell, SWT.ICON_INFORMATION | SWT.OK);
        mb.setText("Exception");
        mb.setMessage(msg);
        mb.open();
    }

    protected void init()
    {

        // customize the shell
        this.shell.setText("Progress Bar test");
        this.shell.setLayout(new GridLayout());

        this.dialog = new Shell(this.shell,SWT.DIALOG_TRIM);
        this.dialog.setLayout(new GridLayout());
        // "single listview" left softkey command
        this.cmdAgain = new Command(this.dialog, Command.SELECT, 1);
        this.cmdAgain.setText("Again");
        this.cmdAgain.addSelectionListener(this);
        this.cmdClose = new Command(this.dialog, Command.EXIT, 1);
        this.cmdClose.setText("Close");
        this.cmdClose.addSelectionListener(this);

        // set up the menu
        Menu mainmenu = new Menu(this.shell, SWT.BAR);
        this.shell.setMenuBar(mainmenu);

        // "Runnable" menuitem for "add" submenu
        MenuItem item = new MenuItem(mainmenu, SWT.PUSH);
        item.setText(this.cmdRunnable);
        item.addSelectionListener(this);
        // "Smooth" menuitem for "add" submenu
        item = new MenuItem(mainmenu, SWT.PUSH);
        item.setText(this.cmdSmooth);
        item.addSelectionListener(this);
        // "Indeterminate" menuitem for "add" submenu
        item = new MenuItem(mainmenu, SWT.PUSH);
        item.setText(this.cmdIndeterminate);
        item.addSelectionListener(this);
        // "Smooth - vertical" menuitem for "add" submenu
        item = new MenuItem(mainmenu, SWT.PUSH);
        item.setText(this.cmdSmoothVertical);
        item.addSelectionListener(this);
        // "Indeterminate - vertical" menuitem for "add" submenu
        item = new MenuItem(mainmenu, SWT.PUSH);
        item.setText(this.cmdIndeterminateVertical);
        item.addSelectionListener(this);

        item = new MenuItem(mainmenu, SWT.PUSH);
        item.setText("Add Selection");
        item.addSelectionListener(this);
        item = new MenuItem(mainmenu, SWT.PUSH);
        item.setText(this.cmdRun);
        item.addSelectionListener(this);
        item = new MenuItem(mainmenu, SWT.PUSH);
        item.setText("Clear Shell");
        item.addSelectionListener(this);

    }

    /**
     * Cleanup the screen.
     * Removes all widget objects which are currently presented
     * on the shell.
     */
    private void shellCleanup()
    {

        if (this.shell == null || this.shell.isDisposed()) return;

        Control[] controls = this.shell.getChildren();

        for (int i=0; i<controls.length; i++)
        {
            controls[i].dispose();
        }

        this.shell.update();
    }
    void after(final ProgressBar bar, final Label label1, final String text, int miliseconds)
    {
        KTime = miliseconds;
        setTimeout(KTime);
        bar.setSelection(0);
        new Thread(new Runnable()
        {
            public void run()
            {
                while (timeout() > 0)
                {
                    try
                    {
                        Thread.sleep(50);
                    }
                    catch (InterruptedException e)
                    {
                        e.printStackTrace();
                    }
                    Display.getDefault().syncExec(new Runnable()
                    {
                        public void run()
                        {
                            bar.setSelection(KTime - timeout());
                            if (timeout() % 1000 == 0)
                            {
                                label1.setText(text + " " + timeout() / 1000 + " seconds");
                                bar.getParent().layout();
                            }
                        }
                    });
                    setTimeout(timeout() - 50);
                }
                Display.getDefault().syncExec(new Runnable()
                {
                    public void run()
                    {
                        bar.setSelection(KTime);
                    }
                });


            }
        }).start();
    }
    public void widgetSelected(SelectionEvent event)
    {
        if (event.widget == this.cmdClose)
        {
            Control[] controls = this.dialog.getChildren();
            for (int i=0; i<controls.length; i++)
            {
                controls[i].dispose();
            }
            this.dialog.setVisible(false);
        }
        else if (event.widget == this.cmdAgain)
        {
            Label label = null;

            this.after(this.smooth, this.smoothLabel, "Finishing in", 5000);
        }
        if (event.widget instanceof MenuItem)
        {
            try
            {
                String scmd = ((MenuItem) event.widget).getText();
                if (this.cmdSmooth.equals(scmd))
                {

                    this.dialog.setText("Smooth");
                    this.smoothLabel = new Label(this.dialog,SWT.BORDER);
                    this.smoothLabel.setText("Running");
                    this.smoothLabel.setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, false));
                    this.smooth = new ProgressBar(this.dialog, SWT.BORDER|SWT.SMOOTH);
                    this.smooth.setMaximum(5000);
                    this.smooth.setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, false));
                    this.cmdAgain.setEnabled(true);
                    this.dialog.layout();
                    this.dialog.open();
                    this.after(this.smooth, this.smoothLabel, "Finishing in", 5000);
                }
                else if (this.cmdIndeterminate.equals(scmd))
                {
                    this.indeterminate = new ProgressBar(this.shell, SWT.INDETERMINATE);
                    this.indeterminate.setLayoutData(new GridData(SWT.FILL, SWT.CENTER, true, false));
                }
                else if (this.cmdIndeterminateVertical.equals(scmd))
                {
                    Label label = new Label(this.dialog,SWT.BORDER);
                    label.setText("Running");
                    label.setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, false));
                    this.dialog.setText("Indeterminate");
                    this.indeterminate = new ProgressBar(this.dialog, SWT.INDETERMINATE);
                    this.indeterminate.setLayoutData(new GridData(SWT.FILL, SWT.CENTER, true, false));
                    this.cmdAgain.setEnabled(false);
                    this.dialog.layout();
                    this.dialog.open();
                }
                else if (this.cmdSmoothVertical.equals(scmd))
                {
                    this.runnable = new ProgressBar(this.shell, SWT.SMOOTH|SWT.VERTICAL);
                    this.runnable.setLayoutData(new GridData(SWT.CENTER, SWT.FILL, true, false));
                }
                else if (this.cmdRunnable.equals(scmd))
                {
                    try
                    {
                        this.runnable = new ProgressBar(this.shell, SWT.SMOOTH);
                        this.runnable.setMinimum(0);
                        this.runnable.setMaximum(10);
                        this.runnable.setLayoutData(new GridData(SWT.FILL, SWT.CENTER, true, false));
                    }
                    catch (Exception ex)
                    {
                        this.showException(ex.getMessage());
                    }
                }
                else if (scmd.equals("Clear Shell"))
                {
                    this.shellCleanup();
                }
                else if (scmd.equals("Add Selection"))
                {
                    if (this.shell == null || this.shell.isDisposed()) return;

                    if (this.smooth != null)
                    {
                        if (this.smooth.getSelection()+10 <= this.smooth.getMaximum())
                            this.smooth.setSelection(this.smooth.getSelection()+10);
                    }
                    else
                    {
                        this.smooth.setSelection(this.smooth.getMinimum());
                    }
                }
                else if (scmd.equals("Run it"))
                {
                    if (this.shell == null || this.shell.isDisposed()) return;

                    if (this.runnable != null)
                    {
                        while (this.runnable.getSelection() < this.runnable.getMaximum())
                        {
                            try
                            {
                                Thread.sleep(1000);
                            }
                            catch (InterruptedException e)
                            {
                                e.printStackTrace();
                            }
                            this.runnable.setSelection(this.runnable.getSelection()+1);
                            this.runnable.update();
                        }
                    }
                }
            }
            catch (Exception ex)
            {
                this.showException(ex.getMessage());
            }

            this.shell.layout();
            this.shell.update();
            return;
        }

        super.widgetSelected(event);
    }

}


