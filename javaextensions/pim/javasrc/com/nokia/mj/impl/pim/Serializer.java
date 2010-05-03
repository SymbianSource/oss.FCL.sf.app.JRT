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
* Description:  Implements PIM Item Serialization functionalities
 *
*/


// PACKAGE
package com.nokia.mj.impl.pim;

// IMPORTS
import javax.microedition.pim.Contact;
import javax.microedition.pim.Event;
import javax.microedition.pim.PIM;
import javax.microedition.pim.PIMException;
import javax.microedition.pim.PIMItem;
import javax.microedition.pim.RepeatRule;
import javax.microedition.pim.ToDo;
import java.io.ByteArrayOutputStream;
import java.io.InputStream;
import java.io.IOException;
import java.io.UnsupportedEncodingException;
import java.util.Vector;
import com.nokia.mj.impl.pim.ErrorString;
import com.nokia.mj.impl.pim.GenericException;
import com.nokia.mj.impl.rt.support.Finalizer;
import com.nokia.mj.impl.rt.support.ShutdownListener;
import com.nokia.mj.impl.rt.support.ApplicationUtils;
import com.nokia.mj.impl.pim.utils.NativeError;


/**
 * Serializer.java
 *
 * Implements PIMItem serialization.
 *
 * Created: Mon Mar 08 14:34:55 2004
 *
 */
final class Serializer
{

    /** The only supported encoding for versit objects */
    private static final String[] PIM_SUPPORTED_ENCODINGS = { "UTF-8",
            "ISO-8859-1"
                                                            };



    /** Beginning of a begin tag */
    private static final String PIM_BEGIN = "BEGIN";

    /** Beginning of an end tag */
    private static final String PIM_END = "END";

    /** colon */
    private static final byte PIM_COLON = ':';

    /** space */
    private static final byte PIM_SPACE = 32;

    /** tab */
    private static final byte PIM_TAB = 9;

    /** End of vcard tag */
    private static final String PIM_VCARD = "VCARD";

    /** End of vcalendar tag */
    private static final String PIM_VCALENDARD = "VCALENDAR";

    /** End of event tag */
    private static final String PIM_EVENT = "EVENT";

    /** End of todo tag */
    private static final String PIM_TODO = "TODO";

    /** Beginning of a vCard */
    private static final String PIM_BEGIN_VCARD = "BEGIN:VCARD";

    /** Beginning of a vCalendar */
    private static final String PIM_BEGIN_VCALENDAR = "BEGIN:VCALENDAR";

    /** End of a vCard */
    private static final String PIM_END_VCARD = "END:VCARD";

    /** End of a vCalendar */
    private static final String PIM_END_VCALENDAR = "END:VCALENDAR";

    private Finalizer iFinalizer;

    private int iSerializerHandle;

    Serializer(int aManagerHandle)
    {
        int[] err = new int[1];
        iSerializerHandle = _createSerializer(aManagerHandle, err);
        if (!NativeError.checkSuccess(err[0]))
        {
            throw new GenericException(ErrorString.GENERAL_ERROR_COLON + err[0]);
        }
        iFinalizer = registerForFinalization();
        setShutdownListener();
    } // Serializer constructor

    public Finalizer registerForFinalization()
    {
        return new Finalizer()
        {
            public void finalizeImpl()
            {
                doFinalize();
            }
        };
    }

    void doFinalize()
    {
        if (iFinalizer == null)
        {
            return;
        }
        iFinalizer = null;
        if (iSerializerHandle != 0)
        {
            _dispose(iSerializerHandle);
            iSerializerHandle = 0;
        }
    }

    /**
     * Registers for shutdown listener
     */
    private void setShutdownListener()
    {
        // Get the insatnce of ApplicationUtils.
        ApplicationUtils appUtils = ApplicationUtils.getInstance();

        // Get the name of the application.
        appUtils.addShutdownListener(new ShutdownListener()
        {
            public void shuttingDown()
            {

                if (iSerializerHandle != 0)
                {
                    _dispose(iSerializerHandle);
                    iSerializerHandle = 0;
                }
            }

        });
    }

