#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "communication.h"
#include "drink.h"
#include "user.h"
#include <QVideoWidget>
#include <QTime>
#include <QMediaPlayer>
#include <QMediaPlaylist>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);

    ~MainWindow();

    void dealDate(QString data);

    void decideCanlog(QString data);

    void decideCanRegister(QString data);

    void decideCanDelete(QString data);

    void load_drinks(QString data);

    void flush_buyWindow(QString data);

    void setOneDrink(int num,int temp);

    void load_user(QString data);

    void setOneUser();

    void decideCanbuy(QString data);

    void sendUserlog(QString name,QString type,QString message);

    void setHot(QString data);

    void op_advertisment(QString data);

    QString MD5(QString password);

    QString get_drink_num(int num,int temp);

public slots:
    void connectSLOT();

    void readDate();

    void on_button_log_clicked();

    void on_button_register_clicked();

    void on_button_returnLog_clicked();

    int change_temp(QString temp);

    int change_equ(QString equ);

    void button_buy_1_clicked();

    void actionBuy();

    void actionSendMessage();

private slots:
//    void on_button_register_2_clicked();

    void button_d_clicked();

    void register_2_clicked();


    void on_comboBox_equipment_currentIndexChanged(int index);

    void on_comboBox_temperature_currentIndexChanged(int index);

    void on_button_buy_2_clicked();

    void on_button_buy_3_clicked();

    void on_button_buy_4_clicked();

    void on_button_buy_5_clicked();

    void on_button_buy_6_clicked();

    void on_button_buy_7_clicked();

    void on_button_buy_8_clicked();

    void on_button_change_log_clicked();

    void on_button_change_returnBuy_clicked();

    void on_change_user_clicked();

    void on_button_admin_clicked();

    void on_button_change_logOff_clicked();

    void on_button_send_message_clicked();

    void on_button_return_clicked();

    void actionRecharge();

    void on_button_recharge_clicked();

    void actionLink();

    void actionDisconnect();

    void on_Button_QR_clicked();

protected:
    //声明
    void mouseDoubleClickEvent(QMouseEvent*);
    void timerEvent( QTimerEvent * event );
    void mousePressEvent(QMouseEvent *event);


private:
    Ui::MainWindow *ui;

    /** 通信实例 */
    communication *client = nullptr;
    /** 本地饮料库 */
    QList<drink*> drinks;
    /** 本地用户信息 */
    User *user;
    /** 播放视频的窗口 */
    QVideoWidget *videoWidget;
    /** 播放器 */
    QMediaPlayer *player;
    /** 播放列表 */
    QMediaPlaylist *playlist;
    /** 计时器 */
    QTime time;
    int time_id;
    int time_id2;
};

#endif // MAINWINDOW_H
