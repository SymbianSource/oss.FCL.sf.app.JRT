/*
* Copyright (c) 2000-2006 Nokia Corporation and/or its subsidiary(-ies).
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

/**
 * @file
 * This file specifies any error codes specific to Java MIDP installation
 *
 * @internalTechnology
 */
#ifndef JAVAERROR_H
#define JAVAERROR_H

//////////////////////////////////////////////////////////////////////////////
// Errors produced by JavaReader
//////////////////////////////////////////////////////////////////////////////

/** The manifest is invalid */
const TInt KJavaErrInvalidManifest=-10000;
/** A line in the manifest exceeds 72 characters */
const TInt KJavaErrLineTooLong=-10001;
/** The manifest file is missing or blank */
const TInt KJavaErrMissingManifest=-10002;
/** One of the valus in the manifest is not a UTF8 value */
const TInt KJavaErrNoneUTF8=-10003;
/** The MIDlet-Name attribute is missing from manifest */
const TInt KJavaErrMissingName=-10004;
/** The MIDlet-Vendor attribute is missing from manifest */
const TInt KJavaErrMissingVendor=-10005;
/** The MIDlet-Version attribute is missing from manifest */
const TInt KJavaErrMissingVersion=-10006;
/** The MIDlet-Version attribute is invalid should be #.#[.#] where # is 0..99   */
const TInt KJavaErrInvalidVersion=-10007;
/** The MicroEditionProfile attribute is missing from manifest */
const TInt KJavaErrMissingMicroEditionProfile=-10008;
/** The MicroEditionConfiguration attribute is missing from manifest */
const TInt KJavaErrMissingMicroEditionConfiguration=-10009;
/** The MIDlet-Jar-Size attribute is missing */
const TInt KJavaErrMissingJarSize=-10010;
/** The MIDlet-Jar-Size attribute is invalid */
const TInt KJavaErrInvalidJarSize=-10011;
/** The MIDlet-Jar-URL attribute is missing  */
const TInt KJavaErrMissingJarURL=-10012;
/** The MIDlet-Data-Size attribute is invalid */
const TInt KJavaErrInvalidDataSize=-10013;
/** The jad is invalid */
const TInt KJavaErrInvalidJad=-10014;
/** The size of the jar is not what is announced in the jad */
const TInt KJavaErrJarSizeMismatch=-10015;
/** There is a mismatch between the value of an attribute in the jad and the jar */
const TInt KJavaErrAttributeMismatch=-10016;
/** The number of MIDlets in the suite exceeds the system maximum */
const TInt KJavaErrTooManyMIDlets=-10017;
/** The name of MIDlet-<n> attribute is missing */
const TInt KJavaErrMidletNameMissing=-10018;
/** The class name of MIDlet-<n> attribute is missing */
const TInt KJavaErrMidletClassNameMissing=-10019;

//////////////////////////////////////////////////////////////////////////////
// Errors produced by JavaReader (JAD/JAR Manifest parse errors)
//////////////////////////////////////////////////////////////////////////////
/// Successful parse with no error is KErrNone
/** Internal error in parser */
const TInt KJavaParseJarMetaFileParserError     = -10700;
/** Manifest-Version attribute missing */
const TInt KJavaParseVersionMissing             = -10701;
/** @deprecated */
const TInt KJavaParseVersionMismatch            = -10702;
/** Lines in JAD / JAR > 72 characters are accepted, so this error is not generated */
const TInt KJavaParseLineTooLong                = -10703;
/** The name of an attribute exceeds maximum */
const TInt KJavaParseNameTooLong                = -10704;
/** Invalid character in file */
const TInt KJavaParseInvalidCharacter           = -10705;
/** Unexpected blank line. */
const TInt KJavaParseUnexpectedBlankLine        = -10706;
/** Unexpected continuation of a line. */
const TInt KJavaParseUnexpectedContinuationLine = -10707;
/** Badly formed attribute e.g. no value, colon etc. */
const TInt KJavaParseMalformedHeader            = -10708;
/** Name of attrubute does not conform to standard */
const TInt KJavaParseInvalidAttributeName       = -10709;
/** Attribute Name: found in manifest manin section */
const TInt KJavaParseUnexpectedNameAttribute    = -10710;
/** Attribute Name: not present in individual section */
const TInt KJavaParseNameAttributeMissing       = -10711;
/** File has been parsed (not an error). */
const TInt KJavaParseEndOfFileReached           = -10712;
/** MIDlet-Install-Notify attribute value exceeds maximum length */
const TInt KJavaParseNotifyUrlTooLong           = -10713;
/** MIDlet-Delete-Confirm attribute value exceeds maximum length */
const TInt KJavaParseDeleteUrlTooLong           = -10714;
/** JAD or JAR have not yet been parsed. */
const TInt KJavaParseErrNotParsed               = -10715;

//////////////////////////////////////////////////////////////////////////////
// Errors produced by JavaInstaller
//////////////////////////////////////////////////////////////////////////////

/** Upgrade from signed to unsigned not allowed */
const TInt KJavaInstUnsignedReplacesSigned = -10500;
/** Cannot upgrade since midlet is running */
const TInt KJavaInstMIDletRunning = -10501;
/** Error downloading Jar */
const TInt KJavaInstJarDownloadError = -10502;
/** Error converting icons */
const TInt KJavaInstIconConversionError = -10503;
/** JAD file is invalid */
const TInt KJavaInstInvalidJad = -10504;
/** JAR file is invalid */
const TInt KJavaInstInvalidJar = -10505;
/** Insufficient space to install on device */
const TInt KJavaInstInsufficientSpace = -10506;
/** JAR download location is invalid */
const TInt KJavaInstBadDownloadLocation = -10507;
/** Java version required by MIDlet not available on device */
const TInt KJavaInstIncompatibleJavaVersion = -10508;
/** Could not authenticate with AMS server */
const TInt KJavaInstAMSAuthenticationFailure = -10509;
/** JAD has a signature but no certificate */
const TInt KJavaInstSignatureWithoutCertificate = -10510;
/** Error decoding base64 certificate/signature data */
const TInt KJavaInstBadBase64 = -10511;
/** error code to indicate that abort is expected and no need to display another dialog */
const TInt KJavaInstNormalAbort = -10512;
/** error code to indicate that user abort occurred during version check */
const TInt KJavaInstVersionCheckAbort = -10513;
/** error code to indicate loss of service during http transaction */
const TInt KJavaInstLossOfService = -10514;
/** error code to indicate that we are awaiting rights for a jar file separate delivery case */
const TInt KJavaInstWaitingForDRMRights = -10515;
/** error code to indicate non-acceptable content in DRM download case*/
const TInt KJavaInstNonAcceptableContent = -10516;
/** error code to indicate uninstallation is not allowed*/
const TInt KJavaInstUnInstNotAllowed = -10517;


#endif // JAVAERROR_H

