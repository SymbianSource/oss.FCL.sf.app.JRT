/*******************************************************************************
 * Copyright (c) 2000, 2005 IBM Corporation and others.
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *
 * Contributors:
 *     IBM Corporation - initial API and implementation
 *     Nokia Corporation - S60 implementation
 *******************************************************************************/
package org.eclipse.ercp.swt.mobile.internal;


import org.eclipse.ercp.swt.mobile.*;
import org.eclipse.ercp.swt.mobile.MultiPageDialog.*;
import org.eclipse.swt.graphics.*;
import org.eclipse.swt.widgets.Shell;


/**
 * @brief This is the place where all native functions are located.
 *
 * This class is not supposed to be instanciated as it only contains static
 * methods. <b>Warning</b>: the content of this class is platform dependent.
 */
public class OS
{

    // Load the DLL needed for the JNIs
    /*
    static {
      org.eclipse.swt.internal.Library.loadLibrary("eswt");
    }
    */

    /*
     * Class MobileShell
     */
    public static final native int MobileShell_New(MobileShell peer, int parentShell, int style);
    public static final native void MobileShell_ChangeTrim(int handle, int style, int statusStyle);
    public static final native boolean MobileShell_GetFullScreenMode(int handle);
    public static final native boolean MobileShell_GetKeyState(int handle, int keyCode);
    public static final native int MobileShell_GetShellHandle(int handle);
    public static final native void MobileShell_SetFullScreenMode(int handle, boolean isFullScreen);
    public static final native void MobileShell_SetStatusText(int handle, String statusText);
    public static final native int MobileDevice_New(MobileDevice peer);
    public static final native void MobileDevice_Dispose(int handle);
    public static final native boolean MobileDevice_IsOpen(int handle);
    public static final native void MobileDevice_Alert(int handle, int level);
    public static final native int MobileDevice_GetInputNumber(int handle);
    public static final native int MobileDevice_GetScreenNumber(int handle);

    /*
     * Class Input
     */
    public static final native int Input_New(Input peer, int id);
    public static final native void Input_Dispose(int handle);
    public static final native int Input_GetLocation(int handle);
    public static final native int Input_GetType(int handle);

    /*
     * Class Screen
     */
    public static final native int Screen_New(Screen peer, int id);
    public static final native void Screen_Dispose(int handle);
    public static final native Rectangle Screen_GetBounds(int handle);
    public static final native int Screen_GetColorDepth(int handle);
    public static final native int Screen_GetLocation(int handle);
    public static final native int Screen_GetOrientation(int handle);
    public static final native void Screen_SetOrientation(int handle, int orientation);
    public static final native int Screen_GetUsage(int handle);
    public static final native boolean Screen_IsActive(int handle);
    public static final native boolean Screen_IsTouchScreen(int handle);

    /*
     * Class MultiPageDialog
     */
    public static final native int MultiPageDialog_New(internal_MultiPageDialogControl peer, int parentHandle);
    public static final native int MultiPageDialog_ControlHandle(int handle);
    public static final native void MultiPageDialog_Close(int handle);
    public static final native void MultiPageDialog_CreatePage(int handle, int compositeHandle, String title, int imageHandle);
    public static final native void MultiPageDialog_DeletePage(int handle, int index);
    public static final native int MultiPageDialog_GetSelectedIndex(int handle);
    public static final native void MultiPageDialog_Open(int handle);
    public static final native void MultiPageDialog_SetSelection(int handle, int index);
    public static final native void MultiPageDialog_SetTitleAndImage(int handle, int index, String text, int imageHandle);
    public static final native void MultiPageDialog_SetTitle(int handle, int index, String Title);

    /*
     * Class CaptionedControl
     */
    public static final native int CaptionedControl_New(CaptionedControl peer, int parentComposite, int style);
    public static final native int CaptionedControl_CompositeHandle(int handle);
    public static final native void CaptionedControl_SetText(int handle, String string);
    public static final native void CaptionedControl_SetTrailingText(int handle, String string);
    public static final native String CaptionedControl_GetText(int handle);
    public static final native String CaptionedControl_GetTrailingText(int handle);
    public static final native void CaptionedControl_SetImage(int handle, int imageHandle);
    public static final native void CaptionedControl_SetChildPreferredSize(int handle, int width, int height);

