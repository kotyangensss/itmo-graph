#include "QMain.h"
#include "QImageWidget.h"
#include "QSavePictureWindow.h"
#include "QOpenPictureWindow.h"
#include "Pnm.h"
#include "QChangeColorspaceWindow.h"
#include "QAssignGammaWindow.h"
#include "QConvertGammaWindow.h"
#include "QLineParametersSelectionWindow.h"
#include "QDrawLineWindow.h"
#include "DrawColoredLine.h"
#include "QDitheringParametersWindow.h"
#include "QGradientGenerationWindow.h"

void QMain::openOpenWindow() {
    auto openWindow = new QOpenPictureWindow();
    openWindow->exec();
    try {
        if (openWindow->checkSubmitted()) {
            auto path = openWindow->getPicturePath();
            auto colorspaceChoice = openWindow->getColorSpace();
            auto file = Pnm(path);
            *pixels = Pixels(file.data, file.width, file.height, file.tag, colorspaceChoice, ColorChannel::All, 0);
            delete picture;
            picture = new QImageWidget(pixels, this);
            this->setCentralWidget(picture);
        }
    }
    catch (const std::invalid_argument &e) {
        auto messageBox = new QMessageBox();
        messageBox->setText(e.what());
        messageBox->exec();
    }
}

void QMain::openSaveWindow() {
    auto saveWindow = new QSavePictureWindow();
    saveWindow->exec();
    auto savePicturePath = saveWindow->getPicturePath();
    try {
        if (saveWindow->checkSubmitted()) {
            Pnm file;
            file.width = pixels->getWidth();
            file.height = pixels->getHeight();
            file.max = 255;
            file.tag[0] = 'P';
            if (pixels->getTag() == PnmFormat::P5)
                file.tag[1] = '5';
            else {
                if (pixels->getColorChannel() == ColorChannel::All)
                    file.tag[1] = '6';
                else
                    file.tag[1] = '5';
            }
            pixels->setGamma(0);
            file.data = remove_other_channels(pixels->getValues(), pixels->getColorChannel());
            file.write(savePicturePath);
        }
    }
    catch (const std::invalid_argument& e){
        auto box = new QMessageBox();
        box->setText(e.what());
        box->exec();
    }
}

void QMain::openColorSpaceAndChannelWindow() {
    auto changeColorspaceWindow = new QChangeColorspaceWindow(pixels->getColorSpace(), pixels->getColorChannel());
    changeColorspaceWindow->exec();

    auto colorSpace = changeColorspaceWindow->getColorSpace();
    auto colorChannel  = changeColorspaceWindow->getColorChannel();

    delete picture;

    pixels->setColorSpace(colorSpace);
    pixels->setColorChannel(colorChannel);

    picture = new QImageWidget(pixels, this);
    this->setCentralWidget(picture);
}

void QMain::openAssignGammaWindow() {
    auto assignGammaWindow = new QAssignGammaWindow(picture->getGamma());
    assignGammaWindow->exec();
    if (assignGammaWindow->checkSubmited()) {
        picture->setGamma(assignGammaWindow->getNewGamma());
        this->setCentralWidget(picture);
    }
}

void QMain::openConvertGammaWindow() {
    auto convertGammaWindow = new QConvertGammaWindow(pixels->getGamma());
    convertGammaWindow->exec();
    if (convertGammaWindow->checkSubmited()) {
        auto gamma = picture->getGamma();
        delete picture;
        pixels->setGamma(convertGammaWindow->getNewGamma());
        picture = new QImageWidget(pixels, this);
        picture->setGamma(gamma);
        this->setCentralWidget(picture);
    }
}

void QMain::openDrawLineWindow() {
    auto drawLineWindow = new QDrawLineWindow(pixels, &picture, this, lineColor, lineThickness, lineTransparency);
    drawLineWindow->show();
}

void QMain::openLineParametersWindow() {
    auto lineParametersWindow = new QLineParametersSelectionWindow();
    lineParametersWindow->exec();
    if (lineParametersWindow->checkSubmitted()) {
        lineColor = lineParametersWindow->getColor();
        lineThickness = lineParametersWindow->getLineThickness();
        lineTransparency = lineParametersWindow->getLineTransparency();
    }
}

