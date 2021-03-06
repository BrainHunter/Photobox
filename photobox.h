#ifndef PHOTOBOX_H
#define PHOTOBOX_H

#include <QWidget>
#include <QLabel>
#include <QKeyEvent>
#include <QScrollArea>
#include <QtWidgets>

namespace Ui {
class Photobox;
}

class Photobox : public QWidget
{
    Q_OBJECT

public:
    explicit Photobox(QWidget *parent = 0);
    ~Photobox();
    void keyPressEvent(QKeyEvent *event);
    void resizeEvent(QResizeEvent *event);

private slots:
    void on_pushButton_clicked();

    void on_browseButton_clicked();

    void updateEvent();
    void updateEventTest(QString string);

    void on_saveButton_clicked();

    void on_loadButton_clicked();

private:
    Ui::Photobox *ui;
    void setGuiVisible(bool value);
    QString settingsFile;
    void loadSettings();
    void saveSettings();

    // Display
    QImage image;
    QLabel *imageLabel;
    QScrollArea *scrollArea;
    bool loadFile(const QString &fileName);
    void setImage(const QImage &newImage);
    bool loadFile(int num);
    void resizeScrollArea(QSize windowSize);

    // FileList
    QStringList* imageFileList;
    bool checkForNewImages(QString path);

    //Processing
    QTimer* timer;
    int slideShowCounter = 0;
    QFileSystemWatcher* watcher;


};

#endif // PHOTOBOX_H
