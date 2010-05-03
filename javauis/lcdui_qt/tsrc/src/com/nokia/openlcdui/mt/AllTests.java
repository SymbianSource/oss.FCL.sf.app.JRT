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
package com.nokia.openlcdui.mt;

import java.util.Vector;

import com.nokia.mj.impl.installer.utils.InstallerMain;

import junit.framework.Test;
import junit.framework.TestCase;
import junit.framework.TestSuite;
import junit.framework.TestResult;
import junit.framework.TestFailure;
import junit.textui.ResultPrinter;



/**
 * A simple main application for conveniently executing all of the eSWT tests
 * with text JUnit UI runner.
 */
public class AllTests extends TestSuite implements InstallerMain {

        static boolean runningMain = false;
        static boolean resultPrinter = true;
        static Vector disabledTests = SWTTestCase.getDisabledTests();

        boolean started;
        
        boolean enableUIRobotTests = false;
        
        public void installerMain(String[] args) {
            if (!started) {
                started = true;
                
                Thread uiThread = new Thread(new Runnable() {
                    public void run() {
                        runMainUI();
                    }});
                    
                uiThread.start();
                try {
                    uiThread.join();
                } catch (InterruptedException e) {
                    e.printStackTrace();
                }
            }
        }
        
        public static void runMainUI() {
            resultPrinter = false;
            java.io.PrintStream stream = null;
            final com.nokia.mj.impl.utils.OmjTestRunner runner;
            try {
                Test suite = suite();
                runner = new com.nokia.mj.impl.utils.OmjTestRunner(suite);
                
                class eSWTPrinter extends ResultPrinter {
                    public eSWTPrinter() {
                        super(System.out);
                    }
                    public void startTest(Test test) {
                        System.out.println(test.toString());
                    }
                    //public void addFailure(Test t, junit.framework.AssertionFailedError e) {}
                    //public void addError(Test t, Throwable e) {}
                    public void endTest(Test test) {
                        runner.endTest(test);
                        super.endTest(test);
                    }
                    public void printDefect(TestFailure testfailure, int i)
                    {
                        printDefectHeader(testfailure, i);
                        super.printDefect(testfailure, i);
                    }    
                    //protected void printErrors(TestResult result) {}
                };
                runner.setPrinter(new eSWTPrinter());
                TestResult result = runner.doRun(suite);
                
                /* Test results will be written to: 
                 *  - fileconn.dir.photos\results (c:\data\images\results) on S60,
                 *  - JAVA_BIN_ROOT/results on Linux
                 */
                runner.writeResultFile(result);
                
            } catch (Exception e) {
                e.printStackTrace(System.out);
            
            } finally {
                if (stream != null) stream.close();
            }
        }

        public static void main(String[] args) {
            runningMain = true;
                        
            Thread uiThread = new Thread(new Runnable() {
                public void run() {
                    runMainUI();
                }});
                
            uiThread.start();
            try {
                uiThread.join();
            } catch (InterruptedException e) {
                e.printStackTrace();
            }
            
            // Exit forcefully when running outside of MIDP environment, this causes
            // also OpenLCDUI thread to close.
            System.exit(0);
        }
        
        public static junit.framework.Test suite() {
            return new AllTests();
        }
        
        public void init() {
            
        }
        
