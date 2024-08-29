#include "filemonitoring.h"

#include <QDebug>
#include <QString>
#include <QTime>
#include <QCoreApplication>

// #include <objidl.h>
#include <gdiplus.h>
#pragma comment(lib,"gdiplus.lib")

DirectoryMonitoringController::DirectoryMonitoringController(DirectoryPath *dirPath) :
    m_dirPath{dirPath}//, ovl{0}
{
    qDebug() << "Initializing DMC.";
}

DirectoryMonitoringController::~DirectoryMonitoringController()
{
    qDebug() << "Cleaning up, done.";
}


void DirectoryMonitoringController::run(){
    qDebug() << "Starting directory monitoring.";
    // main file monitoring loop

    // ::ReadDirectoryChangesW(
    //     m_dirPath->getDHandler(),      // Handle to directory
    //     m_buffer,                      // Buffer to receive changes
    //     BUFFERSIZE,                    // Buffer size
    //     FALSE,                         // Monitor subdirectories
    //     FILE_NOTIFY_CHANGE_FILE_NAME,  // Changes to monitor    //FILE_NOTIFY_CHANGE_FILE_NAME
    //     &m_bytesReturned,              // Number of bytes returned
    //     &ovl,                          // Overlapped structure
    //     NULL                           // Completion routine
    // );

    // while(m_running){
    //     if (::ReadDirectoryChangesW(
    //             m_dirPath->getDHandler(),      // Handle to directory
    //             m_buffer,                      // Buffer to receive changes
    //             BUFFERSIZE,                    // Buffer size
    //             FALSE,                         // Monitor subdirectories
    //             FILE_NOTIFY_CHANGE_FILE_NAME,  // Changes to monitor    //FILE_NOTIFY_CHANGE_FILE_NAME
    //             &m_bytesReturned,              // Number of bytes returned
    //             NULL,                          // Overlapped structure
    //             NULL                           // Completion routine
    //             )) {
    //         FILE_NOTIFY_INFORMATION* fni = reinterpret_cast<FILE_NOTIFY_INFORMATION*>(m_buffer);
    //         if (fni->Action == FILE_ACTION_ADDED) {
    //             qDebug() << "On new file.";
    //             size_t lenFileName = fni->FileNameLength / sizeof(WCHAR);
    //             qDebug() << "New file created: " << std::wstring(fni->FileName, lenFileName);
    //             qDebug() << QString::fromWCharArray(m_dirPath->getDirPath());

    //             // concatenating folder path, with new file name
    //             size_t lenFullFilePath = m_dirPath->getLenDirPath() + lenFileName + 1;
    //             wchar_t* fullFilePath = new wchar_t[lenFullFilePath];
    //             if (fullFilePath == nullptr) {
    //                 qDebug() << "Memmory allocation failed!";
    //                 break;
    //             }
    //             wchar_t* fileName = fni->FileName;

    //             qDebug() << "Len: " << m_dirPath->getLenDirPath() << ". Dir path: " << QString::fromWCharArray(m_dirPath->getDirPath());
    //             wcscpy_s(fullFilePath, m_dirPath->getLenDirPath()+1, m_dirPath->getDirPath());
    //             qDebug() << "Full file Path: " << QString::fromWCharArray(fullFilePath);
    //             wcsncat_s(fullFilePath, lenFullFilePath, fileName, lenFileName);

    //             qDebug() << "Full new file path: "<< fullFilePath;
    //             qDebug() << "Full new file path(to QString): " << QString::fromWCharArray(fullFilePath);
    //             copyToClipboard(fullFilePath);

    //             delete [] fullFilePath;
    //         }
    //     }
    //     else {
    //         qDebug() << "Failed to read directory changes. Error: " << GetLastError();
    //         qDebug() << "Handler to directory: " << m_dirPath->getDHandler();
    //         qDebug() << "Directory path: " << QString::fromWCharArray(m_dirPath->getDirPath());
    //         break;
    //     }
    // }


    while (m_running) {
        // DWORD dw;
        // DWORD result = ::WaitForMultipleObjects(ovl.hEvent, 0);
        // switch (result)
        // {
        // case WAIT_TIMEOUT:
            // background processes
            // qDebug() << "-";
            // break;
        // case WAIT_OBJECT_0:
            // ::GetOverlappedResult(m_dirPath->getDHandler(), &ovl, &dw, FALSE);

            // HANDLE WINAPI m_hCompletionPort = CreateIoCompletionPort(m_dirPath->getDHandler(), NULL, 0, 1);

            if (ReadDirectoryChangesW(
                    m_dirPath->getDHandler(),      // Handle to directory
                    m_buffer,                      // Buffer to receive changes
                    BUFFERSIZE,                    // Buffer size
                    FALSE,                         // Monitor subdirectories
                    FILE_NOTIFY_CHANGE_FILE_NAME,  // Changes to monitor            //FILE_NOTIFY_CHANGE_FILE_NAME | FILE_FLAG_OVERLAPPED
                    &m_bytesReturned,              // Number of bytes returned
                    NULL,                          // Overlapped structure          //  //&ovl
                    NULL                           // Completion routine
                    )) {
                FILE_NOTIFY_INFORMATION* fni = reinterpret_cast<FILE_NOTIFY_INFORMATION*>(m_buffer);
                if (fni->Action == FILE_ACTION_ADDED) {
                    qDebug() << "On new file.";
                    size_t lenFileName = fni->FileNameLength / sizeof(WCHAR);
                    qDebug() << "New file created: " << std::wstring(fni->FileName, lenFileName);
                    qDebug() << QString::fromWCharArray(m_dirPath->getDirPath());

                    // concatenating folder path, with new file name
                    size_t lenFullFilePath = m_dirPath->getLenDirPath() + lenFileName + 1;
                    wchar_t* fullFilePath = new wchar_t[lenFullFilePath];
                    if (fullFilePath == nullptr) {
                        qDebug() << "Memmory allocation failed!";
                        break;
                    }
                    wchar_t* fileName = fni->FileName;

                    qDebug() << "Len: " << m_dirPath->getLenDirPath() << ". Dir path: " << QString::fromWCharArray(m_dirPath->getDirPath());
                    wcscpy_s(fullFilePath, m_dirPath->getLenDirPath()+1, m_dirPath->getDirPath());
                    qDebug() << "Full file Path: " << QString::fromWCharArray(fullFilePath);
                    wcsncat_s(fullFilePath, lenFullFilePath, fileName, lenFileName);

                    qDebug() << "Full new file path: "<< fullFilePath;
                    qDebug() << "Full new file path(to QString): " << QString::fromWCharArray(fullFilePath);
                    copyToClipboard(fullFilePath);

                    delete [] fullFilePath;
                }
            }
            else {
                int error_status = GetLastError();
                if(error_status == ERROR_SUCCESS){
                    qDebug() << "Closing loop. Running loop again.";
                }else if(error_status == ERROR_OPERATION_ABORTED){
                    qDebug() << "Directory changed. Running loop again.";
                }else{
                    qDebug() << "\\/-------------------------------------------------\\/";
                    qDebug() << "ERROR! Failed to read directory changes. Error: " << error_status;
                    qDebug() << "Handler to directory: " << m_dirPath->getDHandler();
                    qDebug() << "Directory path: " << QString::fromWCharArray(m_dirPath->getDirPath());
                    qDebug() << "/\\-------------------------------------------------/\\";
                    break;
                }
            }

        qDebug() << "New change in directory." << QTime::currentTime();
        // QThread::sleep(static_cast<std::chrono::nanoseconds>(500000000));
        // QThread::sleep(1);
    }
    qDebug() << "Ending directory monitoring.";
}

