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

function init(){
}

function render() {
  ClearBackground({ r: 128, g: 128, b: 128 });
  if(is_key_down("space") || is_mouse_button_pressed("left")){
    DrawRectangleRec({ x: x, y: y, w: w, h: h }, color_red);
  }else{
    DrawRectangleRec({ x: x, y: y, w: w, h: h }, color_green);
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
  // console.log("node", node,mouse_x, mouse_y,is_mouse_button_pressed("left"));
  if(node != null){
    var event = {
      target:node,
      mouse_x: mouse_x,
      mouse_y: mouse_y,
    };
    if(is_mouse_button_pressed("left")){
      event.name = "mouse_pressed";
      console.log("name", "mouse_pressed");
    }else if(is_mouse_button_released("left")){
      event.name = "mouse_released";
      console.log("name", "mouse_released");
    }
    model=ui.update(model,event);
    // console.log("model", JSON.stringify(model),"event",JSON.stringify(event));
  }
  ui_clear();
  ui_compute(ui.render(model));
  ui_draw(root_ui);
}
