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

import org.w3c.dom.DOMException;

import org.w3c.dom.Element;
import org.w3c.dom.events.EventTarget;

/**
 * This interface represents an SVG element in the document tree. Element's id can be set only
 * if it does not already have an id. {@link org.w3c.dom.DOMException DOMException} with error
 * code NO_MODIFICATION_ALLOWED_ERR is raised if an attempt is made to change an existing id.
 * Elements with non-null id can be inserted, but <b>cannot be removed</b> from the DOM tree
 * (see {@link org.w3c.dom.Node#removeChild removeChild}). This interface also provides methods
 * to traverse elements in the DOM tree.
 *
 * <p>
 * This interface can also be used read and manipulate the value of "traits" associated with this
 * SVGElement. Each <i>trait</i> corresponds to an attribute or property,which is parsed and
 * understood by the element and in most cases animatable. Unlike attributes, each element has a
 * well-defined set of traits and attempting to access undefined trait is an error. Also unlike
 * attributes traits are typed and their values are normalized; for instance SVG path specification
 * is parsed and all path commands are converted to their absolute variants, it is not possible to
 * say through the value of the trait if a path command was absolute or relative. When getting and
 * setting trait values, accessor of the correct type must be used or exception will be thrown.
 * </p>
 * <p>
 * Initial trait values come from parsing corresponding attributes. If value is not specified, but
 * corresponing attribute (or property for environments where styling is supported) is inherited,
 * inherited value is returned as a result of the trait query method. If it is not inherited, default
 * value is returned. Default values are also returned in the case when there is no parent to inherit
 * from, for ex: when you create a new element, set a trait value to 'inherit', but there is no parent for
 * inheritance. It is important to note that the value which is returned is always a base value (i.e. before animation
 * is applied), and this is true for both static and animated content.
 * </p>
 * <p>
 * Setting a trait value has the same effect as changing a corresponding attribute, but trait
 * setters can operate on typed values. The value which is modified is always a base value.
 * For inheritable traits the trait value can always be set to "inherit"
 * (but querying the value will always return the actual inherited value as explained above).
 * </p>
 * <p>
 * There are two situations where the various trait setter methods (such as
 * setTrait, setFloatTrait or setPathTrait methods) consider a value
 * invalid and throw a DOMException with the INVALID_ACCESS_ERR code.
 * The first situation is when the trait value is invalid with regards to
 * its definition (for example, trying to set the "stroke-linejoin" trait
 * to "foo" would cause this exception).
 * The second situation is when the trait value is invalid with regards to
 * animations currently applied to the trait. The value is considered
 * invalid because it would put the animation, and therefore the document,
 * in an error state. For example, if a <path> element has animations on
 * its "d" attribute, trying to change the "d" attribute to a value
 * incompatible with the animations will cause the exception to happen.
 * </p>
 *
 * <h3>Traits supported in this specification, SVG Tiny 1.1 DOM</h3>
 *
 *
 * <p>The table below shows the list of attributes and properties that SVG Tiny
 * DOM 1.1 implementations must support. Each light gray section lists one or
 * multiple elements for which the subsequent attributes or properties
 * apply. Each attribute row lists the allowed getter and setter (s). The last
 * column specifies the default values that must be used for each attribute or
 * property.</p>
 * <p><b>Note:</b> For 'REQUIRED' attributes, there are two cases:
 *
 * <ul>
 *  <li>i) The document is in error, if this attribute was not present at the time of loading.</li>
 *  <li>ii) When using uDOM API, the specified default value (in parenthesis) must be used.</li>
 * </ul>
 * </p>
 *
 * <table height="1586" cellpadding="2" cellspacing="2" border="1"
 *  width="825" style="border-collapse: collapse;">
 *   <tbody>
 *     <tr>
 *       <th width="150" valign="top" bgcolor="#999999"><b>Property<br>
 *       </b> </th>
 *       <th valign="top" bgcolor="#999999"><b>Trait Getter <br>[possible return value(s)]<br>
 *       </b> </th>
 *       <th bgcolor="#999999" valign="top"><b>Trait Setter <br>[allowed value(s)]<br>
 *       </b> </th>
 *       <th bgcolor="#999999" valign="top"><b>Default Values<br>
 *       </b> </th>
 *     </tr>
 *     <tr>
 *       <td valign="top"><br>
 *       </td>
 *       <td valign="top"><br>
 *       </td>
 *       <td valign="top"><br>
 *       </td>
 *       <td valign="top"><br>
 *       </td>
 *     </tr>
 *     <tr>
 *       <td bgcolor="#cccccc" colspan="4" rowspan="1" valign="top">&lt;svg&gt;,
 *  &lt;rect&gt;, &lt;circle&gt;, &lt;ellipse&gt;, &lt;line&gt;,
 *  &lt;path&gt;, &lt;g&gt;, &lt;image&gt;, &lt;text&gt;, &lt;a&gt;, and &lt;use&gt;</td>
 *     </tr>
 *     <tr>
 *       <td width="150" valign="top">color <br>
 *       </td>
 *       <td valign="top">getRGBColorTrait [SVGRGBColor]<br>
 *       </td>
 *       <td valign="top">setTrait [inherit]<br>setRGBColorTrait [SVGRGBColor]<br>
 *       </td>
 *       <td valign="top" align="center">rgb(0,0,0)<br>
 *      </td>
 *    </tr>
 *      <tr>
 *       <td width="150" valign="top">display<br>
 *       </td>
 *       <td valign="top">getTrait [inline | none ] <br>
 *       </td>
 *       <td valign="top">setTrait [inline | none | inherit ] </td>
 *       <td valign="top" align="center">"inline"<br>
 *       </td>
 *     </tr>
 *     <tr>
 *       <td width="150" valign="top">fill<br>
 *       </td>
 *       <td valign="top">getRGBColorTrait [null, SVGRGBColor]<br>
 *       </td>
 *       <td valign="top">setRGBColorTrait [SVGRGBColor]<br>setTrait(none | currentColor | inherit)<br>
 *       </td>
 *       <td valign="top" align="center">rgb(0,0,0)<br>
 *       </td>
 *     </tr>
 *     <tr>
 *       <td width="150" valign="top">fill-rule<br>
 *       </td>
 *       <td valign="top">getTrait [nonzero | evenodd] <br>
 *       </td>
 *       <td valign="top">setTrait [nonzero | evenodd | inherit] </td>
 *       <td valign="top" align="center">"nonzero"<br>
 *       </td>
 *     </tr>
 *     <tr>
 *       <td width="150" valign="top">stroke</td>
 *      <td valign="top">getRGBColorTrait [null, SVGRGBColor]</td>
 *       <td valign="top">setRGBColorTrait [SVGRGBColor]<br>setTrait [none | currentColor | inherit]</td>
 *       <td valign="top" align="center">"none"<br>
 *       </td>
 *     </tr>
 *     <tr>
 *       <td valign="top">stroke-dashoffset</td>
 *       <td valign="top">getFloatTrait </td>
 *       <td valign="top">setTrait [inherit]<br>setFloatTrait </td>
 *       <td valign="top" align="center">0.0f<br>
 *       </td>
 *     </tr>
 *     <tr>
 *       <td valign="top">stroke-linecap</td>
 *       <td valign="top">getTrait [butt | round | square]</td>
 *       <td valign="top">setTrait [butt | round | square | inherit]</td>
 *       <td valign="top" align="center">"butt"<br>
 *       </td>
 *     </tr>
 *     <tr>
 *       <td valign="top">stroke-linejoin</td>
 *       <td valign="top">getTrait [miter | round | bevel ]</td>
 *       <td valign="top">setTrait [miter | round | bevel | inherit]</td>
 *       <td valign="top" align="center">"miter"<br>
 *       </td>
 *     </tr>
 *     <tr>
 *       <td valign="top">stroke-miterlimit</td>
 *       <td valign="top">getFloatTrait [ value &gt;= 1]</td>
 *       <td valign="top">setTrait [inherit]<br>setFloatTrait [value &gt;= 1]</td>
 *       <td valign="top" align="center">4.0f<br>
 *       </td>
 *     </tr>
 *     <tr>
 *       <td valign="top">stroke-width</td>
 *       <td valign="top">getFloatTrait [value &gt;= 0]</td>
 *       <td valign="top">setTrait [inherit]<br> setFloatTrait [value &gt;= 0]</td>
 *       <td valign="top" align="center">1.0f<br>
 *       </td>
 *     </tr>
 *     <tr>
 *       <td valign="top">visibility</td>
 *       <td valign="top">getTrait [visible | hidden]</td>
 *       <td valign="top">setTrait [visible | hidden | inherit]</td>
 *       <td valign="top" align="center">"visible"<br>
 *       </td>
 *     </tr>
 *     <tr>
 *       <td valign="top"><br>
 *       </td>
 *       <td valign="top"><br>
 *       </td>
 *       <td valign="top"><br>
 *       </td>
 *       <td valign="top"><br>
 *       </td>
 *     </tr>
 *     <tr>
 *       <td bgcolor="#cccccc" colspan="4" rowspan="1" valign="top">&lt;svg&gt;, &lt;text&gt;, &lt;g&gt;, &lt;a&gt, and &lt;use&gt;;<br>
 *      </td>
 *     </tr>
 *     <tr>
 *       <td valign="top">font-family<br>
 *       </td>
 *       <td valign="top">getTrait [single, computed font-family value]<br>
 *      </td>
 *      <td valign="top">setTrait [same syntax as font-family attribute]<br>
 *     </td>
 *      <td valign="top" align="center"> User-Agent <br>
 *      </td>
 *     </tr>
 *     <tr>
 *       <td width="150" valign="top">font-size<br>
 *       </td>
 *       <td valign="top">getFloatTrait&nbsp; [value &gt;= 0]<br>
 *     </td>
 *       <td valign="top">setFloatTrait [value &gt;= 0]<br>setTrait [inherit]<br>
 *      </td>
 *       <td valign="top" align="center"> User-Agent <br>
 *       </td>     </tr>
 *     <tr>
 *       <td width="150" valign="top">font-style<br>
 *       </td>
 *      <td valign="top">getTrait [normal | italic | oblique ] </td>
 *       <td valign="top">setTrait [normal | italic | oblique | inherit] </td>
 *       <td valign="top" align="center">"normal"<br>
 *       </td>     </tr>
 *     <tr>
 *       <td width="150" valign="top">font-weight<br>
 *       </td>
 *       <td valign="top">getTrait [100 | 200 | 300 <br> | 400 | 500 | 600 | 700 | 800 | 900 ] </td>
 *       <td valign="top">setTrait [normal | bold | bolder | lighter | 100 | 200 | 300 <br />
 *       | 400 | 500 | 600 | 700 | 800 | 900 | inherit] </td>
 *       <td valign="top" align="center">"normal"<br>
 *       </td>     </tr>
 *     <tr>
 *       <td width="150" valign="top">text-anchor<br>
 *       </td>
 *       <td valign="top">getTrait [start | middle | end]<br>
 *       </td>
 *       <td valign="top">setTrait [start | middle | end | inherit ]<br>
 *       </td>
 *       <td valign="top" align="center">"start"<br>
 *       </td>     </tr>
 *     <tr>
 *       <td width="150" colspan="4" rowspan="1" valign="top"><br>
 *       </td>
 *     </tr>
 *     <tr>
 *       <td width="150" bgcolor="#999999" valign="top"><b>Attribute<br>
 *       </b></td>
 *       <td bgcolor="#999999" valign="top"><b>Trait Getter<br>
 *       </b></td>
 *       <td bgcolor="#999999" valign="top"><b>Trait Setter<br>
 *       </b></td>
 *       <td bgcolor="#999999" valign="top"><b>Default Values<br>
 *       </b></td>
 *     </tr>
 *     <tr>
 *       <td width="150" valign="top"><br>
 *       </td>
 *       <td valign="top"><br>
 *       </td>
 *       <td valign="top"><br>
 *       </td>
 *       <td valign="top"><br>
 *       </td>
 *     </tr>
 *     <tr>
 *       <td width="150" bgcolor="#cccccc" colspan="4" rowspan="1" valign="top">
 *       &lt;rect&gt;, &lt;circle&gt;, &lt;ellipse&gt;, &lt;line&gt;, &lt;path&gt;, &lt;g&gt;,
 *       &lt;image&gt;, &lt;text&gt;, &lt;a&gt;, and &lt;use&gt;</td>
 *     </tr>
 *     <tr>
 *       <td width="150" valign="top">transform<br>
 *       </td>
 *       <td valign="top">getMatrixTrait [SVGMatrix]<br>
 *       </td>
 *      <td valign="top">setMatrixTrait [SVGMatrix]<br>
 *       </td>
 *       <td valign="top" align="center">Identity matrix<br>
 *       (1,0,0,1,0,0)<br>
 *     </tr>
 *     <tr>
 *       <td width="150" valign="top"><br>
 *       </td>
 *       <td valign="top"><br>
 *      </td>
 *      <td valign="top"><br>
 *      </td>
 *      <td valign="top"><br>
 *      </td>
 *    </tr>
 *    <tr>
 *      <td width="150" colspan="4" rowspan="1" bgcolor="#cccccc" valign="top">&lt;rect&gt;<br>
 *      </td>
 *    </tr>
 *    <tr>
 *      <td width="150" valign="top">height<br>
 *      </td>
 *      <td valign="top">getFloatTrait [ value &gt;= 0]<br>
 *      </td>
 *      <td valign="top">setFloatTrait [ value &gt;= 0]<br>
 *      </td>
 *      <td valign="top" align="center">REQUIRED<br>(0.0f)<br>
 *      </td>
 *    </tr>
 *    <tr>
 *      <td width="150" valign="top">width<br>
 *      </td>
 *      <td valign="top">getFloatTrait [ value &gt;= 0]</td>
 *      <td valign="top">setFloatTrait [ value &gt;= 0]</td>
 *      <td valign="top" align="center">REQUIRED<br>(0.0f)<br>
 *      </td>
 *    </tr>
 *    <tr>
 *      <td width="150" valign="top">x<br>
 *      </td>
 *      <td valign="top">getFloatTrait<br>
 *      </td>
 *      <td valign="top">setFloatTrait<br>
 *      </td>
 *      <td valign="top" align="center">0.0f<br>
 *      </td>
 *    </tr>
 *    <tr>
 *      <td width="150" valign="top">y<br>
 *      </td>
 *      <td valign="top">getFloatTrait<br>
 *      </td>
 *      <td valign="top">setFloatTrait<br>
 *      </td>
 *      <td valign="top" align="center">0.0f<br>
 *      </td>
 *    </tr>
 *    <tr>
 *      <td width="150" valign="top">rx<br>
 *      </td>
 *      <td valign="top">getFloatTrait [value &gt;= 0]</td>
 *      <td valign="top">setFloatTrait [value &gt;= 0]</td>
 *      <td valign="top" align="center">0.0f<br>
 *      </td>
 *    </tr>
 *    <tr>
 *      <td width="150" valign="top">ry<br>
 *      </td>
 *      <td valign="top">getFloatTrait [value &gt;= 0]<br>
 *      </td>
 *      <td valign="top">setFloatTrait [value &gt;= 0]<br>
 *      </td>
 *      <td valign="top" align="center">0.0f<br>
 *      </td>
 *    </tr>
 *    <tr>
 *      <td width="150" valign="top"><br>
 *      </td>
 *      <td valign="top"><br>
 *      </td>
 *      <td valign="top"><br>
 *      </td>
 *      <td valign="top"><br>
 *      </td>
 *    </tr>
 *    <tr>
 *      <td width="150" colspan="4" rowspan="1" bgcolor="#cccccc" valign="top">&lt;circle&gt;<br>
 *      </td>
 *    </tr>
 *    <tr>
 *      <td width="150" valign="top">cx<br>
 *      </td>
 *      <td valign="top">getFloatTrait<br>
 *      </td>
 *      <td valign="top">setFloatTrait<br>
 *      </td>
 *      <td valign="top" align="center">0.0f<br>
 *      </td>
 *    </tr>
 *    <tr>
 *      <td width="150" valign="top">cy<br>
 *      </td>
 *      <td valign="top">getFloatTrait<br>
 *      </td>
 *      <td valign="top">setFloatTrait<br>
 *      </td>
 *      <td valign="top" align="center">0.0f<br>
 *      </td>
 *    </tr>
 *    <tr>
 *      <td width="150" valign="top">r<br>
 *      </td>
 *      <td valign="top">getFloatTrait [ value &gt;= 0]<br>
 *      </td>
 *      <td valign="top">setFloatTrait [value &gt;= 0]<br>
 *      </td>
 *      <td valign="top" align="center">REQUIRED<br>(0.0f)<br>
 *      </td>
 *    </tr>
 *    <tr>
 *      <td width="150" valign="top"><br>
 *      </td>
 *      <td valign="top"><br>
 *      </td>
 *      <td valign="top"><br>
 *      </td>
 *      <td valign="top"><br>
 *      </td>
 *    </tr>
 *    <tr>
 *      <td width="150" bgcolor="#cccccc" colspan="4" rowspan="1" valign="top">&lt;ellipse&gt;<br>
 *      </td>
 *    </tr>
 *    <tr>
 *      <td width="150" valign="top">cx<br>
 *      </td>
 *      <td valign="top">getFloatTrait<br>
 *      </td>
 *      <td valign="top">setFloatTrait<br>
 *      </td>
 *      <td valign="top" align="center">0.0f<br>
 *      </td>
 *    </tr>
 *    <tr>
 *      <td width="150" valign="top">cy<br>
 *      </td>
 *      <td valign="top">getFloatTrait<br>
 *      </td>
 *      <td valign="top">setFloatTrait<br>
 *      </td>
 *      <td valign="top" align="center">0.0f<br>
 *      </td>
 *    </tr>
 *    <tr>
 *      <td width="150" valign="top">rx<br>
 *      </td>
 *      <td valign="top">getFloatTrait [value &gt;= 0]<br>
 *      </td>
 *      <td valign="top">setFloatTrait [value &gt;= 0]<br>
 *      </td>
 *      <td valign="top" align="center">REQUIRED<br>(0.0f)<br>
 *      </td>
 *    </tr>
 *    <tr>
 *      <td width="150" valign="top">ry<br>
 *      </td>
 *      <td valign="top">getFloatTrait [value &gt;= 0]<br>
 *      </td>
 *      <td valign="top">setFloatTrait [value &gt;= 0]<br>
 *      </td>
 *      <td valign="top" align="center">REQUIRED<br>(0.0f)<br>
 *      </td>
 *    </tr>
 *    <tr>
 *      <td width="150" valign="top"><br>
 *      </td>
 *      <td valign="top"><br>
 *      </td>
 *      <td valign="top"><br>
 *      </td>
 *      <td valign="top"><br>
 *      </td>
 *    </tr>
 *    <tr>
 *      <td width="150" colspan="4" rowspan="1" bgcolor="#cccccc" valign="top">&lt;line&gt;<br>
 *      </td>
 *    </tr>
 *    <tr>
 *      <td width="150" valign="top">x1<br>
 *      </td>
 *      <td valign="top">getFloatTrait<br>
 *      </td>
 *      <td valign="top">setFloatTrait<br>
 *      </td>
 *      <td valign="top" align="center">0.0f<br>
 *      </td>
 *    </tr>
 *    <tr>
 *      <td width="150" valign="top">x2<br>
 *      </td>
 *      <td valign="top">getFloatTrait<br>
 *      </td>
 *      <td valign="top">setFloatTrait<br>
 *      </td>
 *      <td valign="top" align="center">0.0f<br>
 *      </td>
 *    </tr>
 *    <tr>
 *      <td width="150" valign="top">y1<br>
 *      </td>
 *      <td valign="top">getFloatTrait<br>
 *      </td>
 *      <td valign="top">setFloatTrait<br>
 *      </td>
 *      <td valign="top" align="center">0.0f<br>
 *      </td>
 *    </tr>
 *    <tr>
 *      <td width="150" valign="top">y2<br>
 *      </td>
 *      <td valign="top">getFloatTrait<br>
 *      </td>
 *      <td valign="top">setFloatTrait<br>
 *      </td>
 *      <td valign="top" align="center">0.0f<br>
 *      </td>
 *    </tr>
 *    <tr>
 *      <td width="150" valign="top"><br>
 *      </td>
 *      <td valign="top"><br>
 *      </td>
 *      <td valign="top"><br>
 *      </td>
 *      <td valign="top"><br>
 *      </td>
 *    </tr>
 *    <tr>
 *      <td width="150" colspan="4" rowspan="1" bgcolor="#cccccc" valign="top">&lt;path&gt; (path-length is not supported)<br>
 *      </td>
 *    </tr>
 *    <tr>
 *      <td width="150" valign="top">d<br>
 *      </td>
 *      <td valign="top">getPathTrait [SVGPath]<br>
 *      </td>
 *      <td valign="top">setPathTrait [SVGPath]<br>
 *      </td>
 *      <td valign="top" align="center">REQUIRED<br>(Empty SVGPath)<br>
 *      </td>
 *    </tr>
 *    <tr>
 *      <td width="150" valign="top"><br>
 *      </td>
 *      <td valign="top"><br>
 *      </td>
 *      <td valign="top"><br>
 *      </td>
 *      <td valign="top"><br>
 *      </td>
 *    </tr>
 *    <tr>
 *      <td width="150" colspan="4" rowspan="1" bgcolor="#cccccc" valign="top">&lt;image&gt;<br>
 *      </td>
 *    </tr>
 *    <tr>
 *      <td width="150" valign="top">x<br>
 *      </td>
 *      <td valign="top">getFloatTrait <br>
 *      </td>
 *      <td valign="top">setFloatTrait<br>
 *      </td>
 *      <td valign="top" align="center">0.0f<br>
 *      </td>
 *    </tr>
 *    <tr>
 *      <td width="150" valign="top">y<br>
 *      </td>
 *      <td valign="top">getFloatTrait<br>
 *      </td>
 *      <td valign="top">setFloatTrait<br>
 *      </td>
 *      <td valign="top" align="center">0.0f<br>
 *      </td>
 *    </tr>
 *    <tr>
 *      <td width="150" valign="top">width<br>
 *      </td>
 *      <td valign="top">getFloatTrait [value &gt;= 0]<br>
 *      </td>
 *      <td valign="top">setFloatTrait [value &gt;= 0]<br>
 *      </td>
 *      <td valign="top" align="center">REQUIRED<br>(0.0f)<br>
 *      </td>
 *    </tr>
 *    <tr>
 *      <td width="150" valign="top">height<br>
 *      </td>
 *      <td valign="top">getFloatTrait [value &gt;= 0]<br>
 *      </td>
 *      <td valign="top">setFloatTrait [value &gt;= 0]<br>
 *      </td>
 *      <td valign="top" align="center">REQUIRED<br>(0.0f)<br>
 *      </td>
 *    </tr>
 *    <tr>
 *      <td width="150" valign="top">xlink:href<br>
 *      </td>
 *      <td valign="top">getTrait NS[absolute URI]<br>
 *      </td>
 *      <td valign="top">setTraitNS [non local-URI value]<br>
 *      </td>
 *      <td valign="top" align="center">REQUIRED<br>( "" )<br>
 *      </td>
 *    </tr>
 *    <tr>
 *      <td width="150" valign="top"><br>
 *      </td>
 *      <td valign="top"><br>
 *      </td>
 *      <td valign="top"><br>
 *      </td>
 *      <td valign="top"><br>
 *      </td>
 *    </tr>
 *    <tr>
 *      <td width="150" colspan="4" rowspan="1" bgcolor="#cccccc" valign="top">&lt;use&gt;<br>
 *      </td>
 *    </tr>
 *    <tr>
 *      <td width="150" valign="top">x<br>
 *      </td>
 *      <td valign="top">getFloatTrait<br>
 *      </td>
 *      <td valign="top">setFloatTrait<br>
 *      </td>
 *      <td valign="top" align="center">0.0f<br>
 *      </td>
 *    </tr>
 *    <tr>
 *      <td width="150" valign="top">y<br>
 *      </td>
 *      <td valign="top">getFloatTrait<br>
 *      </td>
 *      <td valign="top">setFloatTrait<br>
 *      </td>
 *      <td valign="top" align="center">0.0f<br>
 *      </td>
 *    </tr>
 *    <tr>
 *      <td valign="top">xlink:href<br>
 *      </td>
 *      <td valign="top">getTraitNS[absolute URI]<br>
 *      </td>
 *      <td valign="top">setTraitNS<br>
 *      </td>
 *      <td valign="top" align="center">""<br>
 *      </td>
 *    </tr>
 *    <tr>
 *      <td width="150" valign="top"><br>
 *      </td>
 *      <td valign="top"><br>
 *      </td>
 *      <td valign="top"><br>
 *      </td>
 *      <td valign="top"><br>
 *      </td>
 *    </tr>
 *    <tr>
 *    <tr>
 *      <td width="150" colspan="4" rowspan="1" bgcolor="#cccccc" valign="top">&lt;a&gt;<br>
 *      </td>
 *    </tr>
 *    <tr>
 *      <td width="150" valign="top">target<br>
 *      </td>
 *      <td valign="top">getTrait<br>
 *      </td>
 *      <td valign="top">setTrait<br>
 *      </td>
 *      <td valign="top" align="center">""<br>
 *      </td>
 *    </tr>
 *    <tr>
 *      <td valign="top">xlink:href<br>
 *      </td>
 *      <td valign="top">getTraitNS[absolute URI]<br>
 *      </td>
 *      <td valign="top">setTraitNS<br>
 *      </td>
 *      <td valign="top" align="center">""<br>
 *      </td>
 *    </tr>
 *    <tr>
 *      <td width="150" valign="top"><br>
 *      </td>
 *      <td valign="top"><br>
 *      </td>
 *      <td valign="top"><br>
 *      </td>
 *      <td valign="top"><br>
 *      </td>
 *    </tr>
 *    <tr>
 *      <td width="*" colspan="4" rowspan="1" bgcolor="#cccccc" valign="top">&lt;text&gt;<br />(Notes:
 * For 'x' and 'y', it is only possible
 * to provide floating point scalar values; an array of x or y values is not supported. <br />
 * 'rotate' attribute is not supported.)<br/>
 *      </td>
 *    </tr>
 *    <tr>
 *      <td width="150" valign="top">x<br>
 *      </td>
 *      <td valign="top">getFloatTrait<br>
 *      </td>
 *      <td valign="top">setFloatTrait<br>
 *      </td>
 *      <td valign="top" align="center">0.0f<br>
 *      </td>
 *    </tr>
 *    <tr>
 *      <td width="150" valign="top">y<br>
 *      </td>
 *      <td valign="top">getFloatTrait<br>
 *      </td>
 *      <td valign="top">setFloatTrait<br>
 *      </td>
 *      <td valign="top" align="center">0.0f<br>
 *      </td>
 *    </tr>
 *    <tr>
 *      <td width="150" valign="top">#text<br>
 *      </td>
 *      <td valign="top">getTrait [not null]<br>
 *      </td>
 *      <td valign="top">setTrait [not null]<br>
 *      </td>
 *      <td valign="top" align="center">""<br>
 *      </td>
 *    </tr>
 *    <tr>
 *      <td width="150" valign="top"><br>
 *      </td>
 *      <td valign="top"><br>
 *      </td>
 *      <td valign="top"><br>
 *      </td>
 *      <td valign="top"><br>
 *      </td>
 *    </tr>
 *    <tr>
 *      <td width="150" colspan="4" rowspan="1" bgcolor="#cccccc" valign="top">&lt;svg&gt;<br>
 *      </td>
 *    </tr>
 *    <tr>
 *      <td width="150" valign="top">version<br>
 *      </td>
 *      <td valign="top">getTrait<br>
 *      </td>
 *      <td valign="top">Not available (readonly)<br>
 *      </td>
 *      <td valign="top" align="center">"1.1"<br>
 *      </td>
 *    </tr>
 *    <tr>
 *      <td width="150" valign="top">baseProfile<br>
 *      </td>
 *      <td valign="top">getTrait<br>
 *      </td>
 *      <td valign="top">Not available (readonly)<br>
 *      </td>
 *      <td valign="top" align="center">"tiny"<br>
 *      </td>
 *    </tr>
 *    <tr>
 *      <td width="150" valign="top">viewBox<br>
 *      </td>
 *      <td valign="top">getRectTrait [null, SVGRect]<br>
 *      </td>
 *      <td valign="top">setRectTrait [SVGRect]<br>
 *      </td>
 *      <td valign="top" align="center">null<br>
 *      </td>
 *    </tr>
 *    <tr>
 *      <td width="150" valign="top">zoomAndPan<br>
 *      </td>
 *      <td valign="top">getTrait [disable | magnify]<br>
 *      </td>
 *      <td valign="top">setTrait [disable | magnify]<br>
 *      </td>
 *      <td valign="top" align="center">"magnify"<br>
 *      </td>
 *    </tr>
 *    <tr>
 *      <td width="150" valign="top"><br>
 *      </td>
 *      <td valign="top"><br>
 *      </td>
 *      <td valign="top"><br>
 *      </td>
 *      <td valign="top"><br>
 *      </td>
 *     </tr>
 *   </tbody>
 * </table>
 */

