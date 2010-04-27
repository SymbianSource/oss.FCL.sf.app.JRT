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

/**
 * This class defines the names of the java packages which
 * are protected/restricted
 */
public final class PackageNames
{
    public final static String[] protectedPackages = new String[]
    {
        /* these have been taken from the implementation package */
        "com.nokia.mj.impl.",
        "com.sun.j2mews.xml.rpc.",
        "org.eclipse.ercp.swt.expanded.internal.",
        "org.eclipse.ercp.swt.mobile.internal.",
        "org.eclipse.swt.internal.",
        "org.eclipse.swt.widgets.Internal_PackageSupport",
        "org.eclipse.swt.graphics.Internal_GfxPackageSupport",
        "org.eclipse.ercp.swt.mobile.Internal_MobilePackageSupport",
        "com.nokia.amms.",
        "com.intel.bluetooth.",
        "com.nokia.microedition.",
        "com.nokia.mid.impl.",
        "com.nokia.mid.ui.impl.",
        "com.nokia.satsa.",
        "com.sun.ukit.jaxp.",
        "com.symbian.epoc.",
        "com.symbian.gcf.",
        "com.symbian.j2me.",
        "com.symbian.lcdjava.",
        "com.symbian.midp.",
        "com.symbian.util.",
        "java.util.PropertyPermission",
        "java.lang.RuntimePermission",
        "javax.microedition.io.CommProtocolPermission",
        "javax.microedition.io.DatagramProtocolPermission",
        "javax.microedition.io.FileProtocolPermission",
        "javax.microedition.io.GCFPermission",
        "javax.microedition.io.HttpProtocolPermission",
        "javax.microedition.io.HttpsProtocolPermission",
        "javax.microedition.io.SocketProtocolPermission",
        "javax.microedition.io.SSLProtocolPermission",
        "java.security.AccessControlException",
        "java.security.BasicPermission",
        "java.security.PermissionCollection",
        "java.security.Permission",
        "com.ibm.oti.util.NumberConverter"
    };

    public final static String[] restrictedPackages = new String[]
    {
        "org.eclipse.ercp.swt.mobile.",    //eSWT
        "org.eclipse.swt.browser.",        //eSWT
        "org.eclipse.swt.dnd.",            //eSWT
        "org.eclipse.swt.events.",         //eSWT
        "org.eclipse.swt.graphics.",       //eSWT
        "org.eclipse.swt.layout.",         //eSWT
        "org.eclipse.swt.widgets.",        //eSWT
        "org.eclipse.ercp.swt.midp.",      //eSWT
        "org.w3c.dom.",                    //m2g
        "org.xml.sax.",                     //WebServices
        "com.nokia.mid.activeidle.",       //Nokia interfaces
        "com.nokia.mid.iapinfo.",          //Nokia interfaces
        "com.nokia.mid.newmailled.",       //Nokia interfaces
        "com.nokia.mid.sasf.",             //Nokia interfaces
        "com.nokia.mid.softnotification.", //Nokia interfaces
        "com.nokia.mid.sound.",            //Nokia interfaces
        "com.nokia.mid.ui.",               //Nokia interfaces
        "java.security."                   //SATSA CRYPTO
    };

}
