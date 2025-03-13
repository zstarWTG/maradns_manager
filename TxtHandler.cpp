//
// Author: zzstar
// Date:   25-3-10 上午12:11
// Brief:
// Attention:
//

#include "TxtHandler.h"
#include <QFile>
#include <QDebug>
#include <QRegExp>
TxtHandler::TxtHandler(const QString &path): path_(path)
{
}

void TxtHandler::setPath(const QString& path)
{
    this->path_ = path;
}

QMap<QString, QString> TxtHandler::getDns()
{
    QFile file(this->path_);
    QMap<QString, QString> ipMap;

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning() << "Failed to open file:" << this->path_;
        return ipMap;
    }

    QTextStream in(&file);
    while (!in.atEnd()) {
        QString line = in.readLine();
        if (line.startsWith("ip4[")) {
            QString key, value;
            if (extractKeyAndValue(line, key, value)) {
                ipMap.insert(key, value);
            }
        }
    }

    file.close();
    return ipMap;
}

void TxtHandler::setDns(const QMap<QString, QString>& dns)
{
    QFile file(path_);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning() << "Failed to open file for reading:" << path_;
        return;
    }

    QTextStream in(&file);
    QList<QString> lines;
    bool dnsSectionStarted = false;

    // Read existing lines and filter out lines starting with "ip4["
    while (!in.atEnd()) {
        QString line = in.readLine();
        if (line.startsWith("ip4 = {}")) {
            dnsSectionStarted = true;
            lines.append(line);
        } else if (!line.startsWith("ip4[")){
            if (dnsSectionStarted) {
                // Insert new DNS entries at the first non-dns line after dns section
                for (auto it = dns.begin(); it != dns.end(); ++it) {
                    if (this->dnsCheck(it.key(), it.value()))
                    {
                        lines.append(QString("ip4[\"%1.\"] = \"%2\"").arg(it.key(), it.value()));
                    }
                }
                dnsSectionStarted = false; // Ensure we only insert once
            }
            lines.append(line);
        }

    }

    file.close();

    // If the file ends with dns entries, append new DNS entries at the end
    if (dnsSectionStarted) {
        for (auto it = dns.begin(); it != dns.end(); ++it) {
            if (this->dnsCheck(it.key(), it.value()))
            {
                lines.append(QString("ip4[\"%1.\"] = \"%2\"").arg(it.key(), it.value()));
            }

        }
    }

    // Write all lines back to the file
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qWarning() << "Failed to open file for writing:" << path_;
        return;
    }

    QTextStream out(&file);
    for (const QString& line : lines) {
        out << line << "\n";
    }

    file.close();
}

bool TxtHandler::dnsCheck(const QString& domain, const QString& ip)
{
    if (domain.isEmpty() || ip.isEmpty())
    {
        return false;
    }

    QRegExp regdomain("^[a-z][a-z0-9]*(\\.[a-z0-9]+)*\\.[a-z]{2,}$");
    QRegExp regip("^(\\d{1,3}\\.){3,3}\\d{1,3}$");
    if (!regdomain.exactMatch(domain) || !regip.exactMatch(ip))
    {
        return false;
    }

    return true;
}

bool TxtHandler::extractKeyAndValue(const QString& line, QString& key, QString& value) {
    int startKey = line.indexOf('"') + 1;
    if (startKey == -1) return false;

    int endKey = line.indexOf('"', startKey);
    if (endKey == -1) return false;

    key = line.mid(startKey, endKey - startKey-1);

    int startValue = line.indexOf('"', endKey + 1) + 1;
    if (startValue == -1) return false;

    int endValue = line.indexOf('"', startValue);
    if (endValue == -1) return false;

    value = line.mid(startValue, endValue - startValue);

    return true;
}
