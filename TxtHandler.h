//
// Author: zzstar
// Date:   25-3-10 上午12:11
// Brief:
// Attention:
//

#ifndef TXTHANDLER_H
#define TXTHANDLER_H

#include <QString>
#include <QMap>

class TxtHandler {
public:
    explicit TxtHandler(const QString &path="");

    void setPath(const QString &path);

    QMap<QString, QString> getDns();
    void setDns(const QMap<QString, QString> &dns);
    bool dnsCheck(const QString &domain, const QString &ip);
private:

    bool extractKeyAndValue(const QString& line, QString& key, QString& value);

    QString path_;
};



#endif //TXTHANDLER_H
