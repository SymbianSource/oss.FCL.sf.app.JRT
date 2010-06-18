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

import org.eclipse.swt.events.FocusEvent;
import org.eclipse.swt.events.FocusListener;


import com.nokia.mid.ui.TactileFeedback;
import java.util.Date;

import com.nokia.test.java.eswt.control.CustomControl;
import com.nokia.test.java.eswt.midlet.TestBaseMIDlet;

public class ControlTestMIDlet_02 extends TestBaseMIDlet
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
    private Command cmdAddCustomControl     = null;
    private Command cmdAddImageCanvas       = null;
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

    private Command cmdRemoveArea               = null;
    private Command cmdAddAreas             = null;
    private Command cmdUnregister1          = null;
    private Command cmdUnregister2          = null;
    private Command cmdSwitch           = null;
    private Command cmdSupported    = null;

    private TactileFeedback tf              = null;
    /* (non-Javadoc)
     * @see com.nokia.test.java.eswt.midlet.TestBaseMIDlet#init()
     */
    protected void init()
    {
        // init the shell
        try
        {
            if (this.shell != null && !this.shell.isDisposed())
                this.shell.dispose();

            this.shell = new Shell(this.display, SWT.V_SCROLL);
            this.shell.setText("Control Test 02");

            GridLayout layout = new GridLayout(1, false);
            this.shell.setLayout(layout);

            this.tf = new TactileFeedback();
            // set the menu
            this.menuSetup();
        }
        catch (Exception ex)
        {
            // show a messagebox about the exception
            MessageBox mb = new MessageBox(this.shell, SWT.ICON_INFORMATION | SWT.OK);
            mb.setText("Exception");
            mb.setMessage(ex.getMessage());
            mb.open();
        }
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

        this.cmdAddLabel = new Command(add, Command.GENERAL, 9);
        this.cmdAddLabel.setText("Label");
        this.cmdAddLabel.addSelectionListener(this);

        this.cmdAddHyperlink = new Command(add, Command.GENERAL, 8);
        this.cmdAddHyperlink.setText("Hyperlink");
        this.cmdAddHyperlink.addSelectionListener(this);

        this.cmdAddText = new Command(add, Command.GENERAL, 7);
        this.cmdAddText.setText("Text");
        this.cmdAddText.addSelectionListener(this);

        this.cmdAddConstrainedText = new Command(add, Command.GENERAL, 6);
        this.cmdAddConstrainedText.setText("Constrained Text");
        this.cmdAddConstrainedText.addSelectionListener(this);

        this.cmdAddDateEditor = new Command(add, Command.GENERAL, 5);
        this.cmdAddDateEditor.setText("Date Editor");
        this.cmdAddDateEditor.addSelectionListener(this);

        this.cmdAddButton = new Command(add, Command.GENERAL, 4);
        this.cmdAddButton.setText("Button");
        this.cmdAddButton.addSelectionListener(this);

        this.cmdAddCombo = new Command(add, Command.GENERAL, 3);
        this.cmdAddCombo.setText("Combo");
        this.cmdAddCombo.addSelectionListener(this);

        this.cmdAddSlider = new Command(add, Command.GENERAL, 2);
        this.cmdAddSlider.setText("Slider");
        this.cmdAddSlider.addSelectionListener(this);

        this.cmdAddProgressBar = new Command(add, Command.GENERAL, 1);
        this.cmdAddProgressBar.setText("Progress Bar");
        this.cmdAddProgressBar.addSelectionListener(this);

        this.cmdAddCustomControl = new Command(add, Command.GENERAL, 1);
        this.cmdAddCustomControl.setText("CustomControl");
        this.cmdAddCustomControl.addSelectionListener(this);

        this.cmdAddImageCanvas = new Command(add, Command.GENERAL, 1);
        this.cmdAddImageCanvas.setText("ImageCanvas");
        this.cmdAddImageCanvas.addSelectionListener(this);

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
            new Command(this.cmdResize, Command.GENERAL, 1);
        this.cmdStretch.setText("Stretch");
        this.cmdStretch.addSelectionListener(this);

        this.cmdSkew =
            new Command(this.cmdResize, Command.GENERAL, 0);
        this.cmdSkew.setText("Skew");
        this.cmdSkew.addSelectionListener(this);

        this.cmdSetToolTip = new Command(this.shell, Command.GENERAL, 0);
        this.cmdSetToolTip.setText("Set ToolTip");
        this.cmdSetToolTip.addSelectionListener(this);

        this.cmdSetMenu = new Command(this.shell, Command.GENERAL, 0);
        this.cmdSetMenu.setText("Set Menu");
        this.cmdSetMenu.addSelectionListener(this);

        // tactile feedback submenu
        Command tactile = new Command(this.shell, Command.COMMANDGROUP, 1);
        tactile.setText("Tactile Feedback");

        this.cmdSwitch = new Command(tactile, Command.GENERAL, 9);
        this.cmdSwitch.setText("Switch");
        this.cmdSwitch.addSelectionListener(this);

        this.cmdUnregister1 = new Command(tactile, Command.GENERAL, 8);
        this.cmdUnregister1.setText("Unregister red area");
        this.cmdUnregister1.addSelectionListener(this);

        this.cmdUnregister2 = new Command(tactile, Command.GENERAL, 7);
        this.cmdUnregister2.setText("Unregister blue area");
        this.cmdUnregister2.addSelectionListener(this);

        this.cmdRemoveArea = new Command(tactile, Command.GENERAL, 6);
        this.cmdRemoveArea.setText("Remove Tactile Feedback");
        this.cmdRemoveArea.addSelectionListener(this);

        this.cmdAddAreas = new Command(tactile, Command.GENERAL, 0);
        this.cmdAddAreas.setText("Add areas");
        this.cmdAddAreas.addSelectionListener(this);

        this.cmdSupported = new Command(tactile, Command.GENERAL, 0);
        this.cmdSupported.setText("Is Supported");
        this.cmdSupported.addSelectionListener(this);

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
            this.enableCommands(true);
            changed = true;
        }
        if (event.widget == this.cmdAddHyperlink)
        {
            this.addHyperlink();
            this.enableCommands(true);
            changed = true;
        }
        if (event.widget == this.cmdAddText)
        {
            this.addText();
            this.enableCommands(true);
            changed = true;
        }
        if (event.widget == this.cmdAddConstrainedText)
        {
            this.addConstrainedText();
            this.enableCommands(true);
            changed = true;
        }
        if (event.widget == this.cmdAddDateEditor)
        {
            this.addDateEditor();
            this.enableCommands(true);
            changed = true;
        }
        if (event.widget == this.cmdAddButton)
        {
            this.addButton();
            this.enableCommands(true);
            changed = true;
        }
        if (event.widget == this.cmdAddCombo)
        {
            this.addCombo();
            this.enableCommands(true);
            changed = true;
        }
        if (event.widget == this.cmdAddSlider)
        {
            this.addSlider();
            this.enableCommands(true);
            changed = true;
        }
        if (event.widget == this.cmdAddProgressBar)
        {
            this.addProgressBar();
            this.enableCommands(true);
            changed = true;
        }
        if (event.widget == this.cmdAddCustomControl)
        {
            this.addCustomControl();
            this.enableCommands(true);
            changed = true;
        }
        if (event.widget == this.cmdAddImageCanvas)
        {
            this.addImageCanvas();
            this.enableCommands(true);
            changed = true;
        }
        if (event.widget == this.cmdAddAll)
        {
            this.addAll();
            this.enableCommands(true);
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

        // update the shell if needed
        if (changed)
        {
            if (this.shell.getChildren().length == 0)
            {
                this.enableCommands(false);
                this.cmdShowInvisible.setEnabled(false);
                this.cmdEnableDisabled.setEnabled(false);
            }
            this.shell.pack();
            this.shell.layout();
            this.shell.update();
            return;
        }
        else if (event.widget == this.cmdAddAreas)
        {
            this.addTactileFeedback(this.getSelectedControl());
        }
        else if (event.widget == this.cmdRemoveArea)
        {
            this.removeFeedback(this.getSelectedControl());
        }
        else if (event.widget == this.cmdUnregister1)
        {
            this.unregisterArea(this.getSelectedControl(),0);
        }
        else if (event.widget == this.cmdUnregister2)
        {
            this.unregisterArea(this.getSelectedControl(),1);
        }
        else if (event.widget == this.cmdSwitch)
        {
            this.switchAreas(this.getSelectedControl());
        }
        else if (event.widget == this.cmdSupported)
        {

            String note = null;
            if (tf.isTouchFeedbackSupported())
            {
                note = "true";
            }
            else
            {
                note = "false";
            }
            // show a messagebox
            MessageBox mb = new MessageBox(this.shell, SWT.ICON_INFORMATION | SWT.OK);
            mb.setText("Tactile Feedback");
            mb.setMessage("Is Supported: "+note);
            mb.open();

        }
        super.widgetSelected(event);
    }

    /**
     * Create a Label and add it to current shell.
     */
    private void enableCommands(boolean state)
    {
        this.cmdSetInvisible.setEnabled(state);
        this.cmdDisable.setEnabled(state);
        this.cmdRemove.setEnabled(state);
        this.cmdResize.setEnabled(state);

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
        control.addFocusListener(new FocusListener()
        {
            public void focusGained(FocusEvent e)
            {
                tf.directFeedback(TactileFeedback.FEEDBACK_STYLE_BASIC);
            }
            public void focusLost(FocusEvent e)
            {
                tf.directFeedback(TactileFeedback.FEEDBACK_STYLE_SENSITIVE);
            }
        });
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
        control.addFocusListener(new FocusListener()
        {
            public void focusGained(FocusEvent e)
            {
                tf.directFeedback(TactileFeedback.FEEDBACK_STYLE_BASIC);
            }
            public void focusLost(FocusEvent e)
            {
                tf.directFeedback(TactileFeedback.FEEDBACK_STYLE_SENSITIVE);
            }
        });
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
     * Create a Custom Control and add it to current shell.
     */
    private void addCustomControl()
    {
        try
        {
            // label
            /*Label label = new Label(this.shell, SWT.NONE);
            label.setText("Date Editor:");*/
            // control
            CustomControl control = new CustomControl(this.shell);
            //GridData data = new GridData();
            GridData data = new GridData(SWT.FILL,SWT.CENTER,true,true);
            control.setLayoutData(data);
        }
        catch (Exception ex)
        {

            this.showException(ex.getMessage());
        }

    }
    private void addImageCanvas()
    {
        try
        {
            // label
            /*Label label = new Label(this.shell, SWT.NONE);
            label.setText("Date Editor:");*/
            // control
            ImageCanvas control = new ImageCanvas(this.shell,SWT.NO_BACKGROUND);

            control.setBackground(this.display.getSystemColor(SWT.COLOR_YELLOW));
            GridData data = new GridData();
            data.grabExcessHorizontalSpace = true;
            data.horizontalAlignment = SWT.CENTER;
            control.setLayoutData(data);
        }
        catch (Exception ex)
        {

            this.showException(ex.getMessage());
        }

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

    private void addTactileFeedback(Control control)
    {

        tf.registerFeedbackArea(control,0,0,0,control.getBounds().width/2,control.getBounds().height,TactileFeedback.FEEDBACK_STYLE_BASIC);
        tf.registerFeedbackArea(control,1,control.getBounds().width/2,0,control.getBounds().width,control.getBounds().height,TactileFeedback.FEEDBACK_STYLE_SENSITIVE);

    }
    private void unregisterArea(Control control, int id)
    {
        if (control instanceof CustomControl)
        {
            CustomControl ctrl = (CustomControl)control;
            ctrl.unregisterArea(id);
            control = ctrl;
        }
        tf.unregisterFeedbackArea(control,id);
    }
    private void removeFeedback(Control control)
    {
        if (control instanceof CustomControl)
        {
            CustomControl ctrl = (CustomControl)control;
            ctrl.removeFeedback();
            control = ctrl;
        }
        else
        {
            tf.removeFeedbackForComponent(control);
        }
    }
    private void switchAreas(Control control)
    {

        if (control instanceof CustomControl)
        {
            CustomControl ctrl = (CustomControl)control;
            ctrl.switchAreas();
            control = ctrl;
        }
        else
        {

            tf.moveFeedbackAreaToFirstPriority(control,1);
        }
    }

}


