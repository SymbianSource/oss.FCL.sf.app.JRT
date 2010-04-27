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


// ----------------------- packae name -----------------------
package javax.wireless.messaging;

import com.nokia.mj.impl.mms.MessagePartImpl;
import com.nokia.mj.impl.mms.MessagePartFactory;

public class MessagePart
{

    static
    {
        // Initialize the factory on internal class
        MessagePartImpl.registerFactory(new MessagePartFactoryClass());
    }

    MessagePartImpl iMessagePartImpl = null;

    /**
     *Refer About this in JSR-205(Wireless Messaging API 2.0)
     */
    public MessagePart(java.io.InputStream aInputStream, String aMimeType,
                       String aContentId, String aContentLocation, String aEncoding)
    throws java.io.IOException, SizeExceededException
    {
        iMessagePartImpl = new MessagePartImpl(aInputStream, aMimeType,
                                               aContentId,    aContentLocation, aEncoding);
    }


    /**
     *Refer About this in JSR-205(Wireless Messaging API 2.0)
     */
    public MessagePart(byte[] aContents, String aMimeType, String aContentId,
                       String aContentLocation, String aEncoding)
    throws SizeExceededException
    {
        iMessagePartImpl = new MessagePartImpl(aContents, aMimeType, aContentId,
                                               aContentLocation, aEncoding);
    }


    /**
     *Refer About this in JSR-205(Wireless Messaging API 2.0)Specification
     */
    public MessagePart(byte[] aContents, int aOffset, int aLength,
                       String aMimeType, String aContentId, String aContentLocation,
                       String aEncoding) throws SizeExceededException
    {
        iMessagePartImpl = new MessagePartImpl(aContents, aOffset, aLength,
                                               aMimeType, aContentId, aContentLocation, aEncoding);
    }

    /**
    /**
     *Refer About this in JSR-205(Wireless Messaging API 2.0) specification
     */
    public byte[] getContent()
    {
        return iMessagePartImpl.getContent();
    }

    /**
     *Refer About this in JSR-205(Wireless Messaging API 2.0)
     */
    public java.io.InputStream getContentAsStream()
    {
        return iMessagePartImpl.getContentAsStream();
    }

    /**
     *Refer About this in JSR-205(Wireless Messaging API 2.0)
     */
    public String getContentID()
    {
        return iMessagePartImpl.getContentID();
    }

    /**
     *Refer About this in JSR-205(Wireless Messaging API 2.0)
     */
    public String getMIMEType()
    {
        return iMessagePartImpl.getMIMEType();
    }

    /**
    *Refer About this in JSR-205(Wireless Messaging API 2.0)
    */

    public String getEncoding()
    {
        return iMessagePartImpl.getEncoding();
    }

    /**
     *Refer About this in JSR-205(Wireless Messaging API 2.0)
     */
    public String getContentLocation()
    {
        return iMessagePartImpl.getContentLocation();
    }
    /**
    *Refer About this in JSR-205(Wireless Messaging API 2.0)
    */

    public int getLength()
    {
        return iMessagePartImpl.getLength();
    }

    public String toString()
    {
        return iMessagePartImpl.toString();
    }

    public boolean equals(Object aObj)
    {
        return iMessagePartImpl.equals(aObj);
    }

    private MessagePart()
    {
        iMessagePartImpl = new MessagePartImpl();
    }

    private void initMessagePart(byte[] aContents, String aMimeType,
                                 String aContentId, String aContentLocation,
                                 String aEncoding)
    {
        iMessagePartImpl.initMessagePart(aContents, aMimeType, aContentId,
                                         aContentLocation, aEncoding);
    }

    private static class MessagePartFactoryClass implements MessagePartFactory
    {
        public MessagePart initializeMessagePart(byte[] aContents,
                String aMimeType, String aContentId, String aContentLocation,
                String aEncoding)
        {
            MessagePart part = new MessagePart();
            part.initMessagePart(aContents, aMimeType, aContentId,
                                 aContentLocation, aEncoding);
            return part;
        }
    }

}// end of file
