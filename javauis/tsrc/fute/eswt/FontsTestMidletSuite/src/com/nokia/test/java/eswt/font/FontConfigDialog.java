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
import org.eclipse.ercp.swt.mobile.QueryDialog;
import org.eclipse.swt.SWT;
import org.eclipse.swt.events.SelectionEvent;
import org.eclipse.swt.events.SelectionListener;
import org.eclipse.swt.graphics.Font;
import org.eclipse.swt.graphics.FontData;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.widgets.Dialog;
import org.eclipse.swt.widgets.Display;
import org.eclipse.swt.widgets.Label;
import org.eclipse.swt.widgets.Shell;

/**
 *
 */
public class FontConfigDialog extends Dialog implements SelectionListener
{
    /** shell */
    protected Shell shell = null;

    /** */
    protected Font font = null;

    /** Info labels */
    protected Label infoHeight  = null;
    protected Label infoStyle       = null;

    /** success flag */
    protected boolean success = false;

    /** Commands */
    protected Command cmdChangeHeight           = null;
    protected Command cmdChangeStyleNORMAL          = null;
    protected Command cmdChangeStyleITALIC          = null;
    protected Command cmdChangeStyleBOLD            = null;
    protected Command cmdChangeStyleBOLDITALIC      = null;

    protected Command cmdOK     = null;
    protected Command cmdCancel = null;

    /**
     * Constructor
     */
    public FontConfigDialog(Shell parent)
    {
        super(parent, SWT.DIALOG_TRIM | SWT.APPLICATION_MODAL);
    }

    /**
     * Initialization
     */
    private void init(Font font)
    {
        // shell init
        this.shell = new Shell(this.getParent(), SWT.DIALOG_TRIM | SWT.APPLICATION_MODAL);
        this.shell.setText("Font Configuration");
        this.shell.setLayout(new GridLayout(2, false));

        // set default font
        if (font == null && font.isDisposed())
        {
            throw new IllegalArgumentException("The parameter cannot be null");
        }
        FontData fdata = font.getFontData()[0];
        this.font = new Font(this.getParent().getDisplay(), fdata);

        // create info labels
        Label heightTitle = new Label(this.shell, SWT.NONE);
        heightTitle.setText("Height: ");
        this.infoHeight = new Label(this.shell, SWT.BORDER);
        this.infoHeight.setText(
            String.valueOf(this.font.getFontData()[0].getHeight()));

        Label styleTitle = new Label(this.shell, SWT.NONE);
        styleTitle.setText("Style: ");
        this.infoStyle = new Label(this.shell, SWT.BORDER);
        this.setStyle(this.font.getFontData()[0].getStyle());

        // create commands
        // change command group
        Command cmdSet = new Command(this.shell, Command.COMMANDGROUP, 1);
        cmdSet.setText("Set");

        // change height
        this.cmdChangeHeight = new Command(cmdSet, Command.GENERAL, 1);
        this.cmdChangeHeight.setText("Height");
        this.cmdChangeHeight.addSelectionListener(this);

        // change style
        Command cmdStyle = new Command(cmdSet, Command.COMMANDGROUP, 0);
        cmdStyle.setText("Style");

        this.cmdChangeStyleNORMAL = new Command(cmdStyle, Command.GENERAL, 3);
        this.cmdChangeStyleNORMAL.setText("NORMAL");
        this.cmdChangeStyleNORMAL.addSelectionListener(this);

        this.cmdChangeStyleITALIC = new Command(cmdStyle, Command.GENERAL, 2);
        this.cmdChangeStyleITALIC.setText("ITALIC");
        this.cmdChangeStyleITALIC.addSelectionListener(this);

        this.cmdChangeStyleBOLD = new Command(cmdStyle, Command.GENERAL, 1);
        this.cmdChangeStyleBOLD.setText("BOLD");
        this.cmdChangeStyleBOLD.addSelectionListener(this);

        this.cmdChangeStyleBOLDITALIC = new Command(cmdStyle, Command.GENERAL, 0);
        this.cmdChangeStyleBOLDITALIC.setText("BOLD | ITALIC");
        this.cmdChangeStyleBOLDITALIC.addSelectionListener(this);

        // OK command
        this.cmdOK = new Command(this.shell, Command.GENERAL, 0);
        this.cmdOK.setText("OK");
        this.cmdOK.addSelectionListener(this);

        // Cancel command
        this.cmdCancel = new Command(this.shell, Command.EXIT, 0);
        this.cmdCancel.setText("Cancel");
        this.cmdCancel.addSelectionListener(this);

        // restore success flag
        this.success = false;
    }

