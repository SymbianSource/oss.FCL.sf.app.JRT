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
/**
 * TEST CASE SPECIFICATION<br>
 *
 * Code complexity rate (of the tested module) if measured:<br>
 *
 * The purpose of the module test:<br>
 *
 * To test MessagePart methods.<br>
 *
 * Test case descriptions and inputs:<br>
 *
 * 1 MessagePart tests<br>
 * 1.1 Construct message part with null mimeType => IllegalArgumentException<br>
 * 1.2 Construct message part with null contentId => IllegalArgumentException<br>
 * 1.3 Construct message part with non us-ascii chars in contentId => IllegalArgumentException<br>
 * 1.4 Construct message part with non us-ascii chars in contentLocation => IllegalArgumentException<br>
 * 1.5 Construct message part with too large content => SizeExceededException<br>
 * 1.6 Construct message part with valid parameters => OK (check with getters)<br>
 *
 * Used stubs and description:<br>
 *
 * Setting up the environment:<br>
 *
 * How to run the test:<br>
 *
 * Is it tested with WINSCW and/or HW:<br>
 *
 * Outputs and expected results:<br>
 *
 * Branch coverage percentage (explanation if it is under 70%):<br>
 *
 * In case some of the tests cannot be run, an explanation should be given:<br>
 *
 * Included test files:<br>
 *
 * Build number (in which the tested module is integrated):<br>
 *
 * Note whether the test case belongs to the MRT set:<br>
 *
 **/

package com.nokia.mj.test.wma;

import j2meunit.framework.*;
import javax.microedition.midlet.MIDlet;

import java.io.ByteArrayInputStream;
import java.io.InputStream;
import java.io.IOException;
import java.io.UnsupportedEncodingException;
import java.util.Vector;

import javax.wireless.messaging.MessagePart;
import javax.wireless.messaging.SizeExceededException;

public class TestMessagePart extends WmaTestBase
{

    public TestMessagePart()
    {
    }

    public TestMessagePart(String sTestName, TestMethod rTestMethod)
    {
        super(sTestName, rTestMethod);
    }



