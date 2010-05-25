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
package com.nokia.test.java.eswt.tasktip;

import java.util.Date;
import java.util.Vector;

import org.eclipse.ercp.swt.mobile.Command;
import org.eclipse.ercp.swt.mobile.ConstrainedText;
import org.eclipse.ercp.swt.mobile.DateEditor;
import org.eclipse.ercp.swt.mobile.HyperLink;
import org.eclipse.ercp.swt.mobile.MobileShell;
import org.eclipse.ercp.swt.mobile.QueryDialog;
import org.eclipse.swt.SWT;
import org.eclipse.swt.events.SelectionEvent;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.widgets.Button;
import org.eclipse.swt.widgets.Combo;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Label;
import org.eclipse.swt.widgets.MenuItem;
import org.eclipse.swt.widgets.MessageBox;
import org.eclipse.swt.widgets.ProgressBar;
import org.eclipse.swt.widgets.Shell;
import org.eclipse.swt.widgets.Slider;
import org.eclipse.swt.widgets.Text;
import org.eclipse.ercp.swt.mobile.TaskTip;

import com.nokia.test.java.eswt.midlet.TestBaseMIDlet;

public class TaskTipTestMIDlet_01 extends TestBaseMIDlet
{

    /**  */
    private Vector tasktips = new Vector();

    private Command cmdSetInvisible         = null;
    private Command cmdSetInvisibleSel      = null;
    private Command cmdSetInvisibleAll      = null;
    /** */

    private Command cmdAddPlain             = null;
    private Command cmdAddSmooth            = null;
    private Command cmdAddIndeterminate     = null;

    private Command cmdRemove               = null;
    private Command cmdRemoveTaskTip        = null;
    private Command cmdRemoveAll            = null;

    private Command cmdSetTaskTip           = null;
    private Command cmdSetLongText          = null;

    private Command cmdSetHalf              = null;
    private Command cmdSetFull              = null;

    private Command cmdChange                   = null;

    private int PLAIN = 0;

    /** mobileshell instance */
    private MobileShell mshell = null;

    private String longtxt = "This is a tasktip! It has a long text that should be"+
                             "truncated at some point. Let's see if that happens...";


    protected void init()
    {
        // init the shell
        if (this.shell != null && !this.shell.isDisposed())
            this.shell.dispose();

        this.shell = new Shell(this.display, SWT.V_SCROLL);
        this.shell.setText("TaskTip Test");

        GridLayout layout = new GridLayout(1, false);
        this.shell.setLayout(layout);

        // set the menu
        this.menuSetup(this.shell);
        this.addAll(this.shell);

        // create the mobileshell with the labels
        this.mshell = new MobileShell(this.display, SWT.SHELL_TRIM, MobileShell.SMALL_STATUS_PANE);
        this.mshell.setText("Label Test 3 - Mobile Shell");
        this.mshell.setLayout(new GridLayout());
        Command cmd = null;
        // "switch" left softkey command
        cmd = new Command(this.mshell, Command.SELECT, 0);
        cmd.setText("Switch");
        cmd.addSelectionListener(this);
        // "exit" right softkey command
        cmd = new Command(this.mshell, Command.EXIT, 0);
        cmd.setText("Exit");
        cmd.addSelectionListener(this);
        this.addAll(this.mshell);
        this.mshell.pack();
        this.mshell.open();
        this.mshell.setVisible(false);
    }
    protected void showException(String msg)
    {
        MessageBox mb = new MessageBox(this.shell, SWT.ICON_INFORMATION | SWT.OK);
        mb.setText("Exception");
        mb.setMessage(msg);
        mb.open();
    }
    private void menuSetup(Composite parent)
    {
        // create menu commands

        // Set Invisible submenu
        this.cmdSetInvisible = new Command(parent, Command.COMMANDGROUP, 3);
        this.cmdSetInvisible.setText("Set Invisible");

        this.cmdSetInvisibleSel =
            new Command(this.cmdSetInvisible, Command.GENERAL, 1);
        this.cmdSetInvisibleSel.setText("Top");
        this.cmdSetInvisibleSel.addSelectionListener(this);

        this.cmdSetInvisibleAll =
            new Command(this.cmdSetInvisible, Command.GENERAL, 0);
        this.cmdSetInvisibleAll.setText("All");
        this.cmdSetInvisibleAll.addSelectionListener(this);

        this.cmdAddPlain =
            new Command(parent, Command.GENERAL, 1);
        this.cmdAddPlain.setText("Plain TaskTip");
        this.cmdAddPlain.addSelectionListener(this);

        this.cmdAddSmooth =
            new Command(parent, Command.GENERAL, 1);
        this.cmdAddSmooth.setText("Smooth TaskTip");
        this.cmdAddSmooth.addSelectionListener(this);

        this.cmdAddIndeterminate =
            new Command(parent, Command.GENERAL, 1);
        this.cmdAddIndeterminate.setText("Indeterminate TaskTip");
        this.cmdAddIndeterminate.addSelectionListener(this);

        // Remove submenu
        this.cmdRemove = new Command(parent, Command.COMMANDGROUP, 0);
        this.cmdRemove.setText("Remove");

        this.cmdRemoveTaskTip =
            new Command(this.cmdRemove, Command.GENERAL, 1);
        this.cmdRemoveTaskTip.setText("Remove Selected");
        this.cmdRemoveTaskTip.addSelectionListener(this);

        this.cmdRemoveAll =
            new Command(this.cmdRemove, Command.GENERAL, 0);
        this.cmdRemoveAll.setText("Remove All");
        this.cmdRemoveAll.addSelectionListener(this);

        this.cmdSetTaskTip = new Command(parent, Command.GENERAL, 0);
        this.cmdSetTaskTip.setText("Set TaskTip");
        this.cmdSetTaskTip.addSelectionListener(this);

        this.cmdSetLongText = new Command(parent, Command.GENERAL, 0);
        this.cmdSetLongText.setText("Set Long Text TaskTip");
        this.cmdSetLongText.addSelectionListener(this);

        this.cmdSetHalf = new Command(parent, Command.GENERAL, 0);
        this.cmdSetHalf.setText("Set Half");
        this.cmdSetHalf.addSelectionListener(this);

        this.cmdSetFull = new Command(parent, Command.GENERAL, 0);
        this.cmdSetFull.setText("Set Full");
        this.cmdSetFull.addSelectionListener(this);

        this.cmdChange = new Command(parent, Command.SELECT, 0);
        this.cmdChange.setText("Switch");
        this.cmdChange.addSelectionListener(this);


    }

