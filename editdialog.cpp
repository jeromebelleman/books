#include "editdialog.h"

EditDialog::EditDialog(void)
{
	QGridLayout *grid;
	QHBoxLayout *hbox;
	QLabel *label;
	QLineEdit *lineedit;

	grid = new QGridLayout;

	/* Author */
	label = new QLabel(tr("Author"));
	grid->addWidget(label, 0, 0);
	lineedit = new QLineEdit();
	grid->addWidget(lineedit, 0, 1);

	/* Title */
	label = new QLabel(tr("Title"));
	grid->addWidget(label, 1, 0);
	lineedit = new QLineEdit();
	grid->addWidget(lineedit, 1, 1);

	/* HBox */
	hbox = new QHBoxLayout;

	/* Rating */
/* 	rating = new Rating(); */


	setLayout(grid);
}
