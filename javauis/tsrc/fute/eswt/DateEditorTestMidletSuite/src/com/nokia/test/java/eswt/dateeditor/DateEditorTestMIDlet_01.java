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

package com.nokia.test.java.eswt.dateeditor;

import java.util.Date;
import java.util.TimeZone;
import java.util.Calendar;


import org.eclipse.ercp.swt.mobile.Command;
import org.eclipse.ercp.swt.mobile.DateEditor;

import org.eclipse.ercp.swt.mobile.TimedMessageBox;
import org.eclipse.swt.SWT;
import org.eclipse.swt.events.SelectionEvent;
import org.eclipse.swt.events.SelectionListener;
import org.eclipse.swt.layout.FillLayout;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.layout.GridLayout;

import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Label;
import org.eclipse.swt.widgets.MessageBox;
import org.eclipse.swt.widgets.Shell;

import com.nokia.test.java.eswt.midlet.TestBaseMIDlet;


/**
 *
 */
public class DateEditorTestMIDlet_01 extends TestBaseMIDlet
{

    /** DateEditor field */
    private DateEditor field = null;

    /** Style and Type variables */
    private int style = DateEditor.DATE;//default value
    private int type  = DateEditor.COMPACT;//default value

    /** Type and Style labels */
    private Label typeLabel  = null;
    private Label styleLabel = null;

    /** Type menu commands */
    private Command cmdTypeCOMPACT = null;
    private Command cmdTypeFULL    = null;

    /** Style menu commands */
    private Command cmdStyleDATE        = null;
    private Command cmdStyleTIME        = null;
    private Command cmdStyleDATE_TIME   = null;
    private Command cmdStyleDURATION    = null;
    private Command cmdStyleOFFSET      = null;

    /** Set commands */
    private Command cmdSetTime = null;
    private Command cmdSetDate = null;
    private Command cmdSetTimeZone1 = null;
    private Command cmdSetTimeZone2 = null;
    private Command cmdSetTimeZone3 = null;
    private Command cmdSetTimeZoneDefault = null;

    private Command cmdShowTimeZones = null;

