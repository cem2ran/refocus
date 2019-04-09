type t('a, 'b) = {
  get: 'a => 'b,
  set: ('b, 'a) => 'a,
};

let (|-) = (f, g, x) => g(f(x));
let (|?) = (f, g, x) =>
  switch (f(x)) {
  | Some(x) => g(x)
  | None => None
  };

let over = (l, f, a) => {
  let value = l.get(a);
  let new_value = f(value);
  new_value;
};

let modify = (l, f, a) => {
  let value = l.get(a);
  let new_value = f(value);
  l.set(new_value, a);
};

let flatMap = (f, x) =>
  switch (x) {
  | Some(x) => f(x)
  | None => None
  };

let optionalModify = (l, f, a) => {
  let value = l.get(a);
  switch (value) {
  | Some(x) =>
    let new_value = f(x);
    l.set(Some(new_value), a);
  | None => a
  };
};

let compose = (l1, l2) => {
  get: l2.get |- l1.get,
  set: l1.set |- modify(l2),
};

let optional = (l1, l2) => {
  get: l2.get |? l1.get,
  set: l1.set |- optionalModify(l2),
};

let (--|) = compose;
let (|--) = (l1, l2) => compose(l2, l1);
let (|-?) = (l1, l2) => optional(l2, l1);
