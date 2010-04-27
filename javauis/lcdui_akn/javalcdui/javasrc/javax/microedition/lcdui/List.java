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

import com.nokia.mj.impl.rt.legacy.NativeError;

public class List extends Screen implements Choice
{
    //
    // This is the default select command sent to the command listener
    // when an choice is selected on an implicit list.
    //
    public static final Command SELECT_COMMAND = new Command(Command.SCREEN);

    // the real select command
    private Command iSelectCommand; //pointer to the current select command
    private int iListType;
    private final ChoiceComponent iChoiceComponent;

    public List(String aTitle,int aListType)
    {
        this(aTitle,aListType,new String[] {},null);
    }

    public List(String aTitle,int aListType,String[] aTextArray,Image[] aIconArray)
    {
        super();
        if (aListType < Choice.EXCLUSIVE || aListType > Choice.IMPLICIT)
        {
            throw new IllegalArgumentException();
        }
        aIconArray = ChoiceComponent.validateElements(aTextArray, aIconArray);
        synchronized (iToolkit)
        {
            iListType = aListType;
            iSelectCommand = SELECT_COMMAND;
            iChoiceComponent = new ChoiceComponent();

            final int[] createListReturn = new int[3];
            NativeError.check(_create(iToolkit.getHandle(), getContainerHandle(), iListType, createListReturn));
            setContent(createListReturn[0], createListReturn[1], createListReturn[2]);

            final int count = aTextArray.length;
            for (int i=0; i<count; i++)
            {
                Insert(i, aTextArray[i], aIconArray[i]);
            }
        }
        setTitle(aTitle);
    }

    public int size()
    {
        return iChoiceComponent.size();
    }

    public String getString(int aIndex)
    {
        synchronized (iToolkit)
        {
            return iChoiceComponent.GetString(aIndex);
        }
    }

    public Image getImage(int aIndex)
    {
        synchronized (iToolkit)
        {
            return iChoiceComponent.GetImage(aIndex);
        }
    }

    public int append(String aText, Image aIcon)
    {
        int pos;
        synchronized (iToolkit)
        {
            pos = size();
            Insert(pos, aText, aIcon);
        }
        return pos;
    }

    public void insert(int aIndex, String aText, Image aIcon)
    {
        synchronized (iToolkit)
        {
            Insert(aIndex, aText, aIcon);
        }
    }

    private void Insert(int aIndex, String aText, Image aIcon)
    {
        iChoiceComponent.Insert(aIndex, aText, aIcon);
        final int image = aIcon==null?0:aIcon.getHandle(true);
        NativeError.check(_insert(getContentHandle(), iToolkit.getHandle(), aIndex, aText, image));
    }

    public void delete(int aIndex)
    {
        synchronized (iToolkit)
        {
            iChoiceComponent.Delete(aIndex);
            NativeError.check(_delete(getContentHandle(),iToolkit.getHandle(),aIndex));
        }
    }

    public void deleteAll()
    {
        synchronized (iToolkit)
        {
            iChoiceComponent.DeleteAll();
            NativeError.check(_deleteAll(getContentHandle(),iToolkit.getHandle()));
        }
    }

    public void set(int aIndex,String aText,Image aIcon)
    {
        synchronized (iToolkit)
        {
            iChoiceComponent.Set(aIndex,aText,aIcon);
            final int image = aIcon==null?0:aIcon.getHandle(true);
            NativeError.check(_set(getContentHandle(),iToolkit.getHandle(),aIndex,aText,image));
        }
    }

    public boolean isSelected(int aIndex)
    {
        synchronized (iToolkit)
        {
            return IsSelected(aIndex);
        }
    }

    /**
     Internal implementation of isSelected
     SYNC NOTE: requires LCDUI lock to be held
     */
    boolean IsSelected(int aIndex)
    {
        iChoiceComponent.checkIndexInBounds(aIndex,iChoiceComponent.size());
        return _isSelected(getContentHandle(),iToolkit.getHandle(),aIndex);
    }

    public int getSelectedIndex()
    {
        synchronized (iToolkit)
        {
            if (iListType != Choice.MULTIPLE)
            {
                final int len = iChoiceComponent.size();
                for (int i=0; i<len; i++)
                {
                    // SYNC NOTE: cannot call public API method here in case
                    // it has been overridden by application subclass.
                    if (IsSelected(i))
                        return i;
                }
            }
            return -1;
        }
    }

    public int getSelectedFlags(boolean[] aSelectedArrayReturn)
    {
        synchronized (iToolkit)
        {
            final int len = aSelectedArrayReturn.length;
            final int choiceSize = iChoiceComponent.size();
            if (len < choiceSize)
                throw new IllegalArgumentException();
            int nsel=0;
            for (int i=0; i<len; i++)
            {
                // SYNC NOTE: cannot call public API method here in case
                // it has been overridden by application subclass.
                if (i<choiceSize && IsSelected(i))
                {
                    nsel++;
                    aSelectedArrayReturn[i] = true;
                }
                else
                    aSelectedArrayReturn[i] = false;
            }
            return nsel;
        }
    }


