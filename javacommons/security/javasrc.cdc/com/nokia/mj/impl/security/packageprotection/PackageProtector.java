/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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

import com.nokia.mj.impl.security.common.InstallerSecurityException;
import com.nokia.mj.impl.security.midp.common.SecurityExtensionsReader;
import com.nokia.mj.impl.utils.InstallerDetailedErrorMessage;
import com.nokia.mj.impl.utils.InstallerErrorMessage;
import com.nokia.mj.impl.utils.OtaStatusCode;
import com.nokia.mj.impl.security.utils.Logger;

import java.io.InputStream;
import java.io.DataInputStream;
import java.io.IOException;
import java.io.ByteArrayOutputStream;
import java.util.jar.JarFile;
import java.util.jar.JarEntry;
import java.util.Enumeration;
import java.util.Hashtable;
import java.util.Vector;

/**
 * The java core functionality is made available for MIDlets through a set of
 * public APIs. In order to keep that functionality unchanged, there needs to
 * be restrictions on the way applications may access classes/packages.
 * These restrictions can be divided into two categories:
 *  - Protected packages are packages that applications are not allowed to
 *    reference neither have their own classes within
 *  - Restricted packages are packages that applications may reference but
 *    not have their own classes within
 * The checks against these violations are done at installation time
 * (to prevent installing an application that wouldn't be able to run
 * anyway) and at runtime (to prevent applications dynamically violating the
 * rules).
 * Package protection implies keeping track of the protected and restricted
 * packages, provide list of those packages for Java Runtime (that registers
 * them to the JVM) and for scanning application's deployment package against
 * possible conflicts with these restrictions.
 * When new extensions are installed to the system, they can register new
 * protected and restricted packages to the system.
 */
public final class PackageProtector
{
    // extension for files being scanned
    private static final String CLASS_FILE_EXTENSION = ".class";

    // hard-coded upper limit for a java class size 1M
    private static final int CLASS_FILE_MAX_SIZE = 1000*1024;

    /**
     * Creates an instance of the PackageProtector
     *
     * @return An instance of PackageProtector
     */
    public static PackageProtector getInstance()
    {
        if (self == null)
        {
            self = new PackageProtector();
        }
        return self;
    }

    /**
     * Scans an application's deployment package against the
     * protected/restricted packages
     *
     * @param appPackagePath path to the application package which is to be scanned
     */
    public void scanApplication(String appPackagePath)
    {
        scanApplication(appPackagePath, null);
    }

