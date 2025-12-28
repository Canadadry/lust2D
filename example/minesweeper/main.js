var createNode =  function (node, props, children) {
  if (typeof node === "function") {
    return node(props || {}, children);
  }
  return ui_create(node, props, children);
};

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

function computeView(w, h, lw, lh, tw, th) {
  var mapWidth = lw * tw;
  var mapHeight = lh * th;
  var zoom = Math.min(w / mapWidth, h / mapHeight);
  var offset_x = (w - mapWidth * zoom) / 2;
  var offset_y = (h - mapHeight * zoom) / 2;

  return { offset_x: offset_x, offset_y: offset_y, zoom:zoom };
}

function getTileAt(mouse_x, mouse_y, offset_x, offset_y, zoom, tw, th) {
  var xInMap = (mouse_x - offset_x) / zoom;
  var yInMap = (mouse_y - offset_y) / zoom;
  var i = Math.floor(xInMap / tw);
  var j = Math.floor(yInMap / th);
  return { i:i, j:j };
}

function render() {
  var view = computeView(window.width, window.height, size, size, tilesize, tilesize);
  ClearBackground({ r: 128, g: 128, b: 128 });
  for (var i = 0; i < size;i++){
    for (var j = 0; j < size;j++){
      var tile = game.tileAt(game, i, j);
      var tile_x = tile % tile_width;
      var tile_y = Math.floor(tile / tile_width);
      DrawImagePro(
        "minesweeper.png",
        { x: tile_x*tilesize, y: tile_y*tilesize, w: tilesize, h: tilesize },
        {
          x: i*tilesize*view.zoom+view.offset_x,
          y: j*tilesize*view.zoom+view.offset_y,
          w: tilesize*view.zoom, h: tilesize*view.zoom }
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
  if (!game.inState(game,"playing")) {
    restart();
    return;
  }
  var view = computeView(window.width, window.height, size, size, tilesize, tilesize);
  var pos = getTileAt(x, y, view.offset_x, view.offset_y, view.zoom, tilesize, tilesize);
  var ret = false;

  if (button == "left") {
    ret = game.reveal(game, pos.i, pos.j);
  } else {
    ret = game.flag(game, pos.i, pos.j);
  }

  if (game.inState(game,"win")) {
    PlaySound("win.wav");
  } else if (game.inState(game,"loose")) {
    PlaySound("loose.wav");
  } else if (ret) {
    PlaySound("nothing.wav");
  }

}