public interface SVGElement extends Element, EventTarget
{

    /**
     * Sets the Element's id attribute.
     * @param Id the value of Id to be set for this Element.
     * @throws DOMException with error code NO_MODIFICATION_ALLOWED_ERR is raised if an attempt is made to change an existing Id.
     * @throws DOMException with error code INVALID_ACCESS_ERR is raised if the Id is not unique i.e. if this Id already exists in the document.
     * @throws NullPointerException if <code>Id</code> is null.
     * @throws SecurityException if the application does not have the necessary privilege rights
     * to access this (SVG) content.
     */
    public void setId(String Id)
    throws DOMException;

    /**
     * Returns the Element's Id, null if no id specified.
     * @return the Element's Id.
     */
    public String getId();

    /**
     * Returns the first child element node of this element. <code>null</code> if this element has no child elements.
     * @return the first child element node of this element.
     */
    public Element getFirstElementChild();

    /**
     * Returns the next sibling element node of this element. <code>null</code> if this element has no element sibling
     * nodes that come after this one in the document tree.
     * @return the next sibling element node of this element.
     */
    public Element getNextElementSibling();


    /**
     * Returns the trait value as String. In SVG Tiny only certain traits can be obtained as a String value. Syntax of the
     * returned String matches the syntax of the corresponding attribute. This element is exactly equivalent to {@link org.w3c.dom.svg.SVGElement#getTraitNS getTraitNS}
     * with namespaceURI set to null.
     *
     * @param name the name of the trait to retrieve.
     * @return the trait value as String for the specified name.
     * @throws DOMException with error code NOT_SUPPORTED_ERR if the requested trait is not supported on this element or null.
     * @throws DOMException with error code TYPE_MISMATCH_ERR if requested trait's computed value cannot be converted to a String (SVG Tiny only).
     * @throws SecurityException if the application does not have the necessary privilege rights
     * to access this (SVG) content.
     */
    public String getTrait(String name)
    throws DOMException;

