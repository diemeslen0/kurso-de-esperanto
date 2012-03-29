/*
    Kurso de Esperanto - a free multimedia course to teach yourself the
    international language.
    Copyright (C) 2000-2012 Carlos Alberto Alves Pereira
    karlo@kurso.com.br

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License version 3 as
    published by the Free Software Foundation.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
    - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    Kurso de Esperanto - multrimeda komputila kurso por memlernado de
    la internacia lingvo.
    Kopirajto (C) 2000-2012 Carlos Alberto Alves Pereira
    karlo@kurso.com.br

    Tiu ĉi programo estas libera softvaro: vi povas ĝin redisdoni aŭ
    ĝin modifi laŭ la kondiĉoj de GNU-a Ĝenerala Publika Uzpermeso versio 3
    kiel publikigita de Free Software Foundation.

    Tiu ĉi programo estas disdonita kun la espero, ke ĝi estos utila,
    sed sen ia garantio; eĉ sen la implica garantio de komercebleco aŭ
    taŭgeco por specifa celo. Vidu la GNU-an Ĝeneralan Publikan Uzpermeson
    por pliaj detaloj.

    Vi devus esti ricevinta kopion de la GNU-a Ĝenerala Publika Uzpermeso
    kune kun ĉi tiu programo. Se ne, iru al <http://www.gnu.org/licenses/>.
*/


#include "kurso.h"
#include "ui_kurso.h"
#include <QtGui>
#include <QtCore>
#include <funkcioj.h>
#include <lingvoelekto.h>
#include <klakbutono.h>


kurso::kurso(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::kurso)
{
    randomize();
    ui->setupUi(this);
    QIcon kurso_ikono(":kurso4.ico");
    QApplication::setWindowIcon(kurso_ikono);
    ui->prog_printilo->setVisible(false);

    ludilo =  Phonon::createPlayer(Phonon::MusicCategory,
                                   Phonon::MediaSource());

    alteco_originala = this->minimumHeight();
    largheco_originala = this->minimumWidth();
    int alteco = QApplication::desktop()->height();
    int largheco = QApplication::desktop()->width();
    this->setMaximumSize(largheco,alteco);

    komencakonfiguro();
    Traduku();
    Konfiguru();

    // Sxargxas la respondon al la skribaj ekzercoj
    int i;
    QList<QTabWidget *> chiuj_tab = ui->Paghujo->findChildren<QTabWidget *>();
    for (i=0; i < chiuj_tab.count(); i++)
    {
        if ( (chiuj_tab.at(i)->objectName().left(3) == "Lec") || (chiuj_tab.at(i)->objectName() == "Fina_Ekzameno"))
            SxargxasEk(chiuj_tab.at(i), false);
    }
    // fino de la sxargxado

    // Por centrigi la fenestron
    QRect frect = frameGeometry();
    frect.moveCenter(QDesktopWidget().availableGeometry().center());
    move(frect.topLeft());


    Stilo(); // Shanghas la stilon de la lecionoj


    ui->Paghujo->setCurrentIndex(0); // iras al la komenca pagho


}

kurso::~kurso()
{
    delete ui;
}


void kurso::on_Hejmo_clicked()
{
    QList<QTabWidget *> chiuj_tab =  ui->Paghujo->currentWidget()->findChildren<QTabWidget *>();
    if (chiuj_tab.count() > 0)
        if (chiuj_tab[0]->objectName().left(3) == "Lec")
        {
            LastaLeciono = ui->Paghujo->currentIndex();
            LastaPagxo = chiuj_tab[0]->currentIndex();
        }

    ui->Paghujo->setCurrentIndex(0);
    ui->Hejmo->setEnabled(false);
    ui->printilo->setEnabled(false);
    ui->Sendas->setEnabled(false);
}

