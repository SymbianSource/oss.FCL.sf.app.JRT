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

/**
 * The class <code>TextTransfer</code> provides a platform specific mechanism
 * for converting plain text represented as a java <code>String</code>
 * to a platform specific representation of the data and vice versa.  See
 * <code>Transfer</code> for additional information.
 *
 * <p>An example of a java <code>String</code> containing plain text is shown
 * below:</p>
 *
 * <code><pre>
 *     String textData = "Hello World";
 * </code></pre>
 */
public class TextTransfer extends ByteArrayTransfer
{
    private static TextTransfer static_instance = new TextTransfer();
    final static private int NB_SUPPORTED_TYPES = 1;



    /**
     * Returns the singleton instance of the TextTransfer class.
     *
     * @return the singleton instance of the TextTransfer class
     */
    public static TextTransfer getInstance()
    {
        return static_instance;
    }

    /**
     * This implementation of <code>javaToNative</code> converts plain text
     * represented by a java <code>String</code> to a platform specific representation.
     * For additional information see <code>Transfer#javaToNative</code>.
     *
     * @param object a java <code>String</code> containing text
     * @param transferData an empty <code>TransferData</code> object; this
     *  object will be filled in on return with the platform specific format of the data
     */
    public void javaToNative(Object object, TransferData transferData)
    {
        transferData.type = internal_ESWT_DND_TEXT_TRANSFER_TYPE;
        transferData.internal_setData(object);
    }

    /**
     * This implementation of <code>nativeToJava</code> converts a platform specific
     * representation of plain text to a java <code>String</code>.
     * For additional information see <code>Transfer#nativeToJava</code>.
     *
     * @param transferData the platform specific representation of the data to be
     * been converted
     * @return a java <code>String</code> containing text if the
     * conversion was successful; otherwise null
     */
    public Object nativeToJava(TransferData transferData)
    {
        return transferData.internal_getData();
    }


    protected int[] getTypeIds()
    {
        return new int[] {internal_ESWT_DND_TEXT_TRANSFER_TYPE};
    }


    protected String[] getTypeNames()
    {
        return new String[] {internal_ESWT_DND_TEXT_TRANSFER_NAME};
    }

    public boolean isSupportedType(TransferData transferData)
    {
        return (transferData.type == internal_ESWT_DND_TEXT_TRANSFER_TYPE);
    }



    public TransferData[] getSupportedTypes()
    {
        TransferData[] types = new TransferData[NB_SUPPORTED_TYPES];
        types[0] = new TransferData();
        types[0].type = internal_ESWT_DND_TEXT_TRANSFER_TYPE;
        return types;
    }


}