    /**
     * Same as {@link org.w3c.dom.svg.SVGElement#getTrait getTrait}, but for namespaced traits. Parameter name must be a non-qualified trait name, i.e. without prefix.
     *
     * @param namespaceURI the namespaceURI of the trait to retrieve.
     * @param name the name of the trait to retrieve.
     * @return the trait value as String for the specified name.
     * @throws DOMException with error code NOT_SUPPORTED_ERR if the requested trait is not supported on this element or null.
     * @throws DOMException with error code TYPE_MISMATCH_ERR if requested trait's computed value cannot be converted to a String (SVG Tiny only).
     * @throws SecurityException if the application does not have the necessary privilege rights
     * to access this (SVG) content.
     */
    public String getTraitNS(String namespaceURI, String name)
    throws DOMException;

    /**
     * Get the trait value as float.
     *
     * @param name the name of the trait to retrieve.
     * @return the trait value as float for the specified name.
     * @throws DOMException with error code NOT_SUPPORTED_ERR if the requested trait is not supported on this element or null.
     * @throws DOMException with error code TYPE_MISMATCH_ERR if requested trait's computed value cannot be converted to a float
     * @throws SecurityException if the application does not have the necessary privilege rights
     * to access this (SVG) content.
     */
    public float getFloatTrait(String name)
    throws DOMException;

