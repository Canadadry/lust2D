try{
var useStateLib = require('useState');
var useState = useStateLib.useState;
var startApp = useStateLib.startApp;
useStateLib.registerFunc(ui_create, ui_draw, ui_clear);

Buton =function(props){
  return <txt msg={props.name} class="grow max-bound-x" font_size={props.size} color="#000"></txt>
}

TodoApp=function (initialData) {
  var state = useState(initialData);
  var todos = state[0];
  var setTodos = state[1];

  return <item>
    <Buton name="add" onClick={function () {setTodos(todos.concat(["task"]));}}></Buton>
    <Buton name="remove" onClick={function () {setTodos(todos.slice(1));}}></Buton>
    {todos.map(function(t){return <text msg={t}></text>})}
  </item>
};

function conf() {
  window.width = 800;
  window.height = 600;
  window.title = "widget";
}

function init(){}

function render() {
  ClearBackground("#ff0");
  try{
    startApp(<TodoApp></TodoApp>);
  }catch(e){
    console.log(e.message, e.stack);
  }
}
}catch(e){
  console.log(e.message, e.stack);
}
