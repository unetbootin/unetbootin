#include "remixosudisk.h"

void ExecuteShell(wchar_t *parameters)
{
    SHELLEXECUTEINFO shExecInfo = {0};
    shExecInfo.cbSize = sizeof(SHELLEXECUTEINFO);
    shExecInfo.fMask = SEE_MASK_NOCLOSEPROCESS;
    shExecInfo.hwnd = NULL;
    shExecInfo.lpVerb = NULL;
    shExecInfo.lpFile = L"cmd.exe";
    shExecInfo.lpParameters = parameters;
    shExecInfo.lpDirectory = NULL;
    shExecInfo.nShow = SW_HIDE;
    shExecInfo.hInstApp = NULL;
    ShellExecuteEx(&shExecInfo);
    WaitForSingleObject(shExecInfo.hProcess,INFINITE);
}

/*
 * construct and initialize
 */

RemixOSUDisk::RemixOSUDisk(WCHAR* wszDrive)
{
    wmemset(m_wszDrive, 0, MAX_PATH + 1);
    wmemset(m_wszPhysicalName, 0, MAX_PATH + 1);
    memset(&m_volumeSize, 0, sizeof(REMIXOS_VOLUME_SIZE));
    memset(&m_diskGeometryEx, 0, sizeof(DISK_GEOMETRY_EX));
    wcsncpy(m_wszDrive, wszDrive, MAX_PATH);
    m_bValidUDisk = FALSE;
    m_bRemixOSUDisk = FALSE;
    m_bFormat = FALSE;
}

void RemixOSUDisk::FormatDataPartition(BOOL bFormat)
{
    m_bFormat = bFormat;
}

void RemixOSUDisk::Initialize()
{
    if(GetPhysicalDiskName())
    {
        m_bValidUDisk = CheckUDisk();
        if(m_bValidUDisk)
        {
            GetVolumeSize();
            m_bRemixOSUDisk = ReadRemixOSInfo();
        }
    }
}

typedef struct _STORAGE_DEVICE_NUMBER {
  DEVICE_TYPE DeviceType;
  ULONG       DeviceNumber;
  ULONG       PartitionNumber;
} STORAGE_DEVICE_NUMBER, *PSTORAGE_DEVICE_NUMBER;

BOOL RemixOSUDisk::GetPhysicalDiskName()
{
    HANDLE hDevice = INVALID_HANDLE_VALUE;
    WCHAR wszDrivePath[MAX_PATH];
    swprintf(wszDrivePath, L"\\\\.\\%c:", m_wszDrive[0]);

    hDevice = CreateFileW(wszDrivePath,
                          GENERIC_READ | GENERIC_WRITE,
                          FILE_SHARE_READ | FILE_SHARE_WRITE,
                          NULL,
                          OPEN_EXISTING,
                          0,
                          NULL);
    if (hDevice == INVALID_HANDLE_VALUE)
    {
        return FALSE;
    }

    BOOL bResult = FALSE;

    DWORD dwBytesReturned = 0;
    STORAGE_DEVICE_NUMBER devnr;

    bResult = DeviceIoControl(hDevice,
                              IOCTL_STORAGE_GET_DEVICE_NUMBER,
                              NULL,
                              0,
                              &devnr,
                              sizeof(STORAGE_DEVICE_NUMBER),
                              &dwBytesReturned,
                              NULL);

    if (bResult && devnr.DeviceType==FILE_DEVICE_DISK)
    {
        swprintf(m_wszPhysicalName, L"\\\\.\\PhysicalDrive%d", devnr.DeviceNumber);
    }
    else
    {
        bResult = FALSE;
    }

    CloseHandle(hDevice);

    return bResult;
}

