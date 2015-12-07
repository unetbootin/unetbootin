function TetrixWindow(ui)
{
    this.ui = ui;

    var boardUi = ui.findChild("board");
    boardUi.nextPieceLabel = ui.findChild("nextPieceLabel");

    this.board = new TetrixBoard(boardUi);

    ui.findChild("startButton").clicked.connect(this.board, this.board.start);
    ui.findChild("quitButton").clicked.connect(Qt.App.quit);
    ui.findChild("pauseButton").clicked.connect(this.board, this.board.pause);
    boardUi.scoreChanged.connect(ui.findChild("scoreLcd")["display(int)"]);
    boardUi.levelChanged.connect(ui.findChild("levelLcd")["display(int)"]);
    boardUi.linesRemovedChanged.connect(ui.findChild("linesLcd")["display(int)"]);
}
