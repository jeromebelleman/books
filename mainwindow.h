#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui>

class QMenu;
class Db;

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	MainWindow(void);

public slots:
	void filter(void);

private:
	Db *m_db;
	QTreeWidget *m_tree;
	QLineEdit *m_lineedit;
	int menus(void);
	int toolbar(void);
};



#endif
