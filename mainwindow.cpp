#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <qstring.h>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_quitButton_clicked()
{
    QCoreApplication::quit();
}


void MainWindow::on_startButton_clicked()
{
    Game g(3);
    game = g;

    update();
}


void MainWindow::on_playButton_clicked()
{
    std::string play = ui->playEdit->text().toStdString();

    int turn = game.getTurn();
    for (unsigned int i = 0; i < game.playerDecks[turn].size(); i++) {
        int idx = game.playerDecks[turn][i];
        Card c = game.getCard(idx);
        if (play.find(c.rank) != std::string::npos && play.find(c.suit) != std::string::npos) {
            if (game.playCard(idx)) {
                update();
                break;
            }
        }
    }
}


void MainWindow::on_drawButton_clicked()
{
    if (game.drawCard())
        update();
}

void MainWindow::update() {
    ui->player1scoreEdit->setText(QString::number(game.scores[0]));
    ui->player2scoreEdit->setText(QString::number(game.scores[1]));
    ui->player3scoreEdit->setText(QString::number(game.scores[2]));

    ui->playEdit->setText("");

    int idx = game.faceUp[game.faceUp.size() - 1];
    Card face = game.getCard(idx);

    QString r = QString::fromStdString(face.rank);
    ui->rankEdit->setText(r);

    QString s = QString::fromStdString(face.suit);
    ui->suitEdit->setText(s);

    int turn = game.getTurn();
    ui->playerEdit->setText(QString::number(turn + 1));
    ui->yourCardsEdit->setPlainText("");
    for(int i = 0; i < game.playerDecks[turn].size(); i++) {
        Card yourCard = game.getCard(game.playerDecks[turn][i]);

        QString rank = QString::fromStdString(yourCard.rank);
        QString suit = QString::fromStdString(yourCard.suit);

        ui->yourCardsEdit->appendPlainText(rank + " " + suit + "\n");
    }
}

