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

package org.eclipse.swt.widgets;

import org.eclipse.ercp.swt.mobile.Command;
import org.eclipse.ercp.swt.mobile.MobileDevice;
import org.eclipse.swt.graphics.Image;
import org.eclipse.swt.graphics.Point;
import org.eclipse.swt.internal.qt.CommandArranger;
import org.eclipse.swt.internal.qt.graphics.WindowSurface;

/**
 * <p>
 * <b>IMPORTANT:</b> This class is <em>not</em> part of the SWT public API. It
 * must never be accessed from application code.
 * </p>
 */
public final class Internal_PackageSupport {
/*
 * From the class Widget
 */
public static int handle(Widget w) {
    return w.handle;
}

public static boolean isInternalWidget(Widget w) {
    return w.isInternalWidget_pp();
}
public static void setHandle(Widget w, int handle) {
    w.handle = handle;
}
public static void setWidgetInternalOnly(Widget w) {
    w.setWidgetInternalOnly_pp();
}
public static int topHandle(Widget w) {
    return w.topHandle;
}
public static void setTopHandle(Widget w, int handle) {
    w.topHandle = handle;
}
public static int frameHandle(Control c) {
    return c.frameHandle;
}
public static void setFrameHandle(Control c, int handle) {
    c.frameHandle = handle;
}
public static void addCommand(Control control, final Command command) {
    control.addCommand(command);
}
public static void addWidget(Widget w, int handle) {
    w.addWidget(handle);
}
public static void checkOrientation(Widget w, Widget parent) {
    w.checkOrientation(parent);
}
public static void createWidget(Widget w, int index) {
    w.createWidget(index);
}
public static void createHandle(Widget w, int index) {
    w.createHandle_pp(index);
}
public static void setStyle(Widget w, int style) {
    w.style = style;
}
public static int style(Widget w) {
    return w.style;
}
public static void setState(Widget w, int state) {
    w.state = state;
}
public static int state(Widget w) {
    return w.state;
}
public static Display display(Widget w) {
    return w.display;
}
public static int checkBits (int style, int int0, int int1, int int2, int int3, int int4, int int5) {
    return Widget.checkBits(style, int0, int1, int2, int3, int4, int5);
}
public static void deregister (Widget w) {
    w.deregister_pp();
}
public static final Command[] getCommands(Control control) {
    return Widget.getCommands(control);
}
public static void hookEvents (Widget w) {
    w.hookEvents_pp();
}
public static String getPhoneNumber(Widget w) {
    return w.getPhoneNumber_pp();
}

public static boolean isSelectionKey(Widget w, int key, int character) {
    return w.isSelectionKey(key, character);
}
public static boolean isValidSubclass(Widget w) {
    return w.isValidSubclass();
}
public static boolean qt_event_keypress(Widget w, int widgetHandle, int key, int modifier, int character, int nativeScanCode) {
    return w.qt_event_keypress_pp(widgetHandle, key, modifier, character, nativeScanCode);
}
public static boolean qt_event_keyrelease(Widget w, int widgetHandle, int key, int modifier, int character, int nativeScanCode) {
    return w.qt_event_keyrelease_pp(widgetHandle, key, modifier, character, nativeScanCode);
}
public static boolean qt_event_mouseButtonDblClick(Widget w, int widgetHandle, int button, int x, int y, int state, int buttons ) {
    return w.qt_event_mouseButtonDblClick_pp(widgetHandle, button, x, y, state, buttons);
}
public static boolean qt_event_mouseButtonPress(Widget w, int widgetHandle, int button, int x, int y, int state, int buttons) {
    return w.qt_event_mouseButtonPress_pp(widgetHandle, button, x, y, state, buttons);
}
public static boolean qt_event_mouseButtonRelease(Widget w, int widgetHandle, int button, int x, int y, int state, int buttons) {
    return w.qt_event_mouseButtonRelease_pp(widgetHandle, button, x, y, state, buttons);
}
public static void qt_signal_qapplication_focusChanged(Widget w, int old, int now) {
    w.qt_signal_qapplication_focusChanged_pp(old, now);
}
public static void qt_signal_current_tab_changed(Widget w, int index) {
    w.qt_signal_current_tab_changed_pp(index);
}
public static void qt_signal_dateedit_dateTimeChanged(Widget w) {
    w.qt_signal_dateedit_dateTimeChanged_pp();
}
public static void qt_signal_linkActivated(Widget w, String link) {
    w.qt_signal_linkActivated_pp(link);
}
public static void qt_signal_list_itemActivated(Widget w, int selectionHandle) {
    w.qt_signal_list_itemActivated_pp(selectionHandle);
}
public static void qt_signal_list_itemDoubleClicked(Widget w, int selectionHandle) {
    w.qt_signal_list_itemDoubleClicked_pp(selectionHandle);
}
public static void qt_signal_list_itemSelectionChanged(Widget w) {
    w.qt_signal_list_itemSelectionChanged_pp();
}

public static void qt_signal_requestCompleted(Widget w, int value) {
    w.qt_signal_requestCompleted_pp(value);
}

public static void qt_signal_return_pressed(Widget w) {
    w.qt_signal_return_pressed_pp();
}
public static void qt_signal_selectionChanged(Widget w, int selectionHandle, int deSelectionHandle) {
    w.qt_signal_selectionChanged_pp(selectionHandle, deSelectionHandle);
}
public static void qt_signal_text_changed(Widget w, String text) {
    w.qt_signal_text_changed_pp(text);
}
public static void qt_signal_text_cursorPositionChanged(Widget w, int oldPos, int newPos) {
    w.qt_signal_text_cursorPositionChanged_pp(oldPos, newPos);
}
public static void qt_signal_text_selectionChanged(Widget w) {
    w.qt_signal_text_selectionChanged_pp();
}
public static void qt_signal_triggered(Widget w, boolean checked) {
    w.qt_signal_triggered_pp(checked);
}
public static void qt_signal_abstractitemview_activated(Widget w, int row, int column) {
    w.qt_signal_abstractitemview_activated_pp(row, column);
}
public static void qt_swt_event_widgetResized(Widget w, int widgetHandle, int oldWidth, int oldHeight, int width, int height) {
    w.qt_swt_event_widgetResized_pp(widgetHandle, oldWidth, oldHeight, width, height);
}
public static void register(Widget w) {
    w.register_pp();
}
public static void releaseChildren(Widget w, boolean destroy) {
    w.releaseChildren_pp(destroy);
}
public static void releaseHandle(Widget w) {
    w.releaseHandle_pp();
}
public static void releaseParent(Widget w) {
    w.releaseParent_pp();
}
public static void releaseWidget(Widget w) {
    w.releaseWidget_pp();
}
public static void removeCommand(Control control, final Command command) {
    control.removeCommand(command);
}
public static Widget removeWidget (Widget w, int handle) {
    return w.removeWidget(handle);
}
public static void sendEvent(Widget w, int eventType) {
    w.sendEvent(eventType);
}
public static void sendEvent(Widget w, int eventType, Event event) {
    w.sendEvent(eventType, event);
}
public static boolean sendKeyEvent(Widget w, int type, int key, int modifier, int character, int nativeScanCode) {
    return w.sendKeyEvent(type, key, modifier, character, nativeScanCode);
}
/*
 * From the class Control
 */
public static int extraStyle(Control c) {
    return c.extraStyle;
}
public static void setExtraStyle(Control c, int extraStyle) {
    c.extraStyle = extraStyle;
}
public static void setTraverseDoIt(Control c, boolean doIt) {
    c.traverseDoit = doIt;
}
public static void setTraverseCancel(Control c, boolean cancel) {
    c.traverseCancel = cancel;
}
public static void backgroundImageApplied(Control c, Image image) {
    c.backgroundImageApplied_pp(image);
}
public static void checkBorder(Control c) {
    c.checkBorder_pp();
}
public static boolean _isFocusControl(Control c) {
    return c._isFocusControl_pp();
}
public static int[] getBackgroundImageRoles(Control c) {
    return c.getBackgroundImageRoles();
}
public static boolean hasFocus(Control c) {
    return c.hasFocus();
}
public static boolean isShowing(Control c) {
    return c.isShowing();
}
public static Point mapToClientArea(Control c, int widgetHandle, int x, int y) {
    return c.mapToClientArea(widgetHandle, x, y);
}
public static boolean sendMouseEvent(Control c, int type, int button, boolean send, int x, int y,
        int modifiers, int buttons) {
    return c.sendMouseEvent(type, button, send, x, y, modifiers, buttons);
}
public static void setBackground(Control c) {
    c.setBackground_pp();
}
public static boolean setTabGroupFocus(Control c) {
    return c.setTabGroupFocus_pp();
}
public static void setTraversalFlags(Control c, int type, int key, int modifier, int character) {
    c.setTraversalFlags_pp(type, key, modifier, character);
}
/*
 * From the class Scrollable
 */
public static int scrollAreaHandle(Scrollable s) {
    return s.scrollAreaHandle;
}
public static void setScrollAreaHandle(Scrollable s, int handle) {
    s.scrollAreaHandle = handle;
}
public static void setVerticalBar(Scrollable s, ScrollBar vbar) {
    s.verticalBar = vbar;
}
public static void setHorizontalBar(Scrollable s, ScrollBar hbar) {
    s.horizontalBar = hbar;
}
public static ScrollBar createScrollBar(Scrollable s, int style) {
    return s.createScrollBar(style);
}
public static Point getPreferredClientAreaSize(Scrollable s) {
    return s.getPreferredClientAreaSize_pp();
}
/*
 * From the class Composite
 */
public static void addControl (Composite c, Control control) {
    c.addControl_pp(control);
}
public static int handleWithChildren(Composite c) {
    return c.handleWithChildren_pp();
}
public static Control [] _getChildren(Composite c) {
    return c._getChildren();
}
public static void removeControl(Composite c, Control control) {
    c.removeControl_pp(control);
}
/*
 * From the class Decorations
 */
public static boolean hasState(Decorations d, int state) {
    return d.hasState(state);
}
public static void setRestoreState(Decorations d, int state, boolean restore) {
    d.setRestoreState(state, restore);
}
/*
 * From the class Shell
 */
public static int checkStyle (int style, Composite parent) {
    return Shell.checkStyle(style, parent);
}
public static WindowSurface getWindowsurface(Shell s) {
    return s.getWindowSurface();
}
public static int windowFlagsFromStyle(Shell s) {
    return s.windowFlagsFromStyle();
}
/*
 * From the class Table
 */
public static boolean getWordWrap(Table t) {
    return t.getWordWrap();
}
public static void setWordWrap(Table t, boolean on) {
    t.setWordWrap(on);
}
public static int getFocusIndex(Table t) {
    return t.getFocusIndex();
}
public static void setFocusIndex(Table t, int index) {
    t.setFocusIndex(index);
}
/*
 * From the class Dialog
 */
public static void checkParent (Dialog d, Shell parent) {
    d.checkParent(parent);
}
public static int checkStyle (Shell parent, int style) {
    return Dialog.checkStyle(parent, style);
}
public static int style(Dialog d) {
    return d.style;
}
/*
 * From the class Text
 */
public static int variant(Text t) {
    return t.variant;
}
public static void setVariant(Text t, int variant) {
    t.variant = variant;
}
/*
 * From the class Display
 */
public static Display getDisplayInstance() {
    return Display.Default;
}
public static Display getInternalDisplayInstance() {
    return Display.Internal;	
}
public static Display internalInstance() {
	return Display.internalInstance();
}
public static Widget getDWidget(Display display, int widgetHandle) {
    return Display.getWidget(widgetHandle);
}
public static int initializeMobileDevice(Display display) {
    return Display.initializeMobileDevice();
}
public static int handle(Display d) {
    return Display.handle;
}
public static Point readAndClearImageLoadSize(Display d) {
	return Display.readAndClearImageLoadSize();
}
public static CommandArranger commandArranger() {
	return Display.commandArranger;
}
public static int untranslateKey (int key) {
	return Display.untranslateKey(key);
}
/*
 * From the class Label
 */
public static void setStandardIcon(Label l, int iconType, int iconWidth, int iconHeight) {
    l.setStandardIcon(iconType, iconWidth, iconHeight);
}

public static void setMobileDevice(Display display, MobileDevice mobileDevice){
	Display.mobileDevice = mobileDevice;
}

}