    /**
     * Scans an application's deployment package against the
     * protected/restricted packages
     *
     * @param appPackagePath path to the application package which is to be scanned
     * @param scanChecks custom scan checks to be made, can be null
     */
    public void scanApplication(String appPackagePath, ScanCheck scanChecks)
    {
        // give each of the JAR's .class files to JavaClassInfo to
        // extract the (referenced) class names and put these against
        // the list of restricted and protected package names

        JarFile jar = null;
        try
        {
            Hashtable classInfos = new Hashtable();
            jar = new JarFile(appPackagePath);
            for (Enumeration e=jar.entries(); e.hasMoreElements();)
            {
                JarEntry entry = (JarEntry)e.nextElement();
                if (entry.getName().toLowerCase().endsWith(
                            CLASS_FILE_EXTENSION))
                {
                    // check the entries size does not exceed the max size
                    // for a java class file
                    long entrySize = entry.getSize();
                    if (entrySize > CLASS_FILE_MAX_SIZE)
                    {
                        // stop here
                        String details = "Could not process "
                                         + entry.getName()
                                         + " since its size ("
                                         + entrySize
                                         + ") is bigger than the maximum allowed size for a class file ("
                                         + CLASS_FILE_MAX_SIZE + ")";
                        Logger.logError(details);
                        throw new InstallerSecurityException(
                            InstallerErrorMessage.INST_NO_MEM,
                            null, /* no params for short msg */
                            InstallerDetailedErrorMessage.INTERNAL_ERROR,
                            new String[] {details},
                            OtaStatusCode.INTERNAL_ERROR);
                    }
                    try
                    {
                        JavaClassInfo classInfo = new JavaClassInfo(
                            jar.getInputStream(entry), entry.getName(),
                            classInfos);
                        checkForProtectedPackagesConflicts(
                            classInfo.getClassName(),
                            classInfo.getSuperClassName(),
                            classInfo.getReferencedClassNames());
                        checkForRestrictedPackagesConflicts(
                            classInfo.getClassName(),
                            classInfo.getSuperClassName());
                    }
                    catch (IOException exc)
                    {
                        Logger.logWarning("The class " + entry.getName() + " has invalid format: " + exc.getMessage());
                        throw new InstallerSecurityException(
                            InstallerErrorMessage.INST_CORRUPT_PKG,
                            null, /* no params for short msg */
                            InstallerDetailedErrorMessage.INTERNAL_ERROR,
                            new String[] {"The class " + entry.getName()
                                          + " has invalid format."
                                         },
                            OtaStatusCode.INTERNAL_ERROR);
                    }
                }
            }
            // Make custom scan checks after information of all classes
            // has been collected to classInfos.
            doScanChecks(scanChecks, classInfos);
        }
        catch (IOException e)
        {
            Logger.logWarning("Scanning the deployment package " + appPackagePath + " failed: " + e.getMessage());
            throw new InstallerSecurityException(
                InstallerErrorMessage.INST_CORRUPT_PKG,
                null, /* no params for short msg */
                InstallerDetailedErrorMessage.INTERNAL_ERROR,
                new String[] {e.getMessage()},
                OtaStatusCode.INTERNAL_ERROR);
        }
        finally
        {
            if (jar != null)
            {
                try
                {
                    jar.close();
                }
                catch (IOException e) {}
            }
        }
    }

    /**
     * Retrieves the names of the protected packages
     *
     * @return An array of strings containing the names of the protected
     *         packages
     */
    public String[] getProtectedPackageNames()
    {
        return PackageNames.protectedPackages;
    }

    /**
     * Retrieves the names of the restricted packages
     *
     * @return An array of strings containing the names of the restricted
     *         packages
     */
    public String[] getRestrictedPackageNames()
    {
        return PackageNames.restrictedPackages;
    }

    /**
     * Marks package names as protected packages
     *
     * @param aPackageNames The names of the packages to be marked as protected
     *                      packages
     */
    public void addProtectedPackageNames(String[] aPackageNames)
    {
        // Add implementation
    }

    /**
     * Marks package names as restricted packages
     *
     * @param aPackageNames The names of the packages to be marked as
     *                      restricted packages
     */
    public void addRestrictedPackageNames(String[] aPackageNames)
    {
        // Add implementation
    }

    /**
     * Performs a cleanup (e.g. on cached data)
     *
     */
    public void cleanup()
    {
    }

    // self
    private static PackageProtector self;

    private static String[] protectedPackages2;
    private static String[] restrictedPackages2;

    static
    {
        Vector extProtectedPackages = SecurityExtensionsReader.getExtProtectedPackages();
        protectedPackages2 = new String[PackageNames.protectedPackages.length + extProtectedPackages.size()];
        int i=0;
        for (i=0; i<PackageNames.protectedPackages.length; i++)
        {
            protectedPackages2[i] = PackageNames.protectedPackages[i].replace('.', '/');
        }
        for (int j=0; j<extProtectedPackages.size(); j++)
        {
            protectedPackages2[i + j] = ((String)extProtectedPackages.elementAt(j)).replace('.', '/');
        }
        Vector extRestrictedPackages = SecurityExtensionsReader.getExtRestrictedPackages();
        restrictedPackages2 = new String[PackageNames.restrictedPackages.length + extRestrictedPackages.size()];
        for (i=0; i<PackageNames.restrictedPackages.length; i++)
        {
            restrictedPackages2[i] = PackageNames.restrictedPackages[i].replace('.', '/');
        }
        for (int j=0; j<extRestrictedPackages.size(); j++)
        {
            restrictedPackages2[i + j] = ((String)extRestrictedPackages.elementAt(j)).replace('.', '/');
        }
    }


