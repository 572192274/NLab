#ifndef LOGIN_H
#define LOGIN_H

#include <QDialog>
#include<QFile>
#include<QDebug>
#include<QGraphicsDropShadowEffect>
#include<QMouseEvent>
#include<QSystemTrayIcon>
#include"DB/conn.h"
#include<ctime>
#include<QPixmap>
#include<QImage>
namespace Ui {
class login;
}

class login : public QDialog
{
    Q_OBJECT
//去掉标签栏，还能让对话框移动起来
protected:
    void mousePressEvent(QMouseEvent *e);
    void mouseMoveEvent(QMouseEvent *e);
    void mouseReleaseEvent(QMouseEvent *e);
public:
    explicit login(QWidget *parent = 0);
    ~login();
    void iniUI();//初始化界面
    void keyPressEvent(QKeyEvent *event);//设置回车登录函数

private slots:
    void show_login();//显示登录界面
    void on_btn_exit_clicked();

    void on_btn_small_clicked();

    void on_btn_register_clicked();

    void on_btn_login_clicked();

private:
    QPoint last;//获得点坐标
    QSystemTrayIcon *trayIcon;
    Ui::login *ui;
signals:
    void showmain(QString,bool,bool);
};

#endif // LOGIN_H
