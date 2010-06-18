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

package com.nokia.test.java.eswt.slider;

import org.eclipse.ercp.swt.mobile.Command;
import org.eclipse.ercp.swt.mobile.ConstrainedText;
import org.eclipse.swt.SWT;
import org.eclipse.swt.events.SelectionEvent;
import org.eclipse.swt.events.ShellEvent;
import org.eclipse.swt.events.ShellListener;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.widgets.Label;
import org.eclipse.swt.widgets.Shell;
import org.eclipse.swt.widgets.Slider;

import com.nokia.test.java.eswt.midlet.TestBaseMIDlet;

/**
 * this class is intended to test the slider component
 * - creating slider
 * - changing values of slider
 * - operating slider
 */
public class SliderTestMIDlet_01 extends TestBaseMIDlet implements ShellListener
{
    /** screen to get initial slider parameters */
    private Shell startScreen = null;
    /** screen for changing slider settings */
    private Shell settingsShell = null;
    /** min. value (startup) */
    private ConstrainedText minStart = null;
    /** max. value (startup) */
    private ConstrainedText maxStart = null;
    /** min. value (settings) */
    private ConstrainedText minSet = null;
    /** max. value (settings) */
    private ConstrainedText maxSet = null;
    /** selection value */
    private ConstrainedText selection = null;
    /** thumb size */
    private ConstrainedText thumb = null;
    /** increment value */
    private ConstrainedText increment = null;
    /** pageincrement value */
    private ConstrainedText pageinc = null;
    /** slider instance */
    private Slider slider = null;

    /**
     * create startup screen
     */
    private void setupStartupShell()
    {
        // screen for the initial slider creation
        this.startScreen = new Shell(this.display, SWT.SHELL_TRIM);
        this.startScreen.setText("Create Slider");
        this.startScreen.setLayout(new GridLayout(2, false));

        // "min. value:" label
        Label label = new Label(this.startScreen, SWT.NONE);
        label.setText("Min. value:");
        label.setLayoutData(new GridData(SWT.RIGHT, SWT.CENTER, false, false));
        // input field for "min. value:"
        this.minStart = new ConstrainedText(this.startScreen, SWT.BORDER, ConstrainedText.NUMERIC);
        this.minStart.setText(String.valueOf(0));
        this.minStart.setLayoutData(new GridData(SWT.FILL, SWT.CENTER, true, false));

        // "max. value:" label
        label = new Label(this.startScreen, SWT.NONE);
        label.setText("Max. value:");
        label.setLayoutData(new GridData(SWT.RIGHT, SWT.CENTER, false, false));
        // input field for "max. value:"
        this.maxStart = new ConstrainedText(this.startScreen, SWT.BORDER, ConstrainedText.NUMERIC);
        this.maxStart.setText(String.valueOf(100));
        this.maxStart.setLayoutData(new GridData(SWT.FILL, SWT.CENTER, true, false));

        // "create" left softkey command
        Command cmd = new Command(this.startScreen, Command.SELECT, 0);
        cmd.setText("Create");
        cmd.addSelectionListener(this);
    }

    /**
     * create settings screen
     */
    private void setupSettingsShell()
    {
        // screen for changing the slider's settings
        this.settingsShell = new Shell(this.display, SWT.SHELL_TRIM);
        this.settingsShell.setText("Settings");
        this.settingsShell.setLayout(new GridLayout(2, false));

        // "min. value:" label
        Label label = new Label(this.settingsShell, SWT.NONE);
        label.setText("Min. value:");
        label.setLayoutData(new GridData(SWT.RIGHT, SWT.CENTER, false, false));
        // input field for "min. value:"
        this.minSet = new ConstrainedText(this.settingsShell, SWT.BORDER, ConstrainedText.NUMERIC);
        this.minSet.setLayoutData(new GridData(SWT.FILL, SWT.CENTER, true, false));

        // "max. value:" label
        label = new Label(this.settingsShell, SWT.NONE);
        label.setText("Max. value:");
        label.setLayoutData(new GridData(SWT.RIGHT, SWT.CENTER, false, false));
        // input field for "max. value:"
        this.maxSet = new ConstrainedText(this.settingsShell, SWT.BORDER, ConstrainedText.NUMERIC);
        this.maxSet.setLayoutData(new GridData(SWT.FILL, SWT.CENTER, true, false));

        // "selection:" label
        label = new Label(this.settingsShell, SWT.NONE);
        label.setText("Selection:");
        label.setLayoutData(new GridData(SWT.RIGHT, SWT.CENTER, false, false));
        // input field for " :"
        this.selection = new ConstrainedText(this.settingsShell, SWT.BORDER, ConstrainedText.NUMERIC);
        this.selection.setLayoutData(new GridData(SWT.FILL, SWT.CENTER, true, false));

        // "thumb:" label
        label = new Label(this.settingsShell, SWT.NONE);
        label.setText("Thumb:");
        label.setLayoutData(new GridData(SWT.RIGHT, SWT.CENTER, false, false));
        // input field for "thumb:"
        this.thumb = new ConstrainedText(this.settingsShell, SWT.BORDER, ConstrainedText.NUMERIC);
        this.thumb.setLayoutData(new GridData(SWT.FILL, SWT.CENTER, true, false));

        // "increment:" label
        label = new Label(this.settingsShell, SWT.NONE);
        label.setText("Increment:");
        label.setLayoutData(new GridData(SWT.RIGHT, SWT.CENTER, false, false));
        // input field for "increment:"
        this.increment = new ConstrainedText(this.settingsShell, SWT.BORDER, ConstrainedText.NUMERIC);
        this.increment.setLayoutData(new GridData(SWT.FILL, SWT.CENTER, true, false));

        // "page increment:" label
        label = new Label(this.settingsShell, SWT.NONE);
        label.setText("Page increment:");
        label.setLayoutData(new GridData(SWT.RIGHT, SWT.CENTER, false, false));
        // input field for page increment":"
        this.pageinc = new ConstrainedText(this.settingsShell, SWT.BORDER, ConstrainedText.NUMERIC);
        this.pageinc.setLayoutData(new GridData(SWT.FILL, SWT.CENTER, true, false));

        // "ok" left softkey command
        Command cmd = new Command(this.settingsShell, Command.SELECT, 0);
        cmd.setText("Ok");
        cmd.addSelectionListener(this);
        // "cancel" right softkey command
        cmd = new Command(this.settingsShell, Command.EXIT, 0);
        cmd.setText("Cancel");
        cmd.addSelectionListener(this);

        this.settingsShell.open();
    }

