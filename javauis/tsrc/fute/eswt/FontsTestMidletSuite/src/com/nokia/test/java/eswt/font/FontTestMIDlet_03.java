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

package com.nokia.test.java.eswt.font;

import java.util.Date;

import org.eclipse.ercp.swt.mobile.Command;
import org.eclipse.ercp.swt.mobile.DateEditor;
import org.eclipse.ercp.swt.mobile.HyperLink;
import org.eclipse.swt.SWT;
import org.eclipse.swt.events.SelectionEvent;
import org.eclipse.swt.graphics.Font;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.widgets.Button;
import org.eclipse.swt.widgets.Control;
import org.eclipse.swt.widgets.Label;
import org.eclipse.swt.widgets.MessageBox;

import com.nokia.test.java.eswt.midlet.TestBaseMIDlet;

/**
 *
 */
public class FontTestMIDlet_03 extends TestBaseMIDlet
{

    /** currently used font */
    protected Font font = null;

    /** current control */
    protected Control control = null;

    protected Button controls[];
    /** commands */
    protected Command cmdControlButton      = null;
    protected Command cmdControlDateEditor  = null;
    protected Command cmdControlLabel       = null;
    protected Command cmdControlHyperLink   = null;

    protected Command cmdFont               = null;
    protected Command cmdFontDEFINE         = null;
    protected Command cmdFontNULL           = null;


    /* (non-Javadoc)
     * @see com.nokia.test.java.eswt.midlet.TestBaseMIDlet#init()
     */
    protected void init()
    {
        // init the shell
        this.shell.setText("Font Test");
        this.shell.setLayout(new GridLayout(1, false));

        this.controls = new Button[3];
        // setup commands
        // composite
        Command cmdControl = new Command(this.shell, Command.COMMANDGROUP, 0);
        cmdControl.setText("Control");

        this.cmdControlButton = new Command(cmdControl, Command.GENERAL, 3);
        this.cmdControlButton.setText("Button");
        this.cmdControlButton.addSelectionListener(this);

        this.cmdControlDateEditor = new Command(cmdControl, Command.GENERAL, 2);
        this.cmdControlDateEditor.setText("Date Editor");
        this.cmdControlDateEditor.addSelectionListener(this);

        this.cmdControlLabel = new Command(cmdControl, Command.GENERAL, 1);
        this.cmdControlLabel.setText("Label");
        this.cmdControlLabel.addSelectionListener(this);

        this.cmdControlHyperLink = new Command(cmdControl, Command.GENERAL, 0);
        this.cmdControlHyperLink.setText("HyperLink");
        this.cmdControlHyperLink.addSelectionListener(this);

        // font settings
        this.cmdFont = new Command(this.shell, Command.COMMANDGROUP, 1);
        this.cmdFont.setText("Font");
        this.cmdFont.setEnabled(false);

        this.cmdFontDEFINE = new Command(this.cmdFont, Command.GENERAL, 1);
        this.cmdFontDEFINE.setText("Define");
        this.cmdFontDEFINE.addSelectionListener(this);

        this.cmdFontNULL = new Command(this.cmdFont, Command.GENERAL, 0);
        this.cmdFontNULL.setText("null");
        this.cmdFontNULL.addSelectionListener(this);
    }

    /**
     * Clean the shell
     */
    protected void clean()
    {
        // dispose composite
        if (this.control != null && !this.control.isDisposed())
        {
            this.control.dispose();
        }

        // dispose the font
        if (this.font != null && !this.font.isDisposed())
        {
            this.font.dispose();
        }

        this.shell.update();
        this.shell.redraw();
    }

