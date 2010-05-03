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
* Description:
*
*/


package com.nokia.mj.impl.mms;

import java.io.InputStream;
import java.io.IOException;
import java.io.ByteArrayInputStream;
import java.io.ByteArrayOutputStream;

import javax.wireless.messaging.MessagePart;
import javax.wireless.messaging.SizeExceededException;

import com.nokia.mj.impl.utils.Logger;
import com.nokia.mj.impl.mms.MmsPropertyRetriever;
/**
 * Implementation of the MessagePart interface defined in JSR205. A message part
 * (attachment) contains: the following fields/attributes: .mime type .content
 * id .content location .content encoding .data
 */
public final class MessagePartImpl
{
    /**
     * The mime type of the attachment. This is a MANDATORY field of an
     * attachment. In this implementation the mime type is considered as valid
     * if it contains at least the "/" character
     */
    private String iMimeType = null;

    /**
     * The content identifier. The content identifier is a MANDATORY field of an
     * attachment. In this implementation the content identifier is considered
     * as valid if it contains only US-ASCII characters
     */
    private String iContentId = null;

    /**
     * The content location. It specifies the file name of the file that
     * contains the data of the attachment. The content location is an OPTIONAL
     * field of an attachment. If provided, the content location is considered
     * as valid if it contains only US-ASCII characters
     */
    private String iContentLocation = null;

    /**
     * The content encoding. This is relevant information for the receiver of
     * the attachment. It specifies how the data should be interpreted. This
     * content encoding is an OPTIONAL field of the attachment
     */
    private String iContentEncoding = null;

    /**
     * The content. It contains a byte array representing the data/content of
     * the attachment. The content is a MANDATORY field of the attachment. The
     * content can be provided directly (as a byte array) or it can be read from
     * an input stream
     */
    private byte[] iContent = null;

    /**
     * Factory for creating instances of MessagePart class
     */
    private static MessagePartFactory iMessagePartFactory = null;

    /**
     * Max length for contentLocation
     */
    private static final int MAX_CONTENT_LOCATION_LENGTH = 256;

    /**
     * Max cummulated length for attachment headers (mime, contentId and
     * contentLocation)
     */
    private static final int MAX_ATTCH_HEADERS_LENGTH = 10000;

    /**
     * Method for registering a message part factrory. There can only be one
     * message part factory
     *
     * @param aMessagePartFactory
     *            The factory to be registered
     */
    public static void registerFactory(MessagePartFactory aMessagePartFactory)
    {
        if (aMessagePartFactory != null && iMessagePartFactory == null)
        {
            iMessagePartFactory = aMessagePartFactory;
        }
    }

    /**
     * Empty constructor
     *
     */
    public MessagePartImpl()
    {
        Logger.LOG(Logger.EWMA, Logger.EInfo,
                   "+ MessagePartImpl::MessagePartImpl()");
        if (iMessagePartFactory == null)
        {
            // we shouldn't get here, but just as a double safety measure,
            // we give hints to the MessagePart that it should register
            // the factory
            try
            {
                Class.forName("javax.wireless.messaging.MessagePart");
            }
            catch (ClassNotFoundException e)
            {
                // ignore it, it can't happen
            }
        }
        Logger.LOG(Logger.EWMA, Logger.EInfo,
                   "- MessagePartImpl::MessagePartImpl()");
    }

