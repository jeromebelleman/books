#include <stdio.h>
#include <iostream>
#include <string>
#include <vector>
#include "db.h"

#define LOOKUP "SELECT author, title, rating, count FROM books \
WHERE author LIKE ? OR title LIKE ?"
#define LOOKUPSTRICT "SELECT author, title, rating, count FROM books \
WHERE author = ? AND title = ?"
#define AUTHORS "SELECT DISTINCT author FROM books"
#define TITLES "SELECT DISTINCT title FROM books"
#define INSERT "INSERT INTO books (author, title, rating, count) \
VALUES (?, ?, ?, ?)"
#define UPDATE "UPDATE books SET author = ?, title = ?, rating = ?, count = ? \
WHERE author = ? AND title = ?"
#define DELETE "DELETE FROM books WHERE author = ? AND title = ?"

Db::Db(const std::string& _path)
	: m_stmts(STMTC)
{
	int rc;

	/* Open DB */
	rc = sqlite3_open_v2(_path.c_str(), &m_db, SQLITE_OPEN_READWRITE, NULL);
	if (rc != SQLITE_OK) {
		fprintf(stderr, "Couldn't open database: %s\n", sqlite3_errmsg(m_db));
		sqlite3_close(m_db);
		m_db = NULL;
		return;
	}

	m_queries.push_back(LOOKUP);
	m_queries.push_back(LOOKUPSTRICT);
	m_queries.push_back(AUTHORS);
	m_queries.push_back(TITLES);
	m_queries.push_back(INSERT);
	m_queries.push_back(UPDATE);
	m_queries.push_back(DELETE);

	/* Prepare statements */
	for (unsigned i = 0; i < STMTC; ++i) {
		rc = sqlite3_prepare_v2(m_db, m_queries[i].c_str(), -1,
								&m_stmts[i], NULL);
		if (rc) {
			fprintf(stderr, "Couldn't prepare statement: %s\n",
					sqlite3_errmsg(m_db));
			sqlite3_finalize(m_stmts[i]); /* FIXME Does that make sense? */
			sqlite3_close(m_db);
			m_stmts[i] = NULL;
			m_db = NULL;
			return;
		}
	}
}

bool Db::is_open(void)
{
	if (m_db) {
		return true;
	} else {
		return false;
	}
}

const char *Db::version(void)
{
	return sqlite3_libversion();
}

int Db::lookup(const std::string& _author,
			   const std::string& _title, bool _isStrict)
{
	int rc;
	sqlite3_stmt *stmt;

	if (_isStrict) {
		stmt = m_stmts[LOOKUPSTRICTSTMT];
	} else {
		stmt = m_stmts[LOOKUPSTMT];
	}

	if (!m_db || !stmt) {
		return 1;
	}

	rc = sqlite3_reset(stmt);
	if (rc != SQLITE_OK) {
		fprintf(stderr, "Couldn't reset statement: %s\n", sqlite3_errmsg(m_db));
		return 1;
	}

	rc = sqlite3_bind_text(stmt, 1, _author.c_str(),
						   -1, SQLITE_TRANSIENT);
	if (rc) {
		fprintf(stderr, "Couldn't bind values: %s\n", sqlite3_errmsg(m_db));
		return 1;
	}

	rc = sqlite3_bind_text(stmt, 2, _title.c_str(),
						   -1, SQLITE_TRANSIENT);
	if (rc) {
		fprintf(stderr, "Couldn't bind values: %s\n", sqlite3_errmsg(m_db));
		return 1;
	}

	return 0;
}

