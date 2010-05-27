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

package com.nokia.mj.tools.security.midp;

import java.util.Vector;
import java.io.IOException;
import java.io.FileInputStream;
import java.util.StringTokenizer;
import java.io.ByteArrayOutputStream;
import java.io.ObjectOutputStream;
import java.io.FileOutputStream;
import java.io.ObjectInputStream;
import java.io.FileInputStream;
import java.io.IOException;
import java.io.File;
import com.nokia.mj.impl.security.midp.authorization.SecurityPolicy;
import com.nokia.mj.impl.security.midp.authorization.SecurityPolicyPermission;
import com.nokia.mj.impl.security.midp.authorization.SecurityPolicyPermissionSettings;
import com.nokia.mj.impl.security.midp.common.UserSecuritySettings;
import com.nokia.mj.impl.security.midp.common.PolicyBasedPermission;
import com.nokia.mj.impl.security.midp.common.MIDPPermission;
import com.nokia.mj.impl.security.midp.common.PermissionMappingTable;

/**
 * Utility class for parsing security policy files from external/txt format
 * into internal format.
 *
 * External Policy Format:
 *
 * policy: domain+;
 * domain: 'domain' Identifier ';' grant+;
 * grant: 'grant' (grant_user | grant_allowed | grant_assigned);
 * grant_allowed: 'allowed' permissions;
 * grant_assigned: 'assigned' permissions;
 * grant_user: 'user' grant_name initial_mode other_modes permissions;
 * grant_name: '"' Identifier '"';
 * permissions: '{' permission+ '}';
 * permission: 'permission' permission_class target_name? target_action? ';';
 * permission_class: Identifier ( '.' Identifier)*;
 * target_name: '"' ( .~('"')+) '"';
 * target_action: '"' ( .~('"')+) '"';
 * initial_mode: interaction_mode;
 * other_modes: (',' interaction_mode)*;
 * interaction_mode: 'blanket' | 'session' | 'oneshot';
 * ALPHA: ('a'..'z'|'A'..'Z');
 * DIGIT: '0'..'9';
 * Identifier: ALPHA (ALPHA | DIGIT)*;
 *
 * @see com.nokia.mj.impl.security.midp.authorization.SecurityPolicy
 */
public final class PolicyEditor
{
    private int offset = 0;
    private byte[] policy;
    private SecurityPolicy securityPolicy;
    private static String destPath;
    private static final String POLICY_FILE_EXTENSION = ".txt";

    public static void main(String[] args)
    {
        if (args.length != 2)
        {
            System.out.println("Usage: SecurityPolicyEditor <sourcePath> <destPath>"
                               + "\n       where <sourcePath> = Path to the directory from where policies in the external format will be read"
                               + "\n             <destPath> = Path to the directory where the policies in the internal format will be stored");
            System.exit(1);
        }
        destPath = args[1];
        File[] files = new File(args[0]).listFiles();
        if (files == null || files.length == 0)
        {
            System.out.println("No files under " + args[0] + ". Security system can not operate without security policy files");
            System.exit(1);
        }
        for (int i=0; i<files.length; i++)
        {
            if (files[i].isFile() && files[i].getName().toLowerCase().endsWith(POLICY_FILE_EXTENSION))
            {
                PolicyEditor editor = new PolicyEditor();
                editor.edit(files[i].getAbsolutePath());
            }
        }
    }

    private void edit(String filePath)
    {
        try
        {
            // read the policy file
            policy = read(filePath);
            // parse it
            parse();
            // flatten the policy object into a file
            serialize(filePath);
        }
        catch (IOException e)
        {
            e.printStackTrace();
            System.exit(1);
        }
    }

    private byte[] read(String filePath)
    throws IOException
    {
        FileInputStream iStream = null;
        ByteArrayOutputStream oStream = null;

        try
        {
            iStream = new FileInputStream(filePath);
            oStream = new ByteArrayOutputStream();

            byte[] buf = new byte[2048];
            int i = 0;
            while ((i = iStream.read(buf)) != -1)
            {
                oStream.write(buf, 0, i);
            }
            return oStream.toByteArray();
        }
        finally
        {
            if (iStream != null)
            {
                iStream.close();
            }
            if (oStream != null)
            {
                oStream.close();
            }
        }
    }