    /**
     * Returns the trait value as {@link org.w3c.dom.svg.SVGMatrix SVGMatrix}. The returned object is a copy of the actual trait value and will not change if
     * the corresponding trait changes.
     *
     * @param name the name of the trait to retrieve.
     * @return the trait value as SVGMatrix for the specified name.
     * @throws DOMException with error code NOT_SUPPORTED_ERR if the requested trait is not supported on this element or null.
     * @throws DOMException with error code TYPE_MISMATCH_ERR if requested trait's computed value cannot be converted to {@link org.w3c.dom.svg.SVGMatrix SVGMatrix}
     * @throws SecurityException if the application does not have the necessary privilege rights
     * to access this (SVG) content.
     */
    public SVGMatrix getMatrixTrait(String name)
    throws DOMException;

    /**
     * Returns the trait value as {@link org.w3c.dom.svg.SVGRect SVGRect}. The returned object is a copy of the actual trait value and will not change if
     * the corresponding trait changes.
     *
     * @param name the name of the trait to retrieve.
     * @return the trait value as SVGRect for the specified name.
     * @throws DOMException with error code NOT_SUPPORTED_ERR if the requested trait is not supported on this element or null.
     * @throws DOMException with error code TYPE_MISMATCH_ERR if requested trait's computed value cannot be converted to {@link org.w3c.dom.svg.SVGRect SVGRect}
     * @throws SecurityException if the application does not have the necessary privilege rights
     * to access this (SVG) content.
     */
    public SVGRect getRectTrait(String name)
    throws DOMException;