    /**
     * Constructor
     *
     * @param aInputStream
     *            The input stream from where the content could be obtaied
     * @param aMimeType
     *            The mime type of the attachment
     * @param aContentId
     *            The content identifier
     * @param aContentLocation
     *            The content location
     * @param aEncoding
     *            The encoding applied to the content
     * @throws IOException
     *             If there are some problems with reading the content from the
     *             provided input stream
     * @throws SizeExceededException
     *             If the provided content is larger than the available memory
     *             or supported size of the attachment
     */
    public MessagePartImpl(InputStream aInputStream, String aMimeType,
                           String aContentId, String aContentLocation,
                           String aEncoding) throws IOException,
                SizeExceededException
    {
        Logger.LOG(Logger.EWMA, Logger.EInfo,
                   "+ MessagePartImpl::MessagePartImpl()");
        // get the content into an array first
        ByteArrayOutputStream oStream = new ByteArrayOutputStream();
        if (aInputStream != null)
        {
            byte buf[] = new byte[1024];
            int length = aInputStream.read(buf, 0, 1024);
            while (length != -1)
            {
                oStream.write(buf, 0, length);
                length = aInputStream.read(buf, 0, 1024);
            }
        }
        byte[] contents = oStream.toByteArray();
        oStream.close();

        // check the provided fields
        initMessagePart(contents, 0, contents.length, aMimeType, aContentId,
                        aContentLocation, aEncoding);
        Logger.LOG(Logger.EWMA, Logger.EInfo,
                   "- MessagePartImpl::MessagePartImpl()");
    }

    /**
     * Constructor
     *
     * @param aContents
     *            The content/data of the attachment
     * @param aMimeType
     *            The mime type of the attachment
     * @param aContentId
     *            The content identifier
     * @param aContentLocation
     *            The content location
     * @param aEncoding
     *            The encoding applied to the content
     * @throws SizeExceededException
     *             If the provided content is larger than the available memory
     *             or supported size of the attachment
     */
    public MessagePartImpl(byte[] aContents, String aMimeType, String aContentId,
                           String aContentLocation, String aEncoding)
    throws SizeExceededException
    {
        Logger.LOG(Logger.EWMA, Logger.EInfo,
                   "+ MessagePartImpl::MessagePartImpl()");
        // check the provided fields
        initMessagePart(aContents, 0, (aContents == null ? 0 : aContents.length),
                        aMimeType, aContentId, aContentLocation, aEncoding);
        Logger.LOG(Logger.EWMA, Logger.EInfo,
                   "- MessagePartImpl::MessagePartImpl()");
    }

    /**
     * Constructor
     *
     * @param aContents
     *            The byte array containing the content of the attachment
     * @param aOffset
     *            The index within the provided byte array which delimitates the
     *            start of the attachment's content
     * @param aLength
     *            the length of the attachment's content
     * @param aMimeType
     *            The mime type of the attachment
     * @param aContentId
     *            The content identifier
     * @param aContentLocation
     *            The content location
     * @param aEncoding
     *            The encoding applied to the content
     * @throws SizeExceededException
     *             If the provided content is larger than the available memory
     *             or supported size of the attachment
     */
    public MessagePartImpl(byte[] aContents, int aOffset, int aLength,
                           String aMimeType, String aContentId,
                           String aContentLocation, String aEncoding)
    throws SizeExceededException
    {
        // check the provided fields
        initMessagePart(aContents, aOffset, aLength, aMimeType, aContentId,
                        aContentLocation, aEncoding);
    }

    /**
     * Get method for the attachment's content
     *
     * @return A byte array containing the attachment's content
     */
    public byte[] getContent()
    {
        return iContent;
    }

    /**
     * Get method for the attachment's content
     *
     * @return An input stream from where the attachment's content could be read
     */
    public InputStream getContentAsStream()
    {
        ByteArrayInputStream inputStream = new ByteArrayInputStream(iContent);
        return inputStream;
    }

    /**
     * Get method for the attachment's content identifier
     *
     * @return A string containing the attachment's content idenfifier
     */
    public String getContentID()
    {
        return iContentId;
    }

    /**
     * Get method for the attachment's MIME type
     *
     * @return A string containing the attachment's MIME type
     */
    public String getMIMEType()
    {
        return iMimeType;
    }

    /**
     * Get method for the attachment's encoding
     *
     * @return A string containing the encoding of the attachment's data
     */
    public String getEncoding()
    {
        return iContentEncoding;
    }

    /**
     * Get method for the attachment's content location
     *
     * @return A string containing the file name of the file containing the
     *         attachment's data
     */
    public String getContentLocation()
    {
        return iContentLocation;
    }

