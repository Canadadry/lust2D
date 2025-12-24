var nextInstanceId = 0;
var instances = {};
var currentInstance = null;
var rootComponent = null;
var rootProps = null;
var loc_createNode = null;
var loc_render = null;
var loc_clear_ui = null;

function useState(initialValue) {
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

function renderComponent(type, props, key) {
  if (props == null) props = {};
  var instance = getInstance(type, key);

  currentInstance = instance;
  instance.hookIndex = 0;

  var nodeDescription = type(props);

  currentInstance = null;

  instance.nodeId = mountNode(nodeDescription);
  return instance.nodeId;
}

function mountNode(desc) {
  var childIds = desc.children ? desc.children.map(mountNode) : [];
  return loc_createNode(desc.type, desc.props, childIds);
}

function scheduleRender() {
  loc_clear_ui();
  if (!rootComponent) return;
  var rootNodeId = renderComponent(rootComponent,rootProps);
  loc_render(rootNodeId);
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
exports.instances= instances;
exports.scheduleRender=scheduleRender;
exports.registerFunc=registerFunc;
exports.setCurrentInstance= function(instance) { currentInstance = instance; };
exports.getCurrentInstance= function() { return currentInstance; };