    private void parse()
    {
        String domain = null;
        find("domain",true);
        if (offset >= policy.length)
        {
            // domain not found
            throw new IllegalArgumentException(
                "Invalid security policy - missing domain information");
        }
        domain = extractToken(';');
        Vector perms = new Vector();
        find("grant");
        while (offset < policy.length)
        {
            SecurityPolicyPermissionSettings settings = null;
            int type = getType(extractToken());
            if (type == PolicyBasedPermission.USER_TYPE)
            {
                String settingsName = extractToken('"', '"');
                check(settingsName, "User grant group without name");
                // trim off the leading and trailing double-quotes
                if (settingsName.startsWith("\""))
                {
                    settingsName = settingsName.substring(1);
                }
                if (settingsName.endsWith("\""))
                {
                    settingsName = settingsName.substring(0, settingsName.length() - 1);
                }
                String strInteractionModes = extractToken('{');
                check(strInteractionModes,
                      "User grant group without interaction modes");
                String[] interactionModes = split(strInteractionModes,",");
                if (interactionModes == null || interactionModes.length == 0)
                {
                    throw new IllegalArgumentException(
                        "Missing interaction modes for an user grant group");
                }
                int currentInteractionMode = getInteractionMode(
                                                 interactionModes[0]);
                int[] allowedInteractionModes = getInteractionModes(
                                                    interactionModes);
                settings = new SecurityPolicyPermissionSettings(settingsName,
                        currentInteractionMode, allowedInteractionModes);
            }
            find("permission");
            while (offset < policy.length && policy[offset] != '}')
            {
                String pName = extractToken(';');
                String pTarget = null;
                String pActionList = null;
                if (policy[offset] != ';')
                {
                    pTarget = extractToken(';');
                    // remove the leading and trailing " characters
                    if (pTarget != null && pTarget.length() > 2)
                    {
                        if (pTarget.charAt(0) == '"')
                        {
                            pTarget = pTarget.substring(1);
                        }
                        if (pTarget.charAt(pTarget.length() - 1) == '"')
                        {
                            pTarget = pTarget.substring(0,
                                                        pTarget.length() - 1);
                        }
                    }
                    if (policy[offset] != ';')
                    {
                        pActionList = extractToken(';');
                        // remove the leading and trailing " characters
                        if (pActionList != null && pActionList.length() > 2)
                        {
                            if (pActionList.charAt(0) == '"')
                            {
                                pActionList = pActionList.substring(1);
                            }
                            if (pActionList.charAt(pActionList.length() - 1)
                                    == '"')
                            {
                                pActionList = pActionList.substring(0,
                                                                    pActionList.length() - 1);
                            }
                        }
                    }
                }
                // change the name of the external/public permission into the
                // name of the name of the internal permission
                MIDPPermission internalPermission = PermissionMappingTable.get(
                                                        pName);
                if (internalPermission != null)
                {
                    pName = internalPermission.getName();
                }
                // split the composite action list into simple actions and grant
                // return multiple permissions with simple actions. This step is
                // to be removed when APIs support composite actions
                String[] actions = split(pActionList,",");
                if (actions != null)
                {
                    for (int i=0; i<actions.length; i++)
                    {
                        perms.add(getPermission(pName,
                                                pTarget, actions[i], type, settings));
                    }
                }
                else
                {
                    perms.add(getPermission(pName,
                                            pTarget, pActionList, type, settings));
                }
                find("permission");
            }
            find("grant", true);
        }
        if (perms.size() == 0)
        {
            throw new IllegalArgumentException(
                "Invalid security policy - missing/invalid permissions information");
        }
        SecurityPolicyPermission[] permissions
        = new SecurityPolicyPermission[perms.size()];
        perms.copyInto(permissions);
        securityPolicy = new SecurityPolicy(domain, permissions);
    }

    private void serialize(String filePath)
    throws IOException
    {
        // extract the filename from path
        filePath = new File(filePath).getName();
        // strip off the extension (if any)
        int endIndex = filePath.indexOf(".");
        if (endIndex == -1)
        {
            endIndex = filePath.length();
        }
        filePath = filePath.substring(0, endIndex);
        FileOutputStream oStream = new FileOutputStream(
            destPath
            + File.separator
            + filePath
            + ".ser");
        ObjectOutputStream out = new ObjectOutputStream(oStream);
        out.writeObject(securityPolicy);
        out.close();
    }

