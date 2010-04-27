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
package com.nokia.mj.test.pim.automatic.System.Properties;
//IMPORTS
import java.io.IOException;
import java.io.InputStream;
import java.util.*;
import com.nokia.mj.test.pim.adaptation.*;


// CLASS DEFINITION

public class SystemPropertiesTest extends MIDPTest
{

    public SystemPropertiesTest(
        String sTestName, j2meunit.framework.TestMethod rTestMethod)
    {
        super(sTestName, rTestMethod);
    }


    public void runTest() throws TestEndThrowable
    {
        String prop = "microedition.pim.version";
        String val = "1.0";

        String sysVal = System.getProperty(prop);


        if (sysVal == null)
        {
            failTest("Property '"+ prop +"' does not exist.");
        }

        // Property contains the needed value
        if (!sysVal.equals(val))
        {
            failTest("Property '"+prop+"' does not contain value: '" + val + "', but '"+sysVal+"'");
        }

        passTest();
    }
}
