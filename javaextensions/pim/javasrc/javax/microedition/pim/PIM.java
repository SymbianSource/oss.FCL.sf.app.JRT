/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:  Abstract PIM manager class.
 *
*/


// PACKAGE
package javax.microedition.pim;

// IMPORTS
import java.io.UnsupportedEncodingException;
import com.nokia.mj.impl.rt.support.Jvm;

// CLASS DEFINITION
/**
 * PIM API 1.0 compliant PIM class. Minor parts of the PIM class implementation
 * are included in this class (mandatory initializations).
 * com.nokia.microedition.pim.PIMManager contains the most of the class
 * implementation.
 */
public abstract class PIM
{
    public static final int CONTACT_LIST = 1;

    public static final int EVENT_LIST = 2;

    public static final int TODO_LIST = 3;

    public static final int READ_ONLY = 1;

    public static final int WRITE_ONLY = 2;

    public static final int READ_WRITE = 3;

    /**
     * @par Implementation notes:
     * @li The repeat rule invoker must be initialized before anything else is
     *     done, for which the PIM class static initialization block suits just
     *     fine.
     */
    static
    {
        try
        {
            Jvm.loadSystemLibrary("javapim");
        }
        catch (Exception e)
        {
        }
        RepeatRuleInvokerImpl.initialize();
    }

    /**
     * @par Implementation notes:
     * @li An singleton instance of com.nokia.microedition.pim.PIMManager is
     *     returned.
     */
    public static PIM getInstance()
    {
        return com.nokia.mj.impl.pim.PIMManager.getInstance();
    }

    /**
     * @par Implementation notes:
     * @li The constructor must not be publicly accessible.
     */
    protected PIM()
    {
    }

    /**
     * @param aPimListType which list user want to open Ex.CONTACT_LIST, EVENT_LIST, or TODO_LIST.
     * @param aMode list open mode  READ_ONLY or  WRITE_ONLY or READ_WRITE
     * @return PIMList
     * @throws javax.microedition.pim.PIMException
     */
    public abstract PIMList openPIMList(int aPimListType, int aMode)
    throws PIMException;

    /**
     *
     * @param aPimListType aPimListType which list user want to open Ex.CONTACT_LIST, EVENT_LIST, or TODO_LIST.
     * @param aMode list open mode  READ_ONLY or  WRITE_ONLY or READ_WRITE
     * @param aName List name
     * @return  PIMList
     * @throws javax.microedition.pim.PIMException
     */
    public abstract PIMList openPIMList(int aPimListType, int aMode,
                                        String aName) throws PIMException;

    /**
     *
     * @param aPimListType  aPimListType which list user want to open Ex.CONTACT_LIST, EVENT_LIST, or TODO_LIST.
     * @return String which contains list of PIMList names
     */
    public abstract String[] listPIMLists(int aPimListType);

    /**
     * @param aIs an inputstream object containing PIM information
     * @param aEnc encoding of the characters in the input stream
     * @return PIMItem Array
     * @throws javax.microedition.pim.PIMException
     * @throws java.io.UnsupportedEncodingException
     */
    public abstract PIMItem[] fromSerialFormat(java.io.InputStream aIs,
            String aEnc) throws PIMException, UnsupportedEncodingException;

    /**
     *
     * @param aItem the item to export
     * @param aOs the OutputStream object that where is written to as a character stream.
     * @param aEnc  encoding of the characters in the input stream
     * @param aDataFormat PIM data format to use
     * @throws javax.microedition.pim.PIMException
     * @throws java.io.UnsupportedEncodingException
     */
    public abstract void toSerialFormat(PIMItem aItem,
                                        java.io.OutputStream aOs, String aEnc, String aDataFormat)
    throws PIMException, UnsupportedEncodingException;

    /**
     *
     * @param aPimListType which list user want to open Ex.CONTACT_LIST, EVENT_LIST, or TODO_LIST.
     * @return String object
     */
    public abstract String[] supportedSerialFormats(int aPimListType);



}
