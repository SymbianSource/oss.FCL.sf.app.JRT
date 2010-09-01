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
package org.eclipse.swt.internal.symbian;


import org.eclipse.swt.widgets.*;
import org.eclipse.swt.graphics.*;
import java.io.OutputStream;
import org.eclipse.swt.internal.Library;


/**
 * @brief This is the place where all native functions are located.
 *
 * This class is not supposed to be instantiated as it only contains static
 * methods. <b>Warning</b>: the content of this class is platform dependent.
 */
public class OS
{

    // Load the DLL needed for the JNIs
    static
    {
        Library.loadLibrary("eswt");
    }
    
    public static final int WS_SYMBIAN_S60_50 = 1;
    public static final int WS_SYMBIAN_S60_92 = 2;

    // Underlying window server.
    public static final int windowServer = windowServer();

    /*
     * Class Device
     */
    public static final native Rectangle Device_GetBounds(int handle);
    public static final native Rectangle Device_GetClientArea(int handle);
    public static final native int Device_GetDepth(int handle);
    public static final native Point Device_GetDPI(int handle);
    public static final native FontData[] Device_GetFontList(int handle, String faceName, boolean scalable);
    public static final native RGB Device_GetSystemColor(int handle, int id);
    public static final native int Device_GetSystemFont(int handle);
    //public static final native String Device_GetSystemFontName(int handle);

    /*
     * Class Display
     */
    public static final native int Display_New(Display display, int displayParameter);
    public static final native int Display_DeviceHandle(int handle);
    public static final native void Display_Dispose(int handle);
    public static final native void Display_SetAppName(String name);
    public static final native boolean Display_Sleep(int handle);
    public static final native void Display_Wake(int handle);
    public static final native boolean Display_ReadAndDispatch(int handle);
    public static final native Shell Display_GetActiveShell(int handle);
    public static final native int Display_getDoubleClickTime(int displayHandle);
    public static final native int Display_getIconDepth(int displayHandle);
    public static final native Shell[] Display_GetShells(int displayHandle);
    public static final native void Display_Beep(int displayHandle);
    public static final native Control Display_GetFocusControl(int displayHandle);
    public static final native void Display_StartTimer(int displayHandle, int milliseconds, int index);
    public static final native boolean Display_Post(int eventType, char eventCharacter, int eventKeyCode, int eventStateMask);
    public static final native boolean Display_MousePost(int eventType, int x, int y, int eventStateMask);
    // Internal public api for eRCP
    public static final native void Display_SetAppUID(int uid);
    // Internal public api for eRCP
    public static final native void Display_SetAppVisible(boolean visible);
    public static final native void Display_SetOptimizedPaint(int handle, boolean status);
    public static final native void Display_NotifyFirstPaintComplete(boolean topShellPainted);

    /*
     * Class DisplayExtension
     */
    public static final native Point DisplayExtension_getBestImageSize(int imageType);

    /*
     * Class Canvas
     */
    public static final native int Canvas_New(Canvas peer, int parentComposite, int style);
    public static final native int Canvas_CompositeHandle(int handle);
    public static final native void Canvas_Scroll(int handle, int destX, int destY, int x, int y, int width, int height, boolean all);

    /*
     * Class Color
     */
    public static final native int Color_New(int device, int red, int green, int blue);
    public static final native void Color_Dispose(int device, int handle);
    public static final native int Color_RgbValue(int device, int handle);

    /*
     * Class Composite
     */
    public static final native int Composite_New(Composite peer, int parentComposite, int style);
    public static final native int Composite_ScrollableHandle(int handle);
    public static final native Control[] Composite_GetChildren(int handle);
    public static final native void Composite_SetTabList(int handle, int[] list);
    public static final native Control[] Composite_GetTabList(int handle);
    public static final native void Composite_SetScrolledCompositeContent(int handle, int contentHandle);