    /* (non-Javadoc)
     * @see com.nokia.test.java.eswt.midlet.TestBaseMIDlet#init()
     */
    protected void init()
    {
        // intit the shell
        this.shell.setText("Date Editor Test");

        // create and set layout
        this.shell.setLayout(new GridLayout(2, true));

        // create menu
        // Type submenu
        Command cmdType = new Command(this.shell, Command.COMMANDGROUP, 3);
        cmdType.setText("Type");

        this.cmdTypeCOMPACT = new Command(cmdType, Command.GENERAL, 1);
        this.cmdTypeCOMPACT.setText("COMPACT");
        this.cmdTypeCOMPACT.addSelectionListener(this);

        this.cmdTypeFULL = new Command(cmdType, Command.GENERAL, 0);
        this.cmdTypeFULL.setText("FULL");
        this.cmdTypeFULL.addSelectionListener(this);

        // Style submenu
        Command cmdStyle = new Command(this.shell, Command.COMMANDGROUP, 0);
        cmdStyle.setText("Style");

        this.cmdStyleDATE = new Command(cmdStyle, Command.GENERAL, 4);
        this.cmdStyleDATE.setText("DATE");
        this.cmdStyleDATE.addSelectionListener(this);

        this.cmdStyleTIME = new Command(cmdStyle, Command.GENERAL, 3);
        this.cmdStyleTIME.setText("TIME");
        this.cmdStyleTIME.addSelectionListener(this);

        this.cmdStyleDATE_TIME = new Command(cmdStyle, Command.GENERAL, 2);
        this.cmdStyleDATE_TIME.setText("DATE_TIME");
        this.cmdStyleDATE_TIME.addSelectionListener(this);

        this.cmdStyleDURATION = new Command(cmdStyle, Command.GENERAL, 1);
        this.cmdStyleDURATION.setText("DURATION");
        this.cmdStyleDURATION.addSelectionListener(this);

        this.cmdStyleOFFSET = new Command(cmdStyle, Command.GENERAL, 0);
        this.cmdStyleOFFSET.setText("OFFSET");
        this.cmdStyleOFFSET.addSelectionListener(this);

        // Set submenu
        Command cmdSet = new Command(this.shell, Command.COMMANDGROUP, 0);
        cmdSet.setText("Set");

        this.cmdSetTime = new Command(cmdSet, Command.GENERAL, 1);
        this.cmdSetTime.setText("zero time");
        this.cmdSetTime.addSelectionListener(this);

        this.cmdSetDate = new Command(cmdSet, Command.GENERAL, 0);
        this.cmdSetDate.setText("zero date");
        this.cmdSetDate.addSelectionListener(this);

        // Set submenu
        Command cmdTimeZone = new Command(cmdSet, Command.COMMANDGROUP, 0);
        cmdTimeZone.setText("Time zone");

        this.cmdSetTimeZone1 = new Command(cmdTimeZone, Command.GENERAL, 1);
        this.cmdSetTimeZone1.setText("GMT");
        this.cmdSetTimeZone1.addSelectionListener(this);

        this.cmdSetTimeZone2 = new Command(cmdTimeZone, Command.GENERAL, 1);
        this.cmdSetTimeZone2.setText(TimeZone.getAvailableIDs()[0]);
        this.cmdSetTimeZone2.addSelectionListener(this);

        this.cmdSetTimeZone3 = new Command(cmdTimeZone, Command.GENERAL, 1);
        this.cmdSetTimeZone3.setText(TimeZone.getAvailableIDs()[1]);
        this.cmdSetTimeZone3.addSelectionListener(this);

        this.cmdSetTimeZoneDefault = new Command(cmdTimeZone, Command.GENERAL, 0);
        this.cmdSetTimeZoneDefault.setText("Default");
        this.cmdSetTimeZoneDefault.addSelectionListener(this);

        this.cmdShowTimeZones = new Command(cmdTimeZone, Command.GENERAL, 1);
        this.cmdShowTimeZones.setText("Show Timezones");
        this.cmdShowTimeZones.addSelectionListener(this);

        // create label and field
        Label tlabel = new Label(this.shell, SWT.NONE);
        tlabel.setText("Type: ");
        GridData tldata = new GridData();
        tldata.horizontalAlignment       = SWT.RIGHT;
        tlabel.setLayoutData(tldata);

        this.typeLabel = new Label(this.shell, SWT.NONE);
        this.typeLabel.setText("default");
        GridData tdata = new GridData();
        tdata.horizontalAlignment       = SWT.LEFT;
        this.typeLabel.setLayoutData(tdata);

        Label slabel = new Label(this.shell, SWT.NONE);
        slabel.setText("Style: ");
        GridData sldata = new GridData();
        sldata.horizontalAlignment       = SWT.RIGHT;
        slabel.setLayoutData(sldata);

        this.styleLabel = new Label(this.shell, SWT.NONE);
        this.styleLabel.setText("default");
        GridData sdata = new GridData();
        sdata.horizontalAlignment       = SWT.LEFT;
        this.styleLabel.setLayoutData(sdata);

        this.field = new DateEditor(this.shell, SWT.BORDER, DateEditor.DATE | DateEditor.FULL);
        GridData fdata = new GridData();
        fdata.horizontalSpan = 2;
        fdata.grabExcessHorizontalSpace = true;
        fdata.grabExcessVerticalSpace   = true;
        fdata.horizontalAlignment       = SWT.CENTER;
        fdata.verticalAlignment         = SWT.CENTER;
        this.field.setLayoutData(fdata);
    }

