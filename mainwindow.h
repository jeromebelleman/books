#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui>
#include "cnf.h"
#include "rating.h"

class Db;

class RatingDelegate : public QItemDelegate
{
public:
	void paint(QPainter *_painter,
			   const QStyleOptionViewItem& _option,
			   const QModelIndex&) const;
};

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	MainWindow(void);

public slots:
	int filter(void);
	void open(void);
	void bye(void);
	void editBook(const QModelIndex&);
	void newBook(bool _checked);
	void deleteBook(bool _checked);
	int ls(void);
	void finish(const QModelIndex&);

signals:
	void closed(void);

private:
	QAction *m_newbook;
	QStandardItemModel m_authormodel, m_titlemodel;
	QStandardItemModel m_model;
	Cnf m_cnf;
	Db *m_db;
	QFileDialog *m_filedialog;
	QTreeView *m_tree;
	QLineEdit *m_lineedit;
	QStringList m_headers;
	RatingDelegate *m_delegate;
	QList<QModelIndex> m_opened;

	int menus(void);
	int toolbar(void);
	int filebrowser(void);

	void closeEvent(QCloseEvent *);
};

#endif
