# MereTable
Makes it easy to build and print complex tables
+---------+---------------------------------------+--+--------+----------------+
|         |                                      b|  |       d|                |
|        a|-------------------+-------------------+ c|--+--+--+big column title|
|         |big subcolumn title|                  2|  | 1| 2| 3|                |
+=========+===================+===================+==+==+==+==+================+
|big value|                 b1|                 c1|d1|e1|f1|g1|              h1|
|       a2|                 b2|                 c2|d2|e2|f2|g2|              h2|
|       a3|                 b3|                 c3|d3|e3|f3|g3|              h3|
+---------+-------------------+-------------------+--+--+--+--+----------------+

## Usage
Implementation requires c++17 for unpack pair in one place
and if you redo it you will need c++11 anyway (lambda-functions && initializer list) 
So you can use this module in the following ways:

	#include <iostream>
	#include "ext/MereTable/MereTable.h"

	int main()
	{
		awkward::MereTable t;

		t
			.AddColumns({ "a", "b", "c", "d" })
			.AddColumn("big column title")
			.AddSubcolumn("b", "big subcolumn title")
			.AddSubcolumn("b", "2")
			.AddSubcolumn("d", "1")
			.AddSubcolumn("d", "2")
			.AddSubcolumn("d", "3")
			.AddValues({ "big value", "b1", "c1", "d1", "e1", "f1", "g1", "h1" })
			.AddValues({ "a2", "b2", "c2", "d2", "e2", "f2", "g2", "h2" })
			.AddValues({ "a3", "b3", "c3", "d3", "e3", "f3", "g3", "h3" });

		std::cout
			<< t << std::endl
			<< t.Clear();

		std::getchar();
		return 0;
	}

## License

This project is licensed under the MIT License - see the LICENSE.md file for details
   
