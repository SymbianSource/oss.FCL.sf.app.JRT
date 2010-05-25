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

import org.eclipse.ercp.swt.mobile.Command;
import org.eclipse.ercp.swt.mobile.TextExtension;
import org.eclipse.swt.SWT;
import org.eclipse.swt.events.SelectionEvent;
import org.eclipse.swt.graphics.Font;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.widgets.Control;
import org.eclipse.swt.widgets.MessageBox;
import org.eclipse.swt.widgets.Text;

import com.nokia.test.java.eswt.midlet.TestBaseMIDlet;

/**
 *
 */
public class FontTestMIDlet_04 extends TestBaseMIDlet
{

    /** currently used font */
    protected Font font = null;

    /** current text control */
    protected Control control = null;

    /** commands */
    protected Command cmdControlText            = null;
    protected Command cmdControlTextExtension   = null;

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

        // setup commands
        // composite
        Command cmdControl = new Command(this.shell, Command.COMMANDGROUP, 0);
        cmdControl.setText("Control");


        this.cmdControlText = new Command(cmdControl, Command.GENERAL, 1);
        this.cmdControlText.setText("Text");
        this.cmdControlText.addSelectionListener(this);

        this.cmdControlTextExtension = new Command(cmdControl, Command.GENERAL, 0);
        this.cmdControlTextExtension.setText("Text Extension");
        this.cmdControlTextExtension.addSelectionListener(this);

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
            // Text
            if (event.widget == this.cmdControlText)
            {
                if (this.control instanceof Text)
                {
                    // nothing to change
                    return;
                }
                // cleaning
                this.clean();
                // create new Text
                Text control = new Text(this.shell, SWT.BORDER);
                control.setText("some text");
                control.setLayoutData(
                    new GridData(SWT.CENTER, SWT.CENTER, true, true));
                this.control = control;
                this.cmdFont.setEnabled(true);
                this.shell.layout();
                this.shell.update();
                return;
            }
            // DateEditor
            if (event.widget == this.cmdControlTextExtension)
            {
                if (this.control instanceof TextExtension)
                {
                    // nothing to change
                    return;
                }
                // cleaning
                this.clean();
                // create new TextExtension
                TextExtension control =
                    new TextExtension(this.shell, SWT.BORDER, TextExtension.URL);
                control.setText("www.nokia.com");
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
                    this.control.setFont(font);
                    // dispose current font
                    if (this.font != null && !this.font.isDisposed())
                    {
                        this.font.dispose();
                    }
                    this.font = font;
                    // update control
                    this.shell.layout();
                    this.shell.update();
                }
                return;
            }
            if (event.widget == this.cmdFontNULL)
            {
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
        System.out.println("; Style: " + msg);
    }
}