    /**
     *
     */
    public Font open(Font font)
    {
        // init
        this.init(font);

        // open the dialog
        shell.open();
        Display display = this.getParent().getDisplay();
        while (!shell.isDisposed())
        {
            if (!display.readAndDispatch()) display.sleep();
        }

        if (!this.success)
        {
            this.font.dispose();
            this.font = null;
        }

        return this.font;
    }

    /**
     * Set specified style to current font and unpdate style info label
     */
    private void setStyle(int style)
    {
        FontData data = this.font.getFontData()[0];
        this.font.dispose();
        data.setStyle(style);
        this.font = new Font(this.getParent().getDisplay(), data);

        // update info label
        String sstyle = "";
        switch (style)
        {
        case SWT.NORMAL:
            sstyle = "NORMAL";
            break;

        case SWT.ITALIC:
            sstyle = "ITALIC";
            break;

        case SWT.BOLD:
            sstyle = "BOLD";
            break;

        case SWT.BOLD | SWT.ITALIC:
            sstyle = "BOLD | ITALIC";
            break;
        }
        this.infoStyle.setText(sstyle);
        this.infoStyle.pack();
    }


    /* (non-Javadoc)
     * @see org.eclipse.swt.events.SelectionListener#widgetSelected(org.eclipse.swt.events.SelectionEvent)
     */
    public void widgetSelected(SelectionEvent e)
    {
        // change height command
        if (e.widget == this.cmdChangeHeight)
        {
            QueryDialog dlg = new QueryDialog(this.shell, SWT.BORDER, QueryDialog.NUMERIC);
            dlg.setPromptText("Input height",
                              String.valueOf(this.font.getFontData()[0].getHeight()));
            dlg.setMinimum(0);
            dlg.setMaximum(2);
            String sheight = dlg.open();
            if (sheight != null)
            {
                FontData data = this.font.getFontData()[0];
                this.font.dispose();
                data.setHeight(Integer.parseInt(sheight));
                this.font = new Font(this.getParent().getDisplay(), data);
                this.infoHeight.setText(sheight);
                this.infoHeight.pack();
            }
            return;
        }

        // change style commands
        if (e.widget == this.cmdChangeStyleNORMAL)
        {
            this.setStyle(SWT.NORMAL);
            return;
        }
        if (e.widget == this.cmdChangeStyleITALIC)
        {
            this.setStyle(SWT.ITALIC);
            return;
        }
        if (e.widget == this.cmdChangeStyleBOLD)
        {
            this.setStyle(SWT.BOLD);
            return;
        }
        if (e.widget == this.cmdChangeStyleBOLDITALIC)
        {
            this.setStyle(SWT.BOLD | SWT.ITALIC);
            return;
        }

        // OK command
        if (e.widget == this.cmdOK)
        {
            this.success = true;
            this.shell.dispose();
        }

        // Cancel command
        if (e.widget == this.cmdCancel)
        {
            this.success = false;
            this.shell.dispose();
        }
    }

    /* (non-Javadoc)
     * @see org.eclipse.swt.events.SelectionListener#widgetDefaultSelected(org.eclipse.swt.events.SelectionEvent)
     */
    public void widgetDefaultSelected(SelectionEvent e)
    {
    }
}
