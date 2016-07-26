/*

  Copyright (c) 2014, Michael Tesch
  All rights reserved.

  Redistribution and use in source and binary forms, with or without
  modification, are permitted provided that the following conditions are met:

  * Redistributions of source code must retain the above copyright notice, this
    list of conditions and the following disclaimer.

  * Redistributions in binary form must reproduce the above copyright notice,
    this list of conditions and the following disclaimer in the documentation
    and/or other materials provided with the distribution.

  * Neither the name of Michael Tesch nor the names of other
    contributors may be used to endorse or promote products derived from
    this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

  https://github.com/tesch1/qt-google-analytics-collector

  to enable debugging messages, '#define GANALYTICS_DEBUG 1' before including this file
  to get super verbose debugging, '#define GANALYTICS_DEBUG 2'
*/
#include <unistd.h>
#include <map>
#include <QCoreApplication>
#include <QSettings>
#ifdef QT_GUI_LIB
#include <QApplication>
#include <QDesktopWidget>
#endif
#include <QUuid>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QUrl>
#include <QDebug>
#include <QProcess>
#include <QLocale>
#include <QtGui>
#ifdef Q_OS_WIN32
#include <windows.h>
#include <shellapi.h>
#include "cpu-features-link-time.h"
#include "LspciInfo.h"
#endif

/*!
 * send google tracking data according to
 * https://developers.google.com/analytics/devguides/collection/protocol/v1/reference
 */

#ifndef GANALYTICS_DEBUG
#define GANALYTICS_DEBUG 0
#endif

class GAnalytics : public QObject {
  Q_OBJECT
private:
public:
  GAnalytics(QCoreApplication * parent,
             QString trackingID,
             QString clientID = "",
             bool useGET = false,
             bool getExtraSystemInfo=true)
    : QObject(parent), _qnam(this), _trackingID(trackingID), _clientID(clientID), _useGET(useGET), _isFail(false)
  {
    if (parent) {
      setAppName(parent->applicationName());
      setAppVersion(parent->applicationVersion());
#ifdef QT_GUI_LIB
      parent->dumpObjectTree();
#endif
    }
    if (!_clientID.size()) {
      _clientID = getSavedClientID();
    }
    connect(&_qnam, SIGNAL(finished(QNetworkReply *)), this, SLOT(postFinished(QNetworkReply *)));
#if QT_VERSION >= 0x040800
#if GANALYTICS_DEBUG
    if (!_qnam.networkAccessible())
      qDebug() << "error: network inaccessible\n";
#endif
#endif
#ifdef Q_OS_WIN32
  powershell = "powershell.exe";
#endif
    generate_system_info_map(getExtraSystemInfo);
  }

  ~GAnalytics() {
    // this generally happens after the event-loop is done, so no more network processing
#if GANALYTICS_DEBUG
    QList<QNetworkReply *> replies = _qnam.findChildren<QNetworkReply *>();
    for (QList<QNetworkReply *>::iterator it = replies.begin(); it != replies.end(); it++) {
      if ((*it)->isRunning())
        qDebug() << "~GAnalytics, request still running: " << (*it)->url().toString() << ", aborting.";
      //reply->deleteLater();
    }
#endif
  }

  // manual config of static fields
  void setClientID(QString clientID) { _clientID = clientID; }
  void setUserID(QString userID) { _userID = userID; }
  void setUserIPAddr(QString userIPAddr) { _userIPAddr = userIPAddr; }
  void setUserAgent(QString userAgent) { _userAgent = userAgent; }
  void setAppName(QString appName) { _appName = appName; }
  void setAppVersion(QString appVersion) { _appVersion = appVersion; }
  void setScreenResolution(QString screenResolution) { _screenResolution = screenResolution; }
  void setViewportSize(QString viewportSize) { _viewportSize = viewportSize; }
  void setUserLanguage(QString userLanguage) { _userLanguage = userLanguage; }
  QString getClientID() const { return _clientID; }
  QString getUserAgent() const { return _userAgent; }

  //
  // hit types
  //
  // - https://developers.google.com/analytics/devguides/collection/protocol/v1/devguide
  //

public Q_SLOTS:

  // pageview
  void sendPageview(QString docHostname, QString page, QString title) const {
    QUrl params = build_metric("pageview");
    params.addQueryItem("dh", docHostname);    // document hostname
    params.addQueryItem("dp", page);            // page
    params.addQueryItem("dt", title);           // title
    send_metric(params);
  }

  // event
  void sendEvent(QString eventCategory = "", QString eventAction = "",
                 QString eventLabel = "", int eventValue = 0, bool withParams = false) const {
    QUrl params = build_metric("event", withParams);
    if (_appName.size()) params.addQueryItem("an", _appName); // mobile event app tracking
    if (_appVersion.size()) params.addQueryItem("av", _appVersion); // mobile event app tracking
    if (eventCategory.size()) params.addQueryItem("ec", eventCategory);
    if (eventAction.size()) params.addQueryItem("ea", eventAction);
    if (eventLabel.size()) params.addQueryItem("el", eventLabel);
    if (eventValue) params.addQueryItem("ev", QString::number(eventValue));
    send_metric(params);
    DBG_INFO(QString("%1,%2,%3").arg(eventCategory).arg(eventAction).arg(eventLabel));
  }

  // transaction
  void sendTransaction(QString transactionID, QString transactionAffiliation = "" /*...todo...*/) const {
    QUrl params = build_metric("transaction");
    params.addQueryItem("ti", transactionID);
    if (transactionAffiliation.size()) params.addQueryItem("ta", transactionAffiliation);
    send_metric(params);
  }

  // item
  void sendItem(QString itemName) const {
    QUrl params = build_metric("item");
    params.addQueryItem("in", itemName);
    //if (appName.size()) params.addQueryItem("an", appName);
    send_metric(params);
  }

  // social
  void sendSocial(QString socialNetwork, QString socialAction, QString socialActionTarget) const {
    QUrl params = build_metric("social");
    params.addQueryItem("sn", socialNetwork);
    params.addQueryItem("sa", socialAction);
    params.addQueryItem("st", socialActionTarget);
    send_metric(params);
  }

  // exception
  void sendException(QString exceptionDescription, bool exceptionFatal = true) const {
    QUrl params = build_metric("exception");
    if (exceptionDescription.size()) params.addQueryItem("exd", exceptionDescription);
    if (!exceptionFatal) params.addQueryItem("exf", "0");
    send_metric(params);
  }

  // timing
  void sendTiming(/* todo */) const {
    QUrl params = build_metric("timing");
    //if (appName.size()) params.addQueryItem("an", appName);
    send_metric(params);
  }

  // appview
  void sendAppview(QString appName, QString appVersion = "", QString screenName = "") const {
    QUrl params = build_metric("appview");
    if (_appName.size()) params.addQueryItem("an", _appName);
    else if (appName.size()) params.addQueryItem("an", appName);
    if (_appVersion.size()) params.addQueryItem("av", _appVersion);
    else if (appVersion.size()) params.addQueryItem("av", appVersion);
    if (screenName.size()) params.addQueryItem("cd", screenName);
    send_metric(params);
  }

  // custom dimensions / metrics
  // todo

  // experiment id / variant
  // todo

  //void startSession();
  void endSession() const {
    QUrl params = build_metric("event");
    params.addQueryItem("sc", "end");
    send_metric(params);
  }

public:

