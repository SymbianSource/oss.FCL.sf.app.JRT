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
* Description: AnimationPlayer
*
*/
package com.nokia.microedition.media.animation;

import java.io.ByteArrayOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.util.Enumeration;
import java.util.Hashtable;

import javax.microedition.media.Control;
import javax.microedition.media.MediaException;
import javax.microedition.media.PlayerListener;
import javax.microedition.media.protocol.DataSource;

import org.eclipse.swt.SWT;
import org.eclipse.swt.SWTException;
import org.eclipse.swt.events.PaintEvent;
import org.eclipse.swt.events.PaintListener;
import org.eclipse.swt.graphics.GC;
import org.eclipse.swt.graphics.Image;
import org.eclipse.swt.graphics.ImageData;
import org.eclipse.swt.graphics.ImageLoader;
import org.eclipse.swt.graphics.Point;
import org.eclipse.swt.widgets.Display;

import com.nokia.microedition.media.BufferDataSource;
import com.nokia.microedition.media.InputStreamDataSource;
import com.nokia.microedition.media.InputStreamSourceStream;
import com.nokia.microedition.media.PlayerBase;
import com.nokia.microedition.media.PlayerListenerImpl;

public class AnimationPlayer extends PlayerBase implements ESWTinitializeListener
{
    // GIF image information, array length will be equal to the number of frames in image
    protected ImageData [] iImageData;
    // number of times we need to repeat the animation
    // by default it's value is one
    private int iTotalLoopCount=1;
    // HashTable object which contains all control related to this player
    private Hashtable iControls= new Hashtable();
    // Current frame index of the Animation file
    private int iFrameIndex;
    // Current Loop Count
    private int iCurrentLoopCount;
    // Indicates if looping indefinitely
    private boolean iRepeatForeEver;
    // Current rate of the player, Used for RateControl
    // Currently only two mode are supported
    // paused and default rate
    private int iCurrentRate;
    // Display object of the eSWT API, creating this object will integrate the current java code with eSWT API.
    private Display iDisplay;
    // Control object of ESWT API, return from LCDUIInvoker.
    org.eclipse.swt.widgets.Control iControl;
    // Following constants are used for comparison of the string, throughout the class
    private static final String ANIMATION_CONTENT_TYPE      = "image/gif";
    private static final String fVideoControl               = VideoControl.class.getName();
    private static final String fFramePositioningControl    = FramePositioningControl.class.getName();
    private static final String fStopTimeControl            = StopTimeControl.class.getName();
    private static final String fRateControl                = RateControl.class.getName();

    /**
     * Default control package. Used when getting control with
     * getControl method which appends default control package if package is
     * not specified.
     */
    private static final String CONTROL_DEFAULT_PACKAGE =
        "javax.microedition.media.control.";

    //For Player listener
    protected PlayerListenerImpl iPlayerListenerImpl;
    //Image to be displayed, an object of eSWT API.
    protected Image iImage;
    // Actual dimension of the image, this should be initialized while creating the player
    // as user can change the size of the image later, in that case too, getSourceheight and getSourceWidth
    // of VideoControl should return the actual width and height of the image
    private Point iSourceDimension;
    // Current dimension of the image, MIDlet developer may change the size of VideoControl
    private Point iCurrentVideoDimension;
    // Total time taken so far to player the animation, it keeps updating as player moves
    private long iMediaTime;
    // Time at which, player should be stopped
    // This will set through StopTimeControl.setTime();
    // otherwise default time is Long.MAX_VALUE
    //private long iStopTime=Long.MAX_VALUE;
    AnimationObserver iAnimationObserver;

