/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include <stdio.h>
#include <string>
#include <x509certchain.h>
#include <memory>

#include "TestHarness.h"

#include "javastorage.h"
#include "javastorageexception.h"
#include "javastoragenames.h"
#include "javauid.h"
#include "javaprocessconstants.h"
#include "javasymbianoslayer.h"
#include "javaregistry.h"
#include "javaregistryentry.h"
#include "logger.h"
#include "mjavaattribute.h"
#include "storagetestutils.h"

#include "javacommonutils.h"

using namespace std;
using namespace Java;
using namespace java::storage;
using namespace java::util;

const JavaStorageApplicationEntry_t createEntry(const wstring& aUid,
        const wstring& aName,
        const wstring& aValue);

const JavaStorageApplicationEntry_t createValidCertsEntry(
    const wstring& aUid,
    const wstring& aCerts);

TEST_GROUP(TestRegistryEntry)
{
    JavaStorage* js;
    JavaStorageTestUtils* jtu;
    CActiveScheduler* newScheduler;

    TEST_SETUP()
    {
        newScheduler = new CActiveScheduler();
        CActiveScheduler::Install(newScheduler);

        js = JavaStorage::createInstance();
        jtu = new JavaStorageTestUtils();
    }
    TEST_TEARDOWN()
    {
        try
        {
            js->rollbackTransaction();
            js->close();
            delete js;
            js = NULL;
        }
        catch (...)
        {
            // No can do
        }

        delete js;
        js = 0;
        delete jtu;
        jtu = 0;

        delete newScheduler;
        newScheduler = NULL;
    }
};

/**
 * Test CJavaRegistryEntry::Uid() method.
 * 1. Application uid.
 * 2. Application suite uid.
 * NOTE: Invalid Uid cannot be tested as it cannot be queried.
 */
TEST(TestRegistryEntry, TestUid)
{
    LOG(EJavaStorage, EInfo, "+TestUid");

    JavaStorageApplicationEntry_t midlet1;
    JavaStorageEntry attr;

    Uid suite1Uid(L"[e22a4577]");

    attr.setEntry(PACKAGE_ID, suite1Uid.toString());
    midlet1.insert(attr);

    Uid midlet1Uid(L"[e22a4588]");

    attr.setEntry(ID, midlet1Uid.toString());
    midlet1.insert(attr);

    js->open();
    js->startTransaction();

    CHECK(jtu->populate(*js, APPLICATION_TABLE, midlet1));

    JavaStorageApplicationEntry_t suite1;
    attr.setEntry(PACKAGE_NAME, L"MySuite");
    suite1.insert(attr);

    attr.setEntry(ID, suite1Uid.toString());
    suite1.insert(attr);

    attr.setEntry(MEDIA_ID, L"-124614446");
    suite1.insert(attr);

    CHECK(jtu->populate(*js, APPLICATION_PACKAGE_TABLE, suite1));

    // Session must be committed before next use of Registry otherwise
    // it is locked.
    js->commitTransaction();

    auto_ptr<CJavaRegistry> registry(CJavaRegistry::NewL());

    // 1. Application uid.
    TUid midletUid;
    uidToTUid(midlet1Uid, midletUid);

    CJavaRegistryEntry* entry = registry->RegistryEntryL(midletUid);
    CHECK(entry != NULL);
    CHECK(entry->Uid().iUid == midletUid.iUid);

    delete entry;
    entry = NULL;

    // 2. Application suite uid.
    TUid suiteUid;
    uidToTUid(suite1Uid, suiteUid);

    entry = registry->RegistryEntryL(suiteUid);
    CHECK(entry != NULL);
    CHECK(entry->Uid().iUid == suiteUid.iUid);

    delete entry;
    entry = NULL;

    // Clean
    js->startTransaction();
    CHECK(jtu->remove(*js, APPLICATION_TABLE, midlet1));
    CHECK(jtu->remove(*js, APPLICATION_PACKAGE_TABLE, suite1));
    js->commitTransaction();
    js->close();
    LOG(EJavaStorage, EInfo, "-TestUid");
}

/**
 * Test CJavaRegistryEntry::Name() method.
 * 1. Application no name defined
 * 2. Application suite no name defined
 * 3. Application name.
 * 4. Application suite name.
 */
