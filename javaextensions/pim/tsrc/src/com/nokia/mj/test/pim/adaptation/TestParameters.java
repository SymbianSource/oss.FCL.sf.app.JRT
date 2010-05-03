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


//PACKAGE
package com.nokia.mj.test.pim.adaptation;


// IMPORTS
import java.util.Hashtable;
import java.util.Vector;
import java.io.*;

// CLASS DEFINITION
/**
 * This class is used for obtaining parameters from the parameter
 * file that is in jar. By default it is named as "agent.dat" in
 * root of jar.
 */


public class TestParameters
{
    // Protected fields
    //protected String iParameterFile; // = "/res/agent.dat";
    protected Hashtable iParameters;

    // Private fields
    //private static TestParameters iInstance;

    /**
     * Constructs a new TestParameter instance and loads
     * parameters from the "/agent.dat" file.
     *
     * @param ?parameter ?parameter_description
     * @exception ? exception ?exception_description
     */
    /*
    private TestParameters()
        {
        loadParameters( iParameterFile );
        }
    */
    /**
     * Constructs a new TestParameter instance and loads
     * parameters from the specified file.
     *
     * @param aParameterFile Filename for parameters
     */
    public TestParameters(String aParameterFile)
    {
        loadParameters(aParameterFile);
    }

    /**
     * Deletes this instance just in case.
     * This is needed if the VM does not do it
     * or some other strangel problem
     */
    public void close()
    {
        //iInstance = null;
    }

    /**
     * Returns whether the specified key is mapped in
     * parameters
     *
     * @param aKey Searched key from the parameters
     * @return if the searched key exists
     *
     */
    public boolean containsKey(String aKey)
    {
        if (iParameters == null)
        {
            return false;
        }
        return iParameters.containsKey(aKey);
    }


    /**
     * Returns the value to which the specified key is mapped in
     * parameters
     *
     * @param aKey Searched key from the parameters
     * @return The value of the specified key or <code>null</code>
     *         if key cannot be found.
     *
     */
    public String getParameter(String aKey)
    {
        if (!containsKey(aKey))
        {
            System.out.println("Key "+aKey+" not found in properties file(s).");
            return null;
        }
        return (String)iParameters.get(aKey);

    }

    /**
     * Returns multiple values to which the specified key
     * is mapped in parameters
     *
     * @param aKey Searched key from the parameters
     * @return The values of the specified key or <code>null</code>
     *         if key cannot be found.
     *
     */
    public String[] getArrayParameter(String aKey)
    {
        Vector v = getVectorParameter(aKey);

        String[] ret = new String[ v.size()];
        for (int i = 0; i < v.size(); i++)
        {
            ret[i] = (String) v.elementAt(i);
        }
        return ret;
    }

    /**
     * Returns multiple values to which the specified key
     * is mapped in parameters
     *
     * @param aKey Searched key from the parameters
     * @return The values of the specified key or <code>null</code>
     *         if key cannot be found.
     *
     */
    public Vector getVectorParameter(String aKey)
    {
        String value = getParameter(aKey);

        Vector v = new Vector();
        int commaIndex = value.indexOf(',');
        while (commaIndex > 0)
        {
            String item = value.substring(0, commaIndex);
            v.addElement(item.trim());
            value = value.substring(commaIndex+1);
            commaIndex = value.indexOf(',');
        }
        // add last one
        if (value.length() > 0)
        {
            v.addElement(value.trim());
        }

        return v;
    }

    /**
     * Returns the value to which the specified key is mapped in
     * parameters
     *
     * @param aKey Searched key from the parameters
     * @return The integer value of the specified key or 0
     *         if key cannot be found.
     *
     */
    public int getIntParameter(String aKey)
    {
        String val = getParameter(aKey);
        int ret = parseInt(val);
        return ret;
    }

    /**
     * Returns multiple values to which the specified key
     * is mapped in parameters
     *
     * @param aKey Searched key from the parameters
     * @return The values of the specified key or <code>null</code>
     *         if key cannot be found.
     *
     */
    public int[] getArrayParameterInt(String aKey)
    {
        Vector v = getVectorParameter(aKey);

        int[] ret = new int[ v.size()];
        for (int i = 0; i < v.size(); i++)
        {
            String val = (String)v.elementAt(i);
            ret[i] = parseInt(val);
        }
        return ret;
    }

    // converts String to int
    private int parseInt(String aValue)
    {
        int ret = 0;

        try
        {
            if (aValue.startsWith("0x"))    // hexadecimal
            {
                ret = Integer.parseInt(aValue.substring(2), 16);
            }
            else                        // decimal
            {
                ret = Integer.parseInt(aValue, 10);
            }
        }
        catch (NumberFormatException e)
        {
            System.out.println("Exception while parsing String <"+aValue+"> to int. returned 0.");
            return ret;
        }
        return ret;
    }

    /**
     * Loads the parameter file. Overwrite to read parameters
     * from somewhere else than file.
     *
     * @param aName Filename for the parameters
     */
    protected void loadParameters(String aName)
    {
        InputStream paraStream = getClass().getResourceAsStream(aName);
        if (paraStream == null)
        {
            return;
        }
        // InputStreamReader is used for possible char encoding reasons
        InputStreamReader isr = new InputStreamReader(paraStream);
        char[] params = new char[ 5543 * 4 ];   // ask AL, JH, MV & ES for more
        int bytesRead = 0;
        try
        {
            bytesRead = isr.read(params);
        }
        catch (IOException ioe)
        {
            System.out.println("com.digia.javatest.agent.TestParameters.loadParameters: Exception while reading: " + ioe);
            ioe.printStackTrace();
            throw new RuntimeException("Failed to read parameter file");
        }
        finally
        {
            try
            {
                isr.close();
                paraStream.close();
            }
            catch (IOException ioe)
            {
                // ignored
            }
        }
        if (bytesRead > 0)
        {
            //parsing string
            parseString(new String(params, 0, bytesRead));
        }
    }

    /**
     * Parses the parameters from the string and puts them
     * to the hashtable. Parameters must be in the following syntax:
     * <code>
     * key=value;
     * key2=value 2;
     * </code>
     * notice that key and value can contain spaces but only value can
     * contain '=' char. Overwrite to make parser for other syntax.
     *
     * @param aParams Parsed String
     */
    protected void parseString(String aParams)
    {
        iParameters = new Hashtable();
        int indx = 0;
        int startIndx = 0;
        while ((indx = aParams.indexOf('\n', indx)) != -1)
        {
            String line = aParams.substring(startIndx, indx);
            int keyEnd = line.indexOf('=');
            String key = line.substring(0, line.length() - 1);
            if (keyEnd > 0)
            {
                key = line.substring(0, keyEnd);
            }
            String value = "";
            try
            {
                // from '=' to the end, without carriage return
                value = line.substring(keyEnd + 1, line.length() - 1);
            }
            catch (IndexOutOfBoundsException ioobe)
            {
                // there is no value, adding empty
            }
            // also remove whitespace from the end and beginning of Strings
            iParameters.put(key.trim(), value.trim());
            indx++;
            startIndx = indx;
        }

        // last line may not have line break
        try
        {
            String line = aParams.substring(startIndx);
            String key = line.substring(0, line.indexOf('='));
            String value = line.substring(line.indexOf('=') + 1);
            iParameters.put(key.trim(), value.trim());
        }
        catch (Exception e)
        {
            // there was not valid last line
        }
    }
}
