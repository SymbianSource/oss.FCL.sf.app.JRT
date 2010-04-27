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

import java.util.Date;
import java.util.Vector;
import java.io.IOException;
import java.io.DataInputStream;
import java.io.DataOutputStream;
import java.io.ByteArrayInputStream;
import java.io.ByteArrayOutputStream;

import com.nokia.mj.impl.utils.Logger;

import javax.wireless.messaging.MessagePart;
import javax.wireless.messaging.MultipartMessage;
import javax.wireless.messaging.SizeExceededException;

/**
 * This class offers an implementation of a MultimediaMessage, as it is
 * described/specified by the JSR205.
 *
 * Internally to this class, a MultimediaMessage is made of: .a header
 * (encapsulated in MultipartMessageHeader object) .and a body (encapsulated in
 * MultipartMessageBody object)
 *
 * This class simply forwards the Java calls for managing a MultimediaMessage
 * object (as specified by JSR205) to either the header or to the body.
 *
 * This class implements also the serialization and deserialization of a
 * MultimediaMessage. Serialization and deserialization operations are needed
 * when the MMS message is transferred from Java to Native.
 *
 * @see MultipartMessageHeader.java
 * @see MultipartMessageBody.java
 */
final class MultipartMessageImpl implements MultipartMessage
{
    // first things first: we force the ClassLoader to load MessagePart
    // class, so that it registers the factory
    static
    {
        try
        {
            Class.forName("javax.wireless.messaging.MessagePart");
        }
        catch (ClassNotFoundException e)
        {
            // ignore it, it can't happen
        }
    }

    // the header and the body
    private MultipartMessageHeader iMessageHeader = null;

    private MultipartMessageBody iMessageBody = null;

    // constants used in (de)serialization
    private static final int REPLY_TO_APPLICATION_ID = 0;

    private static final int APPLICATION_ID = 1;

    private static final int SUBJECT = 2;

    private static final int FROM = 3;

    private static final int TO = 4;

    private static final int BCC = 5;

    private static final int CC = 6;

    // timestamp
    private long iTimestamp;

    // boolean indicating if this is a reply message
    boolean iReply = false;

    // boolean indicating if the validation has been done for this message
    boolean iValidationDone = false;

    /**
     * Constructor
     *
     * @param aAddress
     *            The address of the MMS message
     * @param aTimestamp
     *            The time when this MMS message was created
     */
    MultipartMessageImpl(String aAddress, long aTimestamp, int aMaxMMSSize)
    {

        this(aAddress, aTimestamp, aMaxMMSSize, true);
    }

    /**
     * Constructor
     *
     * @param aAddress
     *            The address of the MMS message
     * @param aTimestamp
     *            The time when this MMS message was created
     */
    private MultipartMessageImpl(String aAddress, long aTimestamp,
                                 int aMaxMMSSize, boolean aValidationOn)
    {
        iTimestamp = aTimestamp;
        iMessageBody = new MultipartMessageBody(aMaxMMSSize, aValidationOn);
        iMessageHeader = new MultipartMessageHeader(aValidationOn);
        if (aAddress != null)
        {
            iMessageHeader.setAddress(aAddress);
        }
        iValidationDone = aValidationOn;
    }

    /**
     * Method for adding a message part to the MultimediaMessage object.
     *
     * @param aPart
     *            The message part to be added
     */
    public void addMessagePart(MessagePart aPart) throws SizeExceededException
    {
        iMessageBody.addMessagePart(aPart);
    }

    /**
     * Method for removing a certain message part
     *
     * @param aContentID
     *            The content identifier of the message part to be removed
     * @return A boolean indicating the result of the removal
     */
    public boolean removeMessagePartId(String aContentID)
    {
        // simply forward the call to the body
        return iMessageBody.removeMessagePartId(aContentID);
    }

    /**
     * Method for removing a certain message part
     *
     * @param aPart
     *            The message part to be removed
     * @return A boolean indicating the result of the removal
     */
    public boolean removeMessagePart(MessagePart aPart)
    {
        return iMessageBody.removeMessagePart(aPart);
    }