TEST(TestRegistryEntry, TestName)
{
    LOG(EJavaStorage, EInfo, "+TestName");
    JavaStorageApplicationEntry_t midlet1;
    JavaStorageEntry attr;

    Uid suite1Uid(L"[e2234577]");

    attr.setEntry(PACKAGE_ID, suite1Uid.toString());
    midlet1.insert(attr);

    Uid midlet1Uid(L"[e2234588]");

    attr.setEntry(ID, midlet1Uid.toString());
    midlet1.insert(attr);

    js->open();
    js->startTransaction();
    CHECK(jtu->populate(*js, APPLICATION_TABLE, midlet1));

    JavaStorageApplicationEntry_t suite1;
    attr.setEntry(ID, suite1Uid.toString());
    suite1.insert(attr);

    attr.setEntry(MEDIA_ID, L"-124614446");
    suite1.insert(attr);

    CHECK(jtu->populate(*js, APPLICATION_PACKAGE_TABLE, suite1));
    js->commitTransaction();

    auto_ptr<CJavaRegistry> registry(CJavaRegistry::NewL());

    // 1. Application uid.
    TUid midletUid;
    uidToTUid(midlet1Uid, midletUid);

    CJavaRegistryEntry* entry = registry->RegistryEntryL(midletUid);
    CHECK(entry != NULL);

    // 1. Application no name defined
    CHECK(entry->Name() == KNullDesC);

    // 2. Application suite no name defined
    TUid suiteUid;
    uidToTUid(suite1Uid, suiteUid);

    delete entry;
    entry = NULL;

    entry = registry->RegistryEntryL(suiteUid);
    CHECK(entry != NULL);
    CHECK(entry->Name() == KNullDesC);

    delete entry;
    entry = NULL;

    JavaStorageApplicationEntry_t midlet2;

    Uid suite2Uid(L"[e2224577]");

    attr.setEntry(PACKAGE_ID, suite2Uid.toString());
    midlet2.insert(attr);

    Uid midlet2Uid(L"[e2224588]");

    attr.setEntry(ID, midlet2Uid.toString());
    midlet2.insert(attr);

    wstring midlet2Name = L"MIDlet2";
    attr.setEntry(NAME, midlet2Name);
    midlet2.insert(attr);

    js->open();
    js->startTransaction();

    CHECK(jtu->populate(*js, APPLICATION_TABLE, midlet2));

    wstring suite2Name = L"MySuite2";
    JavaStorageApplicationEntry_t suite2;
    attr.setEntry(PACKAGE_NAME, suite2Name);
    suite2.insert(attr);

    attr.setEntry(ID, suite2Uid.toString());
    suite2.insert(attr);

    attr.setEntry(MEDIA_ID, L"-124614446");
    suite2.insert(attr);

    CHECK(jtu->populate(*js, APPLICATION_PACKAGE_TABLE, suite2));

    // Session must be committed before next use of Registry otherwise
    // it is locked.
    js->commitTransaction();

    // 3. Application name.
    uidToTUid(midlet2Uid, midletUid);

    entry = registry->RegistryEntryL(midletUid);
    CHECK(entry != NULL);

    const TDesC16& name = entry->Name();
    HBufC16* refName = wstringToBuf(midlet2Name);
    CHECK(name.Compare(refName->Des()) == 0);

    delete refName;
    refName = NULL;
    delete entry;
    entry = NULL;

    // 4. Application suite name.
    uidToTUid(suite2Uid, suiteUid);

    entry = registry->RegistryEntryL(suiteUid);
    CHECK(entry != NULL);

    const TDesC16& name2 = entry->Name();
    refName = wstringToBuf(suite2Name);
    CHECK(name2.Compare(refName->Des()) == 0);

    delete refName;
    refName = NULL;
    delete entry;
    entry = NULL;

    registry.reset(0);

    js->startTransaction();
    CHECK(jtu->remove(*js, APPLICATION_TABLE, midlet1));
    CHECK(jtu->remove(*js, APPLICATION_PACKAGE_TABLE, suite1));
    CHECK(jtu->remove(*js, APPLICATION_TABLE, midlet2));
    CHECK(jtu->remove(*js, APPLICATION_PACKAGE_TABLE, suite2));
    js->commitTransaction();
    js->close();

    LOG(EJavaStorage, EInfo, "-TestName");
}

/**
 * Test CJavaRegistryEntry::Type() method.
 * 1. Application type.
 * 2. Application suite type.
 */
TEST(TestRegistryEntry, TestType)
{
    LOG(EJavaStorage, EInfo, "+TestType");
    JavaStorageApplicationEntry_t midlet1;
    JavaStorageEntry attr;

    Uid suite1Uid(L"[ea234577]");

    attr.setEntry(PACKAGE_ID, suite1Uid.toString());
    midlet1.insert(attr);

    Uid midlet1Uid(L"[ea234588]");

    attr.setEntry(ID, midlet1Uid.toString());
    midlet1.insert(attr);

    js->open();
    js->startTransaction();

    CHECK(jtu->populate(*js, APPLICATION_TABLE, midlet1));

    JavaStorageApplicationEntry_t suite1;
    attr.setEntry(ID, suite1Uid.toString());
    suite1.insert(attr);
    attr.setEntry(MEDIA_ID, L"-124614446");
    suite1.insert(attr);

    CHECK(jtu->populate(*js, APPLICATION_PACKAGE_TABLE, suite1));

    // Session must be committed before next use of Registry otherwise
    // it is locked.
    js->commitTransaction();

    auto_ptr<CJavaRegistry> registry(CJavaRegistry::NewL());

    // 1. Application uid.
    TUid midletUid;
    uidToTUid(midlet1Uid, midletUid);

    CJavaRegistryEntry* entry = registry->RegistryEntryL(midletUid);
    CHECK(entry != NULL);
    CHECK(entry->Type() == EMidp2Midlet);

    delete entry;
    entry = NULL;

    TUid suiteUid;
    uidToTUid(suite1Uid, suiteUid);

    entry = registry->RegistryEntryL(suiteUid);
    CHECK(entry != NULL);
    CHECK(entry->Type() == EMidp2MidletSuite);

    delete entry;
    entry = NULL;

    js->startTransaction();
    CHECK(jtu->remove(*js, APPLICATION_TABLE, midlet1));
    CHECK(jtu->remove(*js, APPLICATION_PACKAGE_TABLE, suite1));
    js->commitTransaction();
    js->close();
    registry.reset(0);

    LOG(EJavaStorage, EInfo, "-TestType");
}

/**
 * Test CJavaRegistryEntry::Drive() method.
 * 1. Get application suite drive.
 * 2. Get application drive.
 */
