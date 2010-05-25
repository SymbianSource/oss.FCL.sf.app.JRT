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

package com.nokia.test.java.eswt.timedmessagebox;

import org.eclipse.ercp.swt.mobile.TimedMessageBox;
import org.eclipse.swt.SWT;
import org.eclipse.swt.events.SelectionEvent;
import org.eclipse.swt.graphics.Image;
import org.eclipse.swt.widgets.Menu;
import org.eclipse.swt.widgets.MenuItem;
import org.eclipse.swt.widgets.MessageBox;

import com.nokia.test.java.eswt.midlet.TestBaseMIDlet;

/**
 * this class is intended to test the timedmessagebox component
 * - icons and images in timedmessagebox
 * - operating timedmessagebox
 * - setting title and message
 */
public class TimedMessageBoxTestMIDlet_01 extends TestBaseMIDlet
{
    /** working timed messagebox */
    private TimedMessageBox workbox = null;
    /** information timed messagebox */
    private TimedMessageBox infobox = null;
    /** warning timed messagebox */
    private TimedMessageBox warnbox = null;
    /** error timed messagebox */
    private TimedMessageBox errbox = null;
    /** normal image */
    private Image normimg = null;
    /** big image */
    private Image bigimg = null;
    /** transparent image */
    private Image transimg = null;
    /** gif image */
    private Image gifimg = null;

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

