#ifndef CNF_H
#define CNF_H

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <vector>
#include <map>

typedef struct {
	unsigned lineno;
	std::string key;
	std::string val;
} prf_t;

typedef struct {
	unsigned lineno;
	std::string data;
} cmt_t;

class Cnf {
public:
	Cnf(void);
	int get(const std::string& _key, std::string *_val);
	int set(const std::string& _key, const std::string& _val);
	int read(void);
	int write(void);
	void addline(void);

private:
	unsigned m_linec;
	std::vector<prf_t> m_prfs;
	std::vector<cmt_t> m_cmts;
	int addcmt(const std::string& _cmt);
	int test(void);
};

#endif 