    /**
     * Returns the trait value as {@link org.w3c.dom.svg.SVGPath SVGPath}. The returned object is a copy of the actual trait value and will not change if
     * the corresponding trait changes.
     *
     * @param name the name of the trait to retrieve.
     * @return the trait value as SVGPath for the specified name.
     * @throws DOMException with error code NOT_SUPPORTED_ERR if the requested trait is not supported on this element or null.
     * @throws DOMException with error code TYPE_MISMATCH_ERR if requested trait's computed value cannot be converted to {@link org.w3c.dom.svg.SVGPath SVGPath}
     * @throws SecurityException if the application does not have the necessary privilege rights
     * to access this (SVG) content.
     */
    public SVGPath getPathTrait(String name)
    throws DOMException;

    /**
     * Returns the trait value as {@link org.w3c.dom.svg.SVGRGBColor SVGRGBColor}. The returned object is a copy of the trait value and will not change if
     * the corresponding trait changes. If the actual trait value is not an RGBColor (i.e. "none"), this method will return null.
     *
     * @param name the name of the trait to retrieve.
     * @return the trait value as SVGRGBColor for the specified name.
     * @throws DOMException with error code NOT_SUPPORTED_ERR if the requested trait is not supported on this element or null.
     * @throws DOMException with error code TYPE_MISMATCH_ERR if requested trait's computed value cannot be converted to {@link org.w3c.dom.svg.SVGRGBColor SVGRGBColor}
     * @throws SecurityException if the application does not have the necessary privilege rights
     * to access this (SVG) content.
     */
    public SVGRGBColor getRGBColorTrait(String name)
    throws DOMException;

