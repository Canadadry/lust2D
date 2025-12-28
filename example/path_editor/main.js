var createNode =  function (node, props, children) {
  if (typeof node === "function") {
    return node(props || {}, children);
  }
  return ui_create(node, props, children);
};


var ui = require('ui');
var WColor = require('../widget/color');
var Path = require('path');

var model = {};

function conf() {
  window.width = 800;
  window.height = 600;
  window.title = "path editor";
}

function init() {
  model=ui.init()
  var root = ui.build(model);
  ui_compute(root);
}

function render() {
  Path.render(model.path);
  ui_clear();
  var root = ui.build(model);
  ui_compute(root);
  model = ui.handle_click(model);
  model.path = Path.handle_click(model.path);
  ui_draw(root);
}
