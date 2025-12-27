exports.useState=function(intialvalue){
  return [intialvalue,function(){}]
}

exports.startApp=function(root){
  ui_compute(root);
  ui_draw(root);
}

exports.renderComponent= function(node_type,props,children){
  console.log("type", node_type, "props", props, "children",children );
  return ui_create(node_type, props, children);
}
