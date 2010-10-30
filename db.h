#ifndef DB_H
#define DB_H

#include <sqlite3.h>

class Db {
public:
	enum { AUTHOR, TITLE };
	enum { ERROR, CONSTRAINT };
	enum { LOOKUPSTMT, LOOKUPSTRICTSTMT, AUTHORSTMT, TITLESTMT,
		   INSERTSTMT, UPDATESTMT, DELETESTMT, STMTC };

	Db(const std::string& _path);
	static const char *version(void);
	int lookup(const std::string& _author = "%%",
			   const std::string& _title = "%%", bool _isStrict = false);
	int lookupnext(std::string *_author, std::string *_title,
				   int *_rating, int *_copies);
	int lsnext(int _what, std::string *_val);
	int insertBook(const std::string& _author, const std::string& _title,
				   int _rating, int _copies);
	int updateBook(const std::string& _oldAuthor, const std::string& _oldTitle,
				   const std::string& _author, const std::string& _title,
				   int _rating, int _copies);
	int deleteBook(const std::string& _author, const std::string& _title);
	~Db(void);

private:
	sqlite3 *m_db;
	std::vector<sqlite3_stmt *> m_stmts;
	std::vector<std::string> m_queries;
};

#endif
