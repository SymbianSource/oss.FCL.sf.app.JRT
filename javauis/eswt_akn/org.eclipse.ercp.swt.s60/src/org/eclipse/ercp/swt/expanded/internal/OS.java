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
package org.eclipse.ercp.swt.expanded.internal;


import org.eclipse.swt.browser.*;
import org.eclipse.swt.dnd.*;
import org.eclipse.swt.graphics.*;
import org.eclipse.swt.widgets.*;

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
     * Class Clipboard
     */
    public static final native int Clipboard_New(Clipboard peer);
    public static final native void Clipboard_Dispose(int clipboardHandle);
    public static final native void Clipboard_Clear(int clipboardHandle);
    public static final native int[] Clipboard_GetAvailableTypes(int clipboardHandle, int[] types);
    public static final native Object Clipboard_GetContents(int clipboardHandle, int typeId);
    public static final native void Clipboard_SetContents(int clipboardHandle, int typeId, Object object);

    /*
     * Class Table
     */
    public static final native int Table_New(Table peer, int parentComposite, int style);
    public static final native TableItem[] Table_GetTableItems(int handle);
    public static final native TableColumn Table_GetColumn(int handle, int index);
    public static final native TableColumn[] Table_GetTableColumns(int handle);
    public static final native TableItem[] Table_GetSelection(int handle);
    public static final native int Table_CompositeHandle(int handle);
    public static final native void Table_Deselect(int handle, int start, int end);
    public static final native void Table_Deselect(int handle, int [] indices);
    public static final native int Table_IndexOfColumn(int handle, int columnHandle);
    public static final native int Table_IndexOfItem(int handle, int itemHandle, int start);
    public static final native int Table_IndexOfItem(int handle, int itemHandle);
    public static final native boolean Table_GetHeaderVisible(int handle);
    public static final native boolean Table_GetLinesVisible(int handle);
    public static final native int Table_GetTopIndex(int handle);
    public static final native void Table_Remove(int handle, int index);
    public static final native void Table_Remove(int handle, int start, int end);
    public static final native void Table_Remove(int handle, int [] indices);
    public static final native void Table_RemoveAll(int handle);
    public static final native void Table_Select(int handle, int index, boolean aScroll);
    public static final native boolean Table_IsSelected(int handle, int index);
    public static final native void Table_Select(int handle, int start, int end);
    public static final native void Table_SelectAll(int handle);
    public static final native void Table_SetFocusIndex(int handle, int index);
    public static final native void Table_SetHeaderVisible(int handle, boolean show);
    public static final native void Table_SetLinesVisible(int handle, boolean show);
    public static final native void Table_SetTopIndex(int handle, int index);
    public static final native void Table_ShowSelection(int handle);
    public static final native int Table_GetColumnCount(int handle);
    public static final native TableItem Table_GetItem(int handle, int index);
    public static final native int Table_GetItemCount(int handle);
    public static final native int Table_GetSelectionCount(int handle);
    public static final native int[] Table_GetSelectionIndices(int handle);
    public static final native int Table_GetSelectionIndex(int handle);
    public static final native int Table_GetGridLineWidth(int handle);
    public static final native int Table_GetHeaderHeight(int handle);
    public static final native void Table_ShowColumn(int handle, int columnHandle);
    public static final native void Table_ShowItem(int handle, int columnItem);
    public static final native TableItem Table_GetItemFromPoint(int handle, int x, int y);
    public static final native int Table_GetItemHeight(int handle);

    /*
     * Class TableColumn
     */
    public static final native int TableColumn_New(TableColumn peer, int parentHandle, int style, int index);
    public static final native void TableColumn_Dispose(int handle);
    public static final native int TableColumn_GetAlignment(int tableColumnHandle);
    public static final native int TableColumn_GetWidth(int tableColumnHandle);
    public static final native void TableColumn_Pack(int tableColumnHandle);
    public static final native void TableColumn_SetAlignment(int tableColumnHandle, int alignment);
    public static final native String TableColumn_GetText(int handle);
    public static final native void TableColumn_SetText(int handle, String string);
    public static final native void TableColumn_SetWidth(int handle, int width);
    public static final native void TableColumn_SetImage(int handle, int imageHandle);
    public static final native boolean TableColumn_GetResizable(int handle);
    public static final native void TableColumn_SetResizable(int handle, boolean resizable);
    public static final native int TableColumn_GetStyle(int handle);

    /*
     * Class TableItem
     */
    public static final native int TableItem_New(TableItem peer, int parentHandle,int style, int index);
    public static final native void TableItem_Dispose(int handle);
    public static final native int TableItem_GetBackground(int handle);
    public static final native int TableItem_GetBackground(int handle, int index);
    public static final native Rectangle TableItem_GetBounds(int tableItemHandle, int index);
    public static final native boolean TableItem_GetChecked(int handle);
    public static final native int TableItem_GetFont(int handle);
    public static final native int TableItem_GetFont(int handle, int index);
    public static final native int TableItem_GetForeground(int handle);
    public static final native int TableItem_GetForeground(int handle, int index);
    public static final native boolean TableItem_GetGrayed(int handle);
    public static final native Rectangle TableItem_GetImageBounds(int tableItemHandle, int index);
    public static final native String TableItem_GetText(int handle, int index);
    public static final native void TableItem_SetBackground(int handle, int colorHandle);
    public static final native void TableItem_SetBackground(int handle, int colorHandle, int index);
    public static final native void TableItem_SetChecked(int handle, boolean checked);
    public static final native void TableItem_SetFont(int handle, int fontHandle);
    public static final native void TableItem_SetFont(int handle, int fontHandle, int index);
    public static final native void TableItem_SetForeground(int handle, int colorHandle);
    public static final native void TableItem_SetForeground(int handle, int colorHandle, int index);
    public static final native void TableItem_SetGrayed(int handle, boolean grayed);
    public static final native void TableItem_SetImage(int handle, int[] imageHandles);
    public static final native void TableItem_SetImage(int handle, int index, int imageHandle);
    public static final native void TableItem_SetText(int handle, String[] strings);
    public static final native void TableItem_SetText(int handle, String string, int index);

    /*
     * Class ColorDialog
     */
    public static final native void ColorDialog_Open(ColorDialog peer, RGB defaultValue);

    /*
     * Class FontDialog
     */
    public static final native String FontDialog_GetCommandOkTitle();
    public static final native String FontDialog_GetCommandCancelTitle();
    public static final native String[] FontDialog_GetFontNames(boolean scalable);

    /*
     * Class DirectoryDialog
     */
    public static final native void DirectoryDialog_Open(DirectoryDialog peer, String filterPath, String title);

    /*
     * Class Browser
     */
    public static final native int Browser_CompositeHandle(int handle);
    public static final native boolean Browser_ExistsInstanceAlready();
    public static final native int Browser_New(Browser peer, int parentComposite, int style);
    public static final native boolean Browser_Back(int handle);
    public static final native boolean Browser_Forward(int handle);
    public static final native String Browser_GetUrl(int handle);
    public static final native boolean Browser_IsBackEnabled(int handle);
    public static final native boolean Browser_IsForwardEnabled(int handle);
    public static final native void Browser_Refresh(int handle);
    public static final native boolean Browser_SetText(int handle,String html);
    public static final native boolean Browser_SetUrl(int handle,String url);
    public static final native void Browser_Stop(int handle);

    /*
     * Class Tree
     */
    public static final native int Tree_New(Tree peer, int parentComposite, int style);
    public static final native int Tree_CompositeHandle(int handle);
    public static final native int Tree_Add(int handle, int itemHandle);
    public static final native Rectangle Tree_Bounds(int handle, int itemHandle);
    public static final native void Tree_Check(int handle, int itemHandle, boolean state);
    public static final native void Tree_Collapse(int handle, int itemHandle);
    public static final native void Tree_Expand(int handle, int itemHandle);
    public static final native int Tree_Insert(int handle, int itemHandle, int index);
    public static final native boolean Tree_IsExpanded(int handle, int itemHandle);
    public static final native boolean Tree_IsSelected(int handle, int itemHandle);
    public static final native int Tree_ItemAt(int handle, int x, int y);
    public static final native int Tree_ItemHeight(int handle);
    public static final native void Tree_Remove(int handle, int itemHandle);
    public static final native void Tree_RemoveAll(int handle);
    public static final native void Tree_Select(int handle, int[] itemHandles, boolean state);
    public static final native void Tree_SelectAll(int handle, boolean state);
    public static final native int[] Tree_Selection(int handle);
    public static final native int Tree_SelectionCount(int handle);
    public static final native void Tree_SetImage(int handle, int itemHandle, int imageHandle);
    public static final native void Tree_SetText(int handle, int itemHandle, String text);
    public static final native int Tree_TopItem(int handle);
}
