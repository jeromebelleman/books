#ifndef EDITDIALOG_H
#define EDITDIALOG_H

#include <QtGui>

class EditDialog : public QDialog
{
	Q_OBJECT

public:
	EditDialog(void);

public slots:
	void setDialogTitle(void);
	void setCopies(int);

private:
	QLineEdit *m_authorlineedit;
	QLineEdit *m_titlelineedit;
	QLabel *m_copieslabel;
};

#endif
