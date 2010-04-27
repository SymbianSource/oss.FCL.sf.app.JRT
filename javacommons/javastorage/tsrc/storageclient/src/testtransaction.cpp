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
#include <memory>

#include "TestHarness.h"

#include "javastorage.h"
#include "javastorageexception.h"
#include "javastoragenames.h"
#include "storagetestutils.h"

using namespace std;
using namespace java::storage;

TEST_GROUP(TestTransaction)
{
    JavaStorage* js;
    JavaStorageTestUtils* jtu;

    TEST_SETUP()
    {
        js = JavaStorage::createInstance();
        jtu = new JavaStorageTestUtils();
    }
    TEST_TEARDOWN()
    {
        try
        {
            js->rollbackTransaction();  // If transaction is left active.

            js->open(); // MultiTr test case does commit and if it fails
            // stuff must be manually removed.
            JavaStorageApplicationEntry_t defaultEntry;
            jtu->createDefaultEntry(defaultEntry);
            jtu->remove(*js, APPLICATION_PACKAGE_TABLE, defaultEntry);
            js->close();
        }
        catch (...)
        {
            // No can do
        }

        delete js;
        js = 0;
        delete jtu;
        jtu = 0;
    }
};

/**
 * 1. Start transaction when no connection. Silently ignored.
 * 2. Start transaction when active connection. Transaction is created.
 * 3. Test two consecutive startTransactions(). Second call is silently ignored.
 * 4. Test transactions can be used multiple times. Close active transaction
 *    using commit and initiate new one.
 * 5. Test multiple transactions can be used. Close active transaction using
 *    rollback and initiate new one.
 * 6. Start transaction after close. Silently ignored.
 */
TEST(TestTransaction, StartTransaction)
{
    // 1. Start without connection
    try
    {
        js->startTransaction();
    }
    catch (...)
    {
        FAIL("Exception while start transaction without open connection\n");
    }

    // 2. Start transaction
    try
    {
        js->open();
        js->startTransaction();
    }
    catch (...)
    {
        FAIL("Exception while starting transaction\n");
    }

    // 3. Two consecutive calls. Second call is silently ignored
    try
    {
        js->startTransaction();
    }
    catch (...)
    {
        FAIL("Exception while starting consecutive transaction\n");
    }

    // 4. Multiple times. Commit and new one
    try
    {
        js->commitTransaction();
        js->startTransaction();
    }
    catch (...)
    {
        FAIL("Exception while re-starting transaction after commit\n");
    }

    // 5. Multiple times. rollback and new one
    try
    {
        js->rollbackTransaction();
        js->startTransaction();
    }
    catch (...)
    {
        FAIL("Exception while re-starting transaction after rollback\n");
    }

    js->close();

    // 6. Start transaction after close.
    try
    {
        js->startTransaction();
    }
    catch (...)
    {
        FAIL("Exception after close when starting transaction\n");
    }
}

/**
 * 1. Commit without open connection. Commit is silently ignored.
 * 2. Rollback without open connection. Rollback is silently ignored.
 * 3. Test commit
 * 4. Test two consecutive commits. Second call is silently ignored.
 * 5. Test rollback.
 * 6. Test two consecutive rollbacks. Ssecond call is silently ignored.
 * 7. Test close connection and then commit. Call is silently ignored.
 * 8. Test close connection and then rollback. Call is silently ignored.
 * 9. Test commit and then rollback. Rollback is silenlty ignored.
 * 10. Test rollback and then commit. Commit is silently ignored.
 */
