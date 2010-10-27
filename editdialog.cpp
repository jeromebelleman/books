#include <iostream>
#include "editdialog.h"

EditDialog::EditDialog(QWidget *_parent, const QModelIndex& _index, Db *_db,
					   QAbstractItemModel *_authormodel,
					   QAbstractItemModel *_titlemodel,
					   const QString& _author, const QString& _title,
					   int _rating, int _copies)
	: QMainWindow(_parent), m_index(_index), m_db(_db), 
	m_authorcompleter(_authormodel), m_titlecompleter(_titlemodel), 
	m_prvrating(_rating)
{
	QGridLayout *grid;
	QLabel *label;
	QPushButton *pushbutton;
	QCheckBox *checkbox;
	QWidget *central;

	connect(this, SIGNAL(closed(QModelIndex)),
			parentWidget(), SLOT(finish(QModelIndex)));

	setAttribute(Qt::WA_DeleteOnClose);

	grid = new QGridLayout;

	central = new QWidget;
	central->setLayout(grid);
	setCentralWidget(central);

	/* Author */
	label = new QLabel(tr("Author"));
	grid->addWidget(label, 0, 0);
	m_authorcompleter.setCaseSensitivity(Qt::CaseInsensitive);
	m_authorlineedit = new QLineEdit(_author);
	m_authorlineedit->setCompleter(&m_authorcompleter);
	connect(m_authorlineedit, SIGNAL(textChanged(QString)),
			this, SLOT(setDialogTitle(void)));
	grid->addWidget(m_authorlineedit, 0, 1, 1, 4);

	/* Title */
	label = new QLabel(tr("Title"));
	grid->addWidget(label, 1, 0);
	m_titlecompleter.setCaseSensitivity(Qt::CaseInsensitive);
	m_titlelineedit = new QLineEdit(_title);
	m_titlelineedit->setCompleter(&m_titlecompleter);
	connect(m_titlelineedit, SIGNAL(textChanged(QString)),
			this, SLOT(setDialogTitle(void)));
	grid->addWidget(m_titlelineedit, 1, 1, 1, 4);

	/* Rating */
	label = new QLabel(tr("Rating"));
	grid->addWidget(label, 2, 0);
	checkbox = new QCheckBox;
	connect(checkbox, SIGNAL(stateChanged(int)),
			this, SLOT(setRatingEnabled(int)));
	m_rating = new RatingEditor(_rating);
	if (_rating > -1) {
		checkbox->setCheckState(Qt::Checked);
		m_rating->setEnabled(true);
	} else {
		checkbox->setCheckState(Qt::Unchecked);
		m_rating->setEnabled(false);
	}
	grid->addWidget(checkbox, 2, 1);
	grid->addWidget(m_rating, 2, 2);

	/* Copies */
	m_spinbox = new QSpinBox();
	m_copieslabel = new QLabel(tr("Copy"));
	connect(m_spinbox, SIGNAL(valueChanged(int)), this, SLOT(setCopies(int)));
	m_spinbox->setValue(_copies);
	grid->addWidget(m_spinbox, 2, 3);
	grid->addWidget(m_copieslabel, 2, 4);

	/* Buttons */
	pushbutton = new QPushButton(tr("Save"));
	connect(pushbutton, SIGNAL(clicked(bool)), this, SLOT(save(bool)));
	pushbutton->setToolTip(tr("Save changes to database"));
	grid->addWidget(pushbutton, 4, 3);

	pushbutton = new QPushButton(tr("Cancel"));
	pushbutton->setToolTip(tr("Cancel changes to database"));
	grid->addWidget(pushbutton, 4, 4);

	setDialogTitle();
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
		if (m_prvrating < 0) {
			m_rating->set(0);
		} else {
			m_rating->set(m_prvrating);
		}
		m_rating->setEnabled(true);
	} else if (_state == Qt::Unchecked) {
		m_rating->setEnabled(false);
		m_prvrating = m_rating->get();
		m_rating->set(-1);
	}
}

void EditDialog::save(bool)
{
	int rc;
	QString author, title;
	int rating, copies;
	ConstraintDialog *dialog;


	author = m_authorlineedit->text();
	title = m_titlelineedit->text();
	rating = m_rating->get();
	copies = m_spinbox->value();

	dialog = new ConstraintDialog(this, author, title, copies);

	rc = m_db->insert(author.toStdString(), title.toStdString(),
					  rating, copies);
	if (rc == Db::CONSTRAINT) {
		dialog->show();
	}
}

void EditDialog::finish(int _result)
{
	if (_result == QDialog::Accepted) {
		std::cout << "accepted\n";
	}
}

void EditDialog::closeEvent(QCloseEvent *)
{
	emit closed(m_index);
}

ConstraintDialog::ConstraintDialog(QWidget *_parent, const QString& _author,
								   const QString& _title,
								   int _copies)
	: QMessageBox(_parent)
{
	QString msg;

	connect(this, SIGNAL(finished(int)), parentWidget(), SLOT(finish(int)));

	msg = "There's already a book called \"" + _title + "\" written by " +
	_author + "; shall I just merge the two copies and say you now have " +
	QString::number(_copies + 1) + " of these?";

	setWindowModality(Qt::WindowModal);
	setWindowFlags(Qt::Sheet);

	setWindowTitle("Duplicate Book");
	setText(msg);
	addButton("Cancel", QMessageBox::RejectRole);
	addButton("Merge", QMessageBox::AcceptRole);
}
