#include "editdialog.h"

EditDialog::EditDialog(void)

{
	QGridLayout *grid;
	QLabel *label;
	QSpinBox *spinbox;
	QPushButton *pushbutton;
	QCheckBox *checkbox;

	setWindowTitle("New Book");

	grid = new QGridLayout;

	/* Author */
	label = new QLabel(tr("Author"));
	grid->addWidget(label, 0, 0);
	m_authorlineedit = new QLineEdit();
	connect(m_authorlineedit, SIGNAL(textChanged(QString)),
			this, SLOT(setDialogTitle(void)));
	grid->addWidget(m_authorlineedit, 0, 1, 1, 4);

	/* Title */
	label = new QLabel(tr("Title"));
	grid->addWidget(label, 1, 0);
	m_titlelineedit = new QLineEdit();
	connect(m_titlelineedit, SIGNAL(textChanged(QString)),
			this, SLOT(setDialogTitle(void)));
	grid->addWidget(m_titlelineedit, 1, 1, 1, 4);

	/* Rating */
	label = new QLabel(tr("Rating"));
	grid->addWidget(label, 2, 0);
	checkbox = new QCheckBox;
	grid->addWidget(checkbox, 2, 1);
	m_rating = new RatingEditor;
	m_rating->setEnabled(false);
	grid->addWidget(m_rating, 2, 2);
	connect(checkbox, SIGNAL(stateChanged(int)),
			this, SLOT(setRatingEnabled(int)));

	/* Copies */
	spinbox = new QSpinBox();
	grid->addWidget(spinbox, 2, 3);
	connect(spinbox, SIGNAL(valueChanged(int)), this, SLOT(setCopies(int)));
	m_copieslabel = new QLabel(tr("Copy"));
	grid->addWidget(m_copieslabel, 2, 4);

	/* Buttons */
	pushbutton = new QPushButton(tr("Save"));
	pushbutton->setToolTip(tr("Save changes to database"));
	grid->addWidget(pushbutton, 4, 3);

	pushbutton = new QPushButton(tr("Cancel"));
	pushbutton->setToolTip(tr("Cancel changes to database"));
	grid->addWidget(pushbutton, 4, 4);

	setLayout(grid);
}

void EditDialog::setDialogTitle(void)
{
	QString author, title, dialogtitle;

	author = m_authorlineedit->text();
	title = m_titlelineedit->text();

	if (author.isEmpty() && !title.isEmpty()) {
		dialogtitle = title;
	} else if (!author.isEmpty() && title.isEmpty()) {
		dialogtitle = author;
	} else if (!author.isEmpty() && !title.isEmpty()) {
		dialogtitle = author + ": " + title;
	} else {
		dialogtitle = "New Book";
	}
	setWindowTitle(dialogtitle);
}

void EditDialog::setCopies(int _n)
{
	if (_n > 1) {
		m_copieslabel->setText(tr("Copies"));
	} else {
		m_copieslabel->setText(tr("Copy"));
	}
}

void EditDialog::setRatingEnabled(int _state)
{
	if (_state == Qt::Checked) {
		m_rating->setEnabled(true);
	} else if (_state == Qt::Unchecked) {
		m_rating->setEnabled(false);
	}

}
