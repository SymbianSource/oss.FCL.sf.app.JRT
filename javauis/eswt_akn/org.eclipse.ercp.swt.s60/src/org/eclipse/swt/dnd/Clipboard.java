/*******************************************************************************
 * Copyright (c) 2000, 2008 IBM Corporation and others.
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *
 * Contributors:
 *     IBM Corporation - initial API and implementation
 *     Nokia Corporation - S60 implementation
 *******************************************************************************/
package org.eclipse.swt.dnd;


import org.eclipse.swt.*;
import org.eclipse.swt.widgets.*;
import org.eclipse.ercp.swt.expanded.internal.OS;
import java.util.Vector;

/**
 * The <code>Clipboard</code> provides a mechanism for transferring data from one
 * application to another or within an application.
 *
 * <p>IMPORTANT: This class is <em>not</em> intended to be subclassed.</p>
 */
public class Clipboard
{
    private Display display;
    private int clipboardHandle;
    static final  String PACKAGE_PREFIX = "org.eclipse.swt.dnd.";


    /**
     * Constructs a new instance of this class.  Creating an instance of a Clipboard
     * may cause system resources to be allocated depending on the platform.  It is therefore
     * mandatory that the Clipboard instance be disposed when no longer required.
     *
     * @param display the display on which to allocate the clipboard
     *
     * @exception SWTException <ul>
     *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the parent</li>
     *    <li>ERROR_INVALID_SUBCLASS - if this class is not an allowed subclass</li>
     * </ul>
     *
     * @see Clipboard#dispose
     * @see Clipboard#checkSubclass
     */
    public Clipboard(Display display)
    {
        checkSubclass();
        Display currentDisplay = display;
        if (currentDisplay == null)
        {
            currentDisplay = Display.getCurrent();
        }
        if (!currentDisplay.internal_isValidThread())
        {
            SWT.error(SWT.ERROR_THREAD_INVALID_ACCESS);
        }

        this.display = currentDisplay;
        clipboardHandle = OS.Clipboard_New(this);
    }

    /**
     * Checks that this class can be subclassed.
     * <p>
     * The SWT class library is intended to be subclassed
     * only at specific, controlled points. This method enforces this
     * rule unless it is overridden.
     * </p><p>
     * <em>IMPORTANT:</em> By providing an implementation of this
     * method that allows a subclass of a class which does not
     * normally allow subclassing to be created, the implementer
     * agrees to be fully responsible for the fact that any such
     * subclass will likely fail between SWT releases and will be
     * strongly platform specific. No support is provided for
     * user-written classes which are implemented in this fashion.
     * </p><p>
     * The ability to subclass outside of the allowed SWT classes
     * is intended purely to enable those not on the SWT development
     * team to implement patches in order to get around specific
     * limitations in advance of when those limitations can be
     * addressed by the team. Subclassing should not be attempted
     * without an intimate and detailed understanding of the hierarchy.
     * </p>
     *
     * @exception SWTException <ul>
     *    <li>ERROR_INVALID_SUBCLASS - if this class is not an allowed subclass</li>
     * </ul>
     */
    protected void checkSubclass()
    {
        String name = getClass().getName();
        if (!name.startsWith(PACKAGE_PREFIX))
        {
            SWT.error(SWT.ERROR_INVALID_SUBCLASS);
        }
    }

    /**
     * Throws an <code>SWTException</code> if the receiver can not
     * be accessed by the caller. This may include both checks on
     * the state of the receiver and more generally on the entire
     * execution context. This method <em>should</em> be called by
     * widget implementors to enforce the standard SWT invariants.
     * <p>
     * Currently, it is an error to invoke any method (other than
     * <code>isDisposed()</code>) on a widget that has had its
     * <code>dispose()</code> method called. It is also an error
     * to call widget methods from any thread that is different
     * from the thread that created the widget.
     * </p><p>
     * In future releases of SWT, there may be more or fewer error
     * checks and exceptions may be thrown for different reasons.
     * </p>
     *
     * @exception SWTException <ul>
     *    <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
     *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
     * </ul>
     */
    protected void checkWidget()
    {
        if (isDisposed())
        {
            SWT.error(SWT.ERROR_WIDGET_DISPOSED);
        }
        else if (!display.internal_isValidThread())
        {
            SWT.error(SWT.ERROR_THREAD_INVALID_ACCESS);
        }
    }

