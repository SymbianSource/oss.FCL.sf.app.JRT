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
* Description:  This class for framing error string specific to OS error codes
*
*/

package com.nokia.mj.impl.utils;

import java.util.Hashtable;

/**
 * Base class for classes defining localized error messages.
 *
 * @author Nokia Corporation
 * @version 1.0
 * @see InstallerExceptionBase
 */
public final class OsErrorMessage
{
    /**
     * OS success error code.
     */
    public static final int SUCCESS = 0;
    public static final int KERR_NOT_FOUND = -1;
    public static final int KERR_GENERAL = -2;
    public static final int KERR_CANCEL = -3;
    public static final int KERR_NO_MEMORY = -4;
    public static final int KERR_NOT_SUPPORTED = -5;
    public static final int KERR_ARGUMENT = -6;
    public static final int KERR_TOTAL_LOSS_OF_PRECISION = -7;
    public static final int KERR_BAD_HANDLE = -8;
    public static final int KERR_OVERFLOW = -9;
    public static final int KERR_UNDERFLOW = -10;
    public static final int KERR_ALREADY_EXISTS = -11;
    public static final int KERR_PATH_NOT_FOUND = -12;
    public static final int KERR_DIED = -13;
    public static final int KERR_IN_USE = -14;
    public static final int KERR_SERVER_TERMINATD = -15;
    public static final int KERR_SERVER_BUSY = -16;
    public static final int KERR_COMPLETION = -17;
    public static final int KERR_NOT_READY = -18;
    public static final int KERR_UNKNOWN = -19;
    public static final int KERR_CORRUPT = -20;
    public static final int KERR_ACCESS_DENIED = -21;
    public static final int KERR_LOCKED = -22;
    public static final int KERR_WRITE = -23;
    public static final int KERR_DISMOUNTED = -24;
    public static final int KERR_EOF = -25;
    public static final int KERR_DISK_FULL = -26;
    public static final int KERR_BAD_DRIVER = -27;
    public static final int KERR_BAD_NAME = -28;
    public static final int KERR_COMMS_LINE_FAIL = -29;
    public static final int KERR_COMMS_FRAME = -30;
    public static final int KERR_COMMS_OVERRUN = -31;
    public static final int KERR_COMMS_PARITY = -32;
    public static final int KERR_TIMED_OUT = -33;
    public static final int KERR_COULD_NOT_CONNECT = -34;
    public static final int KERR_COULD_NOT_DISCONNECT = -35;
    public static final int KERR_DISCONNECTED = -36;
    public static final int KERR_BAD_LIBRARY_ENTRY_POINT = -37;
    public static final int KERR_BAD_DESCRIPTOR = -38;
    public static final int KERR_ABORT = -39;
    public static final int KERR_TOO_BIG = -40;
    public static final int KERR_DIVIDE_BY_ZERO = -41;
    public static final int KERR_BAD_POWER = -42;
    public static final int KERR_DIR_FULL = -43;
    public static final int KERR_HARDWARE_NOT_AVAILABLE = -44;
    public static final int KERR_SESSION_CLOSED = -45;
    public static final int KERR_PERMISSION_DENIED = -46;
    public static final int KPushConnectionAlreadyExists= -11030;
    public static final int KCommonSrvConnPluginError = -11050;

    /**
     * Mapping between OS error codes and OS error string
     */
    private static Hashtable messageTable = null;

