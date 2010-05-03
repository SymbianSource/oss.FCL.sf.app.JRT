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
import java.util.Hashtable;

import com.nokia.mj.impl.utils.Logger;
/**
 * The representation of an MultimediaMessage header. It contains the message
 * following fields of a MultimediaMessage: . subject . destination addresses
 * (to, bcc, cc) . source address (from) . applicationID . headers
 * ('X-Mms-Delivery-Time' and 'X-Mms-Priority')
 */
final class MultipartMessageHeader
{
    // the subject of the MMS message
    String iSubject = null;;

    // the destination addresses
    Vector iToAddresses = null;

    Vector iCcAddresses = null;

    Vector iBccAddresses = null;

    // constant for the 'X-Mms-Priority' header
    static final String PRIORITY_HEADER = "x-mms-priority";

    // constant for the 'X-Mms-Delivery-Time' header
    static final String DELIVERY_TIME_HEADER = "x-mms-delivery-time";

    // the dummy reply-to-application-ID
    static final String DUMMY_REPLY_TO_APP_ID = "$";

    // constants for the allowed values of the 'X-Mms-Priority' header
    static final String HIGH = "high";

    static final String LOW = "low";

    static final String NORMAL = "normal";

    // the applicationID of the MMS message
    String iApplicationID = null;

    // the reply to app ID
    String iReplyToAppID = null;

    // the source address
    String iFrom = null;

    // constant values for the destination types
    static final String ADDRESS_TYPE_TO = "to";

    static final String ADDRESS_TYPE_CC = "cc";

    static final String ADDRESS_TYPE_BCC = "bcc";

    // constat values for address type
    static final int TO_ADDRESS = 0;

    static final int BCC_ADDRESS = 1;

    static final int CC_ADDRESS = 2;

    // boolean indicating if the validation is enabled/disabled
    boolean iValidationOn = true;

    // the headers containing the 'X-Mms-Delivery-Time' and
    // 'X-Mms-Priority' of the MMS message
    private Hashtable iHeaders = null;

    // constant for the other recognized headers
    private static final String SUBJECT_HEADER = "x-mms-subject";

    private static final String FROM_HEADER = "x-mms-from";

    private static final String TO_HEADER = "x-mms-to";

    private static final String CC_HEADER = "x-mms-cc";

    private static final String BCC_HEADER = "x-mms-bcc";

    // constant values for the sender address type
    private static final String ADDRESS_TYPE_FROM = "from";

    // members containing the number of recipients
    private int iToAddressCount = 0;

    private int iBccAddressCount = 0;

    private int iCcAddressCount = 0;

    /**
     * Constructor
     */
    MultipartMessageHeader(boolean aValidationOn)
    {
        Logger.LOG(Logger.EWMA, Logger.EInfo,
                   "+ MultipartMessageHeader::MultipartMessageHeader()");
        iHeaders = new Hashtable();
        // put the default values
        // setHeader(PRIORITY_HEADER,NORMAL);
        iToAddresses = new Vector();
        iCcAddresses = new Vector();
        iBccAddresses = new Vector();
        iValidationOn = aValidationOn;
        Logger.LOG(Logger.EWMA, Logger.EInfo,
                   "- MultipartMessageHeader::MultipartMessageHeader()");
    }

    /**
     * Set method for the source address
     */
    void setFrom(String aFrom)
    {
        iFrom = aFrom;
    }

    /**
     * Get method for a certain header
     *
     * @param aHeaderField
     *            The value of the header to be retrieved
     */
    String getHeader(String aHeaderField)
    {
        Logger.LOG(Logger.EWMA, Logger.EInfo,
                   "+ MultipartMessageHeader::getHeader()");
        // check the header against null
        if (aHeaderField == null)
        {
            throw new IllegalArgumentException(ExceptionStrings.
                                               NULL_HEADER);
        }
        aHeaderField = aHeaderField.toLowerCase();
        // check the header
        if (aHeaderField.equals(DELIVERY_TIME_HEADER)
                || aHeaderField.equals(PRIORITY_HEADER))
        {
            Logger.LOG(Logger.EWMA, Logger.EInfo,
                       "- MultipartMessageHeader::getHeader()");
            return (String) iHeaders.get(aHeaderField);
        }
        // check if the header is among the recognized headers
        if (aHeaderField.equals(SUBJECT_HEADER)
                || aHeaderField.equals(FROM_HEADER)
                || aHeaderField.equals(TO_HEADER)
                || aHeaderField.equals(CC_HEADER)
                || aHeaderField.equals(BCC_HEADER))
        {
            // the header has been recognized, but it can not be
            // accessed via this method
            throw new SecurityException("Access to header field "
                                        + aHeaderField + " is restricted");
        }
        throw new IllegalArgumentException("Unknown header field: "
                                           + aHeaderField);
    }

