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



package javax.wireless.messaging;


public interface MultipartMessage extends Message
{

    public void addMessagePart(MessagePart aPart) throws SizeExceededException;

    public boolean removeMessagePartId(String aContentID);

    public boolean removeMessagePart(MessagePart aPart);

    public boolean removeMessagePartLocation(String aContentLocation);

    public MessagePart getMessagePart(String aContentID);

    public String getStartContentId();

    public MessagePart[] getMessageParts();

    public void setStartContentId(String aContentId);

    public String getSubject();

    public String getHeader(String aHeaderField);

    public void setSubject(String aSubject);

    public void setHeader(String aHeaderField, String aHeaderValue);

    public boolean addAddress(String aType, String aAddress);

    public void removeAddresses();

    public void removeAddresses(String aType);

    public boolean removeAddress(String aType, String aAddress);

    public String[] getAddresses(String aType);

    public String getAddress();

    public void setAddress(String aAddress);

}
