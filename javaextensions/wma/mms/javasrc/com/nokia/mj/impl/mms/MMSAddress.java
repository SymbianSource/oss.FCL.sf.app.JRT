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


package com.nokia.mj.impl.mms;

import com.nokia.mj.impl.utils.Logger;
/**
 * This class contains static methods for verifying the validity of URL
 * connection strings and MMS Header fields.
 */
public class MMSAddress
{

    private static final char COLON = ':';

    // the maximume length of the MSISDN
    private static final int MAX_MSISDN_LENGTH = 256;

    /**
     * Verifies that the parameter represents a valid mms_url
     * <p>
     * mms_url ::== "mms://" address-part
     * </p>
     * Verifies that the string begins with "mms://", and then calls
     * validateAddress() on the remainder. Check that the address is neither a
     * IPv6, nor a IPv4, nor a alphanumerical short code
     *
     * @param url
     *            the url to check
     * @return true if the parameter represents a valid mms_url
     */
    static public void validateUrl(String url) throws IllegalArgumentException
    {
        Logger.LOG(Logger.EWMA, Logger.EInfo,
                   "+ MMSAddress::validateUrl()");
        if (url != null && url.startsWith(MMS_PREFIX))
        {
            String address = url.substring(MMS_PREFIX.length());
            if (address.length() > 0)
            {
                if (validateAddress(address))
                {
                    // exclude IPv6, IPv4 and alpha-numrical short-code, as they
                    // are not supported
                    if (validateIPv4(address))
                    {
                        throw new IllegalArgumentException(
                            ExceptionStrings.ADDRESS_IPV4);
                    }
                    if (validateIPv6(address))
                    {
                        throw new IllegalArgumentException(
                            ExceptionStrings.ADDRESS_IPV6);
                    }
                    if (validateShortcode(address))
                    {
                        // We don't support alphanumerical shortcode,
                        // but we have to allow adding of such addresses
                        // since it is mandated by the TCK
                        // check if there are some alpha characters,
                        // case in which we throw an exception
                        /*
                         * for (int i=0; i<address.length(); i++) { char
                         * ch=address.charAt(i); if ((ch >= 'A' && ch <= 'Z') ||
                         * (ch >= 'a' && ch <= 'z')) { throw new
                         * IllegalArgumentException("Alphanumerical short-code
                         * not supported"); } }
                         */
                    }
                    String deviceAddress = getDeviceAddress(url);
                    if (deviceAddress != null)
                    {
                        int at = indexOfClear(deviceAddress, '@');
                        if (at == -1)
                        {
                            // it means it is a phone number, case in which
                            // its length can not be bigger than 256
                            if (deviceAddress.length() > MAX_MSISDN_LENGTH)
                            {
                                throw new IllegalArgumentException(
                                    ExceptionStrings.ADDRESS_LENGTH_EXCEEDS);
                            }
                        }
                    }
                    Logger.LOG(Logger.EWMA, Logger.EInfo,
                               "- MMSAddress::validateUrl()");
                    return;
                }
            }
        }
        throw new IllegalArgumentException(ExceptionStrings.INVALID_ADDRESS + url);
    }

    /**
     * Verifies that the supplied string is: ALPHA | DIGIT | "." | "_" ALPHA
     * ::== "A".."Z" | "a".."z" DIGIT ::== "0" | "1" | "2" | "3" | "4" | "5" |
     * "6" | "7" | "8" | "9"
     *
     * @param appIdSeg
     * @return
     */
    static private boolean validateAppIdSegment(String appIdSeg)
    {
        Logger.LOG(Logger.EWMA, Logger.EInfo,
                   "+ MMSAddress::validateAppIdSegment()");
        boolean res = true;

        for (int a = 0; a < appIdSeg.length(); a++)
        {
            char c = appIdSeg.charAt(a);
            if (!((c >= '0' && c <= '9') || (c >= 'A' && c <= 'Z')
                    || (c >= 'a' && c <= 'z') || (c == '.') || (c == '_')))
            {
                res = false;
                break;
            }
        }
        Logger.LOG(Logger.EWMA, Logger.EInfo,
                   "- MMSAddress::validateAppIdSegment()");
        return res;
    }

