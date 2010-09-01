/*******************************************************************************
 * Copyright (c) 2000, 2005 IBM Corporation and others.
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *
 * Contributors:
 *     IBM Corporation - initial API and implementation
 *     Nokia Corporation - S60 implementation
 *******************************************************************************/
package org.eclipse.swt.internal;

public class Library
{

    /**
     * The JAVA and SWT versions
     */
    public static final int JAVA_VERSION, SWT_VERSION;

    static
    {
        JAVA_VERSION = parseVersion(System.getProperty("java.version"));
        SWT_VERSION = 1;
    }

    static int parseVersion(String version)
    {
        if (version == null) return 0;
        int major = 0, minor = 0, micro = 0;
        int length = version.length(), index = 0, start = 0;
        while (index < length && Character.isDigit(version.charAt(index))) index++;
        try
        {
            if (start < length) major = Integer.parseInt(version.substring(start, index));
        }
        catch (NumberFormatException e) {}
        start = ++index;
        while (index < length && Character.isDigit(version.charAt(index))) index++;
        try
        {
            if (start < length) minor = Integer.parseInt(version.substring(start, index));
        }
        catch (NumberFormatException e) {}
        start = ++index;
        while (index < length && Character.isDigit(version.charAt(index))) index++;
        try
        {
            if (start < length) micro = Integer.parseInt(version.substring(start, index));
        }
        catch (NumberFormatException e) {}
        return JAVA_VERSION(major, minor, micro);
    }

    /**
     * Returns the Java version number as an integer.
     *
     * @param major
     * @param minor
     * @param micro
     * @return the version
     */
    public static int JAVA_VERSION(int major, int minor, int micro)
    {
        return (major << 16) + (minor << 8) + micro;
    }
    /**
     * Returns the SWT version as an integer in the standard format
     * <em>Mmmm</em> where <em>M</em> is the major version number
     * and <em>mmm</em> is the minor version number.
     *
     * @return the version of the currently running SWT
     */
    public static int getVersion()
    {
        return SWT_VERSION;
    }

    /**
     * Returns the SWT platform name.
     *
     * @return the platform name of the currently running SWT
     */
    public static String getPlatform()
    {
        return Platform.PLATFORM;
    }

    /**
     * Returns the SWT revision number as an integer. Revision changes
     * occur as a result of non-API breaking bug fixes.
     *
     * @return the revision number of the currently running SWT
     */
    public static int getRevision()
    {
        return 0;
    }

    /**
     * Loads the shared library that matches the version of the
     * Java code which is currently running.  SWT shared libraries
     * follow an encoding scheme where the major, minor and revision
     * numbers are embedded in the library name and this along with
     * <code>name</code> is used to load the library.  If this fails,
     * <code>name</code> is used in another attempt to load the library,
     * this time ignoring the SWT version encoding scheme.
     *
     * @param name the name of the library to load
     */
    public static void loadLibrary(String name)
    {
        System.loadLibrary(name);
    }

}

