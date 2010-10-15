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
* Description:  ?Description
*
*/

#include "TestHarness.h"

#include "midpauthenticationmodule.h"
#include "javastoragenames.h"
#include "javastorage.h"
#include "javastorageexception.h"
#include "javaoslayer.h"
#include "javacommonutils.h"
#include "javauid.h"
#include <memory>

using namespace java::security;
using namespace std;
using namespace java::util;
using namespace java::storage;

const JavaStorageApplicationEntry_t createEntry(const wstring& aUid,
        const wstring& aName,
        const wstring& aValue);

const JavaStorageApplicationEntry_t createValidCertsEntry(
    const wstring& aUid,
    const wstring& aCerts);

TEST_GROUP(TestCertChains)
{
    JavaStorage* js;

    TEST_SETUP()
    {
        js = JavaStorage::createInstance();
    }

    TEST_TEARDOWN()
    {
        delete js;
        js = 0;
    }
};

/**
 * Test MIDPAuthenticationModule::getCertChains() method.
 *
 * 1. Test no data in storage.
 * 2. Test certs value '1'
 * 3. Test certs value '1,3'
 * 4. Test certs value '1,3,4,5'
 * 5. Test certs value '1,,2'. Empty is ignored. Valid chains returned.
 * 6. Test certs value '1,A,2'. A is ignored. Valid chains returned.
 * 7. Test certs value '1,2,3,'
 * 8. Test certs value ',1,2'
 * 9. Test certs value '1,2,A'. A is ignored.
 * 10. Test certs value '1' when chain contains multiple certs.
 * 11. Test certs value '1,3' when chains contain multiple certs.
 * 12. Test certs value '1 '. Value is trimmed and chain returned.
 * 13. Test certs value ' 1'. Value is trimmed and chain returned.
 * 14. Test certs value '1' but no certs belogning to chain. Empty response.
 */