TEST(TestRegistryEntry, TestDrive)
{
    LOG(EJavaStorage, EInfo, "+TestDrive");
    JavaStorageApplicationEntry_t midlet1;
    JavaStorageEntry attr;

    Uid suite1Uid(L"[e01a4577]");

    attr.setEntry(PACKAGE_ID, suite1Uid.toString());
    midlet1.insert(attr);

    Uid midlet1Uid(L"[e01a4588]");

    attr.setEntry(ID, midlet1Uid.toString());
    midlet1.insert(attr);

    js->open();
    js->startTransaction();

    CHECK(jtu->populate(*js, APPLICATION_TABLE, midlet1));

    JavaStorageApplicationEntry_t suite1;
    attr.setEntry(ID, suite1Uid.toString());
    suite1.insert(attr);

    attr.setEntry(ROOT_PATH, L"e:\\private\\MIDlets\\[e01a4577]");
    suite1.insert(attr);
    attr.setEntry(MEDIA_ID, L"-124614446");
    suite1.insert(attr);

    CHECK(jtu->populate(*js, APPLICATION_PACKAGE_TABLE, suite1));

    // Session must be committed before next use of Registry otherwise
    // it is locked.
    js->commitTransaction();

    auto_ptr<CJavaRegistry> registry(CJavaRegistry::NewL());

    // 1. Application uid.
    TUid midletUid;
    uidToTUid(midlet1Uid, midletUid);

    CJavaRegistryEntry* entry = registry->RegistryEntryL(midletUid);
    CHECK(entry != NULL);
    CHECK(entry->Drive() == 4); // 4 represents e:

    delete entry;
    entry = NULL;

    TUid suiteUid;
    uidToTUid(suite1Uid, suiteUid);

    entry = registry->RegistryEntryL(suiteUid);
    CHECK(entry != NULL);
    CHECK(entry->Drive() == 4);  // 4 represents c:

    delete entry;
    entry = NULL;

    js->startTransaction();
    CHECK(jtu->remove(*js, APPLICATION_TABLE, midlet1));
    CHECK(jtu->remove(*js, APPLICATION_PACKAGE_TABLE, suite1));
    js->commitTransaction();
    js->close();
    registry.reset(0);

    LOG(EJavaStorage, EInfo, "-TestDrive");
}

/**
 * Test CJavaRegistryEntry::AddributeL() method.
 * 1. Test no attributes in DB. NULL is returned.
 * 2. Test empty name. Leaves with KErrArgument.
 * 3. Test non-existing attribute. NULL is returned.
 * 4. Test existing application attribute.
 * 5. Test existing application suite attribute.
 * 6. Test trusted attribute.
 * 7. Test attribute VALUE not set i.e. empty.
 *    NULL is returned.
 */
TEST(TestRegistryEntry, TestAttribute)
{
    LOG(EJavaStorage, EInfo, "+TestAttribute");
    JavaStorageApplicationEntry_t midlet1;
    JavaStorageEntry attr;

    Uid suite1Uid(L"[ebbb4577]");

    attr.setEntry(PACKAGE_ID, suite1Uid.toString());
    midlet1.insert(attr);

    Uid midlet1Uid(L"[ebbb4588]");

    attr.setEntry(ID, midlet1Uid.toString());
    midlet1.insert(attr);

    js->open();
    js->startTransaction();
    CHECK(jtu->populate(*js, APPLICATION_TABLE, midlet1));

    JavaStorageApplicationEntry_t suite1;
    attr.setEntry(ID, suite1Uid.toString());
    suite1.insert(attr);

    attr.setEntry(ROOT_PATH, L"e:\\private\\MIDlets\\[ebbb4577]");
    suite1.insert(attr);
    attr.setEntry(MEDIA_ID, L"-124614446");
    suite1.insert(attr);

    CHECK(jtu->populate(*js, APPLICATION_PACKAGE_TABLE, suite1));
    js->commitTransaction();

    // 1. Test no attributes in DB.
    TUid midletUid;
    uidToTUid(midlet1Uid, midletUid);

    auto_ptr<CJavaRegistry> registry(CJavaRegistry::NewL());

    CJavaRegistryEntry* entry = registry->RegistryEntryL(midletUid);
    CHECK(entry != NULL);

    // 1. non-existing attribute
    _LIT(KAttrName, "TestClass");
    const MJavaAttribute* regAttr = entry->AttributeL(KAttrName);

    CHECK(regAttr == NULL);

    delete entry;
    entry = NULL;

    // Populate attributes
    JavaStorageApplicationEntry_t suiteAttribute;
    attr.setEntry(ID, suite1Uid.toString());
    suiteAttribute.insert(attr);

    attr.setEntry(NAME, L"MIDlet-1");
    suiteAttribute.insert(attr);

    attr.setEntry(VALUE, L"");
    suiteAttribute.insert(attr);

    attr.setEntry(TRUSTED, L"0", JavaStorageEntry::INT);
    suiteAttribute.insert(attr);

    js->startTransaction();
    CHECK(jtu->populate(*js, APPLICATION_PACKAGE_ATTRIBUTES_TABLE, suiteAttribute));
    suiteAttribute.clear();

    attr.setEntry(ID, suite1Uid.toString());
    suiteAttribute.insert(attr);

    attr.setEntry(NAME, L"Jar-Size");
    suiteAttribute.insert(attr);

    attr.setEntry(VALUE, L"2342");
    suiteAttribute.insert(attr);

    attr.setEntry(TRUSTED, L"1", JavaStorageEntry::INT);
    suiteAttribute.insert(attr);

    CHECK(jtu->populate(*js, APPLICATION_PACKAGE_ATTRIBUTES_TABLE, suiteAttribute));
    suiteAttribute.clear();

    attr.setEntry(ID, suite1Uid.toString());
    suiteAttribute.insert(attr);

    attr.setEntry(NAME, L"TestClass");
    suiteAttribute.insert(attr);

    attr.setEntry(VALUE, L"com.platform.test.app");
    suiteAttribute.insert(attr);

    attr.setEntry(TRUSTED, L"0", JavaStorageEntry::INT);
    suiteAttribute.insert(attr);

    CHECK(jtu->populate(*js, APPLICATION_PACKAGE_ATTRIBUTES_TABLE, suiteAttribute));
    suiteAttribute.clear();
    js->commitTransaction();

    // 2. Test empty name.
    uidToTUid(midlet1Uid, midletUid);

    entry = registry->RegistryEntryL(midletUid);
    CHECK(entry != NULL);

    // 1. non-existing attribute
    _LIT(KAttrName2, "");

    TRAPD(err, regAttr = entry->AttributeL(KAttrName2));
    CHECK(KErrArgument == err);

    TRAP(err, regAttr = entry->AttributeL(KNullDesC));
    CHECK(KErrArgument == err);

    // 3. Test non-existing attribute.
    _LIT(KAttrName3, "FOO_BAR");
    regAttr = entry->AttributeL(KAttrName3);
    CHECK(regAttr == NULL);

    // 4. Test existing application attribute.
    _LIT(KAttrName4, "TestClass");
    regAttr = entry->AttributeL(KAttrName4);
    CHECK(regAttr != NULL);

    _LIT(KAttrValue, "com.platform.test.app");

    CHECK(regAttr->Name() == KAttrName4);
    CHECK(regAttr->Value() == KAttrValue);
    CHECK(regAttr->Trusted() == EFalse);

    delete regAttr;
    regAttr = NULL;

    delete entry;
    entry = NULL;

    // 5. Test existing application suite attribute.
    TUid suiteUid;
    uidToTUid(suite1Uid, suiteUid);

    entry = registry->RegistryEntryL(suiteUid);
    CHECK(entry != NULL);

    regAttr = entry->AttributeL(KAttrName4);
    CHECK(regAttr != NULL);

    CHECK(regAttr->Name() == KAttrName4);
    CHECK(regAttr->Value() == KAttrValue);
    CHECK(regAttr->Trusted() == EFalse);

    delete regAttr;
    regAttr = NULL;

    // 6. Test trusted attribute.
    _LIT(KAttrName5, "Jar-Size");

    regAttr = entry->AttributeL(KAttrName5);
    CHECK(regAttr != NULL);

    _LIT(KAttrValue2, "2342");

    CHECK(regAttr->Name() == KAttrName5);
    CHECK(regAttr->Value() == KAttrValue2);
    CHECK(regAttr->Trusted() == true);

    delete regAttr;
    regAttr = NULL;

    // 7. Test attribute VALUE not set i.e. empty.
    _LIT(KAttrName6, "MIDlet-1");

    regAttr = entry->AttributeL(KAttrName6);
    CHECK(regAttr == NULL);

    delete entry;
    entry = NULL;

    js->startTransaction();
    CHECK(jtu->remove(*js, APPLICATION_TABLE, midlet1));
    CHECK(jtu->remove(*js, APPLICATION_PACKAGE_TABLE, suite1));

    suiteAttribute.clear();
    attr.setEntry(ID, suite1Uid.toString());
    suiteAttribute.insert(attr);
    CHECK(jtu->remove(*js, APPLICATION_PACKAGE_ATTRIBUTES_TABLE, suiteAttribute));

    js->commitTransaction();
    js->close();
    registry.reset(0);

    LOG(EJavaStorage, EInfo, "-TestAttribute");
}

