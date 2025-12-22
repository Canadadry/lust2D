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
  },
  "counter_two_increments": {
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
    update: function () {
      nodes[0].props.onClick();
      nodes[0].props.onClick();
    },
    expected_callstack: [
      "clear_ui",
      "createNode:text:0:count = 0",
      "render:text:count = 0",
      "clear_ui",
      "createNode:text:0:count = 1",
      "render:text:count = 1",
      "clear_ui",
      "createNode:text:0:count = 2",
      "render:text:count = 2"
    ]
  },
  "multiple_useState_same_component": {
    init: function () {

      function Counter() {
        var a = useState(0);
        var b = useState(10);

        return {
          type: "text",
          props: {
            value: "a=" + a[0] + ",b=" + b[0],
            onClick: function () {
              a[1](a[0] + 1);
            }
          },
          children: []
        };
      }

      startApp(Counter);
    },
    update: function () {
      nodes[0].props.onClick();
    },
    expected_callstack: [
      "clear_ui",
      "createNode:text:0:a=0,b=10",
      "render:text:a=0,b=10",
      "clear_ui",
      "createNode:text:0:a=1,b=10",
      "render:text:a=1,b=10"
    ]
  },
  "two_counters_independent_state": {
    init: function () {

      function Counter(label) {
        var state = useState(0);
        return {
          type: "text",
          props: {
            value: label + ":" + state[0],
            onClick: function () { state[1](state[0] + 1); }
          },
          children: []
        };
      }

      function App() {
        return {
          type: "container",
          props: {},
          children: [
            Counter("A"),
            Counter("B")
          ]
        };
      }

      startApp(App);
    },
    update: function () {
      nodes[1].props.onClick();
    },
    expected_callstack: [
      "clear_ui",
      "createNode:text:0:A:0",
      "createNode:text:1:B:0",
      "createNode:container:2:undefined",
      "render:container:undefined",
      "render:text:A:0",
      "render:text:B:0",
      "clear_ui",
      "createNode:text:0:A:0",
      "createNode:text:1:B:1",
      "createNode:container:2:undefined",
      "render:container:undefined",
      "render:text:A:0",
      "render:text:B:1"
    ]
  },
  "no_update_no_rerender": {
    init: function () {

      function Counter() {
        useState(0);
        return {
          type: "text",
          props: {
            value: "static",
            onClick: function () {}
          },
          children: []
        };
      }

      startApp(Counter);
    },
    update: function () {
    },
    expected_callstack: [
      "clear_ui",
      "createNode:text:0:static",
      "render:text:static"
    ]
  },
  "todo_add_and_remove": {
    init: function () {

      function TodoApp() {
        var state = useState([]);
        var todos = state[0];
        var setTodos = state[1];

        return {
          type: "container",
          props: {},
          children: [
            {
              type: "button",
              props: {
                value: "add",
                onClick: function () {
                  setTodos(todos.concat(["task"]));
                }
              },
              children: []
            },
            {
              type: "button",
              props: {
                value: "remove",
                onClick: function () {
                  setTodos(todos.slice(1));
                }
              },
              children: []
            }
          ].concat(
            todos.map(function (t) {
              return {
                type: "text",
                props: { value: t },
                children: []
              };
            })
          )
        };
      }

      startApp(TodoApp);
    },

    update: function () {
      nodes[0].props.onClick();
      nodes[1].props.onClick();
    },

    expected_callstack: [
      "clear_ui",
      "createNode:button:0:add",
      "createNode:button:1:remove",
      "createNode:container:2:undefined",
      "render:container:undefined",
      "render:button:add",
      "render:button:remove",

      "clear_ui",
      "createNode:button:0:add",
      "createNode:button:1:remove",
      "createNode:text:2:task",
      "createNode:container:3:undefined",
      "render:container:undefined",
      "render:button:add",
      "render:button:remove",
      "render:text:task",

      "clear_ui",
      "createNode:button:0:add",
      "createNode:button:1:remove",
      "createNode:container:2:undefined",
      "render:container:undefined",
      "render:button:add",
      "render:button:remove"
    ]
  },
  "todo_insert_middle": {
    init: function () {

      function TodoApp() {
        var state = useState(["A", "B", "C"]);
        var todos = state[0];
        var setTodos = state[1];

        return {
          type: "container",
          props: {},
          children: [
            {
              type: "button",
              props: {
                value: "insert",
                onClick: function () {
                  var next = todos.slice(0, 1)
                    .concat(["X"])
                    .concat(todos.slice(1));
                  setTodos(next);
                }
              },
              children: []
            }
          ].concat(
            todos.map(function (t) {
              return {
                type: "text",
                props: { value: t },
                children: []
              };
            })
          )
        };
      }

      startApp(TodoApp);
    },

    update: function () {
      nodes[0].props.onClick();
    },

    expected_callstack: [
      "clear_ui",
      "createNode:button:0:insert",
      "createNode:text:1:A",
      "createNode:text:2:B",
      "createNode:text:3:C",
      "createNode:container:4:undefined",
      "render:container:undefined",
      "render:button:insert",
      "render:text:A",
      "render:text:B",
      "render:text:C",

      "clear_ui",
      "createNode:button:0:insert",
      "createNode:text:1:A",
      "createNode:text:2:X",
      "createNode:text:3:B",
      "createNode:text:4:C",
      "createNode:container:5:undefined",
      "render:container:undefined",
      "render:button:insert",
      "render:text:A",
      "render:text:X",
      "render:text:B",
      "render:text:C"
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
