var Game = require("game");

var size = 10;
var mine = 10;
var tilesize = 20;
var tile_width = 5;
var tile_height = 3;
var game = {
  tileAt:function(game,i,j){
    return (i + j) % (tile_height * tile_width);
  }
};

function restart() {
  game = new Game.new(size, size, mine);
  PlaySound("start.wav");
}

function conf() {
  window.width = 800;
  window.height = 600;
  window.title = "Minesweeper";
}

function init(){
  restart();
}

function render() {
  ClearBackground({ r: 128, g: 128, b: 128 });
  for (var i = 0; i < size;i++){
    for (var j = 0; j < size;j++){
      var tile = game.tileAt(game, i, j);
      var tile_x = tile % tile_width;
      var tile_y = Math.floor(tile / tile_width);
      DrawImagePro(
        "minesweeper.png",
        { x: tile_x*tilesize, y: tile_y*tilesize, w: tilesize, h: tilesize },
        { x: i*tilesize, y: j*tilesize, w: tilesize, h: tilesize }
      );
    }
  }
  if(is_mouse_button_pressed("left")){
    click(get_mouse_x(), get_mouse_y(), "left");
  }else if(is_mouse_button_released("right")){
    click(get_mouse_x(), get_mouse_y(), "right");
  }
}

function click(x, y, button) {
  var x_pos = Math.floor(x / tilesize);
  var y_pos = Math.floor(y / tilesize);

  if (!game.inState(game,"playing")) {
    restart();
    return;
  }
  var ret = false

  if (button == "left") {
    ret = game.reveal(game,x_pos, y_pos)
  } else {
    ret = game.flag(game,x_pos, y_pos)
  }

  if (game.inState(game,"win")) {
    PlaySound("win.wav");
  } else if (game.inState(game,"loose")) {
    PlaySound("loose.wav");
  } else if (ret) {
    PlaySound("nothing.wav");
  }

}



// love.draw = function() {
//   love.graphics.draw(ress.batch, 0, offsetY, 0, zoom, zoom)
// }

// love.load = function() {
//   if (love.system.getOS() != "iOS" || love.system.getOS() != "Android") {
//     love.window.setMode(600, 600, {})
//   }
//   math.randomseed(os.time())
//   ress = new Ressource()
//   ress.initBatch(size, size)
//   let mode = love.window.getMode()
//   w = mode[0]
//   h = mode[1]
//   zoom = w / (size * ress.tileSize)
//   offsetY = (h - size * ress.tileSize * zoom) / 2
//   restart()
// }

// love.keyreleased = function(key) {
//   if (game.state == "playing") {
//     return
//   }
//   if (key == "space") {
//     restart()
//   }
// }

// love.mousepressed = function(x, y, button, isTouch) {
//   pressStartedAt = love.timer.getTime()
// }

// love.mousereleased = function(x, y, button, isTouch) {
//   let x_pos = math.floor(x / ress.tileSize / zoom)
//   let y_pos = math.floor((y - offsetY) / ress.tileSize / zoom)

//   if (!game.inState("playing")) {
//     restart()
//     return
//   }
//   let ret = false
//   let elapsed = love.timer.getTime() - pressStartedAt

//   if (button == 1 && elapsed <= 0.3) {
//     ret = game.reveal(x_pos, y_pos)
//   } else {
//     ret = game.flag(x_pos, y_pos)
//   }

//   if (game.inState("win")) {
//     ress.play("win")
//   } else if (game.inState("loose")) {
//     ress.play("loose")
//   } else if (ret) {
//     ress.play("nothing")
//   }

//   ress.updateTilesetBatch(size, size, game.tiles())
// }
