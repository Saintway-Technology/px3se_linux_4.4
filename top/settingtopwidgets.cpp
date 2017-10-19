#include "settingtopwidgets.h"
#include "global_value.h"

SettingTopWidgets::SettingTopWidgets(QWidget *parent):BaseWidget(parent)
{
    // Set background color.
    setObjectName("SettingTopWidgets");
    setStyleSheet("#SettingTopWidgets{background:rgb(56,58,66)}");

    initLayout();
    initConnection();
}

void SettingTopWidgets::initLayout()
{
    QHBoxLayout *hmainyout=new QHBoxLayout;

    m_btnReturn = new FourStateButton(return_resource_str,this);
    m_btnReturn->setFixedSize(return_icon_width,return_icon_height);

    m_btnIcon=new FlatButton(this);
    m_btnIcon->setFixedSize(top_icon_size,top_icon_size);
    m_btnIcon->setStyleSheet("QPushButton{background:transparent;border-image:url(:/image/setting/setting_icon.png)}");

    titleText=new QLabel(tr("Setting"),this);
    titleText->setAlignment(Qt::AlignCenter);

    QHBoxLayout *lyout1 = new QHBoxLayout;
    lyout1->addWidget(m_btnReturn);
    lyout1->addSpacing(30);
    lyout1->addWidget(m_btnIcon);
    lyout1->addStretch(0);

    hmainyout->addLayout(lyout1,1);
    hmainyout->addWidget(titleText,1);
    hmainyout->addStretch(1);
    hmainyout->setContentsMargins(0,0,0,0);
    setLayout(hmainyout);
}

void SettingTopWidgets::initConnection()
{
    connect(m_btnReturn,SIGNAL(clicked(bool)),this,SIGNAL(returnClicked()));
    connect(mainWindow,SIGNAL(retranslateUi()),this,SLOT(retranslateUi()));
}
void SettingTopWidgets::retranslateUi(){
    titleText->setText(tr("Setting"));
}

SettingTopWidgets::~SettingTopWidgets()
{
}
