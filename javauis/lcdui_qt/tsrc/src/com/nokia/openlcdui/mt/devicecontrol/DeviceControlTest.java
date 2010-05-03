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
package com.nokia.openlcdui.mt.devicecontrol;

import junit.framework.Test;
import junit.framework.TestSuite;
import com.nokia.openlcdui.mt.SWTTestCase;

import com.nokia.mid.ui.DeviceControl;

public class DeviceControlTest extends SWTTestCase {
	
	 /**
     * Constructor.
     */

    /**
     * @param testName Test name.
     * @param testMethod Test method.
     */
    public DeviceControlTest(String testName) {
        super(testName);
    }
    
    public static Test suite() {
		TestSuite suite = new TestSuite();

	    java.util.Vector methodNames;
	    java.util.Enumeration e;

	    // Add tests
	    methodNames = DeviceControlTest.methodNames();
	    e = methodNames.elements();
	    while (e.hasMoreElements()) {
	        suite.addTest(new DeviceControlTest((String)e.nextElement()));
	    }

		return suite;
    }

    public static java.util.Vector methodNames() {
        java.util.Vector methodNames = new java.util.Vector();
        methodNames.addElement("test_getUserInactivityTime");
        methodNames.addElement("test_resetUserInactivityTime");
        methodNames.addElement("test_setLights");
        methodNames.addElement("test_flashLights");
        methodNames.addElement("test_startVibra");
        methodNames.addElement("test_stopVibra");
        return methodNames;
    }
    
    protected void runTest() throws Throwable {
        if (getName().equals("test_getUserInactivityTime")) test_getUserInactivityTime();
        else if (getName().equals("test_resetUserInactivityTime")) test_resetUserInactivityTime();
        else if (getName().equals("test_setLights")) test_setLights();
        else if (getName().equals("test_flashLights")) test_flashLights();
        else if (getName().equals("test_startVibra")) test_startVibra();
        else if (getName().equals("test_stopVibra")) test_stopVibra();
        else super.runTest();
    }
    
    public void test_getUserInactivityTime(){
    	org.eclipse.swt.widgets.Display.getDefault();
    	try{
    		assertTrue(DeviceControl.getUserInactivityTime()>=0);
    	} catch (Exception e) {
  			fail("Exception - in getUserInactivityTime - " + e.getMessage());
	   	}
    	
    	new Thread(new Runnable() {
    		public void run() {
    			try{
    				DeviceControl.getUserInactivityTime();
    			} catch (Exception e) {
    				fail("Exception - in getUserInactivityTime - " + e.getMessage());
    			}
    		}}).start();
    }
    
    public void test_resetUserInactivityTime(){
    	try{
    		DeviceControl.resetUserInactivityTime();
    	} catch (Exception e) {
            fail("Exception - in resetUserInactivityTime - " + e.getMessage());
        }

    	new Thread(new Runnable() {
    		public void run() {
    			try{
    				DeviceControl.resetUserInactivityTime();
    			} catch (Exception e) {
    				fail("Exception - in resetUserInactivityTime - " + e.getMessage());
    			}
    		}}).start();
    }
    
    public void test_setLights(){
    	
    	try {
    		DeviceControl.setLights(-1, 50);
            fail ("Expected exception not thrown setLights with -1 aNum");
        } catch (IllegalArgumentException e){
            	assertEquals ("Exception thrown is not expected","Light num is not supported", e.getMessage());
        }
    	
        try {
    		DeviceControl.setLights(2, 50);
            fail ("Expected exception not thrown setLights with 2 aNum");
        } catch (IllegalArgumentException e){
            	assertEquals ("Exception thrown is not expected","Light num is not supported", e.getMessage());
        }
        
        try {
    		DeviceControl.setLights(0, 50);
        } catch (Exception e) {
            fail("Exception - in setLights - " + e.getMessage());
        }
        
        try {
    		DeviceControl.setLights(0, -1);
    		fail ("Expected exception not thrown setLights with -1 aLevel");
        }  catch (IllegalArgumentException e){
        	assertEquals ("Exception thrown is not expected","Level not between 0 and 100", e.getMessage());
        }
        
        try {
    		DeviceControl.setLights(0, 101);
    		fail ("Expected exception not thrown setLights with 101 aLevel");
        }  catch (IllegalArgumentException e){
        	assertEquals ("Exception thrown is not expected","Level not between 0 and 100", e.getMessage());
        }
        
        try {
    		DeviceControl.setLights(0, 100);
        }  catch (Exception e) {
        	fail("Exception - in setLights - " + e.getMessage());
        }
        
        try {
    		DeviceControl.setLights(0, 0);
        }  catch (Exception e) {
        	fail("Exception - in setLights - " + e.getMessage());
        }
        
        new Thread(new Runnable() {
        	public void run() {
        		try {
        			DeviceControl.setLights(0, 0);
        		}  catch (Exception e) {
        			fail("Exception - in setLights - " + e.getMessage());
        		}
        	}}).start();
    }
    
