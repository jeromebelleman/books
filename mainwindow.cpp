#include <iostream>
#include <QtGui>
#include <unistd.h>
#include "mainwindow.h"
#include "editdialog.h"
#include "db.h"

#define ROWC 50 /* Good on OS X, good on Linux */

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
	m_proxy.setSourceModel(&m_model);
	m_tree->setModel(&m_proxy);
	m_tree->setRootIsDecorated(false);
	m_tree->setSortingEnabled(true);
	m_tree->setAlternatingRowColors(true);
	m_tree->setSelectionMode(QAbstractItemView::ExtendedSelection);
	m_delegate = new RatingDelegate();
	m_tree->setItemDelegate(m_delegate);
	connect(m_tree, SIGNAL(doubleClicked(const QModelIndex)),
			this, SLOT(editBook(const QModelIndex)));
	setCentralWidget(m_tree);
	statusBar()->showMessage(tr("Books 1.0.2"));

	/* Read RC and open DB */
	m_cnf.read();
	rc = m_cnf.get("dbpath", &path);
	if (rc == 0) {
		m_db = new Db(path);
		if (m_db->is_open()) {
			m_newbook->setEnabled(true);
			m_deletemenu->setEnabled(true);
			filter();
			ls();
		} else {
			statusBar()->showMessage(tr("Invalid DB file"));
		}
		m_tree->setColumnWidth(0, 200);
		m_tree->setColumnWidth(1, 200);
		m_tree->setColumnWidth(2, 100);
	}
}