    // Display Location, of the image
    // there won't be any use of this, in case of Form(customItem)
    private Point iDisplayLocation= new Point(0,0);
    // in case of Canvas(USE_DIRECT_VIDEO) , it should be invisible by default
    // this is handled in initDisplayMode function, if the mode is USE_DIRECT_VIDEO, we are
    // changing it to false
    private boolean iIsControlVisible=true;
    // we keep the background pixel in it, used while taking the snapshot of the currentframe
    private int iBackgroundPixel=-1;
    // Total duration of the player
    private long iDuration=TIME_UNKNOWN;
    /**
     *
     * @param ds DataSource which contains the data to be displayed
     * @throws MediaException
     */
    public AnimationPlayer(DataSource ds) throws MediaException
    {
        iPlayerListenerImpl= new PlayerListenerImpl(this);
        //TODO check if we can do it in better way
        // this is temporary solution
        // for this I have written two functions getDataSource and getInputStream function
        BufferDataSource bds =(BufferDataSource)ds;
        InputStreamDataSource isds=(InputStreamDataSource)bds.getDataSource();
        InputStreamSourceStream isss=(InputStreamSourceStream)isds.getStreams()[0];
        InputStream is = isss.getInputStream();
        if (is!=null)
        {
            ImageLoader imageLoader= new ImageLoader();

            // If it is any other format other than the image(jpeg, png and gif),
            // following line will throw the SWT exception
            iImageData=imageLoader.load(is);
            // If the image is loaded properly, we need to check whether it is GIF image or not.
            // It can be PNG and JPEG as well
            if (iImageData[0].type!=SWT.IMAGE_GIF)
                throw new MediaException("Could not create player");
            try
            {
                is.close();
            }
            catch (IOException e)
            {
                e.printStackTrace();
            }
            iSourceDimension= new Point(imageLoader.logicalScreenWidth, imageLoader.logicalScreenHeight);
            iCurrentVideoDimension= new Point(imageLoader.logicalScreenWidth, imageLoader.logicalScreenHeight);
            iBackgroundPixel= imageLoader.backgroundPixel;
        }
        populateControl();

    }

    /**
     *
     * @param locator
     * @throws SWTException
     */
    public AnimationPlayer(String locator) throws SWTException, MediaException
    {
        ImageLoader imageLoader= new ImageLoader();
        // If it is any other format other than the image(jpeg, png and gif),
        // following line will throw the SWT exception
        iImageData=imageLoader.load(locator);
        // If the image is loaded properly, we need to check whether it is GIF image or not.
        // It can be PNG and JPEG as well
        if (iImageData[0].type!=SWT.IMAGE_GIF)
            throw new MediaException("Could not create player");
        iPlayerListenerImpl= new PlayerListenerImpl(this);
        iSourceDimension= new Point(imageLoader.logicalScreenWidth, imageLoader.logicalScreenHeight);
        iCurrentVideoDimension= new Point(imageLoader.logicalScreenWidth, imageLoader.logicalScreenHeight);
        iBackgroundPixel= imageLoader.backgroundPixel;
        populateControl();
    }

    /**
     * Moved the player to close state and releases all resources, called from PlayerBase class
     */
    protected void doClose()
    {
        // state is already changed in playerbase.close() method
        //      changePlayerState(CLOSED);
        iPlayerListenerImpl.postEvent(PlayerListener.CLOSED, null);
    }

    /**
     * Called from the PlayerBase class if the Player is in pre-fetched state
     *
     */
    protected void doDeallocate()
    {
        //when the player is in pre-fetched state, calling this
        //function should change the state of the player to RELAIZED state
        changePlayerState(REALIZED);
    }

    protected void doPrefetch() throws MediaException
    {
        changePlayerState(PREFETCHED);
        iDuration= getMediaTimeForFrame(getTotalNumberFrames());
    }

    protected void doRealize() throws MediaException
    {
        changePlayerState(REALIZED);
    }

    /**
     *
     */
    protected void doStop() throws MediaException
    {
        // since after stopping the player the player state will move to pre-fetched state
        changePlayerState(PREFETCHED);
        //iPlayerListenerImpl.postEvent(PlayerListener.STOPPED, new Long(iMediaTime * 10000));
        iPlayerListenerImpl.postEvent(PlayerListener.STOPPED, new Long(iMediaTime));
    }