    /*
     * Class Control
     */
    public static final native void Control_Dispose(int handle);
    public static final native boolean Control_SetFocus(int handle);
    public static final native boolean Control_ForceFocus(int handle);
    public static final native int Control_GetBorderWidth(int handle);
    public static final native boolean Control_GetEnabled(int handle);
    public static final native boolean Control_GetVisible(int device, int handle);
    public static final native int Control_GetBackground(int handle);
    public static final native int Control_GetForeground(int handle);
    public static final native int Control_GetFont(int handle);
    public static final native Rectangle Control_GetBounds(int handle);
    public static final native Rectangle Control_GetClientArea(int handle);
    public static final native Point Control_GetLocation(int handle);
    public static final native Point Control_ComputeSize(int handle, int wHint, int hHint);
    public static final native boolean Control_IsFocusControl(int handle);
    public static final native Point Control_GetSize(int handle);
    public static final native int Control_GetStyle(int handle);
    public static final native boolean Control_HasDoNotDrawFlag(int handle);
    public static final native void Control_MoveAbove(int handle, int controlHandle);
    public static final native void Control_MoveBelow(int handle, int controlHandle);
    public static final native boolean Control_PostTraverseEvent(int handle, int traversal);
    public static final native void Control_Paint(int deviceHandle, int handle, int x, int y,
            int width, int height, boolean paintNatively, int gcHandle, int[] intParams, int intCount, String strParams);
    public static final native void Control_Redraw(int handle);
    public static final native void Control_RedrawRegion(int handle, int x, int y, int width, int height);
    public static final native void Control_SetRedraw(int handle, boolean redraw);
    public static final native void Control_SetCapture(int handle, boolean capture);
    public static final native void Control_SetFont(int handle, int fontHandle);
    public static final native void Control_SetBackground(int handle, int colorHandle);
    public static final native void Control_SetForeground(int handle, int colorHandle);
    public static final native void Control_SetVisible(int handle, boolean visible);
    public static final native void Control_SetLocation(int handle, int x, int y);
    public static final native void Control_SetSize(int handle, int width, int height);
    public static final native void Control_SetEnabled(int handle, boolean enabled);
    public static final native void Control_SetBounds(int handle, int x, int y, int width, int height);
    public static final native void Control_SetMenu(int handle, int menuHandle);
    public static final native Point Control_ToControl(int handle, int x, int y);
    public static final native Point Control_ToDisplay(int handle, int x, int y);
    public static final native void Control_Update(int handle);

    /*
     * Class Decorations
     */
    public static final native int Decorations_New(Decorations peer, int parentComposite, int style);
    public static final native int Decorations_CanvasHandle(int handle);
    public static final native String Decorations_GetText(int handle);
    public static final native void Decorations_SetImage(int handle, int imageHandle);
    public static final native void Decorations_SetMaximized(int handle, boolean maximized);
    public static final native boolean Decorations_GetMaximized(int handle);
    public static final native void Decorations_SetMinimized(int handle, boolean minimized);
    public static final native boolean Decorations_GetMinimized(int handle);
    public static final native void Decorations_SetMenuBar(int handle, int menuHandle);
    public static final native void Decorations_SetText(int handle, String title);
    public static final native void Decorations_SetDefaultButton(int handle, int defaultButtonHandle);
    public static final native Button Decorations_GetDefaultButton(int handle);

    /*
     * Interface Drawable
     */
    public static final native int Drawable_NewGc(int handle);

    /*
     * Class Font
     */
    public static final native void Font_Dispose(int deviceHandle, int handle);
    public static final native int Font_GetHeight(int handle);
    public static final native String Font_GetName(int handle);
    public static final native int Font_GetStyle(int handle);
    public static final native int Font_New(int deviceHandle, String name, int height, int style);
    public static final native void Font_AddRef(int handle);
    public static final native void Font_RemoveRef(int handle);

