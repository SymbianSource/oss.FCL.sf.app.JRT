/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Item for VideoControl.
*
*/


package com.nokia.microedition.media.control;

import javax.microedition.lcdui.CustomItem;
import javax.microedition.lcdui.Graphics;
import javax.microedition.media.PlayerListener;
import javax.microedition.media.Player;
import javax.microedition.lcdui.*;

/**
 * Item for VideoControl.
 * This class native equivalent is CMIDItemDisplay.
 * All drawing is done in native.
 * CustomItem has its native bitmap after it is layouted to
 * the form and then SizeChanged method is called and native side
 * gets the bitmap for item.
 *
 */
public class VideoItem extends CustomItem implements PlayerListener
{
    int iNativeHandle;

    private int iEventSourceHandle;

    private Player iPlayer = null;

    public VideoItem(int aEventSourceHandle, Player aPlayer)
    {
        super("");   // we don't have title
        iEventSourceHandle = aEventSourceHandle;
        iPlayer = aPlayer;
    }

    // from PlayerListener
    public void playerUpdate(Player aPlayer, String aEvent, Object aEventData)
    {
        if (aEvent == SIZE_CHANGED)
        {
            invalidate();
        }
    }

    /**
     * From class CustomItem
     *
     */
    protected int getMinContentHeight()
    {
        int minHeight = 0;
        if (iNativeHandle != 0)
        {
            minHeight = _getMinContentHeight(iEventSourceHandle,
                                             iNativeHandle);
        }
        return minHeight;
    }

    /**
     * From class CustomItem
     *
     */
    protected int getMinContentWidth()
    {
        int minWidth = 0;
        if (iNativeHandle != 0)
        {
            minWidth = _getMinContentWidth(iEventSourceHandle,
                                           iNativeHandle);
        }
        return minWidth;
    }

    /**
     * From class CustomItem
     *
     */
    protected int getPrefContentHeight(int aWidth)
    {
        int prefHeight = 0;
        if (iNativeHandle != 0)
        {
            prefHeight = _getPrefContentHeight(iEventSourceHandle,
                                               iNativeHandle,
                                               aWidth);
        }
        return prefHeight;
    }

    /**
     * From class CustomItem
     *
     */
    protected int getPrefContentWidth(int aHeight)
    {
        int prefWidth = 0;
        if (iNativeHandle != 0)
        {
            prefWidth = _getPrefContentWidth(iEventSourceHandle,
                                             iNativeHandle,
                                             aHeight);
        }
        return prefWidth;
    }

    /**
     * From class CustomItem
     *
     * Empty implementation because drawing is done in native side.
     * Must be implemented because this method is abstract.
     *
     */
    protected void paint(Graphics aGrapchics, int aWidth, int aHeight)
    {
    }

    /**
     * From class CustomItem
     *
     */
    protected void sizeChanged(int aWidth, int aHeight)
    {
        if (iNativeHandle != 0)
        {
            _sizeChanged(iEventSourceHandle,
                         iNativeHandle,
                         aWidth,
                         aHeight);
        }
    }

    /**
     * Package private method for invalidating the Form owning this item.
     */
    void privateInvalidate()
    {
        invalidate();
    }

    /**
     * Sets native CMIDItemDisplay handle.
     */
    void setNativeHandle(int aHandle)
    {
        iNativeHandle = aHandle;
    }

    int[] getSourceSize()
    {
        int[] size = new int[2];

        if (iPlayer.getState() >= iPlayer.PREFETCHED)
        {
            int width = _getMinContentWidth(iEventSourceHandle,
                                            iNativeHandle);
            int height = _getMinContentHeight(iEventSourceHandle,
                                              iNativeHandle);
            size[0] = width;
            size[1] = height;
        }
        else
        {
            size[0] = 0;
            size[1] = 0;
        }
        return size;
    }


    // Native methods

    /**
     * Informs got size to native side.
     */
    private native int _sizeChanged(int aEventSourceHandle,
                                    int aNativeHandle,
                                    int aWidth,
                                    int aHeight);

    // Getters for layout.
    private native int _getMinContentWidth(int aEventSourceHandle,
                                           int aNativeHandle);
    private native int _getMinContentHeight(int aEventSourceHandle,
                                            int aNativeHandle);
    private native int _getPrefContentWidth(int aEventSourceHandle,
                                            int aNativeHandle,
                                            int aTentativeWidth);
    private native int _getPrefContentHeight(int aEventSourceHandle,
            int aNativeHandle,
            int aTentativeHeigth);
}
