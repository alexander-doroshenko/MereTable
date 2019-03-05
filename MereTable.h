#pragma once

#include <sstream>
#include <map>
#include <vector>
#include <numeric>
#include <algorithm>
#include <iomanip>
#include <iterator>


namespace awkward
{

	/// <summary>
	///		auxilary class using by Table
	///		Can calculate own width with
	///		respect to own columns and values
	/// </summary>
	class Column
	{
		friend class MereTable;

		std::string title;
		int width;
		std::vector<Column> columns;
		std::vector<std::string> values;
		
	public:
		///
		Column(std::string title)
			: title{ title }
			, width{ 0 }
		{
		}


		///
		~Column() = default;


		/// <summary>
		///		Add subcolumn with specified title
		/// </summary>
		void AddColumn(std::string title) noexcept
		{
			columns.emplace_back(title);
		}


		/// <summary>
		///		Consume required number of values
		///		(equal number of subcolumns)
		///		and move iterator to the forward
		/// </summary>
		template <typename It>
		void ConsumeValues(It& it)
		{
			if ( columns.size() != 0 )
			{
				for ( auto &column : columns )
				{
					column.ConsumeValues(it);
				}
			}
			else
			{
				values.emplace_back(*it);
				++it;
			}
		}


		/// <summary>
		///		clear own values
		///		and values of subcolumns
		/// </summary>
		void Clear() noexcept
		{
			for ( auto &column : columns )
			{
				column.Clear();
			}
			values.clear();
		}


		/// <summary>
		///		calculate own width
		///		total width is max 
		///		from this title width,
		///		max subcolumn title width * number of subcolumns
		///		or max value size
		/// </summary>
		void UpdateWidth()
		{
			width = title.size();
			
			if ( columns.size() == 0 )
			{
				for ( auto &v : values )
				{
					width = std::max<int>(width, v.size());
				}
				return;
			}

			int maxSubcolumnWidth = 0;
			for ( auto& column : columns )
			{
				column.UpdateWidth();
				maxSubcolumnWidth = std::max<int>(column.width, maxSubcolumnWidth);
			}

			int allSubcolumnsWidth = maxSubcolumnWidth * columns.size();

			if ( width > allSubcolumnsWidth )
			{
				maxSubcolumnWidth = width / columns.size();
				if ( width % columns.size() != 0 )
				{
					maxSubcolumnWidth++;
				}
				allSubcolumnsWidth = maxSubcolumnWidth * columns.size();
			}

			width = allSubcolumnsWidth + columns.size() - 1;
			for ( auto& column : columns )
			{
				column.width = maxSubcolumnWidth;
			}
		}
	};


	/// 
	class MereTable
	{
	
		std::vector<Column> columns;
		int numRows = 0;
		

	public:

		///
		MereTable() = default;
		

		/// <summary>
		///		create table with given columns
		/// </summary>
		MereTable(std::initializer_list<std::string> columns)
		{
			AddColumns(columns);
		}


		///
		~MereTable() = default;


		/// <summary>
		///		Add columns to the right of table
		/// </summary>
		MereTable& AddColumns(std::initializer_list<std::string> columns)
		{
			std::copy(std::begin(columns)
				, std::end(columns)
				, std::back_inserter(this->columns));
			return *this;
		}


		/// <summary>
		///		Add subcolumn to the column 
		/// </summary>
		MereTable& AddSubcolumn(std::string columnName, std::string subcolumnName)
		{
			auto &[it, success] = Find(columnName);
			if ( !success )
			{
				columns.emplace_back(columnName);
				columns[columns.size() - 1].AddColumn(subcolumnName);
			}
			else
			{
				(*it).AddColumn(subcolumnName);
			}

			return *this;
		}


		/// <summary>
		///		Add column to the right of the table
		/// </summary>
		MereTable& AddColumn(std::string columnName)
		{
			auto &[it, success] = Find(columnName);
			if ( !success )
			{
				columns.emplace_back(columnName);
			}
			return *this;
		}


		/// <summary>
		///		Add values to the columns
		/// </summary>
		MereTable& AddValues(std::initializer_list<std::string> values)
		{
			auto it = values.begin();
			for ( auto &column : columns )
			{
				column.ConsumeValues(it);
			}
			numRows++;
			return *this;
		}


		///	<summary>
		///		Clear all values
		/// </summary>
		MereTable& Clear()
		{
			for ( auto &column : columns )
			{
				column.Clear();
			}
			numRows = 0;
			UpdateWidth();
			return *this;
		}


		/// <summary>
		///		Print table to string
		/// </summary>
		std::string ToString()
		{
			UpdateWidth();

			std::ostringstream os;

			// I use function programming paradigm

			// general print function
			auto print([&os](char fill, char border, int width, auto& what)
			{
				os << std::setfill(fill) << std::setw(width) << what << border;
			});
			auto printTitle([&print](char border)
			{
				return [&print, border](auto& what) {
					print(' ', border, what.width, what.title);
				};
			});
			auto printFill([&print](char fill, char border)
			{
				return [&print, fill, border](auto& what) {
					print(fill, border, what.width, fill);
				};
			});
			auto forEachSubcolumn([](auto& f)
			{
				return [&f](auto& what)
				{
					std::for_each(std::begin(what.columns), std::end(what.columns), f);
				};
			});
			auto makePrintEl([](auto& ifNotSubcolumns, auto& ifHasSubcolumns)
			{
				return [&ifNotSubcolumns, &ifHasSubcolumns](auto& what) {
					if ( what.columns.size() == 0 )
						ifNotSubcolumns(what);
					else
						ifHasSubcolumns(what);
				};
			});
			auto printLine([=, &os](char leftborder, auto& print)
			{
				os << leftborder;
				std::for_each(std::begin(columns), std::end(columns), print);
				os << '\n';
			});

			// up table border
			printLine('+', makePrintEl(printFill('-', '+'), printFill('-', '+')));
			// titles that has subcolumns
			printLine('|', makePrintEl(printFill(' ', '|'), printTitle('|')));
			// titles tat not has subcolumns
			printLine('|', makePrintEl(printTitle('|'), forEachSubcolumn(printFill('-', '+'))));
			// subcolumns titles
			printLine('|', makePrintEl(printFill(' ', '|'), forEachSubcolumn(printTitle('|'))));
			// border after table header
			printLine('+', makePrintEl(printFill('=', '+'), forEachSubcolumn(printFill('=', '+'))));

			// print values
			int i = 0;
			auto printColumnValue([&i, &print](auto& what) {
				print(' ', '|', what.width, what.values[i]);
			});
			auto printValue = makePrintEl(printColumnValue, forEachSubcolumn(printColumnValue));

			for ( ; i < numRows; i++ ) printLine('|', printValue);

			// print bottom border
			printLine('+', makePrintEl(printFill('-', '+'), forEachSubcolumn(printFill('-', '+'))));

			return os.str();
		}


		///
		friend std::ostream& operator << (std::ostream& os, MereTable& t)
		{
			return os << t.ToString();
		}


	private:

		///	<summary>
		///		find column with the specified name
		///		and return <iterator, true> if find
		///		or <end iterator, false> otherwise
		/// </summary>
		std::pair<std::vector<Column>::iterator, bool>
			Find(const std::string& columnName)
		{
			auto it = std::find_if(std::begin(columns), std::end(columns)
				, [&columnName](const Column& what)
			{
				return (what.title == columnName);
			});

			return{ it, it != columns.end() };
		}


		///
		void UpdateWidth()
		{
			for ( auto &c : columns )
			{
				c.UpdateWidth();
			}
		}
	};
}

