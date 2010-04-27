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
package com.nokia.mj.test.pim.automatic;
//IMPORTS
import java.io.InputStream;
import java.io.UnsupportedEncodingException;
import java.util.*;
import javax.microedition.pim.PIM;
import javax.microedition.pim.PIMException;
import javax.microedition.pim.PIMItem;
import javax.microedition.pim.Contact;
import javax.microedition.pim.Event;
import javax.microedition.pim.ToDo;
import com.nokia.mj.test.pim.adaptation.*;

// CLASS DEFINITION
/**
 * Tests javax.microedition.pim.PIM.fromSerialFormat()
 *
 */
public class PfromSerialFormat extends MIDPTest
{

    public PfromSerialFormat(
        String sTestName, j2meunit.framework.TestMethod rTestMethod)
    {
        super(sTestName, rTestMethod);
    }


    public void runTest() throws TestEndThrowable
    {
    }

    protected void testNullPointer(PIM aP) throws TestEndThrowable
    {
        boolean exceptionThrown = false;
        try
        {
            PIMItem item[] = aP.fromSerialFormat(null, null);
        }
        catch (NullPointerException e)
        {
            exceptionThrown = true;
        } // end of try-catch
        catch (Throwable e)
        {
            failTest("Not a NullPointer:" + e.toString());
        } // end of catch
        if (!exceptionThrown)
        {
            failTest("NullPointer exception not thrown");
        } // end of if (!exceptionThrown)
    }

    protected void testUEE(PIM aP, InputStream aStream, String aEncoding)
    throws TestEndThrowable
    {
        boolean exceptionThrown = false;
        try
        {
            PIMItem item[] = aP.fromSerialFormat(aStream, aEncoding);
        }
        catch (UnsupportedEncodingException e)
        {
            exceptionThrown = true;
        } // end of try-catch
        catch (Throwable e)
        {
            failTest("Not UEE:" + e.toString());
        } // end of catch
        if (!exceptionThrown)
        {
            failTest("UEE exception not thrown");
        } // end of if (!exceptionThrown)
    }

    /**
     * testPIME1 passes if PIMException is thrown, or no exception is thrown
     * at all.
     */
    protected void testPIME1(PIM aP, InputStream aStream, String aDescription)
    throws TestEndThrowable
    {
        boolean exceptionThrown = false;
        try
        {
            PIMItem item[] = aP.fromSerialFormat(aStream, null);
        }
        catch (PIMException e)
        {
            // PIMException is expected
            exceptionThrown = true;
            log(aDescription + " threw PIMException");
        } // end of try-catch
        catch (Throwable e)
        {
            failTest("Not PIMException:" + e.toString());
        } // end of catch
        if (!exceptionThrown)
        {
            log(aDescription + " did not throw anything");
        } // end of if (!exceptionThrown)

    }

    /**
     * testPIME2 passes only if PIMException is thrown.
     */
    protected void testPIME2(PIM aP, InputStream aStream)
    throws TestEndThrowable
    {
        boolean exceptionThrown = false;
        try
        {
            PIMItem item[] = aP.fromSerialFormat(aStream, null);
        }
        catch (PIMException e)
        {
            exceptionThrown = true;
        } // end of try-catch
        catch (Throwable e)
        {
            failTest("Not PIMException:" + e.toString());
        } // end of catch
        if (!exceptionThrown)
        {
            failTest("PIMException not thrown");
        } // end of if (!exceptionThrown)
    }

    protected PIMItem convertItem(PIM aP, InputStream aStream)
    throws TestEndThrowable
    {
        PIMItem item[] = null;
        try
        {
            item = aP.fromSerialFormat(aStream, "UTF-8");
        }
        catch (Throwable e)
        {
            failTest("fromSerialFormat threw: " + e.toString());
        } // end of try-catch
        return item[ 0 ];
    }


    protected void testEvent(PIM aP, InputStream aStream)
    throws TestEndThrowable
    {
        PIMItem item = convertItem(aP, aStream);
        if (!(item instanceof Event))
        {
            failTest("item is not event");
        }
        // The contents of the date fields could be checked if TimeZone
        // and Calendar classes worked.
        if (item.countValues(Event.START) != 1)
        {
            failTest("Event does not have start field");
        }
        if (item.countValues(Event.END) != 1)
        {
            failTest("Event does not have end field");
        }
        String summary = null;
        try
        {
            summary = item.getString(Event.SUMMARY, 0);
        }
        catch (Throwable e)
        {
            failTest("Event getString threw: " + e.toString());
        } // end of try-catch
        if (summary.compareTo("timed appointment") != 0)
        {
            failTest("Event summary: " + summary);
        } // end of if (summary.compareTo( "timed appointment" ) != 0)
    }

