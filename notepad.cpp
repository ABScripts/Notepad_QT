#include "notepad.h"
#include "ui_notepad.h"
#include "headers.h"

Notepad::Notepad(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::Notepad)
{
    ui->setupUi(this);
    //TODO: get rid off this old connect style
    // connectors which belong...
    // ...to the file section
    connect(ui->openFile, SIGNAL(triggered()), SLOT(openFileAs()));
    connect(ui->saveFileAs, SIGNAL(triggered()), SLOT(saveFileAs()));
    connect(ui->saveFile, SIGNAL(triggered()), SLOT(saveFile()));
    // ...to the formatting section
    connect(ui->fontWindow, SIGNAL(triggered()), SLOT(changeFont()));
    connect(ui->redoAction, &QAction::triggered, [this](){ ui->textEdit->redo(); } );
    connect(ui->undoAction, &QAction::triggered, [this](){ ui->textEdit->undo(); } );
    connect(ui->copyText, &QAction::triggered, [this](){ ui->textEdit->copy(); });
    connect(ui->cutText, &QAction::triggered, [this](){ ui->textEdit->cut(); });
    connect(ui->pasteText, &QAction::triggered, [this](){ ui->textEdit->paste(); });

    connect(clipboard, &QClipboard::changed, [this](){ ui->pasteText->setEnabled(true); });

    connect(ui->textEdit, &QTextEdit::copyAvailable, [this](bool b)
                { ui->cutText->setEnabled(b);
                  ui->copyText->setEnabled(b); });

    connect(ui->selectAll, &QAction::triggered, [this](){ ui->textEdit->selectAll(); });
    // ...to the view section
    connect(ui->zoomIn, SIGNAL(triggered()), SLOT(setZoom()));
    connect(ui->zoomOut, SIGNAL(triggered()), SLOT(setZoom()));
    connect(ui->setDefault, SIGNAL(triggered()), SLOT(setZoom()));

    // These connectors are for tracking redo-undo button`s states
    // as well as state of "whether text in file was changed"
    connect(ui->textEdit, &QTextEdit::redoAvailable,
                [this](bool b){ ui->redoAction->setEnabled(b); });

    connect(ui->textEdit, &QTextEdit::undoAvailable,
                [this](bool b){ ui->undoAction->setEnabled(b); });

    connect(ui->textEdit, &QTextEdit::textChanged, [this](){ textChanged = true; });
}

Notepad::~Notepad()
{
    delete ui;
}

void Notepad::saveFileAs(){
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save file"),
                                     "C:/", tr("TextFiles (*.txt *.cpp *.h)"));

    interactWithFile(fileName, QIODevice::WriteOnly, &Notepad::saveFile);
}

void Notepad::openFileAs(){
    // getting the direct path to our file

    if (textChanged) { // if the user have already opened some project and changed some text there

        QMessageBox msgBox;
        msgBox.setText("The document has been modified.");
        msgBox.setInformativeText("Do you want to save your changes?");
        msgBox.setStandardButtons(QMessageBox::Save | QMessageBox::Cancel);
        msgBox.setDefaultButton(QMessageBox::Save);

        int reply = msgBox.exec();

        if ( reply == QMessageBox::Save) // switch or if - that`s the question...
            saveFile();
    }

    QString fileName = QFileDialog::getOpenFileName(this, tr("Open file"),  // set parent and title
                                       "C:/", tr("TextFiles (*.txt *.cpp *.h)")); // set types of files which could be opened

    interactWithFile(fileName, QIODevice::ReadOnly, &Notepad::openFile);
}

void Notepad::interactWithFile(QString fileName, QIODevice::OpenModeFlag flag,
                                            void (Notepad::*action)(QFile* file)){
    if (fileName == "" ) // if user canceled opening or saving file
        return;          // then do nothing

    QFile file(fileName);

    //and check whether it was opened succesfully or not...
    if ( !file.open(flag | QIODevice::Text ) )
        // if it was not - print this kind of a warning message
        QMessageBox::critical(this, "Error!", "The error occured while openinig file!");
    else {
        // running a proper function to save either open the file
        (this->*action)(&file);
        currentFilePath = fileName;
        textChanged = false;
    }
    file.flush();
    file.close();
}

void Notepad::openFile(QFile *file){
    QTextStream in(file);
    ui->textEdit->setPlainText(in.readAll());
}

void Notepad::saveFile(QFile *file){
    QTextStream out(file);
    out << ui->textEdit->toPlainText();
}

void Notepad::saveFile(){ // is used for saving file without opening the dialog window if it`s not nedeed
    if ( currentFilePath == "" ) // if this is a new file
        saveFileAs();            // treat it as a new file
    else                         // if this file already exists - just save
        interactWithFile(currentFilePath, QIODevice::WriteOnly, &Notepad::saveFile);
}

void Notepad::setZoom(){
    QString senderName = sender()->objectName();

    if ( senderName == "zoomIn" && textEditScale < 100){
        ui->textEdit->zoomIn(1);
        textEditScale += 1;
    }
    else if ( senderName == "zoomOut" && textEditScale > 0){
        ui->textEdit->zoomOut(1);
        textEditScale -= 1;
    }
    else{
        while ( textEditScale > 0){
            ui->textEdit->zoomOut(1);
            textEditScale -= 1;
        }
    }
}

void Notepad::changeFont(){
    bool ok;
    QFont font = QFontDialog::getFont(&ok, this);
    if( ok )
        ui->textEdit->setFont(font);
}

void Notepad::changeColor(){
    QColor color = QColorDialog::getColor(Qt::white, this, "Select new color", NULL);
    QPalette palette;
    palette.setColor(QPalette::Text, color);
}
