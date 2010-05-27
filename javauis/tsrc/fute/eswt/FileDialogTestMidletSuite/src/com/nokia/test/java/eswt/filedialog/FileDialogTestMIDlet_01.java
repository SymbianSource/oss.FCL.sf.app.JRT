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

package com.nokia.test.java.eswt.filedialog;

import org.eclipse.ercp.swt.mobile.Command;
import org.eclipse.swt.SWT;
import org.eclipse.swt.events.SelectionEvent;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.widgets.Control;
import org.eclipse.swt.widgets.FileDialog;
import org.eclipse.swt.widgets.Label;
import org.eclipse.swt.widgets.MessageBox;
import org.eclipse.swt.widgets.Text;

import com.nokia.test.java.eswt.midlet.TestBaseMIDlet;

/**
 *
 */
public class FileDialogTestMIDlet_01 extends TestBaseMIDlet
{
    /** dialog settings */

    /** config fields */
    protected Label fieldTYPE           = null;
    protected Text fieldFILE            = null;
    protected Text fieldFilterEXTENSION = null;
    protected Text fieldFilterPATH      = null;

    /** commands */
    protected Command cmdTypeOPEN = null;
    protected Command cmdTypeSAVE = null;

    protected Command cmdSetFileDEFINE  = null;
    protected Command cmdSetFileNULL        = null;

    protected Command cmdSetFilterPATH      = null;
    protected Command cmdSetFilterEXTENTION = null;

    protected Command cmdSetClean   = null;

    // open command
    protected Command cmdOpen = null;

    /* (non-Javadoc)
     * @see com.nokia.test.java.eswt.midlet.TestBaseMIDlet#init()
     */
    protected void init()
    {
        this.shell.setText("File Dialog Test");
        this.shell.setLayout(new GridLayout(2, true));

        // create menu commands
        // settings command group
        Command cmdSettings = new Command(this.shell, Command.COMMANDGROUP, 1);
        cmdSettings.setText("Settings");

        // type commands
        Command cmdType = new Command(cmdSettings, Command.COMMANDGROUP, 3);
        cmdType.setText("Type");

        this.cmdTypeOPEN = new Command(cmdType, Command.GENERAL, 1);
        this.cmdTypeOPEN.setText("OPEN");
        this.cmdTypeOPEN.addSelectionListener(this);

        this.cmdTypeSAVE = new Command(cmdType, Command.GENERAL, 0);
        this.cmdTypeSAVE.setText("SAVE");
        this.cmdTypeSAVE.addSelectionListener(this);

        // defaut file commands
        Command cmdSetFile = new Command(cmdSettings, Command.COMMANDGROUP, 2);
        cmdSetFile.setText("File");

        this.cmdSetFileDEFINE = new Command(cmdSetFile, Command.GENERAL, 1);
        this.cmdSetFileDEFINE.setText("define");
        this.cmdSetFileDEFINE.addSelectionListener(this);

        this.cmdSetFileNULL = new Command(cmdSetFile, Command.GENERAL, 0);
        this.cmdSetFileNULL.setText("null");
        this.cmdSetFileNULL.addSelectionListener(this);

        // filter commands
        Command cmdFilter = new Command(cmdSettings, Command.COMMANDGROUP, 1);
        cmdFilter.setText("Filter");

        this.cmdSetFilterEXTENTION = new Command(cmdFilter, Command.GENERAL, 1);
        this.cmdSetFilterEXTENTION.setText("Extension");
        this.cmdSetFilterEXTENTION.addSelectionListener(this);

        this.cmdSetFilterPATH = new Command(cmdFilter, Command.GENERAL, 0);
        this.cmdSetFilterPATH.setText("Path");
        this.cmdSetFilterPATH.addSelectionListener(this);

        // clean settings command
        this.cmdSetClean = new Command(cmdSettings, Command.GENERAL, 0);
        this.cmdSetClean.setText("Clean");
        this.cmdSetClean.addSelectionListener(this);

        // open dialog command
        this.cmdOpen = new Command(this.shell, Command.GENERAL, 0);
        this.cmdOpen.setText("Open");
        this.cmdOpen.addSelectionListener(this);

        // clean settings
        this.clean();
    }