    /**
     * Disposes of the operating system resources associated with the clipboard.
     * The data will still be available on the system clipboard after the dispose
     * method is called.
     *
     * <p>NOTE: On some platforms the data will not be available once the application
     * has exited or the display has been disposed.</p>
     *
     * @exception SWTException <ul>
     *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the parent</li>
     * </ul>
     */
    public void dispose()
    {
        if (!display.internal_isValidThread())
        {
            SWT.error(SWT.ERROR_THREAD_INVALID_ACCESS);
        }

        if (isDisposed())
        {
            SWT.error(SWT.ERROR_WIDGET_DISPOSED);
        }

        OS.Clipboard_Dispose(clipboardHandle);
        clipboardHandle = 0;
    }

    /**
     * Retrieve the data of the specified type currently available on the system clipboard.  Refer to the
     * specific subclass of <code>Transfer</code> to determine the type of object returned.
     *
     * <p>The following snippet shows text being retrieved from the clipboard:</p>
     *
     *    <code><pre>
     *    Clipboard clipboard = new Clipboard(display);
     *    TextTransfer textTransfer = TextTransfer.getInstance();
     *    String textData = (String)clipboard.getContents(textTransfer);
     *    if (textData != null) System.out.println("Text is "+textData);
     *    clipboard.dispose();
     *    </code></pre>
     *
     * @see Transfer
     *
     * @param transfer the transfer agent for the type of data being requested
     *
     * @return the data obtained from the clipboard or null if no data of this type is available
     *
     * @exception SWTException <ul>
     *    <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
     *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
     * </ul>
     * @exception IllegalArgumentException <ul>
     *    <li>ERROR_NULL_ARGUMENT - if transfer is null</li>
     * </ul>
     */
    public Object getContents(Transfer transfer)
    {
        checkWidget();
        if (transfer == null)
        {
            SWT.error(SWT.ERROR_NULL_ARGUMENT);
        }

        int[] types = transfer.getTypeIds();
        TransferData data = new TransferData();
        Object clipboardContents = null;

        for (int i=0; i<types.length; i++)
        {
            clipboardContents = OS.Clipboard_GetContents(clipboardHandle, types[i]);
            if (clipboardContents != null)
            {
                data.type = types[i];
                data.internal_setData(clipboardContents);
                return transfer.nativeToJava(data);
            }
        }

        return null;
    }
    /**
     * Returns <code>true</code> if the clipboard has been disposed,
     * and <code>false</code> otherwise.
     * <p>
     * This method gets the dispose state for the clipboard.
     * When a clipboard has been disposed, it is an error to
     * invoke any other method using the clipboard.
     * </p>
     *
     * @return <code>true</code> when the widget is disposed and <code>false</code> otherwise
     *
     * @since 3.0
     */
    public boolean isDisposed()
    {
        return (clipboardHandle == 0);
    }

