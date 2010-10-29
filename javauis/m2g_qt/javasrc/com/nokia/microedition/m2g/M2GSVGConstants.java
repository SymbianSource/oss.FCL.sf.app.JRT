/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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

package com.nokia.microedition.m2g;

import java.util.Hashtable;
import java.util.Enumeration;
import java.util.Vector;
import org.w3c.dom.svg.SVGPath;

/*
 * General constants
 */
public class M2GSVGConstants
{

    //--------------------------------------------------
    // STATIC CONSTANTS - General exception messages
    //--------------------------------------------------
    public static final String ACCESS_RIGHTS_ESTR =
        "Not necessary privilege rights to access this (SVG) content.";

    //--------------------------------------------------
    // STATIC CONSTANTS
    //--------------------------------------------------
    static final String SVG_NAMESPACE_URI =
        "http://www.w3.org/2000/svg";
    static final String XLINK_NAMESPACE_URI =
        "http://www.w3.org/1999/xlink";
    static final String ROOT_SVG_ELEMENT_NAME =
        "svg";

    //--------------------------------------------------
    // STATIC CONSTANTS - Supported event types
    //--------------------------------------------------
    static final String EVENT_ACTIVATE = "DOMActivate";
    static final String EVENT_CLICK = "click";
    static final String EVENT_FOCUS_IN = "DOMFocusIn";
    static final String EVENT_FOCUS_OUT = "DOMFocusOut";

    //--------------------------------------------------
    // STATIC CONSTANTS - SVGT element ids
    //--------------------------------------------------
    static final short EL_UNDEFINED = -1;
    static final short EL_A = 0;
    static final short EL_ANIMATE = 1;
    static final short EL_ANIMATECOLOR = 2;
    static final short EL_ANIMATEMOTION = 3;
    static final short EL_ANIMATETRANSFORM = 4;
    static final short EL_CIRCLE = 5;
    static final short EL_DEFS = 6;
    static final short EL_DESC = 7;
    static final short EL_ELLIPSE = 8;
    static final short EL_FONT = 9;
    static final short EL_FONTFACE = 10;
    static final short EL_FONTFACENAME = 11;
    static final short EL_FONTFACESRC = 12;
    static final short EL_FOREIGNOBJECT = 13;
    static final short EL_G = 14;
    static final short EL_GLYPH = 15;
    static final short EL_HKERN = 16;
    static final short EL_IMAGE = 17;
    static final short EL_LINE = 18;
    static final short EL_METADATA = 19;
    static final short EL_MISSINGGLYPH = 20;
    static final short EL_MPATH = 21;
    static final short EL_PATH = 22;
    static final short EL_POLYGON = 23;
    static final short EL_POLYLINE = 24;
    static final short EL_RECT = 25;
    static final short EL_SET = 26;
    static final short EL_SVG = 27;
    static final short EL_SWITCH = 28;
    static final short EL_TEXT = 29;
    static final short EL_TITLE = 30;
    static final short EL_USE = 31;

    //--------------------------------------------------
    // STATIC CONSTANTS - SVGT Attribute ids
    //--------------------------------------------------
    static final short AT_UNDEFINED = -1;
    static final short AT_ACCENTHEIGHT = 50;
    static final short AT_ACCUMULATE = 51;
    static final short AT_ADDITIVE = 52;
    static final short AT_ALPHABETIC = 53;
    static final short AT_ARABICFORM = 54;
    static final short AT_ASCENT = 55;
    static final short AT_ATTRIBUTENAME = 56;
    static final short AT_ATTRIBUTETYPE = 57;
    static final short AT_BASEPROFILE = 58;
    static final short AT_BBOX = 59;
    static final short AT_BEGIN = 60;
    static final short AT_BY = 61;
    static final short AT_CALCMODE = 62;
    static final short AT_CAPHEIGHT = 63;
    static final short AT_COLOR = 64;
    static final short AT_COLORRENDERING = 65;
    // static final short AT_CONTENT = 66;
    static final short AT_CX = 67;
    static final short AT_CY = 68;
    static final short AT_D = 69;
    static final short AT_DESCENT = 70;
    static final short AT_DISPLAY = 71;
    static final short AT_DUR = 72;
    static final short AT_END = 73;
    static final short AT_FILL = 74;
    static final short AT_FILLRULE = 75;
    static final short AT_FONTFAMILY = 76;
    static final short AT_FONTSIZE = 77;
    static final short AT_FONTSTRETCH = 78;
    static final short AT_FONTSTYLE = 79;
    static final short AT_FONTVARIANT = 80;
    static final short AT_FONTWEIGHT = 81;
    static final short AT_FROM = 82;
    static final short AT_G1 = 83;
    static final short AT_G2 = 84;
    static final short AT_GLYPHNAME = 85;
    static final short AT_HANGING = 86;
    static final short AT_HEIGHT = 87;
    static final short AT_HORIZADVX = 88;
    static final short AT_HORIZORIGINX = 89;
    static final short AT_ID = 90;
    static final short AT_IDEOGRAPHIC = 91;
    static final short AT_K = 92;
    static final short AT_KEYPOINTS = 93;
    static final short AT_KEYSPLINES = 94;
    static final short AT_KEYTIMES = 95;
    static final short AT_LANG = 96;
    static final short AT_MATHEMATICAL = 97;
    static final short AT_MAX = 98;
    static final short AT_MIN = 99;
    static final short AT_NAME = 100;
    static final short AT_ORIGIN = 101;
    static final short AT_OVERLINEPOSITION = 102;
    static final short AT_OVERLINETHICKNESS = 103;
    static final short AT_PANOSE1 = 104;
    static final short AT_PATH = 105;
    static final short AT_PATHLENGTH = 106;
    static final short AT_POINTS = 107;
    static final short AT_PRESERVEASPECTRATIO = 108;
    static final short AT_R = 109;
    static final short AT_REPEATCOUNT = 110;
    static final short AT_REPEATDUR = 111;
    static final short AT_REQUIREDEXTENSIONS = 112;
    static final short AT_REQUIREDFEATURES = 113;
    static final short AT_RESTART = 114;
    static final short AT_ROTATE = 115;
    static final short AT_RX = 116;
    static final short AT_RY = 117;
    static final short AT_SLOPE = 118;
    static final short AT_STEMH = 119;
    static final short AT_STEMV = 120;
    static final short AT_STRIKETHROUGHPOSITION = 121;
    static final short AT_STRIKETHROUGHTHICKNESS = 122;
    static final short AT_STROKE = 123;
    static final short AT_STROKEDASHARRAY = 124;
    static final short AT_STROKEDASHOFFSET = 125;
    static final short AT_STROKELINECAP = 126;
    static final short AT_STROKELINEJOIN = 127;
    static final short AT_STROKEMITERLIMIT = 128;
    static final short AT_STROKEWIDTH = 129;
    static final short AT_STYLE = 130;
    static final short AT_SYSTEMLANGUAGE = 131;
    static final short AT_TARGET = 132;
    static final short AT_TEXTANCHOR = 133;
    static final short AT_TO = 134;
    static final short AT_TRANSFORM = 135;
    static final short AT_TYPE = 136;
    static final short AT_U1 = 137;
    static final short AT_U2 = 138;
    static final short AT_UNDERLINEPOSITION = 139;
    static final short AT_UNDERLINETHICKNESS = 140;
    static final short AT_UNICODE = 141;
    static final short AT_UNICODERANGE = 142;
    static final short AT_UNITSPEREM = 143;
    static final short AT_VALUES = 144;
    static final short AT_VERSION = 145;
    static final short AT_VIEWBOX = 146;
    static final short AT_VISIBILITY = 147;
    static final short AT_WIDTH = 148;
    static final short AT_WIDTHS = 149;
    static final short AT_X = 150;
    static final short AT_XHEIGHT = 151;
    static final short AT_X1 = 152;
    static final short AT_X2 = 153;
    static final short AT_XLINKACTUATE = 154;
    static final short AT_XLINKARCROLE = 155;
    static final short AT_XLINKHREF = 156;
    static final short AT_XLINKROLE = 157;
    static final short AT_XLINKSHOW = 158;
    static final short AT_XLINKTITLE = 159;
    static final short AT_XLINKTYPE = 160;
    static final short AT_XMLBASE = 161;
    static final short AT_XMLLANG = 162;
    static final short AT_XMLSPACE = 163;
    static final short AT_Y = 164;
    static final short AT_Y1 = 165;
    static final short AT_Y2 = 166;
    static final short AT_ZOOMANDPAN = 167;
    // not in the spec but used to correctly implement animateMotion
    static final short AT_MOTIONTRANSFORM = 168;
    // not actually an attribute, but needed for desc, title, and text
    static final short AT_STRING = 169;
    static final short AT_TEXTDECORATION = 170;
    static final short AT_HORIZORIGINY = 171;
    static final short AT_MOUSEEVENT = 172;
    static final short AT_USERBBOX = 173;

