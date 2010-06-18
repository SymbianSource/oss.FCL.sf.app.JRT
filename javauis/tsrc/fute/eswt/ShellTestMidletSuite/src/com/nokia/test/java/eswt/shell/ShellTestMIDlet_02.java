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

package com.nokia.test.java.eswt.shell;

import org.eclipse.ercp.swt.mobile.Command;
import org.eclipse.swt.SWT;
import org.eclipse.swt.events.KeyEvent;
import org.eclipse.swt.events.KeyListener;
import org.eclipse.swt.events.SelectionEvent;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.widgets.Label;
import org.eclipse.swt.widgets.Menu;
import org.eclipse.swt.widgets.MenuItem;
import org.eclipse.swt.widgets.Shell;

import com.nokia.test.java.eswt.midlet.TestBaseMIDlet;

/**
 * this class is intended to test the shell component
 * - shell trimming
 * - shell stays on top
 * - using shell as popup dialog
 */
public class ShellTestMIDlet_02 extends TestBaseMIDlet implements KeyListener
{
    /** new shell instance */
    private Shell testshell = null;
    /** "close" command */
    private Command cmdClose = null;
    /** "maximize" / "restore" / "switch" command */
    private Command cmdSwitch = null;

    /**
     * initialization
     */
    protected void init()
    {
        // customize the shell
        this.shell.setText("Shell Test 2");
        this.shell.addKeyListener(this);
        this.shell.setLayout(new GridLayout());
        Label label = new Label(this.shell, SWT.CENTER);
        label.setText("Original Shell");
        label.setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, true));
        Label label2 = new Label(this.shell, SWT.CENTER);
        label2.setText("Display CA:"+this.display.getClientArea().width+","+this.display.getClientArea().height);
        label2.setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, true));
        Label label3 = new Label(this.shell, SWT.CENTER);
        label3.setText("Shell CA:"+this.shell.getClientArea().width+","+this.shell.getClientArea().height);
        label3.setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, true));
        Label label4 = new Label(this.shell, SWT.CENTER);
        label4.setText("S to D:"+this.shell.toDisplay(0,0).x+","+this.shell.toDisplay(0,0).y);
        label4.setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, true));
        // setup the menu
        Menu mainmenu = new Menu(this.shell, SWT.BAR);
        this.shell.setMenuBar(mainmenu);
        // "shell trim" menuitem
        MenuItem item = new MenuItem(mainmenu, SWT.PUSH);
        item.setText("Shell Trim");
        item.addSelectionListener(this);
        // "dialog trim" menuitem
        item = new MenuItem(mainmenu, SWT.PUSH);
        item.setText("Dialog Trim");
        item.addSelectionListener(this);
        // "no trim" menuitem
        item = new MenuItem(mainmenu, SWT.PUSH);
        item.setText("No Trim");
        item.addSelectionListener(this);
        // "on top" menuitem
        item = new MenuItem(mainmenu, SWT.PUSH);
        item.setText("On Top");
        item.addSelectionListener(this);
        // "popup" submenu
        Menu menu = new Menu(mainmenu);
        item = new MenuItem(mainmenu, SWT.CASCADE);
        item.setText("Popup");
        item.setMenu(menu);
        // "type 1" menuitem for "popup" submenu
        item = new MenuItem(menu, SWT.PUSH);
        item.setText("Type 1");
        item.addSelectionListener(this);
        // "type 2" menuitem for "popup" submenu
        item = new MenuItem(menu, SWT.PUSH);
        item.setText("Type 2");
        item.addSelectionListener(this);
    }

    /**
     * close the test shell and dispose its buttons
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
        // close the previous shell
        this.closeTestShell();
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
        Label label2 = new Label(this.testshell, SWT.CENTER);
        label2.setText("Display CA:"+this.display.getClientArea().width+","+this.display.getClientArea().height);
        label2.setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, true));
        Label label3 = new Label(this.testshell, SWT.CENTER);
        label3.setText("Shell CA:"+this.testshell.getClientArea().width+","+this.testshell.getClientArea().height);
        label3.setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, true));
        Label label4 = new Label(this.testshell, SWT.CENTER);
        label4.setText("S to D:"+this.testshell.toDisplay(0,0).x+","+this.testshell.toDisplay(0,0).y);
        label4.setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, true));
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
     * selectionlistener implementation
     */
    public void widgetSelected(SelectionEvent event)
    {
        if (event.widget instanceof MenuItem)
        {
            String scmd = ((MenuItem) event.widget).getText();
            if ("Shell Trim".equals(scmd))
            {
                // open a new shell with shell_trim
                this.createTestShell(SWT.SHELL_TRIM, "Shell", "Shell Trim Style", null, false);
                return;
            }
            if ("Dialog Trim".equals(scmd))
            {
                // open a new shell with dialog_trim
                this.createTestShell(SWT.DIALOG_TRIM, "Dialog", "Dialog Trim Style", "Maximize", true);
                return;
            }
            if ("No Trim".equals(scmd))
            {
                // open a new shell with no_trim
                this.createTestShell(SWT.NO_TRIM, "No", "No Trim Style", null, false);
                return;
            }
            if ("On Top".equals(scmd))
            {
                // open a new shell with on_top
                this.createTestShell(SWT.ON_TOP, "On Top", "Shell stays on top", "Switch", false);
                this.testshell.addKeyListener(this);
                return;
            }
            if ("Type 1".equals(scmd))
            {
                // open a shell as a popup with dialog trim
                this.createTestShell(SWT.DIALOG_TRIM | SWT.APPLICATION_MODAL, "Type 1", "Popup type 1", null, true);
                return;
            }
            if ("Type 2".equals(scmd))
            {
                // open a shell as a popup with border trim, no title
                this.createTestShell(SWT.BORDER | SWT.APPLICATION_MODAL, null, "Popup type 2", null, true);
                return;
            }
        }
        else if (event.widget instanceof Command)
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
            if (event.widget == this.cmdClose)
            {
                // close the test shell
                this.closeTestShell();
                return;
            }
        }
        super.widgetSelected(event);
    }

    /**
     *  a key is pressed...
     */
    public void keyPressed(KeyEvent event)
    {
        // N/A
    }

    /**
     * a key is released
     */
    public void keyReleased(KeyEvent event)
    {
        if ((event.keyCode | SWT.KEYCODE_BIT) == SWT.KEYPAD_0)
            // close the test shell
            this.closeTestShell();
    }
}
