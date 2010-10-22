#include <stdio.h>
#include <string>
#include "db.h"

#define SELECT "SELECT author, title, rating, count FROM books \
WHERE author like ? OR title like ?"

Db::Db(void)
{
	int rc;

	/* Open DB */
	printf("Using SQLite %s\n", sqlite3_libversion());
	rc = sqlite3_open_v2("books.db", &m_db, SQLITE_OPEN_READWRITE, NULL);
	if (rc) {
		fprintf(stderr, "Couldn't open database: %s\n", sqlite3_errmsg(m_db));
		sqlite3_close(m_db);
		m_db = NULL;
		m_stmt = NULL;
		return;
	}

	/* Prepare statement */
	rc = sqlite3_prepare_v2(m_db, SELECT, -1, &m_stmt, NULL);
	if (rc) {
		fprintf(stderr, "Couldn't prepare statement: %s\n",
				sqlite3_errmsg(m_db));
		sqlite3_finalize(m_stmt); /* FIXME Does that make sense? */
		sqlite3_close(m_db);
		m_stmt = NULL;
		m_db = NULL;
	}

/* 	db = malloc(sizeof(db_t));
	db->db = sqlitedb;
	db->stmt = stmt; */
}

const char *Db::version(void)
{
	return sqlite3_libversion();
}

int Db::query(std::string _str)
{
	int rc;
	std::string val;

	if (!m_db || !m_stmt) {
		return 1;
	}

	val = "%" + _str + "%";
	rc = sqlite3_bind_text(m_stmt, 1, val.c_str(), -1, SQLITE_TRANSIENT);
	if (rc) {
		fprintf(stderr, "Couldn't bind values: %s\n", sqlite3_errmsg(m_db));
		return 1;
	}
	rc = sqlite3_bind_text(m_stmt, 2, val.c_str(), -1, SQLITE_TRANSIENT);
	if (rc) {
		fprintf(stderr, "Couldn't bind values: %s\n", sqlite3_errmsg(m_db));
		return 1;
	}

	return 0;
}

int Db::next(std::string *_author, std::string *_title,
			 int *_rating, int *_copies)
{
	int rc;
	const char *author, *title;
	int rating, copies;

	if (!m_db || !m_stmt) {
		return 1;
	}

	rc = sqlite3_step(m_stmt);
	if (rc == SQLITE_DONE) {
		rc = sqlite3_reset(m_stmt);
		if (rc) { /* FIXME Check against SQLITE_DONE? */
			fprintf(stderr, "Couldn't reset statement: %s\n",
					sqlite3_errmsg(m_db));
			return 1;
		}
		return 1;
	} else if (rc != SQLITE_ROW) {
		fprintf(stderr, "Couldn't read next result (%d): %s\n", rc,
				sqlite3_errmsg(m_db));
		return 1;
	}

	author = (const char *) sqlite3_column_text(m_stmt, 0);
	title = (const char *) sqlite3_column_text(m_stmt, 1);
	rating = sqlite3_column_int(m_stmt, 2);
	copies = sqlite3_column_int(m_stmt, 3);
	if (author && title) {
		*_author = author;
		*_title = title;
		*_rating = rating;
		*_copies = copies;
/* 		strcpy(_author, (char *) author);
		strcpy(_title, (char *) title); */
		return 0;
	}

	return 1;
}

Db::~Db(void)
{
	int rc;

	if (m_stmt) {
		rc = sqlite3_finalize(m_stmt);
		if (rc) { /* FIXME Check against SQLITE_DONE? */
			fprintf(stderr, "Couldn't delete statement: %s\n",
					sqlite3_errmsg(m_db));
		}
	}

	if (m_db) {
		sqlite3_close(m_db);
	}
	printf("DB safely closed\n");
}
