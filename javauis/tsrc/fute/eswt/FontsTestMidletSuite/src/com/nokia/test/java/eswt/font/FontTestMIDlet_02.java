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

import org.eclipse.ercp.swt.mobile.CaptionedControl;
import org.eclipse.ercp.swt.mobile.Command;
import org.eclipse.ercp.swt.mobile.DateEditor;
import org.eclipse.swt.SWT;
import org.eclipse.swt.events.SelectionEvent;
import org.eclipse.swt.graphics.Font;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.widgets.Combo;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.MessageBox;

import com.nokia.test.java.eswt.midlet.TestBaseMIDlet;

/**
 *
 */
public class FontTestMIDlet_02 extends TestBaseMIDlet
{

    /** currently used font */
    protected Font font = null;

    /** current composite */
    protected Composite composite = null;

    /** commands */
    protected Command cmdCompositeCombo     = null;
    protected Command cmdCaprionedControl   = null;

    protected Command cmdFont               = null;
    private Command cmdFontDEFINE           = null;
    private Command cmdFontNULL             = null;


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
        Command cmdComposite = new Command(this.shell, Command.COMMANDGROUP, 0);
        cmdComposite.setText("Composite");

        this.cmdCompositeCombo = new Command(cmdComposite, Command.GENERAL, 1);
        this.cmdCompositeCombo.setText("Combo");
        this.cmdCompositeCombo.addSelectionListener(this);

        this.cmdCaprionedControl = new Command(cmdComposite, Command.GENERAL, 0);
        this.cmdCaprionedControl.setText("Captioned Control");
        this.cmdCaprionedControl.addSelectionListener(this);

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
    private void clean()
    {
        // dispose composite
        if (this.composite != null && !this.composite.isDisposed())
        {
            this.composite.dispose();
        }

        // dispose the font
        if (this.font != null && !this.font.isDisposed())
        {
            this.font.dispose();
        }

        this.shell.update();
        this.shell.layout();
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
            if (event.widget == this.cmdCompositeCombo)
            {
                if (this.composite instanceof Combo)
                {
                    // nothing to change
                    return;
                }
                // cleaning
                this.clean();
                // create new Combo
                Combo control = new Combo(this.shell, SWT.NONE);
                control.setItems(new String[] {"item 1","item 2","item 3"});
                control.setLayoutData(
                    new GridData(SWT.CENTER, SWT.CENTER, true, true));
                this.composite = control;
                this.cmdFont.setEnabled(true);
                this.shell.layout();
                this.shell.update();
                return;
            }
            if (event.widget == this.cmdCaprionedControl)
            {
                if (this.composite instanceof CaptionedControl)
                {
                    return;
                }
                this.clean();
                CaptionedControl control = new CaptionedControl(this.shell, SWT.NONE);
                new DateEditor(control, DateEditor.DATE);
                control.setText("Today: ");
                control.setTrailingText("*");
                control.setLayoutData(
                    new GridData(SWT.CENTER, SWT.CENTER, true, true));
                control.setFocus();
                this.composite = control;
                this.cmdFont.setEnabled(true);
                this.shell.layout();
                this.shell.update();
                this.log(this.composite.getFont());
                return;
            }

            // font events
            if (event.widget == this.cmdFontDEFINE)
            {
                FontConfigDialog dlg = new FontConfigDialog(this.shell);
                Font font = dlg.open(this.composite.getFont());
                this.log(font);
                if (font != null)
                {
                    this.composite.setFont(font);
                    // dispose current font
                    if (this.font != null && !this.font.isDisposed())
                    {
                        this.font.dispose();
                    }
                    this.composite.redraw();
                    this.composite.update();
                    this.font = font;
                    this.shell.layout();
                    this.shell.update();
                }
                return;
            }
            if (event.widget == this.cmdFontNULL)
            {
                this.composite.setFont(null);
                // dispose current font
                if (this.font != null && !this.font.isDisposed())
                {
                    this.font.dispose();
                }
                // set font to null
                this.font = null;
                this.log(this.composite.getFont());
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