  void generateUserAgentEtc() {
    _appVersion = QCoreApplication::instance()->applicationVersion();
    _appName = QCoreApplication::instance()->applicationName();
    QString locale = QLocale::system().name().toLower().replace("_", "-");
    QString machine = getSystemInfo();
    _userAgent = QString("%1/%2 (%3; %4) GAnalytics/1.0 (Qt/%5)").arg(_appName).arg(_appVersion).arg(machine).arg(locale).arg(QT_VERSION_STR);
    _userLanguage = locale;
#ifdef QT_GUI_LIB
    QString geom = QString::number(QApplication::desktop()->screenGeometry().width())
      + "x" + QString::number(QApplication::desktop()->screenGeometry().height());
    _screenResolution = geom;
#endif
#if GANALYTICS_DEBUG > 1
    qDebug() << "User-Agent:" << _userAgent;
    qDebug() << "Language:" << _userLanguage;
    qDebug() << "Screen Resolution:" << _screenResolution;
#endif
  }

private Q_SLOTS:
  void postFinished(QNetworkReply * reply) {
    //qDebug() << "reply:" << reply->readAll().toHex();
    if (QNetworkReply::NoError != reply->error()) {
      qDebug() << "postFinished error: " << reply->errorString() << "\n";
    }
    else {
      int httpStatus = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
      //qDebug() << "response code: " << httpStatus;
      if (httpStatus < 200 || httpStatus > 299) {
#if GANALYTICS_DEBUG
        qDebug() << "response code: " << httpStatus;
#endif
        _isFail = true;
      }
    }
    reply->deleteLater();
  }
  void postError(QNetworkReply::NetworkError code) {
    qDebug() << "network error signal: " << code << "\n";
  }

private:
  GAnalytics(const GAnalytics &); // disable copy const constructor
  QString getSavedClientID() {
      QSettings settings("remixos-installation-tool", "ganalytics");
      QString clientID;
      if (!settings.contains("GAnalytics-cid"))
      {
          clientID = QUuid::createUuid().toString();
          settings.setValue("GAnalytics-cid", clientID);
      }
      else
      {
          clientID = settings.value("GAnalytics-cid").toString();
          if (clientID.size() == 0) {
              clientID = QUuid::createUuid().toString();
              settings.setValue("GAnalytics-cid", clientID);
          }
      }

      return clientID;
  }

#ifdef Q_OS_WIN32
  QString powershell;
  void DBG_INFO(QString str) const {
      QString logPath = QString("%1\\remixos_install.log").arg(QDir::rootPath());
      QFile logfile(logPath);
      logfile.open(QIODevice::Append);
      logfile.write(str.toAscii());
      logfile.write("\r\n");
      logfile.close();
  }

  DWORD ExecuteCmd(QString command, QString parameters)
  {
      //DBG_INFO(QString("cmd:%1, paras:%2").arg(command).arg(parameters));
      DWORD returnCode;
      SHELLEXECUTEINFO shExecInfo = {0};
      shExecInfo.cbSize = sizeof(SHELLEXECUTEINFO);
      shExecInfo.fMask = SEE_MASK_NOCLOSEPROCESS;
      shExecInfo.hwnd = NULL;
      if (QSysInfo::WindowsVersion == QSysInfo::WV_NT || QSysInfo::WindowsVersion == QSysInfo::WV_2000 || QSysInfo::WindowsVersion == QSysInfo::WV_XP || QSysInfo::WindowsVersion == QSysInfo::WV_2003 )
      {
          shExecInfo.lpVerb = NULL;
      }
      else
      {
          shExecInfo.lpVerb = L"runas";
      }
      shExecInfo.lpFile = LPWSTR(command.utf16());
      shExecInfo.lpParameters = LPWSTR(parameters.utf16());
      shExecInfo.lpDirectory = NULL;
      shExecInfo.nShow = SW_HIDE;
      shExecInfo.hInstApp = NULL;
      BOOL ret = ShellExecuteEx(&shExecInfo);
      if (ret) {
          WaitForSingleObject(shExecInfo.hProcess, INFINITE);
          //DBG_INFO("ExecuteShell, try to get return value\n");

          if (GetExitCodeProcess(shExecInfo.hProcess, &returnCode)) {
              //DBG_INFO(QString("Result: %1").arg(returnCode));
              return returnCode;
          }
      }

      returnCode = GetLastError();
      //DBG_INFO(QString("Error: %1").arg(returnCode));
      return (returnCode != 0) ? returnCode : -1;
  }

  bool FindPowerShell()
  {
      bool result = false;
      powershell = "powershell.exe";
      QString cmd = "where";

      DWORD returnCode = ExecuteCmd(cmd, powershell);
      if (0 == returnCode)
      {
          DBG_INFO(powershell);
          result = true;
      }
      else
      {
          powershell = "%SYSTEMROOT%\\System32\\WindowsPowerShell\\v1.0\\powershell.exe";
          returnCode = ExecuteCmd(cmd, powershell);
          if (0 == returnCode)
          {
              DBG_INFO(powershell);
              result = true;
          }
      }

      return result;
  }

