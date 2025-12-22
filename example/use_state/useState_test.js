var assert = require('assert');

var lib = require('./useState.js');
var useState = lib.useState;
var startApp = lib.startApp;
var instances = lib.instances;

var nodes = {};
var nextNodeId = 0;
var callstack = [];

function clear_ui() {
  nodes = {};
  nextNodeId = 0;
  callstack.push("clear_ui");
}

function createNode(type, props, children) {
  var id = nextNodeId++;
  nodes[id] = {
    id: id,
    type: type,
    props: props,
    children: children
  };
  callstack.push("createNode:" + type + ":" + id + ":" + props.value);
  return id;
}

function render(nodeId) {
  var node = nodes[nodeId];
  callstack.push("render:" + node.type + ":" + node.props.value);
  if (node.children) {
    for (var i = 0; i < node.children.length; i++) {
      render(node.children[i]);
    }
  }
}

global.createNode = createNode;
global.render = render;
global.clear_ui = clear_ui;

var tests = {
  "counter_initial_and_increment": {
    init: function () {

      function Counter() {
        var state = useState(0);
        var count = state[0];
        var setCount = state[1];
        return {
          type: "text",
          props: {
            value: "count = " + count,
            onClick: function () { setCount(count + 1); }
          },
          children: []
        };
      }

      startApp(Counter);
    },
    update:function(){
      nodes[0].props.onClick();
    },
    expected_callstack: [
      "clear_ui",
      "createNode:text:0:count = 0",
      "render:text:count = 0",
      "clear_ui",
      "createNode:text:0:count = 1",
      "render:text:count = 1"
    ]
  }
};

Object.keys(tests).forEach(function(name) {
  console.log("=== Running test:", name, "===");

  nodes = {};
  nextNodeId = 0;
  instances.clear();
  rootComponent = null;
  callstack = [];

  tests[name].init();
  tests[name].update();

  var expectedFinalCallstack = tests[name].expected_callstack;
  assert.deepStrictEqual(callstack, expectedFinalCallstack, "Test " + name + " failed");

  console.log("Test " + name + " passed");
});
