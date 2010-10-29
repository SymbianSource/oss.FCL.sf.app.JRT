/*******************************************************************************
 * Copyright (c) 2000, 2009 IBM Corporation and others.
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *
 * Contributors:
 *     IBM Corporation - initial API and implementation
 *     Nokia Corporation - Qt implementation
 *******************************************************************************/
package org.eclipse.swt.internal;


import org.eclipse.swt.SWT;
import java.io.*;
import com.nokia.mj.impl.utils.ResourceLoader;
//import java.util.zip.DeflaterOutputStream;

/**
 * This class is a placeholder for utility methods commonly
 * used on J2SE platforms but not supported on some J2ME
 * profiles.
 * <p>
 * It is part of our effort to provide support for both J2SE
 * and J2ME platforms.
 * </p>
 * <p>
 * IMPORTANT: some of the methods have been modified from their
 * J2SE parents. Refer to the description of each method for
 * specific changes.
 * </p>
 * <ul>
 * <li>Exceptions thrown may differ since J2ME's set of
 * exceptions is a subset of J2SE's one.
 * </li>
 * <li>The range of the mathematic functions is subject to
 * change.
 * </li>
 * </ul>
 */
public final class Compatibility {

/**
 * Returns the PI constant as a double.
 */
public static final double PI = (double)3.141592653589793;

private static ResourceLoader iRes;

/**
 * Sine table
 * <p>
 * Values represent sine for each degree between 0 and 90. Values have
 * been multiplied by 65536 and rounded.
 * </p>
 */
static int[] sineTable = {
        0,  1143,  2287,  3429,  4571,  5711,  6850,  7986,  9120, 10252, //  0 to  9 degrees
    11380, 12504, 13625, 14742, 15854, 16961, 18064, 19160, 20251, 21336, // 10 to 19 degrees
    22414, 23486, 24550, 25606, 26655, 27696, 28729, 29752, 30767, 31772, // 20 to 29 degrees
    32767, 33753, 34728, 35693, 36647, 37589, 38521, 39440, 40347, 41243, // 30 to 39 degrees
    42125, 42995, 43852, 44695, 45525, 46340, 47142, 47929, 48702, 49460, // 40 to 49 degrees
    50203, 50931, 51643, 52339, 53019, 53683, 54331, 54963, 55577, 56175, // 50 to 59 degrees
    56755, 57319, 57864, 58393, 58903, 59395, 59870, 60326, 60763, 61183, // 60 to 69 degrees
    61583, 61965, 62328, 62672, 62997, 63302, 63589, 63856, 64103, 64331, // 70 to 79 degrees
    64540, 64729, 64898, 65047, 65176, 65286, 65376, 65446, 65496, 65526, // 80 to 89 degrees
    65536 };                                                              //       90 degrees

private static final String CLDC_COMPATIBILITY_DELEGATE = "org.eclipse.swt.internal.CLDCCompatibilityDelegate";
private static final String CDC_COMPATIBILITY_DELEGATE = "org.eclipse.swt.internal.CDCCompatibilityDelegate";

private static CompatibilityDelegate fDelegate;

/**
 * Tests whether the application can read the file denoted by the path.
 *
 * @param path the path string
 * @return true if and only if the file specified by this path exists and can be opened by the application; false otherwise
 * @exception SecurityException
  */
public static boolean canOpenFile(String path)  throws SecurityException {
    return getDelegate().canOpenFile(path);
}

private static CompatibilityDelegate getDelegate() {
    if(fDelegate == null) {
        Class clazz;
        try {
            clazz = Class.forName(CLDC_COMPATIBILITY_DELEGATE);
            fDelegate = (CompatibilityDelegate) clazz.newInstance();
        } catch(Exception e) {

        }
        if(fDelegate == null) {
            try {
                clazz = Class.forName(CDC_COMPATIBILITY_DELEGATE);
                fDelegate = (CompatibilityDelegate) clazz.newInstance();
            } catch(Exception e) {
                throw new RuntimeException("Can not instantiate Compatibility Delegate");
            }
        }
    }
    return fDelegate;
}

/**
 * Answers the length of the side adjacent to the given angle
 * of a right triangle. In other words, it returns the integer
 * conversion of length * cos (angle).
 * <p>
 * IMPORTANT: the j2me version has an additional restriction on
 * the argument. length must be between -32767 and 32767 (inclusive).
 * </p>
 *
 * @param angle the angle in degrees
 * @param length the length of the triangle's hypotenuse
 * @return the integer conversion of length * cos (angle)
 */
public static int cos(int angle, int length) {
    return sin(90 - angle, length);
}

/**
 * Answers the length of the side opposite to the given angle
 * of a right triangle. In other words, it returns the integer
 * conversion of length * sin (angle).
 * <p>
 * IMPORTANT: the j2me version has an additional restriction on
 * the argument. length must be between -32767 and 32767 (inclusive).
 * </p>
 *
 * @param angle the angle in degrees
 * @param length the length of the triangle's hypotenuse
 * @return the integer conversion of length * sin (angle)
 */
public static int sin(int angle, int length) {
    if (length < -32767 || length > 32767) {
        SWT.error(SWT.ERROR_INVALID_RANGE);
    }
    if (angle < 0 || angle >= 360) {
        angle = angle % 360;
        if (angle < 0) angle += 360;
    }

    int sineValue;
    if (angle >= 0 && angle < 90) sineValue = sineTable[angle];
    else if (angle >= 90 && angle < 180) sineValue = sineTable[180 - angle];
    else if (angle >= 180 && angle < 270) sineValue = 0 - sineTable[angle - 180];
    else {
        // angle >= 270 && angle < 360
        sineValue = 0 - sineTable[360 - angle];
    }

    return (sineValue * length) >> 16;
}

/**
 * Answers the most negative (i.e. closest to negative infinity)
 * integer value which is greater than the number obtained by dividing
 * the first argument p by the second argument q.
 *
 * @param p numerator
 * @param q denominator (must be different from zero)
 * @return the ceiling of the rational number p / q.
 */
public static int ceil(int p, int q) {
    int res = p / q;
    if ((p % q == 0) ||
        (res < 0) ||
        ((res == 0) && ((p < 0 && q > 0) || (p > 0 && q < 0))))
        return res;
    else
        return res + 1;
}

/**
 * Answers the most positive (i.e. closest to positive infinity)
 * integer value which is less than the number obtained by dividing
 * the first argument p by the second argument q.
 *
 * @param p numerator
 * @param q denominator (must be different from zero)
 * @return the floor of the rational number p / q.
 */
public static int floor(int p, int q) {
    int res = p / q;
    if ((p % q == 0) ||
        (res > 0) ||
        ((res == 0) && ((p > 0 && q > 0) || (p < 0 && q < 0))))
        return res;
    else
        return res - 1;
}

/**
 * Answers the result of rounding to the closest integer the number obtained
 * by dividing the first argument p by the second argument q.
 * <p>
 * IMPORTANT: the j2me version has an additional restriction on
 * the arguments. p must be within the range 0 - 32767 (inclusive).
 * q must be within the range 1 - 32767 (inclusive).
 * </p>
 *
 * @param p numerator
 * @param q denominator (must be different from zero)
 * @return the closest integer to the rational number p / q
 */
public static int round(int p, int q) {
    if (p < 0 || p > 32767 || q < 1 || q > 32767) {
        SWT.error(SWT.ERROR_INVALID_RANGE);
    }
    return (2 * p + q) / (2 * q);
}

/**
 * Returns 2 raised to the power of the argument.
 *
 * @param n an int value between 0 and 30 (inclusive)
 * @return 2 raised to the power of the argument
 *
 * @exception IllegalArgumentException <ul>
 *    <li>ERROR_INVALID_RANGE - if the argument is not between 0 and 30 (inclusive)</li>
 * </ul>
 */
public static int pow2(int n) {
    if (n >= 1 && n <= 30)
        return 2 << (n - 1);
    else if (n != 0) {
        SWT.error(SWT.ERROR_INVALID_RANGE);
    }
    return 1;
}

/**
 * Create an DeflaterOutputStream if such things are supported.
 *
 * @param stream the output stream
 * @return a deflater stream or <code>null</code>
 * @exception IOException
 *
 * @since 3.4
 */
public static OutputStream newDeflaterOutputStream(OutputStream stream) throws IOException {
    return null;
}

/**
 * Open a file if such things are supported.
 *
 * @param filename the name of the file to open
 * @return a stream on the file if it could be opened.
 */
public static InputStream newFileInputStream(String filename) throws IOException {
    return getDelegate().getInputStreamFromFile(filename);
}

/**
 * Open a file if such things are supported.
 *
 * @param filename the name of the file to open
 * @return a stream on the file if it could be opened.
 */
public static OutputStream newFileOutputStream(String filename) throws IOException {
    return getDelegate().getOutStreamFromFile(filename);
}

/**
 * Create an InflaterInputStream if such things are supported.
 *
 * @param stream the input stream
 * @return a inflater stream or <code>null</code>
 * @exception IOException
 *
 * @since 3.3
 */
public static InputStream newInflaterInputStream(InputStream stream) throws IOException {
    return null;
}

/**
 * Answers whether the character is a letter.
 *
 * @param c the character
 * @return true when the character is a letter
 */
public static boolean isLetter(char c) {
    return (Character.isUpperCase(c)) || (Character.isLowerCase(c));
}

/**
 * Answers whether the character is a letter or a digit.
 *
 * @param c the character
 * @return true when the character is a letter or a digit
 */
public static boolean isLetterOrDigit(char c) {
    return isLetter(c) || Character.isDigit(c);
}

/**
 * Answers whether the character is a Unicode space character.
 *
 * @param c  the character
 * @return true when the character is a Unicode space character
 */
public static boolean isSpaceChar(char c) {
    return c == ' ';
}

/**
 * Answers whether the character is whitespace character.
 *
 * @param c the character
 * @return true when the character is a whitespace character
 */
public static boolean isWhitespace(char c) {
    // Optimized case for ASCII
    if ((c >= 0x1c && c <= 0x20) || (c >= 0x9 && c <= 0xd))
        return true;
    return false;
}

/**
 * Execute a program in a separate platform process if the
 * underlying platform support this.
 * <p>
 * The new process inherits the environment of the caller.
 * </p>
 *
 * @param program the name of the program to execute
 *
 * @exception IOException
 *  if the program cannot be executed
 * @exception SecurityException
 *  if the current SecurityManager disallows program execution
 */
public static void exec(String prog) throws java.io.IOException {
    throw new IOException();
}

/**
 * Answers whether the indicated file exists or not.
 *
 * @param parent the file's parent directory
 * @param child the file's name
 * @return true if the file exists
 */
public static boolean fileExists(String parent, String child) {
    return false;
}

/**
 * Execute progArray[0] in a separate platform process if the
 * underlying platform support this.
 * <p>
 * The new process inherits the environment of the caller.
 * <p>
 *
 * @param progArray array containing the program to execute and its arguments
 *
 * @exception IOException
 *  if the program cannot be executed
 * @exception   SecurityException
 *  if the current SecurityManager disallows program execution
 */
public static void exec(String[] progArray) throws java.io.IOException{
    throw new IOException();
}

/**
 * Returns the NLS'ed message for the given argument. This is only being
 * called from SWT.
 *
 * @param key the key to look up
 * @return the message for the given key
 *
 * @see SWT#getMessage
 */
public static String getMessage(String key) {

	if (key == null) SWT.error (SWT.ERROR_NULL_ARGUMENT);

    // ok button in dialog
    if(key.equals("ok")){
    	iRes = ResourceLoader.getInstance("common", "txt_common_button_");
   		return iRes.format(key).toString();
    }
    
    // cancel button in dialog
    if(key.equals("cancel")){
    	iRes = ResourceLoader.getInstance("common", "txt_common_button_");
   		return iRes.format(key).toString();
    }
    
    // fetch contact from device phone book - in Options menu
    if(key.equals("opt_fetch")){
    	iRes = ResourceLoader.getInstance("javauitoolkits", "txt_java_");
   		return iRes.format(key).toString();
    }

    // fetch contact from device phone book - in context menu
    if(key.equals("menu_fetch")){
    	iRes = ResourceLoader.getInstance("javauitoolkits", "txt_java_");
   		return iRes.format(key).toString();
    }
    
    // fetch contact from device phone book - in button
    if(key.equals("button_fetch")){
    	iRes = ResourceLoader.getInstance("javauitoolkits", "txt_java_");
   		return iRes.format(key).toString();
    }
    
    // create a call - in Options menu
    if(key.equals("opt_call")){
    	iRes = ResourceLoader.getInstance("javauitoolkits", "txt_java_");
   		return iRes.format(key).toString();
    }

    // create a call - in context menu
    if(key.equals("menu_call")){
    	iRes = ResourceLoader.getInstance("javauitoolkits", "txt_java_");
   		return iRes.format(key).toString();
    }
    
    // create a call - in button
    if(key.equals("button_call")){
    	iRes = ResourceLoader.getInstance("javauitoolkits", "txt_java_");
   		return iRes.format(key).toString();
    }
    
    // title of selection list of bookmarks in TextExtension
    if(key.equals("title_select_bookmark")){
    	iRes = ResourceLoader.getInstance("javauitoolkits", "txt_java_");
   		return iRes.format(key).toString();
   	}
   		
   	// string displayed in empty list
    if(key.equals("info_no_data")){
    	iRes = ResourceLoader.getInstance("javauitoolkits", "txt_java_");
   		return iRes.format(key).toString();
    }
 		
    return key;
}

public static String getMessage(String key, Object[] args) {
    if (key == null) SWT.error (SWT.ERROR_NULL_ARGUMENT);
    if (key.equals("SWT_Download_File")) return "Download: "+args[0]; //$NON-NLS-1$
    if (key.equals("SWT_Download_Location")) return "Saving "+args[0]+" from "+args[1]; //$NON-NLS-1$
    if (key.equals("SWT_Download_Status")) return "Download: "+args[0]+" KB of "+args[1]+" KB"; //$NON-NLS-1$
    return key;
}

/**
 * Interrupt the current thread.
 * <p>
 * Note that this is not available on CLDC.
 * </p>
 */
public static void interrupt() {
}

/**
 * Compares two instances of class String ignoring the case of the
 * characters and answers if they are equal.
 *
 * @param s1 string
 * @param s2 string
 * @return true if the two instances of class String are equal
 */
public static boolean equalsIgnoreCase(String s1, String s2) {
    if (s1 == s2) return true;
    if (s2 == null || s1.length() != s2.length()) return false;

    char[] cArray1 = s1.toCharArray();
    char[] cArray2 = s2.toCharArray();
    int length = s1.length();
    char c1, c2;

    for (int index = 0; index < length; index++) {
        c1 = cArray1[index];
        c2 = cArray2[index];
        if (c1 != c2 &&
            Character.toUpperCase(c1) != Character.toUpperCase(c2) &&
            Character.toLowerCase(c1) != Character.toLowerCase(c2)) {
            return false;
        }
    }
    return true;
}

}