  QString ExecutePowerShell(QString command) {
      QString tempfile_name;
      {
          QTemporaryFile tempfile;
          if (!tempfile.open()) {
              return "";
          }
          tempfile_name = QDir::toNativeSeparators(tempfile.fileName());
      }
      QString parameters = QString("-Command \"& { chcp 437; %1 | Out-File %2 }\"").arg(command).arg(tempfile_name);

      if (0 !=  ExecuteCmd(powershell, parameters))
      {
          return "";
      }

      QFile file(tempfile_name);
      file.open(QIODevice::ReadOnly);
      QTextStream in(&file);

      QString result = in.readAll();
      //DBG_INFO(result);
      return result;
  }


  mutable QMap<QString, QString>* system_info_map;
  QMap<QString, QString>* get_system_info_map() const {
      return system_info_map;
  }

  QString getRandDirName() {
      QString rfpath = QDir::toNativeSeparators(QString("%1/").arg(QDir::tempPath()));
      qsrand((unsigned int)time(0));
      QString baseDir = QString("%1un%2_%3").arg(rfpath).arg(qrand() % 999999).arg("dir");
      while (QFile::exists(baseDir)) {
          baseDir = QString("%1%2.%3").arg(rfpath).arg(qrand() % 999999).arg("dir");
      }
      QDir(rfpath).mkpath(baseDir);
      return baseDir;
  }

  QString copyResToTemp(QList<QString> srcfNames) {
      QString dirName = getRandDirName();
      for (int i = 0; i < srcfNames.size(); ++i) {
          copyResFileTo(srcfNames[i], dirName + "\\" + srcfNames[i]);
      }
      return dirName;
  }

  void copyResFileTo(QString srcfName, QString dstfName) {
      QFile srcF(QString(":/%1").arg(srcfName));
      if (!srcF.exists()) {
          return;
      }
      if (QFile::exists(dstfName)) {
          return;
      }
      QFile dstF(dstfName);
      dstF.open(QIODevice::WriteOnly);
      srcF.open(QIODevice::ReadOnly);
      dstF.write(srcF.readAll());
      dstF.close();
      srcF.close();
  }

// rm dir recursively
  bool removeDir(const QString& dirName) {
      bool result = true;
      QDir dir(dirName);
      if (dir.exists()) {
          Q_FOREACH(QFileInfo info, dir.entryInfoList(QDir::NoDotAndDotDot | QDir::System | QDir::Hidden  | QDir::AllDirs | QDir::Files, QDir::DirsFirst)) {
              if (info.isDir()) {
                  result = removeDir(info.absoluteFilePath());
              }
              else {
                  result = QFile::remove(info.absoluteFilePath());
              }

              if (!result) {
                  return result;
              }
          }
          result = QDir().rmdir(dirName);
      }
      return result;
  }

  QString executeLspciCmd() {
      QStringList files;
      files << "lspci.exe" << "zlib1.dll" << "pci.ids.gz" << "DirectIOLib32.dll";
      QString tmpDir = copyResToTemp(files);
      QProcess process;
      process.start(tmpDir + "//lspci.exe", QStringList() << "-vmmnn",
                    QIODevice::ReadWrite | QIODevice::Text);
      QString lspciOut;
      if(!process.waitForFinished()) {
          // beware the timeout default parameter
          DBG_INFO("execute lspci.exe failed with error: " + process.errorString());
      } else {
          lspciOut = process.readAllStandardOutput();
      }
      removeDir(tmpDir);
      return lspciOut;
  }

