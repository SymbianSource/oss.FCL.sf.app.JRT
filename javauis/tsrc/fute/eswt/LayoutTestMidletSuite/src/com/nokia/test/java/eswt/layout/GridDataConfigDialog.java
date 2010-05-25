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
import org.eclipse.ercp.swt.mobile.ConstrainedText;
import org.eclipse.swt.SWT;
import org.eclipse.swt.events.SelectionEvent;
import org.eclipse.swt.events.SelectionListener;
import org.eclipse.swt.graphics.Font;
import org.eclipse.swt.graphics.FontData;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.widgets.Control;
import org.eclipse.swt.widgets.Label;
import org.eclipse.swt.widgets.Shell;

/**
 * A modal dialog that is used to configure a GridData.
 */
public class GridDataConfigDialog implements SelectionListener
{
    /** parent shell*/
    private Shell parent = null;
    /** The dialog's shell */
    private Shell shell = null;

    /** GridData object */
    private GridData data = null;

    /** Fonts */
    private Font titleFont = null;
    private Font valueFont = null;

    /** Success flag */
    private boolean success = false;

    /** Config fields */
    private Label vAlignInfo        = null;
    private Label hAlignInfo        = null;
    private Label vGrabInfo         = null;
    private Label hGrabInfo         = null;

    private ConstrainedText wHintField      = null;
    private ConstrainedText hHintField      = null;
    private ConstrainedText vIndentField    = null;
    private ConstrainedText hIndentField    = null;
    private ConstrainedText vSpanField      = null;
    private ConstrainedText hSpanField      = null;

    /** Commands */
    /** Alignment */
    private Command cmdAlign_V_TOP      = null;
    private Command cmdAlign_V_CENTER   = null;
    private Command cmdAlign_V_BOTTOM   = null;
    private Command cmdAlign_V_FILL     = null;
    private Command cmdAlign_H_LEFT     = null;
    private Command cmdAlign_H_CENTER   = null;
    private Command cmdAlign_H_RIGHT    = null;
    private Command cmdAlign_H_FILL     = null;
    /** Hint */
    private Command cmdHint_W   = null;
    private Command cmdHint_H   = null;
    /** Indent */
    private Command cmdIndent_V = null;
    private Command cmdIndent_H = null;
    /** Span */
    private Command cmdSpan_V   = null;
    private Command cmdSpan_H   = null;
    /** Grab Space */
    private Command cmdGrabV_TRUE   = null;
    private Command cmdGrabV_FALSE  = null;
    private Command cmdGrabH_TRUE   = null;
    private Command cmdGrabH_FALSE  = null;

    /** Clear, Done and Cancel commands */
    private Command cmdReset    = null;
    private Command cmdDone     = null;
    private Command cmdCancel   = null;