    /**
     * Set the trait value as String. In SVG Tiny only certain traits can be set through a String value. The syntax of the String
     * that should be given as a value must be the same as syntax of the corresponding XML attribute value. Exactly equivalent
     * to {@link org.w3c.dom.svg.SVGElement#setTraitNS setTraitNS} with namespaceURI attribute set to null.
     *
     * @param name the name of the trait to be set.
     * @param value the value of the trait to be set as String.
     * @throws DOMException with error code NOT_SUPPORTED_ERR if the requested trait is not supported on this element or null.
     * @throws DOMException with error code TYPE_MISMATCH_ERR if the requested trait's value cannot be specified as a String
     * @throws DOMException with error code INVALID_ACCESS_ERR if the input value is an invalid value for the given trait or null.
     * @throws DOMException with error code NO_MODIFICATION_ALLOWED_ERR: if attempt is made to change readonly trait.
     * @throws SecurityException if the application does not have the necessary privilege rights
     * to access this (SVG) content.
     */
    public void setTrait(String name, String value)
    throws DOMException;

    /**
     * Same as {@link org.w3c.dom.svg.SVGElement#setTrait setTrait}, but for namespaced traits. Parameter name must be a non-qualified trait name, i.e. without prefix.
     *
     * @param namespaceURI the namespaceURI of the trait to be set.
     * @param name the name of the trait to be set.
     * @param value the value of the trait to be set as String.
     * @throws DOMException with error code NOT_SUPPORTED_ERR if the requested trait is not supported on this element or null.
     * @throws DOMException with error code TYPE_MISMATCH_ERR if the requested trait's value cannot be specified as a String
     * @throws DOMException with error code INVALID_ACCESS_ERR if the input value is an invalid value for the given trait or null.
     * This error is also thrown when the &lt;use&gt; element is hooked into the document tree and the the value of xlink:href is set invalid.
     * @throws DOMException with error code NO_MODIFICATION_ALLOWED_ERR: if attempt is made to change readonly trait.
     * @throws SecurityException if the application does not have the necessary privilege rights
     * to access this (SVG) content.
     */