/**
 * Test CJavaRegistryEntry::AddributesL() method.
 * 1. Test no attributes in DB. Emtry array returned.
 * 2. Test existing application attribute.
 * 3. Test existing application suite attribute.
 * 4. Test query twice i.e. Caching.
 */
TEST(TestRegistryEntry, TestAttributes)
{
    LOG(EJavaStorage, EInfo, "+TestAttributes");
    JavaStorageApplicationEntry_t midlet1;
    JavaStorageEntry attr;

    Uid suite1Uid(L"[eaaaa577]");

    attr.setEntry(PACKAGE_ID, suite1Uid.toString());
    midlet1.insert(attr);

    Uid midlet1Uid(L"[eaaaa588]");

    attr.setEntry(ID, midlet1Uid.toString());
    midlet1.insert(attr);

    js->open();
    js->startTransaction();

    CHECK(jtu->populate(*js, APPLICATION_TABLE, midlet1));

    JavaStorageApplicationEntry_t suite1;
    attr.setEntry(ID, suite1Uid.toString());
    suite1.insert(attr);

    attr.setEntry(ROOT_PATH, L"e:\\private\\MIDlets\\[eaaaa577]");
    suite1.insert(attr);
    attr.setEntry(MEDIA_ID, L"-124614446");
    suite1.insert(attr);

    CHECK(jtu->populate(*js, APPLICATION_PACKAGE_TABLE, suite1));

    // Session must be committed before next use of Registry otherwise
    // it is locked.
    js->commitTransaction();

    TUid midletUid;
    uidToTUid(midlet1Uid, midletUid);

    auto_ptr<CJavaRegistry> registry(CJavaRegistry::NewL());
    CJavaRegistryEntry* entry = registry->RegistryEntryL(midletUid);
    CHECK(entry != NULL);

    // 1. Test no attributes in DB.
    const RPointerArray<MJavaAttribute>& attrs = entry->AttributesL();
    CHECK(attrs.Count() == 0);

    delete entry;
    entry = NULL;

    // Populate attributes
    JavaStorageApplicationEntry_t suiteAttribute;
    attr.setEntry(ID, suite1Uid.toString());
    suiteAttribute.insert(attr);

    attr.setEntry(NAME, L"MIDlet-1");
    suiteAttribute.insert(attr);

    attr.setEntry(VALUE, L"Greaat MIDlet");
    suiteAttribute.insert(attr);

    attr.setEntry(TRUSTED, L"0", JavaStorageEntry::INT);
    suiteAttribute.insert(attr);

    js->startTransaction();
    CHECK(jtu->populate(*js, APPLICATION_PACKAGE_ATTRIBUTES_TABLE, suiteAttribute));
    suiteAttribute.clear();

    attr.setEntry(ID, suite1Uid.toString());
    suiteAttribute.insert(attr);

    attr.setEntry(NAME, L"Jar-Size");
    suiteAttribute.insert(attr);

    attr.setEntry(VALUE, L"2342");
    suiteAttribute.insert(attr);

    attr.setEntry(TRUSTED, L"1", JavaStorageEntry::INT);
    suiteAttribute.insert(attr);

    CHECK(jtu->populate(*js, APPLICATION_PACKAGE_ATTRIBUTES_TABLE, suiteAttribute));
    suiteAttribute.clear();

    attr.setEntry(ID, suite1Uid.toString());
    suiteAttribute.insert(attr);

    attr.setEntry(NAME, L"TestClass");
    suiteAttribute.insert(attr);

    attr.setEntry(VALUE, L"com.platform.test.app");
    suiteAttribute.insert(attr);

    attr.setEntry(TRUSTED, L"0", JavaStorageEntry::INT);
    suiteAttribute.insert(attr);

    CHECK(jtu->populate(*js, APPLICATION_PACKAGE_ATTRIBUTES_TABLE, suiteAttribute));
    suiteAttribute.clear();

    // Session must be committed before next use of Registry otherwise
    // it is locked.
    js->commitTransaction();

    // 2. Test existing application attributes.
    entry = registry->RegistryEntryL(midletUid);
    CHECK(entry != NULL);

    const RPointerArray<MJavaAttribute>& attrs2 = entry->AttributesL();
    CHECK(attrs2.Count() == 3);

    // Cannot quarantee order so size check should be OK.

    delete entry;
    entry = NULL;

    // 3. Test existing application suite attribute.
    TUid suiteUid;
    uidToTUid(suite1Uid, suiteUid);

    entry = registry->RegistryEntryL(suiteUid);
    CHECK(entry != NULL);

    const RPointerArray<MJavaAttribute>& attrs3 = entry->AttributesL();
    CHECK(attrs3.Count() == 3);

    // Cannot quarantee order so size check should be OK.

    // 4. Test query twice i.e. Caching.
    const RPointerArray<MJavaAttribute>& attrs4 = entry->AttributesL();
    CHECK(attrs4.Count() == 3);

    delete entry;
    entry = NULL;

    js->startTransaction();
    CHECK(jtu->remove(*js, APPLICATION_TABLE, midlet1));
    CHECK(jtu->remove(*js, APPLICATION_PACKAGE_TABLE, suite1));

    suiteAttribute.clear();
    attr.setEntry(ID, suite1Uid.toString());
    suiteAttribute.insert(attr);
    CHECK(jtu->remove(*js, APPLICATION_PACKAGE_ATTRIBUTES_TABLE, suiteAttribute));

    js->commitTransaction();
    js->close();
    registry.reset(0);
    LOG(EJavaStorage, EInfo, "-TestAttributes");
}

