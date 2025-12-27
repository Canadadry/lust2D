exports.useState=function(intialvalue){
  return [intialvalue,function(){}]
}

// exports.renderComponent= function(node_type,props,children){
//   console.log("type", node_type, "props", JSON.stringify(props), "children",JSON.stringify(children) );
//   return ui_create(node_type, props, children);
// }
//
exports.renderComponent = function(node_type, props, children){
  return {
    type: node_type,
    props: props || {},
    children: (children || []).filter(function(c){
      return typeof c === "object";
    })
  };
};

exports.startApp=function(root){
  var root_id = mount(root);
  ui_compute(root_id);
  ui_draw(root_id);
}

function mount(node){
  if (!node) return null;

  if (typeof node.type === "function") {
    node = node.type(node.props || {});
  }

  var childHandles = [];
  for (var i = 0; i < (node.children || []).length; i++) {
    var h = mount(node.children[i]);
    if (h !== null) childHandles.push(h);
  }

  console.log("type", node.type, "props", JSON.stringify(node.props), "children", childHandles);

  return ui_create(node.type, node.props, childHandles);
}