    private void checkForProtectedPackagesConflicts(String className, String superClassName, Vector referencedClassNames)
    {
        for (int i=0; i<protectedPackages2.length; i++)
        {
            int index = find(protectedPackages2[i], referencedClassNames);
            if ((className != null
                    && className.startsWith(protectedPackages2[i]))
                    || (superClassName != null
                        && superClassName.startsWith(protectedPackages2[i]))
                    || index != -1)
            {
                Logger.logWarning(protectedPackages2[i] + " is protected");
                throw new InstallerSecurityException(
                    InstallerErrorMessage.INST_AUTHORIZATION_ERR,
                    null, /* no params for short msg */
                    InstallerDetailedErrorMessage.PROT_PKG_MISUSE,
                    new String[] {protectedPackages2[i]},
                    OtaStatusCode.APPLICATION_AUTHORIZATION_FAILURE);
            }
        }
    }

    private void checkForRestrictedPackagesConflicts(String className, String superClassName)
    {
        if (className == null)
        {
            return;
        }
        for (int i=0; i<restrictedPackages2.length; i++)
        {
            if (className.startsWith(restrictedPackages2[i]))
            {
                Logger.logWarning(className + " is defined inside a restricted package " + protectedPackages2[i]);
                throw new InstallerSecurityException(
                    InstallerErrorMessage.INST_AUTHORIZATION_ERR,
                    null, /* no params for short msg */
                    InstallerDetailedErrorMessage.PROT_PKG_MISUSE,
                    new String[] {restrictedPackages2[i]},
                    OtaStatusCode.APPLICATION_AUTHORIZATION_FAILURE);
            }
        }
    }

    private int find(String str, Vector strs)
    {
        if (strs != null)
        {
            for (int i=0; i<strs.size(); i++)
            {
                // Is this canse sensitive or not
                String el = (String)strs.elementAt(i);
                if (el != null && el.startsWith(str))
                {
                    return i;
                }
            }
        }
        return -1;
    }

    private void doScanChecks(ScanCheck scanCheck, Hashtable classInfos)
    {
        if (scanCheck == null)
        {
            return;
        }
        if (scanCheck.iBaseClassChecks != null)
        {
            for (int i = 0 ; i < scanCheck.iBaseClassChecks.length; i++)
            {
                doBaseClassCheck(scanCheck.iBaseClassChecks[i], classInfos);
            }
        }
        if (scanCheck.iMethodChecks != null)
        {
            for (int i = 0 ; i < scanCheck.iMethodChecks.length; i++)
            {
                doMethodCheck(scanCheck.iMethodChecks[i], classInfos);
            }
        }
    }

    private void doBaseClassCheck(
        ScanCheck.BaseClassCheck baseClassCheck, Hashtable classInfos)
    {
        if (baseClassCheck == null)
        {
            return;
        }
        Vector results = new Vector();
        Enumeration e = classInfos.keys();
        while (e.hasMoreElements())
        {
            ClassInfo classInfo =
                (ClassInfo)classInfos.get(e.nextElement());
            if (isInheritedFrom(
                        classInfo, baseClassCheck.iBaseClassName, classInfos))
            {
                results.addElement(classInfo.iClassName);
            }
        }
        baseClassCheck.iClassNames = new String[results.size()];
        results.copyInto(baseClassCheck.iClassNames);
    }

