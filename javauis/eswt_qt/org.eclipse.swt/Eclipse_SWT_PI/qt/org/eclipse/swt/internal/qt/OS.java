/*******************************************************************************
 * Copyright (c) 2009, 2010 Nokia Corporation and/or its subsidiary(-ies).
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *
 * Contributors:
 *     Nokia Corporation - initial implementation
 *******************************************************************************/
package org.eclipse.swt.internal.qt;


import org.eclipse.swt.SWT;
import org.eclipse.swt.graphics.Point;
import org.eclipse.swt.graphics.Rectangle;
import org.eclipse.swt.internal.Library;

public final class OS {

    static {
        Library.loadLibrary("eswtqt");
    }

    public static final int WS_SYMBIAN_S60 = 1;
    public static final int WS_X11 = 2;

    // Underlying window server. WS_X11 if X11, WS_SYMBIAN_S60 otherwise.
    public static final int windowServer = windowServer();

    // QEventLoop ProcessEventsFlags
    public static final int QEVENTLOOP_ALLEVENTS = 0;
    public static final int QEVENTLOOP_EXCLUDEUSERINPUTEVENTS = 0x01;
    public static final int QEVENTLOOP_WAITFORMOREEVENTS = 0x04;
    public static final int QEVENTLOOP_X11EXCLUDETIMERS = 0x08;

    // QT Key Codes
    public static final int QT_KEY_ALT =0x01000023;
    public static final int QT_KEY_ALTGR = 0x01001103;
    public static final int QT_KEY_META = 0x01000022;
    public static final int QT_KEY_SHIFT = 0x01000020;
    public static final int QT_KEY_CONTROL = 0x01000021;
    public static final int QT_KEY_LEFT = 0x01000012;
    public static final int QT_KEY_UP = 0x01000013;
    public static final int QT_KEY_RIGHT = 0x01000014;
    public static final int QT_KEY_DOWN = 0x01000015;
    public static final int QT_KEY_PAGEUP = 0x01000016;
    public static final int QT_KEY_PAGEDOWN = 0x01000017;
    public static final int QT_KEY_HOME = 0x01000010;
    public static final int QT_KEY_END = 0x01000011;
    public static final int QT_KEY_INSERT = 0x01000006;
    public static final int QT_KEY_BACKSPACE = 0x01000003;
    public static final int QT_KEY_RETURN = 0x01000004;
    public static final int QT_KEY_DELETE = 0x01000007;
    public static final int QT_KEY_ESCAPE = 0x01000000;
    public static final int QT_KEY_ENTER = 0x01000005;
    public static final int QT_KEY_TAB = 0x01000001;
    public static final int QT_KEY_BACKTAB = 0x01000002;
    public static final int QT_KEY_F1 = 0x01000030;
    public static final int QT_KEY_F2 = 0x01000031;
    public static final int QT_KEY_F3 = 0x01000032;
    public static final int QT_KEY_F4 = 0x01000033;
    public static final int QT_KEY_F5 = 0x01000034;
    public static final int QT_KEY_F6 = 0x01000035;
    public static final int QT_KEY_F7 = 0x01000036;
    public static final int QT_KEY_F8 = 0x01000037;
    public static final int QT_KEY_F9 = 0x01000038;
    public static final int QT_KEY_F10 = 0x01000039;
    public static final int QT_KEY_F11 = 0x0100003a;
    public static final int QT_KEY_F12 = 0x0100003b;
    public static final int QT_KEY_F13 = 0x0100003c;
    public static final int QT_KEY_F14 = 0x0100003d;
    public static final int QT_KEY_F15 = 0x0100003e;
    public static final int QT_KEY_CAPSLOCK = 0x01000024;
    public static final int QT_KEY_NUMLOCK = 0x01000025;
    public static final int QT_KEY_SCROLLLOCK = 0x01000026;
    public static final int QT_KEY_PAUSE = 0x01000008;
    public static final int QT_KEY_PRINT = 0x01000009;
    public static final int QT_KEY_HELP = 0x01000058;
    public static final int QT_KEY_ASTERISK = 0x2a;
    public static final int QT_KEY_PLUS = 0x2b;
    public static final int QT_KEY_MINUS = 0x2d;
    public static final int QT_KEY_PERIOD = 0x2e;
    public static final int QT_KEY_SLASH = 0x2f;
    public static final int QT_KEY_0 = 0x30;
    public static final int QT_KEY_1 = 0x31;
    public static final int QT_KEY_2 = 0x32;
    public static final int QT_KEY_3 = 0x33;
    public static final int QT_KEY_4 = 0x34;
    public static final int QT_KEY_5 = 0x35;
    public static final int QT_KEY_6 = 0x36;
    public static final int QT_KEY_7 = 0x37;
    public static final int QT_KEY_8 = 0x38;
    public static final int QT_KEY_9 = 0x39;
    public static final int QT_KEY_EQUAL = 0x3d;
    public static final int QT_KEY_A = 0x41;
    public static final int QT_KEY_B = 0x42;
    public static final int QT_KEY_C = 0x43;
    public static final int QT_KEY_D = 0x44;
    public static final int QT_KEY_E = 0x45;
    public static final int QT_KEY_F = 0x46;
    public static final int QT_KEY_G = 0x47;
    public static final int QT_KEY_H = 0x48;
    public static final int QT_KEY_I = 0x49;
    public static final int QT_KEY_J = 0x4a;
    public static final int QT_KEY_K = 0x4b;
    public static final int QT_KEY_L = 0x4c;
    public static final int QT_KEY_M = 0x4d;
    public static final int QT_KEY_N = 0x4e;
    public static final int QT_KEY_O = 0x4f;
    public static final int QT_KEY_P = 0x50;
    public static final int QT_KEY_Q = 0x51;
    public static final int QT_KEY_R = 0x52;
    public static final int QT_KEY_S = 0x53;
    public static final int QT_KEY_T = 0x54;
    public static final int QT_KEY_U = 0x55;
    public static final int QT_KEY_V = 0x56;
    public static final int QT_KEY_W = 0x57;
    public static final int QT_KEY_X = 0x58;
    public static final int QT_KEY_Y = 0x59;
    public static final int QT_KEY_Z = 0x5a;
    public static final int QT_KEY_UNKNOWN = 0x01ffffff;
    public static final int QT_KEY_YES = 0x01010001;
    public static final int QT_KEY_NO = 0x01010002;

    // Mask for non-printable keys
    public static final int QT_NONPRINTABLEKEYMASK = 0x01000000;

    // Max widget size
    public static final int QWIDGETSIZE_MAX = 16777215;

    // Qt KeyboardModifiers
    public static final int QT_NOMODIFIER = 0x00000000; //No modifier key is pressed.
    public static final int QT_SHIFTMODIFIER = 0x02000000; //A Shift key on the keyboard is pressed.
    public static final int QT_CONTROLMODIFIER = 0x04000000; //A Ctrl key on the keyboard is pressed.
    public static final int QT_ALTMODIFIER = 0x08000000; // An Alt key on the keyboard is pressed.
    public static final int QT_METAMODIFIER = 0x10000000; // A Meta key on the keyboard is pressed.
    public static final int QT_KEYPADMODIFIER = 0x20000000; //A keypad button is pressed.
    public static final int QT_GROUPSWITCHMODIFIER = 0x40000000;// X11 only. A Mode_switch key on the keyboard is pressed.

    // Qt Event type ids
    public static final int QEVENT_NONE = 0;
    public static final int QEVENT_MOUSEBUTTONPRESS = 2;
    public static final int QEVENT_MOUSEBUTTONRELEASE = 3;
    public static final int QEVENT_MOUSEBUTTONDBLCLICK = 4;
    public static final int QEVENT_MOUSEMOVE = 5;
    public static final int QEVENT_KEYPRESS = 6;
    public static final int QEVENT_KEYRELEASE = 7;
    public static final int QEVENT_FOCUSIN = 8; // Focus is about to be gained
    public static final int QEVENT_FOCUSOUT = 9; // Focus is about to be lost
    public static final int QEVENT_PAINT = 12;
    public static final int QEVENT_MOVE = 13;
    public static final int QEVENT_RESIZE = 14;
    public static final int QEVENT_CLOSE = 19;
    public static final int QEVENT_WINDOWACTIVATE = 24;
    public static final int QEVENT_WINDOWDEACTIVATE = 25;
    public static final int QEVENT_APPLICATIONFONTCHANGE = 36;
    public static final int QEVENT_APPLICATIONLAYOUTDIRECTIONCHANGE = 37;
    public static final int QEVENT_APPLICATIONPALETTECHANGE = 38;
    public static final int QEVENT_SHORTCUTOVERRIDE = 51;
    public static final int QEVENT_DEFERREDDELETE = 52;
    public static final int QEVENT_CONTEXTMENU = 82;
    public static final int QEVENT_WINDOWSTATECHANGE = 105;
    public static final int QEVENT_SHORTCUT = 117;
    public static final int QEVENT_REQUESTSOFTWAREINPUTPANEL = 199;

    // QInputContextFactory key
    public static final String  QINPUTCONTEXT_COEFEP = "coefep";
    
    // CntServicesContact actions
    public static final String CNT_ACTIONALL = "all";
    public static final String CNT_ACTIONCALL ="call";
    public static final String CNT_ACTIONSMS = "sms";
    public static final String CNT_ACTIONEMAIL= "email";

    // CntServicesContact filters
    public static final String  CNT_DISPLAYALL = "all";
    public static final String  CNT_FILTERDISPLAYFAVORITES = "favorites";

