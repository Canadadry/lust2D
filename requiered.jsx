exports.test = "test";
exports.create = function () {
  return (
    <rectangle x={10} h={200} r={0} g={255} b={0}>
      <rectangle x={10} h={50} r={0} g={0} b={255}></rectangle>
      <rectangle x={10} h={150} r={255} g={0} b={255}></rectangle>
    </rectangle>
  );
};
