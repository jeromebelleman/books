#ifndef DB
#define DB

#include <sqlite3.h>

#define BUFSIZE 256

/* typedef struct {
	sqlite3 *db;
	sqlite3_stmt *stmt;
} db_t;

db_t *db_create(void);
int db_query(db_t *_db, const char *_str);
int db_next(db_t *_db, char *_author, char *_title);
int db_destroy(db_t *_db); */

class Db
{
private:
	sqlite3 *m_db;
	sqlite3_stmt *m_stmt;

public:
	Db(void);
	static const char *version(void);
	int query(std::string _str);
	int next(std::string *_author, std::string *_title);
	~Db(void);
};

#endif
