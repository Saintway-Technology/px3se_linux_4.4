#include "updaterwidget.h"
#include "ui_updaterwidget.h"
#include <QFileDialog>
#include <QDebug>
#include <QProcess>
#include <QCryptographicHash>

#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <linux/watchdog.h>

#include <unistd.h>
#include <sys/ioctl.h>
#include <stropts.h>
#include "constant.h"
#include "language/language.h"

int vendor_storage_write(int buf_size, uint8 *buf, uint16 vendor_id)
{
    int ret = 0;
    int fd;
    RK_VERDOR_REQ req;

    fd = open(VERDOR_DEVICE, O_RDWR, 0);
    if (fd < 0) {
        printf("vendor_storage open fail, errno = %d\n", errno);
        return -1;
    }
    req.tag = VENDOR_REQ_TAG;
    req.id = vendor_id;
    req.len = buf_size > VENDOR_DATA_SIZE ? VENDOR_DATA_SIZE : buf_size;
    memcpy(req.data, buf, req.len);
    ret = ioctl(fd, VENDOR_WRITE_IO, &req);
    if (ret) {
        printf("vendor write error, ret = %d\n", ret);
        close(fd);
        return -1;
    }
    close(fd);

    return 0;
}

int vendor_storage_read(int buf_size, uint8 *buf, uint16 vendor_id)
{
    int ret = 0;
    int fd;
    RK_VERDOR_REQ req;

    fd = open(VERDOR_DEVICE, O_RDWR, 0);
    if (fd < 0) {
        printf("vendor_storage open fail, errno = %d\n", errno);
        return -1;
    }
    req.tag = VENDOR_REQ_TAG;
    req.id = vendor_id;
    req.len = buf_size > VENDOR_DATA_SIZE ? VENDOR_DATA_SIZE : buf_size;
    ret = ioctl(fd, VENDOR_READ_IO, &req);
    if (ret) {
        printf("vendor read error, ret = %d\n", ret);
        close(fd);
        return -1;
    }
    close(fd);
    memcpy(buf, req.data, req.len);

    return 0;
}


int fw_flag_check(char* path)
{
    int fdfile = 0;
    int ret = 0;
    STRUCT_PART_INFO partition;

    fdfile = open(path, O_RDONLY);
    if (fdfile <= 0) {
        printf("fw_flag_check open %s failed! \n", path);
        perror("open");
        return -1;
    }

    ret = read(fdfile, &partition, sizeof(STRUCT_PART_INFO));
    if (ret <= 0) {
        close(fdfile);
        printf("fw_flag_check read %s failed! \n", path);
        perror("read");
        return -1;
    }
    close(fdfile);

    if (partition.hdr.uiFwTag != RK_PARTITION_TAG) {
        printf("ERROR: Your firmware(%s) is invalid!\n", path);
        return -1;
    }

    return 0;
}

int fw_md5_check()
{
    return 0;
}

int updater_recovery_start(char* path)
{
    int ret = 0;
    UpdaterInfo fwinfo;

    ret = fw_flag_check(path);
    if(ret)
        return ret;

    ret = fw_md5_check();
    if(ret)
        return ret;

    //read fwinfo
    ret = vendor_storage_read(sizeof(UpdaterInfo), (unsigned char*)&fwinfo, VENDOR_UPDATER_ID);
    if(ret)
        return ret;

    printf("Current Path is %s\n", fwinfo.update_path);
    printf("Upadater Path is %s\n", path);

    //write fwinfo
    fwinfo.update_mode = MODE_UPDATER;
    memcpy(fwinfo.update_path, path, strlen(path));

    return vendor_storage_write(sizeof(UpdaterInfo), (unsigned char*)&fwinfo, VENDOR_UPDATER_ID);
}

