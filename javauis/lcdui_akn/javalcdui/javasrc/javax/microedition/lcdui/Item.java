/*
* Copyright (c) 2007 - 2009 Nokia Corporation and/or its subsidiary(-ies).
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
import com.nokia.mj.impl.rt.legacy.NativeError;
import com.nokia.mj.impl.rt.support.Finalizer;

/**
 * Item is only ever sub-classed, but contains all the common data for
 * any subclasses:
 *
 * iHandle - where the handle to the peer is stashed. It is also
 * removed on finalization. Concrete subclasses should set this.
 *
 * iToolkit - can be used by all subclasses for invoking server side
 * methods etc.
 *
 */
abstract public class Item
{
    public final static int LAYOUT_DEFAULT = 0;
    public final static int LAYOUT_LEFT = 1;
    public final static int LAYOUT_RIGHT = 2;
    public final static int LAYOUT_CENTER = 3;
    //
    public final static int LAYOUT_TOP = 0x10;
    public final static int LAYOUT_BOTTOM = 0x20;
    public final static int LAYOUT_VCENTER = 0x30;
    //
    public final static int LAYOUT_NEWLINE_BEFORE = 0x100;
    public final static int LAYOUT_NEWLINE_AFTER = 0x200;
    public final static int LAYOUT_SHRINK = 0x400;
    public final static int LAYOUT_EXPAND = 0x800;
    public final static int LAYOUT_VSHRINK = 0x1000;
    public final static int LAYOUT_VEXPAND = 0x2000;
    public final static int LAYOUT_2 = 0x4000;
    //
    public final static int PLAIN = 0;
    public final static int HYPERLINK = 1;
    public final static int BUTTON = 2;
    //
    final Toolkit iToolkit;
    //
    int iHandle;
    int iLayout; // Default to LAYOUT_DEFAULT value of 0
    int iAppearance; // Default to PLAIN
    boolean iSizeLocked;
    //
    String      iLabel;
    Screen      iScreen;
    CommandSet  iCommands;
    Command     iDefaultCommand;
    ItemCommandListener iListener;
    Object iCallbackLock;

    private Finalizer mFinalizer = new Finalizer()
    {
        public void finalizeImpl()
        {
            doFinalize();
        }
    };

    Item()
    {
        iToolkit = Toolkit.getToolkit();
        iCallbackLock = iToolkit.getCallbackLock();
        iCommands = new CommandSet(iToolkit);
    }

    public void setLabel(String aLabel)
    {
        synchronized (iToolkit)
        {
            checkAlert();
            NativeError.check(_setLabel(getHandle(),iToolkit.getHandle(),aLabel == null?"":aLabel));
            iLabel = aLabel;
            RefreshForm();
        }
    }

    public String getLabel()
    {
        return iLabel;
    }

    public int getLayout()
    {
        return iLayout;
    }

    public void setLayout(int aLayout)
    {
        checkLayout(aLayout);
        synchronized (iToolkit)
        {
            checkAlert();
            NativeError.check(_setLayout(getHandle(),iToolkit.getHandle(),aLayout));
            iLayout = aLayout;
            RefreshForm();
        }
    }

    public void addCommand(Command aCommand)
    {
        synchronized (iToolkit)
        {
            checkAlert();
            DoAddCommand(aCommand);
        }
    }

    public void removeCommand(Command aCommand)
    {
        if (null == aCommand)
        {
            return;
        }
        synchronized (iToolkit)
        {
            CommandPeer peer = iCommands.getPeer(aCommand);
            if (null != peer)
            {
                if (iDefaultCommand == aCommand)
                {
                    DoSetDefaultCommand(null);
                }
                RemoveCommandPeer(peer);
                iCommands.remove(peer);
            }
        }
    }

    //
    // Remove all commands from peer - could be used at removeNotfy or hideNotify time
    //
    void ClearCommands()
    {
        Enumeration e = iCommands.elements();
        while (e.hasMoreElements())
        {
            RemoveCommandPeer((CommandPeer)e.nextElement());
        }
    }

