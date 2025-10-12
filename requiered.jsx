exports.test = "test";
exports.create = function () {
  return (
    <rectangle x={10} class="fit m-2 p-2" r={0} g={255} b={0}>
      <rectangle id="first" w={50} h={50} r={0} g={0} b={255}></rectangle>
      <rectangle id="second" w={10} h={150} r={255} g={0} b={255}></rectangle>
      <img src="spritesheet.png" class="grow"></img>
    </rectangle>
  );
};
