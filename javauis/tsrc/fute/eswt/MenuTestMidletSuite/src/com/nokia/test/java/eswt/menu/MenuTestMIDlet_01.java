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

package com.nokia.test.java.eswt.menu;

import org.eclipse.swt.SWT;
import org.eclipse.swt.events.SelectionEvent;
import org.eclipse.swt.graphics.Image;
import org.eclipse.swt.layout.FormAttachment;
import org.eclipse.swt.layout.FormData;
import org.eclipse.swt.layout.FormLayout;

import org.eclipse.swt.widgets.Button;
import org.eclipse.swt.widgets.Menu;
import org.eclipse.swt.widgets.MenuItem;
import org.eclipse.swt.widgets.MessageBox;

import com.nokia.test.java.eswt.midlet.TestBaseMIDlet;

/**
 * this class is intended to test the menu and menuitem components
 * - enabling and disabling menuitems
 * - menu styles
 * - menu levels
 * - menuitem styles
 * - setting text in menuitems
 */
public class MenuTestMIDlet_01 extends TestBaseMIDlet
{
    /** "enable" menuitem */
    private MenuItem mnuEnable = null;
    /** "disable" menuitem */
    private MenuItem mnuDisable = null;

    /** small image */
    private Image smallimg = null;
    /** big image */
    private Image bigimg = null;
    /** transparent image */
    private Image transimg = null;

    private Button topLeftField     = null;
    private Button topRightField    = null;
    private Button bottomLeftField  = null;
    private Button bottomRightField = null;

    protected void showException(String msg)
    {
        MessageBox mb = new MessageBox(this.shell, SWT.ICON_INFORMATION | SWT.OK);
        mb.setText("Exception");
        mb.setMessage(msg);
        mb.open();
    }

