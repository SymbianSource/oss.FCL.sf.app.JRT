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


package com.nokia.mj.impl.comm;

import java.io.IOException;
import javax.microedition.io.ConnectionNotFoundException;
import java.util.Vector;
import java.util.Hashtable;
import com.nokia.mj.impl.utils.Logger;

public final class CommConnectionUrlParser
{

    private static final String PORT_IR = "IR";

    private static final String PORT_USB = "USB";

    private static final String PORT_COM = "COM";

    private static final String PORT_BT = "BT";

    private static final String COMM_OPTION_BAUD_RATE = "baudrate";

    private static final String COMM_OPTION_PARITY = "parity";

    private static final String COMM_OPTION_BITSPERCHAR = "bitsperchar";

    private static final String COMM_OPTION_STOPBITS = "stopbits";

    private static final String COMM_OPTION_BLOCKING = "blocking";

    private static final String COMM_OPTION_AUTOCTS = "autocts";

    private static final String COMM_OPTION_AUTORTS = "autorts";

    private static String iFullUrl = null;

    /**
     * This function is used to split the URI given by the application in the
     * Connector.open() function.
     *
     * <pre>
     * Input uri will be of the form
     * &quot;comm:COM1;baudrate=19200&quot;;bitsperchar=7;stopbits=2;parity=odd;blocking=on;autocts=off
     * This input URI , will be split into name value pairs, result[0] = &quot;COM1&quot;
     * result[1] = &quot;baudrate=19200&quot; result[2] = &quot;bitsperchar=7&quot;
     *
     * </pre>
     *
     * @param aUri:
     *            uri to be parsed
     * @param aDelimiter:
     *            delimiter based on which the input string will be split.
     * @return a vector containing name=value pairs.
     */

    public static Vector splitString(String aUri, char aDelimiter)
    {
        Vector result = new Vector();
        int currIndx = 0, prevIndex = 0;

        while (true)
        {
            currIndx = aUri.indexOf(aDelimiter, prevIndex);
            if (currIndx == -1)
            {
                break; // end
            }
            result.addElement(aUri.substring(prevIndex, currIndx));
            prevIndex = currIndx;
            prevIndex++;

        }
        /* The last option is not parsed yet */
        if (prevIndex < aUri.length())
        {
            result.addElement(aUri.substring(prevIndex, aUri.length()));
        }

        return result;

    }

    /**
     * Set the default values for all optional parameters
     */

    public static void setDefaultOptions(Hashtable aValidOptions)
    {

        aValidOptions.put(COMM_OPTION_BAUD_RATE, "-1");
        aValidOptions.put(COMM_OPTION_BITSPERCHAR, "8");
        aValidOptions.put(COMM_OPTION_STOPBITS, "1");
        aValidOptions.put(COMM_OPTION_PARITY, "none"); // 0="none" 1="odd" 2="even"
        aValidOptions.put(COMM_OPTION_BLOCKING, "on"); // 1="on" , 0="off"
        aValidOptions.put(COMM_OPTION_AUTORTS, "on");
        aValidOptions.put(COMM_OPTION_AUTOCTS, "on");
    }

    /**
     * parseOptions is used to parse the input URI, specified in the
     * Connector.open() call.
     *
     * <pre>
     *        This function is called
     *        The aParameterValues array is filled with all the values given in the URI. If a particular option is not specified in
     *        the input, then it will contain the default value for that particular option.
     *
     *        The values will be filled to the array in the following sequence.
     *
     *        aParamterValues[0] =  baudrate value
     *        aParamterValues[1] =  bitsperchar value
     *        aParamterValues[2] =  stopbits value
     *        aParamterValues[3] =  parity value
     *        aParamterValues[4] =  blocking value
     *        aParamterValues[5] =  autorts value
     *        aParamterValues[6] =  autocts value
     *
     *        The port name and port id of the paramter aCommPort will be updated with the name and id given in the URI.
     *
     *        aCommPort.mCommPortName = portname, Example values: &quot;IR&quot; , &quot;COM&quot;, &quot;USB&quot;, &quot;BT&quot;
     *        aCommPort.mCommPortNumber = port id,  like &quot;1&quot;,&quot;2&quot;,&quot;3&quot;,...
     *
     *
     * </pre>
     *
     * @param aUri:
     *            uri to be parsed
     * @param aParamterValues:
     *            the options specified in the Connector.open() uri string will
     *            be filled into this array
     * @param aCommPort:
     *            port name and id will be copied into members of this object.
     */