    /*
     * Class Command
     */
    public static final native int     Command_New(Command peer, int parentControl, int parentCommand, int type, int priority);
    public static final native void    Command_Dispose(int handle);
    public static final native void    Command_SetAccelerator(int handle, int accelerator);
    public static final native boolean Command_IsDefaultCommand(int handle);
    public static final native void    Command_SetDefaultCommand(int handle);
    public static final native void    Command_SetEnabled(int handle, boolean enabled);
    public static final native void    Command_SetText(int handle, String string);
    public static final native void    Command_SetImage(int handle, int image);
    public static final native void    Command_SetLongLabel(int handle, String string);

    /*
     * Class ConstrainedText
     */
    public static final native int ConstrainedText_New(ConstrainedText peer, int parentComposite, int style);
    public static final native void ConstrainedText_SetConstraint(int handle, int constraint);
    public static final native int ConstrainedText_ControlHandle(int handle);
    public static final native String ConstrainedText_GetText(int handle);
    public static final native boolean ConstrainedText_SetText(int handle, String text);
    public static final native boolean ConstrainedText_Append(int handle, String text);
    public static final native boolean ConstrainedText_Insert(int handle, String text);
    public static final native int ConstrainedText_GetCharCount(int handle);
    public static final native boolean ConstrainedText_SetTextLimit(int handle, int limit);
    public static final native int ConstrainedText_GetCaretPosition(int handle);
    public static final native void ConstrainedText_SetSelection(int handle, int start, int end);
    public static final native void ConstrainedText_ClearSelection(int handle);
    public static final native void ConstrainedText_SelectAll(int handle);
    public static final native void ConstrainedText_ShowSelection(int handle);
    public static final native boolean ConstrainedText_Cut(int handle);
    public static final native void ConstrainedText_Copy(int handle);
    public static final native boolean ConstrainedText_Paste(int handle);
    public static final native boolean ConstrainedText_GetEditable(int handle);
    public static final native void ConstrainedText_SetEditable(int handle, boolean editable);

    /*
     * Class HyperLink
     */
    public static final native int HyperLink_New(HyperLink peer, int parentComposite, int style);
    public static final native int HyperLink_ControlHandle(int handle);
    public static final native void HyperLink_SetText(int handle, String text);
    public static final native String HyperLink_GetText(int handle);
    public static final native void HyperLink_SetFormat(int handle, int format);

    /*
     * Class ListBox
     */
    public static final native int ListBox_New(ListBox peer, int parentComposite, int style);
    public static final native void ListBox_SetLayoutStyle(int handle, int layoutStyle);
    public static final native int ListBox_ScrollableHandle(int handle);
    public static final native void ListBox_Deselect(int handle, int index);
    public static final native void ListBox_Deselect(int handle, int start, int end);
    public static final native void ListBox_Deselect(int handle, int [] indices);
    public static final native void ListBox_DeselectAll(int handle);
    public static final native int ListBox_GetFocusIndex(int handle);
    public static final native int ListBox_GetSelectionCount(int handle);
    public static final native int[] ListBox_GetSelectionIndices(int handle);
    public static final native void ListBox_RefreshItem(int handle, int index, ListBoxItem item);
    public static final native void ListBox_RefreshList(int handle, ListBoxItem[] items);
    public static final native void ListBox_Select(int handle, int index, boolean scroll);
    public static final native void ListBox_Select(int handle, int start, int end);
    public static final native void ListBox_SelectAll(int handle);
    public static final native void ListBox_SetFocusIndex(int handle, int index);
    public static final native void ListBox_SetDataModel(int handle, ListBoxItem[] items);
    public static final native void ListBox_SetHeadingFont(int handle, int fontHandle);
    public static final native void ListBox_ShowSelection(int handle);

