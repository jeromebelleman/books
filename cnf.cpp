#include "cnf.h"
#include <iostream>
#include <fstream>
#include <QtCore>

Cnf::Cnf(void)
	: m_linec(0)
{
}

int Cnf::get(const std::string& _key, std::string *_val)
{
	for (unsigned i = 0; i < m_prfs.size(); ++i) {
		if (m_prfs[i].key == _key) {
			*_val = m_prfs[i].val;
			return 0;
		}
	}

	return 1;
}

int Cnf::set(const std::string& _key, const std::string& _val)
{
	prf_t prf;

	for (unsigned i = 0; i < m_prfs.size(); ++i) {
		if (m_prfs[i].key == _key) {
			m_prfs[i].val = _val;
			return 0;
		}
	}

	prf.lineno = m_linec - 1;
	prf.key = _key;
	prf.val = _val;

	m_prfs.push_back(prf);

	return 1;
}

int Cnf::addcmt(const std::string& _cmt)
{
	cmt_t cmt;

	cmt.lineno = m_linec - 1;
	cmt.data = _cmt;

	m_cmts.push_back(cmt);

	return 0;
}

int Cnf::read(void)
{
	int rc;

	/* File Variables */
	std::string path;
	std::ifstream file;

	/* State Machine Variables */
	int c;
	enum { NEWLINE, INDENT, COMMENT, KEY, VAL };
	int state = NEWLINE;
	std::string key, val, cmt; /* Working variables */

	getpath(&path);
	file.open(path.c_str());
	if (!file.is_open()) {
		return 1;
	}

	while ((c = file.get()) != -1) {
		switch (state) {
			case NEWLINE:
				addline();
				if (c == '#') {
					cmt.clear();
					state = COMMENT;
				} else if (c == '\n') {
					break;
				} else if (isspace(c)) {
					state = INDENT;
				} else {
					key = c;
					state = KEY;
				}
				break;
			case COMMENT:
				if (c == '\n') {
					rc = addcmt(cmt);
					if (rc) {
						return 1;
					}
					state = NEWLINE;
				} else {
					cmt += c;
				}
				break;
			case KEY:
				if (c == '\n') {
					/* This would be a syntax error */
					state = NEWLINE;
				} else if (c == '#') {
					/* This would be a syntax error */
					cmt.clear();
					state = COMMENT;
				} else if (isspace(c)) {
					val.clear();
					state = VAL;
				} else {
					key += c;
				}
				break;
			case VAL:
				if (c == '\n') {
					if (!val.empty()) {
						set(key, val);
					}
					state = NEWLINE;
				} else if (c == '#') {
					if (!val.empty()) {
						set(key, val);
					}
					cmt.clear();
					state = COMMENT;
				} else if (isspace(c)) {
					break;
				} else {
					val += c;
				}
				break;
			case INDENT:
				break;
		}
	}

	file.close();

	return 0;
}

int Cnf::getpath(std::string *_path)
{
	char *home;

	home = getenv("HOME");
	if (!home) {
		return 1;
	}

	#if defined(Q_WS_MAC)
	*_path = std::string(home) + "/Library/Books/" + ".booksrc";
	#elif defined(Q_WS_X11)
	*_path = std::string(home) + "/" + ".booksrc";
	#else
	#error "Module cnf not implemented for this platform"
	#endif

	return 0;
}

int Cnf::write(void)
{
	std::string path;
	std::ofstream file;
	unsigned lineno = 0, prf_i = 0, cmt_i = 0;

	getpath(&path);
	file.open(path.c_str());
	if (!file.is_open()) {
		return 1;
	}

	while (prf_i < m_prfs.size() || cmt_i < m_cmts.size()) {
		if (prf_i < m_prfs.size() && m_prfs[prf_i].lineno == lineno) {
			file << m_prfs[prf_i].key << " " << m_prfs[prf_i].val << "\n";
			prf_i++;
		}
		if (cmt_i < m_cmts.size() && m_cmts[cmt_i].lineno == lineno) {
			file << "#" << m_cmts[cmt_i].data << "\n";
			cmt_i++;
		}
		lineno++;
	}
	file.close();

	return 0;
}

void Cnf::addline(void)
{
	m_linec++;
}

int Cnf::test(void)
{
	for (unsigned i = 0; i < m_cmts.size(); ++i) {
		std::cout << "comment at " << m_cmts[i].lineno << ": " << 
		m_cmts[i].data << "\n";
	}
	for (unsigned i = 0; i < m_prfs.size(); ++i) {
		std::cout << "pref at " << m_prfs[i].lineno << ": " << m_prfs[i].key <<
		' ' << m_prfs[i].val << "\n";
	}
	std::cout << "Found " << m_linec << " lines\n";
	return 0;
}