  void generate_system_info_map(bool getExtraSystemInfo = true) {
      QMap<QString, QString>& map = * new QMap<QString, QString>();
      system_info_map = &map;
      if (!getExtraSystemInfo) {
          return;
      }

      // yuanfang: WIN7 is treated as WV_VISTA, may add a equaltion here.
      if(QSysInfo::WindowsVersion >= QSysInfo::WV_VISTA && FindPowerShell())
      {
          DBG_INFO("this is WV_VISTA");
          // OS
          {
              QString command = "Get-WmiObject Win32_OperatingSystem | Select -ExpandProperty Caption";
              QString result = ExecutePowerShell(command);
              if (!result.isEmpty()) {
                  map["OS"] = result;
              }
          }
          // CPU
          {
              QString command = "Get-WmiObject Win32_Processor | Select -ExpandProperty Name";
              QString result = ExecutePowerShell(command);
              if (!result.isEmpty()) {
                  map["CPU"] = result;
              }
          }
          // GPU
          {
              QString command = "Get-WmiObject Win32_Videocontroller | Select -ExpandProperty Name";
              QString result = ExecutePowerShell(command);
              if (!result.isEmpty()) {
                  map["GPU"] = result;
              }
          }
          QString lspciOut = executeLspciCmd();
          QList<LspciInfo> infos;
          LspciInfo::getPciInfo(infos, lspciOut);
          QSet<QString> deviceSet;
          foreach(LspciInfo info, infos) {
              QString deviceTag = info.device();
              if (map.contains(deviceTag)) {
                  map[deviceTag] = map[deviceTag] + " || " + info.vendorAndProduct();
              } else {
                  map[deviceTag] = info.vendorAndProduct();
              }
          }
      }
  }

  QString cpu_flags() const {
    uint32_t flags = android_getCpuFeatures_link_time();
    QStringList flaglist;
    if ((flags & ANDROID_CPU_X86_FEATURE_SSSE3) != 0) {
        flaglist << "ssse3";
    }
    if ((flags & ANDROID_CPU_X86_FEATURE_POPCNT) != 0) {
        flaglist << "popcnt";
    }
    if ((flags & ANDROID_CPU_X86_FEATURE_MOVBE) != 0) {
        flaglist << "movbe";
    }
    if ((flags & ANDROID_CPU_X86_FEATURE_SSE4_1) != 0) {
        flaglist << "sse4_1";
    }
    if ((flags & ANDROID_CPU_X86_FEATURE_SSE4_2) != 0) {
        flaglist << "sse4_2";
    }
    if ((flags & ANDROID_CPU_X86_FEATURE_PCLMULQDQ) != 0) {
        flaglist << "pclmulqdq";
    }
    if ((flags & ANDROID_CPU_X86_FEATURE_AES) != 0) {
        flaglist << "aes";
    }
    if ((flags & ANDROID_CPU_X86_FEATURE_AVX) != 0) {
        flaglist << "avx";
    }
    if ((flags & ANDROID_CPU_X86_FEATURE_AVX2) != 0) {
        flaglist << "avx2";
    }
    return flaglist.join(" ");
  }
#endif

QUrl build_metric(QString hitType, bool withParams = false) const {
    QUrl params;
    // required in v1
    params.addQueryItem("v", "1" ); // version
    params.addQueryItem("tid", _trackingID);
    params.addQueryItem("cid", _clientID);
    params.addQueryItem("t", hitType);
    // optional
    if (_userID.size())
      params.addQueryItem("uid", _userID);
    if (_userIPAddr.size())
      params.addQueryItem("uip", _userIPAddr);
    if (_screenResolution.size())
      params.addQueryItem("sr", _screenResolution);
    if (_viewportSize.size())
      params.addQueryItem("vp", _viewportSize);
    if (_userLanguage.size())
      params.addQueryItem("ul", _userLanguage);
    //if (_userAgent.size())
    //  params.addQueryItem("ua", _userAgent);

    //@jide cd1 is customized demention 1: cpu flags
#ifdef Q_OS_WIN32
    if (withParams) {
        params.addQueryItem("cd1", cpu_flags());
        QMap<QString, QString>& map = *get_system_info_map();
        if (map.contains("CPU")){
            params.addQueryItem("cd2", map["CPU"]);
        }
        if (map.contains("GPU")){
            params.addQueryItem("cd3", map["GPU"]);
        }
        if (map.contains("OS")){
            params.addQueryItem("cd4", map["OS"]);
        }
        // Audio device
        if (map.contains(LspciInfo::AUDIO_DEVICE_TAG)){
            DBG_INFO("cd5 :" + map[LspciInfo::AUDIO_DEVICE_TAG]);
            params.addQueryItem("cd5", map[LspciInfo::AUDIO_DEVICE_TAG]);
        }
        // Ethernet device
        if (map.contains(LspciInfo::ETHERNET_DEVICE_TAG)){
            DBG_INFO("cd6 :" + map[LspciInfo::ETHERNET_DEVICE_TAG]);
            params.addQueryItem("cd6", map[LspciInfo::ETHERNET_DEVICE_TAG]);
        }
        // Wireless device
        if (map.contains(LspciInfo::WIRELESS_DEVICE_TAG)){
            DBG_INFO("cd7 :" + map[LspciInfo::WIRELESS_DEVICE_TAG]);
            params.addQueryItem("cd7", map[LspciInfo::WIRELESS_DEVICE_TAG]);
        }
    }
#endif
    return params;
  }


