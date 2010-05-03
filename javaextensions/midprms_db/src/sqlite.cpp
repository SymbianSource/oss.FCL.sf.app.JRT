/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: SQLite wrapper
*
*/

#include <string>
#include <sqlite3.h>

#include "com_nokia_mj_impl_rms_SQLite.h"
#include "javajniutils.h"
#include "logger.h"

std::string jstringToString(JNIEnv* aEnv, const jstring& aString);

/*
 * Class:     com_nokia_mj_impl_rms_SQLite
 * Method:    sqlite3_open
 * Signature: (Ljava/lang/String;)I
 */
JNIEXPORT jint JNICALL Java_com_nokia_mj_impl_rms_SQLite_sqlite3_1open
(JNIEnv* aEnv, jclass, jstring aFilename)
{
    std::string filename = jstringToString(aEnv, aFilename);
    sqlite3* db = 0;
    int rc = sqlite3_open(filename.c_str(), &db);

    if (rc)
    {
        std::string error = sqlite3_errmsg(db);
        ELOG3(EMidpRms, "sqlite3_open failed (%d): %s (db:%s)",
              rc, error.c_str(), filename.c_str());
        sqlite3_close(db);
        java::util::JniUtils::throwNewException(aEnv,
                                                "javax/microedition/rms/RecordStoreException", error);
        return 0;
    }
    return reinterpret_cast<jint>(db);
}

/*
 * Class:     com_nokia_mj_impl_rms_SQLite
 * Method:    sqlite3_close
 * Signature: (I)I
 */
JNIEXPORT jint JNICALL Java_com_nokia_mj_impl_rms_SQLite_sqlite3_1close
(JNIEnv*, jclass, jint aDbHandle)
{
    sqlite3* db = reinterpret_cast<sqlite3*>(aDbHandle);
    int rc = sqlite3_close(db);
    return rc;
}

/*
 * Class:     com_nokia_mj_impl_rms_SQLite
 * Method:    sqlite3_exec
 * Signature: (ILjava/lang/String;)I
 */
JNIEXPORT jint JNICALL Java_com_nokia_mj_impl_rms_SQLite_sqlite3_1exec
(JNIEnv* aEnv, jclass, jint aDbHandle, jstring aStatement)
{
    sqlite3* db = reinterpret_cast<sqlite3*>(aDbHandle);
    std::string statement = jstringToString(aEnv, aStatement);

    char* errmsg = 0;
    int rc = sqlite3_exec(db, statement.c_str(), 0, 0, &errmsg);
    if (rc)
    {
        ELOG3(EMidpRms, "sqlite3_exec failed (%d): %s (sql:%s)",
              rc , errmsg, statement.c_str());
    }
    sqlite3_free(errmsg);

    return rc;
}

/*
 * Class:     com_nokia_mj_impl_rms_SQLite
 * Method:    sqlite3_prepare_v2
 * Signature: (ILjava/lang/String;)I
 */
JNIEXPORT jint JNICALL Java_com_nokia_mj_impl_rms_SQLite_sqlite3_1prepare_1v2
(JNIEnv* aEnv, jclass, jint aDbHandle, jstring aStatement)
{
    sqlite3* db = reinterpret_cast<sqlite3*>(aDbHandle);
    std::string statement = jstringToString(aEnv, aStatement);

    sqlite3_stmt* ppStmt = 0;
    const char* tail = 0;
    int rc = sqlite3_prepare_v2(db, statement.c_str(), statement.size(), &ppStmt, &tail);

    if (rc)
    {
        ELOG3(EMidpRms, "sqlite3_prepare_v2 failed (%d): %s (sql:%s)",
              rc, sqlite3_errmsg(db), statement.c_str());
    }

    return reinterpret_cast<int>(ppStmt);
}

/*
 * Class:     com_nokia_mj_impl_rms_SQLite
 * Method:    sqlite3_step
 * Signature: (I)I
 */
JNIEXPORT jint JNICALL Java_com_nokia_mj_impl_rms_SQLite_sqlite3_1step
(JNIEnv*, jclass, jint aStmtHandle)
{
    sqlite3_stmt* ppStmt = reinterpret_cast<sqlite3_stmt*>(aStmtHandle);
    int rc = sqlite3_step(ppStmt);
    return rc;
}

/*
 * Class:     com_nokia_mj_impl_rms_SQLite
 * Method:    sqlite3_reset
 * Signature: (I)I
 */
