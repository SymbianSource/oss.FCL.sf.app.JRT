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

package com.nokia.test.java.eswt.multipagedialog;

import java.util.Date;

import org.eclipse.swt.SWT;
import org.eclipse.swt.widgets.Button;
import org.eclipse.swt.widgets.Combo;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.MessageBox;
import org.eclipse.swt.widgets.ProgressBar;
import org.eclipse.swt.widgets.Slider;
import org.eclipse.swt.widgets.Text;
import org.eclipse.ercp.swt.mobile.TextExtension;
import org.eclipse.ercp.swt.mobile.Command;
import org.eclipse.ercp.swt.mobile.ConstrainedText;
import org.eclipse.ercp.swt.mobile.DateEditor;
import org.eclipse.ercp.swt.mobile.HyperLink;
import org.eclipse.ercp.swt.mobile.MultiPageDialog;
import org.eclipse.swt.events.SelectionEvent;
import org.eclipse.swt.graphics.Image;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.widgets.Label;

import com.nokia.test.java.eswt.midlet.TestBaseMIDlet;

public class MultiPageDialogTestMIDlet_01 extends TestBaseMIDlet
{

    MultiPageDialog mpdialog = null;
    Composite initialPage = null;

    Command backCMD = null;

    Command showMultipageCMD = null;

    Command addMenu = null;
    Command addPage = null;
    Command addPageWithImage = null;
    Command addManyPages = null;
    Command addManyPagesWithImage = null;
    Command addButtonPage = null;
    Command addListPage = null;
    Command addTextPage = null;
    Command addControlPage = null;
    Command addBrowserPage = null;

    Command removePage = null;
    Command removeAll = null;

    Command SelectFirst = null;
    Command SelectLast = null;

    Command updateTitleEmpty = null;
    Command updateTitleNormal = null;
    Command updateTitleLong = null;
    Command updateTitleSpecialChars = null;

    Command updateImageNull = null;
    Command updateImageIcon = null;
    Command updateImageBig = null;
    Command updateImageSmall = null;

    /** custom icon for the pages */
    private Image icon = null;
    private String icon_fn = "icon.png";
    /** custom big image for the pages */
    private Image bigImage = null;
    private String bigImage_fn = "big.png";
    /** custom small for the pages */
    private Image smallImage = null;
    private String smallImage_fn = "medium.jpg";

    /**
     * show a messagebox with the thrown exception
     * @param the text to be displayed in the messagebox
     */
    protected void showException(String title, String msg)
    {
        MessageBox mb = new MessageBox(this.shell, SWT.ICON_INFORMATION | SWT.OK);
        mb.setText(title);
        mb.setMessage(msg);
        mb.open();
    }

