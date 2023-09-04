#!/usr/bin/env python

'''
/onefs/isilon/lib/isi_km_server/kmserver/sqlite_database_v1.cpp

#define KEY_RECORD_TABLE_NAME "key_record_info"
#define KEY_RECORD_FIELD_KEYIDENTIFIER "key_identifier"
#define KEY_RECORD_FIELD_DOMAINTYPE "domaintype"
#define KEY_RECORD_FIELD_KEYLENGTH "key_length"
#define KEY_RECORD_FIELD_KEYCONTENT "key_content"
#define KEY_RECORD_FIELD_ENCRYPTING_KEY_ID "encrypting_key_id"
#define KEY_RECORD_FIELD_ENCRYPTION_IV "encryption_iv"
'''

import sqlite3

conn = sqlite3.connect('/keystore/sedsprovider.db')

cursor = conn.cursor()
#cursor.execute("SELECT count(*) FROM key_record_info")
#cursor.execute("SELECT key_identifier FROM key_record_info ORDER BY key_identifier")
cursor.execute("SELECT key_identifier, \
			domaintype, \
			key_length, \
			key_content \
			FROM key_record_info ORDER BY key_identifier")

rows = cursor.fetchall()

for row in rows:
	print(row)

