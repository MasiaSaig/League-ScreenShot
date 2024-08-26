#include "filemonitoring.h"

#include <QDebug>
#include <QString>
#include <objidl.h>
#include <gdiplus.h>
#pragma comment(lib,"gdiplus.lib")

DirectoryMonitoringController::DirectoryMonitoringController(const wchar_t* dirPath)
{
    // TODO: load data from file
    // m_dirPath = ;

    // m_directoryMonitoringThread = QThread::create(runDirectoryMonitoring());

    // Gdi Plus initialization
    Gdiplus::GdiplusStartupInput gdiplusStartupInput;
    Gdiplus::GdiplusStartup(&m_gdiplusToken, &gdiplusStartupInput, nullptr);

    qDebug() << "Initialization directory path: " << QString::fromWCharArray(m_dirPath);
    changeDirectory("H:/Riot Games/League of Legends/Screenshots/");
}

DirectoryMonitoringController::~DirectoryMonitoringController()
{
    quit();
    CloseHandle(m_hDir);
    Gdiplus::GdiplusShutdown(m_gdiplusToken);
}


void DirectoryMonitoringController::runDirectoryMonitoring()
{
    // main file monitoring loop
    while (m_running) {
        if (ReadDirectoryChangesW(
                m_hDir,                        // Handle to directory
                m_buffer,                      // Buffer to receive changes
                BUFFERSIZE,                    // Buffer size
                FALSE,                         // Monitor subdirectories
                FILE_NOTIFY_CHANGE_FILE_NAME,  // Changes to monitor
                &m_bytesReturned,              // Number of bytes returned
                NULL,                          // Overlapped structure
                NULL                           // Completion routine
                )) {
            FILE_NOTIFY_INFORMATION* fni = reinterpret_cast<FILE_NOTIFY_INFORMATION*>(m_buffer);
            if (fni->Action == FILE_ACTION_ADDED) {
                size_t lenFileName = fni->FileNameLength / sizeof(WCHAR);
                qDebug() << L"New file created: " << std::wstring(fni->FileName, lenFileName);

                // concatenating folder path, with new file name
                size_t lenFullFilePath = m_lenDirPath + lenFileName + 1;
                wchar_t* fullFilePath = new wchar_t[lenFullFilePath];
                if (fullFilePath == nullptr) {
                    qDebug() << "Memmory allocation failed!";
                    break;
                }
                wchar_t* fileName = fni->FileName;

                wcscpy_s(fullFilePath, m_lenDirPath+1, m_dirPath);
                wcsncat_s(fullFilePath, lenFullFilePath, fileName, lenFileName);

                qDebug() << fullFilePath;
                copyToClipboard(fullFilePath);

                delete [] fullFilePath;
            }
        }
        else {
            qDebug() << "Failed to read directory changes. Error: " << GetLastError();
            qDebug() << "Directory path: " << m_dirPath;
            break;
        }
    }
    emit finished();
}

void DirectoryMonitoringController::changeDirectory(QString dirPath)
{
    qDebug() << "Directory before replace: " << dirPath;
    dirPath = dirPath.replace('/', "\\");
    qDebug() << "Directory after replace: " << dirPath;
    auto archiveWString = dirPath.toStdWString();
    m_dirPath = const_cast<wchar_t *>(archiveWString.c_str());
    m_lenDirPath = wcslen(m_dirPath);
    qDebug() << "Directory after replace and conversion" << QString::fromWCharArray(m_dirPath);

    m_hDir = CreateFile(
        m_dirPath,    // Directory to monitor
        FILE_LIST_DIRECTORY,                // Access type
        FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, // Share mode
        NULL,                               // Security attributes
        OPEN_EXISTING,                      // Creation disposition     //CREATE_NEW
        FILE_FLAG_BACKUP_SEMANTICS,         // Flags and attributes, FILE_FLAG_BACKUP_SEMANTICS needed for directories
        NULL                                // Template file
        );

    // if folder already exists, create handler to existing folder
    if ((m_hDir == INVALID_HANDLE_VALUE) && (GetLastError() == ERROR_FILE_EXISTS)) {
        m_hDir = CreateFile(
            m_dirPath,
            FILE_LIST_DIRECTORY,
            FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
            NULL,
            OPEN_EXISTING,
            FILE_FLAG_BACKUP_SEMANTICS,
            NULL
            );
    }

    if (m_hDir == INVALID_HANDLE_VALUE) {
        qDebug() << "Failed to get directory handle. Error: " << GetLastError() << " Directory path:" << dirPath;
    }
}

void DirectoryMonitoringController::copyToClipboard(const wchar_t* filePath)
{
    qDebug() << filePath;
    Gdiplus::Bitmap gdibmp(filePath);
    if (gdibmp.GetLastStatus() != Gdiplus::Ok)
        return;
    HBITMAP hbitmap;
    auto status = gdibmp.GetHBITMAP(0, &hbitmap);
    if (status != Gdiplus::Ok) {
        qDebug() << "Wrong bitmap status. - " << status;
        return;
    }
    BITMAP bm;
    GetObject(hbitmap, sizeof bm, &bm);
    DIBSECTION ds;
    if (sizeof ds == GetObject(hbitmap, sizeof ds, &ds)){
        HDC hdc = GetDC(NULL);
        HBITMAP hbitmap_ddb = CreateDIBitmap(hdc, &ds.dsBmih, CBM_INIT,
                                             ds.dsBm.bmBits, (BITMAPINFO*)&ds.dsBmih, DIB_RGB_COLORS);
        ReleaseDC(NULL, hdc);

        if (OpenClipboard(NULL)){
            EmptyClipboard();
            //DWORD CF_PNG = RegisterClipboardFormat(L"PNG");
            SetClipboardData(CF_BITMAP, hbitmap_ddb);
            CloseClipboard();
        }
        else {
            qDebug() << "Couldn't open clipboard. - " << GetLastError();
        }
        DeleteObject(hbitmap_ddb);
    }
    DeleteObject(hbitmap);
}

void DirectoryMonitoringController::quit(){
    m_running = false;
}
