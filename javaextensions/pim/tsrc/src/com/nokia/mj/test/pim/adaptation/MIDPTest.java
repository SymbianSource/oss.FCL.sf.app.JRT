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


package com.nokia.mj.test.pim.adaptation;

import j2meunit.framework.*;
import javax.microedition.lcdui.*;
import javax.microedition.midlet.MIDlet;


import javax.microedition.pim.*;
import com.nokia.mj.test.pim.automatic.Contact.CConstants.*;
import com.nokia.mj.test.pim.automatic.Contact.CgetPreferredIndex.*;
import com.nokia.mj.test.pim.automatic.Contact.CgetPreferredIndexEx.*;
import com.nokia.mj.test.pim.automatic.ContactList.CLcreateContact.*;
import com.nokia.mj.test.pim.automatic.ContactList.CLimportContact.*;
import com.nokia.mj.test.pim.automatic.ContactList.CLmultipleLists.*;
import com.nokia.mj.test.pim.automatic.ContactList.CLremoveContact.*;
import com.nokia.mj.test.pim.automatic.ContactList.CLremoveContactEx.*;
import com.nokia.mj.test.pim.automatic.Event.EConstants.*;
import com.nokia.mj.test.pim.automatic.Event.ERepeat.*;
import com.nokia.mj.test.pim.automatic.EventList.ELConstants.*;
import com.nokia.mj.test.pim.automatic.EventList.ELcreateEvent.*;
import com.nokia.mj.test.pim.automatic.EventList.ELgetSupportedRRFields.*;
import com.nokia.mj.test.pim.automatic.EventList.ELimportEvent.*;
import com.nokia.mj.test.pim.automatic.EventList.ELitems.*;
import com.nokia.mj.test.pim.automatic.EventList.ELitemsEx.*;
import com.nokia.mj.test.pim.automatic.EventList.ELmultipleLists.*;
import com.nokia.mj.test.pim.automatic.EventList.ELremoveEvent.*;
import com.nokia.mj.test.pim.automatic.EventList.ELremoveEventEx.*;
import com.nokia.mj.test.pim.automatic.FieldEmptyException.FEEgetField.*;
import com.nokia.mj.test.pim.automatic.FieldFullException.FFEgetField.*;
import com.nokia.mj.test.pim.automatic.PIM.PConstants.*;
import com.nokia.mj.test.pim.automatic.PIM.PlistPIMlists.*;
import com.nokia.mj.test.pim.automatic.PIM.PopenPIMlist.*;
import com.nokia.mj.test.pim.automatic.PIM.PopenPIMlist2.*;
import com.nokia.mj.test.pim.automatic.PIM.Pserialization.*;
import com.nokia.mj.test.pim.automatic.PIM.PsupportedSerialFormats.*;
import com.nokia.mj.test.pim.automatic.PIMException.PEconstants.*;
import com.nokia.mj.test.pim.automatic.PIMException.PEgetReason.*;
import com.nokia.mj.test.pim.automatic.PIMItem.PIaddBinary.*;
import com.nokia.mj.test.pim.automatic.PIMItem.PIaddBinaryEx.*;
import com.nokia.mj.test.pim.automatic.PIMItem.PIaddBoolean.*;
import com.nokia.mj.test.pim.automatic.PIMItem.PIaddDate.*;
import com.nokia.mj.test.pim.automatic.PIMItem.PIaddDateEx.*;
import com.nokia.mj.test.pim.automatic.PIMItem.PIaddInt.*;
import com.nokia.mj.test.pim.automatic.PIMItem.PIaddString.*;
import com.nokia.mj.test.pim.automatic.PIMItem.PIaddStringArray.*;
import com.nokia.mj.test.pim.automatic.PIMItem.PIaddStringArrayEx.*;
import com.nokia.mj.test.pim.automatic.PIMItem.PIcategories.*;
import com.nokia.mj.test.pim.automatic.PIMItem.PIcommit.*;
import com.nokia.mj.test.pim.automatic.PIMItem.PIcommit2.*;
import com.nokia.mj.test.pim.automatic.PIMItem.PIConstants.*;
import com.nokia.mj.test.pim.automatic.PIMItem.PIgetAttributes.*;
import com.nokia.mj.test.pim.automatic.PIMItem.PIgetFields.*;
import com.nokia.mj.test.pim.automatic.PIMItem.PIgetPIMList.*;
import com.nokia.mj.test.pim.automatic.PIMItem.PIremoveValue.*;
import com.nokia.mj.test.pim.automatic.PIMItem.PIremoveValueEx.*;
import com.nokia.mj.test.pim.automatic.PIMItem.PIsetBinary.*;
import com.nokia.mj.test.pim.automatic.PIMItem.PIsetBinaryEx.*;
import com.nokia.mj.test.pim.automatic.PIMItem.PIsetBoolean.*;
import com.nokia.mj.test.pim.automatic.PIMItem.PIsetDate.*;
import com.nokia.mj.test.pim.automatic.PIMItem.PIsetDateEx.*;
import com.nokia.mj.test.pim.automatic.PIMItem.PIsetInt.*;
import com.nokia.mj.test.pim.automatic.PIMItem.PIsetString.*;
import com.nokia.mj.test.pim.automatic.PIMItem.PIsetStringArray.*;
import com.nokia.mj.test.pim.automatic.PIMItem.PIsetStringArrayEx.*;
import com.nokia.mj.test.pim.automatic.PIMList.PLaddCategory.*;
import com.nokia.mj.test.pim.automatic.PIMList.PLattributes.*;
import com.nokia.mj.test.pim.automatic.PIMList.PLclose.*;
import com.nokia.mj.test.pim.automatic.PIMList.PLConstants.*;
import com.nokia.mj.test.pim.automatic.PIMList.PLdeleteCategory.*;
import com.nokia.mj.test.pim.automatic.PIMList.PLfields.*;
import com.nokia.mj.test.pim.automatic.PIMList.PLgetName.*;
import com.nokia.mj.test.pim.automatic.PIMList.PLisCategory.*;
import com.nokia.mj.test.pim.automatic.PIMList.PLitems.*;
import com.nokia.mj.test.pim.automatic.PIMList.PLitems2.*;
import com.nokia.mj.test.pim.automatic.PIMList.PLitems2Ex.*;
import com.nokia.mj.test.pim.automatic.PIMList.PLitemsByCategory.*;
import com.nokia.mj.test.pim.automatic.PIMList.PLrenameCategory.*;
import com.nokia.mj.test.pim.automatic.PIMList.PLstringArrays.*;
import com.nokia.mj.test.pim.automatic.RepeatRule.RRintField.*;
import com.nokia.mj.test.pim.automatic.RepeatRule.RRConstants.*;
import com.nokia.mj.test.pim.automatic.RepeatRule.RRdates.*;
import com.nokia.mj.test.pim.automatic.RepeatRule.RRdates2.*;
import com.nokia.mj.test.pim.automatic.RepeatRule.RRdates3.*;
import com.nokia.mj.test.pim.automatic.RepeatRule.RRdates4.*;
import com.nokia.mj.test.pim.automatic.RepeatRule.RRdatesExcept.*;
import com.nokia.mj.test.pim.automatic.RepeatRule.RRequals.*;
import com.nokia.mj.test.pim.automatic.RepeatRule.RRequals2.*;
import com.nokia.mj.test.pim.automatic.RepeatRule.RRexceptDates.*;
import com.nokia.mj.test.pim.automatic.RepeatRule.RRgetDate.*;
import com.nokia.mj.test.pim.automatic.RepeatRule.RRgetFields.*;
import com.nokia.mj.test.pim.automatic.RepeatRule.RRsetDate.*;
import com.nokia.mj.test.pim.automatic.System.Properties.*;
import com.nokia.mj.test.pim.automatic.ToDo.TConstants.*;
import com.nokia.mj.test.pim.automatic.ToDoList.TLcreateToDo.*;
import com.nokia.mj.test.pim.automatic.ToDoList.TLimportToDo.*;
import com.nokia.mj.test.pim.automatic.ToDoList.TLitems.*;
import com.nokia.mj.test.pim.automatic.ToDoList.TLitemsEx.*;
import com.nokia.mj.test.pim.automatic.ToDoList.TLmultipleLists.*;
import com.nokia.mj.test.pim.automatic.ToDoList.TLremoveToDo.*;
import com.nokia.mj.test.pim.automatic.UnsupportedFieldException.UFEgetField.*;
import com.nokia.mj.test.pim.automatic.OpenMultipleLists.*;
import com.nokia.mj.test.pim.automatic.PIM.PfromSerialFormat.PfromSerialFormat.*;
import com.nokia.mj.test.pim.automatic.PIM.PfromSerialFormat.PfromSerialFormatEx.*;
import com.nokia.mj.test.pim.automatic.PIM.PfromSerialFormat.PfromSFormatInvalid.*;
import com.nokia.mj.test.pim.automatic.PIM.PtoSerialFormat.PtoSerialFormat.*;
import com.nokia.mj.test.pim.automatic.PIM.PtoSerialFormat.PtoSerialFormatEx.*;
import com.nokia.mj.test.pim.automatic.RepeatRule.RRFieldCount.*;
import com.nokia.mj.test.pim.automatic.RepeatRule.RRFieldDayInMonth.*;
import com.nokia.mj.test.pim.automatic.RepeatRule.RRFieldDayInWeek.*;
import com.nokia.mj.test.pim.automatic.RepeatRule.RRFieldDayInYear.*;
import com.nokia.mj.test.pim.automatic.RepeatRule.RRFieldFrequency.*;
import com.nokia.mj.test.pim.automatic.RepeatRule.RRFieldInterval.*;
import com.nokia.mj.test.pim.automatic.RepeatRule.RRFieldMonthInYear.*;
import com.nokia.mj.test.pim.automatic.RepeatRule.RRFieldWeekInMonth.*;

