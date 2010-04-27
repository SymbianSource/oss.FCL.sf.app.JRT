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

import java.util.Vector;
import javax.wireless.messaging.MessagePart;
import javax.wireless.messaging.SizeExceededException;

import com.nokia.mj.impl.utils.Logger;
/**
 * The representation of an MultimediaMessage body. It contains the message
 * parts of a MultimediaMessage and it implements the handling of these message
 * parts (adding, removing)
 */
final class MultipartMessageBody
{
    /**
     * Vector containing the message parts of the MultimediaMessage
     */
    Vector iMessageParts = null;

    /**
     * Boolean indicating if one of the message parts is set to be the first
     * message part
     */
    boolean iStartContentId = false;

    // boolean indicating if the validation is enabled/disabled
    boolean iValidationOn = true;

    /**
     * The length of the MultimediaMessage
     */
    int iMessageLength = 0;

    /**
     * The corresponding native object
     */
    private int iMaxMMSSize;

    /**
     * Constructor
     */
    MultipartMessageBody(int aMaxMMSSize, boolean aValidationOn)
    {
        iMessageParts = new Vector();
        iMaxMMSSize = aMaxMMSSize;
        iValidationOn = aValidationOn;
    }

    /**
     * Method for adding another message part to the MultimediaMessage body.
     *
     * @param aPart
     *            The message part to be added
     * @throws SizeExceededException
     *             If the provided content is larger than the available memory
     *             or supported size of the attachment
     */
    void addMessagePart(MessagePart aPart) throws SizeExceededException
    {
        Logger.LOG(Logger.EWMA, Logger.EInfo,
                   "+ MultipartMessageBody::addMessagePart()");
        // check against null
        if (aPart == null)
        {
            throw new NullPointerException(ExceptionStrings.NULL_MESSAGE_PART);
        }
        // check for duplicated
        if (iMessageParts.contains(aPart))
        {
            // throw an exception
            throw new IllegalArgumentException(
                "Message already contains MessagePart with same ID: "
                + aPart.getContentID());
        }
        // check the size
        if (iMessageLength + aPart.getLength() > iMaxMMSSize)
        {
            throw(new SizeExceededException(ExceptionStrings.
                                            MESSAGE_SIZE_EXCEEDS));
        }
        // simply add the new message part
        iMessageParts.addElement(aPart);
        iMessageLength += aPart.getLength();
        Logger.LOG(Logger.EWMA, Logger.EInfo,
                   "- MultipartMessageBody::addMessagePart()");
    }

    /**
     * Method for removing a certain message part
     *
     * @param aContentID
     *            The content identifier of the message part to be removed
     * @return A boolean indicating the result of the removal
     */
    boolean removeMessagePartId(String aContentID)
    {
        Logger.LOG(Logger.EWMA, Logger.EInfo,
                   "+ MultipartMessageBody::removeMessagePartId()");
        if (aContentID == null)
        {
            throw new NullPointerException(ExceptionStrings.
                                           NULL_CONTENT_IDENTIFIER);
        }
        // check the existence of the message part to be removed
        int index = iMessageParts.indexOf(MessagePartImpl
                                          .getInstance(aContentID));
        if (index >= 0)
        {
            // if this is the first message, make sure cancel the
            // iStartContentId
            if (index == 0)
            {
                iStartContentId = false;
            }
            // remove the provided message part
            MessagePart part = (MessagePart) iMessageParts.elementAt(index);
            iMessageParts.removeElementAt(index);
            iMessageLength -= part.getLength();
            Logger.LOG(Logger.EWMA, Logger.EInfo,
                       "- MultipartMessageBody::removeMessagePartId()");
            return true;
        }
        Logger.LOG(Logger.EWMA, Logger.EInfo,
                   "- MultipartMessageBody::removeMessagePartId()");
        // the message part to be removed has not been found among the
        // existing message parts
        return false;
    }

    /**
     * Method for removing a certain message part
     *
     * @param aPart
     *            The message part to be removed
     * @return A boolean indicating the result of the removal
     */
    boolean removeMessagePart(MessagePart aPart)
    {
        // check against null
        if (aPart == null)
        {
            throw new NullPointerException(ExceptionStrings.
                                           NULL_MESSAGE_PART_TO_REMOVE);
        }
        // return the provided message part
        return removeMessagePartId(aPart.getContentID());
    }

    /**
     * Method for removing a certain message part
     *
     * @param aContentLocation
     *            The content location of the message part to be removed
     * @return A boolean indicating the result of the removal
     */
    boolean removeMessagePartLocation(String aContentLocation)
    {
        Logger.LOG(Logger.EWMA, Logger.EInfo,
                   "+ MultipartMessageBody::removeMessagePartLocation()");
        boolean ret = false;
        if (aContentLocation == null)
        {
            throw new NullPointerException(
                ExceptionStrings.NULL_CONTENT_LOCATION);
        }
        // browse through the existing message parts in order to find
        // the message part to be removed
        int counter = 0;
        while (counter < iMessageParts.size())
        {
            MessagePart msgPart = (MessagePart) iMessageParts
                                  .elementAt(counter);
            if (msgPart.getContentLocation().equals(aContentLocation))
            {
                // if this is the first message, make sure cancel the
                // iStartContentId
                if (counter == 0)
                {
                    iStartContentId = false;
                }
                // simply remove the message part
                iMessageParts.removeElement(msgPart);
                iMessageLength -= msgPart.getLength();
                ret = true;
            }
            else
            {
                counter++;
            }
        }
        Logger.LOG(Logger.EWMA, Logger.EInfo,
                   "- MultipartMessageBody::removeMessagePartLocation()");
        // the message part to be removed has not been found among the
        // existing message parts
        return ret;
    }

