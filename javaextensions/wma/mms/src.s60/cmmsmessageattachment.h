/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:  ?Description
 *
*/


#ifndef _CMMS_MESSAGE_ATTACHMENT_H
#define _CMMS_MESSAGE_ATTACHMENT_H

//  INCLUDES
#include <e32base.h>
#include <f32file.h>

namespace java
{
namespace wma
{
_LIT(KTempMmsAttachmentName, "midp_wma20_attachment");
_LIT(KTempDriveExt, ":\\");

/**
 * Class to store the attachment data on the native side
 * 1. Attachment data
 * 2. Attach data type
 * 3. Content Id
 * 4. Content location
 * 5. IsStarterId
 * 6. Attchment Character encoding
 * 7. Attachment size
 *
 *  @lib hmidpS60v30.lib on wins/winscw and midp2.exe
 *  @since Series 60 3.0
 */

class CMMSMessageAttachment: public CBase
{
public:
    /**
     * Create the Java counter part of MessagePart.
     * @since Series 60 3.0
     * @param aRFs
     * @param aDataArray: data of the message part
     * @param aAttachmentType: data type (attachment type)
     * @param aContentId: Content Id of the data
     * @param aLocation: lovcation of the data
     * @param aValue: Message is from java side to native side or
     *                other way arround.
     * @return newly created CMIDMessageAttachment
     */
    static CMMSMessageAttachment* NewL(RFs& aRFs, const TDesC8& aDataArray,
                                       const TDesC8& aAttachmentType, const TDesC8& aContentId,
                                       const TDesC8& aLocation, TInt aAttachmentNameOrData = TRUE);
    /**
     * Destructor of the CMIDMEssageAttachment.
     * @since Series 60 3.0
     *
     * @return
     */
    ~CMMSMessageAttachment();

    /**
     * Set the character encoding of MessagePart.
     * @since Series 60 3.0
     * @param aEncoding Character encoding as TUint
     *
     * @return
     */
    void SetEncoding(TUint aEncoding);
    /**
     * Get the character encoding of MessagePart.
     * @since Series 60 3.0
     *
     * @return the character encoding of MessagePart
     */
    TInt Encoding();
    /**
     * Get the mime type of MessagePart (attachment).
     * @since Series 60 3.0
     *
     * @return the mime type of MessagePart (attachment).
     */
    TDesC8& AttachmentType();
    /**
     * Get the data of MessagePart (attachment).
     * @since Series 60 3.0
     *
     * @return the data of MessagePart (attachment).
     */
    TDesC8& AttachmentData();
    /**
     * Get the value of starter contentId of MessagePart (attachment).
     * @since Series 60 3.0
     *
     * @return if the messge part is starter content Id it will
     true orther wise false.
     */
    TInt IsStarterContentId();
    /**
     * Set the value of starter contentId of MessagePart (attachment).
     * @since Series 60 3.0
     *
     * @return
     */
    void SetStarterContentId(TInt aValue = FALSE);
    /**
     * Get the contentId of MessagePart (attachment).
     * @since Series 60 3.0
     *
     * @return content Id
     */
    TDesC8& ContentIdL();
    /**
     * Get the location of MessagePart (attachment).
     * @since Series 60 3.0
     *
     * @return content Id
     */
    TDesC8& ContentLocationL();
    /**
     * When sending messages from Java side to native side.
     * We are saving the attachment on the D drive in temp folder.
     * Get the name of that temp file contain data of MessagePart
     * (attachment).
     * @since Series 60 3.0
     *
     * @return temp file name
     */
    // required in client connection
    TDesC& AttachmentName();

private:
    /**
     * private constructor of CMIDMessageAttachment
     * C++ default constructor.
     * @since Series 60 3.0
     * @param aRFs
     * @return temp file name
     */
    CMMSMessageAttachment(RFs& aRFs);
    /**
     * default Symbian 2nd phase constructor.
     * @since Series 60 3.0
     * @param aDataArray: data of the message part
     * @param aAttachmentType: data type (attachment type)
     * @param aContentId: Content Id of the data
     * @param aLocation: lovcation of the data
     * @param aValue: Message is from java side to native side or
     *                other way arround.
     * @return
     */
    void ConstructL(const TDesC8& aDataArray, const TDesC8& aAttachmentType,
                    const TDesC8& aContentId, const TDesC8& aLocation,
                    TInt aAttachmentNameOrData);

    /**
     * Set the contentlocation of the MessagePart (Attachment)
     * @since Series 60 3.0
     * @param aLocation
     * @return
     */
    void SetContentLocationLC(const TDesC8& aLocation);
    /**
     * Set the contentId of the MessagePart (Attachment)
     * @since Series 60 3.0
     * @param aContentId
     * @return
     */
    void SetContentIdLC(const TDesC8& aContentId);
    /**
     * Set the mime type of the MessagePart (Attachment)
     * @since Series 60 3.0
     * @param aAttachmentType
     * @return
     */
    void SetAttchmentTypeLC(const TDesC8& aAttachmentType);

private:
    /*
     * own the mAttachmentNameWithPath
     */
    TFileName *mAttachmentNameWithPath;
    /*
     * own the mAttachmentTypeBuf8
     */
    TDesC8* mAttachmentTypeBuf8;
    /*
     * own the mAttachmentData8
     */
    TDesC8* mAttachmentData8;
    /*
     * own the mContantId8
     */
    HBufC8* mContantId8;
    /*
     * own the mContantLocation8
     */
    HBufC8* mContantLocation8;

    TInt mEncoding;
    //Handle to fileserver
    RFs& mRFs;
    //
    TInt mIsStarterContentId;
};
} //namespace wma
} //namespace java

#endif
