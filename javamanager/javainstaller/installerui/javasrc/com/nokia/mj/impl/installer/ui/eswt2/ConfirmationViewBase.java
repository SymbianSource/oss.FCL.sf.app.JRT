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


package com.nokia.mj.impl.installer.ui.eswt2;

import org.eclipse.swt.SWT;
import org.eclipse.swt.events.DisposeEvent;
import org.eclipse.swt.events.DisposeListener;
import org.eclipse.swt.events.FocusEvent;
import org.eclipse.swt.events.FocusListener;
import org.eclipse.swt.events.KeyEvent;
import org.eclipse.swt.events.KeyListener;
import org.eclipse.swt.events.SelectionEvent;
import org.eclipse.swt.events.SelectionListener;
import org.eclipse.swt.graphics.Point;
import org.eclipse.swt.graphics.Rectangle;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Control;
import org.eclipse.swt.widgets.Button;

/**
 * Base class for confirmation views.
 */
abstract public class ConfirmationViewBase extends ViewBase
{
    protected Button iOkCommand = null;
    protected Button iCancelCommand = null;
    private KeyListener iViewKeyListener = null;
    private KeyListener iSoftKeyListener = null;
    private String iOkText = InstallerUiTexts.get(InstallerUiTexts.OK);
    private String iCancelText = InstallerUiTexts.get(InstallerUiTexts.CANCEL);
    private boolean iCancelled = false;
    private boolean iUserHasAnswered = false;
    /** Object for thread synchronization. */
    private Object iSynchObject = new Object();

    /** Constructor */
    protected ConfirmationViewBase()
    {
        super();
    }

    /** Constructor */
    protected ConfirmationViewBase(InstallerUiEswt aInstallerUi, Composite aParent, int aColumns)
    {
        this(aInstallerUi, aParent, aColumns, false);
    }

    /** Constructor */
    protected ConfirmationViewBase(InstallerUiEswt aInstallerUi, Composite aParent, int aColumns, boolean aScrollable)
    {
        super(aInstallerUi, aParent, aColumns, aScrollable);
    }

    /** Constructor */
    protected ConfirmationViewBase(InstallerUiEswt aInstallerUi, Composite aParent, int aColumns, boolean aScrollable, int aComposites)
    {
        super(aInstallerUi, aParent, aColumns, aScrollable, aComposites);
    }

    /**
     * Set user visible command names for OK and Cancel commands.
     * If either parameter is null, then default value is used.
     */
    public void setCommands(String aOkText, String aCancelText)
    {
        iOkText = aOkText;
        iCancelText = aCancelText;
    }

    /**
     * Synchoronous method for asking user confirmation.
     * This method will call createView() before opening the view
     * and getDataFromView() after user has answered.
     */
    public boolean confirm()
    {
        if (isDisposed())
        {
            return false;
        }
        // Open the dialog from the UI thread.
        getComposite().getDisplay().syncExec(new Runnable()
        {
            public void run()
            {
                if (iOkCommand == null && iCancelCommand == null)
                {
                    createView();
                    createCommands();
                    setDefaultCommand();
                }
                iInstallerUi.loadCss();
                setVisible(true);
            }
        });
        // The UI thread must not be blocked. Let's wait for the answer
        // in the client thread.
        synchronized (iSynchObject)
        {
            try
            {
                iSynchObject.wait();
            }
            catch (InterruptedException ie)
            {
                // Ignore silently.
            }
        }
        if (commandDisposed())
        {
            // Widget got disposed, no confirmation available.
            return false;
        }
        // Hide the dialog.
        getComposite().getDisplay().syncExec(new Runnable()
        {
            public void run()
            {
                if (commandDisposed())
                {
                    return;
                }
                getDataFromView();
                setVisible(false);
            }
        });
        // And return the result to the client.
        boolean result = true;
        if (iCancelled)
        {
            result = false;
        }
        return result;
    }

    /**
     * Returns true if user has answered to this dialog.
     */
    public boolean userHasAnswered()
    {
        return iUserHasAnswered;
    }

    /**
     * This method is called once before view is opened.
     * Inheriting class must implement this method.
     */
    abstract protected void createView();

    /**
     * This method is called after user has answered
     * to confirmation.
     * Inheriting class must implement this method.
     */
    abstract protected void getDataFromView();