    /**
     * Returns true if class specified in classInfo is inherited
     * from class specified in baseClassName, otherwise returns false.
     */
    private boolean isInheritedFrom(
        ClassInfo classInfo, String baseClassName, Hashtable classInfos)
    {
        if (classInfo == null || baseClassName == null)
        {
            return false;
        }
        while (classInfo != null && classInfo.iSuperClassName != null)
        {
            if (classInfo.iSuperClassName.equals(baseClassName))
            {
                return true;
            }
            else
            {
                classInfo =
                    (ClassInfo)classInfos.get(classInfo.iSuperClassName);
            }
        }
        return false;
    }

    private void doMethodCheck(
        ScanCheck.MethodCheck methodCheck, Hashtable classInfos)
    {
        if (methodCheck == null)
        {
            return;
        }
        Vector results = new Vector();
        Enumeration e = classInfos.keys();
        while (e.hasMoreElements())
        {
            ClassInfo classInfo =
                (ClassInfo)classInfos.get(e.nextElement());
            if (classInfo.iMethods != null)
            {
                for (int i = 0; i < classInfo.iMethods.size(); i++)
                {
                    MethodInfo method = (MethodInfo)classInfo.iMethods.elementAt(i);
                    if (methodCheck.iMethodName != null &&
                            methodCheck.iMethodName.equals(method.iName) &&
                            methodCheck.iMethodSignature != null &&
                            methodCheck.iMethodSignature.equals(method.iDescriptor))
                    {
                        results.addElement(
                            new ScanCheck.MethodCheckResult(
                                classInfo.iClassName, method.iCodeLength));
                    }
                }
            }
        }
        methodCheck.iResults = new ScanCheck.MethodCheckResult[results.size()];
        results.copyInto(methodCheck.iResults);
    }

    /**
     * Class holding Java class information needed when
     * traversing class hierarchies.
     */
    private static class ClassInfo
    {
        String iClassName = null;
        String iSuperClassName = null;
        Vector iMethods;

        public ClassInfo(String className, String superClassName, Vector methods)
        {
            iClassName = className;
            iSuperClassName = superClassName;
            iMethods = methods;
        }

        public String toString()
        {
            StringBuffer buf = new StringBuffer("Class: ");
            buf.append(iClassName);
            buf.append(", SuperClass: ");
            buf.append(iSuperClassName);
            if (iMethods != null)
            {
                buf.append(", Methods:\n");
                for (int i = 0; i < iMethods.size(); i++)
                {
                    buf.append(iMethods.elementAt(i).toString());
                    buf.append("\n");
                }
            }
            return buf.toString();
        }
    }

    /**
     * Class holding method information.
     */
    private static class MethodInfo
    {
        String iName = null;
        String iDescriptor = null;
        int iCodeLength = 0;

        public MethodInfo(String name, String descriptor, int codeLength)
        {
            iName = name;
            iDescriptor = descriptor;
            iCodeLength = codeLength;
        }

        public String toString()
        {
            StringBuffer buf = new StringBuffer();
            buf.append(iName).append(", ").append(iDescriptor);
            buf.append(", codeLength=").append(iCodeLength);
            return buf.toString();
        }
    }

    /**
     * Utility class used to look into a .class file and obtain info about the
     * fully qualified name of the defined class and also the fully qualified
     * class names of all the referenced classes
     */
    private static class JavaClassInfo
    {
        // the constant pool's tags
        private static final byte CONSTANT_CLASS_TAG = 7;
        private static final byte CONSTANT_FIELDREF_TAG = 9;
        private static final byte CONSTANT_METHODREF_TAG = 10;
        private static final byte CONSTANT_INTERFACEMETHODREF_TAG = 11;
        private static final byte CONSTANT_STRING_TAG = 8;
        private static final byte CONSTANT_INTEGER_TAG = 3;
        private static final byte CONSTANT_FLOAT_TAG = 4;
        private static final byte CONSTANT_LONG_TAG = 5;
        private static final byte CONSTANT_DOUBLE_TAG = 6;
        private static final byte CONSTANT_NAMEANDTYPE_TAG = 12;
        private static final byte CONSTANT_UTF8_TAG = 1;

