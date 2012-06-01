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

#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/iostreams/stream_buffer.hpp> 
#include <boost/iostreams/stream.hpp> 
#include <boost/iostreams/device/back_inserter.hpp> 

#include <vector>
#include <iostream>

#include <nt2/sdk/unit/module.hpp>
#include <nt2/sdk/unit/tests/relation.hpp>
#include <nt2/sdk/unit/tests/basic.hpp>

//==============================================================================
// Test for save table in an archive
//==============================================================================
NT2_TEST_CASE_TPL( serialization_save, (double))//NT2_TYPES)
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
