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
package com.nokia.test.java.eswt.listview;

import java.util.Vector;

import org.eclipse.ercp.swt.mobile.Command;
import org.eclipse.ercp.swt.mobile.ConstrainedText;
import org.eclipse.ercp.swt.mobile.MobileShell;
import org.eclipse.swt.SWT;
import org.eclipse.swt.events.SelectionEvent;
import org.eclipse.swt.graphics.Image;
import org.eclipse.swt.graphics.Point;
import org.eclipse.swt.layout.RowData;
import org.eclipse.swt.layout.RowLayout;
import org.eclipse.swt.layout.FillLayout;
import org.eclipse.swt.widgets.Control;
import org.eclipse.swt.widgets.Display;
import org.eclipse.swt.widgets.Label;
import org.eclipse.swt.widgets.MessageBox;
import org.eclipse.swt.widgets.Shell;
import org.eclipse.ercp.swt.mobile.ListView;


import com.nokia.test.java.eswt.midlet.TestBaseMIDlet;

public class ListViewTestMIDlet_02 extends TestBaseMIDlet
{

    /** list instance */
    private ListView listView = null;

    protected MobileShell iMobileShell;

    private Command CMDMULTI = null;
    private Command CMDSINGLE = null;
    private Command CMDSINGLE_IMAGE = null;
    private Command CMDMULTI_IMAGE = null;

    private Command CMDENABLETEXT = null;
    private Command CMDDISABLETEXT = null;

    private Command CMDREMOVE = null;
    private Command CMDREMOVEALL = null;
    private Command CMDSELECT = null;
    private Command CMDSELECTALL = null;
    private Command CMDDESELECT = null;
    private Command CMDDESELECTALL = null;
    private Command CMDADD = null;
    private Command CMDADDIMAGE = null;
    private Command CMDADDTRANSIMAGE = null;

    private Command CMDSET_IMAGE = null;
    private Command CMDSET_TRANSIMAGE = null;
    private Command CMDSET_EMPTY = null;
    private Command CMDSET_LONG = null;
    private Command CMDSET_SPECIAL = null;

    private Command CMDSETSELECT = null;
    private Command CMDSETSELECTALL = null;
    private Command CMDSETSELECTINDECS = null;
    private Command CMDSETSELECTSTRING = null;

    private Command CMDDENSITY_LOW = null;
    private Command CMDDENSITY_MEDIUM = null;
    private Command CMDDENSITY_HIGH = null;

    private Command iExitCommand = null;
    private Command iSwitchCommand = null;

    /** custom image for the Listview */
    private Image smallImage = null;
    /** custom large image for the Listview */
    private Image largeImage = null;
    /** custom transparent image for the Listview */
    private Image transImage = null;
    /** custom image for the Listview */
    private Image iImage = null;