    /**
     * try to load image from resourse
     * @param filename
     * @param image
     */
    protected Image loadImage(String filename)
    {
        Image image = null;
        // try to initialize the image
        try
        {
            image = new Image(this.display, this.getClass().getResourceAsStream("/res/"+filename));
        }
        catch (Exception ex)
        {
            // show a messagebox about the exception
            this.showException("Exception",image+" could not be loaded");
            image = null;
        }
        return image;
    }
    protected void init()
    {

        this.shell.setText("Multipage Dialog Test");
        this.shell.setLayout(new GridLayout());

        this.icon = this.loadImage(this.icon_fn);
        this.bigImage = this.loadImage(this.bigImage_fn);
        this.smallImage = this.loadImage(this.smallImage_fn);

        mpdialog = new MultiPageDialog(this.shell);
        this.initialPage = mpdialog.createPage("Initial Page", null);
        this.backCMD = new Command(this.initialPage, Command.OK, 1);
        this.backCMD.setText("Back");
        this.backCMD.addSelectionListener(this);

        this.showMultipageCMD = new Command(this.shell, Command.OK, 1);
        this.showMultipageCMD.setText("Show");
        this.showMultipageCMD.addSelectionListener(this);

        //  Add submenu
        this.addMenu = new Command(this.shell, Command.COMMANDGROUP, 2);
        this.addMenu.setText("Add");

        this.addPage = new Command(addMenu, Command.GENERAL, 1);
        this.addPage.setText("One with label");
        this.addPage.addSelectionListener(this);

        this.addPageWithImage = new Command(addMenu, Command.GENERAL, 1);
        this.addPageWithImage.setText("One with image");
        this.addPageWithImage.addSelectionListener(this);

        this.addManyPages = new Command(addMenu, Command.GENERAL, 0);
        this.addManyPages.setText("100 with label");
        this.addManyPages.addSelectionListener(this);

        this.addManyPagesWithImage = new Command(addMenu, Command.GENERAL, 0);
        this.addManyPagesWithImage.setText("100 with image");
        this.addManyPagesWithImage.addSelectionListener(this);

        this.addButtonPage = new Command(addMenu, Command.GENERAL, 0);
        this.addButtonPage.setText("Buttons");
        this.addButtonPage.addSelectionListener(this);

        this.addListPage = new Command(addMenu, Command.GENERAL, 0);
        this.addListPage.setText("Lists");
        this.addListPage.addSelectionListener(this);

        this.addTextPage = new Command(addMenu, Command.GENERAL, 0);
        this.addTextPage.setText("Texts");
        this.addTextPage.addSelectionListener(this);

        this.addControlPage = new Command(addMenu, Command.GENERAL, 0);
        this.addControlPage.setText("Controls");
        this.addControlPage.addSelectionListener(this);

        this.addBrowserPage = new Command(addMenu, Command.GENERAL, 0);
        this.addBrowserPage.setText("Browser");
        this.addBrowserPage.addSelectionListener(this);

        //  Remove submenu
        Command removeMenu = new Command(this.initialPage.getShell(), Command.COMMANDGROUP, 1);
        removeMenu.setText("Remove");


        this.removePage = new Command(removeMenu, Command.GENERAL, 1);
        this.removePage.setText("Selected");
        this.removePage.addSelectionListener(this);

        this.removeAll = new Command(removeMenu, Command.GENERAL, 0);
        this.removeAll.setText("All");
        this.removeAll.addSelectionListener(this);

        //  Selection submenu
        Command selectMenu = new Command(this.initialPage.getShell(), Command.COMMANDGROUP, 1);
        selectMenu.setText("Select");

        this.SelectFirst = new Command(selectMenu, Command.GENERAL, 1);
        this.SelectFirst.setText("First");
        this.SelectFirst.addSelectionListener(this);

        this.SelectLast = new Command(selectMenu, Command.GENERAL, 0);
        this.SelectLast.setText("Last");
        this.SelectLast.addSelectionListener(this);

        //  Update submenu
        Command updateMenu = new Command(this.initialPage.getShell(), Command.COMMANDGROUP, 1);
        updateMenu.setText("Update");

        //  Update title submenu
        Command updateTitleMenu = new Command(updateMenu, Command.COMMANDGROUP, 1);
        updateTitleMenu.setText("Title");

        this.updateTitleEmpty = new Command(updateTitleMenu, Command.GENERAL, 1);
        this.updateTitleEmpty.setText("Empty Title");
        this.updateTitleEmpty.addSelectionListener(this);

        this.updateTitleNormal = new Command(updateTitleMenu, Command.GENERAL, 1);
        this.updateTitleNormal.setText("Normal Title");
        this.updateTitleNormal.addSelectionListener(this);

        this.updateTitleLong = new Command(updateTitleMenu, Command.GENERAL, 1);
        this.updateTitleLong.setText("Long Title");
        this.updateTitleLong.addSelectionListener(this);

        this.updateTitleSpecialChars = new Command(updateTitleMenu, Command.GENERAL, 1);
        this.updateTitleSpecialChars.setText("Special chars Title");
        this.updateTitleSpecialChars.addSelectionListener(this);

        //  Update image submenu
        Command updateImageMenu = new Command(updateMenu, Command.COMMANDGROUP, 1);
        updateImageMenu.setText("Image");

        this.updateImageIcon = new Command(updateImageMenu, Command.GENERAL, 0);
        this.updateImageIcon.setText("Icon");
        this.updateImageIcon.addSelectionListener(this);

        this.updateImageSmall = new Command(updateImageMenu, Command.GENERAL, 0);
        this.updateImageSmall.setText("Small");
        this.updateImageSmall.addSelectionListener(this);

        this.updateImageBig = new Command(updateImageMenu, Command.GENERAL, 0);
        this.updateImageBig.setText("Big");
        this.updateImageBig.addSelectionListener(this);

        this.updateImageNull = new Command(updateImageMenu, Command.GENERAL, 0);
        this.updateImageNull.setText("Null");
        this.updateImageNull.addSelectionListener(this);

    }