    /**
     * Method for removing a certain message part
     *
     * @param aContentLocation
     *            The content location of the message part to be removed
     * @return A boolean indicating the result of the removal
     */
    public boolean removeMessagePartLocation(String aContentLocation)
    {
        return iMessageBody.removeMessagePartLocation(aContentLocation);
    }

    /**
     * Method for retriving a certain message part
     *
     * @param aContentID
     *            The content identifier of the message part to be retrieved
     * @return The message part whose content identifier is equal to the
     *         provided content identifier or NULL if there is no message part
     *         having the content identifier equal to the provided content
     *         identifier among the existing message parts
     */
    public MessagePart getMessagePart(String aContentID)
    {
        return iMessageBody.getMessagePart(aContentID);
    }

    /**
     * Returns the content identifier of the first message part (if it was set)
     * or null if no message part has been set as being the first one
     */
    public String getStartContentId()
    {
        return iMessageBody.getStartContentId();
    }

    /**
     * Sets a certain message part as being the first one.
     *
     * @param aContentId
     *            The content identifier of the message part to be set as the
     *            first one
     */
    public void setStartContentId(String aContentID)
    {
        iMessageBody.setStartContentId(aContentID);
    }

    /**
     * Retrieves all the existing message parts of the MultimediaMessage
     */
    public MessagePart[] getMessageParts()
    {
        return iMessageBody.getMessageParts();
    }

    /**
     * Set method for the source address
     */
    public void setFrom(String afrom)
    {
        iMessageHeader.setFrom(afrom);
    }

    /**
     * Get method for the subject of the MultimediaMessage
     */
    public String getSubject()
    {
        return iMessageHeader.iSubject;
    }

    /**
     * Get method for a certain header
     *
     * @param headerField
     *            The value of the header to be retrieved
     */
    public String getHeader(String aHeaderField)
    {
        return iMessageHeader.getHeader(aHeaderField);
    }

    /**
     * Set method for the subject of the MultimediaMessage
     */
    public void setSubject(String aSubject)
    {
        iMessageHeader.iSubject = aSubject;
    }

    /**
     * Set method for a certain header
     *
     * @param headerField
     *            The header to be set
     * @param headerValue
     *            The value of the header to be set
     */
    public void setHeader(String aHeaderField, String aHeaderValue)
    {
        iMessageHeader.setHeader(aHeaderField, aHeaderValue);
    }

    /**
     * Method for adding a destination address
     *
     * @param type
     *            The type of the destination address to be added. It can only
     *            be "to" or "bcc" or "cc"
     */
    public boolean addAddress(String aType, String aAddress)
    {
        // mark that the addresses have been touched
        iReply = false;
        return iMessageHeader.addAddress(aType, aAddress);
    }

    /**
     * Removes all the previously set addresses
     */
    public void removeAddresses()
    {
        // mark that the addresses have been touched
        iReply = false;
        iMessageHeader.removeAddresses();
    }

    /**
     * Remove all the addresses of a certain type
     *
     * @param type
     *            The type of the addresses to be removed. It can only be "to"
     *            or "bcc" or "cc"
     */
    public void removeAddresses(String aType)
    {
        // mark that the addresses have been touched
        iReply = false;
        iMessageHeader.removeAddresses(aType);
    }

    /**
     * Removes a certain address of a certain type
     *
     * @param type
     *            The type of the address to be removed. It can onlt be "to" or
     *            "bcc" or "cc"
     * @param address
     *            The address to be removed
     */
    public boolean removeAddress(String aType, String aAddress)
    {
        // mark that the addresses have been touched
        iReply = false;
        return iMessageHeader.removeAddress(aType, aAddress);
    }

    /**
     * Method for retrieving all the addresses of a certain type
     *
     * @param type
     *            The type of the addresses to be retrieved. It can only be
     *            "from" or "to" or "bcc" or "cc"
     */
    public String[] getAddresses(String aType)
    {
        return iMessageHeader.getAddresses(aType);
    }

    /**
     * Retrieves the "from" address if it has been set or the first "to" address
     */
    public String getAddress()
    {
        return iMessageHeader.getAddress();
    }

