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
package com.nokia.test.java.eswt.control;

import org.eclipse.swt.SWT;

import org.eclipse.swt.widgets.Control;
import org.eclipse.swt.widgets.Label;
import org.eclipse.swt.widgets.Button;
import org.eclipse.swt.widgets.Combo;
import org.eclipse.swt.widgets.Menu;
import org.eclipse.swt.widgets.MenuItem;
import org.eclipse.swt.widgets.MessageBox;
import org.eclipse.swt.widgets.ProgressBar;
import org.eclipse.swt.widgets.Shell;
import org.eclipse.swt.widgets.Slider;
import org.eclipse.swt.widgets.Text;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.events.SelectionEvent;

import org.eclipse.ercp.swt.mobile.ConstrainedText;
import org.eclipse.ercp.swt.mobile.DateEditor;
import org.eclipse.ercp.swt.mobile.HyperLink;
import org.eclipse.ercp.swt.mobile.Command;
import org.eclipse.ercp.swt.mobile.MobileDevice;
import org.eclipse.ercp.swt.mobile.Screen;

import java.util.Date;

import com.nokia.test.java.eswt.midlet.TestBaseMIDlet;

/**
 * The Midlet tests some base controls such as Label, Button, Combo etc.
 * The controls are created and added dynamically to a Shell one by one
 * or all at once. Traversal order and visibility changes are tested also.
 *
 */
public class ControlTestMIDlet_01 extends TestBaseMIDlet
{

    /** */
    private Command cmdAddLabel             = null;
    private Command cmdAddHyperlink         = null;
    private Command cmdAddText              = null;
    private Command cmdAddConstrainedText   = null;
    private Command cmdAddDateEditor        = null;
    private Command cmdAddButton            = null;
    private Command cmdAddCombo             = null;
    private Command cmdAddSlider            = null;
    private Command cmdAddProgressBar       = null;
    private Command cmdAddAll               = null;
    /** */
    private Command cmdShowInvisible        = null;
    private Command cmdEnableDisabled       = null;
    /** */
    private Command cmdDisable              = null;
    private Command cmdDisableSel           = null;
    private Command cmdDisableAll           = null;
    /** */
    private Command cmdSetInvisible         = null;
    private Command cmdSetInvisibleSel      = null;
    private Command cmdSetInvisibleAll      = null;
    /** */
    private Command cmdRemove               = null;
    private Command cmdRemoveSelected       = null;
    private Command cmdRemoveAll            = null;

    private Command cmdResize               = null;
    private Command cmdStretch              = null;
    private Command cmdSkew                 = null;


    private Command cmdSetToolTip           = null;

    private Command cmdSetMenu              = null;
    private Command cmdSetPopUpMenu         = null;

    /** new shell instance */
    private Shell testshell = null;

    private Command cmdShell                = null;
    //Command for testshell
    private Command cmdSwitch               = null;
    private Command cmdClose                = null;
    private Command cmdOrientation          = null;