    /**
     * initialization
     */
    protected void init()
    {
        // customize the shell
        this.shell.setText("Slider Test");
        this.shell.addShellListener(this);
        this.shell.setLayout(new GridLayout());

        // "change" left softkey command
        Command cmd = new Command(this.shell, Command.SELECT, 0);
        cmd.setText("Change");
        cmd.addSelectionListener(this);
        // "close" right softkey command
        cmd = new Command(this.shell, Command.EXIT, 0);
        cmd.setText("Close");
        cmd.addSelectionListener(this);

        // setup the initial screen
        this.setupStartupShell();
    }

    /**
     * selectionlistener implementation
     */
    public void widgetSelected(SelectionEvent event)
    {
        if (event.widget instanceof Command)
        {
            String scmd = ((Command) event.widget).getText();
            if ("Create".equals(scmd))
            {
                // create the slider
                this.slider = new Slider(this.shell, SWT.HORIZONTAL);
                this.slider.setMinimum(Integer.parseInt(this.minStart.getText()));
                this.slider.setMaximum(Integer.parseInt(this.maxStart.getText()));
                this.slider.setLayoutData(new GridData(SWT.FILL, SWT.CENTER, true, true));
                this.shell.layout();
                this.shell.setActive();
                return;
            }
            if ("Change".equals(scmd))
            {
                // open the settings screen
                if (this.settingsShell == null)
                    this.setupSettingsShell();
                this.minSet.setText(String.valueOf(this.slider.getMinimum()));
                this.maxSet.setText(String.valueOf(this.slider.getMaximum()));
                this.selection.setText(String.valueOf(this.slider.getSelection()));
                this.thumb.setText(String.valueOf(this.slider.getThumb()));
                this.increment.setText(String.valueOf(this.slider.getIncrement()));
                this.pageinc.setText(String.valueOf(this.slider.getPageIncrement()));
                this.settingsShell.setActive();
                return;
            }
            if ("Close".equals(scmd))
            {
                // close the screen with the slider
                this.minStart.setText(String.valueOf(this.slider.getMinimum()));
                this.maxStart.setText(String.valueOf(this.slider.getMaximum()));
                this.slider.dispose();
                this.startScreen.setActive();
                return;
            }
            if ("Ok".equals(scmd))
            {
                // change slider parameters
                this.slider.setValues(Integer.parseInt(this.selection.getText()), Integer.parseInt(this.minSet.getText()), Integer.parseInt(this.maxSet.getText()), Integer.parseInt(this.thumb.getText()), Integer.parseInt(this.increment.getText()), Integer.parseInt(this.pageinc.getText()));
                this.shell.setActive();
                return;
            }
            if ("Cancel".equals(scmd))
            {
                // cancel new settings
                this.shell.setActive();
                return;
            }
            if (event.widget == this.cmdExit)
            {
                // exit the midlet
                if (this.settingsShell != null && !this.settingsShell.isDisposed())
                    this.settingsShell.dispose();
                if (this.startScreen != null && !this.startScreen.isDisposed())
                    this.startScreen.dispose();
            }
        }
        super.widgetSelected(event);
    }

    /**
     * shelllistener implementation; shell activated
     */
    public void shellActivated(ShellEvent event)
    {
        // at start change to the initial screen
        this.cmdExit.dispose();
        this.cmdExit = new Command(this.startScreen, Command.EXIT, 0);
        this.cmdExit.setText("Exit");
        this.cmdExit.addSelectionListener(this);
        this.shell.removeShellListener(this);
        this.startScreen.layout();
        this.startScreen.open();
    }

    /**
     * shelllistener implementation; shell activated
     */
    public void shellClosed(ShellEvent event)
    {
        // N/A
    }

    /**
     * shelllistener implementation; shell deactivated
     */
    public void shellDeactivated(ShellEvent event)
    {
        // N/A
    }

    /**
     * shelllistener implementation; shell deiconified
     */
    public void shellDeiconified(ShellEvent event)
    {
        // N/A
    }

    /**
     * shelllistener implementation; shell iconified
     */
    public void shellIconified(ShellEvent event)
    {
        // N/A
    }
}
