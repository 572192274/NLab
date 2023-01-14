#ifndef SELECTITEM_H
#define SELECTITEM_H

#include <QWidget>
#include<QFile>
#include<QListView>
#include<QStyledItemDelegate>
namespace Ui {
class selectItem;
}

class selectItem : public QWidget
{
    Q_OBJECT

public:
    explicit selectItem(QWidget *parent = 0);
    ~selectItem();
    void iniUi();
public:
    Ui::selectItem *ui;
private slots:
    void on_slot_value_textEdited(const QString &arg1);
};

#endif // SELECTITEM_H