BOOL RemixOSUDisk::CheckUDisk()
{
    DWORD dwBytesReturned = 0;
    HANDLE hDevice = INVALID_HANDLE_VALUE;
    BOOL bResult = FALSE;

    hDevice = CreateFileW(m_wszPhysicalName,
                          GENERIC_READ | GENERIC_WRITE,
                          FILE_SHARE_READ | FILE_SHARE_WRITE,
                          NULL, OPEN_EXISTING,
                          FILE_ATTRIBUTE_NORMAL,
                          NULL);
    if (hDevice == INVALID_HANDLE_VALUE)
    {
        return FALSE;
    }

    bResult = DeviceIoControl(hDevice,
                              IOCTL_DISK_GET_DRIVE_GEOMETRY_EX,
                              NULL,
                              0,
                              &m_diskGeometryEx,
                              sizeof(DISK_GEOMETRY_EX),
                              &dwBytesReturned,
                              NULL );
    if (bResult)
    {
        LONGLONG GB = 1073741824LL;
        //2GB + 4GB + 1GB(include 2048*512)
        LONGLONG min_udisk = 2*GB + 4*GB + GB;
        if (m_diskGeometryEx.DiskSize.QuadPart >= min_udisk)
            bResult = TRUE;
        else
            bResult = FALSE;
    }

    CloseHandle(hDevice);
    return bResult;
}

BOOL RemixOSUDisk::IsValidUDisk()
{
    return m_bValidUDisk;
}

BOOL RemixOSUDisk::IsRemixOSUDisk()
{
    return m_bRemixOSUDisk;
}

BOOL RemixOSUDisk::ReadRemixOSInfo()
{
    WCHAR volumeName[MAX_PATH + 1] = { 0 };
    WCHAR fileSystemName[MAX_PATH + 1] = { 0 };
    DWORD serialNumber = 0;
    DWORD maxComponentLen = 0;
    DWORD fileSystemFlags = 0;
    WCHAR driveName[10] = { 0 };

    wmemset(driveName, 0, 10);
    wsprintf(driveName, L"%c:\\", m_wszDrive[0]);

    if (GetVolumeInformationW(driveName,
                              volumeName,
                              ARRAYSIZE(volumeName),
                              &serialNumber,
                              &maxComponentLen,
                              &fileSystemFlags,
                              fileSystemName,
                              ARRAYSIZE(fileSystemName)))
    {
        if (wcscmp (REMIXOS_SHARE_LABEL,volumeName) == 0)
            return TRUE;
    }

    return FALSE;
}

void RemixOSUDisk::GetVolumeSize()
{
    /*
     * System Volume = 2.5GB
     * 8GB: DataVolume = 4GB
     * 16GB: DataVolume = 8GB
     * 32GB: DataVolume = 16GB
     * 64GB: DataVolume = 32GB
     */
    LONGLONG GB = 1073741824LL;
    m_volumeSize.MBRTable = 1048576LL;
    m_volumeSize.SystemVolume = 5*GB;

    m_volumeSize.ShareVolume = m_diskGeometryEx.DiskSize.QuadPart - m_volumeSize.MBRTable - m_volumeSize.SystemVolume;

    if (m_diskGeometryEx.DiskSize.QuadPart <= (8*GB + m_volumeSize.SystemVolume))
    {
        m_volumeSize.DataVolume = 1.5*GB;
    }
    else if (m_diskGeometryEx.DiskSize.QuadPart <= (16*GB + m_volumeSize.SystemVolume))
    {
        m_volumeSize.DataVolume = 8*GB;
    }
    else if (m_diskGeometryEx.DiskSize.QuadPart <= (32*GB + m_volumeSize.SystemVolume))
    {
        m_volumeSize.DataVolume = 16*GB;
    }
    else if (m_diskGeometryEx.DiskSize.QuadPart <= (64*GB + m_volumeSize.SystemVolume))
    {
        m_volumeSize.DataVolume = 32*GB;
    }
    else
    {
        m_volumeSize.DataVolume = 64*GB;
    }

    m_volumeSize.ShareVolume -= m_volumeSize.DataVolume;
}

/*
 * handle volume
 */

