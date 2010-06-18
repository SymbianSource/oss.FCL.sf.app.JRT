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
import org.eclipse.swt.graphics.Rectangle;
import org.eclipse.swt.widgets.Menu;
import org.eclipse.swt.widgets.MenuItem;

import com.nokia.test.java.eswt.midlet.TestBaseMIDlet;

/**
 * this class is intended to test the gc (graphics) component
 * - changing line style
 * - changing line width
 * - drawing shapes
 * - drawing strings
 */
public class GraphicsTestMIDlet_01 extends TestBaseMIDlet implements PaintListener
{
    /** menuitems... */
    private MenuItem mnuSolid = null;
    private MenuItem mnuDash = null;
    private MenuItem mnuDot = null;
    private MenuItem mnuDashDot = null;
    private MenuItem mnuDashDotDot = null;
    private MenuItem mnu0 = null;
    private MenuItem mnu1 = null;
    private MenuItem mnu3 = null;
    private MenuItem mnu5 = null;
    /** screen area */
    private Rectangle rect;
    /** line style */
    private int style;
    /** line width */
    private int width;
    /** shape */
    private byte shape;

    /**
     * initialization
     */
    protected void init()
    {
        // customize the shell
        this.shell.setText("Graphics 1");
        this.shell.setBackground(this.display.getSystemColor(SWT.COLOR_YELLOW));
        this.shell.addPaintListener(this);
        // default values
        this.rect = this.shell.getClientArea();
        this.rect = this.shell.getClientArea();
        this.rect.x += 5;
        this.rect.y += 5;
        this.rect.width -= 11;
        this.rect.height -= 11;
        this.style = SWT.LINE_SOLID;
        this.width = 0;
        this.shape = 0;

        // setup the menu
        Menu mainmenu = new Menu(this.shell, SWT.BAR);
        this.shell.setMenuBar(mainmenu);
        // "line style" submenu
        Menu menu = new Menu(mainmenu);
        MenuItem item = new MenuItem(mainmenu, SWT.CASCADE);
        item.setText("Line Style");
        item.setMenu(menu);
        // "solid" menuitem for "line style" submenu
        this.mnuSolid = new MenuItem(menu, SWT.PUSH);
        this.mnuSolid.setText("* Solid");
        this.mnuSolid.addSelectionListener(this);
        // "dash" menuitem for "line style" submenu
        this.mnuDash = new MenuItem(menu, SWT.PUSH);
        this.mnuDash.setText("Dash");
        this.mnuDash.addSelectionListener(this);
        // "dot" menuitem for "line style" submenu
        this.mnuDot = new MenuItem(menu, SWT.PUSH);
        this.mnuDot.setText("Dot");
        this.mnuDot.addSelectionListener(this);
        // "dashdot" menuitem for "line style" submenu
        this.mnuDashDot = new MenuItem(menu, SWT.PUSH);
        this.mnuDashDot.setText("DashDot");
        this.mnuDashDot.addSelectionListener(this);
        // "dashdotdot" menuitem for "line style" submenu
        this.mnuDashDotDot = new MenuItem(menu, SWT.PUSH);
        this.mnuDashDotDot.setText("DashDotDot");
        this.mnuDashDotDot.addSelectionListener(this);
        // "line width" submenu
        menu = new Menu(mainmenu);
        item = new MenuItem(mainmenu, SWT.CASCADE);
        item.setText("Line Width");
        item.setMenu(menu);
        // "0" menuitem for "line width" submenu
        this.mnu0 = new MenuItem(menu, SWT.PUSH);
        this.mnu0.setText("* 0");
        this.mnu0.addSelectionListener(this);
        // "1" menuitem for "line width" submenu
        this.mnu1 = new MenuItem(menu, SWT.PUSH);
        this.mnu1.setText("1");
        this.mnu1.addSelectionListener(this);
        // "3" menuitem for "line width" submenu
        this.mnu3 = new MenuItem(menu, SWT.PUSH);
        this.mnu3.setText("3");
        this.mnu3.addSelectionListener(this);
        // "5" menuitem for "line width" submenu
        this.mnu5 = new MenuItem(menu, SWT.PUSH);
        this.mnu5.setText("5");
        this.mnu5.addSelectionListener(this);
        // "shape" submenu
        menu = new Menu(mainmenu);
        item = new MenuItem(mainmenu, SWT.CASCADE);
        item.setText("Shape");
        item.setMenu(menu);
        // "arc" menuitem for "shape" submenu
        item = new MenuItem(menu, SWT.PUSH);
        item.setText("Arc");
        item.addSelectionListener(this);
        // "line" menuitem for "shape" submenu
        item = new MenuItem(menu, SWT.PUSH);
        item.setText("Line");
        item.addSelectionListener(this);
        // "oval" menuitem for "shape" submenu
        item = new MenuItem(menu, SWT.PUSH);
        item.setText("Oval");
        item.addSelectionListener(this);
        // "point" menuitem for "shape" submenu
        item = new MenuItem(menu, SWT.PUSH);
        item.setText("Point");
        item.addSelectionListener(this);
        // "polygon" menuitem for "shape" submenu
        item = new MenuItem(menu, SWT.PUSH);
        item.setText("Polygon");
        item.addSelectionListener(this);
        // "polyline" menuitem for "shape" submenu
        item = new MenuItem(menu, SWT.PUSH);
        item.setText("Polyline");
        item.addSelectionListener(this);
        // "rectangle" menuitem for "shape" submenu
        item = new MenuItem(menu, SWT.PUSH);
        item.setText("Rectangle");
        item.addSelectionListener(this);
        // "round rectangle" menuitem for "shape" submenu
        item = new MenuItem(menu, SWT.PUSH);
        item.setText("Round Rectangle");
        item.addSelectionListener(this);
        // "string" submenu
        menu = new Menu(mainmenu);
        item = new MenuItem(mainmenu, SWT.CASCADE);
        item.setText("String");
        item.setMenu(menu);
        // "normal" menuitem for "string" submenu
        item = new MenuItem(menu, SWT.PUSH);
        item.setText("Normal");
        item.addSelectionListener(this);
        // "transparent" menuitem for "string" submenu
        item = new MenuItem(menu, SWT.PUSH);
        item.setText("Transparent");
        item.addSelectionListener(this);
        // "text" submenu
        menu = new Menu(mainmenu);
        item = new MenuItem(mainmenu, SWT.CASCADE);
        item.setText("Text");
        item.setMenu(menu);
        // "normal" menuitem for "text" submenu
        item = new MenuItem(menu, SWT.PUSH);
        item.setText("Normal");
        item.addSelectionListener(this);
        // "delimiter" menuitem for "text" submenu
        item = new MenuItem(menu, SWT.PUSH);
        item.setText("Delimiter");
        item.addSelectionListener(this);
        // "tab" menuitem for "text" submenu
        item = new MenuItem(menu, SWT.PUSH);
        item.setText("Tab");
        item.addSelectionListener(this);
        // "mnemonic" menuitem for "text" submenu
        item = new MenuItem(menu, SWT.PUSH);
        item.setText("Mnemonic");
        item.addSelectionListener(this);
        // "transparent" menuitem for "text" submenu
        item = new MenuItem(menu, SWT.PUSH);
        item.setText("Transparent");
        item.addSelectionListener(this);
    }

