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
package javax.microedition.lcdui;


/**
 * Logger utility class.<br>
 */
final class Logger {

    private static StringBuffer methodSb = new StringBuffer();

    private static boolean logVerbose = true;

    private Logger() {
    }

    /**
     * Checks if this logging level will be logged.
     *
     * @param level logging level
     */
    static boolean isLogVerbose() {
        return logVerbose;
    }

    static void method(Object obj, String fn) {
        method(obj, fn, null, null, null);
    }

    static void method(Object obj, String fn, Object p1) {
        method(obj, fn, p1, null, null);
    }
    static void method(Object obj, String fn, Object p1, Object p2) {
        method(obj, fn, p1, p2, null);
    }

    /**
     * Used for logging a method call.
     *
     * @param obj the called object
     * @param fn the method name
     * @param p1 first parameter
     * @param p2 second parameter
     * @param p3 third parameter
     */
    static void method(Object obj, String fn, Object p1, Object p2, Object p3) {
        if (logVerbose) {
            methodSb.setLength(0);
            methodSb.append(obj);
            methodSb.append("::");
            methodSb.append(fn);
            methodSb.append("(");
            if (p1 != null) {
                methodSb.append(p1);
            }
            if (p2 != null) {
                methodSb.append(", ");
                methodSb.append(p2);
            }
            if (p3 != null) {
                methodSb.append(", ");
                methodSb.append(p3);
            }
            methodSb.append(")");
            com.nokia.mj.impl.utils.Logger.LOG(
                    com.nokia.mj.impl.utils.Logger.EJavaUI,
                    com.nokia.mj.impl.utils.Logger.EInfo, methodSb.toString());
        }
    }

    /**
     * Log VERBOSE level message.
     *
     * @param msg a message
     */
    static void verbose(String msg) {
        com.nokia.mj.impl.utils.Logger.LOG(
                com.nokia.mj.impl.utils.Logger.EJavaUI,
                com.nokia.mj.impl.utils.Logger.EInfo, msg);
    }

    /**
     * Log INFO level message.
     *
     * @param msg a message
     */
    static void info(String msg) {
        com.nokia.mj.impl.utils.Logger.LOG(
                com.nokia.mj.impl.utils.Logger.EJavaUI,
                com.nokia.mj.impl.utils.Logger.EInfo, msg);
    }

    /**
     * Log WARNING level message.
     *
     * @param msg a message
     */
    static void warning(String msg) {
        com.nokia.mj.impl.utils.Logger.WLOG(
                com.nokia.mj.impl.utils.Logger.EJavaUI, msg);
    }

    /**
     * Log an EXCEPTION (ERROR level).
     *
     * @param msg a message
     */
    static void exception(String msg, Exception e) {
        com.nokia.mj.impl.utils.Logger.ELOG(
                com.nokia.mj.impl.utils.Logger.EJavaUI, msg, e);
        // e.printStackTrace();
    }

    /**
     * Log ERROR level message.
     *
     * @param msg a message
     */
    static void error(String msg) {
        com.nokia.mj.impl.utils.Logger.ELOG(
                com.nokia.mj.impl.utils.Logger.EJavaUI, msg);
    }

    /**
     * Indents a string with the given number of spaces.
     */
    static String indent(String str, int numSpacesBefore) {
        String ret = (str == null ? "" : str);
        for (int i = 0; i < numSpacesBefore; i++) {
            ret = " " + ret;
        }
        return ret;
    }

}
