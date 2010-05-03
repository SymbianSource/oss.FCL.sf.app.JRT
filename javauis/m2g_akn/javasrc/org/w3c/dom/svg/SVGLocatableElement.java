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
package org.w3c.dom.svg;


/**
 * This interface represents an SVGLocatableElement. It is implemented by all drawable SVG elements
 * in the document tree. Drawable elements are: &lt;rect&gt;, &lt;circle&gt;, &lt;ellipse&gt;,
 * &lt;line&gt;, &lt;path&gt; &lt;use&gt; &lt;image&gt; &lt;text&gt;, &lt;svg&gt;, &lt;a&gt;,
 * and &lt;g&gt;. <b>Note</b> that animations will have an effect on the values of bounding box.
 *
 * <br>
 * <br>
 * <br>
 *The following example further clarify the behavior of the getBBox()
 *method. The example have a short explanation, an SVG fragment and are
 *followed by a set of bounding box values which have the following
 *format:<br>
 *<tt><br>
 *[elementId] : {x, y, width, height} | {null}</tt><br>
 *<br>
 *where x, y, width and height define the values of the SVGRect object's
 *returned from a getBBox call on the element with the specified id.
 *There are a few cases where the bounding box may be null (see example
 *6).<br>
 *<br>
 *<h3>Example #1: Simple groups and bounds</h3>
 *<br>
 *This first example shows the values returned by the getBBox method for
 *various simple basic shapes and groups. In particular, it shows that
 *the transform, on an element, does not change the value of its user
 *space bounding box.<br>
 *<br>
 *<tt>&lt;g id="group1" transform="translate(10, 20)" fill="red" &gt;
 *<br>
 *&nbsp;&lt;rect id="rect1" transform="scale(2)" x="10" y="10" width="50"
 *height="50"/&gt;
 *<br>
 *&nbsp;&lt;rect id="rect2" x="10" y="10" width="100" height="100"/&gt;
 *<br>
 *&nbsp;&lt;g id="group2" transform="translate(10, 20)" &gt;
 *<br>
 *&nbsp;&nbsp; &lt;rect id="rect3" x="0" y="10" width="150" height="50"/&gt;
 *<br>
 *&nbsp;&nbsp; &lt;circle id="circle1" cx="20" cy="20" r="100" /&gt;
 *<br>
 *&nbsp;&lt;/g&gt;
 *<br>
 *&lt;/g&gt;
 *<br>
 *</tt><br>
 *<tt>[group1] : {-70.0, -60.0, 230.0, 200.0}
 *<br>
 *[rect1] : {10.0, 10.0, 50.0, 50.0}
 *<br>
 *[rect2] : {10.0, 10.0, 100.0, 100.0}
 *<br>
 *[group2] : {-80.0, -80.0, 230.0, 200.0}
 *<br>
 *[rect3] : {0.0, 10.0, 150.0, 50.0}
 *<br>
 *[circle1] : {-80.0, -80.0, 200.0, 200.0}
 *</tt><br>
 *<br>
 *<h3>Example #2: Bounding box on zero width or height rectangle<br>
 *</h3>
 *This example illustrates that the bounding box on elements is based on
 *the element's geometry coordinates. For example, the bounding box on a
 *zero-width rectangle is defined (see below), even though the rectangle
 *is not rendered.<br>
 *<pre><tt>&lt;g id="group1" transform="translate(10, 20)" fill="red" &gt;</tt></pre>
 *<pre><tt>&nbsp;&nbsp; </tt>&lt;rect id="rect2" x="10" y="10" width="400" height="0"/&gt;</pre>
 *<pre wrap=""><tt>   &lt;g id="group2" transform="translate(10, 20)" &gt;
 *      &lt;rect id="rect3" x="0" y="10" width="150" height="50"/&gt;
 *   &lt;/g&gt;</tt><tt>
 *&lt;/g&gt;
 *</tt></pre>
 *<pre><tt>[group1] : {10.0, 10.0, 400.0, 70.0}
 *</tt>[rect2] : {10.0, 10.0, 400.0, 0.0}<tt>
 *</tt>[group2] : {0.0, 10.0, 150.0, 50.0}<tt>
 *[rect3] : {0.0, 10.0, 150.0, 50.0}</tt>
 *</pre>
 *<h3>Example #3: Bounding Box on zero radius ellipses.</h3>
 *This is another example of how bounding boxes are based on the
 *element's geometry. Here, the bounding box of an ellipse with a zero
 *x-axis radius is still defined, even though the ellipse is not rendered.<br>
 *<pre wrap="">&lt;svg id="mySVG" width="10" height="20"&gt;
 *&lt;g id="group1" transform="translate(10, 20)" fill="red" &gt;
 *  &lt;rect id="rect1" x="10" y="10" width="100" height="100"/&gt;
 *  &lt;ellipse id="ellipse1" cx="20" cy="20" rx="0" ry="70" /&gt;
 *&lt;/g&gt;</pre>
 *<tt>[mySVG] : {20.0, -30.0, 100.0, 160.0}
 *<br>
 *[group1] : {10.0, -50.0, 100.0, 160.0}
 *<br>
 *[rect1] : {10.0, 10.0, 100.0, 100.0}
 *<br>
 *[ellipse1] : {20.0, -50.0, 0.0, 140.0}
 *</tt><br>
 *<h3>Example #4: Viewports do not clip bounding boxes</h3>
 *This example shows that no matter what the viewport is on the root SVG
 *element, the bounding boxes, based on the geometry, are still defined.
 *Here, even though the root svg has a zero width, the bounding boxes for
 *the root itself and its children is precisely defined.<br>
 *<pre wrap="">&lt;svg id="mySVG" width="0" height="50"&gt;
 *  &lt;g id="group1" transform="translate(10, 20)" fill="red" &gt;
 *    &lt;rect id="rect1" x="10" y="10" width="50" height="50"/&gt;
 *    &lt;g id="group2" transform="translate(10, 20)" &gt;
 *      &lt;rect id="rect2" x="0" y="10" width="150" height="0"/&gt;
 *      &lt;circle id="circle1" cx="20" cy="20" r="500" /&gt;
 *    &lt;/g&gt;
 *  &lt;/g&gt;
 *&lt;/svg&gt;
 *</pre>
 *<tt>[mySVG] : {-460.0, -440.0, 1000.0, 1000.0}
 *<br>
 *[group1] : {-470.0, -460.0, 1000.0, 1000.0}
 *<br>
 *[rect1] : {10.0, 10.0, 50.0, 50.0}
 *<br>
 *[group2] : {-480.0, -480.0, 1000.0, 1000.0}
 *<br>
 *[rect2] : {0.0, 10.0, 150.0, 0.0}
 *<br>
 *[circle1] : {-480.0, -480.0, 1000.0, 1000.0}
 *</tt><br>
 *<h3>Example #5: getBBox on &lt;use&gt;</h3>
 *This example shows that the bounding box for a &lt;use&gt; element
 *accounts for the x and y attributes defined on the element, just like
 *the x and y attributes impact the bounding box computation on a
 *&lt;rect&gt; or on an &lt;image&gt; element.<br>
 *<br>
 *<tt>&lt;svg&gt;
 *<br>
 *&nbsp; &lt;defs&gt;
 *<br>
 *&nbsp;&nbsp;&nbsp;&nbsp; &lt;rect id="myRect" x="0" y="0" width="60" height="40" /&gt;
 *<br>
 *&nbsp; &lt;/defs&gt;
 *<br>
 *&nbsp; &lt;use id="myUse" xlink:href="#myRect" x="-30" y="-20" /&gt;
 *<br>
 *&lt;/svg&gt;
 *<br>
 *</tt>
 *<br>
 *<tt>[myRect] : {0.0, 0.0, 60.0, 40.0}
 *<br>
 *[myUse] : {-30.0, -20.0, 60.0, 40.0}
 *</tt><br>
 *<h3>Example #6: Empty group</h3>
 *This example shows that the bounding box for an empty group is null. By
 *the same token, the bounding box of a &lt;path&gt; with an empty
 *SVGPath (i.e., one with no path commands, which may happen after
 *creating a new &lt;path&gt; element with a Document.createElementNS
 *call) is also null.<br>
 *<br>
 *<tt>&lt;g id="emptyG" /&gt;
 *<br>
 *<br>
 *[emptyG] : {null}
 *</tt><br>
 *<br>
 *<h3>Example #7: Impact of display='none' and visibility='hidden'<br>
 *</h3>
 *This example shows how the bounding box of children with display='none'
 *are not accounted for in the computation of their parent's bounding
 *box. This reflects the definition of the display property and its
 *impact on rendering and bounding box computation. The example also
 *shows that elements with a 'hidden' visibility still contribute to
 *their parent's bounding box computation.<br>
 *<br>
 *<tt>&lt;g id="g1"&gt; <br>
 *&nbsp;&nbsp;&nbsp; &lt;g id="g1.1.display.none" display="none"&gt; <br>
 *&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp; &lt;rect id="rect1" x="10" y="10" width="40" height="40"/&gt;<br>
 *&nbsp;&nbsp;&nbsp; &lt;g/&gt;
 *<br>
 *&nbsp;&nbsp;&nbsp; &lt;rect id="rect2.visibility.hidden" visibility="hidden" <br>
 *&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp; x="30" y="60" width="10" height="20"/&gt;
 *<br>
 *&lt;/g&gt;</tt><br>
 *<br>
 *<tt>[g1] : {30.0, 60.0, 10.0, 20.0} <br>
 *[g1.1.display.none] : {10.0, 10.0, 40.0, 40.0}<br>
 *[rect1] : {10.0, 10.0, 40.0, 40.0}<br>
 *[rec2.visibility.hidden] : {30.0, 60.0, 10.0, 20.0}</tt><br>
 *<h3>Example #8: Concatenating bounding boxes in the container's user
 *space.<br>
 *</h3>
 *This example shows how the concatenation and computation of bounding
 *boxes for container element happens in the container's user space.<br>
 *<br>
 *<tt>&lt;g id="g1"&gt;<br>
 *&nbsp; &lt;line id="line1" x2="100" y2="100" transform="rotate(-45)"/&gt;<br>
 *&lt;/g&gt;</tt><br>
 *<tt><br>
 *[g1] : {0.0, 0.0, 141.42136, 0}<br>
 *[line1] : {0.0, 0.0, 100.0, 100.0}</tt><br>
 *<h3>Example #9: No influence of stroke-width.</h3>
 *This example illustrates that stroking has no impact on the computation
 *of bounding boxes.<br>
 *<tt><br>
 *&lt;g&gt;<br>
 *&nbsp;&nbsp; &lt;line id="thickLine" stroke-width="10" x2="100" y2="0" /&gt;<br>
 *&lt;/g&gt;</tt><br>
 *<br>
 *[thickLine] : {0.0, 0.0, 100.0, 0.0}<br>
 *<br>
 *<h3>Example #10: No influence of viewBox.</h3>
 *This example illustrates that viewBox has no impact on the computation
 *of bounding boxes.<br>
 *<tt><br>
 *&lt;svg id="rootSvg" width="500" height="300" viewBox="0 0 200 100" &gt;<br>
 *&nbsp;&nbsp; &lt;rect x="-100" y="-200" width="500" height="100" /&gt;<br>
 *&lt;/svg&gt;</tt><br>
 *<br>
 *[rootSVG] : {-100, -200, 500, 100}<br>
 *
 *
 */