int Db::lookupnext(std::string *_author, std::string *_title,
				   int *_rating, int *_copies, bool _isStrict)
{
	int rc;
	const char *author, *title;
	int rating, copies;
	sqlite3_stmt *stmt;

	if (_isStrict) {
		stmt = m_stmts[LOOKUPSTRICTSTMT];
	} else {
		stmt = m_stmts[LOOKUPSTMT];
	}

	if (!m_db || !stmt) {
		return 1;
	}

	rc = sqlite3_step(stmt);
	if (rc == SQLITE_DONE) {
		return 1;
	} else if (rc != SQLITE_ROW) {
		fprintf(stderr, "Couldn't read next result (%d): %s\n", rc,
				sqlite3_errmsg(m_db));
		return 1;
	}

	author = (const char *) sqlite3_column_text(stmt, 0);
	title = (const char *) sqlite3_column_text(stmt, 1);
	printf("%s %s\n", author, title);
	rating = sqlite3_column_int(stmt, 2);
	copies = sqlite3_column_int(stmt, 3);
	if (author && title) {
		if (_author) {
			*_author = author;
		}
		if (_title) {
			*_title = title;
		}
		if (_rating) {
			*_rating = rating;
		}
		if (_copies) {
			*_copies = copies;
		}
		return 0;
	}

	return 1;
}

