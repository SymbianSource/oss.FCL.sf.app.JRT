/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
package javax.microedition.lcdui.game;

import java.util.Vector;

import javax.microedition.lcdui.Graphics;

/**
 * LayerManager class.
 */
public class LayerManager
{

    private Vector layers;

    private int viewX;
    private int viewY;

    private int viewWidth = Integer.MAX_VALUE;
    private int viewHeight = Integer.MAX_VALUE;

    public LayerManager()
    {
        layers = new Vector();
    }

    public void append(Layer aLayer)
    {
        remove(aLayer);
        layers.addElement(aLayer);
    }

    public void insert(Layer layer, int index)
    {
        if(layer == null)
        {
            throw new NullPointerException(
                MsgRepository.LAYERMANAGER_EXCEPTION_LAYER_NULL);
        }
        if(index < 0 || index > layers.size())
        {
            throw new IndexOutOfBoundsException(
                MsgRepository.LAYERMANAGER_EXCEPTION_INVALID_LAYER_INDEX);
        }
        if(index > layers.size() - 1 && layers.contains(layer))
        {
            throw new IndexOutOfBoundsException(
                MsgRepository.LAYERMANAGER_EXCEPTION_INVALID_LAYER_INDEX);
        }
        remove(layer);
        layers.insertElementAt(layer, index);
    }

    public void remove(Layer layer)
    {
        if(layer == null)
        {
            throw new NullPointerException(
                MsgRepository.LAYERMANAGER_EXCEPTION_LAYER_NULL);
        }
        layers.removeElement(layer);
    }

    public Layer getLayerAt(int index)
    {
        return (Layer) layers.elementAt(index);
    }

    public int getSize()
    {
        return layers.size();
    }

    public void setViewWindow(int x, int y, int width, int height)
    {
        if(width < 0 || height < 0)
        {
            throw new IllegalArgumentException(
                MsgRepository.LAYERMANAGER_EXCEPTION_INVALID_WIDTH_HEIGHT);
        }
        viewX = x;
        viewY = y;
        viewWidth = width;
        viewHeight = height;
    }

    public void paint(Graphics graphics, int x, int y)
    {
        // save Graphics clip and translate
        final int oldClipX = graphics.getClipX();
        final int oldClipY = graphics.getClipY();
        final int oldClipW = graphics.getClipWidth();
        final int oldClipH = graphics.getClipHeight();
        final int oldTransX = graphics.getTranslateX();
        final int oldTransY = graphics.getTranslateY();

        // intersect Graphics clip region with View Window
        graphics.clipRect(x, y, viewWidth, viewHeight);
        /*
         * translate the graphics object so the point (aX,aY) corresponds to the
         * location of the viewWindow in the coordinate system of the
         * LayerManager.
         */
        graphics.translate(-viewX, -viewY);

        // paint visible Layers
        Layer layer = null;
        for(int i = layers.size() - 1; i >= 0; i--)
        {
            layer = getLayerAt(i);
            if(layer.isVisible(graphics, x, y))
            {
                graphics.translate(x, y);
                layer.paint(graphics);
                graphics.translate(-x, -y);
            }
        }

        // restore Graphics translation because translation is concatenated
        // need to translate back to 0,0 first
        graphics.translate(-graphics.getTranslateX(),
                           -graphics.getTranslateY());

        // restore Graphics translate and clip
        graphics.translate(oldTransX, oldTransY);
        graphics.setClip(oldClipX, oldClipY, oldClipW, oldClipH);
    }

}