    /**
     * Constructor.
     * @param parent A parent shell
     */
    public GridDataConfigDialog(Shell parent)
    {
        this.parent = parent;
        this.shell = new Shell(this.parent, SWT.DIALOG_TRIM);
        this.shell.setText("Grid Data Config");
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

        // grabbing
        Command cmdGrab = new Command(cmdSet, Command.COMMANDGROUP, 0);
        cmdGrab.setText("Grab");
        // horizontal
        Command cmdGrabH = new Command(cmdGrab, Command.COMMANDGROUP, 0);
        cmdGrabH.setText("Horizontal");
        this.cmdGrabH_FALSE = new Command(cmdGrabH, Command.GENERAL, 0);
        this.cmdGrabH_FALSE.setText("false");
        this.cmdGrabH_FALSE.addSelectionListener(this);
        this.cmdGrabH_TRUE = new Command(cmdGrabH, Command.GENERAL, 1);
        this.cmdGrabH_TRUE.setText("true");
        this.cmdGrabH_TRUE.addSelectionListener(this);
        // vertical
        Command cmdGrabV = new Command(cmdGrab, Command.COMMANDGROUP, 1);
        cmdGrabV.setText("Vertical");
        this.cmdGrabV_FALSE = new Command(cmdGrabV, Command.GENERAL, 0);
        this.cmdGrabV_FALSE.setText("false");
        this.cmdGrabV_FALSE.addSelectionListener(this);
        this.cmdGrabV_TRUE = new Command(cmdGrabV, Command.GENERAL, 1);
        this.cmdGrabV_TRUE.setText("true");
        this.cmdGrabV_TRUE.addSelectionListener(this);

        // Span
        Command cmdSpan = new Command(cmdSet, Command.COMMANDGROUP, 1);
        cmdSpan.setText("Span");
        this.cmdSpan_H = new Command(cmdSpan, Command.GENERAL, 0);
        this.cmdSpan_H.setText("Horizontal");
        this.cmdSpan_H.addSelectionListener(this);
        this.cmdSpan_V = new Command(cmdSpan, Command.GENERAL, 1);
        this.cmdSpan_V.setText("Vertical");
        this.cmdSpan_V.addSelectionListener(this);

        // Indent
        Command cmdIndent = new Command(cmdSet, Command.COMMANDGROUP, 2);
        cmdIndent.setText("Indent");
        this.cmdIndent_H = new Command(cmdIndent, Command.GENERAL, 0);
        this.cmdIndent_H.setText("Horizontal");
        this.cmdIndent_H.addSelectionListener(this);
        this.cmdIndent_V = new Command(cmdIndent, Command.GENERAL, 1);
        this.cmdIndent_V.setText("Vertical");
        this.cmdIndent_V.addSelectionListener(this);

        // Hint
        Command cmdHint = new Command(cmdSet, Command.COMMANDGROUP, 3);
        cmdHint.setText("Hint");
        this.cmdHint_H = new Command(cmdHint, Command.GENERAL, 0);
        this.cmdHint_H.setText("height");
        this.cmdHint_H.addSelectionListener(this);
        this.cmdHint_W = new Command(cmdHint, Command.GENERAL, 1);
        this.cmdHint_W.setText("width");
        this.cmdHint_W.addSelectionListener(this);

        // Alignment
        Command cmdAlign = new Command(cmdSet, Command.COMMANDGROUP, 4);
        cmdAlign.setText("Alignment");
        // horizontal
        Command cmdAlign_H = new Command(cmdAlign, Command.COMMANDGROUP, 0);
        cmdAlign_H.setText("Horizontal");
        this.cmdAlign_H_FILL = new Command(cmdAlign_H, Command.GENERAL, 0);
        this.cmdAlign_H_FILL.setText("FILL");
        this.cmdAlign_H_FILL.addSelectionListener(this);
        this.cmdAlign_H_RIGHT = new Command(cmdAlign_H, Command.GENERAL, 1);
        this.cmdAlign_H_RIGHT.setText("RIGHT");
        this.cmdAlign_H_RIGHT.addSelectionListener(this);
        this.cmdAlign_H_CENTER = new Command(cmdAlign_H, Command.GENERAL, 2);
        this.cmdAlign_H_CENTER.setText("CENTER");
        this.cmdAlign_H_CENTER.addSelectionListener(this);
        this.cmdAlign_H_LEFT = new Command(cmdAlign_H, Command.GENERAL, 3);
        this.cmdAlign_H_LEFT.setText("LEFT");
        this.cmdAlign_H_LEFT.addSelectionListener(this);
        // vertical
        Command cmdAlign_V = new Command(cmdAlign, Command.COMMANDGROUP, 1);
        cmdAlign_V.setText("Vertical");
        this.cmdAlign_V_FILL = new Command(cmdAlign_V, Command.GENERAL, 0);
        this.cmdAlign_V_FILL.setText("FILL");
        this.cmdAlign_V_FILL.addSelectionListener(this);
        this.cmdAlign_V_BOTTOM = new Command(cmdAlign_V, Command.GENERAL, 1);
        this.cmdAlign_V_BOTTOM.setText("BOTTOM");
        this.cmdAlign_V_BOTTOM.addSelectionListener(this);
        this.cmdAlign_V_CENTER = new Command(cmdAlign_V, Command.GENERAL, 2);
        this.cmdAlign_V_CENTER.setText("CENTER");
        this.cmdAlign_V_CENTER.addSelectionListener(this);
        this.cmdAlign_V_TOP = new Command(cmdAlign_V, Command.GENERAL, 3);
        this.cmdAlign_V_TOP.setText("TOP");
        this.cmdAlign_V_TOP.addSelectionListener(this);
    }

