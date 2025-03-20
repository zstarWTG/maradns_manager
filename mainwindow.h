//
// Author: zzstar
// Date:   25-3-8 下午10:47
// Brief:
// Attention:
//

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPushButton>
#include <QItemDelegate>
#include <QEvent>
#include <QCloseEvent>
class mainwindow : public QMainWindow{
    Q_OBJECT
public:
    explicit mainwindow(QWidget *parent = nullptr);

    void closeEvent(QCloseEvent* event) override;


    void sync();
    void restore();
    void backup();

    void readSettings();
    void writeSettings();
    void checkBoxStateChanged(int state);
    void rmClicked();

    ~mainwindow(){}

private:
    void tableInsertRow(const int &row, const QString &domain, const QString &ip);
    QString path_;
    bool is_deperate;
    int cnt;
};



class EditableDelegate : public QItemDelegate {
    Q_OBJECT

public:
    EditableDelegate(QObject* parent = nullptr) : QItemDelegate(parent) {}

    QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const override {
        if (index.column() == 0) { // Skip the checkbox column
            return nullptr;
        }
        return QItemDelegate::createEditor(parent, option, index);
    }

    void setEditorData(QWidget* editor, const QModelIndex& index) const override {
        if (index.column() == 0) {
            return;
        }
        QItemDelegate::setEditorData(editor, index);
    }

    void setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const override {
        if (index.column() == 0) {
            return;
        }
        QItemDelegate::setModelData(editor, model, index);
    }

    void updateEditorGeometry(QWidget* editor, const QStyleOptionViewItem& option, const QModelIndex& index) const override {
        // if (index.column() == 0) {
        //     return;
        // }
        QItemDelegate::updateEditorGeometry(editor, option, index);
    }

};

#endif //MAINWINDOW_H