    // Implementation specific codes for the signals emitted by the Qt widgets
    public static final int QSIGNAL_ID_RANGE_FIRST = 1000;
    public static final int QSIGNAL_PRESSED = QSIGNAL_ID_RANGE_FIRST;
    public static final int QSIGNAL_RELEASED = QSIGNAL_ID_RANGE_FIRST + 2;
    public static final int QSIGNAL_TIMEOUT = QSIGNAL_ID_RANGE_FIRST + 3;
    public static final int QSIGNAL_ABOUTTOSHOW = QSIGNAL_ID_RANGE_FIRST + 4;
    public static final int QSIGNAL_ABOUTTOHIDE = QSIGNAL_ID_RANGE_FIRST + 5;
    public static final int QSIGNAL_TRIGGERED = QSIGNAL_ID_RANGE_FIRST + 6;
    public static final int QSIGNAL_HOVER = QSIGNAL_ID_RANGE_FIRST + 7;
    public static final int QSIGNAL_SELECTIONCHANGED = QSIGNAL_ID_RANGE_FIRST + 8;
    public static final int QSIGNAL_TEXT_CHANGED = QSIGNAL_ID_RANGE_FIRST + 9;
    public static final int QSIGNAL_SLIDER_CHANGED = QSIGNAL_ID_RANGE_FIRST + 10;
    public static final int QSIGNAL_RETURN_PRESSED = QSIGNAL_ID_RANGE_FIRST + 11;
    public static final int QSIGNAL_LIST_ITEMSELECTION_CHANGED = QSIGNAL_ID_RANGE_FIRST + 12;
    public static final int QSIGNAL_LIST_ITEM_DOUBLECLICKED = QSIGNAL_ID_RANGE_FIRST + 13;
    public static final int QSIGNAL_LIST_ITEM_ACTIVATED = QSIGNAL_ID_RANGE_FIRST + 14;
    public static final int QSIGNAL_TABLE_ITEMSELECTION_CHANGED = QSIGNAL_ID_RANGE_FIRST + 15;
    public static final int QSIGNAL_TABLE_CURRENTCELL_CHANGED = QSIGNAL_ID_RANGE_FIRST + 16;
    public static final int QSIGNAL_TABLE_CELL_CHANGED = QSIGNAL_ID_RANGE_FIRST + 17;
    public static final int QSIGNAL_TABLE_CELL_ACTIVATED = QSIGNAL_ID_RANGE_FIRST + 18;
    public static final int QSIGNAL_SLIDER_ACTION_TRIGGERED  = QSIGNAL_ID_RANGE_FIRST + 19;
    public static final int QSIGNAL_SLIDER_RELEASED = QSIGNAL_ID_RANGE_FIRST + 20;
    public static final int QSIGNAL_LINKACTIVATED = QSIGNAL_ID_RANGE_FIRST + 21;
    public static final int QSIGNAL_DATETIMEEDIT_CHANGED = QSIGNAL_ID_RANGE_FIRST + 22;
    public static final int QSIGNAL_WEBVIEW_LINKCLICKED = QSIGNAL_ID_RANGE_FIRST + 23;
    public static final int QSIGNAL_WEBVIEW_LOADSTARTED = QSIGNAL_ID_RANGE_FIRST + 24;
    public static final int QSIGNAL_WEBVIEW_LOADPROGRESS = QSIGNAL_ID_RANGE_FIRST + 25;
    public static final int QSIGNAL_WEBVIEW_LOADFINISHED = QSIGNAL_ID_RANGE_FIRST + 26;
    public static final int QSIGNAL_WEBVIEW_STATUSBARMESSAGE = QSIGNAL_ID_RANGE_FIRST + 27;
    public static final int QSIGNAL_WEBVIEW_TITLECHANGED = QSIGNAL_ID_RANGE_FIRST + 28;
    public static final int QSIGNAL_WEBVIEW_URLCHANGED = QSIGNAL_ID_RANGE_FIRST + 29;
    public static final int QSIGNAL_TRAY_ACTIVATED = QSIGNAL_ID_RANGE_FIRST + 30;
    public static final int QSIGNAL_TREE_ITEMSELECTION_CHANGED = QSIGNAL_ID_RANGE_FIRST + 31;
    public static final int QSIGNAL_TREE_ITEM_ACTIVATED = QSIGNAL_ID_RANGE_FIRST + 32;
    public static final int QSIGNAL_TREE_ITEM_EXPANDED = QSIGNAL_ID_RANGE_FIRST + 33;
    public static final int QSIGNAL_TREE_ITEM_COLLAPSED= QSIGNAL_ID_RANGE_FIRST + 34;
    public static final int QSIGNAL_CURRENT_TAB_CHANGED = QSIGNAL_ID_RANGE_FIRST + 35;
    public static final int QSIGNAL_ABSTRACTITEMVIEW_ACTIVATED = QSIGNAL_ID_RANGE_FIRST + 36;
    public static final int QSIGNAL_TEXT_CURSORPOSITIONCHANGED = QSIGNAL_ID_RANGE_FIRST + 37;
    public static final int QSIGNAL_TEXT_SELECTIONCHANGED = QSIGNAL_ID_RANGE_FIRST + 38;
    public static final int QSIGNAL_QAPPLICATION_FOCUSCHANGED = QSIGNAL_ID_RANGE_FIRST + 39;
    public static final int QSIGNAL_QAPPLICATION_FOCUSCHANGED_TO_WIDDGET = QSIGNAL_ID_RANGE_FIRST + 40;
    public static final int QSIGNAL_CLICKED = QSIGNAL_ID_RANGE_FIRST + 41;
 public static final int QSIGNAL_QDESKTOPWIDGET_SCREENCOUNTCHANGED = QSIGNAL_ID_RANGE_FIRST + 42;
    public static final int QSIGNAL_REQUESTCOMPLETED = QSIGNAL_ID_RANGE_FIRST + 43;
 
    // Implementation specific event codes, not real Qt events but only used in this implementation
    // to implement events in situations that don't have a corresponding Qt event.
    public static final int QSWTEVENT_ID_RANGE_FIRST = 2000;
    public static final int QSWTEVENT_WIDGETPAINTED = QSWTEVENT_ID_RANGE_FIRST + 1;
    public static final int QSWTEVENT_WIDGETRESIZED = QSWTEVENT_ID_RANGE_FIRST + 2;
    public static final int QSWTEVENT_WIDGETMOVED = QSWTEVENT_ID_RANGE_FIRST + 3;
    public static final int QSWTEVENT_SYSTEMSHUTDOWN = QSWTEVENT_ID_RANGE_FIRST + 4;
    public static final int QSWTEVENT_BUFFERFLUSH = QSWTEVENT_ID_RANGE_FIRST + 5;
    public static final int QSWTEVENT_RESOURCECHANGE = QSWTEVENT_ID_RANGE_FIRST + 6;
    public static final int QSWTEVENT_MOBILEDEVICEOPENED = QSWTEVENT_ID_RANGE_FIRST + 7;
    public static final int QSWTEVENT_MOBILEDEVICECLOSED = QSWTEVENT_ID_RANGE_FIRST + 8;
    public static final int QSWTEVENT_SCREENACTIVATED = QSWTEVENT_ID_RANGE_FIRST + 9;
    public static final int QSWTEVENT_SCREENDEACTIVATED = QSWTEVENT_ID_RANGE_FIRST + 10;
    public static final int QSWTEVENT_SYMBIAN_WINDOW_FULLY_VISIBLE = QSWTEVENT_ID_RANGE_FIRST + 11;
    public static final int QSWTEVENT_SYMBIAN_WINDOW_PARTIALLY_VISIBLE = QSWTEVENT_ID_RANGE_FIRST + 12;
    public static final int QSWTEVENT_SYMBIAN_WINDOW_NOT_VISIBLE = QSWTEVENT_ID_RANGE_FIRST + 13;

    // Qt connection types for signal-slot connections
    public static final int QT_AUTOCONNECTION = 0;
    public static final int QT_DIRECTCONNECTION = 1;
    public static final int QT_QUEUEDCONNECTION = 2;
    public static final int QT_BLOCKINGQUEUEDCONNECTION = 4;

    // Qt focus reasons
    public static final int QT_MOUSEFOCUSREASON = 0;
    public static final int QT_TABFOCUSREASON = 1;
    public static final int QT_BACKTABFOCUSREASON = 2;
    public static final int QT_ACTIVEWINDOWFOCUSREASON = 3;
    public static final int QT_POPUPFOCUSREASON = 4;
    public static final int QT_SHORTCUTFOCUSREASON = 5;
    public static final int QT_MENUBARFOCUSREASON = 6;
    public static final int QT_OTHERFOCUSREASON = 7;

    // Qt shape
    public static final int QT_NOFRAME = 0;
    public static final int QT_FRAMEBOX = 0x0001;
    public static final int QT_FRAMEPANEL = 0x0002;
    public static final int QT_FRAMESTYLEDPANEL = 0x0006;
    public static final int QT_FRAMEHLINE = 0x0004;
    public static final int QT_FRAMEVLINE = 0x0005;
    public static final int QT_FRAMEWINPANEL = 0x0003;

    // Qt shadow
    public static final int QT_FRAMEPLAIN = 0x0010;
    public static final int QT_FRAMERAISED = 0x0020;
    public static final int QT_FRAMESUNKEN = 0x0030;

    // Qt Alignment
    public static final int QT_ALIGNLEFT = 0x0001;
    public static final int QT_ALIGNRIGHT = 0x0002;
    public static final int QT_ALIGNHCENTER = 0x0004;
    public static final int QT_ALIGNJUSTIFY = 0x0008;
    public static final int QT_ALIGNTOP = 0x0020;
    public static final int QT_ALIGNBOTTOM = 0x0040;
    public static final int QT_ALIGNVCENTER = 0x0080;

    // Qt InputMethodHints
    public static final int QT_IMHNOAUTOUPPERCASE = 0x2;
    public static final int QT_IMHPREFERUPPERCASE = 0x8;
    public static final int QT_IMHPREFERLOWERCASE = 0x10;
    public static final int QT_IMHNOPREDICTIVETEXT = 0x20;
    public static final int QT_IMHEMAILCHARACTERSONLY = 0x200000;
    public static final int QT_IMHURLCHARACTERSONLY = 0x400000;
    public static final int QT_IMHDIGITSONLY = 0x10000;
    public static final int QT_IMHFORMATTEDNUMBERSONLY = 0x20000;
    public static final int QT_IMHDIALABLECHARACTERSONLY = 0x100000;

    // Qt Mouse button
    public static final int QT_NOBUTTON =  0x0000;
    public static final int QT_LEFTBUTTON = 0x0001;
    public static final int QT_RIGHTBUTTON = 0x0002;
    public static final int QT_MIDBUTTON = 0x0004;

    // QDateTimeEdit array indexes
    public static final int QT_DATETIME_FIELD_COUNT = 6;
    public static final int QT_DATETIME_YEAR = 0;
    public static final int QT_DATETIME_MONTH = 1;
    public static final int QT_DATETIME_DAY = 2;
    public static final int QT_DATETIME_HOUR = 3;
    public static final int QT_DATETIME_MINUTE = 4;
    public static final int QT_DATETIME_SECOND = 5;

    // Qt Palette::ColorGroup
    public static final int QPALETTE_COLORGROUP_NORMAL = 0;
    public static final int QPALETTE_COLORGROUP_DISABLED = 1;
    public static final int QPALETTE_COLORGROUP_INACTIVE = 2;

    // Qt Palette::ColorRole
    public static final int QPALETTE_COLORROLE_WINDOW = 10;
    public static final int QPALETTE_COLORROLE_WINDOWTEXT = 0;
    public static final int QPALETTE_COLORROLE_BASE = 9;
    public static final int QPALETTE_COLORROLE_TEXT = 6;
    public static final int QPALETTE_COLORROLE_BUTTON = 1;
    public static final int QPALETTE_COLORROLE_BUTTONTEXT = 8;
    public static final int QPALETTE_COLORROLE_BRIGHTTEXT = 7;
    public static final int QPALETTE_COLORROLE_HIGHLIGHT = 12;
    public static final int QPALETTE_COLORROLE_HIGHLIGHTTEXT = 13;
    public static final int QPALETTE_COLORROLE_MIDLIGHT = 3;
    public static final int QPALETTE_COLORROLE_DARK = 4;
    public static final int QPALETTE_COLORROLE_MID = 5;

    // Qt QFont::Weight
    public static final int QT_FONTNORMAL = 50;
    public static final int QT_FONTBOLD =   75;


    //QFont::Style
    public static final int QFONT_STYLE_NORMAL = 0;
    public static final int QFONT_STYLE_ITALIC = 1;
    public static final int QFONT_STYLE_OBLIQUE = 2;

    // QFont::StyleStrategy
    public static final int QFONT_STYLESTRATEGY_PREFERDEFALUT = 0x0001;
    public static final int QFONT_STYLESTRATEGY_PREFERBITMAP = 0x0002;
    public static final int QFONT_STYLESTRATEGY_PREFERDEVICE = 0x0004;
    public static final int QFONT_STYLESTRATEGY_PREFEROUTLINE   = 0x0008;
    public static final int QFONT_STYLESTRATEGY_FORCEOUTLINE = 0x0010;
    public static final int QFONT_STYLESTRATEGY_NOANTIALIAS = 0x0100;
    public static final int QFONT_STYLESTRATEGY_PREFERANTIALIAS = 0x0080;
    public static final int QFONT_STYLESTRATEGY_OPENGLCOMPATIABLE = 0x0200;
    public static final int QFONT_STYLESTRATEGY_NOFONTMERGING = 0x8000;
    public static final int QFONT_STYLESTRATEGY_PREFERMATCH = 0x0020;
    public static final int QFONT_STYLESTRATEGY_PREFERQUALITY = 0x0040;

    // Qt QAbstractItemView::SelectionBehavior
    public static final int QT_ABSTRACTITEMVIEW_SELECTIONBEHAVIOR_ITEMS = 0;
    public static final int QT_ABSTRACTITEMVIEW_SELECTIONBEHAVIOR_ROWS = 1;
    public static final int QT_ABSTRACTITEMVIEW_SELECTIONBEHAVIOR_COLUMNS = 2;

