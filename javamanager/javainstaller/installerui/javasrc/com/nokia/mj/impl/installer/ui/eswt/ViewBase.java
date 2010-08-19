/*
* Copyright (c) 2008-2010 Nokia Corporation and/or its subsidiary(-ies).
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


package com.nokia.mj.impl.installer.ui.eswt;


import org.eclipse.swt.custom.ScrolledComposite;
import org.eclipse.swt.SWT;
import org.eclipse.swt.graphics.Image;
import org.eclipse.swt.graphics.Point;
import org.eclipse.swt.graphics.Rectangle;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Display;
import org.eclipse.swt.widgets.Label;
import org.eclipse.swt.widgets.Shell;

/**
 * Base class for different InstallerUi views.
 */
abstract public class ViewBase
{
    /** Maximum view height in percentage from display client area height. */
    protected static final int MAX_VIEW_HEIGHT = 80;
    /** Parent shell for this view. */
    protected Shell iParent = null;
    /** Container for the contents of the view */
    private Composite iContainer = null;
    /** ScrolledComposite for iComposite. */
    private ScrolledComposite iScrolledComposite = null;
    /** Composite to which subclasses add their widgets. */
    private Composite iComposite = null;
    /** Composite to which command buttons are added. */
    private Composite iCommandComposite = null;
    /** InstallerUi owning this view. */
    protected InstallerUiEswt iInstallerUi = null;
    /** True if this view is visible. */
    private boolean iVisible = false;
    /** Number of columns for this view's GridLayout. */
    private int iColumns = 1;
    /** Default content size. */
    private Point iDefaultContentSize = null;

    /** Constructor */
    protected ViewBase()
    {
    }

    /** Constructor */
    protected ViewBase(InstallerUiEswt aInstaller, Composite aParent, int aColumns)
    {
        iInstallerUi = aInstaller;

        // Each view gets a separate shell as a parameter.
        iParent = (Shell)aParent;

        iContainer = new Composite(iParent, 0);
        iContainer.setVisible(false);

        iColumns = aColumns;

        // Store the default bounds for later height adjustments.
        Rectangle defShellBounds = iInstallerUi.defaultShellBounds();
        Rectangle defShellClientBounds = iInstallerUi.defaultShellClientBounds();
        iContainer.setSize(defShellClientBounds.width, defShellClientBounds.height);

        // Let the scrolled composite fill the Shell.
        iContainer.setLayout(setZeroMargins(new GridLayout()));

        // Create a ScrolledComposite for views which need ScrollBars.
        iScrolledComposite = new ScrolledComposite(iContainer, getStyle());
        iScrolledComposite.setAlwaysShowScrollBars(false);
        iScrolledComposite.setExpandHorizontal(true);
        GridData gridData = new GridData(GridData.FILL_BOTH);
        iScrolledComposite.setLayoutData(gridData);

        // Create a composite for command buttons.
        iCommandComposite = new Composite(iContainer, SWT.NONE);
        GridLayout cmdLayout = setZeroMargins(new GridLayout(2, true));
        cmdLayout.marginTop = 5;
        iCommandComposite.setLayout(cmdLayout);
        gridData = new GridData(GridData.FILL_HORIZONTAL);
        iCommandComposite.setLayoutData(gridData);

        // Layout now and get the default size of the content area
        iContainer.layout(true);
        Rectangle rect = iScrolledComposite.getClientArea();
        iDefaultContentSize = new Point(rect.width, rect.height);

        // Create the content composite.
        iComposite = new Composite(iScrolledComposite, SWT.NONE);
        iComposite.setLayout(new GridLayout(getColumns(), true));
        iScrolledComposite.setContent(iComposite);
    }

    /**
     * Returns composite which holds the contents of this view.
     */
    public Composite getContainer()
    {
        return iContainer;
    }

    /** Returns composite to which subclasses can add their widgets. */
    public Composite getComposite()
    {
        return iComposite;
    }

    /** Returns ScrolledComposite for Composite. */
    public ScrolledComposite getScrolledComposite()
    {
        return iScrolledComposite;
    }

    /** Returns composite to which command buttons are added. */
    public Composite getCommandComposite()
    {
        return iCommandComposite;
    }

    /** Returns a shell for this view. */
    public Shell getShell()
    {
        return iParent;
    }

    /** Returns display for this view. */
    public Display getDisplay()
    {
        return getShell().getDisplay();
    }

