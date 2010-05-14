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

import java.io.InputStream;
import java.util.Enumeration;
import java.util.Hashtable;

import javax.microedition.media.Control;
import javax.microedition.media.MediaException;
import javax.microedition.media.PlayerListener;
import javax.microedition.media.protocol.DataSource;

import org.eclipse.ercp.swt.mobile.MobileShell;
import org.eclipse.swt.SWTException;
import org.eclipse.swt.events.PaintEvent;
import org.eclipse.swt.events.PaintListener;
import org.eclipse.swt.graphics.GC;
import org.eclipse.swt.graphics.Image;
import org.eclipse.swt.graphics.ImageData;
import org.eclipse.swt.graphics.ImageLoader;
import org.eclipse.swt.graphics.Point;
import org.eclipse.swt.widgets.Display;
import org.eclipse.swt.widgets.Shell;

import com.nokia.microedition.media.BufferDataSource;
import com.nokia.microedition.media.InputStreamDataSource;
import com.nokia.microedition.media.InputStreamSourceStream;
import com.nokia.microedition.media.Locator;
import com.nokia.microedition.media.PlayerBase;
import com.nokia.microedition.media.PlayerListenerImpl;

/**
 * @author d35kumar
 *
 */
public class AnimationPlayer extends PlayerBase {
	// GIF image information, array length will be equal to the number of frames in image 
	protected ImageData [] iImageData;
	// number of times we need to repeat the animation
	// by default it's value is one 
	private int iTotalLoopCount=1;
	// this holds all control related to this player 
	Hashtable iControls= new Hashtable();
	// Current frame index of the Animation file
	private int iFrameindex;
	// Current Loop Count 
	private int iCurrentLoopCount;
	// Display object of the eSWT API, creating this object will integrate the current java code with eSWT API. 
	Display iDisplay; 
	// Control object of ESWT API, return from LCDUIInvoker. 
	org.eclipse.swt.widgets.Control iControl;
	// Constants used everywhere to compare the string 
	private static final String ANIMATION_CONTENT_TYPE = "image/gif";
	
	private final String fVideoControl=VideoControl.class.getName();
	private final String fFramePositioningControl=FramePositioningControl.class.getName();
	private final String fStopTimeControl=StopTimeControl.class.getName();
	private final String fRateControl=RateControl.class.getName();
	 /**
     * Default control package. Used when getting control with
     * getControl method which appends default control package if package is
     * not specified.
     */
    static final String CONTROL_DEFAULT_PACKAGE =
        "javax.microedition.media.control.";
    
	//For Player listener 
	protected PlayerListenerImpl iPlayerListenerImpl;
	//Image to be displayed, again object of eSWT API. 
	protected Image iImage;
	// Actual dimension of the image, this should be initialized while creating the player
	// as user can change the size of the image later, in that case too, getSourceheight and getSourceWidth 
	// of VideoControl should return the actual width and height of the image 
	Point iSourceDimension;
	long iMediaTime;
	
	// Display Location, of the image   
	private Point iDisplayLocation= new Point(0,0);
	

	/**
	 * 
	 * @param ds DataSource which contains the data to be displayed 
	 */
	public AnimationPlayer(DataSource ds){
		final String DEBUG_STR="AnimationPlayer::AnimationPlayer(DataSource)";
		System.out.println(DEBUG_STR+"+");
		iPlayerListenerImpl= new PlayerListenerImpl(this);
		//TODO check if we can do it in better way 
		// this is temporary solution
		// for this I have written two functions getDataSource and getInputStream function 
		BufferDataSource bds =(BufferDataSource )ds;
		InputStreamDataSource isds=(InputStreamDataSource)bds.getDataSource();
		InputStreamSourceStream isss=(InputStreamSourceStream )isds.getStreams()[0];
		InputStream is = isss.getInputStream();
		
		if(is!=null){
			ImageLoader imageLoader= new ImageLoader();
			iImageData=imageLoader.load(is);
			//iRepeatCount=imageLoader.repeatCount;
			iSourceDimension= new Point(imageLoader.logicalScreenWidth, imageLoader.logicalScreenHeight);
		}
		pupulateControl();
		System.out.println(DEBUG_STR+"-");
	}
	
