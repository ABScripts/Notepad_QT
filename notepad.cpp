#include "notepad.h"
#include "ui_notepad.h"
#include "findmatcheswindow.h"

#include <QFileDialog>
#include <QMessageBox>
#include <QTextStream>
#include <QColorDialog>
#include <QFontDialog>
#include <QActionGroup>
#include <QDebug>
#include <QLabel>

Notepad::Notepad(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::Notepad)
{
    ui->setupUi(this);

    //TODO: get rid off this old connect style
    // connectors which belong...
    // ...to the file section
    connect(ui->openFile, &QAction::triggered, this, &Notepad::openFileAs);
    connect(ui->saveFileAs, &QAction::triggered, this, &Notepad::saveFileAs);
    connect(ui->saveFile, &QAction::triggered, this, &Notepad::saveFile);
    // ...to the formatting section
    connect(ui->fontWindow, &QAction::triggered, this, &Notepad::changeFont);
    connect(ui->redoAction, &QAction::triggered, [this](){ ui->textEdit->redo(); } );
    connect(ui->undoAction, &QAction::triggered, [this](){ ui->textEdit->undo(); } );
    connect(ui->copyText,   &QAction::triggered, [this](){ ui->textEdit->copy(); });
    connect(ui->cutText,    &QAction::triggered, [this](){ ui->textEdit->cut(); });
    connect(ui->pasteText,  &QAction::triggered, [this](){ ui->textEdit->paste(); });
    connect(ui->fontColor,  &QAction::triggered, this, &Notepad::changeColor);
    connect(ui->backgroundFontColor, &QAction::triggered, this, &Notepad::changeBackgroundColor);
    connect(clipboard, &QClipboard::changed, [this](){ ui->pasteText->setEnabled(true); });
    connect(ui->findText, &QAction::triggered, this, &Notepad::setFindWindow); // <- pop window

    connect(ui->textEdit, &QTextEdit::copyAvailable,
            [this](bool b) { ui->cutText->setEnabled(b);
                             ui->copyText->setEnabled(b); });

    connect(ui->selectAll, &QAction::triggered, [this](){ ui->textEdit->selectAll(); });
    // ...to the view section

    connect(ui->zoomIn, &QAction::triggered, this, &Notepad::setZoom);
    connect(ui->zoomOut, &QAction::triggered, this, &Notepad::setZoom);
    connect(ui->setDefault, &QAction::triggered, this, &Notepad::setZoom);

    //to the additional features section
    connect(ui->safeData, &QAction::triggered, this, &Notepad::onSafeDataMode);

    // These connectors are for tracking redo-undo button`s states
    // as well as state of "whether text in file was changed"
    connect(ui->textEdit, &QTextEdit::redoAvailable,
            [this](bool b){ ui->redoAction->setEnabled(b); });

    connect(ui->textEdit, &QTextEdit::undoAvailable,
            [this](bool b){ ui->undoAction->setEnabled(b); });

    connect(ui->textEdit, &QTextEdit::textChanged, [this](){ mIsTextChanged = true; });


    connect(ui->changeFontFor, &QAction::triggered, this, &Notepad::changeFontFor);
}

Notepad::~Notepad()
{
    delete ui;
}

void Notepad::saveFileAs()
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save file"),
                                                    QDir::homePath(), tr("TextFiles (*.txt *.cpp *.h)"));

    interactWithFile(fileName, QIODevice::WriteOnly, &Notepad::ssaveFile);
}

void Notepad::openFileAs()
{
    // getting the direct path to our file

    if (mIsTextChanged) // if the user have already opened some project and changed some text there
    {

        QMessageBox msgBox;
        msgBox.setText("The document has been modified.");
        msgBox.setInformativeText("Do you want to save your changes?");
        msgBox.setStandardButtons(QMessageBox::Save | QMessageBox::Cancel);
        msgBox.setDefaultButton(QMessageBox::Save);

        int reply = msgBox.exec();

        if (reply == QMessageBox::Save) // switch or if - that`s the question...
        {
            saveFile();
        }
    }

    QString fileName = QFileDialog::getOpenFileName(this, tr("Open file"),  // set parent and title
                                                    QDir::homePath(), tr("TextFiles (*.txt *.cpp *.h)")); // set types of files which could be opened

    interactWithFile(fileName, QIODevice::ReadOnly, &Notepad::sopenFile);
}