TEST(TestTransaction, CommitTransaction)
{
    string dbName = JAVA_DATABASE_NAME;
    string tableName = APPLICATION_PACKAGE_TABLE;
    JavaStorageEntry entry;

    JavaStorageApplicationEntry_t idEntry;
    entry.setEntry(ID, L"TestCommitTR1");
    idEntry.insert(entry);

    try
    {
        js->open(dbName);
        JavaStorageApplicationEntry_t defaultEntry;
        jtu->createDefaultEntry(defaultEntry);
        entry.setEntry(ID, L"TestCommitTR1");
        defaultEntry.insert(entry);

        // Pre-populate DB
        CHECK(jtu->populate(*js, tableName, defaultEntry));
        CHECK(jtu->checkParams(*js, tableName, defaultEntry, 1));
        js->close();
    }
    catch (...)
    {
        FAIL("Failed to open database\n");
    }

    // 1. Commit without connection
    try
    {
        js->commitTransaction();
    }
    catch (...)
    {
        FAIL("Exception while commit without open connection\n");
    }

    // 2. Rollback without connection
    try
    {
        js->rollbackTransaction();
    }
    catch (...)
    {
        FAIL("Exception while rollback without open connection\n");
    }

    JavaStorageApplicationEntry_t extensionEntry;

    entry.setEntry(JAD_PATH, L"/usr/local/bin/");
    extensionEntry.insert(entry);

    entry.setEntry(JAR_PATH, L"/usr/share/private/[abcs12345]/");
    extensionEntry.insert(entry);

    js->open(dbName);
    js->startTransaction();

    CHECK(jtu->update(*js, tableName, extensionEntry, idEntry));
    CHECK(jtu->checkParams(*js, tableName, extensionEntry, 1));

    // 3. commit and 4. test two consecutive commits
    // Check data written to DB. First close connection and initiate new
    // connection and read same data.
    try
    {
        js->commitTransaction();
        js->commitTransaction();
        js->close();
        js->open(dbName);
        CHECK(jtu->checkParams(*js, tableName, extensionEntry, 1));
    }
    catch (JavaStorageException& jse)
    {
        string failureStr = "UnExp Exp steps 3. and 4.: ";
        failureStr += jse.toString();
        FAIL(failureStr.c_str());
    }

    js->startTransaction();
    CHECK(jtu->remove(*js, tableName, extensionEntry));
    js->commitTransaction();

    // 5. Rollback and 6. two consecutive rollbacks
    js->startTransaction();

    CHECK(jtu->update(*js, tableName, extensionEntry, idEntry));

    js->rollbackTransaction();
    js->rollbackTransaction();

    // Check rollback worked
    CHECK_EQUAL(jtu->checkParams(*js, tableName, extensionEntry, 1), false);

    js->close();
    js->open(dbName);

    // Check rollback worked also after close
    CHECK_EQUAL(jtu->checkParams(*js, tableName, extensionEntry, 1), false);

    // 7. Connection close and commit
    js->startTransaction();
    js->close();
    try
    {
        js->commitTransaction();
    }
    catch (...)
    {
        FAIL("Exception while commit after close.\n");
    }

    // 8. Connection close and rollback
    js->open(dbName);
    js->startTransaction();
    js->close();
    try
    {
        js->rollbackTransaction();
    }
    catch (...)
    {
        FAIL("Exception while rollback after close.\n");
    }

    // 9. Commit and then rollback
    js->open(dbName);
    js->startTransaction();

    JavaStorageApplicationEntry_t defaultEntry;
    jtu->createDefaultEntry(defaultEntry);
    entry.setEntry(ID, L"TestCommitTR1");
    defaultEntry.insert(entry);

    CHECK(jtu->populate(*js, tableName, defaultEntry));
    CHECK(jtu->checkParams(*js, tableName, defaultEntry, 1));

    CHECK(jtu->update(*js, tableName, extensionEntry, idEntry));
    // Check data is really written to DB
    CHECK(jtu->checkParams(*js, tableName, extensionEntry, 1));

    js->commitTransaction();
    js->rollbackTransaction();

    // Check data is really written to DB and not rollbacked
    CHECK(jtu->checkParams(*js, tableName, extensionEntry, 1));
    js->close();
    js->open(dbName);
    CHECK(jtu->checkParams(*js, tableName, extensionEntry, 1));
    CHECK(jtu->remove(*js, tableName, extensionEntry));

    // 10. Rollback and commit
    js->startTransaction();

    // Restore deleted data and then update it
    CHECK(jtu->populate(*js, tableName, defaultEntry));
    CHECK(jtu->checkParams(*js, tableName, defaultEntry, 1));
    CHECK(jtu->update(*js, tableName, extensionEntry, idEntry));
    // Check data is really written to DB
    CHECK(jtu->checkParams(*js, tableName, extensionEntry, 1));

    js->rollbackTransaction();
    js->commitTransaction();

    // Check rollback worked also after close
    CHECK_EQUAL(jtu->checkParams(*js, tableName, extensionEntry, 1), false);
    js->close();
    js->open(dbName);
    // Check rollback worked also after close
    CHECK_EQUAL(jtu->checkParams(*js, tableName, extensionEntry, 1), false);

    js->open(dbName);
    CHECK(jtu->remove(*js, tableName, defaultEntry));
    js->close();
}

/**
 * Test force rollback conditions.
 * 1. Test rollback is done if connection with open transaction is closed.
 * 2. Test rollback is done if storage object is deleted while having
 *    connection.
 */