    /**
     * Set method for a certain header
     *
     * @param aHeaderField
     *            The header to be set
     * @param aHeaderValue
     *            The value of the header to be set
     */
    void setHeader(String aHeaderField, String aHeaderValue)
    {
        // validate the input parameters
        validateAndSetHeader(aHeaderField, aHeaderValue);
    }

    /**
     * Method for adding a destination address
     *
     * @param aType
     *            The type of the destination address to be added. It can only
     *            be "to" or "bcc" or "cc"
     */
    boolean addAddress(String aType, String aAddress)
    {
        // validate the input parameters
        validateTypeAndAddress(aType, aAddress);

        // set the address
        setAddress(aType, aAddress);
        return true;
    }

    /**
     * Removes all the previously set addresses
     */
    void removeAddresses()
    {
        iToAddresses.removeAllElements();
        iCcAddresses.removeAllElements();
        iBccAddresses.removeAllElements();
        iApplicationID = null;
        iToAddressCount = 0;
        iCcAddressCount = 0;
        iBccAddressCount = 0;
    }

    /**
     * Remove all the addresses of a certain type
     *
     * @param aType
     *            The type of the addresses to be removed. It can only be "to"
     *            or "bcc" or "cc"
     */
    void removeAddresses(String aType)
    {
        Logger.LOG(Logger.EWMA, Logger.EInfo,
                   "+ MultipartMessageHeader::removeAddresses()");
        // the recognition if the address type is case insensitive
        aType = aType.toLowerCase();
        // check the type
        if (aType.equals(ADDRESS_TYPE_TO))
        {
            iToAddresses.removeAllElements();
        }
        else if (aType.equals(ADDRESS_TYPE_CC))
        {
            iCcAddresses.removeAllElements();
        }
        else if (aType.equals(ADDRESS_TYPE_BCC))
        {
            iBccAddresses.removeAllElements();
        }
        else
        {
            throw new IllegalArgumentException(ExceptionStrings.
                                               ADDRESS_TYPE_ALLOWED + aType);
        }
        // set to NULL the applicationID if there are no other
        // addresses left
        checkEmptyAddresses();
        Logger.LOG(Logger.EWMA, Logger.EInfo,
                   "- MultipartMessageHeader::removeAddresses()");
    }

    /**
     * Removes a certain address of a certain type
     *
     * @param aType
     *            The type of the address to be removed. It can onlt be "to" or
     *            "bcc" or "cc"
     * @param aAddress
     *            The address to be removed
     */
    boolean removeAddress(String aType, String aAddress)
    {
        Logger.LOG(Logger.EWMA, Logger.EInfo,
                   "+ MultipartMessageHeader::removeAddress()");
        if (aAddress == null)
        {
            return false;
        }
        boolean ret;
        // the recognition if the address type is case insensitive
        aType = aType.toLowerCase();
        // check the type of the address to be removed
        if (aType.equals(ADDRESS_TYPE_TO))
        {
            ret = iToAddresses.removeElement(aAddress);
        }
        else if (aType.equals(ADDRESS_TYPE_CC))
        {
            ret = iCcAddresses.removeElement(aAddress);
        }
        else if (aType.equals(ADDRESS_TYPE_BCC))
        {
            ret = iBccAddresses.removeElement(aAddress);
        }
        else
        {
            throw new IllegalArgumentException(ExceptionStrings.
                                               ADDRESS_TYPE_ALLOWED + aType);
        }
        // set to NULL the applicationID if there are no other
        // addresses left
        checkEmptyAddresses();
        Logger.LOG(Logger.EWMA, Logger.EInfo,
                   "- MultipartMessageHeader::removeAddress()");
        return ret;
    }

