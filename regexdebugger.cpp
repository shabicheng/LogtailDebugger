#include "regexdebugger.h"
#include "ui_regexdebugger.h"
#include <QRegExp>
#include <QDebug>
#include <QDateTime>

RegexDebugger::RegexDebugger(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::RegexDebugger)
{
    ui->setupUi(this);
    connect(ui->checkButton, SIGNAL(clicked(bool)), this, SLOT(OnStart(bool)));
    ui->regexEdit->setAcceptRichText(false);
    ui->regexEdit->setAcceptRichText(false);
    ui->resultEdit->setReadOnly(true);
    mDefaultLogFormat = ui->logTextEdit->currentCharFormat();
    mDefaultRegFormat = ui->regexEdit->currentCharFormat();
}

RegexDebugger::~RegexDebugger()
{
    delete ui;
}

void RegexDebugger::Performance(QRegExp & regex, QString & log, QString & reg)
{
    qint64 nowTime = QDateTime::currentMSecsSinceEpoch();
    qint64 endTime = 0;
    int i = 0;
    for (; i < 100; ++i)
    {
        if (!regex.exactMatch(log))
        {
            qDebug() << "internal error" << endl;
        }
        endTime = QDateTime::currentMSecsSinceEpoch();
        if (endTime - nowTime >= 1000)
        {
            ++i;
            break;
        }
    }

    ui->resultEdit->setPlainText(QString("[INFO] Success, regex match run %1 times, cost %2 ms\n").arg(i).arg(endTime - nowTime) + ui->resultEdit->toPlainText());

}

bool RegexDebugger::FindValidPrefix(QRegExp &, QString & log, QString & reg)
{
    QList<int> leftList;
    for (int i = 0; i < reg.size(); ++i)
    {
        if (reg[i] == '\\')
        {
            ++i;
            continue;
        }
        if (reg[i] == '(')
        {
            leftList.append(i);
        }
        else if (reg[i] == ')')
        {
            leftList.append(i+1);
        }
    }
    for (int i = leftList.size() - 1; i >= 0; --i)
    {
        if (leftList[i] == 0)
            break;
        QString subReg = reg.left(leftList[i]);
        subReg += ".*";
        QRegExp subRegExp(subReg);
        if (!subRegExp.isValid())
        {
            ui->progressBar->setValue(100);
            ui->resultEdit->setText(QString("[ERROR] Internal error : %1, %2").arg(subReg).arg(subRegExp.errorString()));
            return false;
        }
        if (subRegExp.exactMatch(log) && subRegExp.matchedLength() == log.size())
        {
            ui->progressBar->setValue(100);
            QStringList result = subRegExp.capturedTexts();
            result.removeFirst();
            ui->resultEdit->setText(QString("[ERROR] Regex match error, only sub regex matched.\n    extraction result : ") + result.join(", "));

            // check sub regex matched log index
            subReg = reg.left(leftList[i]);
            QRegExp subR(subReg);
            int matchIndex = subR.indexIn(log);
            if (matchIndex == 0)
            {
                qDebug() << "match sub log :" << log.left(subR.matchedLength()) << endl;
                qDebug() << "match sub regex :" << subReg << endl;

                SetValidIndex(subR.matchedLength(), leftList[i]);
            }

            return true;
        }
    }
    ui->progressBar->setValue(100);
    ui->resultEdit->setText(QString("[ERROR] Regex match error, no sub regex matched"));
    return false;
}

void RegexDebugger::ClearFormat()
{
    ui->logTextEdit->setCurrentCharFormat(mDefaultLogFormat);
    ui->regexEdit->setCurrentCharFormat(mDefaultRegFormat);
    {
        QTextCursor defaultCursor(ui->logTextEdit->document());
        defaultCursor.setPosition(0, QTextCursor::MoveAnchor);
        defaultCursor.movePosition(QTextCursor::End, QTextCursor::KeepAnchor);
        defaultCursor.setCharFormat(mDefaultLogFormat);
    }
    {
        QTextCursor defaultCursor(ui->regexEdit->document());
        defaultCursor.setPosition(0, QTextCursor::MoveAnchor);
        defaultCursor.movePosition(QTextCursor::End, QTextCursor::KeepAnchor);
        defaultCursor.setCharFormat(mDefaultRegFormat);
    }
}

void RegexDebugger::SetValidIndex(int logIndex, int regIndex)
{
    {
        QTextDocument * document = ui->logTextEdit->document();
        QTextCursor highlightCursor(document);
        ///QTextCursor cursor(document);
        ///cursor.beginEditBlock();
        QTextCharFormat plainFormat(highlightCursor.charFormat());
        QTextCharFormat colorFormat = plainFormat;
        colorFormat.setBackground(Qt::green);
        colorFormat.setForeground(Qt::blue);
        colorFormat.setFontUnderline(true);
        highlightCursor.setPosition(0, QTextCursor::MoveAnchor);
        highlightCursor.movePosition(QTextCursor::Right, QTextCursor::KeepAnchor, logIndex);
        highlightCursor.mergeCharFormat(colorFormat);
        ///cursor.endEditBlock();
    }
    {
        QTextDocument * document = ui->regexEdit->document();
        QTextCursor highlightCursor(document);
        //QTextCursor cursor(document);
        //cursor.beginEditBlock();
        QTextCharFormat plainFormat(highlightCursor.charFormat());
        QTextCharFormat colorFormat = plainFormat;
        colorFormat.setBackground(Qt::green);
        colorFormat.setForeground(Qt::blue);
        colorFormat.setFontUnderline(true);
        highlightCursor.setPosition(0, QTextCursor::MoveAnchor);
        highlightCursor.movePosition(QTextCursor::Right, QTextCursor::KeepAnchor, regIndex);
        highlightCursor.mergeCharFormat(colorFormat);
        //cursor.endEditBlock();
    }
}



void RegexDebugger::OnStart(bool)
{
    ClearFormat();
    //ui->resultEdit->setPlainText("");
    ui->progressBar->setValue(0);
    QString log = ui->logTextEdit->toPlainText();
    QString reg = ui->regexEdit->toPlainText();
    qDebug() << "log :" << log << "\n reg :" << reg << endl;
    QRegExp regExp(reg);
    if (!regExp.isValid())
    {
        ui->progressBar->setValue(100);
        ui->resultEdit->setText(QString("Invalid regex : error %1").arg(regExp.errorString()));
        this->repaint();
        return;
    }
    if (log.size() == 0)
    {
        ui->progressBar->setValue(100);
        ui->resultEdit->setText(QString("Empty log context"));
        this->repaint();
        return;
    }
    if (regExp.exactMatch(log) && regExp.matchedLength() == log.size())
    {
        ui->progressBar->setValue(100);
        QStringList result = regExp.capturedTexts();
        result.removeFirst();
        ui->resultEdit->setText(QString("   extraction result : ") + result.join(", "));
        SetValidIndex(log.size(), reg.size());
        Performance(regExp, log, reg);
        this->repaint();
        return;
    }
    if (regExp.indexIn(log) == 0)
    {
        ui->progressBar->setValue(100);
        QStringList result = regExp.capturedTexts();
        result.removeFirst();
        ui->resultEdit->setText(QString("[ERROR] Regex match error, only sub regex matched\n    extraction result : ") + result.join(", "));
        SetValidIndex(regExp.matchedLength(), reg.size());
        this->repaint();
        return;
    }
    qDebug() << "unmatch" << endl;

    FindValidPrefix(regExp, log, reg);
    this->repaint();
}