    /**
     * Verifies that the parameter represents a valid Application ID (WITHOUT
     * the initial colon) application-id ::== ":"
     * [manufacture_domain][1*(package-name)]class-name manufacture_domain ::==
     * 1*applicationID-symbol "." package-name ::== manufacture_domain
     * class-name ::== 1*applicationID-symbol applicationID-symbol ::== ALPHA |
     * DIGIT | "." | "_"
     *
     * Note that application-id is maximum 32 characters
     *
     * @param appId
     * @return
     */
    static public boolean validateAppId(String appId)
    {
        boolean res = true;

        // App ID is maximum 32 characters. Remember, the parameter
        // does not include the initial colon!
        if (appId == null || appId.length() > MAX_APP_ID_LENGTH - 1)
        {
            return false;
        }

        if (!validateAppIdSegment(appId))
        {
            res = false;
        }

        return res;
    }

    /**
     * Validates the contents of a quoted string (without quotes) Based on the
     * following: quoted-string ::== """ *(qtext | qpair) """ qtext ::== <any
     * CHAR excluding """, "\" and CR, including LINEAR WHITE SPACE> qpair ::==
     * "\" ALPHA
     *
     * @param quotedStr
     * @return
     */
    static private boolean validateQuotedStr(String quotedStr)
    {
        Logger.LOG(Logger.EWMA, Logger.EInfo,
                   "+ MMSAddress::validateQuotedStr()");
        boolean res = true;
        for (int a = 0; a < quotedStr.length(); a++)
        {
            char ch = quotedStr.charAt(a);

            // May not contain chars higher than 177, '"', or CR.
            if (ch > 177 || ch == '"' || ch < 0 || ch == 13)
            {
                res = false;
                break;
            }

            // A backslash may only be followed by A..Z or a..z
            if (ch == '\\')
            {
                try
                {
                    char next = quotedStr.charAt(a + 1);
                    if (!(next > 'A' && next < 'Z')
                            || (next > 'a' && next < 'z'))
                    {
                        res = false;
                        break;
                    }
                }
                catch (IndexOutOfBoundsException e)
                {
                    res = false;
                }
            }
        }
        Logger.LOG(Logger.EWMA, Logger.EInfo,
                   "- MMSAddress::validateQuotedStr()");
        return res;
    }

    /**
     * Verifies that the parameter represents a valid atom atom ::== 1 * <any
     * CHAR except SPECIALS, CTL and SPACE>
     *
     * @param atom
     * @return
     */
    static private boolean validateAtom(String atom)
    {
        Logger.LOG(Logger.EWMA, Logger.EInfo,
                   "+ MMSAddress::validateAtom()");
        boolean res = true;
        for (int a = 0; a < atom.length(); a++)
        {
            char ch = atom.charAt(a);
            if (ch == '(' || ch == ')' || ch == '<' || ch == '>' || ch == '@'
                    || ch == ',' || ch == ';' || ch == ':' || ch == '\\'
                    || ch == '"' || ch == '.' || ch == '[' || ch == ']'
                    || ch == ' ')
            {
                res = false;
                break;
            }
        }
        Logger.LOG(Logger.EWMA, Logger.EInfo,
                   "- MMSAddress::validateAtom()");
        return res;
    }

    /**
     * Validates that the parameter represents a word word ::== atom |
     * quoted-string
     *
     * @param word
     * @return
     */
    static private boolean validateWord(String word)
    {
        boolean res = true;

        if (word.charAt(0) == '"')
        {
            // We have a quoted string
            int nextQuote = word.indexOf('"', 1);
            if (nextQuote == -1 || (nextQuote != word.length() - 1))
            {
                return false;
            }

            res = validateQuotedStr(word.substring(1, nextQuote));
        }
        else
        {
            // It's an atom
            res = validateAtom(word);
        }
        return res;
    }

