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

package com.nokia.test.java.eswt.layout;

import org.eclipse.ercp.swt.mobile.Command;
import org.eclipse.ercp.swt.mobile.QueryDialog;
import org.eclipse.swt.SWT;
import org.eclipse.swt.events.SelectionEvent;
import org.eclipse.swt.graphics.Point;
import org.eclipse.swt.layout.FormAttachment;
import org.eclipse.swt.layout.FormData;
import org.eclipse.swt.layout.FormLayout;
import org.eclipse.swt.widgets.Control;
import org.eclipse.swt.widgets.Label;
import org.eclipse.swt.widgets.List;
import org.eclipse.swt.widgets.MessageBox;
import org.eclipse.swt.widgets.Slider;
import org.eclipse.swt.widgets.Text;

import com.nokia.test.java.eswt.midlet.TestBaseMIDlet;

/**
 *
 */
public class LayoutTestMIDlet_05 extends TestBaseMIDlet
{
    /** layout */
    protected FormLayout layout = null;

    /** current attachment rule */
    protected int autoAttach = SWT.NONE;

    /** last created control */
    protected Control control = null;

    /** commands */
    protected Command cmdAddLABEL   = null;
    protected Command cmdAddTEXT    = null;
    protected Command cmdAddSLIDER  = null;
    protected Command cmdAddLIST    = null;

    protected Command cmdAttachTOP      = null;
    protected Command cmdAttachBOTTOM   = null;
    protected Command cmdAttachLEFT     = null;
    protected Command cmdAttachRIGHT    = null;

    protected Command cmdMarginTOP      = null;
    protected Command cmdMarginBOTTOM   = null;
    protected Command cmdMarginLEFT     = null;
    protected Command cmdMarginRIGHT    = null;

    protected Command cmdSpacing        = null;

    /* (non-Javadoc)
     * @see com.nokia.test.java.eswt.midlet.TestBaseMIDlet#init()
     */
    protected void init()
    {
        // setup the shell
        this.shell.setText("Form Layout Test");
        this.layout = new FormLayout();
        this.shell.setLayout(this.layout);

        // create commands
        Command cmdAdd = new Command(this.shell, Command.COMMANDGROUP, 0);
        cmdAdd.setText("Add");

        this.cmdAddLABEL = new Command(cmdAdd, Command.SELECT, 3);
        this.cmdAddLABEL.setText("Label");
        this.cmdAddLABEL.addSelectionListener(this);

        this.cmdAddTEXT = new Command(cmdAdd, Command.SELECT, 2);
        this.cmdAddTEXT.setText("Text");
        this.cmdAddTEXT.addSelectionListener(this);

        this.cmdAddSLIDER = new Command(cmdAdd, Command.SELECT, 1);
        this.cmdAddSLIDER.setText("Slider");
        this.cmdAddSLIDER.addSelectionListener(this);

        this.cmdAddLIST = new Command(cmdAdd, Command.SELECT, 0);
        this.cmdAddLIST.setText("List");
        this.cmdAddLIST.addSelectionListener(this);

        Command cmdSettings = new Command(this.shell, Command.COMMANDGROUP, 0);
        cmdSettings.setText("Settings");

        Command cmdAttach = new Command(cmdSettings, Command.COMMANDGROUP, 2);
        cmdAttach.setText("Auto attach");

        this.cmdAttachTOP = new Command(cmdAttach, Command.GENERAL, 3);
        this.cmdAttachTOP.setText("top");
        this.cmdAttachTOP.addSelectionListener(this);

        this.cmdAttachBOTTOM = new Command(cmdAttach, Command.GENERAL, 2);
        this.cmdAttachBOTTOM.setText("bottom");
        this.cmdAttachBOTTOM.addSelectionListener(this);

        this.cmdAttachLEFT = new Command(cmdAttach, Command.GENERAL, 1);
        this.cmdAttachLEFT.setText("left");
        this.cmdAttachLEFT.addSelectionListener(this);

        this.cmdAttachRIGHT = new Command(cmdAttach, Command.GENERAL, 0);
        this.cmdAttachRIGHT.setText("right");
        this.cmdAttachRIGHT.addSelectionListener(this);

        Command cmdMargin = new Command(cmdSettings, Command.COMMANDGROUP, 1);
        cmdMargin.setText("Margin");

        this.cmdMarginTOP = new Command(cmdMargin, Command.GENERAL, 3);
        this.cmdMarginTOP.setText("top");
        this.cmdMarginTOP.addSelectionListener(this);

        this.cmdMarginBOTTOM = new Command(cmdMargin, Command.GENERAL, 2);
        this.cmdMarginBOTTOM.setText("bottom");
        this.cmdMarginBOTTOM.addSelectionListener(this);

        this.cmdMarginLEFT = new Command(cmdMargin, Command.GENERAL, 1);
        this.cmdMarginLEFT.setText("left");
        this.cmdMarginLEFT.addSelectionListener(this);

        this.cmdMarginRIGHT = new Command(cmdMargin, Command.GENERAL, 0);
        this.cmdMarginRIGHT.setText("right");
        this.cmdMarginRIGHT.addSelectionListener(this);

        this.cmdSpacing = new Command(cmdSettings, Command.GENERAL, 0);
        this.cmdSpacing.setText("Spacing");
        this.cmdSpacing.addSelectionListener(this);
    }