TEST(TestCertChains, TestCertChains)
{
    auto_ptr<MIDPAuthenticationModule>authMod(
        MIDPAuthenticationModule::createInstance());

    Uid suite1Uid(L"[e00f4577]");
    list<string> chains;

    JavaStorageEntry attr;
    JavaStorageApplicationEntry_t entry;
    JavaStorageApplicationEntry_t certEntry;

    // 1. Test no data in storage.
    try
    {
        authMod->getCertChains(suite1Uid, chains);
        CHECK(chains.size() == 0);
    }
    catch (JavaStorageException& aJse)
    {
        string es = "UnExp1: " + aJse.toString();
        FAIL(es.c_str());
    }

    // 2. Test certs value '1'
    try
    {
        chains.clear();
        entry.clear();
        certEntry.clear();
        entry = createValidCertsEntry(suite1Uid.toString(), L"1");

        js->open();
        js->write(MIDP_PACKAGE_TABLE, entry);

        certEntry = createEntry(suite1Uid.toString(),
                                L"MIDlet-Certificate-1-1",
                                L"YWJjMTIz");

        js->write(APPLICATION_PACKAGE_ATTRIBUTES_TABLE, certEntry);

        JavaStorageApplicationEntry_t foolEntry;
        attr.setEntry(ID, suite1Uid.toString());
        foolEntry.insert(attr);

        attr.setEntry(NAME, L"Vendor");
        foolEntry.insert(attr);

        attr.setEntry(VALUE, L"1.0.2");
        foolEntry.insert(attr);

        js->write(APPLICATION_PACKAGE_ATTRIBUTES_TABLE, foolEntry);

        authMod->getCertChains(suite1Uid, chains);
        js->remove(MIDP_PACKAGE_TABLE, entry);
        js->remove(APPLICATION_PACKAGE_ATTRIBUTES_TABLE, certEntry);
        js->remove(APPLICATION_PACKAGE_ATTRIBUTES_TABLE, foolEntry);
        
        CHECK(chains.size() == 1);
        string refString("YWJjMTIz");
        CHECK(JavaCommonUtils::base64encode(chains.front()) == refString);
    }
    catch (JavaStorageException& aJse)
    {
        string es = "UnExp2: " + aJse.toString();
        FAIL(es.c_str());
    }

    js->close();

    // 3. Test certs value '1,3'
    try
    {
        chains.clear();
        entry.clear();
        certEntry.clear();

        entry = createValidCertsEntry(suite1Uid.toString(), L"1,3");

        js->open();
        js->write(MIDP_PACKAGE_TABLE, entry);

        certEntry = createEntry(suite1Uid.toString(),
                                L"MIDlet-Certificate-1-1",
                                L"YWJjMTIz");

        js->write(APPLICATION_PACKAGE_ATTRIBUTES_TABLE, certEntry);

        JavaStorageApplicationEntry_t certEntry2;
        certEntry2 = createEntry(suite1Uid.toString(),
                                 L"MIDlet-Certificate-3-1",
                                 L"ZGVmNDU2");

        js->write(APPLICATION_PACKAGE_ATTRIBUTES_TABLE, certEntry2);

        authMod->getCertChains(suite1Uid, chains);
        js->remove(MIDP_PACKAGE_TABLE, entry);
        js->remove(APPLICATION_PACKAGE_ATTRIBUTES_TABLE, certEntry);
        js->remove(APPLICATION_PACKAGE_ATTRIBUTES_TABLE, certEntry2);

        CHECK(chains.size() == 2);

        list<string>::const_iterator iter = chains.begin();

        string cert1Ref("YWJjMTIz");
        CHECK(JavaCommonUtils::base64encode((*iter++)) == cert1Ref);
        string cert2Ref("ZGVmNDU2");
        CHECK(JavaCommonUtils::base64encode((*iter++)) == cert2Ref);
    }
    catch (JavaStorageException& aJse)
    {
        string es = "UnExp3: " + aJse.toString();
        FAIL(es.c_str());
    }

    js->close();

    // 4. Test certs value '1,3,4,5'
    try
    {
        chains.clear();
        entry.clear();
        certEntry.clear();

        entry = createValidCertsEntry(suite1Uid.toString(), L"1,3,4,5");

        js->open();
        js->write(MIDP_PACKAGE_TABLE, entry);

        certEntry = createEntry(suite1Uid.toString(),
                                L"MIDlet-Certificate-1-1",
                                L"YWJjMTIz");

        js->write(APPLICATION_PACKAGE_ATTRIBUTES_TABLE, certEntry);

        JavaStorageApplicationEntry_t certEntry2;
        certEntry2 = createEntry(suite1Uid.toString(),
                                 L"MIDlet-Certificate-3-1",
                                 L"ZGVmNDU2");

        js->write(APPLICATION_PACKAGE_ATTRIBUTES_TABLE, certEntry2);

        JavaStorageApplicationEntry_t certEntry3;
        certEntry3 = createEntry(suite1Uid.toString(),
                                 L"MIDlet-Certificate-4-1",
                                 L"Zm9vQmFy");

        js->write(APPLICATION_PACKAGE_ATTRIBUTES_TABLE, certEntry3);

        JavaStorageApplicationEntry_t certEntry4;
        certEntry4 = createEntry(suite1Uid.toString(),
                                 L"MIDlet-Certificate-5-1",
                                 L"SGVsbG9Xb3JsZA==");

        js->write(APPLICATION_PACKAGE_ATTRIBUTES_TABLE, certEntry4);

        authMod->getCertChains(suite1Uid, chains);
        js->remove(MIDP_PACKAGE_TABLE, entry);
        js->remove(APPLICATION_PACKAGE_ATTRIBUTES_TABLE, certEntry);
        js->remove(APPLICATION_PACKAGE_ATTRIBUTES_TABLE, certEntry2);
        js->remove(APPLICATION_PACKAGE_ATTRIBUTES_TABLE, certEntry3);
        js->remove(APPLICATION_PACKAGE_ATTRIBUTES_TABLE, certEntry4);

        CHECK(chains.size() == 4);

        list<string>::const_iterator iter = chains.begin();

        string cert1Ref("YWJjMTIz");
        CHECK(JavaCommonUtils::base64encode((*iter++)) == cert1Ref);
        string cert2Ref("ZGVmNDU2");
        CHECK(JavaCommonUtils::base64encode((*iter++)) == cert2Ref);
        string cert3Ref("Zm9vQmFy");
        CHECK(JavaCommonUtils::base64encode((*iter++)) == cert3Ref);
        string cert4Ref("SGVsbG9Xb3JsZA==");
        CHECK(JavaCommonUtils::base64encode((*iter++)) == cert4Ref);
    }
    catch (JavaStorageException& aJse)
    {
        string es = "UnExp4: " + aJse.toString();
        FAIL(es.c_str());
    }

    js->close();

    // 5. Test certs value '1,,2'
    try
    {
        chains.clear();
        entry.clear();
        certEntry.clear();

        entry = createValidCertsEntry(suite1Uid.toString(), L"1,,2");

        js->open();
        js->write(MIDP_PACKAGE_TABLE, entry);

        certEntry = createEntry(suite1Uid.toString(),
                                L"MIDlet-Certificate-1-1",
                                L"YWJjMTIz");

        js->write(APPLICATION_PACKAGE_ATTRIBUTES_TABLE, certEntry);

        JavaStorageApplicationEntry_t certEntry2;
        certEntry2 = createEntry(suite1Uid.toString(),
                                 L"MIDlet-Certificate-2-1",
                                 L"ZGVmNDU2");

        js->write(APPLICATION_PACKAGE_ATTRIBUTES_TABLE, certEntry2);

        authMod->getCertChains(suite1Uid, chains);
        js->remove(MIDP_PACKAGE_TABLE, entry);
        js->remove(APPLICATION_PACKAGE_ATTRIBUTES_TABLE, certEntry);
        js->remove(APPLICATION_PACKAGE_ATTRIBUTES_TABLE, certEntry2);

        CHECK(chains.size() == 2);

        list<string>::const_iterator iter = chains.begin();

        string cert1Ref("YWJjMTIz");
        CHECK(JavaCommonUtils::base64encode((*iter++)) == cert1Ref);
        string cert2Ref("ZGVmNDU2");
        CHECK(JavaCommonUtils::base64encode((*iter++)) == cert2Ref);
    }
    catch (JavaStorageException& aJse)
    {
        string es = "UnExp5: " + aJse.toString();
        FAIL(es.c_str());
    }

    js->close();

    // 6. Test certs value '1,A,2'
    try
    {
        chains.clear();
        entry.clear();
        certEntry.clear();

        entry = createValidCertsEntry(suite1Uid.toString(), L"1,A,2");

        js->open();
        js->write(MIDP_PACKAGE_TABLE, entry);

        certEntry = createEntry(suite1Uid.toString(),
                                L"MIDlet-Certificate-1-1",
                                L"YWJjMTIz");

        js->write(APPLICATION_PACKAGE_ATTRIBUTES_TABLE, certEntry);

        JavaStorageApplicationEntry_t certEntry2;
        certEntry2 = createEntry(suite1Uid.toString(),
                                 L"MIDlet-Certificate-2-1",
                                 L"ZGVmNDU2");

        js->write(APPLICATION_PACKAGE_ATTRIBUTES_TABLE, certEntry2);

        authMod->getCertChains(suite1Uid, chains);
        js->remove(MIDP_PACKAGE_TABLE, entry);
        js->remove(APPLICATION_PACKAGE_ATTRIBUTES_TABLE, certEntry);
        js->remove(APPLICATION_PACKAGE_ATTRIBUTES_TABLE, certEntry2);

        CHECK(chains.size() == 2);

        list<string>::const_iterator iter = chains.begin();

        string cert1Ref("YWJjMTIz");
        CHECK(JavaCommonUtils::base64encode((*iter++)) == cert1Ref);
        string cert2Ref("ZGVmNDU2");
        CHECK(JavaCommonUtils::base64encode((*iter++)) == cert2Ref);
    }
    catch (JavaStorageException& aJse)
    {
        string es = "UnExp6: " + aJse.toString();
        FAIL(es.c_str());
    }

    js->close();

    // 7. Test certs value '1,2,3,'
    try
    {
        chains.clear();
        entry.clear();
        certEntry.clear();

        entry = createValidCertsEntry(suite1Uid.toString(), L"1,2,3,");

        js->open();
        js->write(MIDP_PACKAGE_TABLE, entry);

        certEntry = createEntry(suite1Uid.toString(),
                                L"MIDlet-Certificate-1-1",
                                L"YWJjMTIz");

        js->write(APPLICATION_PACKAGE_ATTRIBUTES_TABLE, certEntry);

        JavaStorageApplicationEntry_t certEntry2;
        certEntry2 = createEntry(suite1Uid.toString(),
                                 L"MIDlet-Certificate-2-1",
                                 L"ZGVmNDU2");

        js->write(APPLICATION_PACKAGE_ATTRIBUTES_TABLE, certEntry2);

        JavaStorageApplicationEntry_t certEntry3;
        certEntry3 = createEntry(suite1Uid.toString(),
                                 L"MIDlet-Certificate-3-1",
                                 L"Zm9vQmFy");

        js->write(APPLICATION_PACKAGE_ATTRIBUTES_TABLE, certEntry3);

        authMod->getCertChains(suite1Uid, chains);
        js->remove(MIDP_PACKAGE_TABLE, entry);
        js->remove(APPLICATION_PACKAGE_ATTRIBUTES_TABLE, certEntry);
        js->remove(APPLICATION_PACKAGE_ATTRIBUTES_TABLE, certEntry2);
        js->remove(APPLICATION_PACKAGE_ATTRIBUTES_TABLE, certEntry3);

        CHECK(chains.size() == 3);

        list<string>::const_iterator iter = chains.begin();

        string cert1Ref("YWJjMTIz");
        CHECK(JavaCommonUtils::base64encode((*iter++)) == cert1Ref);
        string cert2Ref("ZGVmNDU2");
        CHECK(JavaCommonUtils::base64encode((*iter++)) == cert2Ref);
        string cert3Ref("Zm9vQmFy");
        CHECK(JavaCommonUtils::base64encode((*iter++)) == cert3Ref);
    }
    catch (JavaStorageException& aJse)
    {
        string es = "UnExp7: " + aJse.toString();
        FAIL(es.c_str());
    }

    js->close();

    // 8. Test certs value ',1,2'
    try
    {
        chains.clear();
        entry.clear();
        certEntry.clear();

        entry = createValidCertsEntry(suite1Uid.toString(), L",1,2");

        js->open();
        js->write(MIDP_PACKAGE_TABLE, entry);

        certEntry = createEntry(suite1Uid.toString(),
                                L"MIDlet-Certificate-1-1",
                                L"YWJjMTIz");

        js->write(APPLICATION_PACKAGE_ATTRIBUTES_TABLE, certEntry);

        JavaStorageApplicationEntry_t certEntry2;
        certEntry2 = createEntry(suite1Uid.toString(),
                                 L"MIDlet-Certificate-2-1",
                                 L"ZGVmNDU2");

        js->write(APPLICATION_PACKAGE_ATTRIBUTES_TABLE, certEntry2);

        authMod->getCertChains(suite1Uid, chains);
        js->remove(MIDP_PACKAGE_TABLE, entry);
        js->remove(APPLICATION_PACKAGE_ATTRIBUTES_TABLE, certEntry);
        js->remove(APPLICATION_PACKAGE_ATTRIBUTES_TABLE, certEntry2);

        CHECK(chains.size() == 2);

        list<string>::const_iterator iter = chains.begin();

        string cert1Ref("YWJjMTIz");
        CHECK(JavaCommonUtils::base64encode((*iter++)) == cert1Ref);
        string cert2Ref("ZGVmNDU2");
        CHECK(JavaCommonUtils::base64encode((*iter++)) == cert2Ref);
    }
    catch (JavaStorageException& aJse)
    {
        string es = "UnExp8: " + aJse.toString();
        FAIL(es.c_str());
    }

    js->close();

    // 9. Test certs value '1,2,A'
    try
    {
        chains.clear();
        entry.clear();
        certEntry.clear();

        entry = createValidCertsEntry(suite1Uid.toString(), L"1,2,A");

        js->open();
        js->write(MIDP_PACKAGE_TABLE, entry);

        certEntry = createEntry(suite1Uid.toString(),
                                L"MIDlet-Certificate-1-1",
                                L"YWJjMTIz");

        js->write(APPLICATION_PACKAGE_ATTRIBUTES_TABLE, certEntry);

        JavaStorageApplicationEntry_t certEntry2;
        certEntry2 = createEntry(suite1Uid.toString(),
                                 L"MIDlet-Certificate-2-1",
                                 L"ZGVmNDU2");

        js->write(APPLICATION_PACKAGE_ATTRIBUTES_TABLE, certEntry2);

        authMod->getCertChains(suite1Uid, chains);
        js->remove(MIDP_PACKAGE_TABLE, entry);
        js->remove(APPLICATION_PACKAGE_ATTRIBUTES_TABLE, certEntry);
        js->remove(APPLICATION_PACKAGE_ATTRIBUTES_TABLE, certEntry2);

        CHECK(chains.size() == 2);

        list<string>::const_iterator iter = chains.begin();

        string cert1Ref("YWJjMTIz");
        CHECK(JavaCommonUtils::base64encode((*iter++)) == cert1Ref);
        string cert2Ref("ZGVmNDU2");
        CHECK(JavaCommonUtils::base64encode((*iter++)) == cert2Ref);
    }
    catch (JavaStorageException& aJse)
    {
        string es = "UnExp9: " + aJse.toString();
        FAIL(es.c_str());
    }

    js->close();

    // 10. Test certs value '1' when chain contains multiple certs.
    try
    {
        chains.clear();
        entry.clear();
        certEntry.clear();
        entry = createValidCertsEntry(suite1Uid.toString(), L"1");

        js->open();
        js->write(MIDP_PACKAGE_TABLE, entry);

        certEntry = createEntry(suite1Uid.toString(),
                                L"MIDlet-Certificate-1-1",
                                L"YWJjMTIz");

        js->write(APPLICATION_PACKAGE_ATTRIBUTES_TABLE, certEntry);

        JavaStorageApplicationEntry_t certEntry2;
        certEntry2 = createEntry(suite1Uid.toString(),
                                 L"MIDlet-Certificate-1-2",
                                 L"ZmliOA==");

        js->write(APPLICATION_PACKAGE_ATTRIBUTES_TABLE, certEntry2);

        JavaStorageApplicationEntry_t certEntry3;
        certEntry3 = createEntry(suite1Uid.toString(),
                                 L"MIDlet-Certificate-1-3",
                                 L"RA==");

        js->write(APPLICATION_PACKAGE_ATTRIBUTES_TABLE, certEntry3);

        authMod->getCertChains(suite1Uid, chains);
        js->remove(MIDP_PACKAGE_TABLE, entry);
        js->remove(APPLICATION_PACKAGE_ATTRIBUTES_TABLE, certEntry);
        js->remove(APPLICATION_PACKAGE_ATTRIBUTES_TABLE, certEntry2);
        js->remove(APPLICATION_PACKAGE_ATTRIBUTES_TABLE, certEntry3);

        CHECK(chains.size() == 1);
        string refString("YWJjMTIzZmliOEQ=");
        CHECK(JavaCommonUtils::base64encode(chains.front()) == refString);
    }
    catch (JavaStorageException& aJse)
    {
        string es = "UnExp10: " + aJse.toString();
        FAIL(es.c_str());
    }

    js->close();

    // 11. Test certs value '1,3' when chains contain multiple certs.
    try
    {
        chains.clear();
        entry.clear();
        certEntry.clear();
        entry = createValidCertsEntry(suite1Uid.toString(), L"1,3");

        js->open();
        js->write(MIDP_PACKAGE_TABLE, entry);

        certEntry = createEntry(suite1Uid.toString(),
                                L"MIDlet-Certificate-1-1",
                                L"U2Vl");

        js->write(APPLICATION_PACKAGE_ATTRIBUTES_TABLE, certEntry);

        JavaStorageApplicationEntry_t certEntry2;
        certEntry2 = createEntry(suite1Uid.toString(),
                                 L"MIDlet-Certificate-1-2",
                                 L"Tm8=");

        js->write(APPLICATION_PACKAGE_ATTRIBUTES_TABLE, certEntry2);

        JavaStorageApplicationEntry_t certEntry3;
        certEntry3 = createEntry(suite1Uid.toString(),
                                 L"MIDlet-Certificate-1-3",
                                 L"TW9yZQ==");

        js->write(APPLICATION_PACKAGE_ATTRIBUTES_TABLE, certEntry3);

        JavaStorageApplicationEntry_t certEntry4;

        certEntry4 = createEntry(suite1Uid.toString(),
                                 L"MIDlet-Certificate-3-1",
                                 L"Me");

        js->write(APPLICATION_PACKAGE_ATTRIBUTES_TABLE, certEntry4);

        JavaStorageApplicationEntry_t certEntry5;
        certEntry5 = createEntry(suite1Uid.toString(),
                                 L"MIDlet-Certificate-3-2",
                                 L"And");

        js->write(APPLICATION_PACKAGE_ATTRIBUTES_TABLE, certEntry5);

        JavaStorageApplicationEntry_t certEntry6;
        certEntry6 = createEntry(suite1Uid.toString(),
                                 L"MIDlet-Certificate-3-3",
                                 L"My");

        js->write(APPLICATION_PACKAGE_ATTRIBUTES_TABLE, certEntry6);

        authMod->getCertChains(suite1Uid, chains);
        js->remove(MIDP_PACKAGE_TABLE, entry);
        js->remove(APPLICATION_PACKAGE_ATTRIBUTES_TABLE, certEntry);
        js->remove(APPLICATION_PACKAGE_ATTRIBUTES_TABLE, certEntry2);
        js->remove(APPLICATION_PACKAGE_ATTRIBUTES_TABLE, certEntry3);
        js->remove(APPLICATION_PACKAGE_ATTRIBUTES_TABLE, certEntry4);
        js->remove(APPLICATION_PACKAGE_ATTRIBUTES_TABLE, certEntry5);
        js->remove(APPLICATION_PACKAGE_ATTRIBUTES_TABLE, certEntry6);

        CHECK(chains.size() == 1);

        list<string>::const_iterator iter = chains.begin();

        string cert1Ref("U2VlTm9Nb3Jl");
        CHECK(JavaCommonUtils::base64encode((*iter++)) == cert1Ref);
    }
    catch (JavaStorageException& aJse)
    {
        string es = "UnExp11: " + aJse.toString();
        FAIL(es.c_str());
    }

    js->close();

    // 12. Test certs value '1 '
    try
    {
        chains.clear();
        entry.clear();
        certEntry.clear();
        entry = createValidCertsEntry(suite1Uid.toString(), L"1 ");

        js->open();
        js->write(MIDP_PACKAGE_TABLE, entry);

        certEntry = createEntry(suite1Uid.toString(),
                                L"MIDlet-Certificate-1-1",
                                L"YWJjMTIz");

        js->write(APPLICATION_PACKAGE_ATTRIBUTES_TABLE, certEntry);

        JavaStorageApplicationEntry_t foolEntry;
        attr.setEntry(ID, suite1Uid.toString());
        foolEntry.insert(attr);

        attr.setEntry(NAME, L"Vendor");
        foolEntry.insert(attr);

        attr.setEntry(VALUE, L"1.0.2");
        foolEntry.insert(attr);

        js->write(APPLICATION_PACKAGE_ATTRIBUTES_TABLE, foolEntry);

        authMod->getCertChains(suite1Uid, chains);
        js->remove(MIDP_PACKAGE_TABLE, entry);
        js->remove(APPLICATION_PACKAGE_ATTRIBUTES_TABLE, certEntry);
        js->remove(APPLICATION_PACKAGE_ATTRIBUTES_TABLE, foolEntry);

        CHECK(chains.size() == 1);

        string refString("YWJjMTIz");
        CHECK(JavaCommonUtils::base64encode(chains.front()) == refString);
    }
    catch (JavaStorageException& aJse)
    {
        string es = "UnExp2: " + aJse.toString();
        FAIL(es.c_str());
    }

    // 13. Test certs value ' 1'
    try
    {
        chains.clear();
        entry.clear();
        certEntry.clear();
        entry = createValidCertsEntry(suite1Uid.toString(), L" 1");

        js->open();
        js->write(MIDP_PACKAGE_TABLE, entry);

        certEntry = createEntry(suite1Uid.toString(),
                                L"MIDlet-Certificate-1-1",
                                L"YWJjMTIz");

        js->write(APPLICATION_PACKAGE_ATTRIBUTES_TABLE, certEntry);

        JavaStorageApplicationEntry_t foolEntry;
        attr.setEntry(ID, suite1Uid.toString());
        foolEntry.insert(attr);

        attr.setEntry(NAME, L"Vendor");
        foolEntry.insert(attr);

        attr.setEntry(VALUE, L"1.0.2");
        foolEntry.insert(attr);

        js->write(APPLICATION_PACKAGE_ATTRIBUTES_TABLE, foolEntry);

        authMod->getCertChains(suite1Uid, chains);
        js->remove(MIDP_PACKAGE_TABLE, entry);
        js->remove(APPLICATION_PACKAGE_ATTRIBUTES_TABLE, certEntry);
        js->remove(APPLICATION_PACKAGE_ATTRIBUTES_TABLE, foolEntry);

        CHECK(chains.size() == 1);

        string refString("YWJjMTIz");
        CHECK(JavaCommonUtils::base64encode(chains.front()) == refString);
    }
    catch (JavaStorageException& aJse)
    {
        string es = "UnExp13: " + aJse.toString();
        FAIL(es.c_str());
    }

    js->close();

    // 14. Test certs value '1' but no certs belogning to chain.
    try
    {
        chains.clear();
        entry.clear();
        certEntry.clear();
        entry = createValidCertsEntry(suite1Uid.toString(), L"1");

        js->open();
        js->write(MIDP_PACKAGE_TABLE, entry);

        authMod->getCertChains(suite1Uid, chains);
        js->remove(MIDP_PACKAGE_TABLE, entry);

        CHECK(chains.size() == 0);
    }
    catch (JavaStorageException& aJse)
    {
        string es = "UnExp14: " + aJse.toString();
        FAIL(es.c_str());
    }

    js->close();
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
