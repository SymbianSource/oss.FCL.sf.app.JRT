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

import org.eclipse.ercp.swt.mobile.TextExtension;
import org.eclipse.swt.graphics.Image;
import org.eclipse.swt.graphics.Point;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Control;
import org.eclipse.swt.widgets.Internal_PackageSupport;
import org.eclipse.swt.widgets.Scrollable;
import org.eclipse.swt.widgets.Widget;

/*
 * Each widget outside the 'widgets' package that needs to override package 
 * protected functionality from the 'widgets' package implements an instance of
 * this class. The methods if this class can be overridden to be called instead
 * of the corresponding methods in the 'widgets' package. The default 
 * implementations call the base class implementation in the 'widgets' package.
 */
public abstract class PackageProxy {

protected Widget w;

/*
 * Sets the widget that this proxy will be calling. 
 */
public void setWidget(Widget w) {
    this.w = w;
}

/*
 * From the class Widget
 */
public void createHandle(int index) {
    Internal_PackageSupport.createHandle(w, index);
}
public void deregister () {
    Internal_PackageSupport.deregister(w);
}
public void hookEvents () {
    Internal_PackageSupport.hookEvents(w);
}
public boolean qt_event_keypress( int widgetHandle, int key, int modifier, int character, int nativeScanCode ) {
    return Internal_PackageSupport.qt_event_keypress(w, widgetHandle, key, modifier, character, nativeScanCode);
}
public boolean qt_event_keyrelease(int widgetHandle, int key, int modifier, int character, int nativeScanCode) {
    return Internal_PackageSupport.qt_event_keyrelease(w, widgetHandle, key, modifier, character, nativeScanCode);
}
public boolean qt_event_mouseButtonDblClick( int widgetHandle, int button, int x, int y, int state, int buttons ) {
    return Internal_PackageSupport.qt_event_mouseButtonDblClick(w, widgetHandle, button, x, y, state, buttons);
}
public boolean qt_event_mouseButtonPress (int widgetHandle, int button, int x, int y, int state, int buttons) {
    return Internal_PackageSupport.qt_event_mouseButtonPress(w, widgetHandle, button, x, y, state, buttons);
}
public boolean qt_event_mouseButtonRelease(int widgetHandle, int button, int x, int y, int state, int buttons) {
    return Internal_PackageSupport.qt_event_mouseButtonRelease(w, widgetHandle, button, x, y, state, buttons);
}
public void qt_signal_qapplication_focusChanged(int old, int now) {
    Internal_PackageSupport.qt_signal_qapplication_focusChanged(w, old, now);
}
public void qt_signal_current_tab_changed(int index) {
    Internal_PackageSupport.qt_signal_current_tab_changed(w, index);
}
public void qt_signal_dateedit_dateTimeChanged() {
    Internal_PackageSupport.qt_signal_dateedit_dateTimeChanged(w);
}
public void qt_signal_linkActivated(String link) {
    Internal_PackageSupport.qt_signal_linkActivated(w, link);
}
public void qt_signal_list_itemActivated(int selectionHandle) {
    Internal_PackageSupport.qt_signal_list_itemActivated(w, selectionHandle);
}
public void qt_signal_list_itemDoubleClicked(int selectionHandle) {
    Internal_PackageSupport.qt_signal_list_itemDoubleClicked(w, selectionHandle);
}
public void qt_signal_list_itemSelectionChanged() {
    Internal_PackageSupport.qt_signal_list_itemSelectionChanged(w);
}

public void qt_signal_requestCompleted(int value) {
    Internal_PackageSupport.qt_signal_requestCompleted(w, value);
}

public void qt_signal_return_pressed() {
    Internal_PackageSupport.qt_signal_return_pressed(w);
}
public void qt_signal_selectionChanged(int selectionHandle, int deSelectionHandle) {
    Internal_PackageSupport.qt_signal_selectionChanged(w, selectionHandle, deSelectionHandle);
}
public void qt_signal_text_changed(String text) {
    Internal_PackageSupport.qt_signal_text_changed(w, text);
}
public void qt_signal_text_cursorPositionChanged(int oldPos, int newPos) {
    Internal_PackageSupport.qt_signal_text_cursorPositionChanged(w, oldPos, newPos);
}
public void qt_signal_text_selectionChanged() {
    Internal_PackageSupport.qt_signal_text_selectionChanged(w);
}
public void qt_signal_triggered(boolean checked) {
    Internal_PackageSupport.qt_signal_triggered(w, checked);
}
public void qt_signal_abstractitemview_activated(int row, int column) {
    Internal_PackageSupport.qt_signal_abstractitemview_activated(w, row, column);
}
public void qt_swt_event_widgetResized(int widgetHandle, int oldWidth, int oldHeight, int width, int height) {
    Internal_PackageSupport.qt_swt_event_widgetResized(w, widgetHandle, oldWidth, oldHeight, width, height);
}
public void register() {
    Internal_PackageSupport.register(w);
}
public void releaseChildren (boolean destroy) {
    Internal_PackageSupport.releaseChildren(w, destroy);
}
public void releaseHandle() {
    Internal_PackageSupport.releaseHandle(w);
}
public void releaseParent () {
    Internal_PackageSupport.releaseParent(w);
}
public void releaseWidget () {
    Internal_PackageSupport.releaseWidget(w);
}

/*
 * From the class Control
 */
public void backgroundImageApplied(Image image) {
    Internal_PackageSupport.backgroundImageApplied((Control)w, image);
}
public void checkBorder() {
    Internal_PackageSupport.checkBorder((Control)w);
}
public boolean _isFocusControl() {
    return Internal_PackageSupport._isFocusControl((Control)w);
}
public void setBackground() {
    Internal_PackageSupport.setBackground((Control)w);
}
public boolean setTabGroupFocus() {
    return Internal_PackageSupport.setTabGroupFocus((Control)w);
}
public void setTraversalFlags(int type, int key, int modifier, int character) {
    Internal_PackageSupport.setTraversalFlags((Control)w, type, key, modifier, character);
}

/*
 * From the class Scrollable
 */
public Point getPreferredClientAreaSize() {
    return Internal_PackageSupport.getPreferredClientAreaSize((Scrollable)w);
}

/*
 * From the class Composite
 */
public void addControl(Control control) {
    Internal_PackageSupport.addControl((Composite)w, control);
}
public int handleWithChildren() {
    return Internal_PackageSupport.handleWithChildren((Composite)w);
}
public void removeControl(Control control) {
    Internal_PackageSupport.removeControl((Composite)w, control);
}

/*
 * From the class TextExtension
 */
public String getPhoneNumber() {
    return Internal_PackageSupport.getPhoneNumber((TextExtension)w);
}



}
