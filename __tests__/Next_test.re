open Jest;
open Expect;
open ReactTestingLibrary;
open Next;

describe("Link", () =>
  test("renders", () =>
    render(
      <Link
        href="http://www.fake.com"
        _as="http://www.another.com"
        replace=false
        shallow=false
        passHref=false>
        "hello"->React.string
      </Link>,
    )
    |> getByText(~matcher=`Str("hello"))
    |> expect
    |> ExpectJs.toBeTruthy
  )
);

describe("Head", () =>
  test("renders", () =>
    render(<Head> <meta name="name" content="nextjs" /> </Head>)
    |> container
    |> expect
    |> ExpectJs.toBeTruthy
  )
);

describe("Error", () =>
  test("renders", () =>
    render(<Error statusCode=500 />)
    |> getByText(~matcher=`Str("500"))
    |> expect
    |> ExpectJs.toBeTruthy
  )
);

describe("QueryString", () =>
  test("renders", () =>
    QueryString.parse("/a/bc") |> expect |> ExpectJs.toBeTruthy
  )
);

describe("Router", () => {
  module RouterContextProvider = {
    type t = {
      route: string,
      pathname: string,
      query: QueryString.t,
      asPath: string,
      push: unit => unit,
    };

    [@bs.module "next/dist/next-server/lib/router-context"]
    external context: React.Context.t(t) = "RouterContext";

    let make = context->React.Context.provider;

    [@bs.obj]
    external makeProps:
      (~value: t, ~children: React.element, ~key: string=?, unit) =>
      {
        .
        "value": t,
        "children": React.element,
      } =
      "";
  };

  let fakeRoute = push =>
    RouterContextProvider.{
      route: "index",
      pathname: "/",
      query: QueryString.parse("/"),
      asPath: "/",
      push,
    };

  module TestComponent = {
    [@react.component]
    let make = () => {
      let router = Next.Router.useRouter();

      let onClick = event => {
        event |> ReactEvent.Mouse.preventDefault;
        router->Belt.Option.map(router =>
          Next.Router.push(router, ~url="/test")
        )
        |> ignore;
      };

      switch (router) {
      | Some(router) => <a onClick> {router##route->React.string} </a>
      | None => "got no route"->React.string
      };
    };
  };

  test("useRouter", () =>
    render(
      <RouterContextProvider value={fakeRoute(_ => ())}>
        <TestComponent />
      </RouterContextProvider>,
    )
    |> getByText(~matcher=`Str("index"))
    |> expect
    |> ExpectJs.toBeTruthy
  );

  test("push", () => {
    let spy = JestJs.inferred_fn();

    let fn = spy |> MockJs.fn;
    let onClick = _ => fn(. "pushed!") |> ignore;

    let node =
      render(
        <RouterContextProvider value={fakeRoute(onClick)}>
          <TestComponent />
        </RouterContextProvider>,
      )
      |> getByText(~matcher=`Str("index"));

    FireEvent.click(node);

    expect(spy |> MockJs.calls) |> toEqual([|"pushed!"|]);
  });
});
