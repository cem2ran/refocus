open Longident;
open Location;
open Asttypes;
open Parsetree;
open Ast_helper;

let fail = (loc, txt) => raise(Location.Error(Location.error(~loc, txt)));
let lid = (~loc=default_loc^, s) => mkloc(Longident.parse(s), loc);
let evar = (~loc=?, ~attrs=?, s) => Exp.ident(~loc?, ~attrs?, lid(~loc?, s));

let updated_record = (record, field, value) =>
  Exp.mk(
    Pexp_record(
      [(mknoloc(Lident(field)), Exp.mk(Pexp_ident(mknoloc(Lident(value)))))],
      Some(Exp.mk(Pexp_ident(mknoloc(Lident(record))))),
    ),
  );

let createLens = name => [%expr
  {
    get: r => [%e Exp.field(evar("r"), mknoloc(Lident(name)))],
    set: (v, r) => [%e updated_record("r", name, "v")],
  }
];

let focus_mapper =
  Parsetree.{
    ...Ast_mapper.default_mapper,
    expr: (mapper, expr) =>
      switch (expr.pexp_desc) {
      | Pexp_field(pexp, {txt}) =>
        %expr
        {
          open Lens;
          let%e rec recurseField = (
            fun
            | {pexp_desc: Pexp_field({pexp_desc: Pexp_ident({txt: Lident(name)})}, {txt: Lident(snd_name)})} => {
                print_string("base case: \n");
                print_string("name: " ++ name);
                print_newline();
                print_string("snd_name: " ++ snd_name);
                print_newline();

                %expr
                [%e createLens(snd_name)];
              }
            | {pexp_desc: Pexp_field(next, {txt: Lident(name)})} => [%expr
                [%e recurseField(next)] |-- [%e createLens(name)]
              ]
            | x => Ast_mapper.default_mapper.expr(mapper, x)
          );

          recurseField(expr);
        }
      | _ => fail(expr.pexp_loc, "invalid lens declaration")
      },
  };

let mapper = _argv =>
  Parsetree.{
    ...Ast_mapper.default_mapper,
    expr: (mapper, expr) =>
      switch (expr.pexp_desc) {
      | Pexp_extension(({txt: "focus", loc}, PStr([{pstr_desc: Pstr_eval(expr, attributes)}]))) =>
        focus_mapper.expr(focus_mapper, expr)
      | _ => Ast_mapper.default_mapper.expr(mapper, expr)
      },
  };

let () = Ast_mapper.run_main(mapper);