    /* (non-Javadoc)
     * @see com.nokia.test.java.eswt.midlet.TestBaseMIDlet#widgetSelected(org.eclipse.swt.events.SelectionEvent)
     */
    public void widgetSelected(SelectionEvent event)
    {
        try
        {
            // type commands
            if (event.widget == this.cmdTypeOPEN)
            {
                this.setType(SWT.OPEN);
                return;
            }
            if (event.widget == this.cmdTypeSAVE)
            {
                this.setType(SWT.SAVE);
                return;
            }

            // file commands
            if (event.widget == this.cmdSetFileDEFINE)
            {
                if (this.fieldFILE == null || this.fieldFILE.isDisposed())
                {
                    this.fieldFILE = this.createField("Default value");
                }
                this.fieldFILE.setEnabled(true);
                return;
            }
            if (event.widget == this.cmdSetFileNULL)
            {
                if (this.fieldFILE == null || this.fieldFILE.isDisposed())
                {
                    this.fieldFILE = this.createField("Default value");
                }
                this.fieldFILE.setText("null");
                this.fieldFILE.setEnabled(false);
                return;
            }

            // filter commands
            if (event.widget == this.cmdSetFilterEXTENTION)
            {
                if (this.fieldFilterEXTENSION == null ||
                        this.fieldFilterEXTENSION.isDisposed())
                {
                    this.fieldFilterEXTENSION =
                        this.createField("Filter extention");
                }
                return;
            }
            if (event.widget == this.cmdSetFilterPATH)
            {
                if (this.fieldFilterPATH == null ||
                        this.fieldFilterPATH.isDisposed())
                {
                    this.fieldFilterPATH =
                        this.createField("Filter path");
                }
                return;
            }

            // clean settings command
            if (event.widget == this.cmdSetClean)
            {
                this.clean();
                return;
            }

            // open command
            if (event.widget == this.cmdOpen)
            {
                this.openDialog();
                return;
            }

        }
        catch (Exception ex)
        {
            MessageBox msg = new MessageBox(this.shell, SWT.NONE);
            msg.setMessage(ex.getMessage());
        }

        if (event.widget == this.cmdExit)
        {
            this.clean();
        }
        super.widgetSelected(event);
    }

    /**
     *
     */
    protected void setType(int type)
    {
        // validate specified type for the correct value
        if (type != SWT.OPEN && type != SWT.SAVE)
        {
            // do nothing
            return;
        }

        // update info labels
        if (this.fieldTYPE == null || this.fieldTYPE.isDisposed())
        {
            // create info labels
            Label label = new Label(this.shell, SWT.NONE);
            label.setText("Type:");
            this.fieldTYPE = new Label(this.shell, SWT.NONE);
        }
        switch (type)
        {
        case SWT.OPEN:
            this.fieldTYPE.setText("OPEN");
            break;
        case SWT.SAVE:
            this.fieldTYPE.setText("SAVE");
            break;
        }
        this.shell.layout();
        this.shell.update();
    }

    /**
     * Clean settings
     */
    protected void clean()
    {
        // dispose all info labels
        Control[] controls = this.shell.getChildren();
        for (int i=0; i<controls.length; i++)
        {
            if (controls[i] != null && !controls[i].isDisposed())
                controls[i].dispose();
        }

        // create Settings label
        Label label = new Label(this.shell, SWT.NONE);
        label.setText("Settings");
        GridData data = new GridData();
        data.horizontalSpan             = 2;
        data.horizontalAlignment        = SWT.CENTER;
        data.grabExcessHorizontalSpace  = true;
        label.setLayoutData(data);

        this.shell.update();
    }

    /**
     * Create a Text field with preceeding Label
     */
    protected Text createField(String title)
    {
        Label label = new Label(this.shell, SWT.NONE);
        label.setText(title);

        Text field = new Text(this.shell, SWT.BORDER);
        this.shell.layout();
        this.shell.update();

        return field;
    }

    /**
     * Create, configure and open a FileDialog
     */
    protected void openDialog()
    {
        try
        {
            // get type & create dialog
            int type = SWT.NONE;
            if (this.fieldTYPE != null && !this.fieldTYPE.isDisposed())
            {
                if ("OPEN".equals(this.fieldTYPE.getText()))
                {
                    type = SWT.OPEN;
                }
                if ("SAVE".equals(this.fieldTYPE.getText()))
                {
                    type = SWT.SAVE;
                }
            }
            FileDialog dlg = new FileDialog(this.shell, type);

            // configure the dialog
            // set file name (default value)
            if (this.fieldFILE != null && !this.fieldFILE.isDisposed())
            {
                String fname = null;
                if (this.fieldFILE.isEnabled())
                {
                    fname = this.fieldFILE.getText();
                }
                dlg.setFileName(fname);
            }

            // set extention filter
            if (this.fieldFilterEXTENSION != null &&
                    !this.fieldFilterEXTENSION.isDisposed())
            {
                String filter = null;
                if (this.fieldFilterEXTENSION.isEnabled())
                {
                    filter = this.fieldFilterEXTENSION.getText();
                }
                dlg.setFilterExtensions(new String[] {filter});
            }

            // set path filter
            if (this.fieldFilterPATH != null &&
                    !this.fieldFilterPATH.isDisposed())
            {
                String filter = null;
                if (this.fieldFilterPATH.isEnabled())
                {
                    filter = this.fieldFilterPATH.getText();
                }
                System.out.println(filter);
                dlg.setFilterPath(filter);
                System.out.println("Filter is set");
            }

            // open the dialog
            String result = dlg.open();

            if (result != null)
            {
                MessageBox msg = new MessageBox(this.shell,
                                                SWT.ICON_INFORMATION | SWT.OK);
                msg.setMessage("Selected file: " + result);
                msg.open();
            }

        }
        catch (Exception ex)
        {
            System.out.println("Exception: " + ex.getMessage());
            MessageBox msg = new MessageBox(this.shell);
            msg.setMessage(ex.getMessage());
            msg.open();
        }
    }
}