    /**
     * Method for retriving a certain message part
     *
     * @param aContentId
     *            The content identifier of the message part to be retrieved
     * @return The message part whose content identifier is equal to the
     *         provided content identifier or NULL if there is no message part
     *         having the content identifier equal to the provided content
     *         identifier among the existing message parts
     */
    MessagePart getMessagePart(String aContentId)
    {
        Logger.LOG(Logger.EWMA, Logger.EInfo,
                   "+ MultipartMessageBody::getMessagePart()");
        if (aContentId == null)
        {
            throw new NullPointerException(ExceptionStrings.
                                           NULL_CONTENT_IDENTIFIER);
        }
        // check the existence of the provided message part
        int index = iMessageParts.indexOf(MessagePartImpl
                                          .getInstance(aContentId));
        if (index >= 0)
        {
            Logger.LOG(Logger.EWMA, Logger.EInfo,
                       "- MultipartMessageBody::getMessagePart()");
            // simply return the found message part
            return (MessagePart) iMessageParts.elementAt(index);
        }
        Logger.LOG(Logger.EWMA, Logger.EInfo,
                   "- MultipartMessageBody::getMessagePart()");
        // the message part to be retrieved has not been found among
        // the existing message parts
        return null;
    }

    /**
     * Returns the content identifier of the first message part (if it was set)
     * or null if no message part has been set as being the first one
     */
    String getStartContentId()
    {
        Logger.LOG(Logger.EWMA, Logger.EInfo,
                   "+ MultipartMessageBody::getStartContentId()");
        // check if there was any message part set as being the first
        // one
        if (iStartContentId && iMessageParts.size() > 0)
        {
            // get the first message part
            MessagePart first = (MessagePart) iMessageParts.elementAt(0);
            if (first != null)
            {
                Logger.LOG(Logger.EWMA, Logger.EInfo,
                           "- MultipartMessageBody::getStartContentId()");
                // simply return its content identifier
                return first.getContentID();
            }
        }
        Logger.LOG(Logger.EWMA, Logger.EInfo,
                   "- MultipartMessageBody::getStartContentId()");
        // no message part has been set as being the first one
        return null;
    }

    /**
     * Sets a certain message part as being the first one.
     *
     * @param aContentId
     *            The content identifier of the message part to be set as the
     *            first one
     */
    void setStartContentId(String aContentId)
    {
        Logger.LOG(Logger.EWMA, Logger.EInfo,
                   "+ MultipartMessageBody::setStartContentId()");
        // check that content Id is a valid one
        if (aContentId == null)
        {
            // just make sure there is no message part set as being the
            // first one
            iStartContentId = false;
            return;
        }
        // try to find the message part whose content identifier has
        // been provided
        int index = iMessageParts.indexOf(MessagePartImpl
                                          .getInstance(aContentId));
        if (index >= 0)
        {
            // mark down that one of the message parts is set as being
            // the first one
            iStartContentId = true;
            if (index == 0)
            {
                // it's already (phisically) the first one within the
                // Vector
                return;
            }
            // move the message into the first position and shift one
            // position right all the others
            MessagePart first = (MessagePart) iMessageParts.elementAt(index);
            iMessageParts.removeElementAt(index);
            iMessageParts.insertElementAt(first, 0);
            return;
        }
        if (iValidationOn)
        {
            // signal that there is no message part whose content
            // identifier is equal to the provided one
            throw new java.lang.IllegalArgumentException(
                "No MessagePart with the content identifier  "
                + aContentId + " has been found");
        }
        Logger.LOG(Logger.EWMA, Logger.EInfo,
                   "- MultipartMessageBody::setStartContentId()");
    }

    /**
     * Retrieves all the existing message parts
     */
    MessagePart[] getMessageParts()
    {
        Logger.LOG(Logger.EWMA, Logger.EInfo,
                   "+ MultipartMessageBody::getMessageParts()");
        // transfer the message parts from Vector into an array
        int size = iMessageParts.size();
        if (size == 0)
        {
            return null;
        }
        MessagePart[] messagePartArray = new MessagePart[size];
        for (int i = 0; i < size; i++)
        {
            messagePartArray[i] = (MessagePart) iMessageParts.elementAt(i);
        }
        Logger.LOG(Logger.EWMA, Logger.EInfo,
                   "- MultipartMessageBody::getMessageParts()");
        return messagePartArray;
    }

    /**
     * The String representation of the MultipartMessage body
     */
    public String toString()
    {
        return "MessageParts: " + iMessageParts.toString();
    }
}