/**
 * Test CJavaRegistryEntry::NumberOfCertificateChains() method.
 * 1. Test when suite no cert chains. Size is 0.
 * 2. Test when suite one cert chains. Size is 1.
 * 3. Test when suite three cert chains. size is 3.
 */
TEST(TestRegistryEntry, TestNumberOfCertificateChains)
{
    LOG(EJavaStorage, EInfo, "+TestNumberOfCertificateChains");

    JavaStorageApplicationEntry_t midlet1;
    JavaStorageEntry attr;

    Uid suite1Uid(L"[e22bf577]");

    attr.setEntry(PACKAGE_ID, suite1Uid.toString());
    midlet1.insert(attr);

    Uid midlet1Uid(L"[e22bf588]");

    attr.setEntry(ID, midlet1Uid.toString());
    midlet1.insert(attr);

    js->open();
    js->startTransaction();
    CHECK(jtu->populate(*js, APPLICATION_TABLE, midlet1));

    JavaStorageApplicationEntry_t suite1;
    attr.setEntry(PACKAGE_NAME, L"TestCertChains");
    suite1.insert(attr);

    attr.setEntry(ID, suite1Uid.toString());
    suite1.insert(attr);
    attr.setEntry(MEDIA_ID, L"-124614446");
    suite1.insert(attr);

    CHECK(jtu->populate(*js, APPLICATION_PACKAGE_TABLE, suite1));
    js->commitTransaction();
    js->close();
    auto_ptr<CJavaRegistry> registry(CJavaRegistry::NewL());

    TUid suiteUid;
    uidToTUid(suite1Uid, suiteUid);

    // 1. Test when no cert chains. Size is 0.
    CJavaRegistryEntry* entry = registry->RegistryEntryL(suiteUid);
    CHECK(entry != NULL);
    CHECK(entry->NumberOfCertificateChains() == 0);

    delete entry;
    entry = NULL;

    TUid midletUid;
    uidToTUid(midlet1Uid, midletUid);

    // 1.2 Test no certs via app.
    entry = registry->RegistryEntryL(midletUid);
    CHECK(entry != NULL);
    CHECK(entry->NumberOfCertificateChains() == 0);

    delete entry;
    entry = NULL;

    // 2. Test when one cert chains. Size is 1.

    JavaStorageApplicationEntry_t certEntry;
    JavaStorageApplicationEntry_t validcertsEntry;

    js->open();
    validcertsEntry = createValidCertsEntry(suite1Uid.toString(), L"1");
    js->write(MIDP_PACKAGE_TABLE, validcertsEntry);

    certEntry = createEntry(suite1Uid.toString(),
                            L"MIDlet-Certificate-1-1",
                            L"abc123");

    js->write(APPLICATION_PACKAGE_ATTRIBUTES_TABLE, certEntry);
    js->close();

    entry = registry->RegistryEntryL(suiteUid);
    CHECK(entry != NULL);
    CHECK(entry->NumberOfCertificateChains() == 1);

    delete entry;
    entry = NULL;

    // 2.2 Test no certs via app.
    entry = registry->RegistryEntryL(midletUid);
    CHECK(entry != NULL);
    CHECK(entry->NumberOfCertificateChains() == 1);

    delete entry;
    entry = NULL;

    js->open();
    js->startTransaction();
    CHECK(jtu->remove(*js, APPLICATION_PACKAGE_ATTRIBUTES_TABLE, certEntry));
    CHECK(jtu->remove(*js, MIDP_PACKAGE_TABLE, validcertsEntry));
    js->commitTransaction();
    js->close();


    // 3. Test when three cert chains. size is 3.
    certEntry.clear();
    validcertsEntry.clear();

    validcertsEntry = createValidCertsEntry(suite1Uid.toString(), L"1,2,3");

    js->open();
    js->write(MIDP_PACKAGE_TABLE, validcertsEntry);

    certEntry = createEntry(suite1Uid.toString(),
                            L"MIDlet-Certificate-1-1",
                            L"abc123");

    js->write(APPLICATION_PACKAGE_ATTRIBUTES_TABLE, certEntry);

    JavaStorageApplicationEntry_t certEntry2;
    certEntry2 = createEntry(suite1Uid.toString(),
                             L"MIDlet-Certificate-2-1",
                             L"def456");

    js->write(APPLICATION_PACKAGE_ATTRIBUTES_TABLE, certEntry2);

    JavaStorageApplicationEntry_t certEntry3;
    certEntry3 = createEntry(suite1Uid.toString(),
                             L"MIDlet-Certificate-3-1",
                             L"fooBar");

    js->write(APPLICATION_PACKAGE_ATTRIBUTES_TABLE, certEntry3);
    js->close();

    entry = registry->RegistryEntryL(suiteUid);
    CHECK(entry != NULL);
    CHECK(entry->NumberOfCertificateChains() == 3);

    delete entry;
    entry = NULL;

    // 2.2 Test no certs via app.
    entry = registry->RegistryEntryL(midletUid);
    CHECK(entry != NULL);
    CHECK(entry->NumberOfCertificateChains() == 3);

    delete entry;
    entry = NULL;

    js->open();
    js->startTransaction();
    CHECK(jtu->remove(*js, APPLICATION_PACKAGE_ATTRIBUTES_TABLE, certEntry));
    CHECK(jtu->remove(*js, APPLICATION_PACKAGE_ATTRIBUTES_TABLE, certEntry2));
    CHECK(jtu->remove(*js, APPLICATION_PACKAGE_ATTRIBUTES_TABLE, certEntry3));
    CHECK(jtu->remove(*js, MIDP_PACKAGE_TABLE, validcertsEntry));
    CHECK(jtu->remove(*js, APPLICATION_TABLE, midlet1));
    CHECK(jtu->remove(*js, APPLICATION_PACKAGE_TABLE, suite1));
    js->commitTransaction();
    js->close();
    LOG(EJavaStorage, EInfo, "-TestNumberOfCertificateChains");
}