    /**
     * Returns title for this view. If the title has not been
     * set, returns an empty string.
     */
    public String getTitle()
    {
        return iParent.getText();
    }

    /** Set title for this view. */
    public void setTitle(String aTitle)
    {
        // Dialog shells have no title anymore
    }

    /** Disposes this view. */
    public void dispose()
    {
        // This must be final because it is used from inner class.
        final String className = this.toString();
        log(className + ": disposing");
        // UI updates must be executed in UI thread.
        iParent.getDisplay().syncExec
        (new Runnable()
        {
            public void run()
            {
                if (!isDisposed())
                {
                    log(className + ": disposing view container");
                    iContainer.dispose();
                }
            }
        });
    }

    /** Returns true this view has been disposed, false otherwise. */
    public boolean isDisposed()
    {
        if (iContainer.isDisposed())
        {
            return true;
        }
        return false;
    }

    /** Set this view visible. Any previously visible view will be hidden.
     *  The size of the dialog Shell host will be changed to fit this view.*/
    public void setVisible(boolean aVisible)
    {
        if (iVisible == aVisible)
        {
            return;
        }
        log(this.toString() + ": setVisible " + aVisible);
        iVisible = aVisible;
        if (iVisible)
        {
            iInstallerUi.setActiveView(this);
        }
        else
        {
            if (iInstallerUi.getActiveView() == this)
            {
                iInstallerUi.setActiveView(null);
            }
        }
        // UI updates must be executed in UI thread.
        iParent.getDisplay().syncExec(new Runnable()
        {
            public void run()
            {
                if (isDisposed())
                {
                    return;
                }
                if (iVisible)
                {
                    updateSize();
                }
                iContainer.setVisible(iVisible);
                if (iVisible)
                {
                    if (iParent.getDisplay().getActiveShell() != iParent)
                    {
                        iParent.open();
                    }
                    if (!iParent.isVisible())
                    {
                        iParent.setVisible(true);
                    }
                    if (forceFocusToView())
                    {
                        iContainer.forceFocus();
                    }
                }
            }
        });
    }

    /** Returns true if this view is visible, false otherwise. */
    public boolean isVisible()
    {
        return iVisible;
    }

    /** Sets the view size according to display size. */
    protected void updateSize()
    {
        getShell().setRedraw(false);
        // First calculate size without vertical scrollbar.
        doUpdateSize(false);
        if (getScrolledComposite().getVerticalBar() != null &&
                getScrolledComposite().getVerticalBar().getVisible())
        {
            // If scrollbar is visible recalculate size with scrollbar.
            doUpdateSize(true);
        }
        getShell().setRedraw(true);
    }

    /** Sets the view size according to display size. */
    private void doUpdateSize(boolean aVerticalScrollBarVisible)
    {
        Shell shell = getShell();
        Composite comp = getComposite();
        Composite cmdComp = getCommandComposite();

        int contentWidth = iDefaultContentSize.x;
        if (aVerticalScrollBarVisible)
        {
            int verticalScrollBarWidth =
                getScrolledComposite().getVerticalBar().getSize().x;
            contentWidth = iDefaultContentSize.x - verticalScrollBarWidth;
        }

        // Recalculate size of the content.
        Point contentSize = comp.computeSize(contentWidth, SWT.DEFAULT);
        comp.setSize(contentSize);
        Point cmdContentSize = cmdComp.computeSize(iDefaultContentSize.x, SWT.DEFAULT);

        // Adjust Shell height. The Shell never changes the x position, nor the width.
        Rectangle dispRect = shell.getDisplay().getClientArea();
        int offset = iDefaultContentSize.y - contentSize.y - cmdContentSize.y;

        Rectangle defShellBounds = iInstallerUi.defaultShellBounds();
        int newHeight = defShellBounds.height - offset;
        int maxHeight = dispRect.height * MAX_VIEW_HEIGHT / 100;

        if (newHeight > maxHeight)
        {
            offset -= maxHeight - newHeight;
            newHeight = maxHeight;
        }

        Rectangle dispBounds = shell.getDisplay().getBounds();
        int y = dispBounds.height - newHeight;
        // For landscape orientation center vertically
        if (dispRect.width > dispRect.height)
        {
            y /= 2;
        }
        // Set bounds when command Buttons are in use.
        shell.setBounds(defShellBounds.x, y, defShellBounds.width, newHeight);
        Rectangle clientArea = shell.getClientArea();
        iContainer.setSize(clientArea.width, clientArea.height);

        iContainer.layout(true);
    }

