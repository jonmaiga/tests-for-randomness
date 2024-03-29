#pragma once
#include <sstream>

namespace tfr {

class table {
public:
	table(const std::vector<std::string>& headers) : _columns(headers.size()) {
		_spaces.resize(_columns);
		row();
		for (const auto& header : headers) {
			col(header);
		}
		row();
	}

	table& app(const char* text) {
		return app(std::string(text));
	}

	table& app(const std::string& text) {
		_current_column += text;
		return *this;
	}

	table& app(int i) {
		return app(std::to_string(i));
	}

	table& app(unsigned int i) {
		return app(std::to_string(i));
	}

	table& app(int64_t i) {
		return app(std::to_string(i));
	}

	table& app(uint64_t i) {
		return app(std::to_string(i));
	}

	table& app(double d) {
		std::ostringstream oss;
		oss << /*std::setprecision(4) << std::noshowpoint <<*/ d;
		return app(oss.str());
	}

	table& app_bool(bool b) {
		return app(b ? "true" : "false");
	}

	template <typename T>
	table& col(const T& x) {
		app(x);
		return col();
	}

	/*
		table& col(int i) {
			app(i);
			return col();
		}
	
		table& col(unsigned int i) {
			app(i);
			return col();
		}
	
		table& col(uint64_t i) {
			app(i);
			return col();
		}
	
		table& col(double d) {
			app(d);
			return col();
		}
	
		table& col_bool(bool b) {
			app_bool(b);
			return col();
		}
	*/
	table& col() {
		_spaces[_current_column_index] = std::max(_spaces[_current_column_index], _current_column.size() + 2);
		_getRow().push_back(_current_column);
		++_current_column_index;
		_current_column = "";
		return *this;
	}

	table& row() {
		if (_current_column_index == _columns - 1) {
			col();
		}
		assertion(_rows.empty() || _getRow().size() == _columns, "To few or many columns");
		_rows.emplace_back();
		_current_column_index = 0;
		return *this;
	}

	std::string to_string() const {
		std::string sb;
		for (const auto& row : _rows) {
			int i = 0;
			for (const auto& col : row) {
				auto spaces = _spaces[i] - col.size();
				sb.append(col);
				while (spaces-- > 0) {
					sb += " ";
				}
				++i;
			}
			if (!row.empty()) {
				sb.append("\n");
			}
		}
		return sb;
	}

	std::size_t get_column_size(std::size_t col) const {
		return _spaces[col];
	}

private:
	const std::size_t _columns;
	std::vector<std::size_t> _spaces;

	int _current_column_index = 0;
	std::string _current_column;
	std::vector<std::vector<std::string>> _rows;

	std::vector<std::string>& _getRow() {
		return _rows[_rows.size() - 1];
	}
};

}
