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
package com.nokia.test.java.eswt.text;

import org.eclipse.ercp.swt.mobile.Command;
import org.eclipse.ercp.swt.mobile.QueryDialog;
import org.eclipse.swt.SWT;
import org.eclipse.swt.events.SelectionEvent;
import org.eclipse.swt.graphics.Point;

import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.layout.GridData;

import org.eclipse.swt.widgets.Control;
import org.eclipse.swt.widgets.Label;
import org.eclipse.swt.widgets.Text;

import com.nokia.test.java.eswt.midlet.TestBaseMIDlet;

public class TextTestMIDlet_02 extends TestBaseMIDlet
{

    /** */
    private Text readonlyField  = null;
    private Text passwordField  = null;

    private Text multiField     = null;

    /** Do menu commands */
    private Command cmdDoAppend     = null;
    private Command cmdDoInsert     = null;

    /** Input mode menu commands */
    private Command cmdModeReadOnly = null;
    private Command cmdModeEditable = null;

    /** Text Limit menu commands */
    private Command cmdLimitNONE    = null;
    private Command cmdLimitCustom  = null;

    /** Aligment menu commands */
    private Command cmdAligmentLEFT   = null;
    private Command cmdAligmentRIGHT  = null;
    private Command cmdAligmentCENTER = null;

    /** PASSWORD menu commands */
    private Command cmdPassword   = null;

    /** Wrapping menu command */
    private Command cmdWrapYES  = null;
    private Command cmdWrapNO   = null;

    /** Scrolling commands */
    private Command cmdScrollVertical   = null;
    private Command cmdScrollHorizontal = null;
    private Command cmdScrollBOTH       = null;
    private Command cmdScrollNONE       = null;

    /* (non-Javadoc)
     * @see com.nokia.test.java.eswt.midlet.TestBaseMIDlet#init()
     */
    protected void init()
    {
        // set title
        this.shell.setText("Text Test");

        // create and set the layout
        this.shell.setLayout(new GridLayout());

        // create menu

        // Do menu
        Command cmdDo = new Command(this.shell, Command.COMMANDGROUP, 1);
        cmdDo.setText("Do");

        this.cmdDoAppend = new Command(cmdDo, Command.GENERAL, 1);
        this.cmdDoAppend.setText("Append");
        this.cmdDoAppend.addSelectionListener(this);

        this.cmdDoInsert = new Command(cmdDo, Command.GENERAL, 1);
        this.cmdDoInsert.setText("Insert");
        this.cmdDoInsert.addSelectionListener(this);

        // Settings menu
        Command cmdSettings = new Command(this.shell, Command.COMMANDGROUP, 0);
        cmdSettings.setText("Settings");

        // Aligment submenu
        Command cmdAligment = new Command(cmdSettings, Command.COMMANDGROUP, 4);
        cmdAligment.setText("Aligment");

        this.cmdAligmentLEFT = new Command(cmdAligment, Command.GENERAL, 2);
        this.cmdAligmentLEFT.setText("LEFT");
        this.cmdAligmentLEFT.addSelectionListener(this);

        this.cmdAligmentCENTER = new Command(cmdAligment, Command.GENERAL, 1);
        this.cmdAligmentCENTER.setText("CENTER");
        this.cmdAligmentCENTER.addSelectionListener(this);

        this.cmdAligmentRIGHT = new Command(cmdAligment, Command.GENERAL, 0);
        this.cmdAligmentRIGHT.setText("RIGHT");
        this.cmdAligmentRIGHT.addSelectionListener(this);

        // Mode submenu
        Command cmdMode = new Command(cmdSettings, Command.COMMANDGROUP, 3);
        cmdMode.setText("Mode");

        this.cmdModeEditable = new Command(cmdMode, Command.GENERAL, 1);
        this.cmdModeEditable.setText("EDITABLE");
        this.cmdModeEditable.addSelectionListener(this);

        this.cmdModeReadOnly = new Command(cmdMode, Command.GENERAL, 0);
        this.cmdModeReadOnly.setText("READ_ONLY");
        this.cmdModeReadOnly.addSelectionListener(this);

        this.cmdPassword = new Command(cmdMode, Command.GENERAL, 1);
        this.cmdPassword.setText("PASSWORD");
        this.cmdPassword.addSelectionListener(this);

        // Text Limit submenu
        Command cmdLimit = new Command(cmdSettings, Command.COMMANDGROUP, 2);
        cmdLimit.setText("Limit");

        this.cmdLimitCustom = new Command(cmdLimit, Command.GENERAL, 1);
        this.cmdLimitCustom.setText("CUSTOM");
        this.cmdLimitCustom.addSelectionListener(this);

        this.cmdLimitNONE = new Command(cmdLimit, Command.GENERAL, 0);
        this.cmdLimitNONE.setText("NONE");
        this.cmdLimitNONE.addSelectionListener(this);

        // Wrapping submenu
        Command cmdWrap = new Command(cmdSettings, Command.COMMANDGROUP, 1);
        cmdWrap.setText("Wrap");

        this.cmdWrapYES = new Command(cmdWrap, Command.GENERAL, 1);
        this.cmdWrapYES.setText("YES");
        this.cmdWrapYES.addSelectionListener(this);

        this.cmdWrapNO = new Command(cmdWrap, Command.GENERAL, 0);
        this.cmdWrapNO.setText("NO");
        this.cmdWrapNO.addSelectionListener(this);

        // scrolling submenu
        Command cmdScroll = new Command(cmdSettings, Command.COMMANDGROUP, 0);
        cmdScroll.setText("Scroll Bar");

        this.cmdScrollNONE = new Command(cmdScroll, Command.GENERAL, 3);
        this.cmdScrollNONE.setText("NONE");
        this.cmdScrollNONE.addSelectionListener(this);

        this.cmdScrollHorizontal = new Command(cmdScroll, Command.GENERAL, 2);
        this.cmdScrollHorizontal.setText("HORIZONTAL");
        this.cmdScrollHorizontal.addSelectionListener(this);

        this.cmdScrollVertical = new Command(cmdScroll, Command.GENERAL, 1);
        this.cmdScrollVertical.setText("VERTICAL");
        this.cmdScrollVertical.addSelectionListener(this);

        this.cmdScrollBOTH = new Command(cmdScroll, Command.GENERAL, 0);
        this.cmdScrollBOTH.setText("BOTH");
        this.cmdScrollBOTH.addSelectionListener(this);

        Label sblabel = new Label(this.shell, SWT.NONE);
        sblabel.setText("Scroll bars");
        sblabel.setLayoutData(new GridData(SWT.LEFT, SWT.CENTER, false, false));

        // multi
        this.multiField = new Text(this.shell, SWT.BORDER | SWT.MULTI |SWT.V_SCROLL|SWT.H_SCROLL);
        this.multiField.setLayoutData(new GridData(SWT.FILL, SWT.CENTER, true, false));
        this.multiField.setText("Here is Text with long text\n"+
                                "with some line feeds \n"+
                                "and scrollbars. \n"+
                                "It's for testing scrolling\n"+
                                "in multi Text.\n"+
                                "Scrolling should go ok\n"+
                                "both up and down.\n"+
                                "Scrolling is done by using\n"+
                                "scroll bars \n"+
                                "and tapping and dragging\n"+
                                "text content.\n"+
                                "Hopefully testing goes ok!");

        // create field Labels
        Label slabel = new Label(this.shell, SWT.NONE);
        slabel.setText("Password");
        slabel.setLayoutData(new GridData(SWT.LEFT, SWT.CENTER, false, false));

        // create Text fields
        // single
        this.passwordField = new Text(this.shell, SWT.BORDER | SWT.SINGLE | SWT.PASSWORD);
        // create and set the layout data
        this.passwordField.setLayoutData(new GridData(SWT.FILL, SWT.CENTER, true, false));

        Label mlabel = new Label(this.shell, SWT.NONE);

        mlabel.setText("ReadOnly");
        mlabel.setLayoutData(new GridData(SWT.LEFT, SWT.CENTER, false, false));

        this.readonlyField = new Text(this.shell, SWT.BORDER | SWT.SINGLE | SWT.READ_ONLY);
        // create and set the layout data
        this.readonlyField.setLayoutData(new GridData(SWT.FILL, SWT.CENTER, true, false));


    }