void kurso::komencakonfiguro()
{
    QSettings Konfiguro(QSettings::IniFormat, QSettings::UserScope,
                        "Esperanto", "Kurso4");
    Konfiguro.setIniCodec("UTF-8");
    Konfiguro.beginGroup("Gxenerala");
    sLingvo = Konfiguro.value("Lingvo", "").toString();
    sTradukoDosiero = Konfiguro.value("Dosiero", "").toString();
    if (sLingvo == "" || sTradukoDosiero == "")
    {
        lingvoelekto nova;
        nova.exec();
    }
    sTiparo     = Konfiguro.value("Tiparo", "DejaVu Sans").toString();
    lCxapeligo  = Konfiguro.value("Cxapeligo", true).toBool();
    iSistemo    = Konfiguro.value("Sistemo", 0).toInt();
    LastaLeciono = Konfiguro.value("LastaLeciono", 1).toInt();
    LastaPagxo  = Konfiguro.value("LastaPagxo", 0).toInt();
    lSxaltilo   = Konfiguro.value("Sxaltilo", true).toBool();
    lNoteto     = Konfiguro.value("Noteto", true).toBool();
    iLiterGrando= Konfiguro.value("LiterGrando", 13).toInt();

}

void kurso::Traduku()
{
    QFile fTraduko(Loko + "tradukoj/" + sTradukoDosiero);
    if (! fTraduko.exists())
    {
        QMessageBox msgBox(QApplication::activeWindow());
        QFile fTraduko1(Loko + "tradukoj/angla.trd");
        if (fTraduko1.exists())
        {
            msgBox.setText("The translation file " + sTradukoDosiero  + " could not be found. English translation will be used as default.");
            msgBox.setIcon(QMessageBox::Warning);
            msgBox.exec();
            sTradukoDosiero = "angla.trd";
            sLingvo = "English (Angla)";
        }
        else
        {
            msgBox.setText("The translation file " + sTradukoDosiero + " could not be found. English translation could not be found either. \nPlease check the path or reinstall Kurso. \nProgram will quit now.");
            msgBox.setIcon(QMessageBox::Critical);
            msgBox.exec();
            NeEkzistasTraduko = true;
            exit(1);
        }
    }

    QSettings Tradukado(Loko + "tradukoj/" + sTradukoDosiero, QSettings::IniFormat);
    Tradukado.setIniCodec("UTF-8");

    Tradukado.beginGroup("Traduko");
    sTradukinto = Tradukado.value("Tradukinto", "").toString();
    Tradukado.endGroup();

    Tradukado.beginGroup("Mesagxoj");
    smRekomencu = Tradukado.value("smRekomencu", "Ne Tradukita").toString();
    smGratulon  = Tradukado.value("smGratulon",  "Ne Tradukita").toString();
    smDenove =    Tradukado.value("smDenove",    "Ne Tradukita").toString();
    smModifis =   Tradukado.value("smModifis",   "Ne Tradukita").toString();
    smTondujo =   Tradukado.value("smTondujo",   "Ne Tradukita").toString();
    smDevasHaviKor = Tradukado.value("smDevasHaviKor", "Ne Tradukita").toString();
    smDevasSavi = Tradukado.value("smDevasSavi", "Ne Tradukita").toString();
    smKoverto   = Tradukado.value("smKoverto",   "Ne Tradukita").toString();
    smDisketo   = Tradukado.value("smDisketo",   "Ne Tradukita").toString();
    smPordo     = Tradukado.value("smPordo",     "Ne Tradukita").toString();
    smLigilo    = Tradukado.value("smLigilo",    "Ne Tradukita").toString();
    Antauen     = Tradukado.value("Antauen",     "Ne Tradukita").toString();
    Nuligi      = Tradukado.value("Nuligi",      "Ne Tradukita").toString();
    smNeSono    = Tradukado.value("smNeSono",    "Ne Tradukita").toString();
    Tradukado.endGroup();

    QString Direkto = Tradukado.value("Traduko/Direkto", "LTR").toString();
    QString RTL_Marko, LTR_Marko;
    if (Direkto == "LTR")
    {
         QApplication::setLayoutDirection(Qt::LeftToRight);
         RTL_Marko = "";
         LTR_Marko = "";

    }
    else
    {
         QApplication::setLayoutDirection(Qt::RightToLeft);
         LTR_Marko = QString::fromUtf8("\u200E");
         RTL_Marko = QString::fromUtf8("\u200F");
    }


    Tradukado.beginGroup("Menuo");
    ui->KioEstas->setText( Tradukado.value("KioEstas",    "Ne Tradukita").toString() );

    ui->KielUzi->setToolTip(Tradukado.value("KielUzi",    "Ne Tradukita").toString() );

    ui->Konfigurado->setToolTip(Tradukado.value("Konfigurado",    "Ne Tradukita").toString());
    ui->Label_Konfigurado->setText(Tradukado.value("Konfigurado",    "Ne Tradukita").toString() );

    ui->Kreditoj->setText( Tradukado.value("Kreditoj",    "Ne Tradukita").toString() );
    ui->Label_Kreditoj->setText(Tradukado.value("Kreditoj",    "Ne Tradukita").toString() );

    ui->Eliri->setText( Tradukado.value("Eliri",    "Ne Tradukita").toString() );
    ui->KieButono->setText(Tradukado.value("KieButono",    "Ne Tradukita").toString() );
    Tradukado.endGroup();

    //Ligilo.Hint := smLigilo;

    int i;
    Tradukado.beginGroup("Konfig");
    QList<QLabel *> chiuj_label = ui->Pagho_Konfigurado->findChildren<QLabel *>();
    QString Tradukota, Tradukita;
    for (i = 0; i < chiuj_label.count(); i++)
    {
        if ( chiuj_label[i]->objectName().left(6) == "Konfig" )
        {
            /* REVIZII CHI TIUN PARTON KIAM ESTOS LINGVO RTL
            if (Direkto == "LTR")
                chiuj_label[i]->setAlignment(Qt::AlignLeft | Qt::AlignVCenter | Qt::AlignAbsolute);
            else
                chiuj_label[i]->setAlignment(Qt::AlignRight | Qt::AlignVCenter | Qt::AlignAbsolute);
            */

            Tradukota = chiuj_label[i]->objectName().mid(7);
            Tradukita = Tradukado.value(Tradukota, "Ne Tradukita").toString();
            chiuj_label[i]->setText(LTR_Marko + Tradukita);
        }
    }
    ui->radioButton->setText(ui->radioButton->text() + LTR_Marko);
    ui->radioButton_2->setText(ui->radioButton_2->text() + LTR_Marko);
    ui->radioButton_3->setText(ui->radioButton_3->text() + LTR_Marko);
    Tradukado.endGroup();


    Tradukado.beginGroup("Menuo4");
    chiuj_label = ui->Kredit->findChildren<QLabel *>();
    for (i = 0; i < chiuj_label.count(); i++)
    {
        if ( chiuj_label[i]->objectName().left(6) == "Kredit" )
        {
            /* REVIZII CHI TIUN PARTON KIAM ESTOS LINGVO RTL
            if (Direkto == "LTR")
                chiuj_label[i]->setAlignment(Qt::AlignLeft | Qt::AlignVCenter | Qt::AlignAbsolute);
            else
                chiuj_label[i]->setAlignment(Qt::AlignRight | Qt::AlignVCenter | Qt::AlignAbsolute);
            */

            Tradukota = chiuj_label[i]->objectName().mid(7);
            chiuj_label[i]->setText( LTR_Marko + Tradukado.value(Tradukota, "Ne Tradukita").toString());
        }
    }
    ui->Tradukinto->setText(sTradukinto);


    for (i=0; i < ui->Paghujo->count(); i++)
    {
        QList<QTabWidget *> chiuj_tab = ui->Paghujo->widget(i)->findChildren<QTabWidget *>();
        if (chiuj_tab.count() > 0)
           if ( (chiuj_tab.at(0)->objectName().left(3) == "Lec") )
               Trad(ui->Paghujo->widget(i), chiuj_tab.at(0)->objectName());
    }


}

