#include "annotationwindow.h"
#include "ui_annotationwindow.h"
QMap<QString,antlWidget*> antlwidget_map;
QString global_dialog;
int dialogId;
int classes;
AnnotationWindow::AnnotationWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::AnnotationWindow)
{
    ui->setupUi(this);
    iniUi();
}

AnnotationWindow::~AnnotationWindow()
{
    delete ui;
}
void AnnotationWindow::iniUi()
{
    //隐藏最大化
    this->setWindowFlags(this->windowFlags()&~Qt::WindowMinMaxButtonsHint|Qt::WindowMinimizeButtonHint);

    QFile qssFile(":antl/qss/annowin.qss");
    qssFile.open(QFile::ReadOnly);
    if(qssFile.isOpen())
    {
        QString style=QString(qssFile.readAll());
        setStyleSheet(style);
        qssFile.close();
    }
}
//登录成功显示标注页面
void AnnotationWindow::receiveLogin(QString username,bool admin,bool test)
{
    this->show();
    //设置进度条
    QSqlDatabase db=Conn::openConnection();
    QSqlQuery query(db);
    query.exec("select utteranceId from utterances");//查询共有多少天需要标注的数据
    ui->data_progress->setMaximum(query.size());
    query.exec("select utteranceId from utterances where annotation is not NULL");//查询标注了多少条数据
    this->setWindowTitle(QString::number(query.size()));
    ui->data_progress->setValue(query.size());

    //因为界面不是指针对象，所以不会自动销毁，返回登录界面后要重新更新控件为初始状态
    ui->username->setText(username);
    ui->btn_import->setEnabled(true);
    ui->btn_annotation->setEnabled(true);
    ui->btn_test->setEnabled(true);
    ui->btn_export->setEnabled(true);
    ui->btn_finish->setEnabled(true);
    ui->btn_import->setVisible(true);
    ui->btn_annotation->setVisible(true);
    ui->btn_test->setVisible(true);
    ui->btn_export->setVisible(true);
    ui->btn_finish->setVisible(true);
    ui->data_path->setVisible(true);
    ui->dialog_list->clear();
    ui->dialog_text->clear();
    ui->index_text->clear();
    //生成的窗口存储在map中，依次清除内存
    foreach (antlWidget *value, antlwidget_map.values()) {
        delete value;
    }
    antlwidget_map.clear();//将map清空
    //将堆叠窗口清空
    for(int i=ui->antl_widget->count();i>=0;i--){
        ui->antl_widget->removeWidget(ui->antl_widget->widget(i));
    }
    if(admin){//此时为管理员
        ui->btn_back->setToolTip("管理员");
        ui->username->setToolTip("管理员");
        ui->btn_annotation->setEnabled(false);//标注数据按钮不可用
        ui->btn_finish->setEnabled(false);
        ui->btn_finish->setVisible(false);
        ui->btn_annotation->setVisible(false);
        ui->dialogNum->setVisible(false);
        classes=2;
    }
    else if(test){
        query.exec("select dialogNum from conversations where checkState='1' and checkName='"+ui->username->text()+"'");
        int dialogNum=0;
        while(query.next()){
            dialogNum+=query.value("dialogNum").toInt();
        }
        ui->dialogNum->setText(QString::number(dialogNum)+"条");
        ui->dialogNum->setVisible(true);
        ui->btn_back->setToolTip("审核员");
        ui->username->setToolTip("审核员");
        ui->btn_import->setEnabled(false);
        ui->btn_annotation->setEnabled(false);
        ui->btn_export->setEnabled(false);
        ui->btn_finish->setEnabled(false);
        ui->btn_import->setVisible(false);
        ui->btn_annotation->setVisible(false);
        ui->btn_export->setVisible(false);
        ui->btn_finish->setVisible(false);
        ui->data_path->setVisible(false);
        classes=1;
    }
    else{
        query.exec("select dialogNum from conversations where conversationState='2' and userName='"+ui->username->text()+"'");
        int dialogNum=0;
        while(query.next()){
            dialogNum+=query.value("dialogNum").toInt();
        }
        ui->dialogNum->setText(QString::number(dialogNum)+"条");
        ui->dialogNum->setVisible(true);
        ui->btn_back->setToolTip("标注员");
        ui->username->setToolTip("标注员");
        classes=0;
        ui->btn_import->setEnabled(false);
        ui->btn_test->setEnabled(false);
        ui->btn_export->setEnabled(false);
        ui->btn_import->setVisible(false);
        ui->btn_test->setVisible(false);
        ui->btn_export->setVisible(false);
        ui->data_path->setVisible(false);
        //显示此用户还未标注完的对话数据
//        QSqlDatabase db=Conn::openConnection();
//        QSqlQuery query(db);
        query.exec("select conversationId from conversations where conversationState = '1' and userName='"+ui->username->text()+"' order by conversationId");
        if(query.next()){
            int conversationId=query.value("conversationId").toInt();
            dialogId=conversationId;
            //将抽取的对话数据显示出来
            query.prepare("select utterance,angetRole,sequenceNo from utterances where conversationId=? order by sequenceNo");
            query.bindValue(0,conversationId);
            query.exec();
            while(query.next()){
                QString utterance = query.value("utterance").toString();
                if(query.value("angetRole").toString()=="doctor"){
                    QListWidgetItem *item=new QListWidgetItem(QIcon(":annowin/img/annowin/doctor.png"),utterance);
                    ui->dialog_list->setIconSize(QSize(40,40));
                    ui->dialog_list->addItem(item);
                }
                else{
                    QListWidgetItem *item=new QListWidgetItem(QIcon(":annowin/img/annowin/patient.png"),utterance);
                    ui->dialog_list->setIconSize(QSize(40,40));
                    ui->dialog_list->addItem(item);
                }

//                ui->dialog_list->addItem(utterance);
            }
            ui->btn_annotation->setEnabled(false);
        }

    }
    db.close();


}
//返回登录界面
void AnnotationWindow::on_btn_back_clicked()
{
    this->close();
    emit(try_exit());//发送信号
}
//导入数据功能
void AnnotationWindow::on_btn_import_clicked()
{
    //读取有多少次对话
    QFile data_length(QDir::currentPath()+"/py/data_length.txt");
    if(!data_length.open(QIODevice::ReadOnly | QIODevice::Text)){
        qDebug()<<"打开data_length文件失败";
    }
    int data_len = QString(data_length.readLine()).toInt();//获得有多少次对话
    data_length.close();
    //读取原始数据到数据库
    QSqlDatabase db=Conn::openConnection();
    QSqlQuery query(db);
    int sequenceNo=0;//对话在对话列表中的顺序号
    QFile data(QDir::currentPath()+"/"+ui->data_path->text());
    if(!data.open(QIODevice::ReadOnly | QIODevice::Text)){
        qDebug()<<"打开data文件失败";
    }
    //不到文件末尾，一直读文件
    while(!data.atEnd()){
        QByteArray line = data.readLine();
        QString str(line);
        str=str.trimmed();
        if(str!=""){
            sequenceNo++;
            QStringList single = str.split(" ");
            //向表utterances插入数据
            query.prepare("insert into utterances(conversationId,utterance,sequenceNo,angetRole) value(?,?,?,?)");
            query.bindValue(0,data_len);
            query.bindValue(1,single[1]);
            query.bindValue(2,sequenceNo);
            query.bindValue(3,single[0]);
            if(query.exec()){
                qDebug()<<"插入对话数据执行成功";
            }
//            qDebug()<<single[0];
//            qDebug()<<single[1];
        }
        else{
            //向表conversations插入数据
            query.prepare("insert into conversations(conversationId,conversationState,checkState,dialogNum) values(?,?,?,?)");
            query.bindValue(0,data_len);
            query.bindValue(1,0);
            query.bindValue(2,0);
            query.bindValue(3,sequenceNo);
            if(query.exec()){
                qDebug()<<"对话状态插入成功";
            }
            data_len++;//对话列表编号
            sequenceNo=0;//新的对话列表了，置于0
            qDebug()<<"我是空格";
        }
    }
    if(data.atEnd()){
        query.prepare("delete conversations where conversationId = ?");
        query.bindValue(0,data_len-1);
        if(query.exec()){
            qDebug()<<"删除成功";
        }
        //更新对话次数
        if(!data_length.open(QIODevice::WriteOnly | QIODevice::Text)){
            qDebug()<<"打开data_length文件失败";
        }
        QTextStream out(&data_length);
        out<<data_len;
        qDebug()<<"到文件末尾了";
        QMessageBox::information(this,"提示","导入成功",QMessageBox::Ok);
    }
    data_length.close();
    data.close();
    db.close();
}
//抽调标注数据
void AnnotationWindow::on_btn_annotation_clicked()
{

    ui->dialog_list->clear();//清空数据展示
    //从数据库中选取数据
    //从conversations表中选取1条conversationState为0的数据，按照conversationId从表utterances中抽取对话数据
    QSqlDatabase db = Conn::openConnection();
    QSqlQuery query(db);
    query.exec("select conversationId from conversations where conversationState = '0' order by conversationId");

    if(query.size()!=0&&query.next()){
        ui->btn_annotation->setEnabled(false);//标注按钮变为不可用
        int conversationId=query.value("conversationId").toInt();
        dialogId=conversationId;
        //将抽取的对话数据显示出来
        query.prepare("select utterance,angetRole,sequenceNo from utterances where conversationId=? order by sequenceNo");
        query.bindValue(0,conversationId);
        query.exec();
        while(query.next()){
            QString utterance = query.value("utterance").toString();
            if(query.value("angetRole").toString()=="doctor"){
                QListWidgetItem *item=new QListWidgetItem(QIcon(":annowin/img/annowin/doctor.png"),utterance);
                ui->dialog_list->setIconSize(QSize(40,40));
                ui->dialog_list->addItem(item);
            }
            else{
                QListWidgetItem *item=new QListWidgetItem(QIcon(":annowin/img/annowin/patient.png"),utterance);
                ui->dialog_list->setIconSize(QSize(40,40));
                ui->dialog_list->addItem(item);
            }
        }
        //抽取后更新标注状态为1，表示未标注完毕的中间状态
        query.prepare("update conversations set conversationState='1',userName=? where conversationId=?");
        query.bindValue(0,ui->username->text());
        query.bindValue(1,conversationId);
        query.exec();
    }
    else{
        QMessageBox::information(this,"提示","没有可标注的数据",QMessageBox::Ok);
    }
    db.close();
}
//解决只标注了一部分
void AnnotationWindow::inserted(antlWidget *antl,QString dialog_data){
    QSqlDatabase db=Conn::openConnection();
    QSqlQuery query(db);
    query.exec("select annotation from utterances where utterance='"+dialog_data+"'");
    while(query.next()){
        QString annotation=query.value("annotation").toString();
        if(annotation!=""){//找到已经标注的
            QJsonDocument anno_docu=QJsonDocument::fromJson(annotation.toUtf8());
            QJsonArray ans_arr=anno_docu.array();
            QJsonObject ans_obj=ans_arr.at(ans_arr.size()-1).toObject();//去最后一个版本号
            QJsonArray anno_array=ans_obj["annotation"].toArray();
            //根据标注数据格式解析json
            for(auto it=anno_array.begin();it!=anno_array.end();++it){
                QJsonObject anno_obj=(*it).toObject();
                QString intent=anno_obj["intent"].toString();
                QJsonArray slot_values=anno_obj["slot_values"].toArray();
                for(auto slot_it=slot_values.begin();slot_it!=slot_values.end();++slot_it){
                    QJsonObject slot_obj=(*slot_it).toObject();
                    QListWidgetItem *antlItem=new QListWidgetItem;
                    selectItem *antlSelect=new selectItem;
                    //找到子控件，用于赋初值
                    QComboBox *intent_box=antlSelect->findChild<QComboBox*>("intent");
                    QComboBox *slot_box=antlSelect->findChild<QComboBox*>("slot");
                    QLineEdit *slot_value=antlSelect->findChild<QLineEdit*>("slot_value");
                    QSpinBox *slot_begin=antlSelect->findChild<QSpinBox*>("slot_begin");
                    QSpinBox *slot_end=antlSelect->findChild<QSpinBox*>("slot_end");
                    intent_box->setCurrentText(intent);
                    slot_box->setCurrentText(slot_obj["slot"].toString());
                    slot_value->setText(slot_obj["value"].toString());
                    slot_begin->setValue(slot_obj["start"].toInt());
                    slot_end->setValue(slot_obj["end"].toInt());
                    //将初始化控件加载到listWidget中
                    antlItem->setSizeHint(QSize(700,40));
                    QListWidget *antl_list=antl->findChild<QListWidget*>("antl_list");
                    antl_list->addItem(antlItem);
                    antl_list->setItemWidget(antlItem,antlSelect);
                }
            }
            break;
        }
    }
    db.close();
}