    /**
     * Function of InternalPlayer interface
     */
    public void addControl(Control aControl, String aControlType)
    {
        iControls.put(aControlType, aControl);
    }

    /**
     *
     */
    public void addPlayerListener(PlayerListener aPlayerListener)
    {
        closeCheck();
        iPlayerListenerImpl.addPlayerListener(aPlayerListener);
    }

    /**
     * Returns the Content type("image/GIF") supported for by this player,
     */
    public String getContentType()
    {
        closeCheck();
        unrealizedCheck();
        return ANIMATION_CONTENT_TYPE;
    }

    /**
     * This function will return total time in microseconds, this player can be played
     */
    public long getDuration()
    {
        closeCheck();
        return iDuration;
    }

    /**
     * This returns the total time taken, till now, to play the video.
     */
    public long getMediaTime()
    {
        closeCheck();
        return iMediaTime;
    }

    public int getState()
    {
        return iState;
    }

    /**
     * Removes the specified playerListner from this player
     */
    public void removePlayerListener(PlayerListener aPlayerListener)
    {
        closeCheck();
        iPlayerListenerImpl.removePlayerListener(aPlayerListener);
    }

    /**
     *
     */
    GC gc = null;
    public void start() throws MediaException
    {
        final String DEBUG_STR = "AnimationPlayer::start()";
//        final long inTime = System.currentTimeMillis();
        prefetch();
        // Only prefetched player may be started. If player is already started
        // this method returns silently.
        if (getState() == PREFETCHED)
        {
            //initialize();
            changePlayerState(STARTED);
            Thread thread = new Thread("Animation")
            {
                int loopCount = iCurrentLoopCount;
                public void run()
                {
                    //changePlayerState(STARTED);
                    final int noOfFrames = iImageData.length;
                    while (iState == STARTED)
                    {
                        final int delayTimeForNextFrame = iImageData[iFrameIndex].delayTime*10000 ;
                        // Since we are going to display first frame, notify all
                        // PlayerListener that Player has started
                        if (iFrameIndex == 0)
                        {
                            iMediaTime=0;
                            iPlayerListenerImpl.postEvent(
                                PlayerListener.STARTED, new Long(0));

                        }
                        if (iDisplay != null)
                        {
                            iDisplay.syncExec(new Runnable()
                            {
                                public void run()
                                {
                                    // For out of memory issue in case of full screen, we are scaling the image
                                    // while displaying it.
                                    ImageData tempImageData =iImageData[iFrameIndex] ;
                                    if (iSourceDimension.x!=iCurrentVideoDimension.x || iSourceDimension.x!=iCurrentVideoDimension.x)
                                    {
                                        tempImageData = iImageData[iFrameIndex].scaledTo(iCurrentVideoDimension.x , iCurrentVideoDimension.y);
                                    }
                                    Image tempImage = new Image(iDisplay,tempImageData);
                                    //Image tempImage = new Image(iDisplay,iImageData[iFrameIndex]);
                                    gc.drawImage(tempImage, 0, 0);
                                    tempImage.dispose();
                                    iFrameIndex = (iFrameIndex + 1) % noOfFrames;
                                    if (iControl != null)
                                    {
                                        iControl.redraw();
                                    }
                                    iMediaTime += delayTimeForNextFrame;
                                }
                            });
                        }
                        else// if the initDisplayMode is not called yet
                        {
                            iFrameIndex = (iFrameIndex + 1) % noOfFrames;
                            iMediaTime += delayTimeForNextFrame;
                        }
                        try
                        {
                            Thread.sleep(delayTimeForNextFrame / 1000);
                        }
                        catch (InterruptedException e)
                        {
                            e.printStackTrace();
                        }
                        // post EOM event
                        if (iFrameIndex == 0)
                        {
                            loopCount++;
                            if (!(iRepeatForeEver || (loopCount < iTotalLoopCount)))
                            {
                                // when this loop is getting braked, we need to change the state to pre-fetched
                                //TODO Player should change it's state in doStop();
                                changePlayerState(PREFETCHED);
                            }
                            // send the END_OF_MEDIA event to all
                            // listener
                            iPlayerListenerImpl.postEvent(
                                PlayerListener.END_OF_MEDIA,
                                new Long(iMediaTime));
                        }
                        if (iAnimationObserver!=null)
                        {
                            iAnimationObserver.animationAdvanced(iMediaTime);
                        }
                    }// end of while loop
                    iCurrentLoopCount = loopCount;
                }
            };
            thread.start();
        }
    }