    /**
     * Get method for the attachment's content length
     *
     * @return An int containing the length of the attachment's content
     */
    public int getLength()
    {
        return ((iContent != null) ? iContent.length : 0);
    }

    /**
     * It implements the comparision between a MessagePart object and either a
     * String object or another MessagePart object.
     *
     * @return A boolean indicating wether the current MessagePart object is
     *         equal to the provided Object or not
     */
    public boolean equals(Object aObj)
    {
        Logger.LOG(Logger.EWMA, Logger.EInfo,
                   "+ MessagePartImpl::equals()");
        if (aObj instanceof String)
        {
            // it's comparison based on contentID
            String ID = (String) aObj;
            String ownID = getContentID();
            if (ownID != null && ownID.equals(ID))
            {
                return true;
            }
        }
        else if (aObj instanceof MessagePart)
        {
            // it's comparison based on other MessagePart
            MessagePart otherPart = (MessagePart) aObj;
            return equals(otherPart.getContentID());
        }
        Logger.LOG(Logger.EWMA, Logger.EInfo,
                   "- MessagePartImpl::equals()");
        // ignore the other objects
        return false;
    }

    /**
     * This method returns the string representation of the attachment. This
     * method is useful for debugging purposes.
     *
     * @return A string containing all the attachment's fields.
     */
    public String toString()
    {
        return "MimeType: " + iMimeType + "\nContentID: " + iContentId
               + "\nContentLocation: " + iContentLocation
               + "\nContentEncoding: " + iContentEncoding + "\nContent: "
               + iContent;
    }

    /**
     * Function that initializes the attachment's fields without any validation
     *
     * @param aContents
     *            The byte array containing the content of the attachment
     * @param aMimeType
     *            The mime type of the attachment
     * @param aContentId
     *            The content identifier
     * @param aContentLocation
     *            The content location
     * @param aEncoding
     *            The encoding applied to the content
     */
    public void initMessagePart(byte[] aContents, String aMimeType,
                                String aContentId, String aContentLocation, String aEncoding)
    {
        Logger.LOG(Logger.EWMA, Logger.EInfo,
                   "+ MessagePartImpl::initMessagePart()");
        iMimeType = aMimeType;
        iContentId = aContentId;
        iContentLocation = aContentLocation;
        iContentEncoding = aEncoding;
        if (aContents == null)
        {
            iContent = null;
        }
        else
        {
            iContent = new byte[aContents.length];
            System.arraycopy(aContents, 0, iContent, 0, aContents.length);
        }
        Logger.LOG(Logger.EWMA, Logger.EInfo,
                   "- MessagePartImpl::initMessagePart()");
    }

    /**
     * Static constructor based on the content ID. This is useful in case of
     * comparision.
     *
     * @param aContentId
     *            The content identifier
     */
    static MessagePartImpl getInstance(String aContentId)
    {
        return new MessagePartImpl(aContentId);
    }

    /**
     * Function which creates a MessagePart instance with the provided
     * parameters
     *
     * @param aContents
     *            The byte array containing the content of the attachment
     * @param aMimeType
     *            The mime type of the attachment
     * @param aContentId
     *            The content identifier
     * @param aContentLocation
     *            The content location
     * @param aEncoding
     *            The encoding applied to the content
     */
    static MessagePart initializeMessagePart(byte[] aContents, String aMimeType,
            String aContentId, String aContentLocation, String aEncoding)
    {
        return iMessagePartFactory.initializeMessagePart(aContents, aMimeType,
                aContentId, aContentLocation, aEncoding);
    }

    /**
     * The private constructor used by the getInstance method
     *
     * @param aContentId
     *            The content identifier
     */
    private MessagePartImpl(String aContentId)
    {
        iContentId = aContentId;
    }

    /**
     * Util function for checking if the provided string contains only US-ASCII
     * charcters or not.
     *
     * @throws IllegalArgumentException
     *             If the provided string contains non US-ASCII characters
     */
    private void isASCII(String aStr)
    {
        for (int i = 0; i < aStr.length(); i++)
        {
            char c = aStr.charAt(i);
            if ((c & 0x00FF) > 127)
            {
                throw new IllegalArgumentException(
                    "String contains non US-ASCII characters" + aStr);
            }
        }
    }

