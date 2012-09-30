#include "matrix.h"

#include<algorithm>
#include<vector>
#include<iostream>

Matrix::Matrix(std::size_t row, std::size_t col) : data(std::vector<std::vector<float>>(row, std::vector<float>(col,0.0f))){}

Matrix::Matrix(std::initializer_list<std::vector<float>> values) : data(values){}

Matrix Matrix::operator *(const Matrix & other)
{
	if(col() != other.row())
		throw;
	Matrix result(row(), other.col());
	for(std::size_t i = 0; i < row(); ++i)
	{
		for(std::size_t j = 0; j < col(); ++j)
		{
			for(std::size_t inner = 0; inner < col(); ++inner)
			{
				result.data[i][j] += data[i][inner] * other.data[inner][j];  
			}
		}	 
	}
	return result;
}

std::vector<float> & Matrix::operator[](std::size_t rowId)
{
	if(rowId >= data.size())
		throw;
	return data[rowId];
}

const std::vector<float> & Matrix::operator[](std::size_t rowId) const
{
	if(rowId >= data.size())
		throw;
	return data[rowId];
}

void Matrix::print() const
{
	std::cout.precision( 2 );
	std::for_each(data.begin(), data.end(), [](const std::vector<float> & row)
	{
		std::for_each(row.begin(), row.end(),[](const float & value)
		{
			std::cout << value << ' ';
		});
		std::cout << std::endl;
	});
}
