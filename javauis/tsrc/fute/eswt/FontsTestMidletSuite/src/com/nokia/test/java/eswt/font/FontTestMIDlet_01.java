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

package com.nokia.test.java.eswt.font;

import java.util.Enumeration;
import java.util.Vector;

import org.eclipse.ercp.swt.mobile.Command;
import org.eclipse.ercp.swt.mobile.QueryDialog;
import org.eclipse.swt.SWT;
import org.eclipse.swt.events.PaintEvent;
import org.eclipse.swt.events.PaintListener;
import org.eclipse.swt.events.SelectionEvent;
import org.eclipse.swt.graphics.Font;
import org.eclipse.swt.graphics.FontData;
import org.eclipse.swt.layout.FillLayout;
import org.eclipse.swt.widgets.Canvas;

import com.nokia.test.java.eswt.midlet.TestBaseMIDlet;

/**
 *
 */
public class FontTestMIDlet_01 extends TestBaseMIDlet implements PaintListener
{
    /** canvas */
    private Canvas canvas = null;

    /** currently used font height */
    private int height          = 12;
    /** currently used font face */
    protected String fontface   = null;
    /** created fonts */
    protected Vector fonts      = null;

    /** commands */
    // font height
    protected Command cmdFontHeight = null;
    // font face commands
    protected Vector fontFaceCMDs   = null;

    /* (non-Javadoc)
     * @see com.nokia.test.java.eswt.midlet.TestBaseMIDlet#init()
     */
    protected void init()
    {
        this.shell.setText("Font Test");
        this.shell.setLayout(new FillLayout());

        this.fonts = new Vector();

        // commands
        Command cmdFont = new Command(this.shell, Command.COMMANDGROUP, 0);
        cmdFont.setText("Font");

        // font height
        this.cmdFontHeight = new Command(cmdFont, Command.GENERAL, 1);
        this.cmdFontHeight.setText("Height");
        this.cmdFontHeight.addSelectionListener(this);

        // font faces
        Command cmdFontFace = new Command(cmdFont, Command.COMMANDGROUP, 0);
        cmdFontFace.setText("Face (name)");

        this.fontFaceCMDs = new Vector();
        String[] faces = this.getFontFaces();
        for (int i=0; i<faces.length; i++)
        {
            Command cmd = new Command(cmdFontFace, Command.GENERAL, i);
            cmd.setText(faces[i]);
            cmd.addSelectionListener(this);
            this.fontFaceCMDs.addElement(cmd);
        }

        // create Canvas
        this.canvas = new Canvas(this.shell,SWT.NO_BACKGROUND);
        canvas.addPaintListener(this);
    }

    /**
     *
     */
    private String[] getFontFaces()
    {
        FontData[] fdata = this.display.getFontList(null, true);

        System.out.println("Font list obtained. Items count " + fdata.length);

        Vector ff = new Vector();
        for (int i=0; i<fdata.length; i++)
        {
            String fface = fdata[i].getName();
            if (!ff.contains(fface))
            {
                ff.addElement(fface);
            }
        }

        String[] result = new String[ff.size()];
        ff.copyInto(result);
        return result;
    }

    /* (non-Javadoc)
     * @see com.nokia.test.java.eswt.midlet.TestBaseMIDlet#widgetSelected(org.eclipse.swt.events.SelectionEvent)
     */
    public void widgetSelected(SelectionEvent event)
    {
        // set height command
        if (event.widget == this.cmdFontHeight)
        {
            QueryDialog dlg =
                new QueryDialog(this.shell, SWT.NONE, QueryDialog.NUMERIC);
            dlg.setPromptText("Set Font height", String.valueOf(this.height));
            String result = dlg.open();
            if (result != null)
            {
                this.height = Integer.parseInt(result);
            }
            this.canvas.redraw();
            return;
        }

        // font face selection
        if (this.fontFaceCMDs.contains(event.widget))
        {
            this.fontface = ((Command) event.widget).getText();
            this.canvas.redraw();
            return;
        }

        // pre-exit cleaning
        if (event.widget == this.cmdExit)
        {
            this.clean();
        }

        super.widgetSelected(event);
    }

    /* (non-Javadoc)
     * @see org.eclipse.swt.events.PaintListener#paintControl(org.eclipse.swt.events.PaintEvent)
     */
    public void paintControl(PaintEvent e)
    {
        if (e.widget == this.canvas)
        {
            if (this.fontface == null)
            {
                // font face is not defined, do nothing
                return;
            }

            // dispose previously allocated fonts
            this.clean();

            int x = 10;
            int y = 10;
            Font font;

            String str = "eSWT Fonts";
            font = new Font(this.display, this.fontface, this.height,
                            SWT.NORMAL);
            e.gc.setFont(font);
            e.gc.drawString(str, x, y);
            y += e.gc.getFontMetrics().getHeight();
            System.out.println("Font Height = " + font.getFontData()[0].getHeight());
            this.fonts.addElement(font);

            font = new Font(this.display, this.fontface, this.height,
                            SWT.ITALIC);
            e.gc.setFont(font);
            e.gc.drawString(str, x, y);
            y += e.gc.getFontMetrics().getHeight();
            this.fonts.addElement(font);

            font = new Font(this.display, this.fontface, this.height,
                            SWT.BOLD);
            e.gc.setFont(font);
            e.gc.drawText(str, x, y);
            y += e.gc.getFontMetrics().getHeight();
            this.fonts.addElement(font);

            font = new Font(this.display, this.fontface, this.height,
                            SWT.ITALIC | SWT.BOLD);
            e.gc.setFont(font);
            e.gc.drawText(str, x, y);
            y += e.gc.getFontMetrics().getHeight();
            this.fonts.addElement(font);
        }
    }

    /**
     * Cleaning
     */
    protected void clean()
    {
        if (this.fonts != null)
        {
            Enumeration fs = this.fonts.elements();
            while (fs.hasMoreElements())
            {
                Font font = (Font)fs.nextElement();
                if (font != null && !font.isDisposed())
                    font.dispose();
            }
            this.fonts.removeAllElements();
        }
    }
}
