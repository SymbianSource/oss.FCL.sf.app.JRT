/*
* Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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

/**
 * The <code>SVGEventListener</code> is used to forward platform specific events
 * to an application. The application can implement this interface and, in
 * response, dispatch SVG events on an <code>SVGImage</code> object.
 * <br />
 * <br>
 * <br>
 * <b>Code example:</b>
 * <pre>
 *
 * // Create an SVGAnimator
 * SVGImage map = ...; // See the SVGImage documentation.
 * SVGAnimator svgAnimator = SVGAnimator.createAnimator(map);
 *
 * // Create an SVGEventListener and set it on the animator.
 * MIDPSVGEventListener listener = new MIDPSVGEventListener(map, animator);
 * svgAnimator.setSVGEventListener(listener);
 *
 * //
 * // SVGEventListener sample implementation. Adds a new
 * // circle every time a key is pressed.
 * //
 * class MIDPSVGEventListener implements SVGEventListener, Runnable {
 *      protected SVGDocument svgDocument;
 *      protected SVGAnimator svgAnimator;
 *      protected Vector addToTree = new Vector();
 *
 *      public MIDPSVGEventListener(SVGImage svgImage, SVGAnimator animator) {
 *          this.svgDocument = svgImage.getDocument();
 *          this.svgAnimator = svgAnimator;
 *      }
 *
 *      public void keyPressed(int keyCode) {
 *          SVGElement circle = svgDocument.createElementNS(svgNS, "circle");
 *          circle.setFloatTrait("cx", ...);
 *          circle.setFloatTrait("cy", ...);
 *          // ....
 *
 *          // We synchronized access to the addToTree vector because
 *          // it is accessed from different threads. Because we do
 *          // no know how fast this runnable is going to be scheduled
 *          // by the animator, it is possible that multiple circles
 *          // be created before the run() method is invoked. This explain
 *          // why we put all the newly created circles in a Vector and why
 *          // the run method inserts the current content of the vector
 *          // into the SVG document.
 *          synchronized (addToTree) {
 *             addToTree.addElement(circle);
 *          }
 *
 *          svgAnimator.invokeLater(this);
 *      }
 *
 *      public run() {
 *          synchronized (addToTree) {
 *              for (int i = 0; i < addToTree.size(); i++) {
 *                  svgDocument.getDocumentElement().appendChild(
 *                     (SVGElement) addToTree.elementAt(i));
 *              }
 *              addToTree.clear();
 *          }
 *      }
 * }
 * </pre>
 *
 */
public interface SVGEventListener
{
    /**
     * Invoked by the SVG implementation when the associated component is
     * hidden.
     *
     * On MIDP, this method is invoked when the
     * <code>javax.microedition.lcdui.Canvas.hideNotify</code> method is invoked.
     *
     * On AWT, this method is invoked when the <code>java.awt.event.ComponentEvent</code>
     * with type <code>java.awt.event.ComponentEvent.COMPONENT_HIDDEN</code> is
     * invoked on a <code>java.awt.Component</code>'s component listener.
     *
     */
    void hideNotify();

    /**
     * Invoked by the SVG implementation when a key was pressed while the
     * component associated with the <code>SVGAnimator</code> had focus.
     *
     * On MIDP, this method is invoked when the
     * <code>javax.microedition.lcdui.Canvas.keyPressed()</code> method is
     * invoked.
     *
     * On AWT, this method is invoked when the
     * <code>java.awt.event.KeyListener.keyPressed()</code> method is invoked on
     * a <code>java.awt.Component</code>'s key listener.
     *
     * @param keyCode the code of the key that was pressed. For MIDP, the code
     * is the same as for the <code>javax.microedition.lcdui.Canvas</code>
     * <code>keyPressed</code> <code>keyCode</code> argument.  For AWT, the code
     * is the same as in the <code>java.awt.event.KeyEvent.getKeyCode()</code>
     * method.
     */
    void keyPressed(int keyCode);

    /**
     * Invoked by the SVG implementation when a key was released while the
     * component associated with the <code>SVGAnimator</code> had focus.
     *
     * On MIDP, this method is invoked when the
     * <code>javax.microedition.lcdui.Canvas.keyReleased()</code> method is
     * invoked.
     *
     * On AWT, this method is invoked when the
     * <code>java.awt.event.KeyListener.keyReleased()</code> method is invoked on
     * a <code>java.awt.Component</code>'s key listener.
     *
     * @param keyCode the code of the key that was pressed. For MIDP, the code
     * is the same as for the <code>javax.microedition.lcdui.Canvas</code>
     * <code>keyReleased</code> <code>keyCode</code> argument.  For AWT, the code
     * is the same as in the <code>java.awt.event.KeyEvent.getKeyCode()</code>
     * method.
     */
    void keyReleased(int keyCode);

