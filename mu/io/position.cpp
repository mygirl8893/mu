#include <mu/io/position.h>

mu::io::position::position ()
	: row (1),
	column (1),
	character (0)
{
}

mu::io::position::position (size_t row_a, size_t column_a, size_t character_a)
	: row (row_a),
	column (column_a),
	character (character_a)
{
}

bool mu::io::position::operator == (mu::io::position const & other) const
{
	bool result (row == other.row && column == other.column && character == other.character);
	return result;
}