    /* (non-Javadoc)
     * @see com.nokia.test.java.eswt.midlet.TestBaseMIDlet#widgetSelected(org.eclipse.swt.events.SelectionEvent)
     */
    public void widgetSelected(SelectionEvent event)
    {
        try
        {
            // composite commands
            // Button
            if (event.widget == this.cmdControlButton)
            {
                if (this.control instanceof Button)
                {
                    // nothing to change
                    return;
                }
                // cleaning
                this.clean();
                // create new Button
                Button control1 = new Button(this.shell, SWT.NONE);
                control1.setText("Button");
                control1.setLayoutData(
                    new GridData(SWT.CENTER, SWT.CENTER, true, true));
                this.control = control1;
                this.controls[0] = control1;

                Button control2 = new Button(this.shell, SWT.CHECK);
                control2.setText("Check");
                control2.setLayoutData(
                    new GridData(SWT.CENTER, SWT.CENTER, true, true));

                this.controls[1] = control2;

                Button control3 = new Button(this.shell, SWT.RADIO);
                control3.setText("Radio");
                control3.setLayoutData(
                    new GridData(SWT.CENTER, SWT.CENTER, true, true));
                this.controls[2] = control3;
                this.cmdFont.setEnabled(true);
                this.shell.layout();
                this.shell.update();
                return;
            }
            // DateEditor
            if (event.widget == this.cmdControlDateEditor)
            {
                if (this.control instanceof DateEditor)
                {
                    // nothing to change
                    return;
                }
                // cleaning
                this.clean();
                // create new DateEditor
                DateEditor control = new DateEditor(this.shell, SWT.NONE);
                control.setDate(new Date());
                control.setLayoutData(
                    new GridData(SWT.CENTER, SWT.CENTER, true, true));
                this.control = control;
                this.cmdFont.setEnabled(true);
                this.shell.layout();
                this.shell.update();
                return;
            }
            // Label
            if (event.widget == this.cmdControlLabel)
            {
                if (this.control instanceof Label)
                {
                    // nothing to change
                    return;
                }
                // cleaning
                this.clean();
                // create new Label
                Label control = new Label(this.shell, SWT.NONE);
                control.setText("Label");
                control.setLayoutData(
                    new GridData(SWT.CENTER, SWT.CENTER, true, true));
                this.control = control;
                this.cmdFont.setEnabled(true);
                this.shell.layout();
                this.shell.update();
                return;
            }
            // HyperLink
            if (event.widget == this.cmdControlHyperLink)
            {
                if (this.control instanceof HyperLink)
                {
                    // nothing to change
                    return;
                }
                // cleaning
                this.clean();
                // create new Label
                HyperLink control = new HyperLink(this.shell, SWT.NONE, HyperLink.EMAIL);
                control.setText("some.body@somewhere.com");
                control.setLayoutData(
                    new GridData(SWT.CENTER, SWT.CENTER, true, true));
                this.control = control;
                this.cmdFont.setEnabled(true);
                this.shell.layout();
                this.shell.update();
                return;
            }

            // font events
            if (event.widget == this.cmdFontDEFINE)
            {
                FontConfigDialog dlg = new FontConfigDialog(this.shell);
                Font font = dlg.open(this.control.getFont());
                if (font != null)
                {
                    if (this.control instanceof Button)
                    {
                        for (int i = 0; i < this.controls.length; i++)
                        {

                            this.controls[i].setFont(font);
                        }
                    }
                    this.control.setFont(font);
                    // dispose current font
                    if (this.font != null && !this.font.isDisposed())
                    {
                        this.font.dispose();
                    }
                    this.font = font;
                    // update control
                    this.control.pack();
                    this.shell.layout();
                    this.shell.update();
                }
                return;
            }
            if (event.widget == this.cmdFontNULL)
            {
                if (this.control instanceof Button)
                {
                    for (int i = 0; i < this.controls.length; i++)
                    {

                        this.controls[i].setFont(null);
                    }
                }
                this.control.setFont(null);

                // dispose current font
                if (this.font != null && !this.font.isDisposed())
                {
                    this.font.dispose();
                }
                // set font to null
                this.font = null;
                this.shell.layout();
                this.shell.update();
                this.log(this.control.getFont());
                return;
            }

            // exit command
            if (event.widget == this.cmdExit)
            {
                this.clean();
            }
        }
        catch (Exception ex)
        {
            MessageBox msg = new MessageBox(this.shell);
            msg.setMessage(ex.getMessage());
            msg.open();
        }

        super.widgetSelected(event);
    }

    /**
     *
     */
    private void log(Font font)
    {
        if (font == null)
        {
            System.out.println("Font is NULL");
            return;
        }

        System.out.print("Height: " + font.getFontData()[0].getHeight());
        int style = font.getFontData()[0].getStyle();
        String msg = "N/A";
        if ((style & SWT.NORMAL) == style)
        {
            msg = "NORMAL";
        }
        else if ((style & SWT.ITALIC) == style)
        {
            msg = "ITALIC";
        }
        else if ((style & SWT.BOLD) == style)
        {
            msg = "BOLD";
        }
        else if ((style & (SWT.ITALIC | SWT.BOLD)) == style)
        {
            msg = "ITALIC | BOLD";
        }
        System.out.println("Style: " + msg);
    }
}
