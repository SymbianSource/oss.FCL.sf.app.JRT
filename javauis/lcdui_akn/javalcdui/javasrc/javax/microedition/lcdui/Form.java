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

public class Form extends Screen
{
    private static final int ARRAY_INCR = 4;
    private static final int MIN_PACK_SPACE = 8;

    private Item[]  iItemArray;
    private int     iItemCount;
    private ItemStateListener iItemStateListener;

    public Form(String aTitle)
    {
        this(aTitle,null);
    }

    public Form(String aTitle,Item[] aItems)
    {
        super();

        final int count = (aItems != null ? aItems.length : 0);
        synchronized (iToolkit)
        {
            iItemArray = new Item[Math.max(count, ARRAY_INCR)];
            iItemCount = 0;
            final int[] createForm = new int[3];
            NativeError.check(_create(iToolkit.getHandle(), getContainerHandle(), createForm));
            setContent(createForm[0], createForm[1], createForm[2]);
        }
        setTitle(aTitle);
        for (int i=0; i<count; i++)
        {
            insert(i, aItems[i]);
        }
    }

    public int append(Item aItem)
    {
        aItem.addNotify(this);
        try
        {
            synchronized (iToolkit)
            {
                final int index = iItemCount;
                Insert(index,aItem);
                return index;
            }
        }
        catch (RuntimeException re)
        {
            aItem.removeNotify(this);
            throw re;
        }
        catch (Error er)
        {
            aItem.removeNotify(this);
            throw er;
        }
    }

    public int append(String aString)
    {
        if (aString == null)
        {
            throw new NullPointerException();
        }
        return append(new StringItem(null,aString));
    }

    public int append(Image aImage)
    {
        if (aImage == null)
        {
            throw new NullPointerException();
        }
        return append(new ImageItem(null,aImage,ImageItem.LAYOUT_DEFAULT,null));
    }

    public void insert(int aIndex,Item aItem)
    {
        aItem.addNotify(this);
        try
        {
            synchronized (iToolkit)
            {
                Insert(aIndex, aItem);
            }
        }
        catch (RuntimeException re)
        {
            aItem.removeNotify(this);
            throw re;
        }
        catch (Error er)
        {
            aItem.removeNotify(this);
            throw er;
        }
    }

    final void Insert(int aIndex, Item aItem)
    {
        // 1. grow array as necessary.
        if (iItemCount == iItemArray.length)
        {
            Item[] array = new Item[ iItemCount + ARRAY_INCR ];
            System.arraycopy(iItemArray, 0, array, 0, iItemCount);
            iItemArray = array;
        }

        if ((aIndex < 0) || (aIndex > iItemCount))
        {
            throw new IndexOutOfBoundsException();
        }

        NativeError.check(_insert(getContentHandle(),iToolkit.getHandle(),aIndex,aItem.getHandle()));

        System.arraycopy(iItemArray, aIndex, iItemArray, aIndex+1, iItemCount-aIndex);
        iItemArray[aIndex] = aItem;
        iItemCount++;
    }

    /* Override from Displayable */
    public int getWidth()
    {
        synchronized (iToolkit)
        {
            if (0 == iWidth)
            {
                GetFormSize();
            }
            return iWidth;
        }
    }

    /* Override from Displayable */
    public int getHeight()
    {
        synchronized (iToolkit)
        {
            if (0 == iHeight)
            {
                GetFormSize();
            }
            return iHeight;
        }
    }

    public void delete(int aIndex)
    {
        final Item item;
        synchronized (iToolkit)
        {
            item = Get(aIndex); // throws.....

            NativeError.check(_delete(getContentHandle(),iToolkit.getHandle(),aIndex));
            System.arraycopy(iItemArray, aIndex+1, iItemArray, aIndex, iItemCount-(aIndex+1));
            --iItemCount;

            Pack();
        }
        item.removeNotify(this);
    }

    public void deleteAll()
    {
        Item[] items;
        int    count;

        synchronized (iToolkit)
        {
            if (iItemCount == 0)
            {
                return;
            }

            count = iItemCount;
            items = iItemArray;

            NativeError.check(_deleteAll(getContentHandle(),iToolkit.getHandle()));
            iItemCount = 0;
            iItemArray = new Item[ARRAY_INCR];
        }

        while (--count>=0)
        {
            items[count].removeNotify(this);
        }
    }