void DirectoryMonitoringController::copyToClipboard(const WCHAR* filePath)
{
    qDebug() << "Trying to copy image to clipboard.";
    qDebug() << QString::fromWCharArray(filePath);
    // wait for a file to finish being copied, for max time of maxWaitTime(2 seconds)
    QTime maxWaitTime = QTime::currentTime().addSecs(2);
    int bitmapStatus;
    // TODO: this seems to not close files, so.. fix it mabe
    Gdiplus::Bitmap* gdibmp = Gdiplus::Bitmap::FromFile(filePath);
    while((QTime::currentTime() < maxWaitTime) && (bitmapStatus != Gdiplus::Ok)){
        QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
        gdibmp = Gdiplus::Bitmap::FromFile(filePath);
        bitmapStatus = gdibmp->GetLastStatus();
    }
    if (bitmapStatus != Gdiplus::Ok){
        qDebug() << "Wrong gdibmp status." << bitmapStatus ;
        return;
    }
    HBITMAP hbitmap;
    auto status = gdibmp->GetHBITMAP(0, &hbitmap);
    if (status != Gdiplus::Ok) {
        qDebug() << "Wrong bitmap status. - " << status;
        return;
    }
    BITMAP bm;
    GetObject(hbitmap, sizeof bm, &bm);
    DIBSECTION ds;
    if (sizeof ds == GetObject(hbitmap, sizeof ds, &ds)){
        qDebug() << "Got object.";
        HDC hdc = GetDC(NULL);
        HBITMAP hbitmap_ddb = CreateDIBitmap(hdc, &ds.dsBmih, CBM_INIT,
                                             ds.dsBm.bmBits, (BITMAPINFO*)&ds.dsBmih, DIB_RGB_COLORS);
        ReleaseDC(NULL, hdc);

        if (OpenClipboard(NULL)){
            qDebug() << "Copying image.";
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

    delete gdibmp;
    qDebug() << "Finished copying to clipboard.";

}

void DirectoryMonitoringController::quit(){
    CancelIoEx(m_dirPath->getDHandler(), NULL);
    m_running = false;
}