// Variation imports
import com.nokia.mj.test.pim.automatic.ContactList.CLcategoryTests.*;
import com.nokia.mj.test.pim.automatic.ContactList.CLfieldsOrder.*;
import com.nokia.mj.test.pim.automatic.ContactList.CLgetAttributes.*;
import com.nokia.mj.test.pim.automatic.ContactList.CLmaxValues.*;
import com.nokia.mj.test.pim.automatic.ContactList.CLserialFormatTests.*;
import com.nokia.mj.test.pim.automatic.ContactList.CLstringMaxLen.*;
import com.nokia.mj.test.pim.automatic.ContactList.CLsupportedArrayElements.*;
import com.nokia.mj.test.pim.automatic.ContactList.CLsupportedAttributes.*;
import com.nokia.mj.test.pim.automatic.ContactList.CLsupportedFields.*;
import com.nokia.mj.test.pim.automatic.EventList.ELfieldsOrder.*;
import com.nokia.mj.test.pim.automatic.EventList.ELserialFormatTests.*;
import com.nokia.mj.test.pim.automatic.EventList.ELstringMaxLen.*;
import com.nokia.mj.test.pim.automatic.EventList.ELsupportedFields.*;
import com.nokia.mj.test.pim.automatic.EventList.ELsupportedRRFields2.*;