    /**
     * Verifies that the parameter represents a Local-Part: local-part ::== word
     * *("." word)
     *
     * @param localPart
     * @return
     */
    static private boolean validateLocalPart(String localPart)
    {
        Logger.LOG(Logger.EWMA, Logger.EInfo,
                   "+ MMSAddress::validateLocalPart()");
        boolean res = true;
        int dot = indexOfClear(localPart, '.');
        int pos = 0;
        boolean finished = false;

        while (!finished)
        {
            if (dot == -1)
            {
                // If there's no dot forthcoming, we can treat the
                // whole thing from pos forward as the word
                res = validateWord(localPart.substring(pos));
                finished = true;
            }
            else
            {
                // If there is a dot coming up, we treat everything
                // between pos and the dot as a word
                String word = localPart.substring(pos, dot);
                if (!validateWord(word))
                {
                    res = false;
                    break;
                }

                // Skip over the word and the dot
                pos += word.length() + 1;
                dot = indexOfClear(localPart.substring(pos), '.');
            }
        }
        Logger.LOG(Logger.EWMA, Logger.EInfo,
                   "- MMSAddress::validateLocalPart()");
        return res;
    }

    /**
     * Verifies that the parameter represents a subdomain: sub-domain ::==
     * domain-ref | domain-literal domain-ref ::== atom domain-literal ::== "["
     * *(dtext | qpair) "]"
     */
    static private boolean validateSubdomain(String subdomain)
    {
        boolean res = true;
        if (subdomain.length() == 0)
        {
            res = false;
        }

        else if (subdomain.charAt(0) == '[')
        {
            // it's a domain-literal
            if (subdomain.charAt(subdomain.length() - 1) != ']')
            {
                return false;
            }
            String inner = subdomain.substring(1, subdomain.length() - 1);
            for (int a = 0; a < inner.length(); a++)
            {
                char ch = inner.charAt(a);
                if (ch == '[' || ch == ']' || ch == '\\' || ch == '"'
                        || ch == 13)
                {
                    return false;
                }
            }
        }
        else
        {
            // it's a domain-ref (i.e. an atom)
            res = validateAtom(subdomain);
        }
        return res;
    }

    /**
     * Returns the index of the next ch in str, which doesn't fall within
     * quotes, angle brackets or square brackets
     *
     * @param str
     * @param ch
     * @return
     */
    static private int indexOfClear(String str, char ch)
    {
        Logger.LOG(Logger.EWMA, Logger.EInfo,
                   "+ MMSAddress::indexOfClear()");
        int res = -1;
        boolean finished = false;
        boolean inQuotes = false;
        boolean inSquareBrackets = false;
        boolean inAngleBrackets = false;
        int squareBracketsLevel = 0;
        int angleBracketsLevel = 0;
        int pos = 0;

        if (str.length() == 0)
        {
            return res;
        }

        while (!finished)
        {
            char current = str.charAt(pos);

            if (current == ch && !inQuotes && !inSquareBrackets
                    && !inAngleBrackets)
            {
                res = pos;
                finished = true;
            }

            else if (current == '[')
            {
                if (!inQuotes && !inAngleBrackets)
                {
                    inSquareBrackets = true;
                    squareBracketsLevel++;
                }
            }

            else if (current == ']')
            {
                if (!inQuotes && !inAngleBrackets)
                {
                    if (inSquareBrackets)
                    {
                        squareBracketsLevel--;
                        if (squareBracketsLevel == 0)
                        {
                            inSquareBrackets = false;
                        }
                    }
                }
            }

            else if (current == '<')
            {
                if (!inQuotes && !inSquareBrackets)
                {
                    inAngleBrackets = true;
                    angleBracketsLevel++;
                }
            }

            else if (current == '>')
            {
                if (!inQuotes && !inSquareBrackets)
                {
                    if (inAngleBrackets)
                    {
                        angleBracketsLevel--;
                        if (angleBracketsLevel == 0)
                        {
                            inAngleBrackets = false;
                        }
                    }
                }
            }

            else if (current == '"')
            {
                if (!inSquareBrackets && !inAngleBrackets)
                {
                    inQuotes = !inQuotes;
                }
            }

            pos++;
            if (pos == str.length())
            {
                finished = true;
            }
        }
        Logger.LOG(Logger.EWMA, Logger.EInfo,
                   "- MMSAddress::indexOfClear()");
        return res;
    }