int MainWindow::menus(void)
{
	QMenu *file, *book;
	QAction *open, *quit, *deletebook;

	file = menuBar()->addMenu(tr("&File"));

	open = new QAction(QIcon(":stack.png"), tr("&Open Database..."), this);
	open->setShortcut(tr("Ctrl+O"));
	connect(open, SIGNAL(triggered(void)), this, SLOT(open(void)));
	file->addAction(open);

	m_print = new QAction(QIcon(":printer.png"), tr("&Print..."), this);
	m_print->setShortcut(tr("Ctrl+P"));
	connect(m_print, SIGNAL(triggered(void)), this, SLOT(printDialog(void)));
	file->addAction(m_print);

	quit = new QAction(QIcon(":/book.png"), tr("&Quit"), this);
	quit->setShortcut(tr("Ctrl+Q"));
	connect(quit, SIGNAL(triggered(void)), this, SLOT(bye(void)));
	file->addAction(quit);

	book = menuBar()->addMenu(tr("&Book"));

	m_newbook = new QAction(QIcon(":/book.png"), tr("&New Book"), this);
	m_newbook->setEnabled(false);
	m_newbook->setShortcut(tr("Ctrl+N"));
	connect(m_newbook, SIGNAL(triggered(bool)), this, SLOT(newBook(bool)));
	book->addAction(m_newbook);

	m_deletemenu = book->addMenu(tr("Delete Book?"));
	m_deletemenu->setEnabled(false);

	deletebook = new QAction(QIcon(":/delete.png"),
							 tr("Yes, &Delete Book"), this);
	connect(deletebook, SIGNAL(triggered(bool)), this, SLOT(deleteBook(bool)));
	m_deletemenu->addAction(deletebook);

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

	str = "%" + m_lineedit->text() + "%";
	rc = m_db->lookup(str.toStdString(), str.toStdString());
	if (rc) {
		while (m_model.removeRow(0));
		return 1;
	}

	while (m_model.removeRow(0));
	while (m_db->lookupnext(&author, &title, &rating, &copies) == 0) {
		items.clear();
		authoritm = new QStandardItem(author.c_str());
		authoritm->setEditable(false);
		titleitm = new QStandardItem(title.c_str());
		titleitm->setEditable(false);
		ratingitm = new QStandardItem();
		ratingitm->setData(qVariantFromValue(Rating(rating)), Qt::DisplayRole);
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
	QStringList files;

	if (m_filedialog->exec()) {
		/* Set default (last) database file */
		files = m_filedialog->selectedFiles();
		m_cnf.addline(); /* Caution, order does matter here! */
		m_cnf.set("dbpath", files[0].toStdString());
		m_cnf.write();

		/* Close currently opened DB */
		if (m_db) {
			delete m_db;
		}

		/* Open and read DB */
		m_db = new Db(files[0].toStdString());
		if (m_db->is_open()) {
			m_newbook->setEnabled(true);
			m_deletemenu->setEnabled(true);
			statusBar()->clearMessage();
			filter();
			ls();
		} else {
			m_newbook->setEnabled(false);
			m_deletemenu->setEnabled(false);
			statusBar()->showMessage(tr("Invalid DB file"));
			filter();
		}
	}
}

void MainWindow::bye(void)
{
	if (m_db) {
		delete m_db;
	}
	QCoreApplication::quit();
}

void MainWindow::closeEvent(QCloseEvent *)
{
	emit closed();
}

void MainWindow::editBook(const QModelIndex& _index)
{
	QModelIndex mainindex, index;
	QString author, title;
	Rating rating;
	int copies;

	if (!m_db) {
		return;
	}

	mainindex = _index.sibling(_index.row(), 0);
	if (m_opened.contains(mainindex)) {
		return;
	}
	m_opened << mainindex;
	author = qVariantValue<QString>(mainindex.data());

	index = _index.sibling(_index.row(), 1);
	title = qVariantValue<QString>(index.data());

	index = _index.sibling(_index.row(), 2);
	rating = qVariantValue<Rating>(index.data());

	index = _index.sibling(_index.row(), 3);
	copies = qVariantValue<int>(index.data());

	EditDialog *edit = new EditDialog(this, mainindex, m_db, &m_authormodel,
									  &m_titlemodel, false, author, title,
									  rating.get(), copies);
	edit->show();
}

void MainWindow::newBook(bool)
{
	EditDialog *edit;

	if (!m_db) {
		return;
	}

	edit = new EditDialog(this, QModelIndex(), m_db,
						  &m_authormodel, &m_titlemodel, true);
	edit->show();
}

int MainWindow::ls(void)
{
	std::string val;
	QStandardItem *item;

	if (!m_db) {
		return 1;
	}

	m_authormodel.clear();
	while (m_db->lsnext(Db::AUTHOR, &val) == 0) {
		item = new QStandardItem(val.c_str());
		m_authormodel.appendRow(item);
	}

	m_titlemodel.clear();
	while (m_db->lsnext(Db::TITLE, &val) == 0) {
		item = new QStandardItem(val.c_str());
		m_titlemodel.appendRow(item);
	}

	return 0;
}

void MainWindow::deleteBook(bool)
{
	QItemSelectionModel *model;
	QModelIndexList selected;
	QModelIndex index;
	QString author, title;

	if (!m_db) {
		return;
	}

	model = m_tree->selectionModel();
	selected = model->selectedIndexes();

	selected = model->selectedRows();
	for (int i = 0; i < selected.size(); ++i) {
		index = selected[i];
		author = qVariantValue<QString>(index.data());

		index = index.sibling(index.row(), 1);
		title = qVariantValue<QString>(index.data());

		m_db->deleteBook(author.toStdString(), title.toStdString());
	}
	filter();
	ls();
}

void MainWindow::finish(const QModelIndex& _index)
{
	if (_index.isValid()) {
		m_opened.removeAll(_index);
	}

	/* FIXME Blunt, because everything is updated even without a change */
	filter();
	ls();
}

void MainWindow::printDialog(void)
{
	QPrinter printer(QPrinter::ScreenResolution);
	QPrintDialog dialog(&printer);
/* 	QStandardItem *item; */
	QModelIndex index;
	QString str;
	QRect viewport;
	QFont roman, italic;
	QRect bbox;
	int margin;
	int w;
	int y = 0;
	Rating rating;
	QPalette palette;

	roman.setPointSize(9);
	italic.setPointSize(9);
	italic.setItalic(true);

/* 	m_printdialog->setWindowModality(Qt::WindowModal);
	m_printdialog->setWindowFlags(Qt::Sheet); */
	if (dialog.exec()) {
		QPainter painter(&printer);
		viewport = painter.viewport();
		margin = viewport.width() / 20;
		w = viewport.width();
		for (int i = 0, j = 0; i < m_proxy.rowCount(); ++i, ++j) {
			if (y > viewport.height() - margin * 2) {
				printer.newPage();
				j = 0;
			}
			y = margin + j * viewport.height() / ROWC;

			index = m_proxy.index(i, 0);
			bbox.setRect(margin, y, w / 4, viewport.height() / ROWC);
			if (i % 2 != 0) {
				painter.fillRect(bbox.x(), bbox.y(),
								 margin + w / 4 + w / 3 + w / 6,
								 viewport.height() / ROWC,
								 QBrush(QColor(224, 224, 224)));
			}
			painter.setFont(roman);
			str = qVariantValue<QString>(index.data());
			painter.drawText(bbox, 0, str);

			index = m_proxy.index(i, 1);
			bbox.setRect(margin + w / 4, y, w / 3, viewport.height() / ROWC);
			painter.setFont(italic);
			str = qVariantValue<QString>(index.data());
			painter.drawText(bbox, 0, str);

/* 			item = m_proxy.item(i, 2); */
			index = m_proxy.index(i, 2);
			bbox.setRect(margin + w / 4 + w / 3, y, w / 6,
						 viewport.height() / ROWC);
			#if 0
			/* No, item->data() is not good enough. Mystery... */
			rating = qVariantValue<Rating>(item->index().data());
			#endif
			rating = qVariantValue<Rating>(index.data());
			rating.paint(&painter, bbox, palette, false);

			index = m_proxy.index(i, 3);
			bbox.setRect(margin + w / 4 + w / 3 + w / 6, y,
						 w / 6, viewport.height() / ROWC);
			painter.setFont(roman);
			str = qVariantValue<QString>(index.data());
			painter.drawText(bbox, 0, str);
		}
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

bool BookModel::lessThan(const QModelIndex& _left,
						 const QModelIndex& _right) const
{
	QString rightstr, leftstr;
	Rating leftrating, rightrating;

	if (_left.column() < 2 or _left.column() == 3) {
		leftstr = qVariantValue<QString>(_right.data());
		rightstr = qVariantValue<QString>(_left.data());
		return leftstr.compare(rightstr, Qt::CaseInsensitive) > 0 ?
			true : false;
	} else {
		leftrating = qVariantValue<Rating>(_left.data());
		rightrating = qVariantValue<Rating>(_right.data());
		return leftrating.get() < rightrating.get();
	}
}