    /*
     * Class GC
     */
    public static final native void GC_Dispose(int handle);
    public static final native void GC_SetBackground(int deviceHandle, int handle, int colorHandle);
    public static final native void GC_SetForeground(int deviceHandle, int handle, int colorHandle);
    public static final native void GC_SetLineStyle(int deviceHandle, int handle, int style);
    public static final native void GC_SetLineWidth(int deviceHandle, int handle, int width);
    public static final native void GC_CopyAreaToImage(int deviceHandle, int handle, int imageHandle, int x, int y);
    public static final native void GC_CopyArea(int deviceHandle, int handle, int srcX, int srcY, int width, int height, int destX, int destY);
    public static final native void GC_DrawImage(int deviceHandle, int handle, int imageHandle, int srcX, int srcY, int srcWidth, int srcHeight, int destX, int destY, int destWidth, int destHeight);
    public static final native void GC_DrawPoint(int deviceHandle, int handle, int x, int y);
    public static final native void GC_DrawLine(int deviceHandle, int handle, int x1, int y1, int x2, int y2);
    public static final native void GC_DrawPolyline(int deviceHandle, int handle, int[] pointArray);
    public static final native void GC_DrawPolygon(int deviceHandle, int handle, int[] pointArray, boolean fill);
    public static final native void GC_DrawRectangle(int deviceHandle, int handle, int x, int y, int width, int height, boolean fill);
    public static final native void GC_DrawRoundRectangle(int deviceHandle, int handle, int x, int y, int width, int height, int arcWidth, int arcHeight, boolean fill);
    public static final native void GC_DrawText(int deviceHandle, int handle, String string, int x, int y, int flags);
    public static final native void GC_DrawOval(int deviceHandle, int handle, int x, int y, int width, int height, boolean fill);
    public static final native void GC_DrawArc(int deviceHandle, int handle, int x, int y, int width, int height, int startAngle, int arcAngle, boolean fill);
    public static final native void GC_DrawFocusRect(int deviceHandle, int handle, int x, int y, int width, int height);
    public static final native boolean GC_IsClipped(int deviceHandle, int handle);
    public static final native int GC_GetAdvanceWidth(int deviceHandle, int handle, char ch);
    public static final native void GC_FillGradientRectangle(int deviceHandle, int handle, int x, int y, int width, int height, boolean vertical);
    public static final native int GC_GetCharWidth(int deviceHandle, int handle, char ch);
    public static final native Rectangle GC_GetClippingRect(int deviceHandle, int handle);
    public static final native int GC_GetFont(int deviceHandle, int handle);
    public static final native int[] GC_GetFontMetrics(int deviceHandle, int handle);
    public static final native int GC_GetLineStyle(int deviceHandle, int handle);
    public static final native int GC_GetLineWidth(int deviceHandle, int handle);
    public static final native boolean GC_GetXORMode(int deviceHandle, int handle);
    public static final native void GC_SetClippingRect(int deviceHandle, int handle, int x, int y, int width, int height);
    public static final native void GC_CancelClipping(int deviceHandle, int handle);
    public static final native void GC_SetAlpha(int deviceHandle, int handle, int alpha);
    public static final native void GC_SetFont(int deviceHandle, int handle, int fontHandle);
    public static final native void GC_SetXORMode(int deviceHandle, int handle, boolean xor);
    public static final native Point GC_TextExtent(int deviceHandle, int handle, String string, int flags);
    public static final native void GC_DrawM2GImage(int deviceHandle, int handle, int[] bitmaphandles, int srcX, int srcY, int srcWidth, int srcHeight, int destX, int destY, int destWidth, int destHeight, int useNativeClear);

    /*
     * Class Image
     */
    public static final native int Image_New(int deviceHandle, int width, int height);
    public static final native int Image_NewFromData(int deviceHandle, ImageData data);
    public static final native int Image_NewFromTheme(int deviceHandle, int id);
    public static final native void Image_Dispose(int deviceHandle, int handle);
    public static final native Rectangle Image_GetBounds(int handle);
    public static final native ImageData Image_GetData(int handle);
    public static final native void Image_AddRef(int handle);
    public static final native void Image_RemoveRef(int handle);
    public static final native int Image_Scale(int deviceHandle, int srcImageHandle, int destWidth, int destHeight, boolean keepAspectRatio);

    /*
     * Class Link
     */
    public static final native int Link_New(Link peer, int parentComposite, int style);
    public static final native int Link_ControlHandle(int handle);
    public static final native void Link_SetText(int handle, String [] textFragments, String [] linkTargets);

    /*
     * Class Menu
     */
    public static final native int Menu_New(Menu peer, int style);
    public static final native void Menu_Dispose(int handle);
    public static final native void Menu_SetEnabled(int handle, boolean enabled);
    public static final native void Menu_SetVisible(int handle, boolean visible);
    public static final native void Menu_SetDefaultItem(int handle, int menuItemHandle);
    public static final native void Menu_SetLocation(int handle, int x, int y);

