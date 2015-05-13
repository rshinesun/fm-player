#ifndef START_H
#define START_H

#include <QWidget>
#include <QMediaPlayer>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <song.h>
namespace Ui {
class Start;
class Song;
}

class Start : public QWidget
{
    Q_OBJECT

public:
    explicit Start(QWidget *parent = 0);
    ~Start();
    void mousePressEvent(QMouseEvent *);
    void mouseReleaseEvent(QMouseEvent *);
    void mouseMoveEvent(QMouseEvent *);
    void changeOpacity();
    void UpdateTime(qint64 time);
    void resolve_lrc(const QString &);
    QString FindLyric(QString);
    void Getlyric(QString);
    void GetSongs();
    void FindSongsList(QString);
    void _Play();

private slots:
    void on_horizontalSlider_sliderMoved(int);
    void replyFinished(QNetworkReply *reply);
    void positionChanged(qint64);
    void durationChanged(qint64);
    void setPosition(int);
    QString CovertTime(qint64);
    void on_pushButton_clicked();
    void on_horizontalSlider_2_actionTriggered();
    void on_pushButton_2_clicked();
    void replyFinished2(QNetworkReply *reply);

private:
    Ui::Start *ui;
    bool	m_MousePressed;
    QPoint	m_MousePos;
    QPoint	m_WindowPos;
    QNetworkAccessManager *manager;
    QNetworkAccessManager *manager2;
    QMediaPlayer* player;
    bool DoingPlay;
    QMap<qint64, QString> lrc_map;
    qint64  allduration;
    QString GetDATA;
    QString GetDATA2;
    bool GetEnd;
    int NowPlay;
    Song playlist[5];

};

#endif // START_H
