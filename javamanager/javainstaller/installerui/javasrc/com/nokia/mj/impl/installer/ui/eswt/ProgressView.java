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

import org.eclipse.swt.SWT;
import org.eclipse.swt.events.KeyEvent;
import org.eclipse.swt.events.KeyListener;
import org.eclipse.swt.events.SelectionEvent;
import org.eclipse.swt.events.SelectionListener;
import org.eclipse.swt.graphics.Point;
import org.eclipse.swt.graphics.Rectangle;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.widgets.Button;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Control;
import org.eclipse.swt.widgets.Label;
import org.eclipse.swt.widgets.ProgressBar;
import org.eclipse.swt.widgets.Shell;

/**
 * ProgressView displays a progress bar.
 * Value range for the progress bar is between 0 and 100,
 * unless the progress bar is created as indeterminate
 * in which case the value is ignored.
 */
public class ProgressView extends ViewBase
{
    private KeyListener iViewKeyListener = null;
    private KeyListener iSoftKeyListener = null;
    private Label iLabel = null;
    private String iMsg = null;
    private ProgressBar iProgressBar = null;
    private boolean iIndeterminate = false;
    private int iValue = 0;
    protected Button iEmptyCommand = null;
    protected Button iCancelCommand = null;

    /** Constructor */
    protected ProgressView()
    {
        super();
    }

    /** Constructor */
    protected ProgressView(InstallerUiEswt aInstaller, Composite aParent)
    {
        this(aInstaller, aParent, "");
    }

    /** Constructor */
    protected ProgressView(InstallerUiEswt aInstaller, Composite aParent, String aMsg)
    {
        this(aInstaller, aParent, aMsg, false);
    }

    /** Constructor */
    protected ProgressView(InstallerUiEswt aInstaller, Composite aParent, String aMsg,
                           boolean aIndeterminate)
    {
        super(aInstaller, aParent, 1);
        iMsg = aMsg;
        setTitle(iMsg);
        iLabel = createLabel(iMsg, SWT.WRAP);

        iIndeterminate = aIndeterminate;
        if (iIndeterminate)
        {
            iProgressBar = new ProgressBar
            (getComposite(), SWT.INDETERMINATE | SWT.HORIZONTAL);
        }
        else
        {
            iProgressBar = new ProgressBar
            (getComposite(), SWT.SMOOTH | SWT.HORIZONTAL);
            iProgressBar.setMinimum(0);
            iProgressBar.setMaximum(100);
            iProgressBar.setSelection(iValue);
        }
        iProgressBar.setLayoutData(new GridData(GridData.FILL_HORIZONTAL));

        // To add cancel command to all progress bars,
        // uncomment the following line.
        addCancelCommand();
    }

    /** Update text for this progress bar. */
    public void setText(String aText)
    {
        iMsg = aText;
        // UI updates must be executed in UI thread.
        getComposite().getDisplay().syncExec
        (new Runnable()
        {
            public void run()
            {
                setTitle(iMsg);
                iLabel.setText(iMsg);
                updateSize();
            }
        });
    }

    /** Update value for this progress bar. */
    public void updateProgress(int aValue)
    {
        iValue = aValue;
        if (iIndeterminate)
        {
            // No need to update an indeterminate progress bar.
            return;
        }
        // UI updates must be executed in UI thread.
        getComposite().getDisplay().syncExec
        (new Runnable()
        {
            public void run()
            {
                iProgressBar.setSelection(iValue);
            }
        });
    }

