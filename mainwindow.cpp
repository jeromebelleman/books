#include <iostream>
#include <QtGui>
#include "mainwindow.h"
#include "editdialog.h"
#include "db.h"

MainWindow::MainWindow(void)
	: m_db(NULL)
{
	int rc;
	std::string path;

	connect(this, SIGNAL(closed(void)), this, SLOT(bye(void)));

	filebrowser();
	
	resize(640, 480);
	setWindowTitle(tr("Books"));
	menus();
	toolbar();
	#ifdef Q_WS_MAC
	setUnifiedTitleAndToolBarOnMac(true);
	#endif

	m_headers << "Author" << "Title" << "Rating" << "Copies";

	m_tree = new QTreeView();
	m_model.setHorizontalHeaderLabels(m_headers);
	m_tree->setModel(&m_model);
	m_tree->setRootIsDecorated(false);
	m_tree->setAlternatingRowColors(true);
	m_tree->setSelectionMode(QAbstractItemView::ExtendedSelection);
	m_delegate = new RatingDelegate();
	m_tree->setItemDelegate(m_delegate);
	connect(m_tree, SIGNAL(doubleClicked(const QModelIndex)),
			this, SLOT(editBook(const QModelIndex)));
	setCentralWidget(m_tree);

	m_cnf.read();
	rc = m_cnf.get("dbpath", &path);
	if (!rc) {
		m_db = new Db(path);
		rc = filter(); /* FIXME Shadowed RC here */
		rc = ls();
		m_tree->setColumnWidth(0, 200);
		m_tree->setColumnWidth(1, 200);
		m_tree->setColumnWidth(2, 100);
		if (rc) {
			statusBar()->showMessage(tr("Invalid DB file"));
		}
	}
}

int MainWindow::menus(void)
{
	QMenu *file, *book, *deletemenu;
	QAction *open, *quit, *deletebook;

	file = menuBar()->addMenu(tr("&File"));

	open = new QAction(tr("&Open"), this);
	open->setShortcut(tr("Ctrl+O"));
	connect(open, SIGNAL(triggered(void)), this, SLOT(open(void)));
	file->addAction(open);

	quit = new QAction(tr("&Quit"), this);
	quit->setShortcut(tr("Ctrl+Q"));
	connect(quit, SIGNAL(triggered(void)), this, SLOT(bye(void)));
	file->addAction(quit);

	book = menuBar()->addMenu(tr("&Book"));

	m_newbook = new QAction(QIcon(":/book.png"), tr("&New Book"), this);
	m_newbook->setShortcut(tr("Ctrl+N"));
	connect(m_newbook, SIGNAL(triggered(bool)), this, SLOT(newBook(bool)));
	book->addAction(m_newbook);

	deletemenu = book->addMenu(tr("Delete Book?"));

	deletebook = new QAction(tr("Yes, &Delete Book"), this);
	connect(deletebook, SIGNAL(triggered(bool)), this, SLOT(deleteBook(bool)));
	deletemenu->addAction(deletebook);

	return 0;
}

int MainWindow::toolbar(void)
{
	QToolBar *toolbar;
	QLabel *lbl;

	m_lineedit = new QLineEdit();
	connect(m_lineedit, SIGNAL(textEdited(const QString)),
			this, SLOT(filter(void)));

	lbl = new QLabel("Look Up:");

	toolbar = addToolBar("Toolbar");
	toolbar->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
	toolbar->addAction(m_newbook);
	toolbar->addWidget(lbl);
	toolbar->addWidget(m_lineedit);


	return 0;
}

int MainWindow::filebrowser(void)
{
	char *dir;

	dir = getcwd(NULL, 0);
	if (!dir) {
		dir = getenv("HOME");
	}

	if (dir) {
		m_filedialog = new QFileDialog(0, "Open Book Database", dir);
	} else {
		m_filedialog = new QFileDialog(0, "Open Book Database");
	}
	free(dir);

	return 0;
}

