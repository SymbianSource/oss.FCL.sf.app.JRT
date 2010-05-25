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
package com.nokia.test.java.eswt.colordialog;

import org.eclipse.ercp.swt.mobile.Command;
import org.eclipse.swt.SWT;
import org.eclipse.swt.events.SelectionEvent;
import org.eclipse.swt.graphics.Color;
import org.eclipse.swt.graphics.RGB;
import org.eclipse.swt.widgets.Canvas;
import org.eclipse.swt.widgets.ColorDialog;
import org.eclipse.swt.widgets.Button;
import org.eclipse.swt.widgets.Combo;
import org.eclipse.swt.layout.FillLayout;

import com.nokia.test.java.eswt.midlet.TestBaseMIDlet;

/**
 * this class is intended to test the colordialog component
 * - creating colordialog
 * - navigating in colordialog
 * - setting and selecting colors from colordialog
 */
public class ColorDialogTestMIDlet_01 extends TestBaseMIDlet
{
    /** colordialog instance */
    private ColorDialog cdlg = null;
    /** canvas instance to change background color */
    private Canvas canvas = null;

    private Combo textCombo = null;
    private Button openButton = null;
    /**
     * initialization
     */
    protected void init()
    {
        // customize the shell
        this.shell.setText("ColorDialog");

        this.shell.setLayout(new FillLayout(SWT.VERTICAL));
        // create canvas, that occupies the whole shell area
        this.canvas = new Canvas(this.shell, SWT.NONE);
        this.canvas.setBounds(0,0,this.shell.getClientArea().width,this.shell.getClientArea().height);

        this.textCombo = new Combo(this.shell,SWT.DROP_DOWN | SWT.BORDER);
        this.textCombo.add("Default title");
        this.textCombo.add("New title");
        this.textCombo.add("!#2£$%&{}+?");
        this.textCombo.add("A very very long title for the dialog");
        this.textCombo.select(0);
        this.textCombo.setFocus();
        this.openButton = new Button(this.shell,SWT.PUSH | SWT.BORDER);
        this.openButton.setText("Open Dialog");
        this.openButton.addSelectionListener(this);

        // initialize the colordialog
        this.cdlg = new ColorDialog(this.shell);

        // "open" left softkey command
        Command cmd = new Command(this.shell, Command.SELECT, 1);
        cmd.setText("Open");
        cmd.addSelectionListener(this);
        // "set new preset" left softkey command
        cmd = new Command(this.shell, Command.SELECT, 0);
        cmd.setText("Set new preset");
        cmd.addSelectionListener(this);
    }

    /**
     * selectionlistener implementation
     */
    public void widgetSelected(SelectionEvent event)
    {
        if (event.widget instanceof Button)
        {
            this.cdlg.setText(this.textCombo.getText());
            RGB rgb = this.cdlg.open();
            if (rgb == null)
            {
                // "cancel" or "none" was selected
                if (this.cdlg.getRGB() == null)
                    // "none" was selected
                    this.canvas.setBackground(null);
            }
            else
            {
                // set the background to the selected color
                Color color = new Color(this.display, rgb);
                this.canvas.setBackground(color);
                color.dispose();
            }
            return;
        }
        if (event.widget instanceof Command)
        {
            String scmd = ((Command) event.widget).getText();
            if ("Open".equals(scmd))
            {
                String comboSelect = this.textCombo.getText();
                if (!comboSelect.equals("Default title"))
                {
                    this.cdlg.setText(comboSelect);
                }
//               open the colordialog
                RGB rgb = this.cdlg.open();
                if (rgb == null)
                {
                    // "cancel" or "none" was selected
                    if (this.cdlg.getRGB() == null)
                        // "none" was selected
                        this.canvas.setBackground(null);
                }
                else
                {
                    // set the background to the selected color
                    Color color = new Color(this.display, rgb);
                    this.canvas.setBackground(color);
                    color.dispose();
                }
                return;
            }
            if ("Set new preset".equals(scmd))
            {
                // add a new custom color to the colordialog
                this.cdlg.setRGB(new RGB(131, 153, 177));
                return;
            }
        }
        super.widgetSelected(event);
    }
}
