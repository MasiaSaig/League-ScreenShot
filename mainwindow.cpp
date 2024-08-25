#include "mainwindow.h"

#ifndef QT_NO_SYSTEMTRAYICON
#include <QDialog>
#include <QAction>
#include <QCoreApplication>
#include <QCloseEvent>
#include <QLabel>
#include <QLineEdit>
#include <QMenu>
#include <QTextEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QHBoxLayout>
#include <QStyle>

MainWindow::MainWindow(QWidget *parent) //QWidget *parent
    : QMainWindow(parent)
    // , ui(new Ui::MainWindow)
{
        // ui->setupUi(this);

    const QString iconPath = ":/icon.png";
    createActions();
    createTrayIcon(iconPath);

    // creating directory path browsing button
    infoLabel = new QLabel("League of Legends Screenshot directory path");
    pathDirectoryLabel = new QLabel("Directory:");
    pathDirectoryButton = new QPushButton();    // "Browse Directory Path"
    pathDirectoryButton->setIcon(pathDirectoryButton->style()->standardIcon(QStyle::SP_DirHomeIcon));
    pathDirectoryEdit = new QLineEdit("C:/Riot/League of Legends/Screenshot");

    // connecting signals
    connect(pathDirectoryButton, SIGNAL(clicked()), SLOT(browse()));
    connect(pathDirectoryEdit, SIGNAL(editingFinished()), SLOT(onEditingFinished()));

    // creating layouts
    vLayout = new QVBoxLayout();
    vLayout->addWidget(infoLabel);

    hLayout = new QHBoxLayout();
    hLayout->addWidget(pathDirectoryLabel);
    hLayout->addWidget(pathDirectoryEdit);
    hLayout->addWidget(pathDirectoryButton);

    vLayout->addLayout(hLayout);
    setCentralWidget(new QWidget);
    centralWidget()->setLayout(vLayout);


    trayIcon->show();
    setWindowTitle(tr("League Screen Show"));
    setWindowIcon(QIcon(iconPath));
    setFixedSize(400, 64);
}

// MainWindow::~MainWindow()
// {
//     // delete ui;
// }

// void MainWindow::setVisible(bool visible)
// {
//     minimizeAction->setEnabled(visible);
//     restoreAction->setEnabled(isMaximized() || !visible);
//     QMainWindow::setVisible(visible);
// }

void MainWindow::createActions()
{
    minimizeAction = new QAction(tr("Mi&nimize"), this);
    connect(minimizeAction, &QAction::triggered, this, &QWidget::hide);

    restoreAction = new QAction(tr("&Settings"), this);
    connect(restoreAction, &QAction::triggered, this, &QWidget::showNormal);

    quitAction = new QAction(tr("&Quit"), this);
    connect(quitAction, &QAction::triggered, qApp, &QCoreApplication::quit);
}
void MainWindow::createTrayIcon(const QString &iconPath)
{
    trayIconMenu = new QMenu(this);
    trayIconMenu->addAction(minimizeAction);
    trayIconMenu->addAction(restoreAction);
    trayIconMenu->addSeparator();
    trayIconMenu->addAction(quitAction);

    trayIcon = new QSystemTrayIcon(this);
    trayIcon->setContextMenu(trayIconMenu);
    trayIcon->setIcon(QIcon(iconPath));
}

void MainWindow::onEditingFinished()
{
    if(pathDirectoryEdit->text().back() == '/'){
        m_directory = pathDirectoryEdit->text();
    }else{
        m_directory = pathDirectoryEdit->text() + '/';
    }

    qDebug() << m_directory;
}

void MainWindow::browse()
{
    // QString directory = QFileDialog::getExistingDirectory(this, tr("Find Files"), QDir::currentPath());
    QString directory = QFileDialog::getExistingDirectory(this, tr("Open Directory"),
                                                          "/home",
                                                          QFileDialog::ShowDirsOnly
                                                              | QFileDialog::DontResolveSymlinks);
    // check if no directory was selected or directory selection was canceled
    if(directory.isEmpty()){
        return;
    }
    m_directory = directory + '/';
    pathDirectoryEdit->setText(directory + '/');
    qDebug() << m_directory;
}

#endif