    // Qt QAbstractItemView::SelectionMode
    public static final int QT_ABSTRACTITEMVIEW_SELECTIONMODE_SINGLE = 1;
    public static final int QT_ABSTRACTITEMVIEW_SELECTIONMODE_CONTIGUOUS = 4;
    public static final int QT_ABSTRACTITEMVIEW_SELECTIONMODE_EXTENDED = 3;
    public static final int QT_ABSTRACTITEMVIEW_SELECTIONMODE_MULTI = 2;
    public static final int QT_ABSTRACTITEMVIEW_SELECTIONMODE_NONE = 0;

    // Qt QAbstractItemView::ScrollHint
    public static final int QT_ABSTRACTITEMVIEW_SCROLLHINT_ENSUREVISIBLE = 0;
    public static final int QT_ABSTRACTITEMVIEW_SCROLLHINT_POSITIONATTOP = 1;
    public static final int QT_ABSTRACTITEMVIEW_SCROLLHINT_POSITIONATBOTTOM = 2;
    public static final int QT_ABSTRACTITEMVIEW_SCROLLHINT_POSITIONATCENTER = 3;

    // Qt QListView::Flow
    public static final int QT_LISTVIEW_FLOW_LEFTTORIGHT = 0;
    public static final int QT_LISTVIEW_FLOW_TOPTOBOTTOM = 1;

    // Qt QListView::ViewMode
    public static final int QT_LISTVIEW_VIEWMODE_LIST = 0;
    public static final int QT_LISTVIEW_VIEWMODE_ICON = 1;

    // Qt QListView::Movement
    public static final int QT_LISTVIEW_MOVEMENT_STATIC = 0;
    public static final int QT_LISTVIEW_MOVEMENT_FREE = 1;
    public static final int QT_LISTVIEW_MOVEMENT_SNAP = 2;

    // Qt QListView::ResizeMode
    public static final int QT_LISTVIEW_RESIZEMODE_FIXED = 0;
    public static final int QT_LISTVIEW_RESIZEMODE_ADJUST = 1;

    // Qt QItemSelectionModel::SelectionFlags
    public static final int QT_ITEMSELECTIONMODEL_SELECTIONFLAGS_NONE = 0x0000;
    public static final int QT_ITEMSELECTIONMODEL_SELECTIONFLAGS_CLEAR = 0x0001;
    public static final int QT_ITEMSELECTIONMODEL_SELECTIONFLAGS_SELECT = 0x0002;
    public static final int QT_ITEMSELECTIONMODEL_SELECTIONFLAGS_DESELECT = 0x0004;
    public static final int QT_ITEMSELECTIONMODEL_SELECTIONFLAGS_TOGGLE = 0x0008;
    public static final int QT_ITEMSELECTIONMODEL_SELECTIONFLAGS_CURRENT = 0x0010;
    public static final int QT_ITEMSELECTIONMODEL_SELECTIONFLAGS_ROWS = 0x0020;
    public static final int QT_ITEMSELECTIONMODEL_SELECTIONFLAGS_COLUMNS = 0x0040;

    // Qt QLineEdit::EchoMode
    public static final int QLINEEDIT_ECHOMODE_NORMAL = 0;
    public static final int QLINEEDIT_ECHOMODE_NOECHO = 1;
    public static final int QLINEEDIT_ECHOMODE_PASSWORD = 2;
    public static final int QLINEEDIT_ECHOMODE_PASSWORDECHOONEDIT = 3;

    // Qt::QScrollBarPolicy
    public static final int QT_SCROLLBARASNEEDED = 0;
    public static final int QT_SCROLLBARALWAYSOFF = 1;
    public static final int QT_SCROLLBARALWAYSON = 2;

    // Qt::LayoutDirection
    public static final int QT_LEFTTORIGHT = 0;
    public static final int QT_RIGHTTOLEFT = 1;

    // Qt::CheckState
    public static final int QT_UNCHECKED = 0;
    public static final int QT_PARTIALLYCHECKED = 1;
    public static final int QT_CHECKED = 2;

    // Qt::Orientation
    public static final int QT_HORIZONTAL = 1;
    public static final int QT_VERTICAL = 2;

    // Qt::SortOrder
    public static final int QT_ASCENDINGORDER = 0;
    public static final int QT_DESCENDINGORDER= 1;

    // QTextOption::LineWrapMode
    public static final int QTEXTEDIT_NOWRAP = 0;
    public static final int QTEXTEDIT_WIDGETWIDTH = 1;
    public static final int QTEXTEDIT_FIXEDPIXELWIDTH = 2;
    public static final int QTEXTEDIT_FIXEDCOLUMNWIDTH = 3;

    // Qt::ItemDataRole
    public static final int QT_DISPLAYROLE = 0;
    public static final int QT_DECORATIONROLE = 1;
    public static final int QT_EDITROLE = 2;
    public static final int QT_TOOLTIPROLE = 3;
    public static final int QT_STATUSTIPROLE = 4;
    public static final int QT_WHATSTHISROLE = 5;
    public static final int QT_SIZEHINTROLE = 13;
    public static final int QT_FONTROLE = 6;
    public static final int QT_TEXTALIGNMENTROLE = 7;
    public static final int QT_BACKGROUNDROLE = 8;
    public static final int QT_BACKGROUNDCOLORROLE = 8;
    public static final int QT_FOREGROUNDROLE = 9;
    public static final int QT_TEXTCOLORROLE = 9;
    public static final int QT_CHECKSTATEROLE = 10;
    public static final int QT_ACCESSIBLETEXTROLE = 11;
    public static final int QT_ACCESSIBLEDESCRIPTIONROLE = 12;
    public static final int QT_USERROLE = 32;

    // Qt::ItemFlags
    public static final int QT_NOITEMFLAGS = 0;
    public static final int QT_ITEMISSELECTABLE = 1;
    public static final int QT_ITEMISEDITABLE = 2;
    public static final int QT_ITEMISDRAGENABLED = 4;
    public static final int QT_ITEMISDROPENABLED = 8;
    public static final int QT_ITEMISUSERCHECKABLE = 16;
    public static final int QT_ITEMISENABLED = 32;
    public static final int QT_ITEMISTRISTATE = 64;

    // Qt::WindowStates
    public static final int QT_WINDOWNOSTATE = 0x0;
    public static final int QT_WINDOWMINIMIZED = 0x1;
    public static final int QT_WINDOWMAXIMIZED = 0x2;
    public static final int QT_WINDOWFULLSCREEN = 0x4;
    public static final int QT_WINDOWACTIVE = 0x8;

    // QHeaderView::ResizeMode
    public static final int QT_RESIZEMODEINTERACTIVE = 0;
    public static final int QT_RESIZEMODEFIXED = 2;
    public static final int QT_RESIZEMODESTRETCH = 1;
    public static final int QT_RESIZEMODERESIZETOCONTENTS = 3;

    // QMessageBox buttons
    public static final int QMESSAGEBOX_OK = 0x00000400;
    public static final int QMESSAGEBOX_CANCEL = 0x00400000;
    public static final int QMESSAGEBOX_YES = 0x00004000;
    public static final int QMESSAGEBOX_NO = 0x00010000;
    public static final int QMESSAGEBOX_ABORT = 0x00040000;
    public static final int QMESSAGEBOX_RETRY = 0x00080000;
    public static final int QMESSAGEBOX_IGNORE = 0x00100000;

    // QMessageBox icon
    public static final int QMESSAGEBOX_NOICON = 0;
    public static final int QMESSAGEBOX_QUESTION = 4;
    public static final int QMESSAGEBOX_INFORMATION = 1;
    public static final int QMESSAGEBOX_WARNING = 2;
    public static final int QMESSAGEBOX_CRITICAL = 3;

    // Qt::FocusPolicy
    public static final int QT_FOCUSPOLICY_TABFOCUS = 0x1;
    public static final int QT_FOCUSPOLICY_CLICKFOCUS = 0x2;
    public static final int QT_FOCUSPOLICY_STRONGFOCUS = QT_FOCUSPOLICY_TABFOCUS | QT_FOCUSPOLICY_CLICKFOCUS | 0x8;
    public static final int QT_FOCUSPOLICY_WHEELFOCUS = QT_FOCUSPOLICY_STRONGFOCUS | 0x4;
    public static final int QT_FOCUSPOLICY_NOFOCUS = 0x0;

    // QComboBox::SizeAdjustPolicy
    public static final int QT_QComboBox_AdjustToContents = 0;
    public static final int QT_QComboBox_AdjustToMinimumContentsLength = 2;

    // QComboBox::InsertPolicy
    public static final int QT_QComboBox_NoInsert = 0;

    // QAbstractSlider::SliderAction
    public static final int QT_SLIDERNOACTION = 0;
    public static final int QT_SLIDERSINGLESTEPADD = 1;
    public static final int QT_SLIDERSINGLESTEPSUB = 2;
    public static final int QT_SLIDERPAGESTEPADD = 3;
    public static final int QT_SLIDERPAGESTEPUSB = 4;
    public static final int QT_SLIDERTOMINIMUM = 5;
    public static final int QT_SLIDERTOMAXIMUM = 6;
    public static final int QT_SLIDERMOVE = 7;

    // Qt::TextInteractionFlags
    public static final int QT_NOTEXTINTERACTION = 0;
    public static final int QT_TEXTSELECTABLEBYMOUSE = 1;
    public static final int QT_TEXTSELECTABLEBYKEYBOARD = 2;
    public static final int QT_LINKSACCESSIBLEBYMOUSE = 4;
    public static final int QT_LINKSACCESSIBLEBYKEYBOARD = 8;
    public static final int QT_TEXTEDITABLE = 16;
    public static final int QT_TEXTEDITORINTERACTION = QT_TEXTSELECTABLEBYMOUSE  | QT_TEXTSELECTABLEBYKEYBOARD | QT_TEXTEDITABLE;
    public static final int QT_TEXTBROWSERINTERACTION = QT_TEXTSELECTABLEBYMOUSE | QT_LINKSACCESSIBLEBYMOUSE | QT_LINKSACCESSIBLEBYKEYBOARD;

    // QSystemTrayIcon::ActivationReason
    public static final int QT_TRAY_REASON_UNKNOWN = 0;
    public static final int QT_TRAY_REASON_CONTEXT = 1;
    public static final int QT_TRAY_REASON_DOUBLECLICK = 2;
    public static final int QT_TRAY_REASON_TRIGGER = 3;
    public static final int QT_TRAY_REASON_MIDDLECLICK = 4;

    // Qt::WindowFlags
    public static final int QT_WINDOWFLAGS_WINDOW = 0x1;
    public static final int QT_WINDOWFLAGS_DIALOG = 0x2 | QT_WINDOWFLAGS_WINDOW;
    public static final int QT_WINDOWFLAGS_TOOLTIP = 0xc | QT_WINDOWFLAGS_WINDOW;
    public static final int QT_WINDOWFLAGS_FRAMELESSWINDOWHINT = 0x800;
    public static final int QT_WINDOWFLAGS_CUSTOMIZEWINDOWHINT = 0x2000000;
    public static final int QT_WINDOWFLAGS_WINDOWTITLEHINT = 0x1000;
    public static final int QT_WINDOWFLAGS_WINDOWSYSTEMMENUHINT = 0x2000;
    public static final int QT_WINDOWFLAGS_WINDOWMINIMIZEBUTTONHINT = 0x4000;
    public static final int QT_WINDOWFLAGS_WINDOWMAXIMIZEBUTTONHINT = 0x8000;
    public static final int QT_WINDOWFLAGS_WINDOWSTAYSONTOPHINT = 0x40000;
    public static final int QT_WINDOWFLAGS_WINDOWCLOSEBUTTONHINT = 0x8000000;

    // Qt::WindowModality
    public static final int QT_WINDOWMODALITY_NONMODAL = 0;
    public static final int QT_WINDOWMODALITY_WINDOWMODAL = 1;
    public static final int QT_WINDOWMODALITY_APPLICATIONMODAL = 2;