void Notepad::interactWithFile(const QString &fileName, QIODevice::OpenModeFlag flag,
                               void (Notepad::*action)(QFile* file)){
    // if user canceled opening or saving file
    if (fileName.isEmpty())
    {
        // then do nothing
        return;
    }

    QFile file(fileName);

    //and check whether it was opened succesfully or not...
    if ( !file.open(flag | QIODevice::Text ) )
        // if it was not - print this kind of a warning message
        QMessageBox::critical(this, "Error!", "The error occured while openinig file!");
    else {
        // running a proper function to save either open the file
        (this->*action)(&file);
        mCurrentFilePath = fileName;
        mIsTextChanged = false;
    }
    file.flush();
    file.close();
}

void Notepad::sopenFile(QFile *file)
{
    QTextStream in(file);
    ui->textEdit->setPlainText(in.readAll());
    //ui->textEdit->setPlainText(in.readLine(0));
    //ui->textEdit->setPlainText(ui->textEdit->toPlainText() + in.readLine(0));
}

void Notepad::ssaveFile(QFile *file)
{
    QTextStream out(file);
    out << ui->textEdit->toPlainText();
}

void Notepad::saveFile() // is used for saving file without opening the dialog window if it`s not nedeed
{
    if ( mCurrentFilePath.isEmpty()) // if this is a new file
    {
        saveFileAs();            // treat it as a new file
    }
    else                         // if this file already exists - just save
    {
        interactWithFile(mCurrentFilePath, QIODevice::WriteOnly, &Notepad::ssaveFile);
    }
}

void Notepad::setZoom()
{
    QString senderName = sender()->objectName();

    if ( senderName == "zoomIn" && mTextEditScale < 100)
    {
        ui->textEdit->zoomIn(1);
        mTextEditScale++;
    } else if ( senderName == "zoomOut" && mTextEditScale > 0)
    {
        ui->textEdit->zoomOut(1);
        mTextEditScale--;
    } else
    {
        while ( mTextEditScale > 0)
        {
            ui->textEdit->zoomOut(1);
            mTextEditScale--;
        }
    }
    changeInfoBar(100 + mTextEditScale * 10, ui->textEdit->isEnabled() ? "виключено" : "включено" );
}

void Notepad::changeFont()
{
    bool changeText;
    QFont font = QFontDialog::getFont(&changeText, this);
    if(changeText)
        ui->textEdit->setCurrentFont(font);
}

void Notepad::changeColor()
{
    QColor color = QColorDialog::getColor(Qt::white, this, "Select new color", nullptr);
    ui->textEdit->setTextColor(color);
}

void Notepad::changeBackgroundColor()
{
    QColor color = QColorDialog::getColor(Qt::white, this, "Select new color", nullptr);
    ui->textEdit->setTextBackgroundColor(color);
}

void Notepad::onSafeDataMode()
{
    bool modeActivated = false;
    if (ui->textEdit->isEnabled())
        modeActivated = true;

    ui->textEdit->setEnabled(!modeActivated);
    ui->openFile->setEnabled(!modeActivated);
    ui->saveFile->setEnabled(!modeActivated);
    ui->saveFileAs->setEnabled(!modeActivated);
    changeInfoBar(100 + mTextEditScale * 10, !modeActivated ? "виключено" : "включено" );
}

void Notepad::setFindWindow()
{
    FindMatchesWindow *findWindow = new FindMatchesWindow;
    // connecting slot which will receive signals from the poped window
    connect(findWindow, &FindMatchesWindow::selectText, this, &Notepad::selectSomeText);
    findWindow->text = ui->textEdit->toPlainText();
    findWindow->currentSearchPosition = ui->textEdit->textCursor().position();
    findWindow->show();
}

void Notepad::selectSomeText(int sposition, int eposition)
{
    QTextCursor c = ui->textEdit->textCursor();
    c.setPosition(sposition);
    c.setPosition(eposition, QTextCursor::KeepAnchor);
    ui->textEdit->setTextCursor(c);
}

void Notepad::changeInfoBar(int scale, QString safeModeEnabled) {
    QLabel *bar = ui->infoBar;
    bar->setText(QString::number(scale) + "% | Безпечний режим: " + safeModeEnabled);
}

void Notepad::changeFontFor(){
    QTextCursor c = ui->textEdit->textCursor();
    c.setPosition(0);                           // change font starting from this symbol
    c.setPosition(4, QTextCursor::KeepAnchor);  // to symbol setted here [Actually in this case changes from 0 to 4]
    ui->textEdit->setTextCursor(c);

    bool changeText;
    QFont font = QFontDialog::getFont(&changeText, this);
    if(changeText)
        ui->textEdit->setCurrentFont(font);
}