    /**
     * State of the player should be changed from this function only.
     * @param aState
     */
    private synchronized void  changePlayerState(int aState)
    {
        iState=aState;
    }
    /**
     * On each repaint event, control is getting disposed and created each time,
     * So we need to call each time the Control is available
     * @param aControl
     */
    private void addPaintListener(org.eclipse.swt.widgets.Control aControl)
    {
        iControl=aControl;
        if (iControl != null)
        {
            iDisplay.syncExec(new Runnable()
            {
                public void run()
                {
                    iControl.addPaintListener(new PaintListener()
                    {
                        public void paintControl(PaintEvent pe)
                        {
                            if (iImage != null && iIsControlVisible)
                            {
                                pe.gc.drawImage(iImage, iDisplayLocation.x,
                                                iDisplayLocation.y);
                            }
                        }
                    });
                }
            });
        }
    }


    /**
     * This returns player Control.
     *
     * @param controlType - the class name of the Control. The class name should be given either
     * as the fully-qualified name of the class; or if the package of the class is not given, the
     * package javax.microedition.media.control is assumed.
     *
     *  @return the object that implements the control, or null.
     */
    public Control getControl(String aControlType)
    {
        if (aControlType == null)
        {
            throw new IllegalArgumentException("argument is null");
        }
        closeCheck();
        unrealizedCheck();

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
        if (control == null)
        {
            try
            {
                // try to create class for control
                Class controlClass = Class.forName(controlType);
                Enumeration elements = iControls.elements();
                // search if any control is same type that requested control
                while (elements.hasMoreElements())
                {
                    Control tmpControl = (Control) elements.nextElement();
                    if (controlClass.isInstance(tmpControl))
                    {
                        // control is found
                        control = tmpControl;
                    }
                }
            }
            catch (ClassNotFoundException cnfe) // the class could not be
                // found
            {
                // Exception is ignored and null is returned from this method
            }
            catch (Error e) // the function failed for any other reason.
            {
                // Error is ignored and null is returned from this method
            }
        }
        return control;
    }
    /**
     * Returns all the control associated with this player
     */
    public Control[] getControls()
    {
        closeCheck();
        unrealizedCheck();
        Control control[]= new Control[iControls.size()];
        Enumeration enumeration= iControls.elements();
        byte index=0;
        while (enumeration.hasMoreElements())
        {
            control[index++]=(Control)enumeration.nextElement();
        }
        return control;
    }
    /**
     *
     */
    public void setLoopCount(int aCount)
    {
        iRepeatForeEver = (aCount == -1);
        super.setLoopCount(aCount);
        iTotalLoopCount=aCount;
    }
    /**
     * From PlayerBase
     * Here iFrameIndex variable will be updated according to the argument(aNow) supplied
     * We are updating the iFrameIndex variable, which is being used in the start function
     *
     * @see PlayerBase
     */
    public long setMediaTime(long aNow) throws MediaException
    {
        long now = super.setMediaTime(aNow);
        if (iDuration == TIME_UNKNOWN)
            iDuration=getMediaTimeForFrame(getTotalNumberFrames());
        if (now >= iDuration)
        {
            iFrameIndex=getTotalNumberFrames()-1;
            iMediaTime = iDuration;
        }
        else
        {
            iFrameIndex = findFrame(now);
            iMediaTime= getMediaTimeForFrame(iFrameIndex);
        }
        return iMediaTime ;
    }
    //////////////////////////////////////////////////////////////////////////////////////
    // Following functions are for internal use, and not exposed to MIDlet developer//////
    /////////////////////////////////////////////////////////////////////////////////////