    /*
     *  (non-Javadoc)
     * @see org.eclipse.swt.events.SelectionListener#widgetSelected(org.eclipse.swt.events.SelectionEvent)
     */
    public void widgetSelected(SelectionEvent event)
    {
        if (!(event.widget instanceof Command || event.widget instanceof MenuItem)) return;

        boolean changed = false;

        if (event.widget instanceof MenuItem)
        {
            event.widget.dispose();
            changed = true;
        }

        // Set Invisible commands
        if (event.widget == this.cmdSetInvisibleSel)
        {
            ((TaskTip)this.tasktips.lastElement()).setVisible(false);
            changed = true;

        }
        else if (event.widget == this.cmdSetInvisibleAll)
        {

            for (int i=0; i<tasktips.size(); i++)
            {
                ((TaskTip)this.tasktips.elementAt(i)).setVisible(false);
            }
            changed = true;
        }
        else


            // Remove commands
            if (event.widget == this.cmdRemoveTaskTip)
            {
                ((TaskTip)this.tasktips.lastElement()).dispose();
                changed = true;
            }
            else if (event.widget == this.cmdRemoveAll)
            {
                this.tasktips.removeAllElements();
                changed = true;
            }
            else if (event.widget == this.cmdSetTaskTip)
            {
                QueryDialog dlg = new QueryDialog(this.shell, SWT.APPLICATION_MODAL, QueryDialog.STANDARD);
                dlg.setPromptText("Enter text:", "");

                String txt = dlg.open();
                ((TaskTip)this.tasktips.lastElement()).setText(txt);

                changed = true;

            }
            else if (event.widget == this.cmdSetLongText)
            {
                ((TaskTip)this.tasktips.lastElement()).setText(this.longtxt);
            }
            else if (event.widget == this.cmdAddPlain)
            {
                this.addTaskTip(this.PLAIN,this.display.getActiveShell());
                changed = true;
            }
            else if (event.widget == this.cmdAddSmooth)
            {
                this.addTaskTip(SWT.SMOOTH,this.display.getActiveShell());
                changed = true;
            }
            else if (event.widget == this.cmdAddIndeterminate)
            {
                this.addTaskTip(SWT.INDETERMINATE,this.display.getActiveShell());
                changed = true;
            }
            else if (event.widget == this.cmdSetHalf)
            {
                ((TaskTip)this.tasktips.lastElement()).setMaximum(100);
                ((TaskTip)this.tasktips.lastElement()).setSelection(50);
            }
            else if (event.widget == this.cmdSetFull)
            {
                ((TaskTip)this.tasktips.lastElement()).setMaximum(100);
                ((TaskTip)this.tasktips.lastElement()).setSelection(100);
            }
            else if (event.widget == this.cmdChange || "Switch".equals(((Command) event.widget).getText()))
            {

                this.mshell.setVisible(!this.mshell.isVisible());
                this.shell.setVisible(!this.shell.isVisible());
                if (this.shell.isVisible())
                    this.menuSetup(shell);
                else
                    this.menuSetup(mshell);

                return;
            }
        // update the shell if needed
        if (changed)
        {
            if (this.shell.getChildren().length == 0)
            {
                this.cmdSetInvisible.setEnabled(false);
                this.cmdRemove.setEnabled(false);
            }
            this.shell.pack();
            this.shell.layout();
            this.shell.update();
            return;
        }

        super.widgetSelected(event);
    }

