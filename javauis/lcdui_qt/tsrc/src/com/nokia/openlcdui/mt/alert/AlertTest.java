/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
package com.nokia.openlcdui.mt.alert;

import junit.framework.*;
import javax.microedition.lcdui.*;

import java.io.IOException;

import com.nokia.openlcdui.mt.SWTTestCase;

/**
 * TEST CASE SPECIFICATION <br>
 * <br>
 * Short description of the module test:
 * <li>Collection of tests to test Alert's API functionality. <br>
 * <br>
 */
public class AlertTest extends SWTTestCase
{

    private static String shortString = "Lorem ipsum";

    private static String longString = "Lorem ipsum dolor sit amet, "
                                       + "consectetuer adipiscing elit. Cras turpis ligula, "
                                       + "condimentum nec, rhoncus quis, molestie in, arcu. "
                                       + "Curabitur id lacus. Quisque dictum nulla id odio. "
                                       + "Nullam nec urna. Cras ac lacus nec lacus iaculis aliquet."
                                       + " Integer ut eros. Proin laoreet justo et augue. "
                                       + "Praesent dui. Proin vel leo a eros auctor convallis. "
                                       + "Aenean urna nunc, sagittis vel, pellentesque a, "
                                       + "luctus a, metus. Phasellus posuere lacus nec augue."
                                       + "Nullam nec urna. Cras ac lacus nec lacus iaculis aliquet."
                                       + " Integer ut eros. Proin laoreet justo et augue. "
                                       + "Praesent dui. Proin vel leo a eros auctor convallis. "
                                       + "Aenean urna nunc, sagittis vel, pellentesque a, "
                                       + "luctus a, metus. Phasellus posuere lacus nec augue."
                                       + "Nullam nec urna. Cras ac lacus nec lacus iaculis aliquet."
                                       + " Integer ut eros. Proin laoreet justo et augue. "
                                       + "Praesent dui. Proin vel leo a eros auctor convallis. "
                                       + "Aenean urna nunc, sagittis vel, pellentesque a, "
                                       + "luctus a, metus. Phasellus posuere lacus nec augue.";


    private static final int INVALID_ALERT_TIMEOUT = -3000;
    private static final int VALID_ALERT_TIMEOUT = 2000;


    /**
     * Constructor.
     */
    public AlertTest()
    {
    }

    /**
     * Constructor.
     *
     * @param sTestName Test name.
     * @param rTestMethod Test method.
     */
    public AlertTest(String sTestName)
    {
        super(sTestName);
    }

    /**
     * To create the test suite. You need to add a new aSuite.addTest entry for
     * any new test methods.
     *
     * @return New testsuite.
     */
    public static Test suite()
    {
        TestSuite suite = new TestSuite();

        java.util.Vector methodNames;
        java.util.Enumeration e;

        // Add widget tests
        methodNames = AlertTest.methodNames();
        e = methodNames.elements();
        while(e.hasMoreElements())
        {
            suite.addTest(new AlertTest((String)e.nextElement()));
        }

        return suite;
    }

    public static java.util.Vector methodNames()
    {
        java.util.Vector methodNames = new java.util.Vector();
        methodNames.addElement("testConstructors");
        methodNames.addElement("testSetGetTimeout");
        methodNames.addElement("testSetGetType");
        methodNames.addElement("testSetGetImage");
        methodNames.addElement("testSetGetTexts");
        methodNames.addElement("testSetGetIndicators");
        methodNames.addElement("testCommands");
        return methodNames;
    }

    protected void runTest() throws Throwable
    {
        if(getName().equals("testConstructors")) testConstructors();
        else if(getName().equals("testSetGetTimeout")) testSetGetTimeout();
        else if(getName().equals("testSetGetType")) testSetGetType();
        else if(getName().equals("testSetGetImage")) testSetGetImage();
        else if(getName().equals("testSetGetTexts")) testSetGetTexts();
        else if(getName().equals("testSetGetIndicators")) testSetGetIndicators();
        else if(getName().equals("testCommands")) testCommands();
        else super.runTest();
    }

    /**
     * Tests the constructors.
     */
    public void testConstructors()
    {
        try
        {
            new Alert(null);
        }
        catch(Exception e)
        {
            fail("Exception - Constructor with null title - " + e.getMessage());
        }

        try
        {
            new Alert(shortString);
        }
        catch(Exception e)
        {
            fail("Exception - Constructor with short text - " + e.getMessage());
        }

        try
        {
            new Alert(shortString, longString, null, null);
        }
        catch(Exception e)
        {
            fail("Exception - Constructor with long text - " + e.getMessage());
        }
    }