    /**
     * Verifies that the parameter represents a domain: domain ::== sub-domain
     * *("." sub-domain)
     *
     * @param domain
     * @return
     */
    static private boolean validateDomain(String domain)
    {
        Logger.LOG(Logger.EWMA, Logger.EInfo,
                   "+ MMSAddress::validateDomain()");
        boolean res = true;
        int dot = domain.indexOf('.');
        int pos = 0;

        // it's a domain-ref (i.e. an atom)
        while ((dot = indexOfClear(domain.substring(pos), '.')) != -1)
        {
            String subdomain = domain.substring(pos, dot + pos);
            res = validateSubdomain(subdomain);
            if (!res)
            {
                return false;
            }
            pos += subdomain.length() + 1;
        }

        // After a dot there is a final subdomain.
        String subdomain = domain.substring(pos);
        res = validateSubdomain(subdomain);
        Logger.LOG(Logger.EWMA, Logger.EInfo,
                   "- MMSAddress::validateDomain()");
        return res;
    }

    /**
     * Verifies that the parameter represents an addr-spec addr-spec ::==
     * local-part "@" domain
     *
     * @param addrSpec
     * @return
     */
    static private boolean validateAddrSpec(String addrSpec)
    {
        Logger.LOG(Logger.EWMA, Logger.EInfo,
                   "+ MMSAddress::validateAddrSpec()");
        boolean res = true;
        int at = addrSpec.indexOf('@');

        res = validateLocalPart(addrSpec.substring(0, at));
        if (res)
        {
            res = validateDomain(addrSpec.substring(at + 1));
        }
        Logger.LOG(Logger.EWMA, Logger.EInfo,
                   "- MMSAddress::validateAddrSpec()");
        return res;
    }

    /**
     * Verifies that the parameter is a valid route: route ::== ("@") domain
     * [1*(","("@"domain))]':'
     *
     * @param route
     * @return
     */
    static private boolean validateRoute(String route)
    {
        boolean res = true;
        int pos = 0;
        int comma;
        String domain;

        // Everything up to the first clear ',' is a Domain
        while ((comma = indexOfClear(route.substring(pos), ',')) != -1)
        {
            if (route.charAt(comma + 1) != '@')
            {
                return false;
            }
            domain = route.substring(pos, pos + comma);
            if (!validateDomain(domain))
            {
                return false;
            }
            pos += domain.length() + 1;
        }
        if (route.charAt(pos) != '@')
        {
            return false;
        }
        domain = route.substring(pos + 1, route.length() - 1);
        res = validateDomain(domain);

        return res;
    }

    /**
     * Verifies that the parameter is a valid mailbox mailbox ::== addr-spec |
     * [phrase] route-addr
     *
     * @param mailbox
     * @return
     */
    static private boolean validateMailbox(String mailbox)
    {
        Logger.LOG(Logger.EWMA, Logger.EInfo,
                   "+ MMSAddress::validateMailbox()");
        boolean res = true;

        int pos = 0;

        int lt = indexOfClear(mailbox, '<');
        if (lt != -1)
        {
            // it's a [phrase] route-addr

            // Must end with '>'
            if (mailbox.charAt(mailbox.length() - 1) != '>')
            {
                return false;
            }

            // The route-addr begins with '<'. Unless it comes right at
            // the start, there must be an optional phrase first.
            // phrase ::== 1*word
            // AMBIGUOUS: 1 or many words. What separates these words??
            // Just treating it as one long word
            if (lt > 0)
            {
                res = validateWord(mailbox.substring(0, lt));
            }

            if (res)
            {
                // Ensure that it's of the form <...>
                String routeAddr = mailbox.substring(lt + 1,
                                                     mailbox.length() - 1);
                // routeAddr ::== "<" [route] addr-spec ">"
                // route ::== ("@" domain) [1*(","("@" domain))]":"
                if (routeAddr.charAt(0) == '@')
                {
                    // there's an optional route at the start.
                    // Routes have to end with ':'
                    int colonIx = indexOfClear(routeAddr, COLON);
                    String route = routeAddr.substring(1, colonIx + 1);
                    res = validateRoute(route);
                    pos += route.length() + 1;
                }

                if (res)
                {
                    res = validateAddrSpec(routeAddr.substring(pos));
                }
            }
        }
        else
        {
            // it's a addr-spec
            res = validateAddrSpec(mailbox);
        }
        Logger.LOG(Logger.EWMA, Logger.EInfo,
                   "- MMSAddress::validateMailbox()");
        return res;
    }

