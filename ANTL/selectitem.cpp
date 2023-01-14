#include "selectitem.h"
#include "ui_selectitem.h"
#include<QCompleter>
#include<QStringList>
#include<QSqlDatabase>
#include<QSqlQuery>
#include<DB/conn.h>
selectItem::selectItem(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::selectItem)
{
    ui->setupUi(this);
    iniUi();
}

selectItem::~selectItem()
{
    delete ui;
}
//初始化页面
void selectItem::iniUi()
{
//    ui->intent->view()->parentWidget()->setAttribute(Qt::WA_TranslucentBackground);
    ui->intent->setView(new QListView(this));
    ui->slot->setView(new QListView(this));
    ui->intent->setItemDelegate(new QStyledItemDelegate());
    ui->slot->setItemDelegate(new QStyledItemDelegate());
    QFile qssFile(":select/qss/select.qss");
    qssFile.open(QFile::ReadOnly);
    if(qssFile.isOpen())
    {
        QString style=QString(qssFile.readAll());
        setStyleSheet(style);
        qssFile.close();
    }

}
//每当在输入框输入数据时，查询数据库，智能搜索
void res_dict(QStringList& res,const QString& slot_str){
    QSqlDatabase db=Conn::openConnection();
    QSqlQuery query(db);
    query.exec("select slot_value from dict where slot='"+slot_str+"'");
    while(query.next()){
        QString slot_value=query.value("slot_value").toString();
        res.append(slot_value);
    }
    db.close();
}

void selectItem::on_slot_value_textEdited(const QString &arg1)
{
    QStringList res;
    QString slot_str=ui->slot->currentText();
    res_dict(res,slot_str);
    QCompleter *completer = new QCompleter(res, this);
    completer->setCaseSensitivity(Qt::CaseInsensitive);
    completer->setFilterMode(Qt::MatchContains);
    ui->slot_value->setCompleter(completer);
}
