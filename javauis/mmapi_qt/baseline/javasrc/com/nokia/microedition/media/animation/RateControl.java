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
/**
 * 
 */
package com.nokia.microedition.media.animation;

import javax.microedition.media.Player;

import com.nokia.microedition.media.control.ControlImpl;

/**
 * @author d35kumar
 *
 */
public class RateControl extends ControlImpl implements
		javax.microedition.media.control.RateControl {

	
	/**
	 * 
	 * @param aPlayer
	 */
	public RateControl(Player aPlayer){
		this.iPlayer=aPlayer;
	}
	
	/* (non-Javadoc)
	 * @see javax.microedition.media.control.RateControl#getMaxRate()
	 */
	public int getMaxRate() {
		checkState();
		return 0;
	}

	/* (non-Javadoc)
	 * @see javax.microedition.media.control.RateControl#getMinRate()
	 */
	public int getMinRate() {
		checkState();
		return 0;
	}

	/* (non-Javadoc)
	 * @see javax.microedition.media.control.RateControl#getRate()
	 */
	public int getRate() {
		checkState();
		return 0;
	}

	/* (non-Javadoc)
	 * @see javax.microedition.media.control.RateControl#setRate(int)
	 */
	public int setRate(int aMillirate) {
		checkState();
		return 0;
	}
}