    /**
     * Create a TaskTip and add it to current shell.
     */
    private void addTaskTip(int style,Shell parent)
    {

        if (parent == null)
        {
            return;
        }

        if (style == this.PLAIN)
        {
            TaskTip tasktip = new TaskTip(parent);
            tasktip.setText("A plain tasktip");
            tasktip.setVisible(true);
            tasktips.addElement(tasktip);
        }
        else
        {
            TaskTip tasktip = new TaskTip(parent, style);
            if (style == SWT.SMOOTH)
                tasktip.setText("A smooth tasktip");
            else
                tasktip.setText("A indeterminate tasktip");
            tasktip.setVisible(true);
            tasktips.addElement(tasktip);
        }

    }
    /**
     * Create a Label and add it to current shell.
     */
    private void addLabel(Composite parent)
    {
        // control
        Label control = new Label(parent, SWT.NONE);
        control.setText("A Label control");
        GridData data = new GridData();
        data.grabExcessHorizontalSpace = true;
        data.horizontalAlignment = SWT.CENTER;
        control.setLayoutData(data);
    }

    /**
     * Create a Hyperlink and add it to current shell.
     */
    private void addHyperlink(Composite parent)
    {
        // control
        HyperLink control = new HyperLink(parent, SWT.NONE, HyperLink.EMAIL);
        control.setText("some.name@foo.com");
        GridData data = new GridData();
        data.grabExcessHorizontalSpace = true;
        data.horizontalAlignment = SWT.CENTER;
        control.setLayoutData(data);
    }

    /**
     * Create a Text and add it to current shell.
     */
    private void addText(Composite parent)
    {
        // control
        Text control = new Text(parent, SWT.BORDER);
        control.setText("Text field");
        GridData data = new GridData();
        data.grabExcessHorizontalSpace = true;
        data.horizontalAlignment = SWT.CENTER;
        control.setLayoutData(data);
    }

    /**
     * Create a ConstrainedText and add it to current shell.
     */
    private void addConstrainedText(Composite parent)
    {
        // control
        ConstrainedText control =
            new ConstrainedText(parent, SWT.BORDER, ConstrainedText.PHONENUMBER);
        control.setText("+*#0123456789");
        control.setTextLimit(15);
        GridData data = new GridData();
        data.grabExcessHorizontalSpace = true;
        data.horizontalAlignment = SWT.CENTER;
        control.setLayoutData(data);
    }

    /**
     * Create a Slider control and add it to current shell.
     */
    private void addSlider(Composite parent)
    {
        // control
        Slider control = new Slider(parent, SWT.HORIZONTAL);
        control.setMaximum(10);
        GridData data = new GridData();
        data.grabExcessHorizontalSpace = true;
        data.horizontalAlignment = SWT.CENTER;
        control.setLayoutData(data);
    }

    /**
     * Create a DateEditor and add it to current shell.
     */
    private void addDateEditor(Composite parent)
    {
        // control
        DateEditor control =
            new DateEditor(parent, SWT.BORDER, DateEditor.DATE_TIME);
        control.setDate(new Date());
        GridData data = new GridData();
        data.grabExcessHorizontalSpace = true;
        data.horizontalAlignment = SWT.CENTER;
        control.setLayoutData(data);
    }

    /**
     * Create a Button and add it to current shell.
     */
    private void addButton(Composite parent)
    {
        // control
        Button control = new Button(parent, SWT.PUSH);
        control.setText("A Button");
        GridData data = new GridData();
        data.grabExcessHorizontalSpace = true;
        data.horizontalAlignment = SWT.CENTER;
        control.setLayoutData(data);
    }

    /**
     * Create a Combo control and add it to current shell.
     */
    private void addCombo(Composite parent)
    {
        // control
        Combo control =
            new Combo(parent, SWT.BORDER | SWT.DROP_DOWN | SWT.READ_ONLY);
        control.add("item 1");
        control.add("item 2");
        control.add("item 3");
        GridData data = new GridData();
        data.grabExcessHorizontalSpace = true;
        data.horizontalAlignment = SWT.CENTER;
        control.setLayoutData(data);
        control.pack();
    }

    /**
     * Create a ProgressBar and add it to current shell.
     */
    private void addProgressBar(Composite parent)
    {
        // control
        ProgressBar control = new ProgressBar(parent, SWT.BORDER | SWT.HORIZONTAL);
        control.setMaximum(100);
        GridData data = new GridData();
        data.grabExcessHorizontalSpace = true;
        data.horizontalAlignment = SWT.CENTER;
        control.setLayoutData(data);
    }

    /**
     * Add all controls at once.
     */
    private void addAll(Composite parent)
    {
        addLabel(parent);
        addHyperlink(parent);
        addText(parent);
        addConstrainedText(parent);
        addDateEditor(parent);
        addButton(parent);
        addCombo(parent);
        addSlider(parent);
        addProgressBar(parent);
    }

}
