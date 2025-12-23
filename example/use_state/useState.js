var nextInstanceId = 0;
var instances = new Map();
var currentInstance = null;
var rootComponent = null;
var rootProps = null;

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
  var instance = instances.get(instanceKey);
  if (!instance) {
    instance = {
      id: nextInstanceId++,
      type: type,
      hooks: [],
      hookIndex: 0,
      nodeId: null
    };
    instances.set(instanceKey, instance);
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
  return createNode(desc.type, desc.props, childIds);
}

function scheduleRender() {
  clear_ui();
  if (!rootComponent) return;
  var rootNodeId = renderComponent(rootComponent,rootProps);
  render(rootNodeId);
}

function startApp(App,props) {
  rootComponent = App;
  rootProps = props;
  scheduleRender();
}

module.exports = {
  useState: useState,
  startApp: startApp,
  renderComponent: renderComponent,
  instances: instances,
  scheduleRender:scheduleRender,
  setCurrentInstance: function(instance) { currentInstance = instance; },
  getCurrentInstance: function() { return currentInstance; }
};
