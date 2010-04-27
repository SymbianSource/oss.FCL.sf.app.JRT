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
* Description:
*
*/


package javax.microedition.lcdui.game;

import javax.microedition.lcdui.Graphics;

import java.util.Vector;

public class LayerManager
{
    private Vector iLayers;
    private int iViewX;
    private int iViewY;
    private int iViewWidth  = Integer.MAX_VALUE;
    private int iViewHeight = Integer.MAX_VALUE;

    public LayerManager()
    {
        iLayers = new Vector();
    }

    public void append(Layer aLayer)
    {
        remove(aLayer);
        iLayers.addElement(aLayer);
    }

    public void insert(Layer aLayer,int aIndex)
    {
        if (aLayer == null)
        {
            throw new NullPointerException();
        }

        int indexLimit = iLayers.size();
        if (aIndex<0 || aIndex>indexLimit)
        {
            throw new IndexOutOfBoundsException();
        }

        indexLimit--;
        if (aIndex>indexLimit && iLayers.contains(aLayer))
        {
            throw new IndexOutOfBoundsException();
        }

        remove(aLayer);
        iLayers.insertElementAt(aLayer,aIndex);
    }

    public void remove(Layer aLayer)
    {
        if (aLayer == null)
        {
            throw new NullPointerException();
        }
        iLayers.removeElement(aLayer);
    }

    public Layer getLayerAt(int aIndex)
    {
        return (Layer)iLayers.elementAt(aIndex);
    }

    public int getSize()
    {
        return iLayers.size();
    }



    public void setViewWindow(int aX,int aY,int aWidth,int aHeight)
    {
        if (aWidth<0 || aHeight<0)
        {
            throw new IllegalArgumentException("Width or height is less than 0");
        }
        iViewX=aX;
        iViewY=aY;
        iViewWidth=aWidth;
        iViewHeight=aHeight;
    }

    public void paint(Graphics aGraphics,int aX,int aY)
    {
        // save Graphics clip and translate
        final int clipX = aGraphics.getClipX();
        final int clipY = aGraphics.getClipY();
        final int clipWidth = aGraphics.getClipWidth();
        final int clipHeight = aGraphics.getClipHeight();
        final int transX = aGraphics.getTranslateX();
        final int transY = aGraphics.getTranslateY();

        // intersect Graphics clip region with View Window
        if ((iViewWidth<Integer.MAX_VALUE) && (iViewWidth<Integer.MAX_VALUE))
            aGraphics.clipRect(aX,aY,iViewWidth,iViewHeight);

        // translate the graphics object so the point (aX,aY) corresponds to
        // the location of the viewWindow in the coordinate system of the LayerManager.
        aGraphics.translate(-iViewX,-iViewY);

        // paint visible Layers
        final int size = iLayers.size();
        for (int i=size-1; i>=0; i--)
        {
            final Layer layer = getLayerAt(i);
            if (layerVisible(aGraphics,layer,aX,aY))
            {
                aGraphics.translate(aX,aY);
                layer.paint(aGraphics);
                aGraphics.translate(-aX,-aY);
            }
        }

        // restore Graphics translation
        // because translation is concatenated, we need to translate back to 0,0 first
        aGraphics.translate(-aGraphics.getTranslateX(), -aGraphics.getTranslateY());
        // now back to original translation, per spec
        aGraphics.translate(transX,transY);
        // restore Graphics clip - must do after restore translation
        aGraphics.setClip(clipX,clipY,clipWidth,clipHeight);
    }

    /**
     * Check if Layer is visible in Graphics clip region.
     */
    private boolean layerVisible(Graphics aGraphics, Layer aLayer, int aX, int aY)
    {
        final int clipTlX = aGraphics.getClipX();
        final int clipTlY = aGraphics.getClipY();
        final int clipBrX = clipTlX + aGraphics.getClipWidth();
        final int clipBrY = clipTlY + aGraphics.getClipHeight();

        final int layerTlX = aLayer.getX() + aX;
        final int layerTlY = aLayer.getY() + aY;
        final int layerBrX = layerTlX + aLayer.getWidth();
        final int layerBrY = layerTlY + aLayer.getHeight();

        return(!(clipBrX<=layerTlX || clipBrY<=layerTlY || clipTlX>=layerBrX || clipTlY>=layerBrY));
    }

}