        // attributes names
        private static final String CODE_ATTRIBUTE_NAME = "Code";
        private static final String LOCAL_VARIABLE_TABLE_ATTRIBUTE_NAME = "LocalVariableTable";
        private static final String LINE_NUMBER_TABLE_ATTRIBUTE_NAME = "LineNumberTable";
        private static final String SOURCE_FILE_ATTRIBUTE_NAME = "SourceFile";
        private static final String DEPRECATED_ATTRIBUTE_NAME = "Deprecated";
        private static final String INNER_CLASSES_ATTRIBUTE_NAME = "InnerClasses";

        // methods&fields descriptors constants
        private static final String OBJECT_TYPE_PREFIX = "L";
        private static final String OBJECT_TYPE_SUFFIX = ";";
        private static final String METHOD_DESCRIPTOR_PREFIX = "(";
        private static final String METHOD_DESCRIPTOR_SUFFIX = ")";

        private Vector referencedClassNames;
        private String myClassName;
        private String superClassName;
        private Vector myMethods = new Vector();

        // the name of the java class being scanned (used for
        // reporting invalid-class-type of errors)
        private String javaClassFileName;

        // collections for the class indexes referenced from fields,
        // methods or directly in the constant pool
        private Hashtable allClassIndexes = new Hashtable();
        private Vector allFieldsIndexes = new Vector();
        private Vector allMethodsIndexes = new Vector();
        private Hashtable UTF8s = new Hashtable();

        // the internal buffer containing the content of the class file
        // and the corresponding index
        byte[] buf;
        int index = 0;

        public JavaClassInfo(InputStream javaClassFile,
                             String javaClassFileName,
                             Hashtable classInfos)
        throws IOException
        {
            this.javaClassFileName = javaClassFileName;

            ByteArrayOutputStream oStream = new ByteArrayOutputStream();
            int len = javaClassFile.available();
            while (len > 0)
            {
                byte[] tmp = new byte[len];
                int bytesRead = javaClassFile.read(tmp);
                oStream.write(tmp, 0, bytesRead);
                len = javaClassFile.available();
            }
            buf = oStream.toByteArray();
            index = 0;
            parse(classInfos);
        }

        public Vector getReferencedClassNames()
        {
            return referencedClassNames;
        }

        public String getClassName()
        {
            return myClassName;
        }

        public String getSuperClassName()
        {
            return superClassName;
        }

        private void parse(Hashtable classInfos)
        {
            try
            {
                // magic
                readInt();
                // minor version
                readUnsignedShort();
                // major version
                readUnsignedShort();
                // constant pool
                readConstantPool();
                // access flags
                readUnsignedShort();
                // this and super class
                readThisAndSuperClass();
                // interfaces
                readInterfaces();
                // fields
                readFields();
                // methods
                readMethods();
                // attributes
                readAttributes();
                // Save information to ClassInfo object.
                classInfos.put(myClassName, new ClassInfo(
                                   myClassName, superClassName, myMethods));
            }
            catch (Throwable t)
            {
                Logger.logWarning("The class " + javaClassFileName + " has invalid format: " + t.getMessage());
                throw new InstallerSecurityException(
                    InstallerErrorMessage.INST_CORRUPT_PKG,
                    null, /* no params for short msg */
                    InstallerDetailedErrorMessage.INTERNAL_ERROR,
                    new String[] {"The class " + javaClassFileName
                                  + " has invalid format."
                                 },
                    OtaStatusCode.INTERNAL_ERROR);
            }
            // collect the referenced class names
            referencedClassNames = new Vector();
            for (Enumeration e=allClassIndexes.elements(); e.hasMoreElements();)
            {
                referencedClassNames.addElement(
                    (String)UTF8s.get(e.nextElement()));
            }
            for (int j=0; j<allFieldsIndexes.size(); j++)
            {
                String className = parseFieldDescriptor(
                                       (String)UTF8s.get(allFieldsIndexes.elementAt(j)));
                if (className != null)
                {
                    referencedClassNames.addElement(className);
                }
            }
            for (int j=0; j<allMethodsIndexes.size(); j++)
            {
                Vector classNames = parseMethodDescriptor(
                                        (String)UTF8s.get(allMethodsIndexes.elementAt(j)));
                if (classNames != null)
                {
                    for (int k=0; k<classNames.size(); k++)
                    {
                        referencedClassNames.addElement(
                            classNames.elementAt(k));
                    }
                }
            }
        }