    //--------------------------------------------------
    // STATIC CONSTANTS - Error flags
    //--------------------------------------------------
    static final short DEFAULT_INHERIT = -2; // = null on native engine;
    static final short SVG_ERROR = -1;
    static final short TEXT_NOT_SHORT = -3;

    //--------------------------------------------------
    // STATIC CONSTANTS - A constant for each type of value
    //--------------------------------------------------
    static final short VAL_EVENT = 200;
    static final short VAL_INTEGER = 201;
    static final short VAL_PAINT = 202;
    static final short VAL_POINT = 203;
    static final short VAL_REAL = 204;
    static final short VAL_STRING = 205;
    static final short VAL_TIME = 206;
    static final short VAL_TRANSFORM = 207;
    static final short VAL_VECTOR = 208;
    // These are used for animation purposes
    static final short VAL_VECTOR_POINT = 215;
    static final short VAL_VECTOR_REAL = 216;
    static final short VAL_VECTOR_STRING = 217;
    static final short VAL_VECTOR_PATH = 218;
    // VAL_VECTOR_X + VECTOR_TO_VAL = VAL_X
    static final short VECTOR_TO_VAL = -12;

    //--------------------------------------------------
    // STATIC CONSTANTS - Constants for "choices"
    //--------------------------------------------------
    // Preserve aspect ratio constants
    static final short PAR_NONE = 310;
    static final short PAR_XMIDYMID = 311;
    // Zoom and pan constants
    static final short ZPN_MAGNIFY = 320;
    static final short ZPN_DISABLE = 321;
    // Paint
    static final short PAINT_NONE = 325;
    static final short PAINT_CURRENT = 326;
    static final short PAINT_COLOR = 327;
    static final short PAINT_INHERIT = 328;
    // Fonts
    static final short FONT_ALL = 330;
    static final short FONT_NORMAL = 331;
    static final short FONT_STYLE_ITALIC = 332;
    static final short FONT_STYLE_OBLIQUE = 333;
    static final short FONT_VARIANT_SMALLCAPS = 334;
    static final short FONT_WEIGHT_BOLD = 335;
    static final short FONT_WEIGHT_BOLDER = 336;
    static final short FONT_WEIGHT_LIGHTER = 337;
    static final short FONT_WEIGHT_100 = 338;
    static final short FONT_WEIGHT_200 = 339;
    static final short FONT_WEIGHT_300 = 340;
    static final short FONT_WEIGHT_400 = 341;
    static final short FONT_WEIGHT_500 = 342;
    static final short FONT_WEIGHT_600 = 343;
    static final short FONT_WEIGHT_700 = 344;
    static final short FONT_WEIGHT_800 = 345;
    static final short FONT_WEIGHT_900 = 346;
    static final short FONT_STRETCH_WIDER = 347;
    static final short FONT_STRETCH_NARROWER = 348;
    static final short FONT_STRETCH_ULTRA_COND = 349;
    static final short FONT_STRETCH_EXTRA_COND = 350;
    static final short FONT_STRETCH_COND = 351;
    static final short FONT_STRETCH_SEMI_COND = 352;
    static final short FONT_STRETCH_SEMI_EXPD = 353;
    static final short FONT_STRETCH_EXPD = 354;
    static final short FONT_STRETCH_EXTRA_EXPD = 355;
    static final short FONT_STRETCH_ULTRA_EXPD = 356;
    // Text
    static final short TEXT_ANCHOR_START = 360;
    static final short TEXT_ANCHOR_MIDDLE = 361;
    static final short TEXT_ANCHOR_END = 362;
    static final short TEXT_UNDER_LINE = 363;
    static final short TEXT_OVER_LINE = 364;
    static final short TEXT_LINE_THROUGH = 365;
    // These are actual sizes in 8:8 fixed point, not "choices"
    static final int FONT_SIZE_XXSMALL = 0x20000;
    static final int FONT_SIZE_XSMALL = 0x40000;
    static final int FONT_SIZE_SMALL = 0x60000;
    static final int FONT_SIZE_MEDIUM = 0xa0000;
    static final int FONT_SIZE_LARGE = 0x100000;
    static final int FONT_SIZE_XLARGE = 0x140000;
    static final int FONT_SIZE_XXLARGE = 0x180000;

    //--------------------------------------------------
    // STATIC CONSTANTS - Styles
    //--------------------------------------------------
    // Fill
    static final short FILL_RULE_EVENODD = 375;
    static final short FILL_RULE_NONZERO = 376;
    // Display
    static final short DISPLAY_NONE = 380;
    static final short DISPLAY_OTHER = 381;
    // Visibility
    static final short VISIBILITY_VISIBLE = 385;
    static final short VISIBILITY_OTHER = 386;
    // Color-rendering
    static final short COLOR_RENDERING_AUTO = 390;
    static final short COLOR_RENDERING_SPEED = 391;
    static final short COLOR_RENDERING_QUALITY = 392;
    // Strokes
    // MUST PRESERVE ORDER!!
    static final short STROKE_LINECAP_BUTT = 395;
    static final short STROKE_LINECAP_ROUND = 396;
    static final short STROKE_LINECAP_SQUARE = 397;
    static final short STROKE_LINEJOIN_MITER = 400;
    static final short STROKE_LINEJOIN_ROUND = 401;
    static final short STROKE_LINEJOIN_BEVEL = 402;

    //--------------------------------------------------
    // STATIC CONSTANTS - Animation
    //--------------------------------------------------
    static final short ANIM_INDEFINITE = 445;
    static final short ACCUMULATE_NONE = 450;
    static final short ACCUMULATE_SUM = 451;
    static final short ADDITIVE_REPLACE = 455;
    static final short ADDITIVE_SUM = 456;
    static final short CALC_MODE_DISCRETE = 460;
    static final short CALC_MODE_LINEAR = 461;
    static final short CALC_MODE_PACED = 462;
    static final short CALC_MODE_SPLINE = 463;
    static final short FILL_REMOVE = 465;
    static final short FILL_FREEZE = 466;
    static final short RESTART_ALWAYS = 470;
    static final short RESTART_NEVER = 471;
    static final short RESTART_WHENNOTACTIVE = 472;
    static final short TYPE_TRANSLATE = 475;
    static final short TYPE_SCALE = 476;
    static final short TYPE_ROTATE = 477;
    static final short TYPE_SKEWX = 478;
    static final short TYPE_SKEWY = 479;
    static final short ATTR_TYPE_CSS = 485;
    static final short ATTR_TYPE_XML = 486;
    static final short ATTR_TYPE_AUTO = 487;
    static final short ROTATE_AUTO = 490;
    static final short ROTATE_AUTOREVERSE = 491;
    static final short ANIM_FROM_TO = 500;
    static final short ANIM_FROM_BY = 501;
    static final short ANIM_BY = 502;
    static final short ANIM_TO = 503;
    static final short ANIM_VALUES = 504;
    static final short ANIM_PATH = 505;

    //--------------------------------------------------
    // STATIC CONSTANTS - Path Commands
    //--------------------------------------------------
    static final short PATH_COMMAND_M = 600;
    static final short PATH_COMMAND_m = 601;
    static final short PATH_COMMAND_Z = 602;
    static final short PATH_COMMAND_L = 603;
    static final short PATH_COMMAND_l = 604;
    static final short PATH_COMMAND_H = 605;
    static final short PATH_COMMAND_h = 606;
    static final short PATH_COMMAND_V = 607;
    static final short PATH_COMMAND_v = 608;
    static final short PATH_COMMAND_C = 609;
    static final short PATH_COMMAND_c = 610;
    static final short PATH_COMMAND_S = 611;
    static final short PATH_COMMAND_s = 612;
    static final short PATH_COMMAND_Q = 613;
    static final short PATH_COMMAND_q = 614;
    static final short PATH_COMMAND_T = 615;
    static final short PATH_COMMAND_t = 616;

    //--------------------------------------------------
    // STATIC CONSTANTS - Events
    //--------------------------------------------------
    // Indicates that the animation has started
    static final short EVENT_BEGIN = 650;
    // Indicates that the animation has finished
    static final short EVENT_END = 651;
    // Indicates that the animation has repeated
    static final short EVENT_REPEAT = 652;
    static final short EVENT_BEGIN_EL = 653;
    static final short EVENT_END_EL = 654;