    /**
     * Place data of the specified type on the system clipboard.  More than one type of
     * data can be placed on the system clipboard at the same time.  Setting the data
     * clears any previous data of the same type from the system clipboard and also
     * clears data of any other type currently on the system clipboard.
     *
     * <p>NOTE: On some platforms, the data is immediately copied to the system
     * clipboard but on other platforms it is provided upon request.  As a result, if the
     * application modifes the data object it has set on the clipboard, that modification
     * may or may not be available when the data is subsequently requested.</p>
     *
     * <p>The following snippet shows text being set on the clipboard:</p>
     *
     * <code><pre>
     *  Clipboard clipboard = new Clipboard(display);
     *      String textData = "Hello World";
     *      TextTransfer textTransfer = TextTransfer.getInstance();
     *      clipboard.setContents(new Object[]{textData}, new Transfer[]{textTransfer});
     *      clipboard.dispose();
     * </code></pre>
     *
     * @param data the data to be set in the clipboard
     * @param dataTypes the transfer agents that will convert the data to its platform
     * specific format; each entry in the data array must have a corresponding dataType
     *
     * @exception IllegalArgumentException <ul>
     *    <li>ERROR_INVALID_ARGUMENT - if data is null or datatypes is null
     *          or the length of data is not the same as the length of dataTypes</li>
     * </ul>
     * @exception SWTException <ul>
     *    <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
     *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
     * </ul>
     *  @exception SWTError <ul>
     *    <li>ERROR_CANNOT_SET_CLIPBOARD - if the clipboard is locked or otherwise unavailable</li>
     * </ul>
     *
     * <p>NOTE: ERROR_CANNOT_SET_CLIPBOARD should be an SWTException, since it is a
     * recoverable error, but can not be changed due to backward compatability.</p>
     */
    public void setContents(Object[] data, Transfer[] dataTypes)
    {
        if ((data == null) || (dataTypes == null))
        {
            SWT.error(SWT.ERROR_INVALID_ARGUMENT);
        }
        else if (data.length != dataTypes.length)
        {
            SWT.error(SWT.ERROR_INVALID_ARGUMENT);
        }
        checkWidget();

        int nbTransfer = dataTypes.length;
        if (nbTransfer == 0)
        {
            // Clear the clipboard
            OS.Clipboard_Clear(clipboardHandle);
            return;
        }

        int nbData = data.length;
        int[] typeId;
        int indexTypeId;
        int indexTransfer;
        TransferData transferData = new TransferData();

        for (int indexData=0; indexData<nbData; indexData++)
        {
            typeId = dataTypes[indexData].getTypeIds();
            for (indexTypeId=0; indexTypeId<typeId.length; indexTypeId++)
            {
                dataTypes[indexData].javaToNative(data[indexData], transferData);
                OS.Clipboard_SetContents(clipboardHandle, typeId[indexTypeId],
                                         transferData.internal_getData());
            }
        }
    }

    /**
     * Returns an array of the data types currently available on the system clipboard. Use
     * with Transfer.isSupportedType.
     *
     * @return array of TransferData
     *
     * @exception SWTException <ul>
     *    <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
     *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
     * </ul>
     *
     * @see Transfer#isSupportedType
     *
     * @since 3.0
     */
    public TransferData[] getAvailableTypes()
    {
        checkWidget();
        Vector integerTypes = Transfer.internal_GetAllSupportedTypes();
        int nbTypes = integerTypes.size();
        int[] intTypes = new int[nbTypes];
        int i;
        for (i=0; i<nbTypes; i++)
        {
            intTypes[i] = ((Integer)integerTypes.elementAt(i)).intValue();
        }
        int[] availableTypes = OS.Clipboard_GetAvailableTypes(clipboardHandle, intTypes);
        if (availableTypes == null)
        {
            return new TransferData[0];
        }

        TransferData[] data = new TransferData[availableTypes.length];
        for (i=0; i<availableTypes.length; i++)
        {
            data[i] = new TransferData();
            data[i].type = availableTypes[i];
        }
        return data;
    }

    /**
     * Returns a platform specific list of the data types currently available on the
     * system clipboard.
     *
     * <p>Note: <code>getAvailableTypeNames</code> is a utility for writing a Transfer
     * sub-class.  It should NOT be used within an application because it provides
     * platform specific information.</p>
     *
     * @return a platform specific list of the data types currently available on the
     * system clipboard
     *
     * @exception SWTException <ul>
     *    <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
     *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
     * </ul>
     */
    public String[] getAvailableTypeNames()
    {
        TransferData[] data = getAvailableTypes();
        String[] names = new String[data.length];
        int index;
        Integer type;
        Vector supportedTypes = Transfer.internal_GetAllSupportedTypes();
        Vector supportedTypeNames = Transfer.internal_GetAllSupportedTypeNames();

        for (int i=0; i<data.length; i++)
        {
            type = new Integer(data[i].type);
            index =  supportedTypes.indexOf(type);
            names[i] = (String)(supportedTypeNames.elementAt(index));
        }

        return names;
    }

}