    //
    // Adds all commands to peer - could be used at addNotify or showNotify time
    //
    void UpdateCommands()
    {
        if (null == iScreen)
        {
            return;
        }
        ClearCommands();
        Enumeration e = iCommands.elements();
        while (e.hasMoreElements())
        {
            CommandPeer peer = (CommandPeer)e.nextElement();
            NativeError.check(_addCommand(getHandle(), iToolkit.getHandle(), peer.getHandle()));
        }
    }

    // used by addCommand/SetDefaultCommand
    void DoAddCommand(Command aCommand)
    {
        CommandPeer peer = iCommands.add(aCommand); // throws NPE
        if (null != peer)
        {
            NativeError.check(_addCommand(getHandle(), iToolkit.getHandle(), peer.getHandle()));
        }
    }

    void RemoveCommandPeer(CommandPeer aPeer)
    {
        NativeError.check(_removeCommand(getHandle(), iToolkit.getHandle(), aPeer.getHandle()));
    }

    public void setItemCommandListener(ItemCommandListener aListener)
    {
        synchronized (iToolkit)
        {
            checkAlert();
            iListener = aListener;
        }
    }

    public int getPreferredWidth()
    {
        synchronized (iToolkit)
        {
            return _getPreferredSizeWidth(getHandle(),iToolkit.getHandle());
        }
    }

    public int getPreferredHeight()
    {
        synchronized (iToolkit)
        {
            return _getPreferredSizeHeight(getHandle(),iToolkit.getHandle());
        }
    }

    public void setPreferredSize(int aWidth,int aHeight)
    {
        if (aWidth < -1 || aHeight < -1)
        {
            throw new IllegalArgumentException();
        }
        synchronized (iToolkit)
        {
            checkAlert();
            NativeError.check(_setPreferredSize(getHandle(),iToolkit.getHandle(),aWidth,aHeight));
            iSizeLocked = (aWidth != -1 || aHeight != -1);
            RefreshForm();
        }
    }

    public int getMinimumWidth()
    {
        synchronized (iToolkit)
        {
            return _getMinimumSizeWidth(getHandle(),iToolkit.getHandle());
        }
    }

    public int getMinimumHeight()
    {
        synchronized (iToolkit)
        {
            return _getMinimumSizeHeight(getHandle(),iToolkit.getHandle());
        }
    }

    public void setDefaultCommand(Command aCommand)
    {
        synchronized (iToolkit)
        {
            checkAlert();
            if (null != aCommand)
            {
                DoAddCommand(aCommand);
            }
            DoSetDefaultCommand(aCommand);
        }
    }

    final void DoSetDefaultCommand(Command aCommand)
    {
        int command = 0;
        if (null != aCommand)
        {
            CommandPeer peer = iCommands.getPeer(aCommand); // throws NPE
            if (null != peer)
            {
                command = peer.getHandle();
            }
        }
        _setDefaultCommand(getHandle(), iToolkit.getHandle(), command);
        iDefaultCommand = aCommand;
    }

    /**
     * Need to go native to serialize with event queue
     * (and be non-blocking)
     */
    public void notifyStateChanged()
    {
        Form form = getForm();
        if (form == null)
        {
            throw new IllegalStateException();
        }
        form.notifyStateChanged(this);
    }

    /**
     * Used in ImageItem, hence not private
     */
    final void checkLayout(int aLayout)
    {
        final int layoutMask = LAYOUT_DEFAULT |
                               LAYOUT_LEFT | LAYOUT_RIGHT | LAYOUT_CENTER |
                               LAYOUT_TOP | LAYOUT_BOTTOM | LAYOUT_VCENTER |
                               LAYOUT_NEWLINE_BEFORE | LAYOUT_NEWLINE_AFTER |
                               LAYOUT_SHRINK | LAYOUT_EXPAND | LAYOUT_VSHRINK | LAYOUT_VEXPAND |
                               LAYOUT_2;
        //
        if (((aLayout | layoutMask) > layoutMask) || (aLayout < 0))
        {
            //illegal bits have been set
            throw new IllegalArgumentException();
        }
    }