	/**
	 * 
	 * @param locator
	 * @throws SWTException
	 */
	public AnimationPlayer(String locator) throws SWTException{
		final String DEBUG_STR="AnimationPlayer::AnimationPlayer(Locator )";
		System.out.println(DEBUG_STR+"+");
		ImageLoader imageLoader= new ImageLoader();
		System.out.println(DEBUG_STR+"Locator string is "+locator);
		// Following line may throw SWTException 
		iImageData=imageLoader.load(locator);
		//iRepeatCount=imageLoader.repeatCount;
		iSourceDimension= new Point(imageLoader.logicalScreenWidth, imageLoader.logicalScreenHeight);
		pupulateControl();
		System.out.println(DEBUG_STR+"-");
	}
	
	/**
	 * Moved the player to close state and releases all resources, called from PlayerBase class  
	 */
	protected void doClose() {
		final String DEBUG_STR="AnimationPlayer::doClose()";
		System.out.println(DEBUG_STR+"+");
		iState=CLOSED;
		iPlayerListenerImpl.postEvent(PlayerListener.CLOSED, null);
		System.out.println(DEBUG_STR+"-");
	}

	protected void doDeallocate() {
		// what to do here, nullify image data and display etc???
	}

	protected void doPrefetch() throws MediaException {
		final String DEBUG_STR="AnimationPlayer::doPrefetch()";
		System.out.println(DEBUG_STR+"+");
		iState=PREFETCHED;
		System.out.println(DEBUG_STR+"-");
	}

	protected void doRealize() throws MediaException {
		final String DEBUG_STR="AnimationPlayer::doRealize()";
		System.out.println(DEBUG_STR+"+");
		iState=REALIZED;
		// this is temporary solution implement it in proper way 
		iImage=new Image(iDisplay, iImageData[0]);
		System.out.println(DEBUG_STR+"-");
	}

	protected void doStop() throws MediaException {
		final String DEBUG_STR="AnimationPlayer::doStop()";
		System.out.println(DEBUG_STR+"+");
		// since after stopping the player the player state will move to pre-fetched state 
		iState=PREFETCHED;
		iPlayerListenerImpl.postEvent(PlayerListener.STOPPED, new Long(iMediaTime * 10000));
		System.out.println(DEBUG_STR+"-");
	}

	public void addControl(Control aControl, String aControlType)
			throws MediaException {
		iControls.put(aControlType, aControl);
	}

	public void addPlayerListener(PlayerListener aPlayerListener) {
		closeCheck();
        iPlayerListenerImpl.addPlayerListener(aPlayerListener);
	}
	
	/**
	 * Returns the Content type("image/GIF") supported for by this player, 
	 */
	public String getContentType() {
		closeCheck();
        unrealizedCheck();
		return ANIMATION_CONTENT_TYPE;
	}
	
	/**
	 * This function will return, total time in microseconds this player can be played 
	 */
	public long getDuration() {
		closeCheck();
		long time = TIME_UNKNOWN;
		int totalNoOfFrames = iImageData.length;
		for (int i = 0; i < totalNoOfFrames; i++) {
			time += iImageData[i].delayTime;
		}
		// Since we have to return it in microsecond multiply it with 1000; 
		return time * 10000;
	}
	
	/**
	 * This returned the total time taken, till now, to play the video.  
	 */
	public long getMediaTime() {
		return iMediaTime*10000;
	}

	public int getState() {
		return iState;
	}
	
