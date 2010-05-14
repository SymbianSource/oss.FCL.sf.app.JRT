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

package com.nokia.mj.impl.rt.support;

import com.nokia.mj.impl.utils.Uid;

/**
 * ApplicationInfo is an abstract base class for API implementations,
 * which allows getting application related static information.
 * <p>
 * The class provides a static getter method for getting an instance of
 * the running application. In those runtimes that are always running only
 * one instance (like MIDP) returns always the same instance of the
 * ApplicationInfo. However those runtimes (like eRCP), that may run several
 * application in the same JVM instance, will return an application specific
 * instance.
 * <p>
 * The runtime implementation hides the solving of the correct application
 * when method <code>getInstace</code> is called. However it must be noted
 * that if the call is made some common thread, the <code>getInstace</code>
 * might not able to solve the correct application.
 * <p>
 * Example how to get the name of the application:
 * <pre>
 * package com.nokia.mj.impl.mypackage;
 *
 * import com.nokia.mj.impl.rt.support;
 *      public void myClass()
 *      {
 *          public void myMethod()
 *          {
 *              //Get the name of the application.
 *              String appName = ApplicationInfo.getInstance().getName();
 *
 *              //my implementation...
 *          }
 *      }
 *</pre>
 * <p>
 * Example how to reuse the application object:
 * <pre>
 * package com.nokia.mj.impl.mypackage;
 *
 * import com.nokia.mj.impl.rt.support;
 *      public void myClass2()
 *      {
 *          public void myMethod()
 *          {
 *              //Get the insatnce of ApplicationInfo.
 *              ApplicationInfo appInfo = ApplicationInfo.getInstance();
 *
 *              //Get the name of the application.
 *              String appName = appInfo.getName();
 *
 *              //Get the vendor of the application.
 *              String appVendor = appInfo.getVendor();
 *
 *              //my implementation...
 *          }
 *      }
 *</pre>
 * @author Nokia Corporation
 * @version 1.0
 */

public class ApplicationInfo
{
    public ApplicationInfo()
    {
    }

    /**
     * A constant representing the manufacturer domain when returned from the
     * method {@link ApplicationInfo#getProtectionDomain
     * getProtectionDomain}
     */
    public static final String MANUFACTURER_DOMAIN = "MFD";

    /**
     * A constant representing the identified third party domain when returned
     * from the method {@link ApplicationInfo#getProtectionDomain
     * getProtectionDomain}
     */
    public static final String IDENTIFIED_THIRD_PARTY_DOMAIN = "ITPD";

    /**
     * A constant representing the operator domain when returned from the
     * method {@link ApplicationInfo#getProtectionDomain getProtectionDomain}
     */
    public static final String OPERATOR_DOMAIN = "OPD";

    /**
     * A constant representing the unidentified third party domain when
     * returned from the method {@link ApplicationInfo#getProtectionDomain
     * getProtectionDomain}
     */
    public static final String UNIDENTIFIED_THIRD_PARTY_DOMAIN = "UTPD";

    /**
     * A constant representing the MIDP runtime when
     * returned from the method {@link ApplicationInfo#getRuntimeType
     * getRuntimeType}
     */
    public static final String RUNTIME_MIDP = "MIDP";

    /**
     * A constant representing the non managed runtime when
     * returned from the method {@link ApplicationInfo#getRuntimeType
     * getRuntimeType}.
     */
    public static final String RUNTIME_NON_MANAGED = "NON_MANAGED";

    /**
     * A constant representing the eRCP runtime when
     * returned from the method {@link ApplicationInfo#getRuntimeType
     * getRuntimeType}.
     */
    public static final String RUNTIME_ERCP = "ERCP";



    /**
     * Property name of the runtime implementation package end. It
     * should be noted that ApplicationUtils class uses the same
     * property for similar purposes as ApplicationInfo class does.
     */
    static final String PACKAGE_END_PROPERTY_NAME = "com.nokia.rt.port";

    /**
     * Runtime implmentation package prefix. It should be noted that
     * ApplicationUtils class uses the same property for similar purposes
     * as ApplicationInfo class does.
     */
    static final String PACKAGE_PREFIX = "com.nokia.mj.impl.rt.";

    /**
     * Name of the runtime specific class that extends this base class.
     */
    private static final String PORTCLASS = ".ApplicationInfoImpl";

    /**
     * A singleton instance of runtime dependent instance of class extending
     * the ApplicationInfo base class.
     */
    private static ApplicationInfo sInstance = null;