    void Pack()
    {
        if (iItemCount + MIN_PACK_SPACE < iItemArray.length)
        {
            Item[] array = new Item[iItemCount];
            System.arraycopy(iItemArray, 0, array, 0, iItemCount);
            iItemArray = array;
        }
    }

    public void set(int aIndex,Item aItem)
    {
        Item prev = null;
        aItem.addNotify(this);
        try
        {
            synchronized (iToolkit)
            {
                prev = Set(aIndex, aItem);
            }
        }
        catch (RuntimeException e)
        {
            aItem.removeNotify(this);
            throw e;
        }
        prev.removeNotify(this);
    }

    final Item Set(int aIndex, Item aItem)
    {
        final Item prev = Get(aIndex);

        NativeError.check(_set(getContentHandle(),iToolkit.getHandle(),aIndex,aItem.getHandle()));

        iItemArray[aIndex] = aItem;

        return prev;
    }

    public Item get(int aIndex)
    {
        synchronized (iToolkit)
        {
            return Get(aIndex);
        }
    }

    final Item Get(int aIndex)
    {
        if (aIndex >= iItemCount)
        {
            throw new IndexOutOfBoundsException();
        }
        return iItemArray[aIndex];
    }

    public void setItemStateListener(ItemStateListener aListener)
    {
        iItemStateListener = aListener;
    }

    public int size()
    {
        return iItemCount;
    }

    final void handleEvent(int aEvent,int aData0, int aData1)
    {
        if (aEvent == Toolkit.EVENT_ITEM_CHANGED)
        {
            handleItemChangedEvent(get(aData0));
        }
        else
        {
            super.handleEvent(aEvent, aData0, aData1);
        }
    }

    final void handleItemChangedEvent(Item aItem)
    {
        if (aItem instanceof DateField)
        {
            final DateField df = (DateField)aItem;
            df.setDateInitialized(true);
        }
        if (iItemStateListener != null)
        {
            iItemStateListener.itemStateChanged(aItem);
        }
    }

    final void Refresh(Item aItem)
    {
        NativeError.check(_refresh(getContentHandle(),iToolkit.getHandle(),Index(aItem)));
    }

    final void Update(Item aItem)
    {
        NativeError.check(_set(getContentHandle(),iToolkit.getHandle(),Index(aItem),aItem.getHandle()));
    }

    final int Index(Item aItem)
    {
        int index = iItemCount - 1;

        while ((index>=0) && (iItemArray[index] != aItem))
        {
            --index;
        }

        return index;
    }

    final void notifyStateChanged(Item aItem)
    {
        synchronized (iToolkit)
        {
            _notifyStateChanged(getContentHandle(), iToolkit.getHandle(), Index(aItem));
        }
    }

    final void setCurrentItem(Item aItem)
    {
        synchronized (iToolkit)
        {
            NativeError.check(_setCurrentItem(getContentHandle(),iToolkit.getHandle(),Index(aItem)));
        }
    }

    private void GetFormSize()
    {
        final int size = _getFormSize(getContentHandle(), iToolkit.getHandle());
        iWidth  = size >>> 16;
        iHeight = size & 0xffff;
    }

    private native int _create(int aToolkit, int aDisplayable, int[] aCreateFormReturn);
    private native int _refresh(int aForm,int aToolkit,int aIndex);
    private native boolean _isVisible(int aForm,int aToolkit,int aIndex);
    private native int _insert(int aForm,int aToolkit,int aIndex,int aItem);
    private native int _set(int aForm,int aToolkit,int aIndex,int aItem);
    private native int _setAll(int aForm,int aToolkit,int[] aItem);
    private native int _delete(int aForm,int aToolkit,int aIndex);
    private native int _deleteAll(int aForm,int aToolkit);
    private native void _notifyStateChanged(int aForm,int aToolkit,int aIndex);
    private native int  _getFormSize(int aForm,int aToolkit);
    native int _setCurrentItem(int aForm,int aToolkit,int aIndex);
}

