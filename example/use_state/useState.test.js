try {
  var lib = require('useState');
  console.log(lib, JSON.stringify(lib));
  var useState = lib.useState;
  var startApp = lib.startApp;
  var setInstances = lib.setInstances;
  var registerFunc = lib.registerFunc;

  var nodes = {};
  var nextNodeId = 0;
  var callstack = [];

  clear_ui = function() {
    nodes = {};
    nextNodeId = 0;
    callstack.push("clear_ui");
  }

  createNode= function (type, props, children) {
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

  render=function (nodeId) {
    var node = nodes[nodeId];
    callstack.push("render:" + node.type + ":" + node.props.value);
    if (node.children) {
      for (var i = 0; i < node.children.length; i++) {
        render(node.children[i]);
      }
    }
  }
  registerFunc(createNode, render, clear_ui);

  Counter = function () {
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
  };

  TodoApp=function (initialData) {
    var state = useState(initialData);
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


  var tests = {
    "counter_initial_and_increment": {
      init: function () {
        startApp(Counter);
      },
      update: function () {
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
              onClick: function () { }
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

        startApp(TodoApp, []);
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
        startApp(TodoApp, ["A", "B", "C"]);
      },

      update: function () {
        nodes[0].props.onClick();
      },

      expected_callstack: [
        'clear_ui',
        'createNode:button:0:add',
        'createNode:button:1:remove',
        'createNode:text:2:A',
        'createNode:text:3:B',
        'createNode:text:4:C',
        'createNode:container:5:undefined',
        'render:container:undefined',
        'render:button:add',
        'render:button:remove',
        'render:text:A',
        'render:text:B',
        'render:text:C',

        'clear_ui',
        'createNode:button:0:add',
        'createNode:button:1:remove',
        'createNode:text:2:A',
        'createNode:text:3:B',
        'createNode:text:4:C',
        'createNode:text:5:task',
        'createNode:container:6:undefined',
        'render:container:undefined',
        'render:button:add',
        'render:button:remove',
        'render:text:A',
        'render:text:B',
        'render:text:C',
        'render:text:task'
      ]
    }
  };

  var keys = Object.keys(tests);
  for (var i = 0; i < keys.length;i++) {
    var name = keys[i];
    var tt = tests[name];
    nodes = {};
    nextNodeId = 0;
    setInstances({});
    rootComponent = null;
    callstack = [];

    tt.init();
    tt.update();

    if(JSON.stringify(callstack)!=JSON.stringify(tt.expected_callstack)){
      console.log("Test " + name + " failed",
        "got",JSON.stringify(callstack,null,2),
        "want",JSON.stringify(tt.expected_callstack,null,2)
      );
      break
    }
    console.log("Test " + name + " passed");
  }
}catch(e){
  console.log(e.message, e.stack);
}
