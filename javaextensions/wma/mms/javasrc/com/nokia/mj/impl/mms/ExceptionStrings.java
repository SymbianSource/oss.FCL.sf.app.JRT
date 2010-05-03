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


// PACKAGE
package com.nokia.mj.impl.mms;

// CLASS DEFINITION

final class ExceptionStrings
{
    /** Frequently used exception strings. */
    final static String INVALID_ADDRESS = "Invalid URL: ";

    final static String ADDRESS_IPV4 =  "Address typeIPv4 Not supported";

    final static String ADDRESS_IPV6 =  "Address typeIPv6 Not supported";

    final static String ADDRESS_LENGTH_EXCEEDS = "The device address's length"
            + " exceeds the  maximum supported size ";

    final static String OFFSET_INVALID = "Length or offset is less than zero" +
                                         " or offset and length exceeds the contents array length";

    final static String MESSAGE_PART_SIZE_EXCEEDS = "MessagePart size exceeds" +
            " the maximum supported size";

    final static String MIME_TYPE_MANDATORY = "MIME type is null";

    final static String CONTENT_ID_MANDATORY = "Content-id is null";

    final static String NULL_HEADER = "Header field is null";

    final static String ADDRESS_TYPE_ALLOWED =
        "Address type must be one of \"to\", \"cc\" or \"bcc\": ";

    final static String NULL_TYPE = "Address type is null";

    final static String MULTIPLE_APPID = "The MultipartMessage can only contain" +
                                         " one applicationID.";

    final static String EMPTY_MESSAGE = "The message to be sent is null";

    final static String INVALID_MESSAGE_TYPE =
        "Sent message must be MultipartMessage";

    final static String NO_RECIPIENTS = "The message is incomplete or" +
                                        " contains invalid information";


    final static String CLIENT_MESSAGE = "This is a message to which a reply" +
                                         " can not be made";

    final static String CONTENT_LOCATION_LENGTH_EXCEEDS = "Content location" +
            " length exceeds the maximum supported size ";

    final static String HEADER_SIZE_EXCEEDS = "Total size of MIME type," +
            "content-id and content location exceeds the max supported size";

    final static String MESSAGE_SIZE_EXCEEDS = "Cannot add message: "
            + "maximum size exceeded";

    final static String NULL_MESSAGE_PART ="Trying to add a message part that" +
                                           " is null";

    final static String NULL_CONTENT_IDENTIFIER = "Content-id is null";

    final static String NULL_MESSAGE_PART_TO_REMOVE = "Null message part";

    final static String NULL_CONTENT_LOCATION = "Content location is null";

    final static String NULL_MESSAGE = "Message to be sent is null";

    final static String INVALID_MESSAGE = "Received message is invalid";

    final static String CONNECTION_CLOSED = "connection is already closed";

    final static String CLIENT_MODE =
        "Message connection must be a server connection";
}