    /*
     * Class MenuItem
     */
    public static final native int MenuItem_New(MenuItem peer, int parentMenu, int index, int style);
    public static final native void MenuItem_Dispose(int handle);
    public static final native boolean MenuItem_GetSelection(int handle);
    public static final native void MenuItem_SetAccelerator(int handle, int accelerator);
    public static final native void MenuItem_SetEnabled(int handle, boolean enabled);
    public static final native void MenuItem_SetImage(int handle, int imageHandle);
    public static final native void MenuItem_SetMenu(int handle, int menuHandle);
    public static final native void MenuItem_SetSelection(int handle, boolean selected);
    public static final native void MenuItem_SetText(int handle, String text);

    /*
     * Class MessageBox
     */
    public static final native void MessageBox_Open(MessageBox peer, String title, String message, int style);

    /*
     * Class Scrollable
     */
    public static final native int Scrollable_ControlHandle(int handle);
    public static final native int Scrollable_CreateScrollBar(int handle, ScrollBar peer, int style);
    public static final native void Scrollable_SetVisibleScrollBar(int handle, int style, boolean aVisible);
    public static final native Rectangle Scrollable_ComputeTrim(int handle, int x, int y, int width, int height);

    /*
     * Class ScrollBar
     */
    public static final native void ScrollBar_Dispose(int handle);
    public static final native Point ScrollBar_GetSize(int handle);
    public static final native void ScrollBar_SetEnabled(int handle, boolean enabled);
    public static final native void ScrollBar_SetVisible(int handle, boolean visible);
    public static final native int ScrollBar_GetThumbPosition(int handle);
    public static final native void ScrollBar_SetThumbPosition(int handle, int position);
    public static final native int ScrollBar_GetThumbSpan(int handle);
    public static final native void ScrollBar_SetThumbSpan(int handle, int span);
    public static final native int ScrollBar_GetMinimum(int handle);
    public static final native void ScrollBar_SetMinimum(int handle, int minimum);
    public static final native int ScrollBar_GetMaximum(int handle);
    public static final native void ScrollBar_SetMaximum(int handle, int maximum);
    public static final native int ScrollBar_GetIncrement(int handle);
    public static final native void ScrollBar_SetIncrement(int handle, int increment);
    public static final native int ScrollBar_GetPageIncrement(int handle);
    public static final native void ScrollBar_SetPageIncrement(int handle, int pageIncrement);
    public static final native void ScrollBar_SetValues(int handle, int selection, int minimum, int maximum, int thumb, int increment, int pageIncrement);

    /*
     * Class Shell
     */
    public static final native int Shell_New(Shell peer, int parentShell, int style);
    public static final native int Shell_DecorationsHandle(int handle);
    public static final native void Shell_ForceActive(int handle);
    public static final native int Shell_GetImeInputMode(int shellHandle);
    public static final native Point Shell_GetMinimumSize(int shellHandle);
    public static final native Shell[] Shell_GetShells(int handle);
    public static final native void Shell_SetMinimumSize(int shellHandle, int width, int height);
    public static final native void Shell_SetImeInputMode(int shellHandle, int mode);
    public static final native Rectangle Shell_GetDefaultBounds(int shellHandle);
    public static final native void Shell_SetAsyncPainting(int shellHandle, boolean status);
    public static final native void Shell_SetTaskTip(int shellHandle);

    /*
     * Class Label
     */
    public static final native int Label_ControlHandle(int handle);
    public static final native int Label_New(Label peer, int parentComposite, int style);
    public static final native int Label_GetAlignment(int handle);
    public static final native void Label_SetAlignment(int handle, int alignment);
    public static final native void Label_SetImage(int handle, int imageHandle);
    public static final native void Label_SetText(int handle, String text);

    /*
     * Class Button
     */
    public static final native int Button_ControlHandle(int handle);
    public static final native int Button_New(Button peer, int parentComposite, int style);
    public static final native int Button_GetAlignment(int handle);
    public static final native void Button_SetAlignment(int handle, int alignment);
    public static final native boolean Button_GetSelection(int handle);
    public static final native void Button_SetSelection(int handle, boolean state);
    public static final native void Button_SetImage(int handle, int imageHandle);
    public static final native void Button_SetText(int handle, String text);
    public static final native void Button_SetDefault(int handle, boolean isDefault);
    public static final native void Button_Click(int handle);