    /**
     * Retrieves the "from" address if it has been set or the first "to" address
     */
    String getDestinationAddress()
    {
        return iMessageHeader.getDestinationAddress();
    }

    /**
     * Short-cut method for setAddress("to", addr)
     */
    public void setAddress(String aAddress)
    {
        // mark that the addresses have been touched
        iReply = false;
        iMessageHeader.setAddress(aAddress);
    }

    /**
     * Get method for the timestamp
     */
    public Date getTimestamp()
    {
        if (iTimestamp == 0)
        {
            return null;
        }
        return new Date(iTimestamp);
    }

    /**
     * Set method for the timestamp
     */
    void setTimestamp(long aTimestamp)
    {
        iTimestamp = aTimestamp;
    }

    /**
     * Get method for the AppID
     */
    String getAppID()
    {
        return iMessageHeader.iApplicationID;
    }

    /**
     * Get method for the replyToAppID
     */
    String getReplyToAppID()
    {
        return iMessageHeader.iReplyToAppID;
    }

    /**
     * Set method for the replyToAppID
     */
    void setReplyToAppID(String aReplyToAppID)
    {
        iMessageHeader.iReplyToAppID = aReplyToAppID;
    }

    /**
     * Method that serializes/transforms the MultimediaMessage object into a
     * byte array. The format of the serialized message is: | Map |
     * HeaderLengths | AttachmentsCount | AttachmentsLengths | StartContentFlag |
     * Headers | Attachments |
     *
     * where:
     *
     * Map Represents the decimal value of a binary representation of MMS header
     * fields (applicationID, Subject, From, To, Bcc, Cc) Ex: 52 = 110101 means
     * that 'applicationID' is present, 'Subject' is present, 'From' is not
     * present,'To' is present, 'Bcc' is not present and 'Cc' is present
     * HeaderLengths The lengths of each of the MMS header fields which are
     * present AttachmentsCount The number of attachments AttachmentsLengths The
     * lengths of each of the attachments StartContentFlag A boolean indicating
     * if the first attachment should be interpreted as startContent or not
     * Headers The MMS header fields Attachments The attachments
     *
     *
     * @return A byte array representation of the MultimediaMessage
     */
    byte[] serialize() throws IOException
    {
        ByteArrayOutputStream bout = new ByteArrayOutputStream();
        DataOutputStream mmsMsg = new DataOutputStream(bout);

        // calculate the mms Map first
        int[] headersLengths = new int[7];
        Vector headers = new Vector();
        byte mmsMap = calculateMap(headersLengths, headers);
        if (mmsMap == 0)
        {
            throw new IllegalArgumentException(ExceptionStrings.EMPTY_MESSAGE);
        }

        // write the mmsMap
        mmsMsg.writeByte(mmsMap);
        // write the headers lengths
        addHeaderLengths(mmsMsg, headersLengths);

        // write the number of attachments
        int parts = iMessageBody.iMessageParts.size();
        mmsMsg.writeByte(parts);

        // write the attachments lengths
        addContentLengths(mmsMsg);

        // write a boolean indicating if the startId is used
        if (parts > 0)
        {
            if (iMessageBody.iStartContentId)
            {
                mmsMsg.writeByte(1);
            }
            else
            {
                mmsMsg.writeByte(0);
            }
        }

        // write the headers
        addHeaders(mmsMsg, headers);

        // write the attachments
        addContents(mmsMsg);

        // flush the buffer
        mmsMsg.flush();

        // return the serialized message
        return bout.toByteArray();
    }

