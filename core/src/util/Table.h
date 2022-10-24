#pragma once

namespace mixer {

class Table {
public:
	Table(const std::vector<std::string>& headers) : _columns(headers.size()) {
		_spaces.resize(_columns);
		row();
		for (const auto& header : headers) {
			col(header);
		}
		row();
	}

	Table& app(const std::string& text) {
		_current_column += text;
		return *this;
	}

	Table& app(int i) {
		return app(std::to_string(i));
	}

	Table& app(uint64_t i) {
		return app(std::to_string(i));
	}

	Table& app(double d) {
		return app(std::to_string(d)); //(MathUtil.round(d, _decimals)));
	}

	Table& app(bool b) {
		return app(b ? "true" : "false");
	}

	Table& col(int i) {
		app(i);
		return col();
	}

	Table& col(uint64_t i) {
		app(i);
		return col();
	}

	Table& col(double d) {
		app(d);
		return col();
	}

	Table& col(bool b) {
		app(b);
		return col();
	}

	Table& col(const std::string& text) {
		app(text);
		return col();
	}

	Table& col() {
		_spaces[_current_column_index] = std::max(_spaces[_current_column_index], _current_column.size() + 2);
		_getRow().push_back(_current_column);
		++_current_column_index;
		_current_column = "";
		return *this;
	}

	Table& row() {
		if (_current_column_index == _columns - 1) {
			col();
		}
		assertion(_getRow().size() == _columns, "To few or many columns");
		_rows.emplace_back();
		_current_column_index = 0;
		return *this;
	}

	std::string to_string() const {
		std::string sb;
		for (const auto& row : _rows) {
			int i = 0;
			for (const auto& col : row) {
				int spaces = _spaces[i] - col.size();
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
