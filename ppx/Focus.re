open Longident;
open Location;
open Asttypes;
open Parsetree;
open Ast_helper;

let fail = (loc, txt) => raise(Location.Error(Location.error(~loc, txt)));
let lid = (~loc=default_loc^, s) => mkloc(Longident.parse(s), loc);
let evar = (~loc=?, ~attrs=?, s) =>
  Exp.ident(~loc?, ~attrs?, lid(~loc?, s));

let updated_record = (record, field, value) =>
  Exp.mk(
    Pexp_record(
      [
        (
          mknoloc(Lident(field)),
          Exp.mk(Pexp_ident(mknoloc(Lident(value)))),
        ),
      ],
      Some(Exp.mk(Pexp_ident(mknoloc(Lident(record))))),
    ),
  );

let createFieldLens = name => [%expr
  {
    get: r => [%e Exp.field(evar("r"), mknoloc(Lident(name)))],
    set: (v, r) => [%e updated_record("r", name, "v")],
  }
];

let focus_mapper =
  Parsetree.{
    ...Ast_mapper.default_mapper,
    expr: (mapper, expr) => [%expr
      {
        open Lens;
        let%e rec recurseField =
          fun
          | {
              pexp_desc:
                Pexp_field(
                  {pexp_desc: Pexp_ident({txt: Lident(name)})},
                  {txt: Lident(snd_name)},
                ) |
                Pexp_apply(
                  {pexp_desc: Pexp_ident({txt: Lident("$.")})},
                  [
                    (_, {pexp_desc: Pexp_ident({txt: Lident(name)})}),
                    (_, {pexp_desc: Pexp_ident({txt: Lident(snd_name)})}),
                  ],
                ),
            } => [%expr
              [%e createFieldLens(snd_name)]
            ]
          | {
              pexp_desc:
                Pexp_apply(
                  {pexp_desc: Pexp_ident({txt: Lident("$.")})},
                  [
                    (_, {pexp_desc: Pexp_ident({txt: Lident(name)})}),
                    (_, next),
                  ],
                ),
            } => {
              %expr
              [%e recurseField(next)]
            }
          | {
              pexp_desc:
                Pexp_field(next, {txt: Lident(name)}) |
                Pexp_apply(
                  {pexp_desc: Pexp_ident({txt: Lident("$.")})},
                  [
                    (_, next),
                    (_, {pexp_desc: Pexp_ident({txt: Lident(name)})}),
                  ],
                ),
            } => [%expr
              [%e recurseField(next)] |-- [%e createFieldLens(name)]
            ]

          | {
              pexp_desc:
                Pexp_apply(
                  {pexp_desc: Pexp_ident({txt: Lident("$.")})},
                  [(_, left), (_, right)],
                ),
            } => [%expr
              [%e recurseField(left)] |-? [%e recurseField(right)]
            ]

          | x => Ast_mapper.default_mapper.expr(mapper, x);

        recurseField(expr);
      }
    ],
  };

let mapper = _argv =>
  Parsetree.{
    ...Ast_mapper.default_mapper,
    expr: (mapper, expr) =>
      switch (expr.pexp_desc) {
      | Pexp_extension((
          {txt: "focus", loc},
          PStr([{pstr_desc: Pstr_eval(expr, attributes)}]),
        )) =>
        focus_mapper.expr(focus_mapper, expr)
      | _ => Ast_mapper.default_mapper.expr(mapper, expr)
      },
  };

let () = Ast_mapper.run_main(mapper);
