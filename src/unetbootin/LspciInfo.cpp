#include "LspciInfo.h"

#include "unetbootin.h"

// Audio device code
const QString LspciInfo::AUDIO_DEVICE_CODE = "0403";
// Ethernet device code
const QString LspciInfo::ETHERNET_DEVICE_CODE = "0200";
// Wireless lan device code
const QString LspciInfo::WIRELESS_DEVICE_CODE = "0280";
const QString LspciInfo::AUDIO_DEVICE_TAG = "Audio device [0403]";
const QString LspciInfo::ETHERNET_DEVICE_TAG = "Ethernet controller [0200]";
const QString LspciInfo::WIRELESS_DEVICE_TAG = "Network controller [0280]";
const QString LspciInfo::NO_DEVICE = "no_device";

QList<CodePair> LspciInfo::deviceCodeList;
QMap<QString, QString> LspciInfo::deviceCodeMap;
// Store which devices are detected
QList<QString> LspciInfo::deviceList;
const QString LspciInfo::LSPCI_DEVICE = "Class";
const QString LspciInfo::LSPCI_VENDOR = "Vendor";
const QString LspciInfo::LSPCI_PRODUCT = "Device";
QSet<QString> LspciInfo::mKeysSet;

// flag whether the static variables are initialized.
bool LspciInfo::isInitialized = false;

// Must be executed before parse LspciInfo.
void LspciInfo::init() {
    if (!isInitialized) {
        // For the fixed order to send information to GA'self-defined vars.
        deviceCodeList.push_back(qMakePair(AUDIO_DEVICE_CODE, AUDIO_DEVICE_TAG));
        deviceCodeList.push_back(qMakePair(ETHERNET_DEVICE_CODE, ETHERNET_DEVICE_TAG));
        deviceCodeList.push_back(qMakePair(WIRELESS_DEVICE_CODE, WIRELESS_DEVICE_TAG));
        foreach (CodePair pair, deviceCodeList) {
            deviceCodeMap.insert(pair.first, pair.second);
        }

        mKeysSet.insert(LSPCI_DEVICE);
        mKeysSet.insert(LSPCI_VENDOR);
        mKeysSet.insert(LSPCI_PRODUCT);
        isInitialized = true;
    }
    deviceList.clear();
}

void LspciInfo::getPciInfo(QList<LspciInfo>& infos, QString lspciOut) {
    init();
    QStringList lines = lspciOut.split( "\n");
    QList<QString> deviceInfoStrs;
    foreach (QString line, lines) {
        if (line.length() == 0) {
            LspciInfo info;
            int err = parseLspciInfo(deviceInfoStrs, info);
            if (err == 0) {
                infos.push_back(info);
            }
            deviceInfoStrs.clear();
        } else {
            deviceInfoStrs.push_back(line);
        }
    }

    ReportNoDevice(infos);
}

void LspciInfo::ReportNoDevice(QList<LspciInfo>& infos) {
    QMap<QString, QString>::const_iterator it = deviceCodeMap.begin();
    while (it != deviceCodeMap.end()) {
        if (!deviceList.contains(it.key())) {
            LspciInfo info(it.value(), "", "");
            infos.push_back(info);
        }
        ++it;
    }
}

QString LspciInfo::parseDeviceInfo(const QString& deviceStr) {
    QRegExp reg("\\[([0-9a-fA-F]+)\\]");
    QString deviceTag = "";
    if (reg.indexIn(deviceStr, 0) != -1) {
        QString deviceCode = reg.cap(1);
        if (deviceCodeMap.contains(deviceCode)) {
            deviceList.push_back(deviceCode);
            deviceTag = deviceCodeMap[deviceCode];
        }
    }
    return deviceTag;
}

// @return: 0 for sucess
//          1 for failed
int LspciInfo::parseLspciInfo(QList<QString>& infoStrs, LspciInfo& info) {
    foreach (QString str, infoStrs) {
        QStringList splitStrs = str.split(":\t");
        if (splitStrs.size() != 2) {
            continue;
        }
        QString key = splitStrs.at(0);
        if (mKeysSet.contains(key)) {
            if (key.compare(LSPCI_DEVICE) == 0) {
                info.mDevice = parseDeviceInfo(splitStrs.at(1));
            } else if (key.compare(LSPCI_VENDOR) == 0) {
                info.mVendor = splitStrs.at(1);
            } else if (key.compare(LSPCI_PRODUCT) == 0) {
                info.mProduct = splitStrs.at(1);
            }
        }
    }
    if (info.mDevice.length() > 0) {
        return 0;
    }
    return 1;
}
// endregion