    private void createPageWithLists()
    {
        Composite page = mpdialog.createPage("Lists",null);
        page.setLayout(new GridLayout(2, true));

        Command back = new Command(page, Command.BACK, 1);
        back.setText("Back");
        back.addSelectionListener(this);

    }
    private void createPageWithButtons()
    {
        Composite page = mpdialog.createPage("Buttons",null);
        page.setLayout(new GridLayout(2, true));

        this.addButton(page,SWT.PUSH,"Push");
        this.addButton(page,SWT.TOGGLE,"Toggle");
        this.addButton(page,SWT.RADIO, "1st");
        this.addButton(page,SWT.RADIO, "2nd");
        this.addButton(page,SWT.RADIO, "3rd");
        this.addButton(page,SWT.CHECK, "1st");
        this.addButton(page,SWT.CHECK, "2nd");
        this.addButton(page,SWT.CHECK, "3rd");

        Command back = new Command(page, Command.BACK, 1);
        back.setText("Back");
        back.addSelectionListener(this);

    }
    private void createPageWithBrowser()
    {
        Composite page = mpdialog.createPage("Browser",null);
        page.setLayout(new GridLayout(2, true));

        Command back = new Command(page, Command.BACK, 1);
        back.setText("Back");
        back.addSelectionListener(this);

    }
    private void createPageWithControls()
    {
        Composite page = mpdialog.createPage("Controls",null);
        page.setLayout(new GridLayout(2, true));

        this.addCombo(page);
        this.addLabel(page);
        this.addDateEditor(page);
        this.addHyperlink(page);
        this.addSlider(page);
        this.addProgressBar(page);

        Command back = new Command(page, Command.BACK, 1);
        back.setText("Back");
        back.addSelectionListener(this);

    }
    private void createPageWithTexts()
    {
        Composite page = mpdialog.createPage("Controls",null);
        page.setLayout(new GridLayout(2, true));

        this.addText(page);
        this.addTextExtension(page);
        this.addConstrainedText(page);

        Command back = new Command(page, Command.BACK, 1);
        back.setText("Back");
        back.addSelectionListener(this);

    }
    public void widgetSelected(SelectionEvent e)
    {

        if (e.widget == this.backCMD)
        {
            this.mpdialog.close();
            this.shell.setActive();
            return;
        }
        if (e.widget instanceof Command)
        {
            String scmd = ((Command) e.widget).getText();
            if (scmd.equals("Back"))
            {
                this.mpdialog.close();
                this.shell.setActive();
                return;
            }
        }
        if (e.widget == this.showMultipageCMD)
        {

            Command addingMenu = new Command(this.initialPage, Command.COMMANDGROUP, 1);
            addingMenu = this.addMenu;
            mpdialog.setSelection(mpdialog.getPageCount() - 1); // The last page

            mpdialog.open(); // open a modal dialog
            return;
        }
        if (e.widget == this.addPage)
        {
            Composite page = mpdialog.createPage("Added Page", null);
            page.setLayout(new GridLayout(1, true));
            Label label = new Label(page, SWT.NONE);
            label.setText("Label in Page");
            label.setAlignment(SWT.CENTER);
            Command back = new Command(page, Command.BACK, 1);
            back.setText("Back");
            back.addSelectionListener(this);
            return;
        }
        if (e.widget == this.addPageWithImage)
        {
            Composite page = mpdialog.createPage("Added Page", this.icon);
            page.setLayout(new GridLayout(1, true));
            Label label = new Label(page, SWT.NONE);
            label.setText("Label in Page");
            label.setAlignment(SWT.CENTER);
            Command back = new Command(page, Command.BACK, 1);
            back.setText("Back");
            back.addSelectionListener(this);
            return;
        }
        if (e.widget == this.addManyPages)
        {
            for (int i = 0; i < 100; i++)
            {
                Composite page = mpdialog.createPage("Page"+(i+1), null);
                page.setLayout(new GridLayout(1, true));
                Label label = new Label(page, SWT.NONE);
                label.setText("Label in Page");
                label.setAlignment(SWT.CENTER);
                Command back = new Command(page, Command.BACK, 1);
                back.setText("Back");
                back.addSelectionListener(this);
            }
            return;
        }
        if (e.widget == this.addManyPagesWithImage)
        {
            for (int i = 0; i < 100; i++)
            {
                Composite page = mpdialog.createPage("",this.icon);
                page.setLayout(new GridLayout(1, true));
                Label label = new Label(page, SWT.NONE);
                label.setText("Label in Page");
                label.setAlignment(SWT.CENTER);
                Command back = new Command(page, Command.BACK, 1);
                back.setText("Back");
                back.addSelectionListener(this);
            }
            return;
        }
        if (e.widget == this.addButtonPage)
        {

            this.createPageWithButtons();
            return;
        }
        if (e.widget == this.addListPage)
        {

            this.createPageWithLists();
            return;
        }
        if (e.widget == this.addControlPage)
        {

            this.createPageWithControls();
            return;
        }
        if (e.widget == this.addBrowserPage)
        {

            this.createPageWithBrowser();
            return;
        }
        if (e.widget == this.addTextPage)
        {

            this.createPageWithTexts();
            return;
        }
        if (e.widget == this.removeAll)
        {
            for (int i = 0; i < this.mpdialog.getPageCount()-1; i++)
            {
                this.mpdialog.deletePage(i);
            }
            return;
        }
        if (e.widget == this.removePage)
        {
            this.mpdialog.deletePage(this.mpdialog.getSelectionIndex());
            return;
        }
        if (e.widget == this.SelectFirst)
        {
            this.mpdialog.setSelection(0);
            return;
        }
        if (e.widget == this.SelectLast)
        {
            this.mpdialog.setSelection(this.mpdialog.getPageCount()-1);
            return;
        }
        if (e.widget == this.updateTitleNormal)
        {
            this.mpdialog.setTitle(this.mpdialog.getSelectionIndex(), "Updated");
            return;
        }
        if (e.widget == this.updateTitleEmpty)
        {
            this.mpdialog.setTitle(this.mpdialog.getSelectionIndex(), "");
            return;
        }
        if (e.widget == this.updateTitleLong)
        {
            this.mpdialog.setTitle(this.mpdialog.getSelectionIndex(), "Very Very Very long title to see how title is truncated");
            return;
        }
        if (e.widget == this.updateTitleSpecialChars)
        {
            this.mpdialog.setTitle(this.mpdialog.getSelectionIndex(), "? \n \t %&@#£$ !!!");
            return;
        }
        if (e.widget == this.updateImageIcon)
        {
            try
            {
                if (this.icon != null)
                    this.mpdialog.setImage(this.mpdialog.getSelectionIndex(), this.icon);
            }
            catch (Exception ex)
            {
                this.showException("Error Updating Image",ex.toString());
            }
            return;
        }
        if (e.widget == this.updateImageBig)
        {
            try
            {
                if (this.bigImage != null)
                    this.mpdialog.setImage(this.mpdialog.getSelectionIndex(), this.bigImage);
            }
            catch (Exception ex)
            {
                this.showException("Error Updating Image",ex.toString());
            }

            return;
        }
        if (e.widget == this.updateImageSmall)
        {
            try
            {
                if (this.smallImage != null)
                    this.mpdialog.setImage(this.mpdialog.getSelectionIndex(), this.smallImage);
            }
            catch (Exception ex)
            {
                this.showException("Error Updating Image",ex.toString());
            }
            return;
        }
        if (e.widget == this.updateImageNull)
        {
            try
            {
                this.mpdialog.setImage(this.mpdialog.getSelectionIndex(), null);
            }
            catch (Exception ex)
            {
                this.showException("Error Updating Image",ex.toString());
            }
            return;
        }
        super.widgetSelected(e);
    }