	/**
	 * Removes the specified playerListner from this player  
	 */
	public void removePlayerListener(PlayerListener aPlayerListener) {
		closeCheck();
        iPlayerListenerImpl.removePlayerListener(aPlayerListener);
	}
	public void start() throws MediaException {
		final String DEBUG_STR = "AnimationPlayer::start()";
//		prefetch();
		initialize();
		addPaintListener(iControl);
		iState = STARTED;
		Thread thread = new Thread("Animation") {
			int frameIndex = iFrameindex;
			int loopCount = iCurrentLoopCount;
			GC gc=null;
			public void run() {
				final int noOfFrames = iImageData.length;
				while (frameIndex < noOfFrames && (loopCount < iTotalLoopCount)
						&& (iState == STARTED)) {
					final int delayTimeForNextFrame = iImageData[frameIndex].delayTime;
					System.out.println("\n\n\n************" + DEBUG_STR
							+ "Inside the while loop " + frameIndex + "/"
							+ noOfFrames);
					// Since we are going to display first frame, notify all
					// PlayerListener that Player has started
					if (frameIndex == 0) {
						iPlayerListenerImpl.postEvent(PlayerListener.STARTED,
								new Long(iMediaTime * 10000));
					}
					iDisplay.asyncExec(new Runnable() {
						public void run() {
							System.out.println(DEBUG_STR+"Inside run method");
							try {
								if (gc == null)gc = new GC(iImage);
							} catch (Exception e) {
								// TODO Auto-generated catch block
								e.printStackTrace();
							}
							System.out.println(DEBUG_STR+"Inside run method246");
							Image tempImage = new Image(iDisplay,
									iImageData[frameIndex]);
							System.out.println(DEBUG_STR+"Inside run method246");
							gc.drawImage(tempImage, 0, 0);
							tempImage.dispose();
							frameIndex = (frameIndex + 1) % noOfFrames;
							System.out.println(DEBUG_STR+"Inside run method253>>>>>>>>>"+frameIndex);
							iControl.redraw();
							// update the mediaTime, as Animation progress
							iMediaTime += delayTimeForNextFrame;
							// If imageIndex becomes zero it means, all frames
							// has been displayed
							// So increase the loopCount
							if (frameIndex == 0) {
								// send the END_OF_MEDIA event to all listener
								iPlayerListenerImpl.postEvent(
										PlayerListener.END_OF_MEDIA, new Long(
												iMediaTime * 10000));
								loopCount++;
								// since player is again going to start from the
								// first frame
								// so media time should be set to zero
								iMediaTime = 0;
							}
							System.out
									.println(DEBUG_STR
											+ "end of asynchronous block imageIndex is "
											+ frameIndex + " and loop count is"
											+ loopCount );
						}
					});
					try {
						Thread.sleep(delayTimeForNextFrame * 10);
					} catch (InterruptedException e) {
						// TODO Auto-generated catch block
						e.printStackTrace();
					}
					System.out
							.println("**********************End of while loop\n\n\n");
				}
				iCurrentLoopCount=loopCount;
				System.out.println(DEBUG_STR + "Came out side the while loop "
						+ iState);
			}
		};
		thread.start();
		System.out.println(DEBUG_STR + "-");
		// }catch(Exception e){
		// System.out.println(DEBUG_STR+"Exception caught"+e);
		// e.printStackTrace();
		// }
	}
	