    /**
     * update the mark in "line style" submenu
     * @param newstyle the selected style
     * @param item the selected menuitem
     */
    private void chgStyle(int newStyle, MenuItem item)
    {
        if (this.style != newStyle)
        {
            switch (this.style)
            {
            case SWT.LINE_SOLID:
                this.mnuSolid.setText("Solid");
                break;
            case SWT.LINE_DASH:
                this.mnuDash.setText("Dash");
                break;
            case SWT.LINE_DOT:
                this.mnuDot.setText("Dot");
                break;
            case SWT.LINE_DASHDOT:
                this.mnuDashDot.setText("DashDot");
                break;
            case SWT.LINE_DASHDOTDOT:
                this.mnuDashDotDot.setText("DashDotDot");
                break;
            default:
                break;
            }
            // set the new style and mark it in the menu
            this.style = newStyle;
            item.setText("* "+item.getText());
            //this.shell.redraw();
            this.shell.update();
        }
    }

    /**
     * update the mark in "line width" submenu
     * @param newwidth the selected width
     * @param item the selected menuitem
     */
    private void chgWidth(int newWidth, MenuItem item)
    {
        if (this.width != newWidth)
        {
            switch (this.width)
            {
            case 0:
                this.mnu0.setText("0");
                break;
            case 1:
                this.mnu1.setText("1");
                break;
            case 3:
                this.mnu3.setText("3");
                break;
            case 5:
                this.mnu5.setText("5");
                break;
            default:
                break;
            }
            // set the new width and mark it in the menu
            this.width = newWidth;
            item.setText("* "+item.getText());
            this.shell.update();
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
            if (event.widget == this.mnuSolid)
            {
                // set the style
                this.chgStyle(SWT.LINE_SOLID, this.mnuSolid);
                return;
            }
            if (event.widget == this.mnuDash)
            {
                // set the style
                this.chgStyle(SWT.LINE_DASH, this.mnuDash);
                return;
            }
            if (event.widget == this.mnuDot)
            {
                // set the style
                this.chgStyle(SWT.LINE_DOT, this.mnuDot);
                return;
            }
            if (event.widget == this.mnuDashDot)
            {
                // set the style
                this.chgStyle(SWT.LINE_DASHDOT, this.mnuDashDot);
                return;
            }
            if (event.widget == this.mnuDashDotDot)
            {
                // set the style
                this.chgStyle(SWT.LINE_DASHDOTDOT, this.mnuDashDotDot);
                return;
            }
            if (event.widget == this.mnu0)
            {
                // set the width
                this.chgWidth(0, this.mnu0);
                return;
            }
            if (event.widget == this.mnu1)
            {
                // set the width
                this.chgWidth(1, this.mnu1);
                return;
            }
            if (event.widget == this.mnu3)
            {
                // set the width
                this.chgWidth(3, this.mnu3);
                return;
            }
            if (event.widget == this.mnu5)
            {
                // set the width
                this.chgWidth(5, this.mnu5);
                return;
            }
            if ("Arc".equals(scmd))
            {
                // draw an arc...
                this.shape = 1;
                this.shell.redraw();
                return;
            }
            if ("Line".equals(scmd))
            {
                // draw a line...
                this.shape = 2;
                this.shell.redraw();
                return;
            }
            if ("Oval".equals(scmd))
            {
                // draw an oval...
                this.shape = 3;
                this.shell.redraw();
                return;
            }
            if ("Point".equals(scmd))
            {
                // draw a point...
                this.shape = 4;
                this.shell.redraw();
                return;
            }
            if ("Polygon".equals(scmd))
            {
                // draw a polygon...
                this.shape = 5;
                this.shell.redraw();
                return;
            }
            if ("Polyline".equals(scmd))
            {
                // draw a polyline...
                this.shape = 6;
                this.shell.redraw();
                return;
            }
            if ("Rectangle".equals(scmd))
            {
                // draw a rectangle...
                this.shape = 7;
                this.shell.redraw();
                return;
            }
            if ("Round Rectangle".equals(scmd))
            {
                // draw a round rectangle...
                this.shape = 8;
                this.shell.redraw();
                return;
            }
            if ("Normal".equals(scmd))
            {
                scmd = ((MenuItem) event.widget).getParent().getParentItem().getText();
                if ("String".equals(scmd))
                {
                    // draw a string with background...
                    this.shape = 9;
                    this.shell.redraw();
                    return;
                }
                if ("Text".equals(scmd))
                {
                    // draw a text with background...
                    this.shape = 11;
                    this.shell.redraw();
                    return;
                }
            }
            if ("Transparent".equals(scmd))
            {
                scmd = ((MenuItem) event.widget).getParent().getParentItem().getText();
                if ("String".equals(scmd))
                {
                    // draw a string without background...
                    this.shape = 10;
                    this.shell.redraw();
                    return;
                }
                if ("Text".equals(scmd))
                {
                    // draw a text without background...
                    this.shape = 15;
                    this.shell.redraw();
                    return;
                }
            }
            if ("Delimiter".equals(scmd))
            {
                // draw a text in multiple lines...
                this.shape = 12;
                this.shell.redraw();
                return;
            }
            if ("Tab".equals(scmd))
            {
                // draw a text with expanded tabs...
                this.shape = 13;
                this.shell.redraw();
                return;
            }
            if ("Mnemonic".equals(scmd))
            {
                // draw a text with underlined mnemonic characters...
                this.shape = 14;
                this.shell.redraw();
                return;
            }
        }
        super.widgetSelected(event);
    }