    /**
     * Creates the commands for confirmation dialogs.
     */
    private void createCommands()
    {
        if (iOkText == null && iCancelText == null)
        {
            return;
        }
        int horizontalSpan = 1;
        if (iOkText == null || iCancelText == null)
        {
            // Only one button, let it fill the whole composite width.
            horizontalSpan = 2;
        }

        if (iOkText != null)
        {
            iOkCommand = new Button(getCommandComposite(), SWT.PUSH);
            if (iCancelText != null)
            {
                setCssId(iOkCommand, "softKeyButton");
            }
            else
            {
                setCssId(iOkCommand, "softKeyButtonWide");
            }
            GridData gridData = new GridData(GridData.FILL_HORIZONTAL);
            gridData.horizontalSpan = horizontalSpan;
            iOkCommand.setLayoutData(gridData);
            iOkCommand.addDisposeListener(new DisposeListener()
            {
                public void widgetDisposed(DisposeEvent aEvent)
                {
                    confirmCancel();
                }
            });
            iOkCommand.addSelectionListener(new SelectionListener()
            {
                public void widgetDefaultSelected(SelectionEvent aEvent)
                {
                    widgetSelected(aEvent);
                }
                public void widgetSelected(SelectionEvent aEvent)
                {
                    confirmOk();
                }
            });
            iOkCommand.addFocusListener(new FocusListener()
            {
                public void focusGained(FocusEvent aEvent)
                {
                    okCommandFocused(aEvent);
                }
                public void focusLost(FocusEvent aEvent)
                {
                    // nop
                }
            });
            if (iOkText != null)
            {
                iOkCommand.setText(iOkText);
            }
            else
            {
                iOkCommand.setEnabled(false);
            }
            addSoftKeyListenerFor(iOkCommand);
        }

        if (iCancelText != null)
        {
            iCancelCommand = new Button(getCommandComposite(), SWT.PUSH);
            if (iOkText != null)
            {
                setCssId(iCancelCommand, "softKeyButton");
            }
            else
            {
                setCssId(iCancelCommand, "softKeyButtonWide");
            }
            GridData gridData = new GridData(GridData.FILL_HORIZONTAL);
            gridData.horizontalSpan = horizontalSpan;
            iCancelCommand.setLayoutData(gridData);
            iCancelCommand.addDisposeListener(new DisposeListener()
            {
                public void widgetDisposed(DisposeEvent aEvent)
                {
                    confirmCancel();
                }
            });
            iCancelCommand.addSelectionListener(new SelectionListener()
            {
                public void widgetDefaultSelected(SelectionEvent aEvent)
                {
                    widgetSelected(aEvent);
                }
                public void widgetSelected(SelectionEvent aEvent)
                {
                    confirmCancel();
                }
            });
            iCancelCommand.addFocusListener(new FocusListener()
            {
                public void focusGained(FocusEvent aEvent)
                {
                    cancelCommandFocused(aEvent);
                }
                public void focusLost(FocusEvent aEvent)
                {
                    // nop
                }
            });
            if (iCancelText != null)
            {
                iCancelCommand.setText(iCancelText);
            }
            else
            {
                iCancelCommand.setEnabled(false);
            }
            addSoftKeyListenerFor(iCancelCommand);
        }

        // Add a KeyListener to handle selection key, LSK and RSK events.
        addViewKeyListenerFor(getContainer());
    }

    /**
     * This method is called when user accepts the dialog.
     */
    protected void confirmOk()
    {
        synchronized (iSynchObject)
        {
            iUserHasAnswered = true;
            iCancelled = false;
            iSynchObject.notify();
        }
    }

    /**
     * This method is called when user cancels the dialog.
     */
    protected void confirmCancel()
    {
        synchronized (iSynchObject)
        {
            iUserHasAnswered = true;
            iCancelled = true;
            iSynchObject.notify();
        }
    }

    /**
     * Called after view and commands have been created. Subclasses
     * can overrride this method to set focus to their own default
     * commands.
     */
    protected void setDefaultCommand()
    {
        if (iOkText != null)
        {
            iOkCommand.setFocus();
            getShell().setDefaultButton(iOkCommand);
        }
        else
        {
            iCancelCommand.setFocus();
            getShell().setDefaultButton(iCancelCommand);
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
                        if (iOkText != null)
                        {
                            iOkCommand.setFocus();
                        }
                        else
                        {
                            iCancelCommand.setFocus();
                        }
                        break;
                    case -5: // selection key
                        selectionKeyPressed(aEvent);
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
                        lskPressed(aEvent);
                        break;
                    case -7: // RSK
                        rskPressed(aEvent);
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

    /**
     * Called when user presses selection key for the Shell.
     */
    protected void selectionKeyPressed(KeyEvent aEvent)
    {
        if (iOkText != null)
        {
            confirmOk();
        }
        else
        {
            confirmCancel();
        }
    }

    /**
     * Called when user presses left soft key.
     */
    protected void lskPressed(KeyEvent aEvent)
    {
        if (iOkText != null)
        {
            confirmOk();
        }
    }

    /**
     * Called when user presses right soft key.
     */
    protected void rskPressed(KeyEvent aEvent)
    {
        if (iCancelText != null)
        {
            confirmCancel();
        }
    }

    /**
     * Called when the OK command gets focus.
     */
    protected void okCommandFocused(FocusEvent aEvent)
    {
    }

    /**
     * Called when the Cancel command gets focus.
     */
    protected void cancelCommandFocused(FocusEvent aEvent)
    {
    }

    /**
     * Returns true if a command from this view has been disposed.
     */
    private boolean commandDisposed()
    {
        if ((iOkCommand != null && iOkCommand.isDisposed()) ||
                (iCancelCommand != null && iCancelCommand.isDisposed()))
        {
            return true;
        }
        return false;
    }
}