void kurso::Konfiguru()
{

       QSettings Tradukoj(Loko + "tradukoj/tradukoj.ini", QSettings::IniFormat);
       Tradukoj.setIniCodec("UTF-8");
       LingvoListo = Tradukoj.childGroups();

       for (int i = 0; i < LingvoListo.count(); i++)
       {
           ui->Lingvo->addItem(Tradukoj.value(LingvoListo[i]+"/Lingvo", "English (Angla)").toString());
       }
       int trovita = ui->Lingvo->findText(sLingvo);
       ui->Lingvo->setCurrentIndex(trovita);

       ui->label_120->setText(QString::fromUtf8("eĥoŝanĝo ĉiuĵaŭda - ")  + sLingvo);

       QFont novaTiparo(sTiparo);

       //ui->centralWidget->setFont(novaTiparo);
       ui->Tiparo->setCurrentFont(novaTiparo);
       ui->LiterGrando->setValue(iLiterGrando);

       if (iSistemo == 0)
           ui->radioButton->setChecked(true);
       else if (iSistemo == 1)
           ui->radioButton_2->setChecked(true);
       else
           ui->radioButton_3->setChecked(true);

       ui->Cxapeligo->setChecked(lCxapeligo);
       ui->Parolo->setChecked(lSxaltilo);
       ui->Noteto->setChecked(lNoteto);
}


