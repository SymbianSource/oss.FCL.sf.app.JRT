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

package com.nokia.mj.impl.security.packageprotection;

/**
 * ScanCheck class is used to specify custom checks
 * for the PackageProtector scanApplication() method.
 */
public final class ScanCheck
{
    /** Array of base class checks to be made. */
    public BaseClassCheck[] iBaseClassChecks;

    /** Array of method checks to be made. */
    public MethodCheck[] iMethodChecks;

    public ScanCheck(
        BaseClassCheck[] baseClassChecks, MethodCheck[] methodChecks)
    {
        iBaseClassChecks = baseClassChecks;
        iMethodChecks = methodChecks;
    }

    public String toString()
    {
        StringBuffer buf = new StringBuffer();
        if (iBaseClassChecks != null)
        {
            buf.append("BaseClassChecks:\n");
            for (int i = 0; i < iBaseClassChecks.length; i++)
            {
                buf.append(iBaseClassChecks[i]);
                buf.append("\n");
            }
        }
        if (iMethodChecks != null)
        {
            buf.append("MethodChecks:\n");
            for (int i = 0; i < iMethodChecks.length; i++)
            {
                buf.append(iMethodChecks[i]);
                buf.append("\n");
            }
        }
        return buf.toString();
    }

    /**
     * Checks if the application has any classes which are inherited
     * from the specified base class.
     */
    public static class BaseClassCheck
    {
        /**
         * Input parameter: base class name.
         */
        public String iBaseClassName;

        /**
         * Output parameter: names of the classes which are inherited
         * from iBaseClassname.
         */
        public String[] iClassNames;

        public BaseClassCheck(String baseClassName)
        {
            iBaseClassName = baseClassName;
        }

        public String toString()
        {
            StringBuffer buf = new StringBuffer();
            buf.append("BaseClassName: ").append(iBaseClassName);
            buf.append(", matching ClassNames: ");
            if (iClassNames != null)
            {
                for (int i = 0; i < iClassNames.length; i++)
                {
                    buf.append(iClassNames[i]);
                    if (i < iClassNames.length-1)
                    {
                        buf.append(", ");
                    }
                }
            }
            return buf.toString();
        }
    }

    /**
     * Checks if the application has any classes which implement
     * the specified methods.
     */
    public static class MethodCheck
    {
        /**
         * Input parameter: method name.
         */
        public String iMethodName;

        /**
         * Input parameter: method signature.
         */
        public String iMethodSignature;

        /**
         * Output parameter: classes which implement the specified method.
         */
        public MethodCheckResult[] iResults;

        public MethodCheck(String methodName, String methodSignature)
        {
            iMethodName = methodName;
            iMethodSignature = methodSignature;
        }

        public String toString()
        {
            StringBuffer buf = new StringBuffer();
            buf.append("MethodName: ").append(iMethodName);
            buf.append("[").append(iMethodSignature);
            buf.append("], results: ");
            if (iResults != null)
            {
                for (int i = 0; i < iResults.length; i++)
                {
                    buf.append(iResults[i]);
                    if (i < iResults.length-1)
                    {
                        buf.append(", ");
                    }
                }
            }
            return buf.toString();
        }
    }

    /**
     * Results for method check.
     */
    public static class MethodCheckResult
    {
        /**
         * Name of the class implementing the method.
         */
        public String iClassName;

        /**
         * Code length for the method. Code length one indicates
         * that method exists but has an empty implementation.
         */
        public int iCodeLength;

        public MethodCheckResult(String className, int codeLength)
        {
            iClassName = className;
            iCodeLength = codeLength;
        }

        public String toString()
        {
            StringBuffer buf = new StringBuffer();
            buf.append("ClassName: ").append(iClassName);
            buf.append(", CodeLength: ").append(iCodeLength);
            return buf.toString();
        }
    }
}