    private Shell tooltip = null;
    private final int NORMAL_ITEM_COUNT = 100;
    private final int LARGE_ITEM_COUNT = 1000;
    protected final String IMAGE1 = "/sphere.jpg";
    protected final String IMAGE2 = "/phone2.jpg";
    protected final int NUMBER_OF_SAMPLES = 1;
    protected final String TESTTEXT = "1234567890"; // Note: used also for NUMERIC ConstrainedText test
    protected final int TESTITEMCOUNT = 10;
    protected final Point TESTIMAGESIZE = new Point(30, 40);
    private Vector images = new Vector();
    private int samples = 0;
    /**
     * initialization
     */
    protected void init()
    {

        // customize the shell
        this.shell.setText("ListView 2");
        this.shell.setLayout(new FillLayout());

        this.iMobileShell = new MobileShell(this.display);
        this.iMobileShell.setText("MobileShell");
        this.iExitCommand = new Command(iMobileShell, Command.EXIT, 0);
        this.iExitCommand.setText("Back");
        this.iExitCommand.addSelectionListener(this);

        iMobileShell.open();
        iMobileShell.setVisible(false);
        // try to initialize the image
        try
        {
            this.smallImage = new Image(this.display, this.getClass().getResourceAsStream("/small.png"));
        }
        catch (Exception ex)
        {
            // show a messagebox about the exception
            MessageBox mb = new MessageBox(this.shell, SWT.ICON_INFORMATION | SWT.OK);
            mb.setText("Exception");
            mb.setMessage("Icon could not be loaded");
            mb.open();
            this.smallImage = null;
        }

        try
        {
            this.largeImage = new Image(this.display, this.getClass().getResourceAsStream("/medium.jpg"));
        }
        catch (Exception ex)
        {
            // show a messagebox about the exception
            MessageBox mb = new MessageBox(this.shell, SWT.ICON_INFORMATION | SWT.OK);
            mb.setText("Exception");
            mb.setMessage("Icon could not be loaded");
            mb.open();
            this.largeImage = null;
        }
        try
        {
            this.transImage = new Image(this.display, this.getClass().getResourceAsStream("/alphatest_small.png"));
        }
        catch (Exception ex)
        {
            // show a messagebox about the exception
            MessageBox mb = new MessageBox(this.shell, SWT.ICON_INFORMATION | SWT.OK);
            mb.setText("Exception");
            mb.setMessage("Icon could not be loaded");
            mb.open();
            this.transImage = null;
        }
        // try to initialize the image
        try
        {
            this.iImage = new Image(this.display, this.getClass().getResourceAsStream("/sphere.jpg"));
        }
        catch (Exception ex)
        {
            // show a messagebox about the exception
            MessageBox mb = new MessageBox(this.shell, SWT.ICON_INFORMATION | SWT.OK);
            mb.setText("Exception");
            mb.setMessage("Icon could not be loaded");
            mb.open();
            this.smallImage = null;
        }
        // "single listview" left softkey command
        this.CMDSINGLE = new Command(this.shell, Command.SELECT, 1);
        this.CMDSINGLE.setText("Single ListView");
        this.CMDSINGLE.addSelectionListener(this);
        // "multi listview" left softkey command
        this.CMDMULTI = new Command(this.shell, Command.OK, 0);
        this.CMDMULTI.setText("Multi ListView");
        this.CMDMULTI.addSelectionListener(this);
        // "single listview" left softkey command
        this.CMDSINGLE_IMAGE = new Command(this.shell, Command.OK, 0);
        this.CMDSINGLE_IMAGE.setText("Single ListView with image");
        this.CMDSINGLE_IMAGE.addSelectionListener(this);
        // "multi listview" left softkey command
        this.CMDMULTI_IMAGE = new Command(this.shell, Command.SELECT, 0);
        this.CMDMULTI_IMAGE.setText("Multi ListView with image");
        this.CMDMULTI_IMAGE.addSelectionListener(this);

        this.CMDENABLETEXT = new Command(this.shell, Command.GENERAL, 0);
        this.CMDENABLETEXT.setText("Enable Text");
        this.CMDENABLETEXT.addSelectionListener(this);

        this.CMDDISABLETEXT = new Command(this.shell, Command.GENERAL, 0);
        this.CMDDISABLETEXT.setText("Disable Text");
        this.CMDDISABLETEXT.addSelectionListener(this);

        Command density = new Command(this.shell, Command.COMMANDGROUP,0);
        density.setText("Density");

        this.CMDDENSITY_LOW = new Command(density, Command.GENERAL, 0);
        this.CMDDENSITY_LOW.setText("Low");
        this.CMDDENSITY_LOW.addSelectionListener(this);

        this.CMDDENSITY_MEDIUM = new Command(density, Command.GENERAL, 0);
        this.CMDDENSITY_MEDIUM.setText("Medium");
        this.CMDDENSITY_MEDIUM.addSelectionListener(this);

        this.CMDDENSITY_HIGH = new Command(density, Command.GENERAL, 0);
        this.CMDDENSITY_HIGH.setText("High");
        this.CMDDENSITY_HIGH.addSelectionListener(this);

        Command remove = new Command(this.shell, Command.COMMANDGROUP,0);
        remove.setText("Remove");

        this.CMDREMOVE = new Command(remove, Command.GENERAL, 0);
        this.CMDREMOVE.setText("Selected");
        this.CMDREMOVE.addSelectionListener(this);

        this.CMDREMOVEALL = new Command(remove, Command.GENERAL, 0);
        this.CMDREMOVEALL.setText("All");
        this.CMDREMOVEALL.addSelectionListener(this);

        Command select = new Command(this.shell, Command.COMMANDGROUP,0);
        select.setText("Select");

        this.CMDSELECT = new Command(select, Command.GENERAL, 0);
        this.CMDSELECT.setText("Focused");
        this.CMDSELECT.addSelectionListener(this);

        this.CMDSELECTALL = new Command(select, Command.GENERAL, 0);
        this.CMDSELECTALL.setText("All");
        this.CMDSELECTALL.addSelectionListener(this);

        this.CMDDESELECT = new Command(select, Command.GENERAL, 0);
        this.CMDDESELECT.setText("Deselect");
        this.CMDDESELECT.addSelectionListener(this);

        this.CMDDESELECTALL = new Command(select, Command.GENERAL, 0);
        this.CMDDESELECTALL.setText("Deselect all");
        this.CMDDESELECTALL.addSelectionListener(this);

        this.CMDADD = new Command(this.shell, Command.GENERAL, 0);
        this.CMDADD.setText("Add");
        this.CMDADD.addSelectionListener(this);

        this.CMDADDIMAGE = new Command(this.shell, Command.GENERAL, 0);
        this.CMDADDIMAGE.setText("Add with image");
        this.CMDADDIMAGE.addSelectionListener(this);

        this.CMDADDTRANSIMAGE = new Command(this.shell, Command.GENERAL, 0);
        this.CMDADDTRANSIMAGE.setText("Add with trans image");
        this.CMDADDTRANSIMAGE.addSelectionListener(this);

        Command set = new Command(this.shell, Command.COMMANDGROUP,0);
        set.setText("Set");

        this.CMDSET_IMAGE = new Command(set, Command.GENERAL,0);
        this.CMDSET_IMAGE.setText("Image");
        this.CMDSET_IMAGE.addSelectionListener(this);

        this.CMDSET_TRANSIMAGE = new Command(set, Command.GENERAL,0);
        this.CMDSET_TRANSIMAGE.setText("Transparent Image");
        this.CMDSET_TRANSIMAGE.addSelectionListener(this);

        this.CMDSET_EMPTY = new Command(set, Command.GENERAL,0);
        this.CMDSET_EMPTY.setText("Empty");
        this.CMDSET_EMPTY.addSelectionListener(this);

        this.CMDSET_LONG = new Command(set, Command.GENERAL,0);
        this.CMDSET_LONG.setText("Long text");
        this.CMDSET_LONG.addSelectionListener(this);

        this.CMDSET_SPECIAL = new Command(set, Command.GENERAL,0);
        this.CMDSET_SPECIAL.setText("Special chars");
        this.CMDSET_SPECIAL.addSelectionListener(this);

        Command setSelection = new Command(this.shell, Command.COMMANDGROUP,0);
        setSelection.setText("Set Selection");
        this.CMDSETSELECT = new Command(setSelection, Command.GENERAL, 0);
        this.CMDSETSELECT.setText("Focused");
        this.CMDSETSELECT.addSelectionListener(this);

        this.CMDSETSELECTALL = new Command(setSelection, Command.GENERAL, 0);
        this.CMDSETSELECTALL.setText("All");
        this.CMDSETSELECTALL.addSelectionListener(this);

        this.CMDSETSELECTINDECS = new Command(setSelection, Command.GENERAL, 0);
        this.CMDSETSELECTINDECS.setText("Deselect");
        this.CMDSETSELECTINDECS.addSelectionListener(this);

        this.CMDSETSELECTSTRING = new Command(setSelection, Command.GENERAL, 0);
        this.CMDSETSELECTSTRING.setText("Deselect all");
        this.CMDSETSELECTSTRING.addSelectionListener(this);

        this.iSwitchCommand = new Command(this.shell, Command.GENERAL, 0);
        this.iSwitchCommand.setText("MobileShell");
        this.iSwitchCommand.addSelectionListener(this);
    }

