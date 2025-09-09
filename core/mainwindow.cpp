#include "lib/mainwindow.h"
#include <QVBoxLayout>
#include <QFileInfo>
#include <QTimer>
#include <QFormLayout>
#include <QPushButton>
#include <QMessageBox>

// Реализация NewWindow
NewWindow::NewWindow(int labNumber, QWidget *parent) : QWidget(parent) {
    setStyleSheet(
            "QWidget {"
            "   background-color: #f5f5f5;"
            "}"
            "QLabel {"
            "   color: #233;"
            "   font-family: 'Segoe UI';"
            "   font-weight: bold;"
            "}"
            "QProgressBar {"
            "   border: 2px solid #233;"
            "   border-radius: 5px;"
            "   text-align: center;"
            "   font-family: 'Segoe UI';"
            "   font-weight: bold;"
            "   height: 20px;"
            "}"
            "QProgressBar::chunk {"
            "   background-color: qlineargradient(spread:pad, x1:0, y1:0, x2:0, y2:1, stop:0 rgba(90, 190, 90, 255), stop:1 rgba(60, 140, 60, 255));"
            "}"
            "QPushButton {"
            "   background: qlineargradient(spread:pad, x1:0, y1:0, x2:0, y2:1, stop:0 rgba(90, 190, 90, 255), stop:1 rgba(60, 140, 60, 255));"
            "   color: white;"
            "   border: 2px solid rgba(255, 255, 255, 0.3);"
            "   border-radius: 10px;"
            "   padding: 8px;"
            "   font-family: 'Segoe UI';"
            "   font-size: 13px;"
            "   font-weight: bold;"
            "   text-align: center;"
            "   min-width: 120px;"
            "}"
            "QPushButton:hover {"
            "   background: qlineargradient(spread:pad, x1:0, y1:0, x2:0, y2:1, stop:0 rgba(120, 200, 120, 255), stop:1 rgba(80, 160, 80, 255));"
            "   color: black;"
            "}"
    );

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(20, 20, 20, 20);
    mainLayout->setSpacing(15);

    QLabel *titleLabel = new QLabel(QString("Лабораторная работа №%1").arg(labNumber), this);
    titleLabel->setAlignment(Qt::AlignCenter);
    QFont font("Segoe UI", 24, QFont::Bold);
    titleLabel->setFont(font);
    mainLayout->addWidget(titleLabel);

    if (labNumber == 1) {
        // Инициализация BatteryInfo
        batteryInfo = new BatteryInfo(this);

        // Форма для информации об энергопитании
        QFormLayout *formLayout = new QFormLayout();
        formLayout->setSpacing(10);
        formLayout->setContentsMargins(10, 10, 10, 10);

        powerTypeLabel = new QLabel(this);
        batteryTypeLabel = new QLabel(this);
        batteryLevelLabel = new QLabel(this);
        powerSchemeLabel = new QLabel(this);
        fullBatteryTimeLabel = new QLabel(this);
        remainingBatteryTimeLabel = new QLabel(this);
        batteryProgressBar = new QProgressBar(this);
        batteryProgressBar->setRange(0, 100);
        batteryProgressBar->setTextVisible(true);
        batteryProgressBar->setFormat("%p%");

        formLayout->addRow("Тип энергопитания:", powerTypeLabel);
        formLayout->addRow("Тип батареи:", batteryTypeLabel);
        formLayout->addRow("Уровень заряда (%):", batteryLevelLabel);
        formLayout->addRow("Режим энергосбережения:", powerSchemeLabel);
        formLayout->addRow("Оставшееся время работы:", fullBatteryTimeLabel);
        formLayout->addRow("Время работы от батареи:", remainingBatteryTimeLabel);
        formLayout->addRow("Прогресс заряда:", batteryProgressBar);

        mainLayout->addLayout(formLayout);

        // Кнопки для спящего режима и гибернации
        QHBoxLayout *buttonLayout = new QHBoxLayout();
        buttonLayout->setSpacing(20);
        buttonLayout->setAlignment(Qt::AlignCenter);

        QPushButton *sleepButton = new QPushButton("Спящий режим", this);
        QPushButton *hibernateButton = new QPushButton("Гибернация", this);

        // Устанавливаем фиксированный размер кнопок
        sleepButton->setFixedSize(150, 40);
        hibernateButton->setFixedSize(150, 40);

        connect(sleepButton, &QPushButton::clicked, this, &NewWindow::enterSleepMode);
        connect(hibernateButton, &QPushButton::clicked, this, &NewWindow::enterHibernateMode);

        buttonLayout->addWidget(sleepButton);
        buttonLayout->addWidget(hibernateButton);

        // Добавляем кнопки в основной layout
        mainLayout->addLayout(buttonLayout);
        mainLayout->addStretch(); // Добавляем растягивающееся пространство

        // Таймер для обновления в реальном времени
        QTimer *timer = new QTimer(this);
        connect(timer, &QTimer::timeout, this, &NewWindow::updatePowerInfo);
        timer->start(300); // 1 секунда

        updatePowerInfo();
    }

    setWindowTitle(QString("Лабораторная работа №%1").arg(labNumber));
    resize(700, 400); // Увеличиваем размер окна
}

