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
* Description: Used for logging and tracing in Java code
*
*
*/


package com.nokia.mj.impl.utils;

import com.nokia.mj.impl.rt.support.Jvm;
import java.io.ByteArrayOutputStream;
import java.io.PrintStream;

/**
 * The class representing Java interface to Logger.
 *
 * The production code includes the following logging levels:
 * * EError
 * * EWarning
 * * EInfoPrd
 * These logging messages must include messages understable for
 * Java application developer as messages will end to the IDE console.
 * You can enable more detailed logging of any component by
 * by changing the value in Logger.Activated table. Remember to
 * create c:\logs\java\full -directory to get detailed logs.
 */
public final class Logger
{

    // ------------------------------------------------------------
    // List of components. Keep the list in sync with native logger
    // javacommons/utils/inc/logger.h.
    // ------------------------------------------------------------

    public static final int EJavaCaptain        = 1;
    public static final int EJavaRuntime        = 2;
    public static final int EJavaComms          = 3;
    public static final int EAppRecognizer      = 4;
    public static final int EMidpRms            = 5;
    public static final int EUtils              = 6;
    public static final int EJavaUI             = 7;
    public static final int EJavaPush           = 8;
    public static final int EJVM                = 9;
    public static final int EGcf                = 10;
    public static final int EJavaStorage        = 11;
    public static final int EJavaInstaller      = 12;
    public static final int EJavaFile           = 13;
    public static final int ESOCKET             = 14;
    public static final int EWMA                = 15;
    public static final int EPim                = 16;
    public static final int EJavaPreinstaller   = 17;
    public static final int ETckRunner          = 18;
    public static final int EJavaBluetooth      = 19;
    public static final int EJavaSecurity       = 20;
    public static final int EGcfProtocols       = 21;
    public static final int EJavaMMAPI          = 22;
    public static final int ESATSA              = 23;
    public static final int EJavaLocation       = 24;
    public static final int EBackup             = 25;
    public static final int EJavaSystemAMS      = 26;
    public static final int EJavaIapInfo        = 27;
    public static final int EJavaContactless    = 28;
    public static final int EJavaConverters     = 29;
    public static final int EMidp2CenrepWrapper = 30;
    public static final int EJavawebservices    = 31;
    public static final int EJavaGlobalInd      = 32;
    public static final int ESensor             = 33;
    public static final int EDebugApi           = 34;
    public static final int EJavaAppMngrPlugin  = 35;
    public static final int EJavaBroadcast      = 36;
    public static final int EJavaAMMS	          = 37;
    public static final int EJavaSettings       = 38;
	public static final int EJavaCentrep       = 40;
    // add name of new components here


    // -----------------------------------------------------
    //    list of component's logger enablers
    // -----------------------------------------------------
    // Preprocessor is not used in Java code build system so in order to avoid
    // overhead on Logger method calls when Logger is disabled, the following
    // code shall be used when the logging level is EEntry or EInfoHeavyLoad:
    //
    //    if (Logger.Activated[Logger.EJavaFile])
    //        Logger.HLOG(Logger.EJavaFile, "Created new file: " + fileName");
    //
    // Logger.Activated is a static final boolean array so java compiler will
    // ignore the whole statment if the value of Logger.Activated[*component*]
    // is false. When this file is stored to version control, all components
    // shall have value "false".
    public static final boolean Activated[] =
    {
        false,  //undefined
        false,  //EJavaCaptain       = 1;
        false,  //EJavaRuntime       = 2;
        false,  //EJavaComms         = 3;
        false,  //EAppRecognizer     = 4;
        false,  //EMidpRms           = 5;
        false,  //EUtils             = 6;
        true,  //EJavaUI            = 7;
        false,  //EJavaPush          = 8;
        false,  //EJVM               = 9;
        false,  //EGcf               = 10;
        false,  //EJavaStorage       = 11;
        false,  //EJavaInstaller     = 12;
        false,  //EJavaFile,         = 13;
        true,  //ESOCKET,           = 14;
        false,  //EWMA,              = 15;
        false,  //EPim,              = 16;
        false,  //EJavaPreinstaller, = 17;
        false,  //ETckRunner         = 18;
        false,  //EJavaBluetooth     = 19;
        false,  //EJavaSecurity      = 20;
        false,  //EGcfProtocols      = 21;
        false,  //EJavaMMAPI         = 22;
        true,  //ESATSA             = 23;
        true,  //EJavaLocation      = 24;
        true,  //EBackup            = 25;
        false,  //EJavaSystemAMS     = 26;
        false,  //EJavaIapInfo       = 27;
        false,  //EJavaContactless   = 28;
        false,  //EJavaConverters    = 29;
        false,  //Midp2CenrepWrapper = 30;
        false,  //EJavawebservices   = 31;
        false,  //EJavaGlobalInd     = 32;
        false,  //ESensor            = 33;
        false,  //EDebugApi          = 34;
        false,  //EJavaAppMngrPlugin = 35;
        false,  //EJavaBroadcast     = 36;
        false,  //EJavaAMMS					 = 37;
        false,  //EJavaSettings 		 = 38;
		false   //EJavaCentrep       = 40;
        // add  new components here
    };


    // ----------------------------------------------------------------
    // Logging levels are documented in javacommons/utils/inc/logger.h.
    // ----------------------------------------------------------------

