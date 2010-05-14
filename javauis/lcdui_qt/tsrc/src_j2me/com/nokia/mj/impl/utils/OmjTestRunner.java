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


package com.nokia.mj.impl.utils;

import junit.framework.AssertionFailedError;
import junit.framework.Test;
import junit.framework.TestCase;
import junit.framework.TestFailure;
import junit.framework.TestListener;
import junit.framework.TestResult;
import junit.framework.TestSuite;
import junit.textui.TestRunner;

import java.io.ByteArrayOutputStream;
import java.io.IOException;
import java.io.OutputStream;
import java.io.PrintStream;
import java.net.InetAddress;
import java.util.Date;
import java.util.Enumeration;
import java.util.Vector;
import javax.microedition.io.file.FileConnection;
import javax.microedition.io.Connector;


public class OmjTestRunner extends TestRunner
{
    protected static final String JAVA_BIN_ROOT =
        System.getProperty("JAVA_BIN_ROOT");
    protected static String RESULTS_DIR = null;
    protected static final String RESULT_FILE_ENCODING = "UTF-8";
    protected String iResultFilename = null;
    protected long iStartTime = 0;
    protected long iEndTime = 0;
    protected Test iSuite = null;
    protected Vector iTests = new Vector();
    protected String iTestClassName = null;

    static
    {
        String platform = System.getProperty("os.name");

        if(platform != null && platform.equalsIgnoreCase("linux"))
        {
            String JAVA_BIN_ROOT = System.getProperty("JAVA_BIN_ROOT");
            RESULTS_DIR = "file://";
            RESULTS_DIR = RESULTS_DIR + (JAVA_BIN_ROOT == null? ".": JAVA_BIN_ROOT) + "/";
        }
        else
        {
            RESULTS_DIR = System.getProperty("fileconn.dir.photos");
        }
    }

    protected OmjTestRunner()
    {
        super();
    }

    public OmjTestRunner(Test aSuite)
    {
        super();
        iSuite = aSuite;
        setTestClassName(aSuite.toString());
        iStartTime = System.currentTimeMillis();
        System.out.println("Results: "+iResultFilename);
    }

    public void setTestClassName(String aTestClassName)
    {
        iResultFilename = RESULTS_DIR + "results/" + aTestClassName + ".xml";
        iTestClassName = aTestClassName;
    }

    public OmjTestRunner(PrintStream aWriter)
    {
        super(aWriter);
    }

    public void endTest(Test aTest)
    {
        System.out.println("endtest");
        iTests.addElement(aTest);
    }

    public void writeResultFile(TestResult aResult)
    {
        iEndTime = System.currentTimeMillis();
        String xmlResult = toString(iSuite, iTestClassName, iTests, aResult,
                                    iEndTime - iStartTime);
        FileConnection fileConn = null;
        OutputStream outStream = null;
        try
        {
            //System.out.println("RESULTS_DIR: " + RESULTS_DIR);
            fileConn = (FileConnection) Connector.open(RESULTS_DIR,Connector.READ_WRITE);
            if(!fileConn.exists())
            {
                fileConn.mkdir();
            }
            fileConn.close();

            RESULTS_DIR = RESULTS_DIR + "results/";
            fileConn = (FileConnection) Connector.open(RESULTS_DIR,Connector.READ_WRITE);
            if(!fileConn.exists())
            {
                fileConn.mkdir();
            }
            fileConn.close();

            //System.out.println("iResultFilename: " + iResultFilename);
            fileConn = (FileConnection) Connector.open(iResultFilename,Connector.READ_WRITE);
            if(!fileConn.exists())
            {
                fileConn.create();
            }
            else
            {
                fileConn.delete();
                fileConn.create();
            }
            outStream = fileConn.openOutputStream();
            outStream.write(xmlResult.getBytes(RESULT_FILE_ENCODING));
            outStream.flush();
        }
        catch(IOException ex)
        {
            throw new RuntimeException
            ("Writing results to " + iResultFilename + " failed: " + ex);
        }
        finally
        {
            try
            {
                if(null != fileConn)
                    fileConn.close();
                if(null != outStream)
                    outStream.close();
            }
            catch(IOException ioe) {}
        }
    }

