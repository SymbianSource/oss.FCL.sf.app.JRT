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
import org.eclipse.swt.widgets.ProgressBar;
import java.util.*;

import com.nokia.test.java.eswt.midlet.TestBaseMIDlet;

/* **
 * this class is intended to test the progress bar component
 *
 *
 */

public class ProgressBarTestMIDlet_01 extends TestBaseMIDlet
{

    /**  smooth progress bar */
    private ProgressBar smooth = null;

    /**  indeterminate progress bar*/
    private ProgressBar indeterminate = null;

    public ProgressBarRunnable runnable = null;
    /** Add runnable command */
    private String cmdRunnable = "Runnable";
    /** Add smooth command */
    private String cmdSmooth = "Smooth";
    /** Add indeterminate command */
    private String cmdIndeterminate = "Indeterminate";
    /** Add smooth vertical command */
    private String cmdSmoothVertical = "Smooth - vertical";
    /** Add indeterminate vertical command */
    private String cmdIndeterminateVertical = "Indeterminate - vertical";


    Timer timer;
    private ProgressTimerTask progTimerTask;

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
    public void widgetSelected(SelectionEvent event)
    {
        if (event.widget instanceof MenuItem)
        {
            try
            {
                String scmd = ((MenuItem) event.widget).getText();
                if (this.cmdSmooth.equals(scmd))
                {
                    this.smooth = new ProgressBar(this.shell, SWT.BORDER|SWT.SMOOTH);
                    this.smooth.setLayoutData(new GridData(SWT.FILL, SWT.CENTER, true, false));
                }
                else if (this.cmdIndeterminate.equals(scmd))
                {
                    this.indeterminate = new ProgressBar(this.shell, SWT.INDETERMINATE);
                    this.indeterminate.setLayoutData(new GridData(SWT.FILL, SWT.CENTER, true, false));
                }
                else if (this.cmdIndeterminateVertical.equals(scmd))
                {
                    this.indeterminate = new ProgressBar(this.shell, SWT.INDETERMINATE|SWT.VERTICAL);
                    this.indeterminate.setLayoutData(new GridData(SWT.CENTER, SWT.FILL, true, false));
                }
                else if (this.cmdSmoothVertical.equals(scmd))
                {
                    this.smooth = new ProgressBar(this.shell, SWT.SMOOTH|SWT.VERTICAL);
                    this.smooth.setLayoutData(new GridData(SWT.CENTER, SWT.FILL, true, false));
                }
                else if (this.cmdRunnable.equals(scmd))
                {
                    try
                    {
                        this.runnable = new ProgressBarRunnable(this.shell, SWT.SMOOTH);
                        this.runnable.setLayoutData(new GridData(SWT.FILL, SWT.CENTER, true, false));
                        this.runnable.setMaximum(10);
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