    protected void testToDo(PIM aP, InputStream aStream)
    throws TestEndThrowable
    {
        PIMItem item = convertItem(aP, aStream);
        if (!(item instanceof ToDo))
        {
            failTest("item is not todo");
        }
        int priority = 0;
        try
        {
            priority = item.getInt(ToDo.PRIORITY, 0);
        }
        catch (Throwable e)
        {
            failTest("Todo get int threw: " + e.toString());
        } // end of try-catch
        if ((priority < 1) || (priority > 10))
        {
            failTest("Priority not in range [1,10]: " + priority);
        }

        String summary = null;
        try
        {
            summary = item.getString(ToDo.SUMMARY, 0);
        }
        catch (Throwable e)
        {
            failTest("ToDo getString threw: " + e.toString());
        } // end of try-catch
        if (summary.compareTo("Priority2") != 0)
        {
            failTest("ToDo summary: " + summary);
        }
    }

    protected void testContact(PIM aP, InputStream aStream)
    throws TestEndThrowable
    {
        PIMItem item = convertItem(aP, aStream);
        if (!(item instanceof Contact))
        {
            failTest("item is not contact");
        }
        String[] nameArray = getStringArray(item, Contact.NAME);
        String familyName = nameArray[ Contact.NAME_FAMILY ];
        if (familyName.compareTo("Internet Mail Consortium") != 0)
        {
            failTest("family name is: " + familyName);
        }
        //checkString( item, Contact.FORMATTED_NAME, "Internet Mail Consortium" );
        checkString(item, Contact.ORG, "Internet Mail Consortium");
        checkString(item, Contact.EMAIL, "info@imc.org");
        checkTel(item, 0);
        checkTel(item, 1);
        String[] addrArray = getStringArray(item, Contact.ADDR);
        if (addrArray[ Contact.ADDR_STREET ].compareTo("127 Segre Place")
                != 0)
        {
            failTest("street:" + addrArray[ Contact.ADDR_STREET ]);
        }
        if (addrArray[ Contact.ADDR_LOCALITY ].compareTo("Santa Cruz")
                != 0)
        {
            failTest("locality:" + addrArray[ Contact.ADDR_LOCALITY ]);
        }
        if (addrArray[ Contact.ADDR_REGION ].compareTo("CA")
                != 0)
        {
            failTest("region:" + addrArray[ Contact.ADDR_REGION ]);
        }
        if (addrArray[ Contact.ADDR_POSTALCODE ].compareTo("95060")
                != 0)
        {
            failTest("region:" + addrArray[ Contact.ADDR_POSTALCODE ]);
        }
        if (addrArray[ Contact.ADDR_COUNTRY ].compareTo("USA")
                != 0)
        {
            failTest("region:" + addrArray[ Contact.ADDR_COUNTRY ]);
        }
        checkString(item, Contact.URL, "http://www.imc.org/");
    }

    protected String[] getStringArray(PIMItem aItem, int aField)
    throws TestEndThrowable
    {
        String[] stringArray = null;
        try
        {
            stringArray = aItem.getStringArray(aField, 0);
        }
        catch (Throwable e)
        {
            failTest("getStringArray threw: " + e.toString());
        } // end of try-catch
        return stringArray;
    }

    protected void checkTel(PIMItem aItem, int aIndex)
    throws TestEndThrowable
    {
        String tel = getString(aItem, Contact.TEL, aIndex);
        int telAttr = aItem.getAttributes(Contact.TEL, aIndex);
        if ((telAttr & Contact.ATTR_FAX) != 0)
        {
            if (tel.compareTo("+1 831 426 7301") != 0)
            {
                failTest("Fax number: " + tel);
            } // end of if (tel.compareTo( "+1 831 426 7301" ) != 0)
        } // end of if (( telAttr & Contact.ATTR_FAX ) != 0)
        else
        {
            if (tel.compareTo("+1 831 426 9827") != 0)
            {
                failTest("Voice number: " + tel);
            }
        } // end of else
    }


    protected String getString(PIMItem aItem, int aField, int aIndex)
    throws TestEndThrowable
    {
        String string = null;
        try
        {
            string = aItem.getString(aField, aIndex);
        }
        catch (Throwable e)
        {
            failTest("getString threw: " + e.toString());
        } // end of try-catch
        return string;
    }

    protected void checkString(PIMItem aItem, int aField, String aString)
    throws TestEndThrowable
    {
        String string = getString(aItem, aField, 0);
        if (string.compareTo(aString) != 0)
        {
            failTest("unexpected string:" + string + ", wanted:" + aString);
        } // end of if (string.compareTo( aString ) != 0)
    }

}