    /**
     * Tests setter and getter t.
     */
    public void testSetGetTimeout()
    {
        Alert alert = new Alert(shortString);
        alert.setTimeout(VALID_ALERT_TIMEOUT);
        assertEquals("Timeouts don't match.", VALID_ALERT_TIMEOUT, alert
                     .getTimeout());
        alert.setTimeout(Alert.FOREVER);
        assertEquals("Timeouts don't match.", Alert.FOREVER, alert.getTimeout());
        try
        {
            alert.setTimeout(INVALID_ALERT_TIMEOUT);
            fail("IllegalArgumentException should be thrown");
        }
        catch(IllegalArgumentException iae)
        {
            // OK
        }
    }

    /**
     * Tests set and get type.
     */
    public void testSetGetType()
    {
        Alert alert = new Alert(shortString);

        alert.setType(AlertType.ALARM);
        assertEquals("Types don't match.", AlertType.ALARM, alert.getType());

        alert.setType(AlertType.CONFIRMATION);
        assertEquals("Types don't match.", AlertType.CONFIRMATION, alert
                     .getType());

        alert.setType(AlertType.ERROR);
        assertEquals("Types don't match.", AlertType.ERROR, alert.getType());

        alert.setType(AlertType.INFO);
        assertEquals("Types don't match.", AlertType.INFO, alert.getType());

        alert.setType(AlertType.WARNING);
        assertEquals("Types don't match.", AlertType.WARNING, alert.getType());
    }

    /**
     * Tests set and get image.
     */
    public void testSetGetImage()
    {
        try
        {
            Alert alert = new Alert(shortString);
            alert.setImage(null);
            assertEquals("Images don't match.", null, alert.getImage());

            Image param = Image.createImage("100x100.png");
            alert.setImage(param);
            assertEquals("Images don't match.", param, alert.getImage());
        }
        catch(IOException e)
        {
            fail("Cannot load image 100x100.png");
        }
    }

    /**
     * Tests set and get texts.
     */
    public void testSetGetTexts()
    {
        Alert alert = new Alert(shortString);
        alert.setString(null);
        assertEquals("String don't match.", null, alert.getString());

        alert.setString(shortString);
        assertEquals("Strings dont match.", shortString, alert.getString());
        assertTrue("Alert with short text is modal",
                   alert.getTimeout() != Alert.FOREVER);

        alert.setString(longString);
        assertEquals("Strings dont match", longString, alert.getString());
        assertTrue("Alert with long text is not modal",
                   alert.getTimeout() == Alert.FOREVER);
    }

    /**
     * Tests set and get indicator methods and Gauge-Alert interaction related
     * Item methods.
     */
    public void testSetGetIndicators()
    {
        Alert alert = new Alert(shortString);
        Gauge param1 = new Gauge(null, false, 10, 0);
        alert.setIndicator(param1);
        assertEquals("Indicators don't match.", param1, alert.getIndicator());

        try
        {
            Gauge param2 = new Gauge("Label", true, 10, 0);
            alert.setIndicator(param2);
            fail("1. IllegalArgumentException expected");
        }
        catch(IllegalArgumentException iae)
        {
        }

        try
        {
            param1.addCommand(new Command("Comm1", Command.ITEM, 0));
            fail("2. IllegalStateException expected");
        }
        catch(IllegalStateException ise)
        {
        }

        try
        {
            param1.setDefaultCommand(new Command("Comm1", Command.ITEM, 0));
            fail("3. IllegalStateException expected");
        }
        catch(IllegalStateException ise)
        {
        }

        try
        {
            ItemCommandListener icl = new ItemCommandListener()
            {
                public void commandAction(Command command, Item item)
                {
                }
            };
            param1.setItemCommandListener(icl);
            fail("4. IllegalStateException expected");
        }
        catch(IllegalStateException ise)
        {
        }

        try
        {
            param1.setLabel("Label");
            fail("5. IllegalStateException expected");
        }
        catch(IllegalStateException ise)
        {
        }

        try
        {
            param1.setLayout(Item.LAYOUT_BOTTOM);
            fail("6. IllegalStateException expected");
        }
        catch(IllegalStateException ise)
        {
        }

        try
        {
            param1.setPreferredSize(10, 10);
            fail("7. IllegalStateException expected");
        }
        catch(IllegalStateException ise)
        {
        }

        alert.setIndicator(null);
        
        setIndicatorAdditionalTests();
    }

