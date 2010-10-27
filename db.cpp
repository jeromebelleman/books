#include <stdio.h>
#include <string>
#include "db.h"

#define FILTER "SELECT author, title, rating, count FROM books \
WHERE author like ? OR title like ?"
#define AUTHORS "SELECT DISTINCT author FROM books"
#define TITLES "SELECT DISTINCT title FROM books"
#define INSERT "INSERT INTO books VALUES (?, ?, ?, ?)"

Db::Db(const std::string& _path)
{
	int rc;

	/* Open DB */
	rc = sqlite3_open_v2(_path.c_str(), &m_db, SQLITE_OPEN_READWRITE, NULL);
	if (rc) {
		fprintf(stderr, "Couldn't open database: %s\n", sqlite3_errmsg(m_db));
		sqlite3_close(m_db);
		m_db = NULL;
		m_filterstmt = NULL;
		return;
	}

	/* Prepare statements */
	rc = sqlite3_prepare_v2(m_db, FILTER, -1, &m_filterstmt, NULL);
	if (rc) {
		fprintf(stderr, "Couldn't prepare statement: %s\n",
				sqlite3_errmsg(m_db));
		sqlite3_finalize(m_filterstmt); /* FIXME Does that make sense? */
		sqlite3_close(m_db);
		m_filterstmt = NULL;
		m_db = NULL;
	}

	rc = sqlite3_prepare_v2(m_db, AUTHORS, -1, &m_authorstmt, NULL);
	if (rc) {
		fprintf(stderr, "Couldn't prepare statement: %s\n",
				sqlite3_errmsg(m_db));
		sqlite3_finalize(m_authorstmt); /* FIXME Does that make sense? */
		sqlite3_close(m_db);
		m_authorstmt = NULL;
		m_db = NULL;
	}

	rc = sqlite3_prepare_v2(m_db, TITLES, -1, &m_titlestmt, NULL);
	if (rc) {
		fprintf(stderr, "Couldn't prepare statement: %s\n",
				sqlite3_errmsg(m_db));
		sqlite3_finalize(m_titlestmt); /* FIXME Does that make sense? */
		sqlite3_close(m_db);
		m_titlestmt = NULL;
		m_db = NULL;
	}

	rc = sqlite3_prepare_v2(m_db, INSERT, -1, &m_insertstmt, NULL);
	if (rc) {
		fprintf(stderr, "Couldn't prepare statement: %s\n",
				sqlite3_errmsg(m_db));
		sqlite3_finalize(m_insertstmt); /* FIXME Does that make sense? */
		sqlite3_close(m_db);
		m_insertstmt = NULL;
		m_db = NULL;
	}
}

const char *Db::version(void)
{
	return sqlite3_libversion();
}

int Db::filter(const std::string& _author, const std::string& _title)
{
	int rc;
	std::string author, title;

	if (!m_db || !m_filterstmt) {
		return 1;
	}

	author = "%" + _author + "%";
	rc = sqlite3_bind_text(m_filterstmt, 1, author.c_str(),
						   -1, SQLITE_TRANSIENT);
	if (rc) {
		fprintf(stderr, "Couldn't bind values: %s\n", sqlite3_errmsg(m_db));
		return 1;
	}

	title = "%" + _title + "%";
	rc = sqlite3_bind_text(m_filterstmt, 2, title.c_str(),
						   -1, SQLITE_TRANSIENT);
	if (rc) {
		fprintf(stderr, "Couldn't bind values: %s\n", sqlite3_errmsg(m_db));
		return 1;
	}

	return 0;
}

int Db::filternext(std::string *_author, std::string *_title,
				   int *_rating, int *_copies)
{
	int rc;
	const char *author, *title;
	int rating, copies;

	if (!m_db || !m_filterstmt) {
		return 1;
	}

	rc = sqlite3_step(m_filterstmt);
	if (rc == SQLITE_DONE) {
		rc = sqlite3_reset(m_filterstmt);
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

	author = (const char *) sqlite3_column_text(m_filterstmt, 0);
	title = (const char *) sqlite3_column_text(m_filterstmt, 1);
	rating = sqlite3_column_int(m_filterstmt, 2);
	copies = sqlite3_column_int(m_filterstmt, 3);
	if (author && title) {
		*_author = author;
		*_title = title;
		*_rating = rating;
		*_copies = copies;
		return 0;
	}

	return 1;
}

int Db::lsnext(int _what, std::string *_val)
{
	int rc;
	sqlite3_stmt *stmt;
	const char *val;

	if (!m_db || !m_authorstmt || !m_titlestmt) {
		return 1;
	}

	if (_what == Db::AUTHOR) {
		stmt = m_authorstmt;
	} else if (_what == Db::TITLE) {
		stmt = m_titlestmt;
	} else {
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

int Db::insert(const std::string& _author, const std::string& _title,
			   int _rating, int _copies)
{
	int rc;

	if (!m_db || !m_filterstmt) {
		return 1;
	}

	rc = sqlite3_bind_text(m_insertstmt, 1, _author.c_str(),
						   -1, SQLITE_TRANSIENT);
	if (rc) {
		fprintf(stderr, "Couldn't bind values: %s\n", sqlite3_errmsg(m_db));
		return 1;
	}

	rc = sqlite3_bind_text(m_insertstmt, 2, _title.c_str(),
						   -1, SQLITE_TRANSIENT);
	if (rc) {
		fprintf(stderr, "Couldn't bind values: %s\n", sqlite3_errmsg(m_db));
		return 1;
	}

	rc = sqlite3_bind_int(m_insertstmt, 4, _rating);
	if (rc) {
		fprintf(stderr, "Couldn't bind values: %s\n", sqlite3_errmsg(m_db));
		return 1;
	}

	rc = sqlite3_bind_int(m_insertstmt, 3, _copies);
	if (rc) {
		fprintf(stderr, "Couldn't bind values: %s\n", sqlite3_errmsg(m_db));
		return 1;
	}

	rc = sqlite3_step(m_insertstmt);
	if (rc == SQLITE_CONSTRAINT) {
		return CONSTRAINT;
	} else if (rc != SQLITE_DONE) {
		fprintf(stderr, "Couldn't insert row: %s\n", sqlite3_errmsg(m_db));
		return 1;
	}

	return 0;
}

Db::~Db(void)
{
	int rc;

	if (m_filterstmt) {
		rc = sqlite3_finalize(m_filterstmt);
		if (rc != SQLITE_OK) {
			fprintf(stderr, "Couldn't delete statement: %s\n",
					sqlite3_errmsg(m_db));
		}
	}

	if (m_authorstmt) {
		rc = sqlite3_finalize(m_authorstmt);
		if (rc != SQLITE_OK) {
			fprintf(stderr, "Couldn't delete statement: %s\n",
					sqlite3_errmsg(m_db));
		}
	}

	if (m_titlestmt) {
		rc = sqlite3_finalize(m_titlestmt);
		if (rc != SQLITE_OK) {
			fprintf(stderr, "Couldn't delete statement: %s\n",
					sqlite3_errmsg(m_db));
		}
	}

	if (m_insertstmt) {
		rc = sqlite3_finalize(m_insertstmt);
		if (rc != SQLITE_OK) {
			fprintf(stderr, "Couldn't delete statement: %s\n",
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
