/*
* Copyright (c) 2008-2010 Nokia Corporation and/or its subsidiary(-ies).
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


package com.nokia.mj.impl.installer.utils;

import com.nokia.mj.impl.installer.utils.InstallerException;
import com.nokia.mj.impl.installer.utils.InstallerMain;
import com.nokia.mj.impl.installer.utils.Log;
import com.nokia.mj.impl.installer.utils.PlatformUid;
import com.nokia.mj.impl.fileutils.FileUtility;
import com.nokia.mj.impl.utils.Attribute;
import com.nokia.mj.impl.utils.JarManifestReader;
import com.nokia.mj.impl.utils.OtaStatusCode;
import com.nokia.mj.impl.utils.Uid;
import com.nokia.mj.impl.utils.exception.InvalidAttributeException;

import java.util.Hashtable;
import java.io.IOException;
import java.io.UnsupportedEncodingException;

import j2meunit.framework.Test;
import j2meunit.framework.TestCase;
import j2meunit.framework.TestMethod;
import j2meunit.framework.TestSuite;

/**
 * Unit tests for JadReader and JarManifestReader.
 */
public class AttributeReaderTest extends TestCase implements InstallerMain
{

    private static final String JAVA_BIN_ROOT = System.getProperty("JAVA_BIN_ROOT");
    private static final String TEST_ROOT_LINUX = JAVA_BIN_ROOT + "installer/jiutdata/";
    private static final String TEST_ROOT_S60 = "c:\\private\\102033E6\\installer\\jiutdata\\";

    // Begin j2meunit test framework setup

