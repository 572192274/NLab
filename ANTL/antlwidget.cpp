#include "antlwidget.h"
#include "ui_antlwidget.h"
extern QMap<QString,antlWidget*> antlwidget_map;
extern QString global_dialog;
antlWidget::antlWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::antlWidget)
{
    ui->setupUi(this);
    iniUi();
}

antlWidget::~antlWidget()
{
    delete ui;
}

void antlWidget::iniUi()
{
    QFile qssFile(":antl/qss/antl.qss");
    qssFile.open(QFile::ReadOnly);
    if(qssFile.isOpen())
    {
        QString style=QLatin1String(qssFile.readAll());
        setStyleSheet(style);
        qssFile.close();
    }
}
//添加标注工具
void antlWidget::on_btn_add_clicked()
{
    QListWidgetItem *antlItem=new QListWidgetItem;
    selectItem *antlSelect=new selectItem;
    antlItem->setSizeHint(QSize(700,40));
    ui->antl_list->addItem(antlItem);
    ui->antl_list->setItemWidget(antlItem,antlSelect);
    //设置滚动条自动到底部
    ui->antl_list->verticalScrollBar()->setSliderPosition(ui->antl_list->verticalScrollBar()->maximum());
}
//将槽位是疾病，症状，药物，检查插入到数据库字典中
void insert_dict(const QString& slot,const QString& slot_value){
    //先查询这对是否在数据库字典中
    QSqlDatabase db=Conn::openConnection();
    QSqlQuery query(db);
    query.exec("select Id from dict where slot='"+slot+"' and slot_value='"+slot_value+"'");
    if(query.size()>0||query.size()==-1){
        db.close();
        return;
    }
    else{//数据库中没有就插入
       query.prepare("insert into dict(slot,slot_value) value(?,?)");
       query.bindValue(0,slot);
       query.bindValue(1,slot_value);
       query.exec();
    }
    db.close();
}