    /**
     * initialization
     */
    protected void init()
    {
        // customize the shell
        this.shell.setText("Menu & MenuItem");
        // create and set the layout
        this.shell.setLayout(new FormLayout());
        // try to initialize the images
        try
        {
            this.smallimg = new Image(this.display, this.getClass().getResourceAsStream("/res/sphere.jpg"));
        }
        catch (Exception ex)
        {
            // show a messagebox about the exception
            this.showException("Small image could not be loaded");
            this.smallimg = null;
        }
        try
        {
            this.bigimg = new Image(this.display, this.getClass().getResourceAsStream("/res/SmallLabel.jpg"));
        }
        catch (Exception ex)
        {
            // show a messagebox about the exception
            this.showException("Big image could not be loaded");
            this.bigimg = null;
        }
        try
        {
            this.transimg = new Image(this.display, this.getClass().getResourceAsStream("/res/alphatest_small.png"));
        }
        catch (Exception ex)
        {
            // show a messagebox about the exception
            this.showException("Transparent image could not be loaded");
            this.transimg = null;
        }

        // setup the menu
        Menu mainmenu = new Menu(this.shell, SWT.BAR);
        this.shell.setMenuBar(mainmenu);
        // "2nd level" submenu
        Menu menu = new Menu(mainmenu);
        MenuItem item = new MenuItem(mainmenu, SWT.CASCADE);
        item.setText("2nd level");
        item.setMenu(menu);
        // "3rd level" submenu for "2nd level" submenu
        item = new MenuItem(menu, SWT.CASCADE);
        menu = new Menu(menu);
        item.setText("3rd level");
        item.setMenu(menu);
        // ""push" menuitem" menuitem for "3rd level" submenu
        item = new MenuItem(menu, SWT.PUSH);
        item.setText("\"Push\" MenuItem");
        // "4th" menuitem for "3rd level" submenu
        try
        {
            item = new MenuItem(menu, SWT.CASCADE);
            menu = new Menu(menu);
            item.setText("4th level");
            item.setMenu(menu);

            // ""push" menuitem" menuitem for "4th level" submenu
            item = new MenuItem(menu, SWT.PUSH);
            item.setText("\"Push\" MenuItem");
        }
        catch (Exception ex)
        {
            this.showException("Not able to create level 4: "+ ex.toString());
            menu = null;
            item = null;
        }
        // "5th" menuitem for "5th level" submenu
        try
        {
            item = new MenuItem(menu, SWT.CASCADE);
            menu = new Menu(menu);
            item.setText("5th level");
            item.setMenu(menu);

            // ""push" menuitem" menuitem for "5th level" submenu
            item = new MenuItem(menu, SWT.PUSH);
            item.setText("\"Push\" MenuItem");
        }
        catch (Exception ex)
        {
            this.showException("Not able to create level 5: "+ ex.toString());
            menu = null;
            item = null;
        }
        // "enabling" submenu
        menu = new Menu(mainmenu);
        item = new MenuItem(mainmenu, SWT.CASCADE);
        item.setText("Enabling");
        item.setMenu(menu);
        // "enable" menuitem for "enabling" submenu
        this.mnuEnable = new MenuItem(menu, SWT.PUSH);
        this.mnuEnable.setText("Enable");
        this.mnuEnable.addSelectionListener(this);
        // "disable" menuitem for "enabling" submenu
        this.mnuDisable = new MenuItem(menu, SWT.PUSH);
        this.mnuDisable.setText("Disable");
        this.mnuDisable.setEnabled(false);
        this.mnuDisable.addSelectionListener(this);
        // "item style" submenu
        menu = new Menu(mainmenu);
        item = new MenuItem(mainmenu, SWT.CASCADE);
        item.setText("Item Style - Check");
        item.setMenu(menu);
        // "push item 1" menuitem for "check" submenu
        item = new MenuItem(menu, SWT.PUSH);
        item.setText("Push Item 1");
        // "check item 1" menuitem for "check" submenu
        item = new MenuItem(menu, SWT.CHECK);
        item.setText("Check Item 1");
        // "check item 2" menuitem for "check" submenu
        item = new MenuItem(menu, SWT.CHECK);
        item.setText("Check Item 2");
        // "check item 3" menuitem for "check" submenu
        item = new MenuItem(menu, SWT.CHECK);
        item.setText("Check Item 3");
        // "push item 2" menuitem for "check" submenu
        item = new MenuItem(menu, SWT.PUSH);
        item.setText("Push Item 2");

        // "item style" submenu
        menu = new Menu(mainmenu);
        item = new MenuItem(mainmenu, SWT.CASCADE);
        item.setText("Item Style - Radio");
        item.setMenu(menu);
        // "push item 1" menuitem for "check" submenu
        item = new MenuItem(menu, SWT.PUSH);
        item.setText("Push Item");
        // "radio item 1" menuitem for "check" submenu
        item = new MenuItem(menu, SWT.RADIO);
        item.setText("Radio Item 1");
        // "radio item 2" menuitem for "check" submenu
        item = new MenuItem(menu, SWT.RADIO);
        item.setText("Radio Item 2");
        // "radio item 3" menuitem for "check" submenu
        item = new MenuItem(menu, SWT.RADIO);
        item.setText("Radio Item 3");
        // "push item 2" menuitem for "check" submenu
        item = new MenuItem(menu, SWT.PUSH);
        item.setText("Push Item 2");
        // "text" submenu
        menu = new Menu(mainmenu);
        item = new MenuItem(mainmenu, SWT.CASCADE);
        item.setText("Text");
        item.setMenu(menu);
        // ("empty") menuitem for "text" submenu
        item = new MenuItem(menu, SWT.PUSH);
        item.setText("");
        // ("normal") menuitem for "text" submenu
        item = new MenuItem(menu, SWT.PUSH);
        item.setText("Normal");
        // ("long") menuitem for "text" submenu
        item = new MenuItem(menu, SWT.PUSH);
        item.setText("This is a long text for the menuitem's label. If it does not fit on the display, then it is truncated.");

        // "image" submenu
        menu = new Menu(mainmenu);
        item = new MenuItem(mainmenu, SWT.CASCADE);
        item.setText("Image");
        item.setMenu(menu);
        // ("smallimage") menuitem for "image" submenu
        item = new MenuItem(menu, SWT.PUSH);
        item.setImage(this.smallimg);
        // ("bigimage") menuitem for "image" submenu
        item = new MenuItem(menu, SWT.PUSH);
        item.setImage(this.bigimg);
        // ("transparent") menuitem for "image" submenu
        item = new MenuItem(menu, SWT.PUSH);
        item.setImage(this.transimg);
        // ("image and text") menuitem for "image" submenu
        item = new MenuItem(menu, SWT.PUSH);
        item.setText("small image");
        item.setImage(this.smallimg);

        // create Text fields
        // top left
        this.topLeftField = new Button(this.shell, SWT.PUSH);
        this.topLeftField.setText("getMenu()");

        // create and set the layout data
        FormData tldata = new FormData();
        tldata.left = new FormAttachment(0, 0);
        tldata.top = new FormAttachment(0, 0);
        tldata.right = new FormAttachment(40, 0);
        this.topLeftField.setLayoutData(tldata);

        // top right
        this.topRightField = new Button(this.shell, SWT.PUSH);
        this.topRightField.setText("getMenuBar()");
        // create and set the layout data
        FormData trdata = new FormData();
        trdata.left = new FormAttachment(50, 0);
        trdata.top = new FormAttachment(0, 0);
        trdata.right = new FormAttachment(90, 0);
        this.topRightField.setLayoutData(trdata);

        // bottom left
        this.bottomLeftField = new Button(this.shell, SWT.PUSH);
        this.bottomLeftField.setText("Disable");
        // create and set the layout data
        FormData bldata = new FormData();
        bldata.left = new FormAttachment(0, 0);
        bldata.top = new FormAttachment(50, 0);
        bldata.right = new FormAttachment(40, 0);
        this.bottomLeftField.setLayoutData(bldata);

        // top right
        this.bottomRightField = new Button(this.shell, SWT.PUSH);
        this.bottomRightField.setText("Enable");
        // create and set the layout data
        FormData brdata = new FormData();
        brdata.left = new FormAttachment(50, 0);
        brdata.top = new FormAttachment(50, 0);
        brdata.right = new FormAttachment(90, 0);
        this.bottomRightField.setLayoutData(brdata);


        this.topLeftField.addSelectionListener(this);
        this.topRightField.addSelectionListener(this);
        this.bottomLeftField.addSelectionListener(this);
        this.bottomRightField.addSelectionListener(this);
        // update the layout and redraw
        this.shell.layout();
        this.shell.update();
    }

    /**
     * selectionlistener implementation
     */
    public void widgetSelected(SelectionEvent event)
    {

        if (event.widget == this.mnuEnable || event.widget == this.mnuDisable)
        {
            // excahnge enabled / disabled menuitems
            this.mnuEnable.setEnabled(!this.mnuEnable.getEnabled());
            this.mnuDisable.setEnabled(!this.mnuDisable.getEnabled());
            return;
        }
        if (event.widget == this.topLeftField)
        {
            this.shell.getMenu().setVisible(true);
            return;
        }
        if (event.widget == this.topRightField)
        {
            this.shell.getMenuBar().setVisible(true);
            return;
        }
        if (event.widget == this.bottomLeftField)
        {
            this.shell.getMenuBar().setEnabled(false);
            return;
        }
        if (event.widget == this.bottomRightField)
        {
            this.shell.getMenuBar().setEnabled(true);
            return;
        }
        super.widgetSelected(event);
    }
}
