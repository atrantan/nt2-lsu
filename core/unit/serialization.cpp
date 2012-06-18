//==============================================================================
//         Copyright 2003 - 2011   LASMEA UMR 6602 CNRS/Univ. Clermont II
//         Copyright 2009 - 2011   LRI    UMR 8623 CNRS/Univ Paris Sud XI
//
//          Distributed under the Boost Software License, Version 1.0.
//                 See accompanying file LICENSE.txt or copy at
//                     http://www.boost.org/LICENSE_1_0.txt
//==============================================================================

#define NT2_UNIT_MODULE "nt2::core serialization"

#include <nt2/table.hpp>
#include <nt2/core/container/io/serialization.hpp>
#include <nt2/core/utility/of_size.hpp>
#include <nt2/include/functions/repnum.hpp>
#include <nt2/include/functions/numel.hpp>
#include <nt2/include/functions/plus.hpp>
#include <nt2/include/functions/multiplies.hpp>
#include <nt2/core/container/dsl.hpp>

#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/iostreams/stream_buffer.hpp> 
#include <boost/iostreams/stream.hpp> 
#include <boost/iostreams/device/back_inserter.hpp> 
#include <boost/proto/debug.hpp>
#include <boost/proto/make_expr.hpp>
#include <boost/type_traits/remove_const.hpp>

#include <vector>
#include <iostream>

#include <nt2/sdk/unit/module.hpp>
#include <nt2/sdk/unit/tests/relation.hpp>
#include <nt2/sdk/unit/tests/basic.hpp>

//==============================================================================
// Test for save and load table in an archive
//==============================================================================
NT2_TEST_CASE_TPL( serialization_table, NT2_TYPES)
{
  using boost::archive::binary_oarchive;
  using boost::archive::binary_iarchive;
  using boost::iostreams::stream;
  using boost::iostreams::back_insert_device;
  using boost::iostreams::basic_array_source;
  using nt2::table;
  using nt2::of_size;
  
  typedef std::vector<char> buffer_type;

  buffer_type buffer;
  table<T> out(of_size(16));
  table<T> in(of_size(16));
  out = nt2::repnum(T(12), of_size(16));
  stream< back_insert_device<buffer_type> > output_stream(buffer);
  
  {// save  
    binary_oarchive oa(output_stream);
    oa << out;
  }
  
  basic_array_source<char> source(&buffer[0],buffer.size()); 
  stream< basic_array_source<char> > input_stream(source); 

  {// load  
    binary_iarchive ia(input_stream); 
    ia >> in;
  }

  for(std::size_t i = 0; i < nt2::numel(out); ++i)
    NT2_TEST_EQUAL(out(i), in(i));
}

//==============================================================================
//Test for save and load expression in an archive
//==============================================================================
NT2_TEST_CASE_TPL( serialization_expression, (double))//NT2_TYPES)
{
  using boost::archive::binary_oarchive;
  using boost::archive::binary_iarchive;
  using boost::iostreams::stream;
  using boost::iostreams::back_insert_device;
  using boost::iostreams::basic_array_source;
  using nt2::table;
  using nt2::of_size;
  
  typedef std::vector<char> buffer_type;

  buffer_type buffer;

  table<T> aout(of_size(16));
  table<T> bout(of_size(16));
  table<T> cout(of_size(16));

  aout = nt2::repnum(T(12), of_size(16));
  bout = nt2::repnum(T(3.5), of_size(16));
  cout = nt2::repnum(T(4.6), of_size(16));

  typedef typename boost::proto::result_of::
  make_expr< nt2::tag::plus_
           , nt2::container::domain
           , typename boost::proto::result_of::
             make_expr< nt2::tag::multiplies_
                      , nt2::container::domain
                      , table<T> const&
                      , table<T> const&
                      >::type
           , table<T> const&
           >::type expr_type;

  expr_type expr_out  = boost::proto::
                        make_expr< nt2::tag::plus_
                                 , nt2::container::domain
                                 >( boost::cref( boost::proto::
                                                 make_expr< nt2::tag::multiplies_
                                                          , nt2::container::domain
                                                          >( boost::cref(aout)
                                                           , boost::cref(bout)))
                                  , boost::cref(cout));

  boost::proto::display_expr(expr_out);

  typedef typename boost::proto::result_of::
  make_expr< nt2::tag::plus_
           , nt2::container::domain
           , typename boost::proto::result_of::
             make_expr< nt2::tag::multiplies_
                      , nt2::container::domain
                      , table<T> &
                      , table<T> &
                      >::type
           , table<T> &
           >::type expr_type_in;
  
  expr_type_in expr_in;

  stream< back_insert_device<buffer_type> > output_stream(buffer);
  
  {// save  
    binary_oarchive oa(output_stream);
    oa << expr_out;
  }
  
  basic_array_source<char> source(&buffer[0],buffer.size()); 
  stream< basic_array_source<char> > input_stream(source); 

  {// load  
    binary_iarchive ia(input_stream); 
    ia >> expr_in;
  }
  
  boost::proto::display_expr(expr_in);
  
  // for(std::size_t i = 0; i < nt2::numel(out); ++i)
  //   NT2_TEST_EQUAL(out(i), in(i));
}