    // Qt::WidgetAttribute
    public static final int QT_WA_NOSYSTEMBACKGROUND = 9;
    public static final int QT_WA_STATICCONTENTS = 5;
    public static final int QT_WA_PAINTOUTSIDEPAINTEVENT = 13;
    public static final int QT_WA_PAINTONSCREEN = 8;
    public static final int QT_WA_PAINTUNCLIPPED = 52;
    public static final int QT_WA_INPUTMETHODENABLED = 14;
    public static final int QT_WA_NOMOUSEPROPAGATION = 73;

    // QStyle::StandardPixmap
    public static final int QSTYLE_SP_MESSAGEBOXINFORMATION = 9;
    public static final int QSTYLE_SP_MESSAGEBOXWARNING = 10;
    public static final int QSTYLE_SP_MESSAGEBOXCRITICAL = 11;
    public static final int QSTYLE_SP_MESSAGEBOXQUESTION = 12;

    // QLayout::SizeConstraint
    public static final int QLAYOUT_SIZECONSTRAINT_SETDEFAULTCONSTRAINT = 0;
    public static final int QLAYOUT_SIZECONSTRAINT_SETNOCONSTRAINT = 1;


    //QAction::SoftKeyRole
    public static final int QACTION_NOSOFTKEY=0;
    public static final int QACTION_POSITIVESOFTKEY	=1;
    public static final int QACTION_NEGATIVESOFTKEY	= 2;
    public static final int QACTION_SELECTSOFTKEY= 3;

    public static final int QCHAR_DIRAL=13;
    public static final int QCHAR_DIRAN=5;
    public static final int QCHAR_DIRB=7;
    public static final int QCHAR_DIRBN=18;
    public static final int QCHAR_DIRCS=6;
    public static final int QCHAR_DIREN=2;
    public static final int QCHAR_DIRES=3;
    public static final int QCHAR_DIRET=4;
    public static final int QCHAR_DIRL=0;
    public static final int QCHAR_DIRLRE=11;
    public static final int QCHAR_DIRLRO=12;
    public static final int QCHAR_DIRNSM=17;
    public static final int QCHAR_DIRON=10;
    public static final int QCHAR_DIRPDF=16;
    public static final int QCHAR_DIRR=1;
    public static final int QCHAR_DIRRLE=14;
    public static final int QCHAR_DIRRLO=15;
    public static final int QCHAR_DIRS=8;
    public static final int QCHAR_DIRWS=9;
    public static final int SYMBIAN_KEYBOARD_KEYPAD=1;
    public static final int SYMBIAN_KEYBOARD_FULL=2;
 

    //
    // QApplication
    //
    public static final native int QApplication_swt_new(int jniUtilsHandle);
    public static final native int QApplication_swt_palette_new();
    public static final native int QApplication_swt_palette_new(int widget);
    public static final native int QApplication_focusWidget();
    public static final native void QApplication_setQuitOnLastWindowClosed(boolean quit);
    public static final native int QApplication_desktop();
    public static final native int QApplication_swt_desktopPaintDevice();
    public static final native int QApplication_activeWindow();
    public static final native void QApplication_beep();
    public static final native int QApplication_swt_font_new();
    public static final native int QApplication_swt_font_new(int widgetHandle);
    public static final native void QApplication_syncX();
    public static final native int QApplication_doubleClickInterval();
    public static final native int QApplication_style();
    public static final native int QApplication_layoutDirection();
    public static final native void QApplication_setLayoutDirection(int direction);
    public static final native void QApplication_setDoubleClickInterval(int interval);
    public static final native boolean QApplication_autoSipEnabled();
	public static final native void QApplication_setStyleSheet(String style);
	public static final native void QApplication_closeAllWindows();

    //
    // QCoreApplication
    //
    public static final native int QCoreApplication_instance();
    public static final native void QCoreApplication_setApplicationName( int handle, String name );
    public static final native void QCoreApplication_processEvents( int handle, int flags );
    public static final native void QCoreApplication_sendPostedEvents ( int handle );
    public static final native void QCoreApplication_sendPostedEvents ( int handle, int receiver, int event_type);
    public static final native boolean QCoreApplication_hasPendingEvents ( int handle );
    public static final native void QCoreApplication_postEvent ( int handle, int receiver, int event );
    public static final native void QCoreApplication_sendEvent ( int handle, int receiver, int event );

    //
    // QAbstractEventDispatcher
    //
    public static final native int QAbstractEventDispatcher_instance( int threadHandle );
    public static final native void QAbstractEventDispatcher_wakeup( int handle );
    public static final native void QAbstractEventDispatcher_interrupt( int handle );

    //
    // QMainWindow
    //
    public static final native int QMainWindow_new( int parent, int flags );
    public static final native void QMainWindow_setMenuBar( int handle, int menuBarHandle );
    public static final native void QMainWindow_setCentralWidget(int handle, int widget);

    //
    // QPaintDevice
    //
    public static final native int QPaintDevice_depth(int handle);
    public static final native int QPaintDevice_physicalDpiX(int handle);
    public static final native int QPaintDevice_physicalDpiY(int handle);
    public static final native int QPaintDevice_width(int handle);
    public static final native int QPaintDevice_height(int handle);
    public static final native boolean QPaintDevice_paintingActive(int handle);

    //
    // QWidget
    //
    public static final native int QWidget_new(int parent, int flags);
    public static final native void QWidget_addAction(int handle, int actionHandle);
    public static final native void QWidget_setVisible( int handle, boolean visible );
    public static final native boolean QWidget_isVisible(int handle);
    public static final native boolean QWidget_isVisibleTo(int handle, int parentHandle);
    public static final native void QWidget_setParent( int handle, int parent );
    public static final native void QWidget_resize(int handle, int width, int height);
    public static final native void QWidget_update(int handle, int x, int y, int width, int height);
    public static final native void QWidget_update(int handle);
    public static final native void QWidget_repaint(int handle, int x, int y, int width, int height);
    public static final native Rectangle QWidget_rect(int handle);
    public static final native Point QWidget_sizeHint(int handle);
    public static final native void QWidget_setMaximumSize(int handle, int maxw, int maxh);
    public static final native void QWidget_setMinimumSize(int handle, int minw, int minh);
    public static final native Point QWidget_maximumSize(int handle);
    public static final native Point QWidget_minimumSize(int handle);
    public static final native Point QWidget_frameSize(int handle);
    public static final native Rectangle QWidget_geometry(int handle);
    public static final native Rectangle QWidget_frameGeometry(int handle);
    public static final native void QWidget_setGeometry(int handle, int x, int y, int width, int height);
    public static final native void QWidget_move(int handle, int x, int y);
    public static final native Point QWidget_pos(int handle);
    public static final native Point QWidget_size(int handle);
    public static final native int QWidget_swt_palette_new( int handle );
    public static final native void QWidget_setPalette( int handle, int palette );
    public static final native void QWidget_swt_unsetPalette(int handle);
    public static final native void QWidget_setAutoFillBackground ( int handle, boolean enabled );
    public static final native void QWidget_setFocus(int handle, int focusReason);
    public static final native void QWidget_setFont( int handle, int fontHandle );
    public static final native void QWidget_activateWindow(int handle);
    public static final native boolean QWidget_isActiveWindow(int handle);
    public static final native void QWidget_raise(int handle);
    public static final native void QWidget_lower(int handle);
    public static final native Point QWidget_mapTo(int handle, int parent, int x, int y);
    public static final native Point QWidget_mapToGlobal(int handle, int x, int y);
    public static final native Point QWidget_mapFromGlobal(int handle, int x, int y);
    public static final native int[] QWidget_actions( int handle );
    public static final native int QWidget_actions_size( int handle );
    public static final native void QWidget_updateGeometry(int handle);
    public static final native void QWidget_insertAction(int handle, int beforeHandle, int actionHandle) ;
    public static final native boolean QWidget_isEnabled(int handle);
    public static final native boolean QWidget_isEnabledTo(int handle, int parent);
    public static final native void QWidget_setEnabled(int handle, boolean enabled);
    public static final native boolean QWidget_isHidden(int handle);
    public static final native void QWidget_setContentsMargins(int handle, int left, int top, int right, int bottom);
    public static final native void QWidget_setToolTip(int handle, String toolTip);
    public static final native String QWidget_toolTip(int handle);
    public static final native void QWidget_stackUnder(int handle, int widget);
    public static final native void QWidget_setWindowTitle(int handle, String title);
    public static final native String QWidget_windowTitle(int handle);
    public static final native void QWidget_setWindowIcon(int handle, int icon);
    public static final native void QWidget_setWindowIconText(int handle, String text);
    public static final native void QWidget_setLayout(int handle, int layoutHandle);
    public static final native void QWidget_setLayoutDirection(int handle, int layoutDirection);
    public static final native int QWidget_layoutDirection(int handle);
    public static final native void QWidget_removeAction( int handle, int actionHandle );
    public static final native int QWidget_swt_font_new( int handle );
    public static final native int QWidget_window(int handle);
    public static final native boolean QWidget_isModal(int handle);
    public static final native int QWidget_heightForWidth(int handle, int width);
    public static final native int QWidget_windowState(int handle);
    public static final native void QWidget_setWindowState(int handle, int windowState);
    public static final native int QWidget_focusPolicy(int handle);
    public static final native void QWidget_setFocusPolicy(int handle, int policy);
    public static final native void QWidget_setWindowModality(int handle, int modality);
    public static final native int QWidget_layout(int handle);
    public static final native void QWidget_setFocusProxy(int handle, int proxyHandle);
    public static final native int QWidget_focusProxy(int handle);
    public static final native int QWidget_windowFlags(int handle);
    public static final native void QWidget_setWindowFlags(int handle, int type);
    public static final native void QWidget_setFixedSize(int handle, int width, int height);
    public static final native void QWidget_setAttribute(int handle, int attribute, boolean on);
    public static final native void QWidget_setInputMethodHints(int handle, int attribute);
    public static final native int QWidget_inputMethodHints(int handle);
    public static final native void QWidget_clearFocus(int handle);
    public static final native void QWidget_scroll(int handle, int dx, int dy, int x, int y, int w, int h);
    public static final native void QWidget_setWindowOpacity(int handle, double level);
    public static final native double QWidget_windowOpacity(int handle);
    public static final native void QWidget_setUpdatesEnabled(int handle, boolean enable);
    public static final native void QWidget_grabMouse(int handle);
    public static final native void QWidget_releaseMouse(int handle);
    public static final native int QWidget_mouseGrabber();
    public static final native int QWidget_swt_paintDevice(int handle);
    public static final native void QWidget_setStyleSheet(int handle, String style);
    public static final native String QWidget_styleSheet( int handle );
    public static final native void QWidget_setInputContext(int handle, int inputContextHandle);
    public static final native boolean QWidget_testAttribute(int handle, int attribute);

    //
    // QFontMetrics
    //
    public static final native int QWidget_QFontMetrics_lineSpacing(int widgetHandle);

    //
    // QDesktopWidget
    //
    public static final native Rectangle QDesktopWidget_availableGeometry(int handle, int index);
    public static final native int QDesktopWidget_primaryScreen(int desktopWidgetHandle);
    public static final native int QDesktopWidget_screenCount(int desktopWidgetHandle);
    public static final native int QDesktopWidget_screen(int handle, int index);
    public static final native Rectangle QDesktopWidget_screenGeometry(int handle, int index);
    public static final native int QDesktopWidget_screenNumber(int desktopWidgetHandle, int handle);

