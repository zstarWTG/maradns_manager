//
// Author: zzstar
// Date:   25-3-8 下午10:47
// Brief:
// Attention:
//

#include <QTableWidget>
#include <QTableWidgetItem>
#include <QCheckBox>
#include <QSettings>
#include <QFileDialog>
#include "mainwindow.h"
#include <QDebug>
#include <qcoreapplication.h>
#include <QLabel>
#include "QLayout"
#include "TxtHandler.h"
#include <QDate>
#include <QMessageBox>
#include <QRegExp>
mainwindow::mainwindow(QWidget* parent) : QMainWindow(parent), is_deperate(false), cnt(0)
{

    this->resize(510, 600);
    this->setWindowTitle(tr("dns manager"));
    auto cent_widget = new QWidget(this);

    auto layout_v = new QVBoxLayout(cent_widget);
    auto layout_h = new QHBoxLayout(dynamic_cast<QWidget*>(layout_v));

    auto label = new QLabel(tr("choose dns config file"), this);
    label->setObjectName("label");

    auto btn_choose = new QPushButton(tr("choose file"), this);
    auto btn_backup_ = new QPushButton(tr("backup"), this);
    auto btn_sync = new QPushButton(tr("sync"), this);
    auto btn_restore = new QPushButton(tr("restore"), this);
    auto btn_add = new QPushButton(this);
    btn_add->setIcon(QIcon(":/res/add.png"));
    auto table = new QTableWidget(this);
    table->setObjectName("table");
    table->setColumnCount(4);
    table->setHorizontalHeaderLabels({tr("enable"), tr("domain"), tr("ip"), tr("edit")});
    table->setFixedSize(QSize(570, 400));
    table->setItemDelegate(new EditableDelegate(this));
    table->setEditTriggers(QAbstractItemView::DoubleClicked | QAbstractItemView::AnyKeyPressed | QAbstractItemView::EditKeyPressed);
    table->setColumnWidth(0, 70);
    table->setColumnWidth(1, 200);
    table->setColumnWidth(2, 200);
    table->setColumnWidth(3, 70);
    table->insertRow(0);
    table->setCellWidget(0, 0, btn_add);
    connect(btn_add, &QPushButton::clicked, [this, table]()
    {
        this->tableInsertRow(table->rowCount()-1, "", "");
    });

    layout_h->addWidget(btn_backup_);
    layout_h->addWidget(btn_restore);
    layout_h->addWidget(btn_sync);
    layout_v->addWidget(btn_choose);
    layout_v->addWidget(label);
    layout_v->addWidget(table);
    layout_v->addLayout(layout_h);

    this->setCentralWidget(cent_widget);
    cent_widget->setLayout(layout_v);

    this->readSettings();
    qDebug() << "load path:" << this->path_;
    if (!this->path_.isEmpty())
    {
        this->sync();
    }

    connect(btn_choose, &QPushButton::clicked, [this]()
    {
        QString fileName = QFileDialog::getOpenFileName(this, tr("choose file"),
            this->path_.isEmpty() ? QDir::currentPath() : this->path_, tr("Text Files (*.txt)"));
        if (!fileName.isEmpty())
        {
                    this->path_ = fileName;
            this->findChild<QLabel*>("label")->setText(fileName);
            this->sync();
            qDebug() << "Selected file: " << fileName;
        }
    });

    connect(btn_sync, &QPushButton::clicked, this, &mainwindow::sync);
    connect(btn_restore, &QPushButton::clicked, this, &mainwindow::restore);
    connect(btn_backup_, &QPushButton::clicked, this, &mainwindow::backup);
    connect(table, &QTableWidget::itemChanged, [this, &table](QTableWidgetItem* item)
    {
        QRegExp domain("^[a-z][a-z0-9]*(\\.[a-z0-9]+)*\\.[a-z]{2,}$");
        QRegExp ip("^(\\d{1,3}\\.){3,3}\\d{1,3}$");
        if (item->text().isEmpty())
        {
            return;
        }

        if (this->is_deperate)
        {
            qDebug() << "changing color";
            this->is_deperate = false;
            return;
        }

        if (item->column() == 1)
        {
            if (!domain.exactMatch(item->text()))
            {
                QMessageBox::warning(this, tr("warning"), tr("domain format error"));
                if (item->background().color() != Qt::red)
                {
                    this->is_deperate = true;
                }
                item->setBackground(Qt::red);
            }else
            {
                item->setBackground(Qt::white);
            }
        }else if (item->column() == 2)
        {
            if (!ip.exactMatch(item->text()))
            {
                QMessageBox::warning(this, tr("warning"), tr("ip format error"));
                if (item->background().color() != Qt::red)
                {
                    this->is_deperate = true;
                }
            }else
            {
                item->setBackground(Qt::white);
            }
        }else
        {
            return;
        }
    });
}

