#include "languagewidgets.h"
#include <QVBoxLayout>

LanguageWidgets::LanguageWidgets(QWidget *parent) : BaseWidget(parent)
{
    setObjectName("LanguageWidgets");
    setStyleSheet("#LanguageWidgets{background:rgb(33,36,43)}");

    initData();
    initLayout();
}

void LanguageWidgets::initData()
{
    m_languageForm = new LanguageForm(this);
}

void LanguageWidgets::initLayout()
{
    QVBoxLayout *vmainlyout = new QVBoxLayout;
    vmainlyout->addSpacing(30);
    vmainlyout->addWidget(m_languageForm);
    vmainlyout->addSpacing(20);
    vmainlyout->addStretch(0);
    vmainlyout->setContentsMargins(0,0,0,0);

    // Set layout in middle.
    QHBoxLayout *hmainlyout = new QHBoxLayout;
    hmainlyout->addStretch(1);
    hmainlyout->addLayout(vmainlyout,4);
    hmainlyout->addStretch(1);
    setLayout(hmainlyout);
}
