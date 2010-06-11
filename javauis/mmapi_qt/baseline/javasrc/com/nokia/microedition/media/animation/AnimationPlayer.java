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

import java.io.IOException;
import java.io.InputStream;
import java.util.Enumeration;
import java.util.Hashtable;

import javax.microedition.media.Control;
import javax.microedition.media.MediaException;
import javax.microedition.media.Player;
import javax.microedition.media.PlayerListener;
import javax.microedition.media.protocol.DataSource;

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
import com.nokia.mj.impl.nokialcdui.LCDUIInvoker;
import com.nokia.mj.impl.utils.Logger;

public class AnimationPlayer extends PlayerBase
{
    // GIF image information, array length will be equal to the number of frames in image
    protected ImageData [] iImageData;
    // number of times we need to repeat the animation
    // by default it's value is one
    private int iTotalLoopCount=1;
    // this holds all control related to this player
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
    // Total time taken so far to playe the animation
    private long iMediaTime;
    // Time at which, player should be stopped
    // This will set through StopTimeControl.setTime();
    private long iStopTime=Long.MAX_VALUE;

    // Display Location, of the image
    // there won't be any use of this, in case of Form(customItem)
    private Point iDisplayLocation= new Point(0,0);

    private boolean iIsControlVisible=true;
    /**
     *
     * @param ds DataSource which contains the data to be displayed
     */
    public AnimationPlayer(DataSource ds)
    {
        System.out.println("AnimationPlayer(DataSource ds) + ");
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
            iImageData=imageLoader.load(is);
            try
            {
                is.close();
            }
            catch (IOException e)
            {
                e.printStackTrace();
            }
            //iRepeatCount=imageLoader.repeatCount;
            iSourceDimension= new Point(imageLoader.logicalScreenWidth, imageLoader.logicalScreenHeight);
        }
        populateControl();
        System.out.println("AnimationPlayer(DataSource ds) - ");
    }

    /**
     *
     * @param locator
     * @throws SWTException
     */
    public AnimationPlayer(String locator) throws SWTException
    {
        System.out.println("AnimationPlayer(String locator) + ");
        iPlayerListenerImpl= new PlayerListenerImpl(this);
        ImageLoader imageLoader= new ImageLoader();
        // Following line may throw SWTException
        iImageData=imageLoader.load(locator);
        //iRepeatCount=imageLoader.repeatCount;
        iSourceDimension= new Point(imageLoader.logicalScreenWidth, imageLoader.logicalScreenHeight);
        populateControl();
        System.out.println("AnimationPlayer(String locator) - ");
    }

    /**
     * Moved the player to close state and releases all resources, called from PlayerBase class
     */
    protected void doClose()
    {
        iState=CLOSED;
        iPlayerListenerImpl.postEvent(PlayerListener.CLOSED, null);
    }

    /**
     * Called from the PlayerBase class
     */
    protected void doDeallocate()
    {
        // dummy implementation
    }

    protected void doPrefetch() throws MediaException
    {
        iState=PREFETCHED;
    }

    protected void doRealize() throws MediaException
    {
        iState=REALIZED;
        // this is temporary solution implement it in proper way
        // initialize the iImage object with first frame
        iImage=new Image(iDisplay, iImageData[0]);
    }

    /**
     *
     */
    protected void doStop() throws MediaException
    {
        // since after stopping the player the player state will move to pre-fetched state
        iState=PREFETCHED;
        iPlayerListenerImpl.postEvent(PlayerListener.STOPPED, new Long(iMediaTime * 10000));
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
     * This function will return, total time in microseconds this player can be played
     */
    public long getDuration()
    {
        closeCheck();
        long time = TIME_UNKNOWN;
        int totalNoOfFrames = iImageData.length;
        for (int i = 0; i < totalNoOfFrames; i++)
        {
            time += iImageData[i].delayTime;
        }
        // Since we have to return it in microsecond multiply it with 1000;
        return time * 10000;
    }

    /**
     * This returns the total time taken, till now, to play the video.
     */
    public long getMediaTime()
    {
        // Since we have to return it in microsecond multiply it with 1000;
        return iMediaTime*10000;
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

    public void start() throws MediaException
    {
        final String DEBUG_STR = "AnimationPlayer::start()";
        final long inTime= System.currentTimeMillis();
        // Logger.LOG(Logger.EJavaMMAPI, Logger.EInfo,DEBUG_STR + "++++++++");
        System.out.println(DEBUG_STR + "+++");
        prefetch();
        // Only prefetched player may be started. If player is already started
        // this method returns silently.
        if (getState() == PREFETCHED)
        {
            initialize();
            iState = STARTED;
            Thread thread = new Thread("Animation")
            {
                int loopCount = iCurrentLoopCount;
                GC gc = null;

                public void run()
                {
                    final int noOfFrames = iImageData.length;
                    while (iFrameIndex < noOfFrames &&
                            (iRepeatForeEver || (loopCount < iTotalLoopCount))&&
                            (iState == STARTED))
                    {
                        final int delayTimeForNextFrame = iImageData[iFrameIndex].delayTime;
                        // if stop time has become more than the media time
                        // TODO This solution may not give the accurate result, if the delay
                        // between two frames is grater than 1 second.
                        // Do we need to implement the TimerTask only?
                        if (iMediaTime > iStopTime)
                        {
                            iPlayerListenerImpl.postEvent(
                                PlayerListener.STOPPED_AT_TIME, new Long(
                                    iMediaTime * 10000));
                            break;
                        }

                        if (iDisplay != null)
                        {
                            iDisplay.asyncExec(new Runnable()
                            {
                                public void run()
                                {
                                    System.out.println(DEBUG_STR+"asynchronous block +++");
                                    // Since we are going to display first frame, notify all
                                    // PlayerListener that Player has started
                                    if (iFrameIndex == 0)
                                    {
                                        iPlayerListenerImpl.postEvent(
                                            PlayerListener.STARTED, new Long(
                                                iMediaTime * 10000));
                                    }
                                    if (gc == null)
                                        gc = new GC(iImage);
                                    Image tempImage = new Image(iDisplay,
                                                                iImageData[iFrameIndex]);
                                    gc.drawImage(tempImage, 0, 0);
                                    tempImage.dispose();
                                    iFrameIndex = (iFrameIndex + 1) % noOfFrames;
                                    if (iControl != null)
                                    {
                                        iControl.redraw();
                                    }
                                    // update the mediaTime, as Animation
                                    // progress
                                    iMediaTime += delayTimeForNextFrame;
                                    // If imageIndex becomes zero it means, all frames
                                    // has been displayed
                                    // So increase the loopCount
                                    if (iFrameIndex == 0)
                                    {
                                        // send the END_OF_MEDIA event to all
                                        // listener
                                        iPlayerListenerImpl.postEvent(
                                            PlayerListener.END_OF_MEDIA,
                                            new Long(iMediaTime * 10000));
                                        loopCount++;
                                        // set iMediaTime to 0
                                        iMediaTime = 0;
                                    }
                                    System.out.println(DEBUG_STR+"asynchronous block ---");
                                }
                            });
                        }
                        else
                        {
                            System.out.println(DEBUG_STR+"InitDisplaymode has not been called yet");
                            if (iFrameIndex == 0)
                            {
                                iPlayerListenerImpl.postEvent(
                                    PlayerListener.STARTED, new Long(
                                        iMediaTime * 10000));
                            }
                            // This else block will execute if the initDisplayMode hasn't been called yet
                            // check if initDisplayMode has been called
                            initialize();
                            iFrameIndex = (iFrameIndex + 1) % noOfFrames;
                            iMediaTime += delayTimeForNextFrame;
                            // post EOM event
                            if (iFrameIndex == 0)
                            {
                                // send the END_OF_MEDIA event to all
                                // listener
                                iPlayerListenerImpl.postEvent(
                                    PlayerListener.END_OF_MEDIA, new Long(
                                        iMediaTime * 10000));
                                loopCount++;
                                // since player is again going to start from
                                // the first frame
                                // so media time should be set to zero
                                iMediaTime = 0;
                            }
                        }
                        try
                        {
                            Thread.sleep(delayTimeForNextFrame * 10);
                        }
                        catch (InterruptedException e)
                        {
                            // TODO Auto-generated catch block
                            e.printStackTrace();
                        }
                        System.out.println(DEBUG_STR+"while lopp ---");
                    }// end of while loop
                    iCurrentLoopCount = loopCount;
                    // Logger.LOG(Logger.EJavaMMAPI, Logger.EInfo, DEBUG_STR
                    System.out.println(DEBUG_STR
                                       + "Came out side the while loop " + iState
                                       + " iFrameIndex " + iFrameIndex + " loopCount "
                                       + loopCount);
                }
            };
            thread.start();
        }
        // Logger.LOG(Logger.EJavaMMAPI, Logger.EInfo,DEBUG_STR + "-");
        System.out.println(DEBUG_STR+" Total time taken:> "+(System.currentTimeMillis()-inTime)+" ---");
    }

    /**
     * This function is also being called from VideoControl class,
     * since on each repaint event, control is getting disposed and created each time
     * @param aControl
     */
    void addPaintListener(org.eclipse.swt.widgets.Control aControl)
    {
        System.out.println("AnimationPlayer::addPaintListener +++");
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
                            //System.out.println("AnimationPlayer::addpaintListener paintControl "+ iImage+" iIsControlVisible "+iIsControlVisible+"current time is "+System.currentTimeMillis());
                            System.out.println("AnimationPlayer::addPaintListener::PaintListener::paintControl");
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
        System.out.println("AnimationPlayer::addPaintListener ---");
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
        final String DEBUG_STR="AnimationPlayer::setmediaTime()";
        long now = super.setMediaTime(aNow);
        int totalFrames = iImageData.length;
        int totalTime = 0;
        for (int i = 0; i < totalFrames; i++)
        {
            totalTime += iImageData[i].delayTime;
            if (totalTime*10000 >= now)
            {
                iFrameIndex=i;
                break;
            }
        }
        // we need to update the iMediaTime as well
        iMediaTime=totalTime;
        return totalTime * 10000;
    }
    //////////////////////////////////////////////////////////////////////////////////////
    // Following functions are for internal use, and not exposed to MIDlet developer//////
    /////////////////////////////////////////////////////////////////////////////////////
    /**
     * This function is responsible for creating all controls and adding it into Controls hashtable.
     */
    private void populateControl()
    {
//      VideoControl videoControl = new VideoControl(this);
//      FramePositioningControl fpc = new FramePositioningControl(this);
//      StopTimeControl stc = new StopTimeControl(this);
//      RateControl rc = new RateControl(this);
        // there are four control provided by AnimationPlayer
        // adding all one by one to the controlList(iControls)
        addControl(new VideoControl(this), fVideoControl);
        addControl(new FramePositioningControl(this), fFramePositioningControl);
        addControl(new StopTimeControl(this), fStopTimeControl);
        addControl(new RateControl(this), fRateControl);
    }

    /**
     * This function initialize iControl and iDisplay object if it is null,
     * otherwise return immediately.
     *
     * In case of Canvas, eSWT control will be returned immediately from VideoControl(vc.getControl()),
     * but in case of CustomItem we need to keep polling, eSWT doesn't return the control for CustomItem
     * until CustomItem is appended to Form.
     */

    private void initialize()
    {
        System.out.println("AnimationPlayer::initialize +++");
        if (iControl == null || iDisplay == null)
        {
            VideoControl vc = (VideoControl) getControl(fVideoControl);
            iDisplay = vc.getiDisplay();
            iControl=vc.getControl();
            addPaintListener(iControl);
            System.out.println("AnimationPlayer::initialize iDsplay "+iDisplay+" iControl "+iControl);
            // in case of CustomItem,
//          while ((iControl = vc.getControl()) == null) {
//              try {
//                  Thread.sleep(100);
//              } catch (InterruptedException e) {
//                  e.printStackTrace();
//              }
//          }
        }
        System.out.println("AnimationPlayer::initialize ---");
    }

    /**
     * This function will be called from setDisplaySize(int width, int height) of animation/VideoControl class
     * When MIDlet developer will try to alter the size of the image
     * @param width : to be set of the video(Animated GIF)
     * @param height : height to be set of video(Animated GIF)
     */
    void updateImageData(int width, int height)
    {
        int noOfFrames= iImageData.length;
        for (int i=0; i<noOfFrames; i++)
        {
            iImageData[i]=iImageData[i].scaledTo(width, height);
        }
        iImage=new Image(iDisplay, iImageData[0]);
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
    javax.microedition.lcdui.Image getCurrentFrame(String format)
    {
        javax.microedition.lcdui.Image currentFrameImage= LCDUIInvoker.createLcduiImage(iImage);
        return currentFrameImage;
    }

    /**
     * Returns Point object, which contains the width and height of the image
     * Called from VideoControl to get the image width and height,
     * so that Item will be created exactly of same dimension
     */
    org.eclipse.swt.graphics.Point getImageDimension()
    {
        return new org.eclipse.swt.graphics.Point(iImageData[0].width, iImageData[0].height);
    }

    /**
     * This returns the imageData array,
     * called from FramePositioningControl class to calculate the frame time
     */
    ImageData[] getImageData()
    {
        return iImageData;
    }

    /**
     *
     * @param aDisplayLocation x,y coordinate where image is to be displayed
     */
    void setDisplayLocation(int aX, int aY)
    {
        iDisplayLocation=new Point(aX,aY);
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
                iState=PREFETCHED;
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
     * @param iStopTime the iStopTime to set
     */
    void setiStopTime(long iStopTime)
    {
        this.iStopTime = iStopTime;
    }

    /**
     * @param iIsControlVisible the iIsControlVisible to set
     */
    void setiIsControlVisible(boolean iIsControlVisible)
    {
        this.iIsControlVisible = iIsControlVisible;
    }
}