    /**
     * Deserializes/transforms a byte array into a MultimediaMessage object. See
     * the format of the serialized MultimediaMessage from the comments of the
     * 'serialize' method
     */
    static MultipartMessageImpl deserialize(byte[] aMsg, int aMaxMMSSize)
    throws IOException
    {
        Logger.LOG(Logger.EWMA, Logger.EInfo,
                   "+ MultipartMessageImpl::deserialize()");
        ByteArrayInputStream bin = new ByteArrayInputStream(aMsg);
        DataInputStream msg = new DataInputStream(bin);

        MultipartMessageImpl mmsMsg = new MultipartMessageImpl(null, 0,
                aMaxMMSSize, false);

        // decode the mmsMap
        int[] headerLengths = new int[7];
        resolveMap(msg, headerLengths);

        // get the number of MessageParts/attachments
        int contentsCount = msg.readByte();
        if (contentsCount < 0)
        {
            throw new IOException("Received message is invalid");
        }

        // get the lengths of the MessageParts/attachments
        Vector MIMELengths = new Vector();
        Vector CIDLengths = new Vector();
        Vector LocationLengths = new Vector();
        Vector EncodingLengths = new Vector();
        Vector ContentLengths = new Vector();
        for (int k = 0; k < contentsCount; k++)
        {
            MIMELengths.addElement(new Integer(msg.readInt()));
            CIDLengths.addElement(new Integer(msg.readInt()));
            LocationLengths.addElement(new Integer(msg.readInt()));
            EncodingLengths.addElement(new Integer(msg.readInt()));
            ContentLengths.addElement(new Integer(msg.readInt()));
        }

        // read the startContentId
        int sCID = 0;
        if (contentsCount > 0)
        {
            sCID = msg.readByte();
        }

        // get the headers
        resolveHeaders(msg, headerLengths, mmsMsg);

        // get the message parts/attachments
        for (int k = 0; k < contentsCount; k++)
        {
            byte[] MIME = new byte[((Integer) MIMELengths.elementAt(k))
                                   .intValue()];
            msg.readFully(MIME);

            byte[] CID = new byte[((Integer) CIDLengths.elementAt(k))
                                  .intValue()];
            msg.readFully(CID);

            byte[] Location = new byte[((Integer) LocationLengths.elementAt(k))
                                       .intValue()];
            msg.readFully(Location);

            byte[] Encoding = new byte[((Integer) EncodingLengths.elementAt(k))
                                       .intValue()];
            msg.readFully(Encoding);

            byte[] content = new byte[((Integer) ContentLengths.elementAt(k))
                                      .intValue()];
            msg.readFully(content);
            MessagePart part = MessagePartImpl.initializeMessagePart(content,
                               MIME != null && MIME.length > 0 ? new String(MIME) : null,
                               CID != null && CID.length > 0 ? new String(CID) : null,
                               Location != null && Location.length > 0 ? new String(
                                   Location) : null, Encoding != null
                               && Encoding.length > 0 ? new String(Encoding)
                               : null);
            // simply add the new message part
            mmsMsg.iMessageBody.iMessageParts.addElement(part);
            mmsMsg.iMessageBody.iMessageLength += part.getLength();
            // add the startContentId
            if (k == 0 && sCID == 1)
            {
                mmsMsg.setStartContentId(new String(CID));
            }
        }

        mmsMsg.iReply = true;
        mmsMsg.iMessageHeader.iValidationOn = true;
        mmsMsg.iMessageBody.iValidationOn = true;
        mmsMsg.iValidationDone = false;
        Logger.LOG(Logger.EWMA, Logger.EInfo,
                   "- MultipartMessageImpl::deserialize()");
        return mmsMsg;
    }

    /**
     * Returns the number of recipients
     */
    int getNumberOfRecipients()
    {
        return iMessageHeader.getNumberOfRecipients();
    }

