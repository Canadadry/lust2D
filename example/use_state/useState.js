var nextInstanceId = 0;
var instances = {};
var currentInstance = null;
var rootComponent = null;
var rootProps = null;
var loc_createNode = null;
var loc_render = null;
var loc_clear_ui = null;

function useState(initialValue) {
  console.log("useState",initialValue)
  var instance = currentInstance;
  var index = instance.hookIndex;

  if (instance.hooks[index] === undefined) {
    instance.hooks[index] = initialValue;
  }

  function setState(newValue) {
    instance.hooks[index] = newValue;
    scheduleRender();
  }

  var value = instance.hooks[index];
  instance.hookIndex++;

  return [value, setState];
}

function getInstance(type, key) {
  var instanceKey = key != null ? key : type;
  var instance = instances[instanceKey];
  if (!instance) {
    instance = {
      id: nextInstanceId++,
      type: type,
      hooks: [],
      hookIndex: 0,
      nodeId: null
    };
    instances[instanceKey] = instance;
  }
  return instance;
}

function renderComponent(type, props, children) {
  if (props == null) props = {};
  var instance = getInstance(type, props.key);
  console.log("renderComponent",typeof type,'getInstance',JSON.stringify(instance))
  currentInstance = instance;
  instance.hookIndex = 0;

  var nodeDescription = { type: type, props: props,children:children };
  if (typeof type==="function"){
    nodeDescription = type(props,children);
  }
  instance.nodeId = mountNode(nodeDescription);
  return instance.nodeId;
}

function is_not_string(v){
  return typeof v!=="string"
}

function flattenInfinity(arr) {
  var result = [];

  for (var i = 0; i < arr.length; i++) {
    if (Array.isArray(arr[i])) {
      var flat = flattenInfinity(arr[i]);
      for (var j = 0; j < flat.length; j++) {
        result.push(flat[j]);
      }
    } else {
      result.push(arr[i]);
    }
  }

  return result;
}

function mountNode(desc) {
  var childIds = [];
  if(desc.children){
    // console.log("get children");
    childIds = desc.children;
    // console.log("reduce");
    childIds = flattenInfinity(childIds)
    // console.log("filter",typeof childIds,JSON.stringify(childIds));
    childIds = childIds.filter(is_not_string);
    // console.log("map");
    childIds = childIds.map(mountNode);
  }
  if(desc.props===null){
    desc.props = {};
  }
  // var childIds = desc.children ? desc.children.reduce(flatten).filter(is_not_string).map(mountNode) : [];
  // console.log("loc_createNode", desc.type, JSON.stringify(desc.props), JSON.stringify(childIds));//,"from",JSON.stringify(desc));
  return loc_createNode(desc.type, desc.props, childIds);
}

function scheduleRender() {
  loc_clear_ui();
  if (!rootComponent) return;
  var rootNodeId = renderComponent(rootComponent,rootProps);
  // loc_render(rootNodeId);
}

function startApp(App,props) {
  rootComponent = App;
  rootProps = props;
  scheduleRender();
}

function registerFunc(fn1,fn2,fn3){
  loc_createNode = fn1;
  loc_render = fn2;
  loc_clear_ui = fn3;
}

exports.useState= useState;
exports.startApp= startApp;
exports.renderComponent= renderComponent;
exports.setInstances= function(i){
  instances=i
};
exports.scheduleRender=scheduleRender;
exports.registerFunc=registerFunc;
exports.setCurrentInstance= function(instance) { currentInstance = instance; };
exports.getCurrentInstance= function() { return currentInstance; };
