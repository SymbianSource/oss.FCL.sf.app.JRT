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

package com.nokia.test.java.eswt.list;

import org.eclipse.swt.SWT;
import org.eclipse.swt.events.SelectionEvent;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.widgets.Label;
import org.eclipse.swt.widgets.List;
import org.eclipse.swt.widgets.Menu;
import org.eclipse.swt.widgets.MenuItem;
import org.eclipse.swt.widgets.Text;

import com.nokia.test.java.eswt.midlet.TestBaseMIDlet;

/**
 * this class is intended to test the list component
 * - adding item to multiple list
 * - removing item from multi list
 * - setting items in multiple list
 * - selecting items in multiple list
 */
public class ListTestMIDlet_03 extends TestBaseMIDlet
{
    /** text instance */
    private Text newItem = null;
    /** list instance */
    private List list = null;

    /**
     * initialization
     */
    protected void init()
    {
        // customize the shell
        this.shell.setText("List 3");
        this.shell.setLayout(new GridLayout(1, true));

        // set up the default List
        this.list = new List(this.shell, SWT.MULTI | SWT.BORDER | SWT.V_SCROLL);
        this.list.setItems(new String[] {"Item 1", "Item 2", "Item 3", "Item 4", "Item 5"});
        this.list.setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, true));

        // "new item:" label
        Label label = new Label(this.shell, SWT.NONE);
        label.setText("New item:");
        label.setLayoutData(new GridData(SWT.LEFT, SWT.CENTER, false, false));
        // input field for "new:"
        this.newItem = new Text(this.shell, SWT.BORDER);
        this.newItem.setLayoutData(new GridData(SWT.FILL, SWT.CENTER, true, false));

        // set up the menu
        Menu mainmenu = new Menu(this.shell, SWT.BAR);
        this.shell.setMenuBar(mainmenu);
        // "add" submenu
        Menu menu = new Menu(mainmenu);
        MenuItem item = new MenuItem(mainmenu, SWT.CASCADE);
        item.setText("Add");
        item.setMenu(menu);
        // "add / before" menuitem
        item = new MenuItem(menu, SWT.PUSH);
        item.setText("Before");
        item.addSelectionListener(this);
        // "add / after" menuitem
        item = new MenuItem(menu, SWT.PUSH);
        item.setText("After");
        item.addSelectionListener(this);
        // "remove" submenu
        menu = new Menu(mainmenu);
        item = new MenuItem(mainmenu, SWT.CASCADE);
        item.setText("Remove");
        item.setMenu(menu);
        // "remove / selected" menuitem
        item = new MenuItem(menu, SWT.PUSH);
        item.setText("Selected");
        item.addSelectionListener(this);
        // "remove / all" menuitem
        item = new MenuItem(menu, SWT.PUSH);
        item.setText("All");
        item.addSelectionListener(this);
        // "select" submenu
        menu = new Menu(mainmenu);
        item = new MenuItem(mainmenu, SWT.CASCADE);
        item.setText("Select");
        item.setMenu(menu);
        // "select / focused" menuitem
        item = new MenuItem(menu, SWT.PUSH);
        item.setText("Focused");
        item.addSelectionListener(this);
        // "select / all" menuitem
        item = new MenuItem(menu, SWT.PUSH);
        item.setText("All");
        item.addSelectionListener(this);
        // "select / deselect" menuitem
        item = new MenuItem(menu, SWT.PUSH);
        item.setText("Deselect");
        item.addSelectionListener(this);
        // "select / none" menuitem
        item = new MenuItem(menu, SWT.PUSH);
        item.setText("None");
        item.addSelectionListener(this);
        // "set" submenu
        menu = new Menu(mainmenu);
        item = new MenuItem(mainmenu, SWT.CASCADE);
        item.setText("Set");
        item.setMenu(menu);
        // "set / selected" menuitem
        item = new MenuItem(menu, SWT.PUSH);
        item.setText("Selected");
        item.addSelectionListener(this);
        // "set / defaults" menuitem
        item = new MenuItem(menu, SWT.PUSH);
        item.setText("Defaults");
        item.addSelectionListener(this);
    }

    /**
     * selectionlistener implementation
     */
    public void widgetSelected(SelectionEvent event)
    {
        if (event.widget instanceof MenuItem)
        {
            String scmd = ((MenuItem) event.widget).getText();
            int[] index = this.list.getSelectionIndices();
            if ("Before".equals(scmd))
            {
                // add an item before the selected item(s)
                if (index.length > 0)
                    for (int i = index.length-1; i >= 0; i--)
                        this.list.add(this.newItem.getText(), index[i]);
                return;
            }
            if ("After".equals(scmd))
            {
                // add an item after the selected item(s)
                if (index.length > 0)
                    for (int i = index.length-1; i >= 0; i--)
                        this.list.add(this.newItem.getText(), index[i]+1);
                return;
            }
            if ("Selected".equals(scmd))
            {
                scmd = ((MenuItem) event.widget).getParent().getParentItem().getText();
                if ("Remove".equals(scmd))
                {
                    // remove the selected item(s)
                    if (index.length > 0)
                        this.list.remove(index);
                    return;
                }
                if ("Set".equals(scmd))
                {
                    // set the selected item(s)
                    for (int i = 0; i < index.length; i++)
                        this.list.setItem(index[i], this.newItem.getText());
                    return;
                }
            }
            if ("Focused".equals(scmd))
            {
                // select the focused item
                int focus = this.list.getFocusIndex();
                if (focus > -1)
                    this.list.select(focus);
                return;
            }
            if ("All".equals(scmd))
            {
                scmd = ((MenuItem) event.widget).getParent().getParentItem().getText();
                if ("Remove".equals(scmd))
                {
                    // remove all items
                    this.list.removeAll();
                    return;
                }
                if ("Select".equals(scmd))
                {
                    // select all items
                    this.list.selectAll();
                    return;
                }
            }
            if ("Deselect".equals(scmd))
            {
                // deselect the focused item
                int focus = this.list.getFocusIndex();
                if (focus > -1)
                    this.list.deselect(focus);
                return;
            }
            if ("None".equals(scmd))
            {
                // deselect all items
                this.list.deselectAll();
                return;
            }
            if ("Defaults".equals(scmd))
            {
                // set the list items to the dafaults
                this.list.setItems(new String[] {"Item 1", "Item 2", "Item 3", "Item 4", "Item 5"});
                return;
            }
        }
        super.widgetSelected(event);
    }
}