    /**
     * Verifies that the parameter represents an email address: e-mail-address
     * ::== mailbox | group mailbox ::== addr-spec | [phrase] route-addr
     * addr-spec ::==
     *
     * @param email
     * @return
     */
    static private boolean validateEmail(String email)
    {
        Logger.LOG(Logger.EWMA, Logger.EInfo,
                   "+ MMSAddress::validateEmail()");
        boolean res = true;

        if (email.charAt(email.length() - 1) == ';')
        {
            // it's a group
            // group ::== phrase ":" [mailbox *("," mailbox)] ";"
            int colonIx = indexOfClear(email, COLON);

            if (colonIx != -1)
            {
                res = validateWord(email.substring(0, colonIx));

                // is there an optional bit next?
                if (res && email.charAt(colonIx + 1) != ';')
                {
                    int comma;
                    int pos = colonIx + 1;
                    String mailbox;
                    while ((comma = indexOfClear(email.substring(pos), ','))
                            != -1)
                    {
                        mailbox = email.substring(pos, pos + comma);
                        if (!validateMailbox(mailbox))
                        {
                            return false;
                        }
                        pos += mailbox.length() + 1;
                    }

                    // Don't want to send the final ';'
                    mailbox = email.substring(pos, email.length() - 1);
                    res = validateMailbox(mailbox);
                    pos += mailbox.length();

                    // must end with a ';'
                    if (email.charAt(pos) != ';')
                    {
                        res = false;
                    }
                }
            }
            else
            {
                res = false;
            }
        }
        else
        {
            res = validateMailbox(email);
        }
        Logger.LOG(Logger.EWMA, Logger.EInfo,
                   "- MMSAddress::validateEmail()");
        return res;
    }

    /**
     * Verifies that the parameter is a valid IPv4 ipv4 ::== 1*3DIGIT "."
     * 1*3DIGIT "." 1*3DIGIT "." 1*3DIGIT
     *
     * @param ipv4
     * @return
     */
    static private boolean validateIPv4(String ipv4)
    {
        Logger.LOG(Logger.EWMA, Logger.EInfo,
                   "+ MMSAddress::validateIPv4()");
        boolean res = true;
        int pos = 0;
        int val = 0;
        int dot;
        String num;

        // 3x <number> "."
        for (int a = 0; a < 3; a++)
        {
            dot = ipv4.indexOf('.', pos);
            if (dot == -1)
            {
                res = false;
                break;
            }
            num = ipv4.substring(pos, dot);
            try
            {
                val = Integer.parseInt(num);
                if (val < 0 || val > 999)
                {
                    res = false;
                    break;
                }

            }
            catch (NumberFormatException nfe)
            {
                res = false;
                break;
            }

            pos += num.length() + 1;
        }

        if (res)
        {
            num = ipv4.substring(pos);
            try
            {
                val = Integer.parseInt(num);
                if (val < 0 || val > 999)
                {
                    res = false;
                }
            }
            catch (NumberFormatException nfe)
            {
                res = false;
            }
        }
        Logger.LOG(Logger.EWMA, Logger.EInfo,
                   "- MMSAddress::validateIPv4()");
        return res;
    }

