open Jest;
open Expect;
open ReactTestingLibrary;
open Next;

module RouterContextProvider = {
  [@bs.deriving abstract]
  type t = {
    route: string,
    pathname: string,
    query: QueryString.t,
    asPath: string,
  };

  [@bs.module "next-server/dist/lib/router-context"]
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

test("Link", () =>
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
);

test("Head", () =>
  render(<Head> <meta name="name" content="nextjs" /> </Head>)
  |> container
  |> expect
  |> ExpectJs.toBeTruthy
);

test("Error", () =>
  render(<Error statusCode=500 />)
  |> getByText(~matcher=`Str("500"))
  |> expect
  |> ExpectJs.toBeTruthy
);

test("QueryString", () =>
  QueryString.parse("/a/bc") |> expect |> ExpectJs.toBeTruthy
);

test("Router", () => {
  let fakeRoute =
    RouterContextProvider.t(
      ~route="index",
      ~pathname="/",
      ~query=QueryString.parse("/"),
      ~asPath="/",
    );

  module TestComponent = {
    [@react.component]
    let make = () => {
      let router = Next.Router.useRouter();
      switch (router) {
      | Some(router) => router##route->React.string
      | None => "got no route"->React.string
      };
    };
  };

  render(
    <RouterContextProvider value=fakeRoute>
      <TestComponent />
    </RouterContextProvider>,
  )
  |> getByText(~matcher=`Str("index"))
  |> expect
  |> ExpectJs.toBeTruthy;
});