    /*
     * Class List
     */
    public static final native int List_New(List peer, int parentComposite, int style);
    public static final native int List_ScrollableHandle(int handle);
    public static final native void List_Add(int handle, String string);
    public static final native void List_Add(int handle, String string, int index);
    public static final native void List_Deselect(int handle, int [] indices);
    public static final native void List_Deselect(int handle, int index);
    public static final native void List_Deselect(int handle, int start, int end);
    public static final native void List_DeselectAll(int handle);
    public static final native int List_GetFocusIndex(int handle);
    public static final native String List_GetItem(int handle, int index);
    public static final native int List_GetItemCount(int handle);
    public static final native int List_GetItemHeight(int handle);
    public static final native int List_GetSelectionCount(int handle);
    public static final native int List_GetSelectionIndex(int handle);
    public static final native int[] List_GetSelectionIndices(int handle);
    public static final native int List_GetTopIndex(int handle);
    public static final native int List_IndexOf(int handle, String string, int start);
    public static final native boolean List_IsSelected(int handle, int index);
    public static final native void List_Remove(int handle, int [] indices);
    public static final native void List_Remove(int handle, int index);
    public static final native void List_Remove(int handle, int start, int end);
    public static final native void List_RemoveAll(int handle);
    public static final native void List_Select(int handle, int index, boolean scroll);
    public static final native void List_Select(int handle, int start, int end);
    public static final native void List_SelectAll(int handle);
    public static final native void List_SetFocusIndex(int handle, int index);
    public static final native void List_SetItems(int handle, String [] items);
    public static final native void List_SetTopIndex(int handle, int index);
    public static final native void List_ShowSelection(int handle);

    /*
     * Class Text
     */
    public static final native int Text_New(Text peer, int parent, int style);
    public static final native int Text_ScrollableHandle(int handle);
    public static final native String Text_GetText(int handle);
    public static final native String Text_GetTextRange(int handle, int start, int end);
    public static final native boolean Text_SetText(int handle, String text);
    public static final native boolean Text_Append(int handle, String text);
    public static final native boolean Text_Insert(int handle, String text);
    public static final native int Text_GetCharCount(int handle);
    public static final native boolean Text_SetTextLimit(int handle, int limit);
    public static final native int Text_GetCaretLineNumber(int handle);
    public static final native Point Text_GetCaretLocation(int handle);
    public static final native int Text_GetCaretPosition(int handle);
    public static final native Point Text_GetSelection(int handle);
    public static final native String Text_GetSelectionText(int handle);
    public static final native void Text_SetSelection(int handle, int start, int end);
    public static final native void Text_ClearSelection(int handle);
    public static final native void Text_SelectAll(int handle);
    public static final native void Text_ShowSelection(int handle);
    //public static final native boolean Text_Cut(int handle);
    public static final native void Text_Copy(int handle);
    public static final native boolean Text_Paste(int handle);
    public static final native boolean Text_GetEditable(int handle);
    public static final native void Text_SetEditable(int handle, boolean editable);
    public static final native int Text_GetLineCount(int handle);
    public static final native int Text_GetLineHeight(int handle);
    public static final native int Text_GetTopIndex(int handle);
    public static final native void  Text_SetTopIndex(int handle, int index);
    public static final native int Text_GetTopPixel(int handle);
    public static final native char Text_GetEchoChar(int handle);
    public static final native void Text_SetEchoChar(int handle, char echo);
    public static final native void Text_SetDoubleClickEnabled(int handle, boolean doubleClick);
    public static final native int Text_GetOrientation(int handle);
    public static final native void Text_SetOrientation(int handle, int orientation);

