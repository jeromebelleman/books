#ifndef DB_H
#define DB_H

#include <sqlite3.h>

class Db {
public:
	enum { AUTHOR, TITLE };
	enum { ERROR, CONSTRAINT };

	Db(const std::string& _path);
	static const char *version(void);
	int filter(const std::string& _author = "", const std::string& _title = "");
	int filternext(std::string *_author, std::string *_title,
				   int *_rating, int *_copies);
	int lsnext(int _what, std::string *_val);
	int insert(const std::string& _author, const std::string& _title,
			   int _rating, int _copies);
	~Db(void);

private:
	sqlite3 *m_db;
	sqlite3_stmt *m_filterstmt, *m_authorstmt, *m_titlestmt, *m_insertstmt;
};

#endif