    /**
     * Function that validates the attachment's fields provided in the
     * construction phase
     *
     * @param aContents
     *            The byte array containing the content of the attachment
     * @param aOffset
     *            The index within the provided byte array which delimitates the
     *            start of the attachment's content
     * @param aLength
     *            the length of the attachment's content
     * @param aMimeType
     *            The mime type of the attachment
     * @param aContentId
     *            The content identifier
     * @param aContentLocation
     *            The content location
     * @param aEncoding
     *            The encoding applied to the content
     * @throws SizeExceededException
     *             If the provided content is larger than the available memory
     *             or supported size of the attachment
     */
    private void initMessagePart(byte[] aContents, int aOffset, int aLength,
                                 String aMimeType, String aContentId,
                                 String aContentLocation,String aEncoding)
    throws SizeExceededException
    {
        Logger.LOG(Logger.EWMA, Logger.EInfo,
                   "+ MessagePartImpl::initMessagePart()");
        boolean nullContent = false;
        // check the content
        if (aContents == null)
        {
            // the TCK wants this kind of implementation
            nullContent = true;
            aContents = new byte[0];
        }
        if ((aLength < 0) || (aOffset < 0) || (aOffset + aLength) > aContents.length)
        {
            throw(new IllegalArgumentException("Length " + aLength +
                                               " or offset " + aOffset + " is < 0, or their sum" +
                                               " exceeds the content array length"));
        }

        // check the length of the MessagePart
        if (aContents.length > MmsPropertyRetriever.getMMSMaxSize())
        {
            throw new SizeExceededException(ExceptionStrings.
                                            MESSAGE_PART_SIZE_EXCEEDS);
        }

        // Check mimetype
        if (aMimeType == null)
        {
            // mimeType is a MANDATORY field
            throw(new IllegalArgumentException(ExceptionStrings.
                                               MIME_TYPE_MANDATORY));
        }
        // check the content identifier
        if (aContentId == null)
        {
            throw(new IllegalArgumentException(ExceptionStrings.
                                               CONTENT_ID_MANDATORY));
        }
        isASCII(aContentId);

        int attchHeadersLength = aMimeType.length() + aContentId.length();
        // check content location if present
        if (aContentLocation != null && aContentLocation.length() > 0)
        {
            isASCII(aContentLocation);
            // don't propagate values bigger than 256,
            // as they are not accepted by the underlying MMS engine
            if (aContentLocation.length() > MAX_CONTENT_LOCATION_LENGTH)
            {
                throw new SizeExceededException(ExceptionStrings.
                                                CONTENT_LOCATION_LENGTH_EXCEEDS);
            }
            attchHeadersLength += aContentLocation.length();
        }
        // don't propagate attach headers bigger than 10K as the
        // underlying MMS engine doesn't accept them
        if (attchHeadersLength >= MAX_ATTCH_HEADERS_LENGTH)
        {
            throw new SizeExceededException(ExceptionStrings.HEADER_SIZE_EXCEEDS);
        }

        // check the encoding if present

        if (aEncoding != null && aEncoding.length() > 0)
        {
            if (!MmsPropertyRetriever.isValidEncoding(aEncoding))
            {
                throw new IllegalArgumentException("Unsupported encoding: "
                                                   + aEncoding);
            }
        }

        // Checking succeeded. Go on setting the private variables
        iMimeType = aMimeType;
        iContentId = aContentId;
        iContentLocation = aContentLocation;
        iContentEncoding = aEncoding;
        if (nullContent)
        {
            iContent = null;
        }
        else
        {
            iContent = new byte[aLength];
            System.arraycopy(aContents, aOffset, iContent, 0, aLength);
        }
        Logger.LOG(Logger.EWMA,Logger.EInfo,"- MessagePartImpl::initMessagePart()");
    }

}