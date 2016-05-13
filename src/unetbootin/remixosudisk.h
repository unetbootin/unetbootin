#ifndef REMIXOSUDISK_H
#define REMIXOSUDISK_H

#include <windows.h>
#include <winioctl.h>
#include <stdio.h>
#include <wchar.h>

#define DISK_SIGNATURE 3272027600
#define REMIXOS_SYS_LABEL L"REMIXOSSYS"
#define REMIXOS_SHARE_LABEL L"REMIX_OS"
#define REMIXOS_DATA_LABEL L"REMIXOS"
#define ARRAYSIZE(a) (sizeof(a)/sizeof(a[0]))

typedef struct {
  LONGLONG MBRTable;
  LONGLONG ShareVolume;
  LONGLONG SystemVolume;
  LONGLONG DataVolume;
} REMIXOS_VOLUME_SIZE;

class RemixOSUDisk
{
public:
    RemixOSUDisk(WCHAR* wszDrive);

    void Initialize();

    BOOL IsValidUDisk();
    BOOL IsRemixOSUDisk();
    BOOL PrepareUDisk();
    BOOL UpdateUDisk();

    void FormatDataPartition(BOOL bFormat);

private:
    RemixOSUDisk(){}
    BOOL CheckUDisk();
    BOOL ReadRemixOSInfo();
    void GetVolumeSize();
    BOOL GetPhysicalDiskName();
    BOOL DeleteExistDriveFromUDisk();
    BOOL CreateDiskInUDisk();
    BOOL CreateDrivesInUDisk(int type);

    BOOL m_bFormat;
    BOOL m_bValidUDisk;
    BOOL m_bRemixOSUDisk;
    WCHAR m_wszDrive[MAX_PATH + 1];
    WCHAR m_wszPhysicalName[MAX_PATH + 1];
    DISK_GEOMETRY_EX m_diskGeometryEx;
    REMIXOS_VOLUME_SIZE m_volumeSize;
};

#endif // REMIXOSUDISK_H