void kurso::on_Sendas_clicked()
{
    QList<QTabWidget *> chiuj_tab =  ui->Paghujo->currentWidget()->findChildren<QTabWidget *>();
    if (chiuj_tab.count() > 0)
        Sendas(chiuj_tab[0]->currentWidget(), chiuj_tab[0]->currentIndex() + 1);
}


void kurso::on_printilo_clicked()
{
    QList<QTabWidget *> chiuj_tab =  ui->Paghujo->currentWidget()->findChildren<QTabWidget *>();
    if (chiuj_tab.count() > 0)
        Sendas_Printilo(chiuj_tab[0], "Leciono " + chiuj_tab[0]->objectName().mid(3,2), ui->prog_printilo);
}

void kurso::on_Cxapeligo_toggled(bool checked)
{
    lCxapeligo = checked;
    ui->checkBox->setChecked(checked);
}


void kurso::on_radioButton_clicked(bool checked)
{
    if (checked)
        iSistemo = 0;
}

void kurso::on_radioButton_2_clicked(bool checked)
{
    if (checked)
        iSistemo = 1;
}

void kurso::on_radioButton_3_clicked(bool checked)
{
    if (checked)
        iSistemo = 2;
}

void kurso::on_Lingvo_activated(int index)
{
    QSettings Tradukoj(Loko + "tradukoj/tradukoj.ini", QSettings::IniFormat);
    Tradukoj.setIniCodec("UTF-8");
    sLingvo = ui->Lingvo->itemText(index);
    sTradukoDosiero = Tradukoj.value(LingvoListo[index] + "/Dosiero", "NeValida").toString();
    ui->label_120->setText(QString::fromUtf8("eĥoŝanĝo ĉiuĵaŭda - ")  + sLingvo);
    Traduku();
}

void kurso::SavasKonfig()
{
    QSettings Konfiguro(QSettings::IniFormat, QSettings::UserScope,
                        "Esperanto", "Kurso4");
    Konfiguro.setIniCodec("UTF-8");
    Konfiguro.beginGroup("Gxenerala");
    Konfiguro.setValue("Lingvo", sLingvo);
    Konfiguro.setValue("Tiparo", ui->Tiparo->currentText());
    Konfiguro.setValue("Dosiero", sTradukoDosiero);
    Konfiguro.setValue("Cxapeligo", lCxapeligo);
    Konfiguro.setValue("Sistemo", iSistemo);
    Konfiguro.setValue("LastaLeciono", LastaLeciono);
    Konfiguro.setValue("LastaPagxo", LastaPagxo);
    Konfiguro.setValue("Sxaltilo", lSxaltilo);
    Konfiguro.setValue("Noteto", lNoteto);
    Konfiguro.setValue("LiterGrando", ui->LiterGrando->value());
    Konfiguro.endGroup();
    Konfiguro.sync();

}

