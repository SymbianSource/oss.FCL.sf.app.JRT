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
package com.nokia.test.java.eswt.tree;

import org.eclipse.ercp.swt.mobile.Command;
import org.eclipse.swt.SWT;
import org.eclipse.swt.events.SelectionEvent;
import org.eclipse.swt.graphics.Font;
import org.eclipse.swt.graphics.Image;
import org.eclipse.swt.graphics.Point;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.layout.RowLayout;
import org.eclipse.swt.widgets.Control;
import org.eclipse.swt.widgets.Display;
import org.eclipse.swt.widgets.Label;
import org.eclipse.swt.widgets.Menu;
import org.eclipse.swt.widgets.MenuItem;
import org.eclipse.swt.widgets.MessageBox;
import org.eclipse.swt.widgets.Shell;
import org.eclipse.swt.widgets.Tree;
import org.eclipse.swt.widgets.TreeItem;
import org.eclipse.swt.widgets.FontDialog;

import com.nokia.test.java.eswt.midlet.TestBaseMIDlet;

public class TreeTestMIDlet_01 extends TestBaseMIDlet
{

    private Tree tree = null;

    /** small image */
    private Image smallimg = null;
    /** big image */
    private Image bigimg = null;
    /** transparent image */
    private Image transimg = null;

    private Shell tooltip = null;

    /** commands */
    protected Command cmdSingleTree = null;
    protected Command cmdMultiTree  = null;
    protected Command cmdCheckSingleTree    = null;
    protected Command cmdCheckMultiTree     = null;
    protected Command cmdImageSingleTree    = null;
    protected Command cmdImageMultiTree     = null;
    protected Command cmdLargeMultiTree     = null;
    protected Command cmdHideTree       = null;
    protected Command cmdLargeSingleTree        = null;
    protected Command cmdHeader         = null;
    protected Command cmdLines          = null;
    protected Command cmdSetCheck       = null;
    protected Command cmdRemoveAll          = null;
    protected Command cmdSetColor           = null;
    protected Command cmdSetFonts           = null;
    protected Command cmdClear              = null;

    private MenuItem cmdSingle;

    private MenuItem cmdMulti;

    protected static final int ROOT_ITEM_COUNT = 6;
    protected static final int CHILD_ITEM_COUNT = 4;
    /**
     * show a messagebox with the thrown exception
     * @param the text to be displayed in the messagebox
     */
    protected void showException(String msg)
    {
        MessageBox mb = new MessageBox(this.shell, SWT.ICON_INFORMATION | SWT.OK);
        mb.setText("Exception");
        mb.setMessage(msg);
        mb.open();
    }

    protected void init()
    {
        // customize the shell
        this.shell.setText("Tree Tests");
        // create and set a layout
        this.shell.setLayout(new GridLayout(2, false));
        this.createToolTip("");
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
            this.bigimg = new Image(this.display, this.getClass().getResourceAsStream("/res/small.png"));
        }
        catch (Exception ex)
        {
            // show a messagebox about the exception
            this.showException("Big image could not be loaded");
            this.bigimg = null;
        }
        // try to initialize the images
        try
        {
            this.transimg = new Image(this.display, this.getClass().getResourceAsStream("/res/alphatest_small.png"));
        }
        catch (Exception ex)
        {
            // show a messagebox about the exception
            this.showException("Small image could not be loaded");
            this.transimg = null;
        }

        // "singleTree" left softkey command
        this.cmdSingleTree = new Command(this.shell, Command.SELECT, 2);
        cmdSingleTree.setText("Single Tree");
        cmdSingleTree.addSelectionListener(this);
        // "multiTree" creation command
        this.cmdMultiTree = new Command(this.shell, Command.SELECT, 1);
        cmdMultiTree.setText("Multi Tree");
        cmdMultiTree.addSelectionListener(this);
        // Tree with hide creation command
        this.cmdHideTree = new Command(this.shell, Command.SELECT, 0);
        cmdHideTree.setText("Tree with Hide");
        cmdHideTree.addSelectionListener(this);
        // large "singleTree" left softkey command
        this.cmdLargeSingleTree = new Command(this.shell, Command.SELECT, 2);
        cmdLargeSingleTree.setText("Large Single Tree");
        cmdLargeSingleTree.addSelectionListener(this);
        // large "multiTree" left softkey command
        this.cmdLargeMultiTree = new Command(this.shell, Command.SELECT, 1);
        cmdLargeMultiTree.setText("Large Multi Tree");
        cmdLargeMultiTree.addSelectionListener(this);
        // large "singleTree" left softkey command
        this.cmdCheckSingleTree = new Command(this.shell, Command.SELECT, 2);
        cmdCheckSingleTree.setText("Check Single Tree");
        cmdCheckSingleTree.addSelectionListener(this);
        // large "multiTree" left softkey command
        this.cmdCheckMultiTree = new Command(this.shell, Command.SELECT, 1);
        cmdCheckMultiTree.setText("Check Multi Tree");
        cmdCheckMultiTree.addSelectionListener(this);
        // image "singleTree" left softkey command
        this.cmdImageSingleTree = new Command(this.shell, Command.SELECT, 2);
        cmdImageSingleTree.setText("Image Single Tree");
        cmdImageSingleTree.addSelectionListener(this);
        // image "multiTree" left softkey command
        this.cmdImageMultiTree = new Command(this.shell, Command.SELECT, 1);
        cmdImageMultiTree.setText("Image Multi Tree");
        cmdImageMultiTree.addSelectionListener(this);