    /**
     * initialization
     */
    protected void init()
    {
        // customize the shell
        this.shell.setText("TimedMessageBox");

        // try to initialize the images
        try
        {
            this.normimg = new Image(this.display, this.getClass().getResourceAsStream("/res/icon.png"));
        }
        catch (Exception ex)
        {
            // show a messagebox about the exception
            this.showException("Small image could not be loaded");
            this.normimg = null;
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
        try
        {
            this.gifimg = new Image(this.display, this.getClass().getResourceAsStream("/res/BigLabel.gif"));
        }
        catch (Exception ex)
        {
            // show a messagebox about the exception
            this.showException("Gif image could not be loaded");
            this.gifimg = null;
        }

        // setup the timed messageboxes
        this.workbox = new TimedMessageBox(this.shell, SWT.ICON_WORKING);
        this.workbox.setText("Working...");
        this.workbox.setMessage("This is a \"working\" timed messagebox.");
        this.infobox = new TimedMessageBox(this.shell, SWT.ICON_INFORMATION);
        this.infobox.setText("Information...");
        this.infobox.setMessage("This is an \"information\" timed messagebox.");
        this.warnbox = new TimedMessageBox(this.shell, SWT.ICON_WARNING);
        this.warnbox.setText("Warning...");
        this.warnbox.setMessage("This is a \"warning\" timed messagebox.");
        this.errbox = new TimedMessageBox(this.shell, SWT.ICON_ERROR);
        this.errbox.setText("Error...");
        this.errbox.setMessage("This is an \"error\" timed messagebox.");

        // setup the menu
        Menu mainmenu = new Menu(this.shell, SWT.BAR);
        this.shell.setMenuBar(mainmenu);
        // "show" submenu
        Menu menu = new Menu(mainmenu);
        MenuItem item = new MenuItem(mainmenu, SWT.CASCADE);
        item.setText("Show");
        item.setMenu(menu);
        // "working" menuitem for "show" submenu
        item = new MenuItem(menu, SWT.PUSH);
        item.setText("Working");
        item.addSelectionListener(this);
        // "information" menuitem for "show" submenu
        item = new MenuItem(menu, SWT.PUSH);
        item.setText("Information");
        item.addSelectionListener(this);
        // "warning" menuitem for "show" submenu
        item = new MenuItem(menu, SWT.PUSH);
        item.setText("Warning");
        item.addSelectionListener(this);
        // "error" menuitem for "show" submenu
        item = new MenuItem(menu, SWT.PUSH);
        item.setText("Error");
        item.addSelectionListener(this);
        // "image" submenu
        menu = new Menu(mainmenu);
        item = new MenuItem(mainmenu, SWT.CASCADE);
        item.setText("Image");
        item.setMenu(menu);
        // "null" menuitem for "image" submenu
        item = new MenuItem(menu, SWT.PUSH);
        item.setText("Null");
        item.addSelectionListener(this);
        // "normal" menuitem for "image" submenu
        item = new MenuItem(menu, SWT.PUSH);
        item.setText("Normal");
        item.addSelectionListener(this);
        // "big" menuitem for "image" submenu
        item = new MenuItem(menu, SWT.PUSH);
        item.setText("Big");
        item.addSelectionListener(this);
        // "transparent" menuitem for "image" submenu
        item = new MenuItem(menu, SWT.PUSH);
        item.setText("Transparent");
        item.addSelectionListener(this);
        // "Gif" menuitem for "image" submenu
        item = new MenuItem(menu, SWT.PUSH);
        item.setText("Gif");
        item.addSelectionListener(this);
        // "invalid" menuitem for "image" submenu
        item = new MenuItem(menu, SWT.PUSH);
        item.setText("Invalid");
        item.addSelectionListener(this);
        // "change title" submenu
        menu = new Menu(mainmenu);
        item = new MenuItem(mainmenu, SWT.CASCADE);
        item.setText("Change Title");
        item.setMenu(menu);
        // "null" menuitem for "change title" submenu
        item = new MenuItem(menu, SWT.PUSH);
        item.setText("Null");
        item.addSelectionListener(this);
        // "empty" menuitem for "change title" submenu
        item = new MenuItem(menu, SWT.PUSH);
        item.setText("Empty");
        item.addSelectionListener(this);
        // "short" menuitem for "change title" submenu
        item = new MenuItem(menu, SWT.PUSH);
        item.setText("Short");
        item.addSelectionListener(this);
        // "long" menuitem for "change title" submenu
        item = new MenuItem(menu, SWT.PUSH);
        item.setText("Long");
        item.addSelectionListener(this);
        // "change text" submenu
        menu = new Menu(mainmenu);
        item = new MenuItem(mainmenu, SWT.CASCADE);
        item.setText("Change Text");
        item.setMenu(menu);
        // "null" menuitem for "change text" submenu
        item = new MenuItem(menu, SWT.PUSH);
        item.setText("Null");
        item.addSelectionListener(this);
        // "empty" menuitem for "change text" submenu
        item = new MenuItem(menu, SWT.PUSH);
        item.setText("Empty");
        item.addSelectionListener(this);
        // "short" menuitem for "change text" submenu
        item = new MenuItem(menu, SWT.PUSH);
        item.setText("Short");
        item.addSelectionListener(this);
        // "long" menuitem for "change text" submenu
        item = new MenuItem(menu, SWT.PUSH);
        item.setText("Long");
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
            if ("Working".equals(scmd))
            {
                // display the working timed messagebox
                this.workbox.open();
                return;
            }
            if ("Information".equals(scmd))
            {
                // display the information timed messagebox
                this.infobox.open();
                return;
            }
            if ("Warning".equals(scmd))
            {
                // display the warning timed messagebox
                this.warnbox.open();
                return;
            }
            if ("Error".equals(scmd))
            {
                // display the error timed messagebox
                this.errbox.open();
                return;
            }
            if ("Null".equals(scmd))
            {
                scmd = ((MenuItem) event.widget).getParent().getParentItem().getText();
                try
                {
                    if ("Image".equals(scmd))
                        // try to set a null image to a timed messagebox
                        this.workbox.setImage(null);
                    else if ("Change Title".equals(scmd))
                        // try to set the title text to null
                        this.workbox.setText(null);
                    else if ("Change Text".equals(scmd))
                        // try to set the message text to null
                        this.workbox.setMessage(null);
                }
                catch (Exception ex)
                {
                    // display the exception
                    this.showException(ex.getMessage());
                }
                return;
            }
            if ("Normal".equals(scmd))
            {
                // set an image with normal size to the timed messageboxes
                if (this.normimg != null && !this.normimg.isDisposed())
                {
                    this.workbox.setImage(this.normimg);
                    this.infobox.setImage(this.normimg);
                    this.warnbox.setImage(this.normimg);
                    this.errbox.setImage(this.normimg);
                }
                return;
            }
            if ("Big".equals(scmd))
            {
                // set an image with big size to the timed messageboxes
                if (this.bigimg != null && !this.bigimg.isDisposed())
                {
                    this.workbox.setImage(this.bigimg);
                    this.infobox.setImage(this.bigimg);
                    this.warnbox.setImage(this.bigimg);
                    this.errbox.setImage(this.bigimg);
                }
                return;
            }
            if ("Transparent".equals(scmd))
            {
                // set an image with normal size to the timed messageboxes
                if (this.transimg != null && !this.transimg.isDisposed())
                {
                    this.workbox.setImage(this.transimg);
                    this.infobox.setImage(this.transimg);
                    this.warnbox.setImage(this.transimg);
                    this.errbox.setImage(this.transimg);
                }
                return;
            }
            if ("Gif".equals(scmd))
            {
                // set an image with big size to the timed messageboxes
                if (this.gifimg != null && !this.gifimg.isDisposed())
                {
                    this.workbox.setImage(this.gifimg);
                    this.infobox.setImage(this.gifimg);
                    this.warnbox.setImage(this.gifimg);
                    this.errbox.setImage(this.gifimg);
                }
                return;
            }
            if ("Invalid".equals(scmd))
            {
                // try to set an invalid image to a messagebox
                try
                {
                    this.workbox.setImage(new Image(this.display, this.getClass().getResourceAsStream("/res/invalid.png")));
                }
                catch (Exception ex)
                {
                    // display the exception
                    this.showException(ex.getMessage());
                }
                return;
            }
            if ("Empty".equals(scmd))
            {
                scmd = ((MenuItem) event.widget).getParent().getParentItem().getText();
                if ("Change Title".equals(scmd))
                {
                    // change titles to an empty text
                    this.workbox.setText("");
                    this.infobox.setText("");
                    this.warnbox.setText("");
                    this.errbox.setText("");
                    return;
                }
                if ("Change Text".equals(scmd))
                {
                    // change message texts to an empty text
                    this.workbox.setMessage("");
                    this.infobox.setMessage("");
                    this.warnbox.setMessage("");
                    this.errbox.setMessage("");
                    return;
                }
            }
            if ("Short".equals(scmd))
            {
                scmd = ((MenuItem) event.widget).getParent().getParentItem().getText();
                if ("Change Title".equals(scmd))
                {
                    // set short titles to the timed messageboxes
                    this.workbox.setText("Working...");
                    this.infobox.setText("Information...");
                    this.warnbox.setText("Warning...");
                    this.errbox.setText("Error...");
                    return;
                }
                if ("Change Text".equals(scmd))
                {
                    // set short message texts to the timed messageboxes
                    this.workbox.setMessage("This is a \"working\" timed messagebox.");
                    this.infobox.setMessage("This is an \"information\" timed messagebox.");
                    this.warnbox.setMessage("This is a \"warning\" timed messagebox.");
                    this.errbox.setMessage("This is an \"error\" timed messagebox.");
                    return;
                }
            }
            if ("Long".equals(scmd))
            {
                scmd = ((MenuItem) event.widget).getParent().getParentItem().getText();
                if ("Change Title".equals(scmd))
                {
                    // set long titles to the timed messageboxes
                    this.workbox.setText("Long title for Working timed messagebox...");
                    this.infobox.setText("Long title for Information timed messagebox...");
                    this.warnbox.setText("Long title for Warning timed messagebox...");
                    this.errbox.setText("Long title for Error timed messagebox...");
                    return;
                }
                if ("Change Text".equals(scmd))
                {
                    // set long message texts to the timed messageboxes
                    this.workbox.setMessage("This is a long text for the timed messagebox. It is not possible to scroll the text, because any keypress dismisses the messagebox. The text is truncated and it is not possible to the read the whole text.");
                    this.infobox.setMessage("This is a long text for the timed messagebox. It is not possible to scroll the text, because any keypress dismisses the messagebox. The text is truncated and it is not possible to the read the whole text.");
                    this.warnbox.setMessage("This is a long text for the timed messagebox. It is not possible to scroll the text, because any keypress dismisses the messagebox. The text is truncated and it is not possible to the read the whole text.");
                    this.errbox.setMessage("This is a long text for the timed messagebox. It is not possible to scroll the text, because any keypress dismisses the messagebox. The text is truncated and it is not possible to the read the whole text.");
                    return;
                }
            }
        }
        else if (event.widget == this.cmdExit)
        {
            // dispose the images before exit
            if (this.normimg != null && !this.normimg.isDisposed())
                this.normimg.dispose();
            if (this.bigimg != null && !this.bigimg.isDisposed())
                this.bigimg.dispose();
        }
        super.widgetSelected(event);
    }
}