    //
    // QObject
    //
    public static final native void QObject_installEventFilter( int handle, int filter );
    public static final native void QObject_removeEventFilter( int handle, int filter );
    public static final native void QObject_setProperty (int handle, String name, int value );
    public static final native int QObject_property (int handle, String name );
    static final native void QObject_delete( int handle ); // Protected on purpose, use QObjectDeleteWrapper
    public static final native void QObject_setParent( int handle, int parent );
    public static final native int QObject_parent( int handle );
    static final native void QObject_deleteLater(int handle); // Protected on purpose, use QObjectDeleteWrapper
    private static final native boolean QObject_connect( int sender, String signal, int receiver, String slot, int connectionType);
    public static final void QObject_connectOrThrow( int sender, String signal, int receiver, String slot, int connectionType) {
        if(!QObject_connect( sender, signal, receiver, slot, connectionType)) {
            SWT.error(SWT.ERROR_UNSPECIFIED);
        }
    }
    public static final native int QObject_thread ( int handle );
    public static final native int[] QObject_children(int handle);
    public static final native void QObject_dumpObjectInfo(int handle);
    public static final native void QObject_dumpObjectTree(int handle);
	public static final native void QObject_setObjectName(int handle, String value);

    //
    // QAbstractButton
    //
    public static final native void QAbstractButton_setCheckable(int handle, boolean checkable);
    public static final native void QAbstractButton_setText(int handle, String text);
    public static final native String QAbstractButton_text(int handle);
    public static final native void QAbstractButton_setChecked(int handle, boolean checked);
    public static final native boolean QAbstractButton_isChecked(int handle);
    public static final native void QAbstractButton_setIcon(int handle,  int iconHandle);
    public static final native void QAbstractButton_setIconSize(int handle, int w, int h);
    public static final native void QAbstractButton_setAutoExclusive(int handle, boolean auto);
    public static final native void QAbstractButton_click(int handle);

    //
    // QPushButton
    //
    public static final native int QPushButton_new();
    public static final native void QPushButton_setFlat(int handle, boolean flat);
    public static final native void QPushButton_setDefault(int handle, boolean def);
    public static final native boolean QPushButton_isDefault(int handle);

    //
    // QRadioButton
    //
    public static final native int QRadioButton_new();

    //
    // QCheckBox
    //
    public static final native int QCheckBox_new();

    //
    // QFrame
    //
    public static final native int QFrame_new();
    public static final native void QFrame_setFrameStyle( int handle, int style );
    public static final native int QFrame_frameWidth( int handle);
    public static final native void QFrame_setLineWidth( int handle, int width );
    public static final native int QFrame_frameShape( int handle );

    //
    // QAbstractScrollArea
    //
    public static final native int QAbstractScrollArea_viewPort(int handle);
    public static final native int QAbstractScrollArea_horizontalScrollBar(int handle);
    public static final native int QAbstractScrollArea_verticalScrollBar(int handle);
    public static final native void QAbstractScrollArea_setHorizontalScrollBarPolicy(int handle, int policy);
    public static final native void QAbstractScrollArea_setVerticalScrollBarPolicy(int handle, int policy);
    public static final native void QAbstractScrollArea_setCornerWidget(int handle, int widget);
    public static final native int QAbstractScrollArea_cornerWidget(int handle);

    //
    // QScrollArea
    //
    // Note that you can't call any QScrollArea specific functions in the base
    // classes common to widgets - only QAbstractScrollArea stuff that all
    // Scrollables implement. So there should not be anything needed from
    // QScrollArea besides creating it.
    //
    public static final native int QScrollArea_new(int parent);

    //
    // QScrollBar
    //
    public static final native int QScrollBar_value(int handle);
    public static final native void QScrollBar_setValue(int handle, int value);

    //
    // QAbstractSlider
    //
    public static final native int QAbstractSlider_singleStep(int handle);
    public static final native int QAbstractSlider_maximum(int handle);
    public static final native int QAbstractSlider_minimum(int handle);
    public static final native int QAbstractSlider_pageStep(int handle);
    public static final native int QAbstractSlider_value(int handle);
    public static final native void QAbstractSlider_setSingleStep(int handle, int value);
    public static final native void QAbstractSlider_setMaximum(int handle, int value);
    public static final native void QAbstractSlider_setMinimum(int handle, int value);
    public static final native void QAbstractSlider_setPageStep(int handle, int value);
    public static final native void QAbstractSlider_setValue(int handle, int value);
    //
    // QLabel
    //
    public static final native String QLabel_text( int handle );
    public static final native void QLabel_setText( int handle, String text );
    public static final native void QLabel_setAlignment ( int handle, int alignment );
    public static final native void QLabel_setWordWrap ( int handle, boolean wrap );
    public static final native void QLabel_setPixmap ( int handle, int imageHandle );
    public static final native int QLabel_new();
    public static final native void QLabel_setBuddy(int handle, int buddy);
    public static final native void QLabel_setTextInteractionFlags(int handle, int flags);
    public static final native void QLabel_setOpenExternalLinks(int handle, boolean bool);

    //
    // QLineEdit
    //
    public static final native int QLineEdit_new();
    public static final native void QLineEdit_setText(int handle, String text);
    public static final native void QLineEdit_cut(int handle);
    public static final native void QLineEdit_copy(int handle);
    public static final native String QLineEdit_text(int handle);
    public static final native void QLineEdit_insert(int handle, String string);
    public static final native void QLineEdit_setMaxLength(int handle, int maxLength);
    public static final native int QLineEdit_maxLength(int handle);
    public static final native void QLineEdit_setSelection(int handle, int start, int length);
    public static final native int QLineEdit_cursorPosition(int handle);
    public static final native void QLineEdit_setCursorPosition(int handle, int position);
    public static final native void QLineEdit_deselect(int handle);
    public static final native void QLineEdit_end(int handle, boolean mark);
    public static final native void QLineEdit_setReadOnly(int handle, boolean readOnly);
    public static final native boolean QLineEdit_isReadOnly(int handle);
    public static final native String QLineEdit_selectedText(int handle);
    public static final native int QLineEdit_selectionStart(int handle);
    public static final native void QLineEdit_selectAll(int handle);
    public static final native void QLineEdit_setEchoMode(int handle, int echoMode);
    public static final native int QLineEdit_echoMode(int handle);
    public static final native void QLineEdit_setAlignment(int handle, int alignment);
    public static final native void QLineEdit_setFrame(int handle, boolean showFrame);
    public static final native int QLineEdit_swt_getBorderWidth(int handle);
    public static final native void QLineEdit_setValidator(int handle, int validatorHandle);
    public static final native Point QLineEdit_swt_preferredClientSize(int handle);

    //
    // QTextEdit
    //
    public static final native int QTextEdit_new();
    public static final native String QTextEdit_toPlainText(int handle);
    public static final native void QTextEdit_setPlainText(int handle, String text);
    public static final native void QTextEdit_setHtml(int handle, String html);
    public static final native void QTextEdit_insertPlainText(int handle, String text);
    public static final native void QTextEdit_cut(int handle);
    public static final native void QTextEdit_copy(int handle);
    public static final native void QTextEdit_setReadOnly(int handle, boolean readOnly);
    public static final native boolean QTextEdit_isReadOnly(int handle);
    public static final native Rectangle QTextEdit_cursorRect(int handle);
    public static final native void QTextEdit_ensureCursorVisible(int handle);
    public static final native void QTextEdit_setAlignment(int handle, int alignment);
    public static final native void QTextEdit_setLineWrapMode(int handle, int wrapMode);

    public static final native void QTextEdit_swt_append(int handle, String string);
    public static final native int QTextEdit_swt_getLineCount(int handle);
    public static final native int QTextEdit_swt_getLineHeight(int handle);
    public static final native void QTextEdit_swt_setTopIndex(int handle, int lineIndex);
    public static final native int QTextEdit_swt_getTopIndex(int handle);
    public static final native int QTextEdit_swt_getCaretPosition(int handle);
    public static final native void QTextEdit_swt_clearSelection(int handle);
    public static final native String QTextEdit_swt_getSelectionText(int handle);
    public static final native int QTextEdit_swt_selectionStart(int handle);
    public static final native int QTextEdit_swt_selectionEnd(int handle);
    public static final native void QTextEdit_swt_setCursorPosition(int handle, int position, boolean moveCursorAnchor);
    public static final native void QTextEdit_swt_selectAll(int handle);
    public static final native int QTextEdit_swt_getCaretLineNumber(int handle);
    public static final native int QTextEdit_swt_getCharCount(int handle);
    public static final native Point QTextEdit_swt_preferredClientSize(int handle, int wHint);

    //
    // QValidator
    //
    public static final native int QRegExpValidator_new(int parentHandle, String regExp);
    public static final native boolean QValidator_validate(int handle, String string);

    //
    // QMenu
    //
    public static final native int QMenu_new( int parentHandle );
    public static final native void QMenu_addAction( int parentHandle, int handle );
    public static final native int QMenu_exec( int handle, int x, int y, int actionHandle );
    public static final native void QMenu_setDefaultAction(int handle, int actionHandle );
    public static final native int QMenu_defaultAction( int handle );
    public static final native void QMenu_popup(int handle, int x, int y, int actionHandle);

    //
    // QMenuBar
    //
    public static final native int QMenuBar_new( int parentHandle );
    public static final native void QMenuBar_addAction( int menuBarHandle, int actionHandle );

    //
    // QAction
    //
    public static final native int QAction_new( int parentHandle );
    public static final native void QAction_setSeparator( int handle, boolean bool );
    public static final native void QAction_setCheckable( int handle, boolean bool );
    public static final native void QAction_setText(int handle, String text);
    public static final native void QAction_setIconText(int handle, String text);
    public static final native void QAction_setMenu(int handle, int menuHandle );
    public static final native void QAction_setShortcut(int handle, int keySequenceHandle);
    public static final native void QAction_setEnabled(int handle, boolean enabled);
    public static final native boolean QAction_isEnabled(int handle);
    public static final native void  QAction_setChecked(int handle, boolean selected);
    public static final native boolean QAction_isChecked(int handle);
    public static final native void QAction_setIcon(int handle, int iconHandle);
    public static final native void QAction_setSoftKeyRole(int handle, int role);
    //
    // QActionGroup
    //
    public static final native int QActionGroup_new(int handle);
    public static final native void QActionGroup_setExclusive( int actionGroupHandle, boolean b);
    public static final native void QActionGroup_addAction(int actionGroupHandle, int handle);
    public static final native void QActionGroup_removeAction(int actionGroupHandle, int handle);

    //
    // QPixmap
    //
    public static final native boolean QPixmap_load( int handle, String fileName );
    public static final native int QPixmap_new();
    public static final native void QPixmap_delete(int handle);
    public static final native int QPixmap_swt_paintDevice(int handle);

    //
    // QPalette
    //
    public static final native void QPalette_setColor ( int handle, int group, int role, int red, int green, int blue );
    public static final native void QPalette_setColor ( int handle, int role, int red, int green, int blue );
    public static final native int[] QPalette_color ( int handle, int group, int role );
    public static final native void QPalette_delete( int handle );
    public static final native void QPalette_swt_setBrush(int handle, int role, int pixmap);
    public static final native void QPalette_swt_copyBrushFromPalette(int handle, int sourceHandle, int role);

    //
    // QTimer
    //
    public static final native int QTimer_new();
    public static final native void QTimer_setSingleShot(int handle, boolean singleShot);
    public static final native void QTimer_start(int handle, int msec);

    //
    // QFont
    //
    public static final native int QFont_new( String family, int pointSize, int weight, boolean italic );
    public static final native int QFont_new();
    public static final native String QFont_family( int handle );
    public static final native int QFont_weight( int handle );
    public static final native int QFont_pointSize( int handle );
    public static final native boolean QFont_italic( int handle );
    public static final native void QFont_delete( int hanlde );
    public static final native void QFont_setUnderline(int handle, boolean enable);
    public static final native void QFont_setOverline(int handle, boolean enable);
    public static final native void QFont_setStrikeOut(int handle, boolean enable);
    public static final native void QFont_setStretch(int handle, int stretch);
    public static final native void QFont_setFixedPitch(int handle, boolean enable);
    public static final native void QFont_setStyle(int handle, int style);
    public static final native void QFont_setWeight(int handle, int weight);
    public static final native void QFont_setStyleStrategy(int handle, int strategy);
    public static final native void QFont_setRawName(int handle, String rawName);
    public static final native boolean QFont_underline(int handle);
    public static final native boolean QFont_overline(int handle);
    public static final native boolean QFont_strikeOut(int handle);
    public static final native int QFont_stretch(int handle);
    public static final native boolean QFont_fixedPitch(int handle);
    public static final native int QFont_style(int handle);
    public static final native int QFont_styleStrategy(int handle);
    public static final native boolean QFont_swt_equals(int handle, int handle2);

