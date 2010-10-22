#include <QtGui>
#include "mainwindow.h"
#include "editdialog.h"
#include "db.h"

MainWindow::MainWindow(void)
{
	QStringList headers;
	EditDialog *editdialog;

	editdialog = new EditDialog();

	m_db = new Db();

	setWindowTitle(tr("Books"));
	menus();
	toolbar();
	statusBar()->showMessage(tr(Db::version()));
	#ifdef Q_WS_MAC
	setUnifiedTitleAndToolBarOnMac(true);
	#endif

	headers << "Author" << "Title" << "Rating" << "Copies";
	m_tree = new QTreeWidget();
	m_tree->setColumnCount(4);
	m_tree->setRootIsDecorated(false);
	m_tree->setHeaderLabels(headers);
	m_tree->setSortingEnabled(true);
	m_tree->setAlternatingRowColors(true);
	connect(m_tree, SIGNAL(doubleClicked(const QModelIndex)),
			editdialog, SLOT(show()));
	setCentralWidget(m_tree);

	filter();
}

int MainWindow::menus(void)
{
	QMenu *file;
	QAction *quit;

	quit = new QAction(tr("&Quit"), this);
	quit->setShortcut(tr("Ctrl+Q"));
	connect(quit, SIGNAL(triggered()), this, SLOT(close()));

	file = menuBar()->addMenu(tr("&File"));
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

void MainWindow::filter(void)
{
	QString str;
	std::string author;
	std::string title;
	int rating, copies;
	QStringList list;
	QTreeWidgetItem *item;
	QList<QTreeWidgetItem *> items;

	str = m_lineedit->text();
	m_db->query(str.toStdString());

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
}
