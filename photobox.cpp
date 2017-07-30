#include "photobox.h"
#include "ui_photobox.h"

#include <QImageReader>
#include <QtWidgets>

Photobox::Photobox(QWidget *parent) :
    QWidget(parent),
    imageLabel(new QLabel),
    scrollArea(new QScrollArea),
    imageFileList(new QStringList),
    ui(new Ui::Photobox)
{
    ui->setupUi(this);

    settingsFile = qApp->applicationDirPath() + "/photobox.ini";
    loadSettings();
    qInfo() << settingsFile;

    //Label for the Image
    imageLabel->setBackgroundRole(QPalette::Base);
    imageLabel->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    imageLabel->setScaledContents(true);

    scrollArea->setParent(this );
    scrollArea->setBackgroundRole(QPalette::Dark);
    scrollArea->setWidget(imageLabel);
    scrollArea->setVisible(false);
    scrollArea->setWidgetResizable(true);

    timer = new QTimer(this);
    timer->setSingleShot(true);
    connect(timer,SIGNAL(timeout()),this,SLOT(updateEvent()));

    watcher = new QFileSystemWatcher(this);
    connect(watcher,SIGNAL(directoryChanged(QString)),this,SLOT(updateEventTest(QString)));

}

Photobox::~Photobox()
{
    delete ui;
}

void Photobox::keyPressEvent(QKeyEvent *event)
{
    //maybe need this some day:
//    bool ctrl_pressed = false;
//    if( event->modifiers() == Qt::ControlModifier )
//    {
//        ctrl_pressed = true;
//    }

    switch( event->key() )
    {
        case Qt::Key_Escape:
            timer->stop();
            watcher->removePaths(watcher->directories());
            this->showNormal();
            scrollArea->setVisible(false);
            setGuiVisible(true);
            this->setPalette(this->style()->standardPalette());
            break;
        default:
            break;
    }
}

void Photobox::resizeEvent(QResizeEvent *event)
{
    // respect the image ratio for resizing the scrollArea
    //QMessageBox::information(this, QGuiApplication::applicationDisplayName(),
    //                       tr("Size: %1 x %2")
    //                       .arg(event->size().width()).arg(event->size().height()));

    qInfo() << "size: "<< event->size().width() << "x" << event->size().height();

    if(imageLabel->pixmap() != NULL) // if no image is loaded don't do this
    {
        QSize imgSize = imageLabel->pixmap()->size();
        scrollArea->resize(imgSize.scaled(event->size(),Qt::KeepAspectRatio));
        int x = event->size().width() - scrollArea->size().width();
        scrollArea->move(x/2,0);
    }

    QWidget::resizeEvent(event);
}

bool Photobox::loadFile(const QString &fileName)
{
    QImageReader reader(fileName);
    reader.setAutoTransform(true);
    const QImage newImage = reader.read();
    if (newImage.isNull()) {
        QMessageBox::information(this, QGuiApplication::applicationDisplayName(),
                                 tr("Cannot load %1: %2")
                                 .arg(QDir::toNativeSeparators(fileName), reader.errorString()));
        return false;
    }
    setImage(newImage);
    return true;
}

bool Photobox::loadFile(int num)
{
    QImageReader reader(imageFileList->at(num));
    reader.setAutoTransform(true);
    const QImage newImage = reader.read();
    if (newImage.isNull()) {
        // file is not readable / does not exist / something
        qInfo() << "Failed to Load ! removing: "+ imageFileList->at(num);
        imageFileList->removeAt(num);
        return false;
    }
    setImage(newImage);
    return true;
}

void Photobox::setImage(const QImage &newImage)
{
    image = newImage;
    imageLabel->setPixmap(QPixmap::fromImage(image));
    imageLabel->adjustSize();
}


//StartButton starts the "Photobox"
void Photobox::on_pushButton_clicked()
{
    // load Image and start Timer:
    updateEvent();

    // do some GUI Stuff ;-)
    QPalette pal;
    pal.setColor(QPalette::Background, Qt::black);
    this->setPalette(pal);

    //hide all gui elements
    setGuiVisible(false);

    // make the image visible
    scrollArea->setVisible(true);
    // Fullscreen:
    if(ui->fullScreenCheckBox->isChecked())
    {
        this->showFullScreen();
    }

    watcher->addPath(ui->directoryEdit->text());
}