        public AllTests() {
            init();

            addTest(com.nokia.openlcdui.mt.alert.AlertTest.suite());
            addTest(com.nokia.openlcdui.mt.canvas.CanvasTest.suite());
            addTest(com.nokia.openlcdui.mt.choicegroup.ChoiceGroupTest.suite());
            addTest(com.nokia.openlcdui.mt.command.CommandTest.suite());
            addTest(com.nokia.openlcdui.mt.command.ItemCommandTest.suite());
            addTest(com.nokia.openlcdui.mt.datefield.DateFieldTest.suite());
            addTest(com.nokia.openlcdui.mt.display.DisplayTest.suite());
            addTest(com.nokia.openlcdui.mt.displayable.DisplayableTest.suite());
            addTest(com.nokia.openlcdui.mt.font.FontTest.suite());
            addTest(com.nokia.openlcdui.mt.form.FormTest.suite());
            addTest(com.nokia.openlcdui.mt.fullcanvas.FullCanvasTest.suite());
            addTest(com.nokia.openlcdui.mt.game.CollisionDetectionTest.suite());
            addTest(com.nokia.openlcdui.mt.game.GameCanvasTest.suite());
            addTest(com.nokia.openlcdui.mt.game.LayerManagerTest.suite());
            addTest(com.nokia.openlcdui.mt.game.LayerTest.suite());
            addTest(com.nokia.openlcdui.mt.game.SpriteTest.suite());
            addTest(com.nokia.openlcdui.mt.game.TiledLayerTest.suite());
            addTest(com.nokia.openlcdui.mt.gauge.GaugeTest.suite());
            addTest(com.nokia.openlcdui.mt.graphics.DirectGraphicsTest.suite());
            addTest(com.nokia.openlcdui.mt.graphics.GraphicsTest.suite());
            addTest(com.nokia.openlcdui.mt.image.ImageTest.suite());
            addTest(com.nokia.openlcdui.mt.imageitem.ImageItemTest.suite());
            addTest(com.nokia.openlcdui.mt.item.ItemTest.suite());
            addTest(com.nokia.openlcdui.mt.list.ListTest.suite());
            addTest(com.nokia.openlcdui.mt.spacer.SpacerTest.suite());
            addTest(com.nokia.openlcdui.mt.stringitem.StringItemTest.suite());
            addTest(com.nokia.openlcdui.mt.textbox.TextBoxTest.suite());
            addTest(com.nokia.openlcdui.mt.textfield.TextFieldTest.suite());
            addTest(com.nokia.openlcdui.mt.ticker.TickerTest.suite());
            addTest(com.nokia.openlcdui.mt.devicecontrol.DeviceControlTest.suite());                        
            if (enableUIRobotTests) {
            	addTest(com.nokia.openlcdui.mt_uirobot.alert.AlertUITest.suite());
            	addTest(com.nokia.openlcdui.mt_uirobot.choicegroup.ChoiceGroupCommandTest.suite());
            	addTest(com.nokia.openlcdui.mt_uirobot.choicegroup.ChoiceGroupPopupTest.suite());
            	addTest(com.nokia.openlcdui.mt_uirobot.command.DynamicUpdateTest.suite());
            	addTest(com.nokia.openlcdui.mt_uirobot.command.ItemCommandUIRobotTest.suite());
            	addTest(com.nokia.openlcdui.mt_uirobot.customitem.CustomItemTest.suite());
            	addTest(com.nokia.openlcdui.mt_uirobot.datefield.DateFieldUIRobotTest.suite());
            	addTest(com.nokia.openlcdui.mt_uirobot.display.FlashAndVibrateTest.suite());
            	addTest(com.nokia.openlcdui.mt_uirobot.display.SetCurrentItemTest.suite());
            	addTest(com.nokia.openlcdui.mt_uirobot.displayable.AddCommandTest.suite());
            	addTest(com.nokia.openlcdui.mt_uirobot.displayable.SendBackgroundTest.suite());
            	addTest(com.nokia.openlcdui.mt_uirobot.displayable.SizeChangedTest.suite());
            	addTest(com.nokia.openlcdui.mt_uirobot.form.FormItemTest.suite());
            	addTest(com.nokia.openlcdui.mt_uirobot.gauge.GaugeCommandsTest.suite());
            	addTest(com.nokia.openlcdui.mt_uirobot.imageitem.ImageItemHyperLinkTest.suite());
            	addTest(com.nokia.openlcdui.mt_uirobot.item.ItemUITest.suite());
            	addTest(com.nokia.openlcdui.mt_uirobot.list.ListUITest.suite());
            	addTest(com.nokia.openlcdui.mt_uirobot.spacer.SpacerTest.suite());
            	addTest(com.nokia.openlcdui.mt_uirobot.stringitem.HyperLinkTest.suite());
            	addTest(com.nokia.openlcdui.mt_uirobot.stringitem.StringItemUIRobotTest.suite());
            	addTest(com.nokia.openlcdui.mt_uirobot.textbox.TestUITextBox.suite());
            	addTest(com.nokia.openlcdui.mt_uirobot.textbox.TextBoxInputModeUITest.suite());
            	addTest(com.nokia.openlcdui.mt_uirobot.textfield.FormChangeFocusCheckTest.suite());
            	addTest(com.nokia.openlcdui.mt_uirobot.textfield.TextFieldCommandTest.suite());
            	addTest(com.nokia.openlcdui.mt_uirobot.textfield.TextFieldSetInitialInputModeTest.suite());
            }
            

            // If tests are executed by some runner as a suite then it won't clean up the UI
            // before it let's the thread to terminate. This is executed as the last test 
            // to clean up in the UI thread before it exits. 
            /*if(!runningMain) {
                addTest(new TestCase() {
                    protected void runTest() throws Throwable {
                        Display display = Display.getCurrent();
                        if(display != null) display.dispose();
                    }
                    public String getName() {
                        return "CleanUpQtBeforeGUIThreadExits";
                    }
                });
            }*/
            // If some tests were disabled then generate a warning about them. 
            if(disabledTests != null) {
                addTest(new TestCase() {
                    protected void runTest() throws Throwable {
                        if(!disabledTests.isEmpty()) {
                            String list = new String();
                            list += "Warning: There are " + disabledTests.size() + " disabled tests that were marked as passing: \r\n";
                            for(int i = 0; i < disabledTests.size(); ++i) {
                                list += disabledTests.elementAt(i);
                                list += "\r\n";
                            }
                            throw new Exception(list);
                        }
                    }
                    public String getName() {
                        return "DisabledTestsCheck";
                    }
                });
            }
        }    
}

