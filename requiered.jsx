exports.test = "test";
exports.create = function () {
  return (
    <rectangle x={10} w={fit()} h={fit()} margin={10} padding={10} r={0} g={255} b={0}>
      <rectangle w={50} h={50} r={0} g={0} b={255}></rectangle>
      <rectangle w={10} h={150} r={255} g={0} b={255}></rectangle>
    </rectangle>
  );
};
