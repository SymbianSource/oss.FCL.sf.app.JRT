/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies).
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


package javax.microedition.m2g;

import com.nokia.microedition.m2g.M2GSVGAnimator;
import org.eclipse.swt.widgets.*;
import org.eclipse.swt.widgets.Display;
import com.nokia.microedition.m2g.M2GSVGeSWTAnimator;
/**
 * The <code>SVGAnimator</code> class handles automatic rendering of updates and
 * animations in an <code>SVGImage</code> to a target user interface (UI) component.
 *
 * The target component type depends on the Java profile this specification is
 * implemented on, as described in the <code>createAnimator</code> and
 * <code>getTargetComponent</code> methods documentation.
 *
 * There are two types of rendering updates the <code>SVGAnimator</code> handles:
 *
 * <ul>
 *   <li><b>Animation Updates.</b> The <code>SVGAnimator</code> can run animations
 *       and automatically and periodically refreshes its rendering to
 *       reflect the effect of animations.</li>
 *   <li><b>SVGImage Updates.</b> The <code>SVGAnimator</code> updates its rendering
 *       following alterations of the associated <code>SVGImage</code>,
 *       for example if the position of one of the graphical elements is
 *       modified by an API call.</li>
 * </ul>
 *
 * An <code>SVGAnimator</code> instance can be in one of the following states:
 * <ul>
 *   <li><b>Stopped</b>. This is the initial state. The <code>SVGAnimator</code>
 *       performs no rendering updates. <br/>
 *       Possible transitions:
 *       <ul>
 *          <li>To the <i>playing</i> state, with the <code>play</code> method.</li>
 *       </ul>
 *       </li>
 *
 *   <li><b>Playing</b>. This is the typical state for an
 *   <code>SVGAnimator</code>.  In that state, the <code>SVGAnimator</code>
 *   performs both Animation and SVGImage updates.
 *
 *   While there are active animations,
 *   the animator updates the rendering at a rate not faster than the one
 *   defined by the <code>setTimeIncrement</code> method. If SVGImage updates are made
 *   (e.g., with calls to the <code>SVGElement</code>
 *   <code>setTrait</code> method, see examples), the rendering is updated at the time of
 *   the next animation rendering.<br />
 *
 *   When there are no active animations, the animator will update the rendering
 *   after each <code>Runnable</code> passed to the <code>invokeLater</code> or
 *   <code>invokeAndWait</code> methods has finished executing.
 *   <br/>
 *       Possible transitions:
 *       <ul>
 *          <li>To the <i>stopped</i> state, with the <code>stop</code> method.</li>
 *          <li>To the <i>paused</i> state, with the <code>pause</code> method.</li>
 *       </ul>
 *   </li>
 *
 *   <li><b>Paused</b>. When in that state, the <code>SVGAnimator</code> only
 *   performs SVGImage updates rendering. The animator no longer automatically
 *   advances the SVG document's current time, so rendering reflects the animation
 *   at the document's current time. Note that a change to the document's current
 *   time while in the <i>paused</i> state will trigger a new rendering for the
 *   new current time.
 *   <br/>
 *       Possible transitions:
 *       <ul>
 *          <li>To the <i>stopped</i> state, with the <code>stop</code> method.</li>
 *          <li>To the <i>playing</i> state, with the <code>play</code> method.</li>
 *       </ul>
 *   </li>
 * </ul>
 *
 * <b>Code example:</b><br />
 * <pre>
 * // Create an SVGAnimator
 * SVGImage map = ...; // See the SVGImage documentation.
 *
 * SVGAnimator svgAnimator = SVGAnimator.createAnimator(map);
 *
 * // Display the associated SVGAnimator component.
 * // Depends on the platform.
 *
 * // =============== AWT Example ===============
 * Panel panel = ....;
 * panel.add((Component) svgAnimator.getTargetComponent());
 * ...
 *
 * // =============== MIDP Example ===============
 * Canvas svgCanvas = (Canvas) svgAnimator.getTargetComponent());
 * ...
 *
 * // Start rendering animations.
 * svgAnimator.play();
 * ....
 * class MapRunnable implements Runnable {
 *    public void run() {
 *        // Perform map updates based on current
 *        // traffic information.
 *        SVGElement statusRect
 *             = map.getDocument().getElementById("statusRect");
 *
 *        // Reflect that traffic status.
 *        statusRect.setRGBTrait(...); // See setRGBTrait documentation.
 *    }
 * }
 *
 * Runnable mapUpdates = new MapRunnable();
 * ....
 *
 * while (someLoopCondition) {
 *     if(hasMapUpdate) {
 *         svgAnimator.invokeAndWait(mapUpdates);
 *     }
 * }
 * </pre>
 */