    // Removes errors and failures from aTests which contain all tests.
    protected static void removeErrorsAndFailures
    (Vector aTests, TestResult aResult)
    {
        Enumeration e = aResult.errors();
        while(e.hasMoreElements())
        {
            TestFailure testFailure = (TestFailure)e.nextElement();
            TestCase testCase = (TestCase)testFailure.failedTest();
            aTests.removeElement(testCase);
        }

        e = aResult.failures();
        while(e.hasMoreElements())
        {
            TestFailure testFailure = (TestFailure)e.nextElement();
            TestCase testCase = (TestCase)testFailure.failedTest();
            aTests.removeElement(testCase);
        }
    }

    protected static String toString
    (Test aSuite, String aTestClassName, Vector aTests, TestResult aResult, long aTime)
    {
        removeErrorsAndFailures(aTests, aResult);

        StringBuffer buf = new StringBuffer
        ("<?xml version=\"1.0\" encoding=\"" +
         RESULT_FILE_ENCODING + "\" ?>\n");

        buf.append("<testsuite errors=\"").append(aResult.errorCount())
        .append("\" failures=\"").append(aResult.failureCount());

        buf.append("\" name=\"").append(aTestClassName)
        .append("\" tests=\"").append(aResult.runCount())
        .append("\" time=\"").append(aTime/1000.0)
        .append("\" timestamp=\"").append(new Date().toString())
        .append("\">\n");

        Enumeration e = aTests.elements();
        while(e.hasMoreElements())
        {
            TestCase testCase = (TestCase)e.nextElement();
            buf.append(toString(testCase));
        }

        e = aResult.errors();
        while(e.hasMoreElements())
        {
            TestFailure testFailure = (TestFailure)e.nextElement();
            buf.append(toString(testFailure, "error"));
        }

        e = aResult.failures();
        while(e.hasMoreElements())
        {
            TestFailure testFailure = (TestFailure)e.nextElement();
            buf.append(toString(testFailure, "failure"));
        }

        buf.append("</testsuite>\n");
        return buf.toString();
    }

    protected static String toString(Test aTest)
    {
        StringBuffer buf = new StringBuffer("");
        TestCase testCase = null;
        if(aTest instanceof TestCase)
        {
            testCase = (TestCase)aTest;
        }
        if(testCase != null)
        {
            buf.append("  <testcase classname=\"").append(testCase.getClass().getName())
            .append("\" name=\"").append(testCase.getName())
            .append("\"/>\n");
        }
        return buf.toString();
    }

    protected static String toString(TestFailure aFailure, String aFailureTag)
    {
        StringBuffer buf = new StringBuffer("");
        TestCase testCase = (TestCase)aFailure.failedTest();
        Throwable thrownException = aFailure.thrownException();
        if(testCase != null)
        {
            buf.append("  <testcase classname=\"").append(testCase.getClass().getName())
            .append("\" name=\"").append(testCase.getName());
            if(thrownException == null)
            {
                buf.append("\"/>\n");
            }
            else
            {
                buf.append("\">\n")
                .append(toString(thrownException, aFailureTag))
                .append("  </testcase>\n");
            }
        }
        else if(thrownException == null)
        {
            buf.append(toString(thrownException, aFailureTag));
        }
        return buf.toString();
    }

    protected static String toString(Throwable aThrowable, String aFailureTag)
    {
        StringBuffer buf = new StringBuffer("");
        if(aThrowable != null)
        {
            String throwableString = aThrowable.toString();
            try
            {
                ByteArrayOutputStream outputStream = new ByteArrayOutputStream();
                aThrowable.printStackTrace(new PrintStream(outputStream));
                throwableString = outputStream.toString();
            }
            catch(Throwable t)
            {
                // ignore
            }
            buf.append("  <").append(aFailureTag).append(" message=\"").append(aThrowable.getMessage())
            .append("\" type=\"").append(aThrowable.getClass().getName())
            .append("\">").append(throwableString)
            .append("</").append(aFailureTag).append(">\n");
        }
        return buf.toString();
    }
}
