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

/*let streetNumber = [%focus person.address?street_number];*/

let addressL = [%focus person.department.address];

let addressStreetL = [%focus address.street];

let department = {
  name: "eng",
  address: {
    street: "some other street 123",
    street_number: None,
  },
};

let person = {
  name: "bob",
  age: 42,
  address: {
    street: "some street",
    street_number: None,
  },
  department,
};

print_string("street: " ++ streetL.get(person));
print_newline();

print_string("department street: " ++ addressL.get(person).street);
print_newline();

print_string("department street inline: " ++ [%focus person.department.address].get(person).street);
print_newline();

print_string(
  "set street on adress record: " ++ addressStreetL.set("Infinite Loop 1", person.department.address).street,
);
print_newline();
