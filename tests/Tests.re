/* helpers */
let string_of_option = x =>
  switch (x) {
  | None => "None"
  | Some(x) => "Some(" ++ x ++ ")"
  };

let opt_map = (f, x) =>
  switch (x) {
  | Some(x) => Some(f(x))
  | None => None
  };

type department = {
  name: string,
  address,
}
and address = {
  street: string,
  street_number: option(int),
};

type person = {
  name: string,
  age: int,
  department,
  address,
  friend: option(person),
};

let streetL = [%focus person.department.address.street];

let streetNumber = [%focus person.address $. street_number];

let addressL = [%focus person.department.address];

let friend = [%focus person $. friend];

let friendStreetNumber = [%focus person $. friend.address $. street_number];

let addressStreetL = [%focus address.street];

let department = {
  name: "eng",
  address: {
    street: "some other street 123",
    street_number: None,
  },
};

let zach = {
  name: "zach",
  age: 42,
  address: {
    street: "some street",
    street_number: Some(100),
  },
  department,
  friend: None,
}

let bob = {
  name: "bob",
  age: 42,
  address: {
    street: "some street",
    street_number: None,
  },
  department,
  friend: Some(zach),
}

let alice = {
  name: "alice",
  age: 24,
  address: {
    street: "some street",
    street_number: Some(13),
  },
  department,
  friend: Some(bob),
};

let f = friend.get(bob);

print_string("bob's street: " ++ streetL.get(bob));
print_newline();

print_string("bob's department street: " ++ addressL.get(bob).street);
print_newline();

print_string(
  "bob's street_number: "
  ++ (streetNumber.get(bob) |> opt_map(string_of_int) |> string_of_option),
);
print_newline();

print_string(
  "alice's friend street_number: "
  ++ (friendStreetNumber.get(alice) |> opt_map(string_of_int) |> string_of_option),
);
print_newline();

print_string(
  "bob's friend street_number: "
  ++ (friendStreetNumber.get(bob) |> opt_map(string_of_int) |> string_of_option),
);
print_newline();

print_string(
  "alice's street_number: "
  ++ (streetNumber.get(alice) |> opt_map(string_of_int) |> string_of_option),
);
print_newline();

print_string(
  "sets alice's street_number: "
  ++ (
    streetNumber.set(None, alice)
    |> streetNumber.get
    |> opt_map(string_of_int)
    |> string_of_option
  ),
);
print_newline();

print_string(
  "bob's department street inline: "
  ++ [%focus person.department.address].get(bob).street,
);
print_newline();

print_string(
  "set bob's street on adress record: "
  ++ addressStreetL.set("Infinite Loop 1", bob.department.address).street,
);
print_newline();

/*
 /* {get: a => a.department, set: (a, value) => {...a, department: value}} |-- {get: a => a.address, set: (a, value) => {...a, address: value}}]
  */
 let l1 = a => a.department;
 let l2 = a => a.address;

 l1 |-- l2;
 compose(l2, l1);
 l1 |- l2;
 l2(l1(x));

 l1 |-- l2;
 compose(l2, l1);
 l1 |- l2;
 switch (l1(x)) {
 | Some(x) => l2(x)
 | None => None
 };

 let l1 = (value, a) => {...a, department: value};
 let l2 = (value, a) => {...a, street: value};

 l1 |-- l2;
 compose(l2, l1);
 l2 |- modify(l1);
 modify(l1, l2(x));

 let modify = (l1, f, a) => {
   let value = l1.get(a);
   switch (value) {
   | Some(x) =>
     let new_value = f(value);
     l1.set(Some(new_value), a);
   | None => l1.set(None, a)
   };
 };

 */
