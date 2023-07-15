#!/usr/bin/env python

import sqlite3

conn = sqlite3.connect('/keystore/sedsprovider.db')

cursor = conn.cursor()
#cursor.execute("SELECT count(*) FROM key_record_info")
cursor.execute("SELECT key_identifier FROM key_record_info ORDER BY key_identifier")

rows = cursor.fetchall()

for row in rows:
	print(row)