    //
    // QFontDatabase in Device
    //
    public static final native int QFontDatabase_new();
    public static final native String[] QFontDatabase_families( int fontDbHandle );
    public static final native int[] QFontDatabase_swt_heights( int fontDbHandle, String family, String style );
    public static final native boolean QFontDatabase_isScalable( int fontDbHandle, String family, String style );
    public static final native String[]  QFontDatabase_styles( int fontDbHandle, String family );
    public static final native boolean QFontDatabase_bold( int fontDbHandle, String family, String style );
    public static final native boolean QFontDatabase_italic( int fontDbHandle, String family, String style );
    public static final native void QFontDatabase_delete(int handle);

    //
    // QKeySequence
    //
    public static final native int QKeySequence_new(int i);
    public static final native void QKeySequence_delete(int aHandle);

    //
    // QIcon
    //
    public static final native int QIcon_new(int pixmap);
    public static final native int QIcon_new();
    public static final native void QIcon_delete(int handle);
    public static final native int QIcon_pixmap(int handle, int w, int h);
    
    // 
    // HbIcon
    // 
    public static final native int HbIcon_new(String name);
    public static final native int HbIcon_pixmap(int handle);
    public static final native void HbIcon_delete(int handle);
    
    //
    // AbstractListModel
    //
    public static final native int ListModel_new( int style, int parent, int listType );
    public static final native void ListModel_beginInsertRows( int handle, int parentIndexHandle, int first, int last );
    public static final native void ListModel_endInsertRows(int handle );
    public static final native void ListModel_beginRemoveRows( int handle, int parentIndexHandle, int first, int last );
    public static final native void ListModel_endRemoveRows(int handle );
    public static final native void ListModel_layoutAboutToBeChanged( int handle );
    public static final native void ListModel_layoutChanged( int handle );
    public static final native void ListModel_setCheckState( int handle, int selectionHandle, int state );
    public static final native void ListModel_append( int handle, String string );
    public static final native void ListModel_append( int handle, String string, int imageHandle );
    public static final native void ListModel_append( int handle, String detailText, int[] detailImageHandles, int detailImageCount, String headingText, int[] headingImageHandles, int headingImageCount );
    public static final native void ListModel_insert( int handle, String string, int index );
    public static final native void ListModel_insert( int handle, String string, int index, int imageHandle );
    public static final native String ListModel_itemString( int handle, int index );
    public static final native int ListModel_itemHeight( int handle );
    public static final native int ListModel_insertPosition( int handle,  String string, boolean ascent );
    public static final native String[] ListModel_itemStrings( int handle );
    public static final native int ListModel_indexOf( int handle, String string, int start );
    public static final native void ListModel_remove( int handle, int index );
    public static final native void ListModel_setItemContentsToNull( int handle, int index);
    public static final native void ListModel_clearList(int dataModelHandle);

    //
    // ListItemDelegate
    //
    public static final native int ListBoxItemDelegate_new( int parentHandle );
    public static final native void ListBoxItemDelegate_setHeadingFont( int handle, int fontHandle );

    //
    // QAbstractItemModel
    //
    public static final native int QAbstractItemModel_index( int handle, int row, int column, int parentIndexHandle );
    public static final native int QAbstractItemModel_rowCount (int handle );

    //
    // QModelIndex
    //
    public static final native int QModelIndex_row( int handle );
    public static final native void QModelIndex_delete( int handle );

    //
    // QItemSelectionModel
    //
    public static final native void QItemSelectionModel_select( int handle, int indexHandle, int command );
    public static final native void QItemSelectionModel_select( int handle, int startIndexHandle, int endIndexHandle, int command  );
    public static final native void QItemSelectionModel_select__set( int handle, int selectionHandle, int command);
    public static final native void QItemSelectionModel_clearSelection( int handle );
    public static final native int QItemSelectionModel_currentIndex( int handle );
    public static final native void QItemSelectionModel_setCurrentIndex( int handle, int indexHandle, int command );
    public static final native int[] QItemSelectionModel_selectedRows( int handle, int column );
    public static final native boolean QItemSelectionModel_isSelected ( int handle, int indexHandle );

    //
    // QAbstractItemView
    //
    public static final native void QAbstractItemView_clearSelection(int handle);
    public static final native Point QAbstractItemView_iconSize(int handle);
    public static final native void QAbstractItemView_scrollTo(int handle, int modelIndexHandle, int  scrollHint);
    public static final native void QAbstractItemView_selectAll(int handle);
    public static final native int QAbstractItemView_selectionModel(int handle );
    public static final native void QAbstractItemView_setIconSize(int handle, int x, int y);
    public static final native void QAbstractItemView_setModel(int handle, int dataModelHandle);
    public static final native void QAbstractItemView_setSelectionBehavior(int handle, int behavior);
    public static final native void QAbstractItemView_setSelectionMode(int handle, int mode);
    public static final native int QAbstractItemView_sizeHintForColumn(int handle, int column);
    public static final native Point QAbstractItemView_sizeHintForIndex(int handle, int row, int column);
    public static final native int QAbstractItemView_sizeHintForRow(int handle, int row);
    public static final native int QAbstractItemView_swt_indexAt(int handle, int x, int y);
    public static final native void QAbstractItemView_swt_removeRows(int handle, int row, int count);
    public static final native void QAbstractItemView_update(int handle, int indexHandle);
    public static final native Rectangle QAbstractItemView_visualRect(int handle, int row, int column);
    public static final native int QAbstractItemView_currentIndex(int handle);

    //
    // QListView
    //
    public static final native int QListView_new();
    public static final native int QListView_flow(int handle);
    public static final native Point QListView_gridSize(int handle);
    public static final native int QListView_resizeMode(int handle);
    public static final native void QListView_setFlow(int handle, int flow);
    public static final native void QListView_setItemDelegate(int handle, int itemDelegateHandle);
    public static final native void QListView_setMovement(int handle, int movement);
    public static final native void QListView_setViewMode(int handle, int mode);
    public static final native void QListView_setGridSize(int handle, int width, int height);
    public static final native void QListView_setResizeMode(int handle, int mode);
    public static final native void QListView_setSpacing(int handle, int spacing);
    public static final native int QListView_spacing(int handle);

    //
    // QModelIndexList
    //
    public static final native int QModelIndexList_size(int handle);
    public static final native int QModelIndexList_at(int handle, int index);

    //
    // QSlider
    //
    public static final native int QSlider_new();
    public static final native int QSlider_singleStep(int handle);
    public static final native int QSlider_maximum(int handle);
    public static final native int QSlider_minimum(int handle);
    public static final native int QSlider_pageStep(int handle);
    public static final native int QSlider_value(int handle);
    public static final native void QSlider_setSingleStep(int handle, int value);
    public static final native void QSlider_setMaximum(int handle, int value);
    public static final native void QSlider_setMinimum(int handle, int value);
    public static final native void QSlider_setPageStep(int handle, int value);
    public static final native void QSlider_setValue(int handle, int value);
    public static final native void QSlider_setOrientation(int handle, int value);

    //
    // QColor
    //
    public static final native int QColor_new();
    public static final native int QColor_new(int red, int green, int blue);
    public static final native int QColor_red(int handle);
    public static final native int QColor_green(int handle);
    public static final native int QColor_blue(int handle);
    public static final native void QColor_setRed(int handle, int red);
    public static final native void QColor_setGreen(int handle, int green);
    public static final native void QColor_setBlue(int handle, int blue);
    public static final native void QColor_delete(int handle);
    public static final native boolean QColor_equals(int handle, int compareHandle);
    public static final native boolean QColor_isValid(int handle);

    //
    // QColorDialog
    //
    public static final native int QColorDialog_getColor(int initialColor, int parentHandle, String dialogId, int layoutDirection) ;

    //
    // QTableWidget
    //
    public static final native int QTableWidget_new();
    public static final native void QTableWidget_clearContents(int handle);
    public static final native int QTableWidget_column(int handle, int columnHandle);
    public static final native int QTableWidget_columnCount(int handle);
    public static final native int QTableWidget_currentColumn(int handle);
    public static final native int QTableWidget_currentRow(int handle);
    public static final native void QTableWidget_insertRow(int handle, int row);
    public static final native void QTableWidget_insertColumn(int handle, int column);
    public static final native int QTableWidget_itemAt(int handle, int x, int y);
    public static final native void QTableWidget_scrollToItem(int handle, int itemHandle, int scrollHint);
    public static final native void QTableWidget_setCellWidget(int handle, int row, int column, int widgetHandle);
    public static final native void QTableWidget_setColumnCount(int handle, int columnCount);
    public static final native void QTableWidget_setCurrentCell(int handle, int row, int column);
    public static final native void QTableWidget_setCurrentItem(int handle, int itemHandle);
    public static final native void QTableWidget_setItem(int handle, int row, int column, int itemHandle);
    public static final native void QTableWidget_setRowCount(int handle, int rowCount);
    public static final native void QTableWidget_removeColumn(int handle, int column);
    public static final native void QTableWidget_removeRow(int handle, int row);
    public static final native int QTableWidget_row(int handle, int rowHandle);
    public static final native int QTableWidget_rowCount(int handle);
    public static final native int QTableWidget_topIndex(int handle);
    public static final native Rectangle QTableWidget_visualItemRect(int handle, int itemHandle);

    public static final native int QTableWidget_swt_header(int handle, boolean horizontalHeader);
    public static final native int QTableWidget_swt_headerHeight(int handle, boolean horizontalHeader);
    public static final native int QTableWidget_swt_headerItem(int handle, int index);
    public static final native int QTableWidget_swt_headerSize(int handle, boolean horizontalHeader, int index);
    public static final native boolean QTableWidget_swt_headerVisible(int handle, boolean horizontalHeader);
    public static final native void QTableWidget_swt_resizeHeaderSection(int handle, boolean horizontalHeader, int index, int size);
    public static final native int[] QTableWidget_swt_selectedItems(int handle);
    public static final native int QTableWidget_swt_selectionCount(int handle);
    public static final native void QTableWidget_swt_setHeaderIcon(int handle, int index, int iconHandle);
    public static final native void QTableWidget_swt_setHeaderText(int handle, int column, String text);
    public static final native void QTableWidget_swt_setHeaderVisible(int handle, boolean horizontalHeader, boolean visible);

    //
    // QTableView
    //
    public static final native int QTableView_columnWidth(int handle, int column);
    public static final native int QTableView_rowAt(int handle, int y);
    public static final native int QTableView_rowHeight(int handle, int row);
    public static final native void QTableView_setColumnHidden(int handle, int column, boolean hide);
    public static final native void QTableView_setColumnWidth(int handle, int column, int width);
    public static final native void QTableView_setRowHeight(int handle, int row, int height);
    public static final native void QTableView_setShowGrid(int handle, boolean show);
    public static final native void QTableView_setWordWrap(int handle, boolean on);
    public static final native void QTableView_showColumn(int handle, int column);
    public static final native boolean QTableView_showGrid(int handle);
    public static final native void QTableView_showRow(int handle, int row);