    /**
     * Method used in serialization.
     *
     * Calculates the 'Map' part of the serialized MultimediaMessage object.
     *
     * See the format of the serialized MultimediaMessage from the comments of
     * the 'serialize' method for a full description of the 'Map' part
     *
     * @param aHeaderLengths
     *            The array containing the lengths of the MMS header fields
     * @param aHeaders
     *            A Vector which will contain the MMS header fields
     * @return The 'Map' part of the serializd MultimediaMessage
     */
    private byte calculateMap(int[] aHeadersLengths, Vector aHeaders)
    {
        Logger.LOG(Logger.EWMA, Logger.EInfo,
                   "+ MultipartMessageImpl::calculateMap()");
        byte mmsMap = 0;
        String str;
        if (validField(iMessageHeader.iReplyToAppID))
        {
            aHeadersLengths[REPLY_TO_APPLICATION_ID] = iMessageHeader.iReplyToAppID
                    .length();
            aHeaders.addElement(iMessageHeader.iReplyToAppID);
            mmsMap += 64;
        }
        if (validField(iMessageHeader.iApplicationID))
        {
            aHeadersLengths[APPLICATION_ID] = iMessageHeader.iApplicationID
                                              .length();
            aHeaders.addElement(iMessageHeader.iApplicationID);
            mmsMap += 32;
        }
        if (validField(iMessageHeader.iSubject))
        {
            aHeadersLengths[SUBJECT] = iMessageHeader.iSubject.length();
            aHeaders.addElement(iMessageHeader.iSubject);
            mmsMap += 16;
        }
        if (validField(iMessageHeader.iFrom))
        {
            aHeadersLengths[FROM] = iMessageHeader.iFrom.length();
            aHeaders.addElement(iMessageHeader.iFrom);
            mmsMap += 8;
        }
        str = iMessageHeader.convertAddresses(iMessageHeader.TO_ADDRESS,
                                              iMessageHeader.iToAddresses);
        if (validField(str))
        {
            aHeadersLengths[TO] = str.length();
            aHeaders.addElement(str);
            mmsMap += 4;
        }
        str = iMessageHeader.convertAddresses(iMessageHeader.BCC_ADDRESS,
                                              iMessageHeader.iBccAddresses);
        if (validField(str))
        {
            aHeadersLengths[BCC] = str.length();
            aHeaders.addElement(str);
            mmsMap += 2;
        }
        str = iMessageHeader.convertAddresses(iMessageHeader.CC_ADDRESS,
                                              iMessageHeader.iCcAddresses);
        if (validField(str))
        {
            aHeadersLengths[CC] = str.length();
            aHeaders.addElement(str);
            mmsMap += 1;
        }
        Logger.LOG(Logger.EWMA, Logger.EInfo,
                   "- MultipartMessageImpl::calculateMap()");
        return mmsMap;
    }

    /**
     * Method used in serialization.
     *
     * It adds the MMS header fields lengths to the DataOupputStream containing
     * the serialized form of the MultimediaMessage object.
     *
     * @param aMmsMsg
     *            The output stream where the serialized form of the
     *            MultimediaMessage will be written to
     * @param aHeaderLengths
     *            The array containing the lengths of the MMS header fields
     */
    private void addHeaderLengths(DataOutputStream aMmsMsg, int[] aHeadersLengths)
    throws IOException
    {
        Logger.LOG(Logger.EWMA, Logger.EInfo,
                   "+ MultipartMessageImpl::addHeaderLengths()");
        for (int i = 0; i < aHeadersLengths.length; i++)
        {
            if (aHeadersLengths[i] > 0)
            {
                aMmsMsg.writeInt(aHeadersLengths[i]);
            }
        }
        Logger.LOG(Logger.EWMA, Logger.EInfo,
                   "- MultipartMessageImpl::addHeaderLengths()");
    }

    /**
     * Method used in serialization.
     *
     * It adds the MMS attachment lengths to the DataOupputStream containing the
     * serialized form of the MultimediaMessage object.
     *
     * @param aMmsMsg
     *            The output stream where the serialized form of the
     *            MultimediaMessage will be written to
     */
    private void addContentLengths(DataOutputStream aMmsMsg) throws IOException
    {
        Logger.LOG(Logger.EWMA, Logger.EInfo,
                   "+ MultipartMessageImpl::addContentLengths()");
        for (int j = 0; j < iMessageBody.iMessageParts.size(); j++)
        {
            aMmsMsg.writeInt((((MessagePart) iMessageBody.iMessageParts
                               .elementAt(j)).getMIMEType().length()));
            aMmsMsg.writeInt(((MessagePart) iMessageBody.iMessageParts
                              .elementAt(j)).getContentID().length());
            String aux = ((MessagePart) iMessageBody.iMessageParts.elementAt(j))
                         .getContentLocation();
            if (aux != null)
            {
                aMmsMsg.writeInt(aux.length());
            }
            else
            {
                aMmsMsg.writeInt(0);
            }
            aux = ((MessagePart) iMessageBody.iMessageParts.elementAt(j))
                  .getEncoding();
            if (aux != null)
            {
                aMmsMsg.writeInt(aux.length());
            }
            else
            {
                aMmsMsg.writeInt(0);
            }
            aMmsMsg.writeInt(((MessagePart) iMessageBody.iMessageParts
                              .elementAt(j)).getLength());
        }
        Logger.LOG(Logger.EWMA, Logger.EInfo,
                   "- MultipartMessageImpl::addContentLengths()");
    }