//保存标注结果到数据库
void antlWidget::on_btn_save_clicked()
{
    QMap<QString,QJsonObject> temp_map;//取出每个意图所对应的槽位信息，有重复的意图，放入数据库前需要处理成标注数据格式
    auto it=antlwidget_map.find(global_dialog);//用于找到所对应的标注窗口
    int itemCount=it.value()->ui->antl_list->count();
    for(int i=0;i<itemCount;i++){
        QWidget *antlSelect=it.value()->ui->antl_list->itemWidget(it.value()->ui->antl_list->item(i));
        QComboBox *intent=antlSelect->findChild<QComboBox*>("intent");
//        QSpinBox *intent_begin=antlSelect->findChild<QSpinBox*>("intent_begin");
//        QSpinBox *intent_end=antlSelect->findChild<QSpinBox*>("intent_end");
        QComboBox *slot=antlSelect->findChild<QComboBox*>("slot");
        QLineEdit *slot_value=antlSelect->findChild<QLineEdit*>("slot_value");
        QSpinBox *slot_begin=antlSelect->findChild<QSpinBox*>("slot_begin");
        QSpinBox *slot_end=antlSelect->findChild<QSpinBox*>("slot_end");
        QString intent_str=intent->currentText();//意图
//        int intent_begin_int=intent_begin->value();//意图开始范围
//        int intent_end_int=intent_end->value();//意图结束范围
        QString slot_str=slot->currentText();//槽位
        QString slot_value_str=slot_value->text();//槽值
        int slot_begin_int=slot_begin->value();//槽值开始范围
        int slot_end_int=slot_end->value();//槽值结束范围
        //如果是如下4种槽位，就加入到字典里
        if(slot_str=="疾病"||slot_str=="症状"||slot_str=="药物"||slot_str=="检查"){
            insert_dict(slot_str,slot_value_str);
        }

        QJsonObject obj;
        obj.insert("slot",slot_str);
        obj.insert("value",slot_value_str);
        obj.insert("start",slot_begin_int);
        obj.insert("end",slot_end_int);
        temp_map.insertMulti(intent_str,obj);
//        qDebug()<<"intent:"<<intent->currentText()<<" slot:"<<slot->currentText()
//                <<" slot_value:"<<slot_value->text()<<" slot_begin:"<<QString::number(slot_begin->value())
//                <<" slot_end:"<<QString::number(slot_end->value());
    }
    //按照标注数据格式
    //意图相同的合并重复项
    QMap<QString,QJsonArray> total_map;
    for(auto it=temp_map.begin();it!=temp_map.end();++it){
        auto total_it=total_map.find(it.key());
        if(total_it==total_map.end()){//未找到，将其放入
            QJsonArray not_find;
            not_find.push_back(it.value());
            total_map.insert(it.key(),not_find);
        }
        else{//在total_map找到了，合并相同意图的槽位信息
            total_map[it.key()].push_back(it.value());
        }
    }
    //构造成标注数据格式
    QJsonArray annotation;
    for(auto it=total_map.begin();it!=total_map.end();++it){
        QJsonObject intent_slot;
        intent_slot.insert("intent",it.key());
        intent_slot.insert("slot_values",it.value());
        annotation.push_back(intent_slot);
    }
    //添加版本号
    QJsonArray ans_arr;
    QJsonObject ans_obj;



    QSqlDatabase db=Conn::openConnection();
    QSqlQuery query(db);
    //因为审核数据和标注数据都使用这个函数，所以要先查看annotation是否为空，确定版本号
    query.exec("select annotation from utterances where utterance='"+global_dialog+"'");
    query.next();
    if(query.value("annotation").toString()==""){
        ans_obj.insert("version",0);
        ans_obj.insert("annotation",annotation);
        ans_arr.push_back(ans_obj);
    }
    else{
        //将读取出来的字符串转换为json
        QJsonDocument tran_anno=QJsonDocument::fromJson(query.value("annotation").toString().toUtf8());
        ans_arr=tran_anno.array();
        ans_obj.insert("version",ans_arr.size());
        ans_obj.insert("annotation",annotation);
        ans_arr.push_back(ans_obj);
    }
    //转换为字符串
    QJsonDocument annotation_docu(ans_arr);
    QString annotation_str=annotation_docu.toJson();
    //把标注数据放到表utterances中annotation

    query.prepare("update utterances set annotation=? where utterance=?");
    query.bindValue(0,annotation_str);
    query.bindValue(1,global_dialog);
    if(query.exec()){
//        QMessageBox::information(this,"提示","提交成功",QMessageBox::Ok);
        qDebug()<<"标注成功";
        //设置提交成功图标，提醒用户此条数据已经标注过了
        QImage img;
        img.load(":/antl/img/antl/finish.png");
        QPixmap pixmap(QPixmap::fromImage(img));
        pixmap=pixmap.scaled(ui->finish_icon->size(),Qt::KeepAspectRatio,Qt::SmoothTransformation);
        ui->finish_icon->setPixmap(pixmap);
    }

    qDebug()<<annotation_str;
    qDebug()<<QString::number(itemCount);
    db.close();
//    qDebug()<<global_dialog;
}
//右键删除信号
//要出现右键菜单，需要设置QListWidget的属性
//list_widget->setProperty("contextMenuPolicy", Qt::CustomContextMenu);
void antlWidget::on_antl_list_customContextMenuRequested(const QPoint &pos)
{
    QListWidgetItem* curItem=ui->antl_list->itemAt(pos);
    if(curItem==NULL)
        return;
    QMenu *popMenu=new QMenu(ui->antl_list);
    QAction *Menu1=popMenu->addAction(QIcon(":/antl/img/antl/delete.png"),"删除");
    connect(Menu1,SIGNAL(triggered(bool)),this,SLOT(menu_delete()));
    popMenu->exec(QCursor::pos());
}
//删除条目
void antlWidget::menu_delete()
{
    QListWidgetItem *item=ui->antl_list->currentItem();
    ui->antl_list->removeItemWidget(item);
    delete item;
}