    /**
     * Initialization of the mapping.
     */
    static
    {
        messageTable = new Hashtable();
        messageTable.put(new Integer(SUCCESS), " Success");
        messageTable.put(new Integer(-1), " Item not found");
        messageTable.put(new Integer(-2), " General Error");
        messageTable.put(new Integer(-3), " Operation is cancelled");
        messageTable.put(new Integer(-4), " Cannot allocate memory");
        messageTable.put(new Integer(-5), " Not supported");
        messageTable.put(new Integer(-6), " Argument is out of range");
        messageTable.put(new Integer(-7), " Total loss of precision");
        messageTable.put(new Integer(-8), " Invalid handle");
        messageTable.put(new Integer(-9), " Overflow");
        messageTable.put(new Integer(-10), " Underflow");
        messageTable.put(new Integer(-11), " Already exists");
        messageTable.put(new Integer(-12), " Path not found");
        messageTable.put(new Integer(-13), " Thread died");
        messageTable.put(new Integer(-14), " Resource already in use");
        messageTable.put(new Integer(-15), " Server terminated");
        messageTable.put(new Integer(-16), " Server busy");
        messageTable.put(new Integer(-17), " Operation complete");
        messageTable.put(new Integer(-18), " Not ready");
        messageTable.put(new Integer(-19), " Unknown device");
        messageTable.put(new Integer(-20), " Data corrupted");
        messageTable.put(new Integer(-21), " Access Denied");
        messageTable.put(new Integer(-22), " Resource is locked");
        messageTable.put(new Integer(-23), " Write operation failed");
        messageTable.put(new Integer(-24), " File system operation is dismounted");
        messageTable.put(new Integer(-25), " End of file");
        messageTable.put(new Integer(-26), " Disk is full");
        messageTable.put(new Integer(-27), " Wrong device driver");
        messageTable.put(new Integer(-28), " Invalid file\resource name");
        messageTable.put(new Integer(-29), " Communication line failed");
        messageTable.put(new Integer(-30), " Communication frame error");
        messageTable.put(new Integer(-31), " Communication overrun ");
        messageTable.put(new Integer(-32), " Communication parity error");
        messageTable.put(new Integer(-33), " Operation timed out");
        messageTable.put(new Integer(-34), " Cannot connect");
        messageTable.put(new Integer(-35), " Cannot disconnect");
        messageTable.put(new Integer(-36), " Disconnected error");
        messageTable.put(new Integer(-37), " Bad library entry point");
        messageTable.put(new Integer(-38), " Bad string descriptor");
        messageTable.put(new Integer(-39), " Operation is aborted");
        messageTable.put(new Integer(-40), " Number is too big");
        messageTable.put(new Integer(-41), " Divide by zero error");
        messageTable.put(new Integer(-42), " Insufficient power available");
        messageTable.put(new Integer(-43), " Directory full");
        messageTable.put(new Integer(-44), " Hardware not available");
        messageTable.put(new Integer(-45), " Session is closed");
        messageTable.put(new Integer(-46), " Permission denied");
        messageTable.put(new Integer(-47), " Extension function not supported");
        messageTable.put(new Integer(-48), " Communication break error");
        messageTable.put(new Integer(-49), " Time source not found");
        messageTable.put(new Integer(-11030), "Push Connection Already Exists");
        messageTable.put(new Integer(-11050), "Common server connection plugin error");

        // Add linux error codes also since openc and opecpp returns linux error codes
        messageTable.put(new Integer(1), " Operation not permitted");
        messageTable.put(new Integer(2), " No such file or directory");
        messageTable.put(new Integer(3), " No such process");
        messageTable.put(new Integer(4), " Interrupted systemcall");
        messageTable.put(new Integer(5), " Input/outputerror");
        messageTable.put(new Integer(6), " No such device or address");
        messageTable.put(new Integer(7), " Argument list too long");
        messageTable.put(new Integer(8), " Exec format error");
        messageTable.put(new Integer(9), " Bad file descriptor");
        messageTable.put(new Integer(10), " No child processes");
        messageTable.put(new Integer(11), " Resource temporarily unavailable");
        messageTable.put(new Integer(12), " Cannot allocate memory");
        messageTable.put(new Integer(13), " Permission denied");
        messageTable.put(new Integer(14), " Bad address");
        messageTable.put(new Integer(15), " Block device required");
        messageTable.put(new Integer(16), " Device or resource busy");
        messageTable.put(new Integer(17), " File exists");
        messageTable.put(new Integer(18), " Invalid cross-devicelink");
        messageTable.put(new Integer(19), " No such device");
        messageTable.put(new Integer(20), " Not a directory");
        messageTable.put(new Integer(21), " Is a directory");
        messageTable.put(new Integer(22), " Invalid argument");
        messageTable.put(new Integer(23), " Too many openfiles in system");
        messageTable.put(new Integer(24), " Too many open files");
        messageTable.put(new Integer(25), " Inappropriate ioctl for device");
        messageTable.put(new Integer(26), " Text file busy");
        messageTable.put(new Integer(27), " File too large");
        messageTable.put(new Integer(28), " No space left on device");
        messageTable.put(new Integer(29), " Illegal seek");
        messageTable.put(new Integer(30), " Read-only file system");
        messageTable.put(new Integer(31), " Too many links");
        messageTable.put(new Integer(32), " Broken pipe");
        messageTable.put(new Integer(33), " Numerical argument out of domain");
        messageTable.put(new Integer(34), " Numerical result out of range");
        messageTable.put(new Integer(35), " Resource deadlock avoided");
        messageTable.put(new Integer(36), " Filename too long");
        messageTable.put(new Integer(37), " No locks available");
        messageTable.put(new Integer(38), " Function not implemented");
        messageTable.put(new Integer(39), " Directory not empty");
        messageTable.put(new Integer(40), " Too many levels of symbolic links");
        messageTable.put(new Integer(41), " Operation would block");
        messageTable.put(new Integer(42), " Nomessage of desired type");
        messageTable.put(new Integer(43), " Identifier removed");
        messageTable.put(new Integer(44), " Channel number out of range");
        messageTable.put(new Integer(45), " Level2 not synchronized");
        messageTable.put(new Integer(46), " Level3 halted");
        messageTable.put(new Integer(47), " Level3 reset");
        messageTable.put(new Integer(48), " Link number out of range");
        messageTable.put(new Integer(49), " Protocol driver not attached");
        messageTable.put(new Integer(50), " No CSI structure available");
        messageTable.put(new Integer(51), " Level2 halted");
        messageTable.put(new Integer(52), " Invalid exchange");
        messageTable.put(new Integer(53), " Invalid request descriptor");
        messageTable.put(new Integer(54), " Exchange full");
        messageTable.put(new Integer(55), " No anode");
        messageTable.put(new Integer(56), " Invalid request code");
        messageTable.put(new Integer(57), " Invalid slot");
        messageTable.put(new Integer(58), " Resource deadlock");
        messageTable.put(new Integer(59), " Bad font file format");
        messageTable.put(new Integer(60), " Device not a stream");
        messageTable.put(new Integer(61), " No data available");
        messageTable.put(new Integer(62), " Timer expired");
        messageTable.put(new Integer(63), " Out of streams resources");
        messageTable.put(new Integer(64), " Machine is not on the network");
        messageTable.put(new Integer(65), " Package not installed");
        messageTable.put(new Integer(66), " Object is remote");
        messageTable.put(new Integer(67), " Link has been severed");
        messageTable.put(new Integer(68), " Advertis eerror");
        messageTable.put(new Integer(69), " Srmount error");
        messageTable.put(new Integer(70), " Communication error on send");
        messageTable.put(new Integer(71), " Protocol error");
        messageTable.put(new Integer(72), " Multihop attempted");
        messageTable.put(new Integer(73), " RFS specific error");
        messageTable.put(new Integer(74), " Bad message");
        messageTable.put(new Integer(75), " Value too large for defined datatype");
        messageTable.put(new Integer(76), " Name not unique on network");
        messageTable.put(new Integer(77), " File descriptorin bad state");
        messageTable.put(new Integer(78), " Remote address changed");
        messageTable.put(new Integer(79), " Cannot access a needed shared library");
        messageTable.put(new Integer(80), " Accessing a corrupted shared library");
        messageTable.put(new Integer(81), " .lib section in a.out corrupted");
        messageTable.put(new Integer(82), " Attempting to link in too many shared libraries");
        messageTable.put(new Integer(83), " Cannot exec a shared library directly");
        messageTable.put(new Integer(84), " Invalid or incomplete multibyte or widecharacter");
        messageTable.put(new Integer(85), " Interrupted systemcall should be restarted");
        messageTable.put(new Integer(86), " Streams pipe error");
        messageTable.put(new Integer(87), " Too many users");
        messageTable.put(new Integer(88), " Socket operation on non-socket");
        messageTable.put(new Integer(89), " Destination address required");
        messageTable.put(new Integer(90), " Message too long");
        messageTable.put(new Integer(91), " Protocol wrong type for socket");
        messageTable.put(new Integer(92), " Protocol not available");
        messageTable.put(new Integer(93), " Protocol not supported");
        messageTable.put(new Integer(94), " Socket type not supported");
        messageTable.put(new Integer(95), " Operation not supported");
        messageTable.put(new Integer(96), " Protocol family not supported");
        messageTable.put(new Integer(97), " Address family not supported by protocol");
        messageTable.put(new Integer(98), " Address already in use");
        messageTable.put(new Integer(99), " Cannot assign requested address");
        messageTable.put(new Integer(100), " Network is down");
        messageTable.put(new Integer(101), " Network is unreachable");
        messageTable.put(new Integer(102), " Network dropped connection on reset");
        messageTable.put(new Integer(103), " Software caused connection abort");
        messageTable.put(new Integer(104), " Connection reset by peer");
        messageTable.put(new Integer(105), " No buffer space available");
        messageTable.put(new Integer(106), " Transport endpoint is already connected");
        messageTable.put(new Integer(107), " Transport endpoint is not connected");
        messageTable.put(new Integer(108), " Cannot send after transport endpoint shutdown");
        messageTable.put(new Integer(109), " Too many references: cannot splice");
        messageTable.put(new Integer(110), " Connection timed out");
        messageTable.put(new Integer(111), " Connection refused");
        messageTable.put(new Integer(112), " Host is down");
        messageTable.put(new Integer(113), " No route to host");
        messageTable.put(new Integer(114), " Operation already in progress");
        messageTable.put(new Integer(115), " Operation now in progress");
        messageTable.put(new Integer(116), " Stale NFS file handle");
        messageTable.put(new Integer(117), " Structure needs cleaning");
        messageTable.put(new Integer(118), " Not a XENIX named typefile");
        messageTable.put(new Integer(119), " No XENIX semaphores available");
        messageTable.put(new Integer(120), " Is a named typefile");
        messageTable.put(new Integer(121), " Remote I/O error");
        messageTable.put(new Integer(122), " Disk quota exceeded");
        messageTable.put(new Integer(123), " No medium found");
        messageTable.put(new Integer(124), " Wrong medium type");
        messageTable.put(new Integer(125), " Operation canceled");
        messageTable.put(new Integer(126), " Required key not available");
        messageTable.put(new Integer(127), " Key has expired");
        messageTable.put(new Integer(128), " Key has been revoked");
        messageTable.put(new Integer(129), " Key was rejected by service");
        messageTable.put(new Integer(130), " Owner died");
        messageTable.put(new Integer(131), " State not recoverable");
    }

    /*** ----------------------------- PUBLIC ------------------------------ */

    /**
     * Returns OTA status report message body.
     *
     * @param aErrCode The OS status code whose message body is queried
     * @return The corresponding Os error string
     */
    public static String getErrorMessage(int aErrCode)
    {
        String result = (String)messageTable.get(new Integer(aErrCode));
        if (result == null)
        {
            result = "Unknown error " + Integer.toString(aErrCode);
        }
        return result;
    }

    /*** ---------------------------- PROTECTED --------------------------- */
    /*** ----------------------------- PACKAGE ---------------------------- */
    /*** ----------------------------- PRIVATE ---------------------------- */
    /*** ----------------------------- NATIVE ----------------------------- */

}