    /**
     * create a listview on the shell
     * @param type the typ of the listbox
     */
    private void createListView(int type, int layoutstyle, boolean image)
    {
        // dispose previous listview
        if (this.listView != null && !this.listView.isDisposed())
            this.listView.dispose();


        if (layoutstyle != -1)
        {
            // create the listview
            this.listView = new ListView(this.shell, type | SWT.BORDER | SWT.V_SCROLL,layoutstyle);
        }
        else
        {
            this.listView = new ListView(this.shell, type);
        }
        try
        {
            if (image)
            {
                for (int i = 0; i < 10; i++)
                    this.listView.add("Item "+(i+1), this.smallImage);
            }
            else
            {
                for (int i = 0; i < 8; i++)
                    this.listView.add("Item "+(i+1), null);
            }
        }
        catch (Exception ex)
        {
            ex.printStackTrace();
        }

        try
        {
            this.listView.addSelectionListener(this);
            this.listView.pack();

            this.shell.layout();
        }
        catch (Exception ex)
        {
            ex.printStackTrace();
        }
    }

    /** * This must NOT printed in actual test run */
    public void trace(String aMessage)
    {
        System.out.println(aMessage);
    }
    protected Image getImage()
    {
        return iImage;
    }
    // Generic interface for item addition testing
    private abstract class AddlItemInterface
    {
        // This is called to add an item to whatever
        public abstract void add();
        // This is called between the samples (not timed) to clean up
        public abstract void cleanUp();
    }