/**
 * Test CJavaRegistryEntry::CertificateChain() method.
 * 1. Test when suite no cert chains.
 * 2. Test when suite one cert chains.
 * 3. Test when suite three cert chains.
 * 4. Test index greater than.
 * 5. Test index less than i.e. -1.
 * 6. Test install application suite and do CX509CertChain::DecodeCertsL().
 */
TEST(TestRegistryEntry, TestCertificateChain)
{
    LOG(EJavaStorage, EInfo, "+TestCertificateChain");

    JavaStorageApplicationEntry_t midlet1;
    JavaStorageEntry attr;

    Uid suite1Uid(L"[e22bf577]");

    attr.setEntry(PACKAGE_ID, suite1Uid.toString());
    midlet1.insert(attr);

    Uid midlet1Uid(L"[e22bf588]");

    attr.setEntry(ID, midlet1Uid.toString());
    midlet1.insert(attr);

    js->open();
    js->startTransaction();
    CHECK(jtu->populate(*js, APPLICATION_TABLE, midlet1));

    JavaStorageApplicationEntry_t suite1;
    attr.setEntry(PACKAGE_NAME, L"TestCertChains");
    suite1.insert(attr);

    attr.setEntry(ID, suite1Uid.toString());
    suite1.insert(attr);
    attr.setEntry(MEDIA_ID, L"-124614446");
    suite1.insert(attr);

    CHECK(jtu->populate(*js, APPLICATION_PACKAGE_TABLE, suite1));
    js->commitTransaction();
    js->close();
    auto_ptr<CJavaRegistry> registry(CJavaRegistry::NewL());

    TUid suiteUid;
    uidToTUid(suite1Uid, suiteUid);

    // 1. Test when no cert chains. Size is 0.
    CJavaRegistryEntry* entry = registry->RegistryEntryL(suiteUid);
    CHECK(entry != NULL);
    CHECK(entry->NumberOfCertificateChains() == 0);
    CHECK(entry->CertificateChain(0) == KNullDesC8);

    delete entry;
    entry = NULL;

    TUid midletUid;
    uidToTUid(midlet1Uid, midletUid);

    // 1.2 Test no certs via app.
    entry = registry->RegistryEntryL(midletUid);
    CHECK(entry != NULL);
    CHECK(entry->NumberOfCertificateChains() == 0);
    CHECK(entry->CertificateChain(0) == KNullDesC8);

    delete entry;
    entry = NULL;

    // 2. Test when one cert chains. Size is 1.
    JavaStorageApplicationEntry_t certEntry;
    JavaStorageApplicationEntry_t validcertsEntry;

    js->open();
    validcertsEntry = createValidCertsEntry(suite1Uid.toString(), L"1");
    js->write(MIDP_PACKAGE_TABLE, validcertsEntry);

    certEntry = createEntry(suite1Uid.toString(),
                            L"MIDlet-Certificate-1-1",
                            L"YWJjMTIz"); // abc123 as base64 encoded.

    js->write(APPLICATION_PACKAGE_ATTRIBUTES_TABLE, certEntry);
    js->close();

    _LIT8(KCertChain1, "abc123");

    entry = registry->RegistryEntryL(suiteUid);
    CHECK(entry != NULL);
    CHECK(entry->NumberOfCertificateChains() == 1);
    CHECK(entry->CertificateChain(0).Compare(KCertChain1) == 0);

    delete entry;
    entry = NULL;

    // 2.2 Test no certs via app.
    entry = registry->RegistryEntryL(midletUid);
    CHECK(entry != NULL);
    CHECK(entry->NumberOfCertificateChains() == 1);
    CHECK(entry->CertificateChain(0).Compare(KCertChain1) == 0);

    delete entry;
    entry = NULL;

    js->open();
    js->startTransaction();
    CHECK(jtu->remove(*js, APPLICATION_PACKAGE_ATTRIBUTES_TABLE, certEntry));
    CHECK(jtu->remove(*js, MIDP_PACKAGE_TABLE, validcertsEntry));
    js->commitTransaction();
    js->close();


    // 3. Test when three cert chains. size is 3.
    certEntry.clear();
    validcertsEntry.clear();

    validcertsEntry = createValidCertsEntry(suite1Uid.toString(), L"1,2,3");

    js->open();
    js->write(MIDP_PACKAGE_TABLE, validcertsEntry);

    certEntry = createEntry(suite1Uid.toString(),
                            L"MIDlet-Certificate-1-1",
                            L"YWJjMTIz");  // abc123

    js->write(APPLICATION_PACKAGE_ATTRIBUTES_TABLE, certEntry);

    JavaStorageApplicationEntry_t certEntry2;
    certEntry2 = createEntry(suite1Uid.toString(),
                             L"MIDlet-Certificate-2-1",
                             L"ZGVmNDU2");  // def456 as base 64 encoded.

    js->write(APPLICATION_PACKAGE_ATTRIBUTES_TABLE, certEntry2);

    JavaStorageApplicationEntry_t certEntry3;
    certEntry3 = createEntry(suite1Uid.toString(),
                             L"MIDlet-Certificate-3-1",
                             L"Zm9vQmFy");  // fooBar

    js->write(APPLICATION_PACKAGE_ATTRIBUTES_TABLE, certEntry3);
    js->close();

    entry = registry->RegistryEntryL(suiteUid);
    CHECK(entry != NULL);
    CHECK(entry->NumberOfCertificateChains() == 3);
    CHECK(entry->CertificateChain(0).Compare(KCertChain1) == 0);
    _LIT8(KCertChain2, "def456");
    CHECK(entry->CertificateChain(1).Compare(KCertChain2) == 0);
    _LIT8(KCertChain3, "fooBar");
    CHECK(entry->CertificateChain(2).Compare(KCertChain3) == 0);

    delete entry;
    entry = NULL;

    // 2.2 Test no certs via app.
    entry = registry->RegistryEntryL(midletUid);
    CHECK(entry != NULL);
    CHECK(entry->NumberOfCertificateChains() == 3);
    CHECK(entry->CertificateChain(0).Compare(KCertChain1) == 0);
    CHECK(entry->CertificateChain(1).Compare(KCertChain2) == 0);
    CHECK(entry->CertificateChain(2).Compare(KCertChain3) == 0);

    // 4. Test index greater than.
    CHECK(entry->CertificateChain(3) == KNullDesC8);

    // 5. Test index less than i.e. -1.
    CHECK(entry->CertificateChain(-1) == KNullDesC8);

    delete entry;
    entry = NULL;

    js->open();
    js->startTransaction();
    CHECK(jtu->remove(*js, APPLICATION_PACKAGE_ATTRIBUTES_TABLE, certEntry));
    CHECK(jtu->remove(*js, APPLICATION_PACKAGE_ATTRIBUTES_TABLE, certEntry2));
    CHECK(jtu->remove(*js, APPLICATION_PACKAGE_ATTRIBUTES_TABLE, certEntry3));
    CHECK(jtu->remove(*js, MIDP_PACKAGE_TABLE, validcertsEntry));
    CHECK(jtu->remove(*js, APPLICATION_TABLE, midlet1));
    CHECK(jtu->remove(*js, APPLICATION_PACKAGE_TABLE, suite1));
    js->commitTransaction();
    js->close();

    // 6. Test install application suite and do CX509CertChain::DecodeCertsL()
    RProcess rJavaInstaller;
    TFileName fileName;
    TBuf<400> commandLine;

    // Build command line used to pass all necessary info to Java Installer
    std::auto_ptr<HBufC> installerStarterDll(
        stringToDes(java::runtime::JAVA_INSTALLER_STARTER_DLL));
    commandLine = installerStarterDll->Des();
    commandLine.Append(_L(" install -jad=c:\\Signed.jad"));
    commandLine.Append(_L(" -silent -upgrade=yes -ocsp=no"));
    commandLine.Append(_L(" -download=no -upgrade_data=yes"));

    // start JavaInstaller
    std::auto_ptr<HBufC> installerProcess(
        stringToDes(java::runtime::JAVA_PROCESS));
    TInt err = rJavaInstaller.Create(installerProcess->Des(), commandLine);

    TRequestStatus status;

    if (KErrNone == err)
    {
        LOG(EJavaStorage, EInfo, "Calling Logon");
        // This call will wait until Java Installer exits (or panics)
        rJavaInstaller.Logon(status);

        LOG(EJavaStorage, EInfo, "Calling Resume");
        rJavaInstaller.Resume();
    }
    else
    {
        ELOG1(EJavaStorage, "Cannot start Installer, error %d", err);
        User::Leave(err);
    }

    LOG(EJavaStorage, EInfo, "Calling RProcess::Close");
    // free resources before returning
    rJavaInstaller.Close();

    // now wait until Java Installer exits
    User::WaitForRequest(status);

    registry.reset(CJavaRegistry::NewL());
    RArray<TUid> uids;
    registry->GetRegistryEntryUidsL(EGeneralPackage, uids);

    CHECK(uids.Count() == 1); // One app installed

    CJavaRegistryEntry* regEntry = registry->RegistryEntryL(uids[0]);
    CHECK(regEntry != NULL);
    CHECK(regEntry->NumberOfCertificateChains() > 0);

    const TDesC8& chain = regEntry->CertificateChain(0);

    auto_ptr<CX509CertChain> theChain(new(ELeave)CX509CertChain());

    CArrayPtrFlat<CX509Certificate>* certsArray =
        theChain->DecodeCertsL(chain);
    CHECK(certsArray->Count() > 0);  // This verifies X509 decode succeeded.

    certsArray->ResetAndDestroy();
    delete regEntry;
    regEntry = NULL;
    registry.reset(0);

    // UNINSTALL
    // Build command line used to pass all necessary info to Java Installer
    commandLine = installerStarterDll->Des();
    commandLine.Append(_L(" uninstall -uid="));
    TUidName uidName = (uids[0]).Name();
    commandLine.Append(uidName);
    uids.Reset();
    uids.Close();
    commandLine.Append(_L(" -silent"));

    // start JavaInstaller
    err = rJavaInstaller.Create(installerProcess->Des(), commandLine);

    status = KRequestPending;

    if (KErrNone == err)
    {
        LOG(EJavaStorage, EInfo, "Calling Logon");
        // This call will wait until Java Installer exits (or panics)
        rJavaInstaller.Logon(status);

        LOG(EJavaStorage, EInfo, "Calling Resume");
        rJavaInstaller.Resume();
    }
    else
    {
        ELOG1(EJavaStorage, "Cannot start Installer, error %d", err);
        User::Leave(err);
    }

    LOG(EJavaStorage, EInfo, "Calling RProcess::Close");
    // free resources before returning
    rJavaInstaller.Close();

    // now wait until Java Installer exits
    User::WaitForRequest(status);

    LOG(EJavaStorage, EInfo, "-TestCertificateChain");
}