    PIMItem[] fromSerialFormat(java.io.InputStream aIs, String aEnc)
    throws PIMException, UnsupportedEncodingException
    {
        if (null == aIs)
        {
            throw new java.lang.NullPointerException();
        }
        int encoding = checkEncoding(aEnc);

        byte[] buffer = null;
        try
        {
            buffer = readLine(aIs, null);
        }
        catch (java.io.IOException e)
        {
            throw new PIMException(ErrorString.STREAM_ERROR_COLON + e.getMessage(),
                                   PIMException.GENERAL_ERROR);
        } // end of try-catch
        if (null == buffer)
        {
            throw new PIMException(ErrorString.DATA_SERIALIZATION_FAILED_COLON +
                                   ErrorString.NOT_A_COMPLETE_ENTRY,
                                   PIMException.GENERAL_ERROR);

        } // end of if (null == buffer)

        String readString = new String(buffer);
        if (readString.regionMatches(true, 0, PIM_BEGIN_VCARD, 0,
                                     PIM_BEGIN_VCARD.length()))
        {
            return fromVCard(readString, aIs, encoding);
        }
        else if (readString.regionMatches(true, 0, PIM_BEGIN_VCALENDAR, 0,
                                          PIM_BEGIN_VCALENDAR.length()))
        {
            return fromVCalendar(readString, aIs, encoding);
        }
        else
        {
            throw new PIMException(ErrorString.DATA_SERIALIZATION_FAILED_COLON +
                                   ErrorString.NOT_VCARD_OR_VCALENDAR);
        } // end of else
    }

    void toSerialFormat(PIMItem aItem, java.io.OutputStream aOs, String aEnc,
                        String aDataFormat) throws PIMException, UnsupportedEncodingException
    {
        if ((aOs == null) || (aDataFormat == null) || (aItem == null))
        {
            throw new java.lang.NullPointerException();
        } // end of if ()

        int encoding = checkEncoding(aEnc);

        byte[] buffer = null;
        int[] error = new int[1];

        PIMItemImpl item = createPIMItemImpl(aItem);

        testSupportedFormat(aDataFormat, item);
        buffer = _toSerialFormat(iSerializerHandle, item.jniNativeHandle(),
                                 encoding, error);
        NativeError.handleToSerialFormatError(error[0]);
        try
        {
            aOs.write(buffer);
        }
        catch (java.io.IOException e)
        {
            throw new PIMException(ErrorString.STREAM_ERROR_COLON + e.getMessage(),
                                   PIMException.GENERAL_ERROR);
        } // end of try-catch
    }

    String[] supportedSerialFormats(int aPimListType)
    {
        // CONTACT_LIST is the smallest list (number) and TODO_LIST biggest
        // EVENT_LIST is in the between
        if ((aPimListType < PIM.CONTACT_LIST) || (aPimListType > PIM.TODO_LIST))
        {
            throw new java.lang.IllegalArgumentException(ErrorString.INVALID_LIST_TYPE_COLON +
                    aPimListType);
        }
        int[] error = new int[1];
        String[] retVal = _supportedSerialFormats(iSerializerHandle,
                          aPimListType, error);
        if (!NativeError.checkSuccess(error[0]))
        {
            throw new OutOfMemoryError(ErrorString.GENERAL_ERROR_COLON + error[0]);
        }
        return retVal;
    }

    /**
     * Creates our implementation of the item, if necessary. The given item
     * is already ours, it is not necessary to create a copy.
     *
     * @param aItem
     *            Item to copy
     * @return Our implementation of the item.
     */
    private PIMItemImpl createPIMItemImpl(PIMItem aItem) throws PIMException
    {
        PIMItemImpl retVal = null;
        if (aItem instanceof PIMItemImpl)
        {
            // copy is not needed
            retVal = (PIMItemImpl) aItem;
        }
        else if (aItem instanceof Contact)
        {
            ContactImpl contact = new ContactImpl();
            contact.copyFrom((Contact) aItem);
            retVal = contact;
        } // end of else if (aItem instanceof Contact)
        else if (aItem instanceof Event)
        {
            EventImpl event = new EventImpl();
            event.copyFrom((Event) aItem);
            retVal = event;
        }
        else if (aItem instanceof ToDo)
        {
            ToDoImpl todo = new ToDoImpl();
            todo.copyFrom((ToDo) aItem);
            retVal = todo;
        } // end of else if (aItem instanceof ToDo)
        else
        {
            throw new PIMException(ErrorString.DATA_SERIALIZATION_FAILED_COLON +
                                   ErrorString.NOT_SUPPORTED_ITEM,
                                   PIMException.GENERAL_ERROR);
        } // end of else
        return retVal;
    }