void NewWindow::updatePowerInfo() {
    powerTypeLabel->setText(batteryInfo->getPowerType());
    batteryTypeLabel->setText(batteryInfo->getBatteryChemistry());
    batteryLevelLabel->setText(batteryInfo->getBatteryLevel());
    powerSchemeLabel->setText(batteryInfo->getPowerSavingMode());
    fullBatteryTimeLabel->setText(batteryInfo->getBatteryTime());
    remainingBatteryTimeLabel->setText(batteryInfo->getEstimatedTime());

    QString levelStr = batteryInfo->getBatteryLevel();
    bool ok;
    int level = levelStr.remove("%").toInt(&ok);
    if (ok && level >= 0 && level <= 100) {
        batteryProgressBar->setValue(level);
    } else {
        batteryProgressBar->setValue(0);
        batteryProgressBar->setFormat("N/A");
    }
}

void NewWindow::enterSleepMode() {
    if (!batteryInfo->enterSleepMode()) {
        QMessageBox::warning(this, "Ошибка", "Не удалось перейти в спящий режим.");
    }
}

void NewWindow::enterHibernateMode() {
    if (!batteryInfo->enterHibernateMode()) {
        QMessageBox::warning(this, "Ошибка", "Не удалось перейти в гибернацию.");
    }
}

// Реализация CustomButton
CustomButton::CustomButton(const QString &text, QWidget *parent)
        : QPushButton(text, parent) {
    m_shadowEffect = new QGraphicsDropShadowEffect(this);
    m_shadowEffect->setBlurRadius(8);
    m_shadowEffect->setColor(QColor(0, 0, 0, 80));
    m_shadowEffect->setOffset(2, 2);
    setGraphicsEffect(m_shadowEffect);

    setStyleSheet(
            "QPushButton {"
            "   background: qlineargradient(spread:pad, x1:0, y1:0, x2:0, y2:1, stop:0 rgba(90, 190, 90, 255), stop:1 rgba(60, 140, 60, 255));"
            "   color: white;"
            "   border: 2px solid rgba(255, 255, 255, 0.3);"
            "   border-radius: 10px;"
            "   padding: 8px;"
            "   font-family: 'Segoe UI';"
            "   font-size: 13px;"
            "   font-weight: bold;"
            "   text-align: center;"
            "}"
    );

    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
}

CustomButton::~CustomButton() {
    if (m_shadowEffect) {
        delete m_shadowEffect;
        m_shadowEffect = nullptr;
    }
}