    /**
     * Method for retrieving all the addresses of a certain type
     *
     * @param aType
     *            The type of the addresses to be retrieved. It can only be
     *            "from" or "to" or "bcc" or "cc"
     */
    String[] getAddresses(String aType)
    {
        Logger.LOG(Logger.EWMA, Logger.EInfo,
                   "+ MultipartMessageHeader::getAddresses()");
        // transfer the addresses from Vector to array
        if (aType == null)
        {
            return null;
        }
        aType = aType.toLowerCase();
        String[] addresses = null;
        if (aType.equals(ADDRESS_TYPE_FROM))
        {
            if (iFrom != null)
            {
                addresses = new String[1];
                addresses[0] = getFullAddress(iFrom, true);
            }
        }
        else if (aType.equals(ADDRESS_TYPE_TO))
        {
            addresses = new String[iToAddresses.size()];
            for (int i = 0; i < iToAddresses.size(); i++)
            {
                addresses[i] = (String) iToAddresses.elementAt(i);
            }
        }
        else if (aType.equals(ADDRESS_TYPE_CC))
        {
            addresses = new String[iCcAddresses.size()];
            for (int i = 0; i < iCcAddresses.size(); i++)
            {
                addresses[i] = (String) iCcAddresses.elementAt(i);
            }
        }
        else if (aType.equals(ADDRESS_TYPE_BCC))
        {
            addresses = new String[iBccAddresses.size()];
            for (int i = 0; i < iBccAddresses.size(); i++)
            {
                addresses[i] = (String) iBccAddresses.elementAt(i);
            }
        }
        else
        {
            throw new IllegalArgumentException(ExceptionStrings.
                                               ADDRESS_TYPE_ALLOWED + aType);
        }
        Logger.LOG(Logger.EWMA, Logger.EInfo,
                   "- MultipartMessageHeader::getAddresses()");
        return (addresses != null && addresses.length > 0 ? addresses : null);
    }

    /**
     * Retrieves the "from" address if it has been set or the first "to" address
     */
    String getAddress()
    {
        // check te "from" first
        if (iFrom != null && iFrom.length() > 0)
        {
            return getFullAddress(iFrom, true);
        }
        // check the "to" addresses
        if (iToAddresses.size() > 0)
        {
            return (String) iToAddresses.elementAt(0);
        }
        // nothing to return
        return null;
    }

    /**
     * Retrieves the "from" address if it has been set or the first "to" address
     */
    String getDestinationAddress()
    {
        // check te "from" first
        if (iFrom != null && iFrom.length() > 0)
        {
            return getFullAddress(iFrom, false);
        }
        // check the "to" addresses
        if (iToAddresses.size() > 0)
        {
            return (String) iToAddresses.elementAt(0);
        }
        // nothing to return
        return null;
    }

    /**
     * Method for converting the Vector into String
     */
    String convertAddresses(int aAddressType, Vector aAddresses)
    {
        Logger.LOG(Logger.EWMA, Logger.EInfo,
                   "+ MultipartMessageHeader::convertAddresses()");
        int addressCount = 0;
        Vector cleanAddresses = new Vector();
        // String sAddresses = "";
        StringBuffer sAddresses = new StringBuffer();
        int i;
        for (i = 0; i < aAddresses.size(); i++)
        {
            // strip down the protocol
            String address = (String) aAddresses.elementAt(i);
            if (address.startsWith(MMSAddress.MMS_PREFIX))
            {
                address = address.substring(6);
            }
            // strip down the application ID
            int index = address.indexOf(":");
            if (index != -1)
            {
                address = address.substring(0, index);
            }
            if (!cleanAddresses.contains(address))
            {
                cleanAddresses.addElement(address);
                //sAddresses += address;
                sAddresses.append(address);
                addressCount++;
            }
            if (i < aAddresses.size() - 1)
            {
                //sAddresses += ";";
                sAddresses.append(";");
            }
        }
        switch (aAddressType)
        {
        case TO_ADDRESS:
            iToAddressCount = addressCount;
            break;
        case BCC_ADDRESS:
            iBccAddressCount = addressCount;
            break;
        case CC_ADDRESS:
            iCcAddressCount = addressCount;
            break;
        }
        Logger.LOG(Logger.EWMA, Logger.EInfo,
                   "- MultipartMessageHeader::convertAddresses()");
        return sAddresses.toString();
    }