    /**
     * Verifies that the parameter is a valid IPv6-Atom ipv6-atom ::==
     * 1*4(DIGIT|HEX-ALPHA) HEX-ALPHA ::== "A".."F"
     *
     * @param ipv6Atom
     * @return
     */
    static private boolean validateIPv6Atom(String ipv6Atom)
    {
        Logger.LOG(Logger.EWMA, Logger.EInfo,
                   "+ MMSAddress::validateIPv6Atom()");
        boolean res = true;
        int len = ipv6Atom.length();
        if (len >= 1 && len <= 4)
        {
            for (int a = 0; a < len; a++)
            {
                char ch = ipv6Atom.charAt(a);

                // Note: Lower case letters NOT allowed according to the spec
                if (!(ch >= '0' && ch <= '9') && !(ch >= 'A' && ch <= 'F'))
                {
                    res = false;
                    break;
                }
            }
        }
        else
        {
            res = false;
        }
        Logger.LOG(Logger.EWMA, Logger.EInfo,
                   "- MMSAddress::validateIPv6Atom()");
        return res;
    }

    /**
     * Verifies that the parameter is a valid IPv6 ipv6 ::== 7*(ipv6-atom ":")
     * ipv6-atom
     *
     * @param ipv6
     * @return
     */
    static private boolean validateIPv6(String ipv6)
    {
        Logger.LOG(Logger.EWMA, Logger.EInfo,
                   "+ MMSAddress::validateIPv6()");
        boolean res = true;

        int pos = 0;
        int colonIx = 0;
        String ipv6Atom;

        for (int a = 0; a < 7; a++)
        {
            colonIx = ipv6.indexOf(COLON, pos);
            if (colonIx == -1)
            {
                res = false;
                break;
            }

            ipv6Atom = ipv6.substring(pos, colonIx);
            if (!validateIPv6Atom(ipv6Atom))
            {
                res = false;
                break;
            }

            pos += ipv6Atom.length() + 1;
        }

        if (res)
        {
            ipv6Atom = ipv6.substring(pos);
            if (!validateIPv6Atom(ipv6Atom))
            {
                res = false;
            }
        }
        Logger.LOG(Logger.EWMA, Logger.EInfo,
                   "- MMSAddress::validateIPv6()");
        return res;
    }

    /**
     * Verifies that the parameter represents a valid shortcode-address
     * shortcode-address ::== shortcode-string shortcode-string ::==
     * 1*(ALPHA|DIGIT)
     *
     * @param shortcode
     * @return
     */
    static private boolean validateShortcode(String shortcode)
    {
        Logger.LOG(Logger.EWMA, Logger.EInfo,
                   "+ MMSAddress::validateShortcode()");
        boolean res = true;
        for (int a = 0; a < shortcode.length(); a++)
        {
            char ch = shortcode.charAt(a);
            if (!(ch >= 'A' && ch <= 'Z') && !(ch >= 'a' && ch <= 'z')
                    && !(ch >= '0' && ch <= '9'))
            {
                res = false;
                break;
            }
        }
        Logger.LOG(Logger.EWMA, Logger.EInfo,
                   "- MMSAddress::validateShortcode()");
        return res;
    }

    /**
     * @return the number of ch in str
     */
    static private int countChar(String str, char ch)
    {
        int count = 0;

        for (int a = 0; a < str.length(); a++)
        {
            if (str.charAt(a) == ch)
            {
                count++;
            }
        }

        return count;
    }