BOOL RemixOSUDisk::DeleteExistDriveFromUDisk()
{
    HANDLE hDevice = INVALID_HANDLE_VALUE;
    BOOL bResult = FALSE;

    hDevice = CreateFileW(m_wszPhysicalName,
                          GENERIC_READ | GENERIC_WRITE,
                          FILE_SHARE_READ | FILE_SHARE_WRITE,
                          NULL,
                          OPEN_EXISTING,
                          0,
                          NULL);
    if (hDevice == INVALID_HANDLE_VALUE)
    {
        return FALSE;
    }

    DWORD dwRet = 0;
    bResult = DeviceIoControl(hDevice, FSCTL_LOCK_VOLUME, NULL, 0, NULL, 0, &dwRet, NULL);
    if (bResult)
    {
        bResult = DeviceIoControl(hDevice, FSCTL_DISMOUNT_VOLUME, NULL, 0, NULL, 0, &dwRet, NULL);
        if(bResult)
        {
            bResult = DeviceIoControl(hDevice, IOCTL_DISK_DELETE_DRIVE_LAYOUT, NULL, 0, NULL, 0, &dwRet, NULL);
        }
    }

    CloseHandle(hDevice);

    return (bResult);
}

BOOL RemixOSUDisk::CreateDiskInUDisk()
{
    DWORD dwBytesReturned = 0;
    HANDLE hDevice = INVALID_HANDLE_VALUE;
    BOOL bResult = FALSE;

    hDevice = CreateFileW(m_wszPhysicalName,
                          GENERIC_READ | GENERIC_WRITE,
                          FILE_SHARE_READ | FILE_SHARE_WRITE,
                          NULL,
                          OPEN_EXISTING,
                          FILE_ATTRIBUTE_NORMAL,
                          NULL);
    if (hDevice == INVALID_HANDLE_VALUE)
    {
        return FALSE;
    }

    CREATE_DISK cd;
    cd.PartitionStyle = PARTITION_STYLE_MBR;
    cd.Mbr.Signature = DISK_SIGNATURE;
    bResult = DeviceIoControl(hDevice, IOCTL_DISK_CREATE_DISK, &cd, sizeof(cd), NULL, 0, &dwBytesReturned, NULL);
    if (bResult)
    {
        bResult = DeviceIoControl(hDevice, IOCTL_DISK_UPDATE_PROPERTIES, NULL, 0, NULL, 0, &dwBytesReturned, NULL);
    }

    CloseHandle(hDevice);
    return bResult;
}

