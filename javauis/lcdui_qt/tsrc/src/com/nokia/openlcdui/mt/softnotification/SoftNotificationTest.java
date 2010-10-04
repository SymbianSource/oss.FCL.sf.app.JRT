/*
* Copyright (c) 2009,2010 Nokia Corporation and/or its subsidiary(-ies).
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
package com.nokia.openlcdui.mt.softnotification;

import java.io.ByteArrayOutputStream;
import java.io.IOException;
import java.io.InputStream;

import junit.framework.Test;
import junit.framework.TestSuite;
import com.nokia.openlcdui.mt.SWTTestCase;

import com.nokia.mid.ui.SoftNotification;
import com.nokia.mid.ui.SoftNotificationException;

public class SoftNotificationTest extends SWTTestCase {
	/**
	 * Constructor.
	 */

	/**
	 * @param testName
	 *            Test name.
	 * @param testMethod
	 *            Test method.
	 */
	public SoftNotificationTest(String testName) {
		super(testName);
	}

	public static Test suite() {
		TestSuite suite = new TestSuite();

		java.util.Vector methodNames;
		java.util.Enumeration e;

		// Add tests
		methodNames = SoftNotificationTest.methodNames();
		e = methodNames.elements();
		while (e.hasMoreElements()) {
			suite.addTest(new SoftNotificationTest((String) e.nextElement()));
		}

		return suite;
	}

	public static java.util.Vector methodNames() {
		java.util.Vector methodNames = new java.util.Vector();
		methodNames.addElement("test_newInstance");
		methodNames.addElement("test_getId");
		methodNames.addElement("test_setImage");
		methodNames.addElement("test_setText");
		methodNames.addElement("test_post");
		methodNames.addElement("test_remove");
		return methodNames;
	}

	protected void runTest() throws Throwable {
		if (getName().equals("test_newInstance"))
			test_newInstance();
		else if (getName().equals("test_setText"))
			test_getId();
		else if (getName().equals("test_setImage"))
			test_setImage();
		else if (getName().equals("test_setText"))
			test_setText();
		else if (getName().equals("test_post"))
			test_post();
		else if (getName().equals("test_remove"))
			test_remove();
		else
			super.runTest();
	}

	SoftNotification sn1;
	SoftNotification sn2;

	private byte[] getImage(String aImageName) throws IOException {
		byte imageData[] = null;
		if (aImageName != null) {
			InputStream is = this.getClass().getResourceAsStream(
					"/" + aImageName);
			ByteArrayOutputStream out = new ByteArrayOutputStream();
			try {
				byte[] buf = new byte[1024];
				for (int readNum; (readNum = is.read(buf)) != -1;) {
					out.write(buf, 0, readNum);
				}

			} catch (Throwable e) {
				e.printStackTrace();
			}
			imageData = out.toByteArray();
			is.close();
		}
		return imageData;
	}

	public void test_newInstance() {
		try {
			sn1 = SoftNotification.newInstance();
		} catch (Exception e) {
			fail("Exception - in newInstance - " + e.getMessage());
		}

		try {
			sn2 = SoftNotification.newInstance(101);
		} catch (Exception e) {
			fail("Exception - in newInstance - " + e.getMessage());
		}
	}

	public void test_getId() {
		sn2 = SoftNotification.newInstance(101);
		if (sn2.getId() != 101) {
			fail(" SoftNotification getId() is not correct");
		}
	}

	public void test_setImage() {
		sn2 = SoftNotification.newInstance(101);

		try {
			sn2.setImage(null);
			sn2.setImage(getImage("200x200.png"));
			sn2.setImage(null);
			
		} catch (IOException e) {
			fail(" Exception in setImage() " + e.getMessage());
		} catch (SoftNotificationException e) {
			fail(" Exception in setImage() " + e.getMessage());
		}
	}

	public void test_setText() {
		sn2 = SoftNotification.newInstance(101);
		try {
			sn2.setText(null, null);
			sn2.setText("Primary Text", "Secondary Text");
			sn2.setText(null, "Changed Secondary Text");
			sn2.setText("Changed Primary Text", null);
		} catch (SoftNotificationException e) {
			fail(" Exception in setText() " + e.getMessage());
		}
	}

	public void test_post() {
		sn2 = SoftNotification.newInstance(101);
		try {
			sn2.post();
		} catch (SoftNotificationException e) {
			fail(" Exception in post() " + e.getMessage());
		}
		try {
			sn2.remove();
			sn2.post();
			fail("Expected exception not thrown post");
		} catch (SoftNotificationException e) {
			assertTrue("Expected Exception", true);
		}
	}

	public void test_remove() {
		sn2 = SoftNotification.newInstance(101);
		try {
			sn2.remove();
		} catch (SoftNotificationException e) {
			fail(" Exception in remove() " + e.getMessage());
		}
		try {
			sn2.remove();
			fail("Expected exception not thrown remove");
		} catch (SoftNotificationException e) {
			assertTrue("Expected Exception", true);
		}
	}
}
