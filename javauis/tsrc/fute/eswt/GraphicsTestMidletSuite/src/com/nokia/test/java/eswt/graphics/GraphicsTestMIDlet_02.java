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
package com.nokia.test.java.eswt.graphics;

import org.eclipse.swt.SWT;
import org.eclipse.swt.events.PaintEvent;
import org.eclipse.swt.events.PaintListener;
import org.eclipse.swt.events.SelectionEvent;
import org.eclipse.swt.graphics.Image;
import org.eclipse.swt.graphics.Rectangle;
import org.eclipse.swt.widgets.Menu;
import org.eclipse.swt.widgets.MenuItem;
import org.eclipse.swt.widgets.MessageBox;
import org.eclipse.swt.graphics.ImageLoader;

import com.nokia.test.java.eswt.midlet.TestBaseMIDlet;

/**
 * this class is intended to test the gc (graphics) component
 * - drawing filled shapes
 * - drawing focus rectangular
 * - drawing images
 * - copying areas
 */
public class GraphicsTestMIDlet_02 extends TestBaseMIDlet implements PaintListener
{
    /** image instance */
    private Image img = null;
    /** loaded image instance */
    private Image loaded = null;
    /** screen area */
    private Rectangle rect;
    /** shape */
    private byte shape;

    private ImageLoader iLoader = null;

