#ifndef EDITDIALOG_H
#define EDITDIALOG_H

#include <QtGui>
#include "rating.h"

class EditDialog : public QDialog {
	Q_OBJECT

public:
	EditDialog(void);

public slots:
	void setDialogTitle(void);
	void setCopies(int);
	void setRatingEnabled(int);

private:
	QLineEdit *m_authorlineedit;
	QLineEdit *m_titlelineedit;
	QLabel *m_copieslabel;
	RatingEditor *m_rating;
};

#endif
