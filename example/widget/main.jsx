var createNode =  function (node, props, children) {
  if (typeof node === "function") {
    return node(props || {}, children);
  }
  return ui_create(node, props, children);
};

var Color = require("color")

var color = Color.init("fill")

function conf() {
  window.width = 800;
  window.height = 600;
  window.title = "widget";
}

function init() {
}

function render() {
  ClearBackground({r:color.r.val,g:color.g.val,b:color.b.val,a:255});
  ui_clear();
  var root = ui_compute(<item class="lv fit-y" w={200}>{Color.draw(color)}</item>);
  ui_draw(root);
  color = Color.update(color);
}