    //--------------------------------------------------
    // VARIABLES
    //--------------------------------------------------
    private static M2GSVGConstants sInstance = null;

    String iEmptySvgDocument = null;
    String iSvgBaseProfileName = null;
    String iSvgBaseProfileVersion = null;
    Hashtable iAllElements = null;
    Hashtable iAllAttributes = null;
    Vector iQualifiedElements = null;
    Vector iAnimatableElements = null;
    Vector iReadOnlyElements = null;

    //--------------------------------------------------
    // METHODS
    //--------------------------------------------------
    /**
     * Constructor
     */
    private M2GSVGConstants()
    {
        // Profile name
        iSvgBaseProfileName = System.getProperty("microedition.m2g.svg.baseProfile");
        // Profile version
        iSvgBaseProfileVersion = System.getProperty("microedition.m2g.svg.version");

        // Empty svg document
        // "<svg version=\"1.1\" baseProfile=\"tiny\" width=\"100\" height=\"100\"></svg>";
        StringBuffer buf = new StringBuffer();
        buf.append("<svg version=\"").append(iSvgBaseProfileVersion);
        buf.append("\" baseProfile=\"").append(iSvgBaseProfileName);
        buf.append("\" width=\"100\" height=\"100\"></svg>");
        iEmptySvgDocument = buf.toString();

        // Elements
        iAllElements = new Hashtable(32);
        iAllElements.put(new Short(EL_A), "a"); // 1
        iAllElements.put(new Short(EL_ANIMATE), "animate"); // 2
        iAllElements.put(new Short(EL_ANIMATECOLOR), "animateColor"); // 3
        iAllElements.put(new Short(EL_ANIMATEMOTION), "animateMotion"); // 4
        iAllElements.put(new Short(EL_ANIMATETRANSFORM), "animateTransform"); // 5
        iAllElements.put(new Short(EL_CIRCLE), "circle"); // 6
        iAllElements.put(new Short(EL_DEFS), "defs"); // 7
        iAllElements.put(new Short(EL_DESC), "desc"); // 8
        iAllElements.put(new Short(EL_ELLIPSE), "ellipse"); // 9
        iAllElements.put(new Short(EL_FONT), "font"); // 10
        iAllElements.put(new Short(EL_FONTFACE), "font-face"); // 11
        iAllElements.put(new Short(EL_FONTFACENAME), "font-face-name"); // 12
        iAllElements.put(new Short(EL_FONTFACESRC), "font-face-src"); // 13
        iAllElements.put(new Short(EL_FOREIGNOBJECT), "foreignObject"); // 14
        iAllElements.put(new Short(EL_G), "g"); // 15
        iAllElements.put(new Short(EL_GLYPH), "glyph"); // 16
        iAllElements.put(new Short(EL_HKERN), "hkern"); // 17
        iAllElements.put(new Short(EL_IMAGE), "image"); // 18
        iAllElements.put(new Short(EL_LINE), "line"); // 19
        iAllElements.put(new Short(EL_METADATA), "metadata"); // 20
        iAllElements.put(new Short(EL_MISSINGGLYPH), "missing-glyph"); // 21
        iAllElements.put(new Short(EL_MPATH), "mpath"); // 22
        iAllElements.put(new Short(EL_PATH), "path"); // 23
        iAllElements.put(new Short(EL_POLYGON), "polygon"); // 24
        iAllElements.put(new Short(EL_POLYLINE), "polyline"); // 25
        iAllElements.put(new Short(EL_RECT), "rect"); // 26
        iAllElements.put(new Short(EL_SET), "set"); // 27
        iAllElements.put(new Short(EL_SVG), "svg"); // 28
        iAllElements.put(new Short(EL_SWITCH), "switch"); // 29
        iAllElements.put(new Short(EL_TEXT), "text"); // 30
        iAllElements.put(new Short(EL_TITLE), "title"); // 31
        iAllElements.put(new Short(EL_USE), "use"); // 32

        // Attributes
        iAllAttributes = new Hashtable(118);
        iAllAttributes.put(new Short(AT_STRING), "#text"); // 1
        iAllAttributes.put(new Short(AT_ACCENTHEIGHT), "accent-heigh"); // 2
        iAllAttributes.put(new Short(AT_ACCUMULATE), "accumulate"); // 3
        iAllAttributes.put(new Short(AT_ADDITIVE), "additive"); // 4
        iAllAttributes.put(new Short(AT_ALPHABETIC), "alphabetic"); // 5
        iAllAttributes.put(new Short(AT_ARABICFORM), "arabic-form"); // 6
        iAllAttributes.put(new Short(AT_ASCENT), "ascent"); // 7
        iAllAttributes.put(new Short(AT_ATTRIBUTENAME), "attributeName"); // 8
        iAllAttributes.put(new Short(AT_ATTRIBUTETYPE), "attributeType"); // 9
        iAllAttributes.put(new Short(AT_BASEPROFILE), "baseProfile"); // 10
        iAllAttributes.put(new Short(AT_BBOX), "bbox"); // 11
        iAllAttributes.put(new Short(AT_BEGIN), "begin"); // 12
        iAllAttributes.put(new Short(AT_BY), "by"); // 13
        iAllAttributes.put(new Short(AT_CALCMODE), "calcMode"); // 14
        iAllAttributes.put(new Short(AT_CAPHEIGHT), "cap-height"); // 15
        iAllAttributes.put(new Short(AT_COLOR), "color"); // 16
        iAllAttributes.put(new Short(AT_COLORRENDERING), "color-rendering"); // 17
        iAllAttributes.put(new Short(AT_CX), "cx"); // 18
        iAllAttributes.put(new Short(AT_CY), "cy"); // 19
        iAllAttributes.put(new Short(AT_D), "d"); // 20
        iAllAttributes.put(new Short(AT_DESCENT), "descent"); // 21
        iAllAttributes.put(new Short(AT_DISPLAY), "display"); // 22
        iAllAttributes.put(new Short(AT_DUR), "dur"); // 23
        iAllAttributes.put(new Short(AT_END), "end"); // 24
        iAllAttributes.put(new Short(AT_FILL), "fill"); // 25
        iAllAttributes.put(new Short(AT_FILLRULE), "fill-rule"); // 26
        iAllAttributes.put(new Short(AT_FONTFAMILY), "font-family"); // 27
        iAllAttributes.put(new Short(AT_FONTSIZE), "font-size"); // 28
        iAllAttributes.put(new Short(AT_FONTSTRETCH), "font-stretch"); // 29
        iAllAttributes.put(new Short(AT_FONTSTYLE), "font-style"); // 30
        iAllAttributes.put(new Short(AT_FONTVARIANT), "font-variant"); // 31
        iAllAttributes.put(new Short(AT_FONTWEIGHT), "font-weight"); // 32
        iAllAttributes.put(new Short(AT_FROM), "from"); // 33
        iAllAttributes.put(new Short(AT_G1), "g1"); // 34
        iAllAttributes.put(new Short(AT_G2), "g2"); // 35
        iAllAttributes.put(new Short(AT_GLYPHNAME), "glyph-name"); // 36
        iAllAttributes.put(new Short(AT_HANGING), "hanging"); // 37
        iAllAttributes.put(new Short(AT_HEIGHT), "height"); // 38
        iAllAttributes.put(new Short(AT_HORIZADVX), "horiz-adv-x"); // 39
        iAllAttributes.put(new Short(AT_HORIZORIGINX), "horiz-origin-x"); // 40
        iAllAttributes.put(new Short(AT_ID), "id"); // 41
        iAllAttributes.put(new Short(AT_IDEOGRAPHIC), "ideographic"); // 42
        iAllAttributes.put(new Short(AT_K), "k"); // 43
        iAllAttributes.put(new Short(AT_KEYPOINTS), "keyPoints"); // 44
        iAllAttributes.put(new Short(AT_KEYSPLINES), "keySplines"); // 45
        iAllAttributes.put(new Short(AT_KEYTIMES), "keyTimes"); // 46
        iAllAttributes.put(new Short(AT_LANG), "lang"); // 47
        iAllAttributes.put(new Short(AT_MATHEMATICAL), "mathematical"); // 48
        iAllAttributes.put(new Short(AT_MAX), "max"); // 49
        iAllAttributes.put(new Short(AT_MIN), "min"); // 50
        iAllAttributes.put(new Short(AT_NAME), "name"); // 51
        iAllAttributes.put(new Short(AT_ORIGIN), "origin"); // 52
        iAllAttributes.put(new Short(AT_OVERLINEPOSITION), "overline-position"); // 53
        iAllAttributes.put(new Short(AT_OVERLINETHICKNESS), "overline-thickness"); // 54
        iAllAttributes.put(new Short(AT_PANOSE1), "panose-1"); // 55
        iAllAttributes.put(new Short(AT_PATH), "path"); // 56
        iAllAttributes.put(new Short(AT_PATHLENGTH), "pathLength"); // 57
        iAllAttributes.put(new Short(AT_POINTS), "points"); // 58
        iAllAttributes.put(new Short(AT_PRESERVEASPECTRATIO), "preserveAspectRatio"); // 59
        iAllAttributes.put(new Short(AT_R), "r"); // 60
        iAllAttributes.put(new Short(AT_REPEATCOUNT), "repeatCount"); // 61
        iAllAttributes.put(new Short(AT_REPEATDUR), "repeatDur"); // 62
        iAllAttributes.put(new Short(AT_REQUIREDEXTENSIONS), "requiredExtensions"); // 63
        iAllAttributes.put(new Short(AT_REQUIREDFEATURES), "requiredFeatures"); // 64
        iAllAttributes.put(new Short(AT_RESTART), "restart"); // 65
        iAllAttributes.put(new Short(AT_ROTATE), "rotate"); // 66
        iAllAttributes.put(new Short(AT_RX), "rx"); // 67
        iAllAttributes.put(new Short(AT_RY), "ry"); // 68
        iAllAttributes.put(new Short(AT_SLOPE), "slope"); // 69
        iAllAttributes.put(new Short(AT_STEMH), "stemh"); // 70
        iAllAttributes.put(new Short(AT_STEMV), "stemv"); // 71
        iAllAttributes.put(new Short(AT_STRIKETHROUGHPOSITION), "strikethrough-position"); // 72
        iAllAttributes.put(new Short(AT_STRIKETHROUGHTHICKNESS), "strikethrough-thickness"); // 73
        iAllAttributes.put(new Short(AT_STROKE), "stroke"); // 74
        iAllAttributes.put(new Short(AT_STROKEDASHARRAY), "stroke-dasharray"); // 75
        iAllAttributes.put(new Short(AT_STROKEDASHOFFSET), "stroke-dashoffset"); // 76
        iAllAttributes.put(new Short(AT_STROKELINECAP), "stroke-linecap"); // 77
        iAllAttributes.put(new Short(AT_STROKELINEJOIN), "stroke-linejoin"); // 78
        iAllAttributes.put(new Short(AT_STROKEMITERLIMIT), "stroke-miterlimit"); // 79
        iAllAttributes.put(new Short(AT_STROKEWIDTH), "stroke-width"); // 80
        iAllAttributes.put(new Short(AT_STYLE), "style"); // 81
        iAllAttributes.put(new Short(AT_SYSTEMLANGUAGE), "systemLanguage"); // 82
        iAllAttributes.put(new Short(AT_TARGET), "target"); // 83
        iAllAttributes.put(new Short(AT_TEXTANCHOR), "text-anchor"); // 84
        iAllAttributes.put(new Short(AT_TO), "to"); // 85
        iAllAttributes.put(new Short(AT_TRANSFORM), "transform"); // 86
        iAllAttributes.put(new Short(AT_TYPE), "type"); // 87
        iAllAttributes.put(new Short(AT_U1), "u1"); // 88
        iAllAttributes.put(new Short(AT_U2), "u2"); // 89
        iAllAttributes.put(new Short(AT_UNDERLINEPOSITION), "underline-position"); // 90
        iAllAttributes.put(new Short(AT_UNDERLINETHICKNESS), "underline-thickness"); // 91
        iAllAttributes.put(new Short(AT_UNICODE), "unicode"); // 92
        iAllAttributes.put(new Short(AT_UNICODERANGE), "unicode-range"); // 93
        iAllAttributes.put(new Short(AT_UNITSPEREM), "units-per-em"); // 94
        iAllAttributes.put(new Short(AT_VALUES), "values"); // 95
        iAllAttributes.put(new Short(AT_VERSION), "version"); // 96
        iAllAttributes.put(new Short(AT_VIEWBOX), "viewBox"); // 97
        iAllAttributes.put(new Short(AT_VISIBILITY), "visibility"); // 98
        iAllAttributes.put(new Short(AT_WIDTH), "width"); // 99
        iAllAttributes.put(new Short(AT_WIDTHS), "widths"); // 100
        iAllAttributes.put(new Short(AT_X), "x"); // 101
        iAllAttributes.put(new Short(AT_XHEIGHT), "xheight"); // 102
        iAllAttributes.put(new Short(AT_X1), "x1"); // 103
        iAllAttributes.put(new Short(AT_X2), "x2"); // 104
        iAllAttributes.put(new Short(AT_XLINKACTUATE), "xlink:actuate"); // 105
        iAllAttributes.put(new Short(AT_XLINKARCROLE), "xlink:arcrole"); // 106
        iAllAttributes.put(new Short(AT_XMLBASE), "xlink:base"); // 107
        iAllAttributes.put(new Short(AT_XLINKHREF), "xlink:href"); // 108
        iAllAttributes.put(new Short(AT_XLINKROLE), "xlink:role"); // 109
        iAllAttributes.put(new Short(AT_XLINKSHOW), "xlink:show"); // 110
        iAllAttributes.put(new Short(AT_XLINKTITLE), "xlink:title"); // 111
        iAllAttributes.put(new Short(AT_XLINKTYPE), "xlink:type"); // 112
        iAllAttributes.put(new Short(AT_XMLLANG), "xml:lang"); // 113
        iAllAttributes.put(new Short(AT_XMLSPACE), "xml:space"); // 114
        iAllAttributes.put(new Short(AT_Y), "y"); // 115
        iAllAttributes.put(new Short(AT_Y1), "y1"); // 116
        iAllAttributes.put(new Short(AT_Y2), "y2"); // 117
        iAllAttributes.put(new Short(AT_ZOOMANDPAN), "zoomAndPan"); // 118

        // Qualified elements
        iQualifiedElements = new Vector(10);
        iQualifiedElements.addElement("a"); // 1
        iQualifiedElements.addElement("circle"); // 2
        iQualifiedElements.addElement("ellipse"); // 3
        iQualifiedElements.addElement("g"); // 4
        iQualifiedElements.addElement("image"); // 5
        iQualifiedElements.addElement("line"); // 6
        iQualifiedElements.addElement("path"); // 7
        iQualifiedElements.addElement("rect"); // 8
        iQualifiedElements.addElement("text"); // 9
        iQualifiedElements.addElement("use"); // 10

        // Animatable elements
        iAnimatableElements = new Vector(6);
        iAnimatableElements.addElement("animate"); // 1
        iAnimatableElements.addElement("animateColor"); // 2
        iAnimatableElements.addElement("animateMotion"); // 3
        iAnimatableElements.addElement("animateTransform"); // 4
        iAnimatableElements.addElement("mpath"); // 5
        iAnimatableElements.addElement("set"); // 6

        // Read only elements
        iReadOnlyElements = new Vector(19);
        iReadOnlyElements.addElement("animateColor"); // 1
        iReadOnlyElements.addElement("animateMotion"); // 2
        iReadOnlyElements.addElement("animateTransform"); // 3
        iReadOnlyElements.addElement("defs"); // 4
        iReadOnlyElements.addElement("desc"); // 5
        iReadOnlyElements.addElement("font"); // 6
        iReadOnlyElements.addElement("font-face"); // 7
        iReadOnlyElements.addElement("font-face-name"); // 8
        iReadOnlyElements.addElement("font-face-src"); // 9
        iReadOnlyElements.addElement("foreignObject"); // 10
        iReadOnlyElements.addElement("glyph"); // 11
        iReadOnlyElements.addElement("hkern"); // 12
        iReadOnlyElements.addElement("metadata"); // 13
        iReadOnlyElements.addElement("missing-glyph"); // 14
        iReadOnlyElements.addElement("mpath"); // 15
        iReadOnlyElements.addElement("polygon"); // 16
        iReadOnlyElements.addElement("polyline"); // 17
        iReadOnlyElements.addElement("set"); // 18
        iReadOnlyElements.addElement("switch"); // 19
    }