public abstract class SVGAnimator
{
    /**
     * This method creates a new <code>SVGAnimator</code> for the specified SVGImage.
     *
     * @param svgImage the <code>SVGImage</code> this animator should render.
     * @return a new <code>SVGAnimator</code> instance.
     * @throws NullPointerException if <code>svgImage</code> is null.
     */
	
	public static SVGAnimator createAnimator(SVGImage svgImage)
	{
		SVGAnimator tempAnimator = null;
		
		
		
		tempAnimator=M2GSVGAnimator.buildAnimator(svgImage);
		
		

		return tempAnimator;
	}
	
	
	
	public static SVGAnimator createAnimator(SVGImage svgImage,java.lang.String componentBaseClass)
	{
		SVGAnimator tempAnimator = null;
		
		if (componentBaseClass == "org.eclipse.swt.widgets.Control")
		{
			
			tempAnimator=M2GSVGeSWTAnimator.buildAnimator(svgImage);
			
		}
		else
		{
			
			tempAnimator=M2GSVGAnimator.buildAnimator(svgImage);
			
		}
		return tempAnimator;
	}
	
   /* public static SVGAnimator createAnimator(SVGImage svgImage)
    {
        SVGAnimator tempAnimator = null;
        
    		
    		
        tempAnimator=M2GSVGeSWTAnimator.buildAnimator(svgImage);
        
				

        //tempAnimator=M2GSVGAnimator.buildAnimator(svgImage);
        
        return tempAnimator;
    }*/

    /**
     * This method creates a new <code>SVGAnimator</code> for the specified SVGImage.
     *
     * The following components types must be supported:
     *
     * <ul>
     * <li> "javax.microedition.lcdui.Canvas" on profiles supporting LCDUI</li>
     * <li> "java.awt.Component" on profiles supporting AWT</li>
     * </ul>
     *
     * On platforms that support the Swing UI component framework, the
     * "javax.swing.JComponent" string may be used to request a Swing component.
     *
     * @param svgImage the <code>SVGImage</code> this animator should render.
     * @param componentBaseClass the desired base class for the component associated
     *        with the animator. This is used when the platform this specification
     *        is implemented on supports multiple UI component frameworks. If
     *        componentBaseClass is null, this is equivalent to invoking the
     *        <code>createAnimator</code> method with the svgImage parameter only.
     * @return a new <code>SVGAnimator</code> instance.
     * @throws NullPointerException if <code>svgImage</code> is null.
     * @throws IllegalArgumentException if the requested
     *         <code>componentBaseClass</code> is not supported by the
     *         implementation.
     */
//	
//	public static SVGAnimator createAnimator(SVGImage svgImage, String componentBaseClass)
//		{
//			SVGAnimator tempAnimator = null;
//			
//			tempAnimator=M2GSVGAnimator.buildAnimator(svgImage, componentBaseClass);
//			
//	        return tempAnimator;
//
//		}
	
	
	
/*    public static SVGAnimator createAnimator(
        SVGImage svgImage, String componentBaseClass)
    {
				

        SVGAnimator tempAnimator = null;
            tempAnimator=M2GSVGeSWTAnimator.buildAnimator(svgImage);
// TODO Check for the toolkit?? tempAnimator=M2GSVGAnimator.buildAnimator(svgImage, componentBaseClass);
        return tempAnimator;
    }*/

