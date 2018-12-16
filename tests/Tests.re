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
};

let streetL = [%focus person.department.address.street];

let streetNumber = [%focus person.address.street_number];

let addressL = [%focus person.department.address];

let addressStreetL = [%focus address.street];

let department1 = {
  name: "eng",
  address: {
    street: "some other street 123",
    street_number: None,
  },
};

let person1 = {
  name: "bob",
  age: 42,
  address: {
    street: "some street",
    street_number: None,
  },
  department: department1,
};

print_string("street: " ++ streetL.get(person1));
print_newline();

print_string("department street: " ++ addressL.get(person1).street);
print_newline();

print_string(
  "set street on adress record: " ++ addressStreetL.set("Infinite Loop 1", person1.department.address).street,
);
print_newline();