void kurso::closeEvent(QCloseEvent *event)
{
    Adiau();
    int i;

    QList<QTabWidget *> chiuj_tab = ui->Paghujo->findChildren<QTabWidget *>();

    for (i=0; i < chiuj_tab.count(); i++)
    {
        if ( (chiuj_tab.at(i)->objectName().left(3) == "Lec") || (chiuj_tab.at(i)->objectName() == "Fina_Ekzameno"))
            SxargxasEk(chiuj_tab.at(i), true);
    }


    SavasKonfig();

    QString Provizorujo = QDir::tempPath();
    if (Provizorujo.right(1) != "/")
        Provizorujo.append("/");


    // KODO POR FORIGI LA PORTEMPAJN PRONONCAJN DOSIEROJN
    for (i = 0; i < 4; i++)
        QFile::remove(Provizorujo + "prononco" + QString::number(i) + ".raw");



    if (lSxaltilo)
        Pauzu(2); //pauzo necesa por ke la mp3-dosiero estu ludita ghis la fino.

    // QFontDatabase::removeAllApplicationFonts();

    event->accept();
}

void kurso::on_KieButono_clicked()
{
    ui->Paghujo->setCurrentIndex(LastaLeciono);
    QList<QTabWidget *> chiuj_tab =  ui->Paghujo->currentWidget()->findChildren<QTabWidget *>();

    if (chiuj_tab.count() > 0)
        if (chiuj_tab[0]->objectName().left(3) == "Lec")
        {
            if (chiuj_tab[0]->objectName().left(5) == "Lec08")
                EkzercoAkuzativoTrad = "0802";
            if (chiuj_tab[0]->objectName().left(5) == "Lec02")
                EkzercoAkuzativoTrad = "0204";
            if (chiuj_tab[0]->objectName().left(5) == "Lec10")
                EkzercoAkuzativoTrad = "1001";
            if (chiuj_tab[0]->objectName().left(5) == "Lec12")
                EkzercoAkuzativoTrad = "1202";
            if (chiuj_tab[0]->objectName().left(5) == "Lec06")
                EkzercoAkuzativoTrad = "0605";
            chiuj_tab[0]->setCurrentIndex(LastaPagxo);
        }

    ui->Hejmo->setEnabled(true);
    ui->printilo->setEnabled(true);
}

void kurso::Stilo()
{
    QString Loko;
    if (QApplication::applicationDirPath().contains("Debug"))
        Loko = "R:/kurso4/";
    else
    {
        Loko = ":";
        ui->butono_stilo->setVisible(false);
    }



    QFile file5(Loko + "stiloj/kurso.qss");
    file5.open(QFile::ReadOnly);
    QString kurso = QLatin1String(file5.readAll());

    this->setUpdatesEnabled(false);
    ui->centralWidget->setStyleSheet(kurso);
    int i;
    QString pagho, stilo;

    for (i = 0; i < ui->Paghujo->count(); i++)
    {
        if  (ui->Paghujo->widget(i)->objectName().right(2).toInt() > 0)
        {
            pagho = ui->Paghujo->widget(i)->objectName().right(2);
            QFile file1(Loko + "stiloj/lec" + pagho + ".qss");
            file1.open(QFile::ReadOnly);
            stilo = QLatin1String(file1.readAll());
            ui->Paghujo->widget(i)->setStyleSheet(stilo);

        }
    }
    this->setUpdatesEnabled(true);


}

void kurso::on_Tiparo_currentIndexChanged(const QString &arg1)
{
    this->setUpdatesEnabled(false);
    //this->setStyleSheet("font-family: \"" + arg1 + "\"; font-size: " + QString::number(qRound(ui->LiterGrando->value() * 1.333333)) + "px;");
    int alteco = QApplication::desktop()->height();
    int largheco = QApplication::desktop()->width();
    int kroma_alteco = ( ui->LiterGrando->value() - 14 ) * 15;
    int kroma_largheco = ( ui->LiterGrando->value() - 14 ) * 24;
    if (kroma_alteco > 0)
    {
        if ( ( (alteco_originala + kroma_alteco) < alteco)  && ( (largheco_originala + kroma_largheco) < largheco) )
        {
            this->setMinimumSize(largheco_originala + kroma_largheco, alteco_originala + kroma_alteco);
            this->resize(1,1);
        }
    }
    else
        if (this->minimumHeight() > alteco_originala)
        {
            this->setMinimumSize(largheco_originala,alteco_originala);
            this->resize(1,1);
        }

    this->setUpdatesEnabled(true);

    this->setStyleSheet("font-family: \"" + arg1 + "\"; font-size: " + QString::number(ui->LiterGrando->value()) + "px;");

}