    private MobileDevice iDevice;
    private Screen[] iScreen;
    /* (non-Javadoc)
     * @see com.nokia.test.java.eswt.midlet.TestBaseMIDlet#init()
     */
    protected void init()
    {
        // init the shell
        if (this.shell != null && !this.shell.isDisposed())
            this.shell.dispose();

        this.shell = new Shell(this.display, SWT.V_SCROLL);
        this.shell.setText("Control Test");

        GridLayout layout = new GridLayout(1, false);
        this.shell.setLayout(layout);

        iDevice = MobileDevice.getMobileDevice();
        iScreen = iDevice.getScreens();
        // set the menu
        this.menuSetup();
    }
    protected void showException(String msg)
    {
        MessageBox mb = new MessageBox(this.shell, SWT.ICON_INFORMATION | SWT.OK);
        mb.setText("Exception");
        mb.setMessage(msg);
        mb.open();
    }
    /**
     *
     */
    private void menuSetup()
    {
        // create menu commands
        // Show Invisible command
        this.cmdShowInvisible = new Command(this.shell, Command.GENERAL, 5);
        this.cmdShowInvisible.setText("Show Invisible controls");
        this.cmdShowInvisible.setEnabled(false);
        this.cmdShowInvisible.addSelectionListener(this);

        // Enable command
        this.cmdEnableDisabled = new Command(this.shell, Command.GENERAL, 4);
        this.cmdEnableDisabled.setText("Enable disabled controls");
        this.cmdEnableDisabled.setEnabled(false);
        this.cmdEnableDisabled.addSelectionListener(this);

        // Set Invisible submenu
        this.cmdSetInvisible = new Command(this.shell, Command.COMMANDGROUP, 3);
        this.cmdSetInvisible.setText("Set Invisible");
        this.cmdSetInvisible.setEnabled(false);

        this.cmdSetInvisibleSel =
            new Command(this.cmdSetInvisible, Command.GENERAL, 1);
        this.cmdSetInvisibleSel.setText("Selected");
        this.cmdSetInvisibleSel.addSelectionListener(this);

        this.cmdSetInvisibleAll =
            new Command(this.cmdSetInvisible, Command.GENERAL, 0);
        this.cmdSetInvisibleAll.setText("All");
        this.cmdSetInvisibleAll.addSelectionListener(this);

        // Disable submenu
        this.cmdDisable = new Command(this.shell, Command.COMMANDGROUP, 2);
        this.cmdDisable.setText("Disable");
        this.cmdDisable.setEnabled(false);

        this.cmdDisableSel =
            new Command(this.cmdDisable, Command.GENERAL, 1);
        this.cmdDisableSel.setText("Selected");
        this.cmdDisableSel.addSelectionListener(this);

        this.cmdDisableAll =
            new Command(this.cmdDisable, Command.GENERAL, 0);
        this.cmdDisableAll.setText("All");
        this.cmdDisableAll.addSelectionListener(this);

        // Add submenu
        Command add = new Command(this.shell, Command.COMMANDGROUP, 1);
        add.setText("Add");

        this.cmdAddLabel = new Command(add, Command.OK, 9);
        this.cmdAddLabel.setText("Label");
        this.cmdAddLabel.addSelectionListener(this);

        this.cmdAddHyperlink = new Command(add, Command.OK, 8);
        this.cmdAddHyperlink.setText("Hyperlink");
        this.cmdAddHyperlink.addSelectionListener(this);

        this.cmdAddText = new Command(add, Command.OK, 7);
        this.cmdAddText.setText("Text");
        this.cmdAddText.addSelectionListener(this);

        this.cmdAddConstrainedText = new Command(add, Command.OK, 6);
        this.cmdAddConstrainedText.setText("Constrained Text");
        this.cmdAddConstrainedText.addSelectionListener(this);

        this.cmdAddDateEditor = new Command(add, Command.OK, 5);
        this.cmdAddDateEditor.setText("Date Editor");
        this.cmdAddDateEditor.addSelectionListener(this);

        this.cmdAddButton = new Command(add, Command.OK, 4);
        this.cmdAddButton.setText("Button");
        this.cmdAddButton.addSelectionListener(this);

        this.cmdAddCombo = new Command(add, Command.OK, 3);
        this.cmdAddCombo.setText("Combo");
        this.cmdAddCombo.addSelectionListener(this);

        this.cmdAddSlider = new Command(add, Command.OK, 2);
        this.cmdAddSlider.setText("Slider");
        this.cmdAddSlider.addSelectionListener(this);

        this.cmdAddProgressBar = new Command(add, Command.OK, 1);
        this.cmdAddProgressBar.setText("Progress Bar");
        this.cmdAddProgressBar.addSelectionListener(this);

        this.cmdAddAll = new Command(add, Command.GENERAL, 0);
        this.cmdAddAll.setText("All");
        this.cmdAddAll.addSelectionListener(this);

        // Remove submenu
        this.cmdRemove = new Command(this.shell, Command.COMMANDGROUP, 0);
        this.cmdRemove.setText("Remove");
        this.cmdRemove.setEnabled(false);

        this.cmdRemoveSelected =
            new Command(this.cmdRemove, Command.GENERAL, 1);
        this.cmdRemoveSelected.setText("Selected");
        this.cmdRemoveSelected.addSelectionListener(this);

        this.cmdRemoveAll =
            new Command(this.cmdRemove, Command.GENERAL, 0);
        this.cmdRemoveAll.setText("All");
        this.cmdRemoveAll.addSelectionListener(this);

        // Resize submenu
        this.cmdResize = new Command(this.shell, Command.COMMANDGROUP, 0);
        this.cmdResize.setText("Resize");
        this.cmdResize.setEnabled(false);

        this.cmdStretch =
            new Command(this.cmdResize, Command.SELECT, 1);
        this.cmdStretch.setText("Stretch");
        this.cmdStretch.addSelectionListener(this);

        this.cmdSkew =
            new Command(this.cmdResize, Command.SELECT, 0);
        this.cmdSkew.setText("Skew");
        this.cmdSkew.addSelectionListener(this);

        this.cmdSetToolTip = new Command(this.shell, Command.GENERAL, 0);
        this.cmdSetToolTip.setText("Set ToolTip");
        this.cmdSetToolTip.addSelectionListener(this);

        this.cmdSetMenu = new Command(this.shell, Command.SELECT, 0);
        this.cmdSetMenu.setText("Set Menu");
        this.cmdSetMenu.addSelectionListener(this);

        this.cmdSetPopUpMenu = new Command(this.shell, Command.SELECT, 0);
        this.cmdSetPopUpMenu.setText("Set Pop up Menu");
        this.cmdSetPopUpMenu.addSelectionListener(this);

        this.cmdShell = new Command(this.shell, Command.GENERAL, 0);
        this.cmdShell.setText("Show shell");
        this.cmdShell.addSelectionListener(this);

        this.cmdOrientation = new Command(this.shell, Command.SELECT, 0);
        this.cmdOrientation.setText("Change orientation");
        this.cmdOrientation.addSelectionListener(this);
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
        // Show Invisible command
        if (event.widget == this.cmdShowInvisible)
        {
            Control[] controls = this.shell.getChildren();
            for (int i=0; i<controls.length; i++)
            {
                controls[i].setVisible(true);
            }
            this.cmdShowInvisible.setEnabled(false);
            changed = true;
        }
        // Enable command
        if (event.widget == this.cmdEnableDisabled)
        {
            Control[] controls = this.shell.getChildren();
            for (int i=0; i<controls.length; i++)
            {
                controls[i].setEnabled(true);
            }
            this.cmdEnableDisabled.setEnabled(false);
            changed = true;
        }
        // Disable commands
        if (event.widget == this.cmdDisableSel)
        {
            Control control = this.getSelectedControl();
            if (control != null)
            {
                control.setEnabled(false);
                this.cmdEnableDisabled.setEnabled(true);
                changed = true;
            }
        }
        if (event.widget == this.cmdDisableAll)
        {
            Control[] controls = this.shell.getChildren();
            for (int i=0; i<controls.length; i++)
            {
                controls[i].setEnabled(false);
            }
            this.cmdEnableDisabled.setEnabled(true);
            changed = true;
        }
        // Set Invisible commands
        if (event.widget == this.cmdSetInvisibleSel)
        {
            Control control = this.getSelectedControl();
            if (control != null)
            {
                control.setVisible(false);
                this.cmdShowInvisible.setEnabled(true);
                changed = true;
            }
        }
        if (event.widget == this.cmdSetInvisibleAll)
        {
            Control[] controls = this.shell.getChildren();
            for (int i=0; i<controls.length; i++)
            {
                controls[i].setVisible(false);
            }
            this.cmdShowInvisible.setEnabled(true);
            changed = true;
        }

        // Add commands
        if (event.widget == this.cmdAddLabel)
        {
            this.addLabel();
            this.cmdSetInvisible.setEnabled(true);
            this.cmdDisable.setEnabled(true);
            this.cmdRemove.setEnabled(true);
            this.cmdResize.setEnabled(true);
            changed = true;
        }
        if (event.widget == this.cmdAddHyperlink)
        {
            this.addHyperlink();
            this.cmdSetInvisible.setEnabled(true);
            this.cmdDisable.setEnabled(true);
            this.cmdRemove.setEnabled(true);
            this.cmdResize.setEnabled(true);
            changed = true;
        }
        if (event.widget == this.cmdAddText)
        {
            this.addText();
            this.cmdSetInvisible.setEnabled(true);
            this.cmdDisable.setEnabled(true);
            this.cmdRemove.setEnabled(true);
            this.cmdResize.setEnabled(true);
            changed = true;
        }
        if (event.widget == this.cmdAddConstrainedText)
        {
            this.addConstrainedText();
            this.cmdSetInvisible.setEnabled(true);
            this.cmdDisable.setEnabled(true);
            this.cmdRemove.setEnabled(true);
            this.cmdResize.setEnabled(true);
            changed = true;
        }
        if (event.widget == this.cmdAddDateEditor)
        {
            this.addDateEditor();
            this.cmdSetInvisible.setEnabled(true);
            this.cmdDisable.setEnabled(true);
            this.cmdRemove.setEnabled(true);
            this.cmdResize.setEnabled(true);
            changed = true;
        }
        if (event.widget == this.cmdAddButton)
        {
            this.addButton();
            this.cmdSetInvisible.setEnabled(true);
            this.cmdDisable.setEnabled(true);
            this.cmdRemove.setEnabled(true);
            this.cmdResize.setEnabled(true);
            changed = true;
        }
        if (event.widget == this.cmdAddCombo)
        {
            this.addCombo();
            this.cmdSetInvisible.setEnabled(true);
            this.cmdDisable.setEnabled(true);
            this.cmdRemove.setEnabled(true);
            this.cmdResize.setEnabled(true);
            changed = true;
        }
        if (event.widget == this.cmdAddSlider)
        {
            this.addSlider();
            this.cmdSetInvisible.setEnabled(true);
            this.cmdDisable.setEnabled(true);
            this.cmdRemove.setEnabled(true);
            this.cmdResize.setEnabled(true);
            changed = true;
        }
        if (event.widget == this.cmdAddProgressBar)
        {
            this.addProgressBar();
            this.cmdSetInvisible.setEnabled(true);
            this.cmdDisable.setEnabled(true);
            this.cmdRemove.setEnabled(true);
            this.cmdResize.setEnabled(true);
            changed = true;
        }
        if (event.widget == this.cmdAddAll)
        {
            this.addAll();
            this.cmdSetInvisible.setEnabled(true);
            this.cmdDisable.setEnabled(true);
            this.cmdRemove.setEnabled(true);
            this.cmdResize.setEnabled(true);
            changed = true;
        }
        // Remove commands
        if (event.widget == this.cmdRemoveSelected)
        {
            Control control = this.getSelectedControl();
            if (control != null)
            {
                control.dispose();
                changed = true;
            }
        }
        if (event.widget == this.cmdRemoveAll)
        {
            Control[] controls = this.shell.getChildren();
            for (int i=0; i<controls.length; i++)
            {
                controls[i].dispose();
                changed = true;
            }
        }
        // Resize commands
        if (event.widget == this.cmdStretch)
        {
            Control control = this.getSelectedControl();
            if (control != null)
            {
                GridData data = new GridData(SWT.FILL,SWT.FILL,true,true);
                control.setLayoutData(data);
                changed = true;
            }
        }
        if (event.widget == this.cmdSkew)
        {
            Control control = this.getSelectedControl();
            if (control != null)
            {
                GridData data = new GridData();
                data.grabExcessHorizontalSpace = true;
                data.horizontalAlignment = SWT.CENTER;
                control.setLayoutData(data);
                changed = true;
            }

        }
        if (event.widget == this.cmdSetToolTip)
        {
            Control control = this.getSelectedControl();
            if (control != null)
            {
                control.setToolTipText(control.toString());
                changed = true;
            }
        }
        if (event.widget == this.cmdShell)
        {
            this.createTestShell(SWT.DIALOG_TRIM,"Dialog Shell","Try to tap this. No Tactile Feedback.", "Maximize", true);
        }
        if (event.widget == this.cmdSetMenu)
        {
            Control control = this.getSelectedControl();
            if (control != null)
            {
                try
                {
                    Menu menu = new Menu(control);
                    MenuItem item = new MenuItem(menu, SWT.PUSH);
                    item.setText(control.toString());
                    item.addSelectionListener(this);

                    control.setMenu(menu);
                    changed = true;
                }
                catch (Exception ex)
                {
                    this.showException("Menu could not be created: "+ ex.toString());
                }
            }
        }
        if (event.widget == this.cmdSetPopUpMenu)
        {
            Control control = this.getSelectedControl();
            if (control != null)
            {
                try
                {
                    Menu menu = new Menu(this.shell,SWT.POP_UP);
                    MenuItem item = new MenuItem(menu, SWT.PUSH);
                    item.setText("POP "+control.toString());
                    item.addSelectionListener(this);

                    control.setMenu(menu);
                    changed = true;
                }
                catch (Exception ex)
                {
                    this.showException("Menu could not be created: "+ ex.toString());
                }
            }
        }
        if (event.widget == this.cmdSwitch)
        {
            String scmd = ((Command) event.widget).getText();
            if ("Maximize".equals(scmd))
            {
                // maximize the test shell
                this.cmdSwitch.setText("Restore");
                this.testshell.setMaximized(true);
                this.testshell.update();
                this.shell.update();
                return;
            }
            if ("Restore".equals(scmd))
            {
                // restore the test shell
                this.cmdSwitch.setText("Maximize");
                this.testshell.setMaximized(false);
                this.testshell.update();
                this.shell.update();
                return;
            }
            if ("Switch".equals(scmd))
            {
                // switch to the original shell
                this.shell.setActive();
                this.shell.update();
                return;
            }
        }
        if (event.widget == this.cmdClose)
        {
            // close the test shell
            this.closeTestShell();
            return;
        }

        // update the shell if needed
        if (changed)
        {
            if (this.shell.getChildren().length == 0)
            {
                this.cmdSetInvisible.setEnabled(false);
                this.cmdShowInvisible.setEnabled(false);
                this.cmdDisable.setEnabled(false);
                this.cmdEnableDisabled.setEnabled(false);
                this.cmdRemove.setEnabled(false);
            }
            this.shell.pack();
            this.shell.layout();
            this.shell.update();
            return;
        }
        else if (event.widget == this.cmdOrientation)
        {

            if (iScreen[0].getOrientation() == Screen.LANDSCAPE)
                this.iScreen[0].setOrientation(Screen.PORTRAIT);
            else
                this.iScreen[0].setOrientation(Screen.LANDSCAPE);
        }
        super.widgetSelected(event);
    }