  void send_metric(QUrl & params) const {
    // when google has err'd us, then stop sending events!
    if (_isFail)
      return;
    QUrl collect_url("http://www.google-analytics.com/collect");
    QNetworkRequest request;
    if (_userAgent.size())
      request.setRawHeader("User-Agent", _userAgent.toUtf8());
    QNetworkReply * reply;
    if (_useGET) {
      // add cache-buster "z" to params
      //params.addQueryItem("z", QString::number(qrand()) );
      collect_url.setQueryItems(params.queryItems());
      request.setUrl(collect_url);
      reply = _qnam.get(request);
    }
    else {
      request.setUrl(collect_url);
      request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");
      reply = _qnam.post(request, params.encodedQuery());
    }
    connect(reply, SIGNAL(error(QNetworkReply::NetworkError)),
            this, SLOT(postError(QNetworkReply::NetworkError)));
#if GANALYTICS_DEBUG > 1
    qDebug() << "GAnalytics sent: " << params.toString();
#endif
    reply->setParent(&_qnam);
  }

#ifdef Q_OS_MAC
/**
 * Only on Mac OS X
 * Get the Operating system name and version.
 * @return os   The operating system name and version in a string.
 */
QString getSystemInfo()
{
    QSysInfo::MacVersion version = QSysInfo::macVersion();
    QString os;
    switch (version)
    {
    case QSysInfo::MV_9:
        os = "Macintosh; Mac OS 9";
        break;
    case QSysInfo::MV_10_0:
        os = "Macintosh; Mac OS 10.0";
        break;
    case QSysInfo::MV_10_1:
        os = "Macintosh; Mac OS 10.1";
        break;
    case QSysInfo::MV_10_2:
        os = "Macintosh; Mac OS 10.2";
        break;
    case QSysInfo::MV_10_3:
        os = "Macintosh; Mac OS 10.3";
        break;
    case QSysInfo::MV_10_4:
        os = "Macintosh; Mac OS 10.4";
        break;
    case QSysInfo::MV_10_5:
        os = "Macintosh; Mac OS 10.5";
        break;
    case QSysInfo::MV_10_6:
        os = "Macintosh; Mac OS 10.6";
        break;
    case QSysInfo::MV_10_7:
        os = "Macintosh; Mac OS 10.7";
        break;
    case QSysInfo::MV_10_8:
        os = "Macintosh; Mac OS 10.8";
        break;
    case QSysInfo::MV_10_9:
        os = "Macintosh; Mac OS 10.9";
        break;
    case QSysInfo::MV_10_10:
        os = "Macintosh; Mac OS 10.10";
        break;
    case QSysInfo::MV_10_11:
        os = "Macintosh; Mac OS 10.11";
        break;
    case QSysInfo::MV_Unknown:
        os = "Macintosh; Mac OS unknown";
        break;
    case QSysInfo::MV_IOS_5_0:
        os = "iPhone; iOS 5.0";
        break;
    case QSysInfo::MV_IOS_5_1:
        os = "iPhone; iOS 5.1";
        break;
    case QSysInfo::MV_IOS_6_0:
        os = "iPhone; iOS 6.0";
        break;
    case QSysInfo::MV_IOS_6_1:
        os = "iPhone; iOS 6.1";
        break;
    case QSysInfo::MV_IOS_7_0:
        os = "iPhone; iOS 7.0";
        break;
    case QSysInfo::MV_IOS_7_1:
        os = "iPhone; iOS 7.1";
        break;
    case QSysInfo::MV_IOS_8_0:
        os = "iPhone; iOS 8.0";
        break;
    case QSysInfo::MV_IOS_8_1:
        os = "iPhone; iOS 8.1";
        break;
    case QSysInfo::MV_IOS_8_2:
        os = "iPhone; iOS 8.2";
        break;
    case QSysInfo::MV_IOS_8_3:
        os = "iPhone; iOS 8.3";
        break;
    case QSysInfo::MV_IOS_8_4:
        os = "iPhone; iOS 8.4";
        break;
    case QSysInfo::MV_IOS_9_0:
        os = "iPhone; iOS 9.0";
        break;
    case QSysInfo::MV_IOS:
        os = "iPhone; iOS unknown";
        break;
    default:
        os = "Macintosh";
        break;
    }
    return os;
}
#endif

#ifdef Q_OS_WIN
/**
 * Only on Windows
 * Get operating system and its version.
 * @return os   A QString containing the oprating systems name and version.
 */
QString getSystemInfo()
{
    QSysInfo::WinVersion version = QSysInfo::windowsVersion();
    QString os("Windows;");
    switch (version)
    {
    case QSysInfo::WV_95:
        os += "Win 95";
        break;
    case QSysInfo::WV_98:
        os += "Win 98";
        break;
    case QSysInfo::WV_Me:
        os += "Win ME";
        break;
    case QSysInfo::WV_NT:
        os += "Win NT";
        break;
    case QSysInfo::WV_2000:
        os += "Win 2000";
        break;
    case QSysInfo::WV_2003:
        os += "Win Server 2003";
        break;
    case QSysInfo::WV_VISTA:
        os += "Win Vista";
        break;
    case 0x0090: // QSysInfo::WV_WINDOWS7:
        os += "Win 7";
        break;
    case 0x00a0: // QSysInfo::WV_WINDOWS8:
        os += "Win 8";
        break;
    case 0x00b0: // QSysInfo::WV_WINDOWS8_1:
        os += "Win 8.1";
        break;
    case 0x00c0: // QSysInfo::WV_WINDOWS10:
        os += "Win 10";
        break;
    default:
        os = "Windows; unknown";
        break;
    }
    return os;
}
#endif

#if defined(Q_OS_ANDROID)
#include <QAndroidJniObject>

QString getSystemInfo()
{
    return QString("Linux; U; Android %1; %2 %3 Build/%4; %5")
            .arg(QAndroidJniObject::getStaticObjectField<jstring>("android/os/Build$VERSION", "RELEASE").toString())
            .arg(QAndroidJniObject::getStaticObjectField<jstring>("android/os/Build", "MANUFACTURER").toString())
            .arg(QAndroidJniObject::getStaticObjectField<jstring>("android/os/Build", "MODEL").toString())
            .arg(QAndroidJniObject::getStaticObjectField<jstring>("android/os/Build", "ID").toString())
            .arg(QAndroidJniObject::getStaticObjectField<jstring>("android/os/Build", "BRAND").toString());
}
#elif defined(Q_OS_LINUX)
#include <sys/utsname.h>

/**
 * Only on Unix systems.
 * Get operation system name and version.
 * @return os       A QString with the name and version of the operating system.
 */
QString getSystemInfo()
{
    struct utsname buf;
    uname(&buf);
    QString system(buf.sysname);
    QString release(buf.release);

    return system + "; " + release;
}
#endif
  mutable QNetworkAccessManager _qnam;
  QString _trackingID;
  QString _clientID;
  bool _useGET; // true=GET, false=POST
  QString _userID;

  // various parameters:
  bool _anonymizeIP;
  bool _cacheBust;
  //
  QString _userIPAddr;
  QString _userAgent;
  QString _appName;
  QString _appVersion;
#if 0 // todo
  // traffic sources
  QString _documentReferrer;
  QString _campaignName;
  QString _campaignSource;
  QString _campaignMedium;
  QString _campaignKeyword;
  QString _campaignContent;
  QString _campaignID;
  QString _adwordsID;
  QString _displayAdsID;
#endif
  // system info
  QString _screenResolution;
  QString _viewportSize;
  QString _userLanguage;
  // etc...

  // internal
  bool _isFail;
};