    private void find(String str, boolean ignoreEndSep)
    {
        byte[] bytes = str.getBytes();
        while (true)
        {
            if (offset >= policy.length
                    || (!ignoreEndSep && policy[offset] == '}'))
            {
                break;
            }
            if (policy[offset] == bytes[0])
            {
                offset++;
                int i = 1;
                while (i < bytes.length && policy[offset] == bytes[i])
                {
                    offset++;
                    i++;
                }
                if (i == bytes.length)
                {
                    break;
                }
            }
            offset++;
        }
    }

    private String extractToken(char startSep, char endSep, char extraSep)
    {
        String token = "";
        // skip spaces
        while (startSep != ' ' && offset < policy.length && policy[offset] == ' ')
        {
            offset++;
        }
        // skip everything until we hit the startSep
        while (offset < policy.length && policy[offset] == startSep)
        {
            offset++;
        }
        while (offset < policy.length && policy[offset] != endSep
                && policy[offset] != '\n' && policy[offset] != '\r'
                && (extraSep == Character.MAX_VALUE || policy[offset] != extraSep))
        {
            token = token + (char)policy[offset];
            offset++;
        }
        if (token.length() == 0)
        {
            return null;
        }
        if (endSep != ' ')
        {
            // skip the end separator
            offset++;
        }
        return token;
    }

    private int getInteractionMode(String interactionMode)
    {
        if (interactionMode.equalsIgnoreCase("oneshot"))
        {
            return UserSecuritySettings.ONESHOT_INTERACTION_MODE;
        }
        else if (interactionMode.equalsIgnoreCase("session"))
        {
            return UserSecuritySettings.SESSION_INTERACTION_MODE;
        }
        else if (interactionMode.equalsIgnoreCase("blanket"))
        {
            return UserSecuritySettings.BLANKET_INTERACTION_MODE;
        }
        else if (interactionMode.equalsIgnoreCase("no"))
        {
            return UserSecuritySettings.NO_INTERACTION_MODE;
        }
        else
        {
            throw new IllegalArgumentException(
                "Invalid interaction mode "+interactionMode);
        }
    }

    private int[] getInteractionModes(String[] interactionModes)
    {
        int[] intModes = new int[interactionModes.length];
        for (int i=0; i<interactionModes.length; i++)
        {
            intModes[i] = getInteractionMode(interactionModes[i]);
        }
        return intModes;
    }

    private int getType(String sType)
    {
        if (sType.equalsIgnoreCase("user"))
        {
            return PolicyBasedPermission.USER_TYPE;
        }
        else if (sType.equalsIgnoreCase("allowed"))
        {
            return PolicyBasedPermission.ALLOWED_TYPE;
        }
        else if (sType.equalsIgnoreCase("assigned"))
        {
            return PolicyBasedPermission.ASSIGNED_TYPE;
        }
        else
        {
            throw new IllegalArgumentException(
                "Invalid grant group type "+sType);
        }
    }

    private void find(String str)
    {
        find(str, false);
    }

    private String extractToken()
    {
        return extractToken(' ' , ' ' , Character.MAX_VALUE);
    }

    private String extractToken(char extraSep)
    {
        return extractToken(' ' , ' ' , extraSep);
    }

    private String extractToken(char startSep, char endSep)
    {
        return extractToken(startSep , endSep , Character.MAX_VALUE);
    }

    private void check(String str, String errMsg)
    {
        if (str == null || str.length() == 0)
        {
            throw new IllegalArgumentException(errMsg);
        }
    }

    private String[] split(String str, String separator)
    {
        if (str == null)
        {
            return null;
        }
        if (separator == null || separator.length() == 0)
        {
            return new String[] {str};
        }
        Vector tokens = new Vector();
        int index = str.indexOf(separator);
        while (index != -1)
        {
            tokens.addElement(str.substring(0,index));
            str = str.substring(index + separator.length());
            index = str.indexOf(separator);
        }
        tokens.addElement(str);
        String[] strTokens = new String[tokens.size()];
        tokens.copyInto(strTokens);
        return strTokens;
    }

    private SecurityPolicyPermission getPermission(String pName,
            String pTarget,
            String pAction,
            int pType,
            SecurityPolicyPermissionSettings pSettings)
    {
        if (pSettings == null)
        {
            return new SecurityPolicyPermission(
                       pName, pTarget, pAction, pType);
        }
        else
        {
            return new SecurityPolicyPermission(
                       pName, pTarget, pAction, pSettings);
        }
    }
}