    /**
     * close the test shell and dispose its controls
     */
    private void closeTestShell()
    {
        if (this.testshell != null && !this.testshell.isDisposed())
            this.testshell.dispose();
    }
    /**
     * creates a shell based on the given parameters
     * @param style the style og the shell
     * @param title the title of the shell
     * @param labelStr string for a label on the shell
     * @param leftCmd string for the left softkey command
     * @param doPack true if the should be pack()-ed before layout()
     */
    private void createTestShell(int style, String title, String labelStr, String leftCmd, boolean doPack)
    {
        // create the new shell
        this.testshell = new Shell(this.shell, style);
        // set title
        if (title != null)
            this.testshell.setText(title);
        this.testshell.setLayout(new GridLayout());
        // add a label
        Label label = new Label(this.testshell, SWT.CENTER);
        label.setText(labelStr);
        label.setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, true));
        // left softkey command
        if (leftCmd != null)
        {
            this.cmdSwitch = new Command(this.testshell, Command.SELECT, 0);
            this.cmdSwitch.setText(leftCmd);
            this.cmdSwitch.addSelectionListener(this);
        }
        // "close" right softkey command
        this.cmdClose = new Command(this.testshell, Command.EXIT, 0);
        this.cmdClose.setText("Close");
        this.cmdClose.addSelectionListener(this);
        // display the new shell
        if (doPack)
            this.testshell.pack();
        this.testshell.layout();
        this.testshell.open();
    }

    /**
     * Create a Label and add it to current shell.
     */
    private void addLabel()
    {
        // label
        /*Label label = new Label(this.shell, SWT.NONE);
        label.setText("Label:");*/
        // control
        Label control = new Label(this.shell, SWT.NONE);
        control.setText("A Label control");
        GridData data = new GridData();
        data.grabExcessHorizontalSpace = true;
        data.horizontalAlignment = SWT.CENTER;
        control.setLayoutData(data);
    }

    /**
     * Create a Hyperlink and add it to current shell.
     */
    private void addHyperlink()
    {
        // label
        /*Label label = new Label(this.shell, SWT.NONE);
        label.setText("Hyperlink:");*/
        // control
        HyperLink control = new HyperLink(this.shell, SWT.NONE, HyperLink.EMAIL);
        control.setText("some.name@foo.com");
        GridData data = new GridData();
        data.grabExcessHorizontalSpace = true;
        data.horizontalAlignment = SWT.CENTER;
        control.setLayoutData(data);
    }

    /**
     * Create a Text and add it to current shell.
     */
    private void addText()
    {
        // label
        /*Label label = new Label(this.shell, SWT.NONE);
        label.setText("Text:");*/
        // control
        Text control = new Text(this.shell, SWT.BORDER);
        control.setText("Text field");
        GridData data = new GridData();
        data.grabExcessHorizontalSpace = true;
        data.horizontalAlignment = SWT.CENTER;
        control.setLayoutData(data);
    }

    /**
     * Create a ConstrainedText and add it to current shell.
     */
    private void addConstrainedText()
    {
        // label
        /*Label label = new Label(this.shell, SWT.NONE);
        label.setText("Constrained Text:");*/
        // control
        ConstrainedText control =
            new ConstrainedText(this.shell, SWT.BORDER, ConstrainedText.PHONENUMBER);
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
    private void addSlider()
    {
        // label
        /*Label label = new Label(this.shell, SWT.NONE);
        label.setText("Slider:");*/
        // control
        Slider control = new Slider(this.shell, SWT.HORIZONTAL);
        control.setMaximum(10);
        GridData data = new GridData();
        data.grabExcessHorizontalSpace = true;
        data.horizontalAlignment = SWT.CENTER;
        control.setLayoutData(data);
    }

    /**
     * Create a DateEditor and add it to current shell.
     */
    private void addDateEditor()
    {
        // label
        /*Label label = new Label(this.shell, SWT.NONE);
        label.setText("Date Editor:");*/
        // control
        DateEditor control =
            new DateEditor(this.shell, SWT.BORDER, DateEditor.DATE_TIME);
        control.setDate(new Date());
        GridData data = new GridData();
        data.grabExcessHorizontalSpace = true;
        data.horizontalAlignment = SWT.CENTER;
        control.setLayoutData(data);
    }

    /**
     * Create a Button and add it to current shell.
     */
    private void addButton()
    {
        // label
        /*Label label = new Label(this.shell, SWT.NONE);
        label.setText("Date Editor:");*/
        // control
        Button control = new Button(this.shell, SWT.PUSH);
        control.setText("A Button");
        GridData data = new GridData();
        data.grabExcessHorizontalSpace = true;
        data.horizontalAlignment = SWT.CENTER;
        control.setLayoutData(data);
    }

    /**
     * Create a Combo control and add it to current shell.
     */
    private void addCombo()
    {
        // label
        /*Label label = new Label(this.shell, SWT.NONE);
        label.setText("Date Editor:");*/
        // control
        Combo control =
            new Combo(this.shell, SWT.BORDER | SWT.DROP_DOWN | SWT.READ_ONLY);
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
    private void addProgressBar()
    {
        // label
        /*Label label = new Label(this.shell, SWT.NONE);
        label.setText("Date Editor:");*/
        // control
        ProgressBar control = new ProgressBar(this.shell, SWT.BORDER | SWT.HORIZONTAL);
        control.setMaximum(100);
        GridData data = new GridData();
        data.grabExcessHorizontalSpace = true;
        data.horizontalAlignment = SWT.CENTER;
        control.setLayoutData(data);
    }

    /**
     * Add all controls at once.
     */
    private void addAll()
    {
        addLabel();
        addHyperlink();
        addText();
        addConstrainedText();
        addDateEditor();
        addButton();
        addCombo();
        addSlider();
        addProgressBar();
    }

    /**
     * Return currently focused control.
     * @return selected control
     */
    private Control getSelectedControl()
    {
        Control result = null;

        Control[] controls = this.shell.getChildren();
        for (int i=0; i<controls.length; i++)
        {
            if (controls[i].isFocusControl())
            {
                result = controls[i];
                break;
            }
        }

        return result;
    }

}

