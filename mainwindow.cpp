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

    // Gdi Plus initialization
    Gdiplus::GdiplusStartupInput gdiplusStartupInput;
    Gdiplus::GdiplusStartup(&m_gdiplusToken, &gdiplusStartupInput, nullptr);

    const QString iconPath = ":/icon.png";
    createActions();
    createTrayIcon(iconPath);

    // creating directory path browsing button
    infoLabel = new QLabel("League of Legends Screenshot directory path");
    pathDirectoryLabel = new QLabel("Directory:");
    pathDirectoryButton = new QPushButton();    // "Browse Directory Path"
    pathDirectoryButton->setIcon(pathDirectoryButton->style()->standardIcon(QStyle::SP_DirHomeIcon));
    // TODO: load path from file
    pathDirectoryEdit = new QLineEdit("H:/Riot Games/League of Legends/Screenshots");

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

    // preparing Directory Controller class
    m_dirPath = new DirectoryPath("H:/Riot Games/League of Legends/Screenshots/");
    qDebug() << "dirPath addres: " << m_dirPath;
    qDebug() << "dirPath path: " << QString::fromWCharArray(m_dirPath->getDirPath());
    // creating a new thread and starting it
    m_dirMonitoringController = new DirectoryMonitoringController(m_dirPath);
    qDebug() << "Starting threads.";
    m_dirMonitoringController->start();

}

MainWindow::~MainWindow()
{
    // closing thread and cleaning up data
    qDebug() << "Cleaning up MainWindow. Closing threads.";
    m_dirMonitoringController->quit();
    // using terminate is discouraged :/
    m_dirMonitoringController->terminate();
    m_dirMonitoringController->wait();
    Gdiplus::GdiplusShutdown(m_gdiplusToken);

    delete m_dirMonitoringController;
    delete m_dirPath;

    delete quitAction; delete restoreAction; delete minimizeAction;
    delete pathDirectoryButton; delete pathDirectoryEdit; delete pathDirectoryLabel;
    delete infoLabel;
    delete vLayout;
    // delete hLayout;
}

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
    qDebug() << "Editing Finished, path: " << pathDirectoryEdit->text();
    if(pathDirectoryEdit->text().back() == '/'){
        m_dirPath->setDirPath(pathDirectoryEdit->text());
    }else{
        m_dirPath->setDirPath(pathDirectoryEdit->text() + '/');
    }
}

void MainWindow::browse()
{
    QString directory = QFileDialog::getExistingDirectory(this, tr("Open Directory"),
                                                          "/home",
                                                          QFileDialog::ShowDirsOnly
                                                              | QFileDialog::DontResolveSymlinks);
    // check if no directory was selected or directory selection was canceled
    if(directory.isEmpty()){
        return;
    }
    pathDirectoryEdit->setText(directory + '/');
    qDebug() << "Browse, setting path:" << directory;
    m_dirPath->setDirPath(directory + '/');
}


#endif
