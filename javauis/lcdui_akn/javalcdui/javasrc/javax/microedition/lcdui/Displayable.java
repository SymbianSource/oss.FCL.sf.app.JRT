/*
* Copyright (c) 1999 - 2004 Nokia Corporation and/or its subsidiary(-ies).
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

import java.util.Vector;
import com.nokia.mj.impl.rt.legacy.NativeError;
import com.nokia.mj.impl.rt.support.Finalizer;

abstract public class Displayable
{
    final Toolkit iToolkit;
    final Object  iCallbackLock;

    // Peered state
    int     iContainer;
    int     iContent;

    Display         iDisplay;
    int             iWidth;
    int             iHeight;

    //
    CommandListener iListener;
    CommandSet      iCommands;
    private Ticker iTicker;
    private String iTitle;

    private Finalizer mFinalizer;

    Displayable()
    {
        iToolkit = Toolkit.getToolkit();
        synchronized (iToolkit)
        {
            iCallbackLock = iToolkit.getCallbackLock();
            iCommands     = new CommandSet(iToolkit);
            final int toolkit = iToolkit.getHandle();
            iContainer = Toolkit.checkHandle(_createContainer(toolkit));
            _setCommandListenerExistence(getContainerHandle(), iToolkit.getHandle(), false);
        }

        mFinalizer = new Finalizer()
        {
            public void finalizeImpl()
            {
                doFinalize();
            }
        };
    }

    public boolean isShown()
    {
        synchronized (iToolkit)
        {
            return IsShown();
        }
    }

    final boolean IsShown()
    {
        Display display = iDisplay;
        return (null != display ? display.IsShown(this) : false);
    }

    public void addCommand(Command aCommand)
    {
        synchronized (iToolkit)
        {
            DoAddCommand(aCommand);
        }
    }

    public void removeCommand(Command aCommand)
    {
        synchronized (iToolkit)
        {
            DoRemoveCommand(aCommand);
        }
    }

    int DoAddCommand(Command aCommand)
    {
        CommandPeer peer = iCommands.add(aCommand);
        if (null != peer)
        {
            NativeError.check(_addCommand(getContainerHandle(), iToolkit.getHandle(), peer.getHandle()));
        }
        return iCommands.count();
    }

    int DoRemoveCommand(Command aCommand)
    {
        if (null != aCommand)
        {
            CommandPeer peer = iCommands.getPeer(aCommand);
            if (null != peer)
            {
                _removeCommand(iToolkit.getHandle(), getContainerHandle(), peer.getHandle());
                iCommands.remove(peer);
            }
        }
        return iCommands.count();
    }

    public void setCommandListener(CommandListener aListener)
    {
        synchronized (iToolkit)
        {
            iListener = aListener;
            _setCommandListenerExistence(getContainerHandle(), iToolkit.getHandle(), null != aListener);
        }
    }

    /**
     * Distributes commands to listener
     */
    final void commandEvent(int aCommandId)
    {
        Command command = null;
        synchronized (iToolkit)
        {
            command=iCommands.getCommandById(aCommandId);
        }
        synchronized (iCallbackLock)
        {
            callCommandAction(command);
        }
    }

    final void callCommandAction(Command aCommand)
    {
        if (null == aCommand)
        {
            return;
        }

        if (!isShown())
        {
            return;
        }

        CommandListener listener = null;
        synchronized (iToolkit)
        {
            listener = iListener;
        }

        if (listener == null)
        {
            return;
        }

        listener.commandAction(aCommand, this);
    }

    /**
     * To be overidded in subclasses. Called with events posted from native event server
     */
    void handleEvent(int aEvent,int aData0, int aData1)
    {
        switch (aEvent)
        {
        case Toolkit.EVENT_SIZE_CHANGED:
            sizeChangedEvent(aData0, aData1);
            break;

        case Toolkit.EVENT_COMMAND:
            commandEvent(aData0);
            break;

        default:
            throw new IllegalArgumentException("unexpected event");
        }
    }

    void update()
    {
    }

    void showNotifyEvent(Display aDisplay)
    {
        iDisplay = aDisplay;
    }

    void hideNotifyEvent(Display aDisplay)
    {
        iDisplay = null;
    }

    public String getTitle()
    {
        return iTitle;
    }

    public void setTitle(String aTitle)
    {
        synchronized (iToolkit)
        {
            NativeError.check(_setTitle(getContainerHandle(),iToolkit.getHandle(),aTitle));
            iTitle  = aTitle;
            UpdateSize();
        }
    }

    public void setTicker(Ticker aTicker)
    {
        synchronized (iToolkit)
        {
            final int toolkit = iToolkit.getHandle();
            final int container = getContainerHandle();
            final int ticker = aTicker!=null?aTicker.getHandle():0;
            NativeError.check(_setTicker(container,toolkit, ticker));
            iTicker = aTicker;
            UpdateSize();
        }
    }

    public Ticker getTicker()
    {
        return iTicker;
    }

    public int getWidth()
    {
        synchronized (iToolkit)
        {
            return iWidth;
        }
    }

    public int getHeight()
    {
        synchronized (iToolkit)
        {
            return iHeight;
        }
    }

    protected void sizeChanged(int aWidth,int aHeight)
    {
    }

    /**
     * May be overridden in subclass. Arguments are ignored since
     * they may already be invalid.
     */
    void sizeChangedEvent(int aWidth,int aHeight)
    {
        int width;
        int height;
        boolean notify;
        synchronized (iToolkit)
        {
            int size = GetContentSize();
            width   = (size >>> 16);
            height  = (size & 0xffff);
            iWidth  = width;
            iHeight = height;
        }
        synchronized (iCallbackLock)
        {
            sizeChanged(width,height);
        }

        // If Displayable is in Vector (see end of Canvas constructor), it can be removed now
        // and GC can finalize it, because SizeChangedEvent is dispatched after Displayable creation
        if (iToolkit.tmpDisplayables.contains(this))
        {
            iToolkit.tmpDisplayables.removeElement(this);
        }
    }

    int GetContentSize()
    {
        return _getContentSize(getContainerHandle(), iToolkit.getHandle());
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
     * If iToolkit is already disposed, then native peers will have already been deleted.
     * handles may be zero if exception thrown in constructor
     */
    private void registeredFinalize()
    {
        synchronized (iToolkit)
        {
            iToolkit.disposeObject(iContent);
            iContent=0;
            iToolkit.disposeObject(iContainer);
            iContainer=0;
        }
    }

    int getContainerHandle()
    {
        if (iContainer <= 0)
        {
            throw new RuntimeException("Displayable("+this+") bad container handle: "+iContainer);
        }
        return iContainer;
    }

    int getContentHandle()
    {
        if (iContent <= 0)
        {
            throw new RuntimeException("Displayable("+this+") bad content handle: "+iContent);
        }
        return iContent;
    }

    void setContent(int aContent, int aWidth, int aHeight)
    {
        iContent = Toolkit.checkHandle(aContent);
        iWidth   = aWidth;
        iHeight  = aHeight;
    }

    void UpdateSize()
    {
        int size = GetContentSize();
        iWidth  = (size >> 16);
        iHeight = (size & 0xffff);
    }


    /*
     * This method gets the position and size of the softkey label
     * defined by softkeyId. The result is converted
     * in format "x,y,w,h"
     * If the softkey is not displayed, null is returned
     */
    String getSoftKeyLabelLocation(int softKeyId)
    {
        int position[] = new int[2];
        int size[] = new int[2];
        String returnValue = null;
        boolean softKeyExists = _getSoftKeyLabelLocation(iToolkit.getHandle(),
                                getContainerHandle(),
                                softKeyId,
                                position,
                                size);
        if (softKeyExists)
        {
            returnValue = String.valueOf(position[0]) + ","
                          + String.valueOf(position[1]) + ","
                          + String.valueOf(size[0]) + "," + String.valueOf(size[1]);
        }
        return returnValue;
    }

    /*
     * This method gets the anchor of the softkey label (as defined in class Graphics)
     * defined by softkeyId. The result is converted into string.
     * If the anchor is not known, null is returned
     */
    String getSoftKeyLabelAnchor(int softKeyId)
    {
        String returnValue = null;
        int anchor = _getSoftKeyLabelAnchor(iToolkit.getHandle(), getContainerHandle(), softKeyId);
        if (anchor != 0)
        {
            returnValue = String.valueOf(anchor);
        }
        return returnValue;
    }

    /*
     * Method returns String value of actual keyboard layout.
     * Value is obtained as String from native side. If returned value was
     * not created or its empty string, method returns null.
     */
    static final String getKeyboardTypeName()
    {
        Toolkit toolkit = Toolkit.getToolkit();
        String returnValue = null;
        synchronized (toolkit)
        {
            String value = _getKeyboardTypeName(toolkit.getHandle());
            if (value != null && value.length() != 0)
            {
                returnValue = value;
            }
        }
        return returnValue;
    }

    /*
     * Method returns String value of the scan code of the latest key event.
     * Value is obtained as int for native side and it is converted to String.
     */
    static final String getKeyScanCode()
    {
        Toolkit toolkit = Toolkit.getToolkit();
        String returnValue = null;
        synchronized (toolkit)
        {
            int value = _getKeyScanCode(toolkit.getHandle());
            returnValue = String.valueOf(value);
        }
        return returnValue;

    }
    /*
     * Method returns String value of modifier key event.
     * Value is obtained as int for native side and it is converted to String.
     */
    static final String getKeyModifier()
    {
        Toolkit toolkit = Toolkit.getToolkit();
        String returnValue = null;
        synchronized (toolkit)
        {
            int value = _getKeyModifier(toolkit.getHandle());
            returnValue = String.valueOf(value);
        }
        return returnValue;
    }

    private native int _createContainer(int aToolkit);
    private native int _addCommand(int aDisplayable,int aToolkit, int aCommand);
    private native void _removeCommand(int aToolkit,int aDisplayable, int aCommand);
    private native int _setTitle(int aDisplayable,int aToolkit,String aTitle);
    private native int _setTicker(int aDisplayable,int aToolkit,int aTicker);
    private native int _getContentSize(int aDisplayable,int aToolkit);
    private native void _setCommandListenerExistence(int aDisplayable, int aToolKit, boolean aExistence);
    private native boolean _getSoftKeyLabelLocation(int aToolkit, int aDisplayable, int softKeyId, int position[], int size[]);
    private native int _getSoftKeyLabelAnchor(int aToolkit, int aDisplayable, int softKeyId);
    static native String _getKeyboardTypeName(int aToolkit);
    static native int _getKeyScanCode(int aToolkit);
    static native int _getKeyModifier(int aToolkit);
}