JNIEXPORT jint JNICALL Java_com_nokia_mj_impl_rms_SQLite_sqlite3_1reset
(JNIEnv*, jclass, jint aStmtHandle)
{
    sqlite3_stmt* ppStmt = reinterpret_cast<sqlite3_stmt*>(aStmtHandle);
    int rc = sqlite3_reset(ppStmt);
    return rc;
}

/*
 * Class:     com_nokia_mj_impl_rms_SQLite
 * Method:    sqlite3_finalize
 * Signature: (I)I
 */
JNIEXPORT jint JNICALL Java_com_nokia_mj_impl_rms_SQLite_sqlite3_1finalize
(JNIEnv*, jclass, jint aStmtHandle)
{
    sqlite3_stmt* ppStmt = reinterpret_cast<sqlite3_stmt*>(aStmtHandle);
    int rc = sqlite3_finalize(ppStmt);
    return rc;
}

/*
 * Class:     com_nokia_mj_impl_rms_SQLite
 * Method:    sqlite3_changes
 * Signature: (I)I
 */
JNIEXPORT jint JNICALL Java_com_nokia_mj_impl_rms_SQLite_sqlite3_1changes
(JNIEnv *, jclass, jint aDbHandle)
{
    sqlite3* db = reinterpret_cast<sqlite3*>(aDbHandle);
    int rc = sqlite3_changes(db);
    return rc;
}

/*
 * Class:     com_nokia_mj_impl_rms_SQLite
 * Method:    sqlite3_errmsg
 * Signature: (I)Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_com_nokia_mj_impl_rms_SQLite_sqlite3_1errmsg
(JNIEnv* aEnv, jclass, jint aDbHandle)
{
    sqlite3* db = reinterpret_cast<sqlite3*>(aDbHandle);
    const char* errmsg = sqlite3_errmsg(db);
    jstring str = aEnv->NewStringUTF(errmsg);
    return str;
}

/*
 * Class:     com_nokia_mj_impl_rms_SQLite
 * Method:    sqlite3_bind_blob
 * Signature: (II[B)I
 */
JNIEXPORT jint JNICALL Java_com_nokia_mj_impl_rms_SQLite_sqlite3_1bind_1blob
(JNIEnv* aEnv, jclass, jint aStmtHandle, jint aIndex, jbyteArray aArray)
{
    sqlite3_stmt* ppStmt = reinterpret_cast<sqlite3_stmt*>(aStmtHandle);

    int len = 0;
    char* blob = 0;
    if (aArray)
    {
        len = aEnv->GetArrayLength(aArray);
        blob = new char[len];
        if (!blob)
        {
            return SQLITE_NOMEM;
        }
        aEnv->GetByteArrayRegion(aArray, 0, len, reinterpret_cast<jbyte*>(blob));
    }
    int rc = sqlite3_bind_blob(ppStmt, aIndex, blob, len, SQLITE_TRANSIENT);
    delete[] blob;

    return rc;
}

/*
 * Class:     com_nokia_mj_impl_rms_SQLite
 * Method:    sqlite3_bind_int
 * Signature: (III)I
 */
JNIEXPORT jint JNICALL Java_com_nokia_mj_impl_rms_SQLite_sqlite3_1bind_1int
(JNIEnv*, jclass, jint aStmtHandle, jint aIndex, jint aValue)
{
    sqlite3_stmt* ppStmt = reinterpret_cast<sqlite3_stmt*>(aStmtHandle);
    int rc = sqlite3_bind_int(ppStmt, aIndex, aValue);
    return rc;
}

/*
 * Class:     com_nokia_mj_impl_rms_SQLite
 * Method:    sqlite3_bind_int64
 * Signature: (IIJ)I
 */
JNIEXPORT jint JNICALL Java_com_nokia_mj_impl_rms_SQLite_sqlite3_1bind_1int64
(JNIEnv*, jclass, jint aStmtHandle, jint aIndex, jlong aValue)
{
    sqlite3_stmt* ppStmt = reinterpret_cast<sqlite3_stmt*>(aStmtHandle);
    int rc = sqlite3_bind_int64(ppStmt, aIndex, aValue);
    return rc;
}

/*
 * Class:     com_nokia_mj_impl_rms_SQLite
 * Method:    sqlite3_bind_text
 * Signature: (ILjava/lang/String;)I
 */
