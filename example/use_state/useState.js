exports.useState=function(intialvalue){
  return [intialvalue,function(){}]
}

// exports.renderComponent= function(node_type,props,children){
//   console.log("type", node_type, "props", JSON.stringify(props), "children",JSON.stringify(children) );
//   return ui_create(node_type, props, children);
// }
//
exports.renderComponent  = function(node,props,children){
  if(typeof node==="function"){
    console.log("cmpt found");
    return node(props, children);
  }
  var id = ui_create(node,props,children)
  console.log("id",id,"node", node, "props", JSON.stringify(props), "children",JSON.stringify(children) );
  return id;
}

exports.startApp=function(root){
  ui_compute(root);
  ui_draw(root);
  ui_clear();
}