    /*
     * Class DateEditor
     */
    public static final native int DateEditor_New(DateEditor peer, int parentComposite, int style);
    public static final native int[] DateEditor_GetDate(int handle);
    public static final native int DateEditor_GetOffset(int handle);
    public static final native int DateEditor_Handle(int handle);
    public static final native void DateEditor_SetDate(int handle, int high32OfDate, int low32OfDate);
    public static final native void DateEditor_SetOffset(int handle, int seconds);
    public static final native void DateEditor_SetDateEditorStyle(int handle, int dateEditorStyle);

    /*
     * Class SortedList
     */
    public static final native int SortedList_New(SortedList peer, int parentComposite, int style);
    public static final native int SortedList_ScrollableHandle(int handle);
    public static final native void SortedList_Add(int handle, String item);
    public static final native String SortedList_GetFocusedItem(int handle);
    public static final native int SortedList_GetItemHeight(int handle);
    public static final native String[] SortedList_GetSelection(int handle);
    public static final native int SortedList_GetSelectionCount(int handle);
    public static final native boolean SortedList_Remove(int handle, String item);
    public static final native void SortedList_RemoveAll(int handle);
    public static final native boolean SortedList_Select(int handle, String item);
    public static final native void SortedList_SetItems(int handle, String[] items);
    public static final native void SortedList_SetModeStyle(int handle, int modeStyle);
    public static final native void SortedList_SetSelection(int handle, String[] items);
    public static final native void SortedList_ShowSelection(int handle);

    /*
     * Class TimedMessageBox
     */
    public static final native void TimedMessageBox_Open(TimedMessageBox peer, String text, String message, int style, int handle);

    /*
     * Class QueryDialog
     */
    public static final native void QueryDialog_Open(QueryDialog peer, String title, int style, int queryType, String promptText, String defaultValue, int minimum, int maximum);
    public static final native String QueryDialog_ConvertDefaultValue(String date, int queryType);

    /*
     * Class TaskTip
     */
    public static final native Rectangle TaskTip_DefaultBounds();

    /*
     * Class TextExtension
     */
    public static final native int TextExtension_New(TextExtension peer, int parent, int style);
    public static final native int TextExtension_TextHandle(int handle);
    public static final native void TextExtension_SetTypeStyle(int handle, int typeStyle);
    public static final native void TextExtension_SetInitialInputMode(int handle, int casingModifier, String mode);

    /*
     * Class ListView
     */
    public static final native int ListView_New(ListView peer, int parentComposite, int style);
    public static final native int ListView_ScrollableHandle(int handle);
    public static final native void ListView_Add(int handle, String string, int iconHandle);
    public static final native void ListView_Add(int handle, String string, int iconHandle, int index);
    public static final native void ListView_Deselect(int handle, int index);
    public static final native void ListView_Deselect(int handle, int start, int end);
    public static final native void ListView_Deselect(int handle, int [] indices);
    public static final native void ListView_DeselectAll(int handle);
    public static final native int ListView_GetFocusIndex(int handle);
    public static final native String ListView_GetItem(int handle, int index);
    public static final native Point ListView_GetIconSize(int handle, int style);
    public static final native int ListView_GetItemCount(int handle);
    public static final native String[] ListView_GetItems(int handle);
    public static final native String[] ListView_GetSelection(int handle);
    public static final native int ListView_GetSelectionCount(int handle);
    public static final native int[] ListView_GetSelectionIndices(int handle);
    public static final native int ListView_GetTopIndex(int handle);
    public static final native int ListView_IndexOf(int handle, String string, int start);
    public static final native boolean ListView_IsSelected(int handle, int index);
    public static final native void ListView_Remove(int handle, int index);
    public static final native void ListView_Remove(int handle, int start, int end);
    public static final native void ListView_Remove(int handle, int [] indices);
    public static final native void ListView_RemoveAll(int handle);
    public static final native void ListView_Select(int handle, int index, boolean scroll);
    public static final native void ListView_Select(int handle, int start, int end);
    public static final native void ListView_SelectAll(int handle);
    public static final native void ListView_SetFocusIndex(int handle, int index);
    public static final native void ListView_SetItems(int handle, String [] items, int[] iconsHandles);
    public static final native void ListView_SetLayout(int handle, int style);
    public static final native void ListView_SetLayoutDensity(int handle, int density);
    public static final native void ListView_ShowSelection(int handle);
}