	/**
	 * This function is also being called from VideoControl class, 
	 * Since on each event 
	 * @param aControl
	 * @throws MediaException 
	 */
	 void addPaintListener(org.eclipse.swt.widgets.Control aControl) throws MediaException{
		 final String DEBUG_STR="AnimationPlayer::addPaintListener()";
		 iControl=aControl;
		// iDisplay and IControl shouldn't be null here 
		System.out.println(DEBUG_STR+" Control is "+iControl+" Display "+iDisplay);
		// Following line should never execute
		// TODO check what message to provide in case of Exception 
		if(iControl==null)
			throw new MediaException("Update this message");
		iDisplay.syncExec(new Runnable(){
			public void run() {
				// if user is writing his MIDlet using eSWT API, then the Control object will be MobileShell
				// and in that case it is mandatory to open the shell
//				if(iControl instanceof MobileShell){
				if(iControl instanceof Shell){
					((Shell)iControl).open();
				}
				iControl.addPaintListener(new PaintListener(){
					public void paintControl(PaintEvent pe) {
//						System.out.println(DEBUG_STR+"paintControl() paintEvent is "+pe);
//						System.out.println(DEBUG_STR+"paintControl() Display location is "+iDisplayLocation.x+", "+iDisplayLocation.y);
						if(iImage!=null){
							pe.gc.drawImage(iImage, iDisplayLocation.x, iDisplayLocation.y);
						}
					}
				});
			}
		});
	}
	/**
	 * 
	 */
	public Control getControl(String aControlType) {
		if (aControlType == null) {
			throw new IllegalArgumentException("argument is null");
		}
		
		String controlType = aControlType;

        // check if package name exists
        if (controlType.indexOf(".") == -1)
        {
            // add package name
            controlType = CONTROL_DEFAULT_PACKAGE + aControlType;
        }
        Control control = (Control)iControls.get(controlType);
        // If control does not exists with default name, check if there is
        // is a control with same type ( extends the given class name ).
		if (control == null) {
			try {
				// try to create class for control
				Class controlClass = Class.forName(controlType);
				Enumeration elements = iControls.elements();
				// search if any control is same type that requested control
				while (elements.hasMoreElements()) {
					Control tmpControl = (Control) elements.nextElement();
					if (controlClass.isInstance(tmpControl)) {
						// control is found
						control = tmpControl;
					}
				}
			} catch (ClassNotFoundException cnfe) // the class could not be
													// found
			{
				// Exception is ignored and null is returned from this method
			} catch (Error e) // the function failed for any other reason.
			{
				// Error is ignored and null is returned from this method
			}
		}
        return control;
	}
	/**
	 * Returns all the control associated with this player 
	 */
	public Control[] getControls() {
		Control control[]= new Control[iControls.size()];
		Enumeration enumeration= iControls.elements();
		byte index=0;
		while(enumeration.hasMoreElements()){
			control[index++]=(Control)enumeration.nextElement();
		}
		return control;
	}
	/**
	 * 
	 */
	public void setLoopCount(int aCount){
		super.setLoopCount(aCount);
		iTotalLoopCount=aCount;
	}
	/**
     * From PlayerBase
     * @see PlayerBase
     */
	public long setMediaTime(long aNow) throws MediaException {
		long now = super.setMediaTime(aNow);
		int totalFrames = iImageData.length;
		int totalTime = 0;
		for (int i = 0; i < totalFrames; i++) {
			if (now < totalTime) {
				iFrameindex=i;
				break;
			}
			totalTime += iImageData[i].delayTime * 10000;
		}
		return totalTime;
	}
	//////////////////////////////////////////////////////////////////////////////////////
	// Following functions are for internal use, and not exposed to MIDlet developer//////
	/////////////////////////////////////////////////////////////////////////////////////
	/**
	 * 
	 */
	private void doStart(){
		
	}
	
	
    /**
	 * This function is responsible for creating all controls and adding it into Controls hashtable.
	 */
	private void pupulateControl(){
		final String DEBUG_STR="AnimationPlayer::pupulateControl()";
		System.out.println(DEBUG_STR+"+");
		VideoControl videoControl= new VideoControl(this);
		FramePositioningControl fpc= new FramePositioningControl(this);
		StopTimeControl stc= new StopTimeControl(this);
		RateControl rc= new RateControl(this);
		try {
			addControl(videoControl, fVideoControl);
			addControl(fpc, fFramePositioningControl);
			addControl(stc, fStopTimeControl);
			addControl(rc,fRateControl);
		} catch (MediaException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
		System.out.println(DEBUG_STR+"-");
	}
	/**
	 * This function initialize iControl and iDisplay object if it is null,
	 * otherwise return immediately.
	 * 
	 * In case of Canvas, eSWT control will be returned immediately from VideoControl(vc.getControl()), 
	 * but in case of CustomItem we need to keep polling, eSWT doesn't return the control for CustomItem 
	 * until CustomItem is appended to Form.      
	 */
	
	private void initialize() {
		final String DEBUG_STR = "AnimationPlayer::initialize()";
		System.out.println(DEBUG_STR + "+");
		System.out.println(DEBUG_STR + " Control is " + iControl
				+ " display is " + iDisplay);
		if (iControl == null || iDisplay == null) {
			VideoControl vc = (VideoControl) getControl(fVideoControl);
			iDisplay = vc.getiDisplay();
			// in case of CustomItem,
			while ((iControl = vc.getControl()) == null) {
				try {
					Thread.sleep(100);
				} catch (InterruptedException e) {
					e.printStackTrace();
				}
			}
		}
		System.out.println(DEBUG_STR + "-");
	}
	
	/**
	 * This function will be called from setDisplaySize(int width, int height) of animation/VideoControl class
	 * When MIDlet developer will try to alter the size of the image 
	 * @param width : to be set of the video(Animated GIF) 
	 * @param height : height to be set of video(Animated GIF) 
	 */
	void updateImageData(int width, int height){
		final String DEBUG_STR= "AnimationPlayer::updateImageData(int width, int height)";
		System.out.println(DEBUG_STR+"+");
		int noOfFrames= iImageData.length;
		for(int i=0;i<noOfFrames;i++){
			iImageData[i]=iImageData[i].scaledTo(width, height);
		}
		iImage=new Image(iDisplay, iImageData[0]);
		System.out.println(DEBUG_STR+"-");
	}
	
	/**
	 * This function will be called from getSnapshot() function of VideoControl class 
	 * to get the snap shot of the video 
	 * 
	 * @param format
	 */
	// This function is not implemented fully  
	int[] getCurrentFrame(String format){
		final String DEBUG_STR="AnimationPlayer::getCurrentFrame()";
		System.out.println(DEBUG_STR+"+");
		ImageData currentFrameImageData=iImage.getImageData();
		int data[][]= new int[currentFrameImageData.height][currentFrameImageData.width];
		System.out.println("Width of the current Image is "+data[0].length+" height "+data.length);
		
		int imageWidth=data[0].length;
		int imageHeight=data.length;
		System.out.println("Image Width "+imageWidth+", "+currentFrameImageData.width+" height "+imageHeight+", "+currentFrameImageData.height);
		
		for(int i=0;i<imageHeight;i++){
			currentFrameImageData.getPixels(0, i, imageWidth, data[i], 0);
			System.out.println(DEBUG_STR+"=========>"+data[i]);
		}
		System.out.println(DEBUG_STR+"After reading all pixel value");
		int[] byteArray=new int[imageWidth*imageHeight];
		//int dataLength=data.length;
		
		for(int i=0;i<imageHeight;i++){
			System.arraycopy(data[i], 0, byteArray, i*imageWidth, imageWidth);
		}
		System.out.println(DEBUG_STR+"After merging all array into single array ");
		System.out.println(DEBUG_STR+"-"+byteArray+">>>>>"+byteArray.length);
		return byteArray;
	}
	
	/**
	 * Returns Point object, which contains the width and height of the image
	 * Called from VideoControl to get the image width and height, 
	 * so that Item will be created exactly of same dimension 
	 */
	org.eclipse.swt.graphics.Point getImageDimension(){
		return new org.eclipse.swt.graphics.Point(iImageData[0].width, iImageData[0].height);  
	}
	
	/**
	 * This returns the imageData array, 
	 * called from FramePositioningControl class to calculate the frame time 
	 */
	ImageData[] getImageData(){
		return iImageData;
	}
	
	/**
	 * 
	 * @param aDisplayLocation x,y coordinate where image is to be displayed 
	 */
	void setDisplayLocation(int aX, int aY){
		iDisplayLocation=new Point(aX,aY);
	}
	
	/**
	 * @return the position of the image to be displayed 
	 */
	Point getiDisplayLocation() {
		return iDisplayLocation;
	}
	/**
	 * Called from VideoControl to get the dimension of original image size 
	 * @return
	 */
	Point getSourceDimension() {
		return iSourceDimension;
	}

	/**
	 * @return the iPlayerListenerImpl
	 */
	PlayerListenerImpl getiPlayerListenerImpl() {
		return iPlayerListenerImpl;
	}
}
