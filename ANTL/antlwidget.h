#ifndef ANTLWIDGET_H
#define ANTLWIDGET_H

#include <QWidget>
#include"selectitem.h"
#include<QDebug>
#include<QComboBox>
#include<QLineEdit>
#include<QSpinBox>
#include<QJsonArray>
#include<QJsonObject>
#include<QJsonDocument>
#include"DB/conn.h"
#include<QMenu>
#include<QPixmap>
#include<QStyledItemDelegate>
#include<QScrollBar>
namespace Ui {
class antlWidget;
}

class antlWidget : public QWidget
{
    Q_OBJECT

public:
    explicit antlWidget(QWidget *parent = 0);
    ~antlWidget();
    void iniUi();

private slots:
    void on_btn_add_clicked();

    void on_btn_save_clicked();

    void on_antl_list_customContextMenuRequested(const QPoint &pos);
    void menu_delete();

private:
    Ui::antlWidget *ui;
};

#endif // ANTLWIDGET_H