    // Pass in the interface and how many items to create in each sample
    private void testAddingItems(AddlItemInterface control, int itemCount)
    {
        this.iMobileShell.layout();

        // Event flush before starting the test
        while (Display.getCurrent().readAndDispatch()) {}

        for (int i = 0; i < 1; ++i)
        {
            for (int item = 0; item < itemCount; ++item)
            {
                control.add();
            }

            // Event flush, dispatching events caused by each sample is included in the test time
            while (Display.getCurrent().readAndDispatch()) {}

            control.cleanUp();

            // Event flush, events possibly caused by cleanUp
            while (Display.getCurrent().readAndDispatch()) {}
        }
    }
    // Helper, tests appending item to the end of a list
    String stringItems[] = null;
    Image[] listViewImages;
    Image listViewImage;
    public void testAppendingListViewItemAtIndex(int index, int style)
    {
        listViewImage = getImage();
        listView = new ListView(this.iMobileShell, style);
        stringItems = new String[index];
        listViewImages = new Image[index];
        for (int i = 0; i < index; ++i)
        {
            stringItems[i] = new String(TESTTEXT);
            listViewImages[i] = listViewImage;
        }
        listView.setItems(stringItems, listViewImages);
        testAddingItems(new AddlItemInterface()
        {
            public void add()
            {
                listView.add(TESTTEXT, listViewImage);
            }
            public void cleanUp()
            {
                // The whole list is disposed so that item memory is not preallocated from the last sample
                int style = listView.getStyle();
                listView.dispose();
                listView = new ListView(iMobileShell, style);
                listView.setItems(stringItems, listViewImages);
                iMobileShell.layout();
            }
        }, 1);
    }

    public boolean append1stItemToSingleListView()
    {
        trace("\n [ListPopulatingTest]append1stItemToSingleListView");
        try
        {
            testAppendingListViewItemAtIndex(0, SWT.SINGLE);
        }
        catch (Exception ex)
        {
            ex.printStackTrace();
        }
        return true;
    }

    public boolean append100thItemToSingleListView()
    {
        trace("\n [ListPopulatingTest]append100thItemToSingleListView");
        try
        {
            testAppendingListViewItemAtIndex(99, SWT.SINGLE);
        }
        catch (Exception ex)
        {
            ex.printStackTrace();
        }
        return true;
    }

    public boolean append1000thItemToSingleListView()
    {
        trace("\n [ListPopulatingTest]append1000thItemToSingleListView");
        try
        {
            testAppendingListViewItemAtIndex(999, SWT.SINGLE);
        }
        catch (Exception ex)
        {
            ex.printStackTrace();
        }
        return true;
    }