    /**
     * Create a Label and add it to current shell.
     */
    private void addLabel(Composite page)
    {

        // control
        Label control = new Label(page, SWT.NONE);
        control.setText("A Label control");
        GridData data = new GridData();
        data.grabExcessHorizontalSpace = true;
        data.horizontalAlignment = SWT.CENTER;
        control.setLayoutData(data);
    }

    /**
     * Create a Hyperlink and add it to current shell.
     */
    private void addHyperlink(Composite page)
    {

        // control
        HyperLink control = new HyperLink(page, SWT.NONE, HyperLink.EMAIL);
        control.setText("some.name@foo.com");
        GridData data = new GridData();
        data.grabExcessHorizontalSpace = true;
        data.horizontalAlignment = SWT.CENTER;
        control.setLayoutData(data);
    }

    /**
     * Create a Text and add it to current shell.
     */
    private void addText(Composite page)
    {

        // control
        Text control = new Text(page, SWT.BORDER);
        control.setText("Text field");
        GridData data = new GridData();
        data.grabExcessHorizontalSpace = true;
        data.horizontalAlignment = SWT.CENTER;
        control.setLayoutData(data);
    }

    private void addTextExtension(Composite page)
    {

        // control
        TextExtension control = new TextExtension(page, SWT.BORDER);
        control.setText("Text field");
        GridData data = new GridData();
        data.grabExcessHorizontalSpace = true;
        data.horizontalAlignment = SWT.CENTER;
        control.setLayoutData(data);
    }