    /**
     * Method used in serialization.
     *
     * It adds the MMS header fields to the DataOupputStream containing the
     * serialized form of the MultimediaMessage object.
     *
     * @param aMmsMsg
     *            The output stream where the serialized form of the
     *            MultimediaMessage will be written to
     * @param aHeaders
     *            The vector containing the MMS header fields
     */
    private void addHeaders(DataOutputStream aMmsMsg, Vector aHeaders)
    throws IOException
    {
        Logger.LOG(Logger.EWMA, Logger.EInfo,
                   "+ MultipartMessageImpl::addHeaders()");
        // add the priority first
        // 1 = low
        // 2 = normal
        // 3 = high
        String priority = iMessageHeader
                          .getHeader(iMessageHeader.PRIORITY_HEADER);
        if (priority == null)
        {
            // the default priority is normal
            aMmsMsg.writeByte(2);
        }
        else
        {
            if (priority.equalsIgnoreCase(iMessageHeader.LOW))
            {
                aMmsMsg.writeByte(1);
            }
            else if (priority.equalsIgnoreCase(iMessageHeader.HIGH))
            {
                aMmsMsg.writeByte(3);
            }
            else
            {
                aMmsMsg.writeByte(2);
            }
        }

        // then add the date
        long date = 0;
        String dateString = iMessageHeader
                            .getHeader(MultipartMessageHeader.DELIVERY_TIME_HEADER);
        if (dateString != null)
        {
            try
            {
                date = Long.parseLong(dateString);
            }
            catch (Exception e)
            {
                Logger.ELOG(Logger.EWMA, "invalid date (" + dateString + "): "
                            + e);
            }
        }
        aMmsMsg.writeLong(date);

        for (int j = 0; j < aHeaders.size(); j++)
        {
            byte[] header = ((String) aHeaders.elementAt(j)).getBytes();
            aMmsMsg.write(header, 0, header.length);
        }
        Logger.LOG(Logger.EWMA, Logger.EInfo,
                   "- MultipartMessageImpl::addHeaders()");
    }

    /**
     * Method used in serialization.
     *
     * It adds the attachments to the DataOupputStream containing the serialized
     * form of the MultimediaMessage object.
     *
     * @param aMmsMsg
     *            The output stream where the serialized form of the
     *            MultimediaMessage will be written to
     */
    private void addContents(DataOutputStream aMmsMsg) throws IOException
    {
        Logger.LOG(Logger.EWMA, Logger.EInfo,
                   "+ MultipartMessageImpl::addContents()");
        for (int j = 0; j < iMessageBody.iMessageParts.size(); j++)
        {
            byte[] prop = ((MessagePart) iMessageBody.iMessageParts
                           .elementAt(j)).getMIMEType().getBytes();
            aMmsMsg.write(prop, 0, prop.length);
            prop = ((MessagePart) iMessageBody.iMessageParts.elementAt(j))
                   .getContentID().getBytes();
            aMmsMsg.write(prop, 0, prop.length);
            String aux = ((MessagePart) iMessageBody.iMessageParts.elementAt(j))
                         .getContentLocation();
            if (aux != null)
            {
                prop = aux.getBytes();
                aMmsMsg.write(prop, 0, prop.length);
            }
            aux = ((MessagePart) iMessageBody.iMessageParts.elementAt(j))
                  .getEncoding();
            if (aux != null)
            {
                prop = aux.getBytes();
                aMmsMsg.write(prop, 0, prop.length);
            }
            prop = ((MessagePart) iMessageBody.iMessageParts.elementAt(j))
                   .getContent();
            if (prop == null)
            {
                prop = new byte[0];
            }
            aMmsMsg.write(prop, 0, prop.length);
        }
        Logger.LOG(Logger.EWMA, Logger.EInfo,
                   "- MultipartMessageImpl::addContents()");
    }