public interface SVGLocatableElement extends SVGElement
{

    /**
     * <p>
     * Returns the tight bounding box in current user coordinate space. Tight bounding box is the smallest
     * possible rectangle that includes the geometry of all contained graphics elements excluding stroke.
     * The calculation is done in the user coordinate space of the element. When bounding box
     * is calculated elements with display property (trait) set to none are ignored. Exact rules for the bounding
     * box calculation are given in the <a href="http://www.w3.org/TR/SVG/coords.html#ObjectBoundingBox">SVG spec</a>.
     * </p>
     *
     * @return the tight bounding box in current user coordinate space.
     */
    public SVGRect getBBox();

    /**
     * <p>
     * Returns the transformation matrix from current user units (i.e., after application of the transform attribute, if any)
     * to the parent user agent's notion of a "pixel". For display devices, ideally this represents a physical screen pixel.
     * For other devices or environments where physical pixel sizes are not known, then an algorithm similar to the CSS2
     * definition of a "pixel" can be used instead. Note that <code>null</code> is returned if this element is not hooked into the
     * document tree.
     * </p>
     *
     * @return the transformation matrix from current user units to the parent user agent's notion of a "pixel".
     */
    public SVGMatrix getScreenCTM();

    /**
     * <p>
     * Returns the tight bounding box in screen coordinate space. Tight bounding box is the smallest
     * possible rectangle that includes the geometry of all contained graphics elements excluding stroke.
     * The box coordinates are in the screen coordinate space, which is connected to the current user
     * coordinate space by the matrix returned by {@link org.w3c.dom.svg.SVGLocatableElement#getScreenCTM getScreenCTM} method.
     * Note that <code>null</code> is returned if this element is not hooked into the
     * document tree.
     * </p>
     *
     * @return the tight bounding box in screen coordinate space.
     */
    public SVGRect getScreenBBox();
}