    /* (non-Javadoc)
     * @see com.nokia.test.java.eswt.midlet.TestBaseMIDlet#widgetSelected(org.eclipse.swt.events.SelectionEvent)
     */
    public void widgetSelected(SelectionEvent e)
    {
        // add commands
        if (e.widget == this.cmdAddLABEL)
        {
            Label control = new Label(this.shell, SWT.BORDER);
            control.setText("Label");
            if (!this.setLayoutData(control))
            {
                this.autoAttach(control, null);
            }
            control.pack();
            this.shell.layout();
            this.control = control;
            return;
        }
        if (e.widget == this.cmdAddTEXT)
        {
            Text control = new Text(this.shell, SWT.BORDER);
            control.setText("Text control");
            if (!this.setLayoutData(control))
            {
                this.autoAttach(control, null);
            }
            control.pack();
            this.shell.layout();
            this.control = control;
            return;
        }
        if (e.widget == this.cmdAddSLIDER)
        {
            Slider control = new Slider(this.shell, SWT.BORDER);
            if (!this.setLayoutData(control))
            {
                this.autoAttach(control, null);
            }
            this.shell.layout();
            this.control = control;
            return;
        }
        if (e.widget == this.cmdAddLIST)
        {
            List control = new List(this.shell, SWT.BORDER);
            control.setItems(new String[] {"Item 1","Item 2","Item 3"});
            control.setSize(100, 100);
            if (!this.setLayoutData(control))
            {
                this.autoAttach(control, control.getSize());
            }
            this.shell.layout();
            this.control = control;
            return;
        }

        // attach commands
        if (e.widget == this.cmdAttachTOP)
        {
            this.autoAttach = SWT.TOP;
            return;
        }
        if (e.widget == this.cmdAttachBOTTOM)
        {
            this.autoAttach = SWT.BOTTOM;
            return;
        }
        if (e.widget == this.cmdAttachLEFT)
        {
            this.autoAttach = SWT.LEFT;
            return;
        }
        if (e.widget == this.cmdAttachRIGHT)
        {
            this.autoAttach = SWT.RIGHT;
            return;
        }

        // set margin commands
        if (e.widget == this.cmdMarginTOP)
        {
            int value = this.numQuery("Top Marging:", this.layout.marginTop);
            if (value != this.layout.marginTop)
            {
                this.layout.marginTop = value;
                this.shell.layout();
                this.shell.update();
            }
            return;
        }
        if (e.widget == this.cmdMarginBOTTOM)
        {
            int value = this.numQuery("Bottom Marging:", this.layout.marginBottom);
            if (value != this.layout.marginBottom)
            {
                this.layout.marginBottom = value;
                this.shell.layout();
                this.shell.update();
            }
            return;
        }
        if (e.widget == this.cmdMarginLEFT)
        {
            int value = this.numQuery("Left Marging:", this.layout.marginLeft);
            if (value != this.layout.marginLeft)
            {
                this.layout.marginLeft = value;
                this.shell.layout();
                this.shell.update();
            }
            return;
        }
        if (e.widget == this.cmdMarginRIGHT)
        {
            int value = this.numQuery("Right Marging:", this.layout.marginRight);
            if (value != this.layout.marginRight)
            {
                this.layout.marginRight = value;
                this.shell.layout();
                this.shell.update();
            }
            return;
        }

        // spacing command
        if (e.widget == this.cmdSpacing)
        {
            int value = this.numQuery("Spacing:", this.layout.spacing);
            if (value != this.layout.spacing)
            {
                this.layout.spacing = value;
                this.shell.layout();
                this.shell.update();
            }
            return;
        }

        super.widgetSelected(e);
    }

    /**
     * Prompt a user for layout data (FormData).
     * First, the user is asked does he want to set a layout data. If he does,
     * a FormDataConfig dialog is shown. After the user specifies
     * all required values and clicks OK, a FormData object is created and
     * set to the specified control.
     * @param control a Control to which created FormData is set
     */
    protected boolean setLayoutData(Control control)
    {
        // prompt a user does he want to set FormData for a control
        MessageBox msg =
            new MessageBox(this.shell, SWT.ICON_QUESTION | SWT.YES | SWT.NO);
        msg.setMessage("Would you like to set a layout data for the control?");
        if (msg.open() == SWT.YES)
        {
            // prompt for layout data
            FormDataConfigDialog dlg = new FormDataConfigDialog(this.shell);
            FormData data = dlg.open();
            if (data != null)
            {
                control.setLayoutData(data);
                return true;
            }
        }

        return false;
    }

    /**
     *
     */
    protected void autoAttach(Control control, Point size)
    {
        FormData data = new FormData();
        if (size != null)
        {
            data.width  = size.x;
            data.height = size.y;
        }

        if (this.control != null && !this.control.isDisposed())
        {
            switch (this.autoAttach)
            {
            case SWT.TOP:
            {
                data.bottom = new FormAttachment(this.control, 0, SWT.TOP);
                data.left   = new FormAttachment(this.control, 0, SWT.LEFT);
                break;
            }

            case SWT.BOTTOM:
            {
                data.top    = new FormAttachment(this.control, 0, SWT.BOTTOM);
                data.left   = new FormAttachment(this.control, 0, SWT.LEFT);
                break;
            }

            case SWT.LEFT:
            {
                data.right = new FormAttachment(this.control, 0, SWT.LEFT);
                data.top    = new FormAttachment(this.control, 0, SWT.TOP);
                break;
            }

            case SWT.RIGHT:
                data.left = new FormAttachment(this.control, 0, SWT.RIGHT);
                data.top    = new FormAttachment(this.control, 0, SWT.TOP);
                break;
            }
        }
        control.setLayoutData(data);
    }

    /**
     *
     */
    protected int numQuery(String msg, int value)
    {
        int result = value;
        QueryDialog dlg =
            new QueryDialog(this.shell, SWT.NONE, QueryDialog.NUMERIC);
        dlg.setMaximum(3);
        dlg.setPromptText(msg, String.valueOf(value));
        String res = dlg.open();
        if (res != null)
        {
            result = Integer.parseInt(res);
        }

        return result;
    }
}