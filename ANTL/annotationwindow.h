#ifndef ANNOTATIONWINDOW_H
#define ANNOTATIONWINDOW_H

#include <QMainWindow>
#include<QFile>
#include<QDebug>
#include<QDir>
#include"DB/conn.h"
#include"antlwidget.h"
#include<iostream>
#include<ctime>
#include<QScrollBar>
#include<QDesktopServices>
#include<QUrl>
namespace Ui {
class AnnotationWindow;
}

class AnnotationWindow : public QMainWindow
{
    Q_OBJECT
signals:
    void try_exit();
public:
    explicit AnnotationWindow(QWidget *parent = 0);
    ~AnnotationWindow();
    void iniUi();
    static void inserted(antlWidget *,QString);
private slots:
    void receiveLogin(QString username,bool admin,bool test);//成功登录时
    void on_btn_back_clicked();

    void on_btn_import_clicked();

    void on_btn_annotation_clicked();

    void on_dialog_list_clicked(const QModelIndex &index);

    void on_dialog_text_selectionChanged();

    void on_btn_finish_clicked();

    void on_btn_test_clicked();

    void on_btn_export_clicked();

    void on_btn_explain_clicked();

    void on_btn_bili_clicked();

private:
    Ui::AnnotationWindow *ui;
};

#endif // ANNOTATIONWINDOW_H