    private void setIndicatorAdditionalTests()
    {
        boolean failed = false;
        String s = "";
        
        Gauge[] g = 
        {
          new Gauge(null, true, 10, 1),      // interactive
          new Gauge("", false, 10, 1),        // has a label
          new Gauge("Label", false, 10, 1),   // has a label

          new Gauge(null, false, 10, 1),  // preferred width will be locked
          new Gauge(null, false, 10, 1),  // preferred height will be locked
          new Gauge(null, false, 10, 1),  // preferred width and height will be locked

          new Gauge(null, false, 10, 1), // layout will be changed
          new Gauge(null, false, 10, 1),  // to be added into another Alert
          new Gauge(null, false, 10, 1),  // to be added into Form
          new Gauge(null, false, 10, 1), // some Command(s) will be added using addCommand method
          new Gauge(null, false, 10, 1), // some Command(s) will be added using setDefaultCommand method

          new Gauge(null, false, 10, 1), // ItemCommandListener will be added to
      };

      g[3].setPreferredSize(5, -1);
      g[4].setPreferredSize(-1, 5);
      g[5].setPreferredSize(5, 5);
      g[6].setLayout(Item.LAYOUT_CENTER);
      new Alert("Another Alert").setIndicator(g[7]);
      new Form("Another Form", new Item[]{g[8]});
      g[9].addCommand(new Command("Command", Command.OK, 0));
      g[10].setDefaultCommand(new Command("Command", Command.OK, 0));
      g[11].setItemCommandListener(new ItemCommandListener() 
      { 
          public void commandAction(Command c, Item item)
          {}
      });

      for(int i=0; i != g.length; ++i) 
      {
          try 
          {
            new Alert("Test Alert").setIndicator(g[i]);
            s = "IllegalStateException expected: index = "+i;
            failed = true;
          } catch(IllegalArgumentException ise) {
          }
      }

      if (failed)
      {
          fail("8. " + s);
      }
      return; 
    }

    /**
     * Tests commands on Alert.
     */
    public void testCommands()
    {
        try
        {
            Alert alert = new Alert(shortString);
            CommandListener param = new CommandListener()
            {
                public void commandAction(Command c, Displayable d)
                {
                }
            };
            alert.setCommandListener(param);
            alert.setCommandListener(null);
        }
        catch(Exception e)
        {
            fail("Exception - Setting and removing CommandListener - "
                 + e.getMessage());
        }

        try
        {
            Alert alert = new Alert(shortString);
            // adding null command
            alert.addCommand(null);
            fail("NullPointerException expected - NULL command added to Alert");
        }
        catch(NullPointerException npe)
        {
            // Ok
        }
        catch(Exception e)
        {
            fail("Wrong exception is thrown instead of NullPointerException - "
                 + "NULL command added to Alert" + e.getMessage());
        }

        try
        {
            Alert alert = new Alert(shortString);

            // test default command
            alert.addCommand(Alert.DISMISS_COMMAND);
            assertTrue("Default Alert shouldn't be modal",
                       alert.getTimeout() != Alert.FOREVER);
            alert.removeCommand(Alert.DISMISS_COMMAND);

            Command c1 = new Command("Comm1", Command.ITEM, 0);
            Command c2 = new Command("Comm2", Command.ITEM, 0);
            Command c3 = new Command("Comm3", Command.ITEM, 0);
            Command c4 = new Command("Comm4", Command.ITEM, 0);
            Command c5 = new Command("Comm5", Command.ITEM, 0);

            // add commands
            alert.addCommand(c1);
            assertTrue("Default Alert with 1 command shouldn't be modal", alert
                       .getTimeout() != Alert.FOREVER);
            alert.addCommand(c2);
            assertTrue("Default Alert with 2 commands should be modal", alert
                       .getTimeout() == Alert.FOREVER);
            alert.addCommand(c3);
            alert.addCommand(c4);
            alert.addCommand(c5);
            assertTrue("Default Alert with 5 commands should be modal", alert
                       .getTimeout() == Alert.FOREVER);

            // remove commands
            alert.removeCommand(c5);
            alert.removeCommand(c4);
            alert.removeCommand(c3);
            assertTrue("Default Alert with 2 commands should be modal", alert
                       .getTimeout() == Alert.FOREVER);
            alert.removeCommand(c2);
            assertTrue("Default Alert with 1 command shouldn't be modal", alert
                       .getTimeout() != Alert.FOREVER);
            alert.removeCommand(c1);
            assertTrue("Default Alert shouldn't be modal",
                       alert.getTimeout() != Alert.FOREVER);
        }
        catch(Exception e)
        {
            fail("Exception - Adding 5 commands - " + e.getMessage());
        }
    }

}