import com.nokia.mj.test.pim.automatic.ToDoList.TLfieldsOrder.*;
import com.nokia.mj.test.pim.automatic.ToDoList.TLpriorityMappings.*;
import com.nokia.mj.test.pim.automatic.ToDoList.TLserialFormatTests.*;
import com.nokia.mj.test.pim.automatic.ToDoList.TLstringMaxLen.*;
import com.nokia.mj.test.pim.automatic.ToDoList.TLsupportedFields.*;
import com.nokia.mj.test.pim.automatic.Event.Ecommit.*;
import com.nokia.mj.test.pim.automatic.ToDo.Tcommit.*;
import com.nokia.mj.test.pim.automatic.Contact.CcommitPartial.*;
import com.nokia.mj.test.pim.automatic.Contact.CcommitUpdate.*;

import com.nokia.mj.test.pim.automatic.*;
/**

 * <b>TEST CASE SPECIFICATION</b>
 *
  * <br><br><b>Short description of the module test:</b>
 * <p>
 *
 * <br><br><b>Used stubs and description:</b>
 * <p>
 *
 * <br><br><b>Setting up the environment:</b> See the general test environment specification. <!-- change this if necessary -->
 *
 * <br><br><b>Included test files:</b>
 * <p>
 *
 * <br><br><b>Note whether the test case belongs to the MRT set:</b> No
 * <p>
 *
 */

public class MIDPTest extends TestCase
{
    //protected SpedeRunner spede;

    private long iFreeMemoryAtStart;

    public MIDPTest()
    {
    }

    public MIDPTest(String sTestName, TestMethod rTestMethod)
    {
        super(sTestName, rTestMethod);
//      spede = (SpedeRunner)SpedeRunner.getInstance();
    }

    /**
     * Any pretest setup can be done here
     */
    protected void setUp() throws Exception
    {
//        if (spede == null) {
//          spede = (SpedeRunner)SpedeRunner.getInstance();
//      }
        iFreeMemoryAtStart = Runtime.getRuntime().freeMemory();
    }

