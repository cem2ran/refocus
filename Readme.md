# Refocus

Lens creation made Reasonable™

## What is a Lens

1. Lenses are simply collections of utilities that help you manipulate deeply nested immutable data
structures: `let {get, set, modify...} = lens`

2. Lenses compose. If you have a lens for manipulating the address of a person (person.address) and
you have a lens for manipulating the street_number of an address (address.street_number) you also,
have a lens that can operate on the street_number of a persons address (person.address.street_number)
by simply composing your two lenses: `addressLens + street_number = person_street_number`