    //--------------------------------------------------
    // STATIC METHODS
    //--------------------------------------------------
    /**
     * Determines if the specified parentElement can have the specified childElement.
     * This method works for only the element types that can be added in JSR-226.
     */
    static final boolean checkElementAsChild(short aParentElement, short aChildElement)
    {
        switch (aParentElement)
        {
        case EL_A:
            switch (aChildElement)
            {
            case EL_A:
            case EL_CIRCLE:
            case EL_ELLIPSE:
            case EL_G:
            case EL_IMAGE:
            case EL_LINE:
            case EL_PATH:
            case EL_RECT:
            case EL_TEXT:
            case EL_USE:
                return true;
            default:
                return false;
            }

        case EL_ANIMATE:
        case EL_ANIMATECOLOR:
        case EL_ANIMATEMOTION:
        case EL_ANIMATETRANSFORM:
        case EL_CIRCLE:
            return false;

        case EL_DEFS:
            switch (aChildElement)
            {
            case EL_A:
            case EL_CIRCLE:
            case EL_ELLIPSE:
            case EL_G:
            case EL_IMAGE:
            case EL_LINE:
            case EL_PATH:
            case EL_RECT:
            case EL_TEXT:
            case EL_USE:
                return true;
            default:
                return false;
            }

        case EL_DESC:
        case EL_ELLIPSE:
        case EL_FONT:
        case EL_FONTFACE:
        case EL_FONTFACENAME:
        case EL_FONTFACESRC:
        case EL_FOREIGNOBJECT:
            return false;

        case EL_G:
            switch (aChildElement)
            {
            case EL_A:
            case EL_CIRCLE:
            case EL_ELLIPSE:
            case EL_G:
            case EL_IMAGE:
            case EL_LINE:
            case EL_PATH:
            case EL_RECT:
            case EL_TEXT:
            case EL_USE:
                return true;
            default:
                return false;
            }

        case EL_GLYPH:
        case EL_HKERN:
        case EL_IMAGE:
        case EL_LINE:
        case EL_METADATA:
        case EL_MISSINGGLYPH:
        case EL_MPATH:
        case EL_PATH:
        case EL_POLYGON:
        case EL_POLYLINE:
        case EL_RECT:
        case EL_SET:
            return false;

        case EL_SVG:
        case EL_SWITCH:
            switch (aChildElement)
            {
            case EL_A:
            case EL_CIRCLE:
            case EL_ELLIPSE:
            case EL_G:
            case EL_IMAGE:
            case EL_LINE:
            case EL_PATH:
            case EL_RECT:
            case EL_TEXT:
            case EL_USE:
                return true;
            default:
                return false;
            }

        case EL_TEXT:
            switch (aChildElement)
            {
            case EL_A:
                return true;
            default:
                return false;
            }

        case EL_TITLE:
        case EL_USE:
        default:
            return false;
        }
    }