    public void installerMain(String[] args)
    {
        TestSuite suite = new TestSuite(this.getClass().getName());

        suite.addTest(new AttributeReaderTest("testSimple", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((AttributeReaderTest)tc).testSimple();
            }
        }));

        suite.addTest(new AttributeReaderTest("testCompareWithManifestParser", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((AttributeReaderTest)tc).testCompareWithManifestParser();
            }
        }));

        suite.addTest(new AttributeReaderTest("testCtrlCharsInAttributeValues", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((AttributeReaderTest)tc).testCtrlCharsInAttributeValues();
            }
        }));

        suite.addTest(new AttributeReaderTest("testSkipEmptyLines", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((AttributeReaderTest)tc).testSkipEmptyLines();
            }
        }));

        suite.addTest(new AttributeReaderTest("testContinuationLinesAndAttrWithNoValue", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((AttributeReaderTest)tc).testContinuationLinesAndAttrWithNoValue();
            }
        }));

        suite.addTest(new AttributeReaderTest("testLongLines", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((AttributeReaderTest)tc).testLongLines();
            }
        }));

        suite.addTest(new AttributeReaderTest("testLongAttrValue", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((AttributeReaderTest)tc).testLongAttrValue();
            }
        }));

        if (Platform.isS60())
        {
            suite.addTest(new AttributeReaderTest("testBigJadFile", new TestMethod()
            {
                public void run(TestCase tc)
                {
                    ((AttributeReaderTest)tc).testBigJadFile();
                }
            }));
        }

        suite.addTest(new AttributeReaderTest("testEmptyJadFile", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((AttributeReaderTest)tc).testEmptyJadFile();
            }
        }));

        suite.addTest(new AttributeReaderTest("testNotJadFile", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((AttributeReaderTest)tc).testNotJadFile();
            }
        }));

        suite.addTest(new AttributeReaderTest("testDifferentCharacterEncodings", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((AttributeReaderTest)tc).testDifferentCharacterEncodings();
            }
        }));

        suite.addTest(new AttributeReaderTest("testIllegalAttrnames", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((AttributeReaderTest)tc).testIllegalAttrnames();
            }
        }));

        suite.addTest(new AttributeReaderTest("testRecogniseCharacterEncodings", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((AttributeReaderTest)tc).testRecogniseCharacterEncodings();
            }
        }));

        suite.addTest(new AttributeReaderTest("testHugeManifest", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((AttributeReaderTest)tc).testHugeManifest();
            }
        }));

        suite.addTest(new AttributeReaderTest("testCorruptedJarFiles", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((AttributeReaderTest)tc).testCorruptedJarFiles();
            }
        }));

        suite.addTest(new AttributeReaderTest("testBomSkippingSpeed", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((AttributeReaderTest)tc).testBomSkippingSpeed();
            }
        }));



        com.nokia.mj.impl.utils.OmjTestRunner.run(suite);
    }

    public AttributeReaderTest()
    {
    }

    public AttributeReaderTest(String aTestName, TestMethod aTestMethod)
    {
        super(aTestName, aTestMethod);
    }

    public void assertFalse(String aMsg, boolean aCondition)
    {
        assertTrue(aMsg, !aCondition);
    }

    // End j2meunit test framework setup

    protected void setUp()
    {
    }

    protected void tearDown()
    {
    }

    public void testSimple()
    {
        // Just parse a couple of .jad files used in jadjarmatcher tests
        Hashtable attrs;
        try
        {
            Log.log(" ");

            attrs = JadReader.getAttributes(getTestAppsPath("HelloWorld2.jad"), "UTF-8");
            Log.log("HelloWorld2 attrs: " + attrs.toString());

            Log.log(" ");

            attrs = JadReader.getAttributes(getTestAppsPath("HelloWorld1_no_match.jad"), "UTF-8");
            Log.log("HelloWorld1_no_match attrs: " + attrs.toString());

            Log.log(" ");
        }
        catch (IOException ioe)
        {
            assertTrue("Unexpected IOException: " + ioe.toString(), false);
        }
    }

    public void testCompareWithManifestParser()
    {
        // Parse a couple of .jad files
        // and check that JadReader output is same as JarManifestReader output
        // The .jad file must be stored as meta-inf\Manifest.mf inside a .zip
        // file so that JarManifestReader can open it.
        Hashtable attrs;
        Hashtable manAttrs;
        try
        {
            Log.log(" ");


            // This .jad file has one duplicate attribute and one triplicate attribute
            attrs = JadReader.getAttributes(getUtilsPath("calc2_en_de_es_ES_fr_it_tr_TR_nl_NL_pt_PT.jad"), "UTF-8");
            Log.log("calc2 attrs: " + attrs.toString());

            Log.log(" ");

            manAttrs = JarManifestReader.getAttributes(
                           getUtilsPath("calc2_en_de_es_ES_fr_it_tr_TR_nl_NL_pt_PT.zip"));
            Log.log("calc2 manAttrs: " + manAttrs.toString());

            Log.log(" ");

            assertTrue("calc2: JadReader parse != JarManifestReader parse result", attrs.equals(manAttrs));



            Log.log(" ");

            attrs = JadReader.getAttributes(getUtilsPath("BCExchanger.jad"), "UTF-8");
            Log.log("BCExchanger attrs: " + attrs.toString());

            Log.log(" ");

            manAttrs = JarManifestReader.getAttributes(getUtilsPath("BCExchanger.zip"));
            Log.log("BCExchanger manAttrs: " + manAttrs.toString());

            Log.log(" ");

            assertTrue("BCExchanger: JadReader parse != JarManifestReader parse result", attrs.equals(manAttrs));

            Log.log(" ");


            attrs = JadReader.getAttributes(getUtilsPath("SunShines3D_DEMO_nokia_N900_EN_IGP_ATandT_901.jad"), "UTF-8");
            Log.log("SunShines3D attrs: " + attrs.toString());

            Log.log(" ");

            manAttrs = JarManifestReader.getAttributes(getUtilsPath("SunShines3D_DEMO_nokia_N900_EN_IGP_ATandT_901.zip"));
            Log.log("SunShines3D manAttrs: " + manAttrs.toString());

            Log.log(" ");

            assertTrue("SunShines3D: JadReader parse != JarManifestReader parse result", attrs.equals(manAttrs));

            Log.log(" ");


            // The following tests MANIFEST.MF that does not have EOL before EOF.
            // Also Jad file starts with UTF-8 BOM
            attrs = JadReader.getAttributes(getUtilsPath("SimpleRMS8.jad"));
            Log.log("SimpleRMS8 attrs: " + attrs.toString());

            Log.log(" ");

            manAttrs = JarManifestReader.getAttributes(getUtilsPath("SimpleRMS8.jar"));
            Log.log("SimpleRMS8 manAttrs: " + manAttrs.toString());

            Log.log(" ");

            assertTrue("SimpleRMS8: JadReader parse != JarManifestReader parse result", attrs.equals(manAttrs));

            Log.log(" ");
        }
        catch (IOException ioe)
        {
            assertTrue("Unexpected IOException: " + ioe.toString(), false);
        }
    }


    public void testCtrlCharsInAttributeValues()
    {
        // This .jad file containtains unicode $011 in MIDlet-Name value
        // and $0C in MIDlet-Install-Notify value
        Hashtable attrs;
        try
        {
            Log.log(" ");

            attrs = JadReader.getAttributes(getUtilsPath("NDRM.jad"), "UTF-8");

            assertTrue("Illegal CTL characters in attrValue were allowed.", false);
        }
        catch (IOException ioe)
        {
            Log.log("Illegal character in attribute value caused exception like is it should.");
            Log.log("Exception is: " + ioe.toString());
        }
    }


    public void testSkipEmptyLines()
    {
        // Tests also EOF at the end of the last attribute value
        // Tests also white space (SP and HT) after ':' and before attribute value
        // Tests also white space AFTER attribute value

        Hashtable attrs;
        try
        {
            Log.log(" ");

            attrs = JadReader.getAttributes(getUtilsPath("3DSpaceShooter.jad"), "UTF-8");
            Log.log("3DSpaceShooter attrs: " + attrs.toString());

            Attribute attr = (Attribute)attrs.get("Nokia-MIDlet-On-Screen-Keypad");
            String value = attr.getValue();
            assertTrue("Nokia-MIDlet-On-Screen-Keypad attribute did not have correct value.",
                       value.equals("navigationkeys"));

            Log.log(" ");
        }
        catch (IOException ioe)
        {
            assertTrue("Unexpected IOException: " + ioe.toString(), false);
        }
    }


    public void testContinuationLinesAndAttrWithNoValue()
    {
        // The line that contains MIDlet-Certificate-1-1 has several continuations

        Hashtable attrs;
        try
        {
            Log.log(" ");

            attrs = JadReader.getAttributes(getUtilsPath("mine_en_fr_de_it_es_ES_pt_PT.jad"), "UTF-8");
            Log.log("mine_en_fr_de_it_es_ES_pt_PT attrs: " + attrs.toString());

            Attribute attr = (Attribute)attrs.get("MIDlet-Certificate-1-1");
            String value = attr.getValue();
            assertTrue("MIDlet-Certificate-1-1 attribute did not have correct value. Len was: " +
                       String.valueOf(value.length()) + ". Should have been: 1208." ,
                       value.length() == 1208);

            attr = (Attribute)attrs.get("MIDlet-Jar-RSA-SHA1");
            value = attr.getValue();
            assertTrue("MIDlet-Jar-RSA-SHA1 attribute did not have correct value. Len was: " +
                       String.valueOf(value.length()) + ". Should have been: 172.",
                       value.length() == 172);

            attr = (Attribute)attrs.get("Nokia-MIDlet-Name-en");
            value = attr.getValue();
            assertTrue("Nokia-MIDlet-Name-en attribute did not have empty value",
                       value.length() == 0);

            Log.log(" ");
        }
        catch (IOException ioe)
        {
            assertTrue("Unexpected IOException: " + ioe.toString(), false);
        }
    }

    public void testHugeManifest()
    {
        Hashtable attrs;
        try
        {
            Log.log(" ");

            // Try to read corrupted .jar file, manifest size is huge
            // Read should fail gracefully
            attrs = JarManifestReader.getAttributes(getUtilsPath("00000182.jar"));
            Log.log("00000182 attrs: " + attrs.toString());

            assertTrue("Reading corrupted manifest should have failed.", false);

            Log.log(" ");
        }
        catch (IOException ioe)
        {
            Log.log("Corrupted manifest caused exception like is it should.");
            Log.log("Exception is: " + ioe.toString());
        }
    }

    public void testCorruptedJarFiles()
    {
        // Try to read some corrupted .jar files
        // Reading MUST NOT JAM. (Read must either succeed or fail gracefully)
        String[] jarFiles  = { "00000019", "00000046", "00000058", "00000070",
                               "00000090", "00000150", "00000172", "00000175", "00000189",
                               "00000250", "00000305", "00000390", "00000500", "00000582",
                               "00000703", "00000618"
                             };

        Hashtable attrs;

        Log.log("testCorruptedJarFiles starting.");

        for (int i=0; i < jarFiles.length; i++)
        {
            try
            {
                Log.log(" ");
                Log.log("Jar file is: " + jarFiles[i]);

                attrs = JarManifestReader.getAttributes(getUtilsPath(jarFiles[i] + ".jar"));
                Log.log(jarFiles[i] + " attrs: " + attrs.toString());

                Log.log(" ");
            }
            catch (UnsupportedEncodingException uee)
            {
                assertTrue("testCorruptedJarFiles: UnsupportedEncodingException: " +
                           uee.toString(), false);
            }
            catch (IOException ioe)
            {
                Log.log("Corrupted " + jarFiles[i] + ".jar caused IOException like it should.");
                Log.log("Exception is " + ioe.toString());
            }
        }
    }

    public void testLongLines()
    {
        Hashtable attrs;
        try
        {
            Log.log(" ");

            attrs = JadReader.getAttributes(getUtilsPath("PlatformReqHTTP.jad"), "UTF-8");
            Log.log("PlatformReqHTTP attrs: " + attrs.toString());

            Attribute attr = (Attribute)attrs.get("MIDlet-Certificate-1-1");
            String value = attr.getValue();
            assertTrue("MIDlet-Certificate-1-1 attribute did not have correct value. Len was: " +
                       String.valueOf(value.length()) + ". Should have been: 768.",
                       value.length() == 768);

            Log.log(" ");
        }
        catch (IOException ioe)
        {
            assertTrue("Unexpected IOException: " + ioe.toString(), false);
        }
    }

    public void testLongAttrValue()
    {
        Hashtable attrs;
        try
        {
            Log.log(" ");

            attrs = JadReader.getAttributes(getUtilsPath("DevCertTestMIDlet.jad"));
            Log.log("DevCertTestMIDlet attrs: " + attrs.toString());

            Attribute attr = (Attribute)attrs.get("MIDlet-Certificate-1-1");
            String value = attr.getValue();
            assertTrue("MIDlet-Certificate-1-1 attribute did not have correct value. Len was: " +
                       String.valueOf(value.length()) + ". Should have been: 37188.",
                       value.length() == 37188);

            Log.log(" ");
        }
        catch (IOException ioe)
        {
            assertTrue("Unexpected IOException: " + ioe.toString(), false);
        }
    }

    public void testBigJadFile()
    {
        // This jad file has 99 attributes
        Hashtable attrs;
        try
        {
            Log.log(" ");

            // Rename jad file to have a special character in the filename.
            String jadPathOrig = getUtilsPath("800_x_N97_64GB_fr_speed_v5_32_01.jad");
            String jadPath = getUtilsPath("800€_x_N97_64GB_fr_speed_v5_32_01.jad");
            try
            {
                FileUtility jadFile = new FileUtility(jadPathOrig);
                if (jadFile.exists())
                {
                    // Rename only if file has not yet been renamed.
                    jadFile.renameTo(jadPath);
                }
            }
            catch (IOException ioe)
            {
                ioe.printStackTrace();
                assertTrue("Renaming " + jadPath + " failed: " + ioe.toString(), false);
            }
            attrs = JadReader.getAttributes(jadPath, "UTF-8");
            Log.log("800€_x_N97_64GB_fr_speed_v5_32_01 attrs: " + attrs.toString());

            Attribute attr = (Attribute)attrs.get("MIDlet-Certificate-1-1");
            String value = attr.getValue();
            assertTrue("MIDlet-Certificate-1-1 attribute did not have correct value. Len was: " +
                       String.valueOf(value.length()) + ". Should have been: 1448.",
                       value.length() == 1448);

            assertTrue("Number of attributes should have been 99. It was: " +
                       String.valueOf(attrs.size()),
                       attrs.size() == 99);

            Log.log(" ");
        }
        catch (IOException ioe)
        {
            assertTrue("Unexpected IOException: " + ioe.toString(), false);
        }
    }


    public void testDifferentCharacterEncodings()
    {
        // All other tests use UTF-8 encoding,
        // test parsing a couple of .jad files written using different character encodings

        /*
        T == succeeds in emulator with J9
        F == fails in emulator with J9

        F cejp04     : JISX0208
        T ceac00     : US-ASCII
        T cecn02     : Big5
        T cecn06     : GBK
        T ceis00     : ISO-8859-1
        T ceis09     : ISO-8859-15
        F ceis14     : ISO8859_16
        F ceko01     : KOI8_R
        F cems01     : windows-1250
        F cems11     : windows-1256
        T ceut03     : UTF-16 in Big-endian Byte order with an inital Byte Order Mark
        T ceut04     : UTF-16 in Litle-Endian Byte order with an inital Byte Order Mark
        T utf8bom    : UTF-8 with an initial Byte Order Mark
        F ceut08     : UCS-2 in Litle-Endian Byte order
        T ceut09     : UCS-2 in Big-endian Byte order

        */


        String[] jadFiles  = { "cejp04",     "ceac00",     "cecn02",  "cecn06",  "ceis00",
                               "ceis09",      "ceis14",     "ceko01",     "cems01",  "cems11",  "ceut03",
                               "ceut04",      "utf8bom",    "ceut08",     "ceut09"
                             };
        String[] encodings = { "JIS0208",    "US-ASCII",   "Big5",    "GBK",     "ISO-8859-1",
                               "ISO-8859-15", "ISO8859_16", "KOI8_R",     "Cp1250",  "Cp1256",  "UTF-16BE",
                               "UTF-16LE",    "UTF-8",      "UCS-2",      "UCS-2"
                             };

        Hashtable attrs;

        int i;
        int numberOfNotSupportedEncodings = 0;
        boolean unexpectedIoExc = false;

        for (i=0; i<jadFiles.length; i++)
        {
            try
            {
                Log.log("Encoding is: " + encodings[i]);

                attrs = JadReader.getAttributes(getUtilsPath(jadFiles[i] + ".jad"), encodings[i]);
                Log.log(jadFiles[i] + " attrs: " + attrs.toString());

                Log.log(" ");
            }
            catch (UnsupportedEncodingException uee)
            {
                numberOfNotSupportedEncodings++;
                Log.log("testDifferentCharacterEncodings UnsupportedEncodingException: " + uee.toString());
            }
            catch (IOException ioe)
            {
                unexpectedIoExc = true;
                Log.log("testDifferentCharacterEncodings Unexpected IOException: " + ioe.toString());
            }
        }

        assertTrue("Less than expected number of char encodings were supported", (numberOfNotSupportedEncodings <= 5));
        assertFalse("Unexpected IOException", unexpectedIoExc);
    }

    public void testRecogniseCharacterEncodings()
    {
        // Using optional BOM (byte order marks) it is possible to detect
        // some UFT-8, UTF-16 and UTF-32 files.

        /*
          ceut03     : UTF-16 in Big-endian Byte order with an inital Byte Order Mark
          ceut04     : UTF-16 in Litle-Endian Byte order with an inital Byte Order Mark
          utf8bom    : UTF-8 with an initial Byte Order Mark
        */


        String[] jadFiles  = { "ceut03", "ceut04",      "utf8bom"};

        Hashtable attrs;
        boolean unexpectedIoExc = false;

        for (int i=0; i<jadFiles.length; i++)
        {
            try
            {
//                Log.log("Jad file is: " + jadFiles[i]);

                attrs = JadReader.getAttributes(getUtilsPath(jadFiles[i] + ".jad"));
//                Log.log(jadFiles[i] + " attrs: " + attrs.toString());

//                Log.log(" ");
            }
            catch (UnsupportedEncodingException uee)
            {
                assertTrue("testRecogniseCharacterEncodings: UnsupportedEncodingException: " +
                           uee.toString(), false);
            }
            catch (IOException ioe)
            {
                assertTrue("testRecogniseCharacterEncodings: Unexpected IOException: " +
                           ioe.toString(), false);
            }
        }
    }


    public void testBomSkippingSpeed()
    {
        Log.log("BOM Skip test, parse 3 Jad files that contain BOM 300 times");
        Log.log("BOM Skip test STARTS");

        for (int i = 0; i< 100; i++)
        {
            testRecogniseCharacterEncodings();
        }

        Log.log("BOM Skip test ENDS");
    }

    public void testEmptyJadFile()
    {
        // both .jad file with 0 bytes,
        // .jad file that contains only empty lines

        Hashtable attrs;
        try
        {
            Log.log(" ");

            attrs = JadReader.getAttributes(getUtilsPath("Empty.jad"), "UTF-8");

            assertTrue("Empty.jad should not have any attributes. Number of attributes was : " +
                       String.valueOf(attrs.size()),
                       attrs.size() == 0);

            Log.log(" ");

            attrs = JadReader.getAttributes(getUtilsPath("EmptyLines.jad"), "UTF-8");

            assertTrue("EmptyLines.jad should not have any attributes. Number of attributes was : " +
                       String.valueOf(attrs.size()),
                       attrs.size() == 0);

        }
        catch (IOException ioe)
        {
            assertTrue("testEmptyJadFile : Unexpected IOException: " + ioe.toString(), false);
        }

        assertTrue("msg", true);
    }

    public void testNotJadFile()
    {
        // What happens when JadReader tries to parse e.g. a .jar file
        // Parsing must fail with IOException
        Hashtable attrs;
        try
        {
            attrs = JadReader.getAttributes(getUtilsPath("BCExchanger.jar"), "UTF-8");

            Log.log(" ");
            assertTrue("Error: BCExchanger.jar was parsed.", false);

        }
        catch (IOException ioe)
        {
            // Fine, parsing a binary file failed like it should.
        }

        // Try parsing non-existing .jad file
        try
        {
            attrs = JadReader.getAttributes(getUtilsPath("non-existing9.jad"), "UTF-8");

            Log.log(" ");
            assertTrue("Error: Non-existing .jad was parsed.", false);

        }
        catch (IOException ioe2)
        {
            // Fine, parsing non-existing file failed like it should.
        }

    }

    public void testIllegalAttrnames()
    {
        // Empty attribute names
        Hashtable attrs;
        try
        {
            attrs = JadReader.getAttributes(getUtilsPath("TestMidlet.jad"), "UTF-8");

            Log.log(" ");
            Log.log("Empty attribute names: attrs: " + attrs.toString());
            assertTrue("Empty atribute names were parsed.", false);
        }
        catch (IOException ioe)
        {
            // Fine, parsing file failed like it should.
        }

        // CTLsAndSeparators in attribute name
        try
        {
            attrs = JadReader.getAttributes(getUtilsPath("TestMidlet2.jad"), "UTF-8");

            Log.log(" ");
            Log.log("CTLs in attribute names: attrs: " + attrs.toString());
            assertTrue("Atribute names with CTLs were parsed.", false);
        }
        catch (IOException ioe)
        {
            // Fine, parsing file failed like it should.
        }

        // attribute name is preceeded by CTLs and WSP
        try
        {
            attrs = JadReader.getAttributes(getUtilsPath("TestMidlet3.jad"), "UTF-8");

            Log.log(" ");
            Log.log("attribute name starts with CTLs: attrs: " + attrs.toString());
            assertTrue("Atribute name starting with CTLs was parsed.", false);
        }
        catch (IOException ioe)
        {
            // Fine, parsing file failed like it should.
        }
    }

    private String getUtilsPath(String aFilename)
    {
        String result = null;
        if (Platform.isLinux())
        {
            result = TEST_ROOT_LINUX + "utils/" + aFilename;
        }
        else
        {
            result = TEST_ROOT_S60 + "utils\\" + aFilename;
        }
        return result;
    }

    private String getTestAppsPath(String aFilename)
    {
        String result = null;
        if (Platform.isLinux())
        {
            result = TEST_ROOT_LINUX + "jadjarmatcher/testapps/" + aFilename;
        }
        else
        {
            result = TEST_ROOT_S60 + "jadjarmatcher\\testapps\\" + aFilename;
        }
        return result;
    }
}