    /**
     * Returns the number of recipients
     */
    int getNumberOfRecipients()
    {
        return iToAddressCount + iBccAddressCount + iCcAddressCount;
    }

    /**
     * Short-cut method for setAddress("to", addr)
     */
    void setAddress(String aAddr)
    {
        if (aAddr == null)
        {
            // removeAddresses(ADDRESS_TYPE_TO);
            return;
        }
        addAddress(ADDRESS_TYPE_TO, aAddr);
    }

    /**
     * Method for setting a certain address of a certain type
     *
     * @param aType
     *            The type of the address to be set. It can onlt be "to", "bcc"
     *            or "cc"
     * @param aAddress
     *            The actual address to be set
     */
    void setAddress(String aType, String aAddress)
    {
        Logger.LOG(Logger.EWMA, Logger.EInfo,
                   "+ MultipartMessageHeader::setAddress()");
        aType = aType.toLowerCase();
        if (aType.equals(ADDRESS_TYPE_TO))
        {
            if (!iToAddresses.contains(aAddress))
            {
                iToAddresses.addElement(aAddress);
            }
        }
        else if (aType.equals(ADDRESS_TYPE_CC))
        {
            if (!iCcAddresses.contains(aAddress))
            {
                iCcAddresses.addElement(aAddress);
            }
        }
        else if (aType.equals(ADDRESS_TYPE_BCC))
        {
            if (!iBccAddresses.contains(aAddress))
            {
                iBccAddresses.addElement(aAddress);
            }
        }
        else
        {
            throw new IllegalArgumentException(ExceptionStrings.
                                               ADDRESS_TYPE_ALLOWED + aType);
        }
        Logger.LOG(Logger.EWMA, Logger.EInfo,
                   "- MultipartMessageHeader::setAddress()");
    }

    /**
     * The String representation of the MultipartMessage header
     */
    public String toString()
    {
        return "\nSubject: " + iSubject + "\nDeliveryDate: " + "\nFrom: "
               + iFrom + "\nTo: " + convertAddresses(TO_ADDRESS, iToAddresses)
               + "\nBcc: " + convertAddresses(BCC_ADDRESS, iBccAddresses)
               + "\nCc: " + convertAddresses(CC_ADDRESS, iCcAddresses)
               + "\nHeaders \n" + iHeaders.toString();
    }


    /**
     * Variant of getFullAddress
     *
     * @param aUseReplyToAppID
     *            Boolean specifying if the reply to app ID should be used
     *            instead of application id
     */
    private String getFullAddress(String aAddress, boolean aUseReplyToAppID)
    {
        Logger.LOG(Logger.EWMA, Logger.EInfo,
                   "+ MultipartMessageHeader::getFullAddress()");
        String fullAddress = MMSAddress.MMS_PREFIX + aAddress;
        if (aUseReplyToAppID)
        {
            if (iReplyToAppID != null
                    && !iReplyToAppID.equals(DUMMY_REPLY_TO_APP_ID))
            {
                fullAddress = fullAddress + ":" + iReplyToAppID;
            }
        }
        else if (iApplicationID != null)
        {
            fullAddress = fullAddress + ":" + iApplicationID;
        }
        Logger.LOG(Logger.EWMA, Logger.EInfo,
                   "- MultipartMessageHeader::getFullAddress()");
        return fullAddress;
    }

    /**
     * Method for checking if there are no addresses set so far, case in which
     * it deletes the iApplicationID which might have been previously set
     */
    private void checkEmptyAddresses()
    {
        if (iToAddresses.size() == 0 && iCcAddresses.size() == 0
                && iBccAddresses.size() == 0)
        {
            iApplicationID = null;
            iToAddressCount = 0;
            iCcAddressCount = 0;
            iBccAddressCount = 0;
        }
    }