    /**
     * Method used in deserialization.
     *
     * Resolves the 'Map' part of the serialized MultimediaMessage object.
     *
     * See the format of the serialized MultimediaMessage from the comments of
     * the 'serialize' method for a full description of the 'Map' part
     *
     * @param aMsg
     *            The inputstream containing the serialized form of the
     *            MultimediaMessage
     * @param aHeaderLengths
     *            The array containing the lengths of the MMS header fields and
     *            which will be updated according to the 'Map'
     */
    private static void resolveMap(DataInputStream aMsg, int[] aHeaderLengths)
    throws IOException
    {
        Logger.LOG(Logger.EWMA, Logger.EInfo,
                   "+ MultipartMessageImpl::resolveMap()");
        byte mmsMsg = aMsg.readByte();
        if (mmsMsg > 127 || mmsMsg <= 0)
        {
            throw new IOException(ExceptionStrings.INVALID_MESSAGE);
        }
        byte div = divide(mmsMsg, 64);
        if (div >= 0)
        {
            aHeaderLengths[REPLY_TO_APPLICATION_ID] = aMsg.readInt();
            if (div == 0)
            {
                return;
            }
            mmsMsg = div;
        }
        div = divide(mmsMsg, 32);
        if (div >= 0)
        {
            aHeaderLengths[APPLICATION_ID] = aMsg.readInt();
            if (div == 0)
            {
                return;
            }
            mmsMsg = div;
        }
        div = divide(mmsMsg, 16);
        if (div >= 0)
        {
            aHeaderLengths[SUBJECT] = aMsg.readInt();
            if (div == 0)
            {
                return;
            }
            mmsMsg = div;
        }
        div = divide(mmsMsg, 8);
        if (div >= 0)
        {
            aHeaderLengths[FROM] = aMsg.readInt();
            if (div == 0)
            {
                return;
            }
            mmsMsg = div;
        }
        div = divide(mmsMsg, 4);
        if (div >= 0)
        {
            aHeaderLengths[TO] = aMsg.readInt();
            if (div == 0)
            {
                return;
            }
            mmsMsg = div;
        }
        div = divide(mmsMsg, 2);
        if (div >= 0)
        {
            aHeaderLengths[BCC] = aMsg.readInt();
            if (div == 0)
            {
                return;
            }
            mmsMsg = div;
        }
        if (div == 1)
        {
            aHeaderLengths[CC] = aMsg.readInt();
        }
        Logger.LOG(Logger.EWMA, Logger.EInfo,
                   "- MultipartMessageImpl::resolveMap()");
    }