    public boolean append10000thItemToSingleListView()
    {
        trace("\n [ListPopulatingTest]append10000thItemToSingleListView");
        try
        {
            testAppendingListViewItemAtIndex(9999, SWT.SINGLE);
        }
        catch (Exception ex)
        {
            ex.printStackTrace();
        }
        return true;
    }

    public boolean append1stItemToMultiListView()
    {
        trace("\n [ListPopulatingTest]append1stItemToMultiListView");
        try
        {
            testAppendingListViewItemAtIndex(0, SWT.MULTI);
        }
        catch (Exception ex)
        {
            ex.printStackTrace();
        }
        return true;
    }

    public boolean append100thItemToMultiListView()
    {
        trace("\n [ListPopulatingTest]append100thItemToMultiListView");
        try
        {
            testAppendingListViewItemAtIndex(99, SWT.MULTI);
        }
        catch (Exception ex)
        {
            ex.printStackTrace();
        }
        return true;
    }

    public boolean append1000thItemToMultiListView()
    {
        trace("\n [ListPopulatingTest]append1000thItemToMultiListView");
        try
        {
            testAppendingListViewItemAtIndex(999, SWT.MULTI);
        }
        catch (Exception ex)
        {
            ex.printStackTrace();
        }
        return true;
    }

    public boolean append10000thItemToMultiListView()
    {
        trace("\n [ListPopulatingTest]append10000thItemToMultiListView");
        try
        {
            testAppendingListViewItemAtIndex(9999, SWT.MULTI);
        }
        catch (Exception ex)
        {
            ex.printStackTrace();
        }
        return true;
    }

    // Helper, tests populating the list by adding in a loop
    private void testAddingItemsToListViewInALoop(int nItems, int style)
    {
        listViewImage = getImage();
        listView = new ListView(this.iMobileShell, style);
        testAddingItems(new AddlItemInterface()
        {
            public void add()
            {
                listView.add(TESTTEXT, listViewImage);
            }
            public void cleanUp()
            {
                // The whole list is disposed so that item memory is not preallocated from the last sample
                int style = listView.getStyle();
                listView.dispose();
                listView = new ListView(iMobileShell, style);
                iMobileShell.layout();
            }
        }, nItems);
    }

    public boolean appendNormalNumberOfItemsToSingleViewListInALoop()
    {
        trace("\n [ListPopulatingTest]appendNormalNumberOfItemsToSingleViewListInALoop");
        try
        {
            testAddingItemsToListViewInALoop(NORMAL_ITEM_COUNT, SWT.SINGLE);
        }
        catch (Exception ex)
        {
            ex.printStackTrace();
        }
        return true;
    }

    public boolean appendLargeNumberOfItemsToSingleListViewInALoop()
    {
        trace("\n [ListPopulatingTest]appendLargeNumberOfItemsToSingleListViewInALoop");
        try
        {
            testAddingItemsToListViewInALoop(LARGE_ITEM_COUNT, SWT.SINGLE);
        }
        catch (Exception ex)
        {
            ex.printStackTrace();
        }
        return true;
    }

    public boolean appendNormalNumberOfItemsToMultiListViewInALoop()
    {
        trace("\n [ListPopulatingTest]appendNormalNumberOfItemsToMultiListViewInALoop");
        try
        {
            testAddingItemsToListViewInALoop(NORMAL_ITEM_COUNT, SWT.MULTI);
        }
        catch (Exception ex)
        {
            ex.printStackTrace();
        }
        return true;
    }

