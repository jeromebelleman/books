#ifndef EDITDIALOG_H
#define EDITDIALOG_H

#include <QtGui>
#include "rating.h"
#include "db.h"

class EditDialog : public QMainWindow {
	Q_OBJECT

public:
	EditDialog(QWidget *_parent, const QModelIndex& _index, Db *_db, 
			   QAbstractItemModel *_authormodel,
			   QAbstractItemModel *_titlemodel,
			   const QString& _author = "", const QString& _title = "",
			   int _rating = -1, int _copies = 0);

public slots:
	void setDialogTitle(void);
	void setCopies(int);
	void setRatingEnabled(int);
	void save(bool);
	void finish(int);

signals:
	void closed(QModelIndex);

private:
	QModelIndex m_index;
	Db *m_db;
	QCompleter m_authorcompleter, m_titlecompleter;
	QLineEdit *m_authorlineedit;
	QLineEdit *m_titlelineedit;
	QLabel *m_copieslabel;
	RatingEditor *m_rating;
	QSpinBox *m_spinbox;
	int m_prvrating;

	void closeEvent(QCloseEvent *);
};

class ConstraintDialog : public QMessageBox {
public:
	ConstraintDialog(QWidget *_widget, const QString& _author,
					 const QString& _title,
					 int _copies);
};

#endif