    private PIMItem[] fromVCard(String aFirstLine, java.io.InputStream aIs,
                                int aEncoding) throws PIMException
    {
        // this is a vCard
        boolean completeVCard = false;
        int sizeOfVCard = 0;
        String readString = aFirstLine;
        byte[] buffer = readString.getBytes();
        int[] error = new int[1];
        while (!completeVCard)
        {
            sizeOfVCard = lengthOfVCard(new String(buffer));
            if (sizeOfVCard == -1)
            {
                // read more
                // vCard can end at EOF
                try
                {
                    byte[] newBuffer = readLine(aIs, buffer);
                    if (newBuffer == null)
                    {
                        completeVCard = true;
                        sizeOfVCard = buffer.length;
                    }
                    else
                    {
                        buffer = newBuffer;
                    }
                }
                catch (java.io.IOException e)
                {
                    throw new PIMException(ErrorString.STREAM_ERROR_COLON + e.getMessage(),
                                           PIMException.GENERAL_ERROR);
                } // end of try-catch
            }
            else
            {
                // there was a complete vCard
                completeVCard = true;
            } // end of else
        }
        // go native..
        int[] itemHandles = _fromSerialFormat(iSerializerHandle, buffer,
                                              sizeOfVCard, aEncoding, error);
        NativeError.handleSerializationError(error[0]);
        // after native and error check..
        int itemCount = itemHandles.length / 2;
        PIMItem[] itemArray = new PIMItem[itemCount];
        for (int i = 0; i < itemCount; i++)
        {
            if (itemHandles[i * 2] != PIM.CONTACT_LIST)
            {
                throw new OutOfMemoryError(ErrorString.NOT_CONTACT);
            } // end of if (itemHandles[ i*2 ] != PIM.CONTACT_LIST)
            itemArray[i] = new ContactImpl(

                itemHandles[i * 2 + 1]);
        } // end of for (int i = 0; i < itemCount; i++)
        return itemArray;
    }

    private PIMItem[] fromVCalendar(String aFirstLine, java.io.InputStream aIs,
                                    int aEncoding) throws PIMException
    {
        // this is a vCalendar
        boolean endFound = false;
        String readString = aFirstLine;
        byte[] buffer = readString.getBytes();
        int[] error = new int[1];
        while (!endFound)
        {
            int indexOfEnd = readString.indexOf(PIM_END_VCALENDAR);
            if (indexOfEnd != -1)
            {
                endFound = true;
            }
            else
            {
                // vCalendar must have an end; error is at end of stream
                try
                {
                    byte[] newBuffer = readLine(aIs, buffer);
                    if (newBuffer == null)
                    {
                        // EOF
                        throw new PIMException(ErrorString.NOT_A_COMPLETE_ENTRY,
                                               PIMException.GENERAL_ERROR);
                    } // end of if (nextByte == -1)
                    buffer = newBuffer;
                }
                catch (java.io.IOException e)
                {
                    throw new PIMException(ErrorString.STREAM_ERROR_COLON + e.getMessage(),
                                           PIMException.GENERAL_ERROR);
                } // end of try-catch
                readString = new String(buffer);
            } // end of else
        } // end of while (!endFound)

        // go native..
        int[] itemHandles = _fromSerialFormat(iSerializerHandle, buffer,
                                              buffer.length, aEncoding, error);
        NativeError.handleSerializationError(error[0]);

        // after native and error check..
        int handleCount = itemHandles.length / 2;
        Vector itemVector = new Vector(handleCount);

        for (int i = 0; i < handleCount; i++)
        {
            switch (itemHandles[i * 2])
            {
            case PIM.EVENT_LIST:
            {
                // must create a repeat rule first
                // repeat rule handle is in position i*2+2
                // See the description of _fromSerialFormat()
                int repeatRuleHandle = itemHandles[i * 2 + 2];
                RepeatRule repeatRule = RepeatRuleInvoker.createRepeatRule(

                                            repeatRuleHandle);

                itemVector.addElement(new EventImpl(itemHandles[i * 2 + 1],
                                                    repeatRule));
                i++;
                break;
            }

            case PIM.TODO_LIST:
            {
                itemVector.addElement(new ToDoImpl(itemHandles[i * 2 + 1]));
                break;
            }
            default:
            {
                throw new PIMException();
            }
            } // end of switch (itemHandles[ i*2 ])
        }
        int itemCount = itemVector.size();
        PIMItem[] itemArray = new PIMItem[itemCount];
        for (int i = 0; i < itemCount; i++)
        {
            itemArray[i] = (PIMItem) itemVector.elementAt(i);
        }
        return itemArray;
    }

