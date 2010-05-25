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

package com.nokia.test.java.eswt.mobileshell;

import org.eclipse.ercp.swt.mobile.Command;
import org.eclipse.ercp.swt.mobile.MobileShell;
import org.eclipse.swt.SWT;
import org.eclipse.swt.events.KeyEvent;
import org.eclipse.swt.events.KeyListener;
import org.eclipse.swt.events.MouseEvent;
import org.eclipse.swt.events.MouseListener;
import org.eclipse.swt.events.SelectionEvent;
import org.eclipse.swt.graphics.Image;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.widgets.Label;
import org.eclipse.swt.widgets.Menu;
import org.eclipse.swt.widgets.MenuItem;
import org.eclipse.swt.widgets.MessageBox;
import org.eclipse.swt.widgets.Text;

import com.nokia.test.java.eswt.midlet.TestBaseMIDlet;

/**
 * this class is intended to test the mobileshell component
 * - changing status style of mobileshell
 * - changing to full screen mode
 * - set title & image
 * - creating mobileshell
 * - setting status text
 */
public class MobileShellTestMIDlet_01 extends TestBaseMIDlet implements KeyListener, MouseListener
{
    /** menuitems... */
    private MenuItem mnuNone = null;
    private MenuItem mnuSmall = null;
    private MenuItem mnuLarge = null;
    private MenuItem mnuShell = null;
    private MenuItem mnuDialog = null;
    private MenuItem mnuNo = null;
    private MenuItem mnuTitle = null;
    private MenuItem mnuClose = null;
    private MenuItem mnuBorder = null;
    private MenuItem mnuTEmpty = null;
    private MenuItem mnuTNormal = null;
    private MenuItem mnuTLong = null;
    private MenuItem mnuOn = null;
    private MenuItem mnuOff = null;
    private MenuItem mnuSEmpty = null;
    private MenuItem mnuSNormal = null;
    private MenuItem mnuSLong = null;
    private MenuItem mnuMShell = null;
    /** custom icon for the mobileshell */
    private Image icon = null;
    /** mobileshell instance */
    private MobileShell mshell = null;
    /** "enable" / "disable" command */
    private Command cmdSwitch = null;
    /** text instance */
    private Text text = null;
    /** status pane style */
    private int status;
    /** trimming style */
    private int trimming;
    /** title pane text */
    private String titleText;
    /** image on the status pane */
    private boolean img;
    /** status pane text */
    private String statusText;

