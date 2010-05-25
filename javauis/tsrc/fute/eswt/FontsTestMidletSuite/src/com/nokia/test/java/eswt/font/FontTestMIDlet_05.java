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

import org.eclipse.swt.SWT;
import org.eclipse.ercp.swt.mobile.Command;
import org.eclipse.ercp.swt.mobile.ListBox;
import org.eclipse.ercp.swt.mobile.ListBoxItem;
import org.eclipse.ercp.swt.mobile.ListView;
import org.eclipse.ercp.swt.mobile.SortedList;
import org.eclipse.swt.events.SelectionEvent;
import org.eclipse.swt.graphics.Font;
import org.eclipse.swt.layout.FillLayout;
import org.eclipse.swt.widgets.Control;
import org.eclipse.swt.widgets.List;
import org.eclipse.swt.widgets.MessageBox;

import com.nokia.test.java.eswt.midlet.TestBaseMIDlet;

/**
 *
 */
public class FontTestMIDlet_05 extends TestBaseMIDlet
{

    /** currently used font */
    protected Font font = null;

    /** current List control */
    protected Control control = null;

    /** list items */
    protected String[] items = new String[] {"item 4","item 2","item 3","item 1"};

    /** commands */
    protected Command cmdControlList        = null;
    protected Command cmdControlListBox     = null;
    protected Command cmdControlListView    = null;
    protected Command cmdControlSortedList  = null;

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
        this.shell.setLayout(new FillLayout());

        // setup commands
        // composite
        Command cmdControl = new Command(this.shell, Command.COMMANDGROUP, 0);
        cmdControl.setText("List Control");

        this.cmdControlList = new Command(cmdControl, Command.GENERAL, 3);
        this.cmdControlList.setText("List");
        this.cmdControlList.addSelectionListener(this);

        this.cmdControlListBox = new Command(cmdControl, Command.GENERAL, 2);
        this.cmdControlListBox.setText("List Box");
        this.cmdControlListBox.addSelectionListener(this);

        this.cmdControlListView = new Command(cmdControl, Command.GENERAL, 1);
        this.cmdControlListView.setText("List View");
        this.cmdControlListView.addSelectionListener(this);

        this.cmdControlSortedList = new Command(cmdControl, Command.GENERAL, 0);
        this.cmdControlSortedList.setText("Sorted List");
        this.cmdControlSortedList.addSelectionListener(this);

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
            this.shell.pack();
            this.shell.layout();
            this.shell.update();
        }

        // dispose the font
        if (this.font != null && !this.font.isDisposed())
        {
            this.font.dispose();
        }
    }

    /* (non-Javadoc)
     * @see com.nokia.test.java.eswt.midlet.TestBaseMIDlet#widgetSelected(org.eclipse.swt.events.SelectionEvent)
     */
    public void widgetSelected(SelectionEvent event)
    {
        try
        {
            // composite commands
            // List
            if (event.widget == this.cmdControlList)
            {
                if (this.control instanceof List)
                {
                    // nothing to change
                    return;
                }
                // cleaning
                this.clean();
                // create new List
                List control = new List(this.shell, SWT.BORDER);
                control.setItems(this.items);
                this.control = control;
                this.cmdFont.setEnabled(true);
                this.shell.layout();
                this.shell.update();
                return;
            }
            // ListBox
            if (event.widget == this.cmdControlListBox)
            {
                if (this.control instanceof ListBox)
                {
                    // nothing to change
                    return;
                }
                // cleaning
                this.clean();
                // create new ListBox
                ListBox control =
                    new ListBox(this.shell, SWT.BORDER,
                                ListBox.LB_STYLE_NO_HEADING_TEXT);
                ListBoxItem[] data = new ListBoxItem[this.items.length];
                for (int i=0; i<data.length; i++)
                {
                    data[i] = new ListBoxItem();
                    data[i].setDetailText(this.items[i]);
                }
                control.setDataModel(data);
                this.control = control;
                this.cmdFont.setEnabled(true);
                this.shell.layout();
                this.shell.update();
                return;
            }
            // ListView
            if (event.widget == this.cmdControlListView)
            {
                if (this.control instanceof ListView)
                {
                    // nothing to change
                    return;
                }
                // cleaning
                this.clean();
                // create new ListView
                ListView control = new ListView(this.shell, SWT.BORDER);
                control.setItems(this.items, null);
                this.control = control;
                this.cmdFont.setEnabled(true);
                this.shell.layout();
                this.shell.update();
                return;
            }
            // SortedList
            if (event.widget == this.cmdControlSortedList)
            {
                if (this.control instanceof SortedList)
                {
                    // nothing to change
                    return;
                }
                // cleaning
                this.clean();
                // create new SortedList
                SortedList control = new SortedList(this.shell, SWT.BORDER);
                control.setItems(this.items);
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
                    this.control.update();
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
