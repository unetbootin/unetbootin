TetrixShape = {
    NoShape:0,
    ZShape:1,
    SShape:2,
    LineShape:3,
    TShape:4,
    SquareShape:5,
    LShape:6,
    MirroredLShape:7
}

TetrixCoordsTable = [
    [ [ 0,   0 ], [ 0,  0 ], [ 0,  0 ], [ 0,  0 ] ],
    [ [ 0,  -1 ], [ 0,  0 ], [ -1, 0 ], [ -1, 1 ] ],
    [ [ 0,  -1 ], [ 0,  0 ], [ 1,  0 ], [ 1,  1 ] ],
    [ [ 0,  -1 ], [ 0,  0 ], [ 0,  1 ], [ 0,  2 ] ],
    [ [ -1,  0 ], [ 0,  0 ], [ 1,  0 ], [ 0,  1 ] ],
    [ [ 0,   0 ], [ 1,  0 ], [ 0,  1 ], [ 1,  1 ] ],
    [ [ -1, -1 ], [ 0, -1 ], [ 0,  0 ], [ 0,  1 ] ],
    [ [ 1,  -1 ], [ 0, -1 ], [ 0,  0 ], [ 0,  1 ] ]
]

function TetrixPiece()
{
    this.shape = TetrixShape.NoShape;
}

TetrixPiece.prototype.__defineGetter__(
    "shape",
    function() {
      return this._shape;
    }
);

TetrixPiece.prototype.__defineSetter__(
    "shape",
    function(shape) {
      this._shape = shape;
      this._coords = new Array(4);
      for (var i = 0; i < 4; ++i)
          this._coords[i] = TetrixCoordsTable[shape][i].slice();
    }
);

TetrixPiece.prototype.setRandomShape = function() {
    this.shape = Math.floor(((Math.random() * 100000) % 7) + 1);
}

TetrixPiece.prototype.getX = function(index) {
    return this._coords[index][0];
}

TetrixPiece.prototype.getY = function(index) {
    return this._coords[index][1];
}

TetrixPiece.prototype._setX = function(index, x) {
    this._coords[index][0] = x;
}

TetrixPiece.prototype._setY = function(index, y) {
    this._coords[index][1] = y;
}

TetrixPiece.prototype.__defineGetter__(
    "minX",
    function() {
        var min = this._coords[0][0];
        for (var i = 1; i < 4; ++i)
            min = Math.min(min, this._coords[i][0]);
        return min;
    }
);

TetrixPiece.prototype.__defineGetter__(
    "maxX",
    function() {
        var max = this._coords[0][0];
        for (var i = 1; i < 4; ++i)
            max = Math.max(max, this._coords[i][0]);
        return max;
    }
);

TetrixPiece.prototype.__defineGetter__(
    "minY",
    function() {
      var min = this._coords[0][1];
      for (var i = 1; i < 4; ++i)
          min = Math.min(min, this._coords[i][1]);
      return min;
    }
);

TetrixPiece.prototype.__defineGetter__(
    "maxY",
    function() {
        var max = this._coords[0][1];
        for (var i = 1; i < 4; ++i)
            max = Math.max(max, this._coords[i][1]);
        return max;
    }
);

TetrixPiece.prototype.rotatedLeft = function() {
    var result = new TetrixPiece();
    if (this._shape == TetrixShape.SquareShape) {
        result.shape = this._shape;
        return result;
    }
    result._shape = this._shape;
    for (var i = 0; i < 4; ++i) {
        result._setX(i, this.getY(i));
        result._setY(i, -this.getX(i));
    }
    return result;
}

TetrixPiece.prototype.rotatedRight = function() {
    var result = new TetrixPiece();
    if (this._shape == TetrixShape.SquareShape) {
        result.shape = this._shape;
        return result;
    }
    result._shape = this._shape;
    for (var i = 0; i < 4; ++i) {
        result._setX(i, -this.getY(i));
        result._setY(i, this.getX(i));
    }
    return result;
}