    public static final int ENoLog             = 0;
    public static final int EError             = 0x1; // Use ELOG(s)
    public static final int EWarning           = 0x2; // Use WLOG(s)
    public static final int EInfoPrd           = 0x4; // Use PLOG(s)
    public static final int EInfo              = 0x8; // Use ILOG(s)
    public static final int EEntry             = 0x10; //Use JELOG(s)
    public static final int EInfoHeavyLoad     = 0x20; // Use HLOG(s)

    //private static ByteArrayOutputStream byte_stream = new ByteArrayOutputStream();
    //private static PrintStream print_stream = new PrintStream(byte_stream);

    static
    {

        try
        {
            Jvm.loadSystemLibrary("javautils");
        }

        catch (Exception e)
        {

            System.out.println(e.toString());
        }
    }

    /**
     * Logging function for error messages
     * @param component a unique id of component
     * @param str emiting information
     */
    public static void ELOG(int component, String str)
    {
        _logging(component, EError, str);
    }

    /**
     * Logging function for errors, prints exception info and a backtrace of the stack
     * @param component a unique id of component
     * @param str emiting information
     * @param thrown exception object
     */
    public static void ELOG(int component, String str, Throwable thrown)
    {

        ByteArrayOutputStream byte_stream = new ByteArrayOutputStream();
        PrintStream print_stream = new PrintStream(byte_stream);

        _loggingException(component, EError, str, thrown, byte_stream, print_stream);

    }

    /**
     * Logging function for warning messages
     * @param component a unique id of component
     * @param str emiting information
     */
    public static void WLOG(int component, String str)
    {
        _logging(component, EWarning, str);
    }

    /**
     * Logging function for warnings, prints exception info and a backtrace of the stack
     * @param component a unique id of component
     * @param str emiting information
     * @param thrown exception object
     */
    public static void WLOG(int component, String str, Throwable thrown)
    {
        ByteArrayOutputStream byte_stream = new ByteArrayOutputStream();
        PrintStream print_stream = new PrintStream(byte_stream);

        _loggingException(component, EWarning, str, thrown, byte_stream, print_stream);
    }

    /**
     * Logging function for information ending to the products
     * @param component a unique id of component
     * @param str emiting information
     */
    public static void PLOG(int component, String str)
    {
        _logging(component, EInfoPrd, str);
    }

    /**
     * Logging function for information ending to the products,
     * prints exception info and a backtrace of the stack
     * @param component a unique id of component
     * @param str emiting information
     * @param thrown exception object
     */
    public static void PLOG(int component, String str, Throwable thrown)
    {
        ByteArrayOutputStream byte_stream = new ByteArrayOutputStream();
        PrintStream print_stream = new PrintStream(byte_stream);
        _loggingException(component, EInfoPrd, str, thrown, byte_stream, print_stream);
    }

    /**
     * Logging function for information ending to the debug releases
     * @param component a unique id of component
     * @param str emiting information
     */
    public static void ILOG(int component, String str)
    {
        _logging(component, EInfo, str);
    }

    /**
     * Logging function for information ending to the debug releases,
     * prints exception info and a backtrace of the stack
     * @param component a unique id of component
     * @param str emiting information
     * @param thrown exception object
     */
    public static void ILOG(int component, String str, Throwable thrown)
    {
        ByteArrayOutputStream byte_stream = new ByteArrayOutputStream();
        PrintStream print_stream = new PrintStream(byte_stream);
        _loggingException(component, EInfoPrd, str, thrown, byte_stream, print_stream);
    }

    /**
     * Used for function entries,
     * e.g. JELOG(Logger.EJavaCaptain, "-> myFunction()"
     * @param component a unique id of component
     * @param str emiting information
     */
    public static void JELOG(int component, String str)
    {
        if (Activated[component])
        {
            _logging(component, EEntry, str);
        }
    }

    /**
     * Used for high load data
     * @param component a unique id of component
     * @param str emiting information
     */
    public static void HLOG(int component, String str)
    {
        if (Activated[component])
        {
            _logging(component, EInfoHeavyLoad, str);
        }
    }

    /**
     * Function for emitting debug traces at development time.
     * This does not do anything in release builds.
     * @param component a unique id of component
     * @param level id level of tracing information (EInfo, EInfoHEavyLoad, EError, EWarning)
     * @param str emiting information
     */
    public static void LOG(int component, int level, String str)
    {
        if (Activated[component])
            _logging(component, level, str);
    }

    /**
     * Function for printing an exception info and a backtrace of the stack to log at development time
     * This does not do anything in release builds
     * @param component a unique id of component
     * @param level id level of tracing information (EInfo, EInfoHEavyLoad, EError, EWarning)
     * @param str emiting information
     * @param thrown exception object
     */
    public static void LOG(int component, int level, String str, Throwable thrown)
    {
        if (Activated[component])
        {
            ByteArrayOutputStream byte_stream = new ByteArrayOutputStream();
            PrintStream print_stream = new PrintStream(byte_stream);

            _loggingException(component, level, str, thrown, byte_stream, print_stream);
        }
    }


    /** Native function prototype. */
    private static native void _logging(int component, int level, String str);

    /** Native function for logging stack trace when exception happend */
    private static native void _loggingException(int component, int level, String str, Throwable thrown,
            ByteArrayOutputStream byte_stream, PrintStream print_stream);

}