//Browse Button opens a file dialog and sets the new Path in the Edit Box
void Photobox::on_browseButton_clicked()
{
    QUrl newPath = QFileDialog::getExistingDirectory(this, tr("Open Directory"), ui->directoryEdit->text());
    ui->directoryEdit->setText(newPath.toString());
}


// check the directory for new images.
// if new images exist --> add to the imageFileList
// return true if new image added, else false
// note: this function will not remove images from the list.
bool Photobox::checkForNewImages(QString path)
{
    bool ret =false;
    QStringList FilterList;
    FilterList << "*.jpg" << "*.JPG" << "*.BMP" ;

    QDir directory(path);
    QFileInfoList dirList= directory.entryInfoList(FilterList, QDir::NoFilter,QDir::Time);

    while( dirList.length() )
    {
        QString tmp=dirList.last().absoluteFilePath();
        dirList.removeLast();

        if(!imageFileList->contains(tmp))
        {
            ret = true;
            imageFileList->append(tmp);
            qInfo() << "adding to list:" + tmp;

        }
    }
    return ret;
}

void Photobox::updateEventTest(QString string)
{
    qInfo() << "Directory has changed! " + string;
    updateEvent();
}

//
void Photobox::updateEvent()
{
    if(checkForNewImages(ui->directoryEdit->text()))
    {   //got new images!
        //display the newest:

        if(imageFileList->length() != 0) // check if there is any!
        {
            if(loadFile(imageFileList->length()-1))
            {
                //fire timer with review Time:
                timer->start(ui->reviewTimeSpinBox->value()*1000);
            }
            else
            {   // file read failed
                timer->start(100);  // try again in 100ms:
                                    // will not be executed if there is no image in the list
            }
        }
    }
    else
    {   //Slideshow
        bool check = true;

        // only show new one if timer is expired:
        if(timer->isActive()) return;

        do
        {   // get next from list:
            // check for overflow:
            if(slideShowCounter >= imageFileList->length())
            {
                slideShowCounter = 0;
            }

            if(imageFileList->length() != 0) // check if there is any!
            {
                // try to load image
                if(loadFile(slideShowCounter))
                {
                    check=false;
                    //fire timer with review Time:
                    timer->start(ui->slideShowTimeSpinBox->value()*1000);
                }
                else
                {
                    //imageFileList->removeAt(slideShowCounter);

                }
                slideShowCounter++;
            }
            else
            {
                check=false;
            }
        }while(check);
    }
}

void Photobox::setGuiVisible(bool value)
{
    ui->browseButton->setVisible(value);
    ui->directoryEdit->setVisible(value);
    ui->fullScreenCheckBox->setVisible(value);
    ui->label->setVisible(value);
    ui->label_2->setVisible(value);
    ui->label_3->setVisible(value);
    ui->pushButton->setVisible(value);
    ui->reviewTimeSpinBox->setVisible(value);
    ui->slideShowTimeSpinBox->setVisible(value);
    ui->loadButton->setVisible(value);
    ui->saveButton->setVisible(value);
}

void Photobox::loadSettings()
{
     QSettings settings(settingsFile, QSettings::IniFormat);

     // Path:
     QString sPath = settings.value("path", "C:\\Photos").toString();
     if (ui->directoryEdit)
     {
        ui->directoryEdit->setText(sPath);
     }

     //Slidehow Time:
     int ssTime= settings.value("slideshowTime", "5").toInt();
     if (ui->slideShowTimeSpinBox)
     {
        ui->slideShowTimeSpinBox->setValue(ssTime);
     }

     //review Time
     int rTime= settings.value("reviewTime", "20").toInt();
     if (ui->reviewTimeSpinBox)
     {
        ui->reviewTimeSpinBox->setValue(rTime);
     }

     //Show Fullscreen
     bool showFS = settings.value("showFullscreen", "true").toBool();
     if (ui->fullScreenCheckBox)
     {
        ui->fullScreenCheckBox->setChecked(showFS);
     }

}

void Photobox::saveSettings()
{
     QSettings settings(settingsFile, QSettings::IniFormat);

     // Path:
     settings.setValue("text", ui->directoryEdit->text());
     //Slidehow Time:
     settings.setValue("slideshowTime", ui->slideShowTimeSpinBox->value());
     //review Time
     settings.setValue("reviewTime", ui->reviewTimeSpinBox->value());
     //Show Fullscreen
     settings.setValue("showFullscreen", ui->fullScreenCheckBox->isChecked());

}

void Photobox::on_saveButton_clicked()
{
    saveSettings();
}

void Photobox::on_loadButton_clicked()
{
    loadSettings();
}
