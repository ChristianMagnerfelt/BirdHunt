#include "matrix.h"

#include<algorithm>
#include<vector>
#include<iostream>

Matrix::Matrix(std::size_t row, std::size_t col) : data(std::vector<std::vector<float>>(row, std::vector<float>(col,0.0f))){}

Matrix Matrix::operator *(const Matrix & other)
{
	if(col() != other.row())
		throw;
	Matrix result(row(), other.col());


	return result;
}

const std::vector<float> & Matrix::operator[](std::size_t rowId) const
{
	if(rowId >= data.size())
		throw;
	return data[rowId];
}

void Matrix::print() const
{
	std::for_each(data.begin(), data.end(), [](const std::vector<float> & row)
	{
		std::for_each(row.begin(), row.end(),[](const float & value)
		{
			std::cout << value;
		});
		std::cout << std::endl;
	});
}