    public static void parseURI(String aUri, int[] aParamterValues,
                                CommConnectionPort aCommPort) throws IOException
    {
        Hashtable validOptions = new Hashtable();
        iFullUrl= "comm:"+aUri;
        setDefaultOptions(validOptions);


        // call splitString() to get individual parmeters into Vector
        // this is written only because the split() function is not present in
        // the CLDC java String class

        Vector result = CommConnectionUrlParser.splitString(aUri, ';');
        int n = result.size();
        if (n <= 0)
        {
            throw new IllegalArgumentException("Invalid URL: " + iFullUrl);
        }
        // first element contains port name and number.
        mapPortNumber(aCommPort, (String) result.elementAt(0));

        if (n <= 1)
        {
            aParamterValues[CommConnectionImpl.COMM_BAUDRATE_INDEX] = -1;
            aParamterValues[CommConnectionImpl.COMM_BITSPERCHAR_INDEX] = 8;
            aParamterValues[CommConnectionImpl.COMM_STOPBITS_INDEX] = 1;
            aParamterValues[CommConnectionImpl.COMM_PARITY_INDEX] = 0;
            aParamterValues[CommConnectionImpl.COMM_BLOCKING_INDEX] = 1;
            aParamterValues[CommConnectionImpl.COMM_AUTORTS_INDEX] = 1;
            aParamterValues[CommConnectionImpl.COMM_AUTOCTS_INDEX] = 1;

            return; // Nothing other than port number was specified in the uri
        }
        String option;
        int indexOfequal = -1, valueinInt;
        String value;
        String validOption;
        String name;

        for (int i = 1; i < n; i++)
        {
            option = (String) result.elementAt(i);
            // option is of the form "name=value", Example "baudrate=1800"
            indexOfequal = option.indexOf("=");
            if (indexOfequal == -1)
            {
                throw new IllegalArgumentException(
                    "Invalid URL: " + iFullUrl);
            }
            name = option.substring(0, indexOfequal);
            if (indexOfequal + 1 >= option.length())
                throw new IllegalArgumentException(
                    "Invalid URL: " + iFullUrl);
            value = option.substring(indexOfequal + 1, option.length());
            if (!validOptions.containsKey(name))
            {
                throw new IllegalArgumentException(
                    "Invalid URL: " + iFullUrl);
            }
            validOptions.put(name, value);
        } // end for

        aParamterValues[CommConnectionImpl.COMM_BAUDRATE_INDEX] = getIntValue((String) validOptions.get(COMM_OPTION_BAUD_RATE));
        aParamterValues[CommConnectionImpl.COMM_BITSPERCHAR_INDEX] = getIntValue((String) validOptions
                .get(COMM_OPTION_BITSPERCHAR));
        aParamterValues[CommConnectionImpl.COMM_STOPBITS_INDEX] = getIntValue((String) validOptions.get(COMM_OPTION_STOPBITS));

        // parity option
        String parity = (String) validOptions.get(COMM_OPTION_PARITY);
        if (parity.compareTo("none") == 0)
            aParamterValues[CommConnectionImpl.COMM_PARITY_INDEX] = 0;
        else if (parity.compareTo("odd") == 0)
            aParamterValues[CommConnectionImpl.COMM_PARITY_INDEX] = 1;
        else if (parity.compareTo("even") == 0)
            aParamterValues[CommConnectionImpl.COMM_PARITY_INDEX] = 2;
        else
            throw new IllegalArgumentException("Invalid URL: " + iFullUrl);

        aParamterValues[CommConnectionImpl.COMM_BLOCKING_INDEX] = mapFlagToInteger((String) validOptions
                .get(COMM_OPTION_BLOCKING));
        aParamterValues[CommConnectionImpl.COMM_AUTORTS_INDEX] = mapFlagToInteger((String) validOptions
                .get(COMM_OPTION_AUTORTS));
        aParamterValues[CommConnectionImpl.COMM_AUTOCTS_INDEX] = mapFlagToInteger((String) validOptions
                .get(COMM_OPTION_AUTOCTS));

    }

