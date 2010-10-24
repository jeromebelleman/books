#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui>
#include "cnf.h"

class QMenu;
class Db;

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	MainWindow(void);

public slots:
	int filter(void);
	void open(void);
	void bye(void);

signals:
	void closed(void);

private:
	Cnf m_cnf;
	Db *m_db;
	QFileDialog *m_filedialog;
	QTreeWidget *m_tree;
	QLineEdit *m_lineedit;
	int menus(void);
	int toolbar(void);
	int filebrowser(void);

	void closeEvent(QCloseEvent *);
};



#endif
