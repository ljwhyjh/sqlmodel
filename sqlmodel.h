#ifndef SQLMODEL_H
#define SQLMODEL_H

#include <QWidget>
#include <QDateTime>
#include <QString>
#include <QSettings>

#include <QSqlDatabase>
#include <QDate>
#include <QSqlQuery>
//#include <QDebug>
//#include <QMessageBox>
//#include<QSqlError>
//#include <QTime>

#include <QStandardItemModel>
#include <QHeaderView>
#include <QtXml/QDomDocument>

#define MAXROW 1000
#define MAXCOL 3
namespace Ui {
class sqlmodel;
}

class sqlmodel : public QWidget
{
    Q_OBJECT

public:
    explicit sqlmodel(QWidget *parent=0 );
    ~sqlmodel();
    QSqlDatabase db;
    QSqlDatabase lisdb;
    //QSqlQuery query(db);


    QSettings *configIniRead;
    QString text1;
    QString text2; //代码转换的路径

    QDomDocument *dom;

    QStandardItemModel  *model;

    QString lastStr;
    QString numberStr;
    int numberId;
    QString lastConcen;
    int lastId;

    bool selectAll(const QDate& date);
    bool selectOne(const QDate& date,const QString& ybh);

    void selectView(int sampleNum);

private slots:
     void selectCurrent();

    void on_btnSelect_clicked();

    void on_ckByb_clicked();

    void on_btnSend_clicked();

private:
    Ui::sqlmodel *ui;

private:
    QString machineCode;
    int num;  //样本数
    int total; //样本*项目数
    QDate current_date_time;
    QString current_date;

    QString selectData[MAXROW][MAXCOL];
    int n;


    QString todayStr;
    QString afterStr;

    QDate selectdate;
    bool selectCheckBox=false;//复选框
    QString yangbenhao;



    bool sqlConnection();
    bool lisConnection();

    bool saveData(const QDate& date,const int& sampleNum);  //保存数据
    bool sendData(const QDate& date);  //上传数据
    void sendOne(const QDate &date,const QString& sampleId, const QString& itemCode,const QString& concentration);

  //  QString itemIdConvert(QString& itemId);//将仪器的项目编码转换成LIS 里面的项目编码
    QString getXmlValue(const QString &key); //从XML文件读入仪器项目代码


};

#endif // SQLMODEL_H