    /**
     * Open the dialog.
     * @return return created GridData or null if a user
     * has canceled the dialog
     */
    public GridData open()
    {
        // set font
        FontData[] fdata = this.parent.getFont().getFontData();
        Font font = new Font(this.parent.getDisplay(), fdata);
        this.shell.setFont(font);
        // create GridData
        this.data = new GridData();

        // create custom fonts
        this.titleFont = new Font(this.parent.getDisplay(),
                                  new FontData("Arial", 6, SWT.ITALIC));
        this.valueFont = new Font(this.parent.getDisplay(),
                                  new FontData("Arial", 6, SWT.NORMAL));

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
     * Create a Label control with preceding title.
     * @param title a String to be used as a title
     * @param value a value for the info label
     * @return created Label control
     */
    private Label createInfoLabel(String title, String value)
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

    /**
     * Create numerical field with preceding title label and default value.
     * @param title a String for a title
     * @param value default value
     * @return created control
     */
    private ConstrainedText createNumField(String title, int value)
    {
        Label label = new Label(this.shell, SWT.NONE);
        label.setText(title);
        label.setFont(this.titleFont);
        ConstrainedText field =
            new ConstrainedText(this.shell, SWT.BORDER, ConstrainedText.NUMERIC);
        field.setText(String.valueOf(value));
        field.setFont(this.valueFont);

        this.shell.layout();
        return field;
    }

    /* (non-Javadoc)
     * @see org.eclipse.swt.events.SelectionListener#widgetSelected(org.eclipse.swt.events.SelectionEvent)
     */
    public void widgetSelected(SelectionEvent e)
    {
        // Setting commands
        // horizontalAlignment
        if (e.widget == this.cmdAlign_H_LEFT)
        {
            if (this.hAlignInfo == null || this.hAlignInfo.isDisposed())
            {
                this.hAlignInfo =
                    this.createInfoLabel("horizontalAlignment: ", "LEFT");
            }
            else
            {
                this.hAlignInfo.setText("LEFT");
                this.hAlignInfo.pack();
            }
            this.data.horizontalAlignment = SWT.LEFT;
            this.shell.update();
            return;
        }
        if (e.widget == this.cmdAlign_H_CENTER)
        {
            if (this.hAlignInfo == null || this.hAlignInfo.isDisposed())
            {
                this.hAlignInfo =
                    this.createInfoLabel("horizontalAlignment: ", "CENTER");
            }
            else
            {
                this.hAlignInfo.setText("CENTER");
                this.hAlignInfo.pack();
            }
            this.data.horizontalAlignment = SWT.CENTER;
            this.shell.update();
            return;
        }
        if (e.widget == this.cmdAlign_H_RIGHT)
        {
            if (this.hAlignInfo == null || this.hAlignInfo.isDisposed())
            {
                this.hAlignInfo =
                    this.createInfoLabel("horizontalAlignment: ", "RIGHT");
            }
            else
            {
                this.hAlignInfo.setText("RIGHT");
                this.hAlignInfo.pack();
            }
            this.data.horizontalAlignment = SWT.RIGHT;
            this.shell.update();
            return;
        }
        if (e.widget == this.cmdAlign_H_FILL)
        {
            if (this.hAlignInfo == null || this.hAlignInfo.isDisposed())
            {
                this.hAlignInfo =
                    this.createInfoLabel("horizontalAlignment: ", "FILL");
            }
            else
            {
                this.hAlignInfo.setText("FILL");
                this.hAlignInfo.pack();
            }
            this.data.horizontalAlignment = SWT.FILL;
            this.shell.update();
            return;
        }
        // verticalAlignment
        if (e.widget == this.cmdAlign_V_TOP)
        {
            if (this.vAlignInfo == null || this.vAlignInfo.isDisposed())
            {
                this.vAlignInfo =
                    this.createInfoLabel("verticalAlignment: ", "TOP");
            }
            else
            {
                this.vAlignInfo.setText("TOP");
                this.vAlignInfo.pack();
            }
            this.data.verticalAlignment = SWT.TOP;
            this.shell.update();
            return;
        }
        if (e.widget == this.cmdAlign_V_CENTER)
        {
            if (this.vAlignInfo == null || this.vAlignInfo.isDisposed())
            {
                this.vAlignInfo =
                    this.createInfoLabel("verticalAlignment: ", "CENTER");
            }
            else
            {
                this.vAlignInfo.setText("CENTER");
                this.vAlignInfo.pack();
            }
            this.data.verticalAlignment = SWT.CENTER;
            this.shell.update();
            return;
        }
        if (e.widget == this.cmdAlign_V_BOTTOM)
        {
            if (this.vAlignInfo == null || this.vAlignInfo.isDisposed())
            {
                this.vAlignInfo =
                    this.createInfoLabel("verticalAlignment: ", "BOTTOM");
            }
            else
            {
                this.vAlignInfo.setText("BOTTOM");
                this.vAlignInfo.pack();
            }
            this.data.verticalAlignment = SWT.BOTTOM;
            this.shell.update();
            return;
        }
        if (e.widget == this.cmdAlign_V_FILL)
        {
            if (this.vAlignInfo == null || this.vAlignInfo.isDisposed())
            {
                this.vAlignInfo =
                    this.createInfoLabel("verticalAlignment: ", "FILL");
            }
            else
            {
                this.vAlignInfo.setText("FILL");
                this.vAlignInfo.pack();
            }
            this.data.verticalAlignment = SWT.FILL;
            this.shell.update();
            return;
        }
        // hint
        if (e.widget == this.cmdHint_W)
        {
            if (this.wHintField == null || this.wHintField.isDisposed())
            {
                this.wHintField =
                    this.createNumField("widthHint: ", this.data.widthHint);
                this.shell.update();
            }
            return;
        }
        if (e.widget == this.cmdHint_H)
        {
            if (this.hHintField == null || this.hHintField.isDisposed())
            {
                this.hHintField =
                    this.createNumField("heightHint: ", this.data.heightHint);
                this.shell.update();
            }
            return;
        }
        // indent
        if (e.widget == this.cmdIndent_V)
        {
            if (this.vIndentField == null || this.vIndentField.isDisposed())
            {
                this.vIndentField =
                    this.createNumField("verticalIndent: ", this.data.verticalIndent);
                this.shell.update();
            }
            return;
        }
        if (e.widget == this.cmdIndent_H)
        {
            if (this.hIndentField == null || this.hIndentField.isDisposed())
            {
                this.hIndentField =
                    this.createNumField("horizontalIndent: ", this.data.horizontalIndent);
                this.shell.update();
            }
            return;
        }
        // span
        if (e.widget == this.cmdSpan_V)
        {
            if (this.vSpanField == null || this.vSpanField.isDisposed())
            {
                this.vSpanField =
                    this.createNumField("verticalSpan: ", this.data.verticalSpan);
                this.shell.update();
            }
            return;
        }
        if (e.widget == this.cmdSpan_H)
        {
            if (this.hSpanField == null || this.hSpanField.isDisposed())
            {
                this.hSpanField =
                    this.createNumField("horizontalSpan: ", this.data.horizontalSpan);
                this.shell.update();
            }
            return;
        }
        // grab
        // vertical
        if (e.widget == this.cmdGrabV_TRUE)
        {
            if (this.vGrabInfo == null || this.vGrabInfo.isDisposed())
            {
                this.vGrabInfo =
                    this.createInfoLabel("grabExcessVerticalSpace: ", "true");
            }
            else
            {
                this.vGrabInfo.setText("true");
                this.vGrabInfo.pack();
            }
            this.data.grabExcessVerticalSpace = true;
            this.shell.update();
            return;
        }
        if (e.widget == this.cmdGrabV_FALSE)
        {
            if (this.vGrabInfo == null || this.vGrabInfo.isDisposed())
            {
                this.vGrabInfo =
                    this.createInfoLabel("grabExcessVerticalSpace: ", "false");
            }
            else
            {
                this.vGrabInfo.setText("false");
                this.vGrabInfo.pack();
            }
            this.data.grabExcessVerticalSpace = false;
            this.shell.update();
            return;
        }
        // horizontal
        if (e.widget == this.cmdGrabH_TRUE)
        {
            if (this.hGrabInfo == null || this.hGrabInfo.isDisposed())
            {
                this.hGrabInfo =
                    this.createInfoLabel("grabExcessHorizontalSpace: ", "true");
            }
            else
            {
                this.hGrabInfo.setText("true");
                this.hGrabInfo.pack();
            }
            this.data.grabExcessHorizontalSpace = true;
            this.shell.update();
            return;
        }
        if (e.widget == this.cmdGrabH_FALSE)
        {
            if (this.hGrabInfo == null || this.hGrabInfo.isDisposed())
            {
                this.hGrabInfo =
                    this.createInfoLabel("grabExcessHorizontalSpace: ", "false");
            }
            else
            {
                this.hGrabInfo.setText("false");
                this.hGrabInfo.pack();
            }
            this.data.grabExcessHorizontalSpace = false;
            this.shell.update();
            return;
        }

        // Clear command
        if (e.widget == this.cmdReset)
        {
            this.data = new GridData();

            Control[] controls = this.shell.getChildren();
            for (int i=0; i<controls.length; i++)
                controls[i].dispose();

            this.shell.layout();
            this.shell.update();
            return;
        }

        // Done command
        if (e.widget == this.cmdDone)
        {
            this.success = true;
            this.adjustSettings();
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
     * Adjust numerical settings to the current GridData.
     */
    private void adjustSettings()
    {
        // width\height hint
        if (this.wHintField != null && !this.wHintField.isDisposed())
        {
            this.data.widthHint = Integer.parseInt(this.wHintField.getText());
        }
        if (this.hHintField != null && !this.hHintField.isDisposed())
        {
            this.data.heightHint = Integer.parseInt(this.hHintField.getText());
        }
        // indent
        if (this.vIndentField != null && !this.vIndentField.isDisposed())
        {
            this.data.verticalIndent =
                Integer.parseInt(this.vIndentField.getText());
        }
        if (this.hIndentField != null && !this.hIndentField.isDisposed())
        {
            this.data.horizontalIndent =
                Integer.parseInt(this.hIndentField.getText());
        }
        // span
        if (this.vSpanField != null && !this.vSpanField.isDisposed())
        {
            this.data.verticalSpan =
                Integer.parseInt(this.vSpanField.getText());
        }
        if (this.hSpanField != null && !this.hSpanField.isDisposed())
        {
            this.data.horizontalSpan =
                Integer.parseInt(this.hSpanField.getText());
        }
    }

    /* (non-Javadoc)
     * @see org.eclipse.swt.events.SelectionListener#widgetDefaultSelected(org.eclipse.swt.events.SelectionEvent)
     */
    public void widgetDefaultSelected(SelectionEvent e)
    {
    }
}
