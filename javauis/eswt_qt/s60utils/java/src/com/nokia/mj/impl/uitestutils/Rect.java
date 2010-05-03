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
package com.nokia.mj.impl.uitestutils;

/**
 * Simple rect container
 * @author sampkaar
 *
 */
public class Rect {
	
	private int x;
	private int y;
	private int width;
	private int height;
	
	public Rect(int x, int y, int width, int height) {
		this.x = x;
		this.y = y;
		this.width = width;
		this.height = height;
	}
	
	public int x() {
		return this.x;
	}
	
	public int y() {
		return this.y;
	}
	
	public int width() {
		return this.width;
	}
	
	public int height() {
		return this.height;
	}

	public Rect intersection(Rect aRect){

		if( aRect.x < this.x && aRect.x+aRect.width < this.x ||
            aRect.x > this.x+this.width && aRect.x+aRect.width > this.x+this.width)
        { // no intersection
        	return new Rect(0,0,0,0);
        } 
        if( aRect.y < this.y && aRect.y+aRect.height < this.y ||
            aRect.y > this.y+this.height && aRect.y+aRect.height > this.y+this.height)
        { // no intersection
            return new Rect(0,0,0,0);
        }
        int x = 0;
        int y = 0;
        int w = 0;
        int h = 0;
        
        if(aRect.x < this.x)
        {
        	x = this.x;
        	if (this.x + this.width < aRect.x + aRect.width)
        	{
        		w = this.width;
        	}else
        	{
        		w = aRect.x + aRect.width - this.x;        		
        	}
        } else
        { // aRect.x >= this.x
        	x = aRect.x;        	
        	if (aRect.x + aRect.width < this.x + this.width)
        	{
        		w = aRect.width;
        	}else
        	{
        		w = this.x + this.width - aRect.x;
        	}
        }
        if(aRect.y < this.y)
        {
        	y = this.y;
        	if (this.y + this.height < aRect.y + aRect.height)
        	{
        		h = this.height;
        	}else
        	{
        		h = aRect.y + aRect.height - this.y;
        	}
        } else
        { // aRect.y >= this.y
        	y = aRect.y;        	
        	if (aRect.y + aRect.height < this.y + this.height)
        	{
        		h = aRect.height;
        	}else
        	{
        		h = this.y + this.height - aRect.y;        		
        	}
        }
		return new Rect(x, y, w, h);
	}

}