    /**
     * The type of target component associated with the animator depends on the
     * Java profile this specification is implemented on:
     *
     * <ul>
     * <li> javax.microedition.lcdui.Canvas on profiles supporting LCDUI</li>
     * <li> java.awt.Component on profiles supporting AWT</li>
     * </ul>
     * @return target the target component associated with the animator.
     * @see #createAnimator
     */
    public abstract Object getTargetComponent();

    /**
     * Returns the current time increment used for animation rendering. The
     * SVGAnimator increments the SVG document's current time by this amount
     * between each rendering.
     *
     * @return the current time increment used for animation rendering. The default
     *         value is 0.1 (100 milliseconds)
     * @see #setTimeIncrement
     */
    public abstract float getTimeIncrement();

    /**
     * Invoke the input Runnable in the Document update thread and return after
     * the Runnable has completed.
     *
     * @param runnable the new Runnable to invoke.
     * @throws java.lang.InterruptedException if the current thread is waiting,
     * sleeping, or otherwise paused for a long time and another thread
     * interrupts it.
     * @throws NullPointerException if <code>runnable</code> is null.
     * @throws IllegalStateException if the animator is in the <i>stopped</i> state.
     */
    public abstract void invokeAndWait(
        Runnable runnable) throws InterruptedException;

    /**
     * Schedule the input Runnable for execution in the update thread at a later time.
     *
     * @param runnable the new Runnable to execute in the Document's update
     * thread when time permits.
     * @throws NullPointerException if <code>runnable</code> is null.
     * @throws IllegalStateException if the animator is in the <i>stopped</i> state.
     */
    public abstract void invokeLater(java.lang.Runnable runnable);

    /**
     * Transitions this <code>SVGAnimator</code> to the <i>paused</i> state.
     * The <code>SVGAnimator</code> stops advancing the document's current time
     * automatically (see the SVGDocument's setCurrentTime method). As a result,
     * animations are paused until another call to the <code>play</code> method
     * is made, at which points animations resume from the document's current
     * time. SVGImage updates (through API calls) trigger a rendering update
     * while the <code>SVGAnimator</code> is in the <i>paused</i> state.
     *
     * @throws IllegalStateException if the animator is not in the <i>playing</i>
     *         state.
     */
    public abstract void pause();

    /**
     * Transitions this <code>SVGAnimator</code> to the <i>playing</i>
     * state. While in the <i>playing</i> state, both Animation and SVGImage
     * updates trigger rendering updates. Note that a change to the document's
     * current time while in the playing state will cause the animator to seek
     * to the new time, and continue to play animations forward.
     *
     * @throws IllegalStateException if the animator is not currently in
     *         the <i>stopped</i> or <i>paused</i> state.
     */
    public abstract void play();

    /**
     * Sets the time increment to use for animation rendering.
     *
     * @param timeIncrement the minimal time that should ellapse between frame
     * rendering. In seconds. Should be greater than zero.
     * @throws IllegalArgumentException if timeIncrement is less than or equal to
     *         zero.
     * @see #getTimeIncrement
     */
    public abstract void setTimeIncrement(float timeIncrement);

    /**
     * Sets the <code>SVGEventListener</code> associated with this
     * <code>SVGAnimator</code>.
     *
     * @param svgEventListener the new SVGEventListener which will receive
     *        events forwarded by this <code>SVGAnimator</code>. May be null,
     *        in which case events are not forwarded by the <code>SVGAnimator</code>.
     */
    public abstract void setSVGEventListener(SVGEventListener svgEventListener);

    /**
     * Transitions this <code>SVGAnimator</code> to the <i>stopped</i> state.
     * This causes the <code>SVGAnimator</code> to stop advancing the document's
     * current time automatically, and no rendering updates are performed while
     * in <i>stopped</i> state. A call to the <i>play</i> method will cause
     * the animations to resume from the document's current time.
     *
     * @throws IllegalStateException if the animator is not in the <i>playing</i>
     *         or <i>paused</i> state.
     */
    public abstract void stop();
}