void QMain::openDitheringParametersWindow() {
    auto ditheringParametersWindow = new QDitheringParametersWindow(pixels, this);
    ditheringParametersWindow->exec();
    auto ditheringPixels = ditheringParametersWindow->getDitheringPixels();
    auto saveWindow = new QSavePictureWindow();
    saveWindow->exec();
    auto savePicturePath = saveWindow->getPicturePath();
    try {
        if (saveWindow->checkSubmitted()) {
            Pnm file;
            file.width = ditheringPixels->getWidth();
            file.height = ditheringPixels->getHeight();
            file.max = 255;
            file.tag[0] = 'P';
            if (ditheringPixels->getTag() == PnmFormat::P5)
                file.tag[1] = '5';
            else {
                if (ditheringPixels->getColorChannel() == ColorChannel::All)
                    file.tag[1] = '6';
                else
                    file.tag[1] = '5';
            }
            ditheringPixels->setGamma(0);
            file.data = remove_other_channels(ditheringPixels->getValues(), ditheringPixels->getColorChannel());
            file.write(savePicturePath);
        }
    }
    catch (const std::invalid_argument& e){
        auto box = new QMessageBox();
        box->setText(e.what());
        box->exec();
    }
}

void QMain::openGradientGenerationWindow() {
    auto gradientGenerationWindow = new QGradientGenerationWindow(pixels->getWidth(), pixels->getHeight());
    gradientGenerationWindow->exec();
}


QMain::QMain(Pixels* pixels_, QImageWidget* picture_){
    pixels = pixels_;
    picture = new QImageWidget(pixels_, this);
    this->resize(300, 300);


    auto fileMenu = new QMenu("Файл");

    auto openFile = new QAction("Открыть");
    openFile->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_O));

    auto saveFile = new QAction("Сохранить как");
    saveFile->setShortcut(QKeySequence(Qt::CTRL | static_cast<Qt::Key>(Qt::SHIFT) + Qt::Key_S));

    auto editMenu = new QMenu("Редактировать");

    auto colorspaceChange = new QAction("Изменить цветовое пространство");
    colorspaceChange->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_C));

    auto assignGamma = new QAction("Назначить гамму");
    assignGamma->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_A));

    auto convertGamma = new QAction("Преобразовать гамму");
    convertGamma->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_G));

    auto drawLineMenu = new QMenu("Рисовать");

    auto drawLine = new QAction("Нарисовать линию");
    drawLine->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_D));

    auto lineParameters = new QAction("Изменить параметры линии");
    lineParameters->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_L));

    auto ditheringMenu = new QMenu("Дизеринг");

    auto ditheringParameters = new QAction("Изменить параметры дизеринга");
    ditheringParameters->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_Z));

    auto gradientGeneration = new QAction("Сгенерировать изображение с горизонтальным градиентом");
    gradientGeneration->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_R));

    drawLineMenu->addAction(drawLine);
    drawLineMenu->addAction(lineParameters);

    ditheringMenu->addAction(ditheringParameters);
    ditheringMenu->addAction(gradientGeneration);

    fileMenu->addAction(openFile);
    fileMenu->addAction(saveFile);

    editMenu->addAction(colorspaceChange);
    editMenu->addAction(assignGamma);
    editMenu->addAction(convertGamma);
    editMenu->addMenu(drawLineMenu);
    editMenu->addMenu(ditheringMenu);

    auto close = new QAction("Закрыть");
    close->setShortcut(QKeySequence(Qt::Key_Escape));
    connect(close, &QAction::triggered, [this](){
        this->close();
    });

    auto menuBar = new QMenuBar();
    menuBar->addMenu(fileMenu);
    menuBar->addMenu(editMenu);
    menuBar->addAction(close);

    this->setMenuBar(menuBar);
    this->setCentralWidget(picture);

    connect(openFile, &QAction::triggered, this, &QMain::openOpenWindow);
    connect(saveFile, &QAction::triggered, this, &QMain::openSaveWindow);
    connect(colorspaceChange, &QAction::triggered, this, &QMain::openColorSpaceAndChannelWindow);
    connect(assignGamma, &QAction::triggered, this, &QMain::openAssignGammaWindow);
    connect(convertGamma, &QAction::triggered, this, &QMain::openConvertGammaWindow);
    connect(drawLine, &QAction::triggered, this, &QMain::openDrawLineWindow);
    connect(lineParameters, &QAction::triggered, this, &QMain::openLineParametersWindow);
    connect(ditheringParameters, &QAction::triggered, this, &QMain::openDitheringParametersWindow);
    connect(gradientGeneration, &QAction::triggered, this, &QMain::openGradientGenerationWindow);
}