    /**
     * Before calling this function check frameIndex is in range or not, from 0 to last index
     */
    long getMediaTimeForFrame(int aFrameIndex)
    {
        long time=0;
        for (int i=0; i < aFrameIndex; i++)
        {
            time+=iImageData[i].delayTime;
        }
        return time*10000;
    }

    /**
     * Utility function to calculate the framNumber
     * @param aTime
     * @return
     */
    int findFrame(long aTime)
    {
        long time=0;
        int frameIndex=0;
        if (aTime > iDuration)
            return -1;
        else if (aTime==0)
            return 0;
        int totalNoOfFrame= getTotalNumberFrames();
        while (time *10000 < aTime && frameIndex < totalNoOfFrame)
        {
            time += iImageData[frameIndex++].delayTime;
        }
        return frameIndex;
    }
    /**
     * This function is responsible for creating all controls and adding it into Controls hashtable.
     */
    private void populateControl()
    {
        // there are four control provided by AnimationPlayer
        // adding all one by one to the controlList(iControls)
        addControl(new VideoControl(this), fVideoControl);
        addControl(new FramePositioningControl(this), fFramePositioningControl);
        addControl(new StopTimeControl(this), fStopTimeControl);
        addControl(new RateControl(this), fRateControl);
    }

    /**
     *  Notified when the display object of ESWT is created
     *  It is getting created, when initDisplayMode of VideoControl class is called.
     */
    public void notifyDisplayAvailable(Display aDisplay)
    {
        iDisplay= aDisplay;
        //iImage=new Image(iDisplay, iImageData[0]);
        iImage=new Image(iDisplay, iImageData[iFrameIndex]);
        iDisplay.syncExec(new Runnable()
        {
            public void run()
            {
                gc = new GC(iImage);
            }
        });
    }
    /**
     * Notified when the control is available
     * @param aControl(this is eSWTControl )
     */
    public void notifyControlAvailable(org.eclipse.swt.widgets.Control aControl)
    {
        iControl=aControl;
        addPaintListener(iControl);
    }



    /**
     * This function will be called from setDisplaySize(int width, int height) of animation/VideoControl class
     * When MIDlet developer will try to alter the size of the image
     * @param width : to be set of the video(Animated GIF)
     * @param height : height to be set of video(Animated GIF)
     */
    void updateImageData(int aWidth, int aHeight)
    {
//        int noOfFrames= iImageData.length;
//        for (int i=0; i<noOfFrames; i++)
//        {
//            iImageData[i]=iImageData[i].scaledTo(aWidth, aHeight);
//        }
//        iImage=new Image(iDisplay, iImageData[iFrameIndex]);
        iCurrentVideoDimension.x=aWidth;
        iCurrentVideoDimension.y= aHeight;
        iImage=new Image(iDisplay, iImageData[iFrameIndex].scaledTo(aWidth, aHeight));
    }

    /**
     * Overloaded function for calling the above function
     */
    void updateImageData(Point aSize)
    {
        updateImageData(aSize.x, aSize.y);
    }