    /**
     * Determines if the specified attribute type can be set on the specified element type.
     * @param aAttribute
     * @param aElement
     * @return true if ok
     */
    static final boolean checkElementAttribute(short aAttribute, short aElement)
    {
        switch (aElement)
        {
        case EL_A:
            switch (aAttribute)
            {
            case AT_COLOR:
            case AT_DISPLAY:
            case AT_FILL:
            case AT_FILLRULE:
            case AT_STROKE:
            case AT_STROKEDASHOFFSET:
            case AT_STROKELINECAP:
            case AT_STROKELINEJOIN:
            case AT_STROKEMITERLIMIT:
            case AT_STROKEWIDTH:
            case AT_VISIBILITY:
            case AT_FONTFAMILY:
            case AT_FONTSIZE:
            case AT_FONTSTYLE:
            case AT_FONTWEIGHT:
            case AT_TEXTANCHOR:
            case AT_TRANSFORM:
            case AT_TARGET:
            case AT_XLINKHREF:
                return true;
            default:
                return false;
            }
        case EL_CIRCLE:
            switch (aAttribute)
            {
            case AT_COLOR:
            case AT_DISPLAY:
            case AT_FILL:
            case AT_FILLRULE:
            case AT_STROKE:
            case AT_STROKEDASHOFFSET:
            case AT_STROKELINECAP:
            case AT_STROKELINEJOIN:
            case AT_STROKEMITERLIMIT:
            case AT_STROKEWIDTH:
            case AT_VISIBILITY:
            case AT_TRANSFORM:
            case AT_CX:
            case AT_CY:
            case AT_R:
                return true;
            default:
                return false;
            }
        case EL_ELLIPSE:
            switch (aAttribute)
            {
            case AT_COLOR:
            case AT_DISPLAY:
            case AT_FILL:
            case AT_FILLRULE:
            case AT_STROKE:
            case AT_STROKEDASHOFFSET:
            case AT_STROKELINECAP:
            case AT_STROKELINEJOIN:
            case AT_STROKEMITERLIMIT:
            case AT_STROKEWIDTH:
            case AT_VISIBILITY:
            case AT_TRANSFORM:
            case AT_CX:
            case AT_CY:
            case AT_RX:
            case AT_RY:
                return true;
            default:
                return false;
            }
        case EL_G:
            switch (aAttribute)
            {
            case AT_COLOR:
            case AT_DISPLAY:
            case AT_FILL:
            case AT_FILLRULE:
            case AT_STROKE:
            case AT_STROKEDASHOFFSET:
            case AT_STROKELINECAP:
            case AT_STROKELINEJOIN:
            case AT_STROKEMITERLIMIT:
            case AT_STROKEWIDTH:
            case AT_VISIBILITY:
            case AT_FONTFAMILY:
            case AT_FONTSIZE:
            case AT_FONTSTYLE:
            case AT_FONTWEIGHT:
            case AT_TEXTANCHOR:
            case AT_TRANSFORM:
                return true;
            default:
                return false;
            }
        case EL_IMAGE:
            switch (aAttribute)
            {
            case AT_COLOR:
            case AT_DISPLAY:
            case AT_FILL:
            case AT_FILLRULE:
            case AT_STROKE:
            case AT_STROKEDASHOFFSET:
            case AT_STROKELINECAP:
            case AT_STROKELINEJOIN:
            case AT_STROKEMITERLIMIT:
            case AT_STROKEWIDTH:
            case AT_VISIBILITY:
            case AT_TRANSFORM:
            case AT_X:
            case AT_Y:
            case AT_WIDTH:
            case AT_HEIGHT:
            case AT_XLINKHREF:
                return true;
            default:
                return false;
            }
        case EL_LINE:
            switch (aAttribute)
            {
            case AT_COLOR:
            case AT_DISPLAY:
            case AT_FILL:
            case AT_FILLRULE:
            case AT_STROKE:
            case AT_STROKEDASHOFFSET:
            case AT_STROKELINECAP:
            case AT_STROKELINEJOIN:
            case AT_STROKEMITERLIMIT:
            case AT_STROKEWIDTH:
            case AT_VISIBILITY:
            case AT_TRANSFORM:
            case AT_X1:
            case AT_X2:
            case AT_Y1:
            case AT_Y2:
                return true;
            default:
                return false;
            }
        case EL_PATH:
            switch (aAttribute)
            {
            case AT_COLOR:
            case AT_DISPLAY:
            case AT_FILL:
            case AT_FILLRULE:
            case AT_STROKE:
            case AT_STROKEDASHOFFSET:
            case AT_STROKELINECAP:
            case AT_STROKELINEJOIN:
            case AT_STROKEMITERLIMIT:
            case AT_STROKEWIDTH:
            case AT_VISIBILITY:
            case AT_TRANSFORM:
            case AT_D:
                return true;
            default:
                return false;
            }
        case EL_RECT:
            switch (aAttribute)
            {
            case AT_COLOR:
            case AT_DISPLAY:
            case AT_FILL:
            case AT_FILLRULE:
            case AT_STROKE:
            case AT_STROKEDASHOFFSET:
            case AT_STROKELINECAP:
            case AT_STROKELINEJOIN:
            case AT_STROKEMITERLIMIT:
            case AT_STROKEWIDTH:
            case AT_VISIBILITY:
            case AT_TRANSFORM:
            case AT_HEIGHT:
            case AT_WIDTH:
            case AT_X:
            case AT_Y:
            case AT_RX:
            case AT_RY:
                return true;
            default:
                return false;
            }
        case EL_SVG:
            switch (aAttribute)
            {
            case AT_COLOR:
            case AT_DISPLAY:
            case AT_FILL:
            case AT_FILLRULE:
            case AT_STROKE:
            case AT_STROKEDASHOFFSET:
            case AT_STROKELINECAP:
            case AT_STROKELINEJOIN:
            case AT_STROKEMITERLIMIT:
            case AT_STROKEWIDTH:
            case AT_VISIBILITY:
            case AT_FONTFAMILY:
            case AT_FONTSIZE:
            case AT_FONTSTYLE:
            case AT_FONTWEIGHT:
            case AT_TEXTANCHOR:
            case AT_VERSION:
            case AT_BASEPROFILE:
            case AT_VIEWBOX:
            case AT_ZOOMANDPAN:
                return true;
            default:
                return false;
            }
        case EL_TEXT:
            switch (aAttribute)
            {
            case AT_COLOR:
            case AT_DISPLAY:
            case AT_FILL:
            case AT_FILLRULE:
            case AT_STROKE:
            case AT_STROKEDASHOFFSET:
            case AT_STROKELINECAP:
            case AT_STROKELINEJOIN:
            case AT_STROKEMITERLIMIT:
            case AT_STROKEWIDTH:
            case AT_VISIBILITY:
            case AT_FONTFAMILY:
            case AT_FONTSIZE:
            case AT_FONTSTYLE:
            case AT_FONTWEIGHT:
            case AT_TEXTANCHOR:
            case AT_TRANSFORM:
            case AT_X:
            case AT_Y:
            case AT_STRING:
                return true;
            default:
                return false;
            }
        case EL_USE:
            switch (aAttribute)
            {
            case AT_COLOR:
            case AT_DISPLAY:
            case AT_FILL:
            case AT_FILLRULE:
            case AT_STROKE:
            case AT_STROKEDASHOFFSET:
            case AT_STROKELINECAP:
            case AT_STROKELINEJOIN:
            case AT_STROKEMITERLIMIT:
            case AT_STROKEWIDTH:
            case AT_VISIBILITY:
            case AT_FONTFAMILY:
            case AT_FONTSIZE:
            case AT_FONTSTYLE:
            case AT_FONTWEIGHT:
            case AT_TEXTANCHOR:
            case AT_TRANSFORM:
            case AT_X:
            case AT_Y:
            case AT_XLINKHREF:
                return true;
            default:
                return false;
            }
        default:
            return false;
        }
    }