    public Test suite()
    {
        TestSuite aSuite = new TestSuite();

        aSuite.addTest(new TestMessagePart("constructMessagePartNullMimeType", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestMessagePart) tc).constructMessagePartNullMimeType();
            }
        }));


        aSuite.addTest(new TestMessagePart("constructMessagePartNullContentId", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestMessagePart) tc).constructMessagePartNullContentId();
            }
        }));


        aSuite.addTest(new TestMessagePart("constructMessagePartNonAsciiContentId", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestMessagePart) tc).constructMessagePartNonAsciiContentId();
            }
        }));


        aSuite.addTest(new TestMessagePart("constructMessagePartNonAsciiContentLocation", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestMessagePart) tc).constructMessagePartNonAsciiContentLocation();
            }
        }));


        aSuite.addTest(new TestMessagePart("constructTooLargeMessagePart", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestMessagePart) tc).constructTooLargeMessagePart();
            }
        }));


        aSuite.addTest(new TestMessagePart("constructMessagePart", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestMessagePart) tc).constructMessagePart();
            }
        }));

        return aSuite;
    }


    /**
     * 1.1 Construct message part with null mimeType => IllegalArgumentException<br>
     */
    public void constructMessagePartNullMimeType()
    {
        String content = "content";
        String mimeType = null;
        String contentId = "1";
        String contentLoc = "contentLocation";
        String enc = "utf-8";

        testMessagePartConstructorWithIllegalArgumentException(
            "null mimeType", content, mimeType, contentId, contentLoc, enc);
    }

    /**
     * 1.2 Construct message part with null contentId => IllegalArgumentException<br>
     */
    public void constructMessagePartNullContentId()
    {
        String content = "content";
        String mimeType = "text/plain";
        String contentId = null;
        String contentLoc = "contentLocation";
        String enc = "utf-8";

        testMessagePartConstructorWithIllegalArgumentException(
            "null contentId", content, mimeType, contentId, contentLoc, enc);
    }

    /**
     * 1.3 Construct message part with non us-ascii chars in contentId
     * => IllegalArgumentException<br>
     */
    public void constructMessagePartNonAsciiContentId()
    {
        String content = "content";
        String mimeType = "text/plain";
        String contentId = "non us-ascii contentId: €";
        String contentLoc = "contentLocation";
        String enc = "utf-8";

        testMessagePartConstructorWithIllegalArgumentException(
            "non us-ascii contentId", content, mimeType, contentId,
            contentLoc, enc);
    }

    /**
     * 1.4 Construct message part with non us-ascii chars in contentLocation
     * => IllegalArgumentException<br>
     */
    public void constructMessagePartNonAsciiContentLocation()
    {
        String content = "content";
        String mimeType = "text/plain";
        String contentId = "1";
        String contentLoc = "non us-ascii contentLocation: €";
        String enc = "utf-8";

        testMessagePartConstructorWithIllegalArgumentException(
            "non us-ascii contentLocation", content, mimeType, contentId,
            contentLoc, enc);
    }

    /**
     * 1.5 Construct message part with too large content => SizeExceededException<br>
     */
    public void constructTooLargeMessagePart()
    {
        MessagePart msgPart = null;
        String content = getTooLargeContent();
        String mimeType = "text/plain";
        String contentId = "1";
        String contentLoc = "contentLocation";
        String enc = "iso-8859-1";

        try
        {
            msgPart = new MessagePart(
                content.getBytes(enc), 0, content.length(), mimeType,
                contentId, contentLoc, enc);
            assertTrue(
                "MessagePart1 creation with too large content "
                + "should not succeed, size=" + MAX_CONTENT_SIZE,false);
        }
        catch (SizeExceededException se)
        {
            // passed
        }
        catch (Exception e)
        {
            assertTrue(
                "MessagePart1 creation with too large content ("
                + MAX_CONTENT_SIZE
                + ") failed with wrong exception: " + e.toString(),false);
        }

        try
        {
            msgPart = new MessagePart(
                content.getBytes(enc), mimeType, contentId, contentLoc, enc);
            assertTrue(
                "MessagePart2 creation with too large content "
                + "should not succeed, size=" + MAX_CONTENT_SIZE,false);
        }
        catch (SizeExceededException se)
        {
            // passed
        }
        catch (Exception e)
        {
            assertTrue(
                "MessagePart2 creation with too large content ("
                + MAX_CONTENT_SIZE
                + ") failed with wrong exception: " + e.toString(),false);
        }

        try
        {
            msgPart = new MessagePart(
                getInputStream(content), mimeType, contentId, contentLoc, enc);
            assertTrue(
                "MessagePart3 creation with too large content "
                + "should not succeed, size=" + MAX_CONTENT_SIZE,false);
        }
        catch (SizeExceededException se)
        {
            // passed
        }
        catch (Exception e)
        {
            assertTrue(
                "MessagePart3 creation with too large content ("
                + MAX_CONTENT_SIZE
                + ") failed with wrong exception: " + e.toString(),false);
        }

        // construct a MessagePart with too large content location
        try
        {
            msgPart = new MessagePart(
                getInputStream(getContentString(10*1024)), getContentString(7*1024), getContentString(2*1024), getContentString(1*1024), enc);
            assertTrue(
                "MessagePart3 creation with too large headers "
                + "should not succeed, size",false);
        }
        catch (SizeExceededException se)
        {
            // passed
        }
        catch (Exception e)
        {
            assertTrue(
                "MessagePart3 creation with too large headers "
                + "failed with wrong exception: " + e.toString(),false);
        }

        // construct a MessagePart with too large headers
        try
        {
            msgPart = new MessagePart(
                getInputStream(getContentString(10*1024)), getContentString(5000), getContentString(4745), getContentString(256), enc);
            assertTrue(
                "MessagePart3 creation with too large headers "
                + "should not succeed, size",false);
        }
        catch (SizeExceededException se)
        {
            // passed
        }
        catch (Exception e)
        {
            assertTrue(
                "MessagePart3 creation with too large headers "
                + "failed with wrong exception: " + e.toString(),false);
        }

        assertTrue("",true);
    }

    /**
     * 1.6 Construct message part with valid parameters => OK (check with getters)<br>
     */
    public void constructMessagePart()
    {
        MessagePart msgPart = null;
        String content = "content";
        String mimeType = "text/plain";
        String contentId = "1";
        String contentLoc = "\\some\\path/res/contentLocation";
        String enc = "iso-8859-1";
        String checkResult = null;

        try
        {
            msgPart = new MessagePart(
                content.getBytes(enc), 0, content.length(), mimeType,
                contentId, contentLoc, enc);
            checkResult = checkMessagePart(
                              msgPart, content, mimeType, contentId, contentLoc, enc);
            if (checkResult != null)
            {
                assertTrue("MessagePart1 creation failed: " + checkResult,false);
            }
        }
        catch (Exception e)
        {
            assertTrue("MessagePart1 creation failed with exception: "
                       + e.toString(),false);
        }

        try
        {
            msgPart = new MessagePart(
                content.getBytes(enc), mimeType, contentId, contentLoc, enc);
            checkResult = checkMessagePart(
                              msgPart, content, mimeType, contentId, contentLoc, enc);
            if (checkResult != null)
            {
                assertTrue("MessagePart2 creation failed: " + checkResult,false);
            }
        }
        catch (Exception e)
        {
            assertTrue("MessagePart2 creation failed with exception: "
                       + e.toString(),false);
        }

        try
        {
            msgPart = new MessagePart(
                getInputStream(content), mimeType, contentId, contentLoc, enc);
            checkResult = checkMessagePart(
                              msgPart, content, mimeType, contentId, contentLoc, enc);
            if (checkResult != null)
            {
                assertTrue("MessagePart3 creation failed: " + checkResult,false);
            }
        }
        catch (Exception e)
        {
            assertTrue("MessagePart3 creation failed with exception: "
                       + e.toString(),false);
        }

        assertTrue("",true);
    }

    // BEGIN - Private helper methods for this test suite.

    /**
     * Calls all MessagePart constructors with given parameters and
     * expects IllegalArgumentException to be thrown.
     */
    private void testMessagePartConstructorWithIllegalArgumentException(
        String msg, String content, String mimeType, String contentId,
        String contentLoc, String enc)
    {
        MessagePart msgPart = null;

        try
        {
            msgPart = new MessagePart(content.getBytes(enc), 0,
                                      content.length(), mimeType, contentId, contentLoc, enc);
            assertTrue("MessagePart1 creation with " + msg
                       + " should not succeed",false);
        }
        catch (IllegalArgumentException iae)
        {
            // passed
        }
        catch (Exception e)
        {
            assertTrue("MessagePart1 creation with " + msg
                       + " failed with wrong exception: " + e.toString(),false);
        }

        try
        {
            msgPart = new MessagePart(content.getBytes(enc), mimeType,
                                      contentId, contentLoc, enc);
            assertTrue("MessagePart2 creation with " + msg
                       + " should not succeed",false);
        }
        catch (IllegalArgumentException iae)
        {
            // passed
        }
        catch (Exception e)
        {
            assertTrue("MessagePart2 creation with " + msg
                       + " failed with wrong exception: " + e.toString(),false);
        }

        try
        {
            msgPart = new MessagePart(getInputStream(content), mimeType,
                                      contentId, contentLoc, enc);
            assertTrue("MessagePart3 creation with " + msg
                       + " should not succeed",false);
        }
        catch (IllegalArgumentException iae)
        {
            // passed
        }
        catch (Exception e)
        {
            assertTrue("MessagePart3 creation with " + msg
                       + " failed with wrong exception: " + e.toString(),false);
        }

        assertTrue("",true);
    }

    /**
     * Checks that given MessagePart contains given data.
     */
    private String checkMessagePart(
        MessagePart msgPart, String content, String mimeType,
        String contentId, String contentLoc, String enc)
    {
        if (msgPart == null)
        {
            return "MessagePart is null";
        }
        if (mimeType != null && !mimeType.equals(msgPart.getMIMEType()))
        {
            return "wrong MimeType, got: " + msgPart.getMIMEType()
                   + ", expected: " + mimeType;
        }
        if (contentId != null && !contentId.equals(msgPart.getContentID()))
        {
            return "wrong contentId, got: " + msgPart.getContentID()
                   + ", expected: " + contentId;
        }
        if (contentLoc != null && !contentLoc.equals(msgPart.getContentLocation()))
        {
            return "wrong contentLoc, got: " + msgPart.getContentLocation()
                   + ", expected: " + contentLoc;
        }
        if (enc != null && !enc.equals(msgPart.getEncoding()))
        {
            return "wrong encoding, got: " + msgPart.getEncoding()
                   + ", expected: " + enc;
        }
        if (content != null)
        {
            // Check that getContent returns correct byte array.
            byte contentBytes[] = content.getBytes();
            if (enc != null)
            {
                try
                {
                    contentBytes = content.getBytes(enc);
                }
                catch (UnsupportedEncodingException use)
                {
                    return use.toString();
                }
            }
            byte msgBytes[] = msgPart.getContent();
            if (contentBytes.length != msgBytes.length)
            {
                return "wrong content, got getContent().length: "
                       + msgBytes.length + ", expected: " + contentBytes.length;
            }
            for (int i = 0; i < contentBytes.length; i++)
            {
                if (contentBytes[i] != msgBytes[i])
                {
                    return "wrong content, got getContent[" + i + "]: "
                           + msgBytes[i] + ", expected: " + contentBytes[i];
                }
            }
            // Check that getLength returns correct length.
            if (content.length() != msgPart.getLength())
            {
                return "wrong content length, got getLength(): "
                       + msgPart.getLength() + ", expected: " + content.length();
            }
            // Check that getContentAsStream returns correct content.
            InputStream contentStream = getInputStream(content);
            InputStream msgStream = msgPart.getContentAsStream();
            try
            {
                for (int i = 0; contentStream.available() > 0; i++)
                {
                    int i1, i2;
                    if ((i1 = contentStream.read()) !=
                            (i2 = msgStream.read()))
                    {
                        return "wrong content, got getContentAsStream["
                               + i + "]: " + i2 + ", expected: " + i1;
                    }
                }
            }
            catch (IOException ioe)
            {
                return "wrong content stream, got exception: " + ioe;
            }
        }
        return null;
    }

    // END - Private helper methods for this test suite.
}