JNIEXPORT jint JNICALL Java_com_nokia_mj_impl_rms_SQLite_sqlite3_1bind_1text
(JNIEnv* aEnv, jclass, jint aStmtHandle, jint aIndex, jstring aText)
{
    sqlite3_stmt* ppStmt = reinterpret_cast<sqlite3_stmt*>(aStmtHandle);
    std::string text = jstringToString(aEnv, aText);
    int rc = sqlite3_bind_text(ppStmt, aIndex, text.c_str(), text.size(), SQLITE_TRANSIENT);
    return rc;
}

/*
 * Class:     com_nokia_mj_impl_rms_SQLite
 * Method:    sqlite3_column_blob
 * Signature: (II)[B
 */
JNIEXPORT jbyteArray JNICALL Java_com_nokia_mj_impl_rms_SQLite_sqlite3_1column_1blob
(JNIEnv* aEnv, jclass, jint aStmtHandle, jint aIndex)
{
    sqlite3_stmt* ppStmt = reinterpret_cast<sqlite3_stmt*>(aStmtHandle);
    const void* blob = sqlite3_column_blob(ppStmt, aIndex);
    int len = sqlite3_column_bytes(ppStmt, aIndex);

    jbyteArray bytes = aEnv->NewByteArray(len);
    aEnv->SetByteArrayRegion(bytes, 0, len, (jbyte*)blob);
    return bytes;
}

/*
 * Class:     com_nokia_mj_impl_rms_SQLite
 * Method:    sqlite3_column_int
 * Signature: (II)I
 */
JNIEXPORT jint JNICALL Java_com_nokia_mj_impl_rms_SQLite_sqlite3_1column_1int
(JNIEnv*, jclass, jint aStmtHandle, jint aIndex)
{
    sqlite3_stmt* ppStmt = reinterpret_cast<sqlite3_stmt*>(aStmtHandle);
    int rc = sqlite3_column_int(ppStmt, aIndex);
    return rc;
}

/*
 * Class:     com_nokia_mj_impl_rms_SQLite
 * Method:    sqlite3_column_int64
 * Signature: (II)J
 */
JNIEXPORT jlong JNICALL Java_com_nokia_mj_impl_rms_SQLite_sqlite3_1column_1int64
(JNIEnv*, jclass, jint aStmtHandle, jint aIndex)
{
    sqlite3_stmt* ppStmt = reinterpret_cast<sqlite3_stmt*>(aStmtHandle);
    jlong rc = sqlite3_column_int64(ppStmt, aIndex);
    return rc;
}

/*
 * Class:     com_nokia_mj_impl_rms_SQLite
 * Method:    sqlite3_column_text
 * Signature: (II)Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_com_nokia_mj_impl_rms_SQLite_sqlite3_1column_1text
(JNIEnv* aEnv, jclass, jint aStmtHandle, jint aIndex)
{
    sqlite3_stmt* ppStmt = reinterpret_cast<sqlite3_stmt*>(aStmtHandle);
    char* text = (char*)sqlite3_column_text(ppStmt, aIndex);

    jstring str = aEnv->NewStringUTF(text);
    return str;
}

/*
 * Class:     com_nokia_mj_impl_rms_SQLite
 * Method:    sqlite3_last_insert_rowid
 * Signature: (I)J
 */
JNIEXPORT jlong JNICALL Java_com_nokia_mj_impl_rms_SQLite_sqlite3_1last_1insert_1rowid
(JNIEnv *, jclass, jint aDbHandle)
{
    sqlite3* db = reinterpret_cast<sqlite3*>(aDbHandle);
    sqlite_int64 rc = sqlite3_last_insert_rowid(db);
    return rc;
}

/*
 * Class:     com_nokia_mj_impl_rms_SQLite
 * Method:    sqlite3_busy_timeout
 * Signature: (II)I
 */
JNIEXPORT jint JNICALL Java_com_nokia_mj_impl_rms_SQLite_sqlite3_1busy_1timeout
(JNIEnv *, jclass, jint aDbHandle, jint aMilliseconds)
{
    sqlite3* db = reinterpret_cast<sqlite3*>(aDbHandle);
    int rc = sqlite3_busy_timeout(db, aMilliseconds);
    return rc;
}

std::string jstringToString(JNIEnv* aEnv, const jstring& aString)
{
    const char* buf = aEnv->GetStringUTFChars(aString, 0);
    std::string result = "";
    if (buf)
    {
        result = buf;
    }
    aEnv->ReleaseStringUTFChars(aString, buf);
    return result;
}