    /**
     * Checks if the attribute name needs prefix
     * @param aAttributeTypeName
     * @return Attribute name
     */
    static final String checkPrefixNeed(String aAttributeTypeName)
    {
        if (aAttributeTypeName.equals("actuate")) return  "xlink:actuate";

        else if (aAttributeTypeName.equals("arcrole")) return  "xlink:arcrole";

        else if (aAttributeTypeName.equals("href")) return  "xlink:href";

        else if (aAttributeTypeName.equals("role")) return  "xlink:role";

        else if (aAttributeTypeName.equals("show")) return  "xlink:show";

        else if (aAttributeTypeName.equals("title")) return  "xlink:title";

        else if (aAttributeTypeName.equals("type")) return  "xlink:type";

        else if (aAttributeTypeName.equals("base")) return  "xml:base";

        else if (aAttributeTypeName.equals("lang")) return  "xml:lang";

        else if (aAttributeTypeName.equals("space")) return  "xml:space";

        else return aAttributeTypeName;
    }

    /**
     * Get internal data containers
     * @return Data containers
     */
    static M2GSVGConstants getInstance()
    {
        if (sInstance == null)
        {

            sInstance = new M2GSVGConstants();
        }
        return sInstance;
    }

    /**
     * Checks if element can be inserted.
     * @param aElementTypeId -
     * @return true if ok
     */
    static final boolean isAllowedToInsertElement(short aElementTypeId)
    {
        switch (aElementTypeId)
        {
        case EL_A:
        case EL_CIRCLE:
        case EL_ELLIPSE:
        case EL_G:
        case EL_IMAGE:
        case EL_LINE:
        case EL_PATH:
        case EL_RECT:
        case EL_TEXT:
        case EL_USE:
            return true;
        default:
            return false;
        }
    }

    /**
     * Checks if element can be removed.
     * @param aElementTypeId -
     * @return true if ok
     */
    static final boolean isAllowedToRemoveElement(short aElementTypeId)
    {
        switch (aElementTypeId)
        {
        case EL_A:
        case EL_CIRCLE:
        case EL_ELLIPSE:
        case EL_G:
        case EL_IMAGE:
        case EL_LINE:
        case EL_PATH:
        case EL_RECT:
        case EL_TEXT:
        case EL_USE:
            return true;
        default:
            return false;
        }
    }

    /**
       * Checks if element is animatable
       * @param aElementTypeName
       * @return true if element is animatable
       */
    static boolean isAnimatableElement(String aElementTypeName)
    {
        if (getInstance().iAnimatableElements.contains(
                    aElementTypeName))
        {
            return true;
        }
        return false;
    }

    /**
     * Checks that float trait is allowed to get
     * @param aAttributeTypeId
     * @return true if float trait is allowed to get
     */
    static final boolean isGetFloatTraitAllowed(short aAttributeTypeId)
    {
        switch (aAttributeTypeId)
        {
        case AT_STROKEDASHOFFSET:
        case AT_STROKEMITERLIMIT:
        case AT_STROKEWIDTH:
        case AT_FONTSIZE:
        case AT_HEIGHT:
        case AT_WIDTH:
        case AT_X:
        case AT_Y:
        case AT_RX:
        case AT_RY:
        case AT_CX:
        case AT_CY:
        case AT_R:
        case AT_X1:
        case AT_X2:
        case AT_Y1:
        case AT_Y2:
            return true;
        default:
            return false;
        }
    }

    /**
     * Checks that matrix trait is allowed to set
     * @param aAttributeTypeId
     * @return true if matrix trait is allowed to set
     */
    static final boolean isGetMatrixTraitAllowed(short aAttributeTypeId)
    {
        switch (aAttributeTypeId)
        {
        case AT_TRANSFORM:
            return true;
        default:
            return false;
        }
    }

    /**
     * Checks that path trait is allowed to get
     * @param aAttributeTypeId
     * @return true if path trait is allowed to get
     */
    static final boolean isGetPathTraitAllowed(short aAttributeTypeId)
    {
        switch (aAttributeTypeId)
        {
        case AT_D:
            return true;
        default:
            return false;
        }
    }

    /**
     * Checks that rect trait is allowed to get
     * @param aAttributeTypeId
     * @return true if rect trait is allowed to get
     */
    static final boolean isGetRectTraitAllowed(short aAttributeTypeId)
    {
        switch (aAttributeTypeId)
        {
        case AT_VIEWBOX:
            return true;
        default:
            return false;
        }
    }

    /**
     * Checks that RGB trait is allowed to get.
     * @param aAttributeTypeId
     * @return true if RGB trait is allowed to get.
     */
    static final boolean isGetRgbColorTraitAllowed(short aAttributeTypeId)
    {
        switch (aAttributeTypeId)
        {
        case AT_COLOR:
        case AT_FILL:
        case AT_STROKE:
            return true;
        default:
            return false;
        }
    }

    /**
     * Checks that trait that can be obtained as a string is allowed to get.
     * In SVG Tiny only certain traits can be obtained as a String value.
     * @param aAttributeTypeId
     * @return true if ok
     */
    static final boolean isGetTraitNSStringAllowed(short aAttributeTypeId)
    {
        switch (aAttributeTypeId)
        {
        case AT_DISPLAY:
        case AT_FILLRULE:
        case AT_STROKELINECAP:
        case AT_STROKELINEJOIN:
        case AT_VISIBILITY:
        case AT_FONTFAMILY:
        case AT_FONTSTYLE:
        case AT_FONTWEIGHT:
        case AT_TEXTANCHOR:
        case AT_TARGET:
        case AT_STRING: // not an SVG aTrait, but represents the contents of <text>
        case AT_VERSION:
        case AT_BASEPROFILE:
        case AT_XLINKHREF:
        case AT_ZOOMANDPAN:
            return true;
        default:
            return false;
        }
    }


    /**
     * Check trait
     * @param aAttributeTypeId
     * @return
     */
    static final boolean isGetTraitNSAllowed(short aAttributeTypeId)
    {
        switch (aAttributeTypeId)
        {
        case AT_XLINKHREF:
            return true;
        default:
            return false;
        }
    }

    /**
     * Checks that an element is locatable
     * @param aElementTypeName
     * @return true if the element is locatable
     */
    static boolean isLocatableElement(String aElementTypeName)
    {
        if (isRootElement(aElementTypeName))
        {
            return true;
        }
        if (getInstance().iQualifiedElements.contains(
                    aElementTypeName))
        {
            return true;
        }
        return false;
    }

    /**
     * Checks that an element is qualified
     * @param aElementTypeName
     * @return true if the element is qualified
     */
    static boolean isQualifiedElement(String aElementTypeName)
    {
        if (getInstance().iQualifiedElements.contains(
                    aElementTypeName))
        {
            return true;
        }
        return false;
    }

