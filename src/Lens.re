type t('a, 'b) = {
  get: 'a => 'b,
  set: ('b, 'a) => 'a,
};

let (|-) = (f, g, x) => g(f(x));

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

let compose = (l1, l2) => {get: l2.get |- l1.get, set: l1.set |- modify(l2)};

let (--|) = compose;
let (|--) = (l1, l2) => compose(l2, l1);