    /*
     * class Combo
     */
    public static final native int Combo_CompositeHandle(int handle);
    public static final native int Combo_New(Combo peer, int parentComposite, int style);
    public static final native void Combo_Add(int handle, String string);
    public static final native void Combo_Add(int handle, String string, int index);
    public static final native void Combo_ClearSelection(int handle);
    public static final native void Combo_Copy(int handle);
    public static final native void Combo_Deselect(int handle, int index);
    public static final native void Combo_DeselectAll(int handle);
    public static final native String Combo_GetItem(int handle, int index);
    public static final native int Combo_GetItemCount(int handle);
    public static final native int Combo_GetItemHeight(int handle);
    public static final native String[] Combo_GetItems(int handle);
    public static final native int Combo_GetOrientation(int handle);
    public static final native Point Combo_GetSelection(int handle);
    public static final native int Combo_GetSelectionIndex(int handle);
    public static final native String Combo_GetText(int handle);
    public static final native int Combo_GetTextHeight(int handle);
    public static final native int Combo_GetTextLimit(int handle);
    public static final native int Combo_GetVisibleItemCount(int handle);
    public static final native int Combo_IndexOf(int handle, String string, int start);
    public static final native boolean Combo_InsertInTextControl(int handle, String string);
    public static final native boolean Combo_Paste(int handle);
    public static final native void Combo_Remove(int handle, int index);
    public static final native void Combo_Remove(int handle, int start, int end);
    public static final native void Combo_RemoveAll(int handle);
    public static final native void Combo_Select(int handle, int index);
    public static final native void Combo_SetItems(int handle, String [] items);
    public static final native void Combo_SetOrientation(int handle, int orientation);
    public static final native void Combo_SetSelection(int handle, int start, int end);
    public static final native boolean Combo_SetText(int handle, String text);
    public static final native boolean Combo_SetTextLimit(int handle, int limit);
    public static final native void Combo_SetVisibleItemCount(int handle, int count);

    /*
     * Class ProgressBar
     */
    public static final native int ProgressBar_ControlHandle(int handle);
    public static final native int ProgressBar_New(ProgressBar peer, int parent, int style);
    public static final native int ProgressBar_GetMaximum(int peer);
    public static final native int ProgressBar_GetMinimum(int peer);
    public static final native int ProgressBar_GetSelection(int peer);
    public static final native void ProgressBar_SetMaximum(int peer, int value);
    public static final native void ProgressBar_SetMinimum(int peer, int value);
    public static final native void ProgressBar_SetSelection(int peer, int value);

    /*
     * Class Slider
     */
    public static final native int Slider_ControlHandle(int handle);
    public static final native int Slider_New(Slider peer, int parent, int style);
    public static final native int Slider_GetMaximum(int peer);
    public static final native int Slider_GetMinimum(int peer);
    public static final native int Slider_GetPageIncrement(int peer);
    public static final native int Slider_GetIncrement(int peer);
    public static final native int Slider_GetSelection(int peer);
    public static final native void Slider_SetMaximum(int peer, int value);
    public static final native void Slider_SetMinimum(int peer, int value);
    public static final native void Slider_SetIncrement(int peer, int value);
    public static final native void Slider_SetPageIncrement(int peer, int value);
    public static final native void Slider_SetSelection(int peer, int value);
    public static final native void Slider_SetValues(int peer, int selection, int minimum, int maximum, int increment, int pageIncrement);

    /*
     * Class FileDialog
     */
    public static final native void FileDialog_Open(FileDialog peer, String initPath, String title, int style);

    /*
     * Class ImageDecoder
     */
    public static final native int ImageDataLoader_New();
    public static final native ImageData[] ImageDataLoader_DecodeImage(int handle,String fileName);
    public static final native ImageData[] ImageDataLoader_DecodeWholeImageFromBuffer(int handle,byte[] buffer);
    public static final native void ImageDataLoader_EncodeImage(int handle,ImageData imageData, int format, String fileName);
    public static final native void ImageDataLoader_EncodeImageToStream(int handle,ImageData imageData,int  format, OutputStream stream);
    public static final native void ImageDataLoader_Dispose(int handle);
    public static final native void ImageDataLoader_StartImageDecoding(int handle, byte[] buffer);
    public static final native void ImageDataLoader_AppendData(int handle, byte[] buffer);
    public static final native ImageData[] ImageDataLoader_GetImageData(int handle);
    public static final native int ImageDataLoader_GetLogicalScreenHeight(int handle);
    public static final native int ImageDataLoader_GetLogicalScreenWidth(int handle);

    /*
     * General
     */
    private static final native int windowServer();
}