    /**
     * Checks if element is read only
     * @param aElementTypeName
     * @return true if the element is read only
     */
    static boolean isElementReadOnly(String aElementTypeName)
    {
        if (aElementTypeName != null)
        {
            return getInstance().iReadOnlyElements.contains(
                       aElementTypeName);
        }
        return false;
    }

    /**
     * Checks that an element is a root element
     * @param aElementTypeName
     * @return true if the element is a root element
     */
    static boolean isRootElement(String aElementTypeName)
    {
        return ROOT_SVG_ELEMENT_NAME.equals(aElementTypeName);
    }

    /**
     * Checks that float trait is allowed to set
     * @param aAttributeTypeId
     * @return true if float trait is allowed to set
     */
    static final boolean isSetFloatTraitAllowed(short aAttributeTypeId)
    {
        return isGetFloatTraitAllowed(aAttributeTypeId);
    }

    /**
     * Checks that matrix trait is allowed to set
     * @param aAttributeTypeId
     * @return true if matrix trait is allowed to set
     */
    static final boolean isSetMatrixTraitAllowed(short aAttributeTypeId)
    {
        return isGetMatrixTraitAllowed(aAttributeTypeId);
    }


    /**
     * Checks that path trait is allowed to set
     * @param aAttributeTypeId
     * @return true if path trait is allowed to set
     */
    static final boolean isSetPathTraitAllowed(short aAttributeTypeId)
    {
        return isGetPathTraitAllowed(aAttributeTypeId);
    }

    /**
     * Checks that rect trait is allowed to set
     * @param aAttributeTypeId
     * @return true if rect trait is allowed to set
     */
    static final boolean isSetRectTraitAllowed(short aAttributeTypeId)
    {
        switch (aAttributeTypeId)
        {
        case AT_VIEWBOX:
            return true;
        default:
            return false;
        }
    }

    /**
     * Checks that RGB trait is allowed to set.
     * @param aAttributeTypeId
     * @return true if RGB trait is allowed to set.
     */
    static final boolean isSetRgbColorTraitAllowed(short aAttributeTypeId)
    {
        switch (aAttributeTypeId)
        {
        case AT_COLOR:
        case AT_FILL:
        case AT_STROKE:
            return true;
        default:
            return false;
        }
    }

    /**
     * Checks that trait that can be obtained as a string is allowed to set.
     * In SVG Tiny only certain traits can be obtained as a String value.
     * @param aAttributeTypeId
     * @return true if ok
     */
    static final boolean isSetTraitNSStringAllowed(short aAttributeTypeId)
    {
        switch (aAttributeTypeId)
        {
        case AT_COLOR:
        case AT_DISPLAY:
        case AT_FILL:
        case AT_FILLRULE:
        case AT_STROKE:
        case AT_STROKEDASHOFFSET:
        case AT_STROKELINECAP:
        case AT_STROKELINEJOIN:
        case AT_STROKEMITERLIMIT:
        case AT_STROKEWIDTH:
        case AT_VISIBILITY:
        case AT_FONTFAMILY:
        case AT_FONTSIZE:
        case AT_FONTSTYLE:
        case AT_FONTWEIGHT:
        case AT_TEXTANCHOR:
        case AT_TARGET:
        case AT_STRING: // not an SVG trait, but represents the contents of <text>
        case AT_XLINKHREF:
        case AT_ZOOMANDPAN:
            return true;
        default:
            return false;
        }
    }

    /**
     * Check trait
     * @param aAttributeTypeId
     * @return
     */
    static final boolean isSetTraitNSAllowed(short aAttributeTypeId)
    {
        return isGetTraitNSAllowed(aAttributeTypeId);
    }

    /**
     * Converts a string trait name to the appropriate short constant.
       * This method covers all of SVGT 1.1 and not just JSR-226.
       * @param aAttributeTypeName
       * @return Attribute's type id
     */
    static final short parseAttributeTypeId(String aAttributeTypeName)
    {
        String name = checkPrefixNeed(aAttributeTypeName);
        Enumeration keys = getInstance().iAllAttributes.keys();
        while (keys.hasMoreElements())
        {
            Object id = keys.nextElement();
            try
            {
                if (((String)getInstance().iAllAttributes.get(
                            id)).equals(name))
                {
                    return ((Short)id).shortValue();
                }
            }
            catch (Exception e)
            {
            }

        }
        return SVG_ERROR;
    }

    /**
     * Converts a short constant to the string representation of a SVGT 1.1 trait name.
       * This method covers all of SVGT 1.1 and not just JSR-226.
       * @param aAttributeTypeId
       * @return Attribute's type name
     */
    static final String parseAttributeTypeName(short aAttributeTypeId)
    {

        Object attributeName = getInstance().iAllAttributes.get(
                                   new Short(aAttributeTypeId));
        if (attributeName != null)
        {
            return (String)attributeName;
        }
        return "";
    }

    /**
     * Converts a string element name to the appropriate short constant.
       * This method covers all of SVGT 1.1 and not just JSR-226.
       * @param aElementTypeName
       * @return Element's type id
     */
    static final short parseElementTypeId(String aElementTypeName)
    {
        Enumeration keys = getInstance().iAllElements.keys();
        while (keys.hasMoreElements())
        {
            Object id = keys.nextElement();
            try
            {
                if (((String)getInstance().iAllElements.get(
                            id)).equals(aElementTypeName))
                {
                    return ((Short)id).shortValue();
                }
            }
            catch (Exception e)
            {
            }
        }
        return SVG_ERROR;
    }

    /**
     * Converts a short constant to the string representation of a SVGT 1.1 element name.
       * This method covers all of SVGT 1.1 and not just JSR-226.
       * @param aElementTypeId
       * @return Element's type name
     */
    static final String parseElementTypeName(short aElementTypeId)
    {
        Object elementName = getInstance().iAllElements.get(
                                 new Short(aElementTypeId));
        if (elementName != null)
        {
            return (String)elementName;
        }
        return "";
    }