    /**
     * Method used in deserialization.
     *
     * It reads the headers from the DataInputStream according to the header
     * lengths specified in headerLengths array and it updates them into the
     * MultipartMessageImpl object
     *
     * @param aMsg
     *            The input stream containing the derialized MultimediaMessage
     * @param aHeaderLengths
     *            The array containing the lengths of the MMS header fields
     * @param aMmsMsg
     *            The deserialized MultimediaMessage object which will be
     *            updated with the read headers.
     */
    private static void resolveHeaders(DataInputStream aMsg,
                                       int[] aHeaderLengths, MultipartMessageImpl aMmsMsg)
    throws IOException
    {
        Logger.LOG(Logger.EWMA, Logger.EInfo,
                   "+ MultipartMessageImpl::resolveHeaders()");
        // first get the priority
        switch (aMsg.readByte())
        {
        case 1:
            // low priority
            aMmsMsg.setHeader(MultipartMessageHeader.PRIORITY_HEADER,
                              MultipartMessageHeader.LOW);
            break;
        case 2:
            // normal priority
            aMmsMsg.setHeader(MultipartMessageHeader.PRIORITY_HEADER,
                              MultipartMessageHeader.NORMAL);
            break;
        case 3:
            // high priority
            aMmsMsg.setHeader(MultipartMessageHeader.PRIORITY_HEADER,
                              MultipartMessageHeader.HIGH);
            break;
        }
        // read the date
        aMmsMsg.setTimestamp(aMsg.readLong());

        String applicationID = null;
        for (int k = 0; k < aHeaderLengths.length; k++)
        {
            if (aHeaderLengths[k] > 0)
            {
                byte[] header = new byte[aHeaderLengths[k]];
                aMsg.readFully(header);
                String sHeader = new String(header);
                switch (k)
                {
                case REPLY_TO_APPLICATION_ID:
                    aMmsMsg.setReplyToAppID(sHeader);
                    break;
                case APPLICATION_ID:
                    applicationID = sHeader;
                    aMmsMsg.iMessageHeader.iApplicationID = applicationID;
                    break;
                case SUBJECT:
                    aMmsMsg.setSubject(sHeader);
                    break;
                case FROM:
                    aMmsMsg.setFrom(sHeader);
                    break;
                case TO:
                    parseAndSetAddress(MultipartMessageHeader.ADDRESS_TYPE_TO,
                                       sHeader, applicationID, aMmsMsg);
                    break;
                case BCC:
                    parseAndSetAddress(MultipartMessageHeader.ADDRESS_TYPE_BCC,
                                       sHeader, applicationID, aMmsMsg);
                    break;
                case CC:
                    parseAndSetAddress(MultipartMessageHeader.ADDRESS_TYPE_CC,
                                       sHeader, applicationID, aMmsMsg);
                    break;
                }
            }
        }
        Logger.LOG(Logger.EWMA, Logger.EInfo,
                   "- MultipartMessageImpl::resolveHeaders()");
    }

    /**
     * Splits the concatenated addresses (";" separated string) into sepparate
     * tokens and sets them individually to the internal representation of the
     * multimedia message.
     */
    private static void parseAndSetAddress(String aAddressType,
                                           String aAddresses, String aApplicationID, MultipartMessageImpl aMmsMsg)
    {
        Logger.LOG(Logger.EWMA, Logger.EInfo,
                   "+ MultipartMessageImpl::parseAndSetAddress()");
        Vector vAddresses = parseAddress(aAddresses);
        for (int j = 0; j < vAddresses.size(); j++)
        {
            aMmsMsg.iMessageHeader.setAddress(aAddressType, "mms://"
                                              + (String) vAddresses.elementAt(j)
                                              + (aApplicationID != null ? ":" + aApplicationID : ""));
        }
        Logger.LOG(Logger.EWMA, Logger.EInfo,
                   "- MultipartMessageImpl::parseAndSetAddress()");
    }

    /**
     * Util method for splitting a string into tokens, having the ";" character
     * as delimiter
     */
    private static Vector parseAddress(String aAddresses)
    {
        Logger.LOG(Logger.EWMA, Logger.EInfo,
                   "+ MultipartMessageImpl::parseAddress()");
        Vector vAddresses = new Vector();
        int index = aAddresses.indexOf(";");
        while (index != -1)
        {
            vAddresses.addElement(aAddresses.substring(0, index));
            aAddresses = aAddresses.substring(index + 1);
            index = aAddresses.indexOf(";");
        }
        vAddresses.addElement(aAddresses);
        Logger.LOG(Logger.EWMA, Logger.EInfo,
                   "- MultipartMessageImpl::parseAddress()");
        return vAddresses;
    }

    /**
     * Util method for calculating the rest of the a/b operation
     */
    private static byte divide(byte a, int b)
    {
        if (a / b > 0)
        {
            return (byte)(a - (byte)(b *(a / b)));
        }
        return -1;
    }

    /**
     * Util method for checking if a String is valid (not null and not empty)
     */
    private boolean validField(String aField)
    {

        if (aField != null && aField.length() > 0)
        {
            return true;
        }
        return false;
    }

    /**
     * The String representation of the MultimediaMesage
     */
    public String toString()
    {
        return iMessageHeader.toString() + "\n" + iMessageBody.toString();
    }
}