/**
 * Test CJavaRegistryEntry::DrmContentId() method.
 * 1. Test DrmContentId no ContentId.
 * 2. Test DrmContentId DRM protected file SuiteEntry.
 * 3. Test DrmContentId DRM protected file AppEntry.
 */
TEST(TestRegistryEntry, TestDrmContentId)
{
    LOG(EJavaStorage, EInfo, "+TestDrmContentId");

    JavaStorageApplicationEntry_t midlet1;
    JavaStorageEntry attr;

    Uid suite1Uid(L"[e22ba577]");

    attr.setEntry(PACKAGE_ID, suite1Uid.toString());
    midlet1.insert(attr);

    Uid midlet1Uid(L"[e22ba588]");

    attr.setEntry(ID, midlet1Uid.toString());
    midlet1.insert(attr);

    js->open();
    js->startTransaction();
    CHECK(jtu->populate(*js, APPLICATION_TABLE, midlet1));

    JavaStorageApplicationEntry_t suite1;
    attr.setEntry(PACKAGE_NAME, L"MySuite");
    suite1.insert(attr);

    attr.setEntry(JAR_PATH, L"c:\\HelloWorld1.jar");

    suite1.insert(attr);

    attr.setEntry(ID, suite1Uid.toString());
    suite1.insert(attr);
    attr.setEntry(MEDIA_ID, L"-124614446");
    suite1.insert(attr);

    CHECK(jtu->populate(*js, APPLICATION_PACKAGE_TABLE, suite1));
    js->commitTransaction();

    auto_ptr<CJavaRegistry> registry(CJavaRegistry::NewL());

    // 1. Test DrmContentId no DRM protected file.
    TUid suiteUid;
    uidToTUid(suite1Uid, suiteUid);

    CJavaRegistryEntry* entry = registry->RegistryEntryL(suiteUid);
    CHECK(entry != NULL);
    CHECK(entry->DrmContentId() == KNullDesC);

    delete entry;
    entry = NULL;

    JavaStorageApplicationEntry_t updateEntry;
    attr.setEntry(JAR_PATH, L"c:\\HelloWorld2.jar");
    updateEntry.insert(attr);
    attr.setEntry(CONTENT_ID, L"MyGreatContentId");
    updateEntry.insert(attr);

    // 2. Test DrmContentId DRM protected file SuiteEntry.
    js->startTransaction();
    CHECK(jtu->update(*js, APPLICATION_PACKAGE_TABLE, updateEntry, suite1));
    js->commitTransaction();

    entry = registry->RegistryEntryL(suiteUid);
    CHECK(entry != NULL);
    _LIT(KContentId, "MyGreatContentId");
    CHECK(entry->DrmContentId().Compare(KContentId) == 0);

    delete entry;
    entry = NULL;

    // 3. Test DrmContentId DRM protected file AppEntry.
    TUid midletUid;
    uidToTUid(midlet1Uid, midletUid);

    entry = registry->RegistryEntryL(midletUid);
    CHECK(entry != NULL);
    CHECK(entry->DrmContentId().Compare(KContentId) == 0);

    delete entry;
    entry = NULL;

    js->startTransaction();
    CHECK(jtu->remove(*js, APPLICATION_TABLE, midlet1));
    CHECK(jtu->remove(*js, APPLICATION_PACKAGE_TABLE, updateEntry));
    js->commitTransaction();
    js->close();
    LOG(EJavaStorage, EInfo, "-TestDrmContentId");
}

const JavaStorageApplicationEntry_t createEntry(const wstring& aUid,
        const wstring& aName,
        const wstring& aValue)
{
    JavaStorageApplicationEntry_t entry;
    JavaStorageEntry attr;

    attr.setEntry(ID, aUid);
    entry.insert(attr);

    attr.setEntry(NAME, aName);
    entry.insert(attr);

    attr.setEntry(VALUE, aValue);
    entry.insert(attr);

    return entry;
}

const JavaStorageApplicationEntry_t createValidCertsEntry(const wstring& aUid,
        const wstring& aCerts)
{
    JavaStorageApplicationEntry_t entry;
    JavaStorageEntry attr;

    attr.setEntry(ID, aUid);
    entry.insert(attr);

    attr.setEntry(VALID_CERTS, aCerts);
    entry.insert(attr);

    return entry;
}

