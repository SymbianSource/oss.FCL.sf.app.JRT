/**
 * 
 */
package com.nokia.microedition.media.animation;

import javax.microedition.media.Player;

import org.eclipse.swt.graphics.ImageData;

import com.nokia.microedition.media.control.ControlImpl;

/**
 * @author d35kumar
 *
 */
public class FramePositioningControl extends ControlImpl implements
		javax.microedition.media.control.FramePositioningControl {
	
	ImageData[] iImagedata;
	/**
	 * 
	 */
	 FramePositioningControl(Player aPlayer) {
		iPlayer=aPlayer;
		iImagedata=((AnimationPlayer)iPlayer).getImageData();
	}

	/* (non-Javadoc)
	 * @see javax.microedition.media.control.FramePositioningControl#mapFrameToTime(int)
	 */
	public long mapFrameToTime(int aFrameNumber) {
		checkState();
		long time=0;
		int totalNoOfFrames=iImagedata.length;
		// if invalid parameter is passed
		if(aFrameNumber<0 || aFrameNumber>totalNoOfFrames)
			return -1;
		for(int i=0;i<totalNoOfFrames;i++){
			if(i==aFrameNumber)
				break;
			time+=iImagedata[i].delayTime;
		}
		return time*10000;
	}

	/* (non-Javadoc)
	 * @see javax.microedition.media.control.FramePositioningControl#mapTimeToFrame(long)
	 */
	public int mapTimeToFrame(long aMediaTime) {
		checkState();
		int frameNumber=-1;
		int totalNoOfFrames=iImagedata.length;
		long time=0;
		for(int i=0;i<totalNoOfFrames;i++){
			if(time > aMediaTime){
				frameNumber=i-1;
				break;
			}
			time+=iImagedata[i].delayTime*10000;
		}
		return frameNumber;
	}

	/* (non-Javadoc)
	 * @see javax.microedition.media.control.FramePositioningControl#seek(int)
	 */
	public int seek(int aFrameNumber) {
		// TODO Auto-generated method stub
		return 0;
	}

	/* (non-Javadoc)
	 * @see javax.microedition.media.control.FramePositioningControl#skip(int)
	 */
	public int skip(int aFramesToSkip) {
		// TODO Auto-generated method stub
		return 0;
	}

}