        //  command
        this.cmdSetColor = new Command(this.shell, Command.SELECT, 0);
        cmdSetColor.setText("Change color");
        cmdSetColor.addSelectionListener(this);

        //  command
        this.cmdSetCheck = new Command(this.shell, Command.SELECT, 0);
        cmdSetCheck.setText("Change Check");
        cmdSetCheck.addSelectionListener(this);

        //  command
        this.cmdSetFonts = new Command(this.shell, Command.SELECT, 1);
        cmdSetFonts.setText("Change Fonts");
        cmdSetFonts.addSelectionListener(this);

        // Remove submenu
        Command cmdRemoveMenu = new Command(this.shell, Command.COMMANDGROUP, 4);
        cmdRemoveMenu.setText("Remove");

        this.cmdRemoveAll = new Command(cmdRemoveMenu, Command.GENERAL, 1);
        this.cmdRemoveAll.setText("Remove All");
        this.cmdRemoveAll.addSelectionListener(this);

        //  command
        this.cmdClear = new Command(this.shell, Command.SELECT, 1);
        cmdClear.setText("Clear Shell");
        cmdClear.addSelectionListener(this);


    }

    /**
     * Cleanup the screen.
     * Removes all widget objects which are currently presented
     * on the shell.
     */
    private void shellCleanup()
    {

        if (this.shell == null || this.shell.isDisposed()) return;

        Control[] controls = this.shell.getChildren();

        for (int i=0; i<controls.length; i++)
        {
            controls[i].dispose();
        }

        this.shell.update();
    }

    protected void populateTree(Tree tree,int root,int child,boolean image)
    {
        TreeItem myitem = null, myitem2 = null;
        int ROOT_ITEM_COUNT = root;
        int CHILD_ITEM_COUNT = child;
        for (int i=0; i<ROOT_ITEM_COUNT; i++)
        {
            TreeItem iItem = new TreeItem(tree, 0);
            iItem.setText("Item (0) -" + i);
            if (image)
                iItem.setImage(this.bigimg);
            if (i == 2)
                myitem = iItem;
            for (int j=0; j<CHILD_ITEM_COUNT; j++)
            {
                TreeItem jItem = new TreeItem(iItem, SWT.CHECK);
                jItem.setText("TreeTreeItem (1) - " + j);
                if (image)
                    jItem.setImage(this.smallimg);
                if (i == 2 && j == 2)
                    myitem2 = jItem;
                for (int k=0; k<4; k++)
                {
                    TreeItem kItem = new TreeItem(jItem, 0);
                    kItem.setText("I (2) - " + k);
                    if (image)
                        kItem.setImage(this.transimg);
                    for (int l=0; l<CHILD_ITEM_COUNT; l++)
                    {
                        TreeItem lItem = new TreeItem(kItem, 0);
                        lItem.setText("TreeItem (3) - " + l);
                    }
                }
            }
        }
        myitem.setExpanded(true);
        myitem2.setExpanded(true);
    }

    private void createTree(int style, int root, int child, boolean pack)
    {

        if (pack && style == SWT.MULTI)
        {
            tree = new Tree(this.shell,style|SWT.CHECK);
        }
        else
        {

            tree = new Tree(this.shell,style);
        }

        tree.setSize(new Point(this.shell.getClientArea().width, this.shell.getClientArea().height));

        this.populateTree(tree,root,child,false);

        tree.setLayoutData(new GridData(SWT.FILL, SWT.CENTER, true, false));
        tree.addSelectionListener(this);
        Menu mainmenu = new Menu(this.tree);
        this.cmdSingle = new MenuItem(mainmenu, SWT.PUSH);
        cmdSingle.setText("Single");
        cmdSingle.addSelectionListener(this);
        this.cmdMulti = new MenuItem(mainmenu, SWT.PUSH);
        cmdMulti.setText("Multi");
        cmdMulti.addSelectionListener(this);
        this.tree.setMenu(mainmenu);
        this.shell.layout();
        this.shell.update();
    }
    private void createImageTree(int style, int root, int child, boolean pack)
    {
        tree = new Tree(this.shell,style);
        tree.setSize(new Point(this.shell.getClientArea().width, this.shell.getClientArea().height));

        this.populateTree(tree,root,child,true);
        tree.setLayoutData(new GridData(SWT.FILL, SWT.CENTER, true, false));

        this.shell.layout();
        this.shell.update();
    }


    /**
     * selectionlistener implementation
     */
    public void widgetSelected(SelectionEvent event)
    {
        if (event.widget instanceof Command || event.widget instanceof MenuItem)
        {
            try
            {
                if (event.widget == this.cmdSingleTree|| event.widget == this.cmdSingle)
                {
                    createTree(SWT.SINGLE,4,3,true);
                }
                else if (event.widget == this.cmdMultiTree|| event.widget == this.cmdMulti)
                {
                    createTree(SWT.MULTI,4,3,true);
                }
                else if (event.widget == this.cmdLargeMultiTree)
                {
                    createTree(SWT.MULTI,8,6,true);
                }
                else if (event.widget == this.cmdLargeSingleTree)
                {
                    createTree(SWT.SINGLE,8,6,true);
                }
                else if (event.widget == this.cmdCheckMultiTree)
                {
                    createTree(SWT.MULTI,5,1,false);
                }
                else if (event.widget == this.cmdCheckSingleTree)
                {
                    createTree(SWT.SINGLE,5,6,false);
                }
                else if (event.widget == this.cmdImageSingleTree)
                {
                    createImageTree(SWT.SINGLE,5,3,false);
                }
                else if (event.widget == this.cmdImageMultiTree)
                {
                    createImageTree(SWT.MULTI,5,3,true);
                }
                else if (event.widget == this.cmdClear)
                {
                    this.shellCleanup();
                }
                else

                    if (event.widget == this.cmdRemoveAll)
                    {
                        tree.removeAll();
                    }
                    else

                        if (event.widget == this.cmdSetCheck)
                        {

                            for (int i = 0; i < tree.getItems().length; i++)
                            {
                                if (!tree.getItems()[i].getChecked())
                                    tree.getItems()[i].setChecked(true);
                                else
                                    tree.getItems()[i].setChecked(false);
                            }
                        }
                        else if (event.widget == this.cmdSetColor)
                        {

                            for (int i= 0; i < tree.getSelection().length; i++)
                            {
                                tree.getSelection()[i].setBackground(Display.getDefault().getSystemColor(SWT.COLOR_GREEN));
                                tree.getSelection()[i].setForeground(Display.getDefault().getSystemColor(SWT.COLOR_WHITE));
                            }

                        }
                        else if (event.widget == this.cmdSetFonts)
                        {

                            TreeItem trItem = tree.getSelection()[0];
                            FontDialog dlg = new FontDialog(this.shell);
                            Font font = new Font(this.display,dlg.open());

                            if (font != null)
                            {
                                trItem.setFont(font);

                                this.shell.layout();
                                this.shell.update();
                            }

                        }

            }
            catch (Exception ex)
            {
                // display the exception
                MessageBox msg = new MessageBox(this.shell, SWT.ICON_INFORMATION | SWT.OK);
                msg.setMessage(ex.getMessage());
                msg.open();
                return;
            }
        }
        else if (event.widget instanceof Tree)
        {
            updateToolTip("Sel");
        }
        super.widgetSelected(event);

    }
    /* (non-Javadoc)
     * @see org.eclipse.swt.events.SelectionListener#widgetDefaultSelected(org.eclipse.swt.events.SelectionEvent)
     */
    public void widgetDefaultSelected(SelectionEvent event)
    {

        updateToolTip("Def");

    }
    /**
     *
     */
    private void createToolTip(String msg)
    {

        // create a modeless shell with border
        this.tooltip = new Shell(this.shell, SWT.MODELESS | SWT.NO_FOCUS | SWT.BORDER);
        this.tooltip.setLayout(new RowLayout(SWT.HORIZONTAL));

        // create a label to be used as a tooltip text
        Label label = new Label(this.tooltip, SWT.NONE);
        label.setText(msg);

        // set the location so the tooltip will be shown in
        // middle of the screen
        this.tooltip.setLocation(5, 5);

        // finilize
        this.tooltip.pack();
        this.tooltip.layout();
        this.tooltip.setVisible(false);
    }
    /**
     *
     */
    private void updateToolTip(String msg)
    {

        Control[] controls = this.tooltip.getChildren();
        for (int i=0; i<controls.length; i++)
        {
            controls[i].dispose();
        }
        // create a label to be used as a tooltip text
        Label label = new Label(this.tooltip, SWT.NONE);
        label.setText(msg);

        // finilize
        this.tooltip.pack();
        this.tooltip.layout();
        this.tooltip.setVisible(true);
    }


}

