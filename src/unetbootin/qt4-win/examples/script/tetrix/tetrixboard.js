function TetrixBoard(ui)
{
    this.ui = ui;

    this.isStarted = false;
    this.isPaused = false;

    this._board = new Array(TetrixBoard.BoardWidth * TetrixBoard.BoardHeight);
    this.clearBoard();

    this.curPiece = new TetrixPiece();
    this.nextPiece = new TetrixPiece();
    this.nextPiece.setRandomShape();

    ui.timer.timeout.connect(this, this.onTimer);
    ui.keyPressed.connect(this, this.onKeyPress);
    ui.paintRequested.connect(this, this.onPaint);
    ui.paintNextPieceRequested.connect(this, this.onPaintNextPiece);
}

TetrixBoard.BoardWidth = 10;
TetrixBoard.BoardHeight = 22;

TetrixBoard.prototype.start = function() {
    if (this.isPaused)
        return;

    this.isStarted = true;
    this.isWaitingAfterLine = false;
    this.numLinesRemoved = 0;
    this.numPiecesDropped = 0;
    this.score = 0;
    this.level = 1;
    this.clearBoard();

    this.ui.linesRemovedChanged(this.numLinesRemoved);
    this.ui.scoreChanged(this.score);
    this.ui.levelChanged(this.level);

    this.newPiece();
    this.ui.timer.start(this.timeoutTime());
}

TetrixBoard.prototype.pause = function() {
    if (!this.isStarted)
        return;

    this.isPaused = !this.isPaused;
    if (this.isPaused) {
        this.ui.timer.stop();
    } else {
        this.ui.timer.start(this.timeoutTime());
    }
    this.ui.update();
}

TetrixBoard.prototype.getShapeAt = function(x, y) {
    return this._board[(y * TetrixBoard.BoardWidth) + x];
}

TetrixBoard.prototype.setShapeAt = function(x, y, newShape) {
    this._board[(y * TetrixBoard.BoardWidth) + x] = newShape;
}

TetrixBoard.prototype.clearBoard = function() {
    for (var i = 0; i < TetrixBoard.BoardHeight * TetrixBoard.BoardWidth; ++i)
        this._board[i] = TetrixShape.NoShape;
}

TetrixBoard.prototype.dropDown = function() {
    var dropHeight = 0;
    var newY = this.curY;
    while (newY > 0) {
        if (!this.tryMove(this.curPiece, this.curX, newY - 1))
            break;
        --newY;
        ++dropHeight;
    }
    this.pieceDropped(dropHeight);
}

TetrixBoard.prototype.oneLineDown = function() {
    if (!this.tryMove(this.curPiece, this.curX, this.curY - 1))
        this.pieceDropped(0);
}

TetrixBoard.prototype.pieceDropped = function(dropHeight) {
    for (var i = 0; i < 4; ++i) {
        var x = this.curX + this.curPiece.getX(i);
        var y = this.curY - this.curPiece.getY(i);
        this.setShapeAt(x, y, this.curPiece.shape);
    }

    ++this.numPiecesDropped;
    if ((this.numPiecesDropped % 25) == 0) {
        ++this.level;
        this.ui.timer.start(this.timeoutTime());
        this.ui.levelChanged(this.level);
    }

    this.score += dropHeight + 7;
    this.ui.scoreChanged(this.score);
    this.removeFullLines();

    if (!this.isWaitingAfterLine)
        this.newPiece();
}

TetrixBoard.prototype.removeFullLines = function() {
    var numFullLines = 0;

    for (var i = TetrixBoard.BoardHeight - 1; i >= 0; --i) {
        var lineIsFull = true;

        for (var j = 0; j < TetrixBoard.BoardWidth; ++j) {
            if (this.getShapeAt(j, i) == TetrixShape.NoShape) {
                lineIsFull = false;
                break;
            }
        }

        if (lineIsFull) {
            ++numFullLines;
            for (var k = i; k < TetrixBoard.BoardHeight - 1; ++k) {
                for (var j = 0; j < TetrixBoard.BoardWidth; ++j)
                    this.setShapeAt(j, k, this.getShapeAt(j, k + 1));
            }
            for (var j = 0; j < TetrixBoard.BoardWidth; ++j)
                this.setShapeAt(j, TetrixBoard.BoardHeight - 1, TetrixShape.NoShape);
        }
    }

    if (numFullLines > 0) {
        this.numLinesRemoved += numFullLines;
        this.score += 10 * numFullLines;
        this.ui.linesRemovedChanged(this.numLinesRemoved);
        this.ui.scoreChanged(this.score);

        this.ui.timer.start(500);
        this.isWaitingAfterLine = true;
        this.curPiece.shape = TetrixShape.NoShape;
        this.ui.update();
    }
}

