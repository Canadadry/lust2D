function shuffleTab(array) {
  var l = array.length;

  while (l >= 0) {
    var j = 1 + Math.floor(Math.random() * l);
    var valI = array[l];
    var valJ = array[j];
    array[l] = valJ;
    array[j] = valI;
    l = l - 1;
  }
  return array;
}

function Cell(m, d, f) {
  return {
    mineAround: m,
    discovered: d,
    flagged: f,
  };
}

exports.new = function (w, h, m) {
  var self = {};
  self.width = w;
  self.height = h;
  self.mines = m;
  self.minesFound = 0;
  self.flagLeft = m;
  self.state = "playing";
  self.revealedCells = 0;
  self.inState = function (game, s) {
    return game.state == s;
  };
  self.placeMine = function (game, x, y) {
    if (x < 0) {
      return false;
    }
    if (y < 0) {
      return false;
    }
    if (x >= game.width) {
      return false;
    }
    if (y >= game.height) {
      return false;
    }

    if (game.map[x][y].mineAround == 9) {
      return false;
    }

    game.map[x][y].mineAround = 9;
    game.neighbourMinePlaced(game, x - 1, y - 1);
    game.neighbourMinePlaced(game, x, y - 1);
    game.neighbourMinePlaced(game, x + 1, y - 1);
    game.neighbourMinePlaced(game, x - 1, y);
    game.neighbourMinePlaced(game, x + 1, y);
    game.neighbourMinePlaced(game, x - 1, y + 1);
    game.neighbourMinePlaced(game, x, y + 1);
    game.neighbourMinePlaced(game, x + 1, y + 1);

    return true;
  };
  self.neighbourMinePlaced = function (game, x, y) {
    if (x < 0) {
      return;
    }
    if (y < 0) {
      return;
    }
    if (x >= game.width) {
      return;
    }
    if (y >= game.height) {
      return;
    }

    if (game.map[x][y].mineAround == 9) {
      return;
    }

    game.map[x][y].mineAround++;
  }
  self.flag = function (game, x, y) {
    if (x < 0) {
      return false;
    }
    if (y < 0) {
      return false;
    }
    if (x >= game.width) {
      return false;
    }
    if (y >= game.height) {
      return false;
    }

    var cell = game.map[x][y];

    if (cell.discovered) {
      return false;
    }
    if (game.flagLeft == 0 && cell.flagged == false) {
      return false;
    }

    game.flagLeft += cell.flagged ? -1 : 1;
    cell.flagged = !cell.flagged;

    if (cell.mineAround != 9) {
      return true;
    }

    game.minesFound += cell.flagged ? 1 : -1;
    if (game.minesFound == game.mines) {
      game.state = "win";
    }
    return true;
  }
  self.reveal = function (game, x, y) {
    if (x < 0) {
      return false;
    }
    if (y < 0) {
      return false;
    }
    if (x >= game.width) {
      return false;
    }
    if (y >= game.height) {
      return false;
    }

    var cell = game.map[x][y];

    if (cell.discovered) {
      return false;
    }
    if (cell.flagged) {
      return false;
    }

    if (cell.discovered == false) {
      game.revealedCells++;
    }

    cell.discovered = true
    if (cell.mineAround == 9) {
      game.state = "loose";
      return true;
    }
    if (cell.mineAround == 0) {
      game.revealAround(game, x, y);
    }

    var cellsLeft = game.width * game.height - game.revealedCells;

    if (cellsLeft + game.minesFound == game.mines) {
      game.state = "win";
      return false;
    }

    return true;
  }
  self.revealAround = function (game, x, y) {
    if (x < 0) {
      return;
    }
    if (y < 0) {
      return;
    }
    if (x >= game.width) {
      return;
    }
    if (y >= game.height) {
      return;
    }

    game.reveal(game, x - 1, y - 1);
    game.reveal(game, x, y - 1);
    game.reveal(game, x + 1, y - 1);
    game.reveal(game, x - 1, y);
    game.reveal(game, x + 1, y);
    game.reveal(game, x - 1, y + 1);
    game.reveal(game, x, y + 1);
    game.reveal(game, x + 1, y + 1);
  }
  self.tileAt = function (game, x, y) {
    if (x < 0) {
      return -1;
    }
    if (y < 0) {
      return -1;
    }
    if (x >= game.width) {
      return -1;
    }
    if (y >= game.height) {
      return -1;
    }

    var c = game.map[x][y];

    if (game.state == "playing") {
      if (c.discovered) {
        switch (c.mineAround) {
          case 0:
            return 6;
          case 9:
            return 3;
          default:
            return c.mineAround + 6;
        }
      }
      if (c.flagged) {
        return 0;
      }
      return 5;
    }
    if (!c.flagged) {
      switch (c.mineAround) {
        case 0:
          return 6;
        case 9:
          return c.discovered ? 3 : 2;
        default:
          return c.mineAround + 6;
      }
    }
    if (c.mineAround == 9) {
      return 0;
    }
    return 4;
  }
  self.tiles = function (game) {
    var tiles = [];
    for (var x = 0; x < game.width; x++) {
      tiles[x] = [];
      for (var y = 0; y < game.height; y++) {
        tiles[x][y] = game.tileAt(game, x, y);
      }
    }
    return tiles;
  }
  self.init = function (self) {
    self.map = [];
    for (var x = 0; x < self.width; x++) {
      self.map[x] = [];
      for (var y = 0; y < self.height; y++) {
        self.map[x][y] = Cell(0, false, false);
      }
    }

    var place = [];
    for (var i = 0; i < this.width * this.height; i++) {
      place[i] = i;
    }
    place = shuffleTab(place);

    for (var i = 0; i < self.mines; i++) {
      var x = place[i] % self.width;
      var y = Math.floor(place[i] / self.width);
      self.placeMine(self,x, y);
    }
  }
  self.init(self);
  return self;
};
