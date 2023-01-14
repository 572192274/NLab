#include "login.h"
#include "ui_login.h"
//鼠标点击时获得点坐标
void login::mousePressEvent(QMouseEvent *e)
{
    last=e->globalPos();
}
//移动时获得点坐标
void login::mouseMoveEvent(QMouseEvent *e)
{
    int dx = e->globalX() - last.x();
    int dy = e->globalY() - last.y();
    last = e->globalPos();
    move(x()+dx, y()+dy);
}
//释放鼠标获得点坐标
void login::mouseReleaseEvent(QMouseEvent *e)
{
    int dx = e->globalX() - last.x();
    int dy = e->globalY() - last.y();
    move(x()+dx, y()+dy);
}

login::login(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::login)
{

    ui->setupUi(this);
    iniUI();
}

login::~login()
{
    delete ui;
}
//初始化界面
void login::iniUI()
{
    //随机挑选图片，保持好心情
    qsrand(time(NULL));//随机数种子
    QImage img;
    img.load(":login/img/login/emojo/"+QString::number(qrand()%19)+".png");
    QPixmap pixmap(QPixmap::fromImage(img));
    pixmap=pixmap.scaled(ui->user_img->size(),Qt::KeepAspectRatio,Qt::SmoothTransformation);
    ui->user_img->setPixmap(pixmap);

    QGraphicsDropShadowEffect *shadow = new QGraphicsDropShadowEffect(this);
    //设置阴影距离
    shadow->setOffset(100, 0);
    //设置阴影颜色
    shadow->setColor(QColor("#444444"));
    //设置阴影圆角
    shadow->setBlurRadius(30);
    //给嵌套QWidget设置阴影
    this->setGraphicsEffect(shadow);
    //设置无边框
    this->setWindowFlags(Qt::Window | Qt::FramelessWindowHint);
    ui->topic->setAttribute(Qt::WA_TranslucentBackground);//设置标签lable透明
    QFile qssFile(":login/qss/login.qss");
    qssFile.open(QFile::ReadOnly);
    if(qssFile.isOpen())
    {
        qDebug()<<"打开qss成功";
        QString style=QLatin1String(qssFile.readAll());
        setStyleSheet(style);
        qssFile.close();
    }
    ui->username->setFocus();
}
//关闭窗口
void login::on_btn_exit_clicked()
{
    this->close();
}
//窗口最小化
void login::on_btn_small_clicked()
{
    this->hide();
    QIcon icon = QIcon(":/login/img/login/logo.ico");
    trayIcon = new QSystemTrayIcon(this);
    trayIcon->setIcon(icon);
    trayIcon->setToolTip("ANTL");
    trayIcon->show();
    //响应双击或者单击恢复界面的槽函数
    connect(trayIcon, &QSystemTrayIcon::activated, this, [=](QSystemTrayIcon::ActivationReason reason){
        switch (reason)
        {
            //点击托盘显示窗口
            case QSystemTrayIcon::Trigger:
            {
                showNormal();
                delete trayIcon;
                break;
            }
            //双击托盘显示窗口
            case QSystemTrayIcon::DoubleClick:
            {
                showNormal();
                delete trayIcon;
                break;
            }
            default:
                break;
        }
    });
}
//注册用户
void login::on_btn_register_clicked()
{
    QSqlDatabase db = Conn::openConnection();
    // 如果 MySQL 数据库中已经存在同名的表， 那么下面的代码不会执行
    QSqlQuery query(db);
    QSqlQuery query2(db);
    QString username = ui->username->text();
    QString password = ui->password->text();
    if(username == "" || password == "")
    {
        QMessageBox::warning(this,tr("提示"),tr("用户名或密码不可以为空"), QMessageBox::Yes);
        ui->username->clear();
        ui->password->clear();
        ui->username->setFocus();
    } else {
        query.prepare("insert into user(userName,password,classes) values(?,?,?)");
        query.bindValue(0,username);
        query.bindValue(1,password);
        query.bindValue(2,0);//注册时都是标注员
        QString sql = "select userName from user where userName = '%1'";
        sql = sql.arg(username);
        bool result = query2.exec(sql);//判断sql执行是否成功
        qDebug() << result;
        query2.next();//移动到第一条数据
        qDebug() << query2.value("userName").toString();
        if(result && query2.value("userName").toString()==""){
            //可以注册
            if(query.exec()){
                QMessageBox::information(this,"提示","注册成功",QMessageBox::Ok);
                qDebug()<<"注册成功";
            } else {
                QMessageBox::information(this,"提示","注册失败",QMessageBox::Ok);
                qDebug()<<"注册失败";
            }
        } else if(query2.value("userName").toString()==username){
            //重复
            QMessageBox::information(this,"提示","用户名已注册",QMessageBox::Ok);
        } else {
            QMessageBox::information(this,"提示","注册失败",QMessageBox::Ok);
            qDebug()<<"sql查询失败";
        }
    }
    Conn::closeConnection(db);
}
//登录
void login::on_btn_login_clicked()
{
    QString username=ui->username->text();
    QString password=ui->password->text();
    QSqlDatabase db = Conn::openConnection();
    QSqlQuery query(db);
    QString sql="select userName,password,classes from user where userName='"+username+"'";
    if(query.exec(sql))
    {
        qDebug()<<"查询成功";
    }
    query.next();

    if((ui->username->text()=="")||(ui->password->text()==""))
    {
        QMessageBox::warning(this,tr("提示"),tr("用户名或密码不为空"),QMessageBox::Yes);
        ui->username->clear();
        ui->username->setFocus();
    }
    else if(ui->username->text()!=query.value("userName").toString())
    {
        QMessageBox::warning(this,tr("提示"),tr("用户名不存在，请重新输入"),QMessageBox::Yes);
        ui->username->clear();
        ui->username->setFocus();
    }
    else if((ui->username->text()==query.value("userName").toString())&&(ui->password->text()!=query.value("password").toString()))
    {
        QMessageBox::warning(this,tr("提示"),tr("用户名密码不匹配，请重新输入"),QMessageBox::Yes);
        ui->username->setFocus();
    }
    //登录成功
    else if((query.value("userName").toString()==username)&&(query.value("password").toString()==password))
    {
//        globalusername=ui->username->text();
//        ConnectService(globalusername);

        ui->username->clear();
        ui->password->clear();
        ui->username->setFocus();
        if(query.value("classes").toInt()==2&&ui->check_admin->isChecked()){
            this->hide();
            emit showmain(username,true,false);//现在是管理员登录
        }
        else if(query.value("classes").toInt()==1&&ui->check_test->isChecked()){
            this->hide();
            emit showmain(username,false,true);//现在是审核员登录
        }
        else if(query.value("classes").toInt()==0){
            this->hide();
            emit showmain(username,false,false);//现在是标注员登录
        }
        else{
            QMessageBox::information(this,tr("提示"),tr("身份错误"),QMessageBox::Ok);
        }

    }
    Conn::closeConnection(db);
}
//设置回车登录响应
void login::keyPressEvent(QKeyEvent *event)
{
    if(event->key()==Qt::Key_Enter||event->key()==Qt::Key_Return)
    {
        emit ui->btn_login->click();
    }
}
//回显登录界面
void login::show_login()
{
    this->show();
    //随机挑选图片，保持好心情
    qsrand(time(NULL));//随机数种子
    QImage img;
    img.load(":login/img/login/emojo/"+QString::number(qrand()%19)+".png");
    QPixmap pixmap(QPixmap::fromImage(img));
    pixmap=pixmap.scaled(ui->user_img->size(),Qt::KeepAspectRatio,Qt::SmoothTransformation);
    ui->user_img->setPixmap(pixmap);

}