    //
    // QTableWidgetItem
    //
    public static final native int QTableWidgetItem_new();
    public static final native int QTableWidgetItem_data(int handle, int role);
    public static final native void QTableWidgetItem_delete(int handle);
    public static final native int QTableWidgetItem_flags(int handle);
    public static final native boolean QTableWidgetItem_grayed(int handle);
    public static final native boolean QTableWidgetItem_isSelected(int handle);
    public static final native void QTableWidgetItem_setData(int handle, int role, int data);
    public static final native void QTableWidgetItem_setFlags(int handle, int flags);
    public static final native void QTableWidgetItem_setIcon(int handle, int iconHandle);
    public static final native void QTableWidgetItem_setSelected(int handle, boolean selected);
    public static final native void QTableWidgetItem_setText(int handle, String text);
    public static final native void QTableWidgetItem_setTextAlignment(int handle, int alignment);
    public static final native String QTableWidgetItem_text(int handle);

    public static final native void QTableWidgetItem_swt_setBackground(int handle, int red, int green, int blue, boolean restore);
    public static final native void QTableWidgetItem_swt_setFont(int handle, int fontHandle);
    public static final native void QTableWidgetItem_swt_setForeground(int handle, int red, int green, int blue, boolean restore);
    public static final native void QTableWidgetItem_swt_setGrayed(int handle, boolean grayed);

    //
    // QHeaderView
    //
    public static final native int QHeaderView_resizeMode(int handle, int index);
    public static final native int QHeaderView_sectionSizeHint(int handle, int index);
    public static final native void QHeaderView_setResizeMode(int handle, int index, int resizeMode);

    //
    // QProgressBar
    //
    public static final native int QProgressBar_new();
    public static final native void QProgressBar_setOrientation(int handle,int orientation);
    public static final native void QProgressBar_setTextInVisible(int handle);
    public static final native void QProgressBar_setValue(int handle, int value);
    public static final native void QProgressBar_setMinimum(int handle, int value);
    public static final native void QProgressBar_setMaximum(int handle, int value);

    //
    // QComboBox
    //
    public static final native int QComboBox_new();
    public static final native void QComboBox_clear(int handle);
    public static final native int QComboBox_count(int handle);
    public static final native int QComboBox_currentIndex(int handle);
    public static final native int QComboBox_findText(int handle, String text);
    public static final native int QComboBox_lineEdit(int handle);
    public static final native int QComboBox_view(int handle);
    public static final native void QComboBox_insertItem( int handle, String string,int index );
    public static final native String QComboBox_itemText(int handle, int index);
    public static final native int QComboBox_maxVisibleItems(int handle);
    public static final native void QComboBox_removeItem(int handle, int index);
    public static final native void QComboBox_setCurrentIndex(int handle, int index);
    public static final native void QComboBox_setEditable(int handle, boolean editable);
    public static final native void QComboBox_setInsertPolicy(int handle, int policy);
    public static final native void QComboBox_setItemText(int handle, String text, int index);
    public static final native void QComboBox_setMaxVisibleItems(int handle, int count);
    public static final native void QComboBox_setSizeAdjustPolicy(int handle, int adjustPolicy);
    public static final native void QComboBox_swt_deselect(int handle);
    public static final native int QComboBox_swt_getItemHeight(int handle);
    public static final native Point QComboBox_swt_getSelection(int handle);
    public static final native int QComboBox_swt_getTextHeight(int handle);
    public static final native void QComboBox_swt_setItems(int handle, String [] items);
    public static final native void QComboBox_swt_setText(int handle, String text);

    //
    // QListWidget
    //
    public static final native int QListWidget_new();
    public static final native void QListWidget_addItem(int handle, String string, int iconHandle);
    public static final native void QListWidget_addItems(int handle, String[] items, int[] iconHandles);
    public static final native void QListWidget_clear(int handle);
    public static final native int QListWidget_count(int handle);
    public static final native int QListWidget_currentRow(int handle);
    public static final native void QListWidget_insertItem(int handle, int index, String string, int iconHandle);
    public static final native void QListWidget_scrollToItem(int handle, int index, int scrollHint);
    public static final native void QListWidget_setSortingEnabled(int handle, boolean status);
    public static final native void QListWidget_sortItems(int handle, int order);
    public static final native int QListWidget_swt_indexOf(int handle, String text, int startIndex);
    public static final native boolean QListWidget_swt_isItemVisible(int handle, int index);
    public static final native boolean QListWidget_swt_isItemSelected(int handle, int index);
    public static final native String QListWidget_swt_itemText(int handle, int index);
    public static final native String[] QListWidget_swt_itemTexts(int handle);
    public static final native void QListWidget_swt_removeArray(int handle, int[] arr, int count);
    public static final native void QListWidget_swt_removeIndex(int handle, int index);
    public static final native void QListWidget_swt_removeRange(int handle, int start, int end);
    public static final native void QListWidget_swt_selectArray(int handle, int[] arr, int count, int command, boolean scrollFocus);
    public static final native void QListWidget_swt_selectIndex(int handle, int index, int command);
    public static final native void QListWidget_swt_selectRange(int handle, int start, int end, int command);
    public static final native int[] QListWidget_swt_selectedRows(int handle, int column);
    public static final native void QListWidget_swt_setFocusIndex(int handle, int index);
    public static final native void QListWidget_swt_setItemVisible(int handle, int index, boolean status);
    public static final native void QListWidget_swt_setText(int handle, int index, String text);

    //
    // QTreeWidget
    //
    public static final native int QTreeWidget_new();
    public static final native void QTreeWidget_addTopLevelItem(int handle, int itemHandle);
    public static final native int[] QTreeWidget_addTopLevelItems(int handle, int count);
    public static final native void QTreeWidget_clear(int handle);
    public static final native int QTreeWidget_currentItem(int handle);
    public static final native int QTreeWidget_indexOfTopLevelItem(int handle, int itemHandle);
    public static final native void QTreeWidget_insertTopLevelItem(int handle, int itemHandle, int index);
    public static final native int QTreeWidget_itemAt(int handle, int xPos, int yPos);
    public static final native void QTreeWidget_scrollToItem(int handle, int itemHandle, int scrollHint);
    public static final native int[] QTreeWidget_selectedItems(int handle);
    public static final native void QTreeWidget_setColumnCount(int handle, int count);
    public static final native void QTreeWidget_setCurrentItem(int handle, int itemHandle);
    public static final native void QTreeWidget_swt_setHeaderVisible(int handle, boolean visible);
    public static final native void QTreeWidget_swt_setHeaderStretch(int handle, boolean visible);
    public static final native int QTreeWidget_swt_selectionCount(int handle);
    public static final native int QTreeWidget_swt_rowAt(int handle, int x, int y);
    public static final native void QTreeWidget_takeTopLevelItem(int handle, int index);
    public static final native Rectangle QTreeWidget_visualItemRect(int handle, int itemHandle);

    //
    // QTreeWidgetItem
    //
    public static final native int QTreeWidgetItem_new();
    public static final native int[] QTreeWidgetItem_addChildren(int handle, int count);
    public static final native int QTreeWidgetItem_checkState(int handle);
    public static final native int QTreeWidgetItem_indexOfChild(int parentHandle, int childHandle);
    public static final native void QTreeWidgetItem_insertChild(int handle, int childHandle, int index);
    public static final native boolean QTreeWidgetItem_isExpanded(int handle);
    public static final native void QTreeWidgetItem_removeChild(int parentHanle, int childHandle);
    public static final native void QTreeWidgetItem_setCheckState(int handle, int flags);
    public static final native void QTreeWidgetItem_setExpanded(int handle, boolean expanded);
    public static final native void QTreeWidgetItem_setFlags(int handle, int flags);
    public static final native void QTreeWidgetItem_setIcon(int handle, int imageHandle);
    public static final native void QTreeWidgetItem_setSelected(int handle, boolean select);
    public static final native void QTreeWidgetItem_setText(int handle, String text);
    public static final native int QTreeWidgetItem_swt_isSameParent(int firstHandle, int lastHandle);
    public static final native int QTreeWidget_swt_parentChildCount(int handle);
    public static final native void QTreeWidgetItem_swt_setBackground(int handle, int red, int green, int blue, boolean restore);
    public static final native void QTreeWidgetItem_swt_setFont(int handle, int fontHandle);
    public static final native void QTreeWidgetItem_swt_setForeground(int handle, int red, int green, int blue, boolean restore);

    //
    // QTreeView
    //
    public static final native int QTreeView_columnWidth(int handle, int column);
    public static final native void QTreeView_resizeColumnToContents(int handle, int column);
    public static final native void QTreeView_selectAll(int handle);

    //
    // QSwtTimeEdit
    //
    public static final native int QSwtTimeEdit_swt_new(int style);
    public static final native int QSwtTimeEdit_editor(int handle);
    public static final native int QSwtTimeEdit_cursorPosition(int handle);
    public static final native int QSwtTimeEdit_textLength(int handle);
    public static final native void QSwtTimeEdit_swt_setTextVisible(int handle, boolean visible);

    //
    // QDateTimeEdit
    //
    public static final native int QDateTimeEdit_new();
    public static final native int QDateTimeEdit_calendarWidget(int handle);
    public static final native void QDateTimeEdit_setCalendarPopup(int handle, boolean enable);
    public static final native void QDateTimeEdit_setDisplayFormat(int handle, String format);
    public static final native int QDateTimeEdit_currentSectionIndex(int handle);
    public static final native int QDateTimeEdit_sectionCount(int handle);

    public static final native int [] QDateTimeEdit_swt_dateTime(int handle);
    public static final native void QDateTimeEdit_swt_setDate(int handle, int [] date);
    public static final native void QDateTimeEdit_swt_setTime(int handle, int [] time);
    public static final native void QDateTimeEdit_swt_setDateTime(int handle, int [] dateTime);

    //
    // QOffsetTimeEdit
    //
    public static final native int QOffsetTimeEdit_new();
    public static final native int QOffsetTimeEdit_offset(int handle);
    public static final native void QOffsetTimeEdit_setOffset(int handle, int offset);
    public static final native void QOffsetTimeEdit_setMinimum(int handle, int minimum);
    public static final native void QOffsetTimeEdit_setMaximum(int handle, int maximum);

    //
    // QProgressDialog
    //
    public static final native int QProgressDialog_swt_new(int parentHandle, boolean showBar);
    public static final native int QProgressDialog_maximum(int handle);
    public static final native int QProgressDialog_minimum(int handle);
    public static final native int QProgressDialog_value(int handle);
    public static final native void QProgressDialog_setAutoClose(int handle, boolean close);
    public static final native void QProgressDialog_setAutoReset(int handle, boolean reset);
    public static final native void QProgressDialog_setMaximum(int handle, int maximum);
    public static final native void QProgressDialog_setMinimum(int handle, int minimum);
    public static final native void QProgressDialog_setValue(int handle, int value);
    public static final native void QProgressDialog_setLabelText(int handle, String text);
    public static final native void QProgressDialog_setBar(int handle, int barHandle);
    public static final native void QProgressDialog_setCancelButton(int handle, int buttonHandle);


    //
    // QEvent
    //
    public static final native int QEvent_new(int type);

    //
    // QMouseEvent
    //
    public static final native int QMouseEvent_new(int type, int xPos, int yPos, int xGlobalPos, int yGlobalPos, int button, int buttons, int modifiers);

    //
    // QKeyEvent
    //
    public static final native int QKeyEvent_new(int type, int key, int modifiers, String text);

    //
    // QCaptionedWidget
    //
    public static final native int QCaptionedWidget_new();

    //
    // QVBoxLayout
    //
    public static final native int QVBoxLayout_new(int parent);

    //
    // QLayout
    //
    public static final native void QLayout_addWidget(int handle, int widget);
    public static final native void QLayout_setContentsMargins(int handle, int left, int top, int right, int bottom);
    public static final native void QLayout_setMenuBar(int handle, int menubar);
    public static final native void QLayout_setSpacing(int handle, int spacing);
    public static final native boolean QLayout_activate(int handle);
    public static final native void QLayout_update(int handle);
    public static final native int QLayout_sizeConstraint(int handle);
    public static final native void QLayout_setSizeConstraint(int handle, int constraint);