    /**
     * Validates that the parameter represents a valid address-part address-part
     * ::== (e-mail-address | device-address | shortcode-address |
     * application-id)
     *
     * @param addr
     * @return
     */
    static private boolean validateAddress(String addr)
    {
        Logger.LOG(Logger.EWMA, Logger.EInfo,
                   "+ MMSAddress::validateAddress()");
        boolean res = true;

        String theAddr;

        // Start by taking the rightmost colon, and see if there's
        // an appID to the right of it.
        boolean appIdPresent = false;
        int colonIx = addr.lastIndexOf(COLON);

        if (colonIx != -1)
        {

            appIdPresent = validateAppId(addr.substring(colonIx + 1));
            // Count the number of colons. If there are seven - the app ID may
            // appear to be valid,
            // it could actually be part of an IPv6
            if (countChar(addr, COLON) == 7)
            {
                if (validateIPv6(addr))
                {
                    return true;
                }
            }
        }

        if (colonIx == 0)
        {
            return appIdPresent;
        }

        theAddr = appIdPresent ? addr.substring(0, colonIx) : addr;

        int at = indexOfClear(addr, '@');

        // Phone number
        if (addr.charAt(0) == '+')
        {
            try
            {
                long num = Long.parseLong(theAddr.substring(1));
                if (num >= 0)
                {
                    return true;
                }
            }
            catch (NumberFormatException nfe)
            {
            }
            // A string beginning with '+' could still be an Email address
        }
        else
        {
            try
            {
                long num = Long.parseLong(theAddr);
                if (num >= 0)
                {
                    return true;
                }
            }
            catch (NumberFormatException nfe)
            {
            }
        }

        // Email address
        if (at != -1)
        {
            if (!(appIdPresent && (at > colonIx)))
            {
                res = validateEmail(theAddr);
                // we can't have email address and appID in the same time
                if (res && appIdPresent)
                {
                    res = false;
                }
            }
            else
            {
                res = false;
            }
        }
        else
        {
            // Try an IPv4 or IPv6
            int colons = countChar(theAddr, COLON);
            int dots = countChar(theAddr, '.');

            if (dots == 3)
            {
                res = validateIPv4(theAddr);
                // we can't have IPv4 and appID in the same time
                if (res && appIdPresent)
                {
                    res = false;
                }
            }
            else if (colons == 7)
            {
                res = validateIPv6(theAddr);
                // we can't have IPv6 and appID in the same time
                if (res && appIdPresent)
                {
                    res = false;
                }
            }
            else
            {
                res = validateShortcode(theAddr);
                // we can't have shortcode and appID in the same time
                if (res && appIdPresent)
                {
                    res = false;
                }
            }
        }
        Logger.LOG(Logger.EWMA, Logger.EInfo,
                   "- MMSAddress::validateAddress()");
        return res;
    }

    /**
     * Verifies that the parameter represents a valid mms_url
     * <p>
     * mms_url ::== "mms://" address-part
     * </p>
     * Verifies that the string begins with "mms://", and then calls
     * validateAddress() on the remainder. If parameter 2 is false, verifies
     * that the string begins with "//"
     *
     * @param url
     *            the url to check
     * @param checkProtocol
     *            if true, we require the URL to begin "mms://". if false, it
     *            must begin "//"
     * @return true if the parameter represents a valid mms_url
     */
    static public boolean validateUrl(String url, boolean checkProtocol)
    {
        Logger.LOG(Logger.EWMA, Logger.EInfo,
                   "+ MMSAddress::validateUrl()");
        boolean res = true;

        String str;

        if (url == null)
        {
            return false;
        }

        if (checkProtocol)
        {
            str = MMS_PREFIX;
        }
        else
        {
            str = "//";
        }

        if (url.startsWith(str))
        {
            res = validateAddress(url.substring(str.length()));
        }
        else
        {
            res = false;
        }
        Logger.LOG(Logger.EWMA, Logger.EInfo,
                   "- MMSAddress::validateUrl()");
        return res;
    }

    /**
     * Gets the application ID from within the address
     * <p>
     * <b> This method does <i>not</i> verify that the parameter represents a
     * valid URL </b> - it is up to the caller to check this beforehand.
     * </p>
     *
     * @param address
     * @return the application ID if the address contains one, null if it does
     *         not
     */
    static public String getAppIdFromAddress(String address)
    {
        Logger.LOG(Logger.EWMA, Logger.EInfo,
                   "+ MMSAddress::getAppIdFromAddress()");
        String tmpAppID = null;

        // Ensure that address is not an IPv6. If it is, the following
        // algorithm will not return a valid App ID!
        if (address != null && !validateIPv6(address))
        {
            int colonIx = address.lastIndexOf(COLON);
            if (colonIx != -1 && colonIx != address.length() - 1)
            {
                tmpAppID = address.substring(colonIx + 1);
                if (!validateAppId(tmpAppID))
                {
                    tmpAppID = null;
                }
            }
        }
        Logger.LOG(Logger.EWMA, Logger.EInfo,
                   "- MMSAddress::getAppIdFromAddress()");
        return tmpAppID;
    }