TEST(TestTransaction, ForceRollback)
{
    string tableName = APPLICATION_PACKAGE_TABLE;
    JavaStorageEntry entry;

    // 1. Test rollback is done if connection with open transaction is closed.
    js->open();
    js->startTransaction();

    JavaStorageApplicationEntry_t defaultEntry;
    jtu->createDefaultEntry(defaultEntry);

    entry.setEntry(ID, L"TestForceRollback1");
    defaultEntry.insert(entry);

    CHECK(jtu->populate(*js, tableName, defaultEntry));
    CHECK(jtu->checkParams(*js, tableName, defaultEntry, 1));

    js->close();
    js->open();
    // Check force rollback is done.
    CHECK(jtu->checkParams(*js, tableName, defaultEntry, 0));
    js->close();

    // 2. Test rollback is done if storage object is deleted while having
    //    connection.
    js->open();
    js->startTransaction();

    defaultEntry.clear();
    jtu->createDefaultEntry(defaultEntry);
    entry.setEntry(ID, L"TestForceRollback2");
    defaultEntry.insert(entry);

    CHECK(jtu->populate(*js, tableName, defaultEntry));
    CHECK(jtu->checkParams(*js, tableName, defaultEntry, 1));

    // Delete triggers force rollback if transaction is active.
    delete js;
    js = NULL;

    js = JavaStorage::createInstance();
    js->open();
    // Check force rollback is done.
    CHECK(jtu->checkParams(*js, tableName, defaultEntry, 0));
    js->close();
}

/**
 * 1. Test two transactions to same target. They're not seeing each others
 *    changes before commit.
 *    Test sequence;
 *     1. Start first transaction.
 *     2. Add data using first transaction.
 *     3. Start second transaction.
 *     4. Check transaction 1 added data is not visible.
 *     5. Add data using second transaction.
 *     6. Check data is not visible to transaction 1.
 *     7. Commit both transactions.
 *     8. Check both connections can see each other's data.
 */
TEST(TestTransaction, MultiTransaction)
{
    string dbName = JAVA_DATABASE_NAME;
    string tableName = APPLICATION_PACKAGE_TABLE;
    auto_ptr<JavaStorage> js2(0);
    JavaStorageEntry entry;

    JavaStorageApplicationEntry_t idEntry;
    entry.setEntry(ID, L"MultiTransaction1");
    idEntry.insert(entry);

    try
    {
        js->open(dbName);

        // Write and read to verify connection
        JavaStorageApplicationEntry_t defaultEntry;
        jtu->createDefaultEntry(defaultEntry);
        entry.setEntry(ID, L"MultiTransaction1");
        defaultEntry.insert(entry);

        CHECK(jtu->populate(*js, tableName, defaultEntry));

        js2.reset(JavaStorage::createInstance());
        js2->open(dbName);
        // Opened to same target. Not yet transactions.
        CHECK(jtu->checkParams(*js2, tableName, defaultEntry, 1));
    }
    catch (...)
    {
        FAIL("Failed to open database\n");
    }

    // 1.1. Start first transaction.
    js->startTransaction();

    JavaStorageApplicationEntry_t extensionEntry;
    entry.setEntry(JAD_PATH, L"/usr/local/bin/");
    extensionEntry.insert(entry);

    entry.setEntry(JAR_PATH, L"/usr/share/private/[abcs12345]/");
    extensionEntry.insert(entry);

    // 1.2. Add data using first transaction.
    CHECK(jtu->update(*js, tableName, extensionEntry, idEntry));
    // Check data is really written to DB
    CHECK(jtu->checkParams(*js, tableName, extensionEntry, 1));


    // 1.4. Check transaction 1 added data is not visible.
    CHECK(jtu->checkParams(*js2, tableName, extensionEntry, 0));

    JavaStorageApplicationEntry_t extensionEntry2;

    entry.setEntry(MEDIA_ID, L"MMC");
    extensionEntry2.insert(entry);

    entry.setEntry(ACCESS_POINT, L"home.bin");
    extensionEntry2.insert(entry);

    // 1.5 commit first transaction before second transaction starts
    js->commitTransaction();

    // 1.6. Start second transaction.
    js2->startTransaction();

    // 1.7. Add data using second transaction.
    entry.setEntry(ID, L"ExtensionUid1");
    extensionEntry2.insert(entry);

    CHECK(jtu->populate(*js2, tableName, extensionEntry2));
    // Check data is really written to DB
    CHECK(jtu->checkParams(*js2, tableName, extensionEntry2, 1));

    // 1.8. Check data is not visible to transaction 1.
    CHECK(jtu->checkParams(*js, tableName, extensionEntry2, 0));

    // 1.9. Commit second transaction.
    js2->commitTransaction();

    // 2.0. Check both connections can see each other's data.
    CHECK(jtu->checkParams(*js, tableName, extensionEntry2, 1));
    CHECK(jtu->checkParams(*js2, tableName, extensionEntry, 1));

    // Clean
    CHECK(jtu->remove(*js, tableName, extensionEntry));
    CHECK(jtu->remove(*js2, tableName, extensionEntry2));

    js->close();
    js2->close();
}