    /**
     * Invoked by the SVG implementation when the pointer device (if any), is
     * pressed over the component associated with the <code>SVGAnimator</code>.
     *
     * On MIDP, this method is invoked when the
     * <code>javax.microedition.lcdui.Canvas.pointerPressed()</code> method is
     * invoked. Note that pointer events are only supported on MIDP if the
     * platform supports them, as defined by the
     * <code>Canvas.hasPointerEvents</code> method.
     *
     * On AWT, this method is invoked when the
     * <code>java.awt.event.MouseListener.mousePressed()</code> method is invoked on
     * a <code>java.awt.Component</code>'s mouse listener.
     *
     * @param x the x-axis coordinate, in the target component's space (i.e.,
     * relative to the upper left corner of the component associated with the
     * <code>SVGAnimator</code>.  On MIDP, this is the same value as passed to the
     * <code>javax.microedition.midp.Canvas.pointerPressed()</code> method.  On
     * AWT, this is the same value as returned from the
     * <code>java.awt.event.MouseEvent.getX()</code> method.
     *
     * @param y the y-axis coordinate, in the target component's space (i.e.,
     * relative to the upper left corner of the component associated with the
     * <code>SVGAnimator</code>.  On MIDP, this is the same value as passed to the
     * <code>javax.microedition.midp.Canvas.pointerPressed()</code> method.  On
     * AWT, this is the same value as returned from the
     * <code>java.awt.event.MouseEvent.getY()</code> method.
     */
    void pointerPressed(int x, int y);

    /**
     * Invoked by the SVG implementation when the pointer device (if any), is
     * released over the component associated with the <code>SVGAnimator</code>.
     *
     * On MIDP, this method is invoked when the
     * <code>javax.microedition.lcdui.Canvas.pointerReleased()</code> method is
     * invoked. Note that pointer events are only supported on MIDP if the
     * platform supports them, as defined by the
     * <code>Canvas.hasPointerEvents</code> method.
     *
     * On AWT, this method is invoked when the
     * <code>java.awt.event.MouseListener.mouseReleased()</code> method is invoked on
     * a <code>java.awt.Component</code>'s mouse listener.
     *
     * @param x the x-axis coordinate, in the target component's space (i.e.,
     * relative to the upper left corner of the component associated with the
     * <code>SVGAnimator</code>.  On MIDP, this is the same value as passed to the
     * <code>javax.microedition.midp.Canvas.pointerReleased()</code> method.  On
     * AWT, this is the same value as returned from the
     * <code>java.awt.event.MouseEvent.getX()</code> method.
     *
     * @param y the y-axis coordinate, in the target component's space (i.e.,
     * relative to the upper left corner of the component associated with the
     * <code>SVGAnimator</code>.  On MIDP, this is the same value as passed to the
     * <code>javax.microedition.midp.Canvas.pointerReleased()</code> method.  On
     * AWT, this is the same value as returned from the
     * <code>java.awt.event.MouseEvent.getY()</code> method.
     */
    void pointerReleased(int x, int y);

    /**
     * Invoked by the SVG implementation when the associated component is
     * shown.
     *
     * On MIDP, this method is invoked when the
     * <code>javax.microedition.lcdui.Canvas.showNotify</code> method is invoked.
     *
     * On AWT, this method is invoked when the <code>java.awt.event.ComponentEvent</code>
     * with type <code>java.awt.event.ComponentEvent.COMPONENT_SHOWN</code> is
     * invoked on a <code>java.awt.Component</code>'s component listener.
     *
     */
    void showNotify();

    /**
     * Invoked by the SVG implementation when the associated component is
     * resized.
     *
     * On MIDP, this method is invoked when the
     * <code>javax.microedition.lcdui.Canvas.sizeChanged</code> method is invoked.
     *
     * On AWT, this method is invoked when the <code>java.awt.event.ComponentEvent</code>
     * with type <code>java.awt.event.ComponentEvent.COMPONENT_RESIZED</code> is
     * invoked on a <code>java.awt.Component</code>'s component listener.
     *
     * @param width the new component width.
     * @param height the new component height.
     */
    void sizeChanged(int width, int height);
}