    /**
     * This function will be called from getSnapshot() function of VideoControl class
     * to get the snap shot of the video
     *
     * @param format
     */
    // This function is not implemented fully
    byte[] getCurrentFrame(String format)
    {
        // create a new ImageLoader object
        ImageLoader il= new ImageLoader();
        // create a ImageData array of length 1 and assign it to 'data' member of ImageLoader
        il.data= new ImageData[1];
        int currentFrameIndex= iFrameIndex;
        // assign the current frame ImageData to image
        il.data[0]= iImageData[currentFrameIndex];
        il.logicalScreenWidth  = iImageData[currentFrameIndex].width;
        il.logicalScreenHeight = iImageData[currentFrameIndex].height;
        il.backgroundPixel= iBackgroundPixel;
        ByteArrayOutputStream baos= new ByteArrayOutputStream();
        il.save(baos, SWT.IMAGE_PNG);
        //il.save(baos, SWT.IMAGE_GIF);
        return baos.toByteArray();
    }

    /**
     * Returns Point object, which contains the width and height of the image
     * Called from VideoControl to get the image width and height,
     * so that Item will be created exactly of same dimension
     */
    Point getCurrentVideoDimension()
    {
        return iCurrentVideoDimension;
        // return new org.eclipse.swt.graphics.Point(iImageData[0].width, iImageData[0].height);
    }


    int getTotalNumberFrames()
    {
        return iImageData.length;
    }
    /**
     *
     * @param aDisplayLocation x,y coordinate where image is to be displayed
     */
    void setDisplayLocation(int aX, int aY)
    {
        iDisplayLocation.x=aX;
        iDisplayLocation.y=aY;
    }
    /**
     * @return the position of the image to be displayed
     */
    Point getiDisplayLocation()
    {
        return iDisplayLocation;
    }
    /**
     * Called from VideoControl to get the dimension of original image size
     * @return
     */
    Point getSourceDimension()
    {
        return iSourceDimension;
        //return new Point(iImageData[0].width, iImageData[0].height);
    }

    /**
     * @return the iPlayerListenerImpl
     */
    PlayerListenerImpl getiPlayerListenerImpl()
    {
        return iPlayerListenerImpl;
    }

    /**
     * @return the iFrameIndex
     */
    int getiFrameIndex()
    {
        return iFrameIndex;
    }

    /**
     * @return the iCurrentRate
     */
    int getiCurrentRate()
    {
        return iCurrentRate;
    }

    /**
     * @param aCurrentRate the iCurrentRate to set
     * @return actual rate set
     */
    int setiCurrentRate(int aCurrentRate)
    {
        // if the player is already started and was paused due to setRate
        // then we need to start the player again from the same frame
        if (iState == STARTED &&  iCurrentRate != aCurrentRate)
        {
            if (aCurrentRate<=0)
            {
                // pause the player
                // following line will break the while loop in start method
                // Objective here is to pause the animation, if it is in started state
                // also we do not need to notify to the playerListener that player has been stopped or paused
                changePlayerState(PREFETCHED);
            }
            else
            {
                //start the player
                // this will start playing animation from the very same frame
                // where it was paused due to setRate(0)
                try
                {
                    start();
                }
                catch (MediaException e)
                {
                    // ignore the exception
                    e.printStackTrace();
                }
            }
        }
        this.iCurrentRate = aCurrentRate;
        return iCurrentRate;
    }

    /**
     * @param iIsControlVisible the iIsControlVisible to set
     */
    void setiIsControlVisible(boolean iIsControlVisible)
    {
        this.iIsControlVisible = iIsControlVisible;
    }
    /**
     * Called from StopTimeControl.animationAdvanced(long), once the stopped time is greater than or equal media time.
     */
    void postEvent(long aMediaTime)
    {
        changePlayerState(PREFETCHED);
        iPlayerListenerImpl.postEvent(PlayerListener.STOPPED_AT_TIME, new Long(
                                          aMediaTime));
    }

    /**
     * @param aAnimationObserver the iAnimationObserver to set
     */
    void setiAnimationObserver(AnimationObserver aAnimationObserver)
    {
        this.iAnimationObserver = aAnimationObserver;
    }


}