    /**
     * initialization
     */
    protected void init()
    {
        // customize the shell
        this.shell.setText("MobileShell Test");
        // default values
        this.status = MobileShell.NO_STATUS_PANE;
        this.trimming = SWT.SHELL_TRIM;
        this.titleText = "";
        this.img = false;
        this.statusText = "";
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
        // try to initialize the image
        try
        {
            this.icon = new Image(this.display, this.getClass().getResourceAsStream("/res/icon.png"));
        }
        catch (Exception ex)
        {
            // show a messagebox about the exception
            MessageBox mb = new MessageBox(this.shell, SWT.ICON_INFORMATION | SWT.OK);
            mb.setText("Exception");
            mb.setMessage("Icon could not be loaded");
            mb.open();
            this.icon = null;
        }

        // set up the menu
        Menu mainmenu = new Menu(this.shell, SWT.BAR);
        this.shell.setMenuBar(mainmenu);
        // "status pane" submenu
        Menu menu = new Menu(mainmenu);
        MenuItem item = new MenuItem(mainmenu, SWT.CASCADE);
        item.setText("Status Pane");
        item.setMenu(menu);
        // "none" menuitem for "status pane" submenu
        this.mnuNone = new MenuItem(menu, SWT.PUSH);
        this.mnuNone.setText("* None");
        this.mnuNone.addSelectionListener(this);
        // "small" menuitem for "status pane" submenu
        this.mnuSmall = new MenuItem(menu, SWT.PUSH);
        this.mnuSmall.setText("Small");
        this.mnuSmall.addSelectionListener(this);
        // "large" menuitem for "status pane" submenu
        this.mnuLarge = new MenuItem(menu, SWT.PUSH);
        this.mnuLarge.setText("Large");
        this.mnuLarge.addSelectionListener(this);
        // "trimming" submenu
        menu = new Menu(mainmenu);
        item = new MenuItem(mainmenu, SWT.CASCADE);
        item.setText("Trimming");
        item.setMenu(menu);
        // "shell_trim" menuitem for "trimming" submenu
        this.mnuShell = new MenuItem(menu, SWT.PUSH);
        this.mnuShell.setText("* Shell_Trim");
        this.mnuShell.addSelectionListener(this);
        // "dialog_trim" menuitem for "trimming" submenu
        this.mnuDialog = new MenuItem(menu, SWT.PUSH);
        this.mnuDialog.setText("Dialog_Trim");
        this.mnuDialog.addSelectionListener(this);
        // "no_trim" menuitem for "trimming" submenu
        this.mnuNo = new MenuItem(menu, SWT.PUSH);
        this.mnuNo.setText("No_Trim");
        this.mnuNo.addSelectionListener(this);
        // "title" menuitem for "trimming" submenu
        this.mnuTitle = new MenuItem(menu, SWT.PUSH);
        this.mnuTitle.setText("Title");
        this.mnuTitle.addSelectionListener(this);
        // "close" menuitem for "trimming" submenu
        this.mnuClose = new MenuItem(menu, SWT.PUSH);
        this.mnuClose.setText("Close");
        this.mnuClose.addSelectionListener(this);
        // "border" menuitem for "trimming" submenu
        this.mnuBorder = new MenuItem(menu, SWT.PUSH);
        this.mnuBorder.setText("Border");
        this.mnuBorder.addSelectionListener(this);
        // "set title text" submenu
        menu = new Menu(mainmenu);
        item = new MenuItem(mainmenu, SWT.CASCADE);
        item.setText("Set Title Text");
        item.setMenu(menu);
        // "empty" menuitem for "set title text" submenu
        this.mnuTEmpty = new MenuItem(menu, SWT.PUSH);
        this.mnuTEmpty.setText("* Empty");
        this.mnuTEmpty.addSelectionListener(this);
        // "normal" menuitem for "set title text" submenu
        this.mnuTNormal = new MenuItem(menu, SWT.PUSH);
        this.mnuTNormal.setText("Normal");
        this.mnuTNormal.addSelectionListener(this);
        // "long" menuitem for "set title text" submenu
        this.mnuTLong = new MenuItem(menu, SWT.PUSH);
        this.mnuTLong.setText("Long");
        this.mnuTLong.addSelectionListener(this);
        // "image" submenu
        menu = new Menu(mainmenu);
        item = new MenuItem(mainmenu, SWT.CASCADE);
        item.setText("Image");
        item.setMenu(menu);
        // "on" menuitem for "image" submenu
        this.mnuOn = new MenuItem(menu, SWT.PUSH);
        this.mnuOn.setText("On");
        this.mnuOn.addSelectionListener(this);
        // "off" menuitem for "image" submenu
        this.mnuOff = new MenuItem(menu, SWT.PUSH);
        this.mnuOff.setText("* Off");
        this.mnuOff.addSelectionListener(this);
        // "set status text" submenu
        menu = new Menu(mainmenu);
        item = new MenuItem(mainmenu, SWT.CASCADE);
        item.setText("Set Status Text");
        item.setMenu(menu);
        // "empty" menuitem for "set status text" submenu
        this.mnuSEmpty = new MenuItem(menu, SWT.PUSH);
        this.mnuSEmpty.setText("* Empty");
        this.mnuSEmpty.addSelectionListener(this);
        // "normal" menuitem for "set status text" submenu
        this.mnuSNormal = new MenuItem(menu, SWT.PUSH);
        this.mnuSNormal.setText("Normal");
        this.mnuSNormal.addSelectionListener(this);
        // "long" menuitem for "set status text" submenu
        this.mnuSLong = new MenuItem(menu, SWT.PUSH);
        this.mnuSLong.setText("Long");
        this.mnuSLong.addSelectionListener(this);
        // "create" menuitem
        this.mnuMShell = new MenuItem(mainmenu, SWT.PUSH);
        this.mnuMShell.setText("Create");
        this.mnuMShell.addSelectionListener(this);
    }

    /**
     * update the mark in "status pane" submenu
     * @param newStatus the selected status pane style
     * @param item the selected menuitem
     */
    private void chgStatus(int newStatus, MenuItem item)
    {
        if (this.status != newStatus)
        {
            switch (this.status)
            {
            case MobileShell.NO_STATUS_PANE:
                this.mnuNone.setText("None");
                break;
            case MobileShell.SMALL_STATUS_PANE:
                this.mnuSmall.setText("Small");
                break;
            case MobileShell.LARGE_STATUS_PANE:
                this.mnuLarge.setText("Large");
                break;
            default:
                break;
            }
            this.status = newStatus;
            item.setText("* "+item.getText());
        }
    }