    /**
     * Called when screen orientation changes.
     */
    protected void screenOrientationChanged()
    {
        updateSize();
    }

    /**
     * Returns true if the View should have focus after it has been opened.
     */
    protected boolean forceFocusToView()
    {
        return true;
    }

    /**
     * Returns SWT style for this view. Subclasses should override
     * this method to return style with SWT.H_SCROLL and/or SWT.V_SCROLL
     * if they need scrollbars.
     */
    protected int getStyle()
    {
        return SWT.NONE;
    }

    /**
     * Returns number of columns for this view.
     */
    protected int getColumns()
    {
        return iColumns;
    }

    /**
     * Creates a new label with given text and adds it to this view.
     * The created label takes one column from this view's GridLayout.
     *
     * @param aText text for the label
     * @param aStyle SWT style for the label
     * @return label that was added to this view
     */
    protected Label createLabel(String aText, int aStyle)
    {
        return createLabel(aText, getColumns(), aStyle);
    }

    /**
     * Creates a new label with given text and adds it to this view.
     *
     * @param aText text for the label
     * @param aColumns number of columns the label takes
     * @param aStyle SWT style for the label
     * @return label that was added to this view
     */
    protected Label createLabel(String aText, int aColumns, int aStyle)
    {
        Label label = new Label(getComposite(), aStyle);
        label.setText(aText);
        GridData gridData = new GridData(GridData.FILL_HORIZONTAL);
        gridData.horizontalSpan = aColumns;
        label.setLayoutData(gridData);
        return label;
    }

    /**
     * Creates a new label with given image and adds it to this view.
     * The created label takes one column from this view's GridLayout.
     *
     * @param aImage image for the label
     * @param aStyle SWT style for the label
     * @return label that was added to this view
     */
    protected Label createLabel(Image aImage, int aStyle)
    {
        return createLabel(aImage, getColumns(), aStyle);
    }

    /**
     * Creates a new label with given image and adds it to this view.
     * The created label takes one column from this view's GridLayout.
     *
     * @param aImage image for the label
     * @param aColumns number of columns the label takes
     * @param aStyle SWT style for the label
     * @return label that was added to this view
     */
    protected Label createLabel(Image aImage, int aColumns, int aStyle)
    {
        Label label = new Label(getComposite(), aStyle);
        label.setImage(aImage);
        GridData gridData = new GridData(GridData.FILL_HORIZONTAL);
        gridData.horizontalSpan = aColumns;
        gridData.horizontalAlignment = SWT.CENTER;
        gridData.verticalAlignment = SWT.TOP;
        // This is to lower the image at the same level with the texts.
        // eSWT's Label adds extra space around texts but not around images.
        gridData.verticalIndent = iInstallerUi.iconLabelTopMargin();
        label.setLayoutData(gridData);
        label.setFont(iInstallerUi.getBoldFont());
        return label;
    }

    /**
     * Creates a new label with security icon.
     *
     * @param aIdentified true if security icon is for an
     * identified application, false otherwise
     * @return label that was added to this view
     */
    protected Label createSecurityLabel(boolean aIdentified)
    {
        Label label = createLabel((Image)null, 1, SWT.NONE);
        Image securityIcon = null;
        if (iInstallerUi != null)
        {
            securityIcon = iInstallerUi.getSecurityIcon(
                getDisplay(), aIdentified);
        }
        if (securityIcon != null)
        {
            label.setImage(securityIcon);
        }
        else
        {
            label.setText(aIdentified? "I": "U");
        }
        return label;
    }

    /**
     * Sets margins and spacing for the given layout to zero.
     */
    protected GridLayout setZeroMargins(GridLayout aLayout)
    {
        aLayout.horizontalSpacing = 0;
        aLayout.verticalSpacing = 0;
        aLayout.marginBottom = 0;
        aLayout.marginTop = 0;
        aLayout.marginHeight = 0;
        aLayout.marginWidth = 0;
        aLayout.marginLeft = 0;
        aLayout.marginRight = 0;
        return aLayout;
    }

    protected void log(String aMsg)
    {
        if (iInstallerUi != null)
        {
            iInstallerUi.log(aMsg);
        }
    }

    protected void logError(String aMsg, Throwable aThrowable)
    {
        if (iInstallerUi != null)
        {
            iInstallerUi.logError(aMsg, aThrowable);
        }
    }
}