int MainWindow::filter(void)
{
	int rc;
	QString str;
	std::string author;
	std::string title;
	int rating, copies;
/* 	QStringList list; */
/* 	QTreeWidgetItem *item; */
	QList<QStandardItem *> items;
	QStandardItem *authoritm, *titleitm, *ratingitm, *copiesitm;

	if (!m_tree) {
		return 1;
	}

	if (!m_db) {
		return 1;
	}

	str = m_lineedit->text();
	rc = m_db->filter(str.toStdString(), str.toStdString());
	if (rc) {
		m_model.clear();
		return 1;
	}

	while (m_model.removeRow(0));
	while (m_db->filternext(&author, &title, &rating, &copies) == 0) {
		items.clear();
		authoritm = new QStandardItem(QString::fromUtf8(author.c_str()));
		authoritm->setEditable(false);
		titleitm = new QStandardItem(QString::fromUtf8(title.c_str()));
		titleitm->setEditable(false);
		ratingitm = new QStandardItem();
		ratingitm->setData(qVariantFromValue(Rating(rating)), 0);
		ratingitm->setEditable(false);
		copiesitm = new QStandardItem(QString::number(copies));
		copiesitm->setEditable(false);
		items << authoritm << titleitm << ratingitm << copiesitm;

/* 		items << new QStandardItem(QString::fromUtf8(author.c_str())) <<
		new QStandardItem(QString::fromUtf8(title.c_str())) <<
		new QStandardItem(QString::number(rating)) <<
		new QStandardItem(QString::number(copies)); */

		m_model.appendRow(items);
/* 		list = QStringList();
		list << QString::fromUtf8(author.c_str()) << 
			QString::fromUtf8(title.c_str()) << QString::number(rating) <<
			QString::number(copies);
		item = new QTreeWidgetItem((QTreeWidget *) 0, list);
		m_tree->addTopLevelItem(item); */
/* 		printf("pop\n"); */
/* 		m_tree->setItemWidget(item, 2, new QPushButton("voila")); */
	}

	return 0;
}

void MainWindow::open(void)
{
	int rc;
	QStringList files;

	if (m_filedialog->exec()) {
		/* Set default (last) database file */
		files = m_filedialog->selectedFiles();
		m_cnf.addline(); /* Caution, order does matter here! */
		m_cnf.set("dbpath", files[0].toStdString());
		m_cnf.write();

		/* Close currently opened DB */
		delete m_db;

		/* Open and read DB */
		m_db = new Db(files[0].toStdString());
		rc = filter();
		if (rc) {
			statusBar()->showMessage(tr("Invalid DB file"));
		}
	}
}

void MainWindow::bye(void)
{
	delete m_db;
	QCoreApplication::quit();
}

void MainWindow::closeEvent(QCloseEvent *)
{
	emit closed();
}

void MainWindow::editBook(const QModelIndex& _index)
{
	QStandardItem *item;
	QModelIndex mainindex, index;
	QString author, title;
	Rating rating;
	int copies;

	mainindex = _index.sibling(_index.row(), 0);
	if (m_opened.contains(mainindex)) {
		return;
	}
	m_opened << mainindex;
	item = m_model.itemFromIndex(mainindex);
	author = item->text();

	index = _index.sibling(_index.row(), 1);
	item = m_model.itemFromIndex(index);
	title = item->text();

	index = _index.sibling(_index.row(), 2);
	item = m_model.itemFromIndex(index);
	rating = qVariantValue<Rating>(index.data());

	index = _index.sibling(_index.row(), 3);
	item = m_model.itemFromIndex(index);
	copies = item->text().toInt();

	EditDialog *edit = new EditDialog(this, mainindex, m_db, &m_authormodel,
									  &m_titlemodel, author, title,
									  rating.get(), copies);
	edit->show();
}

void MainWindow::newBook(bool)
{
	EditDialog *edit;

	edit = new EditDialog(this, QModelIndex(), m_db,
						  &m_authormodel, &m_titlemodel);
	edit->show();
}

int MainWindow::ls(void)
{
	std::string val;
	QStandardItem *item;


	m_authormodel.clear();
	while (m_db->lsnext(Db::AUTHOR, &val) == 0) {
		item = new QStandardItem(QString::fromUtf8(val.c_str()));
		m_authormodel.appendRow(item);
	}

	m_titlemodel.clear();
	while (m_db->lsnext(Db::TITLE, &val) == 0) {
		item = new QStandardItem(QString::fromUtf8(val.c_str()));
		m_titlemodel.appendRow(item);
	}

	return 0;
}

void MainWindow::deleteBook(bool)
{
	std::cout << "Book to be deleted here\n";
}

void MainWindow::finish(const QModelIndex& _index)
{
	if (_index.isValid()) {
		m_opened.removeAll(_index);
	}
}

void RatingDelegate::paint(QPainter *_painter,
						   const QStyleOptionViewItem& _option,
						   const QModelIndex& _index) const
{
	if (qVariantCanConvert<Rating>(_index.data())) {
		Rating rating = qVariantValue<Rating>(_index.data());

		if (_option.state & QStyle::State_Selected) {
			_painter->fillRect(_option.rect, _option.palette.highlight());
			rating.paint(_painter, _option.rect, _option.palette, true);
		} else {
			rating.paint(_painter, _option.rect, _option.palette, false);
		}
	} else {
		QItemDelegate::paint(_painter, _option, _index);
	}
}