        private int readUnsignedShort()
        {
            return (((buf[index++] & 0xff) << 8) | (buf[index++] & 0xff));
        }

        private int readUnsignedByte()
        {
            return buf[index++];
        }

        private byte readByte()
        {
            return buf[index++];

        }

        private int readInt()
        {
            return (((buf[index++] & 0xff) << 24) | ((buf[index++] & 0xff) << 16) | ((buf[index++] & 0xff) << 8) | (buf[index++] & 0xff));
        }

        private String readUTF()
        {
            int utfLen = readUnsignedShort();
            int cIndex = 0;
            char[] s = new char[utfLen];
            int a;
            int i = 0;

            while (i < utfLen)
            {
                a = (int) buf[index++] & 0xff;
                i++;
                switch (a >> 4)
                {
                case 0:
                case 1:
                case 2:
                case 3:
                case 4:
                case 5:
                case 6:
                case 7:
                    /* 0xxxxxxx*/
                    s[cIndex] = (char)a;
                    cIndex++;
                    break;
                case 12:
                case 13:
                    /* 110x xxxx   10xx xxxx*/
                    s[cIndex] = (char)(((a & 0x1F) << 6) | (buf[index++] & 0x3F));
                    i++;
                    cIndex++;
                    break;
                case 14:
                    /* 1110 xxxx  10xx xxxx  10xx xxxx */
                    s[cIndex] = (char)(((a & 0x0F) << 12) | ((buf[index++] & 0x3F) << 6)  | ((buf[index++] & 0x3F) << 0));
                    i +=2;
                    cIndex++;
                    break;
                }
            }
            return new String(s, 0, cIndex);
        }

        private void readConstantPool()
        {
            // constant pool count
            int constantPoolCount = readUnsignedShort();
            for (int i=1; i<constantPoolCount; i++)
            {
                byte tag = readByte();
                switch (tag)
                {
                case CONSTANT_CLASS_TAG:
                    // name index
                    allClassIndexes.put(new Integer(i),
                                        new Integer(readUnsignedShort()));
                    break;
                case CONSTANT_FIELDREF_TAG:
                case CONSTANT_METHODREF_TAG:
                case CONSTANT_INTERFACEMETHODREF_TAG:
                case CONSTANT_NAMEANDTYPE_TAG:
                    // name index
                    readUnsignedShort();
                    // descriptor index
                    readUnsignedShort();
                    break;
                case CONSTANT_STRING_TAG:
                    // string index
                    readUnsignedShort();
                    break;
                case CONSTANT_INTEGER_TAG:
                case CONSTANT_FLOAT_TAG:
                    // bytes
                    readInt();
                    break;
                case CONSTANT_LONG_TAG:
                case CONSTANT_DOUBLE_TAG:
                    // high bytes
                    readInt();
                    // low bytes
                    readInt();
                    // a long/double constant takes two entries in the
                    // constant pool (see chapter 4.4.5 from
                    // "The Java Virtual Machine Specification" for details)
                    // -> increase the current index
                    i++;
                    break;
                case CONSTANT_UTF8_TAG:
                    String utf8 = readUTF();
                    UTF8s.put(new Integer(i), utf8);
                    break;
                }
            }
        }