/* CHI TIU PARTO ENIROS EN ESTONTA VERSIO
void kurso::on_BildoLudo_clicked()
{
    QList<QWidget *> chiuj_paghoj = ui->Paghujo->findChildren<QWidget *>("Pagho_BildoLudo");
    if (chiuj_paghoj.size() > 0)
        ui->Paghujo->setCurrentWidget(chiuj_paghoj.at(0));
    ui->Hejmo->setEnabled(true);
}
*/

void kurso::on_Konfigurado_clicked()
{
    QList<QTabWidget *> chiuj_tab =  ui->Paghujo->currentWidget()->findChildren<QTabWidget *>();
    if (chiuj_tab.count() > 0)
        if (chiuj_tab[0]->objectName().left(3) == "Lec")
        {
            LastaLeciono = ui->Paghujo->currentIndex();
            LastaPagxo = chiuj_tab[0]->currentIndex();
        }

    QList<QWidget *> chiuj_paghoj = ui->Paghujo->findChildren<QWidget *>("Pagho_Konfigurado");
    if (chiuj_paghoj.size() > 0)
        ui->Paghujo->setCurrentWidget(chiuj_paghoj.at(0));
    ui->Hejmo->setEnabled(true);

}

void kurso::on_KioEstas_clicked()
{
    QDesktopServices::openUrl(QUrl(helpdosiero("kio"), QUrl::TolerantMode));

}

void kurso::on_KielUzi_clicked()
{

    QDesktopServices::openUrl(QUrl(helpdosiero("kiel"), QUrl::TolerantMode));
}

void kurso::on_Kreditoj_clicked()
{
    QList<QWidget *> chiuj_paghoj = ui->Paghujo->findChildren<QWidget *>("Pagho_Kreditoj");
    if (chiuj_paghoj.size() > 0)
    {
        ui->Paghujo->setCurrentWidget(chiuj_paghoj.at(0));
        ui->Kredit->setCurrentIndex(0);
        ui->Hejmo->setEnabled(true);
    }

}

void kurso::on_Parolo_toggled(bool checked)
{
    lSxaltilo = checked;
}

void kurso::on_Noteto_toggled(bool checked)
{
    lNoteto = checked;
}



void kurso::on_LiterGrando_valueChanged(int arg1)
{
    on_Tiparo_currentIndexChanged(ui->Tiparo->currentText());
}


void kurso::ShanghasPaghon(QString Nomo)
{
    QList<QWidget *> chiuj_paghoj = ui->Paghujo->findChildren<QWidget *>(Nomo);
    if (chiuj_paghoj.size() > 0)
    {
        QList<QTabWidget *> chiuj_tab = chiuj_paghoj.at(0)->findChildren<QTabWidget *>();
        if (chiuj_tab.count() > 0)
        {
            ui->Paghujo->setCurrentWidget(chiuj_paghoj.at(0));
            chiuj_tab.at(0)->setCurrentIndex(0);
            ui->Hejmo->setEnabled(true);
            ui->printilo->setEnabled(true);
        }
    }
}

void kurso::on_Lec01_currentChanged(QWidget *arg1)
{
    ui->Sendas->setEnabled((arg1->objectName().left(6) == "Sendas"));
}

void kurso::on_Lec02_currentChanged(QWidget *arg1)
{
    ui->Sendas->setEnabled((arg1->objectName().left(6) == "Sendas"));
}

void kurso::on_Lec03_currentChanged(QWidget *arg1)
{
    ui->Sendas->setEnabled((arg1->objectName().left(6) == "Sendas"));
}

void kurso::on_Lec04_currentChanged(QWidget *arg1)
{
    ui->Sendas->setEnabled((arg1->objectName().left(6) == "Sendas"));
}

void kurso::on_Lec05_currentChanged(QWidget *arg1)
{
    ui->Sendas->setEnabled((arg1->objectName().left(6) == "Sendas"));
}

void kurso::on_Lec06_currentChanged(QWidget *arg1)
{
    ui->Sendas->setEnabled((arg1->objectName().left(6) == "Sendas"));
}

void kurso::on_Lec07_currentChanged(QWidget *arg1)
{
    ui->Sendas->setEnabled((arg1->objectName().left(6) == "Sendas"));
}


