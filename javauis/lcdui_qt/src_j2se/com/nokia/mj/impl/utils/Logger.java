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
* Description:  ?Description
*
*/


package com.nokia.mj.impl.utils;

import java.io.ByteArrayOutputStream;
import java.io.PrintStream;

/**
 * A class representing Java interface to Logger.
 */
public final class Logger
{

    // -----------------------------------------------------
    //    list of components
    // -----------------------------------------------------

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
    // add name of new components here


    // -----------------------------------------------------
    //    list of component's logger enablers
    // -----------------------------------------------------

    public static final boolean Activated[] = {

        false,  //undefined
        false,  //EJavaCaptain       = 1;
        true,   //EJavaRuntime       = 2;
        false,  //EJavaComms         = 3;
        false,  //EAppRecognizer     = 4;
        true,   //EMidpRms           = 5;
        false,  //EUtils             = 6;
        false,  //EJavaUI            = 7;
        true,  //EJavaPush          = 8;
        false,  //EJVM               = 9;
        false,  //EGcf               = 10;
        false,  //EJavaStorage       = 11;
        true ,  //EJavaInstaller     = 12;
        false,  //EJavaFile,         = 13;
        true,   //ESOCKET,           = 14;
        false,  //EWMA,              = 15;
        false,  //EPim,              = 16;
        false,  //EJavaPreinstaller, = 17;
        true ,  //ETckRunner         = 18;
        false,   //EJavaBluetooth    = 19;
        true,   //EJavaSecurity      = 20;
        false,  //EGcfProtocols      = 21;
        false,  //EJavaMMAPI         = 22;
        true,   //ESATSA             = 23;
        true,   //EJavaLocation      = 24;
        true,   //EBackup            = 25;
        false,  // EJavaSystemAMS    = 26;
        true,   //EJavaIapInfo       = 27;
        true,   //EJavaContactless   = 28;
        false,  //EJavaConverters    = 29;
        false,  //Midp2CenrepWrapper = 30;
        false,  //EJavawebservices   = 31
        // add  new components here
    };


    // ---------------------------------------------------------
    //    logging levels
    // ---------------------------------------------------------

    public static final int ENoLog             = 0;
    public static final int EError             = 0x1;
    public static final int EWarning           = 0x2;
    public static final int EInfo              = 0x4;
    public static final int EEntry             = 0x8;  //Used in JELOG(s), don't use this in LOG(s)
    public static final int EInfoHeavyLoad     = 0x10;


    //private static ByteArrayOutputStream byte_stream = new ByteArrayOutputStream();
    //private static PrintStream print_stream = new PrintStream(byte_stream);

    /*static {

        try {
            Jvm.loadSystemLibrary("javautils");
        }

        catch(Exception e) {

            System.out.println(e.toString());
        }
    }*/


    /**
     * Function for emitting debug traces at development time
     * usually do nothing for release builds
     * @param component a unique id of component
     * @param level id level of tracing information (EInfo, EInfoHEavyLoad, EError, EWarning)
     * @param str emiting information
     */
    public static void LOG(int component, int level, String str) {

        if (Activated[component])
            _logging(component, level, str);
    }

    /**
     * Function for printing an exception info and a backtrace of the stack to log at development time
     * usually do nothing for release builds
     * @param component a unique id of component
     * @param level id level of tracing information (EInfo, EInfoHEavyLoad, EError, EWarning)
     * @param str emiting information
     * @param thrown exception object
     */
    public static void LOG(int component, int level, String str, Throwable thrown) {

        if (Activated[component]) {

            ByteArrayOutputStream byte_stream = new ByteArrayOutputStream();
            PrintStream print_stream = new PrintStream(byte_stream);

            _loggingException(component, level, str, thrown, byte_stream, print_stream);
        }
    }

    /**
     * Logging function for error messages
     * @param component a unique id of component
     * @param str emiting information
     */
    public static void ELOG(int component, String str) {

        _logging(component, EError, str);
    }

    /**
     * Logging function for errors, prints exception info and a backtrace of the stack
     * @param component a unique id of component
     * @param str emiting information
     * @param thrown exception object
     */
    public static void ELOG(int component, String str, Throwable thrown) {

        ByteArrayOutputStream byte_stream = new ByteArrayOutputStream();
        PrintStream print_stream = new PrintStream(byte_stream);

        _loggingException(component, EError, str, thrown, byte_stream, print_stream);

    }

    /**
     * Logging function for warning messages
     * @param component a unique id of component
     * @param str emiting information
     */
    public static void WLOG(int component, String str) {
        _logging(component, EWarning, str);
    }

    /**
     * Logging function for warnings, prints exception info and a backtrace of the stack
     * @param component a unique id of component
     * @param str emiting information
     * @param thrown exception object
     */
    public static void WLOG(int component, String str, Throwable thrown) {

        ByteArrayOutputStream byte_stream = new ByteArrayOutputStream();
        PrintStream print_stream = new PrintStream(byte_stream);

        _loggingException(component, EWarning, str, thrown, byte_stream, print_stream);
    }


    private static void _logging(int component, int level, String str) {
        System.out.println(str);
    }

    private static void _loggingException(int component, int level, String str, Throwable thrown,
                                                 ByteArrayOutputStream byte_stream, PrintStream print_stream) {
        System.out.println(str+" "+thrown.toString());
    }

}