    /**
     * Gets the application ID from within the URL. Actually, just passes the
     * URL to getAppIdFromAddress() - the logic is exactly the same.
     * <p>
     * <b> This method does <i>not</i> verify that the parameter represents a
     * valid URL </b> - it is up to the caller to check this beforehand.
     * </p>
     *
     * @param url
     * @return the application ID if the URL contains one, null if it does not
     */
    static public String getAppIdFromUrl(String url)
    {
        return getAppIdFromAddress(url);
    }

    /**
     * Gets the address component of the URL.
     * <p>
     * <b> This method <i>only</i> verifies that the beginning of the URL is
     * valid</b> - it is up to the caller to check the address part.
     * </p>
     *
     * @param url
     * @return the URL's address-part if the URL begins with "//", null if it
     *         does not
     */
    static public String getAddressFromUrl(String url)
    {
        String res = null;
        if (url != null)
        {
            if (url.startsWith("//"))
            {
                res = url.substring(2);
            }
        }
        return res;
    }

    /**
     * Indicates whether the parameter appears to represent a Client or Server
     * mode connection.
     * <p>
     * <b> This method does <i>not</i> verify that the parameter represents a
     * valid URL </b> - it is up to the caller to check this beforehand.
     * </p>
     *
     * @param url
     *            The URL to check
     * @return MmsAddress.SERVER if the URL starts with "//:", MmsAddress.CLIENT
     *         if not
     */
    static public boolean getConnectionMode(String url)
    {
        boolean res = CLIENT;

        if (url != null)
        {
            if (url.startsWith("//:"))
            {
                res = SERVER;
            }
        }

        return res;
    }

    /**
     * Gets the device ID <b> OR </b> email address or shortcode from within the
     * address. This method does <b> NOT </b> verify that the returned component
     * is valid.
     *
     * @param address
     * @return the device address, email address or shortcode if the address
     *         contains one, null if not. Therefore - will always return null if
     *         address represents a server. Also returns null if the address
     *         does not start with "mms://".
     */
    static public String getDeviceAddress(String address)
    {
        Logger.LOG(Logger.EWMA, Logger.EInfo,
                   "+ MMSAddress::getDeviceAddress()");
        String devAddr = null;
        String tmpAddress = address;
        String tmpAppId;

        if (address != null)
        {
            // Ensure it begins with mms://
            if (tmpAddress.startsWith(MMS_PREFIX))
            {
                tmpAddress = tmpAddress.substring(MMS_PREFIX.length());
            }
            else
            {
                return null;
            }

            // Ensure it is not a server
            if (MMSAddress.getConnectionMode(address) == SERVER)
            {
                return null;
            }

            // Check if it is an IPv6 - this will confuse the app ID parser!
            if (validateIPv6(tmpAddress))
            {
                return tmpAddress;
            }

            // Strip off the application ID if present
            int colonIx = tmpAddress.lastIndexOf(COLON);
            if (colonIx != -1 && colonIx != tmpAddress.length() - 1)
            {
                tmpAppId = tmpAddress.substring(colonIx + 1);
                if (validateAppId(tmpAppId))
                {
                    devAddr = tmpAddress.substring(0, colonIx);
                }
                else
                {
                    devAddr = tmpAddress;
                }
            }
            else
            {
                devAddr = tmpAddress;
            }
        }
        Logger.LOG(Logger.EWMA, Logger.EInfo,
                   "- MMSAddress::getDeviceAddress()");
        return devAddr;
    }

    /**
     * Checks whether this address represents a phone number.
     *
     * @param addr
     *            the address item
     * @return <code>true</code> if is a phone number
     */
    public static boolean isPhoneNumber(String addr)
    {
        long phone = -1;

        try
        {
            if (addr.startsWith("+"))
            {
                phone = Long.parseLong(addr.substring(1));
            }
            else
            {
                phone = Long.parseLong(addr);
            }
        }
        catch (NumberFormatException nfe)
        {
            return false; // Address format error
        }
        catch (NullPointerException npe)
        {
            return false;
        }

        return (phone >= 0);
    }

    public static final boolean SERVER = true;

    public static final boolean CLIENT = false;

    public static final String MMS_PREFIX = "mms://";

    public static final int MAX_APP_ID_LENGTH = 32;
}
// End on class MMSAddress