    public void test_flashLights(){
    	
    	 try {
     		DeviceControl.flashLights(-1);
     		fail ("Expected exception not thrown setLights with -1 duration");
         }  catch (IllegalArgumentException e){
         	assertEquals ("Exception thrown is not expected","Negative duration", e.getMessage());
         }
         
         try {
        	 DeviceControl.flashLights(0);
         }  catch (Exception e) {
         	fail("Exception - in flashLights - " + e.getMessage());
         }
         
         try {
        	 DeviceControl.flashLights(3000);
         }  catch (Exception e) {
         	fail("Exception - in flashLights - " + e.getMessage());
         }

         new Thread(new Runnable() {
        	 public void run() {
        		 try {
        			 DeviceControl.flashLights(3000);
        		 }  catch (Exception e) {
        			 fail("Exception - in flashLights - " + e.getMessage());
        		 }
        	 }}).start();
    }
    
    public void test_startVibra(){
    	  try {
      		DeviceControl.startVibra(-1, 10);
       		fail ("Expected exception not thrown setLights with -1 aFreq");
          }  catch (IllegalArgumentException e){
          	assertEquals ("Exception thrown expected",null, e.getMessage());
         }
      	 try {
      		DeviceControl.startVibra(101, 10);
      		fail ("Expected exception not thrown setLights with 101 aFreq");
          }  catch (IllegalArgumentException e){
          	assertEquals ("Exception thrown expected",null, e.getMessage());
          }
          
          try {
        	  DeviceControl.startVibra(0, 10);
          }  catch (IllegalStateException e) {
        	  assertEquals ("Exception thrown expected only in emulator",null, e.getMessage());
        	  //fail("Exception - in startVibra - " + e.getMessage());
          }catch (Exception e){
          	  fail("Exception - in startVibra - " + e.getMessage());
          }
          
          try {
        	  DeviceControl.startVibra(100, 10);
          }  catch (IllegalStateException e) {
        	  assertEquals ("Exception thrown expected only in emulator",null, e.getMessage());
        	  //fail("Exception - in startVibra - " + e.getMessage());
          }catch (Exception e){
          	  fail("Exception - in startVibra - " + e.getMessage());
          }
          
          try {
        		DeviceControl.startVibra(10, -1);
        		fail ("Expected exception not thrown setLights with -1 aDuration");
            }  catch (IllegalArgumentException e){
            	assertEquals ("Exception thrown expected",null, e.getMessage());
            }
            
          try {
        	  	DeviceControl.startVibra(10, 3000);
            }  catch (IllegalStateException e) {
            	assertEquals ("Exception thrown expected only in emulator",null, e.getMessage());
            	//fail("Exception - in startVibra - " + e.getMessage());
            } catch (Exception e){
          	  fail("Exception - in startVibra - " + e.getMessage());
          	} 

            new Thread(new Runnable() {
            	public void run() {
            		try {
            			DeviceControl.startVibra(10, 3000);
            		}  catch (IllegalStateException e) {
            			assertEquals ("Exception thrown expected only in emulator",null, e.getMessage());
            			//fail("Exception - in startVibra - " + e.getMessage());
            		}catch (Exception e){
            			fail("Exception - in startVibra - " + e.getMessage());
            		}
            	}}).start();
    }
    
    public void test_stopVibra(){
    	 try {
    		 DeviceControl.stopVibra();
         }  catch (Exception e) {
         	fail("Exception - in stopVibra - " + e.getMessage());
         }

         new Thread(new Runnable() {
        	 public void run() {
        		 try {
        			 //DeviceControl.startVibra(10, 3000);
        			 DeviceControl.stopVibra();
        		 }  catch (Exception e) {
        			 fail("Exception - in stopVibra - " + e.getMessage());
        		 }
        	 }}).start();
    }
    
}