    /**
     * To be overidded in subclasses.
     * Gets invoked whenever this item is made visible.
     */
    void handleVisible(boolean aVisible)
    {
    }

    /**
     * Called in Form
     */
    void addNotify(Screen aScreen)
    {
        if (null == aScreen)
        {
            throw new NullPointerException("addNotify from null screen?!");
        }
        synchronized (iToolkit)
        {
            if (null != iScreen)
            {
                throw new IllegalStateException("addNotify when already added");
            }
            CheckToolkit(aScreen);
            iScreen = aScreen;
            UpdateCommands();
        }
    }

    void removeNotify(Screen aScreen)
    {
        synchronized (iToolkit)
        {
            if (iScreen != aScreen)
            {
                throw new RuntimeException("removeNotify from wrong screen");
            }
            iScreen = null;
            ClearCommands();
        }
    }

    /**
     * NOTE:
     *
     * Spec does not specify behaviour if an item created by
     * one midlet in a suite is used by another midlet in the
     * same suite. Current LCDUI implementation cannot handle
     * that case so we throw RuntimeException() here.
     *
     */
    private void CheckToolkit(Displayable aDisplayable)
    {
        if (aDisplayable.iToolkit != iToolkit)
        {
            final String msg = "ERROR - toolkit mismatch";
            throw new RuntimeException(msg);
        }
    }

    /**
     * Call this when the contents of an item have changed
     * and the form needs to be redrawn to display the modified item
     */
    final void RefreshForm()
    {
        Form form = getForm();
        if (form != null)
        {
            form.Refresh(this);
        }
    }

    /**
     * More drastic than refreshForm(),e.g. when the native peer has changed
     */
    final void UpdateForm()
    {
        Form form = getForm();
        if (form != null)
        {
            form.Update(this);
        }
    }

    void handleEvent(int aEvent, int aParam0, int aParam1, int aParam2)
    {
        if (aEvent == Toolkit.EVENT_COMMAND)
        {
            final ItemCommandListener listener;
            final Command command;
            final Screen  screen;

            synchronized (iToolkit)
            {
                screen   = iScreen;
                listener = iListener;
                command  = iCommands.getCommandById(aParam0);
            }

            if ((null != screen) && (screen.isShown()) && (null != listener) && (null != command))
            {
                synchronized (iCallbackLock)
                {
                    listener.commandAction(command, this);
                }
            }
        }
        else
        {
            throw new IllegalArgumentException("unexpected event: " + aEvent);
        }
    }

    final Form getForm()
    {
        Screen screen = iScreen;
        if (screen instanceof Form)
        {
            return (Form)screen;
        }
        return null;
    }

    private void doFinalize()
    {
        if (mFinalizer != null)
        {
            registeredFinalize();
            mFinalizer = null;
        }
    }

    /**
     * Removes native Item peer for all items.
     */
    private void registeredFinalize()
    {
        synchronized (iToolkit)
        {
            iToolkit.disposeObject(iHandle);
            iHandle=0;
        }
    }

    int getHandle()
    {
        if (iHandle <= 0)
        {
            throw new RuntimeException("Item.getHandle("+this+") dead item");
        }
        return iHandle;
    }

    private void checkAlert()
    {
        Screen screen = iScreen;
        if ((null != screen) && (screen instanceof Alert))
        {
            throw new IllegalStateException();
        }
    }

    private native int _setLabel(int aItem,int aToolkit,String aLabel);
    private native int _setLayout(int aItem,int aToolkit,int aLayout);
    private native int _getMinimumSizeHeight(int aItem,int aToolkit);
    private native int _getMinimumSizeWidth(int aItem,int aToolkit);
    private native int _getPreferredSizeHeight(int aItem,int aToolkit);
    private native int _getPreferredSizeWidth(int aItem,int aToolkit);
    private native int _setPreferredSize(int aItem,int aToolkit,int aWidth,int aHeight);
    private native int _addCommand(int aItem, int aToolkit, int aCommand);
    private native int _removeCommand(int aItem, int aToolkit, int aCommand);
    private native void _setDefaultCommand(int aItem, int aToolkit, int aCommand);
}