void kurso::on_Lec08_currentChanged(QWidget *arg1)
{
    ui->Sendas->setEnabled((arg1->objectName().left(6) == "Sendas"));
}

void kurso::on_Lec09_currentChanged(QWidget *arg1)
{
    ui->Sendas->setEnabled((arg1->objectName().left(6) == "Sendas"));
}

void kurso::on_Lec10_currentChanged(QWidget *arg1)
{
    ui->Sendas->setEnabled((arg1->objectName().left(6) == "Sendas"));
}

void kurso::on_Lec11_currentChanged(QWidget *arg1)
{
    ui->Sendas->setEnabled((arg1->objectName().left(6) == "Sendas"));
}

void kurso::on_Lec12_currentChanged(QWidget *arg1)
{
    ui->Sendas->setEnabled((arg1->objectName().left(6) == "Sendas"));
}

void kurso::on_Lec13_currentChanged(QWidget *arg1)
{
    ui->Sendas->setEnabled((arg1->objectName().left(6) == "Sendas"));
}


void kurso::on_Leciono1_clicked()
{
    ShanghasPaghon("Pagho01");
}

void kurso::on_Leciono2_clicked()
{
    EkzercoAkuzativoTrad = "0204";
    ShanghasPaghon("Pagho02");
}

void kurso::on_Leciono3_clicked()
{
    ShanghasPaghon("Pagho03");
}

void kurso::on_Leciono4_clicked()
{
    ShanghasPaghon("Pagho04");
}
void kurso::on_Leciono5_clicked()
{
    ShanghasPaghon("Pagho05");
}
void kurso::on_Leciono6_clicked()
{
    EkzercoAkuzativoTrad = "0605";
    ShanghasPaghon("Pagho06");
}
void kurso::on_Leciono7_clicked()
{
    ShanghasPaghon("Pagho07");
}
void kurso::on_Leciono8_clicked()
{
    EkzercoAkuzativoTrad = "0802";
    ShanghasPaghon("Pagho08");
}
void kurso::on_Leciono9_clicked()
{
    ShanghasPaghon("Pagho09");
}
void kurso::on_Leciono10_clicked()
{
    EkzercoAkuzativoTrad = "1001";
    ShanghasPaghon("Pagho10");
}
void kurso::on_Leciono11_clicked()
{
    ShanghasPaghon("Pagho11");
}
void kurso::on_Leciono12_clicked()
{
    EkzercoAkuzativoTrad = "1202";
    ShanghasPaghon("Pagho12");
}
void kurso::on_Leciono13_clicked()
{
    ShanghasPaghon("Pagho13");
}

void kurso::on_checkBox_clicked(bool checked)
{
    lCxapeligo = checked;
    ui->Cxapeligo->setChecked(checked);
}



void kurso::on_butono_stilo_clicked()
{
    Stilo();
}

/*
void kurso::on_butono_listo_clicked()
{
    ui->Paghujo->setCurrentIndex(17);
    int i, j;

    QList<QTabWidget *> chiuj_tab = ui->Paghujo->findChildren<QTabWidget *>();
    for (i=0; i < chiuj_tab.count(); i++)
    {
        if ( (chiuj_tab.at(i)->objectName().left(3) == "Lec") )
        {
            QList<klakbutono *> chiuj_klak = chiuj_tab.at(i)->findChildren<klakbutono *>();
            QList<QLabel *> chiuj_label = chiuj_tab.at(i)->findChildren<QLabel *>();
            ui->listo->appendPlainText("\n[" + chiuj_tab.at(i)->objectName() + "]");
            for (j = 0; j < chiuj_klak.count(); j++)
            {
                ui->listo->appendPlainText(chiuj_klak.at(j)->objectName() + "= " +  chiuj_klak.at(j)->text());
            }

            for (j = 0; j < chiuj_label.count(); j++)
            {
                if (chiuj_label.at(j)->objectName().left(4) == "Memo")
                    ui->listo->appendPlainText(chiuj_label.at(j)->objectName() + "= " +  chiuj_label.at(j)->text() + "\n");
            }
        }

    }

}
*/