    //
    // QDesktopServices
    //
    public static final native boolean QDesktopServices_openUrl(String url);

    //
    // QMessageBox
    //
    public static final native int QMessageBox_swt_exec(int icon, String title, String text,
            int buttons, int parentHandle, int modality, String dialogID, int layoutDirection);
    public static final native void QMessageBox_swt_execTimer(int aIcon, String title, String text,
            int parentHandle, String aDialogID, int aLayoutDirection, int modality, int pixmapHandle);

    //
    // QInputDialog
    //
    public static final native String QInputDialog_swt_getText(int parentHandle, String title, String label, int echoMode, String defaultText, String dialogId, int layoutDirection);
    public static final native String QInputDialog_swt_getInteger(int parentHandle, String title, String label, int min,
            int max, int defaultValue, String dialogId, int layoutDirection);

    //
    // QSystemTrayIcon
    //
    public static final native boolean QSystemTrayIcon_isSystemTrayAvailable();
    public static final native int QSystemTrayIcon_new( int handle);
    public static final native void QSystemTrayIcon_setIcon(int handle,int icon );
    public static final native boolean QSystemTrayIcon_isVisible(int handle);
    public static final native void QSystemTrayIcon_setVisible(int handle, boolean visible);
    public static final native void QSystemTrayIcon_setToolTip(int handle, String string);

    //
    // QFileDialog
    //
    public static final native String[] QFileDialog_swt_getOpenFileName( int parent, String title, String directory, String filter, String selectedFilter, String dialogID, int layoutDirection );
    public static final native String[] QFileDialog_swt_getOpenFileNames( int parent, String title, String directory, String filter, String selectedFilter, String dialogID, int layoutDirection );
    public static final native String[] QFileDialog_swt_getSaveFileName( int parent, String title, String directory, String filter, String selectedFilter, String dialogID, int layoutDirection );
    public static final native String QFileDialog_swt_getOpenDirectoryName( int parent, String title, String directory, String dialogID, int layoutDirection );
    public static final native void QDialog_swt_closeDialogs( int parent, String dialogID );

    //
    // QDialog
    //
    public static final native int QDialog_new(int parent, int flags);

    //
    // QTabWidget
    //
    public static final native int QSwtTabWidget_new();
    public static final native void QTabWidget_addTab(int handle, int pageHandle, int iconHandle, String Label);
    public static final native int QTabWidget_count(int handle);
    public static final native int QTabWidget_currentIndex(int handle);
    public static final native void QTabWidget_removeTab(int handle, int index);
    public static final native void QTabWidget_setCurrentIndex(int handle, int index);
    public static final native void QTabWidget_setTabIcon(int handle, int index, int iconHandle);
    public static final native void QTabWidget_setTabText(int handle, int index, String text);
    public static final native String QTabWidget_tabText(int handle, int index);
    public static final native int QTabWidget_widget(int handle, int index);
    public static final native int QTabWidget_swt_stackedWidgetHandle(int handle);
    public static final native Point QSwtTabWidget_stackedWidgetSize(int handle);
    public static final native int QTabWidget_tabBar(int handle);

    //
    // QClipboard
    //
    public static final native void QClipboard_clear();
    public static final native byte[] QClipboard_mimeData(String mimeType);
    public static final native void QClipboard_setMimeData(String mimeType, byte[] byteArr);
    public static final native String[] QClipboard_swt_formats();
    public static final native boolean QClipboard_swt_isEmptyText();
    public static final native String QClipboard_text();

    //
    // QFontDialog
    //
    public static final native int QFontDialog_swt_getFont(int parentHandle, String title,
            String family, int pointSize, int weight, boolean italic, String dialogId, int layoutDirection);

    //
    // QStyle
    //
    public static final native int QStyle_pixelMetric(int id);
    public static final native int QStyle_standardIcon(int handle, int standardIcon);
    public static final native int QStyle_swt_pm_dockWidgetTitleBarButtonMargin();
    public static final native int QStyle_swt_pm_mdiSubWindowMinimizedWidth();
    public static final native int QStyle_swt_pm_layoutLeftMargin();
    public static final native int QStyle_swt_pm_layoutTopMargin();
    public static final native int QStyle_swt_pm_layoutRightMargin();
    public static final native int QStyle_swt_pm_layoutBottomMargin();
    public static final native int QStyle_swt_pm_layoutHorizontalSpacing();
    public static final native int QStyle_swt_pm_layoutVerticalSpacing();
    public static final native int QStyle_swt_pm_dockWidgetTitleMargin();
    public static final native int QStyle_swt_pm_toolBarFrameWidth();
    public static final native int QStyle_swt_pm_toolBarHandleExtent();
    public static final native int QStyle_swt_pm_toolBarItemMargin();
    public static final native int QStyle_swt_pm_toolBarItemSpacing();
    public static final native int QStyle_swt_pm_toolBarSeparatorExtent();
    public static final native int QStyle_swt_pm_toolBarExtensionExtent();
    public static final native int QStyle_swt_pm_tabBarScrollButtonWidth();
    public static final native int QStyle_swt_pm_tabBarTabShiftHorizontal();
    public static final native int QStyle_swt_pm_tabBarTabShiftVertical();
    public static final native int QStyle_swt_pm_headerMarkSize();
    public static final native int QStyle_swt_pm_headerGripMargin();
    public static final native int QStyle_swt_pm_headerMargin();
    public static final native int QStyle_swt_pm_SpinBoxSliderHeight();
    public static final native int QStyle_swt_pm_toolBarIconSize();
    public static final native int QStyle_swt_pm_smallIconSize();
    public static final native int QStyle_swt_pm_largeIconSize();
    public static final native int QStyle_swt_pm_focusFrameHMargin();
    public static final native int QStyle_swt_pm_focusFrameVMargin();
    public static final native int QStyle_swt_pm_iconViewIconSize();
    public static final native int QStyle_swt_pm_listViewIconSize();
    public static final native int QStyle_swt_pm_toolTipLabelFrameWidth();
    public static final native int QStyle_swt_pm_checkBoxLabelSpacing();
    public static final native int QStyle_swt_pm_radioButtonLabelSpacing();
    public static final native int QStyle_swt_pm_tabBarIconSize();
    public static final native int QStyle_swt_pm_sizeGripSize();
    public static final native int QStyle_swt_pm_messageBoxIconSize();
    public static final native int QStyle_swt_pm_buttonIconSize();
    public static final native int QStyle_swt_pm_textCursorWidth();
    public static final native int QStyle_swt_pm_tabBarScrollButtonOverlap();
    public static final native int QStyle_swt_pm_tabCloseIndicatorWidth();
    public static final native int QStyle_swt_pm_tabCloseIndicatorHeight();

    //
    // QLocale
    //
    public static final native int QLocale_system();
    public static final native int QLocale_language(int handle);
    public static final native void QLocale_delete(int handle);

    //
    // QImage
    //
    public static final native int QImage_swt_paintDevice(int handle);

    //
    // QChar
    //
    public static final native int QChar_direction(char c);

    //
    // SwtFontCache
    //

    /**
     * Ensures the font or another equal font is in the font cache and returns a
     * handle to the cached font. Takes ownership of the passed QFont handle.
     *
     * Only the fonts owned by SWT should be added to the cache. Don't ever add any
     * fonts that the client application has created to the cache. Fonts are never
     * removed from the cache.
     *
     * Returned fonts must never be deleted. They are owned by the cache. Returned
     * fonts can be referenced safely for the life-time of Display.
     *
     * @param handle A handle to a QFont
     * @return A handle to a cached QFont with equal properties to given QFont.
     */
    public static final native int SwtFontCache_cache(int handle);

    /**
     * Checks if the given QFont handle is in the font cache.
     * @param handle QFont handle
     */
    public static final native boolean SwtFontCache_isCached(int handle);

    //
    // QInputContextFactory
    //
    public static final native int QInputContextFactory_create(String key, int parent);
    public static final native String QInputContextFactory_swt_key(String language );

    //
    // MobileDevice, Screen, Input
    //
    public static final native int MobileDevice_new();
    public static final native void MobileDevice_createFlipWatch(int handle);
    public static final native void MobileDevice_destroy(int handle);
    public static final native int MobileDevice_getUserInactivityTime(int handle);
    public static final native void MobileDevice_resetUserInactivityTime(int handle);
    public static final native void MobileDevice_setLight(int handle, int duration);
    public static final native boolean MobileDevice_flashLights(int handle, int duration);
    public static final native boolean MobileDevice_vibration(int handle, int duration);
    public static final native int getScreenDeviceNumber();
    public static final native int getColorDepth();
    public static final native int getHwInputs();
    
    //
    // SwtApplication
    //

    /**
     * Returns the number of currently active Java callbacks. Can't be called
     * after the application is deleted.
     */
    public static final native int SwtApplication_javaCallbackCount();

    /**
     * Posts the QEvents that have been added to the deferred event list. This
     * is used for events that have been canceled for some reason and need to be
     * retried. At least deferred deletion events can fail if it's attempted to
     * delete objects inside their event handlers. In this case the original
     * event gets deleted and a new deferred deletion event is created and added
     * to the deferred event list to wait until this method is called.
     */
    public static final native void SwtApplication_postDeferredEvents();

    //
    // Special purpose global functions exported by Qt
    //
    public static final native void qt_x11_wait_for_window_manager(int handle);
    
    //
    // XQServiceRequest
    //
    public static final native int XQServiceRequest_new(String service, String operation, boolean synchronou);   
    public static final native boolean XQServiceRequest_send(int handle);   
    public static final native void XQServiceRequest_swt_setArgumentsForFetchEmail(int handle, String title, String action, String filter);
    public static final native void XQServiceRequest_swt_setArgumentsForDial(int handle, String number, boolean asyncAnswer); 
    
    //
    // CntServicesContactList
    //
    public static final native String[] CntServicesContactList_swt_contacts(int handle);

    //
    // Other
    //
    public static final native int EventHandler_new( );
    public static final native void EventHandler_destroy(int handle);
    
    /**
     * Creates a QObject which implements slots to receive signals and passes
     * them to the Display along with the signal parameters and the signal id. 
     * @param widget The handle of the widget which will send the signal.
     * @param signalId The id that can be used to identify the signal.
     * @return Handle of the slot object, owned by the widget that was passed as the first parameter. 
     */
    public static final native int SignalHandler_new(int widget, int signalId);
    
    /**
     * Creates a QObject which implements slots to receive signals and forwards
     * them to the given Object along with the signal parameters and the signal id. 
     * @param widget The handle of the widget which will send the signal.
     * @param peer The peer Java object where the signal is delivered to. 
     * @param signalId The id that can be used to identify the signal.
     * @return Handle of the slot object, owned by the widget that was passed as the first parameter. 
     */
    public static final native int SignalForwarder_new(int widget, Object peer, int signalId);
    
    /**
     * Creates the JNI utility used for all JNI activity.
     * @return handle or 0 in case of failure, doesn't throw an exception
     */
    public static final native int JniUtils_new();

    /**
     * Performs a check if the QObject can be safely deleted immediately. If not
     * then a deleteLater event should be posted. It's possible that an object can't
     * be deleted safely e.g. when inside an event listener of the object.
     */
    public static final native boolean JniUtils_safeToDelete(int handle, int qObjectHandle);
    
    private static final native int windowServer();

    /**
     * Some platforms require initialization before Qt-APIs can be
     * used. This method will perform the required initialization.
     *
     * @return 0 in case of success, non-zero in case of failure.
     */
    public static final native int initUiThread(int uid);
    
    /**
     * Some platforms require initialization before Qt-APIs can be used
     * and cleaning up after the application is done with them. This method will
     * perform the required cleaning up.
     */
    public static final native void cleanUpUiThread();

    public static final native void setSymbianAppName(String name);

    // Add new stuff above the category "Other", that is the last category
}