void mainwindow::closeEvent(QCloseEvent* event)
{
    this->writeSettings();
    event->accept();
}

void mainwindow::sync()
{
    //path check
    if (this->path_.isEmpty())
    {
        QMessageBox::warning(this, tr("warning"), tr("please choose file"));
        return;
    }

    auto table = dynamic_cast<QTableWidget*>(this->findChild<QTableWidget*>("table"));
    table->setCellWidget(0,0, table->cellWidget(table->rowCount()-1,0));
    table->setRowCount(1);

    if (table->item(0, 1))
    {
        table->item(0, 1)->setText("");
        table->item(0, 2)->setText("");
        table->removeCellWidget(0,3);
    }
    TxtHandler handler(this->path_);
    auto dns = handler.getDns();

    for (int i = 0; i < dns.size(); ++i)
    {
        this->tableInsertRow(i, dns.keys()[i], dns.values()[i]);
    }
}

void mainwindow::restore()
{
    //path check
    if (this->path_.isEmpty())
    {
        QMessageBox::warning(this, tr("warning"), tr("please choose file"));
        return;
    }
    auto table = dynamic_cast<QTableWidget*>(this->findChild<QTableWidget*>("table"));
    TxtHandler handler(this->path_);
    QMap<QString, QString> dns;

    for (int i = 0; i < table->rowCount(); ++i) {
        QTableWidgetItem* keyItem = table->item(i, 1);
        QTableWidgetItem* valueItem = table->item(i, 2);
        if (keyItem && valueItem) {
            QString domain = keyItem->text();
            QString ip = valueItem->text();
            dns.insert(domain, ip);
        }
    }


    handler.setDns(dns);
}

void mainwindow::backup()
{
    //path check
    if (this->path_.isEmpty())
    {
        QMessageBox::warning(this, tr("warning"), tr("please choose file"));
        return;
    }
    QDateTime currentDateTime = QDateTime::currentDateTime();
    QString dateTimeString = currentDateTime.toString("yyMMdd-hh_mm");

    // 获取文件名和路径
    QFileInfo fileInfo(path_);
    QString baseName = fileInfo.baseName();
    QString suffix = fileInfo.suffix();
    QString dirPath = fileInfo.path();

    // 构造新的文件名
    QString backupFileName = QString("%1-backup-%2.%3").arg(baseName).arg(dateTimeString).arg(suffix);
    QString backupFilePath = QDir(dirPath).filePath(backupFileName);

    // 复制文件
    if (QFile::copy(path_, backupFilePath)) {
        qDebug() << "Backup successful:" << backupFilePath;
    } else {
        qDebug() << "Backup failed:" << path_;
    }
}

void mainwindow::readSettings()
{
    QSettings settings(QCoreApplication::applicationDirPath() + "/config.ini", QSettings::IniFormat);
    this->path_ = settings.value("path").toString();
    if (!this->path_.isEmpty())
    {
        this->findChild<QLabel*>("label")->setText(this->path_);
    }
}

