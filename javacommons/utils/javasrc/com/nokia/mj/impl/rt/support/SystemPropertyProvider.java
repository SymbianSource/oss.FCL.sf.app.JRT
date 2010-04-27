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

/**
 * This interface is used by the system property extension mechanism in
 * order to ask the value of certain dynamic system property. The component
 * that provides this information must implement this interface in a package
 * <code>com.nokia.mj.impl.properties + &lt;jsr_package&gt;</code>. The name
 * of the class can be anything, but must be the same as defined in the value
 * of the dynamic system property. E.g. <code>fileconn.dir.graphics.name</code>
 * dynamic property value is set to :fileapi.DynamicPropertyHandler. Then the
 * name of the
 * class providing the value of <code>fileconn.dir.graphics.name</code>
 * property must be
 *<code>com.nokia.mj.impl.properties.file.DynamicPropertyHandler</code>.
 * <p>
 * If the dynamic system property implementation implements more than one
 * system property in a single class, the implementation must not make any
 * assumption wheter a new instance is always created or does the underlying
 * extemsion mechanism cache the created instance and re-use it.
 * <p>
 * Example how to implement a class providing some dynamic system property.
 * <pre>
 * package com.nokia.mj.impl.properties.file;
 *
 * import com.nokia.mj.impl.rt.support.SystemPropertyProvider;
 * public final class DynamicPropertyHandler implements SystemPropertyProvider
 * {
 *     public String getProperty(String key)
 *     {
 *         if(key.equals("fileconn.dir.graphics"))
 *         {
 *             return FileImpl.getDirGraphics();
 *         }
 *         else if(key.equals("fileconn.dir.graphics.name"))
 *         {
 *             return FileImpl.getDirGraphicsName();
 *         }
 *         else
 *         {
 *             return null;
 *         }
 *     }
 *
 *     public boolean isStatic(String key) {
 *         //All of the example file API dynamic properties are static
 *         return true;
 *     }
 * }
 * </pre>
 */
public interface SystemPropertyProvider
{
    /**
     * Gets the system property indicated by the specified key.
     * Called by the system property extension mechanism as
     * a result of <code>System.getProperty</code>
     * (or <code>System.getProperties</code>) call.
     *
     * @param key - the name of the system property.
     * @return the string value of the system property, or null if there is
     *         no property with that key.
     */
    public String getProperty(String key);

    /**
     * Returns the information whether the system property does
     * not change during the lifetime of the application.
     * <p>
     * Some detailed information how the underlying extension mechanism
     * works. Eventually all the static and dynamic properties are in a
     * Hashtable. The value of the property in the Hashtable tells whether
     * the property is static or dynamic.
     * If the value is static, it is returned to as it is. If the value is
     * dynamic, then a class providing the the dynamic value is created (or
     * previosly created instance might be used). Once the value has been
     * received from the object, the extension mechanism asks if the value
     * is now static using this method. If the value is static the value
     * is stored into the
     * Hashtable and if asked again the value is returned directly from the
     * Hashtable. If it is not a static the value in the HashTable remains
     * the same and if the system property is asked again, the described
     * scenario above happens again.
     *
     * @return true if value is static, false otherwise.
     */
    public boolean isStatic(String key);
}
