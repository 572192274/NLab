#include "annotationwindow.h"
#include <QApplication>
#include"login.h"
#include"DB/conn.h"
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    AnnotationWindow w;
    login Login;
    Login.show();
    QObject::connect(&Login,SIGNAL(showmain(QString,bool,bool)),&w,SLOT(receiveLogin(QString,bool,bool)));
    QObject::connect(&w,SIGNAL(try_exit()),&Login,SLOT(show_login()));
    int ret = a.exec();
    Conn::release();//释放数据库连接池
    return ret;
}
