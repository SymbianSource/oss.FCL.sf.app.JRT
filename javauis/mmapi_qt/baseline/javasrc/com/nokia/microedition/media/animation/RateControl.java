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