//单击对话数据
void AnnotationWindow::on_dialog_list_clicked(const QModelIndex &index)
{
    //分两种情况，审核员审核数据单击和标注员标注数据点击
    if(classes==1||classes==2){//审核员或者管理员
        //获取一条对话数据展示到text框，并打开一个标注窗口
        QString dialog_data = index.data().toString();
        ui->dialog_text->setText(dialog_data);

        global_dialog=dialog_data;//每次点击设置全局变量对话数据
        //选择工具界面展示到标注窗口
        auto it=antlwidget_map.find(dialog_data);
        if(it==antlwidget_map.end()){//无此窗口，就新建
            antlWidget *antl=new antlWidget();
            ui->antl_widget->addWidget(antl);
            ui->antl_widget->setCurrentWidget(antl);
            antlwidget_map.insert(dialog_data,antl);

            //把已经标注的数据取出来，进行解析，看有多少个条目进行循环
            QSqlDatabase db=Conn::openConnection();
            QSqlQuery query(db);
            query.exec("select annotation from utterances where utterance='"+dialog_data+"'");
            while(query.next()){
                QString annotation=query.value("annotation").toString();
                if(annotation!=""){//找到已经标注的
                    QJsonDocument anno_docu=QJsonDocument::fromJson(annotation.toUtf8());
                    QJsonArray ans_arr=anno_docu.array();
                    QJsonObject ans_obj=ans_arr.at(ans_arr.size()-1).toObject();//去最后一个版本号
                    QJsonArray anno_array=ans_obj["annotation"].toArray();
                    //根据标注数据格式解析json
                    for(auto it=anno_array.begin();it!=anno_array.end();++it){
                        QJsonObject anno_obj=(*it).toObject();
                        QString intent=anno_obj["intent"].toString();
                        QJsonArray slot_values=anno_obj["slot_values"].toArray();
                        for(auto slot_it=slot_values.begin();slot_it!=slot_values.end();++slot_it){
                            QJsonObject slot_obj=(*slot_it).toObject();
                            QListWidgetItem *antlItem=new QListWidgetItem;
                            selectItem *antlSelect=new selectItem;
                            //找到子控件，用于赋初值
                            QComboBox *intent_box=antlSelect->findChild<QComboBox*>("intent");
                            QComboBox *slot_box=antlSelect->findChild<QComboBox*>("slot");
                            QLineEdit *slot_value=antlSelect->findChild<QLineEdit*>("slot_value");
                            QSpinBox *slot_begin=antlSelect->findChild<QSpinBox*>("slot_begin");
                            QSpinBox *slot_end=antlSelect->findChild<QSpinBox*>("slot_end");
                            intent_box->setCurrentText(intent);
                            slot_box->setCurrentText(slot_obj["slot"].toString());
                            slot_value->setText(slot_obj["value"].toString());
                            slot_begin->setValue(slot_obj["start"].toInt());
                            slot_end->setValue(slot_obj["end"].toInt());
                            //将初始化控件加载到listWidget中
                            antlItem->setSizeHint(QSize(700,40));
                            QListWidget *antl_list=antl->findChild<QListWidget*>("antl_list");
                            antl_list->addItem(antlItem);
                            antl_list->setItemWidget(antlItem,antlSelect);
                        }
                    }


//                    ui->antl_list->addItem(antlItem);
//                    ui->antl_list->setItemWidget(antlItem,antlSelect);
                    break;
                }
            }

        }
        else{
            ui->antl_widget->setCurrentWidget(it.value());
        }


    }
    else{//标注员
        //获取一条对话数据展示到text框，并打开一个标注窗口
        QString dialog_data = index.data().toString();
        ui->dialog_text->setText(dialog_data);

        global_dialog=dialog_data;//每次点击设置全局变量对话数据
        //选择工具界面展示到标注窗口
        auto it=antlwidget_map.find(dialog_data);
        if(it==antlwidget_map.end()){
            antlWidget *antl=new antlWidget();
            ui->antl_widget->addWidget(antl);
            ui->antl_widget->setCurrentWidget(antl);
            antlwidget_map.insert(dialog_data,antl);
            //检查是否有标注过的
            AnnotationWindow::inserted(antl,dialog_data);
        }
        else{
            ui->antl_widget->setCurrentWidget(it.value());
        }
    }
}
//显示鼠标选中字在对话数据中的位置
void AnnotationWindow::on_dialog_text_selectionChanged()
{
    //显示鼠标选中的字在对话数据中的位置
    int charIndex = ui->dialog_text->textCursor().position();
    QString strText=ui->dialog_text->toPlainText().left(charIndex);
    if(strText.length()>0){
        QString select_word=QString(strText.at(strText.length()-1));
        int index=strText.length();
        QString index_str=QString::number(index);
        ui->index_text->setText(select_word+":"+index_str);
        qDebug()<<QString(strText.at(strText.length()-1));
    }
}
//这次所有对话数据都标注完成后，点击完成标注
void AnnotationWindow::on_btn_finish_clicked()
{
    QSqlDatabase db=Conn::openConnection();
    QSqlQuery query(db);

    query.exec("select utteranceId from utterances where conversationId='"+QString::number(dialogId)+"'");
    int dialog_num=query.size();
    query.exec("select utteranceId from utterances where annotation is not null and conversationId='"+QString::number(dialogId)+"'");
    int anno_num=query.size();
    if(anno_num==dialog_num){
        //清除所有展示文本的控件内容
        ui->dialog_text->clear();//文本展示控件
        ui->index_text->clear();//索引空间
        ui->dialog_list->clear();//对话数据的控件
        //生成的窗口存储在map中，依次清除内存
        foreach (antlWidget *value, antlwidget_map.values()) {
            delete value;
        }
        antlwidget_map.clear();//将map清空
        //将堆叠窗口清空
        for(int i=ui->antl_widget->count();i>=0;i--){
            ui->antl_widget->removeWidget(ui->antl_widget->widget(i));
        }

        //将此次标注的状态改为2，表示已经标注完毕

        query.prepare("update conversations set conversationState=? where conversationId=?");
        query.bindValue(0,2);
        query.bindValue(1,dialogId);
        query.exec();

        //有一种情况，如果此时该用户还有未标注完毕的数据，那就展示它们，继续标注
        //此时标注数据按钮还是不可用状态
        query.prepare("select conversationId from conversations where userName=? and conversationState='1' order by conversationId");
        query.bindValue(0,ui->username->text());
        query.exec();
        if(query.size()!=0&&query.next()){
            int conversationId=query.value("conversationId").toInt();
            dialogId=conversationId;//用于改变标注状态
            //将抽取的对话数据显示出来
            query.prepare("select utterance,sequenceNo from utterances where conversationId=? order by sequenceNo");
            query.bindValue(0,conversationId);
            query.exec();
            while(query.next()){
                QString utterance = query.value("utterance").toString();
                ui->dialog_list->addItem(utterance);
            }
            ui->btn_annotation->setEnabled(false);//标注数据按钮设置为不可用
        }
        else{
            //此时要是没有未标注完的数据就更新按钮状态可用
            ui->btn_annotation->setEnabled(true);//标注数据按钮设置为可用
        }
        //更新进度条
        query.exec("select utteranceId from utterances");//查询共有多少天需要标注的数据
        ui->data_progress->setMaximum(query.size());
        query.exec("select utteranceId from utterances where annotation is not NULL");//查询标注了多少条数据
        ui->data_progress->setValue(query.size());
        this->setWindowTitle(QString::number(query.size()));
    }
    else{
        QMessageBox::information(this,"提示","还未标注完毕，仔细检查!",QMessageBox::Ok);
    }
    //标注数据标注了多少数据
    query.exec("select dialogNum from conversations where conversationState='2' and userName='"+ui->username->text()+"'");
    int dialogNum=0;
    while(query.next()){
        dialogNum+=query.value("dialogNum").toInt();
    }
    ui->dialogNum->setText(QString::number(dialogNum)+"条");
    db.close();
}
//审核数据
void AnnotationWindow::on_btn_test_clicked()
{
    //清除所有展示文本的控件内容
    ui->dialog_text->clear();//文本展示控件
    ui->index_text->clear();//索引空间
    ui->dialog_list->clear();//对话数据的控件
    //生成的窗口存储在map中，依次清除内存
    foreach (antlWidget *value, antlwidget_map.values()) {
        delete value;
    }
    antlwidget_map.clear();//将map清空
    //将堆叠窗口清空
    for(int i=ui->antl_widget->count();i>=0;i--){
        ui->antl_widget->removeWidget(ui->antl_widget->widget(i));
    }

    //从数据库中随机抽取已标注的数据
    qsrand(time(NULL));//随机数种子
    //从convalescences表查询有多少个已经标注完毕的对话
    QSqlDatabase db=Conn::openConnection();
    QSqlQuery query(db);
    if(classes==2){//管理员
        query.exec("select conversationId from conversations where conversationState='2' order by conversationId");
        if(query.size()!=0){
            int id=qrand()%query.size();//产生[0,query.size())之间的数字
            query.seek(id);
            int conversationId=query.value("conversationId").toInt();
            qDebug()<<conversationId;
            query.prepare("select utterance,angetRole,sequenceNo from utterances where conversationId=? order by sequenceNo");
            query.bindValue(0,conversationId);
            query.exec();
            while(query.next()){
                QString utterance = query.value("utterance").toString();
                if(query.value("angetRole").toString()=="doctor"){
                    QListWidgetItem *item=new QListWidgetItem(QIcon(":annowin/img/annowin/doctor.png"),utterance);
                    ui->dialog_list->setIconSize(QSize(40,40));
                    ui->dialog_list->addItem(item);
                }
                else{
                    QListWidgetItem *item=new QListWidgetItem(QIcon(":annowin/img/annowin/patient.png"),utterance);
                    ui->dialog_list->setIconSize(QSize(40,40));
                    ui->dialog_list->addItem(item);
                }
            }
        }
        else{
            QMessageBox::information(this,"提示","无标注数据",QMessageBox::Ok);
        }

    }
    else if(classes==1){//审核员
        //查询审核员审核了多少条数据
        query.exec("select dialogNum from conversations where checkState='1' and checkName='"+ui->username->text()+"'");
        int dialogNum=0;
        while(query.next()){
            dialogNum+=query.value("dialogNum").toInt();
        }
        ui->dialogNum->setText(QString::number(dialogNum)+"条");
        query.exec("select conversationId from conversations where conversationState='2' and checkState='0' order by conversationId");
        if(query.size()!=0){
            query.next();
            int conversationId=query.value("conversationId").toInt();
            qDebug()<<conversationId;
            query.exec("update conversations set checkState='1',checkName='"+ui->username->text()+"' where conversationId='"+QString::number(conversationId)+"'");
            query.prepare("select utterance,angetRole,sequenceNo from utterances where conversationId=? order by sequenceNo");
            query.bindValue(0,conversationId);
            query.exec();
            while(query.next()){
                QString utterance = query.value("utterance").toString();
                if(query.value("angetRole").toString()=="doctor"){
                    QListWidgetItem *item=new QListWidgetItem(QIcon(":annowin/img/annowin/doctor.png"),utterance);
                    ui->dialog_list->setIconSize(QSize(40,40));
                    ui->dialog_list->addItem(item);
                }
                else{
                    QListWidgetItem *item=new QListWidgetItem(QIcon(":annowin/img/annowin/patient.png"),utterance);
                    ui->dialog_list->setIconSize(QSize(40,40));
                    ui->dialog_list->addItem(item);
                }
            }

        }
        else{
            QMessageBox::information(this,"提示","无标注数据",QMessageBox::Ok);
        }
    }


    db.close();
}
//导出数据
void AnnotationWindow::on_btn_export_clicked()
{
    //从表conversations中获取标注状态为2的对话id，再从utterances表中获取数据，封装成json，导出json文件
    QSqlDatabase db=Conn::openConnection();
    QSqlQuery conver_query(db);
    QSqlQuery utter_query(db);
    conver_query.exec("select conversationId,userName from conversations where conversationState='2' order by conversationId");
    if(conver_query.size()==0){
        QMessageBox::information(this,"提示","无数据可导出",QMessageBox::Ok);
    }
    else{//封装数据为json，并写入json文件
        QJsonArray annotation_arr;//最后保存的结果json数组
        while(conver_query.next()){//得到conversationId
            int conversationId=conver_query.value("conversationId").toInt();
            QString userName=conver_query.value("userName").toString();
            utter_query.prepare("select angetRole,utterance,sequenceNo,annotation,sequenceNo from utterances where conversationId=? order by sequenceNo");
            utter_query.bindValue(0,conversationId);
            utter_query.exec();
            QJsonArray utterances_arr;
            QJsonObject dialog_obj;
            while(utter_query.next()){//得到表utterances中的数据
                QString angetRole=utter_query.value("angetRole").toString();//对话角色
                QString utterance=utter_query.value("utterance").toString();//对话文本
                int sequenceNo=utter_query.value("sequenceNo").toInt();//文本顺序号
                QString annotation=utter_query.value("annotation").toString();//标注结果
                //将读取出来的标注结果转换为json对象
                QJsonDocument tran_anno=QJsonDocument::fromJson(annotation.toUtf8());
                QJsonArray ans_arr=tran_anno.array();
                QJsonObject ans_obj=ans_arr.at(ans_arr.size()-1).toObject();
                QJsonArray tran_annoArr=ans_obj["annotation"].toArray();
                QJsonObject single_obj;
                single_obj.insert("angetRole",angetRole);
                single_obj.insert("utterance",utterance);
                single_obj.insert("sequenceNo",sequenceNo);
                single_obj.insert("annotation",tran_annoArr);
                utterances_arr.push_back(single_obj);
            }
            dialog_obj.insert("conversationId",conversationId);
            dialog_obj.insert("userName",userName);
            dialog_obj.insert("utterances",utterances_arr);
            annotation_arr.push_back(dialog_obj);
        }
        QJsonDocument annotation_docu(annotation_arr);
        QFile file("./result/annotation.json");
        if(file.open(QIODevice::WriteOnly)){
            file.write(annotation_docu.toJson());
            QMessageBox::information(this,"提示","导出成功",QMessageBox::Ok);
            file.close();
        }
        else{
            QMessageBox::information(this,"提示","导入文件错误",QMessageBox::Ok);
        }
    }
    db.close();

}
//标注规范
void AnnotationWindow::on_btn_explain_clicked()
{
    QDesktopServices::openUrl(QUrl(QString("https://gitee.com/liugang19970527/antl/blob/master/%E6%A0%87%E6%B3%A8%E8%A7%84%E8%8C%83/%E6%95%B0%E6%8D%AE%E6%A0%87%E6%B3%A8%E8%A7%84%E8%8C%83.md")));
}
//标注过程
void AnnotationWindow::on_btn_bili_clicked()
{
    QDesktopServices::openUrl(QUrl(QString("https://www.bilibili.com/video/BV1Mb4y1s7Yi?share_source=copy_web")));
}