    public boolean appendLargeNumberOfItemsToMultiListViewInALoop()
    {
        trace("\n [ListPopulatingTest]appendLargeNumberOfItemsToMultiListViewInALoop");
        try
        {
            testAddingItemsToListViewInALoop(LARGE_ITEM_COUNT, SWT.MULTI);
        }
        catch (Exception ex)
        {
            ex.printStackTrace();
        }
        return true;
    }
    private int indexToTest = 0;
    // Helper, tests replacing an item in the list, the most efficient way
    private void testReplacingListViewItemAtIndex(int index, int style)
    {
        indexToTest = index;
        listView = new ListView(this.iMobileShell, style);
        stringItems = new String[index + 1];
        listViewImages = new Image[index + 1];
        for (int i = 0; i < index + 1; ++i)
        {
            stringItems[i] = new String(TESTTEXT);
            listViewImages[i] = listViewImage;
        }
        listView.setItems(stringItems, listViewImages);
        testAddingItems(new AddlItemInterface()
        {
            public void add()
            {
                listView.setItem(indexToTest, TESTTEXT, listViewImage);
            }
            public void cleanUp()
            {
            }
        }, 1);
    }

    public boolean replaceSingleStyleListViewItemAtLastIndex0()
    {
        trace("\n [ListPopulatingTest]replaceSingleStyleListViewItemAtLastIndex0");
        try
        {
            testReplacingListViewItemAtIndex(0, SWT.SINGLE);
        }
        catch (Exception ex)
        {
            ex.printStackTrace();
        }
        return true;
    }

    public boolean replaceSingleStyleListViewItemAtLastIndex99()
    {
        trace("\n [ListPopulatingTest]replaceSingleStyleListViewItemAtLastIndex99");
        try
        {
            testReplacingListViewItemAtIndex(99, SWT.SINGLE);
        }
        catch (Exception ex)
        {
            ex.printStackTrace();
        }
        return true;
    }

    public boolean replaceSingleStyleListViewItemAtLastIndex999()
    {
        trace("\n [ListPopulatingTest]replaceSingleStyleListViewItemAtLastIndex999");
        try
        {
            testReplacingListViewItemAtIndex(999, SWT.SINGLE);
        }
        catch (Exception ex)
        {
            ex.printStackTrace();
        }
        return true;
    }