void CustomButton::enterEvent(QEnterEvent *event) {
    QPushButton::enterEvent(event);
    m_shadowEffect->setBlurRadius(10);
    m_shadowEffect->setColor(QColor(0, 0, 0, 100));
    setStyleSheet(
            "QPushButton {"
            "   background: qlineargradient(spread:pad, x1:0, y1:0, x2:0, y2:1, stop:0 rgba(120, 200, 120, 255), stop:1 rgba(80, 160, 80, 255));"
            "   color: black;"
            "   border: 2px solid rgba(255, 255, 255, 0.3);"
            "   border-radius: 10px;"
            "   padding: 8px;"
            "   font-family: 'Segoe UI';"
            "   font-size: 13px;"
            "   font-weight: bold;"
            "   text-align: center;"
            "}"
    );
}

void CustomButton::leaveEvent(QEvent *event) {
    QPushButton::leaveEvent(event);
    m_shadowEffect->setBlurRadius(8);
    m_shadowEffect->setColor(QColor(0, 0, 0, 80));
    setStyleSheet(
            "QPushButton {"
            "   background: qlineargradient(spread:pad, x1:0, y1:0, x2:0, y2:1, stop:0 rgba(90, 190, 90, 255), stop:1 rgba(60, 140, 60, 255));"
            "   color: white;"
            "   border: 2px solid rgba(255, 255, 255, 0.3);"
            "   border-radius: 10px;"
            "   padding: 8px;"
            "   font-family: 'Segoe UI';"
            "   font-size: 13px;"
            "   font-weight: bold;"
            "   text-align: center;"
            "}"
    );
}

// Реализация MainWindow
MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    setStyleSheet("QMainWindow { background-color: transparent; }");

    QWidget *centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    gifLabel = new QLabel(centralWidget);
    gifLabel->setAlignment(Qt::AlignCenter);
    gifLabel->setScaledContents(true);
    QString gifPath = ":/images/2.gif";
    QFileInfo gifFile(gifPath);
    if (!gifFile.exists()) {
        qWarning() << "Ошибка: Файл GIF не найден:" << gifPath;
        gifLabel->setText("Файл GIF не найден");
    } else {
        movie = new QMovie(gifPath, QByteArray(), this);
        if (!movie->isValid()) {
            qWarning() << "Ошибка загрузки GIF";
            gifLabel->setText("Ошибка загрузки GIF");
        } else {
            gifLabel->setMovie(movie);
            movie->start();
        }
    }
    mainLayout->addWidget(gifLabel, 1);

    QWidget *buttonWidget = new QWidget(centralWidget);
    buttonWidget->setStyleSheet("background: transparent;");
    QGridLayout *buttonLayout = new QGridLayout(buttonWidget);
    buttonLayout->setSpacing(15);
    buttonLayout->setContentsMargins(15, 15, 15, 15);
    buttonLayout->setAlignment(Qt::AlignCenter);

    int row = 0, col = 0;
    for (int i = 1; i <= 6; ++i) {
        CustomButton *button = new CustomButton(QString("Лабораторная работа №%1").arg(i), buttonWidget);
        button->setObjectName(QString("labButton_%1").arg(i));
        QFont font("Segoe UI", 12, QFont::Bold);
        button->setFont(font);

        connect(button, &QPushButton::clicked, this, &MainWindow::onLabButtonClicked);
        labButtons.append(button);
        buttonLayout->addWidget(button, row, col);
        col++;
        if (col >= 3) {
            col = 0;
            row++;
        }
    }

    mainLayout->addWidget(buttonWidget, 0, Qt::AlignBottom);

    setWindowTitle("Главное окно");
    setMinimumSize(800, 600);
    showMaximized();
}

MainWindow::~MainWindow() {
    delete movie;
}

void MainWindow::onLabButtonClicked() {
    QPushButton *button = qobject_cast<QPushButton*>(sender());
    if (button) {
        QString objectName = button->objectName();
        int labNumber = objectName.split("_").last().toInt();
        qDebug() << "Нажата кнопка Лабораторная работа №" << labNumber;

        NewWindow *newWindow = new NewWindow(labNumber);
        newWindow->setAttribute(Qt::WA_DeleteOnClose);
        newWindow->show();
    }
}