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

package com.nokia.test.java.eswt.label;

import org.eclipse.ercp.swt.mobile.Command;
import org.eclipse.ercp.swt.mobile.MobileShell;
import org.eclipse.swt.SWT;
import org.eclipse.swt.events.SelectionEvent;
import org.eclipse.swt.graphics.Image;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.widgets.Label;
import org.eclipse.swt.widgets.MessageBox;

import com.nokia.test.java.eswt.midlet.TestBaseMIDlet;

/**
 * this class is intended to test the label component
 * - adding label to different parents
 */
public class LabelTestMIDlet_03 extends TestBaseMIDlet
{
    /** image instance */
    private Image img = null;
    /** mobileshell instance */
    private MobileShell mshell = null;

    /**
     * initialization
     */
    protected void init()
    {
        // customize the shell
        this.shell.setText("Label 3");
        this.shell.setLayout(new GridLayout());

        // try to initialize the image
        try
        {
            this.img = new Image(this.display, this.getClass().getResourceAsStream("/res/sphere.jpg"));
        }
        catch (Exception ex)
        {
            // show a messagebox about the exception
            MessageBox mb = new MessageBox(this.shell, SWT.ICON_INFORMATION | SWT.OK);
            mb.setText("Exception");
            mb.setMessage("Image could not be loaded");
            mb.open();
            this.img = null;
        }

        // create text label on the shell
        Label label = new Label(this.shell, SWT.BORDER);
        label.setText("Label on Shell");
        label.setLayoutData(new GridData(SWT.CENTER, SWT.CENTER, true, true));
        // create image label on the shell
        label = new Label(this.shell, SWT.BORDER);
        label.setImage(this.img);
        label.setLayoutData(new GridData(SWT.CENTER, SWT.TOP, true, true));
        // "switch" left softkey command
        Command cmd = new Command(this.shell, Command.SELECT, 0);
        cmd.setText("Switch");
        cmd.addSelectionListener(this);

        // create the mobileshell with the labels
        this.mshell = new MobileShell(this.display, SWT.SHELL_TRIM, MobileShell.SMALL_STATUS_PANE);
        this.mshell.setText("Label Test 3 - Mobile Shell");
        this.mshell.setLayout(new GridLayout());
        // create image label on the mobileshell
        label = new Label(this.mshell, SWT.BORDER);
        label.setImage(this.img);
        label.setLayoutData(new GridData(SWT.CENTER, SWT.CENTER, true, true));
        // create text label on the mobileshell
        label = new Label(this.mshell, SWT.BORDER);
        label.setText("Label on MobileShell");
        label.setLayoutData(new GridData(SWT.CENTER, SWT.TOP, true, true));
        // "switch" left softkey command
        cmd = new Command(this.mshell, Command.SELECT, 0);
        cmd.setText("Switch");
        cmd.addSelectionListener(this);
        // "exit" right softkey command
        cmd = new Command(this.mshell, Command.EXIT, 0);
        cmd.setText("Exit");
        cmd.addSelectionListener(this);
        this.mshell.pack();
        this.mshell.open();
        this.mshell.setVisible(false);
    }

    /**
     * selectionlistener implementation
     */
    public void widgetSelected(SelectionEvent event)
    {
        if (event.widget instanceof Command && "Switch".equals(((Command) event.widget).getText()))
        {
            // switch between shell & mobileshell...
            this.mshell.setVisible(!this.mshell.isVisible());
            this.shell.setVisible(!this.shell.isVisible());
            return;
        }
        else if (event.widget == this.cmdExit && this.img != null && !this.img.isDisposed())
            // dispose the images before exit
            this.img.dispose();
        super.widgetSelected(event);
    }
}