        private void readThisAndSuperClass()
        {
            // this class
            int myClassIndex = readUnsignedShort();
            // super class
            int superClassIndex = readUnsignedShort();
            Object myClassNameIndex = allClassIndexes.get(
                                          new Integer(myClassIndex));
            Object superClassNameIndex = allClassIndexes.get(
                                             new Integer(superClassIndex));
            if (myClassNameIndex == null || superClassNameIndex == null)
            {
                // invalid class file
                throw new InstallerSecurityException(
                    InstallerErrorMessage.INST_CORRUPT_PKG,
                    null, /* no params for short msg */
                    InstallerDetailedErrorMessage.INTERNAL_ERROR,
                    new String[] {"The class " + javaClassFileName
                                  + " has invalid format."
                                 },
                    OtaStatusCode.INTERNAL_ERROR);
            }
            myClassName = (String)UTF8s.get(myClassNameIndex);
            superClassName = (String)UTF8s.get(superClassNameIndex);
        }

        private void readInterfaces()
        {
            skipLengthBasedInfo(1);
        }

        private void readFields()
        {
            int fieldsCount = readUnsignedShort();
            for (int i=0; i<fieldsCount; i++)
            {
                // access flags
                readUnsignedShort();
                // name_index
                readUnsignedShort();
                // descriptor_index
                allFieldsIndexes.addElement(new Integer(
                                                readUnsignedShort()));
                // field attributes
                int fieldAttributesCount = readUnsignedShort();
                for (int j=0; j<fieldAttributesCount; j++)
                {
                    // attribute_name_index
                    readUnsignedShort();
                    // attribute_length
                    skipLengthBasedInfo();
                }
            }
        }

        private void readMethods()
        {
            int methodsCount = readUnsignedShort();
            for (int i=0; i<methodsCount; i++)
            {
                // access flags
                readUnsignedShort();
                // name_index
                int nameIndex = readUnsignedShort();
                // descriptor_index
                int descriptorIndex = readUnsignedShort();
                allMethodsIndexes.addElement(new Integer(descriptorIndex));
                // attributes
                int methodAttributesCount = readUnsignedShort();
                for (int j=0; j<methodAttributesCount; j++)
                {
                    // attribute_name_index
                    String attrName = (String)UTF8s.get(new Integer(
                                                            readUnsignedShort()));
                    if (CODE_ATTRIBUTE_NAME.equals(attrName))
                    {
                        // attribute_length
                        readInt();
                        // max_stack
                        readUnsignedShort();
                        // max_locals
                        readUnsignedShort();
                        // code_length
                        int codeLength = readInt();
                        // skip code[code_length]
                        for (int k=0; k<codeLength; k++)
                        {
                            readUnsignedByte();
                        }
                        // Add method to myMethods table.
                        String methodName = (String)UTF8s.get(
                                                new Integer(nameIndex));
                        String methodDescriptor = (String)UTF8s.get(
                                                      new Integer(descriptorIndex));
                        myMethods.addElement(new MethodInfo(methodName,
                                                            methodDescriptor,
                                                            codeLength));
                        // exception table length
                        skipLengthBasedInfo(4 /* start_pc, end_pc, handler_pc, catch_type*/);
                        // attributes_count
                        int codeAttributesCount = readUnsignedShort();
                        for (int k=0; k<codeAttributesCount; k++)
                        {
                            attrName = (String)UTF8s.get(
                                           new Integer(readUnsignedShort()));
                            if (LOCAL_VARIABLE_TABLE_ATTRIBUTE_NAME
                                    .equals(attrName))
                            {
                                // attribute_length
                                readInt();
                                skipLengthBasedInfo(5 /* start_pc, length, name_index, descriptor_index, index */);
                            }
                            else if (LINE_NUMBER_TABLE_ATTRIBUTE_NAME
                                     .equals(attrName))
                            {
                                // attribute_length
                                readInt();
                                skipLengthBasedInfo(2 /* start_pc, line_number */);
                            }
                            else
                            {
                                skipLengthBasedInfo();
                            }
                        }
                    }
                    else
                    {
                        skipLengthBasedInfo();
                    }
                }
            }
        }