    /**
     * Validation of a header. The header can only be 'X-Mms-Delivery-Time' or
     * 'X-Mms-Priority'.The value of the 'X-Mms-Delivery-Time' header should be
     * a valid long and the value for the 'X-Mms-Priority' can only be either
     * "high", "normal" or "low"
     */
    private void validateAndSetHeader(String aHeaderField, String aHeaderValue)
    {
        Logger.LOG(Logger.EWMA, Logger.EInfo,
                   "+ MultipartMessageHeader::validateAndSetHeader()");
        // check the header against null
        if (aHeaderField == null)
        {
            throw new NullPointerException(ExceptionStrings.NULL_HEADER);
        }
        aHeaderField = aHeaderField.toLowerCase();
        // check the header
        if (aHeaderField.equals(DELIVERY_TIME_HEADER))
        {
            if (aHeaderValue == null)
            {
                // remove the header
                iHeaders.remove(DELIVERY_TIME_HEADER);
                return;
            }
            if (iValidationOn)
            {
                // it must be a valid date and time in milliseconds
                try
                {
                    new java.util.Date(Long.parseLong(aHeaderValue));
                }
                catch (Exception e)
                {
                    throw new IllegalArgumentException(
                        "Invalid header value " +DELIVERY_TIME_HEADER);
                }
            }
            iHeaders.put(DELIVERY_TIME_HEADER, aHeaderValue);
            return;
        }
        if (aHeaderField.equals(PRIORITY_HEADER))
        {
            if (aHeaderValue == null)
            {
                // set it back to the default value
                iHeaders.remove(PRIORITY_HEADER);
                return;
            }
            if (iValidationOn)
            {
                // it must be wither "high", "normal" or "low"
                if (!aHeaderValue.equalsIgnoreCase(HIGH)
                        && !aHeaderValue.equalsIgnoreCase(NORMAL)
                        && !aHeaderValue.equalsIgnoreCase(LOW))
                {
                    throw new IllegalArgumentException(
                        "Invalid header value" + PRIORITY_HEADER);
                }
            }
            iHeaders.put(PRIORITY_HEADER, aHeaderValue);
            return;
        }
        if (iValidationOn)
        {
            // check if the header is among the recognized headers
            if (aHeaderField.equals(SUBJECT_HEADER)
                    || aHeaderField.equals(FROM_HEADER)
                    || aHeaderField.equals(TO_HEADER)
                    || aHeaderField.equals(CC_HEADER)
                    || aHeaderField.equals(BCC_HEADER))
            {
                // the header has been recognized, but it can not be set
                // via this method
                throw new SecurityException("Access to header field "
                                            + aHeaderField + " is restricted");
            }
            throw new IllegalArgumentException(
                "Unknown header field: "+ aHeaderField);
        }
        Logger.LOG(Logger.EWMA, Logger.EInfo,
                   "- MultipartMessageHeader::validateAndSetHeader()");
    }

    /**
     * Validates a certain type
     */
    private void validateType(String aType)
    {
        if (aType == null)
        {
            throw new IllegalArgumentException(ExceptionStrings.NULL_TYPE);
        }
    }

    /**
     * Validates a certain address: it checks that the application ID is unique
     * within the MMS message and it and returns only the destination part of
     * the address
     */
    private void validateAddress(String aAddress)
    {
        Logger.LOG(Logger.EWMA, Logger.EInfo,
                   "+ MultipartMessageHeader::validateAddress()");
        // parse the address and extract the destination and the
        // applicationID
        MMSAddress.validateUrl(aAddress);

        String appID = MMSAddress.getAppIdFromUrl(aAddress);
        // check the applicationID
        if (iApplicationID != null)
        {
            if (appID != null && !iApplicationID.equals(appID))
            {
                throw new IllegalArgumentException(
                    ExceptionStrings.MULTIPLE_APPID);
            }
        }
        else
        {
            iApplicationID = appID;
        }
        Logger.LOG(Logger.EWMA, Logger.EInfo,
                   "- MultipartMessageHeader::validateAddress()");
    }

    /**
     * Validates the type and address
     */
    private void validateTypeAndAddress(String aType, String aAddress)
    {
        validateType(aType);
        validateAddress(aAddress);
    }
}
