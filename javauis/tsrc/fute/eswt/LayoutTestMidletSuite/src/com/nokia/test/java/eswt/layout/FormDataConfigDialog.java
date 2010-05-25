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
package com.nokia.test.java.eswt.layout;

import org.eclipse.ercp.swt.mobile.Command;
import org.eclipse.ercp.swt.mobile.QueryDialog;
import org.eclipse.swt.SWT;
import org.eclipse.swt.events.SelectionEvent;
import org.eclipse.swt.events.SelectionListener;
import org.eclipse.swt.graphics.Font;
import org.eclipse.swt.graphics.FontData;
import org.eclipse.swt.layout.FormAttachment;
import org.eclipse.swt.layout.FormData;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.widgets.Control;
import org.eclipse.swt.widgets.Label;
import org.eclipse.swt.widgets.Shell;

/**
 * A modal dialog that is used to configure a FormData.
 */
public class FormDataConfigDialog implements SelectionListener
{
    /** parent shell*/
    protected Shell parent = null;
    /** The dialog's shell */
    protected Shell shell = null;

    /** GridData object */
    protected FormData data = null;

    /** Fonts */
    protected Font titleFont = null;
    protected Font valueFont = null;

    /** Success flag */
    protected boolean success = false;

    /** Config fields */
    protected Label widthInfo   = null;
    protected Label heightInfo  = null;

    protected Label attTopInfo      = null;
    protected Label attBottomInfo   = null;
    protected Label attLeftInfo     = null;
    protected Label attRightInfo    = null;

    /** commands */
    protected Command cmdSetWIDTH   = null;
    protected Command cmdSetHEIGHT  = null;

    protected Command cmdAttachTOP      = null;
    protected Command cmdAttachBOTTOM   = null;
    protected Command cmdAttachLEFT     = null;
    protected Command cmdAttachRIGHT    = null;

    /** Clear, Done and Cancel commands */
    private Command cmdReset    = null;
    private Command cmdDone     = null;
    private Command cmdCancel   = null;

    /**
     * Constructor.
     * @param parent A parent shell
     */
    public FormDataConfigDialog(Shell parent)
    {
        this.parent = parent;
        this.shell = new Shell(this.parent, SWT.DIALOG_TRIM);
        this.shell.setText("Form Data Config");
        this.shell.setLayout(new GridLayout(2, false));

        // create commands
        this.cmdCancel = new Command(this.shell, Command.CANCEL, 0);
        this.cmdCancel.setText("Cancel");
        this.cmdCancel.addSelectionListener(this);

        this.cmdDone = new Command(this.shell, Command.OK, 0);
        this.cmdDone.setText("Done");
        this.cmdDone.addSelectionListener(this);

        this.cmdReset = new Command(this.shell, Command.GENERAL, 1);
        this.cmdReset.setText("Reset");
        this.cmdReset.addSelectionListener(this);

        // Set commands
        Command cmdSet = new Command(this.shell, Command.COMMANDGROUP, 2);
        cmdSet.setText("Set");

        Command cmdAttach = new Command(cmdSet, Command.COMMANDGROUP, 2);
        cmdAttach.setText("Attach");

        this.cmdAttachTOP = new Command(cmdAttach, Command.SELECT, 3);
        this.cmdAttachTOP.setText("top");
        this.cmdAttachTOP.addSelectionListener(this);

        this.cmdAttachBOTTOM = new Command(cmdAttach, Command.SELECT, 2);
        this.cmdAttachBOTTOM.setText("bottom");
        this.cmdAttachBOTTOM.addSelectionListener(this);

        this.cmdAttachLEFT = new Command(cmdAttach, Command.SELECT, 1);
        this.cmdAttachLEFT.setText("left");
        this.cmdAttachLEFT.addSelectionListener(this);

        this.cmdAttachRIGHT = new Command(cmdAttach, Command.SELECT, 0);
        this.cmdAttachRIGHT.setText("right");
        this.cmdAttachRIGHT.addSelectionListener(this);

        this.cmdSetWIDTH = new Command(cmdSet, Command.SELECT, 1);
        this.cmdSetWIDTH.setText("Width");
        this.cmdSetWIDTH.addSelectionListener(this);

        this.cmdSetHEIGHT = new Command(cmdSet, Command.SELECT, 0);
        this.cmdSetHEIGHT.setText("Height");
        this.cmdSetHEIGHT.addSelectionListener(this);
    }

    /**
     * Open the dialog.
     * @return return created GridData or null if a user
     * has canceled the dialog
     */
    public FormData open()
    {
        // set font
        FontData[] fdata = this.parent.getFont().getFontData();
        Font font = new Font(this.parent.getDisplay(), fdata);
        this.shell.setFont(font);

        // create custom fonts
        this.titleFont = new Font(this.parent.getDisplay(),
                                  new FontData("Arial", 6, SWT.ITALIC));
        this.valueFont = new Font(this.parent.getDisplay(),
                                  new FontData("Arial", 6, SWT.NORMAL));

        // cleaning
        this.reset();

        // open the shell
        this.shell.setBounds(this.parent.getBounds());
        this.shell.layout();
        this.shell.open();

        // event loop
        while (!this.shell.isDisposed())
            if (!this.shell.getDisplay().readAndDispatch())
                this.shell.getDisplay().sleep();

        // dispose fonts
        font.dispose();
        this.titleFont.dispose();
        this.valueFont.dispose();

        if (!this.success)  //user has canceled the dialog
        {
            return null;
        }
        return this.data;
    }