    /**
     * paintlistener implementation
     */
    public void paintControl(PaintEvent event)
    {
        // set the line style & line width
        event.gc.setLineStyle(this.style);
        event.gc.setLineWidth(this.width);
        switch (this.shape)
        {
        case 1:
            // draw an arc
            event.gc.setForeground(this.display.getSystemColor(SWT.COLOR_BLUE));
            event.gc.drawArc(this.rect.x, this.rect.y, this.rect.width, this.rect.height, 40, 180);
            break;
        case 2:
            // draw a line
            event.gc.setForeground(this.display.getSystemColor(SWT.COLOR_CYAN));
            event.gc.drawLine(this.rect.x, this.rect.y, this.rect.width, this.rect.height);
            break;
        case 3:
            // draw an oval
            event.gc.setForeground(this.display.getSystemColor(SWT.COLOR_GREEN));
            event.gc.drawOval(this.rect.x, this.rect.y, this.rect.width, this.rect.height);
            break;
        case 4:
            // draw a point
            event.gc.setForeground(this.display.getSystemColor(SWT.COLOR_BLACK));
            event.gc.drawPoint(this.rect.x+(this.rect.width/2), this.rect.y+(this.rect.height/2));
            break;
        case 5:
            // draw a polygon
            event.gc.setForeground(this.display.getSystemColor(SWT.COLOR_MAGENTA));
            event.gc.drawPolygon(new int[] {5, 5, 40, 40, 80, 150, 200, 180, 150, 30});
            break;
        case 6:
            // draw a polyline
            event.gc.setForeground(this.display.getSystemColor(SWT.COLOR_RED));
            event.gc.drawPolyline(new int[] {5, 5, 40, 40, 80, 150, 200, 180, 150, 30});
            break;
        case 7:
            // draw a rectangle
            event.gc.setForeground(this.display.getSystemColor(SWT.COLOR_DARK_GREEN));
            event.gc.drawRectangle(this.rect);
            break;
        case 8:
            // draw a round rectangle
            event.gc.setForeground(this.display.getSystemColor(SWT.COLOR_DARK_RED));
            event.gc.drawRoundRectangle(this.rect.x, this.rect.y, this.rect.width, this.rect.height, 15, 15);
            break;
        case 9:
            // draw a string with background...
            event.gc.setBackground(this.display.getSystemColor(SWT.COLOR_CYAN));
            event.gc.setForeground(this.display.getSystemColor(SWT.COLOR_BLUE));
            event.gc.drawString("The text has", 5, 5);
            event.gc.drawString("cyan background,", 5, 35);
            event.gc.drawString("blue foreground", 5, 65);
            break;
        case 10:
            // draw a string without background...
            event.gc.setBackground(this.display.getSystemColor(SWT.COLOR_CYAN));
            event.gc.setForeground(this.display.getSystemColor(SWT.COLOR_BLUE));
            event.gc.drawString("Transparent text", 5, 5, true);
            break;
        case 11:
            // draw a text with background...
            event.gc.setBackground(this.display.getSystemColor(SWT.COLOR_CYAN));
            event.gc.setForeground(this.display.getSystemColor(SWT.COLOR_BLUE));
            event.gc.drawText("Text in multiple\nlines\twith\ttabs\ncyan background\nblue foreground", 5, 5);
            break;
        case 12:
            // draw a text in multiple lines...
            event.gc.setBackground(this.display.getSystemColor(SWT.COLOR_CYAN));
            event.gc.setForeground(this.display.getSystemColor(SWT.COLOR_BLUE));
            event.gc.drawText("This\rtext\nhas\nline\ndelimiters", 5, 5, SWT.DRAW_DELIMITER);
            break;
        case 13:
            // draw a text with expanded tabs...
            event.gc.setBackground(this.display.getSystemColor(SWT.COLOR_CYAN));
            event.gc.setForeground(this.display.getSystemColor(SWT.COLOR_BLUE));
            event.gc.drawText("Text\twith\ttabs", 5, 5, SWT.DRAW_TAB);
            break;
        case 14:
            // draw a text with underlined mnemonic characters...
            event.gc.setBackground(this.display.getSystemColor(SWT.COLOR_CYAN));
            event.gc.setForeground(this.display.getSystemColor(SWT.COLOR_BLUE));
            event.gc.drawText("&Te&xt wi&th& mne&mo&nics", 5, 5, SWT.DRAW_MNEMONIC);
            break;
        case 15:
            // draw a text without background...
            event.gc.setBackground(this.display.getSystemColor(SWT.COLOR_CYAN));
            event.gc.setForeground(this.display.getSystemColor(SWT.COLOR_BLUE));
            event.gc.drawText("Transparent text", 5, 5, SWT.DRAW_TRANSPARENT);
            break;
        case 0:
        default:
            break;
        }
    }
}
