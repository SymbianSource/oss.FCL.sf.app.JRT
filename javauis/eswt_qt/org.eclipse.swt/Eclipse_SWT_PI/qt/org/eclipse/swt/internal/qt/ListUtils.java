/*******************************************************************************
 * Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
 * All rights reserved. This program and the accompanying materials 
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 * 
 * Contributors:
 *     Nokia Corporation - initial implementation 
 *******************************************************************************/
package org.eclipse.swt.internal.qt;

import java.util.Vector;
import java.lang.Integer;

import org.eclipse.swt.SWT;
import org.eclipse.swt.graphics.Image;
import org.eclipse.swt.graphics.Internal_GfxPackageSupport;

public final class ListUtils extends Object {
/**
 * @see ListView.add
 */
public static void add(int handle, String item, Image icon) {
    if (item == null) {
        SWT.error(SWT.ERROR_NULL_ARGUMENT);
    }
    // OS may throw SWT.ERROR_ITEM_NOT_ADDED
    OS.QListWidget_addItem(handle, item, 
            icon != null ? Internal_GfxPackageSupport.getIconHandle(icon) : 0);
}

/**
 * @see ListView.add
 */
public static void add(int handle, String item, Image icon, int index) {
    if (item == null) {
        SWT.error(SWT.ERROR_NULL_ARGUMENT);            
    }
    if (index < 0 || index > OS.QListWidget_count(handle)) { // index == count is ok too!
        SWT.error(SWT.ERROR_INVALID_RANGE);
    }
    // OS may throw SWT.ERROR_ITEM_NOT_ADDED
    OS.QListWidget_insertItem(handle, index, item, 
            icon != null ? Internal_GfxPackageSupport.getIconHandle(icon) : 0);
}

/**
 * @see List.deselect
 */
public static void deselect(int handle, int index) {
    if (index < 0 || index >= OS.QListWidget_count(handle)) {
        return;
    }
    OS.QListWidget_swt_selectIndex(handle, index, 
            OS.QT_ITEMSELECTIONMODEL_SELECTIONFLAGS_DESELECT);
}

/**
 * @see List.deselect
 */
public static void deselect(int handle, int[] indices) {
    if (indices == null) {
        SWT.error(SWT.ERROR_NULL_ARGUMENT);
    }
    int length = indices.length;
    if (length == 0) {
        return;
    }
    OS.QListWidget_swt_selectArray(handle, indices, length, 
            OS.QT_ITEMSELECTIONMODEL_SELECTIONFLAGS_DESELECT, false);
}

/**
 * @see List.deselect
 */
public static void deselect(int handle, int start, int end) {
    if (start > end || end < 0) {
        return;
    }
    int count = OS.QListWidget_count(handle);
    if (start > count - 1) {
        return;
    }
    start = Math.max(0, start);
    end = Math.min(end, count - 1);
    OS.QListWidget_swt_selectRange(handle, start, end, 
            OS.QT_ITEMSELECTIONMODEL_SELECTIONFLAGS_DESELECT);
}

/**
 * @see List.deselectAll
 */
public static void deselectAll(int handle) {
    OS.QListWidget_swt_selectRange(handle, 0, 
            OS.QListWidget_count(handle) - 1, 
            OS.QT_ITEMSELECTIONMODEL_SELECTIONFLAGS_DESELECT);
}

/**
 * @see List.getFocusIndex
 */
public static int getFocusIndex(int handle) {
    return OS.QListWidget_currentRow(handle);
}

/**
 * @see List.getItem
 */
public static String getItem(int handle, int index) {
    if (index < 0 || index >= OS.QListWidget_count(handle)) {
        SWT.error(SWT.ERROR_INVALID_RANGE);
    }
    // OS may throw SWT.ERROR_CANNOT_GET_ITEM
    String item = OS.QListWidget_swt_itemText(handle, index);
    if (item == null) {
        SWT.error(SWT.ERROR_CANNOT_GET_ITEM);
    }
    return item;
}

/**
 * @see List.getItemCount
 */
public static int getItemCount(int handle) {
    // OS may throw SWT.SWT.ERROR_CANNOT_GET_COUNT        
    return OS.QListWidget_count(handle);
}

/**
 * @see List.getItemHeight
 */
public static int getItemHeight(int handle) {
    int itemHeight = 0;
    if (OS.QListWidget_count(handle) > 0 ) {
        itemHeight = OS.QAbstractItemView_sizeHintForRow(handle, 0);
    } 
    else {
         ListUtils.add(handle, "", null);
         itemHeight = OS.QAbstractItemView_sizeHintForRow(handle, 0);
         ListUtils.remove(handle, 0);
    }
    return itemHeight;
}

/**
 * @see List.getItems
 */
public static String[] getItems(int handle) {
    String[] res = OS.QListWidget_swt_itemTexts(handle);
    if (res == null) {
        if (OS.QListWidget_count(handle) == 0) {
            return new String[0];
        }
        else {
            SWT.error(SWT.ERROR_CANNOT_GET_ITEM);
            return null;
        }
    }
    else {
        if (OS.QListWidget_count(handle) != res.length) {
            SWT.error(SWT.ERROR_CANNOT_GET_COUNT);
            return null;
        }
    }
    return res;
}

/**
 * @see List.getSelection
 */
public static String[] getSelection(int handle) {
    int[] indices = OS.QListWidget_swt_selectedRows(handle, 0);
    if (indices == null || indices.length <= 0) {
        return new String[] {};
    }
    sort(indices);
    String[] res = new String[indices.length];
    for (int i = 0; i < indices.length; i++) {
        res[i] = OS.QListWidget_swt_itemText(handle, indices[i]);
        if (res[i] == null) {
            SWT.error(SWT.ERROR_CANNOT_GET_ITEM);
        }
    }
    return res;
}

/**
 * @see List.getSelectionCount
 */
public static int getSelectionCount(int handle) {
    int[] indices = OS.QListWidget_swt_selectedRows(handle, 0);
    if (indices != null) {
        return indices.length;
    } 
    else {
        return 0;
    }
}

/**
 * @see List.getSelectionIndices
 */
public static int[] getSelectionIndices(int handle) {
    int[] indices = OS.QListWidget_swt_selectedRows(handle, 0);
    if (indices == null) {
        indices = new int[] {};
    }
    sort(indices);
    return indices;
}

/**
 * @see List.indexOf
 */
public static int indexOf(int handle, String string, int start) {
    if (string == null) {
        SWT.error(SWT.ERROR_NULL_ARGUMENT);
    }
    if (start < 0 || start > OS.QListWidget_count(handle) - 1) {
        return -1;
    }
    return OS.QListWidget_swt_indexOf(handle, string, start);
}

/**
 * @see List.isSelected
 */
public static boolean isSelected(int handle, int index) {
    if (index < 0 || index > OS.QListWidget_count(handle) - 1) {
        return false;
    }
    return OS.QListWidget_swt_isItemSelected(handle, index);
}

/**
 * @see List.remove
 */
public static void remove(int handle, int index) {
    if ( index < 0 || index >= OS.QListWidget_count(handle) ) {
        SWT.error(SWT.ERROR_INVALID_RANGE);
    }
    // OS may throw SWT.ERROR_ITEM_NOT_REMOVED
    OS.QListWidget_swt_removeIndex(handle, index);
}

/**
 * @see List.remove
 */
public static void remove(int handle, String string) {
    if (string == null) {
        SWT.error(SWT.ERROR_NULL_ARGUMENT);
    }
    int index = OS.QListWidget_swt_indexOf(handle, string, 0);
    if (index == -1) {
        SWT.error(SWT.ERROR_INVALID_ARGUMENT);
    }
    OS.QListWidget_swt_removeIndex(handle, index);
}

/**
 * @see List.remove
 */
public static void remove(int handle, int start, int end) {
    if (start > end) {
        return;
    }
    int count = OS.QListWidget_count(handle);
    if (start < 0 || end >= count || start > count) {
        SWT.error(SWT.ERROR_INVALID_RANGE);
    }
    // OS may throw SWT.ERROR_ITEM_NOT_REMOVED
    OS.QListWidget_swt_removeRange(handle, start, end);
}

/**
 * @see List.remove
 * @return sorted array of removed indices
 */
public static int[] remove(int handle, int[] indices) {
    if (indices == null) {
        SWT.error(SWT.ERROR_NULL_ARGUMENT);
    }
    if (indices.length == 0) {
        return indices;
    }
    int[] idxs = sortAndRemoveDuplicates(indices);
    int count = OS.QListWidget_count(handle);
    if ( idxs[0] >= count || idxs[0] < 0 || idxs[idxs.length - 1] >= count) {
        SWT.error(SWT.ERROR_INVALID_RANGE);
    }
    // OS may throw SWT.ERROR_ITEM_NOT_REMOVED
    OS.QListWidget_swt_removeArray(handle, idxs, idxs.length);
    return idxs;
}

/**
 * @see List.removeAll
 */
public static void removeAll(int handle) {
    OS.QListWidget_clear(handle);
}

/**
 * @see List.select
 */
public static void select(int handle, int index, boolean multi) {
    if (index < 0 || index >= OS.QListWidget_count(handle)) {
        return;
    }
    int command = OS.QT_ITEMSELECTIONMODEL_SELECTIONFLAGS_SELECT;
    if (!multi) {
        command |= OS.QT_ITEMSELECTIONMODEL_SELECTIONFLAGS_CLEAR;
    }
    OS.QListWidget_swt_selectIndex(handle, index, command);
    if (!multi) {
        OS.QListWidget_swt_setFocusIndex(handle, index);
    }
}

/**
 * @see SortedList.select
 */
public static void select(int handle, String string, boolean multi) {
    if (string == null) {
        ListUtils.deselectAll(handle);
        return;
    }
    int index = OS.QListWidget_swt_indexOf(handle, string, 0);
    if (index == -1) {
        SWT.error(SWT.ERROR_INVALID_ARGUMENT);
    }
    int command = OS.QT_ITEMSELECTIONMODEL_SELECTIONFLAGS_SELECT;
    if (!multi) {
        command |= OS.QT_ITEMSELECTIONMODEL_SELECTIONFLAGS_CLEAR;
    }
    OS.QListWidget_swt_selectIndex(handle, index, command);
    if (!multi) {
        OS.QListWidget_swt_setFocusIndex(handle, index);
    }
}

/**
 * @see List.select
 */
public static void select(int handle, int start, int end, boolean multi) {
    if (start > end || end < 0) {
        return;
    }
    if (start == end) {
        select(handle, start, multi);
        return;
    }
    if (!multi) {
        return;
    }
    int count = OS.QListWidget_count(handle);
    if (start > count - 1) {
        return;
    }
    start = Math.max(0, start);
    end = Math.min(end, count - 1);
    OS.QListWidget_swt_selectRange(handle, start, end, 
            OS.QT_ITEMSELECTIONMODEL_SELECTIONFLAGS_SELECT);
}

/**
 * @see List.select
 */
public static void select(int handle, int[] indices, boolean multi) {
    if (indices == null) {
        SWT.error(SWT.ERROR_NULL_ARGUMENT);
    }
    int length = indices.length;
    if (length == 1) {
        select(handle, indices[0], multi);
        return;
    }
    if (length == 0 || !multi) {
        return;
    }
    OS.QListWidget_swt_selectArray(handle, indices, length, 
            OS.QT_ITEMSELECTIONMODEL_SELECTIONFLAGS_SELECT, false);
}

/**
 * @see List.selectAll
 */
public static void selectAll(int handle, boolean multi) {
    int count = OS.QListWidget_count(handle);
    if ((!multi) && count != 1) {
        return;
    }
    OS.QListWidget_swt_selectRange(handle, 0, count - 1, 
            OS.QT_ITEMSELECTIONMODEL_SELECTIONFLAGS_SELECT);
}

/**
 * Internal API
 */
public static void setFocusIndex(int handle, int index, boolean multi) {
    OS.QListWidget_swt_setFocusIndex(handle, index);
    if (!multi) {
        int command = OS.QT_ITEMSELECTIONMODEL_SELECTIONFLAGS_SELECT;
        command |= OS.QT_ITEMSELECTIONMODEL_SELECTIONFLAGS_CLEAR;
        OS.QListWidget_swt_selectIndex(handle, index, command);
    }
}

/**
 * @see ListView.setItems
 */
public static void setItems(int handle, String[] items, Image[] icons) {
    if (items == null) {
        SWT.error(SWT.ERROR_NULL_ARGUMENT);
    }
    if (icons != null && icons.length != items.length) {
        SWT.error(SWT.ERROR_INVALID_RANGE);
    }
    removeAll(handle);
    int count = items.length;
    if (count <= 0) {
        return;
    }
    int[] handles = null;
    if (icons != null) {
        handles = new int[icons.length];
        for (int i = 0; i < icons.length; i++) {
            if (icons[i] != null) {
                handles[i] = Internal_GfxPackageSupport.getIconHandle(icons[i]);
            }
            else {
                handles[i] = 0;
            }
        }
    }
    // OS may throw SWT.ERROR_ITEM_NOT_ADDED
    OS.QListWidget_addItems(handle, items, handles);
}

/**
 * @see List.setSelection
 */
public static void setSelection(int handle, int index) {
    int count = OS.QListWidget_count(handle);
    // deselectAll()
    OS.QListWidget_swt_selectRange(handle, 0, count - 1, 
            OS.QT_ITEMSELECTIONMODEL_SELECTIONFLAGS_DESELECT);
    if (index < 0 || index > count - 1) {
        return;
    }
    // select(int)
    OS.QListWidget_swt_selectIndex(handle, index, 
            OS.QT_ITEMSELECTIONMODEL_SELECTIONFLAGS_SELECT);
    OS.QListWidget_scrollToItem(handle, index, OS.QT_ABSTRACTITEMVIEW_SCROLLHINT_ENSUREVISIBLE);
    OS.QListWidget_swt_setFocusIndex(handle, index);
}

/**
 * @see List.setSelection
 */
public static void setSelection(int handle, int start, int end, boolean multi) {
    int count = OS.QListWidget_count(handle);
    // deselectAll()
    OS.QListWidget_swt_selectRange(handle, 0, count - 1, 
            OS.QT_ITEMSELECTIONMODEL_SELECTIONFLAGS_DESELECT);
    if (end < 0 || start > end || ((!multi) && start != end)) {
        return;
    }
    start = Math.max (0, start);
    end = Math.min(end, count - 1);
    // select(int, int)
    OS.QListWidget_swt_selectRange(handle, start, end, 
            OS.QT_ITEMSELECTIONMODEL_SELECTIONFLAGS_SELECT);
    OS.QListWidget_scrollToItem(handle, end, OS.QT_ABSTRACTITEMVIEW_SCROLLHINT_ENSUREVISIBLE);
    OS.QListWidget_swt_setFocusIndex(handle, end); 
}

/**
 * @see List.setSelection
 */
public static void setSelection(int handle, int[] indices, boolean multi) {
    if (indices == null) {
        SWT.error(SWT.ERROR_NULL_ARGUMENT);
    }
    int count = OS.QListWidget_count(handle);
    // deselectAll()
    OS.QListWidget_swt_selectRange(handle, 0, count - 1, 
            OS.QT_ITEMSELECTIONMODEL_SELECTIONFLAGS_DESELECT);        
    int length = indices.length;
    if (length == 0 || ((!multi) && length > 1)) {
        return;
    }
    // select(int[])
    OS.QListWidget_swt_selectArray(handle, indices, length, 
            OS.QT_ITEMSELECTIONMODEL_SELECTIONFLAGS_SELECT, true);
}

/**
 * @see List.setSelection
 */
public static void setSelection(int handle, String[] items, boolean multi) {
    if (items == null) {
        SWT.error (SWT.ERROR_NULL_ARGUMENT);
    }
    int count = OS.QListWidget_count(handle);
    // deselectAll()
    OS.QListWidget_swt_selectRange(handle, 0, count - 1, 
            OS.QT_ITEMSELECTIONMODEL_SELECTIONFLAGS_DESELECT);
    int length = items.length;
    if (length == 0 || ((!multi) && length > 1)) {
        return;
    }
    int selectCmd = OS.QT_ITEMSELECTIONMODEL_SELECTIONFLAGS_SELECT;
    if (!multi) {
        selectCmd |= OS.QT_ITEMSELECTIONMODEL_SELECTIONFLAGS_CLEAR;
    }
    int scrollItem = 0;
    for (int i = 0; i < length; i++) {
        int index = -1;
        String string = items [i];
        if (string != null) {
            while (index < count) {
                // indexOf(String, int)
                index = OS.QListWidget_swt_indexOf(handle, string, index >= 0 ? index : 0);
                if (index > -1) {
                    // select(int)
                    scrollItem = index;
                    OS.QListWidget_swt_selectIndex(handle, index, selectCmd);
                }
                else {
                    break;
                }
                index++;
            }
        }
    }
    // showSelection()
    OS.QListWidget_scrollToItem(handle, scrollItem, OS.QT_ABSTRACTITEMVIEW_SCROLLHINT_ENSUREVISIBLE);
    OS.QListWidget_swt_setFocusIndex(handle, scrollItem); 
}

/**
 * @see List.showSelection
 */
public static void showSelection(int handle) {
    OS.QListWidget_scrollToItem(handle, OS.QListWidget_currentRow(handle), 
        OS.QT_ABSTRACTITEMVIEW_SCROLLHINT_ENSUREVISIBLE);
}

public static void sort(int[] intValues) {
    int idxs[] = intValues;
    int length = idxs.length;
    for (int gap = length / 2; gap > 0; gap /= 2) {
        for (int i = gap; i < length; i++) {
            for (int j = i - gap; j >= 0; j -= gap) {
                if (idxs[j] > idxs[j + gap]) {
                    int swap = idxs[j];
                    idxs[j] = idxs[j + gap];
                    idxs[j + gap] = swap;
                }
            }
        }
    }
}

public static int[] sortAndRemoveDuplicates(int[] intValues) {
    int length = intValues.length;
    int idxs[] = new int[length];
    System.arraycopy(intValues, 0, idxs, 0, length);
    sort(idxs);
    Vector v = new Vector();
    int val = 0;
    for (int i = 0; i < idxs.length; i++) {
        if (i > 0) {
            if (val == idxs[i]) {
                continue;
            }
        }
        val = idxs[i];
        Integer intObj = new Integer(val);
        v.addElement(intObj);
    }
    int[] res = new int[v.size()];
    for (int i = 0; i < res.length; i++) {
        res[i] = ((Integer)v.elementAt(i)).intValue();
    }
    return res;
}
}
