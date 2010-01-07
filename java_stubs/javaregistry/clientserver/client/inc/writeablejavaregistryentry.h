/*
* Copyright (c) 2005-2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  writeablejavaregistryentry definition
*
*/


#ifndef WRITEABLEJAVAREGISTRYENTRY_H
#define WRITEABLEJAVAREGISTRYENTRY_H

// INCLUDES
#include <e32base.h>
#include <f32file.h>
#include "javaregistryentrytype.h"
#include "javaattributes.h"

using namespace Java;

namespace Java
{
namespace Manager
{
namespace Registry
{
// FORWARD DECLARATION
class CJavaProperty;

// CLASS DECLARATION
/**
 * This class provides the Internal API for entries in JavaRegistry.
 * It supports common attributes only.
 *
 * @lib javaregistryclient.lib
 * @since S60 v3.2
 */
class CWriteableJavaRegistryEntry :
        public CBase
{
public:

    // destructor
    /**
     * CWriteableJavaRegistryEntry::~CWriteableJavaRegistryEntry destructor
     *
     * @since S60 v3.2
     */
    IMPORT_C virtual ~CWriteableJavaRegistryEntry();

    // general methods
    /**
     * CWriteableJavaRegistryEntry::Properties method returns all the
     * properties the entry has.
     *
     * @since S60 v3.2
     * @return The properties stored in RPointerArray<CJavaProperty>.
     *         The ownership of objects is NOT subject to change.
     */
    const RPointerArray<CJavaProperty>& Properties() const;


    /**
     * CWriteableJavaRegistryEntry::Uid method gets the Uid of
     * this registry entry.
     *
     * @since S60 v3.2
     * @return A TUid object.
     */
    IMPORT_C TUid Uid() const;


    /**
     * Gets the name of the entity (midlet, suite, etc.) this entry
     * represents.
     *
     * @since S60 v3.2
     * @return Name of the entity this entry represents.
     *         Default value if not set: KNullDesC.
     */
    IMPORT_C const TDesC& Name() const;


    /**
     * CWriteableJavaRegistryEntry::Type method gets the type of this
     * registry entry. Enumeration TJavaRegistryEntryType defines
     * the currently possible options. Mandatory property.
     *
     * @since S60 v3.2
     * @return Type of this registry entry.
     */
    IMPORT_C TJavaRegistryEntryType Type() const;


    /**
     * CWriteableJavaRegistryEntry::Drive method gets the letter
     * of drive where the install package has been installed.
     *
     * @since S60 v3.2
     * @return Letter of drive where the install package has been
     *         installed.
     *         Default value if not set: EDriveC.
     */
    IMPORT_C TDriveNumber Drive() const;


    /**
     * CWriteableJavaRegistryEntry::NumberOfCertificateChains method
     * returns the number of certificate chains of this Java application.
     *
     * @since S60 v3.2
     * @return Number of certificate chains.
     */
    IMPORT_C TInt NumberOfCertificateChains() const;


    /**
     * CWriteableJavaRegistryEntry::CertificateChain method returns the
     * n-th certificate chain of this Java application.
     *
     * @since S60 v3.2
     * @param aIndex Index of the certificate chain to be retrieved.
     * @return N-th certificate chain
     *         Default value if not set: KNullDesC.
     */
    IMPORT_C const TDesC8& CertificateChain(TInt aIndex) const;


    /**
     * Returns the content id of the entity this entry represents.
     * The content id can be used to check if a DRM protected entity
     * has enough right to be launched or not.
     * The method can also be used to check if the entity is DRM
     * protected or not.
     *
     * @since S60 v3.2
     * @return Content id of the entity this entry represent or
     *          KNullDesC if the entity is not DRM protected.
     */
    IMPORT_C const TDesC& DrmContentId() const;


    /**
     * CWriteableJavaRegistryEntry::SetNameL method sets the name of
     * this registry entry.
     * The function may leave with one of the system-wide error codes.
     *
     * @since S60 v3.2
     * @param aName The name to be setted.
     */
    IMPORT_C void SetNameL(const TDesC& aName);

    /**
     * CWriteableJavaRegistryEntry::SetTypeL method sets the type of this
     * registry entry. Enumeration TJavaRegistryEntryType defines
     * the currently possible options.
     * The function may leave with one of the system-wide error codes.
     *
     *
     * @since S60 v3.2
     * @param aType The type to be setted.
     */
    IMPORT_C void SetTypeL(TJavaRegistryEntryType aType);

    /**
     * CWriteableJavaRegistryEntry::SetDriveL method sets the letter
     * of drive where the install package has been installed.
     * The function may leave with one of the system-wide error codes.
     *
     * @since S60 v3.2
     * @param aDrive Letter of drive where the install package has been
     *                  installed.
     */
    IMPORT_C void SetDriveL(const TDriveNumber& aDrive);


    /**
     * CWriteableJavaRegistryEntry::AddCertificateChainL method adds
     * a certificate chain to this Java application.
     * The function may leave with one of the system-wide error codes.
     *
     * @since S60 v3.2
     * @param aChain The new certificate chain.
     */
    IMPORT_C void AddCertificateChainL(const TDesC8& aChain);


    /**
     * CWriteableJavaRegistryEntry::SetDrmContentIdL sets DRM content
     * id of this entry. If the entity represented by this entry is
     * not DRM protected this content id should not be set
     *
     * @since S60 v3.2
     * @param aDrmContentId DRM content id to be set
     */
    IMPORT_C void SetDrmContentIdL(const TDesC& aDrmContentId);


    /**
     * CWriteableJavaRegistryEntry::PropertyL method gets a property,
     * specified by the Id parameter.
     *
     * @since S60 v3.2
     * @param aId The Id of the property to be returned.
     * @return The pointer of the property specified by the Id parameter,
     *         or NULL if it is not set.
     *         The ownership of the object being transferred is NOT
     *         subject to change.
     */
    IMPORT_C const CJavaProperty* Property(TInt32 aId) const;


    /**
     * CWriteableJavaRegistryEntry::SetPropertyL method sets a new, or
     * updtates a stored property if already exists.
     * The function may leave with one of the system-wide error codes.
     *
     * @since S60 v3.2
     * @param aProperty Property to be setted. Pointer ownership taken.
     */
    IMPORT_C virtual void SetPropertyL(CJavaProperty* aProperty);


    /**
     * CWriteableJavaRegistryEntry::SetPropertyL method sets a new, or
     * updtates a stored property if already exists.
     * The function may leave with one of the system-wide error codes.
     *
     * @since S60 v3.2
     * @param aId The Id of the Property.
     * @param aValue Value for the Property.
     */
    IMPORT_C void SetPropertyL(TInt32 aId, const TDesC& aValue);

    /**
     * CWriteableJavaRegistryEntry::Attribute method gets the
     * additional attribute.
     *
     * @since S60 v5.0
     * @param aName The name the searched attribute.
     * @return MJavaAttribute poiter to instance, which contain attribute.
     * If attribute doesn't exist return NULL.
     */
    IMPORT_C virtual const MJavaAttribute* AttributeL(const TDesC& aName) const;

    /**
     * CWriteableJavaRegistryEntry::Attributes method gets the
     * additional attributes array.
     *
     * @since S60 v5.0
     * @return RPointerArray& with all additional attributes.
     */
    IMPORT_C virtual const RPointerArray< MJavaAttribute >& AttributesL() const;

    /**
     * CWriteableJavaRegistryEntry::SerializeAttributes method gets the
     * streamed attributes and store stream to CJavaProperty as
     * special ESerializedAttribute
     * If don't exists any attribute method do nothing
     *
     * @since S60 v5.0
     *
     */
    IMPORT_C virtual void SerializeAttributesL();



protected:

    // constructors

    /**
     * CWriteableJavaRegistryEntry::CWriteableJavaRegistryEntry constructor
     *
     * @since S60 v3.2
     * @param aUid The Uid of this entry.
     */
    IMPORT_C CWriteableJavaRegistryEntry(const TUid& aUid);

    /**
     * 2nd phase constructor. Used by the subclasses.
     * The function may leave with one of the system-wide error codes.
     *
     * @since S60 v3.2
     * @param aType The type to be setted.
     */
    IMPORT_C void ConstructL(TJavaRegistryEntryType aType);

    // general methods

    /**
     * CWriteableJavaRegistryEntry::IntegerProperty method gets an
     * integer propertyvalue, specified by the Id parameter.
     *
     * @since S60 v3.2
     * @param aId The Id of the property to be returned.
     * @param aDefault The value to be returned if aValue does'nt
     *                 contain valid data.
     * @return The integer value of the property.
     */
    IMPORT_C TInt32 IntegerProperty(TInt32 aId, TInt32 aDefault) const;

    /**
     * CWriteableJavaRegistryEntry::BooleanProperty method gets a
     * boolean propertyvalue, specified by the Id parameter.
     *
     * @since S60 v3.2
     * @param aId The Id of the property to be returned.
     * @param aDefault The value to be returned if aValue does'nt
     *                 contain valid data.
     * @return The boolean value of the property.
     */
    IMPORT_C TBool BooleanProperty(TInt32 aId, TBool aDefault) const;


    /**
     * CWriteableJavaRegistryEntry::SetBooleanPropertyL method sets a new,
     * or updtates a stored boolean property if already exists.
     * Used by some specialized boolean property setter methods.
     * The function may leave with one of the system-wide error codes.
     *
     * @since S60 v3.2
     * @param aId The Id of the Property.
     * @param aBoolValue Boolean value for the Property.
     */
    IMPORT_C void SetBooleanPropertyL(TInt32 aId, TBool aBoolValue);

    /**
     * CWriteableJavaRegistryEntry::SetDescriptorPropertyL method sets
     * a new, or updtates a stored descriptor property if already exists.
     * Used by some specialized descriptor property setter methods.
     * The function may leave with one of the system-wide error codes.
     *
     * @since S60 v3.2
     * @param aId The Id of the Property.
     * @param aDescValue Descriptor value for the Property.
     */
    IMPORT_C void SetDescriptorPropertyL(TInt32 aId,
                                         const TDesC& aDescValue);

    /**
     * CWriteableJavaRegistryEntry::SetIntegerPropertyL method sets a new,
     * or updtates a stored integer property if already exists.
     * Used by some specialized integer/Uid property setter methods.
     * The function may leave with one of the system-wide error codes.
     *
     * @since S60 v3.2
     * @param aId The Id of the Property.
     * @param aIntValue Value for the Property.
     */
    IMPORT_C void SetIntegerPropertyL(TInt32 aId, TInt32 aIntValue);

    /**
     * CWriteableJavaRegistryEntry::DeleteProperty method deletes
     * a property, specified by the index parameter.
     *
     * @since S60 v3.2
     * @param aIndex The index of the property to be deleted.
     * @return
     */
    IMPORT_C void DeleteProperty(TInt aIndex);

    /**
     * CWriteableJavaRegistryEntry::GetPropertyIndex method gets the
     * index of the property specified by the Id.
     *
     * @since S60 v3.2
     * @param aId The id of the searched property.
     * @return The index of the searched property. If not found, -1 is
     *         returned.
     */
    IMPORT_C TInt GetPropertyIndex(TInt32 aId) const;



protected: // Data


    /**
     * CWriteableJavaRegistryEntry::iProperties member stores all the
     * properties, whether processed here or in subclasses.
     */
    RPointerArray<CJavaProperty> iProperties;

    /**
     * CWriteableJavaRegistryEntry::iCertificates member stores the
     * certificate chains. The certificate chains are also stored in the
     * iProperties, and are synchronised at every modification.
     */
    RPointerArray<HBufC8> iCertificates;



private: // Data
    /**
     * CWriteableJavaRegistryEntry::iUid member, representing the Uid
     * of this entry.
     */
    TUid iUid;

    RPointerArray<MJavaAttribute> iAttributes;

};

}// namespace Registry
}// namespace Manager
}// namespace Java


#endif // WRITEABLEJAVAREGISTRYENTRY_H

// End of File