    /**
     * Converts a string trait value to the appropriate short constant.
     * NOTE: This method only handles traits that can be set as a string in JSR-226.
     * @param aAttributeTypeId
     * @param aValue
     * @return Value's id
     */
    static final short parseAttributeValueNameId(short aAttributeTypeId, String aValue)
    {
        switch (aAttributeTypeId)
        {
        case AT_STRING: // element content (i.e. <text>, etc.)
            return TEXT_NOT_SHORT;

        case AT_COLOR: // inherit
            if (aValue.equals("inherit")) return PAINT_INHERIT;
            else return SVG_ERROR;

        case AT_DISPLAY: // inline | none | inherit
            if (aValue.equals("none")) return DISPLAY_NONE;
            else if (aValue.equals("inline")) return DISPLAY_OTHER;
            else if (aValue.equals("inherit")) return DEFAULT_INHERIT;
            else return SVG_ERROR;

        case AT_FILL: // none | currentColor | inherit
            if (aValue.equals("none")) return  PAINT_NONE;
            else if (aValue.equals("currentColor")) return PAINT_CURRENT;
            else if (aValue.equals("inherit")) return PAINT_INHERIT;
            else return SVG_ERROR;

        case AT_FILLRULE: // nonzero | evenodd | inherit
            if (aValue.equals("evenodd")) return  FILL_RULE_EVENODD;
            else if (aValue.equals("nonzero")) return FILL_RULE_NONZERO;
            else if (aValue.equals("inherit")) return DEFAULT_INHERIT;
            else return SVG_ERROR;

        case AT_FONTFAMILY: // same syntax as font-family attribute
            if (aValue.equals("inherit")) return DEFAULT_INHERIT;
            else return TEXT_NOT_SHORT;

        case AT_FONTSIZE: // inherit
            if (aValue.equals("inherit")) return DEFAULT_INHERIT;
            else return SVG_ERROR;

        case AT_FONTSTYLE: // normal | italic | oblique | inherit
            if (aValue.equals("normal")) return  FONT_NORMAL;
            else if (aValue.equals("italic")) return  FONT_STYLE_ITALIC;
            else if (aValue.equals("oblique")) return  FONT_STYLE_OBLIQUE;
            else if (aValue.equals("inherit")) return DEFAULT_INHERIT;
            else return SVG_ERROR;

        case AT_FONTWEIGHT: // normal | bold | bolder | lighter | 100 | 200 | 300 | 400 | 500 | 600 | 700 | 800 | 900 | inherit
            if (aValue.equals("normal")) return FONT_NORMAL;
            else if (aValue.equals("bold")) return  FONT_WEIGHT_BOLD;
            else if (aValue.equals("bolder")) return  FONT_WEIGHT_BOLDER;
            else if (aValue.equals("lighter")) return  FONT_WEIGHT_LIGHTER;
            else if (aValue.equals("100")) return  FONT_WEIGHT_100;
            else if (aValue.equals("200")) return  FONT_WEIGHT_200;
            else if (aValue.equals("300")) return  FONT_WEIGHT_300;
            else if (aValue.equals("400")) return  FONT_WEIGHT_400;
            else if (aValue.equals("500")) return  FONT_WEIGHT_500;
            else if (aValue.equals("600")) return  FONT_WEIGHT_600;
            else if (aValue.equals("700")) return  FONT_WEIGHT_700;
            else if (aValue.equals("800")) return  FONT_WEIGHT_800;
            else if (aValue.equals("900")) return  FONT_WEIGHT_900;
            else if (aValue.equals("inherit")) return DEFAULT_INHERIT;
            else return SVG_ERROR;

        case AT_STROKE: // none | currentColor | inherit
            if (aValue.equals("none")) return  PAINT_NONE;
            else if (aValue.equals("currentColor")) return  PAINT_CURRENT;
            else if (aValue.equals("inherit")) return  PAINT_INHERIT;
            else return SVG_ERROR;

        case AT_STROKEDASHOFFSET: // inherit
            if (aValue.equals("inherit")) return DEFAULT_INHERIT;
            else return SVG_ERROR;

        case AT_STROKELINECAP: // butt | round | square | inherit
            if (aValue.equals("butt")) return  STROKE_LINECAP_BUTT;
            else if (aValue.equals("round")) return  STROKE_LINECAP_ROUND;
            else if (aValue.equals("square")) return  STROKE_LINECAP_SQUARE;
            else if (aValue.equals("inherit")) return  DEFAULT_INHERIT;
            else return SVG_ERROR;

        case AT_STROKELINEJOIN: // miter | round | bevel | inherit
            if (aValue.equals("miter")) return  STROKE_LINEJOIN_MITER;
            else if (aValue.equals("round")) return  STROKE_LINEJOIN_ROUND;
            else if (aValue.equals("bevel")) return  STROKE_LINEJOIN_BEVEL;
            else if (aValue.equals("inherit")) return  DEFAULT_INHERIT;
            else return SVG_ERROR;

        case AT_STROKEMITERLIMIT: // inherit
            if (aValue.equals("inherit")) return DEFAULT_INHERIT;
            else return SVG_ERROR;

        case AT_STROKEWIDTH: // inherit
            if (aValue.equals("inherit")) return DEFAULT_INHERIT;
            else return SVG_ERROR;

        case AT_TARGET: // string
            return TEXT_NOT_SHORT;

        case AT_TEXTANCHOR: // start | middle | end | inherit
            if (aValue.equals("start")) return  TEXT_ANCHOR_START;
            else if (aValue.equals("middle")) return  TEXT_ANCHOR_MIDDLE;
            else if (aValue.equals("end")) return  TEXT_ANCHOR_END;
            else if (aValue.equals("inherit")) return  DEFAULT_INHERIT;
            else return SVG_ERROR;

        case AT_VISIBILITY: // visible | hidden | inherit
            if (aValue.equals("visible")) return  VISIBILITY_VISIBLE;
            else if (aValue.equals("hidden")) return  VISIBILITY_OTHER;
            else if (aValue.equals("inherit")) return DEFAULT_INHERIT;
            else return SVG_ERROR;

        case AT_XLINKHREF: // URI aValue
            return TEXT_NOT_SHORT;

        case AT_ZOOMANDPAN: // disable | magnify
            if (aValue.equals("magnify")) return  ZPN_MAGNIFY;
            else if (aValue.equals("disable")) return  ZPN_DISABLE;
            else return SVG_ERROR;

        default:
            return SVG_ERROR;
        }
    }

    /**
     * Parse attribute
       * @param aValueId
     * @return Value's name
     */
    static final String parseAttributeValueName(short aAttributeTypeId, short aValueId)
    {
        if (aValueId == 0)
        {
            return "";
        }
        if (aAttributeTypeId == AT_FONTWEIGHT && aValueId == FONT_WEIGHT_LIGHTER)
        {
            return "300";
        }
        else if (aAttributeTypeId == AT_FONTWEIGHT && aValueId == FONT_NORMAL)
        {
            return "400";
        }
        else if (aAttributeTypeId == AT_FONTWEIGHT && aValueId == FONT_WEIGHT_BOLD)
        {
            return "700";
        }
        else if (aAttributeTypeId == AT_FONTWEIGHT && aValueId == FONT_WEIGHT_BOLDER)
        {
            return "800";
        }
        else
        {
            return parseAttributeValueName(aValueId);
        }
    }

    /**
     * Converts a short constant to the string representation of a
     * SVGT 1.1 attribute value literal.
       * This method covers all of SVGT 1.1 and not just JSR-226.
       * @param aValueId
     * @return Value's name
     */
    static final String parseAttributeValueName(short aValueId)
    {

        switch (aValueId)
        {
        case TEXT_NOT_SHORT:
            return "#text";

        case DEFAULT_INHERIT:
        case PAINT_INHERIT:
            return "inherit";

        case DISPLAY_NONE:
        case PAINT_NONE:
            return "none";

        case DISPLAY_OTHER:
            return "inline";

        case PAINT_CURRENT:
            return "currentColor";

        case FILL_RULE_NONZERO:
            return "nonzero";

        case FILL_RULE_EVENODD:
            return "evenodd";

        case FONT_NORMAL:
            return "normal";

        case FONT_STYLE_ITALIC:
            return "italic";

        case FONT_STYLE_OBLIQUE:
            return "oblique";

        case FONT_WEIGHT_BOLD:
            return "bold";

        case FONT_WEIGHT_BOLDER:
            return "bolder";

        case FONT_WEIGHT_LIGHTER:
            return "lighter";

        case FONT_WEIGHT_100:
            return "100";

        case FONT_WEIGHT_200:
            return "200";

        case FONT_WEIGHT_300:
            return "300";

        case FONT_WEIGHT_400:
            return "400";

        case FONT_WEIGHT_500:
            return "500";

        case FONT_WEIGHT_600:
            return "600";

        case FONT_WEIGHT_700:
            return "700";

        case FONT_WEIGHT_800:
            return "800";

        case FONT_WEIGHT_900:
            return "900";

        case STROKE_LINECAP_BUTT:
            return "butt";

        case STROKE_LINECAP_ROUND:
        case STROKE_LINEJOIN_ROUND:
            return "round";

        case STROKE_LINECAP_SQUARE:
            return "square";

        case STROKE_LINEJOIN_MITER:
            return "miter";

        case STROKE_LINEJOIN_BEVEL:
            return "bevel";

        case TEXT_ANCHOR_START:
            return "start";

        case TEXT_ANCHOR_MIDDLE:
            return "middle";

        case TEXT_ANCHOR_END:
            return "end";

        case VISIBILITY_VISIBLE:
            return "visible";

        case VISIBILITY_OTHER:
            return "hidden";

        case ZPN_MAGNIFY:
            return "magnify";

        case ZPN_DISABLE:
            return "disable";

        default:
            return "";
        }
    }

    /*
     * Parses path command
     * @param aPathCommandIndex
     * @return path command
     */
    static short parsePathCommand(short aPathCommandIndex)
    {
        short command = SVG_ERROR;
        switch (aPathCommandIndex)
        {
        case PATH_COMMAND_m:
        case PATH_COMMAND_M:
            command = SVGPath.MOVE_TO;
            break;
        case PATH_COMMAND_Z:
            command = SVGPath.CLOSE;
            break;
        case PATH_COMMAND_h:
        case PATH_COMMAND_v:
        case PATH_COMMAND_l:
        case PATH_COMMAND_H:
        case PATH_COMMAND_V:
        case PATH_COMMAND_L:
            command = SVGPath.LINE_TO;
            break;
        case PATH_COMMAND_s:
        case PATH_COMMAND_c:
        case PATH_COMMAND_S:
        case PATH_COMMAND_C:
            command = SVGPath.CURVE_TO;
            break;
        case PATH_COMMAND_t:
        case PATH_COMMAND_q:
        case PATH_COMMAND_T:
        case PATH_COMMAND_Q:
            command = SVGPath.QUAD_TO;
            break;
        default:
            command = SVG_ERROR;
            break;
        }
        return command;
    }
}

