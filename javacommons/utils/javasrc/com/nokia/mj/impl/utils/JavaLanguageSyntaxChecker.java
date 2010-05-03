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

package com.nokia.mj.impl.utils;

/**
 * Utility class for validating java language related items (e.g package name)
 * according to Java Language Specification
 *
 * @author Nokia Corporation
 * @version 1.0
 */
public final class JavaLanguageSyntaxChecker
{
    // the names of all the keywords of the java language
    private static final String[] JAVA_KEYWORDS =
    {
        "abstract","continue","for","new","switch",
        "assert","default","if","package","synchronized",
        "boolean","do","goto","private","this","break",
        "double","implements","protected","throw","byte",
        "else","import","public","throws","case","enum",
        "instanceof","return","transient","catch","extends",
        "int","short","try","char","final","interface","static",
        "void","class","finally","long","strictfp","volatile",
        "const","float","native","super","while",
        /* the following are not keywords but literals. still added
           here for simplicity */
        "true", "false", "null"
    };

    /*** ----------------------------- PUBLIC ------------------------------ */

    /**
     * Validates a fully qualified class name, which should be, according to Java
     * Language Specification defined as following:
     *  FullyQualifiedCassName:
     *      PackageName . Identifier
     *  PackageName:
     *      Identifier
     *      PackageName . Identifier
     *  Identifier:
     *      IdentifierChars but not a Keyword or BooleanLiteral or NullLiteral
     *  IdentifierChars:
     *      JavaLetter
     *      IdentifierChars JavaLetterOrDigit
     *  JavaLetter:
     *      any Unicode character that is a Java letter
     *  JavaLetterOrDigit:
     *      any Unicode character that is a Java letter-or-digit
     *
     * @param fullyQualifiedClassName The fully qualified name of the class to be checked
     * @return Returns the boolean value 'true' if the provided class name is a valid
     *         class name or 'false' otherwise
     */
    public static boolean checkClassName(String fullyQualifiedClassName)
    {
        if (fullyQualifiedClassName == null
                || fullyQualifiedClassName.length() == 0)
        {
            return false;
        }
        // check that first char is either a letter or '_' or '$'
        char c = fullyQualifiedClassName.charAt(0);
        boolean isLetter = (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z');
        if (!isLetter && c != '_' && c != '$')
        {
            return false;
        }
        StringBuffer identifier = new StringBuffer();
        identifier.append(c);
        int len = fullyQualifiedClassName.length();
        for (int i=1; i<len; i++)
        {
            c = fullyQualifiedClassName.charAt(i);
            if (c != '.')
            {
                // check that it is JavaLetterOrDigit
                boolean isLetterOrDigit = (c >= 'A' && c <= 'Z')
                                          || (c >= 'a' && c <= 'z')
                                          || (c >= '0' && c <= '9');
                if (!isLetterOrDigit && c != '_' && c != '$')
                {
                    return false;
                }
                // save it so we can check it later
                identifier.append(c);
            }
            else
            {
                // check it is not empty
                if (identifier.length() == 0)
                {
                    return false;
                }
                // check it is not a keyword
                if (isKeyword(identifier.toString()))
                {
                    return false;
                }
                // reset the identifier
                identifier = new StringBuffer();
            }
        }
        // check that the last identifier is not a keyword
        return !isKeyword(identifier.toString());
    }

    /*** ---------------------------- PROTECTED --------------------------- */
    /*** ----------------------------- PACKAGE ---------------------------- */
    /*** ----------------------------- PRIVATE ---------------------------- */

    private static boolean isKeyword(String str)
    {
        for (int i=0; i<JAVA_KEYWORDS.length; i++)
        {
            if (str.equals(JAVA_KEYWORDS[i]))
            {
                return true;
            }
        }
        return false;
    }

    /*** ----------------------------- NATIVE ----------------------------- */
}
