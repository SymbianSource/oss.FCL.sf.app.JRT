/*
* Copyright (c) 2009, 2010 Nokia Corporation and/or its subsidiary(-ies).
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
package javax.microedition.lcdui;

import java.util.Enumeration;
import java.util.Vector;

import javax.microedition.lcdui.EventDispatcher.LCDUIEvent;

import org.eclipse.swt.SWT;
import org.eclipse.swt.events.*;
import org.eclipse.swt.graphics.Rectangle;
import org.eclipse.swt.internal.extension.CompositeExtension;
import org.eclipse.swt.internal.extension.MobileShellExtension;
import org.eclipse.swt.widgets.*;
import com.nokia.mj.impl.rt.support.ApplicationUtils;
import com.nokia.mj.impl.rt.support.ApplicationInfo;

/**
 * Implementation of LCDUI <code>Displayable</code> class.
 */
public abstract class Displayable
{

    private EswtCommandListener eswtCommandListener = new EswtCommandListener();

    private EswtShellListener eswtShellListener = new EswtShellListener();

    private EswtControlListener eswtControlListener = new EswtControlListener();

    private EswtDisposeListener eswtDisposeListener = new EswtDisposeListener();

    /**
     * Are the commands enabled or not.
     */
    private boolean isEnabledCmds = true;

    /**
     * Shell is activated/de-activated. Called by shell listener.
     */
    private boolean isShellActive = true;

    /**
     * Visible from OpenLCDUI's point of view. Called by Display.setCurrent().
     */
    private boolean isLcduiVisible;

    /**
     * Owned mobile shell.
     */
    private Shell shell;

    /**
     * Content composite.
     */
    private Composite contentComp;

    private Rectangle contentArea;

    private boolean initialized;

    private boolean isShownReturnValue;

    private com.nokia.mj.impl.rt.support.Finalizer finalizer;

    private String title;

    private Vector commands = new Vector();

    private CommandListener iCommandListener;

    private Ticker ticker;

    /**
     * eSWT Label which is used to display the Ticker. This is stored in
     * displayable because same Ticker may exists in many displayables but eSWT
     * Controls are always associated with only one Displayable.
     */
    private Label tickerLabel;

    /**
     * Default Constructor.
     */
    Displayable(String title)
    {
        this.title = title;
        finalizer = ((finalizer != null) ? finalizer
                     : new com.nokia.mj.impl.rt.support.Finalizer()
        {
            public void finalizeImpl()
            {
                if(finalizer != null)
                {
                    finalizer = null;
                    if(!ESWTUIThreadRunner.isDisposed())
                    {
                        dispose();
                    }
                }
            }
        });
        ESWTUIThreadRunner.update(getClass().getName(), 1);
    }

    /**
     * Performs eSWT construction of shell and content composite. <br>
     * Should be called from child level constructors.
     */
    final void construct()
    {
        ESWTUIThreadRunner.safeSyncExec(new Runnable()
        {
            public void run()
            {
                shell = eswtConstructShell(SWT.SHELL_TRIM | SWT.PRIMARY_MODAL);
                eswtSetTitle();
                contentComp = eswtConstructContent(SWT.NONE);
                contentArea = eswtLayoutShellContent();
                eswtInitGraphics();
            }
        });
    }

    /**
     * Dispose Displayable.
     */
    void dispose()
    {
        if(ticker != null)
        {
            ticker.removeLabel(tickerLabel);
        }
        ESWTUIThreadRunner.update(getClass().getName(), -1);
        ESWTUIThreadRunner.safeSyncExec(new Runnable()
        {
            public void run()
            {
                if(shell != null)
                {
                    shell.dispose();
                }
            }
        });
    }

    /**
     * Constructs default eSWT Shell.<br>
     * Default SWT shell style is SWT.SYSTEM_MODAL
     *
     * @param style eSWT style
     *
     * @return eSWT shell
     */
    Shell eswtConstructShell(int style)
    {
        return new MobileShellExtension(ESWTUIThreadRunner.getInstance().getDisplay(), style);
    }

    /**
     * Creates content Composite. this Composite is placed inside of
     * shell and contains the actual displayable's content (excluding ticker).
     *
     * Child classes may override this is if for example scrollbar is needed.
     *
     * @return Composite where the content is placed.
     */
    Composite eswtConstructContent(int style)
    {
        Composite comp = new CompositeExtension(shell, style);
        return comp;
    }

    /**
     * Initializes the Displayable for Graphics drawing. 
     */
    void eswtInitGraphics() {
    
    }
    