    /**
     * selectionlistener implementation
     */
    public void widgetSelected(SelectionEvent event)
    {
        if (event.widget instanceof Command)
        {
            try
            {
                if (event.widget == this.CMDSINGLE)
                {
                    // create a single list
                    this.createListView(SWT.SINGLE, -1,false);
                    return;
                }
                if (event.widget == this.CMDMULTI)
                {
                    // create a multi list
                    this.createListView(SWT.MULTI, ListView.HIGH,false);
                    return;
                }
                if (event.widget == this.CMDSINGLE_IMAGE)
                {
                    // create a single list
                    this.createListView(SWT.SINGLE, ListView.MEDIUM,true);
                    return;
                }
                if (event.widget == this.CMDMULTI_IMAGE)
                {
                    // create a multi list
                    this.createListView(SWT.MULTI, ListView.LOW,true);
                    return;
                }
                if (listView != null)
                {
                    int index = this.listView.getFocusIndex();
                    int indeces[] = this.listView.getSelectionIndices();

                    if (event.widget == this.CMDDENSITY_LOW)
                    {
                        // enable text control
                        this.listView.setLayoutDensity(ListView.LOW);
                        return;
                    }
                    if (event.widget == this.CMDDENSITY_MEDIUM)
                    {
                        // enable text control
                        this.listView.setLayoutDensity(ListView.MEDIUM);
                        return;
                    }
                    if (event.widget == this.CMDDENSITY_HIGH)
                    {
                        // enable text control
                        this.listView.setLayoutDensity(ListView.HIGH);
                        return;
                    }
                    if (event.widget == this.CMDREMOVE)
                    {
                        // remove items
                        if (this.listView.getStyle() == SWT.SINGLE)
                            this.listView.remove(index);
                        else
                            this.listView.remove(indeces);

                        return;
                    }
                    if (event.widget == this.CMDREMOVEALL)
                    {
                        // remove all items
                        this.listView.removeAll();
                        return;
                    }
                    if (event.widget == this.CMDSELECT)
                    {
                        // select items
                        this.listView.select(index);
                        return;
                    }
                    if (event.widget == this.CMDSELECTALL)
                    {
                        // select all items
                        this.listView.selectAll();
                        return;
                    }
                    if (event.widget == this.CMDDESELECT)
                    {
                        // deselect items
                        this.listView.deselect(index);
                        return;
                    }
                    if (event.widget == this.CMDDESELECTALL)
                    {
                        // deselect all items
                        this.listView.deselectAll();
                        return;
                    }
                    if (event.widget == this.CMDADD)
                    {
                        // add items
                        this.listView.add("Added",null,index);

                        return;
                    }
                    if (event.widget == this.CMDADDIMAGE)
                    {
                        // add items
                        this.listView.add("Added",this.largeImage);

                        return;
                    }
                    if (event.widget == this.CMDADDTRANSIMAGE)
                    {
                        // add items
                        this.listView.add("Added",this.transImage);

                        return;
                    }
                    if (event.widget == this.CMDSET_LONG)
                    {
                        // set long text items
                        if (this.listView.getStyle() == SWT.SINGLE)
                            this.listView.setItem(index,"Item's text is changed to long so wrapping can be tested",listView.getIcon(index));
                        else
                        {
                            for (int i = 0; i < indeces.length; i++)
                                this.listView.setItem(indeces[i], "Item's text is changed to long so wrapping can be tested", listView.getIcon(indeces[i]));
                        }
                        return;
                    }
                    if (event.widget == this.CMDSET_EMPTY)
                    {
                        // set empty text items
                        if (this.listView.getStyle() == SWT.SINGLE)
                            this.listView.setItem(index,"",listView.getIcon(index));
                        else
                        {
                            for (int i = 0; i < indeces.length; i++)
                                this.listView.setItem(indeces[i], "", listView.getIcon(indeces[i]));
                        }
                        return;
                    }
                    if (event.widget == this.CMDSET_SPECIAL)
                    {
                        // set special text items
                        if (this.listView.getStyle() == SWT.SINGLE)
                            this.listView.setItem(index,"? \n \t %&@#£$ !!!",listView.getIcon(index));
                        else
                        {
                            for (int i = 0; i < indeces.length; i++)
                                this.listView.setItem(indeces[i], "?%&@#£$ \n \t !!!", listView.getIcon(indeces[i]));
                        }
                        return;
                    }

                    if (event.widget == this.CMDSET_IMAGE)
                    {
                        // set images items
                        if (this.listView.getStyle() == SWT.SINGLE)
                            this.listView.setItem(index,listView.getItem(index),this.largeImage);
                        else
                        {
                            for (int i = 0; i < indeces.length; i++)
                                this.listView.setItem(indeces[i], listView.getItem(indeces[i]), this.largeImage);
                        }
                        return;
                    }
                    if (event.widget == this.CMDSET_TRANSIMAGE)
                    {
                        // set images items
                        if (this.listView.getStyle() == SWT.SINGLE)
                            this.listView.setItem(index,listView.getItem(index),this.transImage);
                        else
                        {
                            for (int i = 0; i < indeces.length; i++)
                                this.listView.setItem(indeces[i], listView.getItem(indeces[i]), this.transImage);
                        }
                        return;
                    }
                    if (event.widget == this.iExitCommand)
                    {
                        // set images items
                        this.iMobileShell.setVisible(false);
                        this.shell.setVisible(true);
                        return;
                    }
                }
            }
            catch (Exception ex)
            {
                ex.printStackTrace();
            }
            try
            {
                if (event.widget == this.iSwitchCommand)
                {
                    // set images items
                    this.shell.setVisible(false);
                    this.iMobileShell.setVisible(true);
                    this.append1stItemToSingleListView();
                    this.append100thItemToSingleListView();
                    this.append1000thItemToSingleListView();
                    return;
                }
            }
            catch (Exception ex)
            {
                ex.printStackTrace();
            }
        }
        else if (event.widget instanceof ListView)
        {
        }
        super.widgetSelected(event);

    }

    /* (non-Javadoc)
     * @see org.eclipse.swt.events.SelectionListener#widgetDefaultSelected(org.eclipse.swt.events.SelectionEvent)
     */
    public void widgetDefaultSelected(SelectionEvent event)
    {
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
        this.tooltip.setLocation(Display.getCurrent().getBounds().width/2, Display.getCurrent().getBounds().height/2);

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

