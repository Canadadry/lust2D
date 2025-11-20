var ui = require('ui');
var Path = require('path');

var model = {
  path: {},
  silder_width: 0,
  mode: "fill",
  colors: {
    fill: { r: 255, g: 0, b: 0 },
    border: { r: 0, g: 255, b: 0 }
  }
};

function conf() {
  window.width = 800;
  window.height = 600;
  window.title = "path editor";
}

function init() {
  model.path=Path.init()
  var root = ui.build(model);
  ui_compute(root);
}

function render() {
  Path.render(model);
  ui_clear();
  var root = ui.build(model);
  ui_compute(root);
  handle_click(model,Path.handle_click);
  ui_draw(root);
}
