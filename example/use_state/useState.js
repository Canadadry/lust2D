var currentComponentPath = "";
var componentHooks = {};
var hookIndex = 0;

exports.useState = function (initialValue) {
  if (currentComponentPath === "") {
    throw "useState must be called inside a component";
  }

  var hooks = componentHooks[currentComponentPath];

  if (!hooks) {
    hooks = [];
    componentHooks[currentComponentPath] = hooks;
  }

  if (hooks[hookIndex] === undefined) {
    hooks[hookIndex] = initialValue;
  }

  var setState = function (value) {
    hooks[hookIndex] =
      typeof value === "function"
        ? value(hooks[hookIndex])
        : value;
  };

  hookIndex++;

  return [hooks[hookIndex-1], setState];
};

exports.renderComponent = function (node, props, children) {
  if (typeof node === "function") {
    var prevHookIndex = hookIndex;
    var previsousPath = currentComponentPath;

    currentComponentPath = currentComponentPath+(currentComponentPath==""?"":".")+props.__cid
    if(props.key !== undefined){
      currentComponentPath = currentComponentPath +"."+ props.key;
    }
    hookIndex = 0;
    // console.log("component", currentComponentPath);

    var result = node(props || {}, children);

    currentComponentPath = previsousPath;
    hookIndex = prevHookIndex;

    return result;
  }
  var id = ui_create(node,props,children)
  // console.log("id",id,"node", node, "props", JSON.stringify(props), "children",JSON.stringify(children) );
  return id;
};

exports.startApp = function (root) {
  ui_compute(root);
  ui_draw(root);
  ui_clear();
}