    public void setTraitNS(String namespaceURI, String name, String value)
    throws DOMException;

    /**
     * Set the trait value as float.
     *
     * @param name the name of the trait to be set.
     * @param value the value of the trait to be set as float.
     * @throws DOMException with error code NOT_SUPPORTED_ERR if the requested trait is not supported on this element.
     * @throws DOMException with error code TYPE_MISMATCH_ERR if the requested trait's value cannot be specified as a float
     * @throws DOMException with error code INVALID_ACCESS_ERR if the input value is an invalid value for the given trait.
     * @throws SecurityException if the application does not have the necessary privilege rights
     * to access this (SVG) content.
     */
    public void setFloatTrait(String name, float value)
    throws DOMException;

    /**
     * Set the trait value as {@link org.w3c.dom.svg.SVGMatrix SVGMatrix}. Values in SVGMatrix are copied in the trait so subsequent changes to the given
     * SVGMatrix have no effect on the value of the trait.
     *
     * @param name the name of the trait to be set.
     * @param matrix the value of the trait to be set as SVGMatrix.
     * @throws DOMException with error code NOT_SUPPORTED_ERR if the requested trait is not supported on this element or null.
     * @throws DOMException with error code TYPE_MISMATCH_ERR if the requested trait's value cannot be specified as an {@link org.w3c.dom.svg.SVGMatrix SVGMatrix}
     * @throws DOMException with error code INVALID_ACCESS_ERR if the input matrix value is null.
     * @throws SecurityException if the application does not have the necessary privilege rights
     * to access this (SVG) content.
     */