    public void setSelectedIndex(int aIndex,boolean aSelected)
    {
        synchronized (iToolkit)
        {
            SetSelectedIndex(aIndex,aSelected);
        }
    }

    /**
     Internal implementation of setSelectedIndex
     SYNC NOTE: requires LCDUI lock to be held
     */
    void SetSelectedIndex(int aIndex,boolean aSelected)
    {
        iChoiceComponent.checkIndexInBounds(aIndex,iChoiceComponent.size());
        NativeError.check(_select(getContentHandle(),iToolkit.getHandle(),aIndex,aSelected));
    }

    public void setSelectedFlags(boolean[] aSelectedArray)
    {
        if (aSelectedArray==null)
            throw new NullPointerException();
        synchronized (iToolkit)
        {
            final int len = iChoiceComponent.size();
            if (len==0)
                return;
            if (aSelectedArray.length < len)
                throw new IllegalArgumentException();
            //
            boolean hasTrue=false;
            for (int i=0; i<len; ++i)
                hasTrue = hasTrue || aSelectedArray[i];
            //
            final boolean notMultiple = iListType != Choice.MULTIPLE;
            if (notMultiple && !hasTrue)
            {
                // SYNC NOTE: cannot call public API method here in case
                // it has been overridden by application subclass.
                SetSelectedIndex(0,true);
                return;
            }
            //
            for (int i=0; i<len; ++i)
            {
                // SYNC NOTE: cannot call public API method here
                SetSelectedIndex(i,aSelectedArray[i]);
                if (aSelectedArray[i] && notMultiple)
                {
                    for (int j=i+1; j<len; ++j) //set the rest to false
                    {
                        // SYNC NOTE: cannot call public API method here.
                        SetSelectedIndex(j, false);
                    }
                    return;
                }
            }
        }
    }

    public void removeCommand(Command aCommand)
    {
        synchronized (iToolkit)
        {
            if (aCommand == iSelectCommand)
            {
                iSelectCommand = null;
                if (iListType == Choice.IMPLICIT)
                {
                    _setSelectCommand(getContentHandle(), iToolkit.getHandle(), 0);
                }
            }
            DoRemoveCommand(aCommand);
        }
    }

    public void setSelectCommand(Command aCommand)
    {
        synchronized (iToolkit)
        {
            if (iListType != Choice.IMPLICIT)
            {
                return;
            }

            int commandID = 0;

            if (aCommand != null)
            {
                if (aCommand != SELECT_COMMAND)
                {
                    DoAddCommand(aCommand);
                }
                CommandPeer peer = iCommands.getPeer(aCommand);
                if (null != peer)
                {
                    commandID = peer.getId();
                }
                else
                {
                    commandID = CommandPeer.SELECT_CMD_ID;
                }
            }

            _setSelectCommand(getContentHandle(), iToolkit.getHandle(), commandID);
            iSelectCommand = aCommand;
        }
    }

    public void setFitPolicy(int aFitPolicy)
    {
        synchronized (iToolkit)
        {
            iChoiceComponent.SetFitPolicy(aFitPolicy); //will validate aFitPolicy
            NativeError.check(_setFitPolicy(getContentHandle(), iToolkit.getHandle(), aFitPolicy));
        }
    }

    public int getFitPolicy()
    {
        return iChoiceComponent.getFitPolicy();
    }

    public void setFont(int aElementNum, Font aFont)
    {
        if (aFont == null || aFont.iIsFreeSizeFont == true)
        {
            aFont = Font.getDefaultFont();
        }
        synchronized (iToolkit)
        {
            iChoiceComponent.SetFont(aElementNum, aFont);
            NativeError.check(_setFont(getContentHandle(), iToolkit.getHandle(),
                                       aElementNum, aFont == null ? 0 : aFont.iHandle));
        }
    }

    public Font getFont(int aElementNum)
    {
        synchronized (iToolkit)
        {
            return iChoiceComponent.GetFont(aElementNum);
        }
    }

    final void handleEvent(int aEvent,int aData0, int aData1)
    {
        if (aEvent == Toolkit.EVENT_SELECT)
        {
            final Command command;
            final int count;
            synchronized (iToolkit)
            {
                command = iSelectCommand;
                count   = iChoiceComponent.size();
            }
            if (count > 0)
            {
                callCommandAction(command);
            }
        }
        else
        {
            super.handleEvent(aEvent, aData0, aData1);
        }
    }

    private native int _create(int aToolkit, int aDisplayable, int aType, int[] aCreateListReturn);
    private native int _insert(int aHandle,int aToolkit,int aIndex,String aStringElement,int aImage);
    private native int _delete(int aHandle,int aToolkit,int aIndex);
    private native int _deleteAll(int aHandle,int aToolkit);
    private native int _set(int aHandle,int aToolkit,int aIndex,String aStringElement,int aImage);
    private native boolean _isSelected(int aHandle,int aToolkit,int aIndex);
    private native int _select(int aHandle,int aToolkit,int aIndex,boolean aSelected);
    private native int _setFont(int aHandle,int aToolkit,int aElementNum,int aFont);
    private native int _setFitPolicy(int aHandle,int aToolkit,int aFitPolicy);
    private native void _setSelectCommand(int aHandle,int aToolkit,int aCommandId);
}