    /**
     * Calculates the length of the first vCard in the string.
     *
     * @param aString
     *            searched for vCard
     * @return length of vCard, -1 if string is not a complete vCard
     */
    private int lengthOfVCard(String aString)
    {
        int endCount = 0;
        // we can safely assume that there is a begin in the beginning
        int beginCount = 1;
        int indexOfBegin = 0;
        int indexOfEnd = 0;
        while (endCount < beginCount)
        {
            // read one more end

            indexOfEnd = aString.indexOf(PIM_END_VCARD, indexOfEnd + 1);

            if (indexOfEnd == -1)
            {
                // end of string

                return -1;
            } // end of if (indexOfEnd == -1)
            endCount++;
            indexOfBegin = aString.indexOf(PIM_BEGIN_VCARD, indexOfBegin + 1);
            if ((indexOfBegin != -1) && (indexOfBegin < indexOfEnd))
            {
                beginCount++;
            }
        } // end of while (endCount != beginCount)
        int vCardLength = indexOfEnd + PIM_END_VCARD.length();

        return vCardLength;
    }

    /**
     * Reads a single line and appends it to the byte array. in the case of end
     * delimiter, the line may not be fully read, as the reading stops at the
     * end of the delimiter.
     *
     * @param aIs
     *            Stream to read from
     * @param aReadSoFar
     *            Array of bytes already read. This is appended to the beginning
     *            of the returned array.
     * @returns array of bytes read. Includes bytes read before this call, and
     *          bytes read from this line.
     */
    private byte[] readLine(InputStream aIs, byte[] aReadSoFar)
    throws IOException
    {
        ByteArrayOutputStream os = null;
        ByteArrayOutputStream lastLine = new ByteArrayOutputStream();
        if (aReadSoFar != null)
        {
            os = new ByteArrayOutputStream(aReadSoFar.length);
            os.write(aReadSoFar, 0, aReadSoFar.length);
        } // end of if (aReadSoFar != null)
        else
        {
            os = new ByteArrayOutputStream();
        } // end of else

        int readByte = aIs.read();
        if (readByte == -1)
        {
            return null;
        } // end of if (readByte == -1)
        lastLine.write(readByte);
        byte[] lastLineBytes = null;
        // 10 = end of line
        while (readByte != 10)
        {
            readByte = aIs.read();
            if (readByte == -1)
            {
                lastLineBytes = stripWhiteSpaces(lastLine.toByteArray());
                break;
            } // end of if (readByte == -1)
            lastLine.write(readByte);
            lastLineBytes = stripWhiteSpaces(lastLine.toByteArray());
            String lastLineString = new String(stripWhiteSpaces(lastLineBytes));
            if (lastLineString.equals(PIM_END_VCARD))
            {
                break;
            }
            if (lastLineString.equals(PIM_END_VCALENDAR))
            {
                break;
            }
        }

        os.write(lastLineBytes);

        return os.toByteArray();
    }

    private byte[] stripWhiteSpaces(byte[] aString) throws java.io.IOException
    {
        // 3 = Lenght of END
        if (aString.length < 3)
        {
            return aString;
        } // end of if (aString.length < 3)

        // check if beginning matches with BEGIN or END
        boolean isBegin = false;
        boolean isEnd = false;
        String beginning = new String(aString, 0, 3);
        if (beginning.equals(PIM_END))
        {
            isEnd = true;
        } // end of if (beginning.equals( PIM_END ))
        else
        {
            // 5 = Length of BEGIN
            if (aString.length < 5)
            {
                return aString;
            } // end of if (aString.length < 5)

            beginning = new String(aString, 0, 5);
            if (beginning.equals(PIM_BEGIN))
            {
                isBegin = true;
            } // end of if
        } // end of else

        if ((!isBegin) && (!isEnd))
        {
            return aString;
        } // end of if (( !isBeginning ) && ( !isEnd ))

        ByteArrayOutputStream os = new ByteArrayOutputStream();
        os.write(beginning.getBytes());
        int pos = beginning.length();
        // skip whitespaces
        while (pos < aString.length)
        {
            if ((aString[pos] == PIM_SPACE) || (aString[pos] == PIM_TAB))
            {
                pos++;
                continue;
            }
            if (aString[pos] == PIM_COLON)
            {
                break; // out of the while loop
            } // end of if (aString[ pos ] == PIM_COLON)
            // not ws or colon
            return aString;
        } // end of while (aString[ pos ] != PIM_COLON)
        // write colon
        pos++;
        os.write(PIM_COLON);
        // skip whitespaces
        while ((pos < aString.length)
                && ((aString[pos] == PIM_SPACE) || (aString[pos] == PIM_TAB)))
        {
            pos++;
        }
        if (!(pos < aString.length))
        {
            return aString;
        } // end of if (!( pos < aString.length ))

        os.write(aString, pos, aString.length - pos);
        // check if end matches with any know tag endings
        String end = new String(aString, pos, aString.length - pos);

        if (end.equals(PIM_VCARD))
        {
            return os.toByteArray();
        } // end of if
        if (end.equals(PIM_VCALENDARD))
        {
            return os.toByteArray();
        } // end of if
        if (end.equals(PIM_EVENT))
        {
            return os.toByteArray();
        } // end of if
        if (end.equals(PIM_TODO))
        {
            return os.toByteArray();
        } // end of if
        return aString;
    }

