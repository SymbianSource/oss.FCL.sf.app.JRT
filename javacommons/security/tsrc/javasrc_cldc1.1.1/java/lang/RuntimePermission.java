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

package java.lang;

import java.security.BasicPermission;

/**
 * This class is for runtime permissions. A RuntimePermission contains a name
 * (also referred to as a "target name") but no actions list; you either have
 * the named permission or you don't.
 * <p>
 * The target name is the name of the runtime permission (see below). The
 * naming convention follows the hierarchical property naming convention.
 * Also, an asterisk may appear at the end of the name, following a ".", or by
 * itself, to signify a wildcard match. For example: "target.*" or "*" is
 * valid, "*target" or "a*b" is not valid.
 * <p>
 * The following table lists all the possible RuntimePermission target names,
 * and for each provides a description of what the permission allows and a
 * discussion of the risks of granting code the permission.
 *
 * <table>
 *
 * <tr>
 * <th>Permission Target Name</th>
 * <th>What the Permission Allows</th>
 * <th>Risks of Allowing this Permission</th>
 * </tr>
 *
 * <tr>
 *   <td>exitVM</td>
 *   <td>Halting of the Java Virtual Machine</td>
 *   <td>This allows an attacker to mount a denial-of-service attack by
 * automatically forcing the virtual machine to halt.</td>
 * </tr>
 *
 * <tr>
 *   <td>modifyThread</td>
 *   <td>Modification of threads, e.g., via calls to Thread
 * <code>interrupt</code> and <code>setPriority</code> methods</td>
 * </tr>
 *
 * </table>
 *
 * @see BasicPermission
 * @see Permission
 * @see PermissionCollection
 *
 * @author Nokia Corporation
 * @version 1.0
 */
public final class RuntimePermission
        extends BasicPermission
{
    /**
     * Creates a new RuntimePermission with the specified name. The name is
     * the symbolic name of the RuntimePermission, such as "exitVM",
     * "modifyThread", etc. An asterisk may appear at the end of the name,
     * following a ".", or by itself, to signify a wildcard match.
     *
     * @param name the name of the RuntimePermission.
     * @throws NullPointerException if name is null.
     * @throws IllegalArgumentException if name is empty.
     */
    public RuntimePermission(String name)
    {
        super(name);
    }

    /**
     * Creates a new RuntimePermission object with the specified name. The name
     * is the symbolic name of the RuntimePermission, and the actions String is
     * currently unused and should be null.
     *
     * @param name the name of the RuntimePermission.
     * @param actions should be null.
     * @throws NullPointerException if name is null.
     * @throws IllegalArgumentException if name is empty.
     */
    public RuntimePermission(String name, String actions)
    {
        super(name);
    }
}