BOOL RemixOSUDisk::CreateDrivesInUDisk(int type)
{
    HANDLE hDevice = INVALID_HANDLE_VALUE;
    BOOL bResult = FALSE;
    hDevice = CreateFileW(m_wszPhysicalName,
                          GENERIC_READ | GENERIC_WRITE,
                          FILE_SHARE_READ | FILE_SHARE_WRITE,
                          NULL,
                          OPEN_EXISTING,
                          FILE_ATTRIBUTE_NORMAL,
                          NULL);
    if (hDevice == INVALID_HANDLE_VALUE)
    {
        return FALSE;
    }

    DWORD driveLayoutSize = sizeof(DRIVE_LAYOUT_INFORMATION_EX) + sizeof(PARTITION_INFORMATION_EX) * 3 * 25;
    DRIVE_LAYOUT_INFORMATION_EX* dl = (DRIVE_LAYOUT_INFORMATION_EX*) new BYTE[driveLayoutSize];
    memset(dl, 0, driveLayoutSize);
    DWORD dwBytesReturned = 0;

    DWORD SectorSize = 512;

    LONGLONG offset = 0LL;

    dl->Mbr.Signature = DISK_SIGNATURE;
    dl->PartitionStyle = PARTITION_STYLE_MBR;
    dl->PartitionCount = type;
    if (type == 1)
    {
        dl->PartitionEntry[0].PartitionStyle = PARTITION_STYLE_MBR;
        offset = m_volumeSize.MBRTable + m_volumeSize.ShareVolume + m_volumeSize.SystemVolume;
        dl->PartitionEntry[0].StartingOffset.QuadPart= offset;
        dl->PartitionEntry[0].PartitionLength.QuadPart = m_volumeSize.DataVolume;
        dl->PartitionEntry[0].PartitionNumber = 3;
        dl->PartitionEntry[0].RewritePartition = TRUE;
        dl->PartitionEntry[0].Mbr.PartitionType = 0x83;
        dl->PartitionEntry[0].Mbr.BootIndicator = FALSE;
        dl->PartitionEntry[0].Mbr.RecognizedPartition = TRUE;
        dl->PartitionEntry[0].Mbr.HiddenSectors = (offset)/SectorSize;
    }
    else if (type == 2)
    {
        dl->PartitionEntry[0].PartitionStyle = PARTITION_STYLE_MBR;
        offset = m_volumeSize.MBRTable + m_volumeSize.ShareVolume;
        dl->PartitionEntry[0].StartingOffset.QuadPart= offset;
        dl->PartitionEntry[0].PartitionLength.QuadPart = m_volumeSize.SystemVolume;
        dl->PartitionEntry[0].PartitionNumber = 2;
        dl->PartitionEntry[0].RewritePartition = TRUE;
        dl->PartitionEntry[0].Mbr.PartitionType = PARTITION_FAT32;
        dl->PartitionEntry[0].Mbr.BootIndicator = TRUE;
        dl->PartitionEntry[0].Mbr.RecognizedPartition = TRUE;
        dl->PartitionEntry[0].Mbr.HiddenSectors = (offset)/SectorSize;

        dl->PartitionEntry[1].PartitionStyle = PARTITION_STYLE_MBR;
        offset = m_volumeSize.MBRTable + m_volumeSize.ShareVolume + m_volumeSize.SystemVolume;
        dl->PartitionEntry[1].StartingOffset.QuadPart= offset;
        dl->PartitionEntry[1].PartitionLength.QuadPart = m_volumeSize.DataVolume;
        dl->PartitionEntry[1].PartitionNumber = 3;
        dl->PartitionEntry[1].RewritePartition = TRUE;
        dl->PartitionEntry[1].Mbr.PartitionType = 0x83;
        dl->PartitionEntry[1].Mbr.BootIndicator = FALSE;
        dl->PartitionEntry[1].Mbr.RecognizedPartition = TRUE;
        dl->PartitionEntry[1].Mbr.HiddenSectors = (offset)/SectorSize;
    }
    else if (type == 3)
    {
        dl->PartitionEntry[0].PartitionStyle = PARTITION_STYLE_MBR;
        dl->PartitionEntry[0].StartingOffset.QuadPart = m_volumeSize.MBRTable;
        dl->PartitionEntry[0].PartitionLength.QuadPart = m_volumeSize.ShareVolume;
        dl->PartitionEntry[0].PartitionNumber = 1;
        dl->PartitionEntry[0].RewritePartition = TRUE;
        dl->PartitionEntry[0].Mbr.PartitionType = 0x7;
        dl->PartitionEntry[0].Mbr.BootIndicator = FALSE;
        dl->PartitionEntry[0].Mbr.RecognizedPartition = TRUE;
        dl->PartitionEntry[0].Mbr.HiddenSectors = m_volumeSize.MBRTable/SectorSize;

        dl->PartitionEntry[1].PartitionStyle = PARTITION_STYLE_MBR;
        offset = m_volumeSize.MBRTable + m_volumeSize.ShareVolume;
        dl->PartitionEntry[1].StartingOffset.QuadPart= offset;
        dl->PartitionEntry[1].PartitionLength.QuadPart = m_volumeSize.SystemVolume;
        dl->PartitionEntry[1].PartitionNumber = 2;
        dl->PartitionEntry[1].RewritePartition = TRUE;
        dl->PartitionEntry[1].Mbr.PartitionType = PARTITION_FAT32;
        dl->PartitionEntry[1].Mbr.BootIndicator = TRUE;
        dl->PartitionEntry[1].Mbr.RecognizedPartition = TRUE;
        dl->PartitionEntry[1].Mbr.HiddenSectors = (offset)/SectorSize;

        dl->PartitionEntry[2].PartitionStyle = PARTITION_STYLE_MBR;
        offset = m_volumeSize.MBRTable + m_volumeSize.ShareVolume + m_volumeSize.SystemVolume;
        dl->PartitionEntry[2].StartingOffset.QuadPart= offset;
        dl->PartitionEntry[2].PartitionLength.QuadPart = m_volumeSize.DataVolume;
        dl->PartitionEntry[2].PartitionNumber = 3;
        dl->PartitionEntry[2].RewritePartition = TRUE;
        dl->PartitionEntry[2].Mbr.PartitionType = 0x83;
        dl->PartitionEntry[2].Mbr.BootIndicator = FALSE;
        dl->PartitionEntry[2].Mbr.RecognizedPartition = TRUE;
        dl->PartitionEntry[2].Mbr.HiddenSectors = (offset)/SectorSize;
    }

    if(type>0 && type<4)
    {
        bResult = DeviceIoControl(hDevice,
                                  IOCTL_DISK_SET_DRIVE_LAYOUT_EX,
                                  dl,
                                  driveLayoutSize,
                                  NULL,
                                  0,
                                  &dwBytesReturned,
                                  NULL);
        if (bResult)
        {
            bResult = DeviceIoControl(hDevice,
                                      IOCTL_DISK_UPDATE_PROPERTIES,
                                      NULL,
                                      0,
                                      NULL,
                                      0,
                                      &dwBytesReturned,
                                      NULL);
            if (bResult)
            {
                bResult = DeviceIoControl(hDevice,
                                          IOCTL_DISK_GET_DRIVE_LAYOUT_EX,
                                          NULL,
                                          0,
                                          dl,
                                          driveLayoutSize,
                                          &dwBytesReturned,
                                          NULL);
            }
        }
    }

    CloseHandle(hDevice);

    return bResult;
}