    /**
     * Change the style of the field if needed.
     * The styles defined by mask parameter will be cleaned up (unset)
     * before new style (style parameter) will be applied.
     * @param field A Text control
     * @param mask Set of styles to be cleaned
     * @param change A style constant to be set
     */
    private void changeStyle(Text field, int mask, int change)
    {
        if (field == null || field.isDisposed()) return;

        int style = field.getStyle();
        if ((style & change) != change)  // need to update the style
        {
            // update the style
            style &= ~mask;
            style ^= change;

            field = this.replaceField(field, style);

            // update the layout and redraw
            this.shell.layout();
            this.shell.update();
        }
    }

    /**
     * Set the wrap on or off.
     * @param wrapOn if true, wrapping will be switched on and off otherwise.
     */
    private void setWrap(Text field, boolean wrapOn)
    {
        int style = field.getStyle();
        style &= ~SWT.WRAP;

        if (wrapOn)
        {
            style |= SWT.WRAP;
        }

        field = this.replaceField(field, style);

        this.shell.layout();
        this.shell.update();
    }

    /**
     * Set the wrap on or off.
     * @param wrapOn if true, wrapping will be switched on and off otherwise.
     */
    private void setPassword(Text field, boolean PasswordOn)
    {
        int style = field.getStyle();
        style &= ~SWT.PASSWORD;

        if (PasswordOn)
        {
            style |= SWT.PASSWORD;
        }

        field = this.replaceField(field, style);

        this.shell.layout();
        this.shell.update();
    }
    /**
     * Set the scrollbar(s) for specified field.
     * @param field a Text field
     * @param type type of the scrollbar(s) to be set
     */
    private void setScrollBar(Text field, int type)
    {
        // clean up the scroll bars
        int style = field.getStyle();
        style &= ~SWT.V_SCROLL;
        style &= ~SWT.H_SCROLL;
        // set new scrollbar(s)
        if (type != SWT.NONE)
        {
            style |= type;
        }

        // replace the field
        field = this.replaceField(field, style);

        // update the shell
        this.shell.layout();
        this.shell.update();
    }