        private void readAttributes()
        {
            // Note: going through attributes could be skipped since
            // the attributes could not bring new referenced classes
            // (=all the referenced classes have been identified already)
            int attributesCount = readUnsignedShort();
            for (int i=0; i<attributesCount; i++)
            {
                // attribute_name_index
                String attrName = (String)UTF8s.get(
                                      new Integer(readUnsignedShort()));
                if (SOURCE_FILE_ATTRIBUTE_NAME.equals(attrName))
                {
                    // attribute_length
                    readInt();
                    // source_file_index
                    readUnsignedShort();
                }
                else if (DEPRECATED_ATTRIBUTE_NAME.equals(attrName))
                {
                    // attribute_length
                    readInt();
                    // nothing to read
                }
                else if (INNER_CLASSES_ATTRIBUTE_NAME.equals(attrName))
                {
                    // attribute_length
                    readInt();
                    skipLengthBasedInfo(4 /* inner_class_info_index, outer_class_info_index, inner_name_index, inner_class_access_flags*/);
                }
                else
                {
                    skipLengthBasedInfo();
                }
            }
        }

        private void skipLengthBasedInfo(int infoItems)
        {
            int infoLength = readUnsignedShort();
            for (int i=0; i<infoLength; i++)
            {
                for (int j=0; j<infoItems; j++)
                {
                    readUnsignedShort();
                }
            }
        }

        private void skipLengthBasedInfo()
        {
            int infoLength = readInt();
            for (int i=0; i<infoLength; i++)
            {
                readUnsignedByte();
            }
        }

        private String parseFieldDescriptor(String fieldDescriptor)
        {
            if (fieldDescriptor != null
                    && fieldDescriptor.startsWith(OBJECT_TYPE_PREFIX)
                    && fieldDescriptor.endsWith(OBJECT_TYPE_SUFFIX))
            {
                return fieldDescriptor.substring(1,
                                                 fieldDescriptor.length() - 1);
            }
            return null;
        }

        private Vector parseMethodDescriptor(String methodDescriptor)
        {
            Vector classNames = new Vector();
            if (methodDescriptor != null
                    && methodDescriptor.startsWith(METHOD_DESCRIPTOR_PREFIX))
            {
                int index = methodDescriptor.indexOf(METHOD_DESCRIPTOR_SUFFIX);
                if (index != -1)
                {
                    String paramDescriptors = methodDescriptor
                                              .substring(1, index);
                    String returnDescriptor = methodDescriptor
                                              .substring(index+1);
                    // handle the paramDescriptors
                    while (paramDescriptors.length() > 1)
                    {
                        int endIndex = 1;
                        if (paramDescriptors.startsWith(OBJECT_TYPE_PREFIX))
                        {
                            endIndex = paramDescriptors.indexOf(
                                           OBJECT_TYPE_SUFFIX);
                            if (endIndex != -1)
                            {
                                classNames.addElement(paramDescriptors
                                                      .substring(1,endIndex));
                            }
                            else
                            {
                                throw new InstallerSecurityException(
                                    InstallerErrorMessage.INST_CORRUPT_PKG,
                                    null, /* no params for short msg */
                                    InstallerDetailedErrorMessage.INTERNAL_ERROR,
                                    new String[] {"The class "
                                                  + javaClassFileName + " has invalid format"
                                                 },
                                    OtaStatusCode.INTERNAL_ERROR);
                            }
                            endIndex++;
                        }
                        paramDescriptors = paramDescriptors.substring(
                                               endIndex);
                    }
                    // handle the return descriptor
                    String returnClassName = parseFieldDescriptor(
                                                 returnDescriptor);
                    if (returnClassName != null)
                    {
                        classNames.addElement(returnClassName);
                    }
                }
            }
            return classNames;
        }
    }
}