TetrixBoard.prototype.newPiece = function() {
    this.curPiece = this.nextPiece;
    this.nextPiece = new TetrixPiece();
    this.nextPiece.setRandomShape();
    this.ui.showNextPiece(this.nextPiece.maxX - this.nextPiece.minX + 1,
                          this.nextPiece.maxY - this.nextPiece.minY + 1);
    this.curX = TetrixBoard.BoardWidth / 2 + 1;
    this.curY = TetrixBoard.BoardHeight - 1 + this.curPiece.minY;

    if (!this.tryMove(this.curPiece, this.curX, this.curY)) {
        this.curPiece.shape = TetrixShape.NoShape;
        this.ui.timer.stop();
        this.isStarted = false;
    }
}

TetrixBoard.prototype.tryMove = function(newPiece, newX, newY) {
    for (var i = 0; i < 4; ++i) {
        var x = newX + newPiece.getX(i);
        var y = newY - newPiece.getY(i);
        if ((x < 0) || (x >= TetrixBoard.BoardWidth) || (y < 0) || (y >= TetrixBoard.BoardHeight))
            return false;
        if (this.getShapeAt(x, y) != TetrixShape.NoShape)
            return false;
    }

    this.curPiece = newPiece;
    this.curX = newX;
    this.curY = newY;
    this.ui.update();
    return true;
}

TetrixBoard.prototype.onPaint = function(painter) {
    if (this.isPaused) {
        this.ui.drawPauseScreen(painter);
        return;
    }

    for (var i = 0; i < TetrixBoard.BoardHeight; ++i) {
        for (var j = 0; j < TetrixBoard.BoardWidth; ++j) {
            var shape = this.getShapeAt(j, TetrixBoard.BoardHeight - i - 1);
            if (shape != TetrixShape.NoShape)
                this.ui.drawSquare(painter, j, i, shape);
        }
    }

    if (this.curPiece.shape != TetrixShape.NoShape) {
        for (var i = 0; i < 4; ++i) {
            var x = this.curX + this.curPiece.getX(i);
            var y = this.curY - this.curPiece.getY(i);
            this.ui.drawSquare(painter, x, TetrixBoard.BoardHeight - y - 1,
                               this.curPiece.shape);
        }
    }
}

TetrixBoard.prototype.onPaintNextPiece = function(painter) {
    for (var i = 0; i < 4; ++i) {
        var x = this.nextPiece.getX(i) - this.nextPiece.minX;
        var y = this.nextPiece.getY(i) - this.nextPiece.minY;
        this.ui.drawSquare(painter, x, y, this.nextPiece.shape);
    }
}

TetrixBoard.prototype.onKeyPress = function(key) {
    if (!this.isStarted || this.isPaused || (this.curPiece.shape == TetrixShape.NoShape))
        return;

    switch (key) {
        case Qt.Key_Left:
            this.tryMove(this.curPiece, this.curX - 1, this.curY);
            break;
        case Qt.Key_Right:
            this.tryMove(this.curPiece, this.curX + 1, this.curY);
            break;
        case Qt.Key_Down:
            this.tryMove(this.curPiece.rotatedRight(), this.curX, this.curY);
            break;
        case Qt.Key_Up:
            this.tryMove(this.curPiece.rotatedLeft(), this.curX, this.curY);
            break;
        case Qt.Key_Space:
            this.dropDown();
            break;
        case Qt.Key_D:
            this.oneLineDown();
            break;
    }
}

TetrixBoard.prototype.onTimer = function() {
    if (this.isWaitingAfterLine) {
        this.isWaitingAfterLine = false;
        this.newPiece();
        this.ui.timer.start(this.timeoutTime());
    } else {
        this.oneLineDown();
    }
}

TetrixBoard.prototype.timeoutTime = function() {
    return 1000 / (1 + this.level);
}