    /**
     * Adds a cancel command for this progress view.
     */
    protected void addCancelCommand()
    {
        if (iCancelCommand != null || isDisposed())
        {
            // Cancel command has already been added, do not add it anymore.
            return;
        }
        // UI updates must be executed in UI thread.
        getDisplay().syncExec
        (new Runnable()
        {
            public void run()
            {
                iEmptyCommand = new Button
                (getCommandComposite(), SWT.PUSH);
                GridData gridData = new GridData(GridData.FILL_HORIZONTAL);
                gridData.horizontalSpan = 1;
                iEmptyCommand.setLayoutData(gridData);
                iEmptyCommand.setEnabled(false);
                addSoftKeyListenerFor(iEmptyCommand);

                iCancelCommand = new Button
                (getCommandComposite(), SWT.PUSH);
                gridData = new GridData(GridData.FILL_HORIZONTAL);
                gridData.horizontalSpan = 1;
                iCancelCommand.setLayoutData(gridData);
                iCancelCommand.setText
                (InstallerUiTexts.get(InstallerUiTexts.CANCEL));
                iCancelCommand.addSelectionListener
                (new SelectionListener()
                {
                    public void widgetDefaultSelected
                    (SelectionEvent aEvent)
                    {
                        widgetSelected(aEvent);
                    }
                    public void widgetSelected
                    (SelectionEvent aEvent)
                    {
                        confirmCancel();
                    }
                });
                addSoftKeyListenerFor(iCancelCommand);
                // Set the Cancel command to be the default command.
                iCancelCommand.setFocus();
                getShell().setDefaultButton(iCancelCommand);

                // Add a KeyListener to handle selection key, LSK and RSK events.
                addViewKeyListenerFor(getContainer());
            }
        });
    }

    /**
     * Removes cancel command for this progress view.
     */
    protected void removeCancelCommand()
    {
        if (iCancelCommand == null || isDisposed())
        {
            // Cancel command does not exist, do nothing.
            return;
        }
        // UI updates must be executed in UI thread.
        getDisplay().syncExec
        (new Runnable()
        {
            public void run()
            {
                getShell().setDefaultButton(null);
                iEmptyCommand.dispose();
                iEmptyCommand = null;
                iCancelCommand.dispose();
                iCancelCommand = null;
            }
        });
    }

    /**
     * This method is called when user cancels the dialog.
     */
    protected void confirmCancel()
    {
        if (iCancelCommand == null)
        {
            // Ignore selection of a non-existing command.
            return;
        }
        if (iInstallerUi != null)
        {
            iInstallerUi.cancel();
        }
    }

    /**
     * Adds KeyListener for given control. This KeyListener handles
     * selection key, LSK and RSK.
     */
    protected void addViewKeyListenerFor(Control aControl)
    {
        if (aControl == null)
        {
            return;
        }
        if (iViewKeyListener == null)
        {
            iViewKeyListener = new KeyListener()
            {
                public void keyPressed(KeyEvent aEvent)
                {
                    switch (aEvent.keyCode)
                    {
                    case SWT.ARROW_UP: // up
                    case SWT.ARROW_DOWN: // down
                    case SWT.ARROW_LEFT: // left
                    case SWT.ARROW_RIGHT: // right
                        // When View has focus and user presses arrow
                        // keys, set focus to the default command button.
                        iCancelCommand.setFocus();
                        break;
                    case -5: // selection key
                        // When View has focus the selection key
                        // does nothing in progress bar view.
                        break;
                    }
                }
                public void keyReleased(KeyEvent aEvent)
                {
                }
            };
        }
        aControl.addKeyListener(iViewKeyListener);
        addSoftKeyListenerFor(aControl);
    }

    /**
     * Adds KeyListener for given control. This KeyListener handles
     * LSK and RSK.
     */
    protected void addSoftKeyListenerFor(Control aControl)
    {
        if (aControl == null)
        {
            return;
        }
        if (iSoftKeyListener == null)
        {
            iSoftKeyListener = new KeyListener()
            {
                public void keyPressed(KeyEvent aEvent)
                {
                    switch (aEvent.keyCode)
                    {
                    case -6: // LSK
                        // Empty command, do nothing.
                        break;
                    case -7: // RSK
                        confirmCancel();
                        break;
                    }
                }
                public void keyReleased(KeyEvent aEvent)
                {
                }
            };
        }
        aControl.addKeyListener(iSoftKeyListener);
    }
}