    /**
     *
     */
    protected void reset()
    {
        // clean the shell
        Control[] controls = this.shell.getChildren();
        for (int i=0; i<controls.length; i++)
        {
            controls[i].dispose();
        }

        // create FormData
        this.data = new FormData();

        // create config info fields
        this.widthInfo =
            this.createInfoLabel("Width: ", String.valueOf(this.data.width));
        this.heightInfo =
            this.createInfoLabel("Height: ", String.valueOf(this.data.width));
        this.attTopInfo =
            this.createInfoLabel("Top attachment: ", "none");
        this.attBottomInfo =
            this.createInfoLabel("Bottom attachment: ", "none");
        this.attLeftInfo =
            this.createInfoLabel("Left attachment: ", "none");
        this.attRightInfo =
            this.createInfoLabel("Right attachment: ", "none");

        // update
        this.shell.layout();
        this.shell.update();
    }

    /**
     * Create a Label control with preceding title.
     * @param title a String to be used as a title
     * @param value a value for the info label
     * @return created Label control
     */
    protected Label createInfoLabel(String title, String value)
    {
        Label label = new Label(this.shell, SWT.NONE);
        label.setText(title);
        label.setFont(this.titleFont);
        Label info = new Label(this.shell, SWT.NONE);
        info.setText(value);
        info.setFont(this.valueFont);

        this.shell.layout();
        return info;
    }

    /* (non-Javadoc)
     * @see org.eclipse.swt.events.SelectionListener#widgetDefaultSelected(org.eclipse.swt.events.SelectionEvent)
     */
    public void widgetDefaultSelected(SelectionEvent arg0)
    {
    }

    /* (non-Javadoc)
     * @see org.eclipse.swt.events.SelectionListener#widgetSelected(org.eclipse.swt.events.SelectionEvent)
     */
    public void widgetSelected(SelectionEvent e)
    {
        // Set commands
        if (e.widget == this.cmdSetWIDTH)
        {
            Integer value = this.numQuery("Width:", this.data.width);
            if (value != null)
            {
                this.data.width = value.intValue();
                // update info label
                this.widthInfo.setText(String.valueOf(value));
                this.widthInfo.pack();
            }
            return;
        }
        if (e.widget == this.cmdSetHEIGHT)
        {
            Integer value = this.numQuery("Height:", this.data.height);
            if (value != null)
            {
                this.data.height = value.intValue();
                // update info label
                this.heightInfo.setText(String.valueOf(value));
                this.heightInfo.pack();
            }
            return;
        }

        // attach commands
        if (e.widget == this.cmdAttachTOP)
        {
            this.data.top = this.getAttachment(this.data.top);
            // update info label
            this.attTopInfo.setText("set");
            return;
        }
        if (e.widget == this.cmdAttachBOTTOM)
        {
            this.data.bottom = this.getAttachment(this.data.bottom);
            // update info label
            this.attBottomInfo.setText("set");
            return;
        }
        if (e.widget == this.cmdAttachLEFT)
        {
            this.data.left = this.getAttachment(this.data.left);
            // update info label
            this.attLeftInfo.setText("set");
            return;
        }
        if (e.widget == this.cmdAttachRIGHT)
        {
            this.data.right = this.getAttachment(this.data.right);
            // update info label
            this.attRightInfo.setText("set");
            return;
        }

        // Clear command
        if (e.widget == this.cmdReset)
        {
            this.reset();
            return;
        }

        // Done command
        if (e.widget == this.cmdDone)
        {
            this.success = true;
            this.shell.dispose();
            return;
        }

        // Cancel command
        if (e.widget == this.cmdCancel)
        {
            this.success = false;
            this.shell.dispose();
            return;
        }
    }

    /**
     *
     * @param msg
     * @param value
     * @return
     */
    protected Integer numQuery(String msg, int value)
    {
        // workaround a bug in QueryDialog
        if (value < 0)
        {
            value = 0; //otherwise QueryDialog throws invalid argument exception
        }
        Integer result = null;
        QueryDialog dlg =
            new QueryDialog(this.shell, SWT.NONE, QueryDialog.NUMERIC);
        dlg.setMaximum(3);
        dlg.setPromptText(msg, String.valueOf(value));
        String res = dlg.open();
        if (res != null)
        {
            result = Integer.valueOf(res);
        }

        return result;
    }

    /**
     *
     */
    protected FormAttachment getAttachment(FormAttachment attachment)
    {
        if (attachment == null)
        {
            attachment = new FormAttachment(0, 0);
        }

        Integer numerator =
            this.numQuery("numerator (%):", attachment.numerator);
        if (numerator != null)
        {
            attachment.numerator = numerator.intValue();
        }

        Integer offset =
            this.numQuery("offset:", attachment.offset);
        if (offset != null)
        {
            attachment.offset = offset.intValue();
        }

        return attachment;
    }
}
