//  This header by intention does not have a include guard
//  It is used multiple times inside gsiCallback.h
//
//  It needs these macros to be defined (example for the one argument case)
//   _TMPLARGPART  ", class A1"
//   _FUNCARGLIST  "A1"
//   _CALLARGLIST  ", A1 a1"
//   _SETVALUE     "args.template write<A1> (a1);"

template <class X _COMMA _TMPLARGPART>
void issue (void (X::*) (_FUNCARGLIST) _COMMA _CALLARGLIST) const
{
  SerialArgs args (argsize), ret (retsize);
  _SETVALUE
  call_int (args, ret);
}

template <class X _COMMA _TMPLARGPART>
void issue (void (X::*) (_FUNCARGLIST) const _COMMA _CALLARGLIST) const
{
  SerialArgs args (argsize), ret (retsize);
  _SETVALUE
  call_int (args, ret);
}

template <class X, class R _COMMA _TMPLARGPART>
R issue (R (X::*) (_FUNCARGLIST) _COMMA _CALLARGLIST) const
{
  tl::Heap heap;
  SerialArgs args (argsize), ret (retsize);
  _SETVALUE
  call_int (args, ret);
  return ret.template read<R> (heap);
}

template <class X, class R _COMMA _TMPLARGPART>
R issue (R (X::*) (_FUNCARGLIST) const _COMMA _CALLARGLIST) const
{
  tl::Heap heap;
  SerialArgs args (argsize), ret (retsize);
  _SETVALUE
  call_int (args, ret);
  return ret.template read<R> (heap);
}