int Db::lsnext(int _what, std::string *_val)
{
	int rc;
	sqlite3_stmt *stmt;
	const char *val;

	if (_what == Db::AUTHOR) {
		stmt = m_stmts[AUTHORSTMT];
	} else if (_what == Db::TITLE) {
		stmt = m_stmts[TITLESTMT];
	} else {
		return 1;
	}

	if (!m_db || !stmt) {
		return 1;
	}


	rc = sqlite3_step(stmt);
	if (rc == SQLITE_DONE) {
		rc = sqlite3_reset(stmt);
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

	val = (const char *) sqlite3_column_text(stmt, 0);
	if (val) {
		*_val = val;
		return 0;
	}

	return 1;
}

int Db::insertBook(const std::string& _author, const std::string& _title,
				   int _rating, int _copies)
{
	int rc;

	if (!m_db || !m_stmts[INSERTSTMT]) {
		return 1;
	}

	rc = sqlite3_bind_text(m_stmts[INSERTSTMT], 1, _author.c_str(),
						   -1, SQLITE_TRANSIENT);
	if (rc) {
		fprintf(stderr, "Couldn't bind values: %s\n", sqlite3_errmsg(m_db));
		return 1;
	}

	rc = sqlite3_bind_text(m_stmts[INSERTSTMT], 2, _title.c_str(),
						   -1, SQLITE_TRANSIENT);
	if (rc) {
		fprintf(stderr, "Couldn't bind values: %s\n", sqlite3_errmsg(m_db));
		return 1;
	}

	rc = sqlite3_bind_int(m_stmts[INSERTSTMT], 3, _rating);
	if (rc) {
		fprintf(stderr, "Couldn't bind values: %s\n", sqlite3_errmsg(m_db));
		return 1;
	}

	rc = sqlite3_bind_int(m_stmts[INSERTSTMT], 4, _copies);
	if (rc) {
		fprintf(stderr, "Couldn't bind values: %s\n", sqlite3_errmsg(m_db));
		return 1;
	}

	rc = sqlite3_step(m_stmts[INSERTSTMT]);
	if (rc != SQLITE_DONE) {
		fprintf(stderr, "Couldn't insert row: %s\n", sqlite3_errmsg(m_db));
	}

	rc = sqlite3_reset(m_stmts[INSERTSTMT]);
	if (rc != SQLITE_OK) {
		fprintf(stderr, "Couldn't reset statement: %s\n",
				sqlite3_errmsg(m_db));
		return 1;
	}

	return 0;
}

int Db::updateBook(const std::string& _oldAuthor, const std::string& _oldTitle,
				   const std::string& _author, const std::string& _title,
				   int _rating, int _copies)
{
	int rc;

	if (!m_db || !m_stmts[UPDATESTMT]) {
		return 1;
	}

	rc = sqlite3_bind_text(m_stmts[UPDATESTMT], 1, _author.c_str(),
						   -1, SQLITE_TRANSIENT);
	if (rc) {
		fprintf(stderr, "Couldn't bind values: %s\n", sqlite3_errmsg(m_db));
		return 1;
	}

	rc = sqlite3_bind_text(m_stmts[UPDATESTMT], 2, _title.c_str(),
						   -1, SQLITE_TRANSIENT);
	if (rc) {
		fprintf(stderr, "Couldn't bind values: %s\n", sqlite3_errmsg(m_db));
		return 1;
	}

	rc = sqlite3_bind_int(m_stmts[UPDATESTMT], 3, _rating);
	if (rc) {
		fprintf(stderr, "Couldn't bind values: %s\n", sqlite3_errmsg(m_db));
		return 1;
	}

	rc = sqlite3_bind_int(m_stmts[UPDATESTMT], 4, _copies);
	if (rc) {
		fprintf(stderr, "Couldn't bind values: %s\n", sqlite3_errmsg(m_db));
		return 1;
	}

	rc = sqlite3_bind_text(m_stmts[UPDATESTMT], 5, _oldAuthor.c_str(),
						   -1, SQLITE_TRANSIENT);
	if (rc) {
		fprintf(stderr, "Couldn't bind values: %s\n", sqlite3_errmsg(m_db));
		return 1;
	}

	rc = sqlite3_bind_text(m_stmts[UPDATESTMT], 6, _oldTitle.c_str(),
						   -1, SQLITE_TRANSIENT);
	if (rc) {
		fprintf(stderr, "Couldn't bind values: %s\n", sqlite3_errmsg(m_db));
		return 1;
	}

	rc = sqlite3_step(m_stmts[UPDATESTMT]);
	if (rc != SQLITE_DONE) {
		fprintf(stderr, "Couldn't update row: %s\n", sqlite3_errmsg(m_db));
	}

	rc = sqlite3_reset(m_stmts[UPDATESTMT]);
	if (rc != SQLITE_OK) {
		fprintf(stderr, "Couldn't reset statement: %s\n",
				sqlite3_errmsg(m_db));
		return 1;
	}

	return 0;
}

int Db::deleteBook(const std::string& _author, const std::string& _title)
{
	int rc;

	if (!m_db || !m_stmts[DELETESTMT]) {
		return 1;
	}

	rc = sqlite3_bind_text(m_stmts[DELETESTMT], 1, _author.c_str(),
						   -1, SQLITE_TRANSIENT);
	if (rc) {
		fprintf(stderr, "Couldn't bind values: %s\n", sqlite3_errmsg(m_db));
		return 1;
	}

	rc = sqlite3_bind_text(m_stmts[DELETESTMT], 2, _title.c_str(),
						   -1, SQLITE_TRANSIENT);
	if (rc) {
		fprintf(stderr, "Couldn't bind values: %s\n", sqlite3_errmsg(m_db));
		return 1;
	}

	rc = sqlite3_step(m_stmts[DELETESTMT]);
	if (rc != SQLITE_DONE) {
		fprintf(stderr, "Couldn't update row: %s\n", sqlite3_errmsg(m_db));
	}

	rc = sqlite3_reset(m_stmts[DELETESTMT]);
	if (rc != SQLITE_OK) {
		fprintf(stderr, "Couldn't reset statement: %s\n",
				sqlite3_errmsg(m_db));
		return 1;
	}

	return 0;
}

Db::~Db(void)
{
	int rc;

	for (unsigned i = 0; i < m_stmts.size(); ++i) {
		rc = sqlite3_finalize(m_stmts[i]);
		if (rc != SQLITE_OK) {
			fprintf(stderr, "Couldn't finalize statement: %s\n",
					sqlite3_errmsg(m_db));
		}
	}

	if (m_db) {
		rc = sqlite3_close(m_db);
		/* XXX Whatever close returns is undocumented */
		if (rc != SQLITE_OK) {
			fprintf(stderr, "Couldn't close DB: %s\n",
					sqlite3_errmsg(m_db));
		}
	}
}