void mainwindow::writeSettings()
{
    QSettings settings(QCoreApplication::applicationDirPath() + "/config.ini", QSettings::IniFormat);
    settings.setValue("path", this->path_);
}

void mainwindow::tableInsertRow(const int& row, const QString& domain, const QString& ip)
{
    auto table = this->findChild<QTableWidget*>("table");
    QTableWidgetItem* keyItem = new QTableWidgetItem(domain);
    QTableWidgetItem* valueItem = new QTableWidgetItem(ip);
    keyItem->setTextAlignment(Qt::AlignCenter);
    keyItem->setFlags(Qt::NoItemFlags);
    valueItem->setFlags(Qt::NoItemFlags);
    valueItem->setTextAlignment(Qt::AlignCenter);
    auto widget = new QWidget();
    auto layout = new QHBoxLayout(widget);
    QCheckBox* checkBox = new QCheckBox(widget);
    checkBox->setChecked(false); // 默认启用
    checkBox->setStyleSheet("QCheckBox::indicator { width: 30px; height: 30px;}");
    // 将复选框添加到表格中
    table->insertRow(row);
    layout->setSpacing(0);
    layout->setMargin(0);
    layout->addWidget(checkBox);
    table->setCellWidget(row, 0, widget);
    table->setItem(row, 1, keyItem);
    table->setItem(row, 2, valueItem);

    auto widgetrm = new QWidget();
    auto layoutrm = new QHBoxLayout(widgetrm);
    layoutrm->setSpacing(0);
    layoutrm->setMargin(0);
    auto btn_rm = new QPushButton(widgetrm);
    layoutrm->addWidget(btn_rm);
    btn_rm->setIcon(QIcon(":/res/del.png"));
    table->setCellWidget(row,3, widgetrm);
    connect(btn_rm, &QPushButton::clicked, this, &mainwindow::rmClicked);

    // 连接复选框的信号到槽函数
    connect(checkBox, &QCheckBox::stateChanged, this, &mainwindow::checkBoxStateChanged);
}

void mainwindow::rmClicked()
{
    QTableWidget* table = this->findChild<QTableWidget*>("table");
    auto senderBtn = qobject_cast<QPushButton*>(QObject::sender());
    for (int row = 0; row < table->rowCount(); ++row) {
        QWidget* cellWidget = table->cellWidget(row, 3);
        if (cellWidget && cellWidget->layout()) {
            auto currentbtn = qobject_cast<QPushButton*>(cellWidget->layout()->itemAt(0)->widget());
            if (currentbtn == senderBtn) {
                table->removeRow(row);
                return;
            }
        }
    }
}

void mainwindow::checkBoxStateChanged(int state)
{
    QCheckBox* checkBox = qobject_cast<QCheckBox*>(QObject::sender());
    if (!checkBox) {
        qDebug() << "Sender is not a QCheckBox";
        return;
    }

    QTableWidget* table = this->findChild<QTableWidget*>("table");
    if (!table) {
        qDebug() << "Table not found";
        return;
    }

    // 遍历表格找到checkBox所在的行
    for (int row = 0; row < table->rowCount(); ++row) {
        QWidget* cellWidget = table->cellWidget(row, 0);
        if (cellWidget && cellWidget->layout()) {
            QCheckBox* currentCheckBox = qobject_cast<QCheckBox*>(cellWidget->layout()->itemAt(0)->widget());
            if (currentCheckBox == checkBox) {
                bool isEditable = (state == Qt::Checked);
                table->item(row, 1)->setFlags(isEditable ? (Qt::ItemIsEditable | Qt::ItemIsEnabled | Qt::ItemIsSelectable)
                    : Qt::NoItemFlags);
                table->item(row, 2)->setFlags(isEditable ? (Qt::ItemIsEditable | Qt::ItemIsEnabled | Qt::ItemIsSelectable)
                    : Qt::NoItemFlags);
                return;
            }
        }
    }
}