    /**
     * Create a ConstrainedText and add it to current shell.
     */
    private void addConstrainedText(Composite page)
    {

        // control
        ConstrainedText control =
            new ConstrainedText(page, SWT.BORDER, ConstrainedText.PHONENUMBER);
        control.setText("+*#0123456789");
        control.setTextLimit(15);
        GridData data = new GridData();
        data.grabExcessHorizontalSpace = true;
        data.horizontalAlignment = SWT.CENTER;
        control.setLayoutData(data);
    }

    /**
     * Create a Slider control and add it to current shell.
     */
    private void addSlider(Composite page)
    {

        // control
        Slider control = new Slider(page, SWT.HORIZONTAL);
        control.setMaximum(10);
        GridData data = new GridData();
        data.grabExcessHorizontalSpace = true;
        data.horizontalAlignment = SWT.CENTER;
        control.setLayoutData(data);
    }

    /**
     * Create a DateEditor and add it to current shell.
     */
    private void addDateEditor(Composite page)
    {

        // control
        DateEditor control =
            new DateEditor(page, SWT.BORDER, DateEditor.DATE_TIME);
        control.setDate(new Date());
        GridData data = new GridData();
        data.grabExcessHorizontalSpace = true;
        data.horizontalAlignment = SWT.CENTER;
        control.setLayoutData(data);
    }

    /**
     * Create a Button and add it to current shell.
     */
    private void addButton(Composite page,int style, String name)
    {

        // control
        Button control = new Button(page, style);
        control.setText(name);
        GridData data = new GridData();
        data.grabExcessHorizontalSpace = true;
        data.horizontalAlignment = SWT.CENTER;
        control.setLayoutData(data);
    }

    /**
     * Create a Combo control and add it to current shell.
     */
    private void addCombo(Composite page)
    {

        // control
        Combo control =
            new Combo(page, SWT.BORDER | SWT.DROP_DOWN | SWT.READ_ONLY);
        control.add("item 1");
        control.add("item 2");
        control.add("item 3");
        GridData data = new GridData();
        data.grabExcessHorizontalSpace = true;
        data.horizontalAlignment = SWT.CENTER;
        control.setLayoutData(data);
        control.pack();
    }

    /**
     * Create a ProgressBar and add it to current shell.
     */
    private void addProgressBar(Composite page)
    {

        // control
        ProgressBar control = new ProgressBar(page, SWT.BORDER | SWT.HORIZONTAL);
        control.setMaximum(100);
        GridData data = new GridData();
        data.grabExcessHorizontalSpace = true;
        data.horizontalAlignment = SWT.CENTER;
        control.setLayoutData(data);
    }

}