    private static final String sGIF
    = "file:///e:/Images/graphs_gif.gif";
    private static final String sPNG
    = "file:///e:/Images/graphs_png.png";
    private static final String sJPG
    = "file:///e:/Images/graphs_jpg.jpg";
    /**
     * initialization
     */
    protected void init()
    {
        // customize the shell
        this.shell.setText("Graphics 2");
        this.shell.setBackground(this.display.getSystemColor(SWT.COLOR_WHITE));
        this.shell.addPaintListener(this);

        // default values
        this.rect = this.shell.getClientArea();
        this.rect.x += 5;
        this.rect.y += 5;
        this.rect.width -= 11;
        this.rect.height -= 11;
        this.shape = 0;
        this.iLoader = new ImageLoader();
        // try to initialize the image
        try
        {
            this.img = new Image(this.display, this.getClass().getResourceAsStream("/res/medium.jpg"));
        }
        catch (Exception ex)
        {
            // show a messagebox about the exception
            MessageBox mb = new MessageBox(this.shell, SWT.ICON_INFORMATION | SWT.OK);
            mb.setText("Exception");
            mb.setMessage("Image could not be loaded");
            mb.open();
            this.img = null;
        }

        // try to initialize the image
        try
        {
            this.loaded = new Image(this.display, this.iLoader.load(this.getClass().getResourceAsStream("/res/medium.jpg"))[0]);
        }
        catch (Exception ex)
        {
            // show a messagebox about the exception
            MessageBox mb = new MessageBox(this.shell, SWT.ICON_INFORMATION | SWT.OK);
            mb.setText("Exception");
            mb.setMessage("Image could not be loaded");
            mb.open();
            this.loaded = null;
        }

        // setup the menu
        Menu mainmenu = new Menu(this.shell, SWT.BAR);
        this.shell.setMenuBar(mainmenu);
        // "filled shape" submenu
        Menu menu = new Menu(mainmenu);
        MenuItem item = new MenuItem(mainmenu, SWT.CASCADE);
        item.setText("Filled Shape");
        item.setMenu(menu);
        // "arc" menuitem for "filled shape" submenu
        item = new MenuItem(menu, SWT.PUSH);
        item.setText("Arc");
        item.addSelectionListener(this);
        // "gradient rectangle" menuitem for "filled shape" submenu
        item = new MenuItem(menu, SWT.PUSH);
        item.setText("Gradient Rectangle");
        item.addSelectionListener(this);
        // "oval" menuitem for "filled shape" submenu
        item = new MenuItem(menu, SWT.PUSH);
        item.setText("Oval");
        item.addSelectionListener(this);
        // "polygon" menuitem for "filled shape" submenu
        item = new MenuItem(menu, SWT.PUSH);
        item.setText("Polygon");
        item.addSelectionListener(this);
        // "rectangle" menuitem for "filled shape" submenu
        item = new MenuItem(menu, SWT.PUSH);
        item.setText("Rectangle");
        item.addSelectionListener(this);
        // "round rectangle" menuitem for "filled shape" submenu
        item = new MenuItem(menu, SWT.PUSH);
        item.setText("Round Rectangle");
        item.addSelectionListener(this);
        // "focus rectangular" menuitem
        item = new MenuItem(mainmenu, SWT.PUSH);
        item.setText("Focus Rectangular");
        item.addSelectionListener(this);
        // "image" submenu
        menu = new Menu(mainmenu);
        item = new MenuItem(mainmenu, SWT.CASCADE);
        item.setText("Image");
        item.setMenu(menu);
        // "normal" menuitem for "image" submenu
        item = new MenuItem(menu, SWT.PUSH);
        item.setText("Normal");
        item.addSelectionListener(this);
        // "stretched" menuitem for "image" submenu
        item = new MenuItem(menu, SWT.PUSH);
        item.setText("Stretched");
        item.addSelectionListener(this);
        // "shrunk" menuitem for "image" submenu
        item = new MenuItem(menu, SWT.PUSH);
        item.setText("Shrunk");
        item.addSelectionListener(this);
        // "load" menuitem for "image" submenu
        item = new MenuItem(menu, SWT.PUSH);
        item.setText("Load jpeg");
        item.addSelectionListener(this);

        // "copy" submenu
        menu = new Menu(mainmenu);
        item = new MenuItem(mainmenu, SWT.CASCADE);
        item.setText("Copy");
        item.setMenu(menu);
        // "area" menuitem for "copy" submenu
        item = new MenuItem(menu, SWT.PUSH);
        item.setText("Area");
        item.addSelectionListener(this);
        // "image" menuitem for "copy" submenu
        item = new MenuItem(menu, SWT.PUSH);
        item.setText("Image");
        item.addSelectionListener(this);

//      "save" submenu
        menu = new Menu(mainmenu);
        item = new MenuItem(mainmenu, SWT.CASCADE);
        item.setText("Save");
        item.setMenu(menu);
        // "gif" menuitem for "save" submenu
        item = new MenuItem(menu, SWT.PUSH);
        item.setText("GIF");
        item.addSelectionListener(this);
        // "png" menuitem for "save" submenu
        item = new MenuItem(menu, SWT.PUSH);
        item.setText("PNG");
        item.addSelectionListener(this);
        // "jpg" menuitem for "save" submenu
        item = new MenuItem(menu, SWT.PUSH);
        item.setText("JPG");
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
            if ("Arc".equals(scmd))
            {
                // draw a filled arc...
                this.shape = 1;
                this.shell.redraw();
                return;
            }
            if ("Gradient Rectangle".equals(scmd))
            {
                // draw a gradient filled rectangle...
                this.shape = 2;
                this.shell.redraw();
                return;
            }
            if ("Oval".equals(scmd))
            {
                // draw a filled oval...
                this.shape = 3;
                this.shell.redraw();
                return;
            }
            if ("Polygon".equals(scmd))
            {
                // draw a filled polygon...
                this.shape = 4;
                this.shell.redraw();
                return;
            }
            if ("Rectangle".equals(scmd))
            {
                // draw a filled rectangle...
                this.shape = 5;
                this.shell.redraw();
                return;
            }
            if ("Round Rectangle".equals(scmd))
            {
                // draw a filled round rectangle...
                this.shape = 6;
                this.shell.redraw();
                return;
            }
            if ("Focus Rectangular".equals(scmd))
            {
                // draw a focus rectangle... - ??
                this.shape = 7;
                this.shell.redraw();
                return;
            }
            if ("Normal".equals(scmd))
            {
                // draw an image...
                this.shape = 8;
                this.shell.redraw();
                return;
            }
            if ("Stretched".equals(scmd))
            {
                // draw a stretched image...
                this.shape = 9;
                this.shell.redraw();
                return;
            }
            if ("Shrunk".equals(scmd))
            {
                // draw a shrunk image...
                this.shape = 10;
                this.shell.redraw();
                return;
            }
            if ("Load jpeg".equals(scmd))
            {
                // draw a shrunk image...
                this.shape = 13;
                this.shell.redraw();
                return;
            }
            if ("Area".equals(scmd))
            {
                // copy an area to another location...
                this.shape = 11;
                this.shell.redraw();
                return;
            }
            if ("Image".equals(scmd))
            {
                // copy an area into an image...
                this.shape = 12;
                this.shell.redraw();
                return;
            }
            if ("GIF".equals(scmd))
            {
                // copy an area into an image...
                this.iLoader.save(sGIF,SWT.IMAGE_GIF);
                return;
            }
            if ("PNG".equals(scmd))
            {
                // copy an area into an image...
                // draw a shrunk image...
                this.shape = 14;
                this.shell.redraw();
                return;
            }
            if ("JPG".equals(scmd))
            {
                // copy an area into an image...
                this.iLoader.save(sJPG,SWT.IMAGE_JPEG);
                return;
            }
        }
        else if (event.widget == this.cmdExit && this.img != null && !this.img.isDisposed())
            // dispose the image before exit
            this.img.dispose();
        super.widgetSelected(event);
    }

    /**
     * paintlistener implementation
     */
    public void paintControl(PaintEvent event)
    {
        switch (this.shape)
        {
        case 1:
            // draw a filled arc
            event.gc.setBackground(this.display.getSystemColor(SWT.COLOR_GREEN));
            event.gc.fillArc(this.rect.x, this.rect.y, this.rect.width, this.rect.height, 40, 180);
            break;
        case 2:
            // draw a gradient filled rectangle
            event.gc.setBackground(this.display.getSystemColor(SWT.COLOR_RED));
            event.gc.fillGradientRectangle(this.rect.x, this.rect.y, this.rect.width, this.rect.height, true);
            break;
        case 3:
            // draw a filled oval
            event.gc.setBackground(this.display.getSystemColor(SWT.COLOR_YELLOW));
            event.gc.fillOval(this.rect.x, this.rect.y, this.rect.width, this.rect.height);
            break;
        case 4:
            // draw a filled polygon
            event.gc.setBackground(this.display.getSystemColor(SWT.COLOR_BLUE));
            event.gc.fillPolygon(new int[] {5, 5, 40, 40, 80, 150, 200, 180, 150, 30});
            break;
        case 5:
            // draw a filled rectangle
            event.gc.setBackground(this.display.getSystemColor(SWT.COLOR_CYAN));
            event.gc.fillRectangle(this.rect);
            break;
        case 6:
            // draw a filled round rectangle
            event.gc.setBackground(this.display.getSystemColor(SWT.COLOR_MAGENTA));
            event.gc.fillRoundRectangle(this.rect.x, this.rect.y, this.rect.width, this.rect.height, 15, 15);
            break;
        case 7:
            // draw a focus rectangle - ??
            event.gc.drawRectangle(20, 20, 100, 100);
            event.gc.drawFocus(20, 20, 100, 100);
            break;
        case 8:
            // draw an image
            if (this.img != null && !this.img.isDisposed())
                event.gc.drawImage(this.img, this.rect.x, this.rect.y);
            break;
        case 9:
            // draw a stretched image
            if (this.img != null && !this.img.isDisposed())
                event.gc.drawImage(this.img, 0, 0, 100, 100, this.rect.x, this.rect.y, this.rect.width, this.rect.height);
            break;
        case 10:
            // draw a shrunk image
            if (this.img != null && !this.img.isDisposed())
                event.gc.drawImage(this.img, 0, 0, 100, 100, this.rect.x, this.rect.y, 50, 50);
            break;
        case 11:
            // copy an area to another location...
            event.gc.setForeground(this.display.getSystemColor(SWT.COLOR_RED));
            event.gc.drawRectangle(5, 5, 30, 30);
            event.gc.setForeground(this.display.getSystemColor(SWT.COLOR_GREEN));
            event.gc.drawLine(5, 5, 35, 35);
            event.gc.setForeground(this.display.getSystemColor(SWT.COLOR_BLUE));
            event.gc.drawLine(35, 5, 5, 35);
            event.gc.copyArea(5, 5, 30, 30, 5, 45);

            break;
        case 12:
            // copy an area into an image...

            Image imgcpy = new Image(this.display, 31, 31);
            event.gc.setForeground(this.display.getSystemColor(SWT.COLOR_BLACK));
            event.gc.drawRectangle(5, 5, 30, 30);
            event.gc.setForeground(this.display.getSystemColor(SWT.COLOR_YELLOW));
            event.gc.drawLine(5, 5, 35, 35);
            event.gc.setForeground(this.display.getSystemColor(SWT.COLOR_CYAN));
            event.gc.drawLine(35, 5, 5, 35);
            event.gc.copyArea(imgcpy, 5, 5);
            event.gc.drawImage(imgcpy, 5, 45);
            imgcpy.dispose();
            break;
        case 13:
            // draw a loaded image
            if (this.loaded != null && !this.loaded.isDisposed())
                event.gc.drawImage(this.loaded, 0, 0, 100, 100, this.rect.x, this.rect.y, 50, 50);
            break;
        case 14:
            // draw a loaded image
            try
            {
                if (this.loaded != null && !this.loaded.isDisposed())
                {
                    // copy an area into an image...
                    event.gc.setForeground(this.display.getSystemColor(SWT.COLOR_RED));
                    event.gc.drawRectangle(5, 5, 30, 30);
                    event.gc.setForeground(this.display.getSystemColor(SWT.COLOR_GREEN));
                    event.gc.drawLine(5, 5, 35, 35);
                    event.gc.setForeground(this.display.getSystemColor(SWT.COLOR_BLUE));
                    event.gc.drawLine(35, 5, 5, 35);
                    Image imgcopy = new Image(this.display, this.rect);
                    event.gc.copyArea(imgcopy, this.rect.height, this.rect.width);
                    this.iLoader.data = null;
                    this.iLoader.data[0] = imgcopy.getImageData();
                    this.iLoader.save(sPNG,SWT.IMAGE_PNG);
                }
            }
            catch (Exception ex)
            {
                // show a messagebox about the exception
                MessageBox mb = new MessageBox(this.shell, SWT.ICON_INFORMATION | SWT.OK);
                mb.setText("Exception");
                mb.setMessage(ex.getMessage());
                mb.open();

            }

            break;
        case 0:
        default:
            break;
        }
    }
}