    /* (non-Javadoc)
     * @see com.nokia.test.java.eswt.midlet.TestBaseMIDlet#widgetSelected(org.eclipse.swt.events.SelectionEvent)
     */
    public void widgetSelected(SelectionEvent e)
    {

        if (e.widget == this.cmdTypeCOMPACT)
        {
            this.changeType(DateEditor.COMPACT);
            return;
        }
        if (e.widget == this.cmdTypeFULL)
        {
            this.changeType(DateEditor.FULL);
            return;
        }

        if (e.widget == this.cmdStyleDATE)
        {
            this.changeStyle(DateEditor.DATE);
            return;
        }
        if (e.widget == this.cmdStyleTIME)
        {
            this.changeStyle(DateEditor.TIME);
            return;
        }
        if (e.widget == this.cmdStyleDATE_TIME)
        {
            this.changeStyle(DateEditor.DATE_TIME);
            return;
        }
        if (e.widget == this.cmdStyleDURATION)
        {
            this.changeStyle(DateEditor.DURATION);
            return;
        }
        if (e.widget == this.cmdStyleOFFSET)
        {
            this.changeStyle(DateEditor.OFFSET);
            return;
        }
        if (e.widget == this.cmdSetTime)
        {
            this.field.setTime(0);
            return;
        }
        if (e.widget == this.cmdSetDate)
        {
            this.field.setDate(new Date(0));
            return;
        }
        if (e.widget == this.cmdSetTimeZone1)
        {
            this.field.setTimeZone(TimeZone.getTimeZone("GMT"));
            this.changeStyle(DateEditor.TIME);
            this.shell.layout();
            this.shell.update();
            return;
        }
        if (e.widget == this.cmdSetTimeZone2)
        {
            this.field.setTimeZone(TimeZone.getTimeZone(TimeZone.getAvailableIDs()[0]));
            this.changeStyle(DateEditor.DATE_TIME);
            this.shell.layout();
            this.shell.update();
            return;
        }
        if (e.widget == this.cmdSetTimeZone3)
        {
            this.field.setTimeZone(TimeZone.getTimeZone(TimeZone.getAvailableIDs()[1]));
            this.changeStyle(DateEditor.TIME);
            this.shell.layout();
            this.shell.update();
            return;
        }
        if (e.widget == this.cmdSetTimeZoneDefault)
        {
            this.field.setTimeZone(null);
            this.changeStyle(DateEditor.DATE_TIME);
            this.shell.layout();
            this.shell.update();
            return;
        }
        if (e.widget == this.cmdShowTimeZones)
        {
            // create a shell with all timezones
            this.dateFieldsInScrolledCompositeDialog(this.shell);
            return;
        }

        super.widgetSelected(e);
    }

    /**
     * Update the type and recreate the field.
     * @param type new type for the field.
     */
    private void changeType(int type)
    {
        if (this.field == null || this.field.isDisposed())
            return;

        this.type = type;
        this.update();
    }

    /**
     * Update the style and recreate the field.
     * @param style new style of the field
     */
    private void changeStyle(int style)
    {
        if (this.field == null || this.field.isDisposed())
            return;

        this.style = style;
        this.update();
    }