    /**
     * Called by Display when Displayable should become visible.
     */
    void eswtHandleShowCurrentEvent()
    {
        if(!shell.isDisposed())
        {
            eswtUpdateSizes();
            if(ticker != null)
            {
                ticker.start();
            }
            shell.addShellListener(eswtShellListener);
            shell.addDisposeListener(eswtDisposeListener);
            shell.addControlListener(eswtControlListener);
            eswtAddSelectionListenerForCommands();
            // calling open() causes a resize event to be sent
            shell.open();
            isLcduiVisible = true;
            // shell.setVisible(true);
            // TODO: needed because of eSWT focus bug
            /*if (!isDialogShell()) {
                Shell[] shells =  shell.getDisplay().getShells();
                for (int i = 0; i < shells.length; i++) {
                    if (shells[i] != shell && shells[i].isVisible()) {
                        shells[i].setVisible(false);
                    }
                }
            }*/
        }
    }

    /**
     * Called by Display when Displayable should become hidden.
     */
    void eswtHandleHideCurrentEvent()
    {
        isLcduiVisible = false;
        if(!shell.isDisposed())
        {
            shell.removeShellListener(eswtShellListener);
            shell.removeDisposeListener(eswtDisposeListener);
            shell.removeControlListener(eswtControlListener);
            eswtRemoveSelectionListenerForCommands();
        }
    }

    /**
     * Handle event.
     *
     * @param e eSWT event
     */
    void eswtHandleEvent(Event e)
    {
        // implementation in child classes
        // Logger.method(this, "eswtHandleEvent", e);
    }

    /**
     * Called by ShellListener when shell gets activated.
     */
    void handleShellActivatedEvent()
    {
        // Implementation in child-classes
        // Logger.method(this, "handleShellActivatedEvent");
        if(ESWTUIThreadRunner.getInstance().getDisplay().getActiveShell()
                != null)
        {
            if(JadAttributeUtil.isValue(JadAttributeUtil.ATTRIB_NOKIA_MIDLET_BACKGROUND_EVENT,
                                        JadAttributeUtil.VALUE_PAUSE))
            {
                ApplicationUtils.getInstance().resumeApplication();
            }
            isShellActive = true;
        }
    }

    /**
     * Called by ShellListener when shell gets de-activated.
     */
    void handleShellDeActivatedEvent()
    {
        // Implementation in child-classes
        // Logger.method(this, "handleShellDeActivatedEvent");
        if(isShellActive)
        {
            if(ESWTUIThreadRunner.getInstance().getDisplay().getActiveShell()
                    == null)
            {
                if(JadAttributeUtil.isValue(JadAttributeUtil.ATTRIB_NOKIA_MIDLET_BACKGROUND_EVENT,
                                            JadAttributeUtil.VALUE_PAUSE))
                {
                    ApplicationUtils.getInstance().pauseApplication();
                    isShellActive = false;
                }
            }
        }
    }

    /**
     * This is called if resolution or orientation was changed. This should be
     * overwritten by sub-classes to get notifications about size changes.
     *
     * @param width new width of Displayable.
     * @param height new height of Displayable.
     */
    void eswtHandleResizeEvent(int width, int height)
    {
        Logger.method(this, "eswtHandleResizeEvent",
                      String.valueOf(width), String.valueOf(height));
        LCDUIEvent event = EventDispatcher.instance().newEvent(LCDUIEvent.DISPLAYABLE_SIZECHANGED, this);
        event.width = width;
        event.height = height;
        EventDispatcher.instance().postEvent(event);
    }

    /**
     * Returns if the shell is Dialog styled.
     */
    private boolean isDialogShell()
    {
        return (shell.getStyle() & SWT.DIALOG_TRIM) == SWT.DIALOG_TRIM;
    }

    /**
     * Set content size.
     *
     * @param aWidth required width or -1 to ignore
     * @param aHeight required height or -1 to ignore
     */
    void eswtSetPreferredContentSize(int aWidth, int aHeight)
    {
        if(isDialogShell())
        {
            // aHeight += Config.DISPLAYABLE_DIALOGSHELL_HEIGHT_DISPLACEMENT;

            Logger.method(this, "eswtSetPreferredContentSize",
                          String.valueOf(aWidth), String.valueOf(aHeight));

            Rectangle contentBounds = contentComp.getBounds();
            int newWidth = (aWidth > 0 ? aWidth : contentBounds.width);
            int newHeight = (aHeight > 0 ? aHeight : contentBounds.height);

            if(tickerLabel != null)
            {
                newHeight += tickerLabel.getBounds().height;
            }

            Rectangle shellBounds = shell.getBounds();
            // compute the trimmed shell size
            Rectangle newSize = shell.computeTrim(0, 0, newWidth, newHeight);
            // set the new size
            shell.setSize(newSize.width, newSize.height);
            // set the location - attached to the bottom growing upwards
            shell.setLocation(shellBounds.x, (shellBounds.y + shellBounds.height) - newSize.height);
        }
    }

