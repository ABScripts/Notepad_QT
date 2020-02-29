#include "findmatcheswindow.h"
#include "ui_findmatcheswindow.h"

#include <QRegularExpressionMatch>
#include <QRegularExpression>
#include <QMessageBox>
#include <QTextEdit>
#include <QDebug>

FindMatchesWindow::FindMatchesWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::FindMatchesWindow)
{
    ui->setupUi(this);

    connect(ui->cancelButton, &QPushButton::clicked, this, &FindMatchesWindow::close);
}

FindMatchesWindow::~FindMatchesWindow()
{
    delete ui;
}

void FindMatchesWindow::on_pushButton_clicked()
{
    // first step -> check text in the textEdit
    QString textToFind = ui->textEdit->toPlainText();
    if ( textToFind.isEmpty() )
    {
        QMessageBox::warning(this, "Notepad", "Fill in the search window!", QMessageBox::Ok);
    }
    else
    {
        // constructing the regular exspression
        QRegularExpression re(textToFind);
        if ( ui->registerFlag->isChecked() )
        {
            re.setPatternOptions(QRegularExpression::CaseInsensitiveOption);
        }

        if ( ui->searchBackward->isChecked() ) // when searching backward
        {
            int index = text.lastIndexOf(re, currentSearchPosition, nullptr);
            if (currentSearchPosition == 0 || index == -1) // if we already on the last position and still try searching ahead
            {
                QMessageBox::warning(this, "Notepad", "Matches were not found!", QMessageBox::Ok);
            }
            else
            {
                currentSearchPosition = index - textToFind.length();
                if (currentSearchPosition < 1)
                {
                    currentSearchPosition = 0;
                }
                emit selectText(index, index+textToFind.length());
            }
        }
        else                                   // when searching forward
        {
            int index = text.indexOf(re, currentSearchPosition, nullptr);
            if (index == -1)
            {
                QMessageBox::warning(this, "Notepad", "Matches were not found!", QMessageBox::Ok);
            }
            else
            {
                currentSearchPosition = index + textToFind.length();
                emit selectText(index, index+textToFind.length());
            }
        }
    }
}
