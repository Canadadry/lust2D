var createNode =  function (node, props, children) {
  if (typeof node === "function") {
    return node(props || {}, children);
  }
  return ui_create(node, props, children);
};

var ui = require("ui");
var root_ui = -1;
x = 10;
y = 10;
w = 100;
h = 100;
dx = 4;
dy = 4;
color_red = { r: 255, g: 0, b: 0 };
color_green = { r: 0, g: 255, b: 0 };
model = ui.init();

function conf() {
  window.width = 800;
  window.height = 600;
  window.title = "from lust2D";
}

function init() {
  NewCanvas("shape", w - 20, h - 20);
  SetCanvas("shape");
  ClearCanvas("#0000");
  SetFillColor("#aaffee");
  MoveTo({ x: 10, y: 10 });
  BezierTo({ x: w - 30, y: 10}, { x: w - 30, y: 10},{ x: w - 30, y: h-30});
  LineTo({ x: 10    , y: h - 30 });
  LineTo({ x: 10    , y: 10 });
  Close();
  SetFillColor("#ffff00aa");
  MoveTo({ x: 20, y: 20 });
  LineTo({ x: 20    , y: h - 40 });
  LineTo({ x: w-20    , y: h - 40 });
  LineTo({ x: w-20    , y: 20 });
  LineTo({ x: 20, y: 20 });
  Close();

  //SaveCanvas("shape.png");
  CanvasToImage("shape");
}

function render() {
  ClearBackground({ r: 128, g: 128, b: 128 });
  if(is_key_down("space") || is_mouse_button_down("left")){
    DrawRectangleRec({ x: x, y: y, w: w, h: h }, color_red);
    DrawImagePro(
      "toucan.png",
      undefined,
      { x: x+10, y: y+10, w: w-20, h: h-20 }
    );
  }else{
    DrawRectangleRec({ x: x, y: y, w: w, h: h }, color_green);
    // UnloadImage("shape.png");
    // CanvasToImage("shape");
    DrawImagePro(
      "shape",
      undefined,
      { x: x+10, y: y+10, w: w-20, h: h-20 }
    );
  }
  x += dx;
  y += dy;
  if (x < 0 || x + w > window.width) {
    dx *= -1;
  }
  if (y < 0 || y + h > window.height) {
    dy *= -1;
  }
  var mouse_x = get_mouse_x();
  var mouse_y = get_mouse_y();
  var node = ui_pick(mouse_x, mouse_y);

  var event = {
    target:node,
    mouse_x: mouse_x,
    mouse_y: mouse_y,
  };
  if(is_mouse_button_pressed("left")){
    event.name = "mouse_pressed";
  }else if(is_mouse_button_released("left")){
    event.name = "mouse_released";
  }
  model=ui.update(model,event);

  ui_clear();
  ui_compute(ui.render(model));
  ui_draw(root_ui);
}