    /**
     * Replace specified field with new one.
     * Creates new field with defined style and copies some parameters along
     * with data from the specified field.
     * @param field a field containing data
     * @return created field
     */
    private Text replaceField(Text field, int style)
    {
        // store current state of the field
        String text = field.getText();
        int limit = field.getTextLimit();
        Point selection = field.getSelection();
        GridData data = (GridData) field.getLayoutData();

        // create new field
        Text nfield = new Text(this.shell, style);
        nfield.setText(text);
        nfield.setTextLimit(limit);
        nfield.setSelection(selection);
        nfield.setLayoutData(data);
        nfield.setFocus();
        // dispose old field
        field.dispose();

        return nfield;
    }

    /*
     *  (non-Javadoc)
     * @see org.eclipse.swt.events.SelectionListener#widgetSelected(org.eclipse.swt.events.SelectionEvent)
     */
    public void widgetSelected(SelectionEvent e)
    {
        if (!(e.widget instanceof Command)) return;

        Text field = this.getFocusedField();

        // Do commands handling
        if (e.widget == this.cmdDoAppend)
        {
            // prompt for a text
            QueryDialog dlg = new QueryDialog(this.shell, SWT.APPLICATION_MODAL, QueryDialog.STANDARD);
            dlg.setPromptText("Input some text:", "default");
            dlg.setMaximum(15);
            String text = dlg.open();
            if (text != null)
            {
                field.append(text);
            }
            return;
        }
        if (e.widget == this.cmdDoInsert)
        {
            // prompt for a text
            QueryDialog dlg = new QueryDialog(this.shell, SWT.APPLICATION_MODAL, QueryDialog.STANDARD);
            dlg.setPromptText("Input some text:", "default");
            dlg.setMaximum(15);
            String text = dlg.open();
            if (text != null)
            {
                field.insert(text);
            }
            return;
        }

        // Settings commands handling
        // aligment
        if (e.widget == this.cmdAligmentLEFT)
        {
            int mask = SWT.NONE | SWT.CENTER | SWT.RIGHT;
            this.changeStyle(field, mask, SWT.LEFT);
            return;
        }
        if (e.widget == this.cmdAligmentCENTER)
        {
            int mask = SWT.NONE | SWT.LEFT | SWT.RIGHT;
            this.changeStyle(field, mask, SWT.CENTER);
            return;
        }
        if (e.widget == this.cmdAligmentRIGHT)
        {
            int mask = SWT.NONE | SWT.LEFT | SWT.CENTER;
            this.changeStyle(field, mask, SWT.RIGHT);
            return;
        }
        // mode
        if (e.widget == this.cmdModeEditable)
        {
            field.setEditable(true);
            return;
        }
        if (e.widget == this.cmdModeReadOnly)
        {
            field.setEditable(false);
            return;
        }
        if (e.widget == this.cmdPassword)
        {
            this.setPassword(this.getFocusedField(), true);
            return;
        }
        // text limit
        if (e.widget == this.cmdLimitCustom)
        {
            QueryDialog dlg = new QueryDialog(this.shell, SWT.APPLICATION_MODAL, QueryDialog.NUMERIC);
            dlg.setPromptText("Input a number:", "10");
            dlg.setMaximum(2);
            String res = dlg.open();
            if (res != null && !("".equals(res)))
            {
                field.setTextLimit(Integer.parseInt(res));
            }
            return;
        }
        if (e.widget == this.cmdLimitNONE)
        {
            field.setTextLimit(Text.LIMIT);
            return;
        }
        // wrapping
        if (e.widget == this.cmdWrapYES)
        {
            this.setWrap(this.getFocusedField(), true);
            return;
        }
        if (e.widget == this.cmdWrapNO)
        {
            this.setWrap(this.getFocusedField(), false);
            return;
        }
        // scrolling
        if (e.widget == this.cmdScrollNONE)
        {
            this.setScrollBar(field, SWT.NONE);
            return;
        }
        if (e.widget == this.cmdScrollVertical)
        {
            this.setScrollBar(field, SWT.V_SCROLL);
            return;
        }
        if (e.widget == this.cmdScrollBOTH)
        {
            this.setScrollBar(field, SWT.H_SCROLL | SWT.V_SCROLL);
            return;
        }
        if (e.widget == this.cmdScrollHorizontal)
        {
            this.setScrollBar(field, SWT.H_SCROLL);
            return;
        }

        super.widgetSelected(e);
    }

    /**
     * Return currently focused Text control.
     * @return currently selected Text field, null otherwise.
     */
    private Text getFocusedField()
    {
        Control control = this.shell.getDisplay().getFocusControl();
        if (control instanceof Text)
            return (Text) control;
        else
            return null;
    }
}


