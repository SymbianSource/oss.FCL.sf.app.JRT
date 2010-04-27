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

public class ChoiceGroup extends Item implements Choice
{
    private final int iChoiceType;
    private final ChoiceComponent iChoiceComponent;

    public ChoiceGroup(String aLabel,int aChoiceType)
    {
        this(aLabel,aChoiceType,new String[] {},null);
    }

    public ChoiceGroup(String aLabel,int aChoiceType,String[] aTextArray,Image[] aIconArray)
    {
        if (aChoiceType != Choice.EXCLUSIVE && aChoiceType != Choice.MULTIPLE && aChoiceType != Choice.POPUP)
        {
            throw new IllegalArgumentException();
        }
        aIconArray = ChoiceComponent.validateElements(aTextArray,aIconArray);
        synchronized (iToolkit)
        {
            iChoiceComponent = new ChoiceComponent();
            if (aLabel == null)
            {
                aLabel = "";
            }
            iLabel  = aLabel;
            iChoiceType = aChoiceType;
            iHandle = Toolkit.checkHandle(_create(iToolkit.getHandle(), aLabel, aChoiceType));
            final int count = aTextArray.length;
            for (int i=0; i<count; i++)
            {
                Insert(i, aTextArray[i], aIconArray[i]);
            }
        }
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

    public int append(String aStringElement,Image aImageElement)
    {
        final int pos;
        synchronized (iToolkit)
        {
            pos = iChoiceComponent.size();
            // SYNC NOTE: cannot call public API method here
            Insert(pos,aStringElement,aImageElement);
            RefreshForm();
        }
        return pos;
    }

    public void insert(int aIndex,String aStringElement,Image aImageElement)
    {
        synchronized (iToolkit)
        {
            Insert(aIndex,aStringElement,aImageElement);
            RefreshForm();
        }
    }

    /**
     * Internal implementation of insert
     * SYNC NOTE: requires LCDUI lock to be held
     */
    void Insert(int aIndex,String aStringElement,Image aImageElement)
    {
        iChoiceComponent.Insert(aIndex,aStringElement,aImageElement);
        final int image = aImageElement==null?0:aImageElement.getHandle(true);
        NativeError.check(_insert(getHandle(),iToolkit.getHandle(),aIndex,aStringElement,image));
    }

    public void delete(int aIndex)
    {
        synchronized (iToolkit)
        {
            iChoiceComponent.Delete(aIndex);
            NativeError.check(_delete(getHandle(),iToolkit.getHandle(),aIndex));
            RefreshForm();
        }
    }

    public void deleteAll()
    {
        synchronized (iToolkit)
        {
            iChoiceComponent.DeleteAll();
            NativeError.check(_deleteAll(getHandle(),iToolkit.getHandle()));
            RefreshForm();
        }
    }

    public void set(int aIndex,String aStringElement,Image aImageElement)
    {
        synchronized (iToolkit)
        {
            iChoiceComponent.Set(aIndex,aStringElement,aImageElement);
            final int image = aImageElement==null?0:aImageElement.getHandle(true);
            NativeError.check(_set(getHandle(),iToolkit.getHandle(),aIndex,aStringElement,image));
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
     * Internal implementation of isSelected
     * SYNC NOTE: requires LCDUI lock to be held
     */
    boolean IsSelected(int aIndex)
    {
        iChoiceComponent.checkIndexInBounds(aIndex,iChoiceComponent.size());
        return _isSelected(getHandle(),iToolkit.getHandle(),aIndex);
    }

    public int getSelectedIndex()
    {
        synchronized (iToolkit)
        {
            if (iChoiceType != Choice.MULTIPLE)
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
     * Internal implementation of setSelectedIndex
     * SYNC NOTE: requires LCDUI lock to be held
     */
    void SetSelectedIndex(int aIndex,boolean aSelected)
    {
        iChoiceComponent.checkIndexInBounds(aIndex,iChoiceComponent.size());
        NativeError.check(_select(getHandle(),iToolkit.getHandle(),aIndex,aSelected));
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
            final boolean notMultiple = iChoiceType != Choice.MULTIPLE;
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

    public void setFitPolicy(int aFitPolicy)
    {
        synchronized (iToolkit)
        {
            iChoiceComponent.SetFitPolicy(aFitPolicy); //will validate aFitPolicy
            NativeError.check(_setFitPolicy(getHandle(), iToolkit.getHandle(), aFitPolicy));
            RefreshForm();
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
            NativeError.check(_setFont(getHandle(), iToolkit.getHandle(),
                                       aElementNum, aFont == null ? 0 : aFont.iHandle));
            RefreshForm();
        }
    }

    public Font getFont(int aElementNum)
    {
        return iChoiceComponent.GetFont(aElementNum);
    }

    private native int _create(int aToolkit, String aLabel, int aType);
    private native int _insert(int aHandle,int aToolkit,int aIndex,String aStringElement,int aImage);
    private native int _delete(int aHandle,int aToolkit,int aIndex);
    private native int _deleteAll(int aHandle,int aToolkit);
    private native int _set(int aHandle,int aToolkit,int aIndex,String aStringElement,int aImage);
    private native boolean _isSelected(int aHandle,int aToolkit,int aIndex);
    private native int _select(int aHandle,int aToolkit,int aIndex,boolean aSelected);
    private native int _setFont(int aHandle,int aToolkit,int aElementNum,int aFont);
    private native int _setFitPolicy(int aHandle,int aToolkit,int aFitPolicy);
}
