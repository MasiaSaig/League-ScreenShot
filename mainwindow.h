#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QSystemTrayIcon>

#ifndef QT_NO_SYSTEMTRAYICON
#include "filemonitoring.h"
#include <QMainWindow>
#include <QDialog>
#include <QFileDialog>

#include <windows.h>
#include <gdiplus.h>
#pragma comment(lib,"gdiplus.lib")

#include "directorypath.h"

QT_BEGIN_NAMESPACE
class QAction;
class QCheckBox;
class QComboBox;
class QGroupBox;
class QLabel;
class QLineEdit;
class QMenu;
class QPushButton;
class QSpinBox;
class QTextEdit;
class QPushButton;
class QHBoxLayout;
class QVBoxLayout;
QT_END_NAMESPACE

// QT_BEGIN_NAMESPACE
// namespace Ui {
// class MainWindow;
// }
// QT_END_NAMESPACE

class MainWindow :  public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void browse();
    void onEditingFinished();

signals:
    void changingDirectory(QString dirPath);

private:
    void createActions();
    void createTrayIcon(const QString &iconPath);
    void updateUI();

    QVBoxLayout *vLayout;
    QHBoxLayout *hLayoutTop;
    QHBoxLayout *hLayoutBottom;
    QLabel *infoLabel;
    QLabel *pathCorrectionLabel;
    QLabel *pathDirectoryLabel;
    QLineEdit *pathDirectoryEdit;
    QPushButton *pathDirectoryButton;

    QAction *minimizeAction;
    QAction *restoreAction;
    QAction *quitAction;

    QSystemTrayIcon *trayIcon;
    QMenu *trayIconMenu;

private:
    DirectoryPath *m_dirPath;
    DirectoryMonitoringController *m_dirMonitoringController;

private:
    ULONG_PTR m_gdiplusToken;
};

#endif // QT_NO_SYSTEMTRAYICON
#endif // MAINWINDOW_H
