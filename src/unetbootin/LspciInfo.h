#ifndef LSPCI_INFO_H_
#define LSPCI_INFO_H_

#include <QDir>
#include <QFile>
#include <QList>
#include <QMap>
#include <QPair>
#include <QSet>
#include <QStringList>
#include <QThread>
#include <QUuid>

typedef QPair<QString, QString> CodePair;

class LspciInfo {
public:
    // Audio device code
    static const QString AUDIO_DEVICE_CODE;
    // Ethernet device code
    static const QString ETHERNET_DEVICE_CODE;
    // Wireless lan device code
    static const QString WIRELESS_DEVICE_CODE;
    static const QString AUDIO_DEVICE_TAG;
    static const QString ETHERNET_DEVICE_TAG;
    static const QString WIRELESS_DEVICE_TAG;
    static const QString NO_DEVICE;
    static const QString LSPCI_DEVICE;
    static const QString LSPCI_VENDOR;
    static const QString LSPCI_PRODUCT;
    // Only after invoke init(), this value is unempty.
    static QList<QPair<QString, QString> > deviceCodeList;
    static QMap<QString, QString> deviceCodeMap;

    QString device() {
        return mDevice;
    }

    QString vendorAndProduct() {
        return ((mVendor.isEmpty() && mProduct.isEmpty()) ?
                NO_DEVICE : (mVendor + " | " + mProduct));
    }

    static void getPciInfo(QList<LspciInfo>& infos, QString lspciOut);
private:
    // Store which devices are detected
    static QList<QString> deviceList;
    // Store keys we care about.
    static QSet<QString> mKeysSet;
    // flag whether the static variables are initialized.
    static bool isInitialized;

    QString mDevice;
    QString mVendor;
    QString mProduct;

    LspciInfo() {}

    LspciInfo(QString device, QString vendor, QString product) {
        mDevice = device;
        mVendor = vendor;
        mProduct = product;
    }

    static void init();

    static void ReportNoDevice(QList<LspciInfo>& infos);

    static QString parseDeviceInfo(const QString& deviceStr);
    // @return: 0 for sucess
    //          1 for failed

    static int parseLspciInfo(QList<QString>& infoStrs, LspciInfo& info);
};
#endif // LSPCI_INFO_H_