    /**
     * Checks if the given format is supported for the given item type.
     *
     * @param aFormat
     *            format to check
     * @param aItem
     *            item which type is checked
     */
    private void testSupportedFormat(String aFormat, PIMItemImpl aItem)
    {
        int listType = aItem.getItemType();
        String[] supportedFormats = supportedSerialFormats(listType);
        for (int i = 0; i < supportedFormats.length; i++)
        {
            if (aFormat.equals(supportedFormats[i]))
            {
                return;
            }
        } // end of for (int i = 0; i < supportedFormats.length; i++)
        throw new IllegalArgumentException(ErrorString.UNSUPPORTED_FORMAT_COLON + aFormat);
    }

    /**
     * Checks if the given encodinf is supported.
     *
     * @param aEnc
     *            encoding to check
     * @return int value to be given to the native side
     * @exception java.io.UnsupportedEncodingException
     *                given encoding is not supported.
     */
    private int checkEncoding(String aEnc)
    throws java.io.UnsupportedEncodingException
    {
        int encoding = 0;
        if (null != aEnc)
        {
            boolean supportedEncoding = false;
            for (int i = 0; i < PIM_SUPPORTED_ENCODINGS.length; i++)
            {
                if (aEnc.equals(PIM_SUPPORTED_ENCODINGS[i]))
                {
                    encoding = i;
                    supportedEncoding = true;
                } // end of if (aEnc.equals( PIM_SUPPORTED_ENCODINGS[ i ] ))
            } // end of for
            if (!supportedEncoding)
            {
                throw new java.io.UnsupportedEncodingException();
            } // end of if (!supportedEncoding)
        }
        return encoding;
    }

    /**
     * Creates Serializer native side.
     *
     * @param aManagerHandle
     *            Handle to native side PIMManager.
     *
     * @return Handle to native side Serializer.
     */
    private native int _createSerializer(int aManagerHandle, int[] aError);

    /**
     * Disposes the native side Serializer.
     *
     * @param aEventSourceHandle
     *            Handle to native side Event Source.
     * @param aSerializerHandle
     *            Handle to native side Serializer.
     */
    private native void _dispose(int aSerializerHandle);

    /**
     * Returns an array of item handles. The "protocol" is as follows: The
     * handles are grouped in groups of two (or four) integers in the array.
     * Contact and Todo items have two integers in a group, while events have
     * four. in the group the first integer specifies the type of the item
     * (CONTACT_LIST, EVENT_LIST or TODO_LIST). The second integer is the actual
     * handle to the item. Event items are a special case: if the type is event,
     * the third integer is the handle to the repeat rule. The fourth integer is
     * currently empty, it just aligns the next group to two integer boundary.
     * The fourth integer may be used in the future for some meaningful
     * information.
     */
    private native int[] _fromSerialFormat(int aManagerHandle, byte[] aBuffer,
                                           int aSizeOfEntry, int aEncoding, int[] aError);

    private native byte[] _toSerialFormat(int aManagerHandle, int aItemHandle,
                                          int aEncoding, int[] aError);

    private native String[] _supportedSerialFormats(int aManagerHandle,
            int aPimListType, int[] aError);

} // Serializer