    Rectangle eswtLayoutShellContent()
    {
        Rectangle shellArea = shell.getClientArea();
        if(tickerLabel != null)
        {
            int tickerHeight = tickerLabel.getBounds().height;

            contentComp.setBounds(0, tickerHeight,
                                  shellArea.width, shellArea.height - tickerHeight);
        }
        else
        {
            contentComp.setBounds(0, 0, shellArea.width, shellArea.height);
        }
        return contentComp.getClientArea();
    }

    /**
     * Update internal size of Displayable.
     */
    void eswtUpdateSizes()
    {
        Rectangle newArea = eswtLayoutShellContent();
        // if the content size has changed or its not initialized
        if(!initialized
                || newArea.width != contentArea.width
                || newArea.height != contentArea.height)
        {
            contentArea = newArea;
            initialized = true;
            if(ticker != null)
            {
                ticker.updateSpeed();
            }
            eswtHandleResizeEvent(contentArea.width, contentArea.height);
        }
    }

    /**
     * Tells is this Displayable visible.
     *
     * @return true if this Displayable is currently visible.
     */
    public synchronized boolean isShown()
    {
        ESWTUIThreadRunner.syncExec(new Runnable()
        {
            public void run()
            {
                isShownReturnValue = eswtIsShown();
            }
        });
        return isShownReturnValue;
    }

    /**
     * eSWT call-back that verifies that the Displayable is shown.
     */
    boolean eswtIsShown()
    {
        if(!isLcduiVisible || !isShellActive)
        {
            // shell.isVisible() doesn't return false if MIDlet
            // is in background. That's why isVisible-variable is
            // used instead.
            return false;
        }
        if(shell.isDisposed() || !shell.isEnabled())
        {
            return false;
        }
        return true;
    }

    /**
     * Validates a Command.
     *
     * @param command a Command
     */
    void validateCommand(Command command)
    {
        if(command == null)
        {
            throw new NullPointerException(
                MsgRepository.DISPLAYABLE_EXCEPTION_NULL_PARAMETER);
        }
    }

    /**
     * Set commands visibility.
     *
     * @param enableCmds visibility switch
     */
    void setCommandsVisibility(boolean enableCmds)
    {
        if(enableCmds != isEnabledCmds)
        {
            isEnabledCmds = enableCmds;
            ESWTUIThreadRunner.syncExec(new Runnable()
            {
                public void run()
                {
                    int numCmd = getNumCommands();
                    for(int i = 0; i < numCmd; i++)
                    {
                        if(isEnabledCmds)
                        {
                            eswtAddCommand(getCommand(i));
                        }
                        else
                        {
                            eswtRemoveCommand(getCommand(i));
                        }
                    }
                }
            });
        }
    }

    /**
     * Adds a command to this Displayable.
     *
     * @param command The Command to be added. If the Command already is added
     *            to this Displayable, nothing happens.
     * @throws NullPointerException If parameter is null.
     */
    public void addCommand(Command command)
    {
        validateCommand(command);
        if(!commands.contains(command))
        {
            commands.addElement(command);
            // Command is not yet added to this Displayable.
            if(isEnabledCmds)
            {
                final Command finalCommand = command;
                ESWTUIThreadRunner.syncExec(new Runnable()
                {
                    public void run()
                    {
                        eswtAddCommand(finalCommand);
                    }
                });
            }
        }
    }

    /**
     * eSWT callback to add a Command.
     */
    void eswtAddCommand(Command cmd)
    {
        cmd.eswtAddESWTCommand(shell, false);
        if(eswtIsShown())
        {
            cmd.eswtAddCommandSelectionListener(shell, eswtCommandListener);
        }
    }

    /**
     * Removes command from the Displayable.
     *
     * @param command Command to be removed. If parameter is null or Command
     *            isn't added to Displayable, nothing happens.
     */
    public void removeCommand(Command command)
    {
        if(command != null && commands.contains(command))
        {
            // Remove command from iCommands-vector
            commands.removeElement(command);
            if(isEnabledCmds)
            {
                final Command finalCommand = command;
                ESWTUIThreadRunner.syncExec(new Runnable()
                {
                    public void run()
                    {
                        eswtRemoveCommand(finalCommand);
                    }
                });
            }
        }
    }

