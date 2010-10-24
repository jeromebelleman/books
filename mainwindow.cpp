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
	QStringList headers;
	EditDialog *editdialog;

	connect(this, SIGNAL(closed(void)), this, SLOT(bye(void)));

	filebrowser();
	
	resize(640, 480);
	setWindowTitle(tr("Books"));
	menus();
	toolbar();
	#ifdef Q_WS_MAC
	setUnifiedTitleAndToolBarOnMac(true);
	#endif

	editdialog = new EditDialog();

	headers << "Author" << "Title" << "Rating" << "Copies";
	m_tree = new QTreeWidget();
	m_tree->setColumnCount(4);
	m_tree->setColumnWidth(0, 200);
	m_tree->setColumnWidth(1, 200);
	m_tree->setColumnWidth(2, 50);
	m_tree->setRootIsDecorated(false);
	m_tree->setHeaderLabels(headers);
	m_tree->setSortingEnabled(true);
	m_tree->setAlternatingRowColors(true);
	connect(m_tree, SIGNAL(doubleClicked(const QModelIndex)),
			editdialog, SLOT(show()));
	setCentralWidget(m_tree);

	m_cnf.read();
	rc = m_cnf.get("dbpath", &path);
	if (!rc) {
		m_db = new Db(path);
		rc = filter();
		if (rc) {
			statusBar()->showMessage(tr("Invalid DB file"));
		}
	}
}

int MainWindow::menus(void)
{
	QMenu *file;
	QAction *open, *quit;

	open = new QAction(tr("&Open"), this);
	open->setShortcut(tr("Ctrl+O"));
	connect(open, SIGNAL(triggered(void)), this, SLOT(open(void)));

	quit = new QAction(tr("&Quit"), this);
	quit->setShortcut(tr("Ctrl+Q"));
	connect(quit, SIGNAL(triggered(void)), this, SLOT(bye(void)));

	file = menuBar()->addMenu(tr("&File"));
	file->addAction(open);
	file->addAction(quit);
	
	return 0;
}

int MainWindow::toolbar(void)
{
	QToolBar *toolbar;
	QAction *newbook;

	newbook = new QAction(tr("New Book"), this);

	m_lineedit = new QLineEdit();
	connect(m_lineedit, SIGNAL(textEdited(const QString)),
			this, SLOT(filter(void)));

	toolbar = addToolBar("Toolbar");
	toolbar->addAction(newbook);
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
	QStringList list;
	QTreeWidgetItem *item;
	QList<QTreeWidgetItem *> items;

	if (!m_tree) {
		return 1;
	}

	if (!m_db) {
		return 1;
	}

	str = m_lineedit->text();
	rc = m_db->query(str.toStdString());
	if (rc) {
		m_tree->clear();
		return 1;
	}

	m_tree->clear();
	while (m_db->next(&author, &title, &rating, &copies) == 0) {
		list = QStringList();
		list << QString::fromUtf8(author.c_str()) << 
			QString::fromUtf8(title.c_str()) << QString::number(rating) <<
			QString::number(copies);
		item = new QTreeWidgetItem((QTreeWidget *) 0, list);
		items.append(item);
	}
	m_tree->insertTopLevelItems(0, items);

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