    /**
     * When the class is first loaded the static initializer will create a
     * singleton instance of runtime dependent instance of class extending the
     * ApplicationInfo base class. The class is created using dynamic
     * instantiation (<code>Class.forname & Class.newInstance</code>) and
     * the name of the class is a combination of PACKAGE_PREFIX +
     * &lt;value of system property PACKAGE_END_PROPERTY_NAME &gt;
     * + .&lt;value of PORTCLASS member variable &gt.
     */
    static
    {
        //Get the system proptery defining the end of package.
        /*String packageEnd = System.getProperty(PACKAGE_END_PROPERTY_NAME);

        if(packageEnd == null)
        {
            throw new RuntimeException
                            ("Not able to load " + PORTCLASS + "class since " +
                            PACKAGE_END_PROPERTY_NAME +
                            " system property is undefined");
        }

        //Construct the class
        String className = PACKAGE_PREFIX + packageEnd + PORTCLASS;
        try
        {
            Class clazz = Class.forName(className);
            sInstance = (ApplicationInfo)clazz.newInstance();
        }
        catch (Exception e)
        {
            e.printStackTrace();
            throw new RuntimeException ("Not able to instantiate class " +
                                         className+". Reason is: " + e);
        }*/
        sInstance = new ApplicationInfo();
    }

    /**
     * Returns an instace of ApplicationInfo class which provides access to
     * application related static information.
     *
     * @return the ApplicationInfo object, null if the construction of the
     *         object has faild
     */
    public static ApplicationInfo getInstance()
    {
        return sInstance;
    }

    /**
     * Returns the type of the runtime where in the application is run. There
     * are three predefined runtime types that are represented by constants
     *
     * @see #RUNTIME_MIDP
     * @see #RUNTIME_NON_MANAGED
     * @see #RUNTIME_ERCP
     *
     *
     * @return the runtime type as String. The return value should never be
     * null
     */
    //public abstract String getRuntimeType();


    /**
     * Returns the protection domain of the application. There are four predefined
     * protection domains that are represented by constants
     *
     * @see #MANUFACTURER_DOMAIN
     * @see #IDENTIFIED_THIRD_PARTY_DOMAIN
     * @see #OPERATOR_DOMAIN
     * @see #UNIDENTIFIED_THIRD_PARTY_DOMAIN
     *
     * @return protection domain as String
     */
    //public abstract String getProtectionDomain();

    /**
     * Returns an unique application identifier.
     *
     * @return the Uid. The return value should never be null
     */
    public Uid getUid()
    {
        return Uid.createUid("OpenLCDUI_Test");
    }

    /**
     * Returns a unique application suite identifier. The suite concept may be
     * different in different runtimes and some of them might not support the
     * concept at all.
     * <p>
     * Note: Please consider carefully if GetUid() would be enough for your
     * purposes, earlier suite UIDs have been used extensively just for nothing.
     * The OMJ code should use this method as little as feasibly possible.
     * <p>
     * @return the suite Uid if availabe, otherwise null.
     */
    //public abstract Uid getSuiteUid();

    /**
     * Returns the suite name of the application. The suite concept may be
     * different in different runtimes and some of them migth not support the
     * concept at all.
     * <p>
     * In MIDP this method returns the name of the MIDlet Suite.
     *
     *
     * @return Suite name as String if availabe, otherwise null.
     */
    public String getSuiteName()
    {
        return null;
    }

    /**
     * Returns the name of the application. Some runtime implmentations
     * might not return the value at all.
     * <p>
     * In MIDP this method returns the name of the MIDlet.
     *
     * @return application name as String if availabe, otherwise null.
     */
    public String getName()
    {
        return null;
    }

    /**
     * Returns the vendor of the application. Some runtime implmentations
     * might not return the value at all.
     *
     * @return application vendor as String if availabe, otherwise null.
     */
    public String getVendor()
    {
        return null;
    }

    /**
     * Returns the version of the application. The format of the version is
     * <code>&lt;main&gt;.&lt;major&gt.&lt;minor&gt</code> where
     * <code>main</code>, <code>major</code> and <code>minor</code> are in
     * decimal format.
     * <p>
     * Some runtime implmentations might not return the value at all.
     * @return application version as String if availabe, otherwise null.
     */
    public String getVersion()
    {
        return null;
    }

    /**
     * Returns the root path of the application. The root path means an
     * application specific directory where the different API implementations
     * are able to write their files. No other applications are able access
     * files under the root path.
     * <p>
     * API implemetations must not rely that the JAR file(s) or descriptor file
     * can always be found from the root path even if they sometimes might be
     * found.
     * @return application root path as String
     */
    //public abstract String getRootPath();

    /**
     * Returns the main class of the application. The main class concept might
     * differ between different runtimes. E.g. the in MIDP this method returns
     * the class that extends the javax.microedition.MIDlet class.
     * <p>
     * Currently only known user for this is MIDP push.
     *
     * @return application main class as String
     */
    //public abstract String getMainClass();

    /**
     * Returns the attribute whose name is given as parameter. The attribute
     * value is obtained from the JAR manifest file and/or some other
     * descriptor (e.g. JAD file) using the runtime specific override rules.
     *
     * @param key Attribute key value.
     *
     * @return the attribute value as String if found, otherwise null.
     */
    public String getAttribute(String key)
    {
        return null;
    }
}