    /**
     * eSWT callback to remove a Command.
     */
    void eswtRemoveCommand(Command cmd)
    {
        if(eswtIsShown())
        {
            cmd.eswtRemoveCommandSelectionListener(shell, eswtCommandListener);
        }
        cmd.eswtRemoveESWTCommand(shell);
    }

    /**
     * Sets CommandListener. If CommandListener already exists, it is replaced
     * with the new one.
     *
     * @param commandListener New CommandListener. If null, existing
     *            CommandListener is removed. If null and no CommandListener
     *            exists, nothing happens.
     */
    public void setCommandListener(CommandListener commandListener)
    {
        this.iCommandListener = commandListener;
    }

    public boolean hasCommandListener()
    {
        if(this.iCommandListener != null)
        {
            return true;
        }
        return false;
    }


    /**
     * Add command listener for all Commands added to this Displayable.
     */
    void eswtAddSelectionListenerForCommands()
    {
        Command cmd = null;
        for(Enumeration e = commands.elements(); e.hasMoreElements();)
        {
            cmd = (Command) e.nextElement();
            cmd.eswtAddCommandSelectionListener(shell, eswtCommandListener);
        }
    }

    /**
     * Remove command listener from Commands added to this Displayable.
     */
    void eswtRemoveSelectionListenerForCommands()
    {
        Command cmd = null;
        for(Enumeration e = commands.elements(); e.hasMoreElements();)
        {
            cmd = (Command) e.nextElement();
            cmd.eswtRemoveCommandSelectionListener(shell, eswtCommandListener);
        }
    }

    /**
     * Calls the command action on the current command listener.
     *
     * @param command the Command
     */
    final void callCommandAction(Command command)
    {
        if(iCommandListener != null && command != null)
        {
            LCDUIEvent event = EventDispatcher.instance().newEvent(LCDUIEvent.DISPLAYABLE_COMMANDACTION, this);
            event.command = command;
            event.commandListener = iCommandListener;
            EventDispatcher.instance().postEvent(event);
        }
    }

    /**
     * Gets the number of commands.
     *
     * @return the number of commands in this Displayable
     */
    final int getNumCommands()
    {
        return commands.size();
    }

    /**
     * Gets a command in the command array.
     *
     * @param index index of command
     * @return the command
     */
    final Command getCommand(int index)
    {
        return (Command) commands.elementAt(index);
    }

    /**
     * Gets the commands array.
     *
     * @return the commands
     */
    Vector getCommands()
    {
        return commands;
    }

    /**
     * Gets the command listener.
     *
     * @return the eswt command listener.
     */
    final EswtCommandListener getCommandListener()
    {
        return eswtCommandListener;
    }

    /**
     * Gets width.
     *
     * @return Width of the Displayable in pixels.
     */
    public int getWidth()
    {
        return contentArea.width;
    }

    /**
     * Gets height.
     *
     * @return Height of the Displayable in pixels.
     */
    public int getHeight()
    {
        return contentArea.height;
    }

    /**
     * Sets ticker. If ticker is added already to other displayable(s),
     * it continues running from position where it was. Otherwise
     * it will start running from beginning when this method returns.
     *
     * @param newTicker New ticker. If null, current ticker is removed.
     */
    public void setTicker(Ticker newTicker)
    {
        if(ticker == newTicker)
        {
            return;
        }
        if(ticker != null)
        {
            // Ticker exists, removing it:
            ticker.removeLabel(getTickerLabel());
        }
        ticker = newTicker;
        if(ticker != null)
        {
            ticker.addLabel(getTickerLabel());
        }

        final Ticker finalTicker = ticker;
        ESWTUIThreadRunner.syncExec(new Runnable()
        {
            public void run()
            {
                if(finalTicker != null)
                {
                    // Setting ticker:
                    tickerLabel.setText(finalTicker.getFormattedString());
                    tickerLabel.pack();
                    // Avoid ticker flashing by setting it out of the
                    // screen first:
                    tickerLabel.setBounds(Integer.MIN_VALUE, 0,
                                          tickerLabel.getBounds().width,
                                          tickerLabel.getBounds().height);
                }
                else
                {
                    // Removing ticker:
                    tickerLabel.setText("");
                    tickerLabel.setBounds(Integer.MIN_VALUE, 0, 0, 0);
                }
                eswtUpdateSizes();
            }
        });
        if(ticker != null)
        {
            if(isLcduiVisible)
            {
                // Start to scroll the ticker. Ticker may be already running
                // if it exists in some other displayable already, but
                // calling this again wont do any harm:
                ticker.start();
            }
        }
    }