BOOL RemixOSUDisk::PrepareUDisk()
{
    if(!m_bValidUDisk)
        return FALSE;

    BOOL bResult = FALSE;
    bResult = DeleteExistDriveFromUDisk();
    if(!bResult)
    {
        return FALSE;
    }
    bResult = CreateDiskInUDisk();
    if(!bResult)
    {
        return FALSE;
    }
    bResult = CreateDrivesInUDisk(1);
    if(!bResult)
    {
        return FALSE;
    }
    if(m_bFormat)
    {
        WCHAR wszCmdLine[MAX_PATH];
        wmemset(wszCmdLine, 0, MAX_PATH);
        wsprintf(wszCmdLine, L" /C format %c: /Q /FS:FAT32 /Y", m_wszDrive[0]);
        ExecuteShell(wszCmdLine);
        SetVolumeLabelW(m_wszDrive, REMIXOS_DATA_LABEL);
    }

    bResult = CreateDrivesInUDisk(2);
    if(!bResult)
    {
        return FALSE;
    }
    if (!m_bRemixOSUDisk || m_bFormat)
    {
        WCHAR wszCmdLine[MAX_PATH];
        wmemset(wszCmdLine, 0, MAX_PATH);
        wsprintf(wszCmdLine, L" /C format %c: /Q /FS:FAT32 /Y", m_wszDrive[0]);
        ExecuteShell(wszCmdLine);
    }

    SetVolumeLabelW(m_wszDrive, REMIXOS_SYS_LABEL);

    return bResult;
}

BOOL RemixOSUDisk::UpdateUDisk()
{
    if(!m_bValidUDisk)
    {
        return FALSE;
    }

    BOOL bResult = FALSE;

    bResult = CreateDrivesInUDisk(3);
    if(!bResult)
    {
        return FALSE;
    }
    if (!m_bRemixOSUDisk || m_bFormat)
    {
        WCHAR wszCmdLine[MAX_PATH];
        wmemset(wszCmdLine, 0, MAX_PATH);
        wsprintf(wszCmdLine, L" /C format %c: /Q /FS:FAT32 /Y", m_wszDrive[0]);
        ExecuteShell(wszCmdLine);
    }
    SetVolumeLabelW(m_wszDrive, REMIXOS_SHARE_LABEL);

    return bResult;
}