    public void setMatrixTrait(String name, SVGMatrix matrix)
    throws DOMException;

    /**
     * Set the trait value as {@link org.w3c.dom.svg.SVGRect SVGRect}. Values in SVGRect are copied in the trait so subsequent changes to the given
     * SVGRect have no effect on the value of the trait.
     *
     * @param name the name of the trait to be set.
     * @param rect the value of the trait to be set as SVGRect.
     * @throws DOMException with error code NOT_SUPPORTED_ERR if the requested trait is not supported on this element or null.
     * @throws DOMException with error code TYPE_MISMATCH_ERR if the requested trait's value cannot be specified as an {@link org.w3c.dom.svg.SVGRect SVGRect}
     * @throws DOMException with error code INVALID_ACCESS_ERR if the input value is an invalid value for the given trait or null.
     * SVGRect is invalid if the width or height values are set to negative.
     * @throws SecurityException if the application does not have the necessary privilege rights
     * to access this (SVG) content.
     */

    public void setRectTrait(String name, SVGRect rect)
    throws DOMException;

    /**
     * Set the trait value as {@link org.w3c.dom.svg.SVGPath SVGPath}. Values in SVGPath are copied in the trait so subsequent changes to the given
     * SVGPath have no effect on the value of the trait.
     *
     * @param name the name of the trait to be set.
     * @param path the value of the trait to be set as SVGPath.
     * @throws DOMException with error code NOT_SUPPORTED_ERR if the requested trait is not supported on this element or null.
     * @throws DOMException with error code TYPE_MISMATCH_ERR if the requested trait's value cannot be specified as an {@link org.w3c.dom.svg.SVGPath SVGPath}
     * @throws DOMException with error code INVALID_ACCESS_ERR if the input value is an invalid value for the given trait or null.
     * SVGPath is invalid if it begins with any segment other than MOVE_TO segment. Note that an empty SVGPath is still a valid value.
     * @throws SecurityException if the application does not have the necessary privilege rights
     * to access this (SVG) content.
     */

    public void setPathTrait(String name, SVGPath path)
    throws DOMException;

    /**
     * Set the trait value as {@link org.w3c.dom.svg.SVGRGBColor SVGRGBColor}. Values in SVGRGBColor are copied in the trait so subsequent changes to the given
     * SVGRGBColor have no effect on the value of the trait.
     *
     * @param name the name of the trait to be set.
     * @param color the value of the trait to be set as SVGRGBColor.
     * @throws DOMException with error code NOT_SUPPORTED_ERR if the requested trait is not supported on this element or null.
     * @throws DOMException with error code TYPE_MISMATCH_ERR if the requested trait's value cannot be specified as an {@link org.w3c.dom.svg.SVGRGBColor SVGRGBColor}
     * @throws DOMException with error code INVALID_ACCESS_ERR if the input value is null.
     * @throws SecurityException if the application does not have the necessary privilege rights
     * to access this (SVG) content.
     */

    public void setRGBColorTrait(String name, SVGRGBColor color)
    throws DOMException;
}