    /***************************************
     * Creates the test suite. You need to add a new aSuite.addTest antry for any new test methods, otherwise they won't be run.
     */
    public Test suite()
    {
        TestSuite aSuite = new TestSuite();
        try
        {
            aSuite.addTest(new CConstants("CConstants", new TestMethod()
            {
                public void run(TestCase tc) throws Throwable
                {
                    ((CConstants) tc).runTest();
                }
            }));

            aSuite.addTest(new CgetPreferredIndex("CgetPreferredIndex", new TestMethod()
            {
                public void run(TestCase tc) throws Throwable
                {
                    ((CgetPreferredIndex) tc).runTest();
                }
            }));

            aSuite.addTest(new CgetPreferredIndexEx("CgetPreferredIndexEx", new TestMethod()
            {
                public void run(TestCase tc) throws Throwable
                {
                    ((CgetPreferredIndexEx) tc).runTest();
                }
            }));

            aSuite.addTest(new CLcreateContact("CLcreateContact", new TestMethod()
            {
                public void run(TestCase tc) throws Throwable
                {
                    ((CLcreateContact) tc).runTest();
                }
            }));

            aSuite.addTest(new CLimportContact("CLimportContact", new TestMethod()
            {
                public void run(TestCase tc) throws Throwable
                {
                    ((CLimportContact) tc).runTest();
                }
            }));

            aSuite.addTest(new CLmultipleLists("CLmultipleLists", new TestMethod()
            {
                public void run(TestCase tc) throws Throwable
                {
                    ((CLmultipleLists) tc).runTest();
                }
            }));

            aSuite.addTest(new CLremoveContact("CLremoveContact", new TestMethod()
            {
                public void run(TestCase tc) throws Throwable
                {
                    ((CLremoveContact) tc).runTest();
                }
            }));

            aSuite.addTest(new CLremoveContactEx("CLremoveContactEx", new TestMethod()
            {
                public void run(TestCase tc) throws Throwable
                {
                    ((CLremoveContactEx) tc).runTest();
                }
            }));

            aSuite.addTest(new EConstants("EConstants", new TestMethod()
            {
                public void run(TestCase tc) throws Throwable
                {
                    ((EConstants) tc).runTest();
                }
            }));

            aSuite.addTest(new ERepeat("ERepeat", new TestMethod()
            {
                public void run(TestCase tc) throws Throwable
                {
                    ((ERepeat) tc).runTest();
                }
            }));

            aSuite.addTest(new ELConstants("ELConstants", new TestMethod()
            {
                public void run(TestCase tc) throws Throwable
                {
                    ((ELConstants) tc).runTest();
                }
            }));

            aSuite.addTest(new ELcreateEvent("ELcreateEvent", new TestMethod()
            {
                public void run(TestCase tc) throws Throwable
                {
                    ((ELcreateEvent) tc).runTest();
                }
            }));

            aSuite.addTest(new ELsupportedRRFields("ELsupportedRRFields", new TestMethod()
            {
                public void run(TestCase tc) throws Throwable
                {
                    ((ELsupportedRRFields) tc).runTest();
                }
            }));

            aSuite.addTest(new ELimportEvent("ELimportEvent", new TestMethod()
            {
                public void run(TestCase tc) throws Throwable
                {
                    ((ELimportEvent) tc).runTest();
                }
            }));

            aSuite.addTest(new ELitems("ELitems", new TestMethod()
            {
                public void run(TestCase tc) throws Throwable
                {
                    ((ELitems) tc).runTest();
                }
            }));

            aSuite.addTest(new ELitemsEx("ELitemsEx", new TestMethod()
            {
                public void run(TestCase tc) throws Throwable
                {
                    ((ELitemsEx) tc).runTest();
                }
            }));

            aSuite.addTest(new ELmultipleLists("ELmultipleLists", new TestMethod()
            {
                public void run(TestCase tc) throws Throwable
                {
                    ((ELmultipleLists) tc).runTest();
                }
            }));

            aSuite.addTest(new ELremoveEvent("ELremoveEvent", new TestMethod()
            {
                public void run(TestCase tc) throws Throwable
                {
                    ((ELremoveEvent) tc).runTest();
                }
            }));

            aSuite.addTest(new ELremoveEventEx("ELremoveEventEx", new TestMethod()
            {
                public void run(TestCase tc) throws Throwable
                {
                    ((ELremoveEventEx) tc).runTest();
                }
            }));

            aSuite.addTest(new FEEgetField("FEEgetField", new TestMethod()
            {
                public void run(TestCase tc) throws Throwable
                {
                    ((FEEgetField) tc).runTest();
                }
            }));

            aSuite.addTest(new FFEgetField("FFEgetField", new TestMethod()
            {
                public void run(TestCase tc) throws Throwable
                {
                    ((FFEgetField) tc).runTest();
                }
            }));

            aSuite.addTest(new PConstants("PConstants", new TestMethod()
            {
                public void run(TestCase tc) throws Throwable
                {
                    ((PConstants) tc).runTest();
                }
            }));

            aSuite.addTest(new PfromSerialFormat("PfromSerialFormat", new TestMethod()
            {
                public void run(TestCase tc) throws Throwable
                {
                    ((PfromSerialFormat) tc).runTest();
                }
            }));

            aSuite.addTest(new PlistPIMlists("PlistPIMlists", new TestMethod()
            {
                public void run(TestCase tc) throws Throwable
                {
                    ((PlistPIMlists) tc).runTest();
                }
            }));

            aSuite.addTest(new PopenPIMlist("PopenPIMlist", new TestMethod()
            {
                public void run(TestCase tc) throws Throwable
                {
                    ((PopenPIMlist) tc).runTest();
                }
            }));

            aSuite.addTest(new PopenPIMlist2("PopenPIMlist2", new TestMethod()
            {
                public void run(TestCase tc) throws Throwable
                {
                    ((PopenPIMlist2) tc).runTest();
                }
            }));

            aSuite.addTest(new Pserialization("Pserialization", new TestMethod()
            {
                public void run(TestCase tc) throws Throwable
                {
                    ((Pserialization) tc).runTest();
                }
            }));

            aSuite.addTest(new PsupportedSerialFormats("PsupportedSerialFormats", new TestMethod()
            {
                public void run(TestCase tc) throws Throwable
                {
                    ((PsupportedSerialFormats) tc).runTest();
                }
            }));

            aSuite.addTest(new PtoSerialFormat("PtoSerialFormat", new TestMethod()
            {
                public void run(TestCase tc) throws Throwable
                {
                    ((PtoSerialFormat) tc).runTest();
                }
            }));

            aSuite.addTest(new PEconstants("PEconstants", new TestMethod()
            {
                public void run(TestCase tc) throws Throwable
                {
                    ((PEconstants) tc).runTest();
                }
            }));

            aSuite.addTest(new PEgetReason("PEgetReason", new TestMethod()
            {
                public void run(TestCase tc) throws Throwable
                {
                    ((PEgetReason) tc).runTest();
                }
            }));

            aSuite.addTest(new PIaddBinary("PIaddBinary", new TestMethod()
            {
                public void run(TestCase tc) throws Throwable
                {
                    ((PIaddBinary) tc).runTest();
                }
            }));

            aSuite.addTest(new PIaddBinaryEx("PIaddBinaryEx", new TestMethod()
            {
                public void run(TestCase tc) throws Throwable
                {
                    ((PIaddBinaryEx) tc).runTest();
                }
            }));

            aSuite.addTest(new PIaddBoolean("PIaddBoolean", new TestMethod()
            {
                public void run(TestCase tc) throws Throwable
                {
                    ((PIaddBoolean) tc).runTest();
                }
            }));

            aSuite.addTest(new PIaddDate("PIaddDate", new TestMethod()
            {
                public void run(TestCase tc) throws Throwable
                {
                    ((PIaddDate) tc).runTest();
                }
            }));

            aSuite.addTest(new PIaddDateEx("PIaddDateEx", new TestMethod()
            {
                public void run(TestCase tc) throws Throwable
                {
                    ((PIaddDateEx) tc).runTest();
                }
            }));

            aSuite.addTest(new PIaddInt("PIaddInt", new TestMethod()
            {
                public void run(TestCase tc) throws Throwable
                {
                    ((PIaddInt) tc).runTest();
                }
            }));

            aSuite.addTest(new PIaddString("PIaddString", new TestMethod()
            {
                public void run(TestCase tc) throws Throwable
                {
                    ((PIaddString) tc).runTest();
                }
            }));

            aSuite.addTest(new PIaddStringArray("PIaddStringArray", new TestMethod()
            {
                public void run(TestCase tc) throws Throwable
                {
                    ((PIaddStringArray) tc).runTest();
                }
            }));

            aSuite.addTest(new PIaddStringArrayEx("PIaddStringArrayEx", new TestMethod()
            {
                public void run(TestCase tc) throws Throwable
                {
                    ((PIaddStringArrayEx) tc).runTest();
                }
            }));

            aSuite.addTest(new PIcategories("PIcategories", new TestMethod()
            {
                public void run(TestCase tc) throws Throwable
                {
                    ((PIcategories) tc).runTest();
                }
            }));

            aSuite.addTest(new PIcommit("PIcommit", new TestMethod()
            {
                public void run(TestCase tc) throws Throwable
                {
                    ((PIcommit) tc).runTest();
                }
            }));

            aSuite.addTest(new PIcommit2("PIcommit2", new TestMethod()
            {
                public void run(TestCase tc) throws Throwable
                {
                    ((PIcommit2) tc).runTest();
                }
            }));

            aSuite.addTest(new PIConstants("PIConstants", new TestMethod()
            {
                public void run(TestCase tc) throws Throwable
                {
                    ((PIConstants) tc).runTest();
                }
            }));

            aSuite.addTest(new PIgetAttributes("PIgetAttributes", new TestMethod()
            {
                public void run(TestCase tc) throws Throwable
                {
                    ((PIgetAttributes) tc).runTest();
                }
            }));

            aSuite.addTest(new PIgetFields("PIgetFields", new TestMethod()
            {
                public void run(TestCase tc) throws Throwable
                {
                    ((PIgetFields) tc).runTest();
                }
            }));

            aSuite.addTest(new PIgetPIMList("PIgetPIMList", new TestMethod()
            {
                public void run(TestCase tc) throws Throwable
                {
                    ((PIgetPIMList) tc).runTest();
                }
            }));

            aSuite.addTest(new PIremoveValue("PIremoveValue", new TestMethod()
            {
                public void run(TestCase tc) throws Throwable
                {
                    ((PIremoveValue) tc).runTest();
                }
            }));

            aSuite.addTest(new PIremoveValueEx("PIremoveValueEx", new TestMethod()
            {
                public void run(TestCase tc) throws Throwable
                {
                    ((PIremoveValueEx) tc).runTest();
                }
            }));

            aSuite.addTest(new PIsetBinary("PIsetBinary", new TestMethod()
            {
                public void run(TestCase tc) throws Throwable
                {
                    ((PIsetBinary) tc).runTest();
                }
            }));

            aSuite.addTest(new PIsetBinaryEx("PIsetBinaryEx", new TestMethod()
            {
                public void run(TestCase tc) throws Throwable
                {
                    ((PIsetBinaryEx) tc).runTest();
                }
            }));

            aSuite.addTest(new PIsetBoolean("PIsetBoolean", new TestMethod()
            {
                public void run(TestCase tc) throws Throwable
                {
                    ((PIsetBoolean) tc).runTest();
                }
            }));

            aSuite.addTest(new PIsetDate("PIsetDate", new TestMethod()
            {
                public void run(TestCase tc) throws Throwable
                {
                    ((PIsetDate) tc).runTest();
                }
            }));

            aSuite.addTest(new PIsetDateEx("PIsetDateEx", new TestMethod()
            {
                public void run(TestCase tc) throws Throwable
                {
                    ((PIsetDateEx) tc).runTest();
                }
            }));

            aSuite.addTest(new PIsetInt("PIsetInt", new TestMethod()
            {
                public void run(TestCase tc) throws Throwable
                {
                    ((PIsetInt) tc).runTest();
                }
            }));

            aSuite.addTest(new PIsetString("PIsetString", new TestMethod()
            {
                public void run(TestCase tc) throws Throwable
                {
                    ((PIsetString) tc).runTest();
                }
            }));

            aSuite.addTest(new PIsetStringArray("PIsetStringArray", new TestMethod()
            {
                public void run(TestCase tc) throws Throwable
                {
                    ((PIsetStringArray) tc).runTest();
                }
            }));

            aSuite.addTest(new PIsetStringArrayEx("PIsetStringArrayEx", new TestMethod()
            {
                public void run(TestCase tc) throws Throwable
                {
                    ((PIsetStringArrayEx) tc).runTest();
                }
            }));

            aSuite.addTest(new PLaddCategory("PLaddCategory", new TestMethod()
            {
                public void run(TestCase tc) throws Throwable
                {
                    ((PLaddCategory) tc).runTest();
                }
            }));

            aSuite.addTest(new PLattributes("PLattributes", new TestMethod()
            {
                public void run(TestCase tc) throws Throwable
                {
                    ((PLattributes) tc).runTest();
                }
            }));

            aSuite.addTest(new PLclose("PLclose", new TestMethod()
            {
                public void run(TestCase tc) throws Throwable
                {
                    ((PLclose) tc).runTest();
                }
            }));

            aSuite.addTest(new PLConstants("PLConstants", new TestMethod()
            {
                public void run(TestCase tc) throws Throwable
                {
                    ((PLConstants) tc).runTest();
                }
            }));

//          aSuite.addTest(new PLdeleteCategory("PLdeleteCategory", new TestMethod()
//          {public void run(TestCase tc) throws Throwable {((PLdeleteCategory) tc).runTest();}}));

            aSuite.addTest(new PLfields("PLfields", new TestMethod()
            {
                public void run(TestCase tc) throws Throwable
                {
                    ((PLfields) tc).runTest();
                }
            }));

            aSuite.addTest(new PLgetName("PLgetName", new TestMethod()
            {
                public void run(TestCase tc) throws Throwable
                {
                    ((PLgetName) tc).runTest();
                }
            }));

            aSuite.addTest(new PLisCategory("PLisCategory", new TestMethod()
            {
                public void run(TestCase tc) throws Throwable
                {
                    ((PLisCategory) tc).runTest();
                }
            }));

            aSuite.addTest(new PLitems("PLitems", new TestMethod()
            {
                public void run(TestCase tc) throws Throwable
                {
                    ((PLitems) tc).runTest();
                }
            }));

            aSuite.addTest(new PLitems2("PLitems2", new TestMethod()
            {
                public void run(TestCase tc) throws Throwable
                {
                    ((PLitems2) tc).runTest();
                }
            }));

            aSuite.addTest(new PLitems2Ex("PLitems2Ex", new TestMethod()
            {
                public void run(TestCase tc) throws Throwable
                {
                    ((PLitems2Ex) tc).runTest();
                }
            }));

            aSuite.addTest(new PLitemsByCategory("PLitemsByCategory", new TestMethod()
            {
                public void run(TestCase tc) throws Throwable
                {
                    ((PLitemsByCategory) tc).runTest();
                }
            }));

            aSuite.addTest(new PLrenameCategory("PLrenameCategory", new TestMethod()
            {
                public void run(TestCase tc) throws Throwable
                {
                    ((PLrenameCategory) tc).runTest();
                }
            }));

            aSuite.addTest(new PLstringArrays("PLstringArrays", new TestMethod()
            {
                public void run(TestCase tc) throws Throwable
                {
                    ((PLstringArrays) tc).runTest();
                }
            }));

            aSuite.addTest(new RRConstants("RRConstants", new TestMethod()
            {
                public void run(TestCase tc) throws Throwable
                {
                    ((RRConstants) tc).runTest();
                }
            }));

            aSuite.addTest(new RRdates("RRdates", new TestMethod()
            {
                public void run(TestCase tc) throws Throwable
                {
                    ((RRdates) tc).runTest();
                }
            }));

            aSuite.addTest(new RRdates2("RRdates2", new TestMethod()
            {
                public void run(TestCase tc) throws Throwable
                {
                    ((RRdates2) tc).runTest();
                }
            }));

            aSuite.addTest(new RRdates3("RRdates3", new TestMethod()
            {
                public void run(TestCase tc) throws Throwable
                {
                    ((RRdates3) tc).runTest();
                }
            }));

            aSuite.addTest(new RRdates4("RRdates4", new TestMethod()
            {
                public void run(TestCase tc) throws Throwable
                {
                    ((RRdates4) tc).runTest();
                }
            }));

            aSuite.addTest(new RRdatesExcept("RRdatesExcept", new TestMethod()
            {
                public void run(TestCase tc) throws Throwable
                {
                    ((RRdatesExcept) tc).runTest();
                }
            }));

            aSuite.addTest(new RRequals("RRequals", new TestMethod()
            {
                public void run(TestCase tc) throws Throwable
                {
                    ((RRequals) tc).runTest();
                }
            }));

            aSuite.addTest(new RRequals2("RRequals2", new TestMethod()
            {
                public void run(TestCase tc) throws Throwable
                {
                    ((RRequals2) tc).runTest();
                }
            }));

            aSuite.addTest(new RRexceptDates("RRexceptDates", new TestMethod()
            {
                public void run(TestCase tc) throws Throwable
                {
                    ((RRexceptDates) tc).runTest();
                }
            }));

            aSuite.addTest(new RRgetDate("RRgetDate", new TestMethod()
            {
                public void run(TestCase tc) throws Throwable
                {
                    ((RRgetDate) tc).runTest();
                }
            }));

            aSuite.addTest(new RRgetFields("RRgetFields", new TestMethod()
            {
                public void run(TestCase tc) throws Throwable
                {
                    ((RRgetFields) tc).runTest();
                }
            }));

            aSuite.addTest(new RRsetDate("RRsetDate", new TestMethod()
            {
                public void run(TestCase tc) throws Throwable
                {
                    ((RRsetDate) tc).runTest();
                }
            }));

            aSuite.addTest(new SystemPropertiesTest("SystemPropertiesTest", new TestMethod()
            {
                public void run(TestCase tc) throws Throwable
                {
                    ((SystemPropertiesTest) tc).runTest();
                }
            }));

            aSuite.addTest(new TConstants("TConstants", new TestMethod()
            {
                public void run(TestCase tc) throws Throwable
                {
                    ((TConstants) tc).runTest();
                }
            }));

            aSuite.addTest(new TLcreateToDo("TLcreateToDo", new TestMethod()
            {
                public void run(TestCase tc) throws Throwable
                {
                    ((TLcreateToDo) tc).runTest();
                }
            }));

            aSuite.addTest(new TLimportToDo("TLimportToDo", new TestMethod()
            {
                public void run(TestCase tc) throws Throwable
                {
                    ((TLimportToDo) tc).runTest();
                }
            }));

            aSuite.addTest(new TLitems("TLitems", new TestMethod()
            {
                public void run(TestCase tc) throws Throwable
                {
                    ((TLitems) tc).runTest();
                }
            }));

            aSuite.addTest(new TLitemsEx("TLitemsEx", new TestMethod()
            {
                public void run(TestCase tc) throws Throwable
                {
                    ((TLitemsEx) tc).runTest();
                }
            }));

            aSuite.addTest(new TLmultipleLists("TLmultipleLists", new TestMethod()
            {
                public void run(TestCase tc) throws Throwable
                {
                    ((TLmultipleLists) tc).runTest();
                }
            }));

            aSuite.addTest(new TLremoveToDo("TLremoveToDo", new TestMethod()
            {
                public void run(TestCase tc) throws Throwable
                {
                    ((TLremoveToDo) tc).runTest();
                }
            }));

            aSuite.addTest(new UFEgetField("UFEgetField", new TestMethod()
            {
                public void run(TestCase tc) throws Throwable
                {
                    ((UFEgetField) tc).runTest();
                }
            }));

            aSuite.addTest(new PfromSerialFormatExTest("PfromSerialFormatExTest", new TestMethod()
            {
                public void run(TestCase tc) throws Throwable
                {
                    ((PfromSerialFormatExTest) tc).runTest();
                }
            }));

            aSuite.addTest(new PfromSerialFormatTest("PfromSerialFormatTest", new TestMethod()
            {
                public void run(TestCase tc) throws Throwable
                {
                    ((PfromSerialFormatTest) tc).runTest();
                }
            }));

            aSuite.addTest(new PfromSerialFormatInvalid("PfromSerialFormatInvalid", new TestMethod()
            {
                public void run(TestCase tc) throws Throwable
                {
                    ((PfromSerialFormatInvalid) tc).runTest();
                }
            }));

            aSuite.addTest(new PtoSerialFormatTest("PtoSerialFormatTest", new TestMethod()
            {
                public void run(TestCase tc) throws Throwable
                {
                    ((PtoSerialFormatTest) tc).runTest();
                }
            }));

            aSuite.addTest(new PtoSerialFormatEx("PtoSerialFormatEx", new TestMethod()
            {
                public void run(TestCase tc) throws Throwable
                {
                    ((PtoSerialFormatEx) tc).runTest();
                }
            }));

            aSuite.addTest(new RRintFieldCount("RRintFieldCount", new TestMethod()
            {
                public void run(TestCase tc) throws Throwable
                {
                    ((RRintFieldCount) tc).runTest();
                }
            }));

            aSuite.addTest(new RRintFieldDayInMonth("RRintFieldDayInMonth", new TestMethod()
            {
                public void run(TestCase tc) throws Throwable
                {
                    ((RRintFieldDayInMonth) tc).runTest();
                }
            }));

            aSuite.addTest(new RRintFieldDayInWeek("RRintFieldDayInWeek", new TestMethod()
            {
                public void run(TestCase tc) throws Throwable
                {
                    ((RRintFieldDayInWeek) tc).runTest();
                }
            }));

            aSuite.addTest(new RRintFieldDayInYear("RRintFieldDayInYear", new TestMethod()
            {
                public void run(TestCase tc) throws Throwable
                {
                    ((RRintFieldDayInYear) tc).runTest();
                }
            }));

            aSuite.addTest(new RRintFieldFrequency("RRintFieldFrequency", new TestMethod()
            {
                public void run(TestCase tc) throws Throwable
                {
                    ((RRintFieldFrequency) tc).runTest();
                }
            }));

            aSuite.addTest(new RRintFieldInterval("RRintFieldInterval", new TestMethod()
            {
                public void run(TestCase tc) throws Throwable
                {
                    ((RRintFieldInterval) tc).runTest();
                }
            }));

            aSuite.addTest(new RRintFieldMonthInYear("RRintFieldMonthInYear", new TestMethod()
            {
                public void run(TestCase tc) throws Throwable
                {
                    ((RRintFieldMonthInYear) tc).runTest();
                }
            }));

            aSuite.addTest(new RRintFieldWeekInMonth("RRintFieldWeekInMonth", new TestMethod()
            {
                public void run(TestCase tc) throws Throwable
                {
                    ((RRintFieldWeekInMonth) tc).runTest();
                }
            }));

            // Variation tests start here. Each release may have different features so tests must
            // be variated (e.g S60 3.1, S60 3.2, S60 ... )

            aSuite.addTest(new CLcategoryTests("CLcategoryTests", new TestMethod()
            {
                public void run(TestCase tc) throws Throwable
                {
                    ((CLcategoryTests) tc).runTest();
                }
            }));

            aSuite.addTest(new CLfieldsOrder("CLfieldsOrder", new TestMethod()
            {
                public void run(TestCase tc) throws Throwable
                {
                    ((CLfieldsOrder) tc).runTest();
                }
            }));

            aSuite.addTest(new CLgetAttributes("CLgetAttributes", new TestMethod()
            {
                public void run(TestCase tc) throws Throwable
                {
                    ((CLgetAttributes) tc).runTest();
                }
            }));

            aSuite.addTest(new CLmaxValues("CLmaxValues", new TestMethod()
            {
                public void run(TestCase tc) throws Throwable
                {
                    ((CLmaxValues) tc).runTest();
                }
            }));

            aSuite.addTest(new CLserialFormatTests("CLserialFormatTests", new TestMethod()
            {
                public void run(TestCase tc) throws Throwable
                {
                    ((CLserialFormatTests) tc).runTest();
                }
            }));

            aSuite.addTest(new CLstringMaxLen("CLstringMaxLen", new TestMethod()
            {
                public void run(TestCase tc) throws Throwable
                {
                    ((CLstringMaxLen) tc).runTest();
                }
            }));

            aSuite.addTest(new CLsupportedArrayElements("CLsupportedArrayElements", new TestMethod()
            {
                public void run(TestCase tc) throws Throwable
                {
                    ((CLsupportedArrayElements) tc).runTest();
                }
            }));

            aSuite.addTest(new CLsupportedAttributes("CLsupportedAttributes", new TestMethod()
            {
                public void run(TestCase tc) throws Throwable
                {
                    ((CLsupportedAttributes) tc).runTest();
                }
            }));

            aSuite.addTest(new CLsupportedFields("CLsupportedFields", new TestMethod()
            {
                public void run(TestCase tc) throws Throwable
                {
                    ((CLsupportedFields) tc).runTest();
                }
            }));

            aSuite.addTest(new ELfieldsOrder("ELfieldsOrder", new TestMethod()
            {
                public void run(TestCase tc) throws Throwable
                {
                    ((ELfieldsOrder) tc).runTest();
                }
            }));

            aSuite.addTest(new ELserialFormatTests("ELserialFormatTests", new TestMethod()
            {
                public void run(TestCase tc) throws Throwable
                {
                    ((ELserialFormatTests) tc).runTest();
                }
            }));

            aSuite.addTest(new ELstringMaxLen("ELstringMaxLen", new TestMethod()
            {
                public void run(TestCase tc) throws Throwable
                {
                    ((ELstringMaxLen) tc).runTest();
                }
            }));

            aSuite.addTest(new ELsupportedFields("ELsupportedFields", new TestMethod()
            {
                public void run(TestCase tc) throws Throwable
                {
                    ((ELsupportedFields) tc).runTest();
                }
            }));

            aSuite.addTest(new ELsupportedRRFields2("ELsupportedRRFields2", new TestMethod()
            {
                public void run(TestCase tc) throws Throwable
                {
                    ((ELsupportedRRFields2) tc).runTest();
                }
            }));

            aSuite.addTest(new TLfieldsOrder("TLfieldsOrder", new TestMethod()
            {
                public void run(TestCase tc) throws Throwable
                {
                    ((TLfieldsOrder) tc).runTest();
                }
            }));

            aSuite.addTest(new TLpriorityMappings("TLpriorityMappings", new TestMethod()
            {
                public void run(TestCase tc) throws Throwable
                {
                    ((TLpriorityMappings) tc).runTest();
                }
            }));

            aSuite.addTest(new TLserialFormatTests("TLserialFormatTests", new TestMethod()
            {
                public void run(TestCase tc) throws Throwable
                {
                    ((TLserialFormatTests) tc).runTest();
                }
            }));

            aSuite.addTest(new TLstringMaxLen("TLstringMaxLen", new TestMethod()
            {
                public void run(TestCase tc) throws Throwable
                {
                    ((TLstringMaxLen) tc).runTest();
                }
            }));

            aSuite.addTest(new TLsupportedFields("TLsupportedFields", new TestMethod()
            {
                public void run(TestCase tc) throws Throwable
                {
                    ((TLsupportedFields) tc).runTest();
                }
            }));

            aSuite.addTest(new Ecommit("Ecommit", new TestMethod()
            {
                public void run(TestCase tc) throws Throwable
                {
                    ((Ecommit) tc).runTest();
                }
            }));

            aSuite.addTest(new Tcommit("Tcommit", new TestMethod()
            {
                public void run(TestCase tc) throws Throwable
                {
                    ((Tcommit) tc).runTest();
                }
            }));

            aSuite.addTest(new CcommitPartial("CcommitPartial", new TestMethod()
            {
                public void run(TestCase tc) throws Throwable
                {
                    ((CcommitPartial) tc).runTest();
                }
            }));

            aSuite.addTest(new CcommitUpdate("CcommitUpdate", new TestMethod()
            {
                public void run(TestCase tc) throws Throwable
                {
                    ((CcommitUpdate) tc).runTest();
                }
            }));

            aSuite.addTest(new PLdeleteCategory("PLdeleteCategory", new TestMethod()
            {
                public void run(TestCase tc) throws Throwable
                {
                    ((PLdeleteCategory) tc).runTest();
                }
            }));
        }
        catch (Throwable t)
        {
            System.out.println("Exception in test suite framework : "+t.toString());
            t.printStackTrace();
            //spede.addToResultsList("<debug>com.digia.javatest.agent.MIDPTest.suite: "+t.toString()+" thrown</debug>");
        }
        return aSuite;
    }


    public void passTest()
    {
        assertTrue("",true);
    }

    public void failTest(String aReason)
    {
        assertTrue(aReason, false);
    }

    public void debug(String aDebug)
    {
        //spede.addToResultsList("<debug>"+aDebug+"</debug>");
    }

    public void log(String aLog)
    {
        //spede.addToResultsList("<debug>"+aLog+"</debug>");
    }

    public void startTimer(String aName)
    {
        //spede.addToResultsList("<debug>Timer start: "+aName+" "+System.currentTimeMillis()+"</debug>");
    }

    public void stopTimer(String aName)
    {
        //spede.addToResultsList("<debug>Timer stop: "+aName+" "+System.currentTimeMillis()+"</debug>");
    }

    public void usedMemory()
    {
        //spede.addToResultsList("<debug>"+(Runtime.getRuntime().freeMemory()-iFreeMemoryAtStart)+"</debug>");
    }

    public void assertString(boolean aCondition, String aMessage) throws TestEndThrowable
    {
        if (!aCondition)
        {
            throw new TestEndThrowable(aMessage);
        }
    }
}


