#ifndef WIDGET_H
#define WIDGET_H
#include <QWidget>
#include "communication.h"
#include "database.h"

namespace Ui {
class Widget;
}

class Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = nullptr);

    ~Widget();

    void dealData(QString data);

    void comfirm_log(QString data);

    void comfirm_register(QString data);

    void comfirm_delete(QString data);

    void insertUser(QString name, QString password,QString money,int type, int isOnline);

    void deletetUser(QString name, QString password,QString money,int type, int isOnline);

    void insertUserLog(QString name, QString type,QString creat_time,QString message);

    void comfirm_drinks();

    void comfirm_buy(QString data);

    QString send_drinks();

    QString send_user(QString name);

    QString comfirm_hot();

    QString count_sale();

public slots:
    void newconnectionSLOT();

    void disconnectedSLOT();

    void readDateSLOT();

private slots:
    void on_button_flush_userLog_clicked();

    void on_button_userLog_clicked();

    void on_button_returnControl_ulog_clicked();

    void on_button_flush_buyLog_clicked();

    void on_button_returnControl_ulog_2_clicked();

    void on_button_buyLog_clicked();

    void on_button_equipment_clicked();

    void on_button_control_equipment_clicked();

    void on_button_drinks_clicked();

    void on_button_returnControl_clicked();

    void on_button_flush_equipment_clicked();

    void on_button_flush_drink_clicked();

    void on_button_update_equipment_clicked();

    void on_button_update_drink_clicked();

    void on_button_start_clicked();

    void on_button_close_clicked();

    void on_button_addAdmin_clicked();

    void on_button_register_clicked();

    void on_pushButton_clicked();

    void on_button_adv_start_clicked();

    void on_button_adv_stop_clicked();

    void on_button_adv_pre_clicked();

    void on_button_adv_next_clicked();

    void on_pushButton_2_clicked();

    void on_button_advertisement_clicked();

    void on_button_adv_insert_clicked();

    void on_button_adv_delete_clicked();

private:
    Ui::Widget *ui;

    /** 通信实例 */
    communication *service = nullptr;

    /** 数据库实列 */
    Database *db = nullptr;

    /** 当前客户用户名 */
    QString userName;

};

#endif // WIDGET_H