    /**
     * Recreate the DateEditor field with current type and style.
     */
    private void update()
    {
        if (this.field == null || this.field.isDisposed())
            return;

        try
        {

            // store the field layout
            Object data = this.field.getLayoutData();

            // dispose old field and create new one
            DateEditor field = this.field;
            this.field =
                new DateEditor(this.shell, SWT.BORDER, this.type | this.style);
            if (this.style == DateEditor.DURATION)
            {
                this.field.setTime(field.getTime());
            }
            else
            {
                if (field.getDate() != null)
                {
                    this.field.setDate(field.getDate());
                }
                else
                {
                    // set current time
                    Calendar c = Calendar.getInstance();
                    Date date = c.getTime();
                    this.field.setDate(date);
                }
            }

            this.field.setLayoutData(data);
            this.field.setFocus();

            // dispose old field
            field.dispose();

        }
        catch (Exception ex)
        {
            MessageBox mb = new MessageBox(this.shell, SWT.ICON_INFORMATION | SWT.OK);
            mb.setText("Exception1");
            mb.setMessage(ex.getMessage());
            mb.open();
        }

        try
        {
            // update info labels
            switch (this.type)
            {
            case DateEditor.COMPACT:
                this.typeLabel.setText("COMPACT");
                break;
            case DateEditor.FULL:
                this.typeLabel.setText("FULL");
                break;
            }

            switch (this.style)
            {
            case DateEditor.DATE:
                this.styleLabel.setText("DATE");
                break;
            case DateEditor.TIME:
                this.styleLabel.setText("TIME");
                break;
            case DateEditor.DATE_TIME:
                this.styleLabel.setText("DATE_TIME");
                break;
            case DateEditor.DURATION:
                this.styleLabel.setText("DURATION");
                break;
            case DateEditor.OFFSET:
                this.styleLabel.setText("OFFSET");
                break;
            }
        }
        catch (Exception ex)
        {
            MessageBox mb = new MessageBox(this.shell, SWT.ICON_INFORMATION | SWT.OK);
            mb.setText("Exception2");
            mb.setMessage(ex.getMessage());
            mb.open();
        }

        // update the shell
        this.shell.layout();
        this.shell.update();
    }
    static void dateFieldsInScrolledCompositeDialog(Shell parent)
    {
        try
        {

            final Shell shell1 = new Shell(parent, SWT.APPLICATION_MODAL | SWT.BORDER);
            shell1.setText("Timezones");

            shell1.setLayout(new FillLayout(SWT.VERTICAL));

            org.eclipse.swt.custom.ScrolledComposite sc = new org.eclipse.swt.custom.ScrolledComposite(shell1, SWT.V_SCROLL);
            sc.setAlwaysShowScrollBars(true);

            final Composite co = new Composite(sc, 0);
            co.setLayout(new GridLayout(2, false));
            Calendar c = Calendar.getInstance();
            for (int i = 0; i<TimeZone.getAvailableIDs().length; i++)
            {
                Label zoneTitle = new Label(co, SWT.WRAP);
                zoneTitle.setText(TimeZone.getTimeZone(TimeZone.getAvailableIDs()[i]).getID());
                zoneTitle.setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, false));

                DateEditor timezone_df = new DateEditor(co,SWT.BORDER,DateEditor.TIME | DateEditor.COMPACT);
                timezone_df.setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, false));

                timezone_df.setTimeZone(TimeZone.getTimeZone(TimeZone.getAvailableIDs()[i]));

                timezone_df.setDate(c.getTime());

            }
            Label localTitle = new Label(co, SWT.WRAP);
            localTitle.setText("Local:");
            localTitle.setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, false));

            DateEditor timezone_df = new DateEditor(co,SWT.BORDER,DateEditor.TIME | DateEditor.COMPACT);
            timezone_df.setTimeZone(null);
            timezone_df.setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, false));
            timezone_df.setDate(c.getTime());


            shell1.layout(true);
            co.setSize(co.computeSize(sc.getClientArea().width, -1));
            sc.setContent(co);

            {
                Command cmd = new Command(shell1, Command.EXIT, 0);
                cmd.setText("Cancel");
                cmd.addSelectionListener(new SelectionListener()
                {
                    public void widgetDefaultSelected(SelectionEvent arg0)
                    {
                    }
                    public void widgetSelected(SelectionEvent arg0)
                    {

                        shell1.dispose();
                    }
                });
            }
            {
                Command cmd = new Command(shell1, Command.GENERAL, 0);
                cmd.setText("Update");
                cmd.addSelectionListener(new SelectionListener()
                {
                    public void widgetDefaultSelected(SelectionEvent arg0)
                    {
                    }
                    public void widgetSelected(SelectionEvent arg0)
                    {
                        Calendar c = Calendar.getInstance();
                        Date date = c.getTime();
                        for (int a = 0; a < co.getChildren().length; a++)
                        {
                            if (co.getChildren()[a] instanceof DateEditor)
                            {
                                ((DateEditor) co.getChildren()[a]).setDate(date);
                            }
                        }
                        shell1.layout(true);
                        shell1.update();
                    }
                });
            }

            shell1.open();
        }
        catch (Exception e)
        {
            TimedMessageBox dlg = new TimedMessageBox(parent, SWT.ICON_ERROR);
            dlg.setMessage(e.getMessage());
            dlg.open();
        }
    }

}