    /**
     * Gets current ticker.
     *
     * @return Current ticker or null if no ticker set.
     */
    public Ticker getTicker()
    {
        return ticker;
    }

    /**
     * Gets the current title.
     *
     * @return The title of the Displayable, or null if no title set.
     */
    public String getTitle()
    {
        return title;
    }

    /**
     * Sets the title of this Displayable.
     *
     * @param newTitle new title or null for no title.
     */
    public void setTitle(String newTitle)
    {
        this.title = newTitle;
        ESWTUIThreadRunner.syncExec(new Runnable()
        {
            public void run()
            {
                eswtSetTitle();
            }
        });
    }

    /**
     * Sets shell's title. Nulls are not allowed.
     */
    void eswtSetTitle()
    {
        // eSWT Shell doesn't take null value as title
        shell.setText((title != null ? title : ""));
    }

    /**
     * Creates singleton Label instance used by Ticker.
     */
    Label getTickerLabel()
    {
        if(tickerLabel == null)
        {
            ESWTUIThreadRunner.syncExec(new Runnable()
            {
                public void run()
                {
                    tickerLabel = new Label(shell,
                                            SWT.SHADOW_NONE | SWT.HORIZONTAL | SWT.CENTER);
                }
            });
        }
        return tickerLabel;
    }

    /**
     * Called by underlying system when the size of the Displayable changes.
     * This might be overwritten by user-side classes.
     *
     * @param width new width of Displayable.
     * @param height new height of Displayable.
     */
    protected void sizeChanged(int width, int height)
    {
    }

    /**
     * eSWT callback to get the Shell of the Displayable.
     */
    final Shell getShell()
    {
        return this.shell;
    }

    /**
     * Gets composite that contains displayable's content.
     *
     * @return Composite.
     */
    Composite getContentComp()
    {
        return contentComp;
    }

    /*
     * The client area. It's ensured that after the construction this is always
     * set.
     */
    final Rectangle getContentArea()
    {
        return contentArea;
    }

    /*
     * Dispatcher thread calls.
     */
    void doCallback(LCDUIEvent event)
    {
        switch(event.type)
        {
        case LCDUIEvent.DISPLAYABLE_SIZECHANGED:
            sizeChanged(event.width, event.height);
            break;
        case LCDUIEvent.DISPLAYABLE_COMMANDACTION:
            event.commandListener.commandAction(event.command, this);
            break;
        }
    }

    /**
     * Inner class which receives SelectionEvents from eSWT and convert and
     * forwards those events to LCDUI's CommandListener.
     */
    class EswtCommandListener implements SelectionListener
    {

        public void widgetDefaultSelected(SelectionEvent e)
        {
        }

        /**
         * Executed by eSWT when event occurs. This method will then call
         * Displayable's CommandListener if event source matches with the
         * Commands added to the Displayable.
         */
        public void widgetSelected(SelectionEvent event)
        {
            // Go through all Commands added to this Displayable:
            for(Enumeration e = commands.elements(); e.hasMoreElements();)
            {
                Command cmd = (Command) e.nextElement();
                // Select eSWT Command from Command which is connected to
                // this Displayable and compare it to the widget which
                // launched the event:
                if(cmd.getESWTCommand(shell) == event.widget)
                {
                    callCommandAction(cmd);
                    break;
                }
            }
        }
    }

    /**
     * Every Displayable must listen shell events to be able to tell is the
     * MIDlet sent to background.
     */
    class EswtShellListener implements ShellListener
    {

        public void shellActivated(ShellEvent e)
        {
            ESWTUIThreadRunner.getInstance().getDisplay()
            .asyncExec(new Runnable()
            {
                public void run()
                {
                    handleShellActivatedEvent();
                }
            });
        }

        public void shellDeactivated(ShellEvent e)
        {
            ESWTUIThreadRunner.getInstance().getDisplay()
            .asyncExec(new Runnable()
            {
                public void run()
                {
                    handleShellDeActivatedEvent();
                }
            });

        }

        public void shellClosed(ShellEvent e)
        {
        }

        public void shellIconified(ShellEvent e)
        {
        }

        public void shellDeiconified(ShellEvent e)
        {
        }

    }

    class EswtDisposeListener implements DisposeListener
    {

        public void widgetDisposed(DisposeEvent e)
        {
            isShellActive = false;
        }
    }

    /**
     * Displayable must listen resize-events to be able to call
     * sizeChanged()-method at the right time.
     */
    class EswtControlListener implements ControlListener
    {

        public void controlResized(ControlEvent e)
        {
            eswtUpdateSizes();
        }

        public void controlMoved(ControlEvent e)
        {
        }
    }

}