UpdaterWidget::UpdaterWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::UpdaterWidget)
{
    ui->setupUi(this);

    connect(mainWindow, SIGNAL(retranslateUi()), this, SLOT(retranslateUi()));

    //read fwinfo
    int ret = vendor_storage_read(sizeof(UpdaterInfo), (unsigned char*)&fwinfo, VENDOR_UPDATER_ID);
    if (ret)
        return;

    char version[255] = {0};
    sprintf(version,"%d.%d.%d\n", fwinfo.update_version >> 24 & 0xFF,
            fwinfo.update_version >> 16 & 0xFF, fwinfo.update_version & 0xFF);
    ui->m_update_version_LineEdit->setText(QString(version));
}

void UpdaterWidget::retranslateUi()
{
    ui->retranslateUi(this);
}

UpdaterWidget::~UpdaterWidget()
{
    delete ui;
}

void UpdaterWidget::on_m_updatePushButton_clicked()
{
    qDebug("======== click click");
    file = new QFileInfo("/mnt/sdcard/Firmware.img");
    if (file->exists()) {
        qDebug()<< file->absoluteFilePath();
        QFile theFile(file->absoluteFilePath());
        theFile.open(QIODevice::ReadOnly);
        QByteArray ba = QCryptographicHash::hash(theFile.readAll(), QCryptographicHash::Md5);
        theFile.close();
        qDebug()<<"Found img:" << file->absoluteFilePath() << " md5:" << ba.toHex().constData();

        ui->m_textBrowser->append("Found img:" + file->absoluteFilePath());
        ui->m_textBrowser->append("md5:" + QString(QLatin1String((ba.toHex().constData()))));
    } else {
        qDebug() << file->absoluteFilePath() << " not found";
        ui->m_textBrowser->append(file->absoluteFilePath() + " not found");
        file = new QFileInfo("/mnt/udisk/Firmware.img");
        if (file->exists()) {
            qDebug() << file->absoluteFilePath();
            QFile theFile(file->absoluteFilePath());
            theFile.open(QIODevice::ReadOnly);
            QByteArray ba = QCryptographicHash::hash(theFile.readAll(), QCryptographicHash::Md5);
            theFile.close();
            qDebug() << ba.toHex().constData();
            qDebug() << "Found img:" << file->absoluteFilePath() << ba.toHex().constData();

            ui->m_textBrowser->append("Found img:" + file->absoluteFilePath());
            ui->m_textBrowser->append("md5:" + QString(QLatin1String((ba.toHex().constData()))));
        } else {
            qDebug() << file->absoluteFilePath() << " not found";
            ui->m_textBrowser->append(file->absoluteFilePath() + " not found");
            return;
        }
    }

    char *path = file->absoluteFilePath().toLatin1().data();
    int ret = fw_flag_check(path);
    if (ret) {
        qDebug() << "fw_flag_check faild";
        ui->m_textBrowser->append("fw_flag_check faild");
        return;
    } else {
        qDebug()<<"fw_flag_check ok";
        ui->m_textBrowser->append("fw_flag_check ok");
    }

    ret = fw_md5_check();
    if (ret) {
        qDebug() << "fw_md5_check faild";
        ui->m_textBrowser->append("fw_md5_check faild");
    } else {
        qDebug()<<"fw_md5_check ok";
        ui->m_textBrowser->append("fw_md5_check ok");
    }

    printf("Current Path is %s\n", fwinfo.update_path);
    printf("Upadater Path is %s\n", path);

    //write fwinfo
    fwinfo.update_mode = MODE_UPDATER;
    memcpy(fwinfo.update_path, path, strlen(path));

    ret = vendor_storage_write(sizeof(UpdaterInfo), (unsigned char*)&fwinfo, VENDOR_UPDATER_ID);

    if (ret) {
        qDebug() << "vendor_storage_write faild";
        ui->m_textBrowser->append("vendor_storage_write faild");
    } else {
        qDebug() << "vendor_storage_write ok";
        ui->m_textBrowser->append("vendor_storage_write ok");
        qDebug() << "update_version:" << fwinfo.update_version << " update_mode:" << fwinfo.update_mode
                 << " update_path:" << fwinfo.update_path;
        qDebug() << "begin reboot";
        ui->m_textBrowser->append("begin reboot");
        QProcess::execute("reboot");
    }
}
