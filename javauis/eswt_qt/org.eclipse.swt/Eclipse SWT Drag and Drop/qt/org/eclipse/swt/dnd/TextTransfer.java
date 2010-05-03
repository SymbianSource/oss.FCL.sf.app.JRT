/*******************************************************************************
 * Copyright (c) 2000, 2008 IBM Corporation and others.
 * Portion Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *
 * Contributors:
 *     IBM Corporation - initial API and implementation
 *     Nokia Corporation - QT implementation      
 *******************************************************************************/
package org.eclipse.swt.dnd;

/**
 * The class <code>TextTransfer</code> provides a platform specific mechanism 
 * for converting plain text represented as a java <code>String</code> 
 * to a platform specific representation of the data and vice versa.
 * 
 * <p>An example of a java <code>String</code> containing plain text is shown 
 * below:</p>
 * 
 * <code><pre>
 *     String textData = "Hello World";
 * </code></pre>
 * 
 * @see Transfer
 */
public class TextTransfer extends ByteArrayTransfer {
	private static TextTransfer instance = new TextTransfer();
	private static final String ID_NAME = "text/plain";
	private static final int ID = registerType(ID_NAME);
	
private TextTransfer() {
}

/**
 * Returns the singleton instance of the TextTransfer class.
 *
 * @return the singleton instance of the TextTransfer class
 */
public static TextTransfer getInstance () {
	return instance;
}

/**
 * This implementation of <code>javaToNative</code> converts plain text
 * represented by a java <code>String</code> to a platform specific representation.
 * 
 * @param object a java <code>String</code> containing text
 * @param transferData an empty <code>TransferData</code> object that will
 *  	be filled in on return with the platform specific format of the data
 *  
 * @see Transfer#nativeToJava
 */
public void javaToNative (Object object, TransferData transferData){
    transferData.result = 0;
    if (!checkText(object) || !isSupportedType(transferData)) {
        return;
    }
    transferData.data = ((String)object).getBytes();
    transferData.result = 1;
}

/**
 * This implementation of <code>nativeToJava</code> converts a platform specific 
 * representation of plain text to a java <code>String</code>.
 * 
 * @param transferData the platform specific representation of the data to be converted
 * @return a java <code>String</code> containing text if the conversion was successful; otherwise null
 * 
 * @see Transfer#javaToNative
 */
public Object nativeToJava(TransferData transferData){
    if (checkByteArray(transferData.data) && isSupportedType(transferData)) {
        transferData.result = 1;
        return new String(transferData.data);
    } else {
        transferData.result = 0;
        return null;
    }
}

protected int[] getTypeIds(){
    return new int[]{ID};
}

protected String[] getTypeNames() {
    return new String[]{ID_NAME};
}

private boolean checkText(Object object) {
	return (object != null  && object instanceof String && ((String)object).length() > 0);
}

}
