exports.test = "test";
var Button = function (props,children) {
  if(props.id==undefined){
    props.id = "btn";
  }
  return <rectangle class="fit m-2 p-2" x={props.x} r={props.r} g={props.g} b={props.b}>
    {children}
  </rectangle>
};

exports.create = function () {
  return (
    <Button x={10} r={0} g={255} b={0}>
      <rectangle id="first" w={50} h={50} r={0} g={0} b={255}></rectangle>
      <rectangle id="second" w={10} h={150} r={255} g={0} b={255}></rectangle>
      <img src="spritesheet.png" class="grow"></img>
    </Button>
  );
};
