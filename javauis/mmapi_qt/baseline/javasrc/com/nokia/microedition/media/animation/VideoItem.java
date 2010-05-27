/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: VideoItem
*
*/

package com.nokia.microedition.media.animation;

import javax.microedition.lcdui.CustomItem;
import javax.microedition.lcdui.Graphics;
import javax.microedition.media.Player;
import javax.microedition.media.PlayerListener;
import org.eclipse.swt.graphics.Point;

import com.nokia.mj.impl.utils.Logger;

public class VideoItem extends CustomItem implements PlayerListener {
	
	/**
	 * Width of this item  
	 */
	
	int iWidth;
	
	/**
	 * Height of this item
	 */
	int iHeight;
	
	/**
	 * 
	 * @param dimension
	 */
	protected VideoItem(Point dimension ) {
		super("");
		iWidth=dimension.x;
		iHeight=dimension.y;
	}
	
	/**
	 * 
	 * @param w
	 * @param h
	 */
	protected VideoItem(int w, int h) {
		super("");
		iWidth=w;
		iHeight=h;
	}
	/**
	 * @return the minimum content width of the item, used by form while layouting the item  
	 */
	protected int getMinContentWidth() {
		return iWidth;
	}

	/**
	 * @return the minimum content height of the item, used by form while layouting the item 
	 */
	protected int getMinContentHeight() {
		return iHeight;
	}
	
	/**
	 * @return the preferred content width of the item, used by form while layouting the item 
	 */
	protected int getPrefContentWidth(int height) {
		return iWidth;
	}

	/**
	 * @return the preferred content height of the item, used by form while layouting the item 
	 */

	protected int getPrefContentHeight(int width) {
		return iHeight;
	}
	/**
	 * This is dummy implementation of the paint method. 
	 */
	int count;
	protected void paint(Graphics g, int w, int h) {
		//TODO remove this code later
		// this is added just for testing purpose 
		g.setColor(0x00a000);
		g.fillRect(0,0,w,h);
		g.setColor(0xFFFFFF);
		g.drawString("paint"+(count++), w>>1, h>>1, Graphics.BASELINE|Graphics.HCENTER);
	}
	
	/**
	 * Whenever player will be resized this function will be invoked and it will call the invalidate method of custom item 
	 */
	public void playerUpdate(Player aPlayer, String aEvent, Object aEventData) {
		Logger.LOG(Logger.EJavaMMAPI, Logger.EInfo,"VideoItem::playerUpdate() Event is "+aEvent+"Event Data is "+aEventData);
		if (aEvent == SIZE_CHANGED) {
			invalidate();
		}
	}
}
