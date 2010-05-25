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
package com.nokia.test.java.eswt.fontdialog;

import org.eclipse.ercp.swt.mobile.Command;
import org.eclipse.swt.SWT;
import org.eclipse.swt.events.SelectionEvent;
import org.eclipse.swt.graphics.Color;
import org.eclipse.swt.graphics.RGB;
import org.eclipse.swt.widgets.Canvas;
import org.eclipse.swt.widgets.Label;
import org.eclipse.swt.widgets.FontDialog;
import org.eclipse.swt.graphics.FontData;
import org.eclipse.swt.graphics.Font;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.layout.GridLayout;


import com.nokia.test.java.eswt.midlet.TestBaseMIDlet;

public class FontDialogTestMIDlet_01 extends TestBaseMIDlet
{

    /** fontdialog instance */
    private FontDialog fdlg = null;

    private FontData fdata = null;

    private Label label = null;

    /** canvas instance to change background color */
    private Canvas canvas = null;

    /**
     * initialization
     */
    protected void init()
    {
        // customize the shell
        this.shell.setText("FontDialog Test");
        this.shell.setLayout(new GridLayout(1, false));

        this.label = new Label(this.shell,SWT.BORDER);
        label.setText("Example Font");
        label.setLayoutData(
            new GridData(SWT.CENTER, SWT.CENTER, true, true));

        // create canvas, that occupies the area in bottom corner
        this.canvas = new Canvas(this.shell, SWT.BORDER);
        this.canvas.setBounds(0,0,this.shell.getBounds().width,50);

        // initialize the colordialog
        this.fdlg = new FontDialog(this.shell);

        // "open" left softkey command
        Command cmd = new Command(this.shell, Command.SELECT, 1);
        cmd.setText("Open");
        cmd.addSelectionListener(this);
        // "set new color"  command
        cmd = new Command(this.shell, Command.SELECT, 0);
        cmd.setText("Set new color");
        cmd.addSelectionListener(this);
        // "set new fontlisy"  command
        cmd = new Command(this.shell, Command.SELECT, 0);
        cmd.setText("Set new fontlist");
        cmd.addSelectionListener(this);
        // "set new Text"  command
        cmd = new Command(this.shell, Command.SELECT, 0);
        cmd.setText("Set new text");
        cmd.addSelectionListener(this);
        // "set long Text"  command
        cmd = new Command(this.shell, Command.SELECT, 0);
        cmd.setText("Set long text");
        cmd.addSelectionListener(this);

        this.shell.layout();
        this.shell.update();
    }

    /**
     * selectionlistener implementation
     */
    public void widgetSelected(SelectionEvent event)
    {
        if (event.widget instanceof Command)
        {
            String scmd = ((Command) event.widget).getText();
            if ("Open".equals(scmd))
            {
                // open the colordialog
                this.fdata  = this.fdlg.open();
                if (this.fdata != null)
                {
                    Font font = new Font(this.display,this.fdata);
                    this.label.setFont(font);

                    if (this.fdlg.getRGB() != null)
                    {
                        // set the background to the selected color
                        Color color = new Color(this.display, this.fdlg.getRGB());
                        this.label.setForeground(color);
                        this.canvas.setBackground(color);
                        color.dispose();
                    }
                }

                this.shell.layout();
                this.shell.update();
                return;
            }
            if ("Set new color".equals(scmd))
            {
                // add a new custom color to the fontdialog
                this.fdlg.setRGB(new RGB(131, 153, 177));
                this.fdlg.setRGB(new RGB(255, 0, 0));
                this.fdlg.open();

                return;
            }
            if ("Set new fontlist".equals(scmd))
            {
                FontData[] fdatalist = this.display.getFontList(null, true);
                FontData[] list = new FontData[1];

                list[0] = fdatalist[2];

                // add a new fontlist
                this.fdlg.setFontList(list);
                this.fdata = this.fdlg.open();

                return;
            }
            if ("Set new text".equals(scmd))
            {
                // add a new custom title to the fontdialog
                this.fdlg.setText("New FontDialog");
                return;
            }
            if ("Set long text".equals(scmd))
            {
                // add a long title to the fontdialog
                this.fdlg.setText("This is a very long title for FontDialog to see how it behaves! Let's see what happens...");
                return;
            }
        }
        super.widgetSelected(event);
    }

}