    /**
     * This function is map the values like "on" and "off" to an integer.
     *
     * <pre>
     *
     *For the options like, &quot;blocking&quot;,&quot;autorts&quot; &amp; &quot;autocts&quot;, the possible values are &quot;on&quot; and &quot;off&quot;.
     *Since all the values are passed as integers to the native side, the following convention is used
     *
     *If, value=&quot;off&quot;, it is mapped to int value &quot;0&quot;
     *    value=&quot;on&quot;,  it is mapped to int value &quot;1&quot;
     *
     * </pre>
     *
     * @param aValue:
     *            the value to be mapped.
     * @return a mapped integer value.
     */

    private static int mapFlagToInteger(String aValue)
    {
        if (aValue.compareTo("off") == 0)
        {
            return 0;
        }
        else if (aValue.compareTo("on") == 0)
        {
            return 1;
        }
        else
        {
            throw new IllegalArgumentException("Invalid URL: "+iFullUrl);
        }

    }

    /**
     * This function is used to split the port id string into name and number.
     *
     * <pre>
     *
     *
     * Theinput paramter aPortIdAndNumber will contain the port name and number passed in the Connector.open() function.
     * Example: aPortIdAndNumber = &quot;IR1&quot; ,&quot;USB1&quot;, &quot;BT2&quot;, &quot;COM4&quot;
     *
     * This functions seperates the port name and the id. The result is copied into the object aCommPort
     * If the input aPortIdAndNumber = &quot;USB1&quot;, then
     *     aCommPort.mCommPortName   = &quot;USB&quot;
     *     aCommPort.mCommPortNumber =  1
     *
     * </pre>
     *
     * @param aPortIdAndNumber:
     *            the string containing port id and number.
     * @param aCommPort:
     *            port name and id will be copied into members of this object.
     * @throws ConnectionNotFoundException
     *             if port name is other than the
     *             four values(BT,USB,IR,COM).
     * @throws IllegalArgumentException
     *             if the port number is invalid
     */

    private static void mapPortNumber(CommConnectionPort aCommPort,
                                      String aPortIdAndNumber) throws IOException
    {
        String tmp = aPortIdAndNumber;
        String portIdAndNumber = aPortIdAndNumber.toUpperCase();
        Logger.ILOG(Logger.ESOCKET, "++mapPortNumber(), aPortIdAndNumber = " + aPortIdAndNumber);
        if (portIdAndNumber.startsWith(PORT_IR))
        {
            String number = portIdAndNumber.substring(PORT_IR.length(), tmp
                            .length());
            aCommPort.mCommPortName = PORT_IR;
            aCommPort.mCommPortNumber = getIntValue(number);

        }
        else if (portIdAndNumber.startsWith(PORT_COM))
        {
            String number = portIdAndNumber.substring(PORT_COM.length(), tmp
                            .length());
            aCommPort.mCommPortName = PORT_COM;
            aCommPort.mCommPortNumber = getIntValue(number);
        }
        else if (portIdAndNumber.startsWith(PORT_USB))
        {
            String number = portIdAndNumber.substring(PORT_USB.length(), tmp
                            .length());
            aCommPort.mCommPortName = PORT_USB;
            aCommPort.mCommPortNumber = getIntValue(number);
        }
        else if (portIdAndNumber.startsWith(PORT_BT))
        {
            String number = aPortIdAndNumber.substring(PORT_BT.length(), tmp
                            .length());
            aCommPort.mCommPortName = PORT_BT;
            aCommPort.mCommPortNumber = getIntValue(number);
        }
        else
        {
            // MIDP 2.0 TCK Compliance, any other port types other than standard
            // should throw ConnectionNotFound Exception
            // Example : Connector.open("comm:any_string");
            Logger.ILOG(Logger.ESOCKET, "++mapPortNumber() - before throwing connectionotfound exception");
            throw new ConnectionNotFoundException("Not Supported: " + iFullUrl);
        }
    }

    /**
     * This is an internal function used to convert an String to int value.
     *
     * @param aValue:
     *            the string representing an int value
     * @return the integer value .
     * @throws IllegalArgumentException
     *             if the input string is not an valid integer.
     *
     */

    private static int getIntValue(String aValue)
    throws IllegalArgumentException
    {
        int ret = 0;
        try
        {
            ret = Integer.parseInt(aValue);
        }
        catch (NumberFormatException e)
        {
            throw new IllegalArgumentException("Invalid value passed in URI");
        }
        return ret;
    }

}