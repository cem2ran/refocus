# Refocus

Lenses made Reasonable™

## Problem

Natively within ReasonML & OCaml you can use record spread to update a record field immutably, but this quickly becomes unwieldy when the field in question is deeply nested:

```reasonml

// Quite easy
let personWithNewName = {
    ...person, name: "Bob"
};

// Becoming unwieldy...
let personWithNewAddress = {
    ...person, address : {
        ...person.address: {
            street: "Unreasonable Street"
        }
    }
};

// Terrible.
let personWithNewDepartmentAddress = {
    ...person, department: {
        ...person.department, address : {
            ...person.department.address: {
                street: "Unreasonable Street"
            }
        }
    }
};
```

## What is a Lens

Lenses can help deal with the above boilerplate, and comes with some nice properties:

1. Lenses are simply put a collections of utilities that help you manipulate deeply nested immutable data structures: `let {get, set, update, ...} = lens`

2. Lenses compose! If you have a lens for manipulating the `address` of a `person` (`person.address`) and you have a lens for manipulating the `street` of an address (`address.street`) you also, have a lens that can operate on the street of a persons address (`person.address.street`) by simply composing your two lenses: `addressLens + streetLens = personStreetLens`

## Usage

Given that you have record types that are nested:

```reasonml
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
```

And one such instance

```reasonml

let person = {
  name: "bob",
  age: 42,
  address: {
    street: "some street",
    street_number: None,
  },
  department: {
    name: "eng",
    address: {
        street: "Infinite Loop",
        street_number: Some(1),
    }
  }
};
```

We can create a lens, with `%focus`, for getting and immutably setting a deeply nested value, e.g. `person.department.address.street`:

```reasonml
let streetL = [%focus person.department.address.street];

streetL.get(person); // > "Infinite Loop"

let updatedPerson = streetL.set("Reasonable Street", person)

streetL.get(updatePerson); // > "Reasonable Street"
```

## Existing solutions

## Lens creation ala Ramda `lensPath`
If you're coming from JS you might have stumbled upon Ramda [lensPath](https://ramdajs.com/docs/#lensPath) and functions to act on paths such as `view`, `set` and `over`.

```javascript
const lens = R.lensPath(["department", "address", "street"]);

const updatedPerson = R.set(lens, "Javascript Ave", person);
```

This is quite convenient, but isn't type-safe. We are only ably to verify that such a path is valid during runtime. 

Other projects exists that provide a more convenient way of specifying the path and generating a lens through proxies, but as far as I understand this may still be unsafe: [yelouafi/focused](https://github.com/yelouafi/focused)

## ocaml-lens

[OCaml-lens](https://github.com/pdonadeo/ocaml-lens) provides a way to generate lenses by annotating records for which one requires lenses. 

```reasonml
[@deriving lens]
type person = {
  name: string,
  age: int,
  ...
};
```

This creates lenses for individual fields of a record and provides a custom operator for composing lenses: `department_lens |-- address_lens |-- street_lens`

## Scala Goggles
The Scala community has a library that provides a convenience DSL on top of the Monocle lens library, which provides a similar way of defining and composing lenses like the `refocus` project:

```scala
set"$person.department.address.street" := "Scala Blvd"
```

`Refocus` is in fact heavily inspired by [kenbot/goggles](https://github.com/kenbot/goggles)

Talk about the rationale and implementation of Goggles: https://youtu.be/t2WTtIwgdLc?t=203