    /**
     * update the mark in "trimming" submenu
     * @param newTrimming the selected trimming style
     * @param item the selected menuitem
     */
    private void chgTrimming(int newTrimming, MenuItem item)
    {
        if (this.trimming != newTrimming)
        {
            switch (this.trimming)
            {
            case SWT.SHELL_TRIM:
                this.mnuShell.setText("Shell_Trim");
                break;
            case SWT.DIALOG_TRIM:
                this.mnuDialog.setText("Dialog_Trim");
                break;
            case SWT.NO_TRIM:
                this.mnuNo.setText("No_Trim");
                break;
            case SWT.TITLE:
                this.mnuTitle.setText("Title");
                break;
            case SWT.CLOSE:
                this.mnuClose.setText("Close");
                break;
            case SWT.BORDER:
                this.mnuBorder.setText("Border");
                break;
            default:
                break;
            }
            this.trimming = newTrimming;
            item.setText("* "+item.getText());
        }
    }

    /**
     * selectionlistener implementation
     */
    public void widgetSelected(SelectionEvent event)
    {
        if (event.widget instanceof MenuItem)
        {
            String scmd = ((MenuItem) event.widget).getText();
            if (event.widget == this.mnuNone)
            {
                // set the status pane to none
                this.chgStatus(MobileShell.NO_STATUS_PANE, this.mnuNone);
                return;
            }
            if (event.widget == this.mnuSmall)
            {
                // set the status pane to small
                this.chgStatus(MobileShell.SMALL_STATUS_PANE, this.mnuSmall);
                return;
            }
            if (event.widget == this.mnuLarge)
            {
                // set the status pane to large
                this.chgStatus(MobileShell.LARGE_STATUS_PANE, this.mnuLarge);
                return;
            }
            if (event.widget == this.mnuShell)
            {
                // set the trimming style to shell_trim
                this.chgTrimming(SWT.SHELL_TRIM, this.mnuShell);
                return;
            }
            if (event.widget == this.mnuDialog)
            {
                // set the trimming style to dialog_trim
                this.chgTrimming(SWT.DIALOG_TRIM, this.mnuDialog);
                return;
            }
            if (event.widget == this.mnuNo)
            {
                // set the trimming style to no_trim
                this.chgTrimming(SWT.NO_TRIM, this.mnuNo);
                return;
            }
            if (event.widget == this.mnuTitle)
            {
                // set the trimming style to title
                this.chgTrimming(SWT.TITLE, this.mnuTitle);
                return;
            }
            if (event.widget == this.mnuClose)
            {
                // set the trimming style to close
                this.chgTrimming(SWT.CLOSE, this.mnuClose);
                return;
            }
            if (event.widget == this.mnuBorder)
            {
                // set the trimming style to border
                this.chgTrimming(SWT.BORDER, this.mnuBorder);
                return;
            }
            if (event.widget == this.mnuTEmpty)
            {
                // set title text to an empty string
                this.mnuTEmpty.setText("* Empty");
                this.mnuTNormal.setText("Normal");
                this.mnuTLong.setText("Long");
                this.titleText = "";
                return;
            }
            if (event.widget == this.mnuTNormal)
            {
                // set a normal (short) title string
                this.mnuTEmpty.setText("Empty");
                this.mnuTNormal.setText("* Normal");
                this.mnuTLong.setText("Long");
                this.titleText = "MobileShell";
                return;
            }
            if (event.widget == this.mnuTLong)
            {
                // set a long title string
                this.mnuTEmpty.setText("Empty");
                this.mnuTNormal.setText("Normal");
                this.mnuTLong.setText("* Long");
                this.titleText = "This supposed to be a very long text for the shell title and most probably it does not fit on that area...";
                return;
            }
            if (event.widget == this.mnuOn)
            {
                // set a custom icon for the mobileshell
                this.mnuOn.setText("* On");
                this.mnuOff.setText("Off");
                this.img = true;
                return;
            }
            if (event.widget == this.mnuOff)
            {
                // disable custom icon
                this.mnuOn.setText("On");
                this.mnuOff.setText("* Off");
                this.img = false;
                return;
            }
            if (event.widget == this.mnuSEmpty)
            {
                // set the status text to an empty string
                this.mnuSEmpty.setText("* Empty");
                this.mnuSNormal.setText("Normal");
                this.mnuSLong.setText("Long");
                this.statusText = "";
                return;
            }
            if (event.widget == this.mnuSNormal)
            {
                // set a normal (short) status string
                this.mnuSEmpty.setText("Empty");
                this.mnuSNormal.setText("* Normal");
                this.mnuSLong.setText("Long");
                this.statusText = "MobileShell";
                return;
            }
            if (event.widget == this.mnuSLong)
            {
                // set a long status string
                this.mnuSEmpty.setText("Empty");
                this.mnuSNormal.setText("Normal");
                this.mnuSLong.setText("* Long");
                this.statusText = "This supposed to be a very long text for the status pane and most probably it does not fit on that area...";
                return;
            }
            if ("Create".equals(scmd))
            {
                // creata a mobileshell and apply the settings
                this.mshell = new MobileShell(this.display, this.trimming, this.status);
                this.mshell.setText(this.titleText);
                this.mshell.setStatusText(this.statusText);
                if (this.img)
                    this.mshell.setImage(this.icon);
                this.mshell.setLayout(new GridLayout(2, false));
                this.mshell.addKeyListener(this);
                this.mnuMShell.setText("Show");
                // display a label and a text finput field for testing purposes...
                Label label = new Label(this.mshell, SWT.LEFT);
                label.setText("Test input:");
                label.setLayoutData(new GridData(SWT.LEFT, SWT.CENTER, false, false));
                this.text = new Text(this.mshell, SWT.LEFT | SWT.BORDER);
                this.text.setText("test");
                this.text.setLayoutData(new GridData(SWT.LEFT, SWT.CENTER, false, false));
                this.text.addKeyListener(this);

                Label label2 = new Label(this.mshell, SWT.CENTER);
                label2.setText("Display CA:"+this.display.getClientArea().width+","+this.display.getClientArea().height);
                label2.setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, true));
                Label label3 = new Label(this.mshell, SWT.CENTER);
                label3.setText("MobileShell CA:"+this.mshell.getClientArea().width+","+this.mshell.getClientArea().height);
                label3.setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, true));
                Label label4 = new Label(this.mshell, SWT.CENTER);
                label4.setText("MS to D:"+this.mshell.toDisplay(0,0).x+","+this.mshell.toDisplay(0,0).y);
                label4.setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, true));

                // "close" right softkey command
                Command cmd = new Command(this.mshell, Command.EXIT, 0);
                cmd.setText("Close");
                cmd.addSelectionListener(this);
                // "hide" left softkey command
                cmd = new Command(this.mshell, Command.SELECT, 1);
                cmd.setText("Hide");
                cmd.addSelectionListener(this);
                // "disable text" left softkey command
                this.cmdSwitch = new Command(this.mshell, Command.SELECT, 0);
                this.cmdSwitch.setText("Disable Text");
                this.cmdSwitch.addSelectionListener(this);
                // open the mobileshell
                this.mshell.addMouseListener(this);
                this.mshell.layout();
                this.mshell.open();
                return;
            }
            if ("Show".equals(scmd))
            {
                // apply the new settings
                this.mshell.changeTrim(this.trimming, this.status);
                this.mshell.setText(this.titleText);
                this.mshell.setStatusText(this.statusText);
                if (this.img)
                    this.mshell.setImage(this.icon);
                else
                    this.mshell.setImage(null);
                // display the mobileshell
                this.mshell.setActive();
                return;
            }
        }
        else if (event.widget instanceof Command)
        {
            String scmd = ((Command) event.widget).getText();
            if ("Hide".equals(scmd))
            {
                // hide the mobileshell
                this.shell.setActive();
                return;
            }
            if ("Close".equals(scmd))
            {
                // close the mobileshell
                this.mnuMShell.setText("Create");
                this.mshell.dispose();
                this.mshell = null;
                return;
            }
            if (event.widget == this.cmdSwitch)
            {
                if (this.text.isEnabled())
                {
                    // disable the test input text
                    this.text.setEnabled(false);
                    this.cmdSwitch.setText("Enable Text");
                }
                else
                {
                    // enable the test input text
                    this.text.setEnabled(true);
                    this.cmdSwitch.setText("Disable Text");
                }
                return;
            }
            if (event.widget == this.cmdExit && this.icon != null && !this.icon.isDisposed())
                // dispose the image before exit
                this.icon.dispose();
        }
        super.widgetSelected(event);
    }
    public void mouseDoubleClick(MouseEvent e)
    {
        System.out.println(e.toString());
        this.text.setText(e.x+","+e.y);
    }
    public void mouseDown(MouseEvent e)
    {
        System.out.println(e.toString());
        this.text.setText(e.x+","+e.y);
    }
    public void mouseUp(MouseEvent e)
    {
        System.out.println(e.toString());
        this.text.setText(e.x+","+e.y);
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
        switch ((event.keyCode | SWT.KEYCODE_BIT))
        {
        case SWT.KEYPAD_0:
            // change the fullscreen mode
            this.mshell.setFullScreenMode(!this.mshell.getFullScreenMode());
            break;
        case SWT.KEYPAD_1:
            // hide the mobileshell
            this.shell.setActive();
            break;
        case SWT.KEYPAD_3:
            // close the mobileshell
            this.mnuMShell.setText("Create");
            this.mshell.dispose();
            this.mshell = null;
            break;
        default:
            break;
        }